<?php
require_once($_SERVER['DOCUMENT_ROOT']."/scripts/includes/interface.inc");
class Admin extends Controller{
	const ADMIN_ERROR = 'admin_error';
	const ADMIN_FORGOT_EMAIL = 'forgot_email';
	const ADMIN_MESSAGE = 'ADMIN_MESSAGE';
	const ADMIN_SECURITY_COUNTER = 'seccnt';
	const ADMIN_REGISTER_EMAIL = 'reg_email';
	const ADMIN_REGISTER_PASS = 'reg_pass';
	const ADMIN_REGISTER_PASSVERIFY = 'reg_pverify';
	const ADMIN_REGISTER_NAME = 'reg_user';
	const ADMIN_REGISTER_IAGREE = 'reg_agree';
	const MAIL_REGISTER_FILE = 'registration.txt';
	const MAIL_CONFIRM_FILE = 'confirmemail.txt';
	const MAX_UPLOAD_SIZE = 30000;
	const DIR_IMAGE_UPLOAD = '/uploaded/imgs/';
	function Admin(){
	}
	public function init(){
		if (isset($_GET['action'])){
			$this->dealWithAccountAuth();
			return;
		}elseif (isset($_GET['go'])){
			User::checkAuth(true,true);
			switch($_GET['go']){
				case 'settings':
					$this->handleSettings();
					break;
			}
			return;
		}
	}
	private function handleSettings(){
		$users_t = new table_users();
		if (isset($_GET['submit'])){
			$parser = new parser($_POST);
			if ($_GET['submit'] == 'email'){
				$email = $parser->getString('email','');
				if (!empty($email) && !($email = $parser->getEmail('email',false))){
					$this->set(self::ADMIN_ERROR,'please enter a valid email address');
				}else{
					if (!empty($email)){
						//check that no one else owns this email
						$query = 'SELECT '.$users_t->field_userid.' FROM '.TABLE_USERS
							.' WHERE '.$users_t->field_email.' = '.toSQL($email).' LIMIT 1';
						$result = db::query($query);
						if ($row = db::fetch_row($result)){
							if ($row[0] != $GLOBALS[GLOBAL_USER]->getUserId()){
								$this->set(self::ADMIN_ERROR,'somebody else already registered that email (if you believe that to be a mistake, contact us at <a href="mailto:info@gnafi.com">info@gnafi.com</a>)');
							}
						}else{
							$query = 'UPDATE '.TABLE_USERS.' SET '.$users_t->field_email.' = '.toSQL($email)
							.' WHERE '.$users_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()
							."' LIMIT 1";
							db::query($query);
							$this->set(self::ADMIN_MESSAGE,'changes were made');
						}
					}
				}
			}elseif($_GET['submit'] == 'pass'){
				$flag = false;
				if (!($oldpass = $parser->getPassword('oldpass',false))){
					$this->set(self::ADMIN_ERROR,'please enter your old password');
				}elseif(!($newpass = $parser->getPassword('pass',false))){
					$this->set(self::ADMIN_ERROR,'please enter a password using more than 4 alphanumeric characters');
				}elseif($newpass != $parser->getPassword('verifypass','*')){
					$this->set(self::ADMIN_ERROR,'passwords do not match');
					}else{
					$flag = true;
				}
				if ($flag && !User::updatePass($oldpass,$newpass)){
					$this->set(self::ADMIN_ERROR,'invalid old password entered');
				}
				$this->set(self::ADMIN_MESSAGE,'password was updated successfully');
			}elseif($_GET['submit'] == 'name'){
				$name = $parser->getString('name','');
				if (!preg_match('/[a-z_]+/i',$name)){
					$this->set(self::ADMIN_ERROR,'please enter a valid nickname');
				}else{
					$query = 'UPDATE '.TABLE_USERS.' SET '.$users_t->field_username.' = '.toSQL($name).' WHERE '
						.$users_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' LIMIT 1";
					db::query($query);
				}
				$this->set(self::ADMIN_MESSAGE,'changes were made');
			}elseif($_GET['submit'] == 'urls'){
				if (isset($_POST['private']) && $_POST['private'] == 1){
					$query = 'UPDATE '.TABLE_USERS.' SET '.$users_t->field_urlsprivate.' = 1 WHERE '
						.$users_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' LIMIT 1";
					$urls_t = new table_urls();
					db::query($query);
					$query = 'UPDATE '.TABLE_URLS.' SET '.$urls_t->field_private.' = 1 WHERE '
						.$urls_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' AND "
						.$urls_t->field_insync.' IS NOT NULL';
					db::query($query);
				}else{
					$query = 'UPDATE '.TABLE_USERS.' SET '.$users_t->field_urlsprivate.' = NULL WHERE '
						.$users_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' LIMIT 1";
					db::query($query);
					$urls_t = new table_urls();
					$query = 'UPDATE '.TABLE_URLS.' SET '.$urls_t->field_private.' = NULL WHERE '
						.$urls_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' AND "
						.$urls_t->field_insync.' IS NOT NULL';
					db::query($query);
				}
				$this->set(self::ADMIN_MESSAGE,'changes were made');
			}elseif($_GET['submit'] == 'delsync'){
				$sync_t = new table_synclatest();
				$query = 'DELETE QUICK FROM '.TABLE_SYNCLATEST.' WHERE '.$sync_t->field_userid
					." = '".$GLOBALS[GLOBAL_USER]->getUserId()."'";
				db::query($query);
				$sync_t = new table_sync();
				$query = 'DELETE QUICK FROM '.TABLE_SYNC.' WHERE '.$sync_t->field_userid
					." = '".$GLOBALS[GLOBAL_USER]->getUserId()."'";
				db::query($query);
				$sync_t = new table_sync_todelete();
				$query = 'DELETE QUICK FROM '.TABLE_SYNC_TODELETE.' WHERE '.$sync_t->field_userid
					." = '".$GLOBALS[GLOBAL_USER]->getUserId()."'";
				db::query($query);
				$this->set(self::ADMIN_MESSAGE,'all cleared');
			}elseif($_GET['submit'] == 'hist'){
				$urls_t = new table_urls();
				$query = 'DELETE QUICK FROM '.TABLE_URLS.' WHERE '.$urls_t->field_userid
					." = '".$GLOBALS[GLOBAL_USER]->getUserId()."'";
				db::query($query);
			}
		}else{
			$fields = array($users_t->field_email,
				$users_t->field_username,$users_t->field_urlsprivate);
			$query = 'SELECT '.sqlFields($fields).' FROM '.TABLE_USERS.' WHERE '.$users_t->field_userid
			." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' LIMIT 1";
			$result = db::query($query);
			$row = db::fetch_row($result);
			assert($row);
			$_POST['email'] = $row[0];
			$_POST['name'] = $row[1];
			if (!is_null($row[2]))
				$_POST['private'] = 1;
		}
		Template::printSearchHeader();
		$GLOBALS['glbFields'] = $_POST;
		ob_start('funcs_fillForm');
		require_once('auth/settings.inc');
		ob_end_flush();
		Template::printFooter();
	}
	private function redirect($url){
		header("Location: http://".$_SERVER['HTTP_HOST'].$url.(isset($_GET['small'])?'&small=1':''));
		exit;
	}
	private function dealWithAccountAuth(){
		$GLOBALS['glbFields'] = $_POST;
		if ($_GET['action'] == 'signin'){
			$ret = false;
			//if we need to verify data
			if (isset($_GET['submit'])){
				if (($ret = User::signIn()) !== true){
					$this->set(self::ADMIN_ERROR,'incorrect login/pass');
				}
			}
			if ($ret !== true){
				$admCntrl =& $this;
				Template::printSearchHeader();
				ob_start('funcs_fillForm');
				require_once('auth/signin.inc');
				ob_end_flush();
				Template::printFooter();
			}else{
				if (!empty($_GET['path']) && !preg_match('/action=signin/',$_GET['path'])){
					$this->redirect($_GET['path']);
					exit;
				}else{
					$this->redirect('/?');
					exit;
				}
			}
			return;
		}elseif ($_GET['action'] == 'forgot'){
			$ret = false;
			//if form is submitted
			if (isset($_GET['submit'])){
				if (($ret = $this->forgotEmail()) !== true){
					$this->set(self::ADMIN_ERROR,'incorrect email address');
				}
			}
			$admCntrl =& $this;
				Template::printSearchHeader();
				ob_start('funcs_fillForm');
				require_once('auth/forgot.inc');
				ob_end_flush();
				Template::printFooter();
			return;
		}elseif ($_GET['action'] == 'register'){
			$ret = false;
			//if form is submitted
			if (isset($_GET['submit'])){
				if (($ret =$this->register()) !== true){
					$this->set(self::ADMIN_ERROR,$ret);
				}else{
					$this->redirect("/my?action=register&success=1");
					exit;
				}
			}elseif(isset($_GET['success'])){
				User::checkAuth();
				if (!isset($GLOBALS[GLOBAL_USER])){
					echo ': in order to continue, you will need to enable your cookies :(';
					Template::beginClean();
				}
			}
			$admCntrl =& $this;
			Template::printSearchHeader();
			ob_start('funcs_fillForm');
			require_once('auth/register.inc');
			ob_end_flush();
			Template::printFooter();
			return;
		}elseif ($_GET['action'] == 'signout'){
			User::signOut();
			if (isset($_GET['path'])){
				$path = $_GET['path'];
				$this->redirect($path);
			}else{
				Template::printSearchHeader();
				Template::printSimpleBody('Logged out successfully!','<a href="http://gnafi.com/"'.(isset($_GET['small'])?' target="_blank"':'').'>back to home</a>');
				Template::printFooter();
			}
			exit;
		}
	}
	private function register(){
		$parser = new parser($_POST);
		if ($parser->getInt(self::ADMIN_REGISTER_IAGREE,0) == 0)
			return 'you must agree to our terms of service in order to proceed, :-)';
		$email = $parser->getEmail(self::ADMIN_REGISTER_EMAIL,null);
		if (is_null($email)){
			return 'please enter a valid email address';
		}
		$username = $parser->getString(self::ADMIN_REGISTER_NAME,'*');
		if (preg_match('/[^a-z_]/i',$username) || strlen($username) < 4){
			return 'please enter the username containing at least 4 characters case-insensitive';
		}
		$pass = $parser->getPassword(self::ADMIN_REGISTER_PASS,false);
		$vpass = $parser->getPassword(self::ADMIN_REGISTER_PASSVERIFY,false);
		if ($pass === false || $vpass === false){
			return 'bad password (needs to be longer than 4 characters)';
		}
		if ($vpass !== $pass){
			return 'passwords do not match';
		}
		$users_t = new table_users();
		//check wherever user already exists
		$query = 'SELECT '.$users_t->field_username.' FROM '.TABLE_USERS.' WHERE '
			.$users_t->field_email.' = '.toSQL($email).' OR '
			.$users_t->field_username.' = '.toSQL($username).' LIMIT 1';
		$result = db::query($query);
		if ($row = db::fetch_row($result)){
			if ($row[0] == $username)
				$mes = 'this nickname is already taken';
			else
				$mes = 'email is used by someone else (please <a href="/doc/faq#contact">contact us</a> if you believe there is a mistake)';
			return $mes;
		}
		$fields = array();
		$fields[$users_t->field_username] = $username;
		$fields[$users_t->field_email] = $email;
		$fields[$users_t->field_userid] = genIDString($users_t->field_userid_length);
		$pass = strtolower(trim($pass));
		$encryptedPass = substr(md5($pass),0,$users_t->field_epass_length);
		$fields[$users_t->field_epass] = $encryptedPass;
		$query = 'INSERT INTO '.TABLE_USERS.' ('.sqlFields(array_keys($fields))
			.','.$users_t->field_joined.') VALUES ('.toSQL($fields).',CURDATE())';
		db::query($query);
		//set variable for necessary display
		User::signIn($username,$pass);
		assert(isset($GLOBALS[GLOBAL_USER]));
		return true;
	}
	private function forgotEmail(){
		$parser = new parser($_POST);
		if ($parser->getInt(self::ADMIN_SECURITY_COUNTER,4) > 3){
			$this->redirect("/");
			exit;
		}
		$_POST[self::ADMIN_SECURITY_COUNTER] += 1;
		$email = $parser->getEmail(self::ADMIN_FORGOT_EMAIL,null);
		if (is_null($email))return false;
		$users_t = new table_users();
		$query = 'SELECT '.$users_t->field_userid.','.$users_t->field_username
			.' FROM '.TABLE_USERS.' WHERE '
			.$users_t->field_email.' = '.toSQL($email);
		$result = db::query($query);
		if (!($row = db::fetch_row($result)))
			return false;
		$userid = $row[0];
		$username = $row[1];
		$pass = strtolower(genIDString(8));
		//encrypt pass
		$epass = substr(md5($pass),0,10);
		$query = 'UPDATE '.TABLE_USERS.' SET '.$users_t->field_epass.' = '
			.toSQL($epass).' WHERE '.$users_t->field_userid." = '".$userid."' LIMIT 1";
		db::query($query);
		//send mail
		$subject = 'Gnafi - password reset service requested';
		//customize the mail message for this user
		$filename = MESSAGE_PATH.'passwordreset.txt';
		$message =& file_get_contents($filename);
		$message = preg_replace('/<<name>>/i',$username,$message);
		$message = preg_replace('/<<pass>>/i',$pass,$message);
		$success = myMail(false, $email, $subject, $message);
		return true;
	}
}
if (!defined('NINCLUDE')){
	db::connect();
	$glbAdmin = new Admin();
	$glbAdmin->init();
	db::close();
}
?>