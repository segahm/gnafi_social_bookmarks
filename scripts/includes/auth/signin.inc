<?php
if (isset($_GET['small'])){
ob_start();
}
?>
<p style="padding-left:10px"><a class="dblArrows" href="/my?action=signin">&gt;&gt;</a>&nbsp;&nbsp;<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Signin</span></p>
<table>
<tr><td style="padding:20px;">
<!-- signin fields begin -->
<form method="POST" action="/my?action=signin&submit=1<?=
(isset($_GET['path']))?'&path='.urlencode($_GET['path']):''?>" name="signin">
<table height="130" width="250"  style="border-width:1px;border-color:#CCCCCC;border-style:solid;">
	<tr>
		<td align="right">e-mail:</td>
		<td><input type="text" name="<?=User::SIGNIN_USER?>" size="20"></td>
	</tr>
	<tr>
		<td align="right">password:</td>
		<td><input type="password" name="<?=User::SIGNIN_PASS?>" size="20"></td>
	</tr>
	<?php
	if (!is_null($admCntrl->get(Admin::ADMIN_ERROR))){
		echo '<tr><td colspan="2" class="error"><b>Error: </b>' . $admCntrl->get(Admin::ADMIN_ERROR) . '</td></tr>';
	}
	?>
	<tr><td colspan=2 align=center><input type=submit value="[signin]" class="btn"></td>
	<tr>
		<td align="center" colspan="2">
		<a class="hdrLinks" href="/my?action=register&login=<?=
		urlencode(((isset($_POST[User::SIGNIN_USER]))?$_POST[User::SIGNIN_USER]:''))?>">[register]</a>&nbsp;&nbsp;&nbsp;
		<a class="hdrLinks" href="/my?action=forgot&login=<?=
		urlencode(((isset($_POST[User::SIGNIN_USER]))?$_POST[User::SIGNIN_USER]:''))?>">[forgot]</a>
		</td>
	</tr>
</table>
</form>
<!-- signin fields end -->
</td>
<td><div><p style="font-size:140%;">
Please login with your nickname/email to begin. With gnafi You can<br/>
<p><strong>keep</strong> your bookmarks synchronized</p>
<p><strong>bookmark</strong> pages from your ordinary browser interface</p>
<p><strong>search</strong> your own bookmarks from a built-in intuitive interface</p>
</div></td>
</tr>
</table>
<?php 
//small window handler
if (isset($_GET['small'])){
	$contents = ob_get_contents();
	ob_clean();
	echo preg_replace(array('-(action="[^"\\?]+)\\??([^"]*)"-','-(href="[^"\\?]+)\\??([^"]*)"-'),'$1?$2&small=1"',$contents);
	ob_end_flush();
}
?>