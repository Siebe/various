
Readme for CreativeCrowds Service Monitor Broker Scripts
by ~Siebe

cc_broker.pl and cc_broker.sh are scripts to be deployed on servers 
that run processes for the connection between verbeterdebuurt and 
different councils. It checks for running processes and open ports and
can be called by the Service Monitor website through port 48036 if the 
sent password string is correct.

Installation:

1. Make sure perl is installed on the server
2. Copy the cc_broker2.pl and cc_broker_conf.pl scripts to the 
   /opt/cc_broker/ directory
3. Copy the cc_broker.sh shell script to /etc/init.d/ (as root)
4. Edit the cc_broker_confg.pl script to disable or add services or 
   port lists.
5. Add the cc_broker.sh to the startup sequense: 
   $ sudo update-rc.d cc_broker.sh defaults
6. Start the broker manually:
   $ sudo /etc/init.d/cc_broker.sh
   (run as root since most services run as root as well)
7. For Apache check install a www-browser (any cli browser):
   $ sudo apt-get install lynx-cur
8. Make sure port 48036 is open for the server that runs the Service
   Monitor website
9. Add the server to the server list in Config.php of the Service 
   Monitor website
