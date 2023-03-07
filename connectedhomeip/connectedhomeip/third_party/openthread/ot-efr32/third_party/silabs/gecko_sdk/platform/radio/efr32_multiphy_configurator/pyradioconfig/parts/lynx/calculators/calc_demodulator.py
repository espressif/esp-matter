from pyradioconfig.parts.panther.calculators.calc_demodulator import CALC_Demodulator_panther
import math
import itertools

class CALC_Demodulator_lynx(CALC_Demodulator_panther):

    def calc_fxo_or_fdec8(self, model):
        # We can not use rx_synth_freq_actual in these calculations due to circular dependency
        flag_using_half_rate_mode = (model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value == 1)
        if flag_using_half_rate_mode:
            model.vars.fxo_or_fdec8.value = model.vars.rx_synth_freq_actual.value / 128.0
        else:
            model.vars.fxo_or_fdec8.value = model.vars.rx_synth_freq_actual.value / 64.0

    def calc_viterbi_demod_expect_patt_value(self, model):
        #This function is lynx-specific because we have a special case for concurrent PHYs

        flag_using_Viterbi_demod = (model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value == 1)
        flag_using_BLELR = (model.vars.MODEM_LONGRANGE_LRBLE.value == 1)
        concurrent_phy = (model.vars.MODEM_COCURRMODE_CONCURRENT.value == 1)
        syncword0 = model.vars.syncword_0.value
        preamble_pattern = model.vars.preamble_pattern.value

        if (flag_using_Viterbi_demod and not flag_using_BLELR) or concurrent_phy:
            # Last bit of preamble followed by all but one bit of syncword
            viterbi_demod_expect_patt = (syncword0 >> 1) | ((preamble_pattern & 1) << 31)
            model.vars.viterbi_demod_expect_patt.value = viterbi_demod_expect_patt
        else:
            # set to default reset value
            model.vars.viterbi_demod_expect_patt.value = 0x123556B7

    def calc_rssi_rf_adjust_db(self, model):
        model.vars.rssi_rf_adjust_db.value = -11.0
