'''
Created on Apr 9, 2013

@author: sesuskic
'''
from .rxchflt_rtl import RxChFlt_rtl
from .trueround import trueround

__all__ = ["Pro2Registers",
           "Pro2PlusRegisters"]

class Pro2Registers(object):
    '''
    classdocs
    '''
    def __init__(self, modem_calc):
        '''
        Constructor
        '''
        self.registers = {}
        # rtl_tx_sfr_reg
        self.registers['rtl_txdata_rate_reg2'] = (int(modem_calc.modulator.fields.txdr) >> 16) & 0xff
        self.registers['rtl_txdata_rate_reg1'] = (int(modem_calc.modulator.fields.txdr) >> 8) & 0xff
        self.registers['rtl_txdata_rate_reg0'] = int(modem_calc.modulator.fields.txdr) & 0xff
        self.registers['rtl_freq_dev_reg2'] = (modem_calc.modulator.fields.freq_dev >> 16) & 0xff
        self.registers['rtl_freq_dev_reg1'] = (modem_calc.modulator.fields.freq_dev >> 8) & 0xff
        self.registers['rtl_freq_dev_reg0'] = modem_calc.modulator.fields.freq_dev & 0xff
        self.registers['rtl_freq_off_reg1'] = 0
        self.registers['rtl_freq_off_reg0'] = 0
        self.registers['rtl_fc_inte_reg'] = 128 + modem_calc.modulator.fields.fc_inte
        self.registers['rtl_fc_frac_reg2'] = ((modem_calc.modulator.fields.ramp_dly*2**21 + modem_calc.modulator.fields.fc_frac) >> 16) & 0xff
        self.registers['rtl_fc_frac_reg1'] = ((modem_calc.modulator.fields.ramp_dly*2**21 + modem_calc.modulator.fields.fc_frac) >> 8) & 0xff
        self.registers['rtl_fc_frac_reg0'] = (modem_calc.modulator.fields.ramp_dly*2**21 + modem_calc.modulator.fields.fc_frac) & 0xff
        # rtl_tx_ssr_reg
        self.registers['rtl_txnco_mod_reg3'] = (modem_calc.modulator.fields.txncomod_reg >> 24) & 0Xff
        self.registers['rtl_txnco_mod_reg2'] = (modem_calc.modulator.fields.txncomod_reg >> 16) & 0Xff
        self.registers['rtl_txnco_mod_reg1'] = (modem_calc.modulator.fields.txncomod_reg >> 8) & 0Xff
        self.registers['rtl_txnco_mod_reg0'] = (modem_calc.modulator.fields.txncomod_reg) & 0xff
        self.registers['rtl_vco_cali_count_tx1'] = (int(modem_calc.modulator.fields.vco_cali_count_tx) >> 8) & 0xff
        self.registers['rtl_vco_cali_count_tx0'] = int(modem_calc.modulator.fields.vco_cali_count_tx) & 0xff
        self.registers['rtl_DSM_CTRL'] = modem_calc.modulator.fields.DSM_CTRL
        self.registers['rtl_sy_sel_Band'] = modem_calc.modulator.fields.sy_sel_Band
        # rtl_rx_sfr_reg
        self.registers['rtl_ndec_reg'] = 64*modem_calc.demodulator.fields.ndec2 + 16*modem_calc.demodulator.fields.ndec1+ 2*modem_calc.demodulator.fields.ndec0 + modem_calc.demodulator.fields.rxgainx2
        self.registers['rtl_osr_rx_reg1'] = ((int(modem_calc.demodulator.fields.chflt_lopw*2**15) + int(modem_calc.demodulator.fields.ph0size*2**12) + int(modem_calc.demodulator.fields.OSR_rx_BCR)) >> 8) & 0xff 
        self.registers['rtl_osr_rx_reg0'] = (int(modem_calc.demodulator.fields.chflt_lopw*2**15) + int(modem_calc.demodulator.fields.ph0size*2**12)+ int(modem_calc.demodulator.fields.OSR_rx_BCR)) & 0xff
        self.registers['rtl_ncoff_reg2'] = ((int(modem_calc.demodulator.fields.bcrfbbyp*2**23+modem_calc.demodulator.fields.slicefbbyp*2**22+modem_calc.demodulator.fields.ncoff)) >> 16) & 0xff
        self.registers['rtl_ncoff_reg1'] = ((int(modem_calc.demodulator.fields.bcrfbbyp*2**23+modem_calc.demodulator.fields.slicefbbyp*2**22+modem_calc.demodulator.fields.ncoff)) >> 8) & 0xff
        self.registers['rtl_ncoff_reg0'] = (int(modem_calc.demodulator.fields.bcrfbbyp*2**23+modem_calc.demodulator.fields.slicefbbyp*2**22+modem_calc.demodulator.fields.ncoff)) & 0xff
        CR_gain_reg = (int(modem_calc.demodulator.fields.dis_midpt*2**15 +
                           modem_calc.demodulator.fields.esc_midpt*2**14 + 
                           modem_calc.demodulator.fields.rxcomp_lat*2**13 + 
                           modem_calc.demodulator.fields.rxncocomp* 2**12 + 
                           modem_calc.demodulator.fields.crgainx2*2**11 + 
                           modem_calc.demodulator.fields.CR_gain))
        self.registers['rtl_bcr_gain_reg1'] = (CR_gain_reg >> 8) & 0xff
        self.registers['rtl_bcr_gain_reg0'] = CR_gain_reg & 0xff
        self.registers['rtl_crgear_reg'] = (modem_calc.demodulator.fields.dwn3byp*2**7+ modem_calc.demodulator.fields.dwn2byp*2**6+(modem_calc.demodulator.fields.crfast*8+modem_calc.demodulator.fields.crslow)) & 0xff

        # rtl_rx_ssr_reg
        rtl_chflt1 = RxChFlt_rtl(modem_calc.demodulator.fields.rx1coearray)
        rtl_chflt2 = RxChFlt_rtl(modem_calc.demodulator.fields.rx2coearray)

        self.registers['rtl_chflt1_coe'] = rtl_chflt1.coe_7_0
        self.registers['rtl_chflt1_coe_m0'] = rtl_chflt1.coe_m0
        self.registers['rtl_chflt1_coe_m1'] = rtl_chflt1.coe_m1
        self.registers['rtl_chflt1_coe_m2'] = rtl_chflt1.coe_m2
        self.registers['rtl_chflt1_coe_m3'] = rtl_chflt1.coe_m3
        self.registers['rtl_afc_gear_reg'] = modem_calc.demodulator.fields.afcbd*2**7 + \
                                                modem_calc.demodulator.fields.afc_est_en*2**6 + \
                                                modem_calc.demodulator.fields.afc_gear_hi*2**3 + \
                                                modem_calc.demodulator.fields.afc_gear_lo
        self.registers['rtl_afc_timing_reg'] = modem_calc.demodulator.fields.afc_shwait*2**4 + \
                                                modem_calc.demodulator.fields.afc_lgwait
        self.registers['rtl_afc_gain1_reg'] = ((int(modem_calc.demodulator.fields.gear_sw*2**14 + 
                                                modem_calc.demodulator.fields.afc_fb_pll*2**13 + 
                                                modem_calc.demodulator.fields.afc_gain) >> 8)) & 0xff
        self.registers['rtl_afc_gain0_reg'] = int(modem_calc.demodulator.fields.gear_sw*2**14 +
                                               modem_calc.demodulator.fields.afc_fb_pll*2**13 +
                                               modem_calc.demodulator.fields.afc_gain) & 0xff
        self.registers['rtl_afclim1_reg'] = ((int(modem_calc.demodulator.fields.afc_freez_en*2**15 + 
                                              modem_calc.demodulator.fields.afclim) >> 8)) & 0xff
        self.registers['rtl_afclim0_reg'] = int(modem_calc.demodulator.fields.afc_freez_en*2**15 + 
                                             modem_calc.demodulator.fields.afclim) & 0xff
        self.registers['rtl_ook_cnt1_reg'] = modem_calc.demodulator.fields.s2p_map*2**6 + \
                                                modem_calc.demodulator.fields.ookfrz_en*2**5 + \
                                                modem_calc.demodulator.fields.ma_freqdwn*2**4 + \
                                                modem_calc.demodulator.fields.raw_syn*2**3 + \
                                                modem_calc.demodulator.fields.slicer_fast*2**2 + \
                                                modem_calc.demodulator.fields.Squelch
        self.registers['rtl_ook_cnt0_reg'] = 0
        self.registers['rtl_pk_reg'] = modem_calc.demodulator.fields.fast_ma*2**7 + \
                                        modem_calc.demodulator.fields.Attack*16 + \
                                        modem_calc.demodulator.fields.Decay
        self.registers['rtl_phcomp4gfsk1_reg'] = modem_calc.demodulator.fields.phcomp_gain_4gfsk1
        self.registers['rtl_phcomp4gfsk0_reg'] = modem_calc.demodulator.fields.phcomp_gain_4gfsk0
        self.registers['rtl_thd4gfsk1_reg'] = (int(modem_calc.demodulator.fields.thd4gfsk) >> 8) & 0xff
        self.registers['rtl_thd4gfsk0_reg'] = int(modem_calc.demodulator.fields.thd4gfsk) & 0xff
        self.registers['rtl_code4gfsk'] = modem_calc.demodulator.fields.code4gfsk
        self.registers['rtl_schprd_reg'] = modem_calc.demodulator.fields.sch_frzen*2**7 +\
                                            modem_calc.demodulator.fields.rawflt_sel*2**6 + \
                                            modem_calc.demodulator.fields.schprd_h*2**3 + \
                                            modem_calc.demodulator.fields.schprd_low
        self.registers['rtl_modem_ctrl_reg'] = modem_calc.demodulator.fields.ph_scr_sel*2**7 + \
                                                modem_calc.demodulator.fields.en2Tb_est*2**6 + \
                                                modem_calc.demodulator.fields.pm_pattern*2**4 + \
                                                modem_calc.demodulator.fields.detector*2**2 + \
                                                modem_calc.demodulator.fields.agc_slow*2**1 + \
                                                modem_calc.demodulator.fields.nonstdpk_final
        self. _set_agc_override(modem_calc)
        self.registers['rtl_agc_override3'] = (self.rtl_agc_override >> 24) & 0xff
        self.registers['rtl_agc_override2'] = (self.rtl_agc_override >> 16) & 0xff
        self.registers['rtl_agc_override1'] = (self.rtl_agc_override >> 8) & 0xff
        self.registers['rtl_agc_override0'] = self.rtl_agc_override & 0xff
        self.registers['rtl_vco_cali_count_rx1'] = (int(modem_calc.demodulator.fields.vco_cali_count_rx) >> 8) & 0xff
        self.registers['rtl_vco_cali_count_rx0'] = int(modem_calc.demodulator.fields.vco_cali_count_rx) & 0xff
        self.registers['rtl_chflt2_coe'] = rtl_chflt2.coe_7_0
        self.registers['rtl_chflt2_coe_m0'] = rtl_chflt2.coe_m0
        self.registers['rtl_chflt2_coe_m1'] = rtl_chflt2.coe_m1
        self.registers['rtl_chflt2_coe_m2'] = rtl_chflt2.coe_m2
        self.registers['rtl_chflt2_coe_m3'] = rtl_chflt2.coe_m3
        self.registers['rtl_ant_mode'] = modem_calc.demodulator.fields.ant_mode
        self.registers['rtl_DC_CAL4'] = modem_calc.demodulator.fields.enfzpmend*2**7 + \
                                        modem_calc.demodulator.fields.enafc_clksw*2**6 + \
                                        modem_calc.demodulator.fields.matap*2**5 + \
                                        modem_calc.demodulator.fields.dc_rst*2**4 + \
                                        modem_calc.demodulator.fields.dc_freez*2**3 + \
                                        modem_calc.demodulator.fields.dc_gear
        
        # IF freq reg
        if (modem_calc.demodulator.fields.zero_if==1):
            if_diff_2s = 0 
        else:
            if_diff_2s = (modem_calc.demodulator.fields.if_diff+2**18);

        if_freq_reg =(modem_calc.demodulator.fields.adc_gain_cor_en*2**21 + 
                      modem_calc.demodulator.fields.zero_if*2**20 +
                      modem_calc.demodulator.fields.fixed_if_mode*2**19 +
                      modem_calc.demodulator.fields.eninv_adcq*2**18 + 
                      if_diff_2s)

        self.registers['rtl_if_freq_reg2'] = (int(if_freq_reg) >> 16) & 0xff 
        self.registers['rtl_if_freq_reg1'] = (int(if_freq_reg) >> 8) & 0xff
        self.registers['rtl_if_freq_reg0'] = int(if_freq_reg) & 0xff
        
        self._add_registers(modem_calc)

    def _set_agc_override(self, modem_calc):
        self.rtl_agc_override = int(modem_calc.demodulator.fields.agcovpkt*2**31 + 
                                    modem_calc.demodulator.fields.ifpdslow*2**30 +
                                    modem_calc.demodulator.fields.rfpdslow*2**29 +
                                    modem_calc.demodulator.fields.sgi_n*2**28 +
                                    modem_calc.demodulator.fields.rst_pkdt_period*2**24 +
                                    modem_calc.demodulator.fields.measwin*2**20 +
                                    modem_calc.demodulator.fields.settlewin*2**16 +
                                    modem_calc.demodulator.fields.rfpd_decay*2**8 +
                                    modem_calc.demodulator.fields.ifpd_decay)
        
    def _add_registers(self, modem_calc):
        self.registers['raw_ctrl1_reg'] = (int(modem_calc.demodulator.fields.rawgain*2**13 + 1*2**12 + modem_calc.demodulator.fields.raweye) >> 8) & 0xff
        self.registers['raw_ctrl0_reg'] = int(modem_calc.demodulator.fields.rawgain*2**13 + 1*2**12 + modem_calc.demodulator.fields.raweye) & 0xff
        self.registers['rtl_preath'] = modem_calc.demodulator.fields.skipsyn*2**7+ modem_calc.demodulator.fields.preath
        self.registers['rtl_agc_ctrl_reg1'] = ((modem_calc.demodulator.fields.antdiv*2**7 + 
                                                modem_calc.demodulator.fields.adcwatch*2**4 + 
                                                modem_calc.demodulator.fields.en_adc_rst*2**3) & 0xff)
        return

