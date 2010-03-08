#!/bin/sh

# generate SSH host keys

hostKeyDir=/boot/home/config/etc

if [ ! -f "$hostKeyDir/ssh_host_key" ] ; then
	ssh-keygen -t rsa1 -f "$hostKeyDir/ssh_host_key" -N ""
fi

if [ ! -f "$hostKeyDir/ssh_host_dsa_key" ] ; then
	ssh-keygen -t dsa -f "$hostKeyDir/ssh_host_dsa_key" -N ""
fi

if [ ! -f "$hostKeyDir/ssh_host_rsa_key" ] ; then
	ssh-keygen -t rsa -f "$hostKeyDir/ssh_host_rsa_key" -N ""
fi
