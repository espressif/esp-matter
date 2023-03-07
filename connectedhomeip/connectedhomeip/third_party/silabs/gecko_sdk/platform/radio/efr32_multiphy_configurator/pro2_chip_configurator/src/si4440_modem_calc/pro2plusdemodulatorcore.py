'''
Created on Apr 1, 2013

@author: sesuskic
'''
import math
from .filter_chain_lu_plus import FilterChainLuPlus
from .pro2demodulatorcore import Pro2DemodulatorCore
from .decode_api import enook, en2fsk, engfsk, en4gfsk, get_df
from .trueround import trueround
from .pro2plusmodemfields import Pro2PlusDemodulatorFields

__all__ = ["Pro2PlusDemodulatorCore"]

class Pro2PlusDemodulatorCore(Pro2DemodulatorCore):

    def __init__(self):
        self.fields = Pro2PlusDemodulatorFields()
        self._init_decim_table()

    def _init_decim_table(self):
        super(Pro2PlusDemodulatorCore, self)._init_decim_table()
        for x in range(8,11) :
            self.decim_table.append(2.0**x)
            self.decim_table.append(3*2.0**(x-1))

    #def _override_tx_rx_err(self, modem_calc):
    #    return

    def _init_force_conditions(self, modem_calc):
        self.force_condition1 = (self.fields.hModInd>=2 and self.fields.nonstdpk==0 and en2fsk(modem_calc.inputs.API_modulation_type))
        self.force_condition2 = 0

    def _init_BW_mod(self, modem_calc):
        self.BW_mod = modem_calc.inputs.API_Rsymb+2*get_df(modem_calc.inputs.API_modulation_type, modem_calc.inputs.API_Fdev) # modulation BW (Hz)

    def _init_preamble_pattern(self, modem_calc) :
        super(Pro2PlusDemodulatorCore, self)._init_preamble_pattern(modem_calc)
        if (modem_calc.inputs.API_pm_pattern in [0, 4, 100, 110, 120]) :  # 1010 PM pattern: Min pm length = 32 or 40 bits
            self.fields.pm_pattern = 0    # modem does pmdet
            self.fields.preath   = 20          # pmdet TH
            self.fields.skip_pmdet_ph = 1      # packet handler skips pmdet
            self.fields.skipsyn  = 0           # 1=skip sync-word search timeout reset
            self.fields.gear_sw  = 0           # AFC switches to lower gear after the pmdet
            self.fields.nonstdpk = 0           # standard packet
            self.fields.detector = 3           # min-max detector
            if (enook(modem_calc.inputs.API_modulation_type)) :
                self.fields.detector = 0
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
            elif (modem_calc.inputs.API_Manchester):
                self.fields.detector
            else:
                self.fields.detector = 3
            # force gear switch to PM detection for BER mode
            if(modem_calc.inputs.API_BER_mode==1) :
                self.fields.pm_pattern = 0
                self.fields.preath   = 6
                self.fields.gear_sw  = 0
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

