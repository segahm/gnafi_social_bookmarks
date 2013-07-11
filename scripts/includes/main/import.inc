<?php
class Import{
	private $userid = null;
	function __construct($userid){
		$this->userid = $userid;
	}
	public function fromHtml(&$str,$length = null){
		if (is_null($length))
			$length = mb_strlen($str);
		$pos = 0;
		$dt_folder = 1;
		$dt_link = 2;
		$dt_tags = 3;
		$dt_unknown = 4;
		$folders = array();
		$info = array();
		$lastFolder = null;
		while($pos < $length && ($pos = mb_strpos($str,'<',$pos)) !== false){
			if (($pos+5)>$length)break;
			$html_tag = mb_strtolower(mb_substr($str,$pos+1,2));
			$debug = mb_substr($str,$pos,100);
			switch($html_tag){
				case 'dd':
					if (empty($info))break;
					$prev = $pos+4;
					$matches = array();
					if (!preg_match('~[^\r\n<]+~',substr($str,$prev),$matches))break;
					$cut = $matches[0];
					$matches = array();
					if (preg_match('~\(tags:[\s\t]+((?:[^\s\t]+[\s\t]?)+)\)~i',$cut,$matches))
						$info['t'] = preg_split('~[\s\t]~',$matches[1]);
					else
						$info['d'] = $cut;
				break;
				case 'dt':
					if (mb_strtolower(mb_substr($str,$pos+4,2)) !== '<h')break;
					$prev = $pos+6;
					$pos = mb_strpos($str,'</',$prev);
					$cut = mb_substr($str,$prev,$pos-$prev+2);
					$matches = array();
					if (preg_match('~>([^<]+)</~',$cut,$matches)){
						$lastFolder = $matches[1];
					}
				break;
				case 'dl':
					if (!is_null($lastFolder))
						$folders[] = $lastFolder;
					$lastFolder = null;
				break;
				case '/d':
					if (strtolower(substr($str,$pos+3,2)) === 'l>')
						array_pop($folders);
				break;
				case 'a ':
					if (!empty($info['url']) && !empty($info['title']))
						$this->addUrl($info);
					$info = array();
					//drop previous info
					$prev = $pos+2;
					$pos = mb_strpos($str,"</",$prev);
					$len = $pos-$prev+2;
					$cut = mb_substr($str,$prev,$len);
					$matches = array();
					if (!preg_match('~(HREF|FEEDURL)="([^"]+)"[^>]*>([^<]+)</~i',$cut,$matches))break;
					if (stristr($matches[1],'feedurl'))
						$info['isfeed'] = 1;
					$info['url'] = $matches[2];
					$info['title'] = $matches[3];
					if (!empty($folders))
						$f = CatPath::parseStrToKeywords(implode('/',$folders),false);
					else
						$f = null;
					$info['folder'] = $f;
				break;
				default:
			}
			++$pos;
		}
		if (!empty($info))
			$this->addUrl($info);
		return true;
	}
	private function addUrl($info){
		static $urlSt = null;
		static $link_t = null;
		static $key_t = null;
		static $paramStr = 'ssddsss';
		if ($urlSt === null){
			$link_t = new table_urls();
			$key_t = new table_keywords();
			//prepare url insert statement
			$fields = array($link_t->field_userid,$link_t->field_description,$link_t->field_isfeed,
					$link_t->field_private,$link_t->field_title,
					$link_t->field_url,$link_t->field_keywords);
			$s = count($fields);
			$query = 'INSERT INTO '.TABLE_URLS.' ('.sqlFields($fields)
				.") VALUES (".substr(str_repeat('?,',$s),0,$s*2-1).')';
			$urlSt = db::prepare($query);
		}
		$isprivate = LinkManager::isPrivateUrl($info['url'])?1:null;
		$isfeed = isset($info['isfeed'])?1:null;
		$description = empty($info['d'])?null:$info['d'];
		$keywords_path = array();
		if (!empty($info['folder']))$keywords_path[] = $info['folder'];
		if (!empty($info['t'])){
			$info['t'] = CatPath::explodeStrToKeywords(CatPath::parseStrToKeywords(implode(' ',$info['t']),true));
			if (!empty($info['t']))
			$keywords_path = array_merge($keywords_path,$info['t']);
		}
		if (empty($keywords_path))$keywords_path = null;
		else $keywords_path = implode(',',$keywords_path);
		$urlSt->bind_param($paramStr,$this->userid,$description,$isfeed,$isprivate,$info['title'],
				$info['url'],$keywords_path);
		if ($urlSt->execute() && !empty($keywords_path)){
			$id = db::insert_id();
			$keywords = CatPath::explodeStrToKeywords($keywords_path);
			$str = '';
			foreach($keywords as $keyword){
				$keyword = toSQL($keyword);
				$str .= "('$id',$keyword),";
			}
			$str = preg_replace('~,$~','',$str,1);
			$keyQuery = 'INSERT DELAYED INTO '.TABLE_KEYWORDS
					.' ('.$key_t->field_urlid.','.$key_t->field_keyword
					.") VALUES ".$str;
			db::query($keyQuery);
		}
	}
	public function fromDelicious($user,$pass){
	}
}

?>