<?php
header('Content-type: text/javascript;');
if (!empty($_GET['f']) && preg_match('/^[a-z\-]+$/',$_GET['f'])){
	echo file_get_contents($_SERVER['DOCUMENT_ROOT'].'/scripts/includes/js/'.$_GET['f'].'.js');
}
?>