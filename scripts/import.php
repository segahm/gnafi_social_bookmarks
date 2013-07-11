<?php
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
class ImportPage{
	private $userid = null;
	private $error = null;
	private $message = null;
	function __construct(){
		if (User::checkAuth(false,true))
		$this->userid = $GLOBALS[GLOBAL_USER]->getUserID();
	}
	public function init(){
		if(($notLoggedin = is_null($this->userid)) || !isset($_GET['submit'])
				|| (//(empty($_POST['deluser']) || empty($_POST['delpass'])) &&
						 (empty($_FILES['file']) || empty($_FILES['file']['tmp_name'])))){
			if ($notLoggedin)
				$this->error = 'please <a href="/my?action=signin&path=%2import">sign in</a> first';
			$this->drawDefault();
			return;
		}
		//check that this hasn't been done recently
		$cron_t = new table_cron_temp();
		$query = 'SELECT 1 FROM '.TABLE_CRON_TEMP.' WHERE '.$cron_t->field_type.' = '
			.CRON_TYPE_IMPORT.' AND '.$cron_t->field_val.' = '
			.toSQL($this->userid).' AND DATE_ADD('.$cron_t->field_time
			.', INTERVAL 30 MINUTE) > NOW() LIMIT 1';;
		$result = db::query($query);
		if ($row = db::fetch_row($result)){
			$this->error = 'sorry, we do not allow the use of this feature more than once every hour';
		}else{
			$fields = array();
			$fields[$cron_t->field_type] = CRON_TYPE_IMPORT;
			$fields[$cron_t->field_val] = $this->userid;
			$query = 'REPLACE DELAYED INTO '.TABLE_CRON_TEMP
				.' ('.sqlFields(array_keys($fields)).') VALUES ('.toSQL($fields).')';
			db::query($query);
			$import = new Import($this->userid);
			//if (!empty($_FILES['file']) && !empty($_FILES['file']['tmp_name'])){
				$cont = file_get_contents($_FILES['file']['tmp_name']);
				if (($error = $import->fromHtml($cont)) !== true)
					$this->error = 'error occured while parsing the file';
			/*}else{
				$ch = curl_init();
	
				// set URL and other appropriate options
				curl_setopt($ch, CURLOPT_URL, "http://del.icio.us/login");
				curl_setopt($ch, CURLOPT_HEADER, 0);
				curl_setopt($ch,CURLOPT_POST,1);
				$fields = 'user_name='.urlencode(trim($_POST['deluser']))
					.'&password='.urlencode(trim($_POST['delpass']));
				curl_setopt($ch,CURLOPT_POSTFIELDS,$fields);
				// grab URL and pass it to the browser
				ob_start();
				curl_exec($ch);
				$out = ob_get_contents();
				ob_end_clean();
				log_writeLog($out);
				$matches = array();
				if (!preg_match('~logged in as <strong>([^<]+)</strong>~',$out,$matches)){
					$this->error = 'failed to sign in into del.icio.us';
				}else{
					$username = $matches[1];
					curl_setopt($ch, CURLOPT_URL, "/settings/$username/export");
					curl_setopt($ch,CURLOPT_POST,1);
					$fields = 'showtags=1&showextended=1';
					curl_setopt($ch,CURLOPT_POSTFIELDS,$fields);
					ob_start();
					curl_exec($ch);
					$out = ob_get_contents();
					ob_end_clean();
					log_writeLog($out);
					if (stripos($out,'<TITLE>Bookmarks</TITLE>') === false)
						$this->error = 'error occured while exporting from del.icio.us';
					else{
						if (($error = $import->fromHtml($out)) !== true)
							$this->error = 'error occured while parsing the file from del.icio.us';
					}
				}
				// close curl resource, and free up system resources
				curl_close($ch);
			}*/
			$this->message = 'finished importing links - <a href="/mine">take a look</a>';
		}
		$this->drawDefault();
	}
	private function drawDefault(){
		Template::printSearchHeader();
		$errStr = is_null($this->error)?'':'<p class="error">: '.$this->error.'</p>';
		if (empty($errStr) && !empty($this->message)){
			$errStr = '<p class="message">: '.$this->message.'</p>';
		}
		echo <<<EOD
		<p style="padding-left:10px"><a class="dblArrows" href="/doc/hacks">&gt;&gt;</a>&nbsp;&nbsp;
		<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Import</span></p>
		<div style="padding:10px;">
		$errStr
		<form action="/import?submit=1" enctype="multipart/form-data" method="post">
		<table><tr><td>
		<span class="field-title">from Html File:</span>
		</td><td>
		<input type="hidden" name="MAX_FILE_SIZE" value="1000000" />
		<input type="file" name="file"> - netscape/mozilla compatible
		</td></tr>
		</table>
		<input style="margin-left:100px;" type="submit" class="btn" value="import"/>
		</form>
		</div>
		<div>
		<table><tr valign="top"><td><p> To import a file go to (Firefox Only) Boomarks &gt;&gt; Manage Bookmarks.. &gt;&gt; (new window) File &gt;&gt; Export
		</p><img style="border: 1px solid #999999;margin:10px;" src="/images/browser_import.jpg"/>
		</td><td style="border-left:1px solid #999999;padding-left:10px;">Note: this will only add your bookmarks to the site (and make them taggable/searchable). 
		If you are looking for a way to synchronize these bookmarks with your browser, please <a href="/download.xpi">install</a> 
		our extension.</td></tr></table>
		</div>	
EOD;
		Template::printFooter();
	}
}

if (!defined('NINCLUDE')){
	db::connect();
	$page = new ImportPage();
	$page->init();
	db::close();
}
?>