"""Core AGC CALC_Utilities Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

from py_2_and_3_compatibility import *

from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities

class CALC_Utilities_Nixi(CALC_Utilities):
    def calc_ook_ebno(self,model):
        # Set the EbN0 for OOK on Nixi as 19
        model.vars.ook_ebno.value = 19.0