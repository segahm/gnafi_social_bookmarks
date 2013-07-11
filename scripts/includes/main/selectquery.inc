<?php
class SelectQuery{
	private $fields = array();
	private $fields_size = 0;
	private $tables = array();
	private $table_size = 0;
	private $joins = array();
	private $limitCond;
	private $resultStr;
	private $whereCond = array();
	public $invalidate = false;
	private $orderby = array();
	function __construct($limit,$start = 0){
		$this->limitCond = "$start, $limit";
	}
	public function compile(){
		if ($this->invalidate)return null;
		$tempJoins = array();
		$joins_size = 0;
		foreach($this->joins as $t => $c){
			$tempJoins[] = $t.' ON '.$c;
			++$joins_size;
		}
		$this->resultStr = 'SELECT '.implode(',',array_keys($this->fields)).' FROM '
			.str_repeat('(',$joins_size).implode(',',$this->tables)
			.(!empty($this->joins)?' LEFT JOIN '.implode(') LEFT JOIN ',$tempJoins).')':'')
			.(!empty($this->whereCond)?' WHERE '.implode(' AND ',$this->whereCond):'')
			.(!empty($this->orderby)?'ORDER BY '.implode(',',array_keys($this->orderby)):'')
			.' LIMIT '.$this->limitCond;
		return $this->resultStr;	
	}
	public function addWhere($cond){
		$this->whereCond[] = $cond;
	}
	public function addField($field,$ns = null){
		if ($ns == null)$ns = $field;
		else $ns = "$ns.$field";
		$this->fields[$ns] = $this->fields_size;
		++$this->fields_size;
	}
	public function addFields($fields,$ns = null){
		if ($ns == null)$ns = '';
		else $ns = "$ns.";
		foreach($fields as $field){
			$this->fields[$ns.$field] = $this->fields_size;
			++$this->fields_size;
		}
	}
	public function removeField($field,$ns = null){
		if ($ns == null)$ns = '';
		else $ns = "$ns.";
		if (isset($this->fields[$ns.$field])){
			unset($this->fields[$ns.$field]);
			--$this->fields_size;
		}else{
			log_writeLog('removeField: field does not exist');
		}
	}
	public function addTable($table){
		$tables[$table] = $this->table_size;
		++$this->table_size;
	}
	public function removeTable($table){
		if (isset($this->fields[$table])){
			unset($this->fields[$table]);
			--$this->table_size;
		}else{
			log_writeLog('removeTable: table does not exist');
		}
	}
	public function join($table,$cond){
		$this->joins[$table] = $cond;
	}
	public function removeJoin($table){
		unset($this->joins[$table]);
	}
	public function orderBy($field){
		$this->orderby[$field] = 1;
	}
}
?>