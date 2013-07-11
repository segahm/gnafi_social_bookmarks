<?php
if (isset($_GET['small'])){
ob_start();
}
if (!isset($GLOBALS['glbFields'][Admin::ADMIN_FORGOT_EMAIL]) && isset($_GET['login'])){
	$GLOBALS['glbFields'][Admin::ADMIN_FORGOT_EMAIL] = $_GET['login']; 
}
?>
<p style="padding-left:10px">
<a class="dblArrows" href="/my?action=signin">&gt;&gt;</a>&nbsp;&nbsp;<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Password Reset</span></p>
<div style="padding:20px;">
<form action="/my?action=forgot&submit=1" method="post">
	<p>If you have forgotten your gnafi password, please enter your email address (gnafi login) 
	and we will send the temporary password to you</p>
	<input type="text" name="<?=Admin::ADMIN_FORGOT_EMAIL?>" value="" size="40">
	<input type="hidden" name="<?=Admin::ADMIN_SECURITY_COUNTER?>" value="0">
	<input type="submit" value="submit" class="btn">
	<?php
	if (isset($_GET['submit']) && is_null($admCntrl->get(Admin::ADMIN_ERROR))){
		echo '<p><span style="color:#0000EE;font-weight:bold;">Important: </span>we have successfully reset your password and you should receive it in a few short minutes</p>';
	}elseif(isset($_GET['submit'])){
		echo '<p><font class="error"><b>Error: </b>'.$admCntrl->get(Admin::ADMIN_ERROR).'</font></p>';
	}
	?>
	</form>
</div>
<?php 
//small window handler
if (isset($_GET['small'])){
	$contents = ob_get_contents();
	ob_clean();
	echo preg_replace(array('-(action="[^"\\?]+)\\??([^"]*)"-','-(href="[^"\\?]+)\\??([^"]*)"-'),'$1?$2&small=1"',$contents);
	ob_end_flush();
}
?>