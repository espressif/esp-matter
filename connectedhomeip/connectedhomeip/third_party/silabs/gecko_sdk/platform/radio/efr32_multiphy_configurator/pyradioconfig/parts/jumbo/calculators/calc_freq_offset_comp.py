"""Core CALC_Freq_Offset_Comp Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat
from pyradioconfig.parts.common.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp
from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities

from py_2_and_3_compatibility import *

class CALC_Freq_Offset_Comp_jumbo(CALC_Freq_Offset_Comp):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0


    def calc_freq_comp_mode(self, model):
        """
        determine best frequency compensation mode based on emprical data

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        preamble_len = model.vars.preamble_length.value
        baudrate = model.vars.baudrate.value
        modulation = model.vars.modulation_type.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value

        # We seem to not be able to get reliable AFC loop for low datarates
        # any rate below 5 Kbps we prefer INTERNAL compensation. For 4-FSK
        # this limit is 10 Kbps bitrate, or 5Kbps baudrate
        baudrate_limit = 5000

        #Default to continuous AFC
        afc_mode = model.vars.afc_run_mode.var_enum.CONTINUOUS

        if in_2fsk_opt_scope:
            if baudrate < 1900000:
                # Use INTERNAL_LOCK_AT_PREAMBLE_DETECT for most proprietary 2FSK PHYs
                freq_mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
            else:
                freq_mode = model.vars.frequency_comp_mode.var_enum.DISABLED

        # choose one-shot AFC in 2-FSK if preamble is between 16 and 32 bits
        elif modulation == model.vars.modulation_type.var_enum.FSK2 and 16 <= preamble_len < 32:
            afc_mode = model.vars.afc_run_mode.var_enum.ONE_SHOT
            freq_mode = model.vars.frequency_comp_mode.var_enum.AFC_LOCK_AT_FRAME_DETECT

        # for amplitude modulation the frequency offset estimator is used to estimate
        # peak amplitude level which is used in slicer level determination.
        # best option for this modulation is to lock at FRAME detect
        elif modulation == model.vars.modulation_type.var_enum.ASK:
            freq_mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_FRAME_DETECT

        # For OOK, set the comp mode to always on to enable the dynamic slicer to write the slicer level any time
        # The dynamic slicer code intelligently detects the DEMODSTATE and handles when to actually read RSSI and write the level
        elif modulation == model.vars.modulation_type.var_enum.OOK:
            freq_mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON

        # For other modulation types enable AFC if we have at least 40 preamble bits and not too low a baudrate
        elif preamble_len >= 32 and baudrate > baudrate_limit:
            freq_mode = model.vars.frequency_comp_mode.var_enum.AFC_LOCK_AT_PREAMBLE_DETECT

        # else default to INTERNAL compensation that is always running
        else:
            freq_mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON

        model.vars.frequency_comp_mode.value = freq_mode
        model.vars.afc_run_mode.value = afc_mode

    def calc_afconeshoft_reg(self, model):

        run_mode = model.vars.afc_run_mode.value
        comp_mode = model.vars.frequency_comp_mode.value

        comp_mode_index = self.freq_comp_mode_index(model, comp_mode)

        if run_mode == model.vars.afc_run_mode.var_enum.ONE_SHOT and comp_mode_index > 3:
            oneshot = 1
            deldet = 1
            dsaoffset = 1
        else:
            oneshot = 0
            deldet = 0
            dsaoffset = 0

        if comp_mode_index > 3:
            limreset = 1
        else:
            limreset = 0

        self._reg_write(model.vars.MODEM_AFC_AFCONESHOT, oneshot)
        self._reg_write(model.vars.MODEM_AFC_AFCDELDET, deldet)
        self._reg_write(model.vars.MODEM_AFC_AFCDSAFREQOFFEST, dsaoffset)
        self._reg_write(model.vars.MODEM_AFC_AFCENINTCOMP, 0)
        self._reg_write(model.vars.MODEM_AFC_AFCLIMRESET, limreset)


    def calc_afclimreset_actual(self, model):

        model.vars.afc_lim_reset_actual.value = model.vars.MODEM_AFC_AFCLIMRESET.value

    def afc_adj_limit(self, model):

        #FIXME: check if afc limit needs to be doubled (it uses both +/- frequencies)
        freq_limit = model.vars.freq_offset_hz.value
        res = model.vars.synth_res_actual.value
        afclimreset = model.vars.afc_lim_reset_actual.value

        # calculate limit
        afcadjlim = freq_limit / res

        # if AFC_LIM_RESET is enabled we reset to the center frequency
        # once the accumulated offset reaches the limit. In this mode we
        # like to set the limit to about 10% higher than where we like the
        # limit to be
        if afclimreset:
            afcadjlim *= 1.1

        return int(round(afcadjlim))

    def calc_afc_scale_value(self, model):
        """
        calculate AFC scaling based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        #FIXME: verify the way we use src1ratio and src2ratio in equation
        #FIXME: may need to scale down afcscale by 3/4 to get better sensitivity numbers when afc is enabled
        fxo = model.vars.xtal_frequency.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        freqgain = model.vars.freq_gain_actual.value
        mod_format = model.vars.modulation_type.value
        res = model.vars.synth_res_actual.value
        mode = model.vars.frequency_comp_mode.value
        scale = model.vars.afc_step_scale.value
        src1ratio = model.vars.src1_ratio_actual.value
        src2ratio = model.vars.src2_ratio_actual.value

        mode_index = self.freq_comp_mode_index(model, mode)

        if mode_index >= 4 and freqgain > 0:

            if mod_format == model.vars.modulation_type.var_enum.FSK2 or \
               mod_format == model.vars.modulation_type.var_enum.FSK4:
                afcscale = fxo / (dec0 * dec1 * dec2 * 256.0 * freqgain * res) * src1ratio *src2ratio
            else:
                afcscale = fxo / (dec0 * dec1 * dec2 * 256.0) * src1ratio * src2ratio

        else:
            afcscale = 0.0

        afcscale = afcscale * scale

        model.vars.afc_scale.value = afcscale

    def calc_afc_scale_reg(self, model):
        """
        convert AFC scale value to mantissa and exponent register values

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        afc_scale = model.vars.afc_scale.value

        if afc_scale == 0:
            best_m = 0
            best_e = 0
        else:
            best_diff = 99e9
            # start with the highest allowed mantissa and find best m, e pair
            for m in xrange(1, 31, 1):
                e = py2round(math.log(afc_scale / m, 2))
                diff = abs(afc_scale - m * 2**e)

                if diff < best_diff:
                    best_diff = diff
                    best_e = e
                    best_m = m

            if best_e > 7:
                best_e = 7

            if best_e < -8:
                best_e = -8

            if best_e < 0:
                best_e += 16

            if best_m > 31:
                best_m = 31

        self._reg_write(model.vars.MODEM_AFC_AFCSCALEE, int(best_e))
        self._reg_write(model.vars.MODEM_AFC_AFCSCALEM, int(best_m))

    def calc_afc_scale_actual(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        e = float(model.vars.MODEM_AFC_AFCSCALEE.value)
        m = float(model.vars.MODEM_AFC_AFCSCALEM.value)

        if e > 7:
            e -= 16

        model.vars.afc_scale_actual.value = m * 2**e

