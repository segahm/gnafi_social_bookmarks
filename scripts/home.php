<?php
define('COOKIE_CUSTOMTAG','customtag');
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
if (empty($_COOKIE[COOKIE_COMMON]) || empty($_COOKIE[COOKIE_COMMON]['firsttime'])){
	setcookie(COOKIE_COMMON."[firsttime]",'1',time()+60*60*24*7);
}
header('Cache-Control: max-age=3600',true);
/*
class Home{
	const DEFAULT_TAG = 'how_to_use_gnafi';
	function __construct(){
		//do a soft user check
		User::checkAuth(false,false);
		if (isset($_GET['tag'])){
			$_GET['tag'] = str_replace(array('+',' '),'_',preg_replace('-^/?p/-','',$_GET['tag']));
			setcookie(COOKIE_COMMON.'['.COOKIE_CUSTOMTAG.']',$_GET['tag'],time()+60*60*24*7);
			$_COOKIE[COOKIE_COMMON][COOKIE_CUSTOMTAG] = $_GET['tag'];
		}
	}
	public function draw(){
		Template::printSearchHeader();
		echo '<table width="100%" id="home-page"><tr valign="top">';
		//draw left side
		$tag = self::DEFAULT_TAG;
		if (!empty($_COOKIE[COOKIE_COMMON]) && !empty($_COOKIE[COOKIE_COMMON][COOKIE_CUSTOMTAG]))
			$tag = $_COOKIE[COOKIE_COMMON][COOKIE_CUSTOMTAG];
		$root = '';
		if (preg_match('-/-',$tag)){
			$root = explode('/',$tag);
			$tag = array_pop($root);
			$root = implode('/',$root).'/';
		}
		echo <<<EOD
		<td id="left-side" width="100%">
		<script type="text/javascript">
		<!--
		function CustomTagCrumb(divid,urladdon){
			this.inheritFrom = Crumb;
  			this.inheritFrom(divid,urladdon);
			this.mouseover = function(e) {
				var crumbelem = \$eTarget(e);
				crumbelem.style.borderColor = "#000000";
				crumbelem.style.borderWidth = "1px";
			}
			this.mouseout = function(e) {
				var crumbelem = \$eTarget(e);
				crumbelem.style.borderWidth = "0px";
			}
			this.focus = function(e) {
				var crumbelem = \$eTarget(e);
				crumbelem.style.color = "#000000";
				crumbelem.style.fontWeight = "bold";
				crumbelem.style.borderWidth = "1px";
			}
			this.blur = function(e) {
				var o = ObjectStorage.get("Crumb-"+divid);
				var crumbelem = \$eTarget(e);
				crumbelem.style.borderWidth = "0px";
				crumbelem.style.color = "#0000FF";
				crumbelem.style.fontWeight = "normal";
				var l = crumbelem.value.length;
				crumbelem.value = o.tag;	//remind user to press enter
				if (l != crumbelem.value.length)
					resizeToText(crumbelem,crumbelem.value,20);
			}
		}
		//-->
		</script>
		<span id="custom-tag"></span>
		<script type="text/javascript">
		crumbHdr = new CustomTagCrumb('custom-tag','?tag=<<p>>');
		crumbHdr.go("$root","$tag");
		</script>
		<div id="custom-tag-result">
EOD;
		require_once($_SERVER['DOCUMENT_ROOT'].'/index.php');
		$_SERVER['CRUMB_PATH'] = $root.$tag;
		$main = new Main();
		$ar2 = array('noout' => 1,Main::SQL_LIMIT_GET => 20,'nopages' => 1);
		$main->processUrl($ar2);
		echo '</div></td>';
		//draw right side
		echo '<td id="right-side"><div style="float:right;">';
		echo '<h2><a id="header" href="/tags/popular">popular</a></h2>';
		$ar = self::getLatestPopular(5,5);
		echo '<ol class="tags-list">';
		foreach($ar[0] as $tag => $urls){
			echo '<li><h3><a href="/p/',$tag,'">',str_replace('_',' ',$tag),'</a></h3>';
			echo '<ol class="urls-list">';
			foreach($urls as $id){
				echo '<li><a href="',toHTML($ar[1][$id][0]),'">',toHTML(funcs_cutLength($ar[1][$id][1],40)),'</a></li>';
			}
			echo '</ol>';
			echo '</li>';
		}
		echo '</ol>';
		echo '</div></td>';
		echo '</tr></table>';
		Template::printFooter();
	}
	public static function getLatestPopular($tags_limit,$urls_limit){
		$urls_t = new table_urls();
		$key_t = new table_keywords();
		$query = 'SELECT '.$key_t->field_keyword
			.', GROUP_CONCAT('.$key_t->field_urlid."),count(*) as c FROM ".TABLE_KEYWORDS.' GROUP BY '.$key_t->field_keyword
			.' ORDER BY c DESC, '.$key_t->field_id.' DESC LIMIT '.$tags_limit;
		$result = db::query($query);
		$tags = array();
		$allids = array();
		$count = 0;
		while($row = db::fetch_row($result)){
			$urls = explode(',',$row[1],$urls_limit);
			$size = count($urls);
			if (($pos = strpos($urls[$size-1],',')) !== false)
				$urls[$size-1] = substr($urls[$size-1],0,$pos);
			foreach($urls as $urlid){
				$allids[] = $urlid;
				++$count;
			}
			$tags[$row[0]] = $urls;
		}
		$ids = array();
		if (!empty($allids)){
			$query = 'SELECT '.$urls_t->field_url.','.$urls_t->field_id.','.$urls_t->field_title.' FROM '
				.TABLE_URLS.' WHERE '.$urls_t->field_id.' IN ('.toSQL($allids).') LIMIT '.$count;
			$allids = null;
			$result = db::query($query);
			while($row = db::fetch_row($result)){
				$ids[$row[1]] = array($row[0],$row[2]);
			}
		}
		return array($tags,$ids);
	}
}
if (!defined('NINCLUDE')){
	define('NINCLUDE',1);
	db::connect();
	$main = new Home();
	$main->draw();
	db::close();
}
*/
class Home{
	const DEFAULT_TAG = 'how_to_use_gnafi';
	function __construct(){
		//do a soft user check
		User::checkAuth(false,false);
		if (isset($_GET['tag'])){
			$_GET['tag'] = str_replace(array('+',' '),'_',preg_replace('-^/?p/-','',$_GET['tag']));
			setcookie(COOKIE_COMMON.'['.COOKIE_CUSTOMTAG.']',$_GET['tag'],time()+60*60*24*7);
			$_COOKIE[COOKIE_COMMON][COOKIE_CUSTOMTAG] = $_GET['tag'];
		}
	}
	public function draw(){
		/*require_once($_SERVER['DOCUMENT_ROOT'].'/index.php');
		$_SERVER['CRUMB_PATH'] = $root.$tag;
		$main = new Main();
		$ar2 = array('noout' => 1,Main::SQL_LIMIT_GET => 20,'nopages' => 1);
		$main->processUrl($ar2);
		*/
		//draw right side
		//attempt to use cache
		$fname = CACHE_DIR.'/home_'.date('j_m_y');
		if ($text = @file_get_contents($fname)){
			echo $text;
		}else{
			$ar = self::getLatestPopular(5,5);
			ob_start();
			echo '<ol class="tags-list">';
			foreach($ar[0] as $tag => $urls){
				echo '<li><h3><a href="/p/',$tag,'">',str_replace('_',' ',$tag),'</a></h3>';
				echo '<ol class="urls-list">';
				foreach($urls as $id){
					echo '<li><a href="',toHTML($ar[1][$id][0]),'">',toHTML(funcs_cutLength($ar[1][$id][1],100)),'</a></li>';
				}
				echo '</ol>';
				echo '</li>';
			}
			echo '</ol>';
			$cont = ob_get_contents();
			ob_end_flush();
			file_put_contents($fname,$cont);
		}
	}
	public static function getLatestPopular($tags_limit,$urls_limit){
		$urls_t = new table_urls();
		$key_t = new table_keywords();
		$query = 'SELECT SQL_CACHE '.$key_t->field_keyword
			.', GROUP_CONCAT('.$key_t->field_urlid."),count(*) as c FROM ".TABLE_KEYWORDS.' GROUP BY '.$key_t->field_keyword
			.' ORDER BY c DESC, '.$key_t->field_id.' DESC LIMIT '.$tags_limit;
		$result = db::query($query);
		$tags = array();
		$allids = array();
		$count = 0;
		while($row = db::fetch_row($result)){
			$urls = explode(',',$row[1],$urls_limit);
			$size = count($urls);
			if (($pos = strpos($urls[$size-1],',')) !== false)
				$urls[$size-1] = substr($urls[$size-1],0,$pos);
			foreach($urls as $urlid){
				$allids[] = $urlid;
				++$count;
			}
			$tags[$row[0]] = $urls;
		}
		$ids = array();
		if (!empty($allids)){
			$query = 'SELECT SQL_CACHE '.$urls_t->field_url.','.$urls_t->field_id.','.$urls_t->field_title.' FROM '
				.TABLE_URLS.' WHERE '.$urls_t->field_id.' IN ('.toSQL($allids).') LIMIT '.$count;
			$allids = null;
			$result = db::query($query);
			while($row = db::fetch_row($result)){
				$ids[$row[1]] = array($row[0],$row[2]);
			}
		}
		return array($tags,$ids);
	}
}
db::connect();
?>

