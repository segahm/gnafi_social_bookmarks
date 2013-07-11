<?php
$_SERVER['DOCUMENT_ROOT'] = 'C:\Webdeploy\gnafi_bookmarks';
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
class Sync{
	const gnRdf = 'http://gnafi#';
	const nsRdf = 'http://home.netscape.com/NC-rdf#';
	const rdfRdf = 'http://www.w3.org/1999/02/22-rdf-syntax-ns#';
	public function Sync(){
			$isSignedIn = User::checkAuth(false,true);
			if (!$isSignedIn){
				header("HTTP/1.1 403 Forbidden");
				db::close();
				exit;
			}
			$this->userid = $GLOBALS[GLOBAL_USER]->getUserID();
	}
	private function doOut(&$str,$ishtml = false){
		if ($ishtml)
			header('Content-type: text/html charset=UTF-8');
		else
			header('Content-type: application/xml charset=UTF-8');
		echo $str;
	}
	public function init(){
		if(isset($_GET['latestrdf'])){
			$out = new UserOut($this->userid);
			$str = $out->latestEntriesRdf();
			$this->doOut($str);
		}elseif(isset($_GET['latesthtml'])){
			require_once('main/userout.inc');
			$out = new UserOut($this->userid);
			$sync_t = new table_synclatest();
			$query = 'SELECT '.$sync_t->field_actpath.','.$sync_t->field_info.' FROM '
				.TABLE_SYNCLATEST.' WHERE '.$sync_t->field_userid." = '".$this->userid."' ORDER BY "
				.$sync_t->field_actpath;
			$result = db::query($query);
			$str = $out->latestUserHtml($result);
			$this->doOut($str,true);
		}elseif(isset($_GET['cvsid']) && strlen($_GET['cvsid']) == 5  && isset($_POST['text'])){
			/*$firewall = new table_firewall();
			$query = 'SELECT 1 FROM '.TABLE_FIREWALL.' WHERE '
				.$firewall->field_cvsid.' = '.toSQL($_GET['cvsid']).' AND '
				.$firewall->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserID()."' AND "
				.$firewall->field_ip." = INET_ATON('".$_SERVER['REMOTE_ADDR']."')";
			$result = db::query($query);
			if (!db::fetch_row($result)){*/
				if ($this->doMajor($_POST['text'],$_GET['cvsid'])){
					//create an entry in temp table
					/*$query = 'INSERT INTO '.TABLE_FIREWALL.' ('.$firewall->field_ip.','.
						$firewall->field_userid.','.
						$firewall->field_cvsid.") VALUES (INET_ATON('".$_SERVER['REMOTE_ADDR']."'),"
						.toSQL($GLOBALS[GLOBAL_USER]->getUserID()).','
						.toSQL($_GET['cvsid']).')';
					db::query($query);*/
				}else{
					header("HTTP/1.1 400 Bad Request");
				}
			/*}else{
				header("HTTP/1.1 501 Not implemented");
				$cnt = '<?xml version="1.0"?><root><message>too many requests</message></root>';
				$this->doOut($cnt);
			}*/
		}else{
			header("HTTP/1.1 400 Bad Request");
		}
	}
	/**
	 * @param string $str - xml document string that we need to process
	 * @param string $cvsid - cvs id of the current session
	 */
	private function doMajor($str,$cvsid){
		try{
			$test_dump = '';
			$userid = $GLOBALS[GLOBAL_USER]->getUserID();
			$xml = new DOMDocument('1.0','UTF-8');
			if (!$xml->loadXML($str))return false;		
			$sync_t = new table_synclatest();
			$temp_t = new table_synctemp();
			/**
			 *since we don't keep a copy of cvs entry unless another cvsid overrides it, 
			 * we first need to check wherever
			 *we have to make a copy of the last cvs entry
			 */
			$query = 'SELECT '.$sync_t->field_cvsid.' FROM '.TABLE_SYNCLATEST.' WHERE '
				.$sync_t->field_userid." = '".$userid."' AND ".$sync_t->field_cvsid." != 'aaaaa' LIMIT 1";
			$result = db::query($query);
			//if we need to do a backup of previous checked-in entries
			$isSameAsLast = true;
			if (($row = db::fetch_row($result)) && $row[0] != $cvsid){
				$isSameAsLast = false;
				$this->doBackupOfLastEntries($userid,$row[0]);
			}
			$field_names = array($sync_t->field_info,$sync_t->field_hashkey,
					$sync_t->field_cvsid,$sync_t->field_userid,$sync_t->field_actpath,
					$sync_t->field_flag);
			//inserts all [new] urls
			$query = 'REPLACE INTO '.TABLE_SYNCLATEST.' ('.sqlFields($field_names)
					.') VALUES ('.implode(',',toChars(str_repeat('?',$l = count($field_names)),$l))
					.')';
			$smnt = db::prepare($query);
			$field_names = array($temp_t->field_userid,$temp_t->field_hashkey,$temp_t->field_aboutid);
			$query = 'REPLACE INTO '.TABLE_SYNCTEMP.' ('.sqlFields($field_names)
					.') VALUES ('.implode(',',toChars(str_repeat('?',$l = count($field_names)),$l))
					.')';
			$tempStatemt = db::prepare($query);
			$nodes = $xml->getElementsByTagNameNS(self::rdfRdf,'li');
			$node_length = $nodes->length;
			for ($node_count=0;$node_count<$node_length;$node_count++){
				$line = array();
				$node = $nodes->item($node_count);
				$id = $node->getAttribute('id');	//needed for removal response
				//find out its parent chain with the top folder as the last element
				$chain = $this->getParentChain($xml,$node);
				$chain = array_reverse($chain);	//now parent goes first
				$path = implode('/',$chain);	//is utf safe?
				//get url
				$childrenNodes = $node->childNodes;
				$vals = array();
				$url = null;
				$len = $childrenNodes->length;
				$srch = '/,/';
				$rpls = ',,';
				for ($c=0;$c<$len;$c++){
					$item = $childrenNodes->item($c);
					if ($item->namespaceURI != null && $item->namespaceURI === self::nsRdf){
						if ($item->localName === 'URL' || $item->localName === 'FeedURL'){
							$url = $item->nodeValue;
						}
						//escape string
						$str = preg_replace($srch,$rpls,$item->nodeValue);	//this has to be multibyte-safe
						//only include relavent part of the namespace
						$vals[$item->localName] = $item->localName.':'.$str;
					}
				}
				if (!isset($vals['Name'])){
					continue;
				}
				//if we were unable to find url, then something is wrong
				if ($url == null){
					throw new Exception('unable to find url string:content:'.$node->textContent);
				}
				//we store all the contents as simple text, since we don't need it for now
				$param1 = implode(',',$vals);
				//unique entry happens based on url,path,title,keywords
				$str = $url.(!empty($chain)?$path:'').$vals['Name'].(isset($vals['ShortcutURL'])?$vals['ShortcutURL']:'');
				$param2 = '0x'.bin2hex(sha1($str,true));	//hash key of the unique string
				$param3 = 1;
				$path = empty($path)?null:$path;
				$smnt->bind_param('sssssd',$param1,$param2,$cvsid,$userid,$path,
					$param3);	//flag telling us that this is a new entry
				$smnt->execute();
				$tempStatemt->bind_param('sbs',$userid,$param2,$id);
				$tempStatemt->execute();
			}
			$smnt->close();
			$tempStatemt->close();
			//delete entries that were deleted by this cvsid, and delete those
			//that previously were deleted by other users but this user still had the bad copy
			$this->deleteRemovedEntries($isSameAsLast,$cvsid);
			$out = $this->printOut($cvsid,$node_length,$xml);
			$this->doOut($out);
		}catch(Exception $e){
			error_log('exception in major:'.$e->getMessage());
			return false;
		}
		return true;
	}
	private function &printOut($cvsid,$node_length,&$xml){
		$temp_t = new table_synctemp();
		$out = new UserOut($this->userid);
		$this->tempOut = $out->CvsOnlyRdf($xml,$cvsid,$node_length);
		$query = 'DELETE QUICK FROM '.TABLE_SYNCTEMP.' WHERE '
			.$temp_t->field_userid." = '".$this->userid."'";
		db::query($query);
		return $this->tempOut;
	}
	/**
	 * @param string $userid - user id
	 * @param boolean $isCvsSameAsLast - wherever previous cvsid was the same as the current
	 * @param string $cvsid - current cvsid
	 * does several actions:
	 * 1) saves a note for other previously created cvsids that this cvsid deleted certain entries
	 * 2) deletes those entries that this cvsid didn't submit this time around (what copied in step1)
	 * 3) deletes entries that other cvsids saved for this cvsid
	 */
	private function deleteRemovedEntries($isCvsSameAsLast,$cvsid){
		$userid = $this->userid;
		$sync_t = new table_synclatest();
		$del_sync_t = new table_sync_todelete();
		$reg_sync_t = new table_sync();
		$fieldsToCopyTo = array($del_sync_t->field_userid,$del_sync_t->field_hashkey,$del_sync_t->field_cvsid);
		$fieldsToCopyFrom = array(
			TABLE_SYNCLATEST.'.'.$sync_t->field_userid,
			TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey,
			TABLE_SYNC.'.'.$reg_sync_t->field_cvsid);
		//get all cvsids
		//if we can deleted entries relative to latest table
		if ($isCvsSameAsLast){
			//copy all values that this user deleted, so that other cvsids know
			$query = 'INSERT INTO '.TABLE_SYNC_TODELETE.' ('
				.sqlFields($fieldsToCopyTo).') SELECT '.implode(',',$fieldsToCopyFrom)
				.' FROM '.TABLE_SYNCLATEST.' JOIN '.TABLE_SYNC.' ON '
				.TABLE_SYNCLATEST.'.'.$sync_t->field_userid.' = '
				.TABLE_SYNC.'.'.$reg_sync_t->field_userid.' AND '
				.TABLE_SYNC.'.'.$reg_sync_t->field_cvsid.' NOT IN ('
				.toSQL($cvsid).",'aaaaa') WHERE "	//where aaaaa is the default cvsid we use for administrative purposes
				//so don't leave a note for aaaaa
				.TABLE_SYNCLATEST.'.'.$sync_t->field_userid." = '".$userid."' AND "
				.TABLE_SYNCLATEST.'.'.$sync_t->field_flag.' IS NULL AND '
				.TABLE_SYNCLATEST.'.'.$sync_t->field_cvsid." != 'aaaaa'"
				.' GROUP BY '.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey.','
				.TABLE_SYNC.'.'.$reg_sync_t->field_cvsid;
			db::query($query);
			//delete these same entries in the latest version
			$query = 'DELETE FROM '.TABLE_SYNCLATEST.' WHERE '
				.$sync_t->field_userid." = '".$userid."' AND "
				.$sync_t->field_flag.' IS NULL AND '
				.$sync_t->field_cvsid." != 'aaaaa'";
			//delete all entries that we didn't insert this time, since we were the last cvsid previously
			db::query($query);
		}else{
			//copy all values that this user deleted, so that other cvsids know about the changes
			$query = 'INSERT INTO '.TABLE_SYNC_TODELETE.' ('
				.sqlFields($fieldsToCopyTo).') SELECT '.implode(',',$fieldsToCopyFrom)
				.' FROM '.TABLE_SYNCLATEST.','.
				TABLE_SYNC.' AS t2 JOIN '.TABLE_SYNC.' ON '
				.TABLE_SYNCLATEST.'.'.$sync_t->field_userid.' = '
				.TABLE_SYNC.'.'.$reg_sync_t->field_userid.' AND '
				.TABLE_SYNC.'.'.$reg_sync_t->field_cvsid.' NOT IN ('
				.toSQL($cvsid).",'aaaaa') WHERE "
				.TABLE_SYNCLATEST.'.'.$sync_t->field_userid." = '".$userid."' AND "
				.TABLE_SYNCLATEST.'.'.$sync_t->field_flag.' IS NULL AND '
				.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey
				.' = t2.'.$reg_sync_t->field_hashkey
				.' AND t2.'.$reg_sync_t->field_cvsid
				.' = '.toSQL($cvsid).' AND t2.'.$sync_t->field_userid." = '".$userid
				."' GROUP BY "
				.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey.','
				.TABLE_SYNC.'.'.$reg_sync_t->field_cvsid;
			db::query($query);
			//we are only allowed to delete entries that we've previously were the owners of (submited)
			$query = 'DELETE FROM '.TABLE_SYNCLATEST.' USING '.TABLE_SYNCLATEST.' ,'
					.TABLE_SYNC.' WHERE '
					.TABLE_SYNCLATEST.'.'.$sync_t->field_userid." = '".$userid."' AND "
					.TABLE_SYNCLATEST.'.'.$sync_t->field_flag.' IS NULL AND '
					.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey
					.' = '.TABLE_SYNC.'.'.$reg_sync_t->field_hashkey.' AND '
						.TABLE_SYNC.'.'.$reg_sync_t->field_cvsid
					.' = '.toSQL($cvsid).' AND '
					.TABLE_SYNC.'.'.$sync_t->field_userid." = '".$userid."'";
			db::query($query);
		}
		//now delete all entries that other cvsids left for this cvsid
		$query = 'DELETE '.TABLE_SYNCLATEST.','.TABLE_SYNC_TODELETE.' FROM '.TABLE_SYNCLATEST.','.TABLE_SYNC_TODELETE
			.' WHERE '.TABLE_SYNC_TODELETE.'.'.$del_sync_t->field_userid." = '".$userid."' AND "
			.TABLE_SYNC_TODELETE.'.'.$del_sync_t->field_cvsid.' = '.toSQL($cvsid).' AND '
			.TABLE_SYNC_TODELETE.'.'.$del_sync_t->field_hashkey.' = '
			.TABLE_SYNCLATEST.'.'.$sync_t->field_hashkey;
		db::query($query);
		$query = 'DELETE FROM '.TABLE_SYNC_TODELETE.' WHERE '.$del_sync_t->field_cvsid
			.' = '.toSQL($cvsid).' AND '.$del_sync_t->field_userid." = '".$userid."'";
		db::query($query);
	}
	//we need this for  removal comparisoon
	private function doBackupOfLastEntries($userid,$cvsid){
		$lastsync_t = new table_synclatest();
		$sync_t = new table_sync();
		//remove previously stored last entries for this cvsid & userid
		$query = 'DELETE QUICK FROM '.TABLE_SYNC.' WHERE '
			.$sync_t->field_cvsid.' = '.toSQL($cvsid).' AND '
			.$sync_t->field_userid." = '".$userid."'";
		db::query($query);
		//now copy all last entries to cvs table
		$field_names = array($sync_t->field_hashkey,
					$sync_t->field_cvsid,$sync_t->field_userid);
		$query = 'INSERT INTO '.TABLE_SYNC.' ('.implode(',',$field_names).') SELECT '
			.TABLE_SYNCLATEST.'.'.implode(','.TABLE_SYNCLATEST.'.',$field_names)
			.' FROM '.TABLE_SYNCLATEST.' WHERE '.TABLE_SYNCLATEST.'.'
			.$sync_t->field_userid." = '".$userid."' AND ".TABLE_SYNCLATEST
			.'.'.$sync_t->field_cvsid." != 'aaaaa'";
		db::query($query);
	}
	/**
	 * assumes that if folder/folder2 then <Seq NC:Name="folder1><Seq NC:Name="folder2"></Seq></Seq>
	 * @param XMLDocument $doc - xml document
	 * @param node $node - node of the bookmark element
	 */
	private function getParentChain(&$doc,$node){
		$chain = array();
		$n = $node->parentNode;
		while($n != null && ($about = $n->getAttributeNS(self::rdfRdf,'about')) != 'NC:BookmarksRoot'){
			$name = $n->getAttributeNS(self::rdfRdf,'Name');
			//if name doesn't exist for this folder, then something is wrong
			if ($name == null)
				throw new Exception('unable to find the name of the folder');
			$chain[] = $name;
			$n = $n->parentNode;
		}
		return $chain;
	}
}
/*if (defined('DEBUG')){
	$_POST['text'] = file_get_contents($_SERVER['DOCUMENT_ROOT'].'/mytemp.dat');
}*/
if (!defined('NINCLUDE')){
	db::connect();
	$sync = new Sync();
	$sync->init();
	db::close();
}
?>