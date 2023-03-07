"""Core CALC_Modulator Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

#import math
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities
from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

from py_2_and_3_compatibility import *

class CALC_Modulator(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):

        self._add_symbol_encoding_var(model)

        var = self._addModelVariable(model, 'manchester_mapping', Enum, ModelVariableFormat.DECIMAL, 'Manchester Code Mapping Options for packet payload')
        member_data = [
            ['Default',  0, '0-bit corresponds to a 0 to 1 transition and 1-bit corresponds to 1 to 0 transition'],
            ['Inverted', 1, '0-bit corresponds to a 1 to 0 transition and 1-bit corresponds to 0 to 1 transition'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'ManchesterMappingEnum',
            'List of supported Manchester Code options',
            member_data)

    def _add_symbol_encoding_var(self, model):
        var = self._addModelVariable(model, 'symbol_encoding', Enum, ModelVariableFormat.DECIMAL,
                                     'Symbol Encoding Options')
        member_data = [
            ['NRZ', 0, 'Non Return Zero Coding'],
            ['Manchester', 1, 'Manchester Coding'],
            ['DSSS', 2, 'Direct Sequence Spread Spectrum Coding'],
        ]
        # Only Nerio (and Panther) support LINECODE Encoding, used for BLE Long Range
        if model.part_family.lower() not in ["dumbo", "jumbo", "nixi"]:
            member_data.append(['LINECODE', 3, 'Maps 0 to 0011 symbol and 1 to 1100 symbol'])
        var.var_enum = CreateModelVariableEnum(
            'SymbolEncodingEnum',
            'List of supported symbol encoding options',
            member_data)

    def calc_tx_baud_rate_actual(self, model):
        """
        calculate actual TX baud rate from register settings

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value
        txbr_ratio = model.vars.txbr_ratio_actual.value

        tx_baud_rate = fxo / (8.0 * txbr_ratio)

        model.vars.tx_baud_rate_actual.value = tx_baud_rate

    def calc_symbol_rates_actual(self, model):
        encoding = model.vars.symbol_encoding.value
        encodingEnum = model.vars.symbol_encoding.var_enum

        baud_per_symbol = 1
        if model.part_family.lower() in ["nerio"]:
            if model.vars.MODEM_LONGRANGE_LRBLE.value == 1:
                # In case of BLE LR 125 kps, baud_per_symbol is 8
                if model.vars.FRC_CTRL_RATESELECT.value == 0:
                    baud_per_symbol = 8
                # In case of BLE LR 500 kps, baud_per_symbol is 2
                elif model.vars.FRC_CTRL_RATESELECT.value == 2:
                    baud_per_symbol = 2
                else:
                    raise ValueError("Invalid FRC_CTRL_RATESELECT value used in LONGRANGE configuration")
            if model.vars.FRC_CTRL_RATESELECT.value == 1:
                encoding = model.vars.MODEM_CTRL6_CODINGB
            if encoding == encodingEnum.LINECODE:
                baud_per_symbol *= 4
        if encoding == encodingEnum.DSSS:
            baud_per_symbol *= model.vars.dsss_len.value
        elif encoding == encodingEnum.Manchester:
            baud_per_symbol *= 2
        model.vars.baud_per_symbol_actual.value = baud_per_symbol

        if encoding == encodingEnum.DSSS:
            bits_per_symbol = model.vars.dsss_bits_per_symbol.value
        else:
            modFormat = model.vars.modulation_type.value
            modFormatEnum = model.vars.modulation_type.var_enum
            if modFormat in [modFormatEnum.FSK4, modFormatEnum.OQPSK]:
                bits_per_symbol = 2
            else:
                bits_per_symbol = 1
        model.vars.bits_per_symbol_actual.value = bits_per_symbol

    #TODO: add support for ASK modulation
    def calc_modindex_value(self, model):
        """
        calculate MODINDEX value
        Equations from Table 5.25 in EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value * 1.0
        modformat = model.vars.modulation_type.value
        freq_dev_hz = model.vars.deviation.value * 1.0
        synth_res = model.vars.synth_res_actual.value
        shaping_filter_gain = model.vars.shaping_filter_gain_actual.value
        interpolation_gain = model.vars.interpolation_gain_actual.value

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
           modformat == model.vars.modulation_type.var_enum.FSK4:
            modindex = freq_dev_hz * 16.0 / (synth_res * shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.OQPSK or \
             modformat == model.vars.modulation_type.var_enum.MSK:
            modindex = fxo / (synth_res * 2 * shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.BPSK or \
             modformat == model.vars.modulation_type.var_enum.OOK or \
             modformat == model.vars.modulation_type.var_enum.DBPSK:
            modindex = 150.0 * 16 / (shaping_filter_gain * interpolation_gain)
        else:
            raise CalculationException("ERROR: %s modulation not yet supported!" % modformat)
            return

        model.vars.modindex.value = modindex


    def calc_modindex_field(self, model):
        """
        convert desired modindex fractional value to MODINDEXM * 2^MODINDEXE
        Equations (5.13) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modindex = model.vars.modindex.value

        # convert fractional modindex into m * 2^e format
        m, e = CALC_Utilities().frac2exp(31, modindex)

        # MODEINDEXE is a signed value
        if e < 0:
            e += 32

        # verify number fits into register
        if m > 31:
            m = 31

        if e > 31:
            e = 31

        if m < 0:
            m = 0

        self._reg_write(model.vars.MODEM_MODINDEX_MODINDEXM,  int(m))
        self._reg_write(model.vars.MODEM_MODINDEX_MODINDEXE,  int(e))


    def calc_modindex_actual(self, model):
        """
        given register settings return actual MODINDEX as fraction
        Equations (5.13) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        m = model.vars.MODEM_MODINDEX_MODINDEXM.value
        e = model.vars.MODEM_MODINDEX_MODINDEXE.value

        # MODEINDEXE is a signed value
        if e > 15:
            e -= 32

        model.vars.modindex_actual.value = 1.0 * m * 2**e


    def calc_modulation_index_actual(self, model):
        """
        calculate the actual modulation index for given PHY
        This is the traditional modulation index as 2 * deviation / baudrate
        the one above we call modindex and is specific value used by EFR32

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        baudrate_hz = model.vars.tx_baud_rate_actual.value
        tx_deviation = model.vars.tx_deviation_actual.value

        model.vars.modulation_index_actual.value = tx_deviation * 2.0 / baudrate_hz


    def calc_tx_freq_dev_actual(self, model):
        """
        given register setting return actual frequency deviation used in the modulator
        Using Equations in Table 5.25 of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modformat = model.vars.modulation_type.value
        modindex = model.vars.modindex_actual.value
        synth_res = model.vars.synth_res_actual.value
        shaping_filter_gain = model.vars.shaping_filter_gain_actual.value
        interpolation_gain = model.vars.interpolation_gain_actual.value

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
           modformat == model.vars.modulation_type.var_enum.FSK4:
            freq_dev_hz = modindex * (synth_res * shaping_filter_gain * interpolation_gain) / 16.0
        else:
            freq_dev_hz = 0.0

        model.vars.tx_deviation_actual.value = freq_dev_hz


    # calculate TX baudrate ratio
    # Using Equation (5.7) of EFR32 Reference Manual (internal.pdf)
    def calc_txbr_value(self, model):
        """
        calculate TX baudrate ratio
        Using Equation (5.7) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value
        baudrate = model.vars.baudrate.value

        # calculate baudrate to fxo ratio
        ratio = fxo / (baudrate * 8.0)

        model.vars.txbr_ratio.value = ratio


    def calc_txbr_reg(self, model):
        """
        given desired TX baudrate ratio calculate TXBRNUM and TXBRDEN
        that gets as close as possible to the ratio.
        Note that we start from the highest possible value for TXBRDEN
        and go down since having largest possible values in these register
        to have better phase resolution in OQPSK and MSK (see end of section
        5.6.5 in the manual)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        ratio = model.vars.txbr_ratio.value

        # find best integer ratio to match desired ratio
        for den in xrange(255, 0, -1):
             num = ratio * den
             if abs(round(num) - num) < 0.003 and num < 32768:
                break

        self._reg_write(model.vars.MODEM_TXBR_TXBRNUM,  int(round(num)))
        self._reg_write(model.vars.MODEM_TXBR_TXBRDEN,  int(den))


    def calc_txbr_actual(self, model):
        """
        given register values calculate actual TXBR ratio implemented

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        num = model.vars.MODEM_TXBR_TXBRNUM.value * 1.0
        den = model.vars.MODEM_TXBR_TXBRDEN.value

        ratio = num / den

        model.vars.txbr_ratio_actual.value = ratio


    def calc_txbases_reg(self, model):
        """
        set TXBASES based on preamble length and base bits value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        txbases = model.vars.preamble_length.value / model.vars.preamble_pattern_len_actual.value

        # Some input combinations can produce values out of range for the register fields,
        # such as applying ludicrously long preamble lengths.

        # MCUW_RADIO_CFG-793

        # TODO: is would be best to query the register model to determine these two fields are 7 bits wide

        if (txbases) > 0xffff:
            raise CalculationException("Calculated TX preamble sequences (TXBASE) value of %s exceeds limit of 65535! Adjust preamble inputs." % txbases )

        self._reg_write(model.vars.MODEM_PRE_TXBASES, int(txbases))


    def calc_symbol_encoding(self, model):
        """
        set CODING register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        encoding = model.vars.symbol_encoding.value

        if encoding == model.vars.symbol_encoding.var_enum.DSSS:
            coding = 2
        elif encoding == model.vars.symbol_encoding.var_enum.Manchester:
            coding = 1
        else:
            coding = 0

        self._reg_write(model.vars.MODEM_CTRL0_CODING,  coding)





    def calc_mapfsk_reg(self, model):
        """
        program MAPFSK register based on input

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mod_format = model.vars.modulation_type.value
        manchester_map = model.vars.manchester_mapping.value
        fsk_map = model.vars.fsk_symbol_map.value
        encoding = model.vars.symbol_encoding.value

        FSKMAP_LOOKUP = {
            model.vars.fsk_symbol_map.var_enum.MAP0.value: 0,
            model.vars.fsk_symbol_map.var_enum.MAP1.value: 1,
            model.vars.fsk_symbol_map.var_enum.MAP2.value: 2,
            model.vars.fsk_symbol_map.var_enum.MAP3.value: 3,
            model.vars.fsk_symbol_map.var_enum.MAP4.value: 4,
            model.vars.fsk_symbol_map.var_enum.MAP5.value: 5,
            model.vars.fsk_symbol_map.var_enum.MAP6.value: 6,
            model.vars.fsk_symbol_map.var_enum.MAP7.value: 7,
        }

        mapfsk = FSKMAP_LOOKUP[fsk_map.value]

        if mod_format != model.vars.modulation_type.var_enum.FSK4:
            # if we're using Manchester encoding (or any FSK modulation actually),
            # then only MAP0 and MAP1 are valid
            if mapfsk > 1:
                raise CalculationException("Invalid fsk symbol map value for modulation type selected.")

        if encoding == model.vars.symbol_encoding.var_enum.Manchester:
            # if we're using Manchester encoding,
            # then only MAP0 and MAP1 are valid
            if mapfsk > 1:
                raise CalculationException("Invalid fsk_symbol_map value for Manchester encoding")

            # if we're using inverted Manchester encoding, then flip the polarity of the fsk
            # map.  This flips the polarity of the entire transmission, including the preamble
            # and syncword.  We don't want the preamble and syncword flipped, so we'll invert those
            # registers elsewhere
            if manchester_map != model.vars.manchester_mapping.var_enum.Default:
                mapfsk ^= 1

        self._reg_write(model.vars.MODEM_CTRL0_MAPFSK, mapfsk)

