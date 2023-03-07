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
#set -x

#######################################################################################################################
# This script performs post-build operations to form Bluetooth application download images.
#
# usage:
#   bt_post_build.bash  --shell=<modus shell path>
#                       --cross=<cross compiler path>
#                       --scripts=<wiced scripts path>
#                       --tools=<wiced tools path>
#                       --builddir=<mainapp build dir>
#                       --elfname=<app elf>
#                       --appname=<app name>
#                       --appver=<app id and major/minor version>
#                       --hdf=<hdf file>
#                       --entry=<entry function name>
#                       --cgslist=<cgs file list>
#                       --failsafe=<failsafe cgs file>
#                       --cgsargs=<cgs tool args>
#                       --btp=<btp file>
#                       --id=<hci id>
#                       --overridebaudfile=<override baud rate list>
#                       --chip=<chip>
#                       --target=<target>
#                       --minidriver=<minidriver file>
#                       --clflags=<chipload tool flags>
#                       --extrahex=<hex file to merge>
#                       --extras=<extra build actions>
#                       --verbose
#
#######################################################################################################################
USAGE="(-s=|--shell=)<shell path> (-x=|--cross=)<cross tools path>"
USAGE+=" (-w=|--scripts=)<wiced scripts path> (-t=|--tools=)<wiced tools>"
USAGE+=" (-b=|--builddir=)<build dir> (-e=|--elfname=)<elf name>"
USAGE+=" (-a=|--appname=)<app name> (-u|--appver=)<app version> (-d=|--hdf=)<hdf file>"
USAGE+=" (-n=|--entry=)<app entry function> (-l=|--cgslist=)<cgs file list>"
USAGE+=" (-z=|--sscgs=)<static cgs file> -f=|--failsafe=)<failsafe cgs file> (-g=|--cgsargs=)<cgs tool arg>"
USAGE+=" (-p=|--btp=)<btp file> (-i=|--id=)<hci id file> (-o=|--overridebaudfile=)<override baud rate list>"
USAGE+=" (-q=|--chip=)<chip> (-r=|--target=)<target>"
USAGE+=" (-m=|--minidriver=)<minidriver file> (-c=|--clflags=)<chipload tool flags>"
USAGE+=" (-j=|--extrahex=)<hex file to merge> (-k=|--extras=)<extra build action>"

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
    -x=*|--cross=*)
        CYCROSSPATH="${i#*=}"
        CYCROSSPATH=${CYCROSSPATH//\\/\/}
        shift
        ;;
    -w=*|--scripts=*)
        CYWICEDSCRIPTS="${i#*=}"
        CYWICEDSCRIPTS=${CYWICEDSCRIPTS//\\/\/}
        shift
        ;;
    -t=*|--tools=*)
        CYWICEDTOOLS="${i#*=}"
        CYWICEDTOOLS=${CYWICEDTOOLS//\\/\/}
        shift
        ;;
    -b=*|--builddir=*)
        CY_MAINAPP_BUILD_DIR="${i#*=}"
        CY_MAINAPP_BUILD_DIR=${CY_MAINAPP_BUILD_DIR//\\/\/}
        shift
        ;;
    -e=*|--elfname=*)
        CY_ELF_NAME="${i#*=}"
        shift
        ;;
    -a=*|--appname=*)
        CY_MAINAPP_NAME="${i#*=}"
        shift
        ;;
    -u=*|--appver=*)
        CY_MAINAPP_VERSION="${i#*=}"
        shift
        ;;
    -d=*|--hdf=*)
        CY_APP_HDF="${i#*=}"
        shift
        ;;
    -n=*|--entry=*)
        CY_APP_ENTRY="${i#*=}"
        shift
        ;;
    -l=*|--cgslist=*)
        CY_APP_CGSLIST="${i#*=}"
        shift
        ;;
    -f=*|--failsafe=*)
        CY_APP_FAILSAFE="${i#*=}"
        shift
        ;;
    -z=*|--sscgs=*)
        CY_APP_SS_CGS="${i#*=}"
        shift
        ;;
    -g=*|--cgsargs=*)
        CY_APP_CGS_ARGS="${i#*=}"
        shift
        ;;
    -p=*|--btp=*)
        CY_APP_BTP="${i#*=}"
        shift
        ;;
    -i=*|--id=*)
        CY_APP_HCI_ID="${i#*=}"
        shift
        ;;
    -o=*|--overridebaudfile=*)
        CY_APP_BAUDRATE_FILE="${i#*=}"
        shift
        ;;
    -q=*|--chip=*)
        CY_CHIP="${i#*=}"
        shift
        ;;
    -r=*|--target=*)
        CY_TARGET="${i#*=}"
        shift
        ;;
    -m=*|--minidriver=*)
        CY_APP_MINIDRIVER="${i#*=}"
        shift
        ;;
    -2=*|--ds2_app=*)
        CY_DS2_APP_HEX="${i#*=}"
        shift
        ;;
    -c=*|--clflags=*)
        CY_APP_CHIPLOAD_FLAGS="${i#*=}"
        shift
        ;;
    -j=*|--extrahex=*)
        CY_APP_MERGE_HEX_NAME="${i#*=}"
        shift
        ;;
    -k=*|--extras=*)
        CY_APP_BUILD_EXTRAS="${i#*=}"
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
        echo "bad parameter $i"
        echo "usage: $0 $USAGE"
        echo "failed to generate download file"
        exit 1
        ;;
    esac
