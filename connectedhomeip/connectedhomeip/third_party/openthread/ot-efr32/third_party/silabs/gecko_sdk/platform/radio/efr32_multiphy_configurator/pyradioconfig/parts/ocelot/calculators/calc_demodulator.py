from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.parts.lynx.calculators.calc_demodulator import CALC_Demodulator_lynx
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from math import *
from py_2_and_3_compatibility import *
from pyradioconfig.parts.ocelot.calculators.calc_shaping import CALC_Shaping_ocelot
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
import numpy as np
import numpy.matlib
from scipy import signal as sp

#This file contains calculations related to the digital signal path, including ADC clocking, decimators, SRCs, channel filter, datafilter, digital mixer, and baud rate

class CALC_Demodulator_ocelot(ICalculator):

    SRC2DENUM = 16384.0
    chf_required_clks_per_sample = 4

    def buildVariables(self, model):
        #TODO: Clean this up and consolidate model variables
        #A lot of code here for now, as we changed the CalcManager to not run Commmon code in Ocelot (use only inheritance)

        #Build variables from Lynx
        calc_demod_lynx_obj = CALC_Demodulator_lynx()
        calc_demod_lynx_obj.buildVariables(model)

        #New variables
        self._addModelVariable(model, 'adc_clock_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_clock_mode.var_enum = CreateModelVariableEnum(
            enum_name = 'AdcClockModeEnum',
            enum_desc = 'Defines how the ADC clock is derived',
            member_data = [
                ['HFXOMULT',0,  'Multiply HFXO for ADC Clock'],
                ['VCODIV',   1,  'Divide VCO for ADC Clock'],
            ])
        self._addModelActual(model, 'adc_clock_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_clock_mode_actual.var_enum = model.vars.adc_clock_mode.var_enum
        self._addModelVariable(model, 'adc_rate_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_rate_mode.var_enum = CreateModelVariableEnum(
            enum_name='AdcRateModeEnum',
            enum_desc='ADC Clock Rate Mode',
            member_data=[
                ['FULLRATE', 0, 'Full rate mode'],
                ['HALFRATE', 1, 'Half rate mode'],
                ['EIGHTHRATE',2,'Eighth rate mode']
            ])
        self._addModelVariable(model, 'adc_xo_mult', int, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'adc_freq', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'datafilter_taps', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'enable_high_mod_trecs', int, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'adc_xo_mult', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'lo_target_freq', long, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'adc_target_freq', int, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'adc_vco_div', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'adc_vco_div', int, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'a_divider', int, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'b_divider', int, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'c_divider', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'adc_freq_error', float, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'digmixfreq', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'src2_ratio', float, ModelVariableFormat.DECIMAL)
        self._addModelActual(model, 'timing_detection_threshold_gain', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'rx_deviation_scaled', float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'demod_select', Enum, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'trecs_enabled', bool, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'max_dec2', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'min_dec2', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'bitrate_gross', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'digmixfreq', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'chflatency_actual', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'preamble_detection_length', int, ModelVariableFormat.DECIMAL,
                               desc='Number of preamble bits to use for timing detection')

        model.vars.demod_select.var_enum = CreateModelVariableEnum(
            enum_name='DemodSelectEnum',
            enum_desc='Demod Selection',
            member_data=[
                ['LEGACY', 0, 'Legacy Demod'],
                ['COHERENT', 1, 'Coherent Demod'],
                ['TRECS_VITERBI', 2, 'TRecS + Viterbi Demod'],
                ['TRECS_SLICER', 3, 'TRecS + HD Demod'],
                ['BCR', 4, 'PRO2 BCR Demod'],
                ['LONGRANGE', 5, 'BLE Long Range Demod']
            ])
        self._addModelVariable(model, 'min_bwsel', float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'max_bwsel', float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'min_src2', float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'max_src2', float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'bandwidth_tol', float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'phscale_derate_factor', int, ModelVariableFormat.DECIMAL)

        self._add_demod_rate_variable(model)

    def _add_demod_rate_variable(self, model):
        self._addModelActual(model, 'demod_rate', int, ModelVariableFormat.DECIMAL)

    def return_solution(self, model, demod_select):

        # Check if we have a solution for OSR, DEC0, and DEC1
        [target_osr, dec0, dec1, min_osr, max_osr] = self.return_osr_dec0_dec1(model, demod_select)

        # If we have selected TRECS but did not find a solution with the above line try to find a solution
        # with relaxed SRC2 limits (SRC2 > 0.55 instead of SRC2 > 0.8)
        # FIXME: once we are comfortable with the limit at 0.55 we might want to make this the general limit and remove this call

        is_trecs = demod_select == model.vars.demod_select.var_enum.TRECS_SLICER or demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI

        # is_vcodiv_high_bw widens the src2 limits for PHYs that would be affected by IPMCUSRW_876
        # The issue occurs when the filter chain is in a VCODIV + dec=4,1 configuration. We'll want to constrain
        # the filter to go to the next decimation factor (likely 3,2) and use fractional interpolation on the SRC2.
        # We can't use dec0_actual, dec1_actual because those are the variables we are solving for
        # instead, base the decision on if the bandwidth is in the range of what would use dec=4,1.
        # the final check is handled by _channel_filter_clocks_valid
        bandwidth_hz_threshold = model.vars.adc_freq_actual.value / (8 * 4 * 1) * 0.2
        is_vcodiv_high_bw = model.vars.adc_clock_mode.value == model.vars.adc_clock_mode.var_enum.VCODIV and \
                            model.vars.bandwidth_hz.value > bandwidth_hz_threshold

        no_solution = target_osr == 0 or target_osr > max_osr
        if (is_trecs or is_vcodiv_high_bw) and no_solution:
            [target_osr, dec0, dec1, min_osr, max_osr] = self.return_osr_dec0_dec1(model, demod_select, relaxsrc2=True)

        # If in TRECS SLICER mode we have one more chance to find a working solution this time with the remodulation
        # path enabled.
        if demod_select == model.vars.demod_select.var_enum.TRECS_SLICER and (
                target_osr == 0 or target_osr > max_osr):
            [target_osr, dec0, dec1, min_osr, max_osr] = self.return_osr_dec0_dec1(model, demod_select, withremod=True)

        # return solution if we have found one
        return target_osr, dec0, dec1, min_osr, max_osr

    def calc_demod_sel(self, model):

        modtype = model.vars.modulation_type.value
        tol = model.vars.baudrate_tol_ppm.value
        mi = model.vars.modulation_index.value
        antdivmode = model.vars.antdivmode.value

        if hasattr(model.profiles, 'Long_Range'):
            is_long_range = model.profile == model.profiles.Long_Range
        else:
            is_long_range = False

        if model.vars.demod_select._value_forced != None:
            demod_select = model.vars.demod_select._value_forced
            [target_osr, dec0, dec1, min_osr, max_osr] = self.return_solution(model, demod_select)

        else:
            # choose demod_select based on modulation and demod priority
            if (modtype == model.vars.modulation_type.var_enum.OOK) or \
                    (modtype==model.vars.modulation_type.var_enum.ASK):
                demod_select = model.vars.demod_select.var_enum.BCR
                [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model,demod_select)
                # TODO: Is there a case where osr < 7

            elif (modtype == model.vars.modulation_type.var_enum.OQPSK):
                if is_long_range:
                    demod_select = model.vars.demod_select.var_enum.COHERENT
                    [target_osr, dec0, dec1, min_osr, max_osr] = self.return_solution(model, demod_select)
                else:
                    demod_select = model.vars.demod_select.var_enum.LEGACY
                    [target_osr, dec0, dec1, min_osr, max_osr] = self.return_solution(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.BPSK) or \
                     (modtype == model.vars.modulation_type.var_enum.DBPSK):
                demod_select = model.vars.demod_select.var_enum.LEGACY
                [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.FSK4):

                demod_select = model.vars.demod_select.var_enum.LEGACY
                [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.FSK2 or \
                    modtype == model.vars.modulation_type.var_enum.MSK):

                # : for these antdivmode, can only use legacy or coherent demod
                if antdivmode == model.vars.antdivmode.var_enum.ANTSELFIRST or \
                        antdivmode == model.vars.antdivmode.var_enum.ANTSELCORR or \
                        antdivmode == model.vars.antdivmode.var_enum.ANTSELRSSI:
                    demod_select = model.vars.demod_select.var_enum.LEGACY
                    [target_osr, dec0, dec1, min_osr, max_osr] = self.return_osr_dec0_dec1(model, demod_select)
                else:
                    if tol > 10000:
                        demod_select = model.vars.demod_select.var_enum.BCR
                        [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)
                    else:
                        if mi < 1.0:
                            if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
                                # : don't use legacy demod for this anntena diversity mode
                                demod_select_list = [model.vars.demod_select.var_enum.TRECS_VITERBI,
                                                     model.vars.demod_select.var_enum.BCR]
                            else:
                                demod_select_list = [model.vars.demod_select.var_enum.TRECS_VITERBI,
                                                     model.vars.demod_select.var_enum.BCR,
                                                     model.vars.demod_select.var_enum.LEGACY]
                        else:
                            if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
                                # : don't use legacy demod for this anntena diversity mode
                                demod_select_list = [model.vars.demod_select.var_enum.TRECS_SLICER,
                                                     model.vars.demod_select.var_enum.BCR]
                            else:
                                demod_select_list = [model.vars.demod_select.var_enum.TRECS_SLICER,
                                                     model.vars.demod_select.var_enum.BCR,
                                                     model.vars.demod_select.var_enum.LEGACY]

                        # loop over demod list and see if we can find a solution
                        for demod_select in demod_select_list:
                            [target_osr, dec0, dec1, min_osr, max_osr] = self.return_solution( model, demod_select)

                            # stop at first solution
                            if target_osr != 0:
                                break

        if target_osr == 0:
            raise CalculationException('WARNING: target_osr=0 in calc_choose_demod()')

        model.vars.demod_select.value = demod_select
        model.vars.target_osr.value = int(target_osr)
        model.vars.targetmin_osr.value = int(min_osr)
        model.vars.targetmax_osr.value = int(max_osr)
        model.vars.dec0.value = int(dec0)
        model.vars.dec1.value = int(dec1)

    def calc_trecs_enabled(self, model):
        demod_select = model.vars.demod_select.value

        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:
            trecs_enabled = True
        else:
            trecs_enabled = False

        model.vars.trecs_enabled.value = trecs_enabled

    def calc_osr_actual(self,model):
        #This function calculates the actual OSR based on the ADC rate and decimator/SRC values

        #Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        baudrate_actual = model.vars.rx_baud_rate_actual.value
        src2_actual = model.vars.src2_ratio_actual.value
        bcr_remod_dec = 2 ** model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value

        osr_actual = adc_freq_actual * src2_actual / (dec0_actual * dec1_actual * 8 * dec2_actual * bcr_remod_dec * baudrate_actual)

        #Load local variables back into model variables
        model.vars.oversampling_rate_actual.value = osr_actual

    def calc_trecsosr_reg(self, model):
        #This function writes the TRECSOSR register

        #Load model variables into local variables
        demod_select = model.vars.demod_select.value
        osr_actual = model.vars.oversampling_rate_actual.value
        remoddwn = model.vars.MODEM_PHDMODCTRL_REMODDWN.value + 1
        trecs_enabled = model.vars.trecs_enabled.value

        if trecs_enabled:
            trecsosr_reg = osr_actual / remoddwn
        else:
            trecsosr_reg = 0

        #Write the register
        self._reg_write(model.vars.MODEM_TRECSCFG_TRECSOSR, int(round(trecsosr_reg)))

    def return_dec0_from_reg(self, reg):
        """convert register value to decimation value

        Args:
            reg (int) : register value to decimation value
        """
        if reg == 0:
            dec0 = 3
        elif reg == 1 or reg == 2:
            dec0 = 4
        elif reg == 3 or reg == 4:
            dec0 = 8
        elif reg == 5:
            dec0 = 5

        return dec0

    def calc_dec0_reg(self,model):
        #This function writes the register for dec0

        #Load model variables into local variables
        dec0_value = model.vars.dec0.value

        #Define a constant list for the (register data, value pairs)
        dec0_list = [(0, 3), (2, 4), (4, 8)]
        # Search for the value in the list
        for dec0_pair in dec0_list:
            if (dec0_pair[1]==dec0_value):
                dec0_reg = dec0_pair[0]

        #Write the registers
        self._reg_write(model.vars.MODEM_CF_DEC0, dec0_reg)

    def calc_dec1_reg(self, model):
        #This function writes the register for dec1

        #Load model variables into local variables
        dec1_value = model.vars.dec1.value

        #Dec1 register is simply one less than the value
        dec1_reg = dec1_value - 1

        #Write the registers
        self._reg_write(model.vars.MODEM_CF_DEC1, dec1_reg)

    def calc_dec0_actual(self,model):
        #This function calculates the actual dec0 based on the register value

        #Load model variables into local variables
        dec0_reg = model.vars.MODEM_CF_DEC0.value

        #Define a constant list for the (register data, value pairs)
        dec0_list = [(0, 3), (1, 4), (2, 4), (3, 8), (4, 8)]
        #Search for the value in the list
        for dec0_pair in dec0_list:
            if (dec0_pair[0]==dec0_reg):
                dec0_value = dec0_pair[1]

        #Load local variables back into model variables
        model.vars.dec0_actual.value = dec0_value

    def calc_dec1_actual(self, model):
        #This function calculates the actual dec1 based on the register value

        #Load model variables into local variables
        dec1_reg = model.vars.MODEM_CF_DEC1.value

        #Dec1 value is simply one more than the register setting
        dec1_value = dec1_reg + 1

        #Load local variables back into model variables
        model.vars.dec1_actual.value = dec1_value

    def calc_src2_dec2(self,model):
        #This function calculates dec2 and src2

        # FIXME: need to have an options for TRecS where DEC2 is bypassed DEC2=1
        #        unless the remod is enabled

        #Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        baudrate = model.vars.baudrate.value #We don't know the actual baudrate yet
        target_osr = model.vars.target_osr.value #We don't know the actual OSR value yet
        demod_sel = model.vars.demod_select.value
        max_dec2 = model.vars.max_dec2.value
        min_dec2 = model.vars.min_dec2.value

        min_src2 = model.vars.min_src2.value  # min value for SRC2
        max_src2 = model.vars.max_src2.value  # max value for SRC2

        if (demod_sel==model.vars.demod_select.var_enum.BCR):
            # BCR demod, dec2 and src2 not enabled
            best_dec2 = 1
            best_src2 = 1.0

        else:
            # Legacy, Coherent, Trecs/Viterbi Demods
            #Calculate the OSR at the input to SRC2
            osr_src2_min = float(adc_freq) / (8 * dec0 * dec1 * baudrate) * min_src2
            osr_src2_max = float(adc_freq) / (8 * dec0 * dec1 * baudrate) * max_src2

            #Calculate dec2 to achieve close to the target OSR
            dec2_min = max(int( ceil(osr_src2_min / target_osr)),min_dec2)
            dec2_max = min(int(floor(osr_src2_max / target_osr)),max_dec2)

            target_src2 = 1.0
            best_error = 999

            # default values
            best_dec2 = 1
            best_src2 = (8 * dec0 * dec1 * baudrate) * target_osr / float(adc_freq)

            for dec2 in range(dec2_min, dec2_max + 1):
                src2 = dec2 * (8 * dec0 * dec1 * baudrate) * target_osr / float(adc_freq)
                error = abs(target_src2 - src2)
                if best_error > error:
                    best_error = error
                    best_src2 = src2
                    best_dec2 = dec2

        #Load local variables back into model variables
        model.vars.dec2.value = best_dec2
        model.vars.src2_ratio.value = best_src2

    def calc_src2_reg(self,model):
        #This function calculates the src2 register writes

        # Load model variables into local variables
        src2_value = model.vars.src2_ratio.value
        min_src2 = model.vars.min_src2.value  # min value for SRC2
        max_src2 = model.vars.max_src2.value  # max value for SRC2

        if (src2_value) >= min_src2 and (src2_value <= max_src2):
            src2_reg = int(round(16384/src2_value))
        else:
            raise CalculationException('WARNING: src2 value out of range in calc_src2_reg()')

        if (src2_reg != 16384):
            src2_en = 1
        else:
            src2_en = 0

        #Write to registers
        self._reg_write(model.vars.MODEM_SRCCHF_SRCRATIO2, src2_reg)
        self._reg_write(model.vars.MODEM_SRCCHF_SRCENABLE2, src2_en)

    def calc_dec2_reg(self,model):
        #This function calculates the dec2 register value

        #Load model variables into local variables
        dec2_value = model.vars.dec2.value

        #The dec2 register is one less than the decimation value
        dec2_reg = dec2_value - 1

        #Write to register
        self._reg_write(model.vars.MODEM_CF_DEC2, dec2_reg)

    def calc_src2_actual(self,model):
        #This function calculates the actual SRC2 ratio from the register value

        #Load model variables into local variables
        src2_reg = model.vars.MODEM_SRCCHF_SRCRATIO2.value
        src2_en_reg = model.vars.MODEM_SRCCHF_SRCENABLE2.value

        if src2_en_reg:
            #The src2 ratio is simply 16384 divided by the register value
            src2_ratio_actual = 16384.0 / src2_reg
        else:
            src2_ratio_actual = 1.0

        #Load local variables back into model variables
        model.vars.src2_ratio_actual.value = src2_ratio_actual

    def calc_dec2_actual(self,model):
        #This function calculates the actual dec2 ratio from the register value

        #Load model variables into local variables
        dec2_reg = model.vars.MODEM_CF_DEC2.value

        #The actual dec2 value is the dec2 register plus one
        dec2_actual = dec2_reg + 1

        #Load local variables back into model variables
        model.vars.dec2_actual.value = dec2_actual

    def calc_rxbr(self,model):
        #This function calculates the receive baudrate settings
        # based on actual dec0,dec1,dec2,src2, and desired baudrate
        # then baudrate_actual will be calculated from rxbrfrac_actual

        #Load model variables into local variables
        target_osr = model.vars.target_osr.value #We don't know the actual OSR yet, because that has to be based on the final baudrate
        targetmax_osr = model.vars.targetmax_osr.value
        targetmin_osr = model.vars.targetmin_osr.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2_actual = model.vars.src2_ratio_actual.value
        baudrate = model.vars.baudrate.value
        demod_select = model.vars.demod_select.value
        bcr_demod_en = model.vars.bcr_demod_en.value

        if demod_select == model.vars.demod_select.var_enum.BCR:
            # FIXME:  do we need to use the pro2 calc values here?
            # BCR uses its own registers for defined in the pro2 calculator
            model.vars.rxbrint.value = 0
            model.vars.rxbrnum.value = 1
            model.vars.rxbrden.value = 2
            return
        else:
            denlist = range(2, 31)
            error_limit = 0.5

        # not using target_osr, because in some cases (e.g. BCR with fractional OSR)
        # the OSR w.r.t desired baudrate and acutal decimators varies from the target
        # allowing 0.01% variation from targetmin_osr and targetmax_osr for range check
        # because this osr calculation uses src2_actual, which has some small quantization noise
        osr = float(adc_freq_actual * src2_actual) / float(dec0_actual * dec1_actual * 8 * dec2_actual * baudrate)
        osr_limit_min = targetmin_osr * (1 - 0.0001)
        osr_limit_max = targetmax_osr * (1 + 0.0001)

        if (osr >= osr_limit_min) and (osr <= osr_limit_max):
            #search for best fraction
            rxbrint = int(floor(osr/2))
            frac = (osr/2) - float(rxbrint)
            numlist = range(0,31)
            min_error = 100
            for den in denlist:
                for num in numlist:
                    frac_error = abs(float(num)/float(den) - frac)
                    if (frac_error<min_error):
                        min_error = frac_error
                        best_den = den
                        best_num = num
            # calculate error in percent of baudrate, and require < 0.5% error
            # matlab simulation sweeping osr with 0.01% step size, showed the max osr relative error = 0.4%
            # using num=0:31, den=2:31
            error_percent = 100 * abs( 2 * ( rxbrint + float(best_num) / float(best_den) ) - osr ) / osr
            if error_percent < error_limit:
                rxbrnum = best_num
                rxbrden = best_den
                if (rxbrnum==rxbrden):
                    rxbrden=2
                    rxbrnum=0
                    rxbrint=rxbrint+1
                elif rxbrnum>rxbrden:
                    raise CalculationException('ERROR: num > den in calc_rxbr()')

            else:
                #print("adc_freq = %f" % adc_freq_actual)
                #print("baudrate = %f" % baudrate)
                #print("target_osr = %f" % target_osr)
                #print("adjust_osr = %f" % osr)
                #print("rxbrint = %d" % rxbrint)
                #print("best_num = %d" % best_num)
                #print("best_den = %d" % best_den)
                #print(model.vars.demod_select.value)
                raise CalculationException('ERROR: baudrate error > 0.5% in calc_rxbr()')
        elif bcr_demod_en:
            rxbrint = 3
            rxbrnum = 1
            rxbrden = 2
        else:
            #print("adc_freq = %f" % adc_freq_actual)
            #print("baudrate = %f" % baudrate)
            #print("target_osr = %f" % target_osr)
            #print("adjust_osr = %f" % osr)
            #print("targetmin_osr = %f" % targetmin_osr)
            #print("targetmax_osr = %f" % targetmax_osr)
            #print(str(model.vars.demod_select.value).split(".")[-1])
            raise CalculationException('ERROR: OSR out of range in calc_rxbr()')

        #Load local variables back into model variables
        model.vars.rxbrint.value = rxbrint
        model.vars.rxbrnum.value = rxbrnum
        model.vars.rxbrden.value = rxbrden

    def calc_rxbr_reg(self,model):
        #This function writes the rxbr registers

        #Load model variables into local variables
        rxbrint =  model.vars.rxbrint.value
        rxbrnum = model.vars.rxbrnum.value
        rxbrden =  model.vars.rxbrden.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        baudrate = model.vars.baudrate.value
        src2_actual = model.vars.src2_ratio_actual.value
        trecs_enabled = model.vars.trecs_enabled.value

        osr = adc_freq_actual * src2_actual / (dec0_actual * dec1_actual * 8 * dec2_actual * baudrate)

        if trecs_enabled and osr >= 8:
            rxbrint = 0
            rxbrden = 2
            rxbrnum = 1

        #Write registers
        self._reg_sat_write(model.vars.MODEM_RXBR_RXBRINT, rxbrint)
        self._reg_sat_write(model.vars.MODEM_RXBR_RXBRNUM, rxbrnum)
        self._reg_sat_write(model.vars.MODEM_RXBR_RXBRDEN, rxbrden)


    def calc_rxbr_actual(self,model):
        #This function shows the actual rxbr values contained in the registers

        #Load model variables into local variables
        rxbrint_actual = model.vars.MODEM_RXBR_RXBRINT.value
        rxbrnum_actual = model.vars.MODEM_RXBR_RXBRNUM.value
        rxbrden_actual = model.vars.MODEM_RXBR_RXBRDEN.value

        #Calculate the rxbr fraction
        rxbrfrac_actual = float(rxbrint_actual + float(rxbrnum_actual) / rxbrden_actual)

        #Load local variables back into model variables
        model.vars.rxbrint_actual.value = rxbrint_actual
        model.vars.rxbrnum_actual.value = rxbrnum_actual
        model.vars.rxbrden_actual.value = rxbrden_actual
        model.vars.rxbrfrac_actual.value = rxbrfrac_actual

    # FIXME: why do we need a special symbol encoding for mbus? we should combine this with the defaul symbol encoding
    def calc_mbus_symbol_encoding(self,model):
        #This function calculates the default value for mbus_symbol_encoding

        #Set defaults
        mbus_symbol_encoding = model.vars.mbus_symbol_encoding.var_enum.NRZ

        # Load local variables back into model variables
        model.vars.mbus_symbol_encoding.value = mbus_symbol_encoding
        model.vars.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ  # mbus_symbol_encoding

    def calc_bitrate_gross(self, model):
        #This function calculates the gross bitrate (bitrate including redundant coding bits)
        #Note that this gross bitrate excludes DSSS, because in RX the DSSS chips never make it
        #through the demod path (they are only used for correlation)

        #Read from model variables
        bitrate = model.vars.bitrate.value
        encoding = model.vars.symbol_encoding.value
        mbus_encoding = model.vars.mbus_symbol_encoding.value
        fec_enabled = model.vars.fec_enabled.value

        #Start by assuming the gross bitrate is equal to the net bitrate
        bitrate_gross = bitrate

        #Calculate the encoded bitrate based on the encoding parameters
        if (encoding == model.vars.symbol_encoding.var_enum.Manchester or encoding == model.vars.symbol_encoding.var_enum.Inv_Manchester):
            bitrate_gross *= 2
        if (mbus_encoding == model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6):
            bitrate_gross *= 1.5
        if fec_enabled:
            bitrate_gross *= 2

        #Write the model variable
        model.vars.bitrate_gross.value =  int(round(bitrate_gross))

    def calc_baudrate(self,model):
        #This function calculates baudrate based on the input bitrate and modulation/encoding settings

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        bitrate_gross = model.vars.bitrate_gross.value
        encoding = model.vars.symbol_encoding.value
        spreading_factor = model.vars.dsss_spreading_factor.value

        #Based on modulation type calculate baudrate from bitrate
        if(mod_type == model.vars.modulation_type.var_enum.OQPSK) or \
                (mod_type == model.vars.modulation_type.var_enum.OOK) or \
                (mod_type == model.vars.modulation_type.var_enum.ASK) or \
                (mod_type == model.vars.modulation_type.var_enum.FSK2) or \
                (mod_type == model.vars.modulation_type.var_enum.MSK) or \
                (mod_type == model.vars.modulation_type.var_enum.BPSK) or \
                (mod_type == model.vars.modulation_type.var_enum.DBPSK):
            baudrate = bitrate_gross
        elif(mod_type == model.vars.modulation_type.var_enum.FSK4):
            baudrate = bitrate_gross / 2
        else:
            raise CalculationException('ERROR: modulation type not supported in calc_baudrate()')

        #Account for the DSSS spreading factor
        if (encoding == model.vars.symbol_encoding.var_enum.DSSS):
            baudrate *= spreading_factor

        #Load local variables back into model variables
        model.vars.baudrate.value = int(round(baudrate))

    def calc_baudrate_actual(self,model,disable_subfrac_divider=False):
        #This function calculates the actual baudrate based on register settings

        #Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2ratio_actual = model.vars.src2_ratio_actual.value
        subfrac_actual = model.vars.subfrac_actual.value
        rxbrfrac_actual = model.vars.rxbrfrac_actual.value
        dec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value
        bcr_demod_en_forced = (model.vars.bcr_demod_en.value_forced is not None)  # This is currently only done for conc PHYs
        agc_subperiod_actual = model.vars.AGC_CTRL7_SUBPERIOD.value

        if (subfrac_actual > 0) and (disable_subfrac_divider == False):
            frac = subfrac_actual * pow(2, dec)
        else:
            frac = rxbrfrac_actual

        #Calculate actual baudrate once the ADC, decimator, SRC, and rxbr settings are known
        if (bcr_demod_en_forced and agc_subperiod_actual == 1):
            n_update = pow(2, dec)
            baudrate_actual = (adc_freq * src2ratio_actual) / (dec0_actual * dec1_actual * n_update * 8 * frac)
        else:
            baudrate_actual = (adc_freq * src2ratio_actual) / (dec0_actual * dec1_actual * dec2_actual * 8 * 2 * frac)

        #Load local variables back into model variables
        model.vars.rx_baud_rate_actual.value = baudrate_actual

    def calc_bwsel(self,model, softmodem_narrowing=False):
        #This function calculates the bwsel ratio that sets the channel bandwidth

        #Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        afc_run_mode = model.vars.afc_run_mode.value
        bandwidth = model.vars.bandwidth_hz.value #We don't know the actual channel bandwidth yet
        lock_bandwidth = model.vars.lock_bandwidth_hz.value  # maybe this cab be reduced further based on residual freq offset
        min_bwsel = model.vars.min_bwsel.value

        #Calculate the required BWSEL from the adc rate, decimators, and required bandwidth
        bwsel = float(bandwidth * 8 * dec0_actual * dec1_actual) / adc_freq
        lock_bwsel = float(lock_bandwidth * 8 * dec0_actual * dec1_actual) / adc_freq

        if (lock_bwsel < min_bwsel) and ((afc_run_mode == model.vars.afc_run_mode.var_enum.ONE_SHOT) or softmodem_narrowing):
            lock_bwsel = min_bwsel

        #Load local variables back into model variables
        model.vars.bwsel.value = bwsel
        model.vars.lock_bwsel.value = lock_bwsel


    def calc_chfilt_reg(self,model):
        #This function calculates the channel filter registers

        #Load model variables into local variables
        bwsel = model.vars.bwsel.value

        coeffs = self.return_coeffs(bwsel)

        bit_widths = [10,10,10,11,11,11,12,12,12,14,14,14,16,16,16]

        # replace negative numbers with 2s complement
        for i in range(15):
            if coeffs[i] < 0:
                coeffs[i] = coeffs[i] + 2**bit_widths[i]

        # Write registers
        self._reg_write(model.vars.MODEM_CHFCOE00_SET0COEFF0,  coeffs[0])
        self._reg_write(model.vars.MODEM_CHFCOE00_SET0COEFF1,  coeffs[1])
        self._reg_write(model.vars.MODEM_CHFCOE00_SET0COEFF2,  coeffs[2])
        self._reg_write(model.vars.MODEM_CHFCOE01_SET0COEFF3,  coeffs[3])
        self._reg_write(model.vars.MODEM_CHFCOE01_SET0COEFF4,  coeffs[4])
        self._reg_write(model.vars.MODEM_CHFCOE02_SET0COEFF5,  coeffs[5])
        self._reg_write(model.vars.MODEM_CHFCOE02_SET0COEFF6,  coeffs[6])
        self._reg_write(model.vars.MODEM_CHFCOE03_SET0COEFF7,  coeffs[7])
        self._reg_write(model.vars.MODEM_CHFCOE03_SET0COEFF8,  coeffs[8])
        self._reg_write(model.vars.MODEM_CHFCOE04_SET0COEFF9,  coeffs[9])
        self._reg_write(model.vars.MODEM_CHFCOE04_SET0COEFF10, coeffs[10])
        self._reg_write(model.vars.MODEM_CHFCOE05_SET0COEFF11, coeffs[11])
        self._reg_write(model.vars.MODEM_CHFCOE05_SET0COEFF12, coeffs[12])
        self._reg_write(model.vars.MODEM_CHFCOE06_SET0COEFF13, coeffs[13])
        self._reg_write(model.vars.MODEM_CHFCOE06_SET0COEFF14, coeffs[14])

        # Load model variables into local variables
        bwsel = model.vars.lock_bwsel.value
        coeffs = self.return_coeffs(bwsel)


        # replace negative numbers with 2s complement
        for i in range(15):
            if coeffs[i] < 0:
                coeffs[i] = coeffs[i] + 2**bit_widths[i]

        # TODO: calculate the second set separately
        self._reg_write(model.vars.MODEM_CHFCOE10_SET1COEFF0,  coeffs[0])
        self._reg_write(model.vars.MODEM_CHFCOE10_SET1COEFF1,  coeffs[1])
        self._reg_write(model.vars.MODEM_CHFCOE10_SET1COEFF2,  coeffs[2])
        self._reg_write(model.vars.MODEM_CHFCOE11_SET1COEFF3,  coeffs[3])
        self._reg_write(model.vars.MODEM_CHFCOE11_SET1COEFF4,  coeffs[4])
        self._reg_write(model.vars.MODEM_CHFCOE12_SET1COEFF5,  coeffs[5])
        self._reg_write(model.vars.MODEM_CHFCOE12_SET1COEFF6,  coeffs[6])
        self._reg_write(model.vars.MODEM_CHFCOE13_SET1COEFF7,  coeffs[7])
        self._reg_write(model.vars.MODEM_CHFCOE13_SET1COEFF8,  coeffs[8])
        self._reg_write(model.vars.MODEM_CHFCOE14_SET1COEFF9,  coeffs[9])
        self._reg_write(model.vars.MODEM_CHFCOE14_SET1COEFF10, coeffs[10])
        self._reg_write(model.vars.MODEM_CHFCOE15_SET1COEFF11, coeffs[11])
        self._reg_write(model.vars.MODEM_CHFCOE15_SET1COEFF12, coeffs[12])
        self._reg_write(model.vars.MODEM_CHFCOE16_SET1COEFF13, coeffs[13])
        self._reg_write(model.vars.MODEM_CHFCOE16_SET1COEFF14, coeffs[14])

    def return_coeffs(self, bwsel):
        # this table is generated with srw_model/models/channel_filters/gen_channel_filter_coeffs.m
        if bwsel < 0.155:
            coeffs = [6, -11, -71, -199, -398, -622, -771, -700, -268, 602, 1869, 3351, 4757, 5769, 6138]
        elif bwsel < 0.165:
            coeffs = [16, 19, -12, -117, -318, -588, -828, -871, -530, 321, 1668, 3318, 4929, 6108, 6540]
        elif bwsel < 0.175:
            coeffs = [22, 45, 48, -18, -200, -495, -814, -976, -750, 44, 1433, 3224, 5025, 6365, 6861]
        elif bwsel < 0.185:
            coeffs = [26, 66, 102, 77, -75, -379, -765, -1043, -942, -222, 1189, 3104, 5087, 6586, 7145]
        elif bwsel < 0.195:
            coeffs = [30, 87, 155, 169, 46, -268, -720, -1112, -1134, -488, 947, 2988, 5154, 6813, 7436]
        elif bwsel < 0.205:
            coeffs = [30, 100, 200, 268, 201, -88, -581, -1088, -1264, -744, 660, 2792, 5128, 6951, 7639]
        elif bwsel < 0.215:
            coeffs = [29, 107, 239, 372, 391, 176, -304, -887, -1219, -877, 400, 2502, 4894, 6799, 7525]
        elif bwsel < 0.225:
            coeffs = [12, 77, 206, 373, 464, 325, -126, -775, -1256, -1075, 127, 2297, 4874, 6971, 7778]
        elif bwsel < 0.235:
            coeffs = [-1, 42, 160, 348, 510, 467, 72, -619, -1250, -1255, -159, 2066, 4836, 7146, 8045]
        elif bwsel < 0.245:
            coeffs = [-14, -5, 74, 257, 485, 569, 290, -397, -1178, -1416, -479, 1791, 4803, 7395, 8417]
        elif bwsel < 0.255:
            coeffs = [-22, -38, 8, 179, 448, 631, 455, -209, -1093, -1515, -718, 1570, 4764, 7576, 8697]
        elif bwsel < 0.265:
            coeffs = [-29, -72, -69, 73, 372, 658, 611, 3, -968, -1585, -954, 1337, 4722, 7779, 9010]
        elif bwsel < 0.275:
            coeffs = [-34, -104, -155, -65, 237, 623, 733, 229, -797, -1614, -1178, 1099, 4690, 8026, 9385]
        elif bwsel < 0.285:
            coeffs = [-39, -138, -255, -244, 35, 510, 800, 440, -610, -1628, -1403, 865, 4710, 8388, 9905]
        elif bwsel < 0.295:
            coeffs = [-30, -141, -307, -386, -170, 352, 801, 593, -478, -1716, -1722, 569, 4774, 8920, 10651]
        elif bwsel < 0.305:
            coeffs = [-14, -107, -294, -452, -329, 190, 787, 783, -223, -1644, -1940, 240, 4662, 9179, 11093]
        elif bwsel < 0.315:
            coeffs = [2, -61, -243, -466, -461, 1, 707, 925, 41, -1517, -2112, -87, 4513, 9395, 11493]
        elif bwsel < 0.325:
            coeffs = [17, -8, -163, -434, -556, -194, 576, 1013, 297, -1351, -2240, -407, 4338, 9570, 11851]
        elif bwsel < 0.335:
            coeffs = [30, 50, -55, -340, -586, -370, 398, 1031, 526, -1149, -2313, -707, 4128, 9676, 12132]
        elif bwsel < 0.345:
            coeffs = [41, 107, 69, -209, -573, -536, 183, 997, 733, -930, -2363, -1012, 3898, 9772, 12412]
        elif bwsel < 0.355:
            coeffs = [50, 163, 211, -17, -468, -629, -28, 904, 882, -723, -2397, -1316, 3631, 9808, 12627]
        elif bwsel < 0.365:
            coeffs = [45, 189, 329, 211, -245, -578, -161, 812, 1047, -385, -2220, -1498, 3232, 9442, 12324]
        elif bwsel < 0.375:
            coeffs = [29, 163, 345, 312, -126, -588, -327, 684, 1153, -145, -2161, -1728, 2996, 9496, 12562]
        elif bwsel < 0.385:
            coeffs = [11, 120, 335, 408, 33, -549, -498, 501, 1231, 133, -2059, -1974, 2716, 9556, 12843]
        elif bwsel < 0.395:
            coeffs = [-7, 65, 286, 460, 189, -462, -623, 306, 1259, 386, -1936, -2188, 2455, 9627, 13136]
        elif bwsel < 0.405:
            coeffs = [-26, -7, 191, 461, 340, -323, -704, 94, 1243, 637, -1784, -2395, 2186, 9727, 13490]
        elif bwsel < 0.415:
            coeffs = [-43, -80, 70, 410, 459, -156, -727, -99, 1197, 860, -1620, -2573, 1931, 9832, 13847]
        elif bwsel < 0.425:
            coeffs = [-61, -173, -113, 268, 526, 43, -691, -284, 1131, 1112, -1394, -2734, 1658, 9961, 14268]
        elif bwsel < 0.435:
            coeffs = [-68, -249, -307, 50, 473, 131, -735, -535, 1017, 1322, -1264, -3025, 1435, 10497, 15288]
        elif bwsel < 0.445:
            coeffs = [-50, -239, -383, -89, 458, 306, -645, -729, 838, 1505, -1001, -3166, 1111, 10603, 15732]
        elif bwsel < 0.455:
            coeffs = [-29, -203, -420, -229, 390, 451, -508, -877, 629, 1637, -729, -3264, 795, 10676, 16131]
        elif bwsel < 0.465:
            coeffs = [-2, -137, -413, -369, 263, 567, -322, -988, 378, 1731, -421, -3331, 448, 10717, 16524]
        elif bwsel < 0.475:
            coeffs = [25, -49, -341, -454, 109, 609, -136, -1028, 141, 1761, -141, -3344, 140, 10696, 16796]
        elif bwsel < 0.485:
            coeffs = [52, 61, -210, -493, -81, 587, 55, -1027, -114, 1747, 156, -3322, -188, 10638, 17045]
        elif bwsel < 0.495:
            coeffs = [83, 196, -7, -446, -254, 508, 211, -1017, -391, 1677, 430, -3295, -526, 10568, 17294]
        else:
            coeffs = [85, 274, 199, -265, -276, 473, 408, -853, -554, 1550, 686, -3080, -779, 10093, 16843]

        # Confirm Sum of the Magnitudes is in spec to not overflow the
        # filter accumulator
        try:
            assert sum([abs(i) for i in coeffs]) < 2**16
        except AssertionError:
            raise CalculationException('ERROR: Channel Filter Coefficients Sum of Magnitudes >= 2^16')

        return coeffs

    def calc_bw_carson(self,model):
        #This function calculates the Carson bandwidth (minimum bandwidth)

        #Load model variables into local variables
        baudrate = model.vars.baudrate.value
        deviation = model.vars.deviation.value
        mod_type = model.vars.modulation_type.value

        #Calculate the Carson bandwidth
        if (mod_type == model.vars.modulation_type.var_enum.FSK4):
            #Assumes deviation = inner symbol deviation
            bw_carson = baudrate + 6*deviation
        else:
            bw_carson = baudrate + 2*deviation

        #Load local variables back into model variables
        model.vars.bandwidth_carson_hz.value = int(bw_carson)

    def calc_rx_tx_ppm(self,model):
        #This function calculates the default RX and TX HFXO tolerance in PPM

        #Set defaults
        rx_ppm = 0
        tx_ppm = 0

        #Load local variables back into model variables
        model.vars.rx_xtal_error_ppm.value = rx_ppm
        model.vars.tx_xtal_error_ppm.value = tx_ppm


    def get_alpha(self, model):
        # Bandwidth adjustment based on mi and bt
        # the thresholds were derived based simulating bandwidth of modulated signal with 98% of the energy
        mi = model.vars.modulation_index.value

        sf = model.vars.shaping_filter.value

        if sf == model.vars.shaping_filter.var_enum.NONE.value:
            if mi < 0.75:
                alpha = 0.1
            elif mi < 0.85:
                alpha = 0
            elif mi < 1.5:
                alpha = -0.1
            else:
                alpha = -0.2
        elif sf == model.vars.shaping_filter.var_enum.Gaussian.value:

            bt = model.vars.shaping_filter_param.value # BT might not be defined if not Gaussian shaping so read it here

            if bt < 0.75:
                if mi < 0.95:
                    alpha = 0.2
                elif mi < 1.5:
                    alpha = 0.1
                elif mi < 6.5:
                    alpha = 0
                else:
                    alpha = -0.1
            elif bt < 1.5:
                if mi < 0.85:
                    alpha = 0.1
                elif mi < 1.5:
                    alpha = 0
                else:
                    alpha = -0.1
            elif bt < 2.5:
                if mi < 0.75:
                    alpha = 0.1
                elif mi < 0.85:
                    alpha = 0
                else:
                    alpha = -0.1
        else:
            # for non Gaussian shaping keeping the original alpha calculation
            if (mi < 1.0):
                alpha = 0.2
            elif (mi == 1.0):
                alpha = 0.1
            else:
                alpha = 0

        return alpha

    def calc_target_bandwidth(self, model):
        #This function calculates the target bandwidth in case the user didn't enter one
        #This is the acquisition bandwidth

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        bw_carson = model.vars.bandwidth_carson_hz.value
        baudrate = model.vars.baudrate.value
        freq_offset_hz = model.vars.freq_offset_hz.value

        #Calculate bw_demod and bw_acq
        #bw_demod is the target demod bandwidth before adding frequency shift
        #bw_acq combines bw_demod and frequency shift
        if (mod_type == model.vars.modulation_type.var_enum.FSK2) or \
                (mod_type == model.vars.modulation_type.var_enum.MSK):
            alpha = self.get_alpha(model)
            bw_acq = bw_carson + 2 * max( 0.0, freq_offset_hz - alpha * bw_carson )

        elif (mod_type == model.vars.modulation_type.var_enum.FSK4):
            bw_acq = bw_carson + 2.0 * freq_offset_hz

        else:
            #Default values for other modulation types
            if (mod_type == model.vars.modulation_type.var_enum.OOK) or \
                    (mod_type == model.vars.modulation_type.var_enum.ASK):
                bw_modulation = baudrate * 5.0
                if (model.vars.bandwidth_hz._value_forced == None):
                    print("  WARNING:  OOKASK bandwidth_hz has not been optimized")
            elif (mod_type == model.vars.modulation_type.var_enum.OQPSK):
                bw_modulation = baudrate * 1.25
            else:
                bw_modulation = baudrate * 1.0

            bw_acq = bw_modulation + 2.0 * freq_offset_hz

        #Set max limit on bandwidth_hz
        bw_acq = min(bw_acq, 2.5e6)

        if model.vars.bandwidth_hz.value_forced:
            if model.vars.bandwidth_hz.value > 1.2 * bw_acq:
                LogMgr.Warning("WARNING: Programmed acquisition channel bandwidth is much higher than calculated")

        #Load local variables back into model variables
        model.vars.bandwidth_hz.value = int(bw_acq)


    def calc_lock_bandwidth(self, model, softmodem_narrowing=False):
        #Load model variables into local variables
        bw_acq = model.vars.bandwidth_hz.value
        bw_demod = model.vars.demod_bandwidth_hz.value
        afc_run_mode = model.vars.afc_run_mode.value
        rtschmode = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value
        antdivmode = model.vars.antdivmode.value

        if (model.vars.demod_bandwidth_hz._value_forced != None):
            # Prioritize forced value
            lock_bandwidth_hz = bw_demod
        elif (afc_run_mode == model.vars.afc_run_mode.var_enum.ONE_SHOT) or softmodem_narrowing:
            # for calculated bw_demod, upper limit: lock_bandwidth_hz <= bandwidth_hz
            lock_bandwidth_hz = min(bw_demod, bw_acq)
        elif (afc_run_mode == model.vars.afc_run_mode.var_enum.CONTINUOUS) and rtschmode == 1:
            if antdivmode != model.vars.antdivmode.var_enum.DISABLE:
                lock_bandwidth_hz = min(bw_demod, bw_acq)
            else:
                lock_bandwidth_hz = bw_acq
        else:
            # for calculated bw_demod, if AFC is disabled, set lock_bandwidth_hz = bandwidth_hz
            lock_bandwidth_hz = bw_acq

        model.vars.lock_bandwidth_hz.value = int(lock_bandwidth_hz)


    def calc_bandwidth_actual(self,model):
        #This function calculates the actual channel bandwidth based on adc rate, decimator, and bwsel settings

        #Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        bwsel = model.vars.bwsel.value

        #Calculate the actual channel bandwidth
        bandwidth_actual = int(adc_freq_actual * bwsel / dec0_actual / dec1_actual / 8)

        #Load local variables back into model variables
        model.vars.bandwidth_actual.value = bandwidth_actual

    def calc_datafilter(self,model):
        #This function calculates the number of datafilter taps

        #Load model variables into local variables
        osr = model.vars.oversampling_rate_actual.value
        demod_sel = model.vars.demod_select.value
        modformat = model.vars.modulation_type.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remoddwn = model.vars.MODEM_PHDMODCTRL_REMODDWN.value + 1
        trecs_enabled = model.vars.trecs_enabled.value
        cplx_corr_enabled = model.vars.MODEM_CTRL6_CPLXCORREN.value == 1

        if demod_sel==model.vars.demod_select.var_enum.COHERENT and \
                modformat == model.vars.modulation_type.var_enum.OQPSK:
            # : For Cohererent demod, set data filter taps to 9
            # : TODO for complex correlation enabled, set datafilter taps to 6
            if cplx_corr_enabled:
                datafilter_taps = 6
            else:
                datafilter_taps = 9
        # no data filter in path when TRecS is enabled
        elif demod_sel==model.vars.demod_select.var_enum.BCR or \
                modformat == model.vars.modulation_type.var_enum.OQPSK or \
                (trecs_enabled and not remoden) or remoddwn > 1:
            datafilter_taps = 2 # 2 here translates to datafilter_reg = 0 meaning disabled datafilter
        #Calculate datafitler based on OSR
        elif (osr > 1) and (osr < 10):
            datafilter_taps = int(round(osr))
        else:
            raise CalculationException('ERROR: OSR out of range in calc_datafilter()')

        #Load local variables back into model variables
        model.vars.datafilter_taps.value = datafilter_taps

    def calc_datafilter_reg(self,model):
        #This function writes the datafilter register

        #Load model variables into local variables
        datafilter_taps = model.vars.datafilter_taps.value

        #The datafilter register setting is 2 less than the number of taps
        datafilter_reg = datafilter_taps - 2

        if datafilter_reg < 0:
            datafilter_reg = 0

        # Write register
        self._reg_write(model.vars.MODEM_CTRL2_DATAFILTER, datafilter_reg)

    def calc_datafilter_actual(self,model):
        #This function calculates the actual datafilter taps from the register value

        #Load model variables into local variables
        datafilter_reg = model.vars.MODEM_CTRL2_DATAFILTER.value

        #The number of taps is the register value plus 2
        datafilter_taps_actual = datafilter_reg

        #Load local variables back into model variables
        model.vars.datafilter_taps_actual = datafilter_taps_actual

    def calc_digmix_res_actual(self,model):
        #This function calculates the digital mixer register

        #Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        # digital mixer frequency resolution, Hz/mixer ticks
        digmix_res = adc_freq_actual/((2**20) * 8.0 * dec0_actual)
        model.vars.digmix_res_actual.value = digmix_res

    def calc_digmixfreq_val(self,model):
        digmix_res = model.vars.digmix_res_actual.value
        fif = model.vars.if_frequency_hz_actual.value  # IF frequency based on the actual SYNTH settings
        model.vars.digmixfreq.value = int(round(fif/digmix_res))


    def calc_digmixfreq_reg(self,model):
        #This function calculates the digital mixer register
        digmixfreq_reg = model.vars.digmixfreq.value

        # Write register
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXFREQ, digmixfreq_reg)

    def calc_digmixfreq_actual(self,model):
        #This function calculates the actual digital mixer frequency based on the register

        #Load model variables into local variables
        digmixfreq_reg = model.vars.MODEM_DIGMIXCTRL_DIGMIXFREQ.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value

        #Calculate the actual mixer frequency
        digmixfreq_actual = int(digmixfreq_reg * model.vars.digmix_res_actual.value)

        #Load local variables back into model variables
        model.vars.digmixfreq_actual.value = digmixfreq_actual

    def calc_devoffcomp_reg(self,model):
        #This function calculates the register value of devoffcomp

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value

        if(mod_type==model.vars.modulation_type.var_enum.FSK4):
            devoffcomp=1
        else:
            devoffcomp=0

        #Write register
        self._reg_write(model.vars.MODEM_CTRL4_DEVOFFCOMP, devoffcomp)


    def calc_demod_rate_actual(self,model):
        #This function calculates the actual sample rate at the demod

        # Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2_actual = model.vars.src2_ratio_actual.value

        demod_rate_actual = int(adc_freq_actual * src2_actual / (8 * dec0_actual * dec1_actual * dec2_actual))

        #Load local variables back into model variables
        model.vars.demod_rate_actual.value = demod_rate_actual

    def calc_rx_deviation_scaled(self,model):
        #This function calculates the scaled RX deviation

        #Load model variables into local variables
        deviation = model.vars.deviation.value
        freq_gain_actual = model.vars.freq_gain_actual.value
        demod_rate_actual = model.vars.demod_rate_actual.value

        rx_deviation_scaled = float(256*deviation*freq_gain_actual/demod_rate_actual)

        #Load local variables back into model variables
        model.vars.rx_deviation_scaled.value = rx_deviation_scaled

    def calc_devweightdis_reg(self,model):
        #This function calculates the register value of devweightdis

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        rx_deviation_scaled = model.vars.rx_deviation_scaled.value

        if(mod_type == model.vars.modulation_type.var_enum.FSK2) or \
          (mod_type == model.vars.modulation_type.var_enum.MSK):
            if(abs(rx_deviation_scaled-64) > 6):
                devweightdis = 1
            else:
                devweightdis = 0
        else:
            devweightdis = 0

        #Write register
        self._reg_write(model.vars.MODEM_CTRL2_DEVWEIGHTDIS, devweightdis)

    def calc_freq_gain_target(self,model):
        #This function calculates the target frequency gain value

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        demod_rate_actual = model.vars.demod_rate_actual.value
        deviation = model.vars.deviation.value
        freq_offset_hz = model.vars.freq_offset_hz.value
        large_tol = (freq_offset_hz > deviation)
        afconeshot = model.vars.MODEM_AFC_AFCONESHOT.value
        trecs_remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value


        if (mod_type == model.vars.modulation_type.var_enum.FSK2 or \
            mod_type == model.vars.modulation_type.var_enum.MSK) and deviation > 0:
            if large_tol and (not afconeshot) and trecs_remoden:
                freq_gain_target = demod_rate_actual / (4.0 * (deviation + freq_offset_hz*0.75) / 2.0)
            else:
                freq_gain_target = demod_rate_actual / (4.0 * (deviation + freq_offset_hz) / 2.0)
        elif (mod_type == model.vars.modulation_type.var_enum.FSK4) and deviation > 0:
            freq_gain_target = demod_rate_actual / (4.0 * (3.0 * deviation + freq_offset_hz) / 2.0)
        else:
            freq_gain_target = 0.0

        #Load local variables back into model variables
        model.vars.freq_gain.value = freq_gain_target

    def calc_freq_gain_reg(self,model):
        #This function calculates the frequency gain registers

        #Load model variables into local variables
        freq_gain_target = model.vars.freq_gain.value

        best_error = 1e9
        bestM=0
        bestE=0

        for M in range(1,8):
            for E in range(0,8):
                calculated_gain = M*2**(2-E)
                error = abs(freq_gain_target - calculated_gain)
                if error < best_error:
                    best_error = error
                    bestM = M
                    bestE = E

        #Write registers
        self._reg_write(model.vars.MODEM_MODINDEX_FREQGAINM, bestM)
        self._reg_write(model.vars.MODEM_MODINDEX_FREQGAINE, bestE)

    def calc_freq_gain_actual(self,model):
        #This function calculates the actual frequency gain from the register values

        #Load model variables into local variables
        M_actual = model.vars.MODEM_MODINDEX_FREQGAINM.value
        E_actual = model.vars.MODEM_MODINDEX_FREQGAINE.value

        freq_gain_actual = M_actual * float(2**(2-E_actual))

        #Load local variables back into model variables
        model.vars.freq_gain_actual.value = freq_gain_actual

