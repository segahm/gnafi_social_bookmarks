<?php
class Template{
	private static $store = array(
		/*'javascript' => array(array('common',null),array('commonobj',null)),
		'css' => array(array('gnafi',null)),
		'jsurls' => array(),
		'cssurls' => array(),
		'header' => ''*/
		'javascript' => array(),
		'css' => array(),
		'jsurls' => array('/js/common.js','/js/commonobj.js'),
		'cssurls' => array('/css/gnafi.css'),
		'header' => ''
	);
	private static $stages = array();
	const STAGE_STYLES = 'STAGE_STYLES';
	const STAGE_SCRIPTS = 'STAGE_SCRIPTS';
	const STAGE_HEADER = 'STAGE_HEADER';
	private static $pageLinks = '';
	private static $inited = false;
	public static $isCleaning = false;
	public static function addHeader($str){
		self::$store['header'] .= $str;
	}
	public static function beginClean(){
		self::$isCleaning = true;	
		ob_start(array('Template','system_stop_clean'));
	}
	public static function endClean(){
		if(self::$isCleaning)ob_end_clean();
	}
	public static function system_stop_clean(){
		return '';
	}
	public static function addPageLink($link,$title,$poup =  null){
		self::$pageLinks .= '&nbsp;<a class="sideLinks" href="'.$link.'"'.(is_null($poup)?'':' title="'.toHTML($poup).'"').'>['.toHTML($title).']</a>';
	}
	//the following 2 functions should only be used for local scripts
	/**
	 * @param string $strFile name of the file to add (ex: common stands for js/common.js) 
     * @param array fields fields that will be passed to the scripts using $js_fields array
     *
     * @access public
     */
	public static function addSystemScript($strFile/*,$fields = null*/){
		assert(!Eventster::isExpired(self::STAGE_SCRIPTS));
		//self::$store['javascript'][] = array($strFile,$fields);
		self::$store['jsurls'][] = '/js/'.$strFile.'.js';
	}
	/**
	 * @param string $strFile name of the file to add (ex: common stands for css/common.css) 
     * @param array fields fields that will be passed to the css include using $css_fields array
     *
     * @access public
     */
	public static function addSystemCSS($strFile/*,$fields = null*/){
		assert(!Eventster::isExpired(self::STAGE_STYLES));
		//self::$store['css'][] = array($strFile,$fields);
		self::$store['cssurls'][] = '/css/'.$strFile.'.css';
	}
	/**
	 * @param string $url which will appear in <script src="url"></script>
     *
     * @access public
     */
	public static function addJsUrl($url){
		assert(!Eventster::isExpired(self::STAGE_HEADER));
		self::$store['jsurls'][] = $url;
	}
	/**
	 * @param string $url which will appear in <link rel="stylesheet" type="text/css" href="$url"/>
     *
     * @access public
     */
	public static function addCssUrl($url){
		assert(!Eventster::isExpired(self::STAGE_HEADER));
		self::$store['cssurls'][] = $url;
	}
	private static function printStyles(){
		//uncomment the following for production stage
		foreach(self::$store['css'] as $value){
			$css_fields =& $value[1];
			require_once('css/'.$value[0].'.css');
			echo "\n";
		}
		Eventster::runEvent(self::STAGE_STYLES);
	}
	private static function printJS(){
		//uncomment the following for production stage
		foreach(self::$store['javascript'] as $value){
			$js_fields =& $value[1];
			require_once('js/'.$value[0].'.js');
			echo "\n";
		}
		Eventster::runEvent(self::STAGE_SCRIPTS);
	}
	public static function printSearchHeader(){
		?>
		<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html;  charset=UTF-8"/>
<style type="text/css">
<?
	self::printStyles();
?>
</style>
<script type="text/javascript">
<!--
<?
	self::printJS();
?>
//-->
</script>
<?php
//remove the following two loops for production stage
/*foreach(self::$store['css'] as $value){
	echo '<link rel="stylesheet" type="text/css" href="/js.css?p='.$value[0].'"/>';
}
foreach(self::$store['javascript'] as $value){
	echo '<script type="text/javascript" src="/js.js?p='.$value[0].'"></script>';
}*/
//include user-made scripts
foreach(self::$store['cssurls'] as $url){ 
	echo '<link rel="stylesheet" type="text/css" href="'.$url.'"/>';
}
foreach(self::$store['jsurls'] as $url){
	echo '<script type="text/javascript" src="'.$url.'"></script>';
}
Eventster::runEvent(self::STAGE_HEADER);
?>
<link rel="shortcut icon" type="image/x-icon" href="http://gnafi.com/images/favicon.ico"/>
<?=self::$store['header'];?>
<title>Gnafi - <?=toHTML($_SERVER['CRUMB_PATH'])?></title>
</head>
<body>
<a name="top">&nbsp;</a>
<table align="center" cellpadding="0" cellspacing="0" 
<?=(isset($_GET['small']))?'width="600"':'width="800"'?>><tr><td>
<table width="100%"><tr><td>
<a href="/" id="hdrcrumb-title" title="Go to gnafi.com"><img style="margin-bottom:-5px;" src="/images/logo.jpg"/></a><span 
class="hdrcrumb-delim">/</span><span id="hdrcrumb"></span>
<script type="text/javascript">
crumbHdr = new Crumb('hdrcrumb',"<?=(isset($_GET['post']) && $_GET['post']==1)?'?post=1':''?>");
crumbHdr.go("<?=$_SERVER['CRUMB_ROOT']?>","<?=$_SERVER['CRUMB_WORD']?>");
</script>
<?php 
/*if (empty($_COOKIE[COOKIE_COMMON]) || empty($_COOKIE[COOKIE_COMMON]['firsttime'])):
echo '<span style="margin-top:5px;color:#999999;">&nbsp;&nbsp;- use this to search under specific keyword[s] (ex: college/berkeley)</span>';
endif;
*/
?>
</td>
<td align="right">
<a class="hdrLinks" href="/doc/faq">[?]</a>&nbsp;&nbsp;
<?php 
if (User::checkAuth(false,false)):?>
<a class="hdrLinks" href="/my?go=settings">[settings]</a>&nbsp;&nbsp;
<a class="hdrLinks" href="/mine">[bookmarks]</a>&nbsp;&nbsp;
<a class="hdrLinks" href="/my?action=signout&path=<?=urlencode($_SERVER['REQUEST_URI'])?>">[signout]</a>&nbsp;&nbsp;
<?php else:
/*
	if (empty($_COOKIE[COOKIE_COMMON]) || empty($_COOKIE[COOKIE_COMMON]['firsttime'])):?>
	<a href="/doc/welcome" id="whatisit">[what is it?]</a>&nbsp;&nbsp;
	<?php else:?>
	<a class="hdrLinks" href="/doc/welcome">[what is it?]</a>&nbsp;&nbsp;
	<?php endif;
	*/
	?>
<a class="hdrLinks" href="/my?action=signin&path=<?=urlencode($_SERVER['REQUEST_URI'])?>">[signin / register]</a>&nbsp;&nbsp;
<?php endif;?>
</td></tr></table>
<table width="100%">
<tr><td align="left">
<div class="hdrsearch">
<table><tr><td>
<form id="frmsrch" method="GET">
<input type="hidden" name="search" value="1"/>
Search: <input type="text" name="q" value="<?=!empty($_GET['q'])?toHTML($_GET['q']):''?>" id="srchField1" <?=(isset($_GET['small']))?'size="20"':'size="50"'?>/>&nbsp;&nbsp;
<input type="submit" id="srchbtn1" value="Go!"/>&nbsp;&nbsp;&nbsp;&nbsp;
</form>
</td>
<script type="text/javascript">
<?php 
if (User::checkAuth(false,false)):?>
document.write( ''+
		'<td><form id="frmsrch2" action="/mine" method="GET"><input type="text" id="srchField2" name="q" value="<?=!empty($_GET['q'])?toHTML($_GET['q']):''?>" size="15"/><input type="submit" id="srchbtn2" value="Find In My Collection..."/></form></td>' );
<?php endif; if (!isset($_GET['crmb'])):?>
<?php if (isset($_GET['search'])):?>
getObjectByID('srchField1').focus();
<?php else:?>
var obj = getObjectByID('srchField2');
if (obj)obj.focus();
<?php endif;?>
<?php else:?>
getObjectByID('hdrcrumb-crumb').focus();
<?php endif;?>
</script>
</tr></table>
</div>
</td></tr></table>
<table width="100%"><tr><td>
<div style="float:right;"><?=self::$pageLinks?></div>
		<?php
	}
	public static function printFooter(){
	?>
</td></tr></table>
<br/>
<br/>
<center>
<a href="/" class="footer">home</a>
&nbsp;|&nbsp;
<a href="/doc/faq" class="footer">help</a>
&nbsp;|&nbsp;
<a href="/doc/hacks" class="footer">hacks</a>
&nbsp;|&nbsp;
<a href="/doc/terms" class="footer">Terms of Use</a>
<font style="font-size:10px">
<br/>a Mirkin creation,<br/> Copyright &copy; 2006 All Rights Reserved.</font>
</center>
</td></tr></table>
</body>
</html>
<?php
	}
	
	
	
	public static function printSimpleBody($title,$message){
	echo <<<EOD
		<div style="padding:20px;">
<span style="font-size:150%;color:#000000">$title</span>
<p style="padding-top:20px;">
$message
	</p>
</div>
EOD;
	}
}
?>