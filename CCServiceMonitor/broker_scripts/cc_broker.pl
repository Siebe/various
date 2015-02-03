#!/usr/local/bin/perl

#		CreativeCrowds Service Monitor Broker Script
#						by ~Siebe

# This script is used to check processes and ports for services of 
# CreativeCrowds, mainly those for the interface between verbeterdebuurt
# and different councils. It has a number of checks build as subs which
# can be deactivated by commenting out. The script keeps a socket open
# (default port 48036) for calls from the CreativeCrowds Service 
# Monitor website and answers with simple strings if the provided 
# password is correct.
# Put this script on the server in /opt/cc_broker/ and use cc_broker.sh
# to start up the script.

use strict;
use IO::Socket;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use POSIX qw(strftime);

#The listen port and password
my $listen_port = 48036;
my $password = md5_hex("haidihoow");

my ($client_address, $client_port);
my ($data_in, $data_out);

# Open a listening socket
our $client_socket;
our $socket = IO::Socket::INET->new(Listen => 5,
	LocalPort => $listen_port,
	Proto => 'tcp');
die "cannot create socket $!\n" unless $socket;
print "server waiting for client connection on port $listen_port\n";

# Shutdown function
$SIG{INT} = \&siginthandler; 

# Main loop
while(1)
{
    # waiting for a new client connection
    $client_socket = $socket->accept();
	my $date = strftime "%d %B %Y %H:%M", localtime;
	
    # get information about a newly connected client
    $client_address = $client_socket->peerhost();
    $client_port = $client_socket->peerport();
    print "-----------------------------------------------------\n
		connection from $client_address:$client_port on $date\n";
 
    # read up to 1024 characters from the connected client
    # deny if password is incorrect
    $data_in = "";
    $client_socket->recv($data_in, 1024);
    if ($data_in ne $password) {
		$data_out = "No permission!";
	} else {
		
		# These are all the checks, if certain software is not installed
		# on the server, comment it out. The check***Ports() functions 
		# may also be adapted. The printed lines should correlate with
		# statements of the website.
		$data_out = "server online\n";
		$data_out .= "mysql: ".checkMySQL()."\n";
		$data_out .= "javaports: ".checkJavaPorts()."\n";
		$data_out .= "apache2: ".checkApache2()."\n";
		$data_out .= "apache2ports: ".checkApache2Ports()."\n";
		$data_out .= "activemq: ".checkActiveMQPID()."\n";
		$data_out .= "mule: ".checkMule()."\n";
		$data_out .= "java: ".checkJava()."\n";
		$data_out .= "postgresql: ".checkPostgresql()."\n";

	}
	
	
	print "Return to client:\n$data_out";
    # write response data to the connected client
    $client_socket->send($data_out);
 
    # notify client that response has been sent
	shutdown($client_socket, 1);
}


# Check wether Java is installed and JVM is working
sub checkJava {
	
	my @java_response = `java -version 2>&1`;
	if (!$java_response[0]) {
		return 0;
	}
	if ($java_response[0] =~ m/^\s*java version/) {
		return 1;
	}
	return 0;
}


# Check wether mysql is running
sub checkMySQL {
	
	my @mysql_response = `service mysql status`;	
	if (!$mysql_response[0]) {
		return 0
	}	
	if ($mysql_response[0] =~ m/^mysql start\/running, process/) {
		return 1
	}	
	return 0;
}


# Check wether activemq is running
sub checkActiveMQPID {
	
	# First retrieve activemq pid file
	my $mq_dir = shift;
	if(!$mq_dir) {
		$mq_dir = "/opt/activemq/data/";
	}	
	if (!(-d $mq_dir)) {
		print "ActiveMQ data directory $mq_dir not found";
		return 0;
	}
	opendir DIR, $mq_dir;
	my $pid_file = 0;
	while(my $filename=readdir(DIR)){
		if($filename =~ m/\.pid$/) {
			$pid_file = $filename;
			last;
		}
	}
	close DIR;	
	if (!$pid_file && !(-f $pid_file)) {
		print "ActiveMQ pid file not found\n";
		return 0;
	}			

	#cat the contents of the file to ge pid number
	my $pid_location = $mq_dir.$pid_file;
	my $pid = `cat $pid_location`; 
	if (!$pid =~ /^\d+$/) {
		print "ActiveMQ last pid not found\n";
		return 0;	
	}
	
	# Check the pid number with a null kill command
	my $running = kill 0, $pid;
	return $running;
	
}


# Check wether mule is running
sub checkMule {
	
	my $mule_dir = shift;
	if (!$mule_dir) {
		$mule_dir = "/opt/vbdbkoppeling"
	}
	my @mule_response = `$mule_dir/bin/mule status`;
	
	if (!$mule_response[1]) {
		return 0
	}
	if ($mule_response[1] =~ m/^Mule is running \(\d+\)\.$/) {
		return 1
	}	
	return 0
	
}


# Check wether apache2 is running
sub checkApache2 {
	my @apache_response = `apache2ctl status`;

	if (!$apache_response[1]) {
		return 0;
	}
	
	foreach my $resp_line (@apache_response) {
		if ($resp_line =~ m/^\s*Server uptime:/) {
			return 1;
		}
	}
	return 0;
}


# Check wether postgresql is running
sub checkPostgresql {
	my @postgresql_response = `service postgresql status`;
	
	if (!$postgresql_response[0]) {
		return 0;
	}
	foreach my $resp_line (@postgresql_response) {
		if ($resp_line =~ m/^\s*Running clusters:/) {
			return 1;
		}
	}
	
	return 0;	
}


# Check the ports used by our Java software (mule & activemq)
# Any ports that don't need to be checked can be commented out
sub checkJavaPorts {
	my @java_ports = getPortsOfProcess("java");
	if ("61613" ~~ @java_ports &&
		"61616" ~~ @java_ports &&
		"8080" ~~ @java_ports &&
		"8081" ~~ @java_ports &&
		"8161" ~~ @java_ports) {
		return 1
	}
	return 0
}


# Check the ports used by apache2 (http and https)
sub checkApache2Ports {
	my @ports = getPortsOfProcess();
	if ("80" ~~ @ports &&
		"443" ~~ @ports) {
		return 1;
	}
	return 0
}


# General ports per process function
sub getPortsOfProcess {
	
	my $process_name = shift;
	my @portlist = `netstat -tulpn`;
	
	if (!@portlist) {
		print "netstat command failed\n";
	}

	my @process_portlist;
	for my $port (@portlist) {
		if (($port =~ m/\s+\d+\/$process_name\s*$/) || 	
			!$process_name || $process_name eq '') {
				
			$port =~ m/^\w{3}\d?\s+\d+\s+\d+\s+(?P<address>[^\s]+)\s+/;			 
			$+{address} =~ m/(:|\.|\d)+:(?P<process_port>\d+)/;
			push @process_portlist, $+{process_port};
		}
	}
	return @process_portlist;
	
}
	

# (somewhat) graceful shutdown SIG
sub siginthandler {
	print "\nShutting down, closing socket\n";
	shutdown($client_socket, 2);
	$socket->shutdown(2);
	close($client_socket);
	close($socket);
	exit;
}



