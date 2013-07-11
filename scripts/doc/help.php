<?php
require_once($_SERVER['DOCUMENT_ROOT']."/scripts/includes/interface.inc");
require_once('main/template.inc');

Template::printSearchHeader();
ob_start();
?>
<div id="help-page">
<ol id="faq-short">
<insertexthere/>
</ol>
<a name="extension"></a>
<h3>What is this extension you keep talking about?</h3>
<p class="doc-terms-text">I've created a simple firefox extension that synchonizes your bookmarks 
with gnafi servers every once in a while. Of course, I know how hard it can be to find anything in your bookmarks when 
you've got tons of them - so I've also created a built-in search tool that let's you find your bookmarks right from the url bar.
Download it <a href="/download.xpi">here</a> - you will need to have a gnafi account.</p>
<a name="extensionfeatures"></a>
<h3>Can the site be used without the extension?</h3>
<p class="doc-terms-text">Absolutely. Just <a href="/import">import</a> your bookmarks from a standard html file and you are all set.</p>
<a name="extensionfeatures"></a>
<h3>What other feautures this extension has?</h3>
<p class="doc-terms-text">I thought you would never ask! Well, I've created a url protocol ("livefeed:") that lets web site owner's 
subscribe your web browser to rss feeds; of cource, your permission is required! But actual feeds are interpreted as nothing more than 
a list of link titles, so this is harmless :). There is also a built-in bookmark search that you can access right from your urlbar by 
typing either "/" (for the lovers of hierarchical systems like myself) or by typing "b: ..." to do a regular search. 
</p>
<a name="moreaboutsearch"></a>
<h3>Can you say more about this built-in search?</h3>
<p class="doc-terms-text">
Again, with our extension you can search your local bookmarks just by typing "b: ..". Usually, you wouldn't need any other search, 
but in some cases you might want to access your urls by typing the path (i.e. "/programming/..."), so we've created that for you also. 
For example, been a news junkie like I am, I have my news feeds under /feeds path. So whenever I want to check wherever 
there is anything new, all I do is type "/feeds/.." and new titles pop up automatically. Isn't that cool?</p>
<a name="builtinsearch"></a>
<h3>What other Searches can I perform from my browser?</h3>
<p class="doc-terms-text">
When you install the extension, it will automatically add a couple of bookmarks that point to our search engine. These bookmarks have keywords associated with them. 
So whenever you want to perform a search of all gnafi bookmarks (not just yours), all you have to do is type "gn ..something...."
("gnafi something" works too - gn is just a shortcut).
</p>
<a name="syncinterval"></a>
<h3>How can I adjust my sync.. interval? (Firefox only)</h3>
<p class="doc-terms-text">Go to about:config >> extensions.gnafi.syncinterval_hour</p>
<a name="export"></a>
<h3>How can I export my bookmarks?</h3>
<p class="doc-terms-text">Since with gnafi you own your original copy of bookmarks, you can always use your browser to export them. 
But we also provide a number of ways to get your links out (such as rss,javascript..) 
- for more info check out our <a href="/doc/hacks">hacks page</a>. Or you can simply follow <a href="/export/html">this link</a> to get a mozilla compatible version.</p>
<a name="import"></a>
<h3>How can I import my bookmarks?</h3>
<p class="doc-terms-text">Have you downloaded our <a href="/download.xpi">extension</a>? It does everything for you!
Of course, if you are looking to do it manually, then go to our <a href="/import">import page</a> - we support a number of interfaces.
</p>
<a name="shortcuts"></a>
<a name="keywords"></a>
<h3>What are Bookmark Keywords?</h3>
<p class="doc-terms-text">Keywords have multiple purposes. Firstly, they are a good way to organize your bookmarks for search-related purposes. 
Secondly, they can be shared with other people like regular tags can. And finally, they can invoke 
the web page directly; let me give you an example: 
I've added a bookmark with google's search page, and then set a keyword "gg" for it. Now when 
I need to search something, all I do is type "gg something ..." (without the quotes) and vuala. More info can be found <a href="http://www.mozilla.org/docs/end-user/keywords.html">here</a></p>
<a name="livelinks"></a>
<h3>What are Livelinks?</h3>
<p class="doc-terms-text">Livelinks are links that pull additional links from rss feeds. For example, 
I have a livelink that points to 
<a href="livefeed:path=/<toolbar>/news/bbc&url=http%3A%2F%2Fnewsrss.bbc.co.uk%2Frss%2Fnewsonline_world_edition%2Ffront_page%2Frss.xml%23">bbc news</a>.
Fore more info check out <a href="http://www.mozilla.com/firefox/livebookmarks.html">mozilla site</a>.</p>
<a name="linkhistory"></a>
<h3>Link History</h3>
<p class="doc-terms-text">Link Histroy keeps your entire collection of links (even those not currently synchronized with your browser); 
this way if you don't like to keep all your bookmarks active within the browser, you can keep them on our site for future use.</p>
<a name="synchronized"></a>
<h3>Browser Bookmarks</h3>
<p class="doc-terms-text">Browser bookmarks are bookmarks that are synchronized with your browser. Of course in order to have 
this feature working, you would first need to have our extension installed - you can <a href="/download.xpi">get it here</a>.</p>
<a name="whatisrss"></a>
<h3>What is RSS?</h3>
<p class="doc-terms-text">RSS is a format that keeps data on the web standardized so that we as viewers and readers can easily 
access it from any type of application. You may have noticed that most web news articles now have an orange rss link that points 
to the news feed - that's rss. We currently only support rss2, but hopefully we will add support for other formats as well 
(i.e. atom, rss1,..).</p>
<a name="privatebookmarks"></a>
<h3>Privacy & My Bookmarks?</h3>
<p class="doc-terms-text">Yes, you can keep your bookmarks private. Simply go to your private page, then click on the link and set it 
to be private. Note: we also automatically set some links private during synchronization - this, however, works only for those that 
contain http/ftp usernames/passwords in them.</p>
<a name="contact"></a>
<h3>More, more, I need more info.. (or how to get in touch with us)</h3>
<p class="doc-terms-text">Let us know - info at gnafi dot com.</p>
</div>
<?php 
$cont = ob_get_contents();
ob_end_clean();
$matches = array();
preg_match_all('~<a name="([^"]+)">[^h]+<h3>([^<]+)<~',$cont,$matches);
$str = '';
foreach($matches[1] as $k => $name){
	$str .= '<li><a href="/doc/faq#'.$name.'" onclick="location.href = \'/doc/faq#'.$name.'\';highlight();return false;">'.$matches[2][$k]."</a></li>\r\n"; 
}
echo preg_replace('-<insertexthere/>-',$str,$cont,1);
?>
<script type="text/javascript">
function highlight(){
	var el = document.getElementById('highlightid');
	if (el){
		setInnerHTML(el.parentNode,el.firstChild.nodeValue);
	}
	if (location.href.match(new RegExp('#','g'))){
		var k = location.href.replace(new RegExp('[^#]+#','g'),'');
		var as = document.getElementsByTagName('a');
		for(var i=0;i<as.length;i++){
			if (as[i].name == k){
				var e = as[i].nextSibling;
				while((typeof e.nodeName) == 'undefined' || !e.nodeName.match(/h3/i)){
					e = e.nextSibling;
				}
				setInnerHTML(e,'<span class="faq-highligted" id="highlightid">'+getInnerHTML(e)+'</span>');
				break;
			}
		}	
	}
}
highlight();
</script>
<?php
Template::printFooter();
?>