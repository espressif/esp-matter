"""
This file defines calculations needed for RAIL firmware
"""
"""
Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException


class CalcRail(ICalculator):

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

        """
        #Inputs
        """

        self._addModelVariable(model,    'rx_sync_delay_ns'         ,    int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rx_eof_delay_ns'          ,    int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'tx_eof_delay_ns'          ,    int, ModelVariableFormat.DECIMAL)

    def calc_rail_delays(self, model):
        """calc_rail_delays

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        # On chips that don't calculate anything, our best guess is no delay
        model.vars.rx_sync_delay_ns.value = 0
        model.vars.rx_eof_delay_ns.value = 0
        model.vars.tx_eof_delay_ns.value = 0

