#!/usr/bin/perl
#
# Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
#
# This software, including source code, documentation and related
# materials ("Software") is owned by Cypress Semiconductor Corporation
# or one of its affiliates ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products.  Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
#

#use warnings;
#use strict;
my $param;
my @args;
my $arg_bdaddr;
my $bdaddr_setting;
my $bdaddr_mask;
my $device;
my $mac;

# This script precedes the "cgs" tool host MAC read and integration with the bdaddr,
# instead providing the -O DLConfigBD_ADDRBase:XXX as an override to "cgs".
# Putting this operation into a script allows a host-appropriate command line to be used to
# fetch the host MAC.
# This way the operation is visible and easily modifiable without recompiling the "cgs" tool.

# get command line from stdin, expecting btp file, device, and other -O params
# if -O DLConfigBD_ADDRBase:XXX is given, XXX should be "random", "default", or <12-digit hex or '*'>
foreach my $arg (@ARGV) {
    if($arg =~ /DLConfigBD_ADDRBase:(.*)/) {
        # this is the parameter we will modify if it is on the command line
        $bdaddr_setting = $1;
        # if mask is given on command line, let it override btp
        if($bdaddr_setting =~ /[\*]+/ && length($bdaddr_setting) == 12) {
            $bdaddr_mask = $bdaddr_setting;
            $bdaddr_setting = 'default';
        }
        $arg_bdaddr = $arg;
        push @args, $arg;
    }
    elsif($arg =~ /\.btp$/) {
        $btp_file = $arg;
    }
    elsif($arg =~ /[0-9]{5}[A-F][0-9]/) {
        $device = $arg;
    }
    else {
        push @args, $arg;
    }
}

# read in btp file and get DLConfigBD_ADDRBase string, unless override was on command line
if(!defined $bdaddr_mask && defined $btp_file) {
    open(my $BTP, "<", $btp_file) || die "Could not open *.btp file \"$btp_file\", $!";
    while(defined(my $line = <$BTP>)) {
        if($line =~ /DLConfigBD_ADDRBase\s*\=\s*(.*)$/) {
            $bdaddr_mask = $1;
            $bdaddr_mask =~ s/\s+$//;
            $bdaddr_mask =~ s/\"//g;
            $bdaddr_mask =~ s/\'//g;
            last;
        }
    }
    close $BTP;
    if(defined $device) {
        $bdaddr_mask =~ s/[0-9]{5}[A-F][0-9]/$device/;
    }
}

# Get bdaddr prefix from btp or device. If neither is on command line make up a device name.
if(!defined $bdaddr_mask) {
    $device = "12345A0" if !defined $device;
    $bdaddr_mask = "$device\*\*\*\*\*";
}

# if default or , use host MAC address as suffix (os dependent)
if($bdaddr_setting eq 'default') {
    if($^O =~ /(MSWin32|cygwin|msys|MINGW64)/) {
        my $ip = `ipconfig -all`;
        my @lines = split "\n", $ip;
        while(defined(my $line = shift @lines)) {
            next if $line !~ /(Ethernet\s*[0-9]?|Wi-Fi|Local Area Connection):/;
            while(defined(my $line = shift @lines)) {
                next if $line !~ /Physical Address/;
                if($line =~ /:\s+([0-9A-F\-]+)/) {
                    $mac = $1;
                    $mac =~ s/\-//g;
                    last;
                }
            }
        }
    }
    elsif($^O =~ /darwin/) {
        my $ip = `ifconfig`;
        my @lines = split "\n", $ip;
        foreach my $line (@lines) {
            if($line =~ /ether\s+([0-9a-fA-F\:]+)\s*$/) {
                $mac = $1;
                $mac =~ s/\://g;
                last;
            }
        }
    }
    else { # assume linux
        my $ip = `ifconfig`;
        my @lines = split "\n", $ip;
        foreach my $line (@lines) {
            if($line =~ /(HWaddr|ether)\s+([0-9a-fA-F\:]+)/) {
                $mac = $2;
                $mac =~ s/\://g;
                last;
            }
        }
    }
    warn "could not determine host mac on which to base bluetooth mac address $^O\n" if !defined $mac;
    $mac = "000000000000" if !defined $mac;
}
elsif($bdaddr_setting eq 'random') {
    # prepare 12-digits random hex to mask with * in DLConfigBD_ADDRBase
    $mac = sprintf '%04X%04X%04X%04X%04X%04X',
                    int(rand(32768)), int(rand(32768)), int(rand(32768)),
                    int(rand(32768)), int(rand(32768)), int(rand(32768));
}

if(defined $mac) {
    # replace '*' in mask with corresponding mac digit, typically last 5
    $bdaddr_setting = "";
    #warn "mac: $mac\n";
    for(my $i = 0; $i < 12; $i++) {
        if(substr($bdaddr_mask, $i, 1) eq '*') {
            $bdaddr_setting .= substr($mac, $i, 1);
        }
        else {
            $bdaddr_setting .= substr($bdaddr_mask, $i, 1);
        }
    }
}

if(!@args) {
    push @args, "-O";
    push @args, "DLConfigBD_ADDRBase:$bdaddr_setting";
}

# return command line on stdout
my $did_it;
foreach my $arg (@args) {
    if($arg =~ /DLConfigBD_ADDRBase:/) {
        print "DLConfigBD_ADDRBase:$bdaddr_setting ";
        $did_it = 1;
    }
    else {
        print "$arg ";
    }
}
print "-O DLConfigBD_ADDRBase:$bdaddr_setting" if !defined $did_it;
