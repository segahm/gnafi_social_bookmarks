<?php
require_once($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/interface.inc');
Template::printSearchHeader();
?>
<div id="welcomepage">
<h2>Welcome to Gnafi!</h2>
<p>Gnafi stores your bookmarks online (as well as locally) and keeps them synchronized between different places in your life: school, work, home.. It works through a <a style="font-weight:bold;" href="/download.xpi">downloadable</a> 
extension that notes any changes automatically. Of course there are times when you need to access your bookmarks from a public place; 
for such times we've specifically made bookmarks easily accessible through this site as well.</p>
<h2>Search Tool</h2>
<table><tr valign="top"><td>
<div style="width:400px">
<img src="/images/register_img2.gif"/>
<img src="/images/register_img3.gif"/>
</div>
</td><td style="padding-left:30px;">
<p>Now you can search your bookmarks right from the url bar. Simply use the special keyword "b:" 
before you type something to search. </p>
<p>The search interface also supports bookmark hierarchy - so for those hardcore news readers out there, you can now put all your <a href="/doc/faq#livelinks">feeds</a> 
under /feeds folder and vuala - all you need is to type /feeds to see the entire list of feeds.</p>
</td></tr></table>
<h2>New Bookmarking interface</h2>
<table><tr valign="top"><td>
<img src="/images/keyword_img.jpg"/></td>
<td style="padding-left:30px;"><p>- simple to use interface that automatically suggests the best 
path to use for the link. We've also made it easy to add a 
<a href="/doc/faq#keywords">shortcut keyword</a> for your bookmark - everything after the comma is considered to be a keyword.</p>
</td></tr></table>
<h2>A Convinient way to Export Your Bookmarks!</h2>
<table><tr><td valign="top"><img src="/images/export0.jpg" class="exportimage"/>
</td><td valign="middle"><span>=&gt;</span></td><td valign="top"><img src="/images/export1.jpg" class="exportimage"/>
</td><td valign="middle"><span>=&gt;</span></td><td valign="top">
<img class="exportimage" src="/images/export2.jpg"/></td>
</tr></table>
<h2>What to do next?</h2>
<a href="/my?action=register&path=%2F">follow this link</a> to create an account. 
Once you create your new username, your bookmarks will automatically be synchronized.
</div>
<?php
Template::printFooter();
?>