###Since we are not inheriting the CALC_Demodulator_Ocelot class from prior parts, the functions below are copied in order to maintain functionality###
#TODO: Go through these and decide if they still apply for Ocelot

    def calc_interpolation_gain_actual(self, model):
        #This function calculates the actual interpolation gain

        #Load model variables into local variables
        txbrnum = model.vars.MODEM_TXBR_TXBRNUM.value
        modformat = model.vars.modulation_type.value

        if txbrnum < 256:
            interpolation_gain = txbrnum / 1.0
        elif modformat == model.vars.modulation_type.var_enum.BPSK or \
             modformat == model.vars.modulation_type.var_enum.DBPSK:
            interpolation_gain = 16 * txbrnum * 2 ** (3-floor(log(txbrnum, 2)))
        elif txbrnum < 512:
            interpolation_gain = txbrnum / 2.0
        elif txbrnum < 1024:
            interpolation_gain = txbrnum / 4.0
        elif txbrnum < 2048:
            interpolation_gain = txbrnum / 8.0
        elif txbrnum < 4096:
            interpolation_gain = txbrnum / 16.0
        elif txbrnum < 8192:
            interpolation_gain = txbrnum / 32.0
        elif txbrnum < 16384:
            interpolation_gain = txbrnum / 64.0
        else:
            interpolation_gain = txbrnum / 128.0

        # calculate phase interpolation gain for OQPSK cases
        if modformat == model.vars.modulation_type.var_enum.OQPSK:
            interpolation_gain = 2 ** (ceil(log(interpolation_gain, 2)))

        #Load local variables back into model variables
        model.vars.interpolation_gain_actual.value = float(interpolation_gain)

    def calc_mod_type_actual(self, model):
        #This function calculates the actual modulation type based on the register value

        #Load model variables into local variables
        mod = model.vars.MODEM_CTRL0_MODFORMAT.value

        if mod == 0:
            modformat = '2-FSK'
        elif mod == 1:
            modformat = '4-FSK'
        elif mod == 2:
            modformat = 'BPSK'
        elif mod == 3:
            modformat = 'DBPSK'
        elif mod == 4:
            modformat = 'OQPSK'
        elif mod == 5:
            modformat = 'MSK'
        elif mod == 6:
            modformat = 'OOKASK'

        #Load local variables back into model variables
        model.vars.mod_format_actual.value = modformat

    def calc_mod_type_reg(self, model):
        #This function writes the modulation type register

        #Load model variables into local variables
        modformat = model.vars.modulation_type.value

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
                modformat == model.vars.modulation_type.var_enum.MSK:
            mod = 0
        elif modformat == model.vars.modulation_type.var_enum.FSK4:
            mod = 1
        elif modformat == model.vars.modulation_type.var_enum.BPSK:
            mod = 2
        elif modformat == model.vars.modulation_type.var_enum.DBPSK:
            mod = 3
        elif modformat == model.vars.modulation_type.var_enum.OQPSK:
            mod = 4
        elif modformat == model.vars.modulation_type.var_enum.OOK or \
                modformat == model.vars.modulation_type.var_enum.ASK:
            mod = 6
        else:
            raise CalculationException('ERROR: modulation method in input file not recognized')

        #Write register
        self._reg_write(model.vars.MODEM_CTRL0_MODFORMAT, mod)

    def calc_resyncper_brcal_val(self, model):
        #This function calculates the resynchronization and baud rate calibration values

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        osr = model.vars.oversampling_rate_actual.value
        symbols_in_timing_window = model.vars.symbols_in_timing_window.value
        baudrate_tol_ppm = model.vars.baudrate_tol_ppm.value
        syncword_length = model.vars.syncword_length.value

        if symbols_in_timing_window > 0:
            timing_wind_size = symbols_in_timing_window
        else:
            timing_wind_size = syncword_length

        #Estimate the baudrate tol with resyncper=2
        estimated_baudrate_tol_ppm = int(1.0/(2*timing_wind_size*osr)*1e6/2) #Divide by 2 is to be conservative

        #Use a resynchronization period of 2 if we don't need much baudrate tolerance, otherwise use 1
        if estimated_baudrate_tol_ppm >= baudrate_tol_ppm:
            resyncper = 2
        else:
            resyncper = 1

        #Baudrate calibration does not work well with the Legacy demod, so disable
        brcalavg = 0
        brcalen = 0

        #Load local variables back into model variables
        model.vars.brcalavg.value = brcalavg
        model.vars.brcalen.value = brcalen
        model.vars.timing_resync_period.value = resyncper

    def calc_brcalmode_reg(self, model):
        #This function writes the brcal model register

        #Write register
        self._reg_write(model.vars.MODEM_CTRL5_BRCALMODE, 0)

    def calc_brcal_reg(self, model):
        #This function writes the brcal average and enable registers

        #Load model variables into local variables
        brcalavg = model.vars.brcalavg.value
        brcalen = model.vars.brcalen.value

        #Write registers
        self._reg_write(model.vars.MODEM_CTRL5_BRCALAVG, brcalavg)
        self._reg_write(model.vars.MODEM_CTRL5_BRCALEN, brcalen)

    def calc_resyncbaudtrans_reg(self, model):
        #This function writes the resyncbaudtrans register
        demod_select = model.vars.demod_select.value

        # : for coherent demod, disable otherwise the measured baudrate tolerance is effectively 0
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            self._reg_write(model.vars.MODEM_CTRL5_RESYNCBAUDTRANS, 0)
        else:
            #Based on Series 1 findings, always set RESYNCBAUDTRANS for all other demods
            self._reg_write(model.vars.MODEM_CTRL5_RESYNCBAUDTRANS, 1)

    def calc_rsyncper_reg(self, model):
        #This function writes the resyncper register

        #Load model variables into local variables
        timing_resync_period = model.vars.timing_resync_period.value

        #Write register
        self._reg_write(model.vars.MODEM_CTRL1_RESYNCPER, timing_resync_period)

    def calc_resyncper_actual(self, model):
        #This function calculates the actual resynchonization period based on the register value

        #Load model variables into local variables
        resyncper_actual = float(model.vars.MODEM_CTRL1_RESYNCPER.value)

        #Load local variables back into model variables
        model.vars.resyncper_actual.value = resyncper_actual

    def calc_phasedemod_reg(self, model):
        #This function writes the phase demod register

        #Load model variables into local variables
        length = model.vars.dsss_len.value
        modulation = model.vars.modulation_type.value
        demod_sel = model.vars.demod_select.value

        if modulation == model.vars.modulation_type.var_enum.OQPSK:
            if demod_sel == model.vars.demod_select.var_enum.COHERENT:
                phasedemod = 2
            else:
                phasedemod = 1
        elif modulation == model.vars.modulation_type.var_enum.BPSK or \
            modulation == model.vars.modulation_type.var_enum.DBPSK:
            if length > 0:
                phasedemod = 2
            else:
                phasedemod = 1
        else:
            phasedemod = 0

        #Load local variables back into model variables
        self._reg_write(model.vars.MODEM_CTRL1_PHASEDEMOD, phasedemod)

    def calc_dsa_enable(self,model):
        #This function sets a value for dsa_enable

        dsa_enable = False

        #Write the model variable
        model.vars.dsa_enable.value = dsa_enable

    # limit accumulated baudrate offset over timing window to 30000 ppm or 3%
    # used in calc_resynper_brcal_val and calc_baudrate_tol_ppm_actual
    # TODO: might need to tweak this number based on PHY performance
    max_accumulated_tolerance_ppm = 30000.0

    def calc_intosr_reg(self, model):
        # This function sets INTOSR register field
        osr = model.vars.oversampling_rate_actual.value

        if abs(round(osr) - osr) < 0.001:
            intosr = 1
        else:
            intosr = 0

        self._reg_write(model.vars.MODEM_SRCCHF_INTOSR, intosr)

    def calc_isicomp_reg(self, model):
        # This function calculates the ISICOMP register field

        # Read in global variables
        modulation = model.vars.modulation_type.value
        shaping_filter = model.vars.shaping_filter.value

        #Calculate the ISICOMP value based on filter type and BT
        if modulation == model.vars.modulation_type.var_enum.FSK4:

            if shaping_filter == model.vars.shaping_filter.var_enum.Gaussian:
                # Currently we only consider Gaussian shaping, support for other filter types with 4FSK and ISICOMP is TBD
                # Read in shaping filter param here as some PHYs do not have shaping filter defined if filter is NONE
                shaping_filter_param = model.vars.shaping_filter_param.value

                if shaping_filter_param >= 0.75:
                    isicomp = 5
                elif shaping_filter_param >= 0.6:
                    isicomp = 8
                else:
                    #This is the default BT=0.5 case
                    isicomp = 10
            else:
                #Not gaussian filtering
                isicomp = 8
        else:
            #Do not use ISI compensation for other modulation types
            isicomp = 0

        #Write the register
        self._reg_write(model.vars.MODEM_CTRL4_ISICOMP, isicomp)

    def calc_offsetphasemasking_reg(self, model):
        # This function calculates OFFSETPHASEMASKING

        modulation = model.vars.modulation_type.value

        if modulation == model.vars.modulation_type.var_enum.BPSK or \
           modulation == model.vars.modulation_type.var_enum.DBPSK:
            self._reg_write(model.vars.MODEM_CTRL4_OFFSETPHASEMASKING, 1)
        else:
            self._reg_write(model.vars.MODEM_CTRL4_OFFSETPHASEMASKING, 0)

    def calc_dec1gain_value(self, model):
        """calculate additional gain we want in the DEC1 decimator for very low bandwidth
        PHY settings.
        see register definition of DEC1GAIN in EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        bw = model.vars.bandwidth_actual.value

        if bw < 500:
            dec1gain = 12
        elif bw < 2000:
            dec1gain = 6
        else:
            dec1gain = 0

        model.vars.dec1gain.value = dec1gain


    def calc_dec1gain_reg(self, model):
        """set DEC1GAIN register based on calculated value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = model.vars.dec1gain.value

        if val == 12:
            reg = 2
        elif val == 6:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_CF_DEC1GAIN,  reg)

    def calc_syncacqwin_actual(self, model):
        """ set syc word acquisition window for TRECS basd on register value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        model.vars.syncacqwin_actual.value = 4 * (model.vars.MODEM_REALTIMCFE_SYNCACQWIN.value + 1)

    def calc_phscale_reg(self, model):
        #Load model variables into local variables
        mi = model.vars.modulation_index.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        demod_sel = model.vars.demod_select.value
        osr = model.vars.oversampling_rate_actual.value
        phscale_derate_factor = model.vars.phscale_derate_factor.value

        if remoden:
            # if remodulation path is enabled freqgain block is handling the scaling
            phscale_reg = 0
        elif mi > 0.0:
            if demod_sel == model.vars.demod_select.var_enum.BCR:
                # phscale_reg = int(floor(log(8 * 4 * mi / osr, 2)))
                bcr_phscale_list = [0,1,2,3]
                bcrksi3_list = []
                diff_from_opt_bcrksi3_list = []
                for bcr_phscale_val in bcr_phscale_list:
                    bcr_phscale_val_actual = float(2 ** (bcr_phscale_val))
                    ksi1_val = self.return_ksi1_calc(model, bcr_phscale_val_actual)
                    ksi2_val, ksi3_val, ksi3wb_val = self.return_ksi2_ksi3_calc(model, ksi1_val)
                    bcrksi3_list.append(ksi3wb_val)
                    diff_from_opt_bcrksi3_list.append(40 - ksi3wb_val)

                # : Determine lowest phscale value with bcrksi3 < 64
                phscale_reg = -1
                for diff_index in range(len(diff_from_opt_bcrksi3_list)):
                    if diff_from_opt_bcrksi3_list[diff_index] >= 0:
                        phscale_reg = bcr_phscale_list[diff_index]
                        break

                # : If fail, calculate following est osr disable case
                if phscale_reg == -1:
                    phscale_reg = int(floor(log(8 * 4 * mi / osr, 2)))
            else:
                # this scaling will bring the nominal soft decision as close to 64 as possible with a power of 2 scaling
                phscale_reg = int(round(log(2 * mi, 2)))
        else:
            phscale_reg = 0

        #Derate phscale per phscale_derate_factor (used to accomodate large freq offset tol)
        phscale_reg += int(round(log2(phscale_derate_factor)))

        # limit phscale_reg from 0 to 3
        phscale_reg = max(min(phscale_reg, 3), 0)

        self._reg_write(model.vars.MODEM_TRECPMDET_PHSCALE, phscale_reg)

    def calc_phscale_actual(self,model):
        phscale_reg = model.vars.MODEM_TRECPMDET_PHSCALE.value
        model.vars.phscale_actual.value = float(2 ** (phscale_reg))

    def return_ksi1_calc(self, model, phscale):
        # Load model variables into local variables
        demod_sel = model.vars.demod_select.value
        modtype = model.vars.modulation_type.value
        trecs_enabled = model.vars.trecs_enabled.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        freq_gain_actual = model.vars.freq_gain_actual.value
        osr = model.vars.oversampling_rate_actual.value
        baudrate = model.vars.baudrate.value
        freq_dev_max = model.vars.freq_dev_max.value
        freq_dev_min = model.vars.freq_dev_min.value

        # when remod is enabled scaling is controlled by freqgain and phscale is currently set to 1
        if remoden:
            gain = freq_gain_actual / phscale / osr
        elif demod_sel == model.vars.demod_select.var_enum.BCR:
            gain = 8 / (phscale * osr)
        else:
            gain = 1 / phscale

        #Calculate minimum and maximum possible modulation indices
        mi_min = 2.0*freq_dev_min/baudrate
        mi_max = 2.0*freq_dev_max/baudrate

        #Determine which modulation index to use for the purposes of KSI calculation
        mi_to_use = mi_min + (mi_max - mi_min) * 0.5

        # calculate ksi values for Viterbi demod only
        # if the gain is set correctly this should give us nominal soft decisions of 64 for regular case
        # in case of remod we actually use the legacy demod's gain which sets the deviation + freq offset to 128
        if ((trecs_enabled or demod_sel == model.vars.demod_select.var_enum.BCR) and
                (modtype == model.vars.modulation_type.var_enum.FSK2 or
                 modtype == model.vars.modulation_type.var_enum.MSK)):
            if demod_sel == model.vars.demod_select.var_enum.BCR:
                saturation_value = 63
            else:
                saturation_value = 127
            ksi1 = int(round(saturation_value * mi_to_use * gain))
        else:
            ksi1 = 0

        return ksi1

    def calc_ksi1(self, model):
        #This function writes the ksi1 model variable that is used to program both
        #hardmodem and softmodem ksi1 regs

        # Read in model vars
        phscale_actual = model.vars.phscale_actual.value

        # Call the calculation routine for ksi1 based on actual selected phscale
        model.vars.ksi1.value = self.return_ksi1_calc(model, phscale_actual)

    def calc_ksi1_reg(self, model):

        #Read in model vars
        ksi1 = model.vars.ksi1.value

        #Write the reg
        self._reg_sat_write(model.vars.MODEM_VITERBIDEMOD_VITERBIKSI1, ksi1)

    def calc_syncbits_actual(self, model):

        model.vars.syncbits_actual.value = model.vars.MODEM_CTRL1_SYNCBITS.value + 1


    def calc_chflatency_actual(self, model):
        chflatency = model.vars.MODEM_CHFLATENCYCTRL_CHFLATENCY.value

        model.vars.chflatency_actual.value = chflatency


    def calc_datapath_delays(self, model):

        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        datafilter_taps = model.vars.datafilter_taps.value
        chflatency = model.vars.chflatency_actual.value
        src2_actual = model.vars.src2_ratio_actual.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remoddwn = model.vars.MODEM_PHDMODCTRL_REMODDWN.value
        trecs_enabled = model.vars.trecs_enabled.value
        oversampling_rate = model.vars.oversampling_rate_actual.value

        # need to flush out the entire delay line so delay is not group delay but number of taps
        # DEC8 delay: 22 taps
        del_dec8 = 22
        # DEC0 delay: 27 or 40 taps depending on decimation
        del_dec0 = 27.0 if dec0 == 3 or dec0 == 4 else 40
        # DC cancel filter group delay of 1, IRCAL delay of 1, no delay in dig mixer
        del_dc_ircal_digmix  = 2
        # DEC1 delay: 4 additional taps per decimation as this is 4th order CIC
        del_dec1 = (dec1 - 1) * 4.0 + 1
        # CHFLT delay: 29 taps minus the 6 taps for each increment in latency reduction field
        del_chflt = 29.0 - chflatency * 6.0
        # SRC delay: can be up to 2 samples
        del_src2 = 2
        # Digital gain and CORDIC do not introduce any delays
        del_digigain = 0
        del_cordic = 0
        # Differentiation delay of 1, frequency gain has no delay
        del_diff = 1
        # DEC2 delay: 1st or CIC so number of taps is the same as decimation
        del_dec2 = dec2
        # DATAFILT delay: number of taps
        del_data = datafilter_taps
        # remod operation delay
        # FIXME: verify the delay in this block
        del_remod = remoddwn

        del_adc_to_diff = (((del_dec8 / 8 + del_dec0) / dec0 + del_dc_ircal_digmix + del_dec1) / dec1 + del_chflt + del_src2 ) / src2_actual + \
            del_digigain + del_cordic + del_diff

        grpdel_mixer_to_diff = ( (del_dec1+1)/2 / dec1 + (del_chflt+1)/2 + del_src2) / src2_actual + del_digigain + del_cordic + del_diff

        if trecs_enabled:
            if remoden == 1 and remoddwn == 0: # demod at DEC2 output
                delay_adc_to_demod =  (del_adc_to_diff + del_dec2) / dec2 # delay at dec2 output in samples at that point
                delay_adc_to_demod_symbols = (delay_adc_to_demod + del_data) / oversampling_rate / dec2
                grpdelay_to_demod = (grpdel_mixer_to_diff + (del_dec2+1)/2) / dec2  # delay at dec2 output in samples at that point
                delay_agc = delay_adc_to_demod * dec2 * src2_actual
            elif remoden == 1 and remoddwn > 1:
                delay_adc_to_demod = ((del_adc_to_diff + del_dec2) / dec2 + del_data + del_remod) / remoddwn
                delay_adc_to_demod_symbols = delay_adc_to_demod / oversampling_rate / dec2
                grpdelay_to_demod = ((grpdel_mixer_to_diff + (del_dec2+1)/2) / dec2 + (del_data+1)/2 + (del_remod+1)/2) / remoddwn
                delay_agc = delay_adc_to_demod * dec2 * src2_actual * remoddwn
            else:
                delay_adc_to_demod = del_adc_to_diff
                delay_adc_to_demod_symbols = delay_adc_to_demod / oversampling_rate
                grpdelay_to_demod = grpdel_mixer_to_diff
                delay_agc = del_adc_to_diff * src2_actual
        else:
            delay_adc_to_demod = (del_adc_to_diff + del_dec2) / dec2 + del_data
            delay_adc_to_demod_symbols = delay_adc_to_demod / oversampling_rate / dec2
            grpdelay_to_demod = (grpdel_mixer_to_diff + (del_dec2+1)/2) / dec2 + (del_data+1)/2
            delay_agc = delay_adc_to_demod * dec2 * src2_actual

        model.vars.grpdelay_to_demod.value = int(ceil(grpdelay_to_demod))
        model.vars.agc_settling_delay.value = int(ceil(delay_agc))
        model.vars.delay_adc_to_demod_symbols.value = int(ceil(delay_adc_to_demod_symbols))

    def calc_src2_denominator(self, model):
        #Needed by RAIL

        # Load model variables into local variables
        osr = model.vars.oversampling_rate_actual.value
        datarate = model.vars.baudrate.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if (model.vars.adc_clock_mode.var_enum.HFXOMULT == adc_clock_mode):
            src2_calcDenominator = 0
        else:
            src2_calcDenominator = datarate * dec0 * dec1 * dec2 * 8 * osr

        # Load local variables back into model variables
        model.vars.src2_calcDenominator.value = int(src2_calcDenominator)

    def calc_dccomp_misc_reg(self, model):

        # always enable both DC offset estimation and compensation blocks
        self._reg_write(model.vars.MODEM_DCCOMP_DCCOMPEN, 1)
        self._reg_write(model.vars.MODEM_DCCOMP_DCESTIEN, 1)

        # don't reset at every packet
        self._reg_write(model.vars.MODEM_DCCOMP_DCRSTEN, 0)

        # always enable gear shifting option
        self._reg_write(model.vars.MODEM_DCCOMP_DCGAINGEAREN, 1)

        # when AGC gain change happens set the gear to fastest
        self._reg_write(model.vars.MODEM_DCCOMP_DCGAINGEAR, 7)

        # final gear setting after settling
        self._reg_write(model.vars.MODEM_DCCOMP_DCCOMPGEAR, 3)

        # limit max DC to 1V
        self._reg_write(model.vars.MODEM_DCCOMP_DCLIMIT, 0)

        # don't freeze state of DC comp filters
        self._reg_write(model.vars.MODEM_DCCOMP_DCCOMPFREEZE, 0)

        # time between gear shifts - set to fixed value for now
        self._reg_write(model.vars.MODEM_DCCOMP_DCGAINGEARSMPS, 40)


    def calc_forceoff_reg(self, model):
        demod_sel = model.vars.demod_select.value
        trecs_enabled = model.vars.trecs_enabled.value

        if demod_sel == model.vars.demod_select.var_enum.BCR or trecs_enabled:
            clock_gate_off_reg = 0xfdff
        else:
            clock_gate_off_reg = 0x00

        self._reg_write(model.vars.MODEM_CGCLKSTOP_FORCEOFF, clock_gate_off_reg)

    def get_limits(self, demod_select, withremod, relaxsrc2, model):
        #Load model variables into local variables
        bandwidth = model.vars.bandwidth_hz.value #from calc_target_bandwidth

        baudrate = model.vars.baudrate.value #We don't know the actual bandrate yet
        modtype = model.vars.modulation_type.value
        mi = model.vars.modulation_index.value
        min_chfilt_osr = None
        max_chfilt_osr = None
        osr_list = None

        # Define constraints for osr, src2, dec2
        if demod_select == model.vars.demod_select.var_enum.BCR:
            # FIXME:  osr_list and resulting target osr are really chfilt_osr, pro2 calculator defines target_osr
            #       This doesn't cause an error but is confusing.
            osr_est = int(ceil(2 * float(bandwidth) / baudrate))

            min_osr = 8
            max_osr = 127
            min_chfilt_osr = 8
            if (modtype == model.vars.modulation_type.var_enum.OOK) or \
                    (modtype == model.vars.modulation_type.var_enum.ASK):
                max_chfilt_osr = 16256  #127*max_bcr_dec = 127*128
                osr_list = range(12, max_chfilt_osr)
            else:
                max_chfilt_osr = 127
                osr_list = [osr_est]

            min_src2 = 1.0
            max_src2 = 1.0
            min_dec2 = 1
            max_dec2 = 1
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
        elif demod_select == model.vars.demod_select.var_enum.LEGACY:
            if (modtype == model.vars.modulation_type.var_enum.FSK2 or \
                  modtype == model.vars.modulation_type.var_enum.FSK4 or \
                  modtype == model.vars.modulation_type.var_enum.MSK) and (mi<1):
                # >=7 is better for sensitivity and frequency offset
                # cost (sens degrade) increases with decreasing osr 6,5,4
                osr_list = [7, 8, 9, 6, 5, 4]
                min_osr = 4
            else:
                osr_list = [5, 7, 6, 4, 8, 9]
                min_osr = 4
            max_osr = 9
            min_src2 = 0.8
            max_src2 = 1.65 if relaxsrc2 else 1.2
            min_dec2 = 1
            max_dec2 = 64
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
        elif demod_select == model.vars.demod_select.var_enum.COHERENT:
            osr_list = [5]
            min_osr = 5
            max_osr = 5
            min_src2 = 0.8
            max_src2 = 1.65 if relaxsrc2 else 1.2
            min_dec2 = 1
            max_dec2 = 1
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
        elif demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            if relaxsrc2 == True:
                min_src2 = 0.55
                max_src2 = 1.3
            else:
                min_src2 = 0.8
                max_src2 = 1.0

            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4

            if withremod == True:
                min_dec2 = 1
                max_dec2 = 64
                min_osr = 4
                max_osr = 32
                osr_list = [4, 5, 6, 7]
            elif mi > 2.5: #FIXME: arbitrary threshold here - for zwave 9.6kbps with mi=2.1 we prefer not to use int/diff path but at some point we will have to
                min_dec2 = 1
                max_dec2 = 64
                min_osr = 4
                max_osr = 7
                osr_list = [4, 5, 6, 7]
            else:
                # Standard TRECs, no DEC2 or remod path
                min_dec2 = 1
                max_dec2 = 1
                min_osr = 4
                max_osr = 7
                osr_list = [4, 5, 6, 7]
        elif demod_select == model.vars.demod_select.var_enum.LONGRANGE:
            min_dec2 = 1
            max_dec2 = 1
            min_osr = 4
            max_osr = 4
            osr_list = [4]
            min_src2 = 0.8
            max_src2 = 1.2
            min_bwsel = 0.2
            target_bwsel = 0.3
            max_bwsel = 0.3
        else:
            raise CalculationException('ERROR: invalid demod_select in return_osr_dec0_dec1()')

        # save to use in other functions
        model.vars.min_bwsel.value = min_bwsel  # min value for normalized channel filter bandwidth
        model.vars.max_bwsel.value = max_bwsel  # max value for normalized channel filter bandwidth
        model.vars.min_src2.value = min_src2  # min value for SRC2
        model.vars.max_src2.value = max_src2  # max value for SRC2
        model.vars.max_dec2.value = max_dec2
        model.vars.min_dec2.value = min_dec2

        return min_bwsel, max_bwsel, min_chfilt_osr, max_chfilt_osr, min_src2, max_src2, min_dec2, max_dec2, min_osr, max_osr, target_bwsel, osr_list

    def calc_bandwdith_tol(self, model):
        model.vars.bandwidth_tol.value = 0.0


    def return_osr_dec0_dec1(self, model, demod_select, withremod=False, relaxsrc2=False, quitatfirstvalid=True):

        # Load model variables into local variables
        bandwidth = model.vars.bandwidth_hz.value  # from calc_target_bandwidth
        adc_freq = model.vars.adc_freq_actual.value
        baudrate = model.vars.baudrate.value  # We don't know the actual bandrate yet
        modtype = model.vars.modulation_type.value
        mi = model.vars.modulation_index.value
        if_frequency_hz = model.vars.if_frequency_hz.value
        etsi_cat1_compatability = model.vars.etsi_cat1_compatible.value
        bw_var = model.vars.bandwidth_tol.value
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value

        # set limits based on selected demod
        [min_bwsel, max_bwsel, min_chfilt_osr, max_chfilt_osr, min_src2, max_src2, min_dec2, \
            max_dec2, min_osr, max_osr, target_bwsel, osr_list] = self.get_limits(demod_select, withremod, relaxsrc2, model)

        # initialize output
        best_bwsel_error = 1e9
        best_osr = 0
        best_dec0 = 0
        best_dec1 = 0

        # Setup for osr loop
        # osr_list is a prioritized list, where first value with valid config will be returned
        if (model.vars.target_osr._value_forced != None):
            osr_forced = model.vars.target_osr._value_forced
            osr_list = [osr_forced]

        # Setup for dec0 loop
        # dec0_list is a prioritized list, where ties in best bwsel go to earlier value in list
        dec0_list = self.return_dec0_list(if_frequency_hz,adc_freq)

        # Search values of osr, dec0, dec1 to find solution
        # Exit on first osr with valid dec0 and dec1
        for osr in osr_list:
            for dec0 in dec0_list:

                # define integer range for dec1
                min_dec1 = int(max(1, ceil(float(adc_freq) * min_bwsel / (8 * dec0 * bandwidth*(1+bw_var)))))
                max_dec1 = int(min(11500, floor(float(adc_freq) * max_bwsel / (8 * dec0 * bandwidth*(1-bw_var)))))
                if min_dec1 <= max_dec1:
                    # Order list from highest to lowest, bwsel from highest to lowest
                    dec1_list = range(max_dec1,min_dec1-1,-1)
                else:
                    # No solution
                    continue

                for dec1 in dec1_list:

                    # check configuration does trigger IPMCUSRW-876 channel filter issue when input sample rate
                    # is too fast relative to the processing clock cycles needed
                    if not self._channel_filter_clocks_valid(model, dec0, dec1):
                        continue

                    # calculated dec2 range
                    if demod_select == model.vars.demod_select.var_enum.BCR:
                        calc_min_dec2 = 1
                        calc_max_dec2 = 1
                        chfilt_osr_actual = float(adc_freq) / (8 * dec0 * dec1 * baudrate)

                        if (modtype == model.vars.modulation_type.var_enum.OOK) or \
                                (modtype == model.vars.modulation_type.var_enum.ASK):
                            if chfilt_osr_actual < osr or chfilt_osr_actual > osr + 1.0:
                                continue
                        else:
                            if (chfilt_osr_actual < min_chfilt_osr) or (chfilt_osr_actual > max_chfilt_osr):
                                # not a solution, next value of dec1 loop
                                continue

                    elif demod_select == model.vars.demod_select.var_enum.TRECS_SLICER or \
                         demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI:
                        # forcing integer osr condition, which applies to TRECS
                        # check for TRECS minimum clk cycle requirements
                        calc_min_dec2 = ceil(min_src2 * float(adc_freq) / (osr * dec0 * dec1 * 8 * baudrate))
                        calc_max_dec2 = floor(max_src2 * float(adc_freq) / (osr * dec0 * dec1 * 8 * baudrate))

                        trecs_src_interp_okay = self._check_trecs_required_clk_cycles(adc_freq, baudrate, osr, dec0,
                                                                                     dec1, xtal_frequency_hz, relaxsrc2, model)
                        if not trecs_src_interp_okay:
                            # not a solution due to trecs clocking constraints, continue
                            continue
                    else:
                        # forcing integer osr condition, which applies to LEGACY, COHERENT
                        calc_min_dec2 = ceil(min_src2 * float(adc_freq) / (osr * dec0 * dec1 * 8 * baudrate))
                        calc_max_dec2 = floor(max_src2 * float(adc_freq) / (osr * dec0 * dec1 * 8 * baudrate))

                    if (calc_min_dec2 <= calc_max_dec2) and (calc_min_dec2 <= max_dec2) and \
                            (calc_max_dec2 >= min_dec2):

                        # calculation of dec1 has constrained bwsel to range bwsel_min to bwsel_max
                        bwsel = bandwidth * (8 * dec0 * dec1) / float(adc_freq)

                        bwsel_error = abs(bwsel - target_bwsel)

                        # Select largest bwsel as best result
                        if (bwsel_error < best_bwsel_error):
                            best_bwsel_error = bwsel_error
                            best_osr = osr
                            best_dec0 = dec0
                            best_dec1 = dec1
                            best_bwsel = bwsel

            if best_osr > 0 and quitatfirstvalid:

                # break out of the osr loop on first successful configuration
                break

        return best_osr, best_dec0, best_dec1, min_osr, max_osr


    def return_dec0_list(self,if_frequency_hz,adc_freq):
        # The purpose of this function is determine the prioritized dec0 list from decimation options 3,4,8
        # Rules:
        # 1) DEC0=8 was only designed for adc_freq <= 40MHz
        # 2) DEC0 anti-aliasing rejection >60dB for DEC0=8 and 4

        first_null_d8 = float(adc_freq) / (8 * 8)
        ratio_d8 = float(if_frequency_hz) / first_null_d8

        first_null_d4 = float(adc_freq) / (8 * 4)
        ratio_d4 = float(if_frequency_hz) / first_null_d4

        if (ratio_d8 < 0.248) and (adc_freq <= 40e6):
            # 0.248 = (.125-.094)/.125 corresponds to >60dB attenuation on d0=8 response
            dec0_priority_list = [8,4,3]
        elif ratio_d4 < 0.27:
            # 0.27 = (.25-0.1825)/.25 corresponds to >60dB attenuation on d0=4 response
            dec0_priority_list = [4,3]
        else:
            dec0_priority_list = [3,4]

        return dec0_priority_list

    def calc_remoden_reg(self, model):
        osr = model.vars.oversampling_rate_actual.value
        dec2_actual = model.vars.dec2_actual.value
        trecs_enabled = model.vars.trecs_enabled.value

        # Current assumption is that we are going to use the REMOD path only for Viterbi/TRecS
        if trecs_enabled and (osr > 7 or dec2_actual > 1):
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_PHDMODCTRL_REMODEN, reg)

    def calc_remoddwn_reg(self, model):
        osr = model.vars.oversampling_rate_actual.value
        #trecs_enabled = model.vars.trecs_enabled.value

        # if trecs_enabled and osr > 7:
        #     remoddwn = int(osr/4) - 1   # we know osr is a multiple of 4 if we're here
        # else:

        # We prefer to not use the slice and remod path so this shoudl always be 0
        remoddwn = 0

        self._reg_write(model.vars.MODEM_PHDMODCTRL_REMODDWN, remoddwn)

    def calc_remodosr_reg(self, model):
        osr = model.vars.oversampling_rate_actual.value
        trecs_enabled = model.vars.trecs_enabled.value

        if trecs_enabled:
            remodosr = int(round(osr)) - 1
        else:
            remodosr = 0

        self._reg_write(model.vars.MODEM_PHDMODCTRL_REMODOSR, remodosr)

    def calc_target_demod_bandwidth(self, model):
        # Calculate target demod bandwidth

        #Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        bw_carson = model.vars.bandwidth_carson_hz.value
        harddecision = model.vars.MODEM_VITERBIDEMOD_HARDDECISION.value
        baudrate = model.vars.baudrate.value
        trecs_enabled = model.vars.trecs_enabled.value

        if (mod_type == model.vars.modulation_type.var_enum.FSK2) or \
                (mod_type == model.vars.modulation_type.var_enum.MSK):
            if trecs_enabled and (harddecision == 0):
                bw_demod = baudrate * 1.1
            else:
                bw_demod = bw_carson

        elif (mod_type == model.vars.modulation_type.var_enum.FSK4):
            bw_demod = bw_carson

        else:
            #Default values for other modulation types
            if (mod_type == model.vars.modulation_type.var_enum.OOK) or \
                    (mod_type == model.vars.modulation_type.var_enum.ASK):
                bw_demod = baudrate * 5.0
                if (model.vars.bandwidth_hz._value_forced == None):
                    print("  WARNING:  OOKASK bandwidth_hz has not been optimized")
            elif (mod_type == model.vars.modulation_type.var_enum.OQPSK):
                bw_demod = baudrate * 1.25
            else:
                bw_demod = baudrate * 1.0

        #Load local variables back into model variables
        model.vars.demod_bandwidth_hz.value = int(bw_demod)

    def calc_lock_bandwidth_actual(self, model):
        # This function calculates the actual channel bandwidth based on adc rate, decimator, and bwsel settings

        # Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        lock_bwsel = model.vars.lock_bwsel.value
        min_bwsel = model.vars.min_bwsel.value

        # Lower limit - calc filter coeffs limits lock_bwsel to min_bwsel
        lock_bwsel_actual = max(lock_bwsel,min_bwsel)

        # Calculate the actual channel bandwidth
        lock_bandwidth_actual = int(adc_freq_actual * lock_bwsel_actual / dec0_actual / dec1_actual / 8)

        # Load local variables back into model variables
        model.vars.lock_bandwidth_actual.value = lock_bandwidth_actual

    def get_data_filter(self, datafilter):

        if datafilter == 0:
            coef = [ 1 ]
        elif datafilter == 1:
            coef = [1/4, 2/4, 1/4]
        elif datafilter == 2:
            coef = [1/4, 1/4, 1/4, 1/4]
        elif datafilter == 3:
            coef = [1/8, 2/8, 2/8, 2/8, 1/8]
        elif datafilter == 4:
            coef = [1/8, 1/8, 2/8, 2/8, 1/8, 1/8]
        elif datafilter == 5:
            coef = [1/8, 1/8, 1/8, 2/8, 1/8, 1/8, 1/8]
        elif datafilter == 6:
            coef = [1/8, 1/8, 1/8, 1/8, 1/8, 1/8, 1/8, 1/8]
        elif datafilter == 7:
            coef = [1/16, 2/16, 2/16, 2/16, 2/16, 2/16, 2/16, 2/16, 1/16]
        else:
            raise CalculationException('ERROR: Invalid setting for datafilter in get_datafilter in calc_demodulator.py')

        return coef

    def get_ma1_filter(self, rawgain):
        if rawgain == 0:
            df = [4]
        elif rawgain == 1:
            df = [2, 2]
        elif rawgain == 2:
            df = [1, 2, 1]
        else:
            df = [1/2, 3/2, 3/2, 1/2]

        return df

    def get_ma2_filter(self, rawfltsel):
        if rawfltsel == 0:
            df = [1/4, 1/4, 1/4, 1/4, 1/4, 1/4, 1/4, 1/4]
        elif rawfltsel == 1:
            df = [1/2, 1/2, 1/2, 1/2]
        else:
            df = [1]

        return df

    def gen_frequency_signal(self, x, sf, cf, sfosr, model):
        # get parameters
        deviation = model.vars.deviation.value
        baudrate = model.vars.baudrate.value
        demodosr = round(model.vars.oversampling_rate_actual.value)
        src2 = model.vars.MODEM_SRCCHF_SRCRATIO2.value
        datafilter = model.vars.MODEM_CTRL2_DATAFILTER.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remodoutsel = model.vars.MODEM_PHDMODCTRL_REMODOUTSEL.value
        demod_select = model.vars.demod_select.value
        dec2 = model.vars.dec2_actual.value

        remodpath = True if remoden or demod_select == model.vars.demod_select.var_enum.BCR else False

        if demod_select == model.vars.demod_select.var_enum.BCR:
            rawndec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value #Moved inside BCR statement to allow inheritance
            dec2 = 2 ** rawndec

        # scale shaping filter to desired amplitude OSR = 8
        sf = sf / np.sum(sf) * sfosr

        # pulse shape OSR = 8
        y = sp.lfilter(sf, 1, x)

        # apply deviation OSR = 8
        z = y * deviation

        # integrate to get phase after scaling by sampling rate at TX OSR = 8
        t = np.cumsum(z / (baudrate * sfosr))

        # modulate at baseband OSR = 8
        u = np.exp(1j * 2 * pi * t)

        # resample at channel filter rate (e.g. sfosr -> osr) OSR = chflt_osr * src2
        # FIXME: handle other remod paths here if we end up using them
        if remodpath:
            osr = demodosr * dec2
        else:
            osr = demodosr

        u2 = sp.resample_poly(u,osr*src2, sfosr*16384)

        # channel filter OSR = chflt_osr * src2
        v = sp.lfilter(cf, 1, u2)

        # src2 - resample to target OSR rate OSR = target_osr * dec2
        v2 = sp.resample_poly(v, 16384, src2)

        # CORDIC OSR = target_osr * dec2
        a = np.unwrap(np.angle(v2))

        # downsample by dec2 to get to target_osr if remod enabled
        if remodpath: #and remodoutsel == 1:
            # differentiate phase to frequency OSR = target_osr * dec2
            f1 = a[1:] - a[0:-1]
            #f = sp.resample_poly(f1, 1, dec2)

            # when downsampling pick the best phase that results in max eye opening as we are going to feed the samples
            # from here to the datafilter. Low value samples will bring the average soft decision to a lower value.
            best_min = 0
            for phase in range(dec2):
                f2 = sp.resample_poly(f1[round(len(f1)/4)+phase:], 1, dec2)
                min_val = min(abs(f2[3:-3]))
                if min_val >= best_min:
                    best_min = min_val
                    f = f2
        else:
            # differentiate phase to frequency OSR = target_osr * dec2
            f = a[osr:] - a[0:-osr]

        # optional decimation and filtering for remod paths
        if demod_select == model.vars.demod_select.var_enum.BCR:
            rawgain = model.vars.MODEM_BCRDEMODOOK_RAWGAIN.value #Moved inside BCR statement to allow inheritance
            rawfltsel = model.vars.MODEM_BCRDEMODCTRL_RAWFLTSEL.value
            ma1 = self.get_ma1_filter(rawgain)
            g1 = sp.lfilter(ma1, 1, f)
            ma2 = self.get_ma2_filter(rawfltsel)
            g = sp.lfilter(ma2, 1, g1)
        elif remoden and (remodoutsel == 0 or remodoutsel == 1):
            df = self.get_data_filter(datafilter)
            g = sp.lfilter(df, 1, f)
        else:
            g = f

        # return frequency signal
        return g

    def return_ksi2_ksi3_calc(self, model, ksi1):
        # get parameters
        lock_bwsel = model.vars.lock_bwsel.value # use the lock bw
        bwsel = model.vars.bwsel.value  # use the lock bw
        osr = int(round(model.vars.oversampling_rate_actual.value))

        # calculate only if needed - ksi1 would be already calculated if that is the case
        if (ksi1 == 0):
            best_ksi2 = 0
            best_ksi3 = 0
            best_ksi3wb = 0
        else:
            # get shaping filter and it oversampling rate with respect to baudrate
            sf = CALC_Shaping_ocelot().get_shaping_filter(model)/1.0
            sfosr = 8 # shaping filter coeffs are sampled at 8x

            # get channel filter and expend the symmetric part
            cfh = np.asarray(self.return_coeffs(lock_bwsel))
            cf = np.block([cfh, cfh[-2::-1]])/1.0
            cfh = np.asarray(self.return_coeffs(bwsel))
            cfwb = np.block([cfh, cfh[-2::-1]])/1.0

            # base sequences for +1 and -1
            a = np.array([ 1.0, 0, 0, 0, 0, 0, 0, 0])
            b = np.array([-1.0, 0, 0, 0, 0, 0, 0, 0])

            # generate frequency signal for periodic 1 1 1 0 0 0 sequence for ksi1
            x1 = np.matlib.repmat(np.append(np.matlib.repmat(a, 1, 3),np.matlib.repmat(b, 1, 3)), 1, 4)
            f1 = self.gen_frequency_signal( x1[0], sf, cf, sfosr, model)

            # generate frequency signal for periodic 1 1 0 0 1 1 sequence for ksi2
            x2 = np.matlib.repmat(np.append(np.matlib.repmat(a, 1, 2), np.matlib.repmat(b, 1, 2)), 1, 6)
            f2 = self.gen_frequency_signal( x2[0], sf, cf, sfosr, model)

            # generate frequency signal for periodic 1 0 1 0 1 0 sequence for ksi3
            x3 = np.matlib.repmat(np.append(np.matlib.repmat(a, 1, 1), np.matlib.repmat(b, 1, 1)), 1, 12)
            f3 = self.gen_frequency_signal( x3[0], sf, cf, sfosr, model)

            # generate frequency signal for periodic 1 0 1 0 1 0 sequence for ksi3 but with aqcusition channel filter
            f3wb = self.gen_frequency_signal( x3[0], sf, cfwb, sfosr, model)

            # find scaling needed to get f1 to the desired ksi1 value and apply it to f2 and f3
            ind = osr - 1
            scaler = ksi1 / np.max(np.abs(f1[ind + 8 * osr - 1: - 2 * osr: osr]))
            f2 = scaler * f2
            f3 = scaler * f3
            f3wb = scaler * f3wb

            # from matplotlib import pyplot as plt
            # plt.plot(f1*scaler,'x-')
            # plt.show()
            # plt.plot(f2,'x-')
            # plt.plot(f3,'x-')
            # plt.plot(f3wb,'x-')

            # search for best phase to sample to get ksi3 value.
            # best phase is the phase that gives largest eye opening
            best_ksi3 = 0
            for ph in range(osr):
                ksi3 = np.max(np.round(np.abs(f3[ - 6 * osr + ph: - 2 * osr: osr])))
                if ksi3 > best_ksi3:
                    best_ksi3 = ksi3

            best_ksi3wb = 0
            for ph in range(osr):
                ksi3wb = np.max(np.round(np.abs(f3wb[ - 6 * osr + ph: - 2 * osr: osr])))
                if ksi3wb > best_ksi3wb:
                    best_ksi3wb = ksi3wb

            # ksi2 is tricky depending if we sampled perfectly (symmetric around a
            # pulse we should see the same value for 1 1 0 and 0 1 1 sequence but
            # most of the time we cannot sample perfectly since can go as low as 4x
            # oversampling for Viterbi PHYs. In this case we have 2 ksi values which we
            # average to get the ksi2 value
            best_cost = 1e9
            for ph in range(osr):
                x = np.round(np.abs(f2[- 6 * osr + ph: - 2 * osr: osr]))
                cost = np.sum(np.abs(x - np.mean(x)))
                if cost < best_cost:
                    best_cost = cost
                    best_ksi2 = np.round(np.mean(x))

        # ensure that ksi1 >= ksi2 >= ksi3
        # this code should only be needed in the extreme case when ksi1 = ksi2 = ksi3 and
        # small variation can cause one to be larger than the other
        best_ksi2 = ksi1 if best_ksi2 > ksi1 else best_ksi2
        best_ksi3 = best_ksi2 if best_ksi3 > best_ksi2 else best_ksi3
        best_ksi3wb = best_ksi2 if best_ksi3wb > best_ksi2 else best_ksi3wb

        return best_ksi2, best_ksi3, best_ksi3wb

    def calc_ksi2_ksi3(self, model):
        # This function writes the ksi2,3 model variables that are used to program both
        # hardmodem and softmodem ksi regs

        #Read in model vars
        ksi1 = model.vars.ksi1.value

        # Call the calculation routine for ksi2 and ksi3
        ksi2, ksi3, ksi3wb = self.return_ksi2_ksi3_calc(model, ksi1)

        #Write the model vars
        model.vars.ksi2.value = int(ksi2)
        model.vars.ksi3.value = int(ksi3)
        model.vars.ksi3wb.value = int(ksi3wb)

    def calc_ksi2_ksi3_reg(self, model):

        #Read in model vars
        ksi2 = model.vars.ksi2.value
        ksi3 = model.vars.ksi3.value
        ksi3wb = model.vars.ksi3wb.value

        #Write the reg fields
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VITERBIKSI2, int(ksi2))
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VITERBIKSI3, int(ksi3))
        self._reg_write(model.vars.MODEM_VTCORRCFG1_VITERBIKSI3WB, int(ksi3wb))

    def calc_prefiltcoeff_reg(self, model):
        dsss0 = model.vars.MODEM_DSSS0_DSSS0.value
        modtype = model.vars.modulation_type.value
        demod_select = model.vars.demod_select.value

        if modtype == model.vars.modulation_type.var_enum.OQPSK and dsss0 != 0:
            dsss0_rotated = ((dsss0 << 1) | (dsss0 >> 31)) & 0xFFFFFFFF
            dsss0_rotated_conj = dsss0_rotated ^ 0x55555555
            prefilt = 2**32 + (dsss0 ^ ~dsss0_rotated_conj)
        elif demod_select == model.vars.demod_select.var_enum.LONGRANGE:
            prefilt = 0x3C3C3C3C
        else:
            prefilt = 0

        self._reg_write(model.vars.MODEM_PREFILTCOEFF_PREFILTCOEFF, prefilt)

    def calc_prefiltercoff_len(self, model):
        demod_select = model.vars.demod_select.value
        cplxcorr_enabled = model.vars.MODEM_CTRL6_CPLXCORREN.value
        dsss_len = model.vars.dsss_len_actual.value

        # : For coherent demod, set prefilter length to 4 symbols
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            prefilter_len_actual = dsss_len * 4
            # If complex correlation is enabled, max length is 64 (prefilter_len_reg = 1)
            if cplxcorr_enabled == 1:
                if prefilter_len_actual > 64:
                    prefilter_len_actual = 64
        else: # : default value for all other demods
            prefilter_len_actual = 64
        # : convert actual length to register values
        prefilter_len_reg = int(round(prefilter_len_actual / 32.0 - 1.0))

        self._reg_write(model.vars.MODEM_LONGRANGE1_PREFILTLEN, prefilter_len_reg)

    def calc_demod_misc(self, model):
        #Now that we always use the digital mixer, the CFOSR reg field is never used
        self._reg_do_not_care(model.vars.MODEM_CF_CFOSR)

    def _check_trecs_required_clk_cycles(self, adc_freq, baudrate, osr, dec0, dec1, xtal_frequency_hz, relaxsrc2, model):
        # Returns True if the filter chain configuration meets the requirement for trecs
        # minimum clock cycles between samples. Returns False if the configuration is invalid
        #
        # IPMCUSRW-668 - TRECS requires minimum of 4 clk between samples. SRC interpolation on ocelot
        # has a fixed 3 clk separation and cannot be used with TRECS. Limiting max_src2_ratio is sufficient
        # for ocelot, but this function is used by inherited classes which are able to adjust the
        # interpolated sample clk delay

        # calculate the src_ratio as this function is called in the process of evaluating
        # osr, dec0, dec1, so the src_ratio_actual cannot be calculated
        dec1_freq = adc_freq / (8 * dec0 * dec1)
        src_freq = baudrate * osr
        src_ratio = src_freq / dec1_freq
        TRECS_REQUIRED_CLKS_PER_SAMPLE = 4
        bandwidth_hz = model.vars.bandwidth_hz.value
        is_vcodiv = model.vars.adc_clock_mode.value == model.vars.adc_clock_mode.var_enum.VCODIV

        if src_ratio > 1:
            # ocelot has fixed clk delay of 3

            # IPMCUSRW-668 when it occurs causes slightly slower waterfall curves, and minor < 1% PER bumps
            # if a PHY suffers from IPMCUSRW-876 (channel filter clocks), it is preferable to solve the channel
            # filter issue by allowing the PHY workaround of a lower f_dec1 and interpolation on SRC2
            bandwidth_threshold = 38e6 / 4 * 0.2  # minimum hfxo / chf_clks_per_sample * min_bwsel
            return relaxsrc2 and is_vcodiv and bandwidth_hz > bandwidth_threshold
        else:
            cycles_per_sample = floor(xtal_frequency_hz / src_freq)
            meets_clk_cycle_requirement = cycles_per_sample >= TRECS_REQUIRED_CLKS_PER_SAMPLE
            return meets_clk_cycle_requirement

    def calc_rx_restart_reg(self, model):
        """
        Calculate collision restart control registers.

        Args:
            model:

        Returns:

        """

        antdivmode = model.vars.antdivmode.value
        fltrsten = 0
        antswrstfltdis = 1
        rxrestartb4predet = 0
        rxrestartmatap = 1
        rxrestartmalatchsel = 0
        rxrestartmacompensel = 2
        rxrestartmathreshold = 6
        rxrestartuponmarssi = 0

        #The following need to be set the same regardless of antdiv enable
        self._reg_write(model.vars.MODEM_RXRESTART_FLTRSTEN, fltrsten)
        self._reg_write(model.vars.MODEM_RXRESTART_ANTSWRSTFLTTDIS, antswrstfltdis)

        if antdivmode == model.vars.antdivmode.var_enum.DISABLE or \
                antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTB4PREDET)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMATAP)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMALATCHSEL)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMACOMPENSEL)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMATHRESHOLD)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTUPONMARSSI)
        else:
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTB4PREDET, rxrestartb4predet)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTMATAP, rxrestartmatap)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTMALATCHSEL, rxrestartmalatchsel)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTMACOMPENSEL, rxrestartmacompensel)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTMATHRESHOLD, rxrestartmathreshold)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTUPONMARSSI, rxrestartuponmarssi)

    def calc_preamble_detection_length(self, model):
        #This method calculates a defualt value for preamble_detection_length

        preamble_length = model.vars.preamble_length.value

        #Set the preamble detection length to the preamble length (TX) by default
        model.vars.preamble_detection_length.value = preamble_length

    def calc_detdis_reg(self, model):
        #This method calculates the MODEM_CTRL0_DETDIS field

        #For Ocelot always set to 0
        self._reg_write(model.vars.MODEM_CTRL0_DETDIS, 0)

    def calc_dec1gain_actual(self, model):
        """given register settings return actual DEC1GAIN used

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        reg = model.vars.MODEM_CF_DEC1GAIN.value

        if reg == 0:
            val = 0
        elif reg == 1:
            val = 6
        else:
            val = 12

        model.vars.dec1gain_actual.value = val

    def calc_rssi_dig_adjust_db(self, model):
        #These variables are passed to RAIL so that RSSI corrections can be made to more accurately measure power

        #Read in model vars
        dec0gain = model.vars.MODEM_DIGIGAINCTRL_DEC0GAIN.value
        dec1_actual = model.vars.dec1_actual.value
        dec1gain_actual = model.vars.dec1gain_actual.value
        digigainen = model.vars.MODEM_DIGIGAINCTRL_DIGIGAINEN.value
        digigainsel = model.vars.MODEM_DIGIGAINCTRL_DIGIGAINSEL.value
        digigaindouble = model.vars.MODEM_DIGIGAINCTRL_DIGIGAINDOUBLE.value
        digigainhalf = model.vars.MODEM_DIGIGAINCTRL_DIGIGAINHALF.value

        #Calculate gains

        dec0_gain_db = 6.0*dec0gain

        dec1_gain_linear = (dec1_actual**4) * (2**(-1*math.floor(4*math.log2(dec1_actual)-4)))
        dec1_gain_db = 20*math.log10(dec1_gain_linear/16) + dec1gain_actual #Normalize so that dec1=0 gives gain=16

        if digigainen:
            digigain_db = -3+(digigainsel*0.25)
        else:
            digigain_db = 0
        digigain_db += 6*digigaindouble-6*digigainhalf

        # For consistency / simplicity, let's treat the rssi_adjust_db  output from the calculator like RAIL handles
        # EFR32_FEATURE_SW_CORRECTED_RSSI_OFFSET in that the value is thought to be added to the RSSI
        # So to compensate for the digital gain, the value should be the negative of the excess gain
        # Note that RSSISHIFT is actually subtracted from the RSSI, but EFR32_FEATURE_SW_CORRECTED_RSSI_OFFSET is
        # subtracted from the default RSSISHIFT so that the proper sign is maintained
        rssi_dig_adjust_db = -(dec0_gain_db + dec1_gain_db + digigain_db)

        #Write the vars
        model.vars.rssi_dig_adjust_db.value = rssi_dig_adjust_db

    def calc_rssi_rf_adjust_db(self, model):

        #Read in model vars
        rf_band = model.vars.rf_band.value

        #Calculate rf adjustment based on band
        if rf_band == model.vars.rf_band.var_enum.BAND_169:
            rssi_rf_adjust_db = -15.5
        elif rf_band == model.vars.rf_band.var_enum.BAND_315:
            rssi_rf_adjust_db = -16.4
        elif rf_band == model.vars.rf_band.var_enum.BAND_434:
            rssi_rf_adjust_db = -14.3
        elif rf_band == model.vars.rf_band.var_enum.BAND_490:
            rssi_rf_adjust_db = -14.3
        elif rf_band == model.vars.rf_band.var_enum.BAND_868 or \
            rf_band == model.vars.rf_band.var_enum.BAND_915:
            rssi_rf_adjust_db = -10.4
        else:
            LogMgr.Warning("Warning: No RSSI adjustment available for this band")
            rssi_rf_adjust_db = 0.0

        #Write the model var
        model.vars.rssi_rf_adjust_db.value = rssi_rf_adjust_db

    def calc_rssi_adjust_db(self, model):

        #Read in model vars
        rssi_dig_adjust_db = model.vars.rssi_dig_adjust_db.value
        rssi_rf_adjust_db = model.vars.rssi_rf_adjust_db.value

        #Add digital and RF adjustments
        rssi_adjust_db = rssi_dig_adjust_db + rssi_rf_adjust_db

        #Write the model var
        model.vars.rssi_adjust_db.value = rssi_adjust_db

    def _channel_filter_clocks_valid(self, model, dec0, dec1):
        # returns if the requested configuration is safe to not trigger ipmcusrw-876
        # to avoid the channel filter sampling issue, clks_per_sample >= 4
        # helper function for return_osr_dec0_dec1

        # no margin on the first check. hfxomult clocking at exactly 4 clks/sample will not trigger this issue
        safe_clks_per_sample = self.chf_required_clks_per_sample
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        adc_freq = model.vars.adc_freq_actual.value
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value
        base_frequency_hz = model.vars.base_frequency_hz.value

        f_dec1 = adc_freq / (8 * dec0 * dec1)
        clks_per_sample = xtal_frequency_hz / f_dec1
        base_config_valid = clks_per_sample >= safe_clks_per_sample

        # for lodiv based clocking, sample rate varies with RF. VCODIV PHYs are only used in the 2.4G band
        # maximum ppm change can be determined by the min, max of the FCC band of 2400-2483.5 MHz
        # for current 2.4G LODIV products, if its LODIV and subG the channel plan doesn't span
        # wide enough where this is a problem
        in_2p4G_band = base_frequency_hz >= 2400e6 and base_frequency_hz <= 2500e6
        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.VCODIV and in_2p4G_band:
            max_rf_frequency = 2480e6
            max_ppm = (max_rf_frequency - base_frequency_hz) / base_frequency_hz
            # (1-max_ppm because adc_freq is in the denominator
            clks_per_sample_highest_channel = clks_per_sample * (1 - max_ppm)
            highest_channel_valid = clks_per_sample_highest_channel >= self.chf_required_clks_per_sample
            valid = base_config_valid and highest_channel_valid
        else:
            valid = base_config_valid
        return valid

    def calc_phscale_derate_factor(self, model):
        #This function calculates the derating factor for PHSCALE for TRECS PHYs with large freq offset tol

        #Always set to 1 on Ocelot for now
        phscale_derate_factor = 1

        #Write the model var
        model.vars.phscale_derate_factor.value = phscale_derate_factor
