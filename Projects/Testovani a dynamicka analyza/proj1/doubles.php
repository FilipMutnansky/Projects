<?php
/* Pripraveno pro test doubles. */
/* Vyzadovano bootstrap.php jako vychozi zavislosti. */
class Db
{
	public static $proforma = array();
	public static $flag;	
	
	public function DbQuery($dotaz)
    {
		Db::$proforma['proforma'] = Db::$flag;
		return Db::$proforma;
	}	
	
}


?>
