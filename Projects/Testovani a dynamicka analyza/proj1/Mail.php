<?php
/* Pripraveno pro test doubles. */
/* Primo vyzadovano testovanym souborem. */
class Mail
{
	public function factory($name, $array)
    {
		return new Mail;
	}	
	
	public function send($to, $headers, $body)
    {
		if (!preg_match('/^.+@.+\..+$/',$to))
			PEAR::$error = true;
			else
				PEAR::$error = false;
		
		return new Mail;
	}
	
	public function getMessage()
    {
		return '';
	}
}

class PEAR
{
	public static $error;
	
	public function iserror($parameter)
    {
		if (PEAR::$error == true)
			return PEAR::$error;
	}	
}

?>
