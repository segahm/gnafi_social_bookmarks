<?php
function prescapeHtml($str){
	$e = array(array('&quot;','"'),array('&amp;','&'),array('&gt;','>'),array('&lt;','<'));
	$size = count($e);
	for($i=0;$i<$size;$i++){
		$str = str_replace($e[i][0],$e[i][1],$str);
	}
	return str;
}
function startsWith($subject,$with){
	if (!is_array($with)){
		$l = strlen($with);
		if (!(strlen($subject) >= $l))
			return false;
		else
			return (substr($subject,0,$l) === $with)?1:0;
	}else{
		$l2 = strlen($subject);
		$count = 0;
		foreach($with as $v){
			++$count;
			$l = strlen($v);
			if ($l2 >= $l && substr($subject,0,$l) === $v)
				return $count;
		}
		return 0;
	}
}
function endsWith($subject,$with){
	if (!is_array($with)){
		$l = strlen($with);
		$l2 = strlen($subject);
		if ($l2 < $l)
			return false;
		else
			return (substr($subject,$l2-$l) === $with)?1:0;
	}else{
		$l2 = strlen($subject);
		$count = 0;
		foreach($with as $v){
			++$count;
			$l = strlen($v);
			if ($l2 >= $l && substr($subject,$l2-$l) === $v)
				return $count;
		}
		return 0;
	}
}
//same as regular toHTML function, but formats text elements but also takes of text formatting like spaces and new lines
function toFormatedHTML($str,$cutLength = false){
	$replace = array('/  /','/\n/');
	$replace_with = array(' &nbsp;',' <br/>');
	$str = toHTML($eventData[$events_t->field_text]);
	if ($cutLength !== false)
		$str = funcs_cutLength($str,null,$cutLength);
	return preg_replace($replace,$replace_with,$str);
}
function funcs_fillForm(){
	$contents = ob_get_contents();
	if (empty($GLOBALS['glbFields'])){
		return $contents;
	}
	//if send failed, then insert post data back
	foreach ($GLOBALS['glbFields'] as $key => $value){
		$GLOBALS['glbFields'][$key] = toHTML($value);
	}
	$contents = funcs_processForm($contents,$GLOBALS['glbFields']);
	return $contents;
}
/**
 * @param string $dir directory in which to remove the contents
 * @param array $ignorePaths array containing paths which shouldn't be removed as values (directories should be ended with "/")
 * @param boolean $_top for inner keeping
 * @param callback_function $func function to find out wherever to remove a file or not
 * @return true if everything under the directory tree was deleted, false otherwise
 */
function recursive_delete($dir,$ignorePaths = array(),$func = false,$_top = true){
	//precotionary step:
	if (!preg_match('/gnafi/i',$dir))
		return false;
	if ($dir{strlen($dir-1)} != '/')
		$dir .= '/';
	$flag = true;
	if ($_top)
		$ignorePaths = array_flip($ignorePaths);
	if ($dh = @opendir($dir)) {
		while (($file = readdir($dh)) !== false ) {
			if( $file != "." && $file != ".." ){
				if( is_dir($dir.$file ) ){
					if (recursive_delete( $dir.$file."/",$ignorePaths,$func,false)
						&& !isset($ignorePaths[$dir .$file.'/'])
						&& ($func === false || call_user_func($func,$dir.$file))
						){
						if(defined('DEBUG')){
							log_writeLog('removing '.$dir . $file."\n");
						}
						rmdir($dir.$file);
					}else{
						$flag = false;
					}
				}else{
					if (!isset($ignorePaths[$dir .$file ]) && ($func === false || call_user_func($func,$dir.$file))){
						if(defined('DEBUG')){
							log_writeLog('removing '.$dir . $file."\n");
						}
						unlink( $dir . $file );
					}else{
						$flag = false;
					}
				}
			}
		}
		closedir($dh);
	}
	return $flag;
}
/**physically resizes image either jpeg/gif to the same location or 
 *optionally to a different destinarion;
 *if it is jpeg file then an optinal quality parameter is used*/
