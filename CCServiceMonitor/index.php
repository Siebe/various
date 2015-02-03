<!DOCTYPE html>

<!--

/****************************************
*   _______ _________________ __ ____   *
*  //  || \\|  // \\ || ||||| ||||      *
* ((   ||_//|==||=|| ||  ||\\ //||==    *
*  \\__|| \\|__|| || ||  || \V/ ||___   *
*     _______  ___ __    ______ __      *
*    //  || \\// \\||    |||| \(( \     *
*   ((   ||_/((   ))\ /\ //||  ))\      *
*    \\__|| \\\\_// \V/\V/ ||_/|_))     *
*                                       *
*  ######## CreativeCrowds ##########   *
*                                       *
* Haarlemmerweg 10c                     *
* 1014 BE Amsterdam                     *
* info@creativecrowds.com               *
* http:://creativecrowds.com            *
*                                       *
*****************************************/

      CreativeCrowds Service Monitor
               by ~Siebe

This is a webpage that connects with different servers of CreativeCrowds to 
check whether processes are running and ports are open for services used to
connect verbeterdebuurt with different councils. The servers must have the 
perl script cc_broker.pl running in order to connect.
Multiple servers can be added here.

-->

<?php
foreach (glob("*.php") as $include)
{
    include_once $include;
}
?>


<html>
    <head>
        <meta charset="UTF-8">
        <title>CC Service Monitor</title>
        <link rel="stylesheet" type="text/css" href="screen.css?v=1">
        <link rel="icon" href="cc_favicon.png">
    </head>
    <body>     
        <div class="wrapper">
            <img class="logo" src="cc_logo.png">
            <p class="title">Service Monitor</p></br>
            <div class="functions_wrap">
                <p class="functions" id="refresh-status">Refresh disabled</p>
                <p class="functions" id="sound-toggle">Sound disabled</p></br>
            </div>
            <?php
            
            foreach (Config::$servers as $server) {
                runBroker($server[0], $server[1]);
            }

            ?>
            </br></br>
            <p class="footer">
                &copy;CreativeCrowds all rights reserved 2014
            </p>
        </div>
        
        <div id="sound"></div>
        
    </body>
</html>


<?php
function runBroker($ip, $name) {
        try {
            $broker = new BrokerConnection($ip, $name);
            echo $broker->checkNice();
            $broker->close();
        } catch (Exception $e) {
            echo '<table class="status">'.
                '<tr><td><p class="servername"><b>'.$name.'</b></p><p class="ip_address">'.$ip.'</p></td></tr>'.
                '<tr><td><p class="status_fail"><b>Could not connect</b>: '.$e->getMessage().'</p></td></tr></table></br>';
        }    
}
?>

<script>

    /*  
    Sound alert variables.
    */
    var failed = document.getElementsByClassName('status_fail');  
    var alarm_toggle = getCookie("alarm_toggle", 0)
    var alarm_enabled_message = "Alarm is enabled - <a href=# onclick='disableSound()'>Turn off alarm</a>"
    var alarm_disabled_message = "Alarm is disabled - <a href=# onclick='enableSound()'>Turn on alarm</a> (needs auto-refresh)"
      
    if (alarm_toggle == 1) {
        document.getElementById("sound-toggle").innerHTML= alarm_enabled_message;
        if (failed != null) {
            playSound('alert');
        }
    } else {
        document.getElementById("sound-toggle").innerHTML= alarm_disabled_message;
    }
    
    /*
    Auto-refresh variabels  
    enter refresh time in "minutes:seconds" Minutes should range from 0 to inifinity. Seconds should range from 0 to 59
    */
    var limit="2:00"
    var refresh_toggle = getCookie("refresh_toggle", 0);
    
    var disable_message = " - <a href=# onclick='disablerefresh()' >disable</a> or <a href='/'>refresh now</a>"
    var enable_message = "Auto-refresh is disabled - <a href=# onclick='enablerefresh()'>enable</a> or <a href='/'>refresh now</a>"
    
    var parselimit=limit.split(":")
    parselimit=parselimit[0]*60+parselimit[1]*1
       
    window.onload=beginrefresh();
    
    /*
     Sound alert functions
     */
      
    function playSound(filename){   
        document.getElementById("sound").innerHTML='<audio autoplay="autoplay"><source src="' + filename + '.mp3" type="audio/mpeg" /><source src="' + filename + '.ogg" type="audio/ogg" /><embed hidden="true" autostart="true" loop="false" src="' + filename +'.mp3" /></audio>';
    }
    
    function disableSound() {
        setCookie("alarm_toggle", 0);
        document.getElementById("sound").innerHTML='';
        document.getElementById("sound-toggle").innerHTML= alarm_disabled_message;
    }
    
    function enableSound() {
        setCookie("alarm_toggle", 1);
        if (refresh_toggle == 0) {
            enablerefresh();
        }
        document.getElementById("sound-toggle").innerHTML= alarm_enabled_message;   
    }      
    /*
    Auto Refresh Page with Time script
    By JavaScript Kit (javascriptkit.com)
    Over 200+ free scripts here!
    */

    
    function beginrefresh(){
  
        if (refresh_toggle == 0) {
            document.getElementById("refresh-status").innerHTML=enable_message
            return
        }
            
        if (!document.images)
            return
        if (parselimit==1)
            window.location.reload()
        else{ 
            parselimit-=1
            curmin=Math.floor(parselimit/60)
            cursec=parselimit%60
            if (curmin!=0)
                curtime="Auto-refresh in "+curmin+" minutes and "+cursec+" seconds"+disable_message
            else
                curtime="Auto-refresh in "+cursec+" seconds"+disable_message
            window.status=curtime
            document.getElementById("refresh-status").innerHTML=curtime
            setTimeout("beginrefresh()",1000)
        }
    }

    
    function disablerefresh() {
        refresh_toggle = 0
        setCookie("refresh_toggle", 0, 365)
    }
    
    function enablerefresh() {
        refresh_toggle = 1  
        setCookie("refresh_toggle", 1, 365)
        beginrefresh()
    }
    
    /*Cookie functions*/
    
    function setCookie(cname,cvalue,exdays)
    {
        var d = new Date();
        d.setTime(d.getTime()+(exdays*24*60*60*1000));
        var expires = "expires="+d.toGMTString();
        document.cookie = cname + "=" + cvalue + "; " + expires;
    } 
    
    function getCookie(cname,defval)
    {
        var name = cname + "=";
        var ca = document.cookie.split(';');
        for(var i=0; i<ca.length; i++)
          {
          var c = ca[i].trim();
          if (c.indexOf(name)==0) return c.substring(name.length,c.length);
          }
        return defval;
    } 
    
    

</script>