<?php
define('FROM_PATH','C:/webdeploy/newsite/phpscripts');
define('TO_PATH','C:/webdeploy/test/phpscripts');
define('TYPE_PHP',1);
define('TYPE_HTML',3);
define('TYPE_JS',2);
define('TYPE_CSS',4);
define('CONVERSION_FILE',1);
define('CONVERSION_DIR',2);
define('TYPE_CONVERSION',CONVERSION_DIR);
foreach ($argv as $arg){
	if (!empty($arg) && $arg{0} === '~'){
		$length = strlen($arg);
		for ($i=1;$i<$length;$i++){
			if ($arg{$i} === 'f'){
				define('FORCE',1);
			}elseif($arg{$i} === 'c'){
				define('COPY_ALL',1);
			}
		}
	}
}
if (TYPE_CONVERSION === CONVERSION_DIR){
	if (!file_exists(FROM_PATH) || !is_dir(FROM_PATH)){
		die('bad from path');
	}
	dealDir(FROM_PATH,TO_PATH);
}else{
	$exists = file_exists(TO_PATH);
	$to_exists = file_exists(TO_PATH);
	if (!file_exists(FROM_PATH) 
		|| !is_file(FROM_PATH) 
		|| ($to_exists && !is_file(TO_PATH))){
		die('invalid to/from file');
	}
	$type = whichType(FROM_PATH);
	if (is_null($type)){
		die('invalid file type');
	}
	dealFile(FROM_PATH,TO_PATH,$type);
}
function dealDir($dir,$to_dir){
	if (!file_exists($to_dir)){
		mkdir($to_dir,0755) or die('failed creating a new directory');
	}
	echo 'directory: ',$dir,"\n";
	$files = scandir($dir);
	foreach($files as $file){
		if (preg_match('/^[\.]+$/',$file)){
			continue;
		}
		$full_name = $dir.'/'.$file;
		$to_file_name = $to_dir.'/'.$file;
		//if directory
		if (is_dir($full_name)){
			dealDir($full_name,$to_file_name);
			continue;
		}
		//if we are not interested in this file type
		$type = whichType($full_name);
			//ask the user wherever to copy this file
		if (is_null($type) && !preg_match('/\.(lck|bak)$/i',$file)){
			if (defined('FORCE')){
				//don't ask
				if (defined('COPY_ALL')){
					if (copy($full_name,$to_file_name)){
						echo 'copied '.$file."\n";
					}else{
						echo 'failed to copy '.$file."\n";
					}
				}
			}else{
				//ask
				echo 'copy file: '.$file.' ([y/yes,n/no])? ';
				$answer = trim(fgets(STDIN));
				if (!empty($answer) && preg_match('/^y/i',$answer)){
					if (copy($full_name,$to_file_name)){
						echo 'copied '.$file."\n";
					}else{
						echo 'failed to copy '.$file."\n";
					}
				}
			}
			continue;
		}
		//parse file
		dealFile($full_name,$to_file_name,$type);
	}
}
function dealFile($from_path,$to_path,$type){
	echo 'parsing: ',substr($from_path,strrpos($from_path,'/')+1),"\n";
	$cont = file_get_contents($from_path);
	$replace = array("\r","\t");
	$with = array("\n",' ');
	$cont = str_replace($replace,$with,$cont);
	$indx = 0;
	$length = strlen($cont);
	if ($type == TYPE_PHP){
		//if it is a php file, still send it to html parsing with php file type marker
		parseHTML($cont,$indx,$length,TYPE_PHP);
	}elseif ($type == TYPE_HTML){
		parseHTML($cont,$indx,$length,TYPE_HTML);
	}elseif ($type == TYPE_JS){
		parseJS($cont,$indx,$length,TYPE_JS);
	}elseif ($type == TYPE_CSS){
		parseCSS($cont,$indx,$length,TYPE_CSS);
	}
	file_put_contents($to_path,$cont);
}
function deleteChar(&$text,$indx,&$length){
	$str1 = substr($text,0,$indx);
	if ($text{$indx} == ' '){
		$offset = 1;
		--$length;
	}else{
		$offset = 1;
		$length -= 1;
	}
	if ($indx<$length){
		$str2 = substr($text,$indx+$offset);
	}else{
		$str2 = '';
	}
	$text = ($str1.$str2);
}
//assumes that text begins with html
function parseHTML(&$text,&$indx,&$length,$file_type){
	$in_tag = false;	//holds quotes,comments
	$in_quote = false;
	$in_comment = false;
	$in_pre = false;
	while ($indx < $length){
		$char = $text{$indx};
		//if we need to check for php stuff
		if ($file_type == TYPE_PHP 
			&& $char === '<' && $text{$indx+1} === '?'){
			//allow php to move the indx to the right position
			parsePHP($text,$indx,$length);
			continue;
		}
		if ($in_comment !== false){
			if ($char === '-' && ($indx+2)<$length && $text{$indx+1} === '-'
				&& $text{$indx+2} === '>'){
				$substr = strlen(substr($text,$in_comment,($indx+3)-$in_comment));
				$length = $length - $substr;
				$text = substr_replace($text,'',$in_comment,$substr);
				$indx = $in_comment;
				$in_comment = false;
			}else{
				++$indx;
			}
			continue;
		}
		if (!$in_quote && $in_tag && $char === '>'){
			if ($in_pre && strtolower(substr($text,$indx-4,4)) === '/pre'){
				$in_pre = false;
			}
			$in_tag = false;
			++$indx;
			continue;
		}
		if (!$in_quote && $char === '<'){
			if (($indx+6)<$length && strtolower(substr($text,$indx,7)) === '<script'){
				$indx = strpos($text,'>',$indx);
				if ($indx === false)die('failed to find the end of the script tag');
				++$indx;
				parseJS($text,$indx,$length,$file_type);
				continue;
			}elseif(($indx+5)<$length && strtolower(substr($text,$indx,7)) === '<style'){
				$indx = strpos($text,'>',$indx);
				if ($indx === false)die('failed to find the end of the style tag');
				++$indx;
				parseCSS($text,$indx,$length,$file_type);
				continue;
			}else{
				if(!$in_pre && ($indx+3)<$length && strtolower(substr($text,$indx,4)) === '<pre'){
					$in_pre = true;
				}elseif(($indx+3)<$length && substr($text,$indx,4) === '<!--'){
					$in_comment = $indx;
				}
				$in_tag = true;
				++$indx;
			}
			continue;
		}
		if ($in_tag){
			if ($in_quote && $char === '"' && $text{$indx-1} !== '\\'){
				$in_quote = false;
			}elseif(!$in_quote && $char === '"'){
				$in_quote = true;
			}elseif(!$in_quote && $char === '>'){
				$in_tag = false;
			}
			++$indx;
			continue;
		}
		if ($in_pre){
			++$indx;
			continue;
		}
		if(ord($char) == 10){
			//replace \n with a space
			$text = substr_replace($text,' ',$indx,1);
		}elseif ($char === ' ' 
				&& ((($indx+1)<$length && ($text{$indx+1} === ' ' || $text{$indx+1} == "\t"))
				|| (($indx-1)>=0 && ($text{$indx-1} === ' ' || $text{$indx-1} == "\t")))){
			deleteChar($text,$indx,$length);
		}else{
			++$indx;
		}
	}
}
function parseCSS(&$text,&$indx,&$length,$file_type){
	$in_quotes = false;
	while ($indx < $length){
		$char = $text{$indx};
		//if we need to check for php stuff
		if ($file_type == TYPE_PHP 
			&& $char === '<' && $text{$indx+1} === '?'){
			//allow php to move the indx to the right position
			parsePHP($text,$indx,$length);
			continue;
		}
		//if we need to check for html stuff
		if ($file_type == TYPE_HTML 
				&& $char === '<' && $in_quotes === false){
			//allow html to move the indx to the right position
			parseHTML($text,$indx,$length,$file_type);
			return;
		}
		if ($in_quotes !== false){
			if ($in_quotes === '"' && $char === '"'){
				$in_quotes = false;
			}elseif($in_quotes === "'" && $char === "'"){
				$in_quotes = false;
			}
			++$indx;
			continue;			
		}elseif($char === "'" || $char === '"'){
			$in_quotes = $char;
			++$indx;
			continue;
		}
		if(ord($char) == 10 && ($indx == 0
					|| ($text{$indx-1} !== '}' && $text{$indx-1} !== '"' && $text{$indx-1} !== "'"
						&& !is_numeric($text{$indx-1}) && !is_letter($text{$indx-1})))){
			//delete \n
			deleteChar($text,$indx,$length);
		}elseif ($char === ' ' 
				&& ((($indx+1)<$length && ($text{$indx+1} === ' ' || $text{$indx+1} == "\t"))
				|| (($indx-1)>=0 && ($text{$indx-1} === ' ' || $text{$indx-1} == "\t")))){
			deleteChar($text,$indx,$length);
		}else{
			++$indx;
		}
	}
}
function parsePHP(&$text,&$indx,&$length){
	$in_quotes = false;
	while ($indx < $length){
		$char = $text{$indx};
		if ($in_quotes === false){
			if ($char === '/' && ($indx+1)<$length && $text{$indx+1} === '*'){
				//find the end of the comment
				$indx = strpos($text,'*/',$indx);
				if ($indx === false)die('failed to find the end of the php multiline comment');
				$indx += 2;
				continue;
			}elseif ($char === '/' && ($indx+1)<$length && $text{$indx+1} === '/'){
				//find the end of the comment
				$indx = strpos($text,"\n",$indx);
				if ($indx === false)die('failed to find the end of the php line comment');
				$indx += 1;
				continue;
			}elseif ($char === '"'){
				$in_quotes = '"';
			}elseif($char === "'"){
				$in_quotes = "'";
			}elseif($char === '?' && ($indx+1)<$length && $text{$indx+1} === '>'){
				//finished php parsing
				$indx+=2;
				return;
			}
		}elseif($char === "'" && $in_quotes === "'" && $text{$indx-1} !== '\\'){
			$in_quotes = false;
		}elseif($char === '"' && $in_quotes === '"' && $text{$indx-1} !== '\\'){
			$in_quotes = false;
		}
		++$indx;
	}
}
function parseJS(&$text,&$indx,&$length,$file_type){
	$stack = null;	//holds quotes,comments
	$temp = null;	//holds any temporary information for stack (such as position)
	while ($indx < $length){
		$char = $text{$indx};
		//if we need to check for php stuff
		if ($file_type == TYPE_PHP 
			&& $char === '<' && $text{$indx+1} === '?'){
			//allow php to move the indx to the right position
			parsePHP($text,$indx,$length);
			continue;
		}
		if (is_null($stack)){
			switch($char){
				//if comment
				case '/':
					if (($indx+1)<$length){
						if ($text{$indx+1} === '/' 
							&& (($indx+4>=$length || substr($text,$indx,5) !== '//-->'))){
							//if line comment (but not //-->)
							$stack = '//';
							$temp = $indx;
							$indx+=2;
						}elseif($text{$indx+1} === '*'){
							//if multiline comment
							$stack = '/*';
							$temp = $indx;
							$indx+=2;
						}
					}
					break;
				case '"':
					$stack = '"';
					$temp = $indx;
					++$indx;
					break;
				case "'":
					$stack = "'";
					$temp = $indx;
					++$indx;
					break;
			}
			//if we set stack
			if (!is_null($stack)){
				continue;
			}
		}else{
			if ($stack === '/*'){
				if ($char === "*" && $text{$indx+1} === '/'){
					++$indx;
					$substr = strlen(substr($text,$temp,$indx-$temp+1));
					$length = $length - $substr;
					$text = substr_replace($text,'',$temp,$substr);
					$indx = $temp;
					$stack = null;
				}else{
					//else continue while we encounter the end of the comemnt
					++$indx;
				}
				continue;
			}
			if ($stack === '//'){
				if ($char === "\n"){
					$substr = strlen(substr($text,$temp,$indx-$temp+1));
					$length = $length - $substr;
					$text = substr_replace($text,'',$temp,$substr);
					$indx = $temp;
					$stack = null;
				}else{
					//else continue while we encounter the end of the line
					++$indx;
				}
				continue;
			}
			if ($stack === '"'){
				if ($char === '"' && $text{$indx-1} !== '\\'){
					$stack = null;
				}
				++$indx;
				continue;
			}
			if ($stack === "'"){
				if ($char === "'"  && $text{$indx-1} !== '\\'){
					$stack = null;
				}
				++$indx;
				continue;
			}
		}
		//if still here then go ahead delete characters according to the rules
		//first check for html tags
		if (($file_type == TYPE_PHP || $file_type == TYPE_HTML)
			&& $char === '<' 
			&& (($indx+7)<$length && strtolower(substr($text,$indx,8)) === '</script')){
			//allow html to move the indx to the right position
			parseHTML($text,$indx,$length,$file_type);
			return;
		}
		$prev = (($indx == 0)?null:$text{$indx-1});
		$next = (($indx == ($length-1))?null:$text{$indx+1});
		if (($char === ' ') 
			&& ((is_null($prev) 
				|| (!is_numeric($prev) && !is_letter($prev)) 
					&& is_ascii($prev) && strpos('$_\\',$prev) === false)
			&& (is_null($next) 
				|| (!is_numeric($next) && !is_letter($next) 
					&& is_ascii($next) && strpos('$_\\',$next) === false)))
			){
			//delete space
			deleteChar($text,$indx,$length);
		}elseif(ord($char) == 10
				&& (is_null($next) 
					|| (!is_numeric($next) && !is_letter($next) 
						&& is_ascii($next) && strpos('\$_{[(+-',$next) === false))
				&& (is_null($prev) 
					|| (!is_numeric($prev) && !is_letter($prev) 
						&& is_ascii($prev) && strpos('\\$_}])+-"\'',$prev) === false))){
			//delete \n
			deleteChar($text,$indx,$length);
		}else{
			++$indx;
		}
	}
}
function is_ascii($char){
	$ascii = ord($char);
	if ($ascii>=0 && $ascii<=127)return true;
	return false;
}
function is_letter($char){
	$ascii = ord($char);
	if (($ascii >= 65 && $ascii <= 90) || ($ascii >= 97 && $ascii <= 122)){
		return true;
	}
	return false;
}
function whichType($filename){
	if (preg_match('/^.*\.(?:php|inc)$/i',$filename)){
		return TYPE_PHP;
	}elseif (preg_match('/^.*\.(?:html|htm)$/i',$filename)){
		return TYPE_HTML;
	}elseif(preg_match('/^.*\.(?:js)$/i',$filename)){
		return TYPE_JS;
	}elseif(preg_match('/^.*\.(?:css)$/i',$filename)){
		return TYPE_CSS;
	}
	return null;
} 
?>