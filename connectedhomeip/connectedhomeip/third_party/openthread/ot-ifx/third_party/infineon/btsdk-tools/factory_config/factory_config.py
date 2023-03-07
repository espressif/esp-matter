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
"""
This script can be used to
    * create JSON file with application TLV data to be programmed to the static section (SS)
    * convert JSON type/value pairs to binary TLV
    * insert TLVs into the SS area of a hex file,
    * just dump the SS from the hex file.
There are pearl and python versions of the file.

Usage factory_config.py [hex file] [json file] [-tv:<name>:<type>:<value>] [-20706A2]

If only hex file name is provided, the tool prints the TLVs in the SS section.
Note that the -20706A2 flag is necessary to tell the tool to use the 20706 style of TLV,
otherwise it defaults to the TLV style used for 20735B1, 20739B2, 20819A1, or 43012C0...
If hex file already contains application TLVs the printout will display both original
and application TLVs.

If only the JSON file is provided, the tool prints the hex TLV that would be generated.

If only one or several -tv structures are included, the tool prints the JSON file that would be generated

If one or several -tv triplets are included and JSON file is provided, the tool creates the JSON file
that includes array of Name/Type/Value objects. For example
    factory_config_json.py -tv:static_oob_data:e1:000102030405060708090a0b0c0d0e0f

If HEX file and JSON file are included, the tool merges data from the JSON into the static section of the
hex file. Note that if hex files contained application TLVs those will be replaced with the
TLVs specified in the json file.

If HEX file and array of TLVs are included, the tool merges TLV information into the static section
of the hex file. Note that if hex files contained application TLVs those will be replaced with the
TLVs specified in the command line.

Example:
factory_config.py light_dimmable-CYW920719Q40EVB_01-rom-ram-Wiced-release.hex  -tv:uuid:e1:b9fe1e3a278b4437a66f59cc74b6cf86

"""
import sys
import json
import os

ss_address_limit = 2048 # TBD

def parse_tv(tv_str):
    name = tv_str[:tv_str.index(":")]
    tv_str = tv_str[len(name) + 1:]
    type = tv_str[:tv_str.index(":")]
    value = tv_str[len(type) + 1:]

    # a Python object (dict):
    tv = {
        "name": name,
        "type": type,
        "value": value
    }

    # check that the type is 1 byte hex integer
    assert (len(type) == 2), "TV format is <name>:<type>:<value>, where type is 2 hex symbol string, for example, e0"

    # add to the array of TVs
    app_tvs.append(tv)
    return

