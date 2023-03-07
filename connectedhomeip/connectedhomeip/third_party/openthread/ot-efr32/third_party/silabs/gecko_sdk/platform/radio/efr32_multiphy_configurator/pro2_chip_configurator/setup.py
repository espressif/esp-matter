'''
Created on Mar 6, 2013

@author: sesuskic
'''

import sys
sys.path.append('src')
from distutils.core import setup
import py2exe
import os
from version_info import version_info

setup(version = version_info.get_version_string(),
      options = {"py2exe": {"optimize": 2,
                            "compressed": 1}},
      zipfile = "lib/library.zip",
      console=['src/modem_calc_tb.py'])