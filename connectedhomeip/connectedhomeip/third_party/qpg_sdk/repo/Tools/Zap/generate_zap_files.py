#!/usr/bin/env python3

import argparse
import sys
import os
import logging
import shutil
import subprocess

DESCRIPTION = """\
1. Starting the ZAP GUI
2. Regenerate the .zap file
3. Generate corresponding source/header files
"""

SCRIPT_PATH = os.path.dirname(__file__)
ZAP_TOOLS_PATH = f"{SCRIPT_PATH}/../../../../gpHub/P236_CHIP/qorvo_patches/scripts/tools/zap"

#Check if we are in the package or in the Qorvo Env
if not os.path.isfile(os.path.join(SCRIPT_PATH, "..", "ota", "crypto_utils.py")):
    ZAP_TOOLS_PATH = os.getenv("MATTER_ZAP_TOOLS_PATH", ZAP_TOOLS_PATH)

def parse_command_line_arguments():
    """Parse command-line arguments"""
    def any_base_int(string):
        return int(string, 0)
    parser = argparse.ArgumentParser(description=DESCRIPTION)

    parser.add_argument("--input",
                        help="path to input .zap file",
                        required=True)

    parser.add_argument("--output",
                        help="Path to directory where headers/sources need to be generated",
                        required=True)

    parser.add_argument("--nogui",
                        help="Add this option if it is not needed to do configuration in the gui",
                        action='store_true')

    args = parser.parse_args()
    if not args.output:
        logging.error("Supply output directory")
        sys.exit(-1)
    else:
        assert os.path.isdir(args.output), f"The path specified as output is not a directory: {args.output}"

    return args

def run_script(command: str):
    """ run a python script using the current interpreter """
    subprocess.check_output(f"{sys.executable} {command}", shell=True)

def main():
    """ Main """

    args = parse_command_line_arguments()

    input_zap = os.path.abspath(args.input)
    output_zap = os.path.abspath(args.output)

    script_args = [f"{input_zap}"]

    if not args.nogui:
        subprocess.call([f"{ZAP_TOOLS_PATH}/run_zaptool.sh"] + script_args)

    run_script(f"{ZAP_TOOLS_PATH}/generate.py {input_zap}"
               f" -o {output_zap}")

if __name__ == "__main__":
    main()
