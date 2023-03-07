'''
Created on Apr 5, 2013

@author: sesuskic
'''

__all__ = ["Pro2ModulatorFields",
           "Pro2DemodulatorFields"]

class Pro2ModulatorFields(object):
    def __init__(self):
        self.modtype = 0
        self.mapctrl = 0
        self.DSM_CTRL = 0
        self.sy_sel_Band = 0
        self.icpff_ctl = 0
        self.icpint_ctl = 0
        self.kvff_ctl = 0
        self.kvint_ctl = 0
        self.rff_ctl = 0
        self.cff_ctl = 0
        self.cff13_ctl = 0
        self.lpf_ff_bias = 0
        self.txdr = 0
        self.hbncomod = 0
        self.txncomod_reg = 0
        self.txncomod = 0
        self.txfd = 0
        self.freq_dev = 0
        self.ramp_dly = 0
        self.PA_TC = 0
        self.fc_inte = 0
        self.fc_frac = 0
        self.fhst_16b = 0
        self.w_size = 0
        
        self.ook_zero_IF_txdr = 0
        self.ook_zero_IF_txfd = 0
        self.dsm_ratio = 0
        self.vco_cali_count_tx = 0
        self.outdiv = 0

        self.tx_rx_err = 0
        self.txosrx2 = 0
        self.txosrx4 = 0
        
        self.en2fsk = 0
        self.engfsk = 0
        self.en4gfsk = 0
        self.enook = 0
        self.unmod = 0
        self.fixed_if_mode= 0
        self.zero_if = 0
        self.pll_delay = 0  # sequencer
        
    def get_field_map(self):
        return self.__dict__

class Pro2DemodulatorFields(object):
    def __init__(self):
        self.vco_rx_adj = 0
        self.ph_scr_sel = 0
        self.zero_if = 0
        self.fixed_if_mode = 0
        self.if_diff = 0
        self.ndec0 = 0
        self.ndec1 = 0
        self.ndec2 = 0
        self.chflt_lopw = 0
        self.droopfltbyp = 0
        self.dwn3byp = 0
        self.dwn2byp = 0
        self.rxgainx2 = 0
        self.OSR_rx_BCR = 0
        self.ncoff = 0
        self.CR_gain = 0
        self.crfast = 0
        self.crslow = 0
        self.bcrfbbyp = 0
        self.slicefbbyp = 0
        self.rxncocomp = 0
        self.rxcomp_lat = 0
        self.crgainx2 = 0
        self.dis_midpt = 0
        self.esc_midpt = 0
        self.adcwatch = 0
        self.en_adc_rst = 0
        self.distogg = 0
        self.ph0size = 0
        self.gear_sw = 0
        self.afc_gear_hi = 0
        self.afc_gear_lo = 0
        self.afc_shwait = 0
        self.afc_lgwait = 0
        self.afc_est_en = 0
        self.afcbd = 0
        self.afc_gain = 0
        self.afclim = 0
        self.afc_freez_en = 0
        self.afc_fb_pll = 0
        self.en2Tb_est = 0
        self.enfzpmend = 0
        self.enafc_clksw = 0
        self.ook_zerog = 0
        self.agcovpkt = 0
        self.ifpdslow = 0
        self.rfpdslow = 0
        self.sgi_n = 0
        self.rst_pkdt_period = 0
        self.agc_slow = 0
        self.adc_gain_cor_en = 0
        self.measwin = 0
        self.settlewin = 0
        self.rfpd_decay = 0
        self.ifpd_decay = 0
        self.phcompbyp = 0
        self.phcomp_gain_4gfsk1 = 0
        self.phcomp_gain_4gfsk0 = 0
        self.thd4gfsk = 0
        self.code4gfsk = 0
        self.Attack = 0
        self.Decay = 0
        self.s2p_map = 0
        self.ookfrz_en = 0
        self.ma_freqdwn = 0
        self.raw_syn = 0
        self.slicer_fast = 0
        self.Squelch = 0
        self.fast_ma = 0
        self.detector = 0
        self.sch_frzen = 0
        self.schprd_h = 0
        self.schprd_low = 0
        self.nonstdpk_final = 0
        self.nonstdpk = 0
        self.pm_pattern = 0
        self.rawgain = 0
        self.raweye = 0
        self.ant_mode = 0
        self.ant2pm_thd = 0
        self.matap = 0
        self.antdiv = 0
	# default RSSI group values 
        # self.RSSI_thresh = 255  # calculator doesn't write 
        # self.RSSI_jump_th = 12  # 12 for revB; 4 for revC
        #self.RSSI_ctrl = 1 # default
	#self.RSSI_ctrl_latch = 1
        #self.RSSI_ctrl_average = 0
        #self.RSSI_ctrl_chthd = 0
        self.RSSI_comp = 64
	#
        self.rx1coearray = 0
        self.rx2coearray = 0
        self.adc_hgain = 0
        self.adc_en_drst = 0
        self.realadc = 0
        self.dc_follow = 0
        self.dc_rst = 0
        self.dc_freez = 0
        self.dc_gear = 0
        self.afczifoff = 0
        self.hModInd = 0
        self.eninv_adcq = 0

        self.rawflt_sel = 0
        self.vco_cali_count_rx = 0

        self.bw_peak = 0

        # include now as calc may change this bit
        self.enrssijmp = 0

        self._add_fields()

    def _add_fields(self):
        self.sch_frz_th = 0
        
    def get_field_map(self):
        return self.__dict__
    

