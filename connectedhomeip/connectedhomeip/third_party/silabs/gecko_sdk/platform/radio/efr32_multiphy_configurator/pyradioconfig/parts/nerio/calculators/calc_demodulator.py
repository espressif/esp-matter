""" CALC_Demodulator_jumbo Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be rturned by overriding the function:
        def getCalculationList(self):
"""

from pyradioconfig.parts.jumbo.calculators.calc_demodulator import CALC_Demodulator_jumbo

class CALC_Demodulator_nerio(CALC_Demodulator_jumbo):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0
