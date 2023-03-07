from pyradioconfig.parts.common.calculators.calc_ircal import CALC_IrCal
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from py_2_and_3_compatibility import *

class CALC_IrCal_Ocelot(CALC_IrCal):

    def calc_ircal_auxpll_dividers(self, model):
        auxdividers = {2419000000: (1, 63),  # For Lodiv = 1 Not currently used by the IRCal algorithm.
                       1306000000 : ( 2, 68),  # For Lodiv = 2 Untested
                       909000000: (3, 71),  # For Lodiv = 3
                       883000000: (3, 69),
                       # For Lodiv = 3 If the intent was to be near the 868 band, why wasn't (3,68) used to get 870?
                       643000000: (4, 67),  # For Lodiv = 4 Untested
                       486000000: (6, 76),  # For Lodiv = 5
                       435000000: (6, 68),  # For Lodiv = 6
                       365000000: (6, 58),  # For Lodiv = 7 # Added to support the 390MHz band.  Untested.
                       314000000: (6, 49),  # For Lodiv = 8
                       288000000: (8, 60),  # For Lodiv = 9 Untested
                       256000000: (9, 60),  # For Lodiv =10 Untested
                       214000000: (12, 67),  # For Lodiv =12 Untested
                       186000000: (12, 58),  # For Lodiv =14 Untested
                       170000000: (12, 53),  # For Lodiv =15
                       160000000: (12, 50),  # For Lodiv =16 Untested
                       143000000: (18, 67),  # For Lodiv =18 Untested
                       128000000: (18, 60),  # For Lodiv =20 Untested
                       124000000: (18, 58),  # For Lodiv =21 Untested
                       107000000: (18, 50),  # For Lodiv =24 Untested
                       102000000: (18, 48),  # For Lodiv =25 Untested
                       96000000 : (18, 45),  # For Lodiv =27 Untested
                       92000000 : (18, 43),  # For Lodiv =28 Untested
                       }
        # Per spec sheet \\silabs.com\design\project\ip_em_90nm\tsmc90f_ull\modules\aux_pll\docs\specs\aux_pll_stop.docx
        # and tests in ESVALID-1673

        rfFreq = model.vars.base_frequency_actual.value
        rfFreqMin = model.vars.tuning_limit_min.value
        rfFreqMax = model.vars.tuning_limit_max.value

        smallestDifference = sys.maxint  # large number
        closestBandFreq = None

        for bandFreq in auxdividers.keys():
            if (bandFreq < rfFreqMax) and (bandFreq > rfFreqMin):
                difference = abs(rfFreq - bandFreq)
                if difference < smallestDifference:
                    smallestDifference = difference
                    closestBandFreq = bandFreq

        # If we can't
        if closestBandFreq is not None:
            (auxLODiv, auxNDiv) = auxdividers[closestBandFreq]
            model.vars.ircal_auxndiv.value = auxNDiv
            model.vars.ircal_auxlodiv.value = auxLODiv
        else:
            if rfFreq > 100000000:
                # This is just to allow one existing 50MHz phy to not generate an error.
                # There are no valid values that would work for this frequency for the IRCal
                # alorithm.  The Aux pll cannot tune that low.
                # That phy probably should just be removed.  It's only used for TX CW testing.
                # We can accomplish the same thing using something like a 100MHz CW phy with
                # the frequency overridden.  For now we can let it go, since the gui would not
                # allow that phy to be created anyway.  The gui limits the input frequency to
                # a minimum value of 100MHz.
                raise CalculationException(
                    "Unexpected frequency band found.  Unable to calculate IR calibration values.")