<?php
require_once($_SERVER['DOCUMENT_ROOT']."/scripts/includes/interface.inc");
require_once('main/template.inc');
if (!empty($_GET['page']) && $_GET['page'] == 'snip'){
	Template::addSystemScript('snip');
}
Template::printSearchHeader();
echo '<p style="padding-left:10px"><a class="dblArrows" href="/doc/hacks">&gt;&gt;</a>&nbsp;&nbsp;<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Hacks</span></p><div id="hacks">';
if (!isset($_GET['page']))$_GET['page'] = 'reg';
switch($_GET['page']){
	case 'reg':
	?>
<table width="100%"><tr valign="top"><td>
<h3>Http Get Requests</h3>
<ul>
<li><a href="/doc/hacks?page=generalinfo">General Info</a></li>
<li><a href="/doc/hacks?page=json">Json</a></li>
<li><a href="/doc/hacks?page=rss">RSS</a></li>
</ul>
</td><td>
<h3>Firefox/Mozilla Extension (<a href="/download.xpi">get it here</a>)</h3>
<ul>
<li><a href="/doc/hacks?page=extensionsearch">Doing Searches</a></li>
<li><a href="/doc/hacks?page=extensionshortcuts">Shortcut Keywords</a></li>
<li><a href="/doc/hacks?page=sidebar">Sidebar</a></li>
</ul>
</td><td>
<h3>Html hacks</h3>
<ul>
<li><a href="/doc/hacks?page=livemark">Live Bookmark link</a></li>
<li><a href="/doc/hacks?page=snip">Link Snippets</a></li>
</ul>
</td></tr></table>
<?php
	break;
	case 'generalinfo':
	?>
	<p class="title">Http requests - general info</p>
	<p>The interface that receieves http requests for link results supports the following get parameters:
	<ul><li>s - start of the result set</li><li>l - limit of items (links) per page</li>
	<li>q - query string that is used to search within the title of the link; you can also optionally 
	specify "site:&lt;some domain name here&gt;" (no quotes) to search only with a specific domain</li>
	<li>who - username of the person whose bookmarks you want to see (only applies to public bookmarks)</li>
	</ul></p>
	<p>Additionally to make requests more specific you can use tags/keywords.</p>
	<span>The tag interface is very simple (you may have noticed the box at the of the page, next to [Gnafi] - that's what this is)!<br/>Simply use the following path "/p/&lt;keyword&gt;" 
	(no quotes) or "/p/&lt;keyword_parent&gt;/&lt;keyword_child&gt;" where the link must necessarily be located under /keyword_parent/keyword_child..</span>
	<?php
	break;
	case 'json':
	?>
	<p class="title">What is Json?</p>
	<p><a href="http://www.json.org">You can read about it here</a>, but our json interface outputs 
	all the link results in a javascript object (like <a href="/json/" target="_blank">so</a>). 
	Hence you can get this object on your web page and then do some interesting things with it.
	To get a better understanding of how it works try a few links: <a href="/json/p/xulplanet?l=2">example1</a> 
	<a href="/json/p/?l=3">example2</a> <a href="/json/p/?who=funkypeer&l=2">exampe3</a>.</p> 
	<p>All requests look exactly the same as in <a href="/doc/hacks?page=generalinfo">general info</a> but you also
	have a number of GET variables available to your disposal
	<ul><li>opt_nourl - don't include url (included by default)</li>
	<li>opt_notitle - don't include the title (included by default)</li>
	<li>opt_notags - don't include tags array (included by default)</li>
	<li>opt_tagslimit - limit on tags (no limit by default)</li>
	<li>opt_withdesc - include description of the link (not included by default)</li>
	<li>opt_withtime - include timestamp (in seconds) of the time the link was added</li>
	<li>opt_withuser - include username (not included by default)</li>
	<ul>
	</p>
	<p>Sample json output:</p>
	<div>
	if((typeof Gnafi) == 'undefined')Gnafi = {};Gnafi.posts = [{"u":"http://www.pandora.com/","d":"No. 5  by  Hollywood Undead - Find Music You'll Love - Pandora","t":["pandora"]},{"u":"http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vcstdlib/html/vclrfcpluspluslibraryoverview.asp","d":"Standard C++ Library Overview","t":["programming"]},{"u":"http://www.php.net/manual/en/index.functions.php","d":"PHP Function Index","t":["php_index"]}]
	<div>
	<?php
	break;
	case 'rss':
	?>
	<p class="title">What is RSS?</p>
	We write about it <a href="/doc/faq#whatisrss">here</a>.
	<p class="title">How does the response look like?</p>
	Here is a sample <a href="/rss/p/?l=2">response</a>. RSS requests work exactly the same way 
	json requests do - you just have to change json to rss wherever you see it. In any case, 
	take a look <a href="/doc/hacks?page=json">here</a> for more info on json. 
	<?php
	break;
	case 'livemark':
	$url = 'livefeed:path='.urlencode('/<toolbar>/Latest from Gnafi').'&url='
		.urlencode('http://gnafi.com/rss/p/?l=5');
	?>
	<p class="title">What are Live Bookmarks?</p>
	They are so cool that I even wrote a <a href="/doc/faq#livelinks" target="_blank">help section</a> on them.
	<p class="title">Well, tell me something interesting?</p>
	Browsers inheritantly do not support live-bookmark links, however our little extension includes a handler 
	for these new urls. Basically you can create a link like 
	<a href="<?=$url?>" style="font-weight:bold;">this</a> 
	(where url is "livefeed:path=&lt;encoded path&gt;&url=&lt;encoded url&gt;" - no tags). If 
	you do have our extension installed, then you should see something like this
	<div style="float:left;padding:20px;">
	<img src="/images/hacks_img1.jpg"/>
	</div>
	<?php
	break;
	case 'snip':
	?>
	<h4>Live Bookmark Link - (Firefox/Mozilla only)</h4>
	<div style="float:left">
	<div style="padding:10px;">
	<form>
	<p>Link: <input type="text" id="bookmark-link" size="50"/> <span style="font-size:10px;">ex: http://www.startuplay.com</span></p>
	<p>Link Title: <input type="text" id="bookmark-title" size="30"/></p>
	<p>Default Path: <input type="text" id="bookmark-path" size="50"/> <span style="font-size:10px;">ex: /books/kasparov</span></p>
	<p>Style: <input type="text" id="bookmark-style" size="20"/> <span style="font-size:10px;">optional</span></p>
	<input type="submit" onclick="Snip.doLiveMark();return false;" value="preview" class="whiteBtn" style="margin-left:130pt"/>
	</form>
	</div>
	<h4>Tag</h4>
	<div style="padding:10px;">
	<form id="tag-form">
	<p>Tag: <input type="text" id="snip-tag" value=""/> <input type="submit" onclick="Snip.doTag();return false;" value="preview" class="whiteBtn"/></p>
	<p>Limit: <input type="text" name="j_l" value="5" maxlength="2" size="2"/> </p>
	<p>Order? <input type="checkbox" name="j_numb" value="1"/></p>
	<p>Max. Length: <input type="text" name="j_cutl" value="100" maxlength="3" size="3"/></p>
	<p>After above Length: <input type="text" name="j_endappend" value="..." size="3"/></p>
	</form>
	</div>
	</div>
	<div style="float:right;">
	<p>Cut & Paste:<br/>
	<textarea cols="25" rows="4" id="text-out"></textarea>
	<p>Preview:</p>
	<div id="link-out" style="width:200px;">
	</div>
	<p align="center">
	</p>
	</div>
	<?php
	break;
	case 'extensionsearch':
	?>
	<p class="title">Search Extension</p>
	<p><img src="/images/register_img2.gif"/></p>
	<img src="/images/register_img3.gif"/>
	<p>&nbsp;</p>
	<p class="title">This is cool, how do I get it?</p>
	<p>The built-in search currently comes only with our main extension. You can download it <a href="/download.xpi">here</a>.</p>
	<p class="title">Can you tell me something interesting?</p>
	<p>For regular searches (i.e. with 'b:' prepended), you can use "^","$" 
	to get results that "start" or "end" with the specified keyword. Sorry other regular expressions are currently not supported.</p>
	<?php
	break;
	case 'extensionshortcuts':
	?>
	<p class="title">What are shortcut keywords?</p>
	<p>Have you visited <a href="/doc/faq#keywords">this page</a>?</p>
	<p class="title">How can I specify these keywords in my browser?</p>
	<p>Once you <a href="/download.xpi">download</a> our extension, you can enter a keyword by simly 
	typing ",keyword" at the end of the path - as follows (where "gnafi" is the keyword):</p>
	<p>
	<img src="/images/keyword_img.jpg"/>
	</p>
	<?php
	break;
	case 'sidebar':
	?>
	<p class="title">Sidebar</p>
	<p><img src="/images/sidebar.jpg" style="border:1px solid #CCCCCC"/></p>
	<p class="title">What is it good for?</p>
	<p>Sidebar lets you browse the links other people have saved for a particular site. 
	It is an excellent tool when you are crawling through forums trying to find a particular answer but still stumbling on repeated questions. With gnafi sidebar you can see what others have been able to find, and thus 
	are unlikely to repeat mistakes many made before you.</p>
	<?php
	break;
}
echo '</div>';
Template::printFooter();
?>