#
# Make TLV contained in a JSON file
#
def make_tlv(filename):
    # Load JSON file
    with open(filename) as f:
        data = json.load(f)

    tlv = ""

    for i in range(len(data)):
        # print("make_tlv: " + data[i]['type'])
        # print("make_tlv: " + data[i]['name'])
        # print("make_tlv: " + data[i]['value'])
        tlv += data[i]['type']
        length = len(data[i]['value']) // 2;
        tlv += format(length % 256, '02X')
        tlv += format(length // 256, '02X')
        tlv += data[i]['value']
    return tlv

#
# Print TLV contained in a JSON file
#
def print_json(filename):
    print ("TLV: " + make_tlv(filename))

#
# Read static section bytes. Return string with all bytes concatenated
#
def read_fw_ss_bytes(hex_filename, fw_only):
    hex_file = open(hex_filename, "r")
    hex_lines = hex_file.readlines()
    num_lines = len(hex_lines)
    ss_bytes = ""
    end_address = 0

    # check if read hex file
    assert (num_lines != 0), "read_fw_ss_bytes: failed to read " + hex_filename

    for i in range(0, num_lines):
        # hex format start_code (1), byte count (1), address (2), record type (1), data (byte_count), checksum (1)
        assert(hex_lines[i][0] == ':'), "read_fw_ss_bytes: hex file parse line " + str(i) + " no start code"

        byte_count  = int(hex_lines[i][1:3], 16)
        address     = int(hex_lines[i][3:7], 16)
        record_type = int(hex_lines[i][7:9], 16)

        # print("read_fw_ss_bytes: byte count " + str(byte_count) + " addr " + str(address) + " type " + str(record_type))

        if record_type == 4:
            assert (byte_count == 2), "read_fw_ss_bytes: hex file illegal record_type 4 byte count " + byte_count

            extended_addr = int(hex_lines[i][9:11], 16)
            # print("read_fw_ss_bytes: extended address " + hex(extended_addr))
            continue

        if (address >= ss_address_limit) or (end_address != address):
            break

        assert (len(hex_lines[i]) == 2 * (byte_count + 6)), "read_fw_ss_bytes: hex file bad length " + str(2 * (byte_count + 6)) + " expected " + str(len(hex_lines[i]))

        ss_bytes += hex_lines[i][9:9 + 2 * byte_count]
        end_address = address + byte_count;

    # It is possible that this hex file already have application bytes.
    # If true, remove them
    if not is_20706:
        ss_offset = 32
    else:
        ss_offset = 0

    # Static section contains FW TLVs followed by App TLVs
    # App TLVs always formatted as 20706
    app_tlvs = False

    while ss_offset < len(ss_bytes):
        if is_20706 or app_tlvs:
            # First byte is item Id then 2 bytes of length.
            item_id  = ss_bytes[ss_offset:ss_offset+2];
            group_id = '00'
            length = int(ss_bytes[ss_offset+2:ss_offset+4], 16) + (256 * int(ss_bytes[ss_offset+4:ss_offset+6], 16))
        else:
            # First byte is item id, then 1 byte group Id, then 1 byte of length
            item_id  = ss_bytes[ss_offset:ss_offset+2];
            group_id = ss_bytes[ss_offset+2:ss_offset+4];
            length = int(ss_bytes[ss_offset+4:ss_offset+6], 16)

        # print("read_fw_ss_bytes: item " + item_id + " group " + group_id + " length " + str(length))

        ss_offset += 2 * (3 + length)

        if (item_id == 'FE') and (group_id == '00'):
            if fw_only:
                break
            app_tlvs = True;

    return ss_bytes[:ss_offset]

#
# Print content of a static section of a hex file
#
def print_ss(hex_filename):

    # read static section from the hex file
    ss_bytes = read_fw_ss_bytes(hex_filename, False)

    # check if header is correct
    if not is_20706:
        if ss_bytes.find('4252434D636667') != 0:
            print("illegal format " + ss_bytes)
            return

    # ss_bytes is contatenated string of static section TLVs
    # print("print_ss: " + ss_bytes)

    # skip header
    if not is_20706:
        ss_bytes = ss_bytes[32:]

    # Static section contains FW TLVs followed by App TLVs
    # App TLVs always formatted as 20706
    app_tlvs = False

    while True:
        if is_20706 or app_tlvs:
            # First byte is item Id then 2 bytes of length.
            item_id = ss_bytes[0:2];
            group_id = '00'
            length = int(ss_bytes[2:4], 16) + (256 * int(ss_bytes[4:6], 16))
            print("item " + item_id + " len " + hex(length) + " data " + ss_bytes[6:6 + 2 * length]);
        else:
            # First byte is item id, then 1 byte group Id, then 1 byte of length
            item_id = ss_bytes[0:2];
            group_id = ss_bytes[2:4];
            length = int(ss_bytes[4:6], 16)
            print("item " + item_id + " group " + group_id + " len " + hex(length) + " data " + ss_bytes[6:6+2*length]);

        ss_bytes = ss_bytes[2 * (3 + length):]
        # print(ss_bytes)
        if (item_id == 'FE') and (group_id == '00'):
            app_tlvs = True;

        if len(ss_bytes) < 3:
            break

#
# Sets the BDADDR of a static section of a hex file
#
def set_bdaddr(hex_filename, bdaddr):
    hex_file = open(hex_filename, "r")
    hex_lines = hex_file.readlines()
    num_lines = len(hex_lines)
    ss_bytes = ""
    end_address = 0
    found_bdaddr = False
    ss_info = [] # Used to track SS # of lines and bytes per line
    bda_line_num = 0
    bdaddr_offset = 0

    # check if read hex file
    assert (num_lines != 0), "read_fw_ss_bytes: failed to read " + hex_filename

    for line_num in range(0, num_lines):
        # hex format start_code (1), byte count (1), address (2), record type (1), data (byte_count), checksum (1)
        assert(hex_lines[line_num][0] == ':'), "read_fw_ss_bytes: hex file parse line " + str(line_num) + " no start code"

        ss_info.append([])

        byte_count  = int(hex_lines[line_num][1:3], 16)
        address     = int(hex_lines[line_num][3:7], 16)
        record_type = int(hex_lines[line_num][7:9], 16)

        # print("read_fw_ss_bytes: byte count " + str(byte_count) + " addr " + str(address) + " type " + str(record_type))

        if record_type == 4:
            assert (byte_count == 2), "read_fw_ss_bytes: hex file illegal record_type 4 byte count " + byte_count

            extended_addr = int(hex_lines[line_num][9:11], 16)
            # print("read_fw_ss_bytes: extended address " + hex(extended_addr))
            ss_info[line_num] = 0
            continue

        if (address >= ss_address_limit) or (end_address != address):
            ss_info[line_num] = byte_count
            break

        assert (len(hex_lines[line_num]) == 2 * (byte_count + 6)), "read_fw_ss_bytes: hex file bad length " + str(2 * (byte_count + 6)) + " expected " + str(len(hex_lines[line_num]))

        ss_bytes += hex_lines[line_num][9:9 + 2 * byte_count]
        end_address = address + byte_count;
        ss_info[line_num] = byte_count

    # It is possible that this hex file already have application bytes.
    # If true, remove them
    if not is_20706:
        ss_offset = 32
        bdaddr_item_id = '00'
        bdaddr_group_id = '03'
    else:
        ss_offset = 0
        bdaddr_item_id = '40'
        bdaddr_group_id = '00'

    # Static section contains FW TLVs followed by App TLVs
    # App TLVs always formatted as 20706
    app_tlvs = False

    if( ss_info[0] == 0 ):
        bda_line_num = 1

    while ss_offset < len(ss_bytes):
        if is_20706 or app_tlvs:
            # First byte is item Id then 2 bytes of length.
            item_id  = ss_bytes[ss_offset:ss_offset+2];
            group_id = '00'
            length = int(ss_bytes[ss_offset+2:ss_offset+4], 16) + (256 * int(ss_bytes[ss_offset+4:ss_offset+6], 16))
        else:
            # First byte is item id, then 1 byte group Id, then 1 byte of length
            item_id  = ss_bytes[ss_offset:ss_offset+2];
            group_id = ss_bytes[ss_offset+2:ss_offset+4];
            length = int(ss_bytes[ss_offset+4:ss_offset+6], 16)

        # print("read_fw_ss_bytes: item " + item_id + " group " + group_id + " length " + str(length))

        if (item_id == bdaddr_item_id) and (group_id == bdaddr_group_id) and (length == 6):
            found_bdaddr = True
            bdaddr_offset = (ss_offset - 2*sum(ss_info[:bda_line_num]) + 15 )
            # print "Found BDADDR at line number %d at offset %d" % (bda_line_num, bdaddr_offset)
            break

        ss_offset += 2 * (3 + length)

        if( ss_offset > 2*sum(ss_info[:bda_line_num+1]) ):
            bda_line_num += 1


        if (item_id == 'FE') and (group_id == '00'):
            break

    assert (found_bdaddr), "ERROR: Cannot find BDADDR Identifier in hex file"

    # BDADDR is written to flash in little endian format
    new_bda_flipped = new_bda[10:12] + new_bda[8:10] + new_bda[6:8] + new_bda[4:6] + new_bda[2:4] + new_bda[0:2]

    # Cannot modify string characters directly, must use list
    hex_line_list = list(hex_lines[bda_line_num])

    # Write new BDADDR to line
    for i in range(0,12):
        hex_line_list[bdaddr_offset+i] = new_bda_flipped[i]

    # Join characters into a single line to be written back to hex file, without the Checksum byte (will be calculated later)
    hex_lines[bda_line_num] = "".join(hex_line_list[:-3])

    # Add the Checksum byte
    hex_lines[bda_line_num] = hex_lines[bda_line_num] + hex_line_checksum(hex_lines[bda_line_num]) + "\n"

    hex_file_out = open(hex_filename, "w")
    hex_file_out.writelines(hex_lines)

    print ("Done updating the BDADDR")

#
# Hex Line checksum
#
def hex_line_checksum(hex_line):
    assert((hex_line[0] == ':') and ((len(hex_line) - 1) % 2 == 0)), "checksum bad hex_line"
    hex_line = hex_line[1:]
    byte = 0
    for i in range(0, len(hex_line) // 2):
        byte += int(hex_line[2 * i: 2 * i + 2], 16)
    return format((0x100 - (byte % 256)), '02X')

#
# merge TVs defined in the JSON file into a static section of the hex file
#
def hex_merge(hex_filename, json_filename):

    # read static section from the hex file
    ss_bytes = read_fw_ss_bytes(hex_filename, True)
    # print(ss_bytes)

    # on 20706 FW is not always terminated with FE0000 (end item, end group, 0 length) add it here
    if is_20706:
        print(ss_bytes[len(ss_bytes) - 6:])
        if ss_bytes[len(ss_bytes) -6:] != 'FE0000':
            ss_bytes += 'FE0000'

    # concatenate with TLVs from the JSON file
    ss_bytes += make_tlv(json_filename)
    # print("hex_merge ss_bytes: " + ss_bytes)

    hex_file_in = open(hex_filename, "r")
    hex_lines = hex_file_in.readlines()
    num_lines_in = len(hex_lines)
    hex_file_in.close()

    # calculate max number of bytes in a line
    max_bytes = 0
    for i in range(0, num_lines_in):
        max_bytes = max((len(hex_lines[i]) - 11) // 2, max_bytes)

    num_lines = 0

    # find beginning of the static section
    for i in range(0, num_lines_in):
        address     = int(hex_lines[i][3:7], 16)
        record_type = int(hex_lines[i][7:9], 16)
        if (record_type == 0) and (address == 0):
            break
        num_lines += 1

    # find the first record not related to the FW static section
    next_record_address = 0
    for j in range(i, num_lines_in):
        byte_count  = int(hex_lines[j][1:3], 16)
        address     = int(hex_lines[j][3:7], 16)
        record_type = int(hex_lines[j][7:9], 16)
        if (record_type != 0) or (address >= ss_address_limit) or (next_record_address != address):
            break
        next_record_address = address + byte_count

    # Delete SS related records from the list of hex_lines
    for k in range (j - 1, i - 1, -1):
        hex_lines.remove(hex_lines[k])

    address = 0
    while ss_bytes != "":
        num_bytes = min (len(ss_bytes) // 2, max_bytes)
        hex_line = ":" + format(num_bytes, '02X') + format(address, '04X') + "00" + ss_bytes[:num_bytes * 2]
        hex_line += hex_line_checksum(hex_line) + "\n"
        address += num_bytes

        hex_lines.insert(num_lines, hex_line)
        num_lines += 1

        ss_bytes = ss_bytes[num_bytes * 2:]

    hex_file_out = open(hex_filename, "w")
    hex_file_out.writelines(hex_lines)

#
# Start processing command line parameters
#
if len(sys.argv) < 2:
    print("factory_config.py [hex file] [json file] [-tv:<name>:<type>:<value>] [-20706A2]")
    exit(0)

is_20706 = False
hex_present = False
bdaddr_present = False
json_present = False
json_filename = ""
num_app_tvs = 0
app_tvs = []
num_fw_tvs = 0
fw_tvs = []

for arg in sys.argv[1:]:
    if arg == "-20706A2":
        is_20706 = True
        continue

    if arg.find("-bda:") == 0:
        new_bda = arg[5:]
        assert (len(new_bda) == 12), "ERROR with bda length"
        bdaddr_present = True

    if arg.find("-tv:") == 0:
        parse_tv(arg[4:])
        num_app_tvs += 1
        continue

    file_extension = os.path.splitext(arg)

    if file_extension[1] == ".hex":
        hex_filename = arg
        hex_present = True

    elif file_extension[1] == ".json":
        json_filename = arg
        json_present = True

# If only hex file name present print the content of the static section of  the hex file
if hex_present and not json_present and num_app_tvs == 0 and not bdaddr_present:
    print_ss(hex_filename)
    exit(0)

# If hex file name present and -bda argument is given, update the BDADDR in the hex file
if hex_present and bdaddr_present:
    print ("Updating BDADDR to " + new_bda + " in " + hex_filename)
    set_bdaddr(hex_filename, new_bda)

# If only json file name present print the content of the JSON file
if json_present and not hex_present and num_app_tvs == 0:
    print_json(json_filename)
    exit(0)

# There are TVs in the command line and json file name, create a JSON file with the content from TVs
if num_app_tvs != 0 and json_present and not hex_present:
    with open(json_filename, 'w') as f:
        json.dump(app_tvs, f, indent=2)
    with open(json_filename, 'r') as f:
        print(f.read())
    exit(0)

# There are only TVs in the command line print out the content in JSON format
if num_app_tvs != 0 and not json_present and not hex_present:
    with open("___temp__.json", 'w') as f:
        json.dump(app_tvs, f, indent=2)
    with open("___temp__.json", 'r') as f:
        print(f.read())
    os.remove("___temp__.json")
    exit(0)

# There are only TVs in the command line print out the content in JSON format
if num_app_tvs != 0 and not json_present and hex_present:
    with open("___temp__.json", 'w') as f:
        json.dump(app_tvs, f, indent=2)
    hex_merge(hex_filename, "___temp__.json")
    os.remove("___temp__.json")
    exit(0)

# If hex file name present and json file present, convert json file info to binary and merge to hex file
if hex_present and json_present:
    hex_merge(hex_filename, json_filename)
    exit(0)

if not bdaddr_present:
    print("factory_config.py [hex file] [json file] [-tv:<name>:<type>:<value>] [-20706A2] [-bda:<value>]")
    exit(1)
