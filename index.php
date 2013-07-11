<?php
//ini_set('output_buffering','On');
//ini_set('implicit_flush','FALSE');
define('IS_INDEX_PAGE',1);
//$_SERVER['DOCUMENT_ROOT'] = preg_replace('-index\.php.*$-','',__FILE__);
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
//ini_set('output_buffering','On');
class Main{
	const SQL_START_GET = 's';
	const SQL_LIMIT_GET = 'l';
	const USERINSTANCE_COOKIE = 'query_instance';
	const KEYWORD_COOKIE = 'query_notfound';
	function __construct(){
		//do a soft user check
		User::checkAuth(false,false);
	}
	/**
	 *	@param $q - query word
	 * @return array of parse options
	 */
	private function parseQuery(&$q,&$shortwords){
		$ar = array();
		$words = preg_split('/[^a-z]*:+[\s\t]*/i',$q);
		$special = array('related','site');
		$keys = array_flip($words);
		foreach($special as $word){
			if (isset($keys[$word])){
				$index = $keys[$word]+1;
				$q = preg_replace('/'.$word.'[^a-z:]*:+[\s\t]*[a-z][^\s\t]+/i','',$q);
				$ar[$word] = $words[$index];
				if (!empty($words[$index])){
					unset($words[$index]);
				}
			}
		}
		$match = array();
		if (preg_match_all('/\b([^\s\t]{3})\b/',$q,$match)){
			$shortwords = $match[1];
		}
		$q = preg_replace('/\b[^\s\t]{1,3}\b/',' ',$q);
		return $ar;
	}
	public function processUrl($options){
		$outType = isset($options['out'])?$options['out']:'html';
		if (isset($options['noout']))$noout = 1;
		$inout = isset($options['inout'])?$options['inout']:null;
		//init tables (not all necessarily will be used)
		$key_t = new table_keywords(true);
		$url_t = new table_urls(true);
		$users_t = new table_users(true);
		
		$tree = CatPath::parsePathToTree($_SERVER['CRUMB_PATH']);
		$parser = new parser($options);
		$whereCond = array();
		$groupby = array($url_t->field_url);
		$invalidate = false;
		//since id is an int, we will have latest first
		$orderby = array($url_t->field_id.' DESC');
		$fields = $url_t->allFields();
		$tables = array(TABLE_URLS);
		$field_keys = array();
		$joins = array();
		if (!empty($tree)){
			if (count($tree) > 1){
				$whereCond[] = $url_t->field_keywords." LIKE '".toSQL(implode('/',$tree),false)."%'";
			}else{
				$res_ar = CatPath::getIndexes($tree);	//if it is a keyword
				if (empty($res_ar))$invalidate = true;
				else $whereCond[] = $url_t->field_id.' IN ('.toSQL($res_ar).')';
			}
		}
		//set default options
		$options['getUsernames'] = 1;
		if ($invalidate)$options = array();
		$trackBackSpelling = false;
		if (!empty($options['q']) && $options['q'] == '%s')
			unset($options['q']);
		if (!empty($options['q'])){
			$trackBackSpelling = $options['q'];
			//if keywords were specified
			$q = $options['q'];
			$shortwords = array();
			$opts = $this->parseQuery($q,$shortwords);
			$options = array_merge($options,$opts);
			$q = trim($q);
			if (!empty($q)){
				$whereCond[] = $relevance = 'MATCH('.$url_t->field_title.' ) AGAINST ('.toSQL($q).')';
				$orderby = array('rel DESC');
			}
			if (!empty($shortwords)){
				foreach($shortwords as $w){
					$whereCond[] = $url_t->field_title." LIKE '".toSQL($w,false)."%'";
				}
			}
		}
		foreach($options as $key => $v){
			$unset = true;
			switch($key){
				case 'site':
					$whereCond[] = $url_t->field_url." LIKE '%".tosql_string($options['site'])."%'";
				break;
				case 'who':
					//find out whom are we searching for
					$users_t = new table_users();
					$query = 'SELECT '.$users_t->field_userid.' FROM '.TABLE_USERS.' WHERE '
						.$users_t->field_username.' = '.$parser->getSql('who').' LIMIT 1';
					$result = db::query($query);
					if ($row = db::fetch_row($result)){
						$whereCond[] = $url_t->field_userid." = '".$row[0]."'";
						$specificUser = $row[0];
					}else{
						$invalidate = true;
					}
					unset($options['getUsernames']);
					unset($options['u']);
				break;
				case 'u':
					unset($options['who']);
					unset($options['getUsernames']);
					//if userid is specified directly
					$whereCond[] = $url_t->field_userid.' = '.toSQL($options['u']);
					$specificUser = $options['u'];
				break;
				case 'related':
					
				break;
				default:
					$unset = false;
			}
			if ($unset)unset($options[$key]);
		}
		if ($invalidate)$options = array();
		if (isset($specificUser)){
			//unless we are searching ourselves, don't display private listings
			$signedin = false;
			if ($signedin = User::checkAuth(false,true)){
				$userid = $GLOBALS[GLOBAL_USER]->getUserId();
			}
			if (!$signedin || $userid !== $specificUser){
				$whereCond[] = $url_t->field_private.' IS NULL';
			}
		}else{
			if(isset($options['getUsernames'])){
				//get usernames
				$tables[] = TABLE_USERS;
				$whereCond[] = $url_t->field_userid.' = '.$users_t->field_userid;
				$fields[] = $users_t->field_username;
				$fields[] = 'GROUP_CONCAT(DISTINCT '.$users_t->field_username
					." SEPARATOR ' ') as others";
				$groupIndex = count($fields)-1;
			}
			$whereCond[] = $url_t->field_private.' IS NULL';
		}
				
		$query = 'SELECT SQL_CACHE '.implode(',',$fields).(isset($relevance)?','.$relevance.' as rel':'')
			.' FROM '.str_repeat('(',count($joins))
			.implode(',',$tables);
		foreach($joins as $join){
			$query .= ' '.$join['type'].' '.$join['table'].' ON '.$join['on'].')';
		}
		$sqlStart = $parser->getSqlStart(self::SQL_START_GET);
		$sqlLimit = $parser->getSqlLimit(self::SQL_LIMIT_GET,40,1000,1);
		$query .= (!empty($whereCond)?' WHERE '.implode(' AND ',$whereCond):'')
			.(!empty($groupby)?' GROUP BY '.implode(',',$groupby):'').(!empty($orderby)?' ORDER BY '
			.implode(',',$orderby):'')
			." LIMIT $sqlStart,$sqlLimit";
		$resultSet = null;
		if (!$invalidate){
			$resultSet = db::query($query);
			db::$lastResultSet = 1;
			if (isset($catnameIndex))
				$fields[$catnameIndex] = $cat_t->field_catname;
			if (isset($groupIndex))
				$fields[$groupIndex] = 'others';
			
			//save field names as keys so that we can use simple fetch_row
			$field_keys = array_flip($fields);
		}
		if (isset($options['r']) && ($row = db::fetch_row($resultSet))){
			//redirect to the result, if one exists
			$link = $row[$field_keys[$url_t->field_url]];
			header('Location: '.$link);
			return;
		}
		switch($outType){
			case 'rss2':
				require_once('out/mainRss2.inc');
				break;
			case 'json':
				require_once('out/mainJson.inc');
				break;
			case 'snip':
				$options['inout'] = $inout;
				require_once('out/mainSnip.inc');
				break;
			case 'sidebar':
				require_once('out/mainSidebar.inc');
				break;
			default:
				Template::addHeader('<link rel="alternate" type="application/rss+xml" title="rss feed for this page" href="/rss'.$_SERVER['REQUEST_URI'].'"/>');
				if (!isset($noout)){
					Template::addPageLink('/rss'.$_SERVER['REQUEST_URI'],'rss');
					Template::printSearchHeader();
				}
				require_once('out/mainHtml.inc');
				if (!isset($noout))
					Template::printFooter();
		}
		if ($trackBackSpelling !== false){
			if (!isset($_COOKIE[COOKIE_COMMON]) 
						|| !isset($_COOKIE[COOKIE_COMMON][self::USERINSTANCE_COOKIE])){
				//track searches made from this user
				$id = genNumberString(10);
				setcookie(COOKIE_COMMON.'['.self::USERINSTANCE_COOKIE.']',$id,time()+60*60*24*360);
			}else{
				$id = $_COOKIE[COOKIE_COMMON][self::USERINSTANCE_COOKIE];
			}
			$word_t = new table_searches();
			$query = 'INSERT DELAYED INTO '.TABLE_SEARCHES.' ('.$word_t->field_visitorid.','
				.$word_t->field_query.') VALUES ('.$id.','.toSQL($trackBackSpelling).')';
			db::query($query);
			
			if (!db::$lastNotEmpty){
				//leave a note for ourselves if the keyword wasn't found
				setcookie(COOKIE_COMMON.'['.self::KEYWORD_COOKIE.']',$trackBackSpelling,time()+20);
			}elseif(isset($_COOKIE[COOKIE_COMMON]) 
						&& isset($_COOKIE[COOKIE_COMMON][self::KEYWORD_COOKIE])){
				//if user's previous cookie query didn't find any results but this one did, 
				//then perhaps there is a connection
				$rel_t = new table_keyword_relations();
				$query = 'INSERT DELAYED INTO '.TABLE_KEYWORD_RELATIONS.' ('.$rel_t->field_keyword1.','
					.$rel_t->field_keyword2
					.") VALUES (".toSQL($_COOKIE[COOKIE_COMMON][self::KEYWORD_COOKIE]).","
					.toSQL($trackBackSpelling).")";
				db::query($query);
				//expire this cookie
				setcookie(COOKIE_COMMON.'['.self::KEYWORD_COOKIE.']',$trackBackSpelling,time()-3600);
			}
		}
	}
}
if (!defined('NINCLUDE')){
	db::connect();
	$main = new Main();
	$main->processUrl($_GET);
	db::close();
}
?>