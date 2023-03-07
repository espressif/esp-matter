from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
import math

class CALC_Utilities_Ocelot(CALC_Utilities):
    def calc_oversampling_val(self, model):
        #The actual OSR calculation has moved into the calc_demodulator.py file for Ocelot
        pass

    def calc_target_sensitivity(self, model):
        #Overriding function due to variable name change

        #Load model variables into local variables
        bitrate = model.vars.bitrate.value #This is the net bitrate (data only), because that is how Eb/No is defined
        freq = model.vars.base_frequency_hz.value
        modformat = model.vars.modulation_type.value
        demod = model.vars.demod_select.value

        # approximate EbNo number to use for our PHY
        if modformat == model.vars.modulation_type.var_enum.FSK4.value:
            ebno = 14.0
        elif modformat == model.vars.modulation_type.var_enum.OOK.value:
            ebno = 19.0
        elif modformat == model.vars.modulation_type.var_enum.OQPSK.value:
            # : For coherent demod, ebno = 8.5 based on measurement and Per's sim
            if demod == model.vars.demod_select.var_enum.COHERENT.value:
                ebno = 8.5
            else: # : Legacy demod
                ebno = 13.0
        else:
            ebno = 13.0

        # approximate noise figure to use for each band
        if freq < 500e6:
            nf = 3.5
        else:
            nf = 4.0

        target_sensitivity = -173.9 + 10 * math.log(bitrate, 10) + ebno + nf

        #Load local variables back into model variables
        model.vars.sensitivity.value = target_sensitivity

    def calc_error_check(self, model):
        #Overriding function due to removal of freq_gain_scale variable

        #Load model variables into local variables
        modulation_type = model.vars.modulation_type.value
        bitrate = model.vars.bitrate.value
        baudrate = model.vars.baudrate.value
        rx_baud_rate = model.vars.rx_baud_rate_actual.value
        tx_baud_rate = model.vars.tx_baud_rate_actual.value
        if_frequency = model.vars.if_frequency_hz_actual.value
        target_deviation = model.vars.deviation.value
        tx_deviation = model.vars.tx_deviation_actual.value
        carson_bw = model.vars.bandwidth_carson_hz.value
        bw_dig = model.vars.bandwidth_actual.value
        osr = model.vars.oversampling_rate_actual.value
        brcalen = model.vars.brcalen.value
        rxbr = model.vars.rxbrfrac_actual.value
        basebits = model.vars.preamble_pattern_len_actual.value
        timingbases = model.vars.MODEM_TIMING_TIMINGBASES.value
        num_timing_windows = model.vars.number_of_timing_windows.value
        preamble_detection_length = model.vars.preamble_detection_length.value
        dsss_length = model.vars.dsss_len.value

        if model.vars.if_frequency_hz.value_forced == None:
            if_frequency_forced = 0
        else:
            if_frequency_forced = 1

        if model.vars.bandwidth_hz.value_forced == None:
            bw_forced = 0
        else:
            bw_forced = 1

        max_timing_window = 256.0 / osr
        timing_window = timingbases * basebits * 1.0

        if num_timing_windows * timing_window > preamble_detection_length + 2 and dsss_length == 0:
            print(" WARNING: timing window chosen too large for given preamble length")

        if timing_window > max_timing_window:
            print("  WARNING: timing window larger than max allowed %d!" % math.floor(128.0 / (2 * rxbr * basebits)))

        rx_bitrate_error = abs(baudrate - rx_baud_rate) * 1.0 / baudrate

        if rx_bitrate_error > 0.004 and brcalen == 0:
            print("  WARNING: RX bitrate is off by more than 0.4%!")

        tx_bitrate_error = abs(baudrate - tx_baud_rate) * 1.0 / baudrate

        if tx_bitrate_error > 0.001:
            print("  WARNING: TX bitrate is off by more than 0.1%!")

        if tx_deviation > 0:
            tx_deviation_error = abs(target_deviation - tx_deviation) * 1.0 / target_deviation
        else:
            tx_deviation_error = 0.0

        if modulation_type == model.vars.modulation_type.var_enum.MSK:
            deviation_within_MSK_bound = math.floor(0.25 * bitrate) <= target_deviation <= math.ceil(0.25 * bitrate)
            if not deviation_within_MSK_bound:
                LogMgr.Warning("WARNING: Deviation is not 1/4 of data rate for MSK!")

        if tx_deviation_error > 0.1:
            print("  WARNING: TX frequency deviation is more than 10% away from target!")

        if baudrate >= 1e6:
            carson_scale = 0.82
        else:
            carson_scale = 0.95

        bw_error = abs(carson_bw * carson_scale - bw_dig) * 1.0 / carson_bw

        # if bw_error > 0.2 and bw_forced == 0 and carson_bw < 3.4e6:
        #     print("  WARNING: BW is set more than 20% away from Carson BW!")

        #if osr > 8 or osr < 3:
        #    print("  WARNING: Oversampling rate is beyond specified limits of 3 to 8!")

        #Load local variables back into model variables
        model.vars.max_timing_window.value  = max_timing_window
        model.vars.timing_window.value      = timing_window
        model.vars.rx_bitrate_error.value   = rx_bitrate_error
        model.vars.tx_bitrate_error.value   = tx_bitrate_error
        model.vars.tx_deviation_error.value = tx_deviation_error
        model.vars.bw_error.value           = bw_error