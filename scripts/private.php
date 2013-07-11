<?php
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
class Priv{
	const TYPE_HTML = 'text/html';
	const TYPE_XML = 'application/xml';
	const TYPE_JS = 'application/javascript';
	private $type;
	private $out = '';
	private $loggedin = false;
	function __construct(){
		$this->type = self::TYPE_HTML;
		if (!User::checkAuth(!isset($opts['au']),true)){
			header("HTTP/1.1 403 Forbidden");
		}else{
			$this->loggedin = true;
		}
	}
	public function process($opts){
		if (!$this->loggedin)return;
		$urls_t = new table_urls();
		$key_t = new table_keywords();
		if (empty($opts['act'])){
			if (isset($opts['type']) && $opts['type'] === 'sync'){
				$this->out('<p style="padding-left:10px"><a class="dblArrows" href="/mine">&gt;&gt;</a>&nbsp;&nbsp;<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Synchronized Bookmarks</span> <a href="/doc/faq#synchronized" class="question">[?]</a></p>');
				Template::addPageLink('/export/html','export');
				Template::addPageLink(preg_replace('/&?type=[^&]*/','',$_SERVER['REQUEST_URI']),'history');
				//sync show
				ob_start();
				$this->echoSynchronized();
				$this->out(ob_get_contents());
				ob_end_clean();
			}else{
				//url show
				Template::addPageLink('javascript:Edit.addNew();','add new','add a new link');
				Template::addPageLink('/import','import','import from file/del.icio.us');
				Template::addPageLink('/rss'.$_SERVER['REQUEST_URI'],'rss');
				Template::addPageLink($_SERVER['REQUEST_URI'].((strpos($_SERVER['REQUEST_URI'],'?') !== false)?'&':'?').'type=sync','browser bookmarks');
				Template::addSystemScript('userbookmarks');
				define('NINCLUDE',1);
				require_once($_SERVER['DOCUMENT_ROOT'].'/index.php');
				$main = new Main();
				$ar2 = array('u' => $GLOBALS[GLOBAL_USER]->getUserId(),'noout' => 1,'edit' => 1);
				if (isset($opts['q'])){
					$ar2['q'] = $opts['q'];
				}
				if (!isset($opts[Main::SQL_LIMIT_GET]))
					$ar2[Main::SQL_LIMIT_GET] = 20;
				$opts = array_merge($opts,$ar2);
				ob_start();
				$main->processUrl($opts);
				if (isset($opts['out'])){
					$this->type = self::TYPE_XML;
					$this->out(ob_get_contents());
				}else{
					$this->out('<p style="padding-left:10px"><a class="dblArrows" 
					href="/mine">&gt;&gt;</a>&nbsp;&nbsp;<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Link History</span> <a href="/doc/faq#linkhistory" class="question">[?]</a><span id="edit-link-message"></span></p><div id="edit-link-history"></div>');
					$c = ob_get_contents();
					if (preg_match('/^<p>no results were found/',$c)){
						$c = '<p>Your link history is currently empty! You can add links <a href="javascript:Edit.addNew();">manually</a> or by <a href="/download.xpi">downloading</a> 
						our extension - which will automatically add links to this page. Note: it might take a few minutes for them to appear hear after each synchronization.</p>';
					}
					$this->out($c);
				}
				ob_end_clean();
			}
		}else{
			switch($opts['act']){
				case 'deletehist':
					$this->type = self::TYPE_JS;
					if (empty($opts['urlid']))break;
					/*$query = 'SELECT '.$urls_t->field_insync.' FROM '.TABLE_URLS.' WHERE '.$urls_t->field_id
						.' = '.toSQL($opts['urlid']).' AND '
						.$urls_t->field_userid." = '{$GLOBALS[GLOBAL_USER]->getUserId()}' LIMIT 1";
					$result = db::query($query);
					if (!($row = db::fetch_row($result)))break;*/
					//if ($row[0] == null){
						//if we are only deleting from url history
					$query = 'SELECT '.$urls_t->field_url.' FROM '.TABLE_URLS.' WHERE '
							.$urls_t->field_id.' = '.toSQL($opts['urlid']).' AND '
							.$urls_t->field_userid
							." = '{$GLOBALS[GLOBAL_USER]->getUserId()}' LIMIT 1";
					$result = db::query($query);
					if (!($row = db::fetch_row($result)))break;
					$query = 'DELETE FROM '.TABLE_URLS.' WHERE '.$urls_t->field_id.' = '.toSQL($opts['urlid']).' AND '
						.$urls_t->field_userid." = '{$GLOBALS[GLOBAL_USER]->getUserId()}' LIMIT 1";
					db::query($query);
					//save a note for ourselves that this url was deleted
					$del_t = new table_deletedurls();
					$fields = array($del_t->field_url => $row[0],
						$del_t->field_userid => $GLOBALS[GLOBAL_USER]->getUserId());
					$query = 'INSERT DELAYED INTO '
						.TABLE_DELETEDURLS.' ('.sqlFields(array_keys($fields))
						.') VALUES ('.toSQL($fields).')';
					db::query($query);
					/*}else{
						$urls_t = new table_urls(true);
						$last_t = new table_synclatest(true);
						$query = 'DELETE FROM '.TABLE_URLS.','.TABLE_SYNCLATEST.' USING '.TABLE_URLS.','.TABLE_SYNCLATEST
							.' WHERE '.$urls_t->field_id.' = '.toSQL($opts['urlid']).' AND '
							.$urls_t->field_userid." = '{$GLOBALS[GLOBAL_USER]->getUserId()}' AND "
							.$urls_t->field_insync.' = '.$last_t->field_id.' AND '
							.$last_t->field_userid.' = '.$urls_t->field_userid.' LIMIT 1';
					}*/
					$query = 'DELETE FROM '.TABLE_KEYWORDS.' WHERE '.$key_t->field_urlid
						.' = '.toSQL($opts['urlid']);
					db::query($query);
				break;
				case 'edithist':
					if (empty($_POST) || empty($opts['urlid']))break;
					$updated = false;
					if (!empty($_POST['link']) 
							|| !empty($_POST['title']) 
							|| array_key_exists('description',$_POST)
							|| array_key_exists('tags',$_POST)){
						if (array_key_exists('tags',$_POST)){
							$keywords = CatPath::parseStrToKeywords($_POST['tags'],true);
							$keywords = empty($keywords)?null:$keywords;
						}
						$updated = true;
						$query = 'UPDATE '.TABLE_URLS.' SET '
							.$urls_t->field_time.' = NOW(),'
							//if the change to the url occured, then we need to initialize
							//its insync and private fields as well
							.(!empty($_POST['link'])?$urls_t->field_url.' = '.toSQL($_POST['link']).','
							.$urls_t->field_private.' = '.(LinkManager::isPrivateUrl($_POST['link'])?'1':'NULL').','
							.$urls_t->field_insync.' = NULL,':'')
							//keywords?
							.(array_key_exists('tags',$_POST)?$urls_t->field_keywords.' = '
								.toSQL($keywords).',':'')
							//title?
							.(!empty($_POST['title'])?$urls_t->field_title.' = '.toSQL($_POST['title']).',':'')
							.(array_key_exists('description',$_POST)?$urls_t->field_description.' = '
							.(empty($_POST['description'])?'NULL':toSQL($_POST['description'])):'');
						$query = preg_replace('/,$/','',$query);
						$query .= ' WHERE '.$urls_t->field_id.' = '.toSQL($opts['urlid']).' AND '.$urls_t->field_userid
							." = '".$GLOBALS[GLOBAL_USER]->getUserId()."' LIMIT 1";
						db::query($query);
						//make sure this url belonged to this user
						if (db::affected_rows()){
							//delete previous keywords
							$query = 'DELETE FROM '.TABLE_KEYWORDS.' WHERE '.$key_t->field_urlid
								.' = '.toSQL($opts['urlid']);
							db::query($query);
							if (!empty($keywords)){
								$keywords = CatPath::explodeStrToKeywords($keywords);
								foreach($keywords as $k => $keyword){
									$keywords[$k] = toSQL($keyword);
								}
								$query = 'INSERT DELAYED IGNORE INTO '.TABLE_KEYWORDS.' ('
									.sqlFields(array($key_t->field_urlid,$key_t->field_keyword))
									.") VALUES ({$opts['urlid']},".implode("),({$opts['urlid']},",$keywords).')';
								db::query($query);
							}
						}
					}
				break;
				case 'movelink':
					$this->type = self::TYPE_JS;
					if (empty($opts['urlid']))break;
					$linkManager = new LinkManager($GLOBALS[GLOBAL_USER]->getUserId());
					$linkManager->moveToSync($opts['urlid']);
				break;
				case 'savenew':
					if (empty($_POST['link']) || empty($_POST['title']))break;
					$linkManager = new LinkManager($GLOBALS[GLOBAL_USER]->getUserId());
					$linkManager->addNewUrl($_POST['link'],$_POST['title'],
						!empty($_POST['description'])?$_POST['description']:null,
						!empty($_POST['tags'])?$_POST['tags']:null,
						(!empty($_POST['isfeed']) && $_POST['isfeed'] === 'f'));
				break;
				case 'makeprivate':
					if (empty($opts['urlid']))break;
					$query = 'UPDATE '.TABLE_URLS.' SET '.$urls_t->field_private.' = 1'
						.' WHERE '.$urls_t->field_id.' = '.toSQL($opts['urlid']).' AND '
						.$urls_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()
						."' LIMIT 1";
					db::query($query);
				break;
				case 'makepublic':
					if (empty($opts['urlid']))break;
					$query = 'UPDATE '.TABLE_URLS.' SET '.$urls_t->field_private.' = NULL'
						.' WHERE '.$urls_t->field_id.' = '.toSQL($opts['urlid']).' AND '
						.$urls_t->field_userid." = '".$GLOBALS[GLOBAL_USER]->getUserId()
						."' LIMIT 1";
					db::query($query);
				break;
				default:
					log_writeLog('unknow action "act" in private.php, request:'.$_SERVER['REQUEST_URI']);
			}
		}
	}
	private function out($str){
		$this->out .= $str;
	}
	public function flush(){
		if ($this->out === '')return;
		if ($this->type !== self::TYPE_HTML)
			header("Content-type: {$this->type}; charset=UTF-8");
		if ($this->type === self::TYPE_HTML)
			Template::printSearchHeader();
		echo $this->out;
		if ($this->type === self::TYPE_HTML)
			Template::printFooter();
	}
	private function echoSynchronized(){
		echo '<div style="padding:20px;">Please use your browser 
		to manage synchronized bookmarks. If you are looking to export your bookmarks, a number of 
		options are available: <a href="/export/html">html</a> <a href="/rss/mine?l=1000">rss</div>';
	}
}
if (!defined('NINCLUDE')){
	db::connect();
	$priv = new Priv();
	$priv->process($_GET);
	$priv->flush();
	db::close();
}
?>