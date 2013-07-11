<html>
<head>
<link rel="stylesheet" type="text/css" href="/css/gnafi.css"/>
</head>
<body>
<div id="side-search">
<div class="hdrsearch">
<form method="get" action="/">
<input name="search" value="1" type="hidden">
Search: <input name="q" value="<?=toHTML(!empty($options['q'])?$options['q']:'')?>" id="srchField1" size="20" type="text">&nbsp;&nbsp;
<input id="srchbtn1" value="Go!" type="submit">
<input type="hidden" name="out" value="sidebar"/>
</form>
<script type="text/javascript">
document.getElementById('srchField1').focus();
</script>
</div>
<?php
if ($resultSet != null && $row = db::fetch_row($resultSet)){
	assert(isset($field_keys));
	assert(isset($url_t));
	assert(isset($users_t));
	assert(isset($parser));
	assert(isset($options));
	//make the above global
	$GLOBALS['url_t'] = $url_t;
	$GLOBALS['users_t'] = $users_t;
	$GLOBALS['field_keys'] = $field_keys;
	$GLOBALS['options'] = $options;
	echo '<div id="indexlinks"><ol>';
	$count = 0;
	do{
		printUrl($row);
		++$count;
	}while($row = db::fetch_row($resultSet));
	echo '</ol></div>';
	echo '<p align="center">',$parser->getSqlPages($count),'</p>';
}else{
	echo '<p style="padding-left:5px;">no results were found</p>';
}
function printUrl($row){
	static $count = 0;
	++$count;
	global $url_t,$field_keys,$users_t,$options;
	//$field_keys are inverted keys, such that to get a value we use: 
	//$row[$field_keys[$url_t->field...]] - this elimates the problem with when fetch_assoc
	//returns field names without table names
	$title = $row[$field_keys[$url_t->field_title]];
	$title = toHTML($title);
	$link = toHTML($row[$field_keys[$url_t->field_url]]);
	if (!empty($row[$field_keys[$url_t->field_keywords]])){
		$tags = CatPath::explodeStrToKeywords($row[$field_keys[$url_t->field_keywords]]);
		foreach($tags as $k => $t){
			$tags[$k] = '<a href="/p/'.urlencode($t).'?out=sidebar" id="history-link-'.$row[$field_keys[$url_t->field_id]].'-'.$k.'" class="tag">'.$t.'</a>';
		}
	}
	echo '<li id="link-',$row[$field_keys[$url_t->field_id]],'" class="item"><div>'.$count.') <a target="_blank" class="bk-link" id="history-link-'.$row[$field_keys[$url_t->field_id]].'" href="',$link,'">',
		$title.'</a></div></span>',(!empty($tags)?implode(' ',$tags):''),'</li>';
}
?>
</div>
</body>
</htm>