function funcs_resizeImageFile($file,$newWidth,$newHeight,$oldWidth,$oldHeight,$destination = null,$quality = 75){
	//create a new pallete for both source and destination
	$dest = imagecreatetruecolor($newWidth, $newHeight);
	if (preg_match('/\.gif$/',$file)){
		$source = imagecreatefromgif($file);
	}else{
		//assume jpeg since we only allow jpeg and gif
		$source = imagecreatefromjpeg($file);
	}
	//copy the source pallete onto destination pallete
	imagecopyresampled($dest,$source,0,0,0,0, $newWidth, $newHeight, $oldWidth,$oldHeight);
	//save file
	if (preg_match('/\.gif$/',$file)){
		imagegif($dest,((is_null($destination))?$file:$destination));
	}else{
		imagejpeg($dest,((is_null($destination))?$file:$destination),$quality);
	}
}
/**takes sql date and returns a timestamp represented by this date
 *this is simply a shortcur to the class date*/
function funcs_getTimeStamp($date){
	$date = new Mydate($date);
	return $date->getStamp();
}
/**if string is longer than length it will cut it and add ... at the end*/
function funcs_cutLength($string, $length, $word_length = null,$act_length = null){
	$string = (!is_null($string))?trim($string):'';
	//do not check the size of the string if we already know it
	if (is_null($act_length))$act_length=mb_strlen($string);
	if (!is_null($word_length) && !empty($string)){
		//in the future use this instead of calling this whole function
		$string = wordwrap($string,$word_length,"\n",1);
	}
	if (!is_null($length) && $act_length>$length && $length>4){
		$string = mb_substr($string,0,$length-3).'...';
	}
	return $string;
}
/**takes html text as an input and sets values according to the names
 *of the fields, i.e.:
 *for radio and checkboxes it sets checked if 
 *fields[name] == value and for text it sets value = fields[name],
 *for select it sets it to the option that has te same value*/
