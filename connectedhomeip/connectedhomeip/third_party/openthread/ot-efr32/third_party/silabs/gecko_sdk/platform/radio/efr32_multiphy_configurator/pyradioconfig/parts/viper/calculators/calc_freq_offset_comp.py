from pyradioconfig.parts.bobcat.calculators.calc_freq_offset_comp import Calc_Freq_Offset_Comp_Bobcat
from pyradioconfig.parts.viper.calculators.calc_utilities import Calc_Utilities_Viper

class calc_freq_offset_comp_viper(Calc_Freq_Offset_Comp_Bobcat):

    def calc_afc_scale_value(self, model):
        # Overriding this function due to variable name change

        # Load model values into local variables
        freqgain = model.vars.freq_gain_actual.value
        mod_format = model.vars.modulation_type.value
        mode = model.vars.frequency_comp_mode.value
        scale = model.vars.afc_step_scale.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remodoutsel = model.vars.MODEM_PHDMODCTRL_REMODOUTSEL.value
        digmix_res = model.vars.digmix_res_actual.value
        synth_res = model.vars.synth_res_actual.value
        phscale = 2 ** model.vars.MODEM_TRECPMDET_PHSCALE.value
        mode_index = self.freq_comp_mode_index(model, mode)
        demod_sel = model.vars.demod_select.value
        digmixfb = Calc_Utilities_Viper().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFBENABLE')
        baudrate = model.vars.rx_baud_rate_actual.value
        osr = model.vars.oversampling_rate_actual.value
        vtafcframe = model.vars.MODEM_REALTIMCFE_VTAFCFRAME.value
        afc_tx_adjust_enable = model.vars.afc_tx_adjust_enable.value
        afc_oneshot = model.vars.MODEM_AFC_AFCONESHOT.value
        bcr_det_en = model.vars.MODEM_PHDMODCTRL_BCRDETECTOR.value

        if digmixfb:
            res = digmix_res
        else:
            res = synth_res

        # AFC to synth for Legacy
        if(demod_sel==model.vars.demod_select.var_enum.LEGACY):
            if mode_index >= 4 and freqgain > 0:
                if mod_format == model.vars.modulation_type.var_enum.FSK2 or \
                    mod_format == model.vars.modulation_type.var_enum.MSK or \
                    mod_format == model.vars.modulation_type.var_enum.FSK4:
                    afcscale = baudrate * osr / ( 256 * freqgain * res)
                    afcscale_tx = baudrate * osr / ( 256 * freqgain * synth_res)
                else:
                    afcscale = baudrate * osr / ( 256 * res)
                    afcscale_tx = baudrate * osr / ( 256 * synth_res)
            else:
                afcscale = 0.0
                afcscale_tx = 0.0

        elif((demod_sel==model.vars.demod_select.var_enum.TRECS_VITERBI or
              demod_sel==model.vars.demod_select.var_enum.TRECS_SLICER) and
             model.vars.MODEM_VITERBIDEMOD_VITERBIKSI1.value != 0) or \
                demod_sel==model.vars.demod_select.var_enum.LONGRANGE:
            if remoden and remodoutsel == 1:
                afcscale = baudrate * osr * phscale / (256 * freqgain * res)
                afcscale_tx = baudrate * osr * phscale / (256 * freqgain * synth_res)
            else:
                if vtafcframe == 0:
                    #Digmix only updated one time, so use 100% scale to ensure we are in the channel bw
                    afcscale = 1.0 * baudrate * phscale / (256 * res)
                else:
                    #Digmix updated constantly, so use smaller scale to reduce jitter
                    afcscale = 0.8 * baudrate * phscale / (256 * res) #Less correction jitter if we use gain < 1
                afcscale_tx = baudrate * phscale / (256 * synth_res)

                # Reduce afc scale by eighth if oneshot is disabled. Following BLE case
                if afc_oneshot == 0 and bcr_det_en == 1:
                    afcscale = afcscale / 8.0
        elif (demod_sel == model.vars.demod_select.var_enum.BCR):
            # digital mixer frequency comp
            afcscale =  model.vars.pro2_afc_gain.value /  res
            afcscale_tx = model.vars.pro2_afc_gain.value / synth_res
        else:
            afcscale = 0.0
            afcscale_tx = 0.0

        afcscale = afcscale * scale

        #Special case to set afc_scale_tx to 0 to disable TX AFC adjust when using oneshot
        #See https://jira.silabs.com/browse/MCUW_RADIO_CFG-1510
        if (afc_tx_adjust_enable == False) and afc_oneshot:
            afcscale_tx = 0.0

        model.vars.afc_scale.value = afcscale
        model.vars.afc_scale_tx.value = afcscale_tx

    def afc_adj_limit(self, model):

        freq_limit = model.vars.freq_offset_hz.value
        synth_res = model.vars.synth_res_actual.value
        afclimreset = model.vars.afc_lim_reset_actual.value
        digmix_res = model.vars.digmix_res_actual.value
        digmixfb = Calc_Utilities_Viper().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFBENABLE')

        if digmixfb:
            res = digmix_res
        else:
            res = synth_res

        # calculate limit
        afcadjlim = freq_limit / res

        # if AFC_LIM_RESET is enabled we reset to the center frequency
        # once the accumulated offset reaches the limit. In this mode we
        # like to set the limit to about 20% higher than where we like the
        # limit to be
        if afclimreset:
            afcadjlim *= 1.2

        return int(round(afcadjlim))

    def calc_afc_adjlim_actual(self, model):

        afcadjlim = model.vars.MODEM_AFCADJLIM_AFCADJLIM.value
        synth_res = model.vars.synth_res_actual.value
        digmix_res = model.vars.digmix_res_actual.value
        digmixfb = digmixfb = Calc_Utilities_Viper().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFBENABLE')

        if digmixfb:
            res = digmix_res
        else:
            res = synth_res

        model.vars.afc_limit_hz_actual.value = afcadjlim * res
