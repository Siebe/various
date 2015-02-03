#!/usr/local/bin/perl

#		CreativeCrowds Service Monitor Broker Script II
#						by ~Siebe

#This is the config file for the CreativeCrowds Broker Script II
#It's a perl file with an hash defined in it, so check your syntax!

#By ~Siebe

%CFG = (

#The port and password which the website will connect with
#Defaults: 48036, 'haidihoow'
	'port' => 48036,
	'password' => 'haidihoow',
	
	
#Set any service that shouldn't be running on the server on 0
    'check_services' => {	
		'apache2' => 1,
		'java' => 1,
		'mysql'  => 1,
		'activemq' => 1,
		'mule' => 1,
		'postgresql' => 1
	},
	
#Comment out port checks if they're not supposed to be open on the server. 
#Do keep the 'check_ports' hash, empty if needed.
	 'check_ports' => {
		 #The key is the name of the service, followed by an array of port numbers
		 #Note: this will be outputed as "servcicename_ports", in this case: "apache2_ports"
		'apache2' => [80, 443],
		'java' => [8080, 8161, 61613, 61616],
		'picolisp' => [12321]
	},

#The commands, line number and regex checks for each service, add new ones here
#and add a new check to the 'check_servcies' hash.
#To disable a check use the 'check_services' hash.
	'services_commands' => {
		#The first key is the name of the service
		'apache2' => {
			#The cmd  has the command that should be ran
			'cmd' => 'apache2ctl status',
			#The out_index is the important line number of the output
			'out_index' => 11,
			#The out_regex contains a regular expression to check the line with
			'out_regex' => qr/^\s*Server uptime:/
		},
		'activemq' => {
			'cmd' => 'ps -p `cat /opt/activemq/data/*.pid 2>&1` 2>&1' ,
			'out_index' => 1,
			'out_regex' => qr/^\s*\d+\s([^\s]+\s+){2,3}(\/usr\/bin\/)?java/
		},
		'java' => {
			'cmd' => 'java -version 2>&1',
			'out_index' => 0,
			'out_regex' => qr/^\s*java version/
		},
		'mysql' => {
			'cmd' => 'service mysql status 2>&1',
			'out_index' => 0,
			'out_regex' => qr/^mysql start\/running, process/
		},
		'mule' => {
			'cmd' => '/opt/vbdbkoppeling/bin/mule status 2>&1',
			'out_index' => 1,
			'out_regex' => qr/^Mule is running \(\d+\)\.$/
		},
		'postgresql' => {
			'cmd' => 'service postgresql status 2>&1',
			'out_index' => 0,
			'out_regex' => qr/^\s*Running clusters:/
		}
	}
);
