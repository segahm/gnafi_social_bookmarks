<?php
header('Content-type: text/javascript; charset=UTF-8');
//json accepts the following parameters
//json_withuser,json_withdesc,json_notags,
//json_nourl,json_notitle,json_withtime
assert(isset($field_keys));
assert(isset($url_t));
assert(isset($users_t));
assert(isset($parser));
assert(isset($options));
$outStr = '<div id="gnafi-linkroll">';
if ($resultSet != null && $row = db::fetch_row($resultSet)){
	$cutl = false;
	$numberList = false;
	if (isset($options['cutl']) 
			&& is_numeric($options['cutl'])){
		$cutl = $options['cutl'];
	}
	if (isset($options['numb'])){
		$numberList = true;
	}
	$count = 1;
	do{
		$title = $row[$field_keys[$url_t->field_title]];
		if ($cutl && strlen($title) > (int)$options['cutl']){
			$title = substr($title,0,$options['cutl'])
				.(!empty($options['endappend'])?$options['endappend']:'...');
		}
		$outStr .= '<div>'.($numberList?$count.') ':'').'<a'.(!empty($options['style'])?' style="'.$options['style']
			.'"':'').' href="'.$row[$field_keys[$url_t->field_url]]
			.'">'.toHTML($title).'</a></div>';
		++$count;
	}while($row = db::fetch_row($resultSet));
	db::free_result($resultSet);
}
$outStr .= '<style type="text/css">.gnafi-linkroll-title{
		color:#000000;font-size:0.5em;
	}
		
	</style><p><span class="gnafi-linkroll-title">powered by </span><a href="http://gnafi.com" class="gnafi-linkroll-title">Gnafi</a></p></div>';
$outStr = str_replace(array("'","\n","\r"),array("\\'",'\n','\r'),$outStr);
if (empty($options['inout']) || $options['inout'] === 'docwrite'){
	echo "\ndocument.write('$outStr');\n";
}else{
	echo "\n{$options['inout']} = '$outStr';\n";
}
if (!empty($options['callback'])){
	$param_str = '';
	$i=0;
	while(isset($options['callparam'.$i])){
		$param_str .= '"'.$options['callparam'.$i].'",';
		++$i;
	}
	if ($param_str != '')$param_str = substr($param_str,0,strlen($param_str)-1);
	echo "\n".$options['callback'].'('.$param_str.');';
}
?>