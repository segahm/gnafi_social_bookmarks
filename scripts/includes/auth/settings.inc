<div style="padding:20px;">
<table>
<?php
echo '<tr><td colspan="2">';
	if ($this->defined(Admin::ADMIN_ERROR)){
		echo '<p class="error"><b>Error: </b>' . $this->get(Admin::ADMIN_ERROR) . '</p>';
	}elseif($this->defined(Admin::ADMIN_MESSAGE)){
		echo '<p><span style="font-weight:bold;color:#6699CC">: '.$this->get(Admin::ADMIN_MESSAGE).'</span></p>';
	}
echo '</td></tr>';
	?>
<tr><td>
<div style="padding:10px;">
<form action="/my?go=settings&submit=email" method="POST">
<input type="hidden" name="name"/>
<p style="font-size:150%;font-weight:bold;color:#2F2C2C;">Email</p>
<table>
<tr valign="top"><td align="right">email:</td>
<td><input type="text" name="email" size="30" maxlength="60"/></td></tr>
<tr><td colspan="2" align="center"><input type="submit" value="change" class="btn"/></td></tr>
</table>
</form>
</div>
<div style="padding:10px">
<form action="/my?go=settings&submit=pass" method="POST">
<input type="hidden" name="name"/>
<input type="hidden" name="email"/>
<p style="font-size:150%;font-weight:bold;color:#2F2C2C;">Password</p>
<table>
<tr><td align="right">old password:</td><td><input type="password" name="oldpass"/></td></tr>
<tr><td align="right">new password:</td><td><input type="password" name="pass"/></td></tr>
<tr><td align="right">verify:</td><td><input type="password" name="verifypass"/></td></tr>
<tr><td colspan="2" align="center"><input type="submit" value="change password" class="btn"/></td></tr>
</table>
</form>
</div>
<div style="padding:10px">
<form action="/my?go=settings&submit=name" method="POST">
<input type="hidden" name="email"/>
<p style="font-size:150%;font-weight:bold;color:#2F2C2C;">Other</p>
<table>
<tr><td align="right">nickname:</td><td><input type="text" name="name"/></td></tr>
<tr><td colspan="2" align="center"><input type="submit" value="apply changes" class="btn"/></td></tr>
</table>
</form>
</div>
<div style="padding:10px">
<form action="/my?go=settings&submit=urls" method="POST">
<input type="hidden" name="email"/>
<input type="hidden" name="name"/>
<p style="font-size:150%;font-weight:bold;color:#2F2C2C;">Permissions</p>
<table>
<tr><td>keep my links private:</td><td><input type="checkbox" name="private" value="1"/></td></tr>
<tr><td colspan="2" align="center"><input type="submit" value="apply changes" class="btn"/></td></tr>
</table>
</form>
</div>
<div style="padding:10px">
<script type="text/javascript">
function confirmDel(){
	return confirm('Are you sure you want to import your bookmarks from scratch?');
}
</script>
<form action="/my?go=settings&submit=delsync" onsubmit="return(confirmDel());" method="POST">
<input type="hidden" name="email"/>
<input type="hidden" name="name"/>
<input type="hidden" name="private"/>
<p style="font-size:150%;font-weight:bold;color:#2F2C2C;">Start Over..</p>
<input type="submit" value="remove all" class="btn"/> synchronized links (web site only - doesn't apply to url history)
</form>
</div>
<div style="padding:10px">
<script type="text/javascript">
function confirmDel2(){
	return confirm('Are you sure you want to clear your url history?');
}
</script>
<form action="/my?go=settings&submit=hist" onsubmit="return(confirmDel2());" method="POST">
<input type="hidden" name="email"/>
<input type="hidden" name="name"/>
<input type="hidden" name="private"/>
<p style="font-size:150%;font-weight:bold;color:#2F2C2C;">Clear Link History..</p>
<input type="submit" value="proceed.." class="btn"/>
</form>
</div>
</td><td valign="top" style="padding-left:20px;" align="left">
<h4 style="margin-left:0px;">How can I adjust my sync.. interval? (Firefox only)</h4>
<p>Go to about:config >> extensions.gnafi.syncinterval_hour</p>
<h4 style="margin-left:0px;">How can I export my bookmarks?</h4>
<p>Since with gnafi you own your original copy of bookmarks, you can always use your browser to export them. 
 Still, the simplest way is to do it from our site and use <a href="/export/html">this link</a>. 
 But we also provide a number of ways to get your links out (such as rss, javascript..) - for more info check out our <a href="/doc/hacks">hacks page</a></p>
<h4 style="margin-left:0px;">How can I import my bookmarks?</h4>
<p>Have you downloaded our <a href="/download.xpi">extension</a>? It does everything for you!
There are <a href="/doc/hacks">hacks</a> however to do it manually..
</p>
<h4 style="margin-left:0px;">What is the result of setting permissions for links to private?</h4>
<p>All the links that are synchronized between your browsers will automatically be marked as private on the site. 
Please note: this does not apply to manual additions.
</p>
</td></tr>
</table>
</div>