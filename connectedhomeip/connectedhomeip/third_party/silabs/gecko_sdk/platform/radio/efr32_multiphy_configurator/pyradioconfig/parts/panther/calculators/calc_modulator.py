from pyradioconfig.parts.common.calculators.calc_modulator import CALC_Modulator
from py_2_and_3_compatibility import *


class CALC_Modulator_Panther(CALC_Modulator):

    def calc_symbol_rates_actual(self, model):
        encoding = model.vars.symbol_encoding.value
        encodingEnum = model.vars.symbol_encoding.var_enum

        baud_per_symbol = 1

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