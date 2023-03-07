"""Core CALC_Radio Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""
from pyradioconfig.parts.jumbo.calculators.calc_radio import CALC_Radio_jumbo


class CALC_Radio_nerio(CALC_Radio_jumbo):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0
