from pyradioconfig.parts.common.calculators.calc_modulator import CALC_Modulator
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum
from py_2_and_3_compatibility import *

class CALC_Modulator_Ocelot(CALC_Modulator):

    def buildVariables(self, model):

        #Add variable for LOW ramp level
        self._addModelVariable(model, 'am_low_ramplev', int, ModelVariableFormat.DECIMAL)

        super().buildVariables(model)

    def _add_symbol_encoding_var(self, model):
        var = self._addModelVariable(model, 'symbol_encoding', Enum, ModelVariableFormat.DECIMAL,
                                     'Symbol Encoding Options')
        member_data = [
            ['NRZ', 0, 'Non Return Zero Coding'],
            ['Manchester', 1, 'Manchester Coding'],
            ['Inv_Manchester', 4, 'Inverted Manchester Coding'],
            ['DSSS', 2, 'Direct Sequence Spread Spectrum Coding'],
            ['LINECODE', 3, 'Maps 0 to 0011 symbol and 1 to 1100 symbol'],
            ['MBUS_3OF6', 5, 'Mbus 3 of 6 coding'],
            ['UART_NO_VAL', 6, 'UART Frame Coding without start/stop bit validation'],
            ['UART_VAL', 7, 'UART Frame Coding with start/stop bit validation']
        ]
        var.var_enum = CreateModelVariableEnum(
            'SymbolEncodingEnum',
            'List of supported symbol encoding options',
            member_data)

    def calc_manchester_mapping(self, model):
        #Since manchester_mapping was removed as an input on Ocelot, we need to calculate it from symbol_encoding

        #Read in model vars
        symbol_encoding = model.vars.symbol_encoding.value

        if symbol_encoding == model.vars.symbol_encoding.var_enum.Inv_Manchester:
            manchester_mapping = model.vars.manchester_mapping.var_enum.Inverted
        else:
            manchester_mapping = model.vars.manchester_mapping.var_enum.Default

        #Write the model var
        model.vars.manchester_mapping.value = manchester_mapping

    def calc_txbr_value(self, model):
        #Overriding this function due to variable name change

        #Load model values into local variables
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        baudrate = model.vars.baudrate.value

        #calculate baudrate to fxo ratio
        #For the phase modulator this should always be based on xtal rate (https://jira.silabs.com/browse/MCUW_RADIO_CFG-1626)
        ratio = xtal_frequency_hz / (8.0 * baudrate)

        #Load local variables back into model variables
        model.vars.txbr_ratio.value = ratio

    def calc_tx_baud_rate_actual(self, model):
        #Overriding this function due to variable name change

        #Load model values into local variables
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        txbr_ratio = model.vars.txbr_ratio_actual.value

        tx_baud_rate = xtal_frequency_hz / (8.0 * txbr_ratio)

        #Load local variables back into model variables
        model.vars.tx_baud_rate_actual.value = tx_baud_rate

    #Class variable to store the max PA value (needs to be correct for full power output)
    max_pa_value = (63 << 2)

    def calc_modindex_value(self, model):
        #Overriding this function due to variable name change

        modem_frequency_hz = model.vars.modem_frequency_hz.value * 1.0
        modformat = model.vars.modulation_type.value
        freq_dev_hz = model.vars.deviation.value * 1.0
        synth_res = model.vars.synth_res_actual.value
        shaping_filter_gain = model.vars.shaping_filter_gain_actual.value
        interpolation_gain = model.vars.interpolation_gain_actual.value

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
           modformat == model.vars.modulation_type.var_enum.MSK or \
           modformat == model.vars.modulation_type.var_enum.FSK4:
            modindex = freq_dev_hz * 16.0 / (synth_res * shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.OQPSK:
            modindex = modem_frequency_hz / (synth_res * 2 * shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.OOK or \
                modformat == model.vars.modulation_type.var_enum.ASK or \
                modformat == model.vars.modulation_type.var_enum.BPSK or \
                modformat == model.vars.modulation_type.var_enum.DBPSK:
            modindex = self.max_pa_value * 16.0 / (shaping_filter_gain * interpolation_gain)
        else:
            modindex = 0.0  # don't care in OFDM

        model.vars.modindex.value = modindex

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
           modformat == model.vars.modulation_type.var_enum.MSK or \
           modformat == model.vars.modulation_type.var_enum.FSK4:
            freq_dev_hz = modindex * (synth_res * shaping_filter_gain * interpolation_gain) / 16.0
        else:
            freq_dev_hz = 0.0

        model.vars.tx_deviation_actual.value = freq_dev_hz

    def calc_symbol_rates_actual(self, model):
        encoding = model.vars.symbol_encoding.value
        encodingEnum = model.vars.symbol_encoding.var_enum
        demod_select = model.vars.demod_select.value

        baud_per_symbol = 1

        if demod_select == model.vars.demod_select.var_enum.LONGRANGE:
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
        elif encoding == encodingEnum.Manchester or encoding == encodingEnum.Inv_Manchester:
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

    def calc_txbases_reg(self, model):

        #Read in model variables
        preamble_length = model.vars.preamble_length.value #This is the TX preamble length
        preamble_pattern_len_actual = model.vars.preamble_pattern_len_actual.value

        #Calculate txbases
        txbases = preamble_length / preamble_pattern_len_actual

        #Limit checking
        if (txbases) > 0xffff:
            raise CalculationException("Calculated TX preamble sequences (TXBASE) value of %s exceeds limit of 65535! Adjust preamble inputs." % txbases )

        #Write the register
        self._reg_write(model.vars.MODEM_PRE_TXBASES, int(txbases))

    def calc_symbol_encoding(self, model):

        encoding = model.vars.symbol_encoding.value

        if encoding == model.vars.symbol_encoding.var_enum.DSSS:
            coding = 2
        elif encoding == model.vars.symbol_encoding.var_enum.Manchester or \
                encoding == model.vars.symbol_encoding.var_enum.Inv_Manchester:
            coding = 1
        else:
            coding = 0

        self._reg_write(model.vars.MODEM_CTRL0_CODING,  coding)

    def calc_mapfsk_reg(self, model):

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

        if encoding == model.vars.symbol_encoding.var_enum.Manchester or \
                encoding == model.vars.symbol_encoding.var_enum.Inv_Manchester:
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

    def calc_am_low_ramplev(self, model):

        #For now, always set to 0 (detailed calculation to be added when we add ASK support to calculator)
        model.vars.am_low_ramplev.value = 0

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
        txbr_num_err_tol = 0.003
        txbr_max_den = 255

        # : local variable for finding baudrate ratio that minimizes error between desired and actual baudrate
        txbr_num_err_map = {}

        # find best integer ratio to match desired ratio
        found_best_ratio = False
        for den in xrange(txbr_max_den, 0, -1):
            num = ratio * den
            txbr_num_err = abs(round(num) - num)

            if num < 32768:
                txbr_num_err_map[den] = abs(txbr_num_err - txbr_num_err_tol)
                if txbr_num_err < txbr_num_err_tol:
                    found_best_ratio = True
                    break

        # if best integer ratio is not found, re-calculate and find the ratio that is closest to the tolerance
        if not found_best_ratio:
            if len(txbr_num_err_map) > 0:
                den = min(txbr_num_err_map, key=txbr_num_err_map.get)
            else:
                den = 1
            num = ratio * den

        self._reg_write(model.vars.MODEM_TXBR_TXBRNUM, int(round(num)))
        self._reg_write(model.vars.MODEM_TXBR_TXBRDEN, int(den))
