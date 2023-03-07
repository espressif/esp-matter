"""CALC_Firmware Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_Firmware(ICalculator):

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

        # Output fields
        self._addModelRegister(model, 'MODEM.RAMPLEV.RAMPLEV0'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.RAMPLEV.RAMPLEV1'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.RAMPLEV.RAMPLEV2'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.LPFCTRL.LPFBW'               , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'CRC.CMD.INITIALIZE'              , int, ModelVariableFormat.HEX )



    def calc_reserve_fields(self, model):
        """calc_reserve_fields

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        if model.part_family.lower() in ["dumbo", "jumbo", "nerio", "nixi"]:
            # skip this for Panther as the preference was to manually re-write the reset values         
            # MCUW_RADIO_CFG-734 Panther: PHY/MAC Design requests more registers added (all MODEM, AGC, etc.) with re-writes of reset values
            model.vars.MODEM_RAMPLEV_RAMPLEV0.value = None
            model.vars.MODEM_RAMPLEV_RAMPLEV1.value = None
            model.vars.MODEM_RAMPLEV_RAMPLEV2.value = None
        model.vars.RAC_LPFCTRL_LPFBW.value = None
        model.vars.CRC_CMD_INITIALIZE.value = None



