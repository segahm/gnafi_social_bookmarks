<?php
ini_set('display_errors','1');
ini_set('error_reporting','E_ALL');
ini_set('max_execution_time',"60");

define('MAX_MESSAGES',40);	//the number of messages we send every 2 minutes
$_SERVER['DOCUMENT_ROOT'] = substr(__FILE__,0,strlen(__FILE__)-strlen('/tools/announce.php'));
require_once($_SERVER['DOCUMENT_ROOT']."/scripts/includes/interface.inc");
db::connect();
$query = 'SELECT email,username FROM users WHERE urlsprivate is NULL LIMIT '.MAX_MESSAGES;
$result = db::query($query);
$count = 0;
$subject = 'Gnafi is Changing Hands';
$message = <<<EOD
I’m writing today to let you know that Gnafi, the hobby project I’ve put much effort into, is moving to another owner. I believe this is good news since I simply don’t have any time to continue with this (and about another dozen past free-lance hobbies). I hope the new owner will commit to it and make sure the ongoing development continues.

During the past few years much has changed for me; I moved to China, started an IT company, and then launched several independent ventures. As such, everything that was in my life a few years ago is already forgotten or on its way there - and I’m not the kind of person who lives the past. Finally, I can’t wait for this feature/site, whatever you want call it, to turn into something very useful – even for a guy like me who now (fortunately or otherwise) doesn’t use internet for much more than emails & skype (well.., also blogging).

For feedback or to request removal of your information: 
http://startuplay.com/gnafi-changing-owners-hands/

Regards,
Sergey Mirkin
EOD;
$success = 0;
while ($row = db::fetch_assoc($result)){
	$email = $row['email'];
	$name = $row['username'];
	$content_type = 'text/plain';
	$header = "From: NoReply@gnafi.com\r\nReply-To: NoReply@gnafi.com\r\n"; 
	$header .= "MIME-Version: 1.0\r\n"; 
	$header .= "Content-Type: $content_type; charset=\"ISO-8859-1\"\r\n"; 
	$header .= "Content-Transfer-Encoding: 8bit\r\n"; 
	if (mail($email,$subject,"Dear Gnafi User ($name),\r\n\r\n".$message,$header))
		++$success;
	++$count;
}
$query = 'UPDATE users set urlsprivate = 1 WHERE urlsprivate is NULL LIMIT '.$count;
db::query($query);
db::close();
echo time(),"\r\nDeleted ",$count,'; Success: ',$success,'last:',$email;
?>