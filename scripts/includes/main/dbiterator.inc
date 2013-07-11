<?php
class DBIterator implements Iterator{
	private $resultSet;
	private $next = null;
	private $type;
	private $counter = 0;
	private $_nextFlag = false;
	const TYPE_ASSOC = 1;
	const TYPE_ARRAY = 2;
	const TYPE_ROW = 3;
	/**
	 * @param mysql_result $resultSet resultSet gotten from mysql_query
	 * @param int $type type of row to get (i.e. array, assoc, row)
	 */
	public function __construct($resultSet,$type){
		$this->resultSet = $resultSet;
		$this->type = $type;
		$this->next();	//move to first element
	}
	public function setType($type){
		$this->type = $type;
	}
	public function getCount(){
		return $this->counter;
	}
	//there is on support for rewinding
	public function rewind(){
		static $rewind = 0;
		++$rewind;
	}
	public function current() {
		$this->_nextFlag = false;
		return $this->next;
	}
	public function key() {
		static $key = -1;
		return ++$key;
	}
	public function next(){
		//make sure we didn't skip any elements
		if ($this->_nextFlag)
			return ($this->next != false);
		$this->_nextFlag = true;
		if ($this->type == self::TYPE_ROW)
			$this->next = db::fetch_row($this->resultSet);
		elseif($this->type == self::TYPE_ASSOC)
			$this->next = db::fetch_assoc($this->resultSet);
		elseif($this->type == self::TYPE_ARRAY)
			$this->next = db::fetch_array($this->resultSet);
		//do not return the actual object, but simply return a boolean
		if ($this->next){
			$this->counter++;
			return true;
		}else{
			return false;
		}
	}

   public function valid() {
       return ($this->next != false);
   }
}
?>