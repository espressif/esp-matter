""" CALC_Viterbi Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be rturned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
import itertools
from pyradioconfig.parts.common.calculators.calc_demodulator import CALC_Demodulator
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities
from pyradioconfig.parts.common.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_DSA(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """
        #self._addModelActual(model,   'agcperiod'             ,float, ModelVariableFormat.DECIMAL)
        #self._addModelVariable(model, 'agc_settling_delay'    ,  int, ModelVariableFormat.DECIMAL, desc='Delay between two AGC gain adjustments in AGC clock cycles'	)



    def calc_dsamode_reg(self, model):

        if model.vars.dsa_enable.value == True:
            enable = 1
        else:
            enable = 0

        self._reg_write(model.vars.MODEM_DSACTRL_DSAMODE, enable)


    def calc_arrthd_reg(self, model):

        preamble_len = model.vars.preamble_length.value

        if preamble_len < 12:
            arrthd = 4
        else:
            arrthd = 7

        self._reg_write(model.vars.MODEM_DSACTRL_ARRTHD, arrthd)


