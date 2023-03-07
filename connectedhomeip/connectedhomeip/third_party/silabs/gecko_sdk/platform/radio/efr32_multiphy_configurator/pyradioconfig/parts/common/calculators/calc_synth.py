"""Core CALC_Synth Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

from py_2_and_3_compatibility import *

class CALC_Synth(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

        self.synth_freq_min_limit = long(2300000000)
        self.synth_freq_max_limit = long(2900000000)

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Inputs
        self._addModelVariable(model,  'channel_spacing_hz',   int,     ModelVariableFormat.DECIMAL, units='Hz', desc='Channel raster used for relative frequency configuration')
        self._addModelActual  (model,  'channel_spacing',      int,     ModelVariableFormat.DECIMAL)
        
        self._addModelRegister(model, 'SYNTH.CALOFFSET.CALOFFSET'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CHCTRL.CHNO'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CHSP.CHSP'                , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.DIVCTRL.LODIVFREQCTRL'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.FREQ.FREQ'                ,long, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.IFFREQ.IFFREQ'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.IFFREQ.LOSIDE'            , int, ModelVariableFormat.HEX )
                                              
        # These aren't really synth registers, but they must follow the synth register values,
        # so we set them here.
        self._addModelRegister(model, 'MODEM.MIXCTRL.MODE'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.MIXCTRL.DIGIQSWAPEN'      , int, ModelVariableFormat.HEX )

        # Tuning limits for the given LODIV setting
        self._addModelVariable(model, 'tuning_limit_min', long, ModelVariableFormat.DECIMAL, units='Hz', desc='Minimum center frequency allowed for the current configuration.')
        self._addModelVariable(model, 'tuning_limit_max', long, ModelVariableFormat.DECIMAL, units='Hz', desc='Maximum center frequency allowed for the current configuration.')

        # A proper flag to determine sub-GHz
        self._addModelVariable(model, 'subgig_band', bool, ModelVariableFormat.DECIMAL, desc="Flag for sub-GHz")

        var = self._addModelVariable(model, 'lo_injection_side', Enum, ModelVariableFormat.DECIMAL, 'Possible LO injection sides')
        member_data = [
            ['HIGH_SIDE' , 0, 'The local oscillator (LO) is higher in frequency than the receive RF channel'],
            ['LOW_SIDE',  1, 'The local oscillator (LO) is lower in frequency than the receive RF channel.'],

        ]
        var.var_enum = CreateModelVariableEnum(
            'LoInjectionSideEnum',
            'List of supported LO injection side configurations',
            member_data)

    #TODO: add AFC adjustment term to rx_synth_freq once AFC is implemented
    def calc_rx_synth_freq_actual(self, model):
        """
        calculate synthesizer frequency for RX
        Equation (5.31) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        chan0_freq = model.vars.SYNTH_FREQ_FREQ.value
        chno = model.vars.SYNTH_CHCTRL_CHNO.value
        chan_spacing = model.vars.SYNTH_CHSP_CHSP.value
        cal_offset = model.vars.SYNTH_CALOFFSET_CALOFFSET.value
        if_freq = model.vars.SYNTH_IFFREQ_IFFREQ.value
        loside = model.vars.SYNTH_IFFREQ_LOSIDE.value
        res = model.vars.synth_res_actual.value
        lodiv = model.vars.lodiv_actual.value

        if loside:
            rx_synth_freq = (chan0_freq + chno * chan_spacing + cal_offset + if_freq) * res * lodiv
        else:
            rx_synth_freq = (chan0_freq + chno * chan_spacing + cal_offset - if_freq) * res * lodiv

        model.vars.rx_synth_freq_actual.value = long(round(rx_synth_freq))


    #TODO: add AFC adjustment term to tx_synth_freq once AFC is implemented
    #TODO: add calbiration offset to tx_synth_freq
    def calc_tx_synth_freq_actual(self, model):
        """
        calculate synthesizer frequency for TX
        Equation (5.32) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        chan0_freq = model.vars.SYNTH_FREQ_FREQ.value
        chno = model.vars.SYNTH_CHCTRL_CHNO.value
        chan_spacing = model.vars.SYNTH_CHSP_CHSP.value
        cal_offset = model.vars.SYNTH_CALOFFSET_CALOFFSET.value
        res = model.vars.synth_res_actual.value
        lodiv = model.vars.lodiv_actual.value

        tx_synth_freq = (1.0 * chan0_freq + chno * chan_spacing + cal_offset) * res * lodiv

        model.vars.tx_synth_freq_actual.value = long(tx_synth_freq)

    # This compares the synth frequency against limits.  I don't know the source of the
    # synth min and max frequencies, but I got this from Rail C source code:
    #
    # define VCO_MAX_FREQ           2900000000
    # define VCO_MIN_FREQ           2300000000
    #
    def calc_check_synth_limits(self, model):

        tx_synth_freq_min = model.vars.tx_synth_freq_actual.value
        tx_synth_freq_max = tx_synth_freq_min           # We don't have min and max yet.

        rx_synth_freq_min = model.vars.rx_synth_freq_actual.value
        rx_synth_freq_max = rx_synth_freq_min           # We don't have min and max yet.

        synth_freq_min = min(rx_synth_freq_min, tx_synth_freq_min)
        synth_freq_max = max(rx_synth_freq_max, tx_synth_freq_max)

        if (synth_freq_max > self.synth_freq_max_limit):
            raise CalculationException("%s Hz above maximum synth frequency limit!" % synth_freq_max)

        if (synth_freq_min < self.synth_freq_min_limit):
            raise CalculationException("%s Hz below minimum synth frequency limit!" % synth_freq_min)

    def calc_synth_res_actual(self, model):
        """
        calculate synthesizer frequency resolution
        Equation (5.33) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value * 1.0
        lodiv = model.vars.lodiv_actual.value

        # calculate frequency resolution
        res = fxo / lodiv / pow(2, 19)

        model.vars.synth_res_actual.value = res

    def calc_base_frequency_actual(self, model):
        """
        calculate the actual base (RF) frequency
        Equation (5.34) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        chan0_freq_reg = model.vars.SYNTH_FREQ_FREQ.value
        res = model.vars.synth_res_actual.value

        ch0_freq_hz = py2round(chan0_freq_reg * res)

        model.vars.base_frequency_actual.value = long(ch0_freq_hz)


    def calc_chsp_freq_reg(self, model):
        """
        calculate channel spacing and frequency register settings
        Equation (5.34) of EFR32 Reference Manual (internal.pdf)
        Equation (5.35) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        ch_spacing = model.vars.channel_spacing.value * 1.0
        f0 = model.vars.base_frequency.value * 1.0
        res = model.vars.synth_res_actual.value
        
        # channel spacing in terms of res
        chsp = py2round(ch_spacing / res)

        self._reg_write(model.vars.SYNTH_CHSP_CHSP,  int(chsp))

        # frequency in terms of res
        freq = math.floor(f0 / res)

        self._reg_write(model.vars.SYNTH_FREQ_FREQ,  long(freq))

        

    def calc_chan_spacing_actual(self, model):
        """
        calculate the actual channel spacing
        Equation (5.35) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        chsp = model.vars.SYNTH_CHSP_CHSP.value
        res = model.vars.synth_res_actual.value

        ch_spacing = py2round(chsp * res)

        model.vars.channel_spacing_actual.value = int(ch_spacing)


    def calc_lodiv_value(self, model):

        f0 = model.vars.base_frequency.value
        part_family = model.part_family.lower()

        # cannot calculate lodiv if RF frequency is not given
        if f0 == 0:
            return                  # gdc:  This is probably not the correct way to handle this error

        #
        # First, make a list of all unique lodiv values.  The total lodiv value is made up of
        # three dividers chained together that can each have a divide value of 1,2,3,4,5.  The
        # min divider is 1 and the max divider is 125.
        #
        lodiv_section_a = [1, 2, 3, 4, 5]       # first in lodiv_chain_option tuple, high order bits in register
        lodiv_section_b = [1, 2, 3, 4, 5]       #
        lodiv_section_c = [1, 2, 3, 4, 5]       # last in lodiv_chain_option tuple, low order bits in register

        # For parts after Dumbo, A divide by 7 option was added to one stage
        if part_family != "dumbo":
            # Now add a bit of a hack...
            # We've already characterized Jumbo and Dumbo in the 169MHz range quite a bit using the divide by 15 lodiv.
            # For now at least, if we're able to use a divide by 15 in that area, then we'll use it.  We'll not enable the
            # divide by 7 option if we're in a range where we know the divide by 14 and divide by 15 ranges overlap.
            # That overlap range covers 167MHz (divide by 14 min) to 191MHz (divide by 15 max).  We really only characterized
            # things in the 169MHz band, so we'll only disable the divide by 14 option in the lower half of that overlap range.
            if (f0 < 166000000) or (f0 > 180000000):
                lodiv_section_c.append(7)

        lodiv_chain_options = dict()

        for lodiv_a in lodiv_section_a:
            for lodiv_b in lodiv_section_b:
                for lodiv_c in lodiv_section_c:
                    # Get total lodiv from the three sections
                    lodiv_total = lodiv_a * lodiv_b * lodiv_c

                    # if this total lodiv value is unique, add it to the dictionary
                    # The values for the three sections are added as a tuple for the
                    # dictionary entry
                    if lodiv_total not in lodiv_chain_options.keys():
                        lodiv_chain_options[lodiv_total] = (lodiv_a, lodiv_b, lodiv_c)

        # now lodiv_chain_options is a dictionary with each unique lodiv value for keys
        # with the data for each key being a tuple representing the divider sections to be
        # used to build the register value later

        # The synth tuning range was found in:  https: // jira.silabs.com / browse / EFRPHY - 12
        # According to that it is 4670 to 5740 MHz.  It's easier to think of half those number as
        # being between 2335MHz and and 2870MHz
        synth_min, synth_max = self._get_synth_min_max()

        # Now go through the valid lodiv options calculated above, and find the one that uses
        # the lowest synth frequency that is still above the minimum value allowed
        best_lodiv_value = 0        # Use this for error checking
        for lodiv in sorted(lodiv_chain_options.keys()):
            synth_freq = lodiv * f0

            #print("Trying lodiv = %d.   Synth freq = %d" % (lodiv, synth_freq))
            if synth_freq > synth_max:
                continue

            if synth_freq < synth_min:
                continue

            # if we made it here, this is a valid lodiv value.  Use the first one (lowest)
            # value we can, to make the synth frequency as low as possible, to give the
            # greatest tuning range possible.
            best_lodiv_value = lodiv
            break

        model.vars.lodiv.value = best_lodiv_value

        if best_lodiv_value == 0:
            raise CalculationException("No valid LODIV value possible for channel frequency of %s MHz!" % float(f0/1000000))

        # todo:  We really need to error check the min and max desired channel frequency.  We currently only
        # todo:  error check the base frequency.  To do that, we need to add an input parameter from the user
        # todo:  specifying how far they want to tune.  That input parameter can be either a max frequency in Hz,
        # todo:  or a max channel number.  It seems like most people aren't as familiar with the desired channel
        # todo:  spacing, so a max frequency might be a better choice for the input parameter.


        # Now code up the register value from the individual pieces
        (lodiv_a, lodiv_b, lodiv_c) = lodiv_chain_options[best_lodiv_value]
        if lodiv_a == 1:
            lodiv_a = 0
        if lodiv_b == 1:
            lodiv_b = 0

        # form register setting from dividers
        reg = (lodiv_a << 6) + (lodiv_b << 3) + lodiv_c

        self._reg_write(model.vars.SYNTH_DIVCTRL_LODIVFREQCTRL, reg)

    def _get_synth_min_max(self):
        synth_min = 2335 * 1000000
        synth_max = 2870 * 1000000
        return synth_min, synth_max

    def calc_lodiv_actual(self, model):
        """
        read LODIV register value and return actual LO divider value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        reg = model.vars.SYNTH_DIVCTRL_LODIVFREQCTRL.value

        # get divider ratios
        divA = (reg & 0x1C0) >> 6
        divB = (reg & 0x38) >> 3
        divC = reg & 0x7

        # if disabled set ratio to 1
        if divA == 0:
            divA = 1

        if divB == 0:
            divB = 1

        if divC == 0:
            divC = 1

        # calculate total factor
        model.vars.lodiv_actual.value = divA * divB * divC

        # calculate proper variable flag to determine sub-GHz
        model.vars.subgig_band.value = bool(model.vars.lodiv_actual.value > 1)

    def calc_iffreq_reg(self, model):
        """
        calculate IFFREQ register\n
        Equation (5.14), (5.33) and (5.37) of EFR32 Reference Manual (internal.pdf)\n
        IFFREQ = f_IF / res
        f_IF = fxo / (DEC0 * CFOSR)
        res = fxo / lodiv / pow(2,19)
        IFFREQ = fxo / (DEC0 * CFOSR) * lodiv * pow(2,19) / fxo
               = lodiv * pow(2,19) / (DEC0 * CFOSR)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        dec0 = model.vars.dec0_actual.value
        cfosr = model.vars.cfosr_actual.value
        lodiv = model.vars.lodiv_actual.value

        # calculate if frequency
        iffreq = math.floor(lodiv * pow(2, 19) / dec0 / cfosr)

        self._reg_write(model.vars.SYNTH_IFFREQ_IFFREQ,  int(abs(iffreq)))

    def calc_lo_side_regs(self, model):
        """
        calculate LOSIDE register in synth and matching one in modem

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.lo_injection_side.value = model.vars.lo_injection_side.var_enum.HIGH_SIDE  # default to high-side

        lo_injection_side = model.vars.lo_injection_side.value

        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            self._reg_write(model.vars.SYNTH_IFFREQ_LOSIDE, 1)
            self._reg_write(model.vars.MODEM_MIXCTRL_MODE, 0)
            self._reg_write(model.vars.MODEM_MIXCTRL_DIGIQSWAPEN, 1)


        if lo_injection_side == model.vars.lo_injection_side.var_enum.LOW_SIDE:
            self._reg_write(model.vars.SYNTH_IFFREQ_LOSIDE, 0)
            self._reg_write(model.vars.MODEM_MIXCTRL_MODE, 2)
            self._reg_write(model.vars.MODEM_MIXCTRL_DIGIQSWAPEN, 0)

        

    def calc_synth_misc(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.SYNTH_CHCTRL_CHNO, 0)
        self._reg_write(model.vars.SYNTH_CALOFFSET_CALOFFSET, 0)


    def calc_tuning_range_limits(self, model):
        model.vars.tuning_limit_min.value = long(self.synth_freq_min_limit / model.vars.lodiv_actual.value)
        model.vars.tuning_limit_max.value = long(self.synth_freq_max_limit / model.vars.lodiv_actual.value)