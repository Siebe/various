<?php

/*

CreativeCrowds Service Monitor BrokerConnection class.
This class connects with a broker and processes it's output. It can return
the answer in a tidy html table.
by ~Siebe

 */



class BrokerConnection {
    
    private $sock;
    private $name;
    private $ip;
    
    //initialize a socket connection with a broker
    public function BrokerConnection($ip, $name = null) {
        $this->name = $name;
        $this->ip = $ip;
        if (($this->sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
            throw new Exception("socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n");
        }
        //set 5 seconds timeout
        socket_set_option($this->sock, SOL_SOCKET, SO_RCVTIMEO, array('sec' => 5, 'usec' => 0));
        if (socket_connect($this->sock, $ip, Config::port) === false) {
            throw new Exception("socket_connect() failed: reason: " . socket_strerror(socket_last_error($this->sock)) . "\n");
        }
        
    }
    
    
    //get the answer from the broker, just return as string
    public function check() {
        $result = false;
        $in = md5(Config::password);       
        socket_write($this->sock, $in, strlen($in));
        while ($out = socket_read($this->sock, 2048)) {
            $result .= $out;
        }        
        return $result;
    }
    
    
    //get the answer from the broker and process it, return as html table
    public function checkNice() {
        try {
            $result = $this->check();
        } catch(Exception $e) {
            throw $e;
        }
        $output = '<table class="status">';
        if ($this->name != null) {
            $output .= '<tr><td><p class="servername"><b>'.$this->name.'</b></p><p class="ip_address">'.$this->ip.'</p></td></tr>';
        }
        if ($result) {          
            $lines = split("\n", $result);
            foreach ($lines as $line) { 
                
                if ($line == "server online") {
                    $output .= '<tr><td> <p class="status_ok">Server is online</p></td></tr>';
                    continue;
                }

                //for each line check the list of services
                foreach (Config::$services as $service => $service_output) {

                    
                   if ($line == "$service: 0") {
                       $output .= '<tr><td> <p class="status_fail">'.$service_output[0].'</p></td></tr>';
                       //break; //break from services loop
                   } 
                   if ($line == "$service: 1") {
                        $output .= '<tr><td> <p class="status_ok">'.$service_output[1].'</p></td></tr>';
                        //break; //break from services loop
                   }
                    
                }
            }
            
            
        } else {
            $output = '<tr><td> <p class="status_fail>Server is not responding, check broker script</p> </td></tr>';
        }
        $output .= "</table></br>";

        return $output;
        
    }
    
    
    //close socket
    public function close() {
        socket_close($this->sock);
    }
    
    
    public function getName() {
        return $this->name;
    }
    
}
