from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
import math

class CALC_DSA_ocelot(ICalculator):

    def buildVariables(self, model):
        pass

    def calc_longrange_timeout_threshold(self, model):
        """
        For COHDSA, wait for this time out after prefilter detect before shutting demod down.

        Args:
            model:

        Returns:

        """

        demod_select = model.vars.demod_select.value
        bitrate_gross = model.vars.bitrate_gross.value
        preamble_pattern_len = model.vars.preamble_pattern_len.value
        demod_rate = model.vars.demod_rate_actual.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            preamble_pattern_time_us = preamble_pattern_len * (1 / bitrate_gross) * 1e6

            # : timeout [us] = LRTIMEOUTTHD / demod_rate_MHz
            # : wait twice preamble pattern length
            demod_rate_MHz = demod_rate / 1e6
            lrtimeoutthd_reg = int(round(demod_rate_MHz * preamble_pattern_time_us * 2.0))
        else:
            lrtimeoutthd_reg = 0

        self._reg_write(model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD, lrtimeoutthd_reg)

    def calc_cohdsa_dynamic_threshold(self, model):
        demod_select = model.vars.demod_select.value
        chpwr_accumux_noise = model.vars.chpwraccu_noise.value
        base_frequency_hz = model.vars.base_frequency_hz.value

        if base_frequency_hz <= 500e6:
            static_cohdsa_threshold = 140
            baseline_dynamic_cohdsa_threshold = 140
        else:
            static_cohdsa_threshold = 100
            baseline_dynamic_cohdsa_threshold = 100

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRSPIKETHD, static_cohdsa_threshold)
            self._reg_write(model.vars.MODEM_COH2_FIXEDCDTHFORIIR, baseline_dynamic_cohdsa_threshold)
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH, int(round(chpwr_accumux_noise+6)))
        else:
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRSPIKETHD, 0)
            self._reg_write(model.vars.MODEM_COH2_FIXEDCDTHFORIIR, 0)

    def calc_cohdsa_mode(self, model):
        demod_select = model.vars.demod_select.value
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            enable_cohdsa = 1
        else:
            enable_cohdsa = 0
        self._reg_write(model.vars.MODEM_COH3_COHDSAEN, enable_cohdsa)

    def calc_cohdsa_dynamic_iir_filter_coefficient(self, model):
        demod_select = model.vars.demod_select.value
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : Set to maximum filtering
            dyniircoef = 3
        else:
            dyniircoef = 0
        self._reg_write(model.vars.MODEM_COH3_DYNIIRCOEFOPTION, dyniircoef)

    def calc_cohdsa_check_peak_index_length(self, model):
        # : Disable check if difference in the indices of prefilter correlation peaks is less than dsapeakindlen
        self._reg_write(model.vars.MODEM_COH3_DSAPEAKCHKEN, 0)
        self._reg_write(model.vars.MODEM_COH3_DSAPEAKINDLEN, 0)

    def calc_cohdsa_addwndsize(self, model):
        demod_select = model.vars.demod_select.value
        target_osr = model.vars.target_osr.value
        dsss_len = model.vars.dsss_len_actual.value

        # : For coherent demod, advance timing window from DSA detection by half symbol
        # : This is assuming that DSA detection occured on 3rd or 4th preamble.
        # : For now, choosing to detect dsa on 3rd or 4th preamble so that the difference between peak and noise will be
        # : high. This may result in sensitivity degradation since the static DSA threshold will be set high.
        # : If DSA detection occurs on 1st or 2nd preamble, need to DELAY (negative) by 2 symbols
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            wndsize = target_osr * dsss_len / 2.0
            #wndsize = math.pow(2,10) - (target_osr * 2.0 * dsss_len)
            #wndsize = math.pow(2, 10) - (target_osr * 3.0 * dsss_len)
        else:
            wndsize = 0

        wndsize = int(wndsize)
        self._reg_write(model.vars.MODEM_COH3_COHDSAADDWNDSIZE, wndsize)

    def calc_cohdsa_signal_select(self, model):
        demod_select = model.vars.demod_select.value

        # : For coherent demod, use 4 bits from 10 bit complex multiplier output for pre filter
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            number_of_bits_from_prefilter = 4
        else:
            number_of_bits_from_prefilter = 0

        self._reg_write(model.vars.MODEM_COH3_CDSS, number_of_bits_from_prefilter)

    def calc_dsamode_reg(self, model):

        if model.vars.dsa_enable.value == True:
            enable = 1
        else:
            enable = 0

        self._reg_write(model.vars.MODEM_DSACTRL_DSAMODE, enable)

    def calc_arrthd_reg(self, model):

        # Read in model variables
        phdsa_enabled = (model.vars.MODEM_DSACTRL_DSAMODE.value == 1)
        preamble_detection_length = model.vars.preamble_detection_length.value

        # If the phase DSA is disabled then set to do not care
        if phdsa_enabled:
            if preamble_detection_length < 12:
                arrthd = 4
            else:
                arrthd = 7
        else:
            arrthd = 15

        self._reg_write(model.vars.MODEM_DSACTRL_ARRTHD, value=arrthd) #We always care about this

    def calc_dsactrl_lowduty(self, model):

        #Read in model variables
        phdsa_enabled = (model.vars.MODEM_DSACTRL_DSAMODE.value == 1)

        # If the phase DSA is disabled then set to do not care
        if phdsa_enabled:
            do_not_care = False
        else:
            do_not_care = True

        lowduty = 0

        self._reg_write(model.vars.MODEM_DSACTRL_LOWDUTY, lowduty, do_not_care=do_not_care)


    def calc_phdsa_defaults(self, model):

        # Read in model variables
        phdsa_enabled = (model.vars.MODEM_DSACTRL_DSAMODE.value == 1)

        #If the phase DSA is disabled then set all to do not care
        if phdsa_enabled:
            do_not_care = False
            ampjupthd = 0
            spikethd = 0x64
            spikethdlo = 100
            arrtolerthd0lo = 2
            arrtolerthd1lo = 4
            powabsthdlog = 100
        else:
            do_not_care = True
            ampjupthd = 15
            spikethd = 255
            spikethdlo = 255
            arrtolerthd0lo = 31
            arrtolerthd1lo = 31
            powabsthdlog = 255

        #Write the registers
        self._reg_write(model.vars.MODEM_DSACTRL_AGCBAUDEN, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_AMPJUPTHD, ampjupthd, do_not_care=False) #We always care about this
        self._reg_write(model.vars.MODEM_DSACTRL_ARRTOLERTHD0, 2, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_ARRTOLERTHD1, 4, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_DSARSTON, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_FREQAVGSYM, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_GAINREDUCDLY, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_RESTORE, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_SCHPRD, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSACTRL_TRANRSTDSA, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD0_FDEVMAXTHD, 0x78, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD0_FDEVMINTHD, 12, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD0_SPIKETHD, spikethd, do_not_care=False) #We always care about this
        self._reg_write(model.vars.MODEM_DSATHD0_UNMODTHD, 4, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_AMPFLTBYP, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_DSARSTCNT, 2, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_FREQLATDLY, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_FREQSCALE, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_POWABSTHD, 0x1388, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_POWRELTHD, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_PWRDETDIS, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_PWRFLTBYP, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD1_RSSIJMPTHD, 6, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_FDADJTHD, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_FREQESTTHD, 6, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_INTERFERDET, 6, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_JUMPDETEN, 1, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_PMDETFORCE, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_PMDETPASSTHD, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD2_POWABSTHDLOG, powabsthdlog, do_not_care=False) #We always care about this
        self._reg_write(model.vars.MODEM_DSATHD3_FDEVMAXTHDLO, 120, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD3_FDEVMINTHDLO, 12, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD3_SPIKETHDLO, spikethdlo, do_not_care=False) #We always care about this
        self._reg_write(model.vars.MODEM_DSATHD3_UNMODTHDLO, 4, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD4_ARRTOLERTHD0LO, arrtolerthd0lo, do_not_care=False) #We always care about this
        self._reg_write(model.vars.MODEM_DSATHD4_ARRTOLERTHD1LO, arrtolerthd1lo, do_not_care=False) #We always care about this
        self._reg_write(model.vars.MODEM_DSATHD4_POWABSTHDLO, 5000, do_not_care=do_not_care)
        self._reg_write(model.vars.MODEM_DSATHD4_SWTHD, 0, do_not_care=do_not_care)

