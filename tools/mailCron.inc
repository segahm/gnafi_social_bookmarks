<?php
define('MAX_MESSAGES',10);	//the number of messages we send every 2 minutes
$_SERVER['DOCUMENT_ROOT'] = substr(__FILE__,0,strlen(__FILE__)-strlen('/tools/mailCron.inc'));
require_once($_SERVER['DOCUMENT_ROOT']."/scripts/includes/interface.inc");
db::connect();
$mail = new table_mail();
$query = 'SELECT * FROM '.TABLE_MAIL.' LIMIT '.MAX_MESSAGES;
$result = db::query($query);
$count = 0;
while ($row = db::fetch_assoc($result)){
	$to = explode(',',$row[$mail->field_to]);
	$body = explode('<<delim>>',$row[$mail->field_message]);
	$subject = $body[0];
	$message = $body[1];
	$content_type = 'text/plain';
	$header = "From: no_reply@gnafi.com\r\nReply-To: no_reply@gnafi.com\r\n"; 
	$header .= "MIME-Version: 1.0\r\n"; 
	$header .= "Content-Type: $content_type; charset=\"ISO-8859-1\"\r\n"; 
	$header .= "Content-Transfer-Encoding: 8bit\r\n"; 
	foreach($to as $email){
		mail($email,$subject,$message,$header) or log_writeLog('failed sending a message to: '.$email);
	}
	$count++;
}
$query = 'DELETE FROM '.TABLE_MAIL.' LIMIT '.$count;
db::query($query);
db::close();
?>