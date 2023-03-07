"""Core AGC CALC_Utilities Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math

from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

from py_2_and_3_compatibility import *

class CALC_Utilities(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Insert new block variables here
        self._addModelVariable(model, 'max_timing_window',  float, ModelVariableFormat.DECIMAL )
        self._addModelVariable(model, 'timing_window',      float, ModelVariableFormat.DECIMAL ) 
        self._addModelVariable(model, 'rx_bitrate_error',   float, ModelVariableFormat.DECIMAL ) 
        self._addModelVariable(model, 'tx_bitrate_error',   float, ModelVariableFormat.DECIMAL ) 
        self._addModelVariable(model, 'rx_deviation_error', float, ModelVariableFormat.DECIMAL )
        self._addModelVariable(model, 'tx_deviation_error', float, ModelVariableFormat.DECIMAL )
        self._addModelVariable(model, 'bw_error',           float, ModelVariableFormat.DECIMAL )
        
        
        

    def frac2exp(self, max_m, frac):
        """
        convert fraction into mantissa and exponent format

        Args:
            max_m (unknown) : unknown\n
            frac (unknown) : unknown\n
        """

        if frac == 0:
            return 0, 0

        best_diff = 99e9
        # start with the highest allowed mantissa and find best m, e pair
        for m in xrange(max_m, 0, -1):
            e = py2round(math.log(frac / m, 2))
            diff = abs(frac - m * 2**e)

            if diff < best_diff:
                best_diff = diff
                best_e = e
                best_m = m

        return best_m, best_e


    def get_target_bw(self, model):
        """
        return bandwidth to be used in calculations

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        config_bw = model.vars.bandwidth.value
        carson_bw = model.vars.bandwidth_carson_hz.value

        if config_bw > 0:
            return config_bw
        else:
            return carson_bw


    def calc_modulation_index(self, model):
        """
        calculate informational modulation index based on baudrate and deviation

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        baudrate_hz = model.vars.baudrate.value
        freq_dev_hz = model.vars.deviation.value * 1.0

        model.vars.modulation_index.value = freq_dev_hz * 2.0 / baudrate_hz


    def calc_oversampling_val(self, model):
        """
        calculate oversampling rate in the receive chain

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.oversampling_rate_actual.value = 2.0 * model.vars.rxbrfrac_actual.value

    def calc_ook_ebno(self,model):
        #Set the EbN0 for OOK on Dumbo, Jumbo, Nerio as 21.5
        model.vars.ook_ebno.value = 21.5

    def calc_target_sensitivity(self, model):
        """
        calculate approximate expected sensitivity for given PHY

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

#TODO: update based on the  numbers from Performance Review presentation

        bitrate = model.vars.bitrate.value
        bandwidth = model.vars.bandwidth_hz.value
        freq = model.vars.base_frequency.value
        spreading_factor = model.vars.dsss_spreading_factor.value * 1.0
        modformat = model.vars.modulation_type.value

        # approximate EbNo number to use for our PHY
        if modformat == model.vars.modulation_type.var_enum.FSK4.value:
            ebno = 14.0
        elif modformat == model.vars.modulation_type.var_enum.OOK.value:
            ebno = model.vars.ook_ebno.value
        else:
            ebno = 13.0

        # approximate noise figure to use for each band
        if freq < 500e6:
            nf = 6.5
        elif freq < 1e9:
            nf = 7.6
        else:
            nf = 9.0

        model.vars.sensitivity.value = -173.9 + 10*math.log(bitrate,10) + ebno + nf

        if model.vars.modulation_type.value == model.vars.modulation_type.var_enum.OOK:
            #If the modulation type is OOK, try to better estimate sensitivity based on channel bandwidth (useful for dynamic slicing)
            #Since the ENBW is a combination of the channel filter and downstream averaging filters, using emperical data here
            if bandwidth/bitrate < 10:
                sensitivity_bw_adjust = 0
            elif bandwidth/bitrate < 20:
                sensitivity_bw_adjust = 1
            elif bandwidth/bitrate < 40:
                sensitivity_bw_adjust = 2
            elif bandwidth/bitrate < 60:
                sensitivity_bw_adjust = 3
            elif bandwidth / bitrate < 80:
                sensitivity_bw_adjust = 4
            elif bandwidth / bitrate < 100:
                sensitivity_bw_adjust = 5
            elif bandwidth / bitrate < 150:
                sensitivity_bw_adjust = 6
            else:
                sensitivity_bw_adjust = 7
            model.vars.sensitivity.value += sensitivity_bw_adjust


    def calc_error_check(self, model):
        """
        perform high level error check and warn user that we have large differences
        between desired values and actual values from the PHY

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        baudrate = model.vars.baudrate.value
        rx_baud_rate = model.vars.rx_baud_rate_actual.value
        tx_baud_rate = model.vars.tx_baud_rate_actual.value
        if_frequency = model.vars.if_frequency_hz_actual.value
        rx_deviation = model.vars.rx_deviation_actual.value
        target_deviation = model.vars.deviation.value
        tx_deviation = model.vars.tx_deviation_actual.value
        carson_bw = model.vars.bandwidth_carson_hz.value
        bw_dig = model.vars.bandwidth_actual.value
        osr = model.vars.oversampling_rate_actual.value
        brcalen = model.vars.brcalen.value
        rxbr = model.vars.rxbrfrac_actual.value
        basebits = model.vars.preamble_pattern_len_actual.value
        timingbases = model.vars.MODEM_TIMING_TIMINGBASES.value
        scale = model.vars.freq_gain_scale.value
        rx_baudrate_offset_hz = model.vars.rx_baudrate_offset_hz.value
        num_timing_windows = model.vars.number_of_timing_windows.value
        preamble_len      = model.vars.preamble_length.value
        dsss_length = model.vars.dsss_len.value

        if model.vars.if_frequency_hz.value_forced == None:
            if_frequency_forced = 0
        else:
            if_frequency_forced = 1

        if model.vars.bandwidth_hz.value_forced == None:
            bw_forced = 0
        else:
            bw_forced = 1

        max_timing_window = 256.0 / (2 * rxbr)
        timing_window = timingbases * basebits * 1.0

        if num_timing_windows * timing_window > preamble_len + 2 and dsss_length == 0:
            LogMgr.Warning(" WARNING: timing window chosen too large for given preamble length")

        if timing_window > max_timing_window:
            LogMgr.Warning("  WARNING: timing window larger than max allowed %d!" % math.floor(128.0 / (2 * rxbr * basebits)))

        rx_bitrate_error = abs(baudrate + rx_baudrate_offset_hz - rx_baud_rate) * 1.0 / baudrate

        if rx_bitrate_error > 0.004 and brcalen == 0:
            LogMgr.Warning("  WARNING: RX bitrate is off by more than 0.4%!")

        tx_bitrate_error = abs(baudrate - tx_baud_rate) * 1.0 / baudrate

        if tx_bitrate_error > 0.001:
            LogMgr.Warning("  WARNING: TX bitrate is off by more than 0.1%!")

        if if_frequency < 150000 and if_frequency_forced == 0:
            LogMgr.Warning("  WARNING: IF frequency is less than the 300KHz limit!")

        if rx_deviation > 0:
            rx_deviation_error = abs(target_deviation - rx_deviation/scale) * 1.0 / target_deviation
        else:
            rx_deviation_error = 0.0

        #if rx_deviation_error > 0.2:
        #    print("  WARNING: RX frequency deviation is more than 10% away from target!")

        if tx_deviation > 0:
            tx_deviation_error = abs(target_deviation - tx_deviation) * 1.0 / target_deviation
        else:
            tx_deviation_error = 0.0

        if tx_deviation_error > 0.1:
            LogMgr.Warning("  WARNING: TX frequency deviation is more than 10% away from target!")

        if baudrate >= 1e6:
            carson_scale = 0.82
        else:
            carson_scale = 0.95

        bw_error = abs(carson_bw * carson_scale - bw_dig) * 1.0 / carson_bw

        if bw_error > 0.2 and bw_forced == 0 and carson_bw < 3.4e6:
            LogMgr.Warning("  WARNING: BW is set more than 20% away from Carson BW!")

        if osr > 8 or osr < 3:
            LogMgr.Warning("  WARNING: Oversampling rate is beyond specified limits of 3 to 8!")

        model.vars.max_timing_window.value  = max_timing_window 
        model.vars.timing_window.value      = timing_window
        model.vars.rx_bitrate_error.value   = rx_bitrate_error
        model.vars.tx_bitrate_error.value   = tx_bitrate_error
        model.vars.rx_deviation_error.value = rx_deviation_error
        model.vars.tx_deviation_error.value = tx_deviation_error
        model.vars.bw_error.value           = bw_error
                                            




