<?php
require_once($_SERVER['DOCUMENT_ROOT'] . "/scripts/includes/interface.inc");
$_SERVER['DOCUMENT_ROOT'] = preg_replace('-.tools.*$-','',__FILE__);
	require_once($_SERVER['DOCUMENT_ROOT'] . "/scripts/includes/DBconnect.inc");
	db::connect();
	$outputFile = $_SERVER['DOCUMENT_ROOT'] . "/scripts/includes/tables.inc";
	$result = db::query("SHOW TABLES");
	//iterate through tables
	$out1 = "<?php\n";
	$out = <<<EOD
class sql_table{
	function __construct(\$prepandTable = false,\$table_name = false){
		if (\$table_name !== false)
			\$this->name = \$table_name;
		if (\$prepandTable){
			foreach(\$this as \$key => \$value) {
           		\$this->\$key = \$this->name.'.'.\$value;
       		}
		}
	}
	public function allFields(){
		\$ar = array();
		foreach(\$this as \$k => \$value) {
			if (!preg_match('-(?:_length$)|(?:^name$)-',\$k)){
           		\$ar[] = \$value;
			}
       	}
       	return \$ar;
	}
}
EOD;
	while ($row = db::fetch_row($result)){
		$tablename = $row[0];
		$out1 .= "define(\"TABLE_" . strtoupper($tablename) . "\",\"" . $tablename . "\");\n";
		$out .= "\nclass table_" . $tablename . " extends sql_table{\n";
		$result2 = db::query("describe " . $tablename);
		while ($row2 = db::fetch_row($result2)){
			$fieldname = $row2[0];
			$out .= "\tpublic \$field_" . $fieldname . " = \"" . $fieldname . "\";\n";
			$out .= "\tconst FIELD_" . strtoupper($fieldname) . " = \"" . $fieldname . "\";\n";
			$type = $row2[1];
			if (!eregi('int',$type) && eregi('^.+\([0-9]+\)$',$type)){
				$out .= "\tpublic \$field_" . $fieldname . "_length = " . ereg_replace('^.+\(([0-9]+)\)$','\\1',$type) . ";\n";
				$out .= "\tconst ".strtoupper($fieldname) . "_LENGTH = " . ereg_replace('^.+\(([0-9]+)\)$','\\1',$type) . ";\n";
			}
		}
		$out .= "\tpublic \$name = \"$tablename\";\n";
		$out .= "}\n";
	}
	$out .= "?>";
	//writing output 
	@ $fp = fopen($outputFile,'w');
	if ($fp){
		fwrite($fp,$out1,strlen($out1));
		fwrite($fp,$out,strlen($out));
		fclose($fp);
	}
	db::close();
?>