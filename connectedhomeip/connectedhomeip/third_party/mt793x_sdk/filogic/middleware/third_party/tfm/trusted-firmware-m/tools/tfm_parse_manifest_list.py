#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

import os
import io
import sys
import argparse
from jinja2 import Environment, BaseLoader, select_autoescape

try:
    import yaml
except ImportError as e:
    print (str(e) + " To install it, type:")
    print ("pip install PyYAML")
    exit(1)

donotedit_warning = \
                    "/*********** " + \
                    "WARNING: This is an auto-generated file. Do not edit!" + \
                    " ***********/"

DEFAULT_MANIFEST_LIST = os.path.join('tools', 'tfm_manifest_list.yaml')
DEFAULT_GEN_FILE_LIST = os.path.join('tools', 'tfm_generated_file_list.yaml')

OUT_DIR = None # The root directory that files are generated to

class TemplateLoader(BaseLoader):
    """
    Template loader class.

    An instance of this class is passed to the template engine. It is
    responsible for reading the template file
    """
    def __init__(self):
        pass

    def get_source(self, environment, template):
        """
        This function reads the template files.
        For detailed documentation see:
        http://jinja.pocoo.org/docs/2.10/api/#jinja2.BaseLoader.get_source

        Please note that this function always return 'false' as 'uptodate'
        value, so the output file will always be generated.
        """
        if not os.path.isfile(template):
            raise TemplateNotFound(template)
        with open(template) as f:
            source = f.read()
        return source, template, False

def process_manifest(manifest_list_file, append):
    """
    Parse the input manifest, generate the data base for genereated files
    and generate manifest header files.

    Parameters
    ----------
    manifest_list_file:
        The manifest list to parse.
    append:
        To append the manifest to original or not.

    Returns
    -------
    The manifest header list and the data base.
    """

    db = []
    manifest_header_list = []
    manifest_list = []

    if append:
        # Load the default manifest first
        with open(DEFAULT_MANIFEST_LIST) as default_manifest_list_yaml_file:
            manifest_dic = yaml.safe_load(default_manifest_list_yaml_file)
            manifest_list.extend(manifest_dic["manifest_list"])

    with open(manifest_list_file) as manifest_list_yaml_file:
        manifest_dic = yaml.safe_load(manifest_list_yaml_file)
        manifest_list.extend(manifest_dic["manifest_list"])

    templatefile_name = 'secure_fw/services/manifestfilename.template'
    template = ENV.get_template(templatefile_name)

    for manifest_item in manifest_list:
        manifest_path = os.path.expandvars(manifest_item['manifest'])
        file = open(manifest_path)
        manifest = yaml.safe_load(file)

        db.append({"manifest": manifest, "attr": manifest_item})

        utilities = {}
        utilities['donotedit_warning']=donotedit_warning

        context = {}
        context['manifest'] = manifest
        context['attr'] = manifest_item
        context['utilities'] = utilities

        manifest_dir, manifest_name = os.path.split(manifest_path)
        outfile_name = manifest_name.replace('yaml', 'h').replace('json', 'h')
        context['file_name'] = outfile_name.replace('.h', '')
        outfile_name = os.path.join(manifest_dir, "psa_manifest", outfile_name).replace('\\', '/')

        manifest_header_list.append(outfile_name)

        if OUT_DIR is not None:
            outfile_name = os.path.join(OUT_DIR, outfile_name)

        outfile_path = os.path.dirname(outfile_name)
        if not os.path.exists(outfile_path):
            os.makedirs(outfile_path)

        print ("Generating " + outfile_name)

        outfile = io.open(outfile_name, "w", newline='\n')
        outfile.write(template.render(context))
        outfile.close()

    return manifest_header_list, db

def gen_files(context, gen_file_list, append):
    """
    Generate files according to the gen_file_list

    Parameters
    ----------
    gen_file_list:
        The list of files to generate
    append:
        To append the manifest to original or not
    """
    file_list = []

    if append:
        # read default file list first
        with open(DEFAULT_GEN_FILE_LIST) as file_list_yaml_file:
            file_list_yaml = yaml.safe_load(file_list_yaml_file)
            file_list.extend(file_list_yaml["file_list"])

    with open(gen_file_list) as file_list_yaml_file:
        # read list of files that need to be generated from templates using db
        file_list_yaml = yaml.safe_load(file_list_yaml_file)
        file_list.extend(file_list_yaml["file_list"])

    for file in file_list:
        outfile_name = os.path.expandvars(file["output"])
        templatefile_name = os.path.expandvars(file["template"])

        if OUT_DIR is not None:
            outfile_name = os.path.join(OUT_DIR, outfile_name)

        outfile_path = os.path.dirname(outfile_name)
        if not os.path.exists(outfile_path):
            os.makedirs(outfile_path)

        template = ENV.get_template(templatefile_name)

        outfile = io.open(outfile_name, "w", newline='\n')
        outfile.write(template.render(context))
        outfile.close()

    print ("Generation of files done")

