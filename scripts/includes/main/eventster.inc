<?php
class Eventster{
	static $events = array();
	static $passed = array();
	/**
	 * @param string $type unique event type
	 * @param mixed $func callback function
	 * @param array $vars an array of vallues to pass to the function 
	 */
	public static function register($type,$func,$vars = null){
		//assert that we didn't run this event before
		assert(!isset(self::$passed[$type]));
		self::$events[$type][] = array($func,$vars);
	}
	public static function runEvent($type){
		//assert that we didn't run this event before
		assert(!isset(self::$passed[$type]));
		//if no one registered for this event, don't do it
		if (!isset(self::$events[$type]))return;
		$events =& self::$events[$type];
		foreach($events as $registered){
			call_user_func_array($registered[0],$registered[1]);
		}
		unset(self::$events[$type]);
		$passed[$type] = 1;
	}
	public static function isExpired($type){
		return isset(self::$passed[$type]);
	}
}
?>