"""This defines the CRC calculations and variables

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from pyradioconfig.parts.panther.calculators.calc_crc import CALC_CRC as CALC_CRC_panther

class CALC_CRC(CALC_CRC_panther):
    pass