def parse_args():
    parser = argparse.ArgumentParser(description='Parse secure partition manifest list and generate files listed by the file list')
    parser.add_argument('-o', '--outdir'
                        , dest='outdir'
                        , required=False
                        , default=None
                        , metavar='out_dir'
                        , help='The root directory for generated files, the default is TF-M root folder.')

    parser.add_argument('-m', '--manifest'
                        , nargs='*'
                        , dest='manifest_args'
                        , required=False
                        , default=[]
                        , metavar='manifest'
                        , help='The secure partition manifest list file to parse, the default is '+ DEFAULT_MANIFEST_LIST + '. \
                                Or the manifest can be append to the default one by explicitly \"append\" it:\
                                -m manifest_to_append append')

    parser.add_argument('-f', '--file-list'
                        , nargs='*'
                        , dest='gen_file_args'
                        , required=False
                        , default=[]
                        , metavar='file-list'
                        , help='The file descripes the file list to generate, the default is ' + DEFAULT_GEN_FILE_LIST + '. \
                                Or the file list can be append to the default one by explicitly \"append\" it:\
                                -f files_to_append append')

    args = parser.parse_args()
    manifest_args = args.manifest_args
    gen_file_args = args.gen_file_args

    if len(manifest_args) > 2 or len(gen_file_args) > 2:
        parser.print_help()
        exit(1)

    if len(manifest_args) == 2 and (manifest_args[1] != 'append' and manifest_args[1] != ''):
        parser.print_help()
        exit(1)

    if len(gen_file_args) == 2 and (gen_file_args[1] != 'append' and gen_file_args[1] != ''):
        parser.print_help()
        exit(1)

    return args

ENV = Environment(
        loader = TemplateLoader(),
        autoescape = select_autoescape(['html', 'xml']),
        lstrip_blocks = True,
        trim_blocks = True,
        keep_trailing_newline = True
    )

def main():
    """
    The entry point of the script.

    Generates the output files based on the templates and the manifests.
    """

    global OUT_DIR

    args = parse_args()

    manifest_args = args.manifest_args
    gen_file_args = args.gen_file_args
    OUT_DIR = args.outdir
    append_manifest = False
    append_gen_file = False

    if len(manifest_args) == 2 and manifest_args[1] == 'append':
        append_manifest = True

    if len(gen_file_args) == 2 and gen_file_args[1] == 'append':
        append_gen_file = True

    if len(manifest_args) == 0:
        manifest_list = DEFAULT_MANIFEST_LIST
    else:
        """
        Only convert to abs path when value is not default
        Because the default value is a fixed relative path to TF-M root folder,
        it will be various to different execution path if converted to absolute path.
        The same for gen_file_list
        """
        manifest_list = os.path.abspath(args.manifest_args[0])
    if len(gen_file_args) == 0:
        gen_file_list = DEFAULT_GEN_FILE_LIST
    else:
        gen_file_list = os.path.abspath(args.gen_file_args[0])

    # Arguments could be relative path.
    # Convert to absolute path as we are going to change diretory later
    if OUT_DIR is not None:
        OUT_DIR = os.path.abspath(OUT_DIR)

    """
    Relative path to TF-M root folder is supported in the manifests
    and default value of manifest list and generated file list are relative to TF-M root folder as well,
    so first change directory to TF-M root folder.
    By doing this, the script can be executed anywhere
    The script is located in <TF-M root folder>/tools, so sys.path[0]<location of the script>/.. is TF-M root folder.
    """
    os.chdir(os.path.join(sys.path[0], ".."))

    manifest_header_list, db = process_manifest(manifest_list, append_manifest)

    utilities = {}
    context = {}

    utilities['donotedit_warning']=donotedit_warning
    utilities['manifest_header_list']=manifest_header_list

    context['manifests'] = db
    context['utilities'] = utilities

    gen_files(context, gen_file_list, append_gen_file)

if __name__ == "__main__":
    main()
