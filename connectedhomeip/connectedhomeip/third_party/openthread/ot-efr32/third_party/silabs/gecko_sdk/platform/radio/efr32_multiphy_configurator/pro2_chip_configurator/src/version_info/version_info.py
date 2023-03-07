'''
Created on Mar 6, 2013

@author: sesuskic
'''

import sys, os, sys
from argparse import ArgumentParser
sys.path.append('src')
from pro2_chip_configurator.src.common import Properties

def _get_file_path():
	base_path = ""
	if hasattr(sys, "frozen"):
		base_path = os.path.dirname(sys.executable)
	else:
		base_path = os.path.abspath(os.path.dirname(__file__))
	return base_path + "/master.properties"

def get_version_info():
	version_props = Properties()
	with open(_get_file_path()) as f:
		version_props.load(f)
	return version_props.getPropertyDict()

def get_version_string():
	version_props = Properties()
	with open(_get_file_path()) as f:
		version_props.load(f)
	return version_props.getProperty("ChipConfigVersion") + "." + version_props.getProperty("ChipConfigBuildNumber")

def set_version_info_build_number(build_number):
	chip_config_version = get_version_info()["ChipConfigVersion"]
	version_props = Properties()
	with open(_get_file_path(), 'w') as f:
		version_props.setProperty("ChipConfigVersion", chip_config_version)
		version_props.setProperty("ChipConfigBuildNumber", build_number)
		version_props.store(f)

def main():
	os.environ["ChipConfigVersion"] = get_version_info()["ChipConfigVersion"]
	parser = ArgumentParser()
	parser.add_argument("--setbuildnumber")
	args = parser.parse_args()
	if args.setbuildnumber is None:
		print(get_version_info()["ChipConfigVersion"] + "." + get_version_info()["ChipConfigBuildNumber"])
		return
	set_version_info_build_number(args.setbuildnumber)

if __name__ == '__main__':
	main()
