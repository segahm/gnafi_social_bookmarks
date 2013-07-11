<?php
class SynchronizedLink{
	public $Description;
	public $LastCharset;
	public $LastModifiedDate;
	public $BookmarkAddDate;
	public $LastVisitDate;
	public $URL;	// [OR]
	public $FeedURL;
	/** @param string $Name - title of the link that includes the path*/
	public $Name;
	/** @param string $Name - one keyword*/
	public $ShortcutURL;
	public $Path;
}
class LinkManager{
	private $userid;
	const DEFAULT_CVSID = 'aaaaa';
	function __construct($userid){
		$this->userid = $userid;
	}
	/**
	 * @param int $linkid - id of the link from url history
	 */
	public function moveToSync($linkid){
		if(!is_numeric($linkid))return;
		$urls_t = new table_urls(true);
		$fields = $urls_t->allFields();
		$query = 'SELECT '.sqlFields($fields).' FROM '.TABLE_URLS
			.' WHERE '.$urls_t->field_id.' = '.$linkid.' AND '.$urls_t->field_userid." = '"
			.$this->userid."' AND ".$urls_t->field_insync." IS NULL LIMIT 1";
		$result = db::query($query);
		if (!($row = db::fetch_row($result)))return;
		$fields = array_flip($fields);
		$obj = new SynchronizedLink();
		if ($row[$fields[$urls_t->field_isfeed]] != null)
			$obj->FeedURL = $row[$fields[$urls_t->field_url]];
		else
			$obj->URL = $row[$fields[$urls_t->field_url]];
		$obj->Name = $row[$fields[$urls_t->field_title]];
		if (!empty($row[$fields[$urls_t->field_description]]))
			$obj->Description = $row[$fields[$urls_t->field_description]];
		if (!empty($row[$fields[$urls_t->field_keywords]])){
			$ar = explode(',',$row[$fields[$urls_t->field_keywords]]);
			if (isset($ar[0]) && preg_match('-/-',$ar[0])){
				$obj->Path = $ar[0];
				unset($ar[0]);
			}
			if (!empty($ar))
				$obj->ShortcutURL = implode(',',$ar);
		}
		$id = $this->addToSync($obj,null,false,true);
		//finally mark this link as synchronized
		$query = 'UPDATE '.TABLE_URLS.' SET '.$urls_t->field_insync.' = 1'
			.' WHERE '.$urls_t->field_id.' = '.$linkid.' LIMIT 1';
		db::query($query);
	}
	public function addNewUrl($url,$title,$description = null,$tagsSeparatedBySpace = null,$isfeed = false){
		$url_t = new table_urls();
		$key_t = new table_keywords();
		$url = toHTML($url);
		//if this was not a new url, then first delete previous kewords
		$query=  'SELECT '.$url_t->field_id.' FROM '.TABLE_URLS.' WHERE '
			.$url_t->field_url.' = '.toSQL($url).' AND '.$url_t->field_userid
			.' = '.toSQL($this->userid).' LIMIT 1';
		$result = db::query($query);
		$new = true;
		if ($row = db::fetch_row($result)){
			$urlid = $row[0];
			$new = false;
			//remove all previous keyword references since no longer relevant
			$query = 'DELETE FROM '.TABLE_KEYWORDS.' WHERE '.$key_t->field_urlid
				.' = '.$urlid;
			db::query($query);
		}else{
			//make sure we no longer have an entry in our delete table regarding this url
			$del_t = new table_deletedurls();
			$query = 'DELETE FROM '.TABLE_DELETEDURLS.' WHERE '
				.$del_t->field_url.' = '.toSQL($url).' AND '.$del_t->field_userid
				.' = '.toSQL($this->userid).' LIMIT 1';
			db::query($query);
		}
			
		$fields = array(1 => $url_t->field_time,$url_t->field_title,$url_t->field_url,
							$url_t->field_userid);
		$vals = array(1 => ($time = Mydate::sqlDateTime(time())),$title,
			$url,
			$this->userid);
		if ($this->isPrivateUrl($url)){
			$fields[0] = $url_t->field_private;
			$vals[0] = '1';
		}
		if ($description != null){
			$fields[] = $url_t->field_description;
			$vals[] = $description;
		}
		$path = '';
		if (!empty($tagsSeparatedBySpace)){
			$keywords = CatPath::parseStrToKeywords($tagsSeparatedBySpace,true);
			if (!empty($keywords)){
				$vals[] = $keywords;
				$fields[] = $url_t->field_keywords;
			}
		}
		$vals[] = $isfeed?1:NULL;
		$fields[] = $url_t->field_isfeed;
		if ($new){
			$query = 'REPLACE INTO '.TABLE_URLS.' ('.sqlFields($fields)
					.') VALUES ('.toSQL($vals).')';
			db::query($query);
		}else{
			$query = 'UPDATE '.TABLE_URLS.' SET ';
			foreach($fields as $k => $f){
				$query .= $f.' = '.toSQL($vals[$k]).',';
			}
			$query = preg_replace('-,$-','',$query);
			$query .= ' WHERE '.$url_t->field_id.' = '
					.$urlid.' LIMIT 1';
			db::query($query);
		}
		if (!empty($keywords)){
			if (empty($urlid))$urlid = db::insert_id();
			$keywords = CatPath::explodeStrToKeywords($keywords);
			foreach($keywords as $k => $keyword){
				$keywords[$k] = toSQL($keyword);
			}
			$query = 'INSERT DELAYED INTO '.TABLE_KEYWORDS.' ('.sqlFields(array($key_t->field_urlid,$key_t->field_keyword))
						.") VALUES ($urlid,".implode("),($urlid,",$keywords).')';
			db::query($query);
		}
	}
	/**
	 *	@param SynchronizedLink $obj - object that contains sync link values
	 *  @param string $cvsid - cvs id under which to inser the link
	 */
	public function addToSync(SynchronizedLink $obj,$cvsid = null,$withFlag = true,$setCronFlag = false){
		assert(!is_null($obj->Name) && (!is_null($obj->URL) || !is_null($obj->FeedURL)));
		if (!is_null($obj->URL))$obj->URL = str_replace(array('"',"'"),'',$obj->URL);
		if (!is_null($obj->FeedURL))$obj->FeedURL = str_replace(array('"',"'"),'',$obj->FeedURL);
		static $sync_t = null;
		static $stmnt = null;
		if ($sync_t == null)$sync_t = new table_synclatest();
		$fields = array();
		$fields[$sync_t->field_cvsid] = is_null($cvsid)?self::DEFAULT_CVSID:$cvsid;
		$fields[$sync_t->field_hashkey] = self::createALinkHash($obj);
		$fields[$sync_t->field_userid] = $this->userid;
		if (!empty($obj->Path))
			$fields[$sync_t->field_actpath] = $obj->Path;
		$fields[$sync_t->field_info] = self::getImplodedInfo($obj);
		if (!is_null($cvsid) && $withFlag)
			$fields[$sync_t->field_flag] = 1;
		if ($setCronFlag)
			$fields[$sync_t->field_cronflag] = 1;
		//ignore duplicate key, since if hash is the same we don't want to replace it here
		$query = $query = 'INSERT DELAYED IGNORE INTO '.TABLE_SYNCLATEST
			." (".sqlFields(array_keys($fields)).") VALUES (".toSQL($fields).")";
		db::query($query);
		return db::insert_id();
	}
	public static function getImplodedInfo(SynchronizedLink $obj){
		$str = '';
		foreach($obj as $k => $v){
			if (!empty($v))
				$str .= $k.':'.preg_replace('/,/',',,',$v).',';
		}
		return mb_substr($str,0,mb_strlen($str)-1);
	}
	public static function extractInfoArray($info){
		//explode where number of commas is odd
		$info = preg_split('/(?<!,),(?=(,{2})*[^,])/',$info);
		$newInfo = array();
		foreach($info as $k => $v){
			$v = preg_split('/:/',preg_replace('/,,/',',',$v),2);
			$newInfo[preg_replace('/,/','',$v[0])] = $v[1];
		}
		return $newInfo;
	}
	public static function extractInfoObject($info){
		$info = self::extractInfoArray($info);
		$obj = new SynchronizedLink();
		foreach($info as $k => $v){
			eval('$obj->'.$k.' = $v;');
		}
		return $obj;
	}
	public static function createALinkHash(SynchronizedLink $obj){
		assert(!is_null($obj->Name) && (!is_null($obj->URL) || !is_null($obj->FeedURL)));
		$url = !is_null($obj->URL)?$obj->URL:$obj->FeedURL;
		$str = $url.(!empty($obj->Path)?$obj->Path:'').$obj->Name.(!empty($obj->ShortcutURL)?$obj->ShortcutURL:'');
		return '0x'.bin2hex(sha1($str,true));
	}
	public static function isPrivateUrl($url){
		return preg_match('/^[^:]+:[^@]+@.+$/',$url);
	}
}
?>