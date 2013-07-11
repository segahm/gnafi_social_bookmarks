<?php
class Mydate{
	public $timestamp;
	function __construct($mysqldate){
		$this->timestamp = self::toStamp($mysqldate);
	}
	public function isToday($timestamp,$local){
		if ((int)date('Ymd',$timestamp) == (int)date('Ymd',$local)){
			return true;
		}else{
			return false;
		}
	}
	public function getStamp(){
		return $this->timestamp;
	}
	public function difference($date2,$date1 = NULL){
		if (is_null($date1)){
			$date1 = $this->timestamp;
		}
		return (self::toStamp($date1) - self::toStamp($date2));
	}
	public static function sqlDate($stamp){
		return date('Y-m-d',$stamp);
	}
	public static function sqlDateTime($stamp){
		return date('Y-m-d H:i:s',$stamp);
	}
	public static function toStamp($mysqldate){
		if (preg_match('/^[0-9]+$/',$mysqldate)){
			//if timestamp already
			return $mysqldate;
		}else if (eregi('^....-..-..[[:space:]]..:..:..$',$mysqldate)){
			//if full date
			$h = ereg_replace('^....-..-..[[:space:]](..):..:..$','\\1',$mysqldate);
			$m = ereg_replace('^....-..-..[[:space:]]..:(..):..$','\\1',$mysqldate);
			$s = ereg_replace('^....-..-..[[:space:]]..:..:(..)$','\\1',$mysqldate);
			$mon = ereg_replace('^....-(..)-..[[:space:]]..:..:..$','\\1',$mysqldate);
			$d = ereg_replace('^....-..-(..)[[:space:]]..:..:..$','\\1',$mysqldate);
			$y = ereg_replace('^(....)-..-..[[:space:]]..:..:..$','\\1',$mysqldate);
			return mktime($h,$m,$s,$mon,$d,$y);
		}else if(eregi('^..:..:..$',$mysqldate)){
			//if only time
			$h = ereg_replace('^(..):..:..$','\\1',$mysqldate);
			$m = ereg_replace('^..:(..):..$','\\1',$mysqldate);
			$s = ereg_replace('^..:..:(..)$','\\1',$mysqldate);
			return mktime($h,$m,$s);
		}else if(eregi('^....-..-..$',$mysqldate)){
			//if only date
			$mon = ereg_replace('^....-(..)-..$','\\1',$mysqldate);
			$d = ereg_replace('^....-..-(..)$','\\1',$mysqldate);
			$y = ereg_replace('^(....)-..-..$','\\1',$mysqldate);
			return mktime(0,0,0,$mon,$d,$y);
		}
	}
	public static function isBeforeCron($timestamp){
		$cron_stamp = mktime(CRON_RUN_HOUR,0,0);
		//if todays cron has passed the time stamp
		if (mktime() > $cron_stamp && $cron_stamp > $timestamp){
			return true;
		}
		//if yesterdays ron passed timestamp
		if (($cron_stamp-60*60*24) > $timestamp){
			return true;
		}
		return false;
	}
}
?>