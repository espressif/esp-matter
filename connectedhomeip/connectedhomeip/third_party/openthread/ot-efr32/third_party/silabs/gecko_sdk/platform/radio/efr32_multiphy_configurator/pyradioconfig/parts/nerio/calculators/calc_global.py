""" CALC_Global_jumbo Package

NERIO specific global calculator functions live here.

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be rturned by overriding the function:
        def getCalculationList(self):
"""

from pyradioconfig.parts.jumbo.calculators.calc_global import CALC_Global_jumbo
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_Global_nerio(CALC_Global_jumbo):

    def buildVariables(self, model):

        # Build variables from the Jumbo calculations
        super().buildVariables(model)

        #Add some variables for Nerio
        self._addModelRegister(model, 'MODEM.CTRL2.RATESELMODE',         int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.CTRL6.CODINGB',             int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE.LRCORRTHD',       int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE.LRTIMCORRTHD',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE.LRCORRSCHWIN',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE.LRBLE',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LRFRC.CI500',               int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LRFRC.FRCACKTIMETHD',       int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE.LRDEC',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE.LRBLEDSA',        int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE1.LRSS',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE1.LRTIMEOUTTHD',   int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE6.LRCHPWRSPIKETH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE6.LRSPIKETHD',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE1.LRSPIKETHADD',   int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE1.CHPWRACCUDEL',   int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE1.HYSVAL',         int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE1.AVGWIN',         int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE2.LRCHPWRTH1',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE2.LRCHPWRTH2',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE2.LRCHPWRTH3',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE2.LRCHPWRTH4',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE3.LRCHPWRTH5',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE3.LRCHPWRTH6',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE3.LRCHPWRTH7',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE3.LRCHPWRTH8',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE4.LRCHPWRTH9',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE4.LRCHPWRTH10',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE4.LRCHPWRSH1',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE4.LRCHPWRSH2',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE4.LRCHPWRSH3',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE4.LRCHPWRSH4',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH5',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH6',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH7',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH8',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH9',     int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH10',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.LONGRANGE5.LRCHPWRSH11',    int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'FRC.FCD0.EXCLUDESUBFRAMEWCNT',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.EXCLUDESUBFRAMEWCNT',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.EXCLUDESUBFRAMEWCNT',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.EXCLUDESUBFRAMEWCNT',    int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.CTRL.RATESELECT',             int, ModelVariableFormat.HEX)

        # This register field was added specifically to enable the firmware fix for RAIL_LIB-2563
        self._addModelRegister(model, 'SEQ.MISC.BLE_VITERBI_FIX_EN',     int, ModelVariableFormat.HEX)
