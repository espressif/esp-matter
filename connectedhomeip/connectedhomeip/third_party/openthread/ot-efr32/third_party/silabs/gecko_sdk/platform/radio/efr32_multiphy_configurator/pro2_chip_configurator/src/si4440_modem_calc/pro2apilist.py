'''
Created on Apr 4, 2013

@author: sesuskic
'''
import math
from collections import OrderedDict
from .decode_api import enook
from .rxchflt_rtl import RxChFlt_rtl

__all__ = ["Pro2ApiList"]
class Pro2ApiList(object):

    def _add_modulator_api_list(self, modem_calc, api_list):
        api_list['MODEM_MOD_TYPE'] = modem_calc.modulator.fields.modtype
        api_list['MODEM_MAP_CONTROL'] = modem_calc.modulator.fields.mapctrl
        api_list['MODEM_DSM_CTRL'] = modem_calc.modulator.fields.DSM_CTRL
        api_list['MODEM_CLKGEN_BAND'] = modem_calc.modulator.fields.sy_sel_Band
        api_list['SYNTH_PFDCP_CPFF'] = (modem_calc.modulator.fields.icpff_ctl ^ 32) & 63
        api_list['SYNTH_PFDCP_CPINT'] = (modem_calc.modulator.fields.icpint_ctl ^ 8) & 15
        api_list['SYNTH_VCO_KV'] = modem_calc.modulator.fields.kvff_ctl*4 + modem_calc.modulator.fields.kvint_ctl
        api_list['SYNTH_LPFILT3'] = modem_calc.modulator.fields.rff_ctl-1
        api_list['SYNTH_LPFILT2'] = modem_calc.modulator.fields.cff_ctl
        api_list['SYNTH_LPFILT1'] = modem_calc.modulator.fields.cff13_ctl
        api_list['SYNTH_LPFILT0'] = modem_calc.modulator.fields.lpf_ff_bias
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_DATA_RATE'] = (int(modem_calc.modulator.fields.txdr)) & 0xffffff
        else:
            api_list['MODEM_DATA_RATE_2'] = (int(modem_calc.modulator.fields.txdr) >> 16) & 0xff
            api_list['MODEM_DATA_RATE_1'] = (int(modem_calc.modulator.fields.txdr) >> 8) & 0xff
            api_list['MODEM_DATA_RATE_0'] = int(modem_calc.modulator.fields.txdr) & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_TX_NCO_MODE'] = (modem_calc.modulator.fields.hbncomod) & 0xffffffff
        else:
            api_list['MODEM_TX_NCO_MODE_3'] = (modem_calc.modulator.fields.hbncomod >> 24) & 0xff
            api_list['MODEM_TX_NCO_MODE_2'] = (modem_calc.modulator.fields.txncomod_reg >> 16) & 0Xff
            api_list['MODEM_TX_NCO_MODE_1'] = (modem_calc.modulator.fields.txncomod_reg >> 8) & 0xff
            api_list['MODEM_TX_NCO_MODE_0'] = modem_calc.modulator.fields.txncomod_reg & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_FREQ_DEV'] = (modem_calc.modulator.fields.freq_dev) & 0x1ffff
        else:
            api_list['MODEM_FREQ_DEV_2'] = (modem_calc.modulator.fields.txfd >> 16) & 0xff
            api_list['MODEM_FREQ_DEV_1'] = (modem_calc.modulator.fields.freq_dev >> 8) & 0xff
            api_list['MODEM_FREQ_DEV_0'] = modem_calc.modulator.fields.freq_dev & 0xff
        api_list['MODEM_TX_RAMP_DELAY'] = modem_calc.modulator.fields.ramp_dly
        api_list['PA_TC'] = modem_calc.modulator.fields.PA_TC
        self._add_seq_cfg(modem_calc, api_list)
        api_list['FREQ_CONTROL_INTE'] = modem_calc.modulator.fields.fc_inte
        if api_list == self._api_dict['object_api']:
            api_list['FREQ_CONTROL_FRAC'] = (modem_calc.modulator.fields.fc_frac) & 0xfffff
        else:
            api_list['FREQ_CONTROL_FRAC_2'] = (modem_calc.modulator.fields.fc_frac >> 16) & 0xff
            api_list['FREQ_CONTROL_FRAC_1'] = (modem_calc.modulator.fields.fc_frac >> 8) & 0xff
            api_list['FREQ_CONTROL_FRAC_0'] = modem_calc.modulator.fields.fc_frac  & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['FREQ_CONTROL_CHANNEL_STEP_SIZE'] = int(math.floor(modem_calc.modulator.fields.fhst_16b)) &0xffff
        else:
            api_list['FREQ_CONTROL_CHANNEL_STEP_SIZE_1'] = int(math.floor(modem_calc.modulator.fields.fhst_16b/2**8))
            api_list['FREQ_CONTROL_CHANNEL_STEP_SIZE_0'] = int(modem_calc.modulator.fields.fhst_16b % 2**8)
        api_list['FREQ_CONTROL_W_SIZE'] = modem_calc.modulator.fields.w_size




    def _add_seq_cfg(self, modem_calc, api_list):
        pass
    def _add_chflt_rx_apis(self, modem_calc, api_list):
        #SSR register
        rtl_chflt_list = [RxChFlt_rtl(modem_calc.demodulator.fields.rx1coearray),
                          RxChFlt_rtl(modem_calc.demodulator.fields.rx2coearray)]
        flt = 1
        for rtl_chflt in rtl_chflt_list:
            idx = 0
            coe_m_list = [rtl_chflt.coe_m0, rtl_chflt.coe_m1, rtl_chflt.coe_m2, rtl_chflt.coe_m3]
            if api_list == self._api_dict['object_api']:
                cflt_val = 0
                for coe_m in coe_m_list[::-1]:
                    cflt_val += coe_m << idx*8
                    idx += 1
                for chfltcoe in rtl_chflt.coe_7_0[::-1]:
                    cflt_val += chfltcoe << idx*8
                    idx += 1
                api_list['MODEM_CHFLT_RX{:d}_CHFLT_COE'.format(flt)] = cflt_val
            else:
                coeidx = len(rtl_chflt.coe_7_0) - 1
                for chfltcoe in rtl_chflt.coe_7_0:
                    api_list['MODEM_CHFLT_RX{:d}_CHFLT_COE{:d}_7_0'.format(flt, coeidx)] = chfltcoe
                    coeidx -= 1
                for coe_m in coe_m_list:
                    api_list['MODEM_CHFLT_RX{:d}_CHFLT_COEM{:d}'.format(flt, idx)] = coe_m
                    idx += 1
            flt += 1

    def _add_misc_apis(self, modem_calc, api_list):
        zero_if = modem_calc.demodulator.fields.zero_if
        adc_hgain = modem_calc.demodulator.fields.adc_hgain
        adc_en_drst = modem_calc.demodulator.fields.adc_en_drst
        realadc = modem_calc.demodulator.fields.realadc
        dc_follow = modem_calc.demodulator.fields.dc_follow
        dc_rst = modem_calc.demodulator.fields.dc_rst
        dc_freez = modem_calc.demodulator.fields.dc_freez
        dc_gear = modem_calc.demodulator.fields.dc_gear
        afczifoff = modem_calc.demodulator.fields.afczifoff
        
        if (zero_if == 1):
            api_list['MODEM_ADC_CTRL'] = adc_hgain * 16 + adc_en_drst * 8 + realadc * 2
            # DC filter
            api_list['MODEM_DC_CONTROL'] = dc_follow * 2 ** 5 + dc_rst * 2 ** 4 + dc_freez * 2 ** 3 + dc_gear
        if (zero_if == 1 and not (enook(modem_calc.inputs.API_modulation_type))):
            api_list['MODEM_AFC_ZIFOFF'] = afczifoff
        if (enook(modem_calc.inputs.API_modulation_type) * zero_if):
            if api_list == self._api_dict['object_api']:
                api_list['PKT_WHT_POLY'] = 0x8000
            else:
                api_list['PKT_WHT_POLY_15_8'] = 128
                api_list['PKT_WHT_POLY_7_0'] = 0
            if api_list == self._api_dict['object_api']:
                api_list['PKT_WHT_SEED'] = 0xaaaa
            else:
                api_list['PKT_WHT_SEED_15_8'] = 170
                api_list['PKT_WHT_SEED_7_0'] = 170
            api_list['PKT_FIELD_1_CONFIG'] = 4
            if api_list == self._api_dict['legacy']:
                # config modulator
                api_list['MODEM_OOKZEROIF_RB_2'] = (modem_calc.modulator.fields.ook_zero_IF_txdr >> 16) & 0xff
                api_list['MODEM_OOKZEROIF_RB_1'] = (modem_calc.modulator.fields.ook_zero_IF_txdr >> 8) & 0xff
                api_list['MODEM_OOKZEROIF_RB_0'] = modem_calc.modulator.outputso.ok_zero_IF_txdr & 0xff
                api_list['MODEM_OOKZEROIF_FDEV_1'] = (modem_calc.modulator.fields.ook_zero_IF_txfd >> 8) & 0xff
                api_list['MODEM_OOKZEROIF_FDEV_0'] = modem_calc.modulator.fields.ook_zero_IF_txfd & 0xff
    
    def _add_modem_raw_search_api(self, modem_calc, api_list):
        sch_frzen = modem_calc.demodulator.fields.sch_frzen
        sch_frz_th = modem_calc.demodulator.fields.sch_frz_th
        schprd_h = modem_calc.demodulator.fields.schprd_h
        schprd_low = modem_calc.demodulator.fields.schprd_low
        api_list['MODEM_RAW_SEARCH'] = sch_frzen * 2 ** 7 + sch_frz_th * 2 ** 4 + schprd_h * 2 ** 2 + schprd_low

    def _add_modem_raw_eye_api(self, modem_calc, api_list):
        raweye = modem_calc.demodulator.fields.raweye
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_RAW_EYE'] = int(raweye) & 0x7ff
        else:
            api_list['MODEM_RAW_EYE_1'] = (int(raweye) >> 8) & 0x07
            api_list['MODEM_RAW_EYE_0'] = int(raweye) & 0xff

    def _add_ook_blopk(self, modem_calc, api_list):
        pass
    
    def _add_rssi_group(self, modem_calc, api_list):
        pass

    # jira 1658: to add 3 fields into OOK_MISC in pro2+
    def _add_modem_ook_misc(self, api_list, modem_calc):
        fast_ma = modem_calc.demodulator.fields.fast_ma
        detector = modem_calc.demodulator.fields.detector
        api_list['MODEM_OOK_MISC'] = int(fast_ma * 128 + detector) 

    def _add_modem_bcr_misc0(self, api_list, modem_calc):
        pass
        # BCR_MISC0 defaults to 0, never touched by calculator before 9/18/2014
        # revC2 adds bits [3] and [0] 
        # jira si4440-1362 -> 1539 -> set MISC0[3]=res_lockup_byp in BERT mode
        # jira si4440-1120 -> 1435 -> clr MISC0[0]=diff0rst_en in recv0/1 or BERT mode;  only set in revC2+ DSA enabled (maybe never set)
        # api_list['MODEM_BCR_MISC0'] = int(adcwatch*128 + adcrst*64 + distogg*32 + ph0size*16) 
    
    def _add_modem_if_control(self, api_list, modem_calc):
        zero_if = modem_calc.demodulator.fields.zero_if
        fixed_if_mode = modem_calc.demodulator.fields.fixed_if_mode
        api_list['MODEM_IF_CONTROL'] = int(zero_if * 16 + fixed_if_mode * 8)
    
    def _add_main_apis(self, modem_calc, api_list):
        vco_rx_adj = modem_calc.demodulator.fields.vco_rx_adj
        ph_scr_sel = modem_calc.demodulator.fields.ph_scr_sel
        if_diff = modem_calc.demodulator.fields.if_diff
        ndec0 = modem_calc.demodulator.fields.ndec0
        ndec1 = modem_calc.demodulator.fields.ndec1
        ndec2 = modem_calc.demodulator.fields.ndec2
        chflt_lopw = modem_calc.demodulator.fields.chflt_lopw
        droopfltbyp = modem_calc.demodulator.fields.droopfltbyp
        dwn3byp = modem_calc.demodulator.fields.dwn3byp
        dwn2byp = modem_calc.demodulator.fields.dwn2byp
        rxgainx2 = modem_calc.demodulator.fields.rxgainx2
        OSR_rx_BCR = modem_calc.demodulator.fields.OSR_rx_BCR
        ncoff = modem_calc.demodulator.fields.ncoff
        CR_gain = modem_calc.demodulator.fields.CR_gain
        crfast = modem_calc.demodulator.fields.crfast
        crslow = modem_calc.demodulator.fields.crslow
        bcrfbbyp = modem_calc.demodulator.fields.bcrfbbyp
        slicefbbyp = modem_calc.demodulator.fields.slicefbbyp
        rxncocomp = modem_calc.demodulator.fields.rxncocomp
        rxcomp_lat = modem_calc.demodulator.fields.rxcomp_lat
        crgainx2 = modem_calc.demodulator.fields.crgainx2
        dis_midpt = modem_calc.demodulator.fields.dis_midpt
        esc_midpt = modem_calc.demodulator.fields.esc_midpt
        gear_sw = modem_calc.demodulator.fields.gear_sw
        afc_gear_hi = modem_calc.demodulator.fields.afc_gear_hi
        afc_gear_lo = modem_calc.demodulator.fields.afc_gear_lo
        afc_shwait = modem_calc.demodulator.fields.afc_shwait
        afc_lgwait = modem_calc.demodulator.fields.afc_lgwait
        afc_est_en = modem_calc.demodulator.fields.afc_est_en
        afcbd = modem_calc.demodulator.fields.afcbd
        afc_gain = modem_calc.demodulator.fields.afc_gain
        afclim = modem_calc.demodulator.fields.afclim
        afc_freez_en = modem_calc.demodulator.fields.afc_freez_en
        afc_fb_pll = modem_calc.demodulator.fields.afc_fb_pll
        en2Tb_est = modem_calc.demodulator.fields.en2Tb_est
        enfzpmend = modem_calc.demodulator.fields.enfzpmend
        enafc_clksw = modem_calc.demodulator.fields.enafc_clksw
        ook_zerog = modem_calc.demodulator.fields.ook_zerog
        agcovpkt = modem_calc.demodulator.fields.agcovpkt
        ifpdslow = modem_calc.demodulator.fields.ifpdslow
        rfpdslow = modem_calc.demodulator.fields.rfpdslow
        sgi_n = modem_calc.demodulator.fields.sgi_n
        rst_pkdt_period = modem_calc.demodulator.fields.rst_pkdt_period
        agc_slow = modem_calc.demodulator.fields.agc_slow
        adc_gain_cor_en = modem_calc.demodulator.fields.adc_gain_cor_en
        measwin = modem_calc.demodulator.fields.measwin
        settlewin = modem_calc.demodulator.fields.settlewin
        rfpd_decay = modem_calc.demodulator.fields.rfpd_decay
        ifpd_decay = modem_calc.demodulator.fields.ifpd_decay
        phcompbyp = modem_calc.demodulator.fields.phcompbyp
        phcomp_gain_4gfsk1 = modem_calc.demodulator.fields.phcomp_gain_4gfsk1
        phcomp_gain_4gfsk0 = modem_calc.demodulator.fields.phcomp_gain_4gfsk0
        thd4gfsk = modem_calc.demodulator.fields.thd4gfsk
        code4gfsk = modem_calc.demodulator.fields.code4gfsk
        Attack = modem_calc.demodulator.fields.Attack
        Decay = modem_calc.demodulator.fields.Decay
        s2p_map = modem_calc.demodulator.fields.s2p_map
        ookfrz_en = modem_calc.demodulator.fields.ookfrz_en
        ma_freqdwn = modem_calc.demodulator.fields.ma_freqdwn
        raw_syn = modem_calc.demodulator.fields.raw_syn
        slicer_fast = modem_calc.demodulator.fields.slicer_fast
        Squelch = modem_calc.demodulator.fields.Squelch
        nonstdpk_final = modem_calc.demodulator.fields.nonstdpk_final
        pm_pattern = modem_calc.demodulator.fields.pm_pattern
        rawgain = modem_calc.demodulator.fields.rawgain
        ant_mode = modem_calc.demodulator.fields.ant_mode
        ant2pm_thd = modem_calc.demodulator.fields.ant2pm_thd
        matap = modem_calc.demodulator.fields.matap
        antdiv = modem_calc.demodulator.fields.antdiv
        RSSI_comp = modem_calc.demodulator.fields.RSSI_comp

        api_list['FREQ_CONTROL_VCOCNT_RX_ADJ'] = int(vco_rx_adj) & 0xff
        api_list['MODEM_MDM_CTRL'] = int(ph_scr_sel * 2 ** 7)
        self._add_modem_if_control(api_list, modem_calc)
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_IF_FREQ'] = int(if_diff) & 0x3ffff
        else:
            api_list['MODEM_IF_FREQ_2'] = ((int(if_diff) & 0x3ffff) >> 16) & 0xf
            api_list['MODEM_IF_FREQ_1'] = ((int(if_diff) & 0x3ffff) >> 8) & 0xff
            api_list['MODEM_IF_FREQ_0'] = ((int(if_diff) & 0x3ffff)) & 0xff
        api_list['MODEM_DECIMATION_CFG1'] = int(64 * ndec2 + 16 * ndec1 + 2 * ndec0)
        api_list['MODEM_DECIMATION_CFG0'] = int((chflt_lopw * 128 + droopfltbyp * 64 + dwn3byp * 32 + dwn2byp * 16 + rxgainx2))
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_BCR_OSR'] = int(OSR_rx_BCR) & 0xfff
        else:
            api_list['MODEM_BCR_OSR_1'] = (int(OSR_rx_BCR) >> 8) & 0x0f
            api_list['MODEM_BCR_OSR_0'] = int(OSR_rx_BCR) & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_BCR_NCO_OFFSET'] = int(ncoff) & 0x3fffff
        else:
            api_list['MODEM_BCR_NCO_OFFSET_2'] = (int(ncoff) >> 16) & 0x3f
            api_list['MODEM_BCR_NCO_OFFSET_1'] = (int(ncoff) >> 8) & 0xff
            api_list['MODEM_BCR_NCO_OFFSET_0'] = int(ncoff) & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_BCR_GAIN'] = int(CR_gain) & 0x7ff
        else:
            api_list['MODEM_BCR_GAIN_1'] = (int(CR_gain) >> 8) & 0xff
            api_list['MODEM_BCR_GAIN_0'] = int(CR_gain) & 0xff
        api_list['MODEM_BCR_GEAR'] = int(crfast * 8 + crslow)
        self._add_modem_bcr_misc0(api_list, modem_calc)
        api_list['MODEM_BCR_MISC1'] = int(bcrfbbyp * 128 + slicefbbyp * 64 + rxncocomp * 16 + rxcomp_lat * 8 + crgainx2 * 4 + dis_midpt * 2 + esc_midpt)
        api_list['MODEM_AFC_GEAR'] = int(gear_sw * 64 + afc_gear_hi * 2 ** 3 + afc_gear_lo)
        api_list['MODEM_AFC_WAIT'] = int(afc_shwait * 16 + afc_lgwait)
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_AFC_GAIN'] = (int(afc_est_en) << 15) + (int(afcbd) << 14) + (int(afc_gain) & 0x1fff)
        else:
            api_list['MODEM_AFC_GAIN_1'] = int(afc_est_en) * 0x80 + int(afcbd) * 0x40 + ((int(afc_gain) >> 8) & 0x1f)
            api_list['MODEM_AFC_GAIN_0'] = int(afc_gain) & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_AFC_LIMITER'] = int(afclim) & 0x7fff
        else:
            api_list['MODEM_AFC_LIMITER_1'] = (int(afclim) >> 8) & 0xff
            api_list['MODEM_AFC_LIMITER_0'] = int(afclim) & 0xff
        api_list['MODEM_AFC_MISC'] = int(afc_freez_en * 128 + afc_fb_pll * 64 + en2Tb_est * 32 + enfzpmend * 16 + enafc_clksw * 8 + ook_zerog * 4)
        api_list['MODEM_AGC_CONTROL'] = int(agcovpkt * 2 ** 7 + ifpdslow * 2 ** 6 + rfpdslow * 2 ** 5 + sgi_n * 2 ** 4 + agc_slow * 2 ** 3 + adc_gain_cor_en * 2 + rst_pkdt_period)
        api_list['MODEM_AGC_WINDOW_SIZE'] = int(measwin * 16 + settlewin)
        api_list['MODEM_AGC_RFPD_DECAY'] = int(rfpd_decay)
        api_list['MODEM_AGC_IFPD_DECAY'] = int(ifpd_decay)
        api_list['MODEM_FSK4_GAIN1'] = (phcompbyp * 0x80) + (int(phcomp_gain_4gfsk1) & 0x7f)
        api_list['MODEM_FSK4_GAIN0'] = int(phcomp_gain_4gfsk0) & 0xff
        if api_list == self._api_dict['object_api']:
            api_list['MODEM_FSK4_TH'] = int(thd4gfsk) & 0xffff
        else:
            api_list['MODEM_FSK4_TH1'] = (int(thd4gfsk) >> 8) & 0xff
            api_list['MODEM_FSK4_TH0'] = int(thd4gfsk) & 0xff
        api_list['MODEM_FSK4_MAP'] = 0*code4gfsk # put default 0
        api_list['MODEM_OOK_PDTC'] = int(Attack * 16 + Decay)
        self._add_ook_blopk(modem_calc, api_list)
        api_list['MODEM_OOK_CNT1'] = s2p_map * 2 ** 6 + ookfrz_en * 2 ** 5 + ma_freqdwn * 2 ** 4 + raw_syn * 2 ** 3 + slicer_fast * 2 ** 2 + Squelch # change API doc

        #api_list['MODEM_OOK_MISC'] = int(fast_ma * 128 + detector) # change API doc, remove frzen
        self._add_modem_ook_misc(api_list, modem_calc)

        self._add_modem_raw_search_api(modem_calc, api_list)
        api_list['MODEM_RAW_CONTROL'] = int(nonstdpk_final * 128 + pm_pattern * 4 + rawgain)
        self._add_modem_raw_eye_api(modem_calc, api_list)
        api_list['MODEM_ANT_DIV_MODE'] = ant_mode #default ANT_mode
        api_list['MODEM_ANT_DIV_CONTROL'] = ant2pm_thd * 2 ** 4 + matap * 2 ** 3 + antdiv # check API doc
        # add RSSI API group (on PRO2 already but used default values) <-> only put in pro2+ as we don't want to touch pro2 calc
        self._add_rssi_group(modem_calc, api_list)
        api_list['MODEM_RSSI_COMP'] = RSSI_comp

    def _add_demodulator_api_list(self, modem_calc, api_list):
        self._add_main_apis(modem_calc, api_list) 
        self._add_chflt_rx_apis(modem_calc, api_list) 
        self._add_misc_apis(modem_calc, api_list)      
        
    def __init__(self):
        self._api_list = OrderedDict({})
        self._object_api_list = OrderedDict({})
        self._api_dict = {'legacy': self._api_list, 'object_api':self._object_api_list}

    def _get_radio_api_property_list(self, radio_api):
        modem_property_list = []
        for key in self._object_api_list.keys():
            prop = radio_api.properties.get_property(key)
            # The radio_api object in here is not checked agains any features,
            # so need to prevent an exception when the radio_api instance does not have
            # properties that are configured in the modem calculator.
            if prop is not None:
                prop.set_value(self._object_api_list[key])
                modem_property_list.append(prop)
        return modem_property_list
    
    def populate(self, modem_calc):
        for api_list in self._api_dict.values():
            self._add_modulator_api_list(modem_calc, api_list)
            self._add_demodulator_api_list(modem_calc, api_list)
        
    def get_api_list(self, radio_api=None):
        if radio_api == None:
            return self._api_list
        else:
            return self._get_radio_api_property_list(radio_api)

    def get_raw_bytes(self, radio_api):
        raw_bytes_list = []
        for key in self._object_api_list.keys():
            radio_api.properties.all_properties_map[key].set_value(self._object_api_list[key])
            raw_bytes_list.append(radio_api.properties.all_properties_map[key].configure().get_arg_stream_raw_bytes())
        return raw_bytes_list
        
