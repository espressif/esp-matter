from .pro2plusdemodulatorcore import Pro2PlusDemodulatorCore
from .pro2_ocelotmodemfields import Pro2OcelotDemodulatorFields
from .decode_api import engfsk,en2fsk,en4gfsk,enook, get_zero_if, get_fixed_if_mode, get_df
from .trueround import trueround
import math

class Pro2OcelotDemodulatorCore(Pro2PlusDemodulatorCore):

    def __init__(self):
        self.fields = Pro2OcelotDemodulatorFields()
        self._init_decim_table()

    def _calc_step_1(self, modem_calc):

        #  BCR loop & raw data mode register
        # sampling rate of CH filter
        ###CRW Modified for BCR DEMOD in Ocelot###
        self.Fs_rx_CH = modem_calc.inputs.API_Fs_rx_CH

        ###From Pro2 Calculator###
        # CH filter's OSR: # OSR_rx_CH = Fs_rx_CH/API_Rsymb;
        self.OSR_rx_CH = self.Fs_rx_CH / modem_calc.inputs.API_Rsymb
        # Calculate ndec0 in data filter
        if (enook(modem_calc.inputs.API_modulation_type)):  # OOK
            self.fields.ndec0 = math.floor(
                math.log(self.OSR_rx_CH / self.OOKminOSR, 2))  # make OSR >= 7 (#define self.OOKminOSR)
            # if (self.fields.ndec0 < 0):
            #     modem_calc.calc_log.add_to_log(
            #         '# Error: OOK filter BW is too small for the data rate. Please increase OOK_BW and try again.\n',
            #         True)
            self.fields.ndec0 = min(5, max(self.fields.ndec0, 0))  # limit to [1,5] for OOK!

            if (modem_calc.inputs.API_OSRtune < 0):
                modem_calc.warning2log += '# In OOK mode, OSR step down is prohibited. API_OSRtune is reset to 0 \n'
            #                 print(('CALC:input:OSRtune', 'In OOK mode, OSR step down is prohibited. API_OSRtune is reset to 0 '))
            else:  # >0
                Nndec0 = self.fields.ndec0 - modem_calc.inputs.API_OSRtune
                if (Nndec0 < 0):
                    newOSRt = self.fields.ndec0 - 1
                    self.fields.ndec0 = 1
                    modem_calc.warning2log += '# No room to step up BCR OSR by {} steps, instead OSRtune={} steps is applied \n'.format(
                        modem_calc.inputs.API_OSRtune, newOSRt)
                #                     print(('CALC:input:OSRtune', ' No room to step up BCR OSR by {} steps, instead OSRtune={} steps is applied'.format(modem_calc.inputs.API_OSRtune, newOSRt)))
                else:
                    self.fields.ndec0 = Nndec0
        else:  # FSK
            # during revB0 EZR2 eval we found self.ndec0>0 yields better performance in (G)FSK if
            # (self.fields.hModInd>=2 & OSR/H >=8) target OSR=24 ==>96 to improve BER at 2.4kbps hi-band
            OSR_per_h = self.OSR_rx_CH / self.fields.hModInd
            if (self.fields.hModInd >= 2 and not (enook(modem_calc.inputs.API_modulation_type)) and OSR_per_h >= 8):
                self.fields.ndec0 = max(min(5, trueround(math.log(self.OSR_rx_CH / 96, 2))),
                                        0)  # target OSR=24 =>96
            else:
                self.fields.ndec0 = 0  # originally self.ndec0=0 for FSK

        ###From Pro2+ Calculator###
        self.raw_diff = self.gaussain_factor * 64 * self.max_hModInd / self.OSR_rx_CH
        self.fields.rawflt_sel = 0  # 0=8tap MA
        self.raweye_factor = 1.3  # magnified RAWEYE multiple for hi self.fields.hModInd && hi OSR

    def _calc_step_2(self, modem_calc):
        # The peak of MA filter input includes two parts:
        # (1)DC-offset caused by tx-rx freq. error (2) frequency modulation
        peak_ma_in = (128 * self.fields.tx_rx_err / modem_calc.inputs.API_Rsymb / self.OSR_rx_CH) + self.raw_diff

        # Set MA gain to max. and adjust it later
        self.fields.rawgain = 3
        # The peak of MA filter output
        peak_ma_out = self.raweye_factor * peak_ma_in * (8 - 2 * self.fields.rawflt_sel)

        # 4/5/2012: minmax detector BAD with self.ndec0>0
        # force self.ndec0=0 as long as OSR<511 for FSK minmaxdet!! OSR overflow in low Rb /hi BW
        ndec0_opt0 = (self.fields.nonstdpk and self.fields.detector == 3 and not (
            enook(modem_calc.inputs.API_modulation_type)))
        if (ndec0_opt0 and self.OSR_rx_CH > 2 ** 9):
            self.fields.ndec0 = 1  # >511?  osr=ufix12.3

        # if self.ndec0 is used, make sure input of self.ndec0 is not overflow. ndc0_input[8:0] = MA_out[8:0]
        # if self.ndec0 is NOT used, make sure MA[12:0] output is not overflow
        if (self.fields.ndec0 > 0):
            if (peak_ma_out >= 2 ** 8):  # self.rawgain \in {0,1,2,3}
                self.fields.rawgain = min(
                    max(0, self.fields.rawgain - math.ceil((math.log((peak_ma_out / 2 ** 8), 2)))), 3)
        else:
            if (peak_ma_out >= 2 ** 12):
                self.fields.rawgain = min(max(0, self.fields.rawgain - math.ceil(math.log((peak_ma_out / 2 ** 12), 2))),3)

        ###CRW Modified for BCR DEMOD in Ocelot###
        if (enook(modem_calc.inputs.API_modulation_type)):
            tap2ma_gain = 1
        else:
            tap2ma_gain = 4

        self.raw_flt_gain = 16 ** ((self.fields.ndec0 > 0)) * (8 - 4 * self.fields.rawflt_sel) *tap2ma_gain  # 10-bit  in ocelot

        # expected raw data eye-opening
        self.fields.raweye = int(min(trueround(0.5*self.raweye_factor * self.raw_flt_gain * self.raw_diff), 2047))  # 11-bit


    def _calc_step_6(self, modem_calc):
        self.fields.skip_pm_det = 0
        self.fields.arriving_src = 1

        if ((modem_calc.inputs.API_pm_pattern == 0 and modem_calc.API_Rb_error >= 1) or
             modem_calc.inputs.API_pm_pattern == 1000):
            self.fields.bcr_sw_sycw = 1
            if modem_calc.revc0_c1:
                self.fields.skip_pm_det = 1
            else:
                if (modem_calc.inputs.API_pm_pattern == 1000):
                    self.fields.rx_pream_src = 0
                else:
                    self.fields.rx_pream_src = 1
        else:
            self.fields.bcr_sw_sycw = 0
            if modem_calc.revc0_c1:
                self.fields.skip_pm_det = 0
            else:
                self.fields.rx_pream_src = 0
        ####

        self.fields.mute_rssi_sel = 0
        if enook(modem_calc.inputs.API_modulation_type) and (modem_calc.inputs.API_pm_len < 20):
            #Change value of MUTERSSICNT for OOK with short preamble
            self.fields.mute_rssi_cnt = 3
        else:
            self.fields.mute_rssi_cnt = 1

        # AFC 4-Tb MA filter:  if disabled, we can reduce PM length

        if (modem_calc.inputs.API_BER_mode == 0 and modem_calc.inputs.API_pm_len > 37):
            if (modem_calc.API_Rb_error >= 1):
                self.fields.est_osr_en = self.fields.oneshot_afc
            else:
                self.fields.est_osr_en = 0
        else:
            self.fields.est_osr_en = 0

        if (
                modem_calc.inputs.API_ant_div == 1 or modem_calc.inputs.API_pm_pattern == 1000 or self.fields.est_osr_en == 1):
            self.fields.oneshot_waitcnt = 1
        else:
            self.fields.oneshot_waitcnt = 3

        # jira-1652 remove revc0_c1 condition for arr_q_sync_en
        if (modem_calc.inputs.API_BER_mode == 0) and not enook(modem_calc.inputs.API_modulation_type):
            if (modem_calc.inputs.API_pm_pattern == 1000):
                self.fields.arr_q_sync_en = 1
                self.fields.bcr_align_en = 1
            elif (modem_calc.inputs.API_pm_pattern == 100):
                self.fields.arr_q_sync_en = 0
                self.fields.bcr_align_en = self.fields.signal_dsa_mode
            elif (modem_calc.inputs.API_pm_pattern == 15 and
                  enook(modem_calc.inputs.API_modulation_type) == 0):
                self.fields.arr_q_sync_en = self.fields.signal_dsa_mode
                self.fields.bcr_align_en = 0
            else:
                self.fields.arr_q_sync_en = 0
                self.fields.bcr_align_en = self.fields.est_osr_en
        else:
            self.fields.arr_q_sync_en = 0
            self.fields.bcr_align_en = 0

        if modem_calc.revc0_c1:
            if (modem_calc.inputs.API_pm_pattern == 100 and
                    en4gfsk(modem_calc.inputs.API_modulation_type)):  # MBUS N2g mode
                self.fields.arr_q_pm_en = 0
            elif (modem_calc.inputs.API_pm_pattern == 150 and
                  enook(modem_calc.inputs.API_modulation_type) == 0):
                self.fields.arr_q_pm_en = self.fields.signal_dsa_mode
            else:
                self.fields.arr_q_pm_en = 0

        if (self.fields.oneshot_afc == 1 and (
                modem_calc.inputs.API_ant_div == 1 or modem_calc.inputs.API_dsa_mode == 3)) or (
                self.fields.est_osr_en == 1):
            self.fields.afcma_en = 1
        else:
            self.fields.afcma_en = 0

        # if bcr_sw_sycw = 1; before signal is arriving, fast_gear = {2'b00,crfast[0]}
        # after signal arriving signal is detected and before the sync-word is detected,
        # fast_gear = {1'b0,crfast[2:1]}
        if (self.fields.bcr_sw_sycw and modem_calc.inputs.API_pm_pattern != 1000):
            self.fields.crfast = 1
        else:
            self.fields.crfast = 0

        if (self.fields.pm_pattern == 0):  # 1010 PM pattern: Min pm length = 32 bit
            if (modem_calc.API_Rb_error >= 1):
                self.fields.rxncocomp = 1
                self.fields.crgainx2 = 0
                self.fields.crslow = 0
                self.fields.crfast = 0
            elif (modem_calc.inputs.API_pm_pattern == 100 and en4gfsk(
                    modem_calc.inputs.API_modulation_type) == 1):  # MBUS N2g mode
                self.fields.rxncocomp = 0
                self.fields.crgainx2 = 0
                self.fields.crslow = 0
            else:
                self.fields.rxncocomp = 0
                self.fields.crgainx2 = 0
                self.fields.crslow = 2
        else:  # non-standard pattern
            if (modem_calc.inputs.API_pm_pattern == 1000):
                self.fields.rxncocomp = 0
                self.fields.crgainx2 = 0
                self.fields.crslow = 2
            else:
                self.fields.rxncocomp = 0
                self.fields.crgainx2 = 0
                self.fields.crslow = 0

    def _calc_step_11(self, modem_calc):
        ###CRW Modified for BCR DEMOD in Ocelot###
        #  AFC Loop GAIN
        self.afc_gain_cal = (2 * modem_calc.inputs.API_Rsymb) / modem_calc.inputs.API_fb_frequency_resolution

        self.afc_adj = 1
        # if (modem_calc.inputs.API_Rsymb >= 100e3):  # 100kbps RX_BW = 206kHz
        #     self.afc_adj = 0
        # else:
        #     self.afc_adj = 1

    def _calc_step_12(self, modem_calc):
        # afc_gain_abs: 9-bit fraction
        # PRO2's AFC gain
        if (self.fields.nonstdpk==1 and self.fields.detector==2 ) : # MA nonstdpk
            self.afc_gain_abs_pro2 = 2**5*self.afc_gain_cal/self.raw_flt_gain/2/(2**self.afc_adj)
        elif (self.fields.nonstdpk==1 and self.fields.detector==3 ) : # minmax nonstdpk
            # The factor of 0.1 here is experimentally derived based on Mbus ModeS and ModeR testing
            # It results in much better frequency tolerance, and is approved by Wentao
            self.afc_gain_abs_pro2 = 0.1 * self.fields.OSR_rx_BCR/2**2*self.afc_gain_cal/self.raw_flt_gain
        else : # stdpk 2Tb
            self.afc_gain_abs_pro2 =  self.afc_gain_cal/2**self.afc_adj/2**4
        # PRO2+'s AFC gain
        if (self.fields.oneshot_afc == 1):
            if(self.fields.est_osr_en ==1):
                self.afc_gain_abs = self.afc_gain_abs_pro2 /16
            else:
                self.afc_gain_abs = self.afc_gain_abs_pro2/4
        else:
            if modem_calc.inputs.API_pm_len >= 48:
                self.afc_gain_abs = self.afc_gain_abs_pro2 / 16
            else:
                self.afc_gain_abs = self.afc_gain_abs_pro2

        if (modem_calc.inputs.API_ant_div==1 and (modem_calc.inputs.API_afc_en == 1 and modem_calc.inputs.API_BER_mode==0)):
             self.afc_gain_abs = self.afc_gain_abs_pro2/8

        self.afc_gain_b4satu = trueround(self.afc_gain_abs/2**3)      # w/ 6 bit fraction
        self.fields.afc_gain = max(min(4095, self.afc_gain_b4satu), 1)  # range: [1, 4095]
        self.fields.afcgain_ovr_flw = 0
        if self.afc_gain_b4satu > 4095:
            self.afc_gain_b4satu = trueround(self.afc_gain_b4satu/2)
            self.fields.afcgain_ovr_flw = 1
        self.fields.afc_gain = max(min(4095, self.afc_gain_b4satu), 1)  # range: [1, 4095]
        if self.fields.large_freq_err ==1:
            self.fields.afc_gain *= 2

    def _calc_orig_afclim(self, modem_calc, k_ratio, afc_RX_BW):
        ###CRW Modified for BCR DEMOD in Ocelot###
        # 9/11/2012 afclim to be modified once we know better the formula
        # 15-bit with 0 bit fraction
        orig_afclim = trueround(2 ** 6 * afc_RX_BW * 1000 *
                                (k_ratio + (0.04 - afc_RX_BW / 206 / 100) * self.afc_adj)
                                / self.fields.afc_gain / modem_calc.inputs.API_fb_frequency_resolution)
        return orig_afclim

    def _calc_step_22(self, modem_calc):
        # demodulator SFR register
        # ANT-DIV && ADC reset enable
        self.fields.en_adc_rst = 0
        self.fields.adcwatch = 0
        # if (modem_calc.inputs.API_ant_div==1) :
        #     self.fields.antdiv = 1
        #     if (en4gfsk(modem_calc.inputs.API_modulation_type)) :
        #        self.fields.ant2pm_thd = 15 # default
        #     else :
        #        self.fields.ant2pm_thd = 8 # default
        # else :
        #     self.fields.antdiv = 0
        # In Ocelot, PRO2 legacy ANT-DIV will be disabled
        self.fields.antdiv = 0
        ###CRW Modified for BCR DEMOD in Ocelot###
        if (self.fields.oneshot_afc == 1):
            self.fields.anwait = 0
        else:
            self.fields.anwait = 2

        # jira 1621 & 1218
        #For IF bandwidths >500kHz we will enable the ADC gain correction (MODEM_AGC_CONTROL = 0x02)
        # and the RSSI jump detector will be increased to 9dB.
        #For IF bandwidths <=500kHz we will disable the ADC gain correction (MODEM_AGC_CONTROL = 0x00)
        # and the RSSI jump detector will be increased to 6dB.
        if(self.Ch_Fil_Bw >500):
            self.fields.adc_gain_cor_en = 1
            self.fields.jmpdlylen = 0   # 0: 2Tb; 1: 4Tb
            self.fields.rssijmpthd = 8
        else:
            self.fields.adc_gain_cor_en = 0
            self.fields.jmpdlylen = 0   # Jira SI4440-1769
            self.fields.rssijmpthd = 6

        # jira-1362: set res_lockup_byp in BERT/ API bit in BCR_MISC0 available after revC2
        if (modem_calc.inputs.API_BER_mode==1):
            self.fields.res_lockup_byp = 1  # disable "consecutive diff counter"
            self.fields.enrssijmp = 0       # jira-1621: disable rssi-jump in BERT
        else:
            self.fields.res_lockup_byp = 0
            #if (self.fields.signal_dsa_mode  ==1 and self.fields.afcbd>0 and self.filter_k2>self.filter_k1):
            if (self.fields.enrssijmp==1 and self.fields.afcbd>0 and self.filter_k2>self.filter_k1):
                self.fields.rssijmpthd = 15   # sufficient across filters?

    def _init_signal_dsa_mode(self, modem_calc):
        if (modem_calc.inputs.API_dsa_mode == 2 or modem_calc.inputs.API_dsa_mode == 3):
            self.fields.duty_cycle_en = 1
            self.fields.signal_dsa_mode   = 1
        if (modem_calc.inputs.API_pm_pattern in [4, 100, 110, 120, 1000]) and (not enook(modem_calc.inputs.API_modulation_type)):
            self.fields.signal_dsa_mode   = 1
        else: #modem_calc.inputs.API_pm_len
            if modem_calc.inputs.API_pm_len < 48:
                self.fields.signal_dsa_mode = self.fields.oneshot_afc
        if(modem_calc.inputs.API_dsa_mode==1):
            self.fields.signal_dsa_mode   = 1

    def _init_preamble_pattern(self, modem_calc) :
        super(Pro2PlusDemodulatorCore, self)._init_preamble_pattern(modem_calc)
        if (modem_calc.inputs.API_pm_pattern in [0, 4, 100, 110, 120]) :  # 1010 PM pattern: Min pm length = 32 or 40 bits
            self.fields.pm_pattern = 0    # modem does pmdet
            self.fields.preath   = 28 # pmdet TH
            self.fields.skip_pmdet_ph = 1      # packet handler skips pmdet
            self.fields.skipsyn  = 0           # 1=skip sync-word search timeout reset
            self.fields.gear_sw  = 0           # AFC switches to lower gear after the pmdet
            self.fields.nonstdpk = 0           # standard packet
            self.fields.detector = 3           # min-max detector

            if (enook(modem_calc.inputs.API_modulation_type)) :
                self.fields.detector = 0
                if modem_calc.inputs.API_pm_len >= 24:
                    #Use a high preamble threshold for long preamble lengths
                    self.fields.preath = 20
                elif modem_calc.inputs.API_pm_len >= 20:
                    #Reduce preamble threshold for shorter preamble lengths
                    self.fields.preath = 15
                else:
                    #For very short preambles, use a very low threshold
                    self.fields.preath = 6

            if (modem_calc.inputs.API_ant_div==1) :  # AntDiv requires long '1010' preamble
                self.fields.preath = 8
            elif (modem_calc.inputs.API_dsa_mode == 1) and (not (enook(modem_calc.inputs.API_modulation_type))):  # DSA is enabled
                self.fields.preath = 3

        elif (modem_calc.inputs.API_pm_pattern == 13) :    #1010 PM pattern: Min pm length = 32 or 40 bits
            self.fields.preath   = 3

        elif (modem_calc.inputs.API_pm_pattern == 1000):
        # include API_pm-pattern=17 for Direct-Mode: plus BCRgear=0x09 / sch_frzen=0 / chprd_losw=1
            self.fields.pm_pattern = 3
            self.fields.preath   = 3
            self.fields.skip_pmdet_ph = 1
            self.fields.skipsyn  = 0
            self.fields.gear_sw  = 2
            self.fields.nonstdpk = 1
            if (enook(modem_calc.inputs.API_modulation_type)) :
                self.fields.detector = 1 - modem_calc.inputs.API_Manchester
                self.fields.preath = 6
            else:
                self.fields.detector = 3
            # force gear switch to PM detection for BER mode
            if(modem_calc.inputs.API_BER_mode==1) :
                self.fields.pm_pattern = 0
                self.fields.preath   = 6
                self.fields.gear_sw  = 0
                self.fields.skipsyn = 1
        elif (modem_calc.inputs.API_pm_pattern == 150) : # (for pm length >16 bits not 1010 patten); PH will give pmdet
            self.fields.pm_pattern = 3 # random pm, if pmdet is done, it's by PH
            self.fields.preath = 0             # pmdet TH
            self.fields.skip_pmdet_ph = 0      # packet handler skips pmdet
            self.fields.skipsyn = 0            # skip sync-word search timeout reset
            self.fields.gear_sw = 2            # to lower gear after signal arrival
            self.fields.nonstdpk = 1           # non std packet
            if (enook(modem_calc.inputs.API_modulation_type)) :
                self.fields.detector = 1-modem_calc.inputs.API_Manchester
            elif (modem_calc.inputs.API_Manchester) :
                self.fields.detector = 2
            else :
                self.fields.detector = 3
        else : # default to 15 (for pm length <16 bits); PH can NOT give pmdet
             self.fields.skipsyn = 0            # skip sync-word search timeout reset

    def _pre_calc_setup(self, modem_calc):
        self.fields.bw_peak = 12
        self._init_BW_mod(modem_calc)
        self._init_tx_rx_err(modem_calc)
        self._init_preamble_pattern(modem_calc)

        self.fields.zero_if = get_zero_if(modem_calc.inputs.API_if_mode)
        self.fields.fixed_if_mode = get_fixed_if_mode(modem_calc.inputs.API_if_mode)
        self.fields.df = get_df(modem_calc.inputs.API_modulation_type, modem_calc.inputs.API_Fdev)
        self.fields.thd4gfsk = min(0xffff, trueround(
            (2 ** 15 * modem_calc.inputs.API_Fdev) / modem_calc.inputs.API_Rsymb))  # unsigned 16-bit

        # modulation index: PRO2 max hModInd is 127
        self.fields.hModInd = 2 * modem_calc.inputs.API_Fdev / modem_calc.inputs.API_Rsymb

        self._init_force_conditions(modem_calc)
        if (self.force_condition1 or self.force_condition2):
            self.fields.pm_pattern = 0
            self.fields.nonstdpk = 1

            if (self.force_condition1):
                modem_calc.warning2log += '# Modulation index is >=2, async demodulator is used \n'
            else:
                modem_calc.warning2log += '# Freqency error between TX & RX is greater than sycn-deomd spec,  async demodulator is used \n'
        self._init_Ch_Fil_Bw(modem_calc)

        self.OOKminOSR = 32  # minimum (BCR) OSR for OOK
        self.raweye_factor = 1.1  # nominal RAWEYE multiple

        # channel filter 1-15 BW in kHz
        self.BW_table = [modem_calc.inputs.API_freq_xo / 30e6 * x for x in
                         [915.7, 824.5, 740.9, 661.1, 593.6, 535.4, 483.9, 436.1, 393.2, 353.2, 317.2, 282.5, 255.7,
                          230.2, 207.5]]