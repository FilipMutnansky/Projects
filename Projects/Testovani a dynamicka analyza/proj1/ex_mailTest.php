<?php

require "ex_mail.php";

class MyMailTest extends PHPUnit_Framework_TestCase
{
    public function testSingleMailSent()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 1; // ['proforma']
        
        MyMail::pear_mail("to@example.com", "Subject text", "Nadpis x y", "body");
        
        $this->assertEquals(Mail_mime::$body, "body");
    }
    
    public function testEmptyNadpis()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 1; // ['proforma']
        
        MyMail::pear_mail("to@example.com", "Subject text", "", "body");
        
        $this->assertEquals(Mail_mime::$body, "body");
    }
    
    public function testHTML()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 1; // ['proforma']
        
        MyMail::pear_mail("to@example.com", "Subject text", "Nadpis x y", "body",0);
        
        $this->assertEquals(Mail_mime::$body, "body");
    }
    
    public function testMailFormatEmpty()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 1; // ['proforma']
        
        MyMail::pear_mail("", "Subject text", "Nadpis x y", "body");
        
        $this->assertTrue(PEAR::$error);
    }
    
    public function testMailFormatCorrect()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 1; // ['proforma']
        
        MyMail::pear_mail("to@example.com", "Subject text", "Nadpis x y", "body");
        
        $this->assertFalse(PEAR::$error);
    }
    
    public function testMailFormatFalse()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 1; // ['proforma']
        
        MyMail::pear_mail("toexamplecom", "Subject text", "Nadpis x y", "body");
        
        $this->assertTrue(PEAR::$error);
    }
    
    
    public function testDatabase()
    {
        global $CONFIG, $ERRORMSG;
        		
        $CONFIG['mail_odesilatel'] = 'from@example.com';
        $CONFIG['mail_server'] = 'smtp.example.com';
        $CONFIG['mail_server_name'] = 'smtp.example.com';
        $CONFIG['mail_server_pass'] = 's3cr3dp4ssw0rd';
        $CONFIG['mail_server_port'] = '25';
        $CONFIG['proforma_splatnost'] = '7';
        Db::$flag = 0; // ['proforma']
        
        MyMail::pear_mail("to@example.com", "Subject text", "Nadpis x y", "body");
    }
}
?>

