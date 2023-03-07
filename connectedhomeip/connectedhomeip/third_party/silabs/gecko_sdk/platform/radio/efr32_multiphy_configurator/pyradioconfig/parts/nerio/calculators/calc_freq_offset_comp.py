"""Core CALC_Freq_Offset_Comp Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from pyradioconfig.parts.jumbo.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp_jumbo

class CALC_Freq_Offset_Comp_nerio(CALC_Freq_Offset_Comp_jumbo):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0
