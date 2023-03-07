from pyradioconfig.parts.ocelot.calculators.calc_synth import CALC_Synth_ocelot
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

class Calc_Synth_Sol(CALC_Synth_ocelot):

    def calc_adc_rate_mode_reg(self, model):

        #This function handles writes to the registers impacting ADC clock mode and HFXO multiplier
        adc_rate_mode = model.vars.adc_rate_mode.value

        if(model.vars.adc_rate_mode.var_enum.FULLRATE == adc_rate_mode):
            reg = 0
        elif (model.vars.adc_rate_mode.var_enum.HALFRATE == adc_rate_mode):            
            reg = 1
        else:
            # Eigthrate
            reg = 0 # really a don't care in this mode

        self._reg_write(model.vars.RFFPLL0_RFFPLLCTRL1_DIVXADCSEL, reg)

    def calc_lo_side_regs(self, model):
        """
        calculate LOSIDE register in synth and matching one in modem

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        model.vars.lo_injection_side.value = model.vars.lo_injection_side.var_enum.HIGH_SIDE  # default to high-side
        lo_injection_side = model.vars.lo_injection_side.value

        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            loside = 1
        else:
            loside = 0

        # Write the registers
        self._reg_write(model.vars.SYNTH_IFFREQ_LOSIDE, loside)

    def calc_digmixctrl_regs(self, model):
        """
        calculate LOSIDE register in synth and matching one in modem

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        fefilt_selected = model.vars.fefilt_selected.value
        lo_injection_side = model.vars.lo_injection_side.value

        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            digiqswapen = 1
            mixerconj = 0
        else:
            digiqswapen = 0
            mixerconj = 1

        # Write the registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGMIXCTRL_DIGIQSWAPEN', digiqswapen)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGMIXCTRL_MIXERCONJ', mixerconj)


    def calc_tx_mode(self, model):

        softmodem_modulator_select = model.vars.softmodem_modulator_select.value

        if (softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD):
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE2
        else:
            CALC_Synth_ocelot.calc_tx_mode(self,model)

    def calc_adc_freq_actual(self, model):
        #For Sol this has been moved to the calc_fpll file
        pass

    def calc_if_frequency_target(self,model):
        #This function calculates the target IF frequency

        #The lower bound for signal content is determined by the lowest usable frequency range of the ADC (DC filtering produces noise near DC, varies based on sample rate)
        #This upper bound for signal content is determined by the highest usable frequency range of the ADC (noise shaping, varies based on sample rate)

        #Load model variables into local variables
        bandwidth_hz = model.vars.bandwidth_hz.value
        adc_rate_mode = model.vars.adc_rate_mode.value #We can't use the actual rate mode, because the IF goes into calculating the actual VCO, which goes into final ADC divider
        modulation_type = model.vars.modulation_type.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.EIGHTHRATE:
            bandwidth_adc_hz = 150e3
            band_edge_min = 50e3
        elif adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            bandwidth_adc_hz = 1.25e6
            band_edge_min = 50e3
        else:
            bandwidth_adc_hz = 2.50e6
            band_edge_min = 100e3

        if modulation_type == model.vars.modulation_type.var_enum.OFDM:
            ofdm_option = model.vars.ofdm_option.value
            #Setting the IF freq at 1/2 chsp
            if ofdm_option == model.vars.ofdm_option.var_enum.OPT1:
                if_frequency_hz = 600000
            elif ofdm_option == model.vars.ofdm_option.var_enum.OPT2:
                if_frequency_hz = 400000
            elif ofdm_option == model.vars.ofdm_option.var_enum.OPT3:
                if_frequency_hz = 200000
            else:
                if_frequency_hz = 100000
        else:
            if_frequency_min = 80e3
            if_frequency_hz = max(if_frequency_min, band_edge_min + bandwidth_hz / 2)

        if ( (if_frequency_hz + bandwidth_hz / 2) > bandwidth_adc_hz):
            LogMgr.Warning("WARNING: IF + BW/2 > ADC Bandwidth")

        #Load local variables back into model variables
        model.vars.if_frequency_hz.value = int(if_frequency_hz)

    def calc_pga_lna_bw_reg(self, model):

        # For Sol, the initial approach is to scale this to 4 ranges for the 4 filter options
        # 1:  BW3dB = 5 MHz -> max IF BW = 2.5 MHz
        # 2:  BW3dB = 2.5 MHz -> max IF BW = 1.25 MHz
        # 4:  BW3dB = 1.25 MHz -> max IF BW = 0.625 MHz
        # 8:  BW3dB = 0.625 MHz -> max IF BW = 0.3125 MHz

        if_frequency_hz = model.vars.if_frequency_hz.value
        bandwidth_hz = model.vars.bandwidth_hz.value
        if_bandwidth_hz = if_frequency_hz + bandwidth_hz / 2.0

        # Initial Ocelot approach:  same BW3dB for both poles, H(f) > -2dB at IF bandwidth edge
        # Adjusted thresholds by -15% for -2dB frequency based on Ocelot Silicon measurements
        if if_bandwidth_hz < 0.260e6:
            # eighth-rate modes
            # LNABWADJ = 15 for BW3dB = 0.625 MHz
            # PGABWMODE = 8 for BW3dB = 0.625 MHz
            mixer_bw_sel = 15
            pga_bw_sel = 8
        elif if_bandwidth_hz < 0.85*0.625e6:
            # half-rate modes
            # LNABWADJ = 7 for BW3dB = 1.25 MHz
            # PGABWMODE = 4 for BW3dB = 1.25 MHz
            mixer_bw_sel = 7
            pga_bw_sel = 4
        elif if_bandwidth_hz < 0.85*1.25e6:
            # half-rate modes
            # LNABWADJ = 3 for BW3dB = 2.5 MHz
            # PGABWMODE = 2 for BW3dB = 2.5 MHz
            mixer_bw_sel = 3
            pga_bw_sel = 2
        else:
            # full-rate modes
            # LNABWADJ = 0 for BW3dB = 5 MHz
            # PGABWMODE = 1 for BW3dB = 5 MHz
            mixer_bw_sel = 0
            pga_bw_sel = 1

        #For now, set mixer_bw_sel_boost to the same value as mixer_bw_sel (MCUW_RADIO_CFG-1174)
        mixer_bw_sel_boost = mixer_bw_sel

        self._reg_write(model.vars.RAC_PGACTRL_PGABWMODE, pga_bw_sel)
        self._reg_write(model.vars.AGC_LNABOOST_LNABWADJ, mixer_bw_sel)
        self._reg_write(model.vars.AGC_LNABOOST_LNABWADJBOOST, mixer_bw_sel_boost)

    def calc_ifadcpolldcobiashalf_reg(self, model):
        pass

    def calc_ifadcplldcofilter_reg(self, model):
        pass


