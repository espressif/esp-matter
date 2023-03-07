from .pro2plusmodemfields import Pro2PlusDemodulatorFields

__all__ = ["Pro2OcelotDemodulatorFields"]

class Pro2OcelotDemodulatorFields(Pro2PlusDemodulatorFields):

    def __init__(self):
        super(Pro2OcelotDemodulatorFields, self).__init__()
        
    def _add_fields(self):
        # jira-1658: add bit[5:2] into ook_misc
        self.ook_limit_discharge = 1  # set for pro+;  clr for pro2 
        self.ook_squelch_en = 0       # default to 0
        self.ook_discharge_div = 0    # default to 0 

        self.ifpkd_th = 232           # default to 0xE8 

        self.spike_rm_en = 0
        self.spike_det_thd = 0

        self.arriving_src = 0
        self.signal_dsa_mode   = 0
        self.arr_rst_en = 0
        self.est_osr_en = 0
        self.arr_toler = 0
    
        self.arr_q_pm_en = 0
        self.arr_q_sync_en = 0
        self.rx_pream_src = 0
        self.bcr_sw_sycw = 0
        self.skip_pm_det = 0
        self.arrival_thd = 0

        self.oneshot_afc = 0
        self.anwait = 0 ###CRW Modified for BCR DEMOD in Ocelot###
        self.large_freq_err = 0
        self.afcgain_ovr_flw = 0
        self.afcma_en = 0
        self.oneshot_waitcnt = 0
#        
        self.arr_eye_qual = 0
#        
        self.rssi_arr_thd = 0
        self.eye_qua_sel = 0
        self.cc_assess_sel = 0
#        
        self.inst_rssi = 0
        self.decay_swal = 0
        self.mute_rssi_sel = 1
        self.mute_rssi_cnt = 0
        
        self.rssi_high_en = 0
        self.rssi_low_en = 0  
        self.arr_squelch = 0
        self.squelch_clk_en = 0
        self.rssi_sel = 1  # 0 in revB

        self.non_frzen = 0
        
        self.rx_sync_timeout_ph = 0
        
        self.pk_pm_manch = 0
        self.pk_sync_manch = 0
        self.pk_field_manch = 0
        
        self.bcr_align_en = 0
        
        self.thd4gfsk_com_en = 0
        self.thd4gfsk_comp_ratio = 0
        
        self.ndec3 = 0
        self.ndec2gain = 0
        self.ndec2agc = 0
        self.jump_hold = 0
        
        self.eyexest_en = 0
        self.eyexest_fast = 0
        
        self.duty_cycle_en = 0
        
        
        self.mcu_timer_src = 0
        self.low_duty = 0
        self.rxclk_irpt_src = 0

        # RSSI group
        # rssi_thresh not touched by calc
        self.rssijmpthd = 8  # 12 in revB; 8 in revC
        # API RSSI_CONTROL = 1
        # API RSSI_CONTROL2:
        self.enjmprx = 0
        self.jmpdlylen = 0
        self.enrssijmp = 0
        self.rssijmp_up = 0
        self.rssijmp_dwn = 0

        #self.modem_ph_th = 0  # this is fsk_mod_dly renamed by wentao
        self.close_hw_dly_comp = 0

        # jira 1120/1435 and 1362/1539  <-> jira 1362
        self.res_lockup_byp = 0
        self.diff0rst_en = 0    # 0 seems to be good for all cases

        self.fh_ch_number = 0
        return
        
