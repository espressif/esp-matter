"""Core CALC_Freq_Offset_Comp Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math

from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

from py_2_and_3_compatibility import *

class CALC_Freq_Offset_Comp(ICalculator):

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
        self._addModelVariable(model, 'afc_step_scale', float, ModelVariableFormat.DECIMAL, desc='Scale applied to the default frequency adjustment step size')


    def calc_freq_offset(self, model):
        """
        calculate maximum frequency offset we might see due to PPM errors in
        RX and TX crystals

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        rx_ppm = model.vars.rx_xtal_error_ppm.value
        tx_ppm = model.vars.tx_xtal_error_ppm.value
        rf_freq_hz = model.vars.base_frequency.value * 1.0

        freq_offset = (rx_ppm + tx_ppm) * rf_freq_hz / 1e6

        model.vars.freq_offset_hz.value = int(round(freq_offset,0))

    def calc_freq_offset_scale_value(self, model):
        """
        calculate FREQOFFSET scaling based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # From the MODEM_FREQOFFEST documentation:
        # When AFC is enabled, this field is the residual frequency offset after
        # AFC correction. Frequency offset between transmitter and receiver is
        # given by AFCADJRX. Encoding of this signed value depends on modulation
        # format and if coherent detection is enabled.
        # For 2/4-FSK, the offset in Hz equals:
        # FREQOFFEST * fHFXO / (FREQGAIN * (Decimation Factor 0) * (Decimation Factor 1) *
        # (Decimation Factor 2) * 256).
        # For OQPSK, MSK and (D)BPSK, the offset in Hz equals:
        # FREQOFFEST * fHFXO / (2 * RXBRFRAC * (Decimation Factor 0) * (Decimation Factor 1) *
        # (Decimation Factor 2) * 256).
        # When coherent detection is enabled, the offset in Hz equals:
        # FREQOFFEST * chiprate / 2^13.
        # Frequency offset estimate is not available for OOK/ASK.

        fxo = model.vars.xtal_frequency.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        freq_gain = model.vars.freq_gain_actual.value
        osr = model.vars.oversampling_rate_actual.value # This is (2*RXBRFRAC)
        mod_format = model.vars.modulation_type.value
        chip_rate = model.vars.baudrate.value # In the calculator chiprate = baudrate

        try:
            # Check for coherent detection first
            if model.vars.MODEM_CTRL1_PHASEDEMOD.value == 3: # (COH)
                freq_offset_scale = (chip_rate//(2**13))*1.0 # force a float #
            else:
                if mod_format == model.vars.modulation_type.var_enum.FSK2 or \
                    mod_format == model.vars.modulation_type.var_enum.FSK4:
                    freq_offset_scale = fxo / (freq_gain * dec0 * dec1 * dec2 * 256.0)
                elif mod_format == model.vars.modulation_type.var_enum.OQPSK or \
                    mod_format == model.vars.modulation_type.var_enum.MSK or \
                    mod_format == model.vars.modulation_type.var_enum.BPSK or \
                    mod_format == model.vars.modulation_type.var_enum.DBPSK:
                    freq_offset_scale = fxo / (osr * dec0 * dec1 * dec2 * 256.0)
                else:
                    freq_offset_scale = 0.0
        except ZeroDivisionError:
            # In case the divisor ends up being 0 (freq_gain or osr or dec0/1/2)
            freq_offset_scale = 0.0

        model.vars.freq_offset_scale.value = freq_offset_scale

    def calc_frequency_offset_factor_value(self, model):
        """
        calculate Frequency Offset factor used by the RAIL Adapter
        RAIL_LIB-878
        MCUW_RADIO_CFG-606

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # Frequency Offset Scale is somewhat complicated to calculate, but
        # I was able to determine that the largest and smallest numbers should
        # come from:
        #   freq_offset_scale = fxo / (freq_gain * dec0 * dec1 * dec2 * 256.0)
        # where:
        #   fxo is fixed at 38400000
        #   freq_gain = 0.21875 = m * 2**(2-e), m = 7, e = 7
        #   dec0 = dec1 = dec2 = 1
        # So, freq_offset_scale max theoretical value is 685714.2857142857
        # Looking at all the current PHY cfg file in the Calculator, I found
        # that the highest number freq_offset_scale can be is 16666.6666667,
        # (PHY_Bluetooth_LE_2M_Viterbi.cfg and others), so, I'm going to round
        # this up (arbitrarily) to 25000.
        freqOffsetScale = model.vars.freq_offset_scale.value

        # Synth Resolution is calculated as:
        # synth_res = fxo / lodiv / pow(2, 19)
        # We know:
        #   fxo is fixed at 38400000
        #   lodiv in theory, can only have only certain values, restricted by
        #   the three subdividers A, B and C in LODIVFREQCTRL, but technically
        #   it can range between 0 (error) and 175 (divA=5, divB=5, divC=7);
        #   see calc_lodiv_value for more details.
        # Therefore, synthResolution can vary from:
        #   0.4185267 (lodiv = 1) to 73.2421875 (lodiv = 175)
        synthResolution = model.vars.synth_res_actual.value

        # In firmware, we need both the freqOffsetScale and the synthResolution
        # to convert the value in FREQOFFEST to synthesizer resolution units
        # (synthTicks). freqOffsetScale only scales the value in FREQOFFEST to
        # actual Hz. synthResolution has units (Hz/synthTicks), so in order to
        # get pure synthTicks, and get rid of the Hz, we need to multiply by the
        # reciprocal of the synthResolution (synthTicks/Hz).
        freqOffsetFactor = freqOffsetScale * (1.0/synthResolution)

        # Assuming the worst case scenario (max possible value for freqOffsetScale,
        # and min possible value for synthResolution; and vice versa):
        #   25000*(1/0.4185267) = 59733.345566722506
        #   0.0018166333859430362*(1/73.2421875) = 0.000024803101162742255
        # It makes sense to scale the freqOffsetFactor to increase the precision
        # of the result in the FW once we multiply by FREQOFFEST, which is 8 bits
        # on Dumbo and 13 bits on Jumbo and above.

        # IMPORTANT: Since we are using FXP<16.16>, we need to make sure the
        # integer part fits in 16 bits, otherwise we can't recover it.
        assert int(freqOffsetFactor) <= 2**16, \
            "Resulting freqOffsetFactor ({}) value does not fit in 16 bits".format(freqOffsetFactor)

        # Convert the floating point value to FXP<16.16>
        # NOTE: Bitwise AND (&) yields a long int
        freqOffsetFactor_fxp = (long(freqOffsetFactor*(2**16)) & 0XFFFFFFFF)

        # Finally, set model output variables
        model.vars.frequency_offset_factor.value = freqOffsetFactor
        model.vars.frequency_offset_factor_fxp.value = freqOffsetFactor_fxp

    def freq_comp_mode_index(self, model, mode):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
            mode (Enum FREQ_COMP_MODE) : FREQ_COMP_MODE
        """

        FREQ_COMP_MODE_LOOKUP = {
            model.vars.frequency_comp_mode.var_enum.DISABLED.value                                  : 0,
            model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT.value          : 1,
            model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_FRAME_DETECT.value             : 2,
            model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON.value                        : 3,
            model.vars.frequency_comp_mode.var_enum.AFC_FREE_RUNNING.value                          : 4,
            model.vars.frequency_comp_mode.var_enum.AFC_START_AT_PREAMBLE_FREE_RUNNING.value        : 5,
            model.vars.frequency_comp_mode.var_enum.AFC_LOCK_AT_TIMING_DETECT.value                 : 6,
            model.vars.frequency_comp_mode.var_enum.AFC_LOCK_AT_PREAMBLE_DETECT.value               : 7,
            model.vars.frequency_comp_mode.var_enum.AFC_LOCK_AT_FRAME_DETECT.value                  : 8,
            model.vars.frequency_comp_mode.var_enum.AFC_START_AT_PREAMBLE_LOCK_AT_FRAME_DETECT.value: 9,
        }

        return FREQ_COMP_MODE_LOOKUP[mode.value]


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

        if in_2fsk_opt_scope:
            if baudrate < 1900000:
                # Use INTERNAL_LOCK_AT_PREAMBLE_DETECT for most proprietary 2FSK PHYs
                mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
            else:
                mode = model.vars.frequency_comp_mode.var_enum.DISABLED

        # for amplitude modulation the frequency offset estimator is used to estimate
        # peak amplitude level which is used in slicer level determination.
        # best option for these modulations is to lock at FRAME detect
        elif modulation == model.vars.modulation_type.var_enum.OOK or \
           modulation == model.vars.modulation_type.var_enum.ASK:
            mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_FRAME_DETECT

        # enable AFC if we have at least 40 preamble bits and not too low a baudrate
        elif preamble_len >= 40 and baudrate > baudrate_limit:
            mode = model.vars.frequency_comp_mode.var_enum.AFC_LOCK_AT_PREAMBLE_DETECT

        # else default to INTERNAL compensation that is always running
        else:
            mode = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON

        model.vars.frequency_comp_mode.value = mode


    def calc_compmode_reg(self, model):
        """
        determine best internal frequency compensation mode and set COMPMODE register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mode = model.vars.frequency_comp_mode.value
        mode_index = self.freq_comp_mode_index(model, mode)

        # Turn off frequency compensation when running BER tests
        if model.vars.ber_force_freq_comp_off.value==True:
            self._reg_write(model.vars.MODEM_CTRL1_COMPMODE, 0)
        elif mode_index == 1:
            # enable internal compensation at preamble
            self._reg_write(model.vars.MODEM_CTRL1_COMPMODE, 1)
        elif mode_index == 2:
            # enable internal compensation at frame detect
            self._reg_write(model.vars.MODEM_CTRL1_COMPMODE, 2)
        elif mode_index == 3:
            # enable continues compensation
            self._reg_write(model.vars.MODEM_CTRL1_COMPMODE, 3)
        elif mode_index == 0 or mode_index >= 4:
            # disable internal compensation
            self._reg_write(model.vars.MODEM_CTRL1_COMPMODE, 0)
        else:
            raise CalculationException("ERROR: frequency_comp_mode not recognized!")


    def calc_fdm0thresh_val(self, model):
        """
        in FDM0 mode set FDM0THRESH register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        timingbases = model.vars.timingbases_actual.value
        scale = model.vars.freq_gain_scale.value

        # only used in FDM0 mode which is active if timingbases = 0
        if timingbases > 0:
            model.vars.fdm0_thresh.value = 0
            return

        # nominal frequency deviation is +/- 64 we like to set this threshold
        # to half of that so 32 but if the FREQGAIN setting is scaled to avoid
        # saturation we need to scale this value accordingly
        fdm0_thresh = 32 * scale

        if fdm0_thresh < 8:
            fdm0_thresh = 8
        elif fdm0_thresh > 71:      # Limiting so the register won't overflow
            fdm0_thresh = 71        # See calc_fdm0thresh_reg for details

        model.vars.fdm0_thresh.value = int(fdm0_thresh)


    def calc_fdm0thresh_reg(self, model):
        """
        set FMD0 register given calculated value
        Equation from definition of FDM0THRESH in registers list of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fdm0_thresh = model.vars.fdm0_thresh.value
        # The threshold can be up to 71 max, and still have the register not overflow a 3 bit field

        if fdm0_thresh == 0:
            reg = 0
        else:
            reg = int(fdm0_thresh / 8.0) - 1
            # Using the equation above, the threshold can be up to 71 max
            # and still have the register not overflow a 3 bit field.  A value of 72
            # will cause an overflow
            # I'm limiting fdm0_thresh and not the register because I'm trying to figure out
            # how to push the limiting upstream to inherently prevent the overflow, in case
            # this overflow is not the correct way to limit things.

        # The register is limited to a value of 7 (3 bits)
        self._reg_write(model.vars.MODEM_TIMING_FDM0THRESH, reg)


    def calc_offsub_reg(self, model):
        """
        calculate OFFSUBDEN and OFFSUBNUM register for more accurate frequency offset estimation
        described in Section 5.7.7.1 of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        #TODO: implement case when AFC is enabled
        rxbrfrac = model.vars.rxbrfrac_actual.value
        rxbrnum = model.vars.rxbrnum_actual.value
        timing_window = model.vars.timing_window_actual.value
        dssslen = model.vars.dsss_len.value
        brcalen = model.vars.brcalen.value

        #if (dssslen > 0 or brcalen > 0) and rxbrnum > 0:
        timing_samples = timing_window * rxbrfrac
        closest_power_of_two = 2**(math.floor(math.log(timing_samples, 2)))

        error_min = 1e9

        # find best den, num pair that gets us closest to 2**N
        for den in xrange(1, 16):
            for num in xrange(1, 16):

                error = abs(closest_power_of_two * num / den - timing_samples)

                if error < error_min:
                    error_min = error
                    best_den = den
                    best_num = num
        #else:
        #    best_num = 0
        #    best_den = 0

        self._reg_write(model.vars.MODEM_TIMING_OFFSUBDEN, best_den)
        self._reg_write(model.vars.MODEM_TIMING_OFFSUBNUM, best_num)


    def calc_offsub_actual(self, model):
        """
        calculate actual ratio

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        den = model.vars.MODEM_TIMING_OFFSUBDEN.value
        num = model.vars.MODEM_TIMING_OFFSUBNUM.value * 1.0

        if den != 0:
            model.vars.offsub_ratio_actual.value = num / den
        else:
            model.vars.offsub_ratio_actual.value = 0.0

    def afc_adj_limit(self, model):
        freq_limit = model.vars.freq_offset_hz.value
        res = model.vars.synth_res_actual.value

        return int(round(freq_limit / res))

    def calc_afcadjlim(self, model):
        """
        set AFCADJLIM register if AFC is enabled

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mode = model.vars.frequency_comp_mode.value
        freq_limit = model.vars.freq_offset_hz.value
        mode_index = self.freq_comp_mode_index(model, mode)

        # check if freq_offset_hz advanced input is forced
        if model.vars.freq_offset_hz.value_forced is None:
            forced = 0
        else:
            forced = 1

        # we only care about this register when AFC is enabled
        # if not set register to 0
        if mode_index >= 4:
            if freq_limit == 0:
                if forced:
                    # if offset limit is intentionally set to zero by forcing
                    # freq_offset_hz that means we want not limit in AFC range
                    # and we should set register to 0
                    afcadjlim = 0
                else:
                    # if offset limit is zero but not because we forced
                    # freq_offset_hz but because ppm values were set to zero
                    # we assume the user wants AFC on with no effect so we set
                    # the limit to minimum value of 1 - not sure why one would do this
                    # but we have PHYs setup this way
                    afcadjlim = 1
            else:
                # if offset limit is non-zero calculate the correct value
                afcadjlim = self.afc_adj_limit(model)

        else:
            afcadjlim = 0


        # make sure we are within register limits
        if afcadjlim > 2**18 - 1:
            afcadjlim = 2**18 - 1

        self._reg_write(model.vars.MODEM_AFCADJLIM_AFCADJLIM, afcadjlim)


    def calc_afc_adjlim_actual(self, model):

        afcadjlim = model.vars.MODEM_AFCADJLIM_AFCADJLIM.value
        res = model.vars.synth_res_actual.value

        model.vars.afc_limit_hz_actual.value = afcadjlim * res

    def calc_afc_scale_value(self, model):
        """
        calculate AFC scaling based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        freqgain = model.vars.freq_gain_actual.value
        mod_format = model.vars.modulation_type.value
        res = model.vars.synth_res_actual.value
        mode = model.vars.frequency_comp_mode.value
        scale = model.vars.afc_step_scale.value

        mode_index = self.freq_comp_mode_index(model, mode)

        if mode_index >= 4 and freqgain > 0:

            if mod_format == model.vars.modulation_type.var_enum.FSK2 or \
               mod_format == model.vars.modulation_type.var_enum.FSK4:
                afcscale = fxo / (dec0 * dec1 * dec2 * 256.0 * freqgain * res)
            else:
                afcscale = fxo / (dec0 * dec1 * dec2 * 256.0)

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

        m = py2round(afc_scale)
        e = 0

        while m > 31 and e < 7:
            m = math.ceil(m / 2.0)
            e += 1

        # if the scale value is less than 0.5 may be rounding it
        # to zero which would mean no frequency adjustment
        if afc_scale > 0 and m == 0:
            m = 1

        self._reg_write(model.vars.MODEM_AFC_AFCSCALEE, int(e))
        self._reg_write(model.vars.MODEM_AFC_AFCSCALEM, int(m))


    def calc_afc_scale_actual(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        e = float(model.vars.MODEM_AFC_AFCSCALEE.value)
        m = float(model.vars.MODEM_AFC_AFCSCALEM.value)

        model.vars.afc_scale_actual.value = m * 2**e


    def calc_afcrxmode_reg(self, model):
        """
        set the AFC RX mode based on input

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mode = model.vars.frequency_comp_mode.value

        mode_index = self.freq_comp_mode_index(model, mode)

        if model.vars.ber_force_freq_comp_off.value==True:
            rxmode = 0
        elif mode_index <= 3:
            rxmode = 0
        else:
            rxmode = mode_index - 3

        self._reg_write(model.vars.MODEM_AFC_AFCRXMODE, rxmode)


    def calc_afcdel_reg(self, model):
        """
        calculate AFC Delay based on over sampling rate (osr) if AFC is enabled

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mode = model.vars.frequency_comp_mode.value
        osr = model.vars.oversampling_rate_actual.value

        mode_index = self.freq_comp_mode_index(model, mode)

        # AFC mode
        if mode_index >= 4:
            # using magic number 3 here - might want to make this a config variable
            afcdel = int(py2round(3 * osr))
        else:
            afcdel = 0

        if afcdel > 31:
            afcdel = 31

        self._reg_write(model.vars.MODEM_AFC_AFCDEL, afcdel)


    def calc_afc_period(self, model):
        """
        calculate AFC period based on osr

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mode = model.vars.frequency_comp_mode.value
        osr = model.vars.oversampling_rate_actual.value
        offsub = model.vars.offsub_ratio_actual.value

        mode_index = self.freq_comp_mode_index(model, mode)

        if mode_index >= 4:
            if offsub == 0:
                afcavgper = int(math.log(4.0 * osr, 2))
            else:
                afcavgper = int(math.log((round(4.0 * osr / offsub)), 2))
        else:
            afcavgper = 0

        model.vars.afc_period.value = afcavgper



    def calc_afcavgper_reg(self, model):
        """
        calculate AFC period based on osr

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        self._reg_write(model.vars.MODEM_AFC_AFCAVGPER, model.vars.afc_period.value)


    def calc_afcxclr_reg(self, model):
        """
        clear AFC register at the beginning of each frame if enabled

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mode = model.vars.frequency_comp_mode.value
        mode_index = self.freq_comp_mode_index(model, mode)

        if mode_index >= 4:
            afcrxclr = 1
        else:
            afcrxclr = 0

        self._reg_write(model.vars.MODEM_AFC_AFCRXCLR, afcrxclr)


    def calc_ook_slicer(self, model):

        # empirically determined to work well used as default value
        model.vars.ook_slicer_level.value = 3


    def calc_freqoffestlim_reg(self, model):
        """
        calculate internal frequency offset limit

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modulation = model.vars.modulation_type.value
        deviation = model.vars.deviation.value
        freq_limit = model.vars.freq_offset_hz.value
        mode = model.vars.frequency_comp_mode.value
        slicer_level = model.vars.ook_slicer_level.value

        mode_index = self.freq_comp_mode_index(model, mode)

        # check if freq_offset_hz advanced input is forced
        if model.vars.freq_offset_hz.value_forced is None:
            forced = 0
        else:
            forced = 1

        # for OOK and ASK FREQOFFESTLIM is used as the slicer level
        # nothing to do with frequency offset compensation
        if modulation == model.vars.modulation_type.var_enum.OOK or \
           modulation == model.vars.modulation_type.var_enum.ASK:
            reg = slicer_level

        # only enable limit with internal compensation if user explicitly
        # set the freq_offset_hz advanced limit and deviation is non-zero
        elif forced and deviation > 0 and mode_index < 4:
            reg = int(round(freq_limit * 64.0 / deviation))

        else:
            reg = 0


        # if limit is larger than we can program in the register disable
        # the limit by setting the register to 0
        if reg > 127:
            reg = 0

        self._reg_write(model.vars.MODEM_CTRL1_FREQOFFESTLIM, reg)

    def calc_freqoffestper_val(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modulation = model.vars.modulation_type.value

        if modulation == model.vars.modulation_type.var_enum.BPSK:
            freqoffestper = 2
        elif modulation == model.vars.modulation_type.var_enum.OQPSK:
            freqoffestper = 1
        else:
            freqoffestper = 0

        model.vars.frequency_offset_period.value = freqoffestper

    def calc_freqoffestper_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        freqoffestper = model.vars.frequency_offset_period.value

        self._reg_write(model.vars.MODEM_CTRL1_FREQOFFESTPER, freqoffestper)

    def calc_afc_step_scale_val(self, model):
        """by default use scale of 1.0

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # by default use scale of 1.0
        model.vars.afc_step_scale.value = 1.0