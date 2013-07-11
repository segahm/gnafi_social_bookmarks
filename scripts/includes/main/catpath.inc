<?php
class CatPath{
	public static function parseStrToKeywords($str,$using_spaces){
		if (empty($str))return '';
		$str = mb_strtolower($str);
		$str = preg_replace('!(?:,?(?:(?:imported.ie.favorites)|(?:bookmarks.toolbar.folder)|(?:from.internet.explorer)|(?:links))/?)|(?:[^a-z, /_\\-])!','',$str);
		$keywords = preg_split('-[\s\t]*['
			.($using_spaces?' ':'').',]+[\s\t]*-',$str);
		$keywordsToMatch = false;
		if (!empty($keywords[0]) && preg_match('-/-',$keywords[0])){
			$keywords[0] = preg_replace(array('-/{2,}-','-(?:^/)|(?:/$)-','-([^/]{'.table_keywords::KEYWORD_LENGTH.'})[^/]+-'),
										array('/','','$1'),
										$keywords[0]);
			if (!empty($keywords[0]))
				$keywordsToMatch = preg_split('-/-',$keywords[0]);
			else
				unset($keywords[0]);
		}
		foreach($keywords as $k => $keyword){
			//trim
			$keyword = preg_replace('-(?:^[\s\t])|(?:[\s\t]$)-','',$keyword);
			if (!$using_spaces){
				$keyword = preg_replace('/ +/','_',$keyword);
			}
			if (mb_strlen($keyword) < 3 
					|| ($k > 0 && $keywordsToMatch && in_array($keyword,$keywordsToMatch))){
				unset($keywords[$k]);
			}else{
				if ($k > 0 && mb_strlen($keyword) > table_keywords::KEYWORD_LENGTH ){
					$keyword = wordwrap($keyword,table_keywords::KEYWORD_LENGTH-3,'...',true);
				}
				if (empty($keyword))
					unset($keywords[$k]);
				else
					$keywords[$k] = $keyword;
			}
		}
		return implode(',',$keywords);
	}
	public static function explodeStrToKeywords($str){
		if (empty($str))return array();
		else return preg_split('-(?<=.)[/,](?=.)-',$str);
	}
	public static function explodeStrToRealKeywords($str){
		if (empty($str))return array();
		$ar =  preg_split('-(?<=.),(?=.)-',$str);
		if (!empty($ar[0]) && preg_match('-(?<=.)/(?=.)-',$ar[0]))
			$ar[0] = preg_split('-(?<=.)/(?=.)-',$ar[0]);
		return $ar;
	}
	public static function parsePathToTree($path){
		$path = trim($path);
		if ($path === '' || $path === '/')return array();
		else return preg_split('-(?<=.)/(?=.)-',$path);
	}
	public static function parseUriToPath($uri){
		if (!preg_match('!^/p/!i',$uri))return '/';
		$uri = substr($uri,3);
		//strip everything on the right from the first illegal character
		$path = preg_replace('!/$!','',strtolower(preg_replace(
					array('/[^\+a-z_\/].*/i','-/{2,}-','-(?:^/)(?:/$)-','-(?:[\+_]|(?:\%20))+-'),
							array('','/','','_'),$uri)));
		return $path;
	}
	public static function crumbWord($path){
		$crumbWord = array();
		if (preg_match('/[^\/]+$/',$path,$crumbWord))
			return $crumbWord[0];
		else
			return '';
	}
	public static function crumbRoot($path){
		if ($path == '')$path = '/';
		return preg_replace('/[^\/]+$/','',$path);
	}
	public static function implodeTreeToPath($tree){
		$path = '/'.implode('/',$tree);
		$path = str_replace('_','+',$path);
		return $path;
	}
	public static function appendWord($path,$word){
		$tree = self::parsePathToTree($path);
		$tree[] = $word;
		return self::implodeTreeToPath($tree);
	}
	public static function getIndexes($tree){
		assert(!empty($tree));
		$key_t = new table_keywords();
		$query = 'SELECT '.$key_t->field_urlid.' FROM '.TABLE_KEYWORDS.' WHERE '.$key_t->field_keyword
			.' IN ('.toSQL($tree).')';
		$result = db::query($query);
		$res = array();
		while($row = db::fetch_row($result)){
			$res[] = $row[0];
		}
		return $res;
	}
}
?>