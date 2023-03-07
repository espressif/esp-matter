'''
Created on Apr 4, 2013

@author: sesuskic
'''
from .pro2apilist import Pro2ApiList
from .trueround import trueround

__all__ = ["Pro2PlusApiList"]

class Pro2PlusApiList(Pro2ApiList):
    def _add_seq_cfg(self, modem_calc, api_list):
#         api_list['SEQ_CFG0'] = (modem_calc.modulator.fields.close_hw_dly_comp)  # need API in FW
        pass
    def _add_modem_raw_search_api(self, modem_calc, api_list):
        sch_frzen = modem_calc.demodulator.fields.sch_frzen
        rawflt_sel = modem_calc.demodulator.fields.rawflt_sel
        schprd_h = modem_calc.demodulator.fields.schprd_h
        schprd_low = modem_calc.demodulator.fields.schprd_low
        api_list['MODEM_RAW_SEARCH2'] = sch_frzen * 2 ** 7 + rawflt_sel * 2**6 + schprd_h * 2 ** 3 + schprd_low

    def _add_chflt_rx_apis(self, modem_calc, api_list):
        super(Pro2PlusApiList, self)._add_chflt_rx_apis(modem_calc, api_list)

        spike_rm_en = modem_calc.demodulator.fields.spike_rm_en
        spike_det_thd = modem_calc.demodulator.fields.spike_det_thd
        api_list['MODEM_SPIKE_DET'] = int(trueround(spike_rm_en*2**7 + spike_det_thd)) & 0xff

        arriving_src = modem_calc.demodulator.fields.arriving_src
        signal_dsa_mode   = modem_calc.demodulator.fields.signal_dsa_mode
        arr_rst_en = modem_calc.demodulator.fields.arr_rst_en
        est_osr_en = modem_calc.demodulator.fields.est_osr_en
        arr_toler = modem_calc.demodulator.fields.arr_toler
        api_list['MODEM_DSA_CTRL1'] = (arriving_src*2**6+signal_dsa_mode  *2**5+arr_toler) & 0xff
        # jira-1652: put arr_q_sync_en  back into rev-c2
        arr_q_sync_en = modem_calc.demodulator.fields.arr_q_sync_en
        if modem_calc.revc0_c1:
            arr_q_pm_en = modem_calc.demodulator.fields.arr_q_pm_en
            skip_pm_det = modem_calc.demodulator.fields.skip_pm_det
        else:
            rx_pream_src = modem_calc.demodulator.fields.rx_pream_src
            
        bcr_sw_sycw = modem_calc.demodulator.fields.bcr_sw_sycw
        arrival_thd = modem_calc.demodulator.fields.arrival_thd
        if modem_calc.revc0_c1:
            api_list['MODEM_DSA_CTRL2'] = (arr_q_pm_en*2**7+arr_q_sync_en*2**6+bcr_sw_sycw*2**5+skip_pm_det*2**4+arrival_thd) & 0xff
        else:
            api_list['MODEM_DSA_CTRL2'] = (arr_q_sync_en*2**6 + bcr_sw_sycw*2**5 + arrival_thd) & 0xff

        api_list['MODEM_ONE_SHOT_AFC'] = ((modem_calc.demodulator.fields.oneshot_afc*2**7 +
                                                modem_calc.demodulator.fields.bcr_align_en*2**6 +
                                                modem_calc.demodulator.fields.est_osr_en*2**5 +
                                                modem_calc.demodulator.fields.afcma_en*2**4 +
                                                modem_calc.demodulator.fields.oneshot_waitcnt) & 0xff)
        api_list['MODEM_DSA_QUAL'] = (int(modem_calc.demodulator.fields.eye_qua_sel*2**7 +
                                                 modem_calc.demodulator.fields.arr_eye_qual)
                                                & 0xff) 
        api_list['MODEM_DSA_RSSI'] =(modem_calc.demodulator.fields.arr_squelch*2**7 +
                                           modem_calc.demodulator.fields.rssi_arr_thd)
        api_list['MODEM_DECIMATION_CFG2'] = ((modem_calc.demodulator.fields.ndec3*32 +
                                                  modem_calc.demodulator.fields.ndec2gain*8 +
                                                  modem_calc.demodulator.fields.ndec2agc*4) & 0xff)
        # jira-1651: set IFPKD-TH for ETSI modes
        if modem_calc.revc0_c1 == False:
            api_list['MODEM_IFPKD_THRESHOLDS'] = modem_calc.demodulator.fields.ifpkd_th;

        api_list['MODEM_RSSI_MUTE'] = (modem_calc.demodulator.fields.mute_rssi_sel*2**3 +
                                            modem_calc.demodulator.fields.mute_rssi_cnt)
        api_list['MODEM_DSA_MISC'] = (modem_calc.demodulator.fields.eyexest_en*2**6 +
                                            modem_calc.demodulator.fields.eyexest_fast*2**5 +
                                            modem_calc.demodulator.fields.low_duty)
        if modem_calc.revc0_c1 == False:                                            
            api_list['PREAMBLE_CONFIG'] = rx_pream_src*2**7 + 0x21
        # DSA RX hopping for super low data rate
        if(modem_calc.demodulator.fields.rx_hopping_en ==1):
            api_list['MODEM_DSM_CTRL'] = 0x13
            api_list['RX_HOP_CONTROL'] = 0x10
            api_list['RX_HOP_TABLE_SIZE'] = modem_calc.demodulator.fields.fh_ch_number+1
           #'SET_PROPERTY' 'RX_HOP_TABLE_ENTRY_0' 05
            table_entry = 0
            while  table_entry <= modem_calc.demodulator.fields.fh_ch_number:
                hop_table = "RX_HOP_TABLE_ENTRY_" + str(table_entry)
                api_list[hop_table] = table_entry
                table_entry = table_entry + 1

    def _add_ook_blopk(self, modem_calc, api_list):
        api_list['MODEM_OOK_BLOPK'] = modem_calc.demodulator.fields.bw_peak
    
    def _add_rssi_group(self, modem_calc, api_list):
        # api_list['MODEM_RSSI_THRESH'] = 0 # default, not touched by calc
        api_list['MODEM_RSSI_JUMP_THRESH'] = modem_calc.demodulator.fields.rssijmpthd
        # self._api_list['MODEM_RSSI_CONTROL'] = 1  # default: latch at pmdet 
        api_list['MODEM_RSSI_CONTROL'] = (modem_calc.demodulator.fields.rssi_sel*8 + 1)  
        api_list['MODEM_RSSI_CONTROL2'] = (modem_calc.demodulator.fields.rssijmp_dwn*32 + 
                                                modem_calc.demodulator.fields.rssijmp_up*16 + 
                                                modem_calc.demodulator.fields.enrssijmp*8 + 
                                                modem_calc.demodulator.fields.jmpdlylen*4 + 
                                                modem_calc.demodulator.fields.enjmprx*2)

    def _add_modem_if_control(self, api_list, modem_calc):
        super(Pro2PlusApiList, self)._add_modem_if_control(api_list, modem_calc)
        api_list['MODEM_IF_CONTROL'] += int(modem_calc.inputs.API_ETSI % 3) # if 3, write 0 
    
    # jira 1658: add 3 fields into OOK_MISC
    def _add_modem_ook_misc(self, api_list, modem_calc):
        fast_ma = modem_calc.demodulator.fields.fast_ma
        detector = modem_calc.demodulator.fields.detector
        api_list['MODEM_OOK_MISC'] = int(fast_ma*128 + 
                                             modem_calc.demodulator.fields.ook_limit_discharge*32 + 
                                             modem_calc.demodulator.fields.ook_squelch_en*16 + 
                                             modem_calc.demodulator.fields.ook_discharge_div*4 + detector)


    def _add_modem_bcr_misc0(self, api_list, modem_calc):
        if modem_calc.revc0_c1 == False:                                            
            # only write BCR_MISC0 in revC2
            # api_list['MODEM_BCR_MISC0'] = int(adcwatch*128 + adcrst*64 + distogg*32 + ph0size*16) 
            # DSA_BCR_RST == diff0rst_en 
            api_list['MODEM_BCR_MISC0'] = int( modem_calc.demodulator.fields.res_lockup_byp*8 + modem_calc.demodulator.fields.diff0rst_en)


    def __init__(self):
        super(Pro2PlusApiList, self).__init__()


        