class Pro2PlusRegisters(Pro2Registers):
    def __init__(self, modem_calc):
        super(Pro2PlusRegisters, self).__init__(modem_calc)
        
    def _set_agc_override(self, modem_calc):
        super(Pro2PlusRegisters, self)._set_agc_override(modem_calc)
        self.rtl_agc_override += int(modem_calc.demodulator.fields.enrssijmp*2**27 + 
                                     modem_calc.demodulator.fields.jmpdlylen*2**26 +
                                     modem_calc.demodulator.fields.enjmprx*2**25)
    
    def _add_registers(self, modem_calc):
        self.registers['rtl_agc_ctrl_reg1'] = ((modem_calc.demodulator.fields.antdiv*2**5 +
                                                modem_calc.demodulator.fields.adcwatch*2**4 + 
                                                modem_calc.demodulator.fields.en_adc_rst*2**3) & 0xff)
        self.registers['raw_ctrl1_reg'] = (int(modem_calc.demodulator.fields.rawgain*2**13 + modem_calc.demodulator.fields.non_frzen*2**12 + modem_calc.demodulator.fields.raweye) >> 8) & 0xff
        self.registers['raw_ctrl0_reg'] = int(modem_calc.demodulator.fields.rawgain*2**13 + modem_calc.demodulator.fields.non_frzen*2**12 + modem_calc.demodulator.fields.raweye) & 0xff
        self.registers['rtl_preath'] = modem_calc.demodulator.fields.skipsyn*2**7+modem_calc.demodulator.fields.rx_sync_timeout_ph*2**6+modem_calc.demodulator.fields.preath
        self.registers['rtl_spike_thd'] = trueround(modem_calc.demodulator.fields.spike_rm_en*2**7 +
                                                    modem_calc.demodulator.fields.spike_det_thd)
        self.registers['rtl_arr_ctrl'] = modem_calc.demodulator.fields.arriving_src*2**6 + \
                                            modem_calc.demodulator.fields.signal_dsa_mode  *2**5 + \
                                            modem_calc.demodulator.fields.arr_toler
        self.registers['rtl_arrival_thd'] = modem_calc.demodulator.fields.arr_q_pm_en*2**7 + \
                                            modem_calc.demodulator.fields.arr_q_sync_en*2**6 + \
                                            modem_calc.demodulator.fields.bcr_sw_sycw*2**5 + \
                                            modem_calc.demodulator.fields.skip_pm_det*2**4 + \
                                            modem_calc.demodulator.fields.arrival_thd
        self.registers['rtl_one_shot_afc'] = modem_calc.demodulator.fields.oneshot_afc*2**7 + \
                                                modem_calc.demodulator.fields.bcr_align_en*2**6 + \
                                                modem_calc.demodulator.fields.est_osr_en*2**5 + \
                                                modem_calc.demodulator.fields.afcma_en*2**4 + \
                                                modem_calc.demodulator.fields.oneshot_waitcnt
        self.registers['rtl_arr_eye_qual'] = modem_calc.demodulator.fields.arr_eye_qual
        self.registers['rtl_arr_rssi'] = (modem_calc.demodulator.fields.arr_squelch*2**7 +
                                          modem_calc.demodulator.fields.rssi_arr_thd)
        self.registers['rtl_rssimute'] = (modem_calc.demodulator.fields.rssi_sel*2**6 +
                                          modem_calc.demodulator.fields.rssi_high_en*2**5 +
                                          modem_calc.demodulator.fields.rssi_low_en*2**4 +
                                          modem_calc.demodulator.fields.mute_rssi_sel*2**3 +
                                          modem_calc.demodulator.fields.mute_rssi_cnt)
        self.registers['agc_ctrl_reg1'] = ((modem_calc.demodulator.fields.antdiv*2**7 +
                                            modem_calc.demodulator.fields.adcwatch*2**4 +
                                            modem_calc.demodulator.fields.en_adc_rst*2**3)
                                           & 0xff)
        self.registers['rtl_bw_peak'] = modem_calc.demodulator.fields.bw_peak & 0xff
        self.registers['rtl_arr_eye_qual'] = (int(modem_calc.demodulator.fields.eye_qua_sel*2**7 +
                                               modem_calc.demodulator.fields.arr_eye_qual)
                                              & 0xff)
        self.registers['rtl_chflt1_coe_m3'] += (modem_calc.demodulator.fields.eyexest_en*128 +
                                                modem_calc.demodulator.fields.eyexest_fast*64)
        self.registers['rtl_chflt2_coe_m3'] += (modem_calc.demodulator.fields.large_freq_err*2**7 +
                                                modem_calc.demodulator.fields.afcgain_ovr_flw*2**6)
        self.registers['rtl_ndec2_agc'] = (modem_calc.demodulator.fields.ndec3*2**5 +
                                           modem_calc.demodulator.fields.ndec2gain*2**3 +
                                           modem_calc.demodulator.fields.ndec2agc*2**2 +
                                           modem_calc.demodulator.fields.jump_hold)
        self.registers['rtl_duty_cycling'] = (modem_calc.demodulator.fields.mcu_timer_src*2**6 +
                                              modem_calc.demodulator.fields.rxclk_irpt_src*2**3 +
                                              modem_calc.demodulator.fields.low_duty)
        self.registers['rtl_pk_decay'] = (modem_calc.demodulator.fields.squelch_clk_en*2**2 +
                                         modem_calc.demodulator.fields.decay_swal)
        self.registers['rtl_lna_pga_max'] = modem_calc.demodulator.fields.cc_assess_sel*2**7+116
        self.registers['rtl_rssi_jump_thd_reg'] = modem_calc.demodulator.fields.rssijmp_up*2**7+modem_calc.demodulator.fields.rssijmpthd
        return