<?php
if (isset($_GET['small'])){
ob_start();
}
//suggest an email
if (!isset($GLOBALS['glbFields'][Admin::ADMIN_REGISTER_NAME]) && isset($_GET['login'])){
	$GLOBALS['glbFields'][Admin::ADMIN_REGISTER_EMAIL] = $_GET['login']; 
}
$temp_t = new table_users();
?>
<p style="padding-left:10px"><a class="dblArrows" href="/my?action=signin">&gt;&gt;</a>&nbsp;&nbsp;<span style="font-size:150%;font-weight:bold;color:#2F2C2C;">Register</span></p>
<div style="padding:20px;">
<?php if (!isset($_GET['success'])):?>
<?php
	if (!is_null($admCntrl->get(Admin::ADMIN_ERROR))){
		echo '<p class="error"><b>Error: </b>' . $admCntrl->get(Admin::ADMIN_ERROR) . '</p>';
	}
	?>
<table><tr><td valign="top">
<form method=post action="/my?action=register&submit=1">
<input type="hidden" name="<?=Admin::ADMIN_SECURITY_COUNTER?>">
			<table width="350" class="cntcbrd" style="border-color:#CCCCCC;">
			<tr>
				<td align="right"><span id="auth_username">username</span>: </td>
				<td><input id="auth_username_f" type="text" name="<?=Admin::ADMIN_REGISTER_NAME?>" size="20" maxlength="<?=$temp_t->field_username_length?>">&nbsp;&nbsp;letters only (4 min)</td>
			</tr>
			<tr>
				<td align="right"><span id="auth_email">email</span>: </td>
				<td><input type="text" id="auth_email_f" name="<?=Admin::ADMIN_REGISTER_EMAIL?>" size="30" maxlength="<?=$temp_t->field_email_length?>"></td>
			</tr>
			<tr>
				<td align="right"><span id="auth_password">password</span>: </td>
				<td><input type="password" id="auth_password_f" name="<?=Admin::ADMIN_REGISTER_PASS?>" size="20"></td>
			</tr>
			<tr>
				<td align="right"><span id="auth_verify">verify</span>: </td>
				<td><input type="password" id="auth_verify_f" name="<?=Admin::ADMIN_REGISTER_PASSVERIFY?>" size="20"></td>
			</tr>
			<tr><td colspan="2" style="font-size:10px;"><input id="auth_terms_f" type="checkbox" name="<?=Admin::ADMIN_REGISTER_IAGREE?>" value="1"> <span id="auth_terms">I acknowledge my understanding and acceptance of the </span><a href="/doc/terms" target="_blank" style="color:#003399">Terms of Use</a></td></tr>
			<tr><td colspan="2" align="center"><input type="submit" value="submit" class="btn"></td></tr>
			</table>
		</form>
<td valign="top"><p style="font-size:140%;">
<?php if (!isset($_GET['small'])):?>
<span style="font-weight:600">Gnafi</span> lets you keep your bookmarks synchronized and accessible from any computer.<br/>
<p><strong>search, </strong>don't sort - unless you really want to..<br/>
<p><img src="/images/register_img2.gif"/>
</p>
<hr/>
<p><img src="/images/register_img3.gif"/>
</p>
</p>
<?php endif;?>
</td>
</tr>
</table>
<script type="text/javascript">
var ids = ['auth_username_f','auth_email_f','auth_password_f','auth_verify_f','auth_terms_f'];
function checkFormat(id){
	
	var obj = getObjectByID(id);
	switch(id){
		case ids[0]:
			if (obj.value.match(new RegExp('^[a-z_]{4,}$','gi')))
				makeRegAuth(id);
			else
				makeBadAuth(id);
			break;
		case ids[1]:
			if (obj.value.match(new RegExp('.+@.+[\.].+$','g')))
				makeRegAuth(id);
			else
				makeBadAuth(id);
			break;
		case ids[2]:
			if (obj.value.match(new RegExp('^[a-z0-9]{4,}$','gi')))
				makeRegAuth(id);
			else
				makeBadAuth(id);
			break;
		case ids[3]:
			if (obj.value.match(new RegExp('^[a-z0-9]{4,}$','gi')) 
					&& getObjectByID(ids[2]).value == obj.value)
				makeRegAuth(id);
			else
				makeBadAuth(id);
			break;
		case ids[4]:
			if (obj.checked)
				makeRegAuth(id);
			else
				makeBadAuth(id);
			break;
	}
}
function keyHandlerAuth(e){
	checkFormat($eTarget(e).id)
}
function makeRegAuth(id){
	getObjectByID(id.strReplace('_f','')).style.color = "#000000";
}
function makeBadAuth(id){
	getObjectByID(id.strReplace('_f','')).style.color = "#CF4D4D";
}
for (i=0;i<ids.length;i++){
	getObjectByID(ids[i]).onkeyup = keyHandlerAuth;
	getObjectByID(ids[i]).onclick = keyHandlerAuth;
	checkFormat(ids[i]);
}
</script>
<?php else:?>
<div style="font-size:150%;color:#6699CC;font-weight:bold;margin-bottom:20px;">Welcome to Gnafi! </div>
<p style="font-size:14px;line-height:20px;">To begin, please <a href="/download.xpi" target="_blank">download</a> our extension, 
and make sure you visit your <a href="/mine">home page</a>. If you are looking for ways to import your bookmarks manually from 
places like del.icio.us, ..(soon more) - follow this link: <a href="/import" style="font-weight:bold;">import my links</a></p>
<?php endif;?>
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