done
CY_APP_HCD=

echo "Begin post build processing"
if [ "$VERBOSE" != "" ]; then
    echo 1:  CYMODUSSHELL         : $CYMODUSSHELL
    echo 2:  CYCROSSPATH          : $CYCROSSPATH
    echo 3:  CYWICEDSCRIPTS       : $CYWICEDSCRIPTS
    echo 4:  CYWICEDTOOLS         : $CYWICEDTOOLS
    echo 5:  CY_MAINAPP_BUILD_DIR : $CY_MAINAPP_BUILD_DIR
    echo 6:  CY_ELF_NAME          : $CY_ELF_NAME
    echo 7:  CY_MAINAPP_NAME      : $CY_MAINAPP_NAME
    echo 7:  CY_MAINAPP_VERSION   : $CY_MAINAPP_VERSION
    echo 8:  CY_APP_HDF           : $CY_APP_HDF
    echo 9:  CY_APP_ENTRY         : $CY_APP_ENTRY
    echo 10: CY_APP_CGSLIST       : $CY_APP_CGSLIST
    echo 11: CY_APP_FAILSAFE      : $CY_APP_FAILSAFE
    echo 12: CY_APP_SS_CGS        : $CY_APP_SS_CGS
    echo 13: CY_APP_CGS_ARGS      : $CY_APP_CGS_ARGS
    echo 14: CY_APP_BTP           : $CY_APP_BTP
    echo 15: CY_APP_HCI_ID        : $CY_APP_HCI_ID
    echo 16: CY_APP_BAUDRATE_FILE : $CY_APP_BAUDRATE_FILE
    echo 17: CY_CHIP              : $CY_CHIP
    echo 18: CY_TARGET            : $CY_TARGET
    echo 19: CY_APP_MINIDRIVER    : $CY_APP_MINIDRIVER
    echo 20: CY_DS2_APP_HEX       : $CY_DS2_APP_HEX
    echo 21: CY_APP_CHIPLOAD_FLAGS: $CY_APP_CHIPLOAD_FLAGS
    echo 22: CY_APP_MERGE_HEX_NAME: $CY_APP_MERGE_HEX_NAME
    echo 23: CY_APP_BUILD_EXTRAS  : $CY_APP_BUILD_EXTRAS
fi

# check that required files are present
if [ ! -e "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" ]; then
    echo "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME not found!"
    exit 1
fi

CY_APP_HEX="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_download.hex"
CY_APP_HEX_STATIC="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_static.hex"
CY_APP_HEX_SS="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_ss.hex"
CY_APP_HCD="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_download.hcd"
CY_APP_LD="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}.ld"
CY_APP_CGS_MAP="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}.cgs.map"

# check dependencies - only rebuild when needed
if [ -e "$CY_APP_HEX" ]; then
    echo "hex file already exists"
    if [ "$CY_APP_HEX" -nt "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" ]; then
      echo
      echo "hex file is newer than elf, skipping post-build operations"
      echo "make clean to refresh hex if needed"
      echo
      exit 0
    fi
fi

