#!/bin/bash
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
(set -o igncr) 2>/dev/null && set -o igncr; # this comment is required
set -e

#######################################################################################################################
# This script is designed to generate a source file used to init patch libraries.
#
# usage:
# 	gen_source_lib_installer.bash <OUTPUT_FILE>
# 	bt_gen_lib_installer.bash 	--shell=<modus shell path>
#								--scripts=<wiced scripts path>
#								--out=<output *.c file>
#								--verbose
#								<list of libraries *.a>
#
#######################################################################################################################

USAGE="(-s=|--shell=)<shell path> (-w=|--scripts=)<wiced scripts path> (-o=|--out=)<output file> <libraries>"
if [[ $# -eq 0 ]]; then
	echo "usage: $0 $USAGE"
	exit 1
fi

for i in "$@"
do
	case $i in
		-s=*|--shell=*)
			CYMODUSSHELL="${i#*=}"
			shift
			;;
		-w=*|--scripts=*)
			SCRIPTS_DIR="${i#*=}"
			SCRIPTS_DIR=${SCRIPTS_DIR//\\/\/}
			shift
			;;
		-o=*|--out=*)
			OUTPUT_FILE="${i#*=}"
			OUTPUT_FILE=${OUTPUT_FILE//\\/\/}
			shift
			;;
		-v|--verbose)
			VERBOSE=1
			shift
			;;
		-h|--help)
			HELP=1
			echo "usage: $0 $USAGE"
			exit 1
			;;
		*)
			LIBS+=" $i"
			;;
	esac
done

if [ "$VERBOSE" != "" ]; then
	echo "Script: $0"
	echo "1: CYMODUSSHELL : $CYMODUSSHELL"
	echo "2: SCRIPTS_DIR  : $SCRIPTS_DIR"
	echo "3: OUTPUT_FILE  : $OUTPUT_FILE"
	echo "4: LIBRARIES    : $LIBS"
fi

# if lib_installer exists, don't make a new one
# we need to always generate until we can 'make clean' to remove existing
if [ -e "$OUTPUT_FILE" ]; then
    echo "$OUTPUT_FILE already present, use clean if it needs updating"
    exit 0
fi

set +e

# set up some tools that may be native and not modus-shell
CY_TOOL_PERL=perl
if ! type "$CY_TOOL_PERL" &> /dev/null; then
CY_TOOL_PERL=$CYMODUSSHELL/bin/perl
fi


"$CY_TOOL_PERL" -I "${SCRIPTS_DIR}" "${SCRIPTS_DIR}/wiced-gen-lib-installer.pl" $LIBS "${OUTPUT_FILE}"
if [ $? -eq 0 ]; then
   echo "generated $OUTPUT_FILE"
else
   echo "failed to generate $OUTPUT_FILE"
   exit 1
fi
