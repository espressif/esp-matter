"""
This file defines invalid input combinations

"""
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

"""
Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum, ModelVariableEmptyValue, ModelVariableInvalidValueType

class CALC_Errors(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        return

    def calc_block_white_errors(self, model):
        """_calc_block_white_errors
        If the user chose frame coding that uses block coder and also enabled
        whitening throw up an error because dumbo/jumbo cannot do both at the
        same time

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        if ((model.vars.frame_coding.value != model.vars.frame_coding.var_enum.NONE) and
            (model.vars.frame_coding.value != model.vars.frame_coding.var_enum.UART_NO_VAL) and
            (model.vars.white_poly.value != model.vars.white_poly.var_enum.NONE)):
            raise CalculationException("Cannot enable frame coding and whitening at the same time on this chip")