# set up some tools that may be native and not modus-shell
CY_TOOL_WC=wc
CY_TOOL_SYNC=sync
CY_TOOL_MV=mv
CY_TOOL_CAT=cat
CY_TOOL_PERL=perl
CY_TOOL_RM=rm
CY_TOOL_CP=cp
CY_TOOL_ECHO=echo
CY_TOOL_TAIL=tail
if ! type "$CY_TOOL_WC" &> /dev/null; then
CY_TOOL_WC=$CYMODUSSHELL/bin/wc
fi
if ! type "$CY_TOOL_SYNC" &> /dev/null; then
CY_TOOL_SYNC=$CYMODUSSHELL/bin/sync
fi
if ! type "$CY_TOOL_MV" &> /dev/null; then
CY_TOOL_MV=$CYMODUSSHELL/bin/mv
fi
if ! type "$CY_TOOL_CAT" &> /dev/null; then
CY_TOOL_CAT=$CYMODUSSHELL/bin/cat
fi
if ! type "$CY_TOOL_PERL" &> /dev/null; then
CY_TOOL_PERL=$CYMODUSSHELL/bin/perl
fi
if ! type "$CY_TOOL_RM" &> /dev/null; then
CY_TOOL_RM=$CYMODUSSHELL/bin/rm
fi
if ! type "$CY_TOOL_CP" &> /dev/null; then
CY_TOOL_CP=$CYMODUSSHELL/bin/cp
fi
if ! type "$CY_TOOL_ECHO" &> /dev/null; then
CY_TOOL_ECHO=$CYMODUSSHELL/bin/echo
fi
if ! type "$CY_TOOL_TAIL" &> /dev/null; then
CY_TOOL_TAIL=$CYMODUSSHELL/bin/tail
fi

# clean up any previous copies
"$CY_TOOL_RM" -f "$CY_MAINAPP_BUILD_DIR/configdef*.hdf" "$CY_APP_HCD" "$CY_APP_HEX" "$CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs" "$CY_MAINAPP_BUILD_DIR/det_and_id.log" "$CY_MAINAPP_BUILD_DIR/download.log"

if [[ $CY_APP_BUILD_EXTRAS = *"DIRECT"* ]]; then
echo "building image for direct ram load (*.hcd)"
CY_APP_DIRECT_LOAD_ADDR=$(echo $CY_APP_CGS_ARGS | "$CY_TOOL_PERL" -ple 's/.*DLConfigSSLocation:(0x[0-9A-Fa-f]+).*/$1/')
#echo "Direct load adress $CY_APP_DIRECT_LOAD_ADDR"
CY_APP_DIRECT_LOAD="DIRECT_LOAD=$CY_APP_DIRECT_LOAD_ADDR"
CY_APP_CGS_ARGS+=" -O \"DLConfigTargeting:RAM runtime\""
CY_APP_CGS_ARGS+=" -O DLConfigVSLocation:0"
CY_APP_CGS_ARGS+=" -O DLConfigVSLength:0"
CY_APP_CGS_ARGS+=" -O DLWriteVerifyMode:Write"
CY_APP_CGS_ARGS+=" -O \"DLSectorEraseMode:Written sectors only\""
fi

# generate asm listing
"${CYCROSSPATH}objdump" --disassemble "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" > "$CY_MAINAPP_BUILD_DIR/${CY_ELF_NAME/elf/asm}"

#create app cgs file
if [ "$VERBOSE" != "" ]; then
    echo "calling $CY_TOOL_PERL -I $CYWICEDSCRIPTS $CYWICEDSCRIPTS/wiced-gen-cgs.pl $CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME $CY_APP_DIRECT_LOAD $CY_APP_CGSLIST $CY_APP_HDF $CY_APP_LD $CY_APP_BTP $CY_APP_ENTRY out=$CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs"
fi
CY_RESOURCE_REPORT=$CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.report.txt
set +e
"$CY_TOOL_PERL" -I "$CYWICEDSCRIPTS" "$CYWICEDSCRIPTS/wiced-gen-cgs.pl" "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" $CY_APP_DIRECT_LOAD $CY_APP_CGSLIST "$CY_APP_HDF" "$CY_APP_LD" "$CY_APP_BTP" $CY_APP_ENTRY out="$CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs" > "$CY_RESOURCE_REPORT"
CY_APP_ERROR=$?
"$CY_TOOL_CAT" "$CY_RESOURCE_REPORT"
set -e
if [ $CY_APP_ERROR -eq 1 ]; then
echo "exiting build"
exit 1
fi

