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

# Syntax:
# ./get_com_port.pl <DetAndId> <IDFile>

# 4 required arguments: <DetAndId> <IDFile> <com_port output file> <baud>. Argument counts from 0.
if ( $#ARGV < 3 )
{
    print "Incomplete arguments\n";
    exit;
}

my $detect_and_id = $ARGV[0];
my $id_file       = $ARGV[1];
my $com_port_file = $ARGV[2];
my $baud_rate     = $ARGV[3];
my $baud_rate_file = $ARGV[4];   # optional for autobaud

my $chip_id       = 0;
my $comp_port     = 0;
my $expected_str  = 0;
my $device_found  = 0;
my $id_file_line  = 0;
my $line          = 0;
my $detected_baud = 0;

print "$com_port_file\n";

# Open ID file and get the chip ID.
open INFILE, "<:raw", $id_file or die "cant open " . $id_file;
open OUTFILE, ">:raw", $com_port_file or die "cant open " . $com_port_file;

($id_file_line) = <INFILE>;

chomp($id_file_line);      # Remove trailing newline
$id_file_line =~ s/^\s+//; # Remove leading spaces
$id_file_line =~ s/\s+$//; # Remove trailing spaces

# Get chip ID (first word)
($chip_id) = $id_file_line =~ /\A([^:\s]+)/;

# Call DetectAndId to identify the board
$output = `$detect_and_id -TRAN UART -BAUDRATE $baud_rate -IDFILE $id_file`;


# Expected string
($expected_str) = qr/(.*\S)\s+HCI\s+$chip_id/;
# $1 will be table1, table2, table3. delim is a newline
my @tables = split(/\n/, $output);
foreach $line (@tables)
{
    chomp($line);      # Remove trailing newline
    $line =~ s/^\s+//; # Remove leading spaces
    $line =~ s/\s+$//; # Remove trailing spaces

    # Check if DetectAndId output contains the expected string
    if ( $line =~ $expected_str )
    {
        # Get COM port (first word)
        ($com_port) = $line =~ /\A([^:\s]+)/;

        # Dump COM port number into com_port_file
        print OUTFILE $com_port;

        $device_found = 1;

        # store baud rate if requested for AUTOBAUD
        if (($baud_rate =~ "AUTO") && (defined $baud_rate_file))
        {
            # Get returned baud rate (last word)
            ($detected_baud) = $line =~ /(\w+)$/;

            # put it to the baud_rate_file
            open BAUDFILE, ">:raw", $baud_rate_file or die "cant open " . $baud_rate_file;
            print BAUDFILE $detected_baud;
            close BAUDFILE;
        }
        last;
    } else {
    }
}

close INFILE;
close OUTFILE;

if ( $device_found == 0 )
{
    exit(1);
}
else
{
    exit(0);
}