# 2GFSK  pre/post cursor compensation is always disabed
    def _init_kphcomp_2fsk(self, modem_calc):
        self.kphcomp_2fsk = 0

    def _init_oneshot_afc(self, modem_calc):
        if (((modem_calc.inputs.API_afc_en == 2 or modem_calc.inputs.API_afc_en == 3)and ( not(modem_calc.inputs.API_pm_pattern == 1000)))and modem_calc.inputs.API_BER_mode==0):
            self.fields.oneshot_afc = 1
        else:
            self.fields.oneshot_afc = 0

    def _init_signal_dsa_mode  (self, modem_calc):
        if (modem_calc.inputs.API_dsa_mode == 2 or modem_calc.inputs.API_dsa_mode == 3):
            self.fields.duty_cycle_en = 1
            self.fields.signal_dsa_mode   = 1
        if (modem_calc.inputs.API_pm_pattern in [4, 100, 110, 120, 1000]) and (not enook(modem_calc.inputs.API_modulation_type)):
            self.fields.signal_dsa_mode   = 1
        else:
            self.fields.signal_dsa_mode   = self.fields.oneshot_afc
        if(modem_calc.inputs.API_dsa_mode==1):
            self.fields.signal_dsa_mode   = 1

    def _pre_calc_lut(self, modem_calc):
        super(Pro2PlusDemodulatorCore, self)._pre_calc_lut(modem_calc)
        self.fields.ndec3 = int(self.filter_chain_lu.nDec - self.fields.ndec1 - self.fields.ndec2)
        self._init_kphcomp_2fsk(modem_calc)
        self._init_oneshot_afc(modem_calc)
        self._init_signal_dsa_mode  (modem_calc)
        if(self.kphcomp_2fsk == 1):
            self.fields.thd4gfsk = 0
        self.fields.large_freq_err = (self.fields.tx_rx_err > 16*modem_calc.inputs.API_Rsymb*0.75 and self.fields.nonstdpk == 0 and en2fsk(modem_calc))
        if engfsk(modem_calc.inputs.API_modulation_type):
            self.gaussain_factor = 0.65
        else:
            self.gaussain_factor = 1.0

        if en4gfsk(modem_calc.inputs.API_modulation_type):
            self.max_hModInd = self.fields.hModInd*3
        else:
            self.max_hModInd = self.fields.hModInd

        Fdev_error_hi = int(modem_calc.inputs.Fdev_error/1000)
        Fdev_error_lo = int(modem_calc.inputs.Fdev_error-1000*Fdev_error_hi)
        if(Fdev_error_hi==0):
            Fdev_error_hi=-Fdev_error_lo
        if(Fdev_error_lo==0):
            Fdev_error_lo=-Fdev_error_hi

        if(modem_calc.inputs.API_BER_mode==0):
            if (en4gfsk(modem_calc.inputs.API_modulation_type)) and(Fdev_error_hi>15 or Fdev_error_lo>15):
                self.fields.thd4gfsk_comp_en =1
            else:
                self.fields.thd4gfsk_comp_en =0
        else:
            self.fields.thd4gfsk_comp_en =0

    def _calc_step_1(self, modem_calc):
        super(Pro2PlusDemodulatorCore, self)._calc_step_1(modem_calc)
        self.raw_diff = self.gaussain_factor*64*self.max_hModInd/self.OSR_rx_CH
        self.fields.rawflt_sel = 0 #0=6tap MA; 1=4-tap MA
        self.raweye_factor = 1.3    # magnified RAWEYE multiple for hi self.fields.hModInd && hi OSR

    def _calc_step_5(self, modem_calc):
        if (en4gfsk(modem_calc.inputs.API_modulation_type)) :
            self.fields.CR_gain = min(2047, trueround(2**17/(self.fields.OSR_rx_BCR*3*self.bcr_h)/2**modem_calc.inputs.API_ant_div)) # 11-bit
        else :
            self.fields.CR_gain = min(2047, trueround(2**17/(self.fields.OSR_rx_BCR*self.bcr_h)/2**modem_calc.inputs.API_ant_div)) # 11-bit

        if (modem_calc.IQ_CAL_setting!=0) : # for IQ calibration, BCR open-loop running
            self.fields.CR_gain = 0

    def _calc_step_6(self, modem_calc):
        self.fields.skip_pm_det = 0
        self.fields.arriving_src = 1

        if (((modem_calc.inputs.API_pm_pattern == 0 and modem_calc.API_Rb_error>=1)or
            modem_calc.inputs.API_pm_pattern == 1000 or
            modem_calc.inputs.API_pm_pattern == 100 ) and (modem_calc.inputs.API_BER_mode==0)) :
            self.fields.bcr_sw_sycw = 1
            if modem_calc.revc0_c1:
                self.fields.skip_pm_det = 1
            else:
              if(modem_calc.inputs.API_pm_pattern == 1000):
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
        self.fields.mute_rssi_cnt = 1

        # AFC 4-Tb MA filter:  if disabled, we can reduce PM length

        if(modem_calc.inputs.API_BER_mode==0):
            if (modem_calc.API_Rb_error>=1):
                self.fields.est_osr_en = self.fields.oneshot_afc
            else:
                self.fields.est_osr_en = 0
        else:
            self.fields.est_osr_en = 0

        if(modem_calc.inputs.API_ant_div==1 or modem_calc.inputs.API_pm_pattern == 1000 or self.fields.est_osr_en ==1):
            self.fields.oneshot_waitcnt = 1
        elif (modem_calc.inputs.API_pm_pattern == 0 and self.fields.est_osr_en ==1 ) :
            self.fields.oneshot_waitcnt = 3
        elif (modem_calc.inputs.API_pm_pattern == 100):
            self.fields.oneshot_waitcnt = 6
        else:
            self.fields.oneshot_waitcnt = 7

        # jira-1652 remove revc0_c1 condition for arr_q_sync_en
        if(modem_calc.inputs.API_BER_mode==0):
            if(modem_calc.inputs.API_pm_pattern==1000):
                self.fields.arr_q_sync_en = 1
                self.fields.bcr_align_en = 1
            elif(modem_calc.inputs.API_pm_pattern==100):
                self.fields.arr_q_sync_en = 0
                self.fields.bcr_align_en = 0
            elif(modem_calc.inputs.API_pm_pattern==15 and
                 enook(modem_calc.inputs.API_modulation_type)==0):
                self.fields.arr_q_sync_en = self.fields.signal_dsa_mode
                self.fields.bcr_align_en = 0
            else:
                self.fields.arr_q_sync_en = 0
                self.fields.bcr_align_en = self.fields.est_osr_en
        else:
            self.fields.arr_q_sync_en = 0
            self.fields.bcr_align_en = 0

        if modem_calc.revc0_c1:
            if(modem_calc.inputs.API_pm_pattern==100 and
               en4gfsk(modem_calc.inputs.API_modulation_type)):   # MBUS N2g mode
                self.fields.arr_q_pm_en = 0
            elif(modem_calc.inputs.API_pm_pattern==150 and
               enook(modem_calc.inputs.API_modulation_type)==0):
                self.fields.arr_q_pm_en = self.fields.signal_dsa_mode
            else:
                self.fields.arr_q_pm_en = 0

        if (self.fields.oneshot_afc==1 and (modem_calc.inputs.API_ant_div==1 or modem_calc.inputs.API_dsa_mode == 3)) or (self.fields.est_osr_en ==1):
           self.fields.afcma_en = 1
        else :
           self.fields.afcma_en = 0

       # if bcr_sw_sycw = 1; before signal is arriving, fast_gear = {2'b00,crfast[0]}
       # after signal arriving signal is detected and before the sync-word is detected,
       # fast_gear = {1'b0,crfast[2:1]}
        if(self.fields.bcr_sw_sycw and  modem_calc.inputs.API_pm_pattern != 1000):
            self.fields.crfast = 1
        else:
            self.fields.crfast = 0

        if (self.fields.pm_pattern == 0) :   # 1010 PM pattern: Min pm length = 32 bit
            if (modem_calc.API_Rb_error>=1) :
                self.fields.rxncocomp = 1
                self.fields.crgainx2  = 0
                self.fields.crslow = 0
                self.fields.crfast = 0
            elif(modem_calc.inputs.API_pm_pattern==100 and en4gfsk(modem_calc.inputs.API_modulation_type)==1):# MBUS N2g mode
                self.fields.rxncocomp = 0
                self.fields.crgainx2  = 0
                self.fields.crslow = 0
            else :
                self.fields.rxncocomp = 0
                self.fields.crgainx2  = 0
                self.fields.crslow = 2
        else : # non-standard pattern
            if (modem_calc.inputs.API_pm_pattern == 1000):
                self.fields.rxncocomp = 0
                self.fields.crgainx2  = 0
                self.fields.crslow = 2
            else:
                self.fields.rxncocomp = 0
                self.fields.crgainx2  = 0
                self.fields.crslow = 0

    def _calc_step_8(self, modem_calc):
        if modem_calc.inputs.API_pm_pattern == 1000:
            self.fields.dis_midpt = 0
        else:
            self.fields.dis_midpt = self.fields.nonstdpk

    def _calc_step_10(self, modem_calc):
        # AFC loop register  
        # MA/Mean searching period
        self.fields.ma_freqdwn = 0
        self.fields.sch_frzen  = 1


        self.fields.schprd_low = 4
        if (((self.fields.pm_pattern != 3 and enook(modem_calc.inputs.API_modulation_type)==0) or modem_calc.inputs.API_pm_pattern == 1000 )and ( modem_calc.inputs.API_BER_mode==0)):
            self.fields.schprd_h = 0
        else:
            self.fields.schprd_h = 2


        if (self.fields.pm_pattern==1 or self.fields.pm_pattern==2 ) :
            self.fields.sch_frzen = 0      # disable minmax search freeze for long 1 or 0 PM
        elif (modem_calc.inputs.API_pm_pattern==17) :
            self.fields.sch_frzen = 0      # disable minmax TH freeze
        elif modem_calc.inputs.API_pm_pattern == 1000:
            self.fields.sch_frzen = 1

        if ((modem_calc.inputs.API_pm_pattern == 0 and self.fields.est_osr_en ==1) or modem_calc.inputs.API_ant_div==1 ):
            self.fields.arrival_thd = 5
        else:
            self.fields.arrival_thd = 4
        self.fields.eyexest_en = 0
        self.fields.eyexest_fast = 1

        ## RSSI jump detection will be disabled for the following mode:
        ## (1)DSA-based RX hopping (2) OOK (3) ANT-DIV
        self.fields.rssi_sel = 1
        self.fields.enrssijmp = 1
        self.fields.enjmprx = 0
        self.fields.rssijmp_up = 1
        if ((modem_calc.inputs.API_dsa_mode == 1 or modem_calc.inputs.API_dsa_mode == 2)and modem_calc.inputs.API_BER_mode==0 and
            (enook(modem_calc.inputs.API_modulation_type)==0) and modem_calc.inputs.API_ant_div==0):
            self.fields.enjmprx = 1

    def _calc_step_12(self, modem_calc):
        # afc_gain_abs: 9-bit fraction
        # PRO2's AFC gain
        if (self.fields.nonstdpk==1 and self.fields.detector==2 ) : # MA nonstdpk
            self.afc_gain_abs_pro2 = 2**5*self.afc_gain_cal/self.raw_flt_gain/2/(2**self.afc_adj)
        elif (self.fields.nonstdpk==1 and self.fields.detector==3 ) : # minmax nonstdpk
            self.afc_gain_abs_pro2 = self.fields.OSR_rx_BCR/2**2*self.afc_gain_cal/self.raw_flt_gain
        else : # stdpk 2Tb
            self.afc_gain_abs_pro2 =  self.afc_gain_cal/2**self.afc_adj/2**4
        # PRO2+'s AFC gain
        if (self.fields.oneshot_afc == 1):
            if(self.fields.est_osr_en ==1):
                self.afc_gain_abs = self.afc_gain_abs_pro2 /16
            else:
                self.afc_gain_abs = self.afc_gain_abs_pro2/4
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
        # 9/11/2012 afclim to be modified once we know better the formula
    # 15-bit with 0 bit fraction
        if (modem_calc.inputs.API_dsa_mode == 3):
           if(modem_calc.inputs.API_fc <= 525000000):
              orig_afclim = trueround(1.4*2 ** 6 * self.BW_mod/2/self.fields.afc_gain / modem_calc.modulator.fields.dsm_ratio)
           else:
           ##LAb testing shows AFC limter is triggered falsely at 900MHz band. To avoid this false trigger, AFC limter value is set to a value more than we expected.
           ##We need to do more investigations.
              orig_afclim = trueround(2.8*2 ** 6 * self.BW_mod/2/self.fields.afc_gain / modem_calc.modulator.fields.dsm_ratio)
        else:
           orig_afclim = trueround(2 ** self.fields.oneshot_afc *
                                2 ** 6 * afc_RX_BW * 1000 *
                                (k_ratio + (0.04 - afc_RX_BW / 206 / 100) * self.afc_adj) /
                                self.fields.afc_gain / modem_calc.modulator.fields.dsm_ratio)
        return orig_afclim

    def _calc_step_14(self, modem_calc):
        # super(Pro2PlusDemodulatorCore, self)._calc_step_14(modem_calc)
        if (self.fields.signal_dsa_mode   == 1 or modem_calc.inputs.API_ant_div==1):
          if (modem_calc.inputs.API_Rsymb >=200000):
            self.fields.afc_shwait = 2 + self.fields.nonstdpk
            self.fields.afc_lgwait = 3 + self.fields.nonstdpk
          else:
            self.fields.afc_shwait = 1 + self.fields.nonstdpk
            self.fields.afc_lgwait = 2 + self.fields.nonstdpk
        else:
          if (modem_calc.inputs.API_Rsymb >=100000):
            self.fields.afc_shwait = 2 + self.fields.nonstdpk
            self.fields.afc_lgwait = 3 + self.fields.nonstdpk
          else:
            self.fields.afc_shwait = 1 + self.fields.nonstdpk
            self.fields.afc_lgwait = 2 + self.fields.nonstdpk

    def _calc_step_15(self, modem_calc):
        super(Pro2PlusDemodulatorCore, self)._calc_step_15(modem_calc)
        self.fields.rst_pkdt_period = 0
        if (enook(modem_calc.inputs.API_modulation_type)):
        ## Force to disable AFC estimator for OOK modem
             self.fields.afc_est_en = 0
        else:
           if(modem_calc.inputs.API_BER_mode==1):
             if modem_calc.revc0_c1 == False:
                self.fields.afc_est_en = 0
             else:
                self.fields.afc_est_en = 1
           else:
                self.fields.afc_est_en = 1

        if (enook(modem_calc.inputs.API_modulation_type) or modem_calc.inputs.API_BER_mode==1) :
            # self.fields.afc_est_en = 0
            self.fields.afc_fb_pll = 0
        else:
            # self.fields.afc_est_en = 1
            self.fields.afc_fb_pll = (modem_calc.inputs.API_afc_en ==2 or modem_calc.inputs.API_afc_en ==1)

    def _calc_step_17(self, modem_calc):
        #  4GFSK equalization
        if (en4gfsk(modem_calc.inputs.API_modulation_type) or
                  self.kphcomp_2fsk ==1):
            self.fields.phcompbyp = 0
        else:
            self.fields.phcompbyp = 1

        if (engfsk(modem_calc.inputs.API_modulation_type)) :
            self.fields.phcomp_gain_4gfsk1 = self.fields.phcompbyp*2**7 + 0  # 4GFSK
            self.fields.phcomp_gain_4gfsk0 = self.kphcomp_2fsk*2**7 + 26  # 4GFSK
        else :
            self.fields.phcomp_gain_4gfsk1 =  self.fields.phcompbyp*2**7 + 0  # 4FSK 12/11/2012
            self.fields.phcomp_gain_4gfsk0 =  2  # 4FSK 12/11/2012

    def _calc_step_20(self, modem_calc):
        # nonstdpk = 1 if we wnat to get raw data ouput & run 2TB frequency
        # estimator, set nonstdpk = 1 at this stage
        # to get raw data output for standard packet,nonstdpk will be forced to 1
        if (modem_calc.inputs.API_raw_demod==1 or self.fields.signal_dsa_mode   == 1) :
            self.fields.nonstdpk_final = 1
        else:
            self.fields.nonstdpk_final = self.fields.nonstdpk

    def _set_rxgainx2(self, modem_calc):
        self.fields.rxgainx2 = 0
        self.fields.jump_hold = 0 # jump_hold not exposed in API
        if (modem_calc.inputs.API_Rsymb >= 9600):
            self.fields.ndec2gain = 0
            self.fields.ndec2agc = 0
        elif (modem_calc.inputs.API_Rsymb > 1000):
            self.fields.ndec2gain = 1
            self.fields.ndec2agc = 1
        else: # default
            self.fields.ndec2gain = 2
            self.fields.ndec2agc = 1

    def _calc_step_22(self, modem_calc):
        # demodulator SFR register
        # ANT-DIV && ADC reset enable
        self.fields.en_adc_rst = 0
        self.fields.adcwatch = 0
        if (modem_calc.inputs.API_ant_div==1) :
            self.fields.antdiv = 4
            if (en4gfsk(modem_calc.inputs.API_modulation_type)) :
               self.fields.ant2pm_thd = 15 # default
            else :
               self.fields.ant2pm_thd = 8 # default
        else :
            self.fields.antdiv = 0


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

    def _calc_step_23(self, modem_calc):
        # # DSA RX hopping for super low data rate
        self.fields.rx_hopping_en = 0
        if (modem_calc.inputs.API_dsa_mode == 3):
            self.fields.rx_hopping_en = 1
            self.fields.signal_dsa_mode   = 1
            self.fields.fh_ch_number = int(modem_calc.inputs.API_fhch)

    def _calculate(self, modem_calc):
        super(Pro2PlusDemodulatorCore, self)._calculate(modem_calc)

        self.fields.spike_rm_en = self.fields.signal_dsa_mode
        # if(self.max_hModInd <= 1):
        #     self.fields.spike_det_thd = self.raw_diff * 2
        # else:
        #     self.fields.spike_det_thd = self.raw_diff
        self.min_spike_det_thd = 3

        if enook(modem_calc.inputs.API_modulation_type) and modem_calc.inputs.API_pm_len < 20:
            self.fields.spike_det_thd = max(self.min_spike_det_thd,min(127, round((20*48)/(self.fields.OSR_rx_BCR/8))))
        else:
            self.fields.spike_det_thd = max(self.min_spike_det_thd,min(127, round(self.raw_diff*0.95*2)))

        if (modem_calc.inputs.API_dsa_mode == 0 or self.fields.est_osr_en ==1):
            self.fields.arr_toler =  0
        elif (modem_calc.inputs.API_dsa_mode == 3):
            self.fields.arr_toler =  1
        else:
            if(self.OSR_rx_CH > 32.0):
               self.fields.arr_toler = 6
            elif(self.OSR_rx_CH > 24.0 and self.OSR_rx_CH <= 32.0):
               self.fields.arr_toler = 5
            elif(self.OSR_rx_CH > 16.0 and self.OSR_rx_CH <= 24.0):
               self.fields.arr_toler = 4
            elif(self.OSR_rx_CH > 12.0 and self.OSR_rx_CH <= 16.0):
               self.fields.arr_toler = 3
            elif(self.OSR_rx_CH > 10.0 and self.OSR_rx_CH <= 12.0):
               self.fields.arr_toler = 2
            else:
               self.fields.arr_toler = 1


        self.fields.rxcomp_lat = 0
        self.fields.esc_midpt = self.fields.est_osr_en
        if (modem_calc.inputs.API_afc_en == 1 and modem_calc.inputs.API_BER_mode==0):
            self.fields.ant_mode = 2
        else:
            self.fields.ant_mode = 1

        self.fields.non_frzen = 0
        self.fields.cc_assess_sel = 1
        self.fields.rssi_high_en = 1
        self.fields.rssi_low_en = 1
        self.fields.rssi_arr_thd = 120
        if (self.fields.thd4gfsk_comp_en == 1):
            ## for (MBUS Ng mode)
            self.fields.arr_eye_qual =  min(127,trueround(1.2*self.raweye_factor*self.raw_diff)+max(0,int(trueround(5*math.log(self.RX_CH_BW_k1*1000/self.BW_mod,10)))))
            self.fields.thd4gfsk_comp_ratio = int(trueround(0.702*256*self.OSR_rx_CH/3/self.raw_flt_gain/self.raweye_factor/self.gaussain_factor))
        else:
            self.fields.arr_eye_qual = min(127,trueround(1.4*self.raweye_factor*self.raw_diff)+max(0,int(trueround(5*math.log(self.RX_CH_BW_k1*1000/self.BW_mod,10)))))
            self.fields.thd4gfsk_comp_ratio = 0

        self.fields.arr_squelch = 0
        self.fields.squelch_clk_en = 0
        if (self.fields.signal_dsa_mode   == 1 and
            enook(modem_calc.inputs.API_modulation_type)):
            #self.fields.rssi_arr_thd = 40
            self.fields.squelch_clk_en = 1
            self.fields.arr_squelch = 1


        self.fields.bw_peak = 12

        # low duty cycle mode
        self.fields.mcu_timer_src = 0
        self.fields.rxclk_irpt_src =2

        if (modem_calc.inputs.API_dsa_mode == 2):
            self.fields.low_duty = 4
        elif (modem_calc.inputs.API_dsa_mode == 3):
            self.fields.low_duty = 2
        else:
            self.fields.low_duty = 0

    def calculate(self, modem_calc):
        self._pre_calc_setup(modem_calc)
        self.filter_chain_lu = FilterChainLuPlus(modem_calc)
        self._pre_calc_lut(modem_calc)
        self._calculate(modem_calc)
        self.field_map = self.fields.get_field_map()

