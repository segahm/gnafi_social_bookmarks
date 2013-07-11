<?php
define('CRON_MAX_USERS',10);
/***** INITIALIZATION ********/
$_SERVER['DOCUMENT_ROOT'] = preg_replace('-.tools.*$-','',__FILE__);
define('NINCLUDE','1');	//so that code in included files does not executed
require_once($_SERVER['DOCUMENT_ROOT']."/scripts/includes/interface.inc");
db::connect();
$firewall = new table_firewall();
$query = 'TRUNCATE TABLE '.TABLE_FIREWALL;
db::query($query);
$optimize = array();	//table to optimize after many deletions
/*****************************/
$sync_t = new table_synclatest();
$link_t = new table_urls();
$key_t = new table_keywords();
$users_t = new table_users();
//prepare url insert statement
$fields = array($link_t->field_userid,$link_t->field_description,$link_t->field_isfeed,
		$link_t->field_private,$link_t->field_time,$link_t->field_title,
		$link_t->field_url,$link_t->field_insync,$link_t->field_keywords);
$s = count($fields);
$query = 'INSERT INTO '.TABLE_URLS.' ('.sqlFields($fields)
	.") VALUES (".substr(str_repeat('?,',$s),0,$s*2-1).')';
$stmt = db::prepare($query);
//get new sync records and create new urls in urls table
function processSyncLinks($cur_userid,$glbisprivate){
	global $optimize, $stmt,$sync_t,$link_t,$key_t;
	$fields = array($sync_t->field_actpath,$sync_t->field_info);
	$query = 'SELECT '.sqlFields($fields).' FROM '.TABLE_SYNCLATEST.' WHERE '
		.$sync_t->field_cronflag.' IS NULL AND '.$sync_t->field_userid." = '".$cur_userid."'";
	$result = db::query($query);
	
	$keyQuery = 'INSERT DELAYED INTO '.TABLE_KEYWORDS.' ('.$key_t->field_urlid.','.$key_t->field_keyword
		.') VALUES ';
	$sync_count = 0;
	$urls_in_sync = '';
	$urls_skipped = 0;
	$urls_added = false;
	$keywords_added = false;
	while($row = db::fetch_row($result)){
		++$sync_count;
		$info = LinkManager::extractInfoArray($row[1]);
		$path = $row[0];
		unset($row);
		
		$path = empty($path)?'':$path;
		$path .= (!empty($info['ShortcutURL'])?','.$info['ShortcutURL']:'');
		$path = CatPath::parseStrToKeywords($path,false);
		$keywords = CatPath::explodeStrToRealKeywords($path);
		
		$isfeed = isset($info['FeedURL'])?1:null;
		$url = $isfeed?$info['FeedURL']:$info['URL'];
		$isprivate = (($glbisprivate OR LinkManager::isPrivateUrl($url))?1:null);
		$description = empty($info['Description'])?null:$info['Description'];
		$time = Mydate::sqlDateTime(time());
		$title = funcs_cutLength($info['Name'],$link_t->field_title_length);
		$insync = 1;
		$path = empty($path)?null:$path;
		$stmt->bind_param('ssddsssds',$cur_userid,$description,$isfeed,$isprivate,$time,
			$title,$url,$insync,$path);
		if (!$stmt->execute()){
			//if it's not a new url continue
			$urls_in_sync .= toSQL($url).',';	//we still need to update this url with
			//insync flag
			++$urls_skipped;
			continue;
		}
		if (!empty($keywords)){
			$urlid = db::insert_id();
			assert($urlid != 0);
			$keywords_added = true;
			if (is_array($keywords[0])){
				$keywords = array_merge(array_shift($keywords),$keywords);
			}
			foreach($keywords as $keyword){
				$keyword = toSQL($keyword);
				$keyQuery .= "($urlid,$keyword),";
			}
		}
		$urls_added = true;
	}
	if ($keywords_added){
		$keyQuery = preg_replace('-,$-','',$keyQuery);
		db::query($keyQuery);
	}
	//leave a note for cron not to touch these urls in the future
	if ($sync_count != 0){
		$query = 'UPDATE '.TABLE_SYNCLATEST.' SET '.$sync_t->field_cronflag.' = 1 WHERE '
			.$sync_t->field_userid." = '".$cur_userid."' AND ".$sync_t->field_cronflag.' IS NULL LIMIT '
			.$sync_count;
		db::query($query);
	}
	if ($urls_added){
		//now delete all urls that user chose to delete from the web interface
		$del_t = new table_deletedurls(true);
		$link_t = new table_urls(true);
		$query = 'DELETE FROM '.TABLE_URLS.' USING '.TABLE_URLS.','.TABLE_DELETEDURLS
			.' WHERE '.$del_t->field_userid.' = '.$link_t->field_userid.' AND '
			.$del_t->field_url.' = '.$link_t->field_url;
		db::query($query);
	}
	if ($urls_skipped != 0){
		$urls_in_sync = preg_replace('/,$/','',$urls_in_sync);
		//now set the syncid flag for the updated urls
		$query = 'UPDATE '.TABLE_URLS.' SET '.$link_t->field_insync.' = 1 WHERE '
			.$link_t->field_userid." = '".$cur_userid."' AND "
			.$link_t->field_url.' IN ('.$urls_in_sync.') LIMIT '.$urls_skipped;
		db::query($query);
	}
}
//first get latest synchronized user
$query = 'SELECT '.TABLE_SYNCLATEST.'.'.$sync_t->field_userid.','
					.TABLE_USERS.'.'.$users_t->field_urlsprivate.' FROM '.
	TABLE_SYNCLATEST.','.TABLE_USERS.' WHERE '
	.TABLE_SYNCLATEST.'.'.$sync_t->field_cronflag.' IS NULL GROUP BY '.TABLE_SYNCLATEST.'.'.$sync_t->field_userid
	.' AND '.TABLE_USERS.'.'.$users_t->field_userid.' = '.TABLE_SYNCLATEST.'.'.$sync_t->field_userid
	.' LIMIT '.CRON_MAX_USERS;
$result = db::query($query);
$users = array();
while ($row = db::fetch_row($result)){
	$users[$row[0]] = $row[1];
}
if (!empty($users)){
	$query = 'UPDATE '.TABLE_URLS.' SET '.$link_t->field_insync.' = NULL WHERE '
			.$link_t->field_insync.' IS NOT NULL AND '
			.$link_t->field_userid.' IN ('.toSQL(array_keys($users)).')';
	db::query($query);
	foreach($users as $user => $isprivate){
		processSyncLinks($user,!is_null($isprivate));
	}
}
if (!empty($optimize)){
	$query = 'OPTIMIZE NO_WRITE_TO_BINLOG TABLE '.implode(',',$optimize);
	db::query($query);
}
db::close();
?>