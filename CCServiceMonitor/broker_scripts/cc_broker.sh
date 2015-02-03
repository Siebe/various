#!/bin/sh

#		CreativeCrowds Service Monitor Broker Script II
#						by ~Siebe

# Shell script to start up the CreativeCrowds Service Monitor Broker
# Script. Put this in /etc/init.d and add this as start up script:
# $ sudo update-rc.d cc_broker.sh defaults

/usr/bin/perl /opt/cc_broker/cc_broker2.pl >> /opt/cc_broker/cc_broker.log 2>&1 &
