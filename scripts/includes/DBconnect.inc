<?php
class db{
	function __construct(&$link){
		$this->link =& $link;
	}
	function __destruct(){
		self::$link->close();
	}
	public static $link;
	private static $obj;
	public static function &connect(){
		//if already connected, then ignore request
		if (is_null(self::$link)){
			self::$link = new mysqli("localhost","slugt2_php", "alternblosomka1",'slugt2_gnafi')
				or (error_log('Error connecting to the database: '.mysqli_connect_error()) and die());
			self::$obj = new db($link);
			return self::$link;
		}else{
			return self::$link;
		}
	}
	public static function close(){
		if (!is_null(self::$link)){
			self::$obj = null;
			self::$link = null;
			return true;
		}
		return false;
	}
	public static function query($query){
		assert(self::$link);
		$obj =& self::$link;
		$result = $obj->query($query) or 
			($error = ("Query failed\n".$query."\n".$obj->error) 
				 and error_log($error) and die());
		return $result;
	}
	public static $lastNotEmpty = false;
	public static $lastResultSet = null;
	public static function fetch_row($result){
		$res = mysqli_fetch_row($result);
		if (self::$lastResultSet != null){
			self::$lastResultSet = $result;
			if ($res)self::$lastNotEmpty = true;
			else self::$lastNotEmpty = false;
			self::$lastResultSet = null;
		}
		return $res;
	}
	public static function fetch_array($result){
		$res = mysqli_fetch_array($result);
		if (self::$lastResultSet != null){
			self::$lastResultSet = $result;
			if ($res)self::$lastNotEmpty = true;
			else self::$lastNotEmpty = false;
			self::$lastResultSet = null;
		}
		return $res;
	}
	public static function fetch_assoc($result){
		$res = mysqli_fetch_assoc($result);
		if (self::$lastResultSet != null){
			self::$lastResultSet = $result;
			if ($res)self::$lastNotEmpty = true;
			else self::$lastNotEmpty = false;
			self::$lastResultSet = null;
		}
		return $res;
	}
	public static function num_fields($result){
		return mysqli_num_fields($result);
	}
	public static function num_rows($result){
		return mysqli_num_rows($result);
	}
	public static function affected_rows(){
		return mysqli_affected_rows(self::$link);
	}
	public static function prepare($query){
		$st = mysqli_prepare(self::$link,$query);
		if (!$st)die('Statement Error:'.self::$link->error);
		return $st;
	}
	public static function insert_id(){
		return mysqli_insert_id(self::$link);
	}
	public static function free_result($result){
		return mysqli_free_result($result);
	}
}
?>