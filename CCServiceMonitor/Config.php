<?php

/*

Config values for the CreativeCrowds Service Monitor Website
by ~Siebe
 */


class Config {
    
    // port that the brokers use, default 48036
    const port = 48036;
    // password that the brokers use, default "haidihoow"
    const password = "";
    
    //List of servers to check. 
    //Each server consists of an array with first element ip-address
    //and second element the name as shown on the site.
    public static $servers = array(
        array("127.0.0.1", "Lokaal (VDB + VDB Staging)"),
        array("46.105.30.4", "ESB"),
        array("46.105.30.38", "ESB Staging"),
        array("176.31.186.218", "ActiveMQ")
         
    );
            
    //List of services that can be returned by the broker script.
    //When adding a new service or portlist to the broker script make sure it's
    //defined here as well, using the same name. Each service has the service 
    //name as key and a 2 element array with the fail output as first and 
    //succes output as second element.
    //There's no need to disable services here, if the broker doesn't return 
    //anything the service is ignored.
    //Note: the port checks will have '_ports' appended by the broker script
    public static $services = array(
        'apache2' => array("Apache 2 is NOT running", "Apache 2 is running"),
        'mysql' => array("MySQL is NOT running", "MySQL is running"),
        'java' => array("Java is NOT installed", "Java is installed"),
        'mule' => array("Mule is NOT running", "Mule is running"),
        'activemq' => array("ActiveMQ is NOT running", "ActiveMQ is running"),
        'postgresql' => array("PostGresQL is NOT running", "PostGresQL is running"),
        
        'java_ports' => array("Java ports are CLOSED", "Java ports are open"),
        'apache2_ports' => array("HTTP(s) ports are CLOSED", "HTTP(s) ports are open"),
        'picolisp_ports' => array("PicoLisp port is CLOSED", "PicoLisp port is open")
        
    );
    
    
}
