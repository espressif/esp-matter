from pyradioconfig.parts.ocelot.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp_ocelot
from pyradioconfig.parts.common.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp
from pyradioconfig.parts.sol.calculators.calc_utilities import Calc_Utilities_Sol

class Calc_Freq_Offset_Comp_Sol(CALC_Freq_Offset_Comp_ocelot):
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
        digmixfb = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFBENABLE')
        osr = model.vars.oversampling_rate_actual.value
        vtafcframe = model.vars.MODEM_REALTIMCFE_VTAFCFRAME.value
        afc_tx_adjust_enable = model.vars.afc_tx_adjust_enable.value
        afc_oneshot = model.vars.MODEM_AFC_AFCONESHOT.value
        bcr_det_en = model.vars.MODEM_PHDMODCTRL_BCRDETECTOR.value
        freq_offset_hz = model.vars.freq_offset_hz.value
        baudrate = model.vars.baudrate.value
        deviation = model.vars.deviation.value

        if digmixfb:
            res = digmix_res
        else:
            res = synth_res

        # AFC to synth for Legacy
        if(demod_sel==model.vars.demod_select.var_enum.LEGACY):
            if mode_index >= 4 and freqgain > 0:
                if mod_format == model.vars.modulation_type.var_enum.FSK2 or \
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
                demod_sel==model.vars.demod_select.var_enum.LONGRANGE or \
            (demod_sel==model.vars.demod_select.var_enum.SOFT_DEMOD and
             mod_format == model.vars.modulation_type.var_enum.FSK2):
            if remoden and remodoutsel == 1:
                afcscale = baudrate * osr * phscale / (256 * freqgain * res)
                afcscale_tx = baudrate * osr * phscale / (256 * freqgain * synth_res)
            else:
                afcscale = 1.0 * baudrate * phscale / (256 * res)
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
        digmixfb = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFBENABLE')

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
        digmixfb = digmixfb = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFBENABLE')

        if digmixfb:
            res = digmix_res
        else:
            res = synth_res

        model.vars.afc_limit_hz_actual.value = afcadjlim * res

    def calc_afconeshoft_reg(self, model):

        modtype = model.vars.modulation_type.value
        run_mode = model.vars.afc_run_mode.value
        comp_mode = model.vars.frequency_comp_mode.value
        demod_select = model.vars.demod_select.value

        comp_mode_index = self.freq_comp_mode_index(model, comp_mode)

        if (run_mode == model.vars.afc_run_mode.var_enum.ONE_SHOT) and (modtype != model.vars.modulation_type.var_enum.OOK and modtype != model.vars.modulation_type.var_enum.ASK):
            oneshot = 1
        else:
            oneshot = 0

        if (comp_mode_index > 3) or (demod_select == model.vars.demod_select.var_enum.BCR):
            limreset = 1
        else:
            limreset = 0

        self._reg_write(model.vars.MODEM_AFC_AFCONESHOT, oneshot)
        self._reg_write(model.vars.MODEM_AFC_AFCDELDET, 0)
        self._reg_write(model.vars.MODEM_AFC_AFCDSAFREQOFFEST, 0)
        self._reg_write(model.vars.MODEM_AFC_AFCENINTCOMP, 0)
        self._reg_write(model.vars.MODEM_AFC_AFCLIMRESET, limreset)
        self._reg_write(model.vars.MODEM_AFC_AFCGEAR, 3)

    def calc_afcadjlim(self, model):
        """
        set AFCADJLIM register if AFC is enabled

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        demod_select = model.vars.demod_select.value

        if demod_select == model.vars.demod_select.var_enum.BCR:
            self._reg_write(model.vars.MODEM_AFCADJLIM_AFCADJLIM, self.afc_adj_limit(model))
        else:
            super().calc_afcadjlim(model)

