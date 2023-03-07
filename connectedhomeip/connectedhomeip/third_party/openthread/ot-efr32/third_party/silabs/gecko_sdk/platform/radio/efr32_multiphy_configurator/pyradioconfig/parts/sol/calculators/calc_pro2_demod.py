from pyradioconfig.parts.ocelot.calculators.calc_pro2_demod import CALC_Pro2_Demod_Ocelot
from pro2_chip_configurator.src.si4440_modem_calc.decode_api import en4gfsk,enook

class Calc_Pro2_Demod_Sol(CALC_Pro2_Demod_Ocelot):
    pass

    def _map_pro2_outputs(self, model, pro2_calculator_obj):
        # This function maps the pro2 calculator outputs to radio configurator variables / outputs

        # Write BCRDEMODCTRL
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_RAWSYN, pro2_calculator_obj.demodulator.fields.raw_syn)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PH0SIZE, pro2_calculator_obj.demodulator.fields.ph0size)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_LOCKUPBYP, pro2_calculator_obj.demodulator.fields.res_lockup_byp)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_RAWFASTMA, pro2_calculator_obj.demodulator.fields.fast_ma)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_SPIKEREMOV, pro2_calculator_obj.demodulator.fields.spike_rm_en)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_RAWFLTSEL, pro2_calculator_obj.demodulator.fields.rawflt_sel)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PREATH, pro2_calculator_obj.demodulator.fields.preath)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_SKIPSYN, pro2_calculator_obj.demodulator.fields.skipsyn)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PMPATTERN, pro2_calculator_obj.demodulator.fields.pm_pattern)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_SLICERFAST, pro2_calculator_obj.demodulator.fields.slicer_fast)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_DETECTORSEL, pro2_calculator_obj.demodulator.fields.detector)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PHSRCSEL, pro2_calculator_obj.demodulator.fields.ph_scr_sel)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_CONSCHKBYP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PULCORRBYP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_MANCHDLY, 0)

        # Write BCRDEMODOOK
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_OOKFRZEN, pro2_calculator_obj.demodulator.fields.ookfrz_en)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_RAWGAIN, int(pro2_calculator_obj.demodulator.fields.rawgain))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_MAFREQDWN, pro2_calculator_obj.demodulator.fields.ma_freqdwn)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_SQUELCLKEN, pro2_calculator_obj.demodulator.fields.squelch_clk_en)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_BWPK, model.vars.calculated_bw_peak.value)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_DECAYSWAL, pro2_calculator_obj.demodulator.fields.decay_swal)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_DECAY, int(pro2_calculator_obj.demodulator.fields.Decay))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_ATTACK, pro2_calculator_obj.demodulator.fields.Attack)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_SQUELCH, pro2_calculator_obj.demodulator.fields.arr_squelch)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_RAWNDEC, int(pro2_calculator_obj.demodulator.fields.ndec0))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_BCRDEMODOOK, int(enook(pro2_calculator_obj.inputs.API_modulation_type)))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_PKTRUNK, 0)

        # Write BCRCTRL0
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRNCOFF, int(pro2_calculator_obj.demodulator.fields.ncoff))
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRALIGN, pro2_calculator_obj.demodulator.fields.bcr_align_en)
        self._reg_write(model.vars.MODEM_BCRCTRL0_DISTOGG, pro2_calculator_obj.demodulator.fields.distogg)
        self._reg_write(model.vars.MODEM_BCRCTRL0_CRFAST, pro2_calculator_obj.demodulator.fields.crfast)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRERRRSTEN, 1)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRFBBYP, pro2_calculator_obj.demodulator.fields.bcrfbbyp)

        # Write BCRCTRL1
        self._reg_write(model.vars.MODEM_BCRCTRL1_CRGAIN, int(pro2_calculator_obj.demodulator.fields.CR_gain))
        self._reg_write(model.vars.MODEM_BCRCTRL1_CGAINX2, pro2_calculator_obj.demodulator.fields.crgainx2)
        self._reg_write(model.vars.MODEM_BCRCTRL1_RXNCOCOMP, pro2_calculator_obj.demodulator.fields.rxncocomp)
        self._reg_write(model.vars.MODEM_BCRCTRL1_RXCOMPLAT, pro2_calculator_obj.demodulator.fields.rxcomp_lat)
        self._reg_write(model.vars.MODEM_BCRCTRL1_ESCMIDPT, pro2_calculator_obj.demodulator.fields.esc_midpt)
        self._reg_write(model.vars.MODEM_BCRCTRL1_DISMIDPT, pro2_calculator_obj.demodulator.fields.dis_midpt)
        self._reg_write(model.vars.MODEM_BCRCTRL1_BCROSR, int(pro2_calculator_obj.demodulator.fields.OSR_rx_BCR))
        self._reg_write(model.vars.MODEM_BCRCTRL1_ESTOSREN, pro2_calculator_obj.demodulator.fields.est_osr_en)
        self._reg_write(model.vars.MODEM_BCRCTRL1_BCRSWSYCW, pro2_calculator_obj.demodulator.fields.bcr_sw_sycw)
        self._reg_write(model.vars.MODEM_BCRCTRL1_PHCOMP2FSK, pro2_calculator_obj.demodulator.kphcomp_2fsk)
        self._reg_write(model.vars.MODEM_BCRCTRL1_SLICEFBBYP, pro2_calculator_obj.demodulator.fields.slicefbbyp)

        # Write BCRDEMODAFC0
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_LARGEFREQERR, int(pro2_calculator_obj.demodulator.fields.large_freq_err))
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_AFCGAINOVRFLW, pro2_calculator_obj.demodulator.fields.afcgain_ovr_flw)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_EN2TBEST, pro2_calculator_obj.demodulator.fields.en2Tb_est)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_ENAFCCLKSW, pro2_calculator_obj.demodulator.fields.enafc_clksw)

        # Write BCRDEMODAFC1
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_LGWAIT, pro2_calculator_obj.demodulator.fields.afc_lgwait)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_SHWAIT, pro2_calculator_obj.demodulator.fields.afc_shwait)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_GEARSW, pro2_calculator_obj.demodulator.fields.gear_sw)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_AFCMAEN, pro2_calculator_obj.demodulator.fields.afcma_en)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENFZPMEND, pro2_calculator_obj.demodulator.fields.enfzpmend)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_NONFRZEN, pro2_calculator_obj.demodulator.fields.non_frzen)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ONESHOTAFCEN, pro2_calculator_obj.demodulator.fields.oneshot_afc)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_SKIPPMDET, pro2_calculator_obj.demodulator.fields.skip_pm_det)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENAFCFRZ, pro2_calculator_obj.demodulator.fields.afc_freez_en)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENAFC, pro2_calculator_obj.demodulator.fields.afc_est_en)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENFBPLL, int(pro2_calculator_obj.demodulator.fields.afc_fb_pll))
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_HALFPHCOMP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_PMRSTEN, 0)

        # Write BCRDEMOD4FSK0
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_THD4GFSK, int(pro2_calculator_obj.demodulator.fields.thd4gfsk))
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_CODE4GFSK, pro2_calculator_obj.demodulator.fields.code4gfsk)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_PHCOMPBYP, pro2_calculator_obj.demodulator.fields.phcompbyp)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_EN4GFSK, int(en4gfsk(pro2_calculator_obj.inputs.API_modulation_type)))

        # Write BCRDEMOD4FSK1
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK0, pro2_calculator_obj.demodulator.fields.phcomp_gain_4gfsk0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK1, pro2_calculator_obj.demodulator.fields.phcomp_gain_4gfsk1 & 0x7f)  # Remove bit 7 which was used in Pro2 for a different purpose
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_S2PMAP, pro2_calculator_obj.demodulator.fields.s2p_map)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_FDEVCOMPEN, pro2_calculator_obj.demodulator.fields.thd4gfsk_comp_en)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_FDEVCOMPRATIO, pro2_calculator_obj.demodulator.fields.thd4gfsk_comp_ratio)

        # Write BCRDEMODANT
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SKIP2PHTHD, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_ANWAIT, pro2_calculator_obj.demodulator.fields.anwait)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_AGCGAINUPB, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SKIP2PH, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_BYP1P5, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_ANT2PMTHD, pro2_calculator_obj.demodulator.fields.ant2pm_thd)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SWANTTIMER, 1)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_BCRDEMODANTDIV, pro2_calculator_obj.demodulator.fields.antdiv)

        # Write BCRDEMODRSSI
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_RSSIARRTHD, pro2_calculator_obj.demodulator.fields.rssi_arr_thd)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_MUTERSSICNT, pro2_calculator_obj.demodulator.fields.mute_rssi_cnt)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_RSSIMATAP, pro2_calculator_obj.demodulator.fields.matap)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_PRWOFFSET, 0)

        # Write BCRDEMODARR0
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_SCHPRDLO, pro2_calculator_obj.demodulator.fields.schprd_low)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_SCHPRDHI, pro2_calculator_obj.demodulator.fields.schprd_h)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRRSTEN, pro2_calculator_obj.demodulator.fields.arr_rst_en)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRTOLER, pro2_calculator_obj.demodulator.fields.arr_toler)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_DIFF0RSTEN, pro2_calculator_obj.demodulator.fields.diff0rst_en)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRDETTHD, pro2_calculator_obj.demodulator.fields.arrival_thd)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_SCHFRZEN, pro2_calculator_obj.demodulator.fields.sch_frzen)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_EYEXESTEN, pro2_calculator_obj.demodulator.fields.eyexest_en)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_EYEXESTFAST, pro2_calculator_obj.demodulator.fields.eyexest_fast)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRDETSRC, pro2_calculator_obj.demodulator.fields.arriving_src)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_EYEQUALEN, 0)

        # Write BCRDEMODARR1
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_ARREYEQUAL, int(pro2_calculator_obj.demodulator.fields.arr_eye_qual))
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_BCRCFESRC, 1)
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_KSICOMPEN, 0)

        # Write BCRDEMODARR2
        self._reg_write(model.vars.MODEM_BCRDEMODARR2_RAWDCHKALWAYON, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR2_CONSYMOL, 0)

        # Write BCRDEMODKSI
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI1, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI2, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI3, model.vars.calculated_ksi3.value)

        # Write BCRDEMODPMEXP
        self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPHSCALE, model.vars.calculated_phscale.value)
        self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPMEXP, 21845)

        # GET afc gain result
        digmix_res = model.vars.digmix_res_actual.value
        model.vars.pro2_afc_gain.value = int(digmix_res * pro2_calculator_obj.demodulator.fields.afc_gain)

    def write_unused_bcr_regs(self, model):
        # Write BCRDEMODCTRL
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_RAWSYN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PH0SIZE, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_LOCKUPBYP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_RAWFASTMA, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_SPIKEREMOV, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_RAWFLTSEL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PREATH, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_SKIPSYN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PMPATTERN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_SLICERFAST, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_DETECTORSEL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PHSRCSEL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_CONSCHKBYP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_PULCORRBYP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_MANCHDLY, 0)

        # Write BCRDEMODOOK
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_OOKFRZEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_RAWGAIN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_MAFREQDWN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_SQUELCLKEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_BWPK, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_DECAYSWAL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_DECAY, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_ATTACK, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_SQUELCH, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_RAWNDEC, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_BCRDEMODOOK, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_PKTRUNK, 0)

        # Write BCRCTRL0
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRNCOFF, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRALIGN, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL0_DISTOGG, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL0_CRFAST, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRERRRSTEN, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRFBBYP, 0)

        # Write BCRCTRL1
        self._reg_write(model.vars.MODEM_BCRCTRL1_CRGAIN, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_CGAINX2, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_RXNCOCOMP, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_RXCOMPLAT, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_ESCMIDPT, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_DISMIDPT, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_BCROSR, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_ESTOSREN, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_BCRSWSYCW, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_PHCOMP2FSK, 0)
        self._reg_write(model.vars.MODEM_BCRCTRL1_SLICEFBBYP, 0)

        # Write BCRDEMODAFC0
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_LARGEFREQERR, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_AFCGAINOVRFLW, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_EN2TBEST, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_ENAFCCLKSW, 0)

        # Write BCRDEMODAFC1
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_LGWAIT, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_SHWAIT, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_GEARSW, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_AFCMAEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENFZPMEND, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_NONFRZEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ONESHOTAFCEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_SKIPPMDET, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENAFCFRZ, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENAFC, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENFBPLL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_HALFPHCOMP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_PMRSTEN, 0)

        # Write BCRDEMOD4FSK0
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_THD4GFSK, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_CODE4GFSK, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_PHCOMPBYP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_EN4GFSK, 0)

        # Write BCRDEMOD4FSK1
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK0, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK1, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_S2PMAP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_FDEVCOMPEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_FDEVCOMPRATIO, 0)

        # Write BCRDEMODANT
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SKIP2PHTHD, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_ANWAIT, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_AGCGAINUPB, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SKIP2PH, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_BYP1P5, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_ANT2PMTHD, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SWANTTIMER, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_BCRDEMODANTDIV, 0)

        # Write BCRDEMODRSSI
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_RSSIARRTHD, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_MUTERSSICNT, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_PRWOFFSET, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_RSSIMATAP, 0)

        # Write BCRDEMODARR0
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_SCHPRDLO, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_SCHPRDHI, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRRSTEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRTOLER, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_DIFF0RSTEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRDETTHD, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_SCHFRZEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_EYEXESTEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_EYEXESTFAST, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRDETSRC, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_EYEQUALEN, 0)

        # Write BCRDEMODARR1
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_ARREYEQUAL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_SYMBWMAX, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_SYMBWMIN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_BCRCFESRC, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_KSICOMPEN, 0)

        # Write BCRDEMODARR2
        self._reg_write(model.vars.MODEM_BCRDEMODARR2_RAWDCHKALWAYON, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR2_CONSYMOL, 0)

        # Write BCRDEMODKSI
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI1, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI2, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI3, 0)

        # Write BCRDEMODPMEXP
        self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPHSCALE, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPMEXP, 0)

    def calc_oneshot_wait_cnt(self, model):
        #This function calculates the value of MODEM_BCRDEMODAFC1_ONESHOTWAITCNT
        # Reading variables from model variables
        preamble_length = model.vars.preamble_detection_length.value
        est_osr_en = model.vars.MODEM_BCRCTRL1_ESTOSREN.value
        bcr_demod_en = model.vars.bcr_demod_en.value
        pro2_ant_div = model.vars.pro2_ant_div.value
        pro2_pm_pattern = model.vars.pro2_pm_pattern.value
        #Modify MODEM_BCRDEMODAFC1_ONESHOTWAITCNT ony if the BCR demod is enabled
        if (bcr_demod_en == 1):

            if (pro2_ant_div == 1 or pro2_pm_pattern == 1000 or est_osr_en == 1):
                oneshot_waitcnt = 1
            elif (preamble_length < 32):
                # Change value of MODEM_BCRDEMODAFC1_ONESHOTWAITCNT for OOK with short preamble
                oneshot_waitcnt = 5
            else:
                oneshot_waitcnt = 8
            #Write the register

        else:
            oneshot_waitcnt = 0

        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ONESHOTWAITCNT, oneshot_waitcnt)

    def calc_crslow(self, model):
        #This function calculates the value of MODEM_BCRCTRL0_CRSLOW
        # Reading variables from model variables

        pm_pattern = model.vars.MODEM_BCRDEMODCTRL_PMPATTERN.value
        bcr_demod_en = model.vars.bcr_demod_en.value
        pro2_pm_pattern = model.vars.pro2_pm_pattern.value
        baudrate_tol_req_ppm = model.vars.baudrate_tol_ppm.value
        mod_format = model.vars.modulation_type.value
        FSK4 = model.vars.modulation_type.var_enum.FSK4

        API_Max_Rb_Error = abs (baudrate_tol_req_ppm / (1e6))

        API_Max_Rb_Error_low_limit = 0.01  # Taken from _get_API_Max_Rb_Error_low_limit in
        # libraries / host_py_radio_config / Package / pro2_chip_configurator / src / si4440_modem_calc / pro2_ocelotmodemcalc.py

        API_Max_Rb_Error_inner_limit = 0.02

        API_Max_Rb_Error_high_limit = 0.2 # Taken from _get_API_Max_Rb_Error_high_limit in
        # libraries / host_py_radio_config / Package / pro2_chip_configurator / src / si4440_modem_calc / pro2plusmodemcalc.py

        if (bcr_demod_en == 1):

            # Conditions below come from original calculation in  _validate() from
            # libraries / host_py_radio_config / Package / pro2_chip_configurator / src / si4440_modem_calc / pro2modemcalc.py

            #Based on the Pro2 algorithm, we are setting API_Rb_error in 3 levels
            # Level 0 fpr baudrate tolerance of ;ess tjam 1%
            # Level 1 fpr baudrate tolerance between `% and 2%
            # Level 2 fpr baudrate tolerance of 2% or more

            if (API_Max_Rb_Error <= API_Max_Rb_Error_low_limit):
                API_Rb_error = 0 #Level 0 fpr baudrate tolerance of less tjam 1%
            elif (API_Max_Rb_Error > API_Max_Rb_Error_low_limit and API_Max_Rb_Error <= API_Max_Rb_Error_inner_limit):
                API_Rb_error = 1   # Level 1 fpr baudrate tolerance between 1% and 2%
            elif (API_Max_Rb_Error > API_Max_Rb_Error_inner_limit and API_Max_Rb_Error <= API_Max_Rb_Error_high_limit):
                API_Rb_error = 2       # level 2 fpr baudrate tolerance of 2% or more
            else:
                API_Rb_error = 2       # level 2 fpr baudrate tolerance of 2% or more

            #  Different conditions below come from original calculation in _calc_step_6 defined in
            # libraries/host_py_radio_config/Package/pro2_chip_configurator/src/si4440_modem_calc/pro2_ocelotdemodulatorcore.py

            if (pm_pattern == 0):  # 1010 PM pattern: Min pm length = 32 bit
                if  (API_Rb_error >= 2):
                    crslow = 0
                elif (API_Rb_error >= 1 and API_Rb_error< 2):
                    crslow = 1
                elif (pro2_pm_pattern == 100 and mod_format == FSK4): # MBUS N2g mode
                    crslow = 0
                else:
                    crslow = 2
            else:  # non-standard pattern
                if (pro2_pm_pattern == 1000):
                    crslow = 2
                else:
                    crslow = 0
        else: #this option covers cases the register is unused (TX or Ctune with no BCR demod selection)
            crslow = 0

        self._reg_write(model.vars.MODEM_BCRCTRL0_CRSLOW, crslow)

    def calc_bcr_cfe_costthd(self, model):
        #This function calculates the value of MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD for Sol and newer EFR ICs
        #The main change in this function, compared to the old one, is  an extra condition to set BCRCFECOSTTHD TO 150
        # for cases when the FSK2/4 modulation index is 0.5

        # Reading variables from model variables
        antdivmode = model.vars.antdivmode.value
        mod_format = model.vars.modulation_type.value
        modulation_index = model.vars.modulation_index.value

        if model.vars.bcr_demod_en.value == 1:
            # Need to read in these variables inside of the if statement as some are don't care if BCR disabled
            calculated_bcrpmacqwin = model.vars.MODEM_BCRDEMODPMEXP_BCRPMACQWIN.value

            if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV or antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
                bcr_cfe_costthd = 150  # If antdiv enabled, bcrpmacqwin is always 2 so the cost thd is constant.
            elif ((mod_format == model.vars.modulation_type.var_enum.FSK2 or mod_format == model.vars.modulation_type.var_enum.FSK4) and \
                    modulation_index == 0.5):
                bcr_cfe_costthd = 150  # In Sol, this condition only applies to PHY_Landis_Gyr_915M_19p2kbps_mi0p5
            else:
                bcr_cfe_costthd = int(round(215 - (3 - calculated_bcrpmacqwin) * 60))
        else:
            bcr_cfe_costthd = 255

        self._reg_sat_write(model.vars.MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD, bcr_cfe_costthd)  # We always care about this