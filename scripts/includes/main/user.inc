<?php
/*User object provides methods for 
 *1) authentication
 *2) storing of user info
 */
define('COOKIE_SESSION','session');
define('COOKIE_USERNAME','username');
class User extends Repository{
	private static $authChecked = null;
	public $storeMore;	//additional storage
	const STORE_MORE = 'store_more';
	
	const SIGNIN_USER = 'signin_user';
	const SIGNIN_PASS = 'signin_pass';
	private $users_t = null;
	function User($parameters = null){
		$users_t = new table_users();
		if (is_null($parameters)){
			$this->storeArray = array();
		}else{
			$this->storeArray = $parameters;
		}
	}
	/**
	 * the function assumes that the user is already authenticated
	 * @param string $oldPass old password to verify on correctness
	 * @param string $newPass new password
	 */
	public static function updatePass($oldPass,$newPass){
		$users_t = new table_users();
		$epass = substr(md5(strtolower(trim($oldPass))),0,$users_t->field_epass_length);
		$userid = $GLOBALS[GLOBAL_USER]->get($users_t->field_userid);
		$query = 'SELECT 1 FROM '.TABLE_USERS.' WHERE '.$users_t->field_userid." = '".$userid."' AND "
			.$users_t->field_epass." = '".$epass."' LIMIT 1";
		$result = db::query($query);
		if (!db::fetch_row($result))return false;
		//if still here update
		$epass = substr(md5(strtolower(trim($newPass))),0,$users_t->field_epass_length);
		$query = 'UPDATE '.TABLE_USERS.' SET '.$users_t->field_epass." = '".$epass."' WHERE "
			.$users_t->field_userid." = '".$userid."' LIMIT 1";
		db::query($query);
		return true;
	}
	public function getUsername(){
		return $this->get(table_users::FIELD_USERNAME);
	}
	public function getUserId(){
		return $this->get(table_users::FIELD_USERID);
	}
	public static function doWidgetCheck($username,$pass){
		$users_t = new table_users();
		$encryptedPass = substr(md5(strtolower(trim($pass)))
			,0,$users_t->field_epass_length);
		$query = 'SELECT '.$users_t->field_userid.' FROM '.TABLE_USERS.' WHERE '.$users_t->field_username.' = '.toSQL($username)
			.' AND '.$users_t->field_epass." = '$encryptedPass' LIMIT 1";
		$result = db::query($query);
		if (!($row = db::fetch_row($result)))
			return false;
		$GLOBALS[GLOBAL_USER] = new User();
		$user =& $GLOBALS[GLOBAL_USER];
		$user->set(table_users::FIELD_USERNAME,$username);
		$user->set(table_users::FIELD_USERID,$row[0]);
		return true;
	}
	/** 
	 * @param bool	$strict  - makes an actual sql check, this should be done if we need to access resouces relavant to the user;
	 * @param bool $noreturn - wherever to return the control to the questioner or exit when failed
	 */
	public static function checkAuth($noreturn = false,$strict = true){
		if (isset($_GET['au']) && !empty($_GET['who']) && !empty($_GET['pass'])){
			return self::doWidgetCheck($_GET['who'],$_GET['pass']);
		}
		//first do basic check of cookie
		if (!isset($_COOKIE[COOKIE_COMMON][COOKIE_SESSION])
			|| !isset($_COOKIE[COOKIE_COMMON][COOKIE_USERNAME]) 
				|| !preg_match('/^[a-z0-9]{'.table_session::SESSIONID_LENGTH.'}$/i',
						$_COOKIE[COOKIE_COMMON][COOKIE_SESSION])){
			if ($noreturn){
				header("Location: http://".$_SERVER['HTTP_HOST']
					."/my?action=signin&path=".urlencode($_SERVER['REQUEST_URI']));
				exit;
			}else{
				return false;
			}
		}elseif(!$strict){
			if (!isset($GLOBALS[GLOBAL_USER])){
				$GLOBALS[GLOBAL_USER] = new User();
				$user =& $GLOBALS[GLOBAL_USER];
				$user->set(table_users::FIELD_USERNAME,$_COOKIE[COOKIE_COMMON][COOKIE_USERNAME]);
			}
			return true;
		}
		//check only if we haven't checked before
		if (!is_null(self::$authChecked)){
			return self::$authChecked;	//this might be false as well as true
		}
		//now do a database check
		$sessionTable = new table_session();
		$users_t = new table_users();
		$sessionID = $_COOKIE[COOKIE_COMMON][COOKIE_SESSION];
		//get userid and college short associated with this id
		$query = 'SELECT '
					.TABLE_SESSION.'.'.$sessionTable->field_userid.','
					.TABLE_SESSION.'.'.$sessionTable->field_username.','
					.TABLE_SESSION.'.'.$sessionTable->field_time
					.' FROM '.TABLE_SESSION.','.TABLE_USERS.' WHERE '
					.TABLE_USERS.'.'.$users_t->field_userid
					.' = '.TABLE_SESSION.'.'.$sessionTable->field_userid
					.' AND '.$sessionTable->field_sessionid
					." = '".$sessionID."' LIMIT 1";
		$result = db::query($query); 
		if (!($row = db::fetch_assoc($result))){
			//if strict check proved that session is invalid, then delete cookie
			if (isset($_COOKIE[COOKIE_COMMON][COOKIE_SESSION]))
				setcookie(COOKIE_COMMON.'['.COOKIE_SESSION.']','',time()-3600);
			if (isset($_COOKIE[COOKIE_COMMON][COOKIE_USERNAME]))
				setcookie(COOKIE_COMMON.'['.COOKIE_USERNAME.']','',time()-3600);
			if ($noreturn){
				header("Location: http://".$_SERVER['HTTP_HOST']."/");
				exit;
			}else{
				self::$authChecked = false;
				return false;
			}
		}
		$GLOBALS[GLOBAL_USER] = new User();
		$user =& $GLOBALS[GLOBAL_USER];
		$user->set($users_t->field_userid,$row[$sessionTable->field_userid]);
		$username = $row[$users_t->field_username];
		$user->set($users_t->field_username,$username);
		//get time the session was last updated
		$time = $row[$sessionTable->field_time];
		$date_class = new Mydate(mktime());
		$difference = $date_class->difference($time);
		/*update the time if the last request was made longer than an hour ago
		 */
		if (($difference/60)>55){
			$query = "UPDATE ".TABLE_SESSION." SET "
				.$sessionTable->field_time." = NOW() WHERE "
				.$sessionTable->field_sessionid." = '".$sessionID."' LIMIT 1";
			$result = db::query($query); 
		}
		self::$authChecked = true;
		return true;
	}
	public static function signIn($master_user = null,$master_pass = null){
		$parser = new parser($_POST);
		$user_email = $master_user?$master_user:$parser->getString(self::SIGNIN_USER,$master_user);
		$pass = $master_pass?$master_pass:$parser->getPassword(self::SIGNIN_PASS,$master_pass);
		if(is_null($master_pass)){
			//do a general check of fields
			if (empty($user_email) || empty($pass)){
				return false;
			}
		}
		$users_t = new table_users();
		$fields = array();
		$fields[] = TABLE_USERS.'.'.$users_t->field_userid;
		$fields[] = TABLE_USERS.'.'.$users_t->field_username;
		/*check for athentication and while at it grab a few values to be
		 *stored for quick access*/
		$encryptedPass = substr(md5(strtolower(trim($pass)))
			,0,$users_t->field_epass_length);
		$query = 'SELECT '.implode(',',$fields).' FROM '
			.TABLE_USERS.' WHERE '.((strpos($user_email,'@') !== false)?TABLE_USERS.'.'.$users_t->field_email 
			.' = '.toSQL($user_email):TABLE_USERS.'.'.$users_t->field_username.' = '.toSQL($user_email)).' AND '
			.TABLE_USERS.'.'.$users_t->field_epass." = '".$encryptedPass."' LIMIT 1";
		$result = db::query($query); 
		if ($row = db::fetch_array($result)){
			/*if found user, then add to session, set cookie with session id
			 *clean buffer, and redirect to to home, and quit
			**/
			$sessionTable = new table_session();
			$genid = genIDString($sessionTable->field_sessionid_length);
			$userid = $row[0];
			$name = $row[1];
			setcookie(COOKIE_COMMON.'['.COOKIE_SESSION.']',$genid,time()+60*60*24*7);	//expire in one week
			setcookie(COOKIE_COMMON.'['.COOKIE_USERNAME.']',$name,time()+60*60*24*7);	//expire in one week
			$GLOBALS[GLOBAL_USER] = new User($row);
			$user =& $GLOBALS[GLOBAL_USER];
			$user->set($users_t->field_userid,$userid);
			$user->set($users_t->field_username,$name);
			$query = 'REPLACE '.TABLE_SESSION.' (' 
				.$sessionTable->field_sessionid.',' 
				.$sessionTable->field_userid.',' 
				.$sessionTable->field_time.','
				.$sessionTable->field_username
					. ") values ('" 
				.$genid."', '" 
				.$userid."', NOW(),'"
				.$name."')";
			$result = db::query($query); 
			//finally update last visit data
			$query = "UPDATE " . TABLE_USERS . " set " 
				.$users_t->field_lastactive . " = CURDATE() WHERE "
				.$users_t->field_userid . " = '" . $userid . "' LIMIT 1";
			$result = db::query($query);
			return true;
		}
		return false;
	}
	public static function signOut(){
		$sessionTable = new table_session();
		if (!isset($_COOKIE[COOKIE_COMMON][COOKIE_SESSION]) 
			|| strlen($_COOKIE[COOKIE_COMMON][COOKIE_SESSION]) != $sessionTable->field_sessionid_length
			|| !preg_match('/^[a-z0-9]+$/i',$_COOKIE[COOKIE_COMMON][COOKIE_SESSION])){
			return false;
		}
		$query = "DELETE FROM " . TABLE_SESSION . " WHERE " 
			. $sessionTable->field_sessionid . " = '" 
			.$_COOKIE[COOKIE_COMMON][COOKIE_SESSION] . "' LIMIT 1";
			//send cookie to expire
		setcookie(COOKIE_COMMON.'['.COOKIE_USERNAME.']','',time()-3600);
		setcookie(COOKIE_COMMON.'['.COOKIE_SESSION.']','',time()-3600);
		$success = db::query($query); 
		return ($success);
	}
}
?>