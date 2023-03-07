"""
The project is organized into the following structure:\n
\n
*  calculator_model_franework - Where the high level calc managers\n
*  parts - Where all and part specific phys, profiles and calculators are defined\n
*  to_be_deprecated - Legacy code that should be ported into new framework, but other projects still depends on\n
*  pycalcmodel - Git Subbmodule link to pyclcmodel commit that is needed in this version\n
*  unit_tests - Unit tests\n
\n
Please see the individual readme files in each dir for more information.\n
\n
main.py - Is the Executable interface\n
"""
from pyradioconfig._version import __version__
from pyradioconfig.calculator_model_framework.CalcManager import CalcManager
# from pyradioconfig.to_be_deprecated.legacy_top_level import *
