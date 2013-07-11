<?php
header('Server: The-Usual',true);
header('X-Powered-By: Gnafi-Engine',true);
// Create a handler function
function my_assert_handler($file, $line, $code){
	log_writeLog("<hr>Assertion Failed:
       File '$file'\r\n
       Line '$line'\r\n
       Code '$code'\r\n");
   throw new Exception("<hr>Assertion Failed:
       File '$file'<br />
       Line '$line'<br />
       Code '$code'<br /><hr />");
}
//define('DEBUG','1');
if (defined('DEBUG')){
	assert_options(ASSERT_ACTIVE, 1);
	assert_options(ASSERT_WARNING, 1);
	assert_options(ASSERT_BAIL, 1);
	assert_options(ASSERT_QUIET_EVAL, 0);
	assert_options(ASSERT_CALLBACK, 'my_assert_handler');
}
define('GLOBAL_USER','glbUser');
define('COOKIE_COMMON','cookie');
define('MESSAGE_PATH',$_SERVER['DOCUMENT_ROOT'].'/scripts/includes/messages/');	//path prepared message files
define('GLOBAL_DOMAIN_NAME','gnafi.com');
//set include path to our include directory
//ini_set('include_path',ini_get('include_path').';'.$_SERVER['DOCUMENT_ROOT'].'/scripts/includes');
define('MY_INCLUDEPATH',$_SERVER['DOCUMENT_ROOT'].'/scripts/includes/');
ini_set('include_path',MY_INCLUDEPATH);
function __autoload($class_name) {
   require_once 'main/'.strtolower($class_name).'.inc';
}
require_once('tables.inc');
require_once('DBconnect.inc');
require_once('utils/funcs.inc');

class Repository{
	protected $vars = array();
	function Controller(){
	}
	public function set($strName,$mixedValue,$overwrite = false){
		if (!$overwrite && isset($this->vars[$strName]) && defined('DEBUG'))
			echo '\n<br/><b>Warning:</b> value for "'.$strName.'" already set to "'
			.$mixedValue.'", resetting'."\n".'<br/>';
		$this->vars[$strName] = $mixedValue;
	}
	public function defined($strName){
		return isset($this->vars[$strName]);
	}
	public function get($strName){
		$ret = null;
		if (isset($this->vars[$strName]))
			$ret = $this->vars[$strName];
		return $ret;
	}
}
class Controller extends Repository{

}
define('CACHE_DIR',$_SERVER['DOCUMENT_ROOT'].'/cache/');
//initialize CRUMB_PATH even if we don't need it
if (!defined('IS_INDEX_PAGE') || !isset($_SERVER['REQUEST_URI'])){
	$_SERVER['CRUMB_PATH'] = '/';
}else{
	$_SERVER['CRUMB_PATH'] = CatPath::parseUriToPath($_SERVER['REDIRECT_URL']);
}
$_SERVER['CRUMB_ROOT'] = CatPath::crumbRoot($_SERVER['CRUMB_PATH']);
$_SERVER['CRUMB_WORD'] = CatPath::crumbWord($_SERVER['CRUMB_PATH']);

define('CRON_TYPE_IMPORT',0);
?>