from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

class Calc_AoX_Bobcat(ICalculator):
    ###AoX Calculations###

    def buildVariables(self, model):
        var = self._addModelVariable(model, 'aox_enable', Enum, ModelVariableFormat.DECIMAL, units='',
                               desc='Enables AoX settings')

        member_data = [
            ['DISABLED', 0, 'AoX Disabled'],
            ['ENABLED', 1, 'AoX Enabled'],
        ]

        var.var_enum = CreateModelVariableEnum(
            'AoxEnableEnum',
            'AoX Enable/Disable Selection',
            member_data)

    def calc_aox_enable(self, model):
        #Disable by default
        model.vars.aox_enable.value = model.vars.aox_enable.var_enum.DISABLED

    def calc_timeperiod_reg(self, model):
        TIMEPERIOD_FRACTIONAL_BITS = 24
        xtal_frequency_hz = model.vars.xtal_frequency.value
        timeperiod = int(2**TIMEPERIOD_FRACTIONAL_BITS / (xtal_frequency_hz / 1e6))

        self._reg_write(model.vars.MODEM_ANTSWCTRL1_TIMEPERIOD, timeperiod)

    def calc_aox_misc(self, model):
        aox_enable = True if model.vars.aox_enable.value == model.vars.aox_enable.var_enum.ENABLED else False

        if aox_enable:
            chfswsel = 2 # CHFSWTRIG
            disafcsupp = 1 # disable AFC during the CTE
            chfswtrig = 1 # clk cycles to trigger after ets_set_mux, must be non-zero
        else:
            chfswsel = 0
            disafcsupp = 0
            chfswtrig = 0

        self._reg_write(model.vars.MODEM_CHFCTRL_CHFSWSEL, chfswsel)
        self._reg_write(model.vars.MODEM_CHFSWCTRL_CHFSWTIME, chfswtrig)
        self._reg_write(model.vars.MODEM_AFC_DISAFCCTE, disafcsupp)

