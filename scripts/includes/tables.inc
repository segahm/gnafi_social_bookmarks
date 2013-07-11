<?php
define("TABLE_CRON_TEMP","cron_temp");
define("TABLE_DELETEDURLS","deletedurls");
define("TABLE_FIREWALL","firewall");
define("TABLE_KEYWORD_RELATIONS","keyword_relations");
define("TABLE_KEYWORDS","keywords");
define("TABLE_MAIL","mail");
define("TABLE_SEARCHES","searches");
define("TABLE_SESSION","session");
define("TABLE_SYNC","sync");
define("TABLE_SYNC_TODELETE","sync_todelete");
define("TABLE_SYNCLATEST","synclatest");
define("TABLE_SYNCTEMP","synctemp");
define("TABLE_TEMPUSERS","tempusers");
define("TABLE_URLS","urls");
define("TABLE_USERS","users");
class sql_table{
	function __construct($prepandTable = false,$table_name = false){
		if ($table_name !== false)
			$this->name = $table_name;
		if ($prepandTable){
			foreach($this as $key => $value) {
           		$this->$key = $this->name.'.'.$value;
       		}
		}
	}
	public function allFields(){
		$ar = array();
		foreach($this as $k => $value) {
			if (!preg_match('-(?:_length$)|(?:^name$)-',$k)){
           		$ar[] = $value;
			}
       	}
       	return $ar;
	}
}
class table_cron_temp extends sql_table{
	public $field_type = "type";
	const FIELD_TYPE = "type";
	public $field_time = "time";
	const FIELD_TIME = "time";
	public $field_val = "val";
	const FIELD_VAL = "val";
	public $field_val_length = 10;
	const VAL_LENGTH = 10;
	public $name = "cron_temp";
}

class table_deletedurls extends sql_table{
	public $field_url = "url";
	const FIELD_URL = "url";
	public $field_url_length = 255;
	const URL_LENGTH = 255;
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $name = "deletedurls";
}

class table_firewall extends sql_table{
	public $field_ip = "ip";
	const FIELD_IP = "ip";
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_cvsid = "cvsid";
	const FIELD_CVSID = "cvsid";
	public $field_cvsid_length = 5;
	const CVSID_LENGTH = 5;
	public $field_time = "time";
	const FIELD_TIME = "time";
	public $name = "firewall";
}

class table_keyword_relations extends sql_table{
	public $field_id = "id";
	const FIELD_ID = "id";
	public $field_keyword1 = "keyword1";
	const FIELD_KEYWORD1 = "keyword1";
	public $field_keyword1_length = 255;
	const KEYWORD1_LENGTH = 255;
	public $field_keyword2 = "keyword2";
	const FIELD_KEYWORD2 = "keyword2";
	public $field_keyword2_length = 255;
	const KEYWORD2_LENGTH = 255;
	public $name = "keyword_relations";
}

class table_keywords extends sql_table{
	public $field_urlid = "urlid";
	const FIELD_URLID = "urlid";
	public $field_keyword = "keyword";
	const FIELD_KEYWORD = "keyword";
	public $field_keyword_length = 35;
	const KEYWORD_LENGTH = 35;
	public $field_id = "id";
	const FIELD_ID = "id";
	public $name = "keywords";
}

class table_mail extends sql_table{
	public $field_id = "id";
	const FIELD_ID = "id";
	public $field_to = "to";
	const FIELD_TO = "to";
	public $field_to_length = 100;
	const TO_LENGTH = 100;
	public $field_message = "message";
	const FIELD_MESSAGE = "message";
	public $field_ip = "ip";
	const FIELD_IP = "ip";
	public $field_date = "date";
	const FIELD_DATE = "date";
	public $name = "mail";
}

class table_searches extends sql_table{
	public $field_visitorid = "visitorid";
	const FIELD_VISITORID = "visitorid";
	public $field_query = "query";
	const FIELD_QUERY = "query";
	public $field_query_length = 255;
	const QUERY_LENGTH = 255;
	public $field_stamp = "stamp";
	const FIELD_STAMP = "stamp";
	public $name = "searches";
}

class table_session extends sql_table{
	public $field_sessionid = "sessionid";
	const FIELD_SESSIONID = "sessionid";
	public $field_sessionid_length = 20;
	const SESSIONID_LENGTH = 20;
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_username = "username";
	const FIELD_USERNAME = "username";
	public $field_username_length = 25;
	const USERNAME_LENGTH = 25;
	public $field_time = "time";
	const FIELD_TIME = "time";
	public $name = "session";
}

class table_sync extends sql_table{
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_cvsid = "cvsid";
	const FIELD_CVSID = "cvsid";
	public $field_cvsid_length = 5;
	const CVSID_LENGTH = 5;
	public $field_hashkey = "hashkey";
	const FIELD_HASHKEY = "hashkey";
	public $field_hashkey_length = 20;
	const HASHKEY_LENGTH = 20;
	public $name = "sync";
}