<?php
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
Template::printSearchHeader();
?>
<div id="home-page">
<div id="lefthalf">
<?php
$main = new Home();
$main->draw();
?>
</div>
<div id="righthalf">
<table width="200" cellpadding="2" cellspacing="2">
	<tr>
		<td class="cntcthdr"><a href="/doc/welcome">[What is It?]</a></td>
	</tr>
	<tr valign="top">
		<td class="cntcbrd" style="border-color:336699">
			<p style="font-family:MS Sans Serif, Arial, Helvetica;font-size:10pt;">
			Gnafi stores your <span style="color:#CF4D6B">browser links</span> and synchronizes them between 
			different places in your life: work, school, home. <a href="/download.xpi" style="color:#6BCF5A;font-weight:bold;">Download it Here..</a>
			</p>
		</td>
	</tr>
</table>
<table width="200" style="margin-top:20px;" cellpadding="2" cellspacing="2">
	<tr>
		<td class="cntcthdr"><a href="/doc/faq">[How does it work?]</a></td>
	</tr>
	<tr valign="top">
		<td class="cntcbrd" style="border-color:336699">
			<p style="font-family:MS Sans Serif, Arial, Helvetica;font-size:10pt;">
			Gnafi works through a <span style="color:#365F17">firefox</span> 
			extension. The extension keeps track of any <span style="color:#5777AF">changes made</span> to your links and does necessary updates 
			for you (all additions & removals). You can <a href="/download.xpi" style="font-weight:bold">download</a> it here.
			</p>
		</td>
	</tr>
</table>
<table width="200" style="margin-top:20px;" cellpadding="2" cellspacing="2">
	<tr>
		<td class="cntcthdr"><a href="/doc/hacks">[Any cool features?]</a></td>
	</tr>
	<tr valign="top">
		<td class="cntcbrd" style="border-color:336699">
			<p style="font-family:MS Sans Serif, Arial, Helvetica;font-size:10pt;">
			Yep! How about the new browser <a href="/doc/hacks?page=extensionsearch">built-in search</a> 
			that shows you matching links as you type. Or how is the idea of been able to <a href="/doc/hacks?page=sidebar">view</a> all links other 
			people have saved for a particular site (great for forums). There are also many <a href="/doc/hacks">hacks</a> available as well..
			</p>
		</td>
	</tr>
</table>
</div>
</div>
<?php
Template::printFooter();
?>