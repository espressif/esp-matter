#!/usr/bin/env python

from fileinput import filename
import os
import json
import re
from jinja2 import Environment, FileSystemLoader
import argparse

def get_input_output_path():

    cwd = os.getcwd()
    parser = argparse.ArgumentParser(
        description="Wi-SUN Configurator code generator"
    )
    parser.add_argument("input", nargs="?", default=cwd)
    parser.add_argument("-o",dest="output", nargs="?", default=cwd)

    args = parser.parse_args()

    # gets the .wisunconf file
    if os.path.isdir(args.input):
        input_filename = next(
            x for x in os.listdir(args.input) if x.lower().endswith(".wisunconf")
        )
        if not input_filename:
            print(f"WisunConfGenerator: No 'wisunconf' file found in {args.input}.")
            exit(1)
        input_path = os.path.join(args.input, input_filename)
    else:
        input_path = args.input
    output_path = args.output

    return input_path, output_path

def config_generate(config, template_dir:str, c_jinja_file_name:str, h_jinja_file_name:str):

    file_loader = FileSystemLoader(template_dir)
    env = Environment(loader=file_loader)

    # getting mac address
    mac_address = []
    if config['macAddress'] is not None:
        mac_address = re.findall("[a-fA-F0-9][a-fA-F0-9]", config['macAddress'])
    # getting mac address list
    mac_list = []
    for mac_list_address in config['macList']:
        mac_list.append(re.findall("[a-fA-F0-9][a-fA-F0-9]", mac_list_address))

    template = env.get_template(c_jinja_file_name)
    c_output = template.render(config=config, macAddress=mac_address, macList=mac_list)

    template = env.get_template(h_jinja_file_name)
    h_output = template.render(config=config, macAddress=mac_address, macList=mac_list)

    return c_output, h_output

def get_wisun_config(path):

    with open(path) as f:
        conf = json.load(f)

    return conf

if __name__ == "__main__":

    print("--------WisunConfGenerator---------")

    # getting paths
    input_path, output_path = get_input_output_path()

    # getting confiuration as a dictionary
    wisunconf = get_wisun_config(path=input_path)

    # getting the tabs' statuses
    only_radio_tab = wisunconf['onlyRadioTab']

    # checking whether the generation is necessary or not
    if not only_radio_tab:
        # using jinja templates it generates the source fils' strings
        wisun_config_c_file, wisun_config_h_file = config_generate(config=wisunconf,
                                                                template_dir="../templates",
                                                                c_jinja_file_name="sl_wisun_config.c.jinja",
                                                                h_jinja_file_name="sl_wisun_config.h.jinja")

        # gets the source files accurate location
        config_c_path = os.path.join(output_path, "sl_wisun_config.c")
        config_h_path = os.path.join(output_path, "sl_wisun_config.h")

        # writes the generated strings into files
        with open(config_c_path, "w") as f:
            f.write(wisun_config_c_file)
        with open(config_h_path, "w") as f:
            f.write(wisun_config_h_file)

        print(f"Wi-SUN config: {input_path}")
        print(f"Generated files:\r\n"
            "  sl_wisun_config.c\r\n"
            "  sl_wisun_config.h")
    else:
        print("Because the application and the security configuration is disabled, no generated files is needed.")

    print("-----------------------------------")