class table_sync_todelete extends sql_table{
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_cvsid = "cvsid";
	const FIELD_CVSID = "cvsid";
	public $field_cvsid_length = 5;
	const CVSID_LENGTH = 5;
	public $field_hashkey = "hashkey";
	const FIELD_HASHKEY = "hashkey";
	public $field_hashkey_length = 20;
	const HASHKEY_LENGTH = 20;
	public $field_date = "date";
	const FIELD_DATE = "date";
	public $name = "sync_todelete";
}

class table_synclatest extends sql_table{
	public $field_id = "id";
	const FIELD_ID = "id";
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_cvsid = "cvsid";
	const FIELD_CVSID = "cvsid";
	public $field_cvsid_length = 5;
	const CVSID_LENGTH = 5;
	public $field_hashkey = "hashkey";
	const FIELD_HASHKEY = "hashkey";
	public $field_hashkey_length = 20;
	const HASHKEY_LENGTH = 20;
	public $field_info = "info";
	const FIELD_INFO = "info";
	public $field_flag = "flag";
	const FIELD_FLAG = "flag";
	public $field_flag_length = 1;
	const FLAG_LENGTH = 1;
	public $field_cronflag = "cronflag";
	const FIELD_CRONFLAG = "cronflag";
	public $field_cronflag_length = 1;
	const CRONFLAG_LENGTH = 1;
	public $field_actpath = "actpath";
	const FIELD_ACTPATH = "actpath";
	public $name = "synclatest";
}

class table_synctemp extends sql_table{
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_hashkey = "hashkey";
	const FIELD_HASHKEY = "hashkey";
	public $field_hashkey_length = 20;
	const HASHKEY_LENGTH = 20;
	public $field_aboutid = "aboutid";
	const FIELD_ABOUTID = "aboutid";
	public $field_aboutid_length = 20;
	const ABOUTID_LENGTH = 20;
	public $name = "synctemp";
}

class table_tempusers extends sql_table{
	public $field_key = "key";
	const FIELD_KEY = "key";
	public $field_key_length = 10;
	const KEY_LENGTH = 10;
	public $field_email = "email";
	const FIELD_EMAIL = "email";
	public $field_email_length = 100;
	const EMAIL_LENGTH = 100;
	public $field_username = "username";
	const FIELD_USERNAME = "username";
	public $field_username_length = 20;
	const USERNAME_LENGTH = 20;
	public $field_epass = "epass";
	const FIELD_EPASS = "epass";
	public $field_epass_length = 10;
	const EPASS_LENGTH = 10;
	public $field_date = "date";
	const FIELD_DATE = "date";
	public $name = "tempusers";
}

class table_urls extends sql_table{
	public $field_id = "id";
	const FIELD_ID = "id";
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_private = "private";
	const FIELD_PRIVATE = "private";
	public $field_private_length = 1;
	const PRIVATE_LENGTH = 1;
	public $field_url = "url";
	const FIELD_URL = "url";
	public $field_url_length = 255;
	const URL_LENGTH = 255;
	public $field_title = "title";
	const FIELD_TITLE = "title";
	public $field_title_length = 100;
	const TITLE_LENGTH = 100;
	public $field_description = "description";
	const FIELD_DESCRIPTION = "description";
	public $field_time = "time";
	const FIELD_TIME = "time";
	public $field_isfeed = "isfeed";
	const FIELD_ISFEED = "isfeed";
	public $field_isfeed_length = 1;
	const ISFEED_LENGTH = 1;
	public $field_insync = "insync";
	const FIELD_INSYNC = "insync";
	public $field_insync_length = 1;
	const INSYNC_LENGTH = 1;
	public $field_keywords = "keywords";
	const FIELD_KEYWORDS = "keywords";
	public $field_keywords_length = 255;
	const KEYWORDS_LENGTH = 255;
	public $name = "urls";
}

class table_users extends sql_table{
	public $field_userid = "userid";
	const FIELD_USERID = "userid";
	public $field_userid_length = 5;
	const USERID_LENGTH = 5;
	public $field_epass = "epass";
	const FIELD_EPASS = "epass";
	public $field_epass_length = 10;
	const EPASS_LENGTH = 10;
	public $field_email = "email";
	const FIELD_EMAIL = "email";
	public $field_email_length = 100;
	const EMAIL_LENGTH = 100;
	public $field_joined = "joined";
	const FIELD_JOINED = "joined";
	public $field_lastactive = "lastactive";
	const FIELD_LASTACTIVE = "lastactive";
	public $field_username = "username";
	const FIELD_USERNAME = "username";
	public $field_username_length = 25;
	const USERNAME_LENGTH = 25;
	public $field_urlsprivate = "urlsprivate";
	const FIELD_URLSPRIVATE = "urlsprivate";
	public $field_urlsprivate_length = 1;
	const URLSPRIVATE_LENGTH = 1;
	public $name = "users";
}
?>