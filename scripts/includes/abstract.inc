<?php
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
?>