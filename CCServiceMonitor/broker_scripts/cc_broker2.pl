#!/usr/local/bin/perl

#		CreativeCrowds Service Monitor Broker Script II
#						by ~Siebe

# This script is used to check processes and ports for services of 
# CreativeCrowds, mainly those for the interface between verbeterdebuurt
# and different councils.
# Check the cc_broker_conf.pl file for configuration options.
# Put this script and the config file on the server in 
# /opt/cc_broker/ and use cc_broker.sh to start up the script.

use strict;
use IO::Socket;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use POSIX qw(strftime);
use Getopt::Long;
use Data::Dumper;

my ($client_address, $client_port);
my ($data_in, $data_out);

my $config_file = $ARGV[0];

if ($config_file =~ m/^-{0,2}help$/) {
	print "help here\n";
} 

$config_file = "cc_broker_conf.pl" if (!(-e $config_file));

# Get our configuration information, becomes variable with own namespace CFG::CFG
if (my $err = ReadCfg($config_file)) {
    print(STDERR $err, "\n");
    exit(1);
}

my $listen_port = $CFG::CFG{'port'};
my $password = $CFG::CFG{'password'};

# Open a listening socket
our $client_socket;
our $socket = IO::Socket::INET->new(Listen => 5,
	LocalPort => $listen_port,
	Proto => 'tcp');
die "cannot create socket $!\n" unless $socket;
print "server waiting for client connection on port $listen_port\n";

# Shutdown function
$SIG{INT} = \&siginthandler; 


while(1)
# Main loop
{
    # waiting for a new client connection
    $client_socket = $socket->accept();
	my $date = strftime "%d %B %Y %H:%M", localtime;
	
    # get information about a newly connected client
    $client_address = $client_socket->peerhost();
    $client_port = $client_socket->peerport();
    print "-----------------------------------------------------\n".
		"connection from $client_address:$client_port on $date\n";
 
    # read up to 1024 characters from the connected client
    # deny if password is incorrect
    $data_in = "";
    $client_socket->recv($data_in, 1024);
    if ($data_in ne md5_hex($password)) {
		print "connection denied\n";
		$data_out = "No permission!\n";
	} else {
		print "connection accepted\n";
	    $data_out = "server online\n";
		$data_out .= runServices();
		$data_out .= runPorts();
	}
	
	
	print "Return to client:\n$data_out";
    # write response data to the connected client
    $client_socket->send($data_out);
 
    # notify client that response has been sent
	shutdown($client_socket, 1);
}


#----Functions:

sub runServices 
#run all the service checks and assemble the output
{
	my $output;
	
	for my $service_name (keys ($CFG::CFG{'services_commands'})) {
		if ($CFG::CFG{'check_services'}{$service_name} == 1) {
			my @service = $CFG::CFG{'services_commands'}{$service_name};			
			my $result = checkService(@service);
			$output .= "$service_name: $result\n";
		}
	}
	
	return $output;
}


sub runPorts 
#run all the port checks and assemble the output
{
	my $output; 

	for my $process_name (keys $CFG::CFG{'check_ports'}) {
		my @ports = $CFG::CFG{'check_ports'}{$process_name};
		my $result = checkPorts($process_name, @ports);
		$output .= "$process_name"."_ports: $result\n";
	}
	
	return $output;
}
	

sub checkService 
#check if a service is running using a command defined in config file
{
	my @service = @_;
   
	my $cmd = $service[0]{'cmd'}; 
	my $out_index = $service[0]{'out_index'}; 
	my $out_regex = $service[0]{'out_regex'};

	my @cmd_output = `$cmd`;
	
	if ($cmd_output[$out_index] =~ $out_regex) {
		return 1;
	}
	
	return 0;
	
}


sub checkPorts 
#check wether a list of ports is used by a service defined in config file
{
	my $process_name = shift;
	my @ports = @{$_[0]};
	my $port_count = scalar @ports;
		
	my @portlist = `netstat -tulpn`;
	
	print "ports: ".Dumper @ports;
	print "portlist: ".Dumper @portlist;
	
	if (!@portlist) {
		print "netstat command failed\n";
	}

	my $found_count = 0;	
	for my $port (@portlist) {
		if (($port =~ m/\s+\d+\/$process_name\s*$/) || 	
			!$process_name || $process_name eq '') {
				
			$port =~ m/^\w{3}\d?\s+\d+\s+\d+\s+(?P<address>[^\s]+)\s+/;			 
			$+{address} =~ m/(:|\.|\d)+:(?P<process_port>\d+)/;
			$found_count++ if ($+{process_port} ~~ @ports);
		}
	}
	
	if ($found_count == $port_count) {
		return 1;
	} else {
		return 0;
	}
}


sub ReadCfg
#http://www.perlmonks.org/?node_id=464358
{
    my $file = $_[0];

    our $err;

    {   # Put config data into a separate namespace
        package CFG;

        # Process the contents of the config file
        my $rc = do($file);

        # Check for errors
        if ($@) {
            $::err = "ERROR: Failure compiling '$file' - $@";
        } elsif (! defined($rc)) {
            $::err = "ERROR: Failure reading '$file' - $!";
        } elsif (! $rc) {
            $::err = "ERROR: Failure processing '$file'";
        }
    }

    return ($err);
}


sub siginthandler 
# (somewhat) graceful shutdown SIG
{
	print "\nShutting down, closing socket\n";
	shutdown($client_socket, 2);
	$socket->shutdown(2);
	close($client_socket);
	close($socket);
	exit;
}