# copy hdf local for cgs tool, it seems to need it despite -D
"$CY_TOOL_CP" "$CY_APP_HDF" "$CY_MAINAPP_BUILD_DIR/."

# set up BDADDR if random or default
CY_APP_CGS_ARGS_ORIG=$CY_APP_CGS_ARGS
CY_APP_CGS_ARGS=$("$CY_TOOL_PERL" -I "$CYWICEDSCRIPTS" "$CYWICEDSCRIPTS/wiced-bdaddr.pl" ${CY_CHIP} "$CY_APP_BTP" ${CY_APP_CGS_ARGS})

# add ss cgs if needed, copy local to use local hdf
if [ "$CY_APP_SS_CGS" != "" ]; then
"$CY_TOOL_CP" "$CY_APP_SS_CGS" "$CY_MAINAPP_BUILD_DIR/."
CY_APP_SS_CGS=$(basename $CY_APP_SS_CGS)
CY_APP_SS_CGS="--ss-cgs \"$CY_MAINAPP_BUILD_DIR/$CY_APP_SS_CGS\""
fi

# DIRECT_LOAD address is moved by wiced-gen-cgs.pl to not overlap with app
# check that DLConfigSSLocation matches DIRECT_LOAD address
if [[ "$CY_APP_CGS_ARGS" = *"DLConfigSSLocation"* ]]; then
# DIRECT_LOAD address that may have been moved by wiced-gen-cgs.pl
CY_DIRECT_LOAD_ADDR_1=$("$CY_TOOL_PERL" -ne 'print "$1" if /DIRECT LOAD address (0x[0-9A-F]+)/' "$CY_RESOURCE_REPORT")
# move load address if overlapping with app
CY_APP_CGS_ARGS=${CY_APP_CGS_ARGS//${CY_APP_DIRECT_LOAD_ADDR}/${CY_DIRECT_LOAD_ADDR_1}}
fi

# Generate hdf and cgs files for the app static configurations
if [[ $CY_APP_BUILD_EXTRAS = *"static_config"* ]]; then
    CY_APP_SS_CGS=${CY_MAINAPP_BUILD_DIR}/${CY_MAINAPP_NAME}_ss.cgs
    CY_APP_SS_CONFIG=static_config.txt
    echo "Generating static configurations from ${CY_APP_SS_CONFIG}"
    GEN_SS_CGS_COMMAND="\
        ${CY_TOOL_PERL} -I ${CYWICEDSCRIPTS} ${CYWICEDSCRIPTS}/wiced-gen-ss-cgs.pl\
        config=${CY_APP_SS_CONFIG}\
        hdf_in=${CY_APP_HDF}\
        hdf_out=${CY_MAINAPP_BUILD_DIR}/$(basename ${CY_APP_HDF})\
        cgs_out=${CY_APP_SS_CGS}\
        ${CY_APP_CGS_ARGS}"

    if [ "${VERBOSE}" != "" ]; then
        echo Calling ${GEN_SS_CGS_COMMAND}
    fi
    set +e
    eval ${GEN_SS_CGS_COMMAND}
    set -e
    CY_APP_SS_CGS="--ss-cgs \"$CY_APP_SS_CGS\""
fi

# for flash downloads of non-HomeKit, this is the download file, done
# generate hex download file, use eval because of those darn quotes needed around "DLConfigTargeting:RAM runtime"
# use set +e because of the darn eval
echo "generate hex file:"
echo "cgs -D $CY_MAINAPP_BUILD_DIR $CY_APP_CGS_ARGS -B $CY_APP_BTP -P $CY_MAINAPP_VERSION -I $CY_APP_HEX -H $CY_APP_HCD $CY_APP_SS_CGS -M $CY_APP_CGS_MAP --cgs-files $CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs"
set +e
eval "$CYWICEDTOOLS/CGS/cgs -D $CY_MAINAPP_BUILD_DIR $CY_APP_CGS_ARGS -B $CY_APP_BTP -P $CY_MAINAPP_VERSION -I $CY_APP_HEX -H $CY_APP_HCD $CY_APP_SS_CGS -M $CY_APP_CGS_MAP --cgs-files $CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs"
set -e
if [[ ! -e $CY_APP_HEX ]]; then
    echo "!! Post build failed, no hex file output"
    exit 1
fi

# handle APP_XIP, merge the XIP code/data into the hex
if [[ $CY_APP_BUILD_EXTRAS = *"_XIP_"* ]]; then
    echo "Building in XIP section"
    "$CY_TOOL_MV" $CY_APP_HEX $CY_APP_HEX.temp
    "$CY_TOOL_RM" $CY_APP_HCD
    "$CY_TOOL_SYNC"
    "${CYCROSSPATH}objcopy" -j .app_xip_area -O ihex "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" "$CY_APP_HEX_STATIC"
    "$CYWICEDTOOLS/IntelHexToBin/IntelHexMerge" "$CY_APP_HEX.temp" "$CY_APP_HEX_STATIC" "$CY_APP_HEX"
    "$CYWICEDTOOLS/IntelHexToBin/IntelHexToHCD" "$CY_APP_HEX" "$CY_APP_HCD"
fi

if [[ $CY_APP_BUILD_EXTRAS = *"_APPDS2_"* ]]; then
if [[ ! -e $CY_DS2_APP_HEX ]]; then
    echo "!! OTA image cannot be built, could not find ds2 app hex file $CY_DS2_APP_HEX"
    exit 1
fi
echo "Appending DS2 section"
"$CY_TOOL_MV" $CY_APP_HEX $CY_APP_HEX.ds1
"$CY_TOOL_RM" $CY_APP_HCD
"$CY_TOOL_SYNC"
#"${CYCROSSPATH}objcopy" -j .app_xip_area_ds2 -O ihex "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" "${CY_APP_HEX}.xip.ds2.code.hex"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexMerge" "$CY_APP_HEX.ds1" "$CY_DS2_APP_HEX" "$CY_APP_HEX"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexToHCD" "$CY_APP_HEX" "$CY_APP_HCD"
fi

# handle STATIC_SECTION
if [[ $CY_APP_BUILD_EXTRAS = *"in_code"* ]]; then
echo "Building in static section"
"$CY_TOOL_MV" $CY_APP_HEX $CY_APP_HEX.temp
"$CY_TOOL_RM" $CY_APP_HCD
"$CY_TOOL_SYNC"
"${CYCROSSPATH}objcopy" -j .static_area -O ihex "$CY_MAINAPP_BUILD_DIR/$CY_ELF_NAME" "$CY_APP_HEX_STATIC"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexMerge" "$CY_APP_HEX.temp" "$CY_APP_HEX_STATIC" "$CY_APP_HEX"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexToHCD" "$CY_APP_HEX" "$CY_APP_HCD"
fi
# the static data can also be defined in a file staticdata.hex, to be copied with app sources
if [[ $CY_APP_BUILD_EXTRAS = *"in_file"* ]]; then
echo "Building in static section from staticdata.hex"
"$CY_TOOL_MV" $CY_APP_HEX $CY_APP_HEX.temp
"$CY_TOOL_RM" $CY_APP_HCD
"$CY_TOOL_SYNC"
"$CYWICEDTOOLS/hexgen/hexgen2" -a 0x500800 -i "$CY_MAINAPP_BUILD_DIR/../../../datainput.txt" -o "$CY_MAINAPP_BUILD_DIR/../../../staticdata.hex"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexMerge" "$CY_APP_HEX.temp" "$CY_MAINAPP_BUILD_DIR/../../../staticdata.hex" "$CY_APP_HEX"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexToHCD" "$CY_APP_HEX" "$CY_APP_HCD"
fi

# handle HOMEKIT hex merge for 20706, only if $CY_APP_MERGE_HEX_NAME
if [[ $CY_APP_BUILD_EXTRAS = *"HOMEKIT"* ]]; then
if [[ $CY_APP_MERGE_HEX_NAME = *"hex"* ]]; then
echo "Merging homekit offset patch"
"$CY_TOOL_MV" $CY_APP_HEX $CY_APP_HEX.temp
"$CY_TOOL_RM" $CY_APP_HCD
"$CY_TOOL_SYNC"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexMerge" "$CY_APP_HEX.temp" "$CY_APP_MERGE_HEX_NAME" "$CY_APP_HEX"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexToHCD" "$CY_APP_HEX" "$CY_APP_HCD"
fi
fi

# make OTA image
if [[ $CY_APP_BUILD_EXTRAS = *"OTA"* ]]; then

if [[ $CY_APP_BUILD_EXTRAS = *"DIRECT"* ]]; then
echo "Warning: skipping OTA upgrade image build because DIRECT_LOAD=1"
else

# add fail safe if present
if [[ $CY_APP_FAILSAFE = *"cgs" ]]; then
echo "inserting fail safe into ota upgrade image"
# create SS hex: $(QUIET)$(call CONV_SLASHES,$(CGS_FULL_NAME)) -D $(SOURCE_ROOT)platforms -B $(SOURCE_ROOT)platforms/$(PLATFORM_FULL)/$(PLATFORM_BOOTP) $(BTP_OVERRIDES_FAIL_SAFE) -I build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_ihexss_temp.hex --cgs-files $(FAIL_SAFE_OTA_CGS)
echo "$CYWICEDTOOLS/CGS/cgs -D $CY_MAINAPP_BUILD_DIR -B $CY_APP_BTP $CY_APP_CGS_ARGS -O DLConfigVSLocation:0x2000 -O DLConfigFixedHeader:0 -O DLMaxWriteSize:251 -I $CY_APP_HEX_SS --cgs-files $CY_APP_FAILSAFE"
"$CYWICEDTOOLS/CGS/cgs" -D "$CY_MAINAPP_BUILD_DIR" -B "$CY_APP_BTP" $CY_APP_CGS_ARGS -O DLConfigVSLocation:0x2000 -O DLConfigFixedHeader:0 -O DLMaxWriteSize:251 -I "$CY_APP_HEX_SS" --cgs-files "$CY_APP_FAILSAFE"
# create text listing: $(call CONV_SLASHES,$(HEX_TO_BIN_FULL_NAME)) -a build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_ihexss_temp.hex build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_full.txt
if [ "$VERBOSE" != "" ]; then
echo "create text listing"
fi
"$CYWICEDTOOLS/IntelHexToBin/IntelHexToBin" -a "$CY_APP_HEX_SS" "$CY_APP_HEX_SS.full.txt"
# trim off FE 00 00 from end of text listing: $(call CONV_SLASHES,$(HEAD_OR_TAIL_FULL_NAME)) H -3 $CY_APP_HEX_SS.full.txt $CY_APP_HEX_SS.txt
if [ "$VERBOSE" != "" ]; then
echo "trim text listing"
fi
"$CYWICEDTOOLS/IntelHexToBin/HeadOrTail" H -3 "$CY_APP_HEX_SS.full.txt" "$CY_APP_HEX_SS.txt"

# create app hex with default SS: $(call CONV_SLASHES,$(CGS_FULL_NAME)) -D $(SOURCE_ROOT)platforms -B $(SOURCE_ROOT)platforms/$(PLATFORM_FULL)/$(PLATFORM_BOOTP) $(BTP_OVERRIDES_SPAR) -I build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_ihexserial_temp.hex --cgs-files build/$(OUTPUT_NAME)/$(BLD)-$(APP)-$(BASE_LOC)-$(SPAR_LOC)-spar.cgs
if [ "$VERBOSE" != "" ]; then
echo "create app hex with default SS, zero origin"
fi
echo "generate hex file:"
CY_APP_CGS_ARGS=$(echo ${CY_APP_CGS_ARGS} | sed -E 's/.*(-O DLConfigBD_ADDRBase:[^ ]+).*/\1/')
CY_APP_CGS_ARGS+=" -O ConfigDSLocation:0x4000 -O DLConfigVSLocation:0x2000"
echo "cgs -D $CY_MAINAPP_BUILD_DIR $CY_APP_CGS_ARGS -B $CY_APP_BTP -I $CY_APP_HEX.ihexserial.temp.hex --cgs-files $CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs"
$CYWICEDTOOLS/CGS/cgs -D "$CY_MAINAPP_BUILD_DIR" $CY_APP_CGS_ARGS -B "$CY_APP_BTP" -I "$CY_APP_HEX.ihexserial.temp.hex" --cgs-files "$CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.cgs"

# append text SS with regular hex: $(call CONV_SLASHES,$(APPEND_TO_INTEL_HEX_FULL_NAME)) -I 0x1b build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_ss_temp.txt build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_ihexserial_temp.hex build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_combined_temp.hex
if [ "$VERBOSE" != "" ]; then
echo "append text ss with fail safe to app hex"
fi
"$CYWICEDTOOLS/IntelHexToBin/AppendToIntelHex" -I 0x1b "$CY_APP_HEX_SS.txt" "$CY_APP_HEX.ihexserial.temp.hex" "$CY_APP_HEX.combined.hex"

# add FLASH offset to hex: $(QUIET)$(call CONV_SLASHES,$(SHIFT_INTEL_HEX_FULL_NAME)) 0xFF000000 build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_combined_temp.hex build/$(OUTPUT_NAME)/$(OUTPUT_NAME)_.hex
"$CYWICEDTOOLS/IntelHexToBin/ShiftIntelHex" 0xFF000000 "$CY_APP_HEX.combined.hex" "$CY_APP_HEX"

# case for homekit
if [[ $CY_APP_MERGE_HEX_NAME = *"hex"* ]]; then
echo "merge hex $CY_APP_MERGE_HEX_NAME"
"$CY_TOOL_MV" $CY_APP_HEX $CY_APP_HEX.temp
"$CY_TOOL_SYNC"
"$CYWICEDTOOLS/IntelHexToBin/IntelHexMerge" "$CY_APP_HEX.temp" "$CY_APP_MERGE_HEX_NAME" "$CY_APP_HEX"
fi
echo "done creating image with fail safe"
fi

echo "building OTA upgrade image (*.bin)"
CY_APP_OTA_HEX="$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_${CY_TARGET}.ota.hex"
CY_APP_OTA_BIN=${CY_APP_OTA_HEX//.hex/.bin}
if [[ $CY_APP_BUILD_EXTRAS = *"_APPDS2_"* ]]; then
    if [ "$VERBOSE" != "" ]; then
        echo "Adjusting image to trim DS2 app, only do ota update with DS1 + XIP code"
    fi
    CY_APP_HEX="$CY_APP_HEX.ds1"
fi
# convert hex to bin
DS_ADDR=$("$CY_TOOL_PERL" -ne 'print "$1" if /DS available \d+ \(0x[0-9A-Fa-f]+\) at (0x[0-9A-Fa-f]+)/' "$CY_RESOURCE_REPORT")
if [[ $CY_APP_CGS_ARGS_ORIG = *"-A 0xFF000000"* ]]; then
    DS_ADDR=$(printf "0x%08X" $((${DS_ADDR}+0xFF000000)))
fi
"$CYWICEDTOOLS/IntelHexToBin/IntelHexToBin" -l $DS_ADDR -f 00 "$CY_APP_HEX" "$CY_APP_OTA_BIN"
# print size
FILESIZE=$("$CY_TOOL_WC" -c < "$CY_APP_OTA_BIN")
echo "OTA Upgrade file size is $FILESIZE"
DS_LENGTH=$("$CY_TOOL_PERL" -ne 'print "$1" if /DS available (\d+)/' "$CY_RESOURCE_REPORT")
echo "Upgrade storage available $DS_LENGTH"
[ "$FILESIZE" -gt "$DS_LENGTH" ] && echo "WARNING: OTA image exceeds DS area"

fi # DIRECT_LOAD check
fi # end if OTA

# copy files necessary for download to help launch config find them
"$CY_TOOL_CP" "$CY_APP_BTP" "$CY_MAINAPP_BUILD_DIR/$CY_MAINAPP_NAME.btp"
"$CY_TOOL_CP" "$CY_APP_HCI_ID" "$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_hci_id.txt"
if [[ -e $CY_APP_BAUDRATE_FILE ]]; then
"$CY_TOOL_CP" "$CY_APP_BAUDRATE_FILE" "$CY_MAINAPP_BUILD_DIR/${CY_MAINAPP_NAME}_baudrates.txt"
fi
"$CY_TOOL_CP" "$CY_APP_MINIDRIVER" "$CY_MAINAPP_BUILD_DIR/minidriver.hex"
"$CY_TOOL_ECHO" "$CY_APP_CHIPLOAD_FLAGS" >"$CY_MAINAPP_BUILD_DIR/chipload_flags.txt"

echo "Post build processing completed"
