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

use warnings;
use strict;
use bigint qw/hex/;
use File::Basename;

my $I1 = ' ' x 4;
my $I2 = $I1 x 2;
my $I3 = $I1 x 3;

main();

sub main
{
    my $config_file;
    my $hdf_in_file;
    my $hdf_out_file;
    my $cgs_out_file;
    my @bt_addr;

    foreach my $arg (@ARGV) {
        if($arg =~ /^config=(\S+)$/) {
            $config_file = $1;
        }
        elsif($arg =~ /^hdf_in=(\S+)$/) {
            $hdf_in_file = $1;
        }
        elsif($arg =~ /^hdf_out=(\S+)$/) {
            $hdf_out_file = $1;
        }
        elsif($arg =~ /^cgs_out=(\S+)$/) {
            $cgs_out_file = $1;
        }
        elsif($arg =~ /^DLConfigBD_ADDRBase:(\S+)$/) {
            my $bt_addr = $1;
            @bt_addr = map(hex, $bt_addr =~ /[0-9A-Fa-f]{2}/g);
        }
    }

    my $config = parse_config($config_file, \@bt_addr);
    insert_config_to_hdf($config, $hdf_in_file, $hdf_out_file);
    generate_ss_cgs($config, $hdf_out_file, $cgs_out_file);
}

sub parse_config
{
    my ($file, $bt_addr) = @_;
    my @config;

    # Check if the target file exists.
    if (-e $file)
    {
        open(my $CONFIG, "<", $file) or die "could not open $file\n";
        while (<$CONFIG>) {
            next if !/(^0x[0-9A-Fa-f]+),\s*(\w+),\s*(\S+)$/;
            my $id = $1;
            my $type = $2;
            my $value = $3;

            if ($type eq 'eui64') {
                my @values;
                if ($value eq 'random') {
                    @values = map {rand(256)} (1..8);
                }
                elsif ($value eq 'btext') {
                    # RFC 4291
                    @values = (@$bt_addr[0..2], 0xff, 0xfe, @$bt_addr[3..5]);
                }
                $value = \@values;
            }
            elsif ($type eq 'hex') {
                my @values = map(hex, split(/,/, $value));
                $value = \@values;
            }
            elsif ($type eq 'string') {
                my @values = unpack("U*", $value);
                $value = \@values;
            }
            elsif ($type =~ /u?int(32|16|8)/) {
                $value =  substr($value, 0, 2) eq '0x' ? hex($value) : int($value);
            }
            push(@config, {id => $id, type => $type, value => $value});
        }
    }
    else
    {
        my @values;
        @values = map {rand(256)} (1..8);

        push(@config, {id => '0x2000', type => 'eui64', value => \@values});
    }

    return \@config;
}

sub insert_config_to_hdf
{
    my ($config, $file_in, $file_out) = @_;

    open(my $HDF_IN, "<", $file_in) or die "could not open $file_in\n";
    open(my $HDF_OUT, ">", $file_out) or die "could not open $file_out\n";

    while (<$HDF_IN>) {
        if (/^}/) {
            print_hdf_commands($config, $HDF_OUT);
        }
        print $HDF_OUT $_;
    }
}

sub print_hdf_commands
{
    my ($config, $OUTPUT) = @_;

    for $config (@{$config})
    {
        print $OUTPUT $I1 . "COMMAND \"App config $config->{id}\" $config->{id}\n";
        print $OUTPUT $I1 . "{\n";
        if (ref($config->{value}) eq 'ARRAY') {
            print $OUTPUT $I2 . "PARAM \"Data\"\n";
            print $OUTPUT $I3 . "uint8[0xff00] omit_pad_bytes;\n";
        }
        else {
            print $OUTPUT $I2 . "PARAM \"Data\"\n";
            print $OUTPUT $I3 . "$config->{type};\n";
        }
        print $OUTPUT $I1 . "}\n\n";
    }
}

sub generate_ss_cgs
{
    my ($config, $hdf_file, $file) = @_;

    open(my $OUTPUT, ">", $file) or die "could not open $file\n";

    print $OUTPUT "# DO NOT EDIT MANUALLY! FW2 VERSION\n";
    print $OUTPUT "DEFINITION <" . basename($hdf_file) . ">\n";

    for $config (@{$config})
    {
        print $OUTPUT "ENTRY \"App config $config->{id}\"\n";
        print $OUTPUT "{\n";
        if (ref($config->{value}) eq 'ARRAY') {
            print $OUTPUT $I1 . "\"Data\" =\n";
            print $OUTPUT $I1 . "COMMENTED_BYTES\n";
            print $OUTPUT $I1 . "{\n";
            print $OUTPUT $I2 . "<hex>";
            for my $index (0 .. $#{$config->{value}}) {
                if ($index % 16 == 0) {
                    print $OUTPUT "\n$I2";
                }
                printf $OUTPUT "%02x ", $config->{value}[$index];
            }
            print $OUTPUT "\n";
            print $OUTPUT $I1 . "} END_COMMENTED_BYTES\n";
        }
        else {
            print $OUTPUT $I1 . "\"Data\" = $config->{value}\n";
        }
        print $OUTPUT "}\n\n";
    }
}
