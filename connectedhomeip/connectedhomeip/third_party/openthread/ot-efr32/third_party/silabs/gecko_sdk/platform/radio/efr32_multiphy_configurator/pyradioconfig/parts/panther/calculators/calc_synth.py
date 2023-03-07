from pyradioconfig.parts.nixi.calculators.calc_synth import CALC_Synth_nixi


class CALC_Synth_panther(CALC_Synth_nixi):

    def __init__(self):
        super().__init__()
        self.synth_freq_min_limit = 2000000000
        self.synth_freq_max_limit = 3250000000

    def _get_synth_min_max(self):
        # Panther supports a wider frequency range: https://jira.silabs.com/browse/MCUW_RADIO_CFG-710
        synth_min = (4.0e3 / 2.0) * 1000000
        synth_max = (6.5e3 / 2.0) * 1000000
        return synth_min, synth_max

    def calc_lo_side_regs(self, model):
        """
        Starting with Panther, make this selectable by optional model.vars.lo_injection_side
        calculate LOSIDE register in synth and matching one in modem

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.lo_injection_side.value = model.vars.lo_injection_side.var_enum.HIGH_SIDE # default to high-side

        lo_injection_side = model.vars.lo_injection_side.value

        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            loside = 1
            digiqswapen = 1
            mixerconj = 0
        else:
            loside = 0
            digiqswapen = 0
            mixerconj = 1

        #Write the registers
        self._reg_write(model.vars.SYNTH_IFFREQ_LOSIDE, loside)
        self._reg_write(model.vars.MODEM_MIXCTRL_DIGIQSWAPEN, digiqswapen)
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_MIXERCONJ, mixerconj)

    def calc_rx_synth_freq_actual(self, model):
       #Overriding this method as SYNTH_CHCTRL_CHNO and SYNTH_CALOFFSET_CALOFFSET no longer exist in the Profile Outputs

        chan0_freq = model.vars.SYNTH_FREQ_FREQ.value
        chno = 0
        chan_spacing = model.vars.SYNTH_CHSP_CHSP.value
        cal_offset = 0
        if_freq = model.vars.SYNTH_IFFREQ_IFFREQ.value
        loside = model.vars.SYNTH_IFFREQ_LOSIDE.value
        res = model.vars.synth_res_actual.value
        lodiv = model.vars.lodiv_actual.value

        if loside:
            rx_synth_freq = (chan0_freq + chno * chan_spacing + cal_offset + if_freq) * res * lodiv
        else:
            rx_synth_freq = (chan0_freq + chno * chan_spacing + cal_offset - if_freq) * res * lodiv

        model.vars.rx_synth_freq_actual.value = int(round(rx_synth_freq))

    def calc_tx_synth_freq_actual(self, model):
        #Overriding this method as SYNTH_CHCTRL_CHNO and SYNTH_CALOFFSET_CALOFFSET no longer exist in the Profile Outputs

        chan0_freq = model.vars.SYNTH_FREQ_FREQ.value
        chno = 0
        chan_spacing = model.vars.SYNTH_CHSP_CHSP.value
        cal_offset = 0
        res = model.vars.synth_res_actual.value
        lodiv = model.vars.lodiv_actual.value

        tx_synth_freq = (1.0 * chan0_freq + chno * chan_spacing + cal_offset) * res * lodiv

        model.vars.tx_synth_freq_actual.value = int(tx_synth_freq)

    def calc_reg_ditherdsmoutput(self, model):
        # Removed because these registers no longer appear in the Profile Outputs
        pass

    def calc_synth_misc(self, model):
        # Removed because these registers no longer appear in the Profile Outputs
        pass