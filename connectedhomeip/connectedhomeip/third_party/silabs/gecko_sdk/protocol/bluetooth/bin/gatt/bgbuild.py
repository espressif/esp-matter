#!/usr/bin/env python3
# Copyright 2020- Silicon Laboratories Inc. www.silabs.com
#
# The licensor of this software is Silicon Laboratories Inc. Your use of this
# software is governed by the terms of Silicon Labs Master Software License
# Agreement (MSLA) available at
# www.silabs.com/about-us/legal/master-software-license-agreement.
# This software is distributed to you in Source Code format and is governed
# by the sections of the MSLA applicable to Source Code.

from jinja2 import Template
from jinja2 import FileSystemLoader
from jinja2.environment import Environment

from gatt import GattConst, GattService, GattCharacteristic, GattAggregateFormat, GattDescriptor, GattAttribute, GattValue, BluetoothUuid
from gattdb import GattDatabase
from gattxml import GattXmlError, GattXmlParser
import pprint
import binascii
import os
import collections
import traceback

dev_print = True
pp = pprint.PrettyPrinter(depth=6)
def devprint(msg):
    if dev_print:
        pp.pprint(msg)

def carray(value):
    txt="{ "
    if(value==None):
        return "NULL"
    for v in value:
        txt+='0x'+'%02x'%v+', '
            
    return txt+"}"
def carray2(value):
    txt=""
    if(value==None):
        return "NULL"
    for v in value:
        txt+='0x'+'%02x'%v+', '
            
    return txt

class GattDatabaseGenerator(object):
    def __init__(self, gattdb):
        self.gattdb = None
        self.attribute_rows = []
        self.set_gattdb(gattdb)

    def set_gattdb(self, gattdb):
        self.gattdb = gattdb

    def generate_header(self, template):
        env = Environment()
        # ensure POSIX path separators are used; Jinja2 seems to dislike everything else
        env.loader = FileSystemLoader(os.path.dirname(__file__).replace("\\", "/"))   
        with open(self.gattdb.header, "w") as f:
            f.write(env.get_template(template.replace("\\", "/")).render(db = self.gattdb))

    def generate(self, outdir=None):
        env = Environment()
        env.filters['carray'] = carray
        env.filters['carray2'] = carray2
        # ensure POSIX path separators are used; Jinja2 seems to dislike everything else
        env.loader = FileSystemLoader(os.path.dirname(__file__).replace("\\", "/"))
        if outdir:
            if not os.path.exists(outdir):
                os.makedirs(outdir)
            cfile = os.path.normpath(os.path.join(outdir, self.gattdb.cfile))
            hfile = os.path.normpath(os.path.join(outdir, self.gattdb.hfile))
        else:
            cfile = self.gattdb.cfile
            hfile = self.gattdb.hfile
        with open(cfile, "w") as f:
            f.write(env.get_template('gattdb.c.jinja').render(db = self.gattdb,GattConst=GattConst))
        with open(hfile, "w") as f:
            f.write(env.get_template('gattdb.h.jinja').render(db = self.gattdb,GattConst=GattConst))

    def generate_to_dict(self):
        """
        Generate ordered dictionary from GATT object.
        Dictionary holds the result of generation in the following form:
            {<path>:<content>, ...}
        """
        gd = {}
        try:
            env = Environment()
            env.filters['carray'] = carray
            env.filters['carray2'] = carray2
            # ensure POSIX path separators are used; Jinja2 seems to dislike everything else
            env.loader = FileSystemLoader(os.path.dirname(__file__).replace("\\", "/"))
            gd[self.gattdb.cfile] = env.get_template('gattdb.c.jinja').render(db = self.gattdb,GattConst=GattConst)
            gd[self.gattdb.hfile] = env.get_template('gattdb.h.jinja').render(db = self.gattdb,GattConst=GattConst)
            rd = {"success": True,
                  "message": ""}
        except:
            tr_type, tr_value, tr_traceback = sys.exc_info()
            gd = None
            rd = {"success": False,
                  "message": "".join(traceback.format_exception(tr_type, tr_value, tr_traceback))}
        finally:
            return gd, rd

def xml_to_dict(xmls):
    xmll = []
    for x in xmls:
        if os.path.isdir(x):
            xmll.extend([os.path.join(x, f) for f in os.listdir(x)])
        else:
            xmll.append(x)
    # maximum one .btconf file is allowed, although more is still tolerated
    # .btconf file(s) are moved to front of the list
    btconfl = [x for x in xmll if x.lower().endswith(".btconf")]
    contribl = [x for x in xmll if x.lower().endswith(".xml")]
    if len(btconfl) > 1:
        print("Warning: multiple 'btconf' files in list!")
    xmll = btconfl + contribl
    # add file paths and contents to dictionary
    od = collections.OrderedDict()
    for x in xmll:
        od[x] = GattXmlParser().to_string(x)
    return od

def dict_to_file(d, out=None):
    for k in d:
        o = os.path.join(out, k) if out else k
        with open(o, "w") as f:
            f.write(d[k])

def generate(orddict):
    """
    Public Studio API for BGBuild Generate
    [Input] ordered dictionary: {<xml path>: <xml content>, ...}
    [Returns] gen dict: {<output file path>: <output file content>, ...},
              res dict: {"success": <True|False>, "message": <message>}
    """
    # merge xmls if needed (more than one)
    gatt_str, ret = GattXmlParser().merge(orddict)
    if not ret["success"]:
        return {}, ret
    # parse xml
    gatt, ret = GattXmlParser().parse_string(gatt_str)
    if not ret["success"]:
        return {}, ret
    # run file generator
    gdbg = GattDatabaseGenerator(gatt)
    gen, ret = gdbg.generate_to_dict()
    if not ret["success"]:
        return {}, ret
    return gen, {"success": True, "message": ""}

if __name__ == '__main__':
    import sys
    import argparse
    parser = argparse.ArgumentParser(description='XML GATT database parser.')
    parser.add_argument('inputs',
                        nargs='+',
                        help='GATT XML files or directories to find XML files. Separate input with ;')
    parser.add_argument('-o', '--outdir',
                        help='the output directory.')

    args = parser.parse_args()
    try:
        od = xml_to_dict(args.inputs)
        gd, rd = generate(od)
        if not rd["success"]:
            print(rd["message"])
            sys.exit(1)
        dict_to_file(gd, args.outdir)
    except GattXmlError as e:
        print ('GATT XML error:', e.msg)

