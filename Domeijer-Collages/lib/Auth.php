<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of Auth
 *
 * @author User
 */
class Auth {
   
   static private $current_user;
   static private $authorized = false;
   static private $salt = 'jadijadi';
   //731b8fb68eff92af19123b8f77b4996eff5c8f5a
   //1234vijf : 9031dfea1c97c5ea71a2582723bd950330e4acc8
   
   
   static public function AuthUser($username, $password) {
       if (self::isAuth()) {
           return true;
       }     
       $user = R::findOne('user', 'WHERE username = ?', array($username));
       
       if (!$user || $user->id < 1) {
           return false;
       }
       
       if ($user->password == sha1(sha1(self::$salt).$password)) {
           self::setUser($user, md5(rand(0, 9999999999)));
           return true;
          
       }
       
       return false;
   } 
   
   static public function isAuth() {
       if (self::$authorized == true) {
           return true;
       }
       if (isset($_COOKIE['dc_user'])) {
           $cookie = $_COOKIE['dc_user'];
           $user = R::findOne('user', 'WHERE cookie = ?', array($cookie));
           if ($user && $user->id > 0) {
               self::setUser($user, $cookie);
               return true;
           }
       }    
       return false;
   }
   
   static private function setUser($user, $cookie) {
        $user->cookie = $cookie;
        $user->last_login = date("Y-m-d H:m:s");
        R::store($user);
        self::$current_user = $user;
        self::$authorized = true;
        setcookie('dc_user', $cookie, time() + (86400 * 7), "/");       
   }
   
   static public function getUser() {
       if (self::isAuth() && self::$current_user != null) {
           return self::$current_user;
       }
       return false;
   }
   
   static public function logOut() {
        self::$current_user = null;
        self::$authorized = false;
        unset($_COOKIE['dc_user']);
        setcookie('dc_user', null, time()-3600, "/");       
   }
}

?>
