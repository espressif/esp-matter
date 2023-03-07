"""Core CALC_Radio Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from pyradioconfig.parts.panther.calculators.calc_radio import CALC_Radio_panther


class CALC_Radio_lynx(CALC_Radio_panther):
    pass