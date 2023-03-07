"""Core AGC CALC_Profile_Base Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from pycalcmodel.core.variable import ModelVariableFormat
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator

from py_2_and_3_compatibility import *

class CALC_Profile_Base(ICalculator):

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """
        self._addModelVariable(model, 'base_frequency',  long, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'xtal_frequency',  int,  ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'channel_spacing', int,  ModelVariableFormat.DECIMAL)


    def calc_map_inputs(self, model):
        """
        the following function maps renamed variables into their previous names to avoid replacing
        them in the code base.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.xtal_frequency.value = int(model.vars.xtal_frequency_hz.value)
        model.vars.base_frequency.value = long(model.vars.base_frequency_hz.value)
        model.vars.channel_spacing.value = int(model.vars.channel_spacing_hz.value)
