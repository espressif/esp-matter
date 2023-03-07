from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_longrange_Jumbo(ICalculator):

    def buildVariables(self, model):
        var = self._addModelVariable(model, 'longrange_mode', Enum, ModelVariableFormat.DECIMAL, 'Long Range Mode')
        member_data = [
            ['LR_1p2k', 0, 'OQPSK DSSS SF8 1.2k long range'],
            ['LR_2p4k', 1, 'OQPSK DSSS SF8 2.4k long range'],
            ['LR_4p8k', 2, 'OQPSK DSSS SF8 4.8k long range'],
            ['LR_9p6k', 3, 'OQPSK DSSS SF8 9.6k long range'],
            ['LR_19p2k', 4, 'OQPSK DSSS SF8 19.2k long range'],
            ['LR_80k', 5, 'OQPSK DSSS SF8 80k FCC 15.247 long range'],
            ]
        var.var_enum = CreateModelVariableEnum(
            'LongRangeModeEnum',
            'List of supported LongRange Modes',
            member_data)