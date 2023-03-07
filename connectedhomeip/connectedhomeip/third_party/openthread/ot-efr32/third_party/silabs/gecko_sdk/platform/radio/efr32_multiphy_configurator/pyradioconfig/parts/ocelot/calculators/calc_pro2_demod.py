from pro2_chip_configurator.src.si4440_modem_calc.pro2_ocelotmodemcalc import Pro2OcelotModemCalc
from pro2_chip_configurator.src.si4440_modem_calc.pro2_ocelotmodemcalcinputs import Pro2OcelotModemCalcInputs
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from collections import OrderedDict
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pro2_chip_configurator.src.si4440_modem_calc.decode_api import en4gfsk,enook
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

class CALC_Pro2_Demod_Ocelot(ICalculator):

    def buildVariables(self,model):
        self._addModelVariable(model, 'pro2_osr_tune', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_pm_pattern', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_modulation_type', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_afc_mode', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_ant_div', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_dsa_mode', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_fs_rx_ch', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'pro2_afc_gain', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'calculated_ksi3', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'calculated_phscale', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'calculated_bw_peak', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'bcr_conc_baudrate', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'bcr_conc_manchester', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'bcr_conc_deviation', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'freq_dev_max', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'freq_dev_min', int, ModelVariableFormat.DECIMAL)


    def calc_run_pro2(self,model):
        # This method calls the pro2 calculator

        # Load model values into local variables
        modem_frequency_hz = model.vars.modem_frequency_hz.value
        rx_xtal_error_ppm = model.vars.rx_xtal_error_ppm.value
        tx_xtal_error_ppm = model.vars.tx_xtal_error_ppm.value
        bandwidth_hz = model.vars.bandwidth_hz.value
        base_frequency_hz = model.vars.base_frequency_hz.value
        test_ber = model.vars.test_ber.value
        pro2_osr_tune = model.vars.pro2_osr_tune.value
        pro2_pm_pattern = model.vars.pro2_pm_pattern.value
        symbol_encoding = model.vars.symbol_encoding.value
        pro2_modulation_type = model.vars.pro2_modulation_type.value
        baudrate_tol_req_ppm = model.vars.baudrate_tol_ppm.value
        deviation = model.vars.deviation.value
        pro2_afc_mode = model.vars.pro2_afc_mode.value
        pro2_ant_div = model.vars.pro2_ant_div.value
        pro2_dsa_mode = model.vars.pro2_dsa_mode.value
        digmix_res = model.vars.digmix_res_actual.value
        pro2_fs_rx_ch = model.vars.pro2_fs_rx_ch.value
        baudrate = model.vars.baudrate.value
        bcr_demod_en = model.vars.bcr_demod_en.value
        bcr_detector_en = model.vars.MODEM_PHDMODCTRL_BCRDETECTOR.value


        #Only run the PRO2 calculator if the BCR demod is enabled
        if bcr_demod_en or bcr_detector_en == 1:
        #Create the input dict
            fdeverror = model.vars.deviation_tol_ppm.value
            pro2_inputs = OrderedDict({})
            pro2_inputs["API_freq_xo"] = modem_frequency_hz
            pro2_inputs["API_crystal_tol"] = (rx_xtal_error_ppm + tx_xtal_error_ppm)/2.0
            pro2_inputs["API_if_mode"] = 2 #Fixed IF = 468.75kHz
            pro2_inputs["API_High_perf_Ch_Fil"] = 1 #High performance channel filter enabled, don't care since we aren't using this IP
            pro2_inputs["API_OSRtune"] = pro2_osr_tune
            pro2_inputs["API_Ch_Fil_Bw_AFC"] = 0 #Disable channel filter switch after AFC, we are not using this channel filter IP
            pro2_inputs["API_ant_div"] = pro2_ant_div
            pro2_inputs["API_pm_pattern"] = pro2_pm_pattern
            pro2_inputs["API_modulation_type"] = pro2_modulation_type
            pro2_inputs["API_Rsymb"] = baudrate
            pro2_inputs["API_Fdev"] = deviation
            pro2_inputs["API_RXBW"] = bandwidth_hz
            pro2_inputs["API_Manchester"] = (symbol_encoding == model.vars.symbol_encoding.var_enum.Manchester or \
                                             symbol_encoding == model.vars.symbol_encoding.var_enum.Inv_Manchester)
            pro2_inputs["API_afc_en"] = pro2_afc_mode
            pro2_inputs["API_Max_Rb_Error"] = baudrate_tol_req_ppm / (1e6)
            pro2_inputs["API_Chip_Version"] = 2 #Assume Rev B PRO2 with VCO 3.4GHz to 4.2GHz
            pro2_inputs["API_fc"] = base_frequency_hz
            pro2_inputs["API_TC"] = 28 #Default ramp time, don't care
            pro2_inputs["API_fhst"] = 2000
            pro2_inputs["API_inputBW"] = 1 #Specify FSK RX BW explicitly
            pro2_inputs["API_BER_mode"] = test_ber
            pro2_inputs["API_raw_demod"] = 0 #TODO: Need to implement this later
            pro2_inputs["API_dsource"] = 0 #Don't care, this is for direct mode TX
            pro2_inputs["API_hi_pfm_div_mode"] = 1 #Always set to 1 for PRO2
            pro2_inputs["API_dsa_mode"] = pro2_dsa_mode
            pro2_inputs["API_dsm_ratio"] = 1
            pro2_inputs["API_Fs_rx_CH"] = pro2_fs_rx_ch
            pro2_inputs["API_fb_frequency_resolution"] = digmix_res
            pro2_inputs["API_modulation_index"] = model.vars.modulation_index_actual.value
            pro2_inputs["Fdev_error"] = fdeverror / 1e6
            pro2_inputs["API_pm_len"] = model.vars.preamble_detection_length.value
            # overwrite BCR demod setting in concurrent mode which is set by forcing bcr_demod_en at input
            if (model.vars.bcr_demod_en._value_forced != None):
                pro2_inputs["API_Rsymb"] = model.vars.bcr_conc_baudrate.value
                pro2_inputs["API_Manchester"] = model.vars.bcr_conc_manchester.value
                pro2_inputs["API_Fdev"] = model.vars.bcr_conc_deviation.value

            #Instantiate an object for pro2 inputs and initialize with the input values defined above
            pro2_input_obj = Pro2OcelotModemCalcInputs(pro2_inputs)

            #Instantiate the pro2 calculator and initialize with our input object
            pro2_calculator_obj = Pro2OcelotModemCalc(pro2_input_obj)

            #Run the pro2 calculator
            pro2_calculator_obj.calculate()

            #Assign the pro2 output values to registers / variables
            self._map_pro2_dsa_outputs(model, pro2_calculator_obj)
            if bcr_detector_en == 1:
                self.write_unused_bcr_regs(model)
            else:
                self._map_pro2_outputs(model, pro2_calculator_obj)
        else:
            #Write default values to the BCR demod regs
            self.write_unused_pro2_dsa_regs(model)
            self.write_unused_bcr_regs(model)

    def _map_pro2_dsa_outputs(self, model, pro2_calculator_obj):
        antdivmode = model.vars.antdivmode.value

        # : check
        phspike_det_thd = pro2_calculator_obj.demodulator.fields.spike_det_thd

        # : If antenna diversity is enabled, scale up the detection threshold
        scale = 1.5 # TODO REVERT 1.5
        if antdivmode != model.vars.antdivmode.var_enum.DISABLE:
            phspike_det_thd = scale * phspike_det_thd

        #Always set ENCFEQUAL to 0 for OOK, otherwise use signal_dsa_mode
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_ENCFEQUAL,
                        pro2_calculator_obj.demodulator.fields.signal_dsa_mode if pro2_calculator_obj.inputs.API_modulation_type != 1 else 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRDETEN, pro2_calculator_obj.demodulator.fields.signal_dsa_mode)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_NONSTDPK, pro2_calculator_obj.demodulator.fields.nonstdpk_final)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_PHSPIKETHD, int(phspike_det_thd))

    def write_unused_pro2_dsa_regs(self, model):
        self._reg_write(model.vars.MODEM_BCRDEMODARR1_ENCFEQUAL, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRDETEN, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_NONSTDPK, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODARR0_PHSPIKETHD, 0)

    def _map_pro2_outputs(self,model, pro2_calculator_obj):
        #This function maps the pro2 calculator outputs to radio configurator variables / outputs

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
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_BWPK,  model.vars.calculated_bw_peak.value)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_DECAYSWAL, pro2_calculator_obj.demodulator.fields.decay_swal)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_DECAY, int(pro2_calculator_obj.demodulator.fields.Decay))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_ATTACK, pro2_calculator_obj.demodulator.fields.Attack)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_SQUELCH, pro2_calculator_obj.demodulator.fields.arr_squelch)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_RAWNDEC, int(pro2_calculator_obj.demodulator.fields.ndec0))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_BCRDEMODOOK,int(enook(pro2_calculator_obj.inputs.API_modulation_type)))
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_PKTRUNK, 0)

        #Write BCRCTRL0
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRNCOFF, int(pro2_calculator_obj.demodulator.fields.ncoff))
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRALIGN, pro2_calculator_obj.demodulator.fields.bcr_align_en)
        self._reg_write(model.vars.MODEM_BCRCTRL0_DISTOGG, pro2_calculator_obj.demodulator.fields.distogg)
        self._reg_write(model.vars.MODEM_BCRCTRL0_CRSLOW, pro2_calculator_obj.demodulator.fields.crslow)
        self._reg_write(model.vars.MODEM_BCRCTRL0_CRFAST, pro2_calculator_obj.demodulator.fields.crfast)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRERRRSTEN, 1)
        self._reg_write(model.vars.MODEM_BCRCTRL0_BCRFBBYP, pro2_calculator_obj.demodulator.fields.bcrfbbyp)

        #Write BCRCTRL1
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

        #Write BCRDEMODAFC0
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_LARGEFREQERR, int(pro2_calculator_obj.demodulator.fields.large_freq_err))
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_AFCGAINOVRFLW, pro2_calculator_obj.demodulator.fields.afcgain_ovr_flw)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_EN2TBEST, pro2_calculator_obj.demodulator.fields.en2Tb_est)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC0_ENAFCCLKSW, pro2_calculator_obj.demodulator.fields.enafc_clksw)

        #Write BCRDEMODAFC1
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_LGWAIT, pro2_calculator_obj.demodulator.fields.afc_lgwait)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_SHWAIT, pro2_calculator_obj.demodulator.fields.afc_shwait)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_GEARSW, pro2_calculator_obj.demodulator.fields.gear_sw)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_AFCMAEN, pro2_calculator_obj.demodulator.fields.afcma_en)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENFZPMEND, pro2_calculator_obj.demodulator.fields.enfzpmend)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_NONFRZEN, pro2_calculator_obj.demodulator.fields.non_frzen)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ONESHOTWAITCNT, pro2_calculator_obj.demodulator.fields.oneshot_waitcnt)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ONESHOTAFCEN,pro2_calculator_obj.demodulator.fields.oneshot_afc)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_SKIPPMDET, pro2_calculator_obj.demodulator.fields.skip_pm_det)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENAFCFRZ, pro2_calculator_obj.demodulator.fields.afc_freez_en)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENAFC, pro2_calculator_obj.demodulator.fields.afc_est_en)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ENFBPLL, int(pro2_calculator_obj.demodulator.fields.afc_fb_pll))
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_HALFPHCOMP, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_PMRSTEN, 0)

        #Write BCRDEMOD4FSK0
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_THD4GFSK, int(pro2_calculator_obj.demodulator.fields.thd4gfsk))
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_CODE4GFSK, pro2_calculator_obj.demodulator.fields.code4gfsk)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_PHCOMPBYP, pro2_calculator_obj.demodulator.fields.phcompbyp)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK0_EN4GFSK, int(en4gfsk(pro2_calculator_obj.inputs.API_modulation_type)))

        #Write BCRDEMOD4FSK1
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK0, pro2_calculator_obj.demodulator.fields.phcomp_gain_4gfsk0)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK1,
                        pro2_calculator_obj.demodulator.fields.phcomp_gain_4gfsk1 & 0x7f) #Remove bit 7 which was used in Pro2 for a different purpose
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_S2PMAP, pro2_calculator_obj.demodulator.fields.s2p_map)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_FDEVCOMPEN, pro2_calculator_obj.demodulator.fields.thd4gfsk_comp_en)
        self._reg_write(model.vars.MODEM_BCRDEMOD4FSK1_FDEVCOMPRATIO, pro2_calculator_obj.demodulator.fields.thd4gfsk_comp_ratio)
        
        #Write BCRDEMODANT
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SKIP2PHTHD, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_ANWAIT, pro2_calculator_obj.demodulator.fields.anwait)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_AGCGAINUPB, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SKIP2PH, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_BYP1P5, 0)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_ANT2PMTHD, pro2_calculator_obj.demodulator.fields.ant2pm_thd)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_SWANTTIMER, 1)
        self._reg_write(model.vars.MODEM_BCRDEMODANT_BCRDEMODANTDIV, pro2_calculator_obj.demodulator.fields.antdiv)

        #Write BCRDEMODRSSI
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_RSSIARRTHD, pro2_calculator_obj.demodulator.fields.rssi_arr_thd)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_MUTERSSICNT, pro2_calculator_obj.demodulator.fields.mute_rssi_cnt)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_RSSIMATAP, pro2_calculator_obj.demodulator.fields.matap)
        self._reg_write(model.vars.MODEM_BCRDEMODRSSI_PRWOFFSET, 0)

        #Write BCRDEMODARR0
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
        self._reg_write(model.vars.MODEM_BCRDEMODKSI_BCRKSI3, model.vars.calculated_ksi3.value )

        # Write BCRDEMODPMEXP
        self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPHSCALE   ,  model.vars.calculated_phscale.value)
        self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPMEXP     , 21845)

        # GET afc gain result
        digmix_res = model.vars.digmix_res_actual.value
        model.vars.pro2_afc_gain.value = int(digmix_res * pro2_calculator_obj.demodulator.fields.afc_gain)

    def write_unused_bcr_regs(self,model):
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
        self._reg_write(model.vars.MODEM_BCRCTRL0_CRSLOW, 0)
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
        self._reg_write(model.vars.MODEM_BCRDEMODAFC1_ONESHOTWAITCNT, 0)
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



    def calc_bcr_demod_en(self,model):
        #This function writes the BCR demod enable register

        # Load model values into local variables
        demod_select = model.vars.demod_select.value

        if (demod_select == model.vars.demod_select.var_enum.BCR):
            bcr_demod_en = 1
        else:
            bcr_demod_en = 0

        #Write the register values
        model.vars.bcr_demod_en.value = bcr_demod_en

    def calc_bcr_demod_en_reg(self,model):

        bcr_demod_en = model.vars.bcr_demod_en.value

        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_BCRDEMODEN, bcr_demod_en)

    def calc_pro2_dsa_mode(self,model):
        preamble_detection_length = model.vars.preamble_detection_length.value
        mod_type = model.vars.pro2_modulation_type.value
        antdivmode = model.vars.antdivmode.value
        ber_force_fdm0 = model.vars.ber_force_fdm0.value

        if not ber_force_fdm0:
            if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV \
                    or antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
                pro2_dsa_mode = 1  # : always enable dsa mode for antenna diversity
            elif (mod_type == 1):  ## OOK or ASK
                #Enable for short preamble lengths
                if preamble_detection_length < 20:
                    pro2_dsa_mode = 1
                else:
                    pro2_dsa_mode = 0
            else:
                if (16 <= preamble_detection_length < 48) :
                    pro2_dsa_mode = 1
                else:
                    pro2_dsa_mode = 0
        else:
            #Disable DSA for BER testing
            pro2_dsa_mode = 0

        # Load local variables back into model variables
        model.vars.pro2_dsa_mode.value = pro2_dsa_mode

    def calc_pro2_afc_mode(self,model):
        # This function calculates the AFC mode for the pro2 calculator

        # Load model values into local variables
        preamble_detection_length = model.vars.preamble_detection_length.value
        mod_type = model.vars.pro2_modulation_type.value
        antdivmode = model.vars.antdivmode.value
        ber_force_freq_comp_off = model.vars.ber_force_freq_comp_off.value

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV \
                or antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            pro2_afc_mode = 2
        # if (preamble_detection_length >= 32):
            # AFC correction value is fed back to PLL in a pre-defined rate and the internal modem frequency compensation is running until the preamble is detected
        #     pro2_afc_mode = 1
        elif (16 <= preamble_detection_length < 48) and ( mod_type!= 1) and not ber_force_freq_comp_off:
            #AFC correction value is fed back to PLL just once while signal arrival detection is triggered and a pre-defined delay timer will be armed.
            # The internal modem frequency compensation is running until this delay timer is timeout.
            pro2_afc_mode = 2
        else:
            # AFC correction value is not fed back to PLL. The internal modem frequency compensation will be frozen once the preamble is detected.
            pro2_afc_mode = 0

        # Load local variables back into model variables
        model.vars.pro2_afc_mode.value = pro2_afc_mode


    def calc_pro2_osr_tune(self,model):
        # This function calculates the OSR tune value for the pro2 calculator

        # For now, just set this to a static 0
        # TODO 32: Decide how we want to divide the decimation between dec0/1/osrtune

        # Load local variables back into model variables
        model.vars.pro2_osr_tune.value = 0

    def calc_pro2_modulation_type(self,model):
        #This function calculates the pro2 modulation type input

        # Load model values into local variables
        mod_type = model.vars.modulation_type.value
        shaping_filter = model.vars.shaping_filter.value

        pro2_modulation_type = 1 #OOK by default
        if (mod_type == model.vars.modulation_type.var_enum.OOK) or (mod_type == model.vars.modulation_type.var_enum.ASK):
            pro2_modulation_type = 1
        elif (mod_type == model.vars.modulation_type.var_enum.FSK2) or \
                (mod_type == model.vars.modulation_type.var_enum.MSK):
            if (shaping_filter == model.vars.shaping_filter.var_enum.Gaussian):
                pro2_modulation_type = 3
            else:
                pro2_modulation_type = 2
        elif (mod_type == model.vars.modulation_type.var_enum.FSK4):
            if (shaping_filter == model.vars.shaping_filter.var_enum.Gaussian):
                pro2_modulation_type = 5
            else:
                pro2_modulation_type = 4

        # Load local variables back into model variables
        model.vars.pro2_modulation_type.value = pro2_modulation_type

    def calc_pro2_pm_pattern(self,model):
        # This function calculates the preamble pattern value for the pro2 calculator
        # May add additional preamble pattern cases later, for now this is sufficient

        # Load model values into local variables
        preamble_detection_length = model.vars.preamble_detection_length.value
        preamble_pattern = model.vars.preamble_pattern.value
        preamble_pattern_len_actual = model.vars.preamble_pattern_len_actual.value
        ber_force_fdm0 = model.vars.ber_force_fdm0.value

        if (preamble_detection_length >= 32) and not ber_force_fdm0:
            if ((preamble_pattern == 1) or (preamble_pattern==2)) and (preamble_pattern_len_actual == 2):
                # 1010 or 0101 repeating
                pro2_pm_pattern = 0
            elif ((preamble_pattern == 1) and (preamble_pattern_len_actual == 1)) or ((preamble_pattern == 3) and (preamble_pattern_len_actual == 2)):
                # 1111 repeating
                pro2_pm_pattern = 1
            elif (preamble_pattern == 0):
                # 0000 repeating
                pro2_pm_pattern = 2
            else:
                # Any other sequence
                pro2_pm_pattern = 3
        elif (preamble_detection_length >= 16) and not ber_force_fdm0:
            if ((preamble_pattern == 1) or (preamble_pattern==2)) and (preamble_pattern_len_actual == 2):
                # 1010 or 0101 repeating
                pro2_pm_pattern = 100
            else:
                # Any other sequence
                pro2_pm_pattern = 150
        else:
            #Any shorter sequence
            pro2_pm_pattern = 1000

        # Load local variables back into model variables
        model.vars.pro2_pm_pattern.value = pro2_pm_pattern

    def calc_pro2_antdiv(self,model):
        # This function calculates the pro2 antenna diversity enable

        # Load model values into local variables
        antdivmode = model.vars.antdivmode.value

        if (antdivmode == 0):
            pro2_ant_div = 0
        else:
            pro2_ant_div = 1

        # Load local variables back into model variables
        model.vars.pro2_ant_div.value = pro2_ant_div

    def calc_pro2_fs_rx_ch(self,model):
        # This function calculates the Pro2 calculator input value for API_Fs_rx_CH

        # Load model values into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        src2_actual = model.vars.src2_ratio_actual.value
        bcr_detector_en = model.vars.MODEM_PHDMODCTRL_BCRDETECTOR.value

        #Calculate a few more variables
        dec8 = 8

        if bcr_detector_en:
            pro2_fs_rx_ch = int(adc_freq_actual*src2_actual/dec8/dec0_actual/dec1_actual)
        else:
            pro2_fs_rx_ch = int(adc_freq_actual/dec8/dec0_actual/dec1_actual)

        # Load local variables back into model variables
        model.vars.pro2_fs_rx_ch.value = pro2_fs_rx_ch

    def calc_bcr_demod_ksi3_and_phscale(self,model):
        # This function calculates the KSI3 value for the BCR demod
        # Load model values into local variables
        calculated_phscale = model.vars.MODEM_TRECPMDET_PHSCALE.value
        ksi3wb_actual = model.vars.ksi3wb.value

        model.vars.calculated_ksi3.value = int(round(ksi3wb_actual))
        model.vars.calculated_phscale.value = calculated_phscale

    def calc_bcr_demod_ook_pkd_thd(self,model):
        # This function calculates the OOK peak detector thrd
        # Load model values into local variables
        mod_type = model.vars.pro2_modulation_type.value
        shaping_filter_mode = model.vars.MODEM_CTRL0_SHAPING.value
        model.vars.calculated_bw_peak.value = 12
        if (shaping_filter_mode != 0) and (mod_type == 1):  ## OOK or ASK
            model.vars.calculated_bw_peak.value = 18

    def calc_bcr_invrxbit(self,model):
        # Reading variables from model variables
        fskmap = model.vars.MODEM_CTRL0_MAPFSK.value
        mod_type = model.vars.modulation_type.value

        #Assigning INVRXBIT the appropiate value based on existing FSKMAP variable
        if (mod_type == model.vars.modulation_type.var_enum.FSK2):
            invrxbit = fskmap
        else:
            invrxbit = 0

        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_INVRXBIT, invrxbit)

    def calc_bcr_manchph(self,model):
        # Reading variables from model variables
        manchester_mapping = model.vars.manchester_mapping.value
        symbol_encoding = model.vars.symbol_encoding.value
        demod_select = model.vars.demod_select.value

        # Assigning MANCHPH the appropiate value based on existing symbol encoding and manchester mapping
        if (symbol_encoding == model.vars.symbol_encoding.var_enum.Inv_Manchester) and (demod_select == model.vars.demod_select.var_enum.BCR):
            manchph = 1
        else:
            manchph = 0

        self._reg_write(model.vars.MODEM_BCRDEMODCTRL_MANCHPH, manchph)
		
    def calc_eyeopenthd_reg(self, model):

        phscale = model.vars.phscale_actual.value
        ksi3 = model.vars.calculated_ksi3.value
        demod_select = model.vars.demod_select.value

        if (demod_select == model.vars.demod_select.var_enum.BCR):
            eyeopenthd = int(4 * ksi3 * phscale)
        else:
            eyeopenthd = 0

        self._reg_write(model.vars.MODEM_BCRDEMODARR1_EYEOPENTHD, eyeopenthd)

    def calc_bbpmdeten_arrqpm(self, model):
        # Reading variables from model variables
        encfequal_actual = model.vars.MODEM_BCRDEMODARR1_ENCFEQUAL.value
        demod_select = model.vars.demod_select.value
        en2TB_est = model.vars.MODEM_BCRDEMODAFC0_EN2TBEST.value
        estosren_actual = model.vars.MODEM_BCRCTRL1_ESTOSREN.value

        if (encfequal_actual == 1) and (demod_select == model.vars.demod_select.var_enum.BCR):
            # : Based on 2FSK low modulation index and preamble len < 48 investigation
            # : Floor issue is resolved by disabling BBPMDET mode and using BT preamble detection mode.
            # : https://jira.silabs.com/browse/MCUW_RADIO_CFG-1440
            if en2TB_est == 0:
                self._reg_write(model.vars.MODEM_BCRDEMODCTRL_BBPMDETEN, 1)
            else:
                self._reg_write(model.vars.MODEM_BCRDEMODCTRL_BBPMDETEN, 0)
            self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRQPM, 1)
        else:
            self._reg_write(model.vars.MODEM_BCRDEMODARR0_ARRQPM, 0)
            self._reg_write(model.vars.MODEM_BCRDEMODCTRL_BBPMDETEN, 0)

    def calc_freq_dev_min(self, model):
        # Reading variables from model variables
        fdeverror = model.vars.deviation_tol_ppm.value
        deviation = model.vars.deviation.value
        freq_dev_min = int(deviation - (fdeverror * deviation) / 1000000)
        model.vars.freq_dev_min.value = freq_dev_min

    def calc_freq_dev_max(self, model):
        # Reading variables from model variables
        fdeverror = model.vars.deviation_tol_ppm.value
        deviation = model.vars.deviation.value
        freq_dev_max = int(deviation + (fdeverror * deviation) / 1000000)
        model.vars.freq_dev_max.value = freq_dev_max

    def calc_bcr_cfe_costthd(self,model):

        antdivmode = model.vars.antdivmode.value

        if model.vars.bcr_demod_en.value == 1:
            #Need to read in these variables inside of the if statement as some are don't care if BCR disabled
            calculated_bcrpmacqwin = model.vars.MODEM_BCRDEMODPMEXP_BCRPMACQWIN.value
            if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV or antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
                bcr_cfe_costthd = 150 # If antdiv enabled, bcrpmacqwin is always 2 so the cost thd is constant.
            else:
                bcr_cfe_costthd = int(round(215 - (3 - calculated_bcrpmacqwin) * 60))
        else:
            bcr_cfe_costthd = 255

        self._reg_sat_write(model.vars.MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD, bcr_cfe_costthd) #We always care about this

    def calc_bcr_symbwmax(self, model):
        pro2_dsa_en = model.vars.pro2_dsa_mode.value
        estosren_actual = model.vars.MODEM_BCRCTRL1_ESTOSREN.value
        bcr_osr_actual = model.vars.MODEM_BCRCTRL1_BCROSR.value
        bcr_osr_trunc = bcr_osr_actual >> 6

        if pro2_dsa_en == 0:
            symbwmin = 0
            symbwmax = 31
        elif bcr_osr_trunc == 0: # : for low BCR OSR, keep short symbol width
            symbwmax = 4
            symbwmin = 1
        else:
            # : Ensure that for mbus case with long preamble has long symbwmax
            if estosren_actual == 1:
                symbwmax = 8
                symbwmin = 2
            else:
                # : Based on 2-FSK with estoren disabled test, sybwmax needs to be set lower to reduce floor near AGC
                # : adjustment region (floor around -50 dBm)
                symbwmax = 6
                symbwmin = 2

        if model.vars.bcr_demod_en.value == 1:
            self._reg_write(model.vars.MODEM_BCRDEMODARR1_SYMBWMIN, symbwmin)
            self._reg_write(model.vars.MODEM_BCRDEMODARR1_SYMBWMAX, symbwmax)
        else:
            self._reg_do_not_care(model.vars.MODEM_BCRDEMODARR1_SYMBWMIN)
            self._reg_do_not_care(model.vars.MODEM_BCRDEMODARR1_SYMBWMAX)

    def calc_bcr_cfeschwin(self, model):
        preamble_detection_length = model.vars.preamble_detection_length.value
        estosren_actual = model.vars.MODEM_BCRCTRL1_ESTOSREN.value
        ber_force_fdm0 = model.vars.ber_force_fdm0.value

        if (preamble_detection_length < 32) or ber_force_fdm0: # : for short preamble, keep CFE window short
            bcrcfeschwin = 3
        elif 32 <= preamble_detection_length < 40:
            if estosren_actual == 1: # : For mbus case with long preamble, keep CFE window short
                bcrcfeschwin = 3
            else:
                # : Based on 2-FSK with estoren disabled test, keep CFE window long to reduce floor near AGC
                # : adjutment region (floor around -50 dBm).
                bcrcfeschwin = 4
        else: # : for long preamble, keep CFE window long
            bcrcfeschwin = 4

        if model.vars.bcr_demod_en.value == 1:
            self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRCFESCHWIN, bcrcfeschwin)
        else:
            self._reg_do_not_care(model.vars.MODEM_BCRDEMODPMEXP_BCRCFESCHWIN)

    def calc_bcr_cfequalthd(self, model):

        if model.vars.bcr_demod_en.value == 1:
            #Need to read this model variable inside the if statement as it may be set to dont care if BCR disabled
            bcrcfeschwin_actual = model.vars.MODEM_BCRDEMODPMEXP_BCRCFESCHWIN.value

            # : zero crossing qualified threshold is recommended by Wentao to be set as CFE window minus 1
            if bcrcfeschwin_actual > 0:
                cfequalthd = bcrcfeschwin_actual - 1
            else:
                cfequalthd = 0
        else:
           cfequalthd = 7

        self._reg_write(model.vars.MODEM_BCRDEMODARR1_CFEQUALTHD, cfequalthd) #We always care about this field

    def calc_bcr_pmacqwin(self, model):
        preamble_detection_length = model.vars.preamble_detection_length.value
        estosren_actual = model.vars.MODEM_BCRCTRL1_ESTOSREN.value
        antdivmode = model.vars.antdivmode.value
        ber_force_fdm0 = model.vars.ber_force_fdm0.value

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV or antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            pmacqwin = 2 # : If antdiv mode enabled, keep window short so that shorter preamble length can be used
        elif (preamble_detection_length < 32) or ber_force_fdm0: # : for short preamble, keep preamble  window short
            pmacqwin = 2
        elif 32 <= preamble_detection_length < 40:
            if estosren_actual == 1:
                pmacqwin = 2
            else:
                pmacqwin = 3
        else:
            pmacqwin = 3

        if model.vars.bcr_demod_en.value == 1:
            self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRPMACQWIN, pmacqwin)
        else:
            self._reg_do_not_care(model.vars.MODEM_BCRDEMODPMEXP_BCRPMACQWIN)

    def calc_bcrdemodook_noiseflest_abpk_regs(self, model):
        #This function programs the BCRDEMODOOK_NOISEFLEST and BCRDEMODOOK_ABPK fields

        #Read in model variables
        modulation_type = model.vars.modulation_type.value
        preamble_detection_length = model.vars.preamble_detection_length.value

        #Set the register value based on mod type and preamble length
        if (modulation_type == model.vars.modulation_type.var_enum.OOK or
            modulation_type == model.vars.modulation_type.var_enum.ASK) and preamble_detection_length < 20:
            #OOK with short preamble case
            noiseflest = 1
            abpk = 4
        else:
            noiseflest = 0
            abpk = 0

        #Write the register

        self._reg_write(model.vars.MODEM_BCRDEMODOOK_NOISEFLEST, noiseflest)
        self._reg_write(model.vars.MODEM_BCRDEMODOOK_ABPK, abpk)
