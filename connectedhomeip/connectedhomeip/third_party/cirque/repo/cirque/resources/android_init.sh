#!/bin/bash

ip_addr=`ifconfig eth0 | awk '/inet/ {print $2}'`
base_ipaddr=`echo $ip_addr | cut -d"." -f1-3`
network=$base_ipaddr.0

ip tuntap add dev tap0 mode tap user $(whoami)
ip link add br0 type bridge
ip link set tap0 master br0
ip link set eth0 master br0
ip addr flush dev eth0
ip link set tap0 up
ip link set br0 up
ifconfig eth0 0.0.0.0 promisc
ifconfig tap0 0.0.0.0 promisc
ip addr add $ip_addr/24 dev br0
route add default gw $base_ipaddr.1 br0
sysctl -w net.ip4.ip_forward=1

cat <<EOT >> /etc/dnsmasq.conf
interface=br0
bind-interfaces
dhcp-option=3,$base_ipaddr.1
dhcp-option=6,8.8.8.8,8.8.4.4
dhcp-range=$base_ipaddr.5,$base_ipaddr.9,255.255.255.0,12h
no-hosts
EOT

/etc/init.d/dnsmasq restart

emulator -avd emulator -no-boot-anim -no-audio -net-tap tap0 -wipe-data -accel on -gpu off -skin 1080x1920

sleep infinity
