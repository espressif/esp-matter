"""Core AGC Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""
from pyradioconfig.parts.jumbo.calculators.calc_shaping import CALC_Shaping_jumbo

class CALC_Shaping_nerio(CALC_Shaping_jumbo):

    pass