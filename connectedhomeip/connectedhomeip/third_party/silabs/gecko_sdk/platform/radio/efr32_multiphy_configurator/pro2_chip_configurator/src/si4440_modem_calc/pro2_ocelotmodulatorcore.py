import math
from .decode_api import get_ramp_dly, engfsk, get_df, enook
from .trueround import trueround
from .pro2modemfields import Pro2ModulatorFields
from .freq_ctl import freq_ctl
from .pll_setting_LUT import pll_setting_lut
from .pro2modulatorcore import Pro2PlusModulatorCore

class Pro2OcelotModulatorCore(Pro2PlusModulatorCore):

    def calculate(self, modem_calc):
        self.df = get_df(modem_calc.inputs.API_modulation_type, modem_calc.inputs.API_Fdev)
        pll_setting_lut(modem_calc)
        self._init_pa_tc(modem_calc)
        self._init_txosrx(modem_calc)
        self.fields.w_size = modem_calc.w_size

        self.fields.modtype = int(modem_calc.inputs.API_dsource * 8 + modem_calc.inputs.API_modulation_type)
        self.fields.mapctrl = int(
            modem_calc.inputs.API_Manchester * 128)  # inv_rxbit.format(inv_txbit.format(inv_fd.format(inv_adcq only change by explicit request
        # ##################  modulator's calculator ###################
        # calculating PLL parameters based on program frequency  API_fc
        self._get_freq_ctl(modem_calc)

        ###CRW Modified for BCR DEMOD in Ocelot###
        self.fields.dsm_ratio = modem_calc.inputs.API_dsm_ratio

        ''' modulator's register calculator '''
        # 4)calculating DSM register
        self.fields.fc_inte = self.fields.fbdiv_int - 1  # send to frac-N divider
        self.fields.fc_frac = int(trueround(self.fields.fbdiv_frac_19b) + 2 ** 19)
        self.fields.DSM_CTRL = 7  # change default of dsm_lsb to 1 from 0

        # clk_gen_stop register sy_sel_Band
        # b3: sy_sel <=> hi_pfm_div_mode; b[2:0] <=> band
        self.fields.sy_sel_Band = self.OUTDIV_SY_SEL_MAP.get(self.fields.outdiv, 5)

        self.fields.sy_sel_Band = int(modem_calc.inputs.API_hi_pfm_div_mode * 2 ** 3 + self.fields.sy_sel_Band)

        if (modem_calc.inputs.API_hi_pfm_div_mode == 1):
            self.fields.vco_cali_count_tx = self.fields.fc_inte * 1 * self.fields.w_size + trueround(
                1 * self.fields.w_size * self.fields.fc_frac / 2 ** 19)
        else:
            self.fields.vco_cali_count_tx = self.fields.fc_inte * 2 * self.fields.w_size + trueround(
                2 * self.fields.w_size * self.fields.fc_frac / 2 ** 19)

            # retrieve mod source from combined input
        # MODEM_MOD_TYPE dsource = MOD_SOURCE + TX_DIRECT_MODE_GPIO*2**2 + TX_DIRECT_MODE_TYPE*2**4
        mod_source = modem_calc.inputs.API_dsource % 4
        TX_DIRECT_MODE_TYPE = math.floor(modem_calc.inputs.API_dsource / 2 ** 4)  # 1: async; 0: sync

        # calculating Data rate
        if ((mod_source == 1) and (TX_DIRECT_MODE_TYPE == 1)):
            self.fields.txncomod = modem_calc.inputs.API_freq_xo
            self.fields.txdr = 500000
        else:
            self.fields.txncomod = modem_calc.inputs.API_freq_xo
            self.fields.txdr = modem_calc.inputs.API_Rsymb * 10

        if (self.fields.txosrx2 == 1):
            self.fields.txdr = 2 * self.fields.txdr
        elif (self.fields.txosrx4 == 1):
            self.fields.txdr = 4 * self.fields.txdr

        # calculating fsk modulation deviation df
        self.fields.txfd = int(min(max(0, trueround(self.df / self.fields.dsm_ratio)), 2 ** 17 - 1))  # 17-bit

        ### frequency deviation
        self.fields.freq_dev = int(
            mod_source * 2 ** 20 + modem_calc.inputs.API_modulation_type * 2 ** 17 + self.fields.txfd)

        ###### TX NCO modulo
        eninv_rxbit = 0
        eninv_txbit = 0
        eninv_fd = 0
        self.fields.txncomod_reg = int(
            modem_calc.inputs.API_Manchester * 2 ** 31 + eninv_rxbit * 2 ** 30 + eninv_txbit * 2 ** 29 + eninv_fd * 2 ** 28 + self.fields.txosrx2 * 2 ** 27 + self.fields.txosrx4 * 2 ** 26 + self.fields.txncomod)
        self.fields.hbncomod = int(self.fields.txosrx2 * 2 ** 27 + self.fields.txosrx4 * 2 ** 26 + self.fields.txncomod)

        if (modem_calc.inputs.API_Rsymb <= 5000):
            ook_zero_IF_df = modem_calc.inputs.API_Rsymb * 6
            self.fields.ook_zero_IF_txdr = 4 * self.fields.txdr
        elif (modem_calc.inputs.API_Rsymb > 5000 and modem_calc.inputs.API_Rsymb <= 40000):
            ook_zero_IF_df = 30000
            self.fields.ook_zero_IF_txdr = 20000 + 2 * self.fields.txdr
        elif (modem_calc.inputs.API_Rsymb > 40000 and modem_calc.inputs.API_Rsymb <= 100000):
            ook_zero_IF_df = modem_calc.inputs.API_Rsymb * 1.5
            self.fields.ook_zero_IF_txdr = self.fields.txdr
        else:
            ook_zero_IF_df = 150000
            self.fields.ook_zero_IF_txdr = self.fields.txdr

        self.fields.ook_zero_IF_txfd = min(max(0, trueround(ook_zero_IF_df / self.fields.dsm_ratio)),
                                           2 ** 17 - 1)  # 17-bit

        self.fields.ramp_dly = get_ramp_dly(modem_calc.inputs.API_modulation_type, modem_calc.inputs.API_if_mode)

        #################### end of modulator's calculator ###############

        self.field_map = self.fields.get_field_map()