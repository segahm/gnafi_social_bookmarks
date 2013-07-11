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
	function formatToJson($str){
		return str_replace(array('"',"\n","\r"),array('\\"','\n','\r'),$str);
	}
	echo "if((typeof Gnafi) == 'undefined')Gnafi = {};Gnafi.posts = [";
	if ($resultSet != null && $row = db::fetch_row($resultSet)){
		$flag = true;
		do{
			if (!$flag)echo ',';
			$flag = false;
			if (!empty($row[$field_keys[$url_t->field_keywords]])){
				$tags = CatPath::explodeStrToKeywords($row[$field_keys[$url_t->field_keywords]]);
				$tags = '"'.implode('","',$tags).'"';
			}
			$vals = array();
			if (!isset($options['opt_nourl']))
				$vals['u'] = formatToJson($row[$field_keys[$url_t->field_url]]);
			if (!isset($options['opt_notitle']))
				$vals['d'] = formatToJson($row[$field_keys[$url_t->field_title]]);
			if (!isset($options['opt_notags']))
				$vals['t'] = '['.(isset($tags)?$tags:'').']';
			if (isset($options['opt_withdesc']))
				$vals['p'] = formatToJson($row[$field_keys[$url_t->field_description]]);
			
			if (isset($options['opt_withtime']))
				$vals['s'] = time($row[$field_keys[$url_t->field_time]]);
			
			if (isset($options['opt_withuser']) && isset($field_keys[$users_t->field_username]))
				$vals['w'] = $row[$field_keys[$users_t->field_username]];
			if (empty($vals))break;
			echo '{';
			$flag2 = false;
			foreach($vals as $k => $v){
				if ($flag2)echo ',';
				if ($k !== 't')
				echo '"',$k,'":"',$v,'"';
				else
				echo '"',$k,'":',$v;
				$flag2 = true;
			}
			echo '}';
		}while($row = db::fetch_row($resultSet));
		db::free_result($resultSet);
	}
	echo '];';

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