function funcs_processForm($text,$fields){
	//do not scan the text if the fields are empty
	if (empty($fields)){
		return $text;
	}
	function input_replace($repl,$instr){
		$pos = strrpos($instr,'>');
		assert($pos !== false && $pos > 0);
		$l = 1;
		if ($instr{$pos-1} == '/'){
			--$pos;++$l;
		}
		return substr_replace($instr,$repl.'/>',$pos,2);
	}
	$pos = 0;
	$element = '<';
	//iterate through each element
	while (($pos = strpos($text,$element,$pos)) !== false){
		if (preg_match('/^<input$/i',substr($text,$pos,6))){
			$tag_type = 1;
			$element2 = '>';
		}elseif (preg_match('/^<select$/i',substr($text,$pos,7))){
			$tag_type = 2;
			$element2 = '</select>';
		}elseif (preg_match('/^<textarea$/i',substr($text,$pos,9))){
			$tag_type = 3;
			$element2 = '</textarea>';
		}else{
			$pos += 1;
			continue;
		}
		//keep grabing pieces with bigger size while not full element object
		$size = 50;
		do{
			$grabed_string =& substr($text,$pos,$size);
			$size += 10;
		}while(($secPos = strpos($grabed_string,$element2)) === false);
		$grabed_string = substr($grabed_string,0,$secPos+strlen($element2));
		if ($tag_type == 1){
			//get element's properties
			$type = preg_replace('/^.*type=[\'"]?([^"\'> ]+).*$/is','\\1',$grabed_string);
			$name = preg_replace('/^.*name=[\'"]?([^"\'> ]+).*$/is','\\1',$grabed_string);
			if (preg_match('/^.*value=[\'"]?([^"\'> ]+).*$/i',$grabed_string)){
				$value = preg_replace('/^.*value=[\'"]?([^"\'> ]+).*$/is','\\1',$grabed_string);
			}else{
				$value = null;
			}
			if (($type == 'checkbox' || $type == 'radio')
					&& isset($fields[$name]) && $fields[$name] == $value ){
				//need to select this value
				$replacement = ' checked';
				$new_string =  input_replace($replacement,$grabed_string);
				$text =& substr_replace($text,$new_string,$pos,strlen($grabed_string));
			}elseif(($type == 'text' || $type == 'hidden') && isset($fields[$name])){
				$replacement = ' value="'.$fields[$name].'"';
				$new_string = preg_replace('/value=[^ >]+/','',$grabed_string);
				$new_string = input_replace($replacement,$new_string);
				$text =& substr_replace($text,$new_string,$pos,strlen($grabed_string));
			}
		}elseif($tag_type == 2){
			//select tag
			$names = array();
			preg_match('/name=[\'"]?([^"\'\> ]+)/i',$grabed_string,$names);
			$name = preg_replace('/^.*=[\'"]?(.+)$/i','\\1',$names[0]);
			if (isset($fields[$name])){
				if ($fields[$name] !== ''){
					$new_string = preg_replace('/(<option[^>]+value=[\'"]?'.$fields[$name].'\b[\'"]?[^\/>]*)([\/]?)>/i','\\1 selected\\2>',$grabed_string);
				}else{
					$new_string = preg_replace('/(<option[^>]+value=[\'"][\'"][^\/>]*)([\/]?)>/i','\\1 selected\\2>',$grabed_string);
				}
				$text = substr_replace($text,$new_string,$pos,strlen($grabed_string));
			}
		}elseif($tag_type == 3){
			//textarea tag
			$names = array();
			preg_match('/name=[\'"]?([^"\'\> ]+)/i',$grabed_string,$names);
			$name = preg_replace('/^.*=[\'"]?(.+)$/i','\\1',$names[0]);
			if (isset($fields[$name])){
				$pos1st = strpos($grabed_string,'>');
				$replacement = $fields[$name];
				$length = strlen($grabed_string)-($pos1st+1);
				$new_string = substr_replace($grabed_string,
					$replacement.'</textarea>',$pos1st+1,
					$length);
				$text =& substr_replace($text,$new_string,$pos,strlen($grabed_string));
			}
		}
		$pos += $secPos;
	}
	return $text;
}
function toHTML($variable){
	if (!isset($variable) || is_null($variable)){
		return '';
	}else{
		return htmlspecialchars(trim($variable));
	}
}
/**converts a string into a valid sql string*/
function tosql_string($string,$withquotes = false){
	if (empty($string) && (!isset($string) || $string!='0')){
		$string = "NULL";
	}else{
		if (is_string($string)){
			//$string = trim(addslashes($string));
			$string = mysqli_escape_string(db::$link,$string);
		}
		if ($withquotes)
			$string = "'".$string."'";
	}
	return $string;
}
/**converts an array to strings separating values with commas*/
function array_tostring($array){
	$string = '';
	foreach($array as $value){
		if ($string != ''){
			$string .= ',';
		}
		$string .= $value;
	}
	return $string;
}
function sqlFields($mixed){
	if (!is_array($mixed)){
		if (strpos($mixed,'.') === false)
			$mixed = '`'.$mixed.'`';
		return $mixed;
	}
	$string = '';
	foreach($mixed as $value){
		if ($string != ''){
			$string .= ',';
		}
		if (strpos($value,'.') === false)
			$string .= '`'.$value.'`';
		else
			$string .= $value;
	}
	return $string;
}
/**converts both arrays and single values into sql*/
function toSQL($mixed,$quotes = true){
	if (is_null($mixed)){
		return 'NULL';
	}
	if (!is_array($mixed)){
		return tosql_string($mixed,$quotes);
	}
	$string = '';
	foreach($mixed as $value){
		if ($string != ''){
			$string .= ',';
		}
		$string .= tosql_string($value,$quotes);
	}
	return $string;
}
function funcs_isValidPass($pass){
	if (empty($pass) 
			|| !preg_match('/^[a-zA-Z0-9]+$/',trim($pass))
		    || strlen($pass)<4){
		return false;
	}else{
		return true;
	}
}
class LogWriter{
	private $out = null;
	private $length = 0;
	const MAX_FILE_SIZE = 2000000;
	function __construct(){
		$this->out = '';
	}
	public function add($message){
		if ($this->length > 2097152)return;
		$this->out .= date("F j, Y, g:i a") . ":\n";
		$this->out .= $message . "\n";
		$this->length += strlen($message);
	}
	function __destruct(){
		
		$dir = preg_replace('-[/\\\\][^/\\\\]+$-','',$_SERVER['DOCUMENT_ROOT']).'/logs/';
		if (!file_exists($dir))mkdir($dir,0755);
		$tryAgain = true;
		$c = 1;
		while($tryAgain){
			$filename = $dir."logs$c.txt";
			$write = 'a';
			if (!file_exists($filename) || filesize($filename)<self::MAX_FILE_SIZE){
				$tryAgain = false;
			}
			++$c;
		}
		$fp = @fopen($filename,$write);
		if ($fp){
			fwrite($fp,$this->out,strlen($this->out));
			fclose($fp);
		}
	}
	
}
function log_writeLog($message){
	static $logClass = false;
	if ($logClass === false)$logClass = new LogWriter();
	$logClass->add($message);
	return true;
}
function log_writeError($errno, $errstr, $errfile, $errline){
	$out = '';
	switch ($errno) {
	  	case E_USER_ERROR:
	  		$out .= "<b>My ERROR</b> [$errno] $errstr<br />\n";
	   		$out .= "  Fatal error in line $errline of file $errfile";
	   		$out .= ", PHP " . PHP_VERSION . " (" . PHP_OS . ")<br />\n";
	   		$out .= "Aborting...<br />\n";
	   	break;
	  		case E_USER_WARNING:
	   		$out .= "<b>My WARNING</b> [$errno] $errstr<br />\nin line $errline of file $errfile";
	   	break;
	  		case E_USER_NOTICE:
	   		$out .= "<b>My NOTICE</b> [$errno] $errstr<br />\nin line $errline of file $errfile";
	   	break;
	  	default:
	   		$out .= "Unkown error type: [$errno] $errstr<br />\nin line $errline of file $errfile";
	   	break;
	}
	log_writeLog($out);
}
function funcs_imageResize(&$width, &$height, $target, $bywidth = null) { 
	//do not resize if already smaller than the target
	if ($width<=$target && $height<=$target){
		return;
	}
	//takes the larger size of the width and height and applies the  
	//formula accordingly...this is so this script will work  
	//dynamically with any size image 
	if (is_null($bywidth)){
		if ($width > $height) { 
			$percentage = ($target / $width); 
		} else { 
			$percentage = ($target / $height); 
		}
	}else{
		if ($bywidth){
			$percentage = ($target / $width);
			//if height would increase as a result
			if ($height < round($height * $percentage)){
				return;
			} 
		}else{
			$percentage = ($target / $height); 
			//if width would increase as a result
			if ($width < round($width * $percentage)){
				return;
			}
		}
	} 
	//gets the new value and applies the percentage, then rounds the value 
	$width = round($width * $percentage); 
	$height = round($height * $percentage); 
}
function isValidImage($image_file,$max_size){
	if ($image_file['size']>$max_size){
		return 'the file is too big';
	}
	if (empty($image_file['type']) || !preg_match('/jpg|jpeg|gif/i',$image_file['type'])){
		return 'the image must be either jpeg or gif';
	}
	if (!empty($image_file['error'])){
		return 'we were unable to process this file';
	}
	if (!preg_match('/^[^\.]+\.(jpg|jpeg|gif)$/i',$image_file['name'])){
		return 'the image must be either jpeg or gif';
	}
	return true;
}
function &array_difKeys($array1,$array2){
	foreach($array1 as $key => $value){
		unset($array2[$key]);
		unset($array1[$key]);
	}
	return array_merge($array1,$array2);
}
function genIDString($length){
	$str = 'qwertyuiopasdfghjklzxcvbnm0123456789QWERTYUIOPASDFGHJKLZXCVBNM';
	$result = '';
	$len = strlen($str);
	for ($i=0;$i<$length;$i++){
		$result .= $str{rand(0,$len-1)};
	}
	return $result;
}
function genNumberString($length){
	$str = '0123456789';
	$result = '';
	$len = strlen($str);
	for ($i=0;$i<$length;$i++){
		$result .= $str{rand(0,$len-1)};
	}
	return $result;
}
function myMail($ishtml, $to, $subject, $message,$note_ip = false){
	if ($ishtml == true){
		$ishtml = 'true';
	}else{
		$ishtml = 'false';
	}
	$mailTable = new table_mail();
	if (is_array($to)){
		$toEmail = implode(',',$to);
	}else{
		$toEmail = $to;
	}
	if ($ishtml){
		$message = preg_replace('/[\r\n]+/',"\n",$message);
	}
	$query = 'INSERT DELAYED INTO '.TABLE_MAIL.' (`'.$mailTable->field_to.'`,`'
		.$mailTable->field_message.'`,`'.$mailTable->field_ip.'`,`'.$mailTable->field_date
		.'`) VALUES ('.toSQL($toEmail).','.toSQL($subject.'<<delim>>'.$message).',INET_ATON('
		.toSQL(($note_ip)?$_SERVER['REMOTE_ADDR']:'127.0.0.1').'),NOW())';
	return db::query($query);
}
//simply returns count of the last executed statement
function sqlFunc_getCount(){
	$query = "SELECT FOUND_ROWS()";
	$result = db::query($query);
	if ($row = db::fetch_row($result)){
		return (int)$row[0];
	}else{
		return 0;
	}
}
function toChars($str,$length = null){
	if (is_null($length))$length = strlen($str);
	$ar = array();
	for ($i=0;$i<$length;$i++){
		$ar[] = $str{$i};
	}
	return $ar;
}
?>