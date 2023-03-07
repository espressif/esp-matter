"""CALC_Firmware_Functions Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be rturned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

class CALC_Firmware_Functions(ICalculator):

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

        # Variable name is the name of the DUT function to call
        var = self._addModelVariable(model, 'IR_CAL_StandardSet', Enum, ModelVariableFormat.DECIMAL, desc='The RPC config function to select the IRCAL value from CC page')
        member_data = [
            ['RADIO_IRCAL_SET_2P4_BLE' , 0, 'Select 2.4GHz BLE standard' ],
            ['RADIO_IRCAL_SET_2P4_IEEE802154',  1, 'Select 2.4 GHz ZigBee standard'],
            ['RADIO_IRCAL_SET_SUBGIG', 2, 'Select proprietary sub-GHz standard'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'RADIO_IRCalStd_t',
            'An enum to select the IR CAL settings.',
            member_data)


    def calc_IR_CAL_StandardSet(self, model):
        """This is just an example to show how to use function variables,

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # TODO: Fix this example hack.  For demonstraetion purposes only

        # Variable value is the function parameter (only one allowed)
        #model.vars.IR_CAL_StandardSet.value = model.vars.IR_CAL_StandardSet.var_enum.RADIO_IRCAL_SET_2P4_BLE

        # Setting value to 'None' means the function will not be called
        #model.vars.IR_CAL_StandardSet.value = None
        pass

