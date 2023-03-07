from pyradioconfig.parts.ocelot.calculators.calc_demodulator import CALC_Demodulator_ocelot
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.parts.sol.calculators.calc_utilities import Calc_Utilities_Sol
from math import *
from enum import Enum
import numpy as np
import numpy.matlib
from scipy import signal as sp

class Calc_Demodulator_Sol(CALC_Demodulator_ocelot):

    SRC2DENUM = 524288.0
    ofdm_tsym_useful = 96e-6

    # override inhereited variable defining threshold for _channel_filter_clocks_valid
    chf_required_clks_per_sample = 3  # fixed in PGSOL-2790

    def buildVariables(self, model):

        #Build variables from Ocelot
        super().buildVariables(model)

        self._addModelVariable(model, 'ofdm_option', Enum, ModelVariableFormat.DECIMAL, desc='Option number of OFDM PHYs')
        model.vars.ofdm_option.var_enum = CreateModelVariableEnum(
            enum_name='OfdmOptionEnum',
            enum_desc='OFDM Option Number',
            member_data=[
                ['OPT1', 0, 'Option 1'],
                ['OPT2', 1, 'Option 2'],
                ['OPT3', 2, 'Option 3'],
                ['OPT4', 3, 'Option 4'],
            ])

        model.vars.demod_select.var_enum = CreateModelVariableEnum(
            enum_name='DemodSelectEnum',
            enum_desc='Demod Selection',
            member_data=[
                ['LEGACY', 0, 'Legacy Demod'],
                ['COHERENT', 1, 'Coherent Demod'],
                ['TRECS_VITERBI', 2, 'TRecS + Viterbi Demod'],
                ['TRECS_SLICER', 3, 'TRecS + HD Demod'],
                ['BCR', 4, 'PRO2 BCR Demod'],
                ['LONGRANGE', 5, 'BLE Long Range Demod'],
                ['SOFT_DEMOD', 6, 'Soft Demod']
            ])
        self._addModelVariable(model, 'upgaps', int, ModelVariableFormat.DECIMAL, desc='Additional clk cycles added for SRC interpolation')
        self._addModelVariable(model, 'ofdm_subcarrier_spacing_hz', float, ModelVariableFormat.DECIMAL, desc = 'OFDM subcarrier spacing')
        self._addModelVariable(model, 'ofdm_subcarrier_count', int, ModelVariableFormat.DECIMAL, desc='Total number of OFDM subcarriers in channel')
        self._addModelVariable(model, 'ofdm_fft_size', int, ModelVariableFormat.DECIMAL, desc='FFT size for OFDM demod')
        self._addModelVariable(model, 'dual_fefilt', bool, ModelVariableFormat.ASCII, desc='PHY uses both FEFILT paths')

    def _add_demod_rate_variable(self, model):
        self._addModelActual(model, 'demod_rate', float, ModelVariableFormat.DECIMAL)

    def calc_init_advanced(self, model):
        pass

    def calc_demod_rate_actual(self,model):
        #This function calculates the actual sample rate at the demod

        # Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2_actual = model.vars.src2_ratio_actual.value

        demod_rate_actual = adc_freq_actual * src2_actual / (8 * dec0_actual * dec1_actual * dec2_actual)

        #Load local variables back into model variables
        model.vars.demod_rate_actual.value = demod_rate_actual

    def calc_mod_type_reg(self, model):
        #This function writes the modulation type register

        #Load model variables into local variables
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value

        if (softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OFDM) or \
                (softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.CW) or \
            (softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_FSK):
            # Not supported in the original demod - just put an arbitrary value in MODFORMAT
            self._reg_write(model.vars.MODEM_CTRL0_MODFORMAT, 0)
        elif (softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OQPSK):
            self._reg_write(model.vars.MODEM_CTRL0_MODFORMAT, 5)
        else:
            #Call the inherited code from Ocelot
            super().calc_mod_type_reg(model)

    def calc_demod_sel(self, model):

        modulation_type = model.vars.modulation_type.value

        # overwrite the result from the Ocelot function if demod_select is not forced AND modulation type is OFDM
        if (model.vars.demod_select._value_forced == model.vars.demod_select.var_enum.SOFT_DEMOD) or \
                (modulation_type == model.vars.modulation_type.var_enum.OFDM) :

            demod_select = model.vars.demod_select.var_enum.SOFT_DEMOD

            [target_osr, dec0, dec1, min_osr, max_osr] = self.return_osr_dec0_dec1(model, demod_select, quitatfirstvalid=False)

            if target_osr == 0:
                raise CalculationException('WARNING: target_osr=0 in calc_choose_demod()')

            model.vars.demod_select.value = demod_select
            model.vars.target_osr.value = int(target_osr)
            model.vars.targetmin_osr.value = int(min_osr)
            model.vars.targetmax_osr.value = int(max_osr)
            model.vars.dec0.value = int(dec0)
            model.vars.dec1.value = int(dec1)
        else:
            # call the ocelot version of this function first
            super().calc_demod_sel(model)

    def calc_modeminfo_reg(self, model):

        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        dual_fefilt = model.vars.dual_fefilt.value

        soft_modem_used = softmodem_modulation_type != model.vars.softmodem_modulation_type.var_enum.NONE

        if (not soft_modem_used) or (soft_modem_used and dual_fefilt):
            #Hardmodem non-concurrent or concurrent hard/soft modem
            bcr_used = model.vars.MODEM_BCRDEMODCTRL_BCRDEMODEN.value
            trecs_used = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
            coherent_used = model.vars.MODEM_CTRL1_PHASEDEMOD.value == 2
            dsa_viterbi_used = model.vars.MODEM_DSACTRL_DSAMODE.value #phase DSA
            if bcr_used or trecs_used or coherent_used:
                legacy_used = 0
            else:
                legacy_used = 1
        else:
            bcr_used = 0
            trecs_used = 0
            coherent_used = 0
            dsa_viterbi_used = 0
            legacy_used = 0

        self._reg_write(model.vars.SEQ_MODEMINFO_SOFTMODEM_DEMOD_EN, int(soft_modem_used))
        self._reg_write(model.vars.SEQ_MODEMINFO_SOFTMODEM_MOD_EN, int(soft_modem_used))
        self._reg_write(model.vars.SEQ_MODEMINFO_BCR_EN, bcr_used)
        self._reg_write(model.vars.SEQ_MODEMINFO_COHERENT_EN, int(coherent_used))
        self._reg_write(model.vars.SEQ_MODEMINFO_TRECS_EN, trecs_used)
        self._reg_write(model.vars.SEQ_MODEMINFO_LEGACY_EN, legacy_used)
        self._reg_write(model.vars.SEQ_MODEMINFO_DSA_VITERBI_EN, dsa_viterbi_used)
        self._reg_write(model.vars.SEQ_MODEMINFO_HARDMODEM_SPARE, 0)
        self._reg_write(model.vars.SEQ_MODEMINFO_SOFTMODEM_SPARE, 0)

    def calc_dec0_reg(self,model):
        #This function writes the register for dec0

        #Load model variables into local variables
        dec0_value = model.vars.dec0.value
        fefilt_selected = model.vars.fefilt_selected.value

        #Define a constant list for the (register data, value pairs)
        dec0_list = [(0, 3), (2, 4), (4, 8), (5, 5)]
        # Search for the value in the list
        for dec0_pair in dec0_list:
            if (dec0_pair[1]==dec0_value):
                dec0_reg = dec0_pair[0]

        #Write the registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'CF_DEC0', dec0_reg)

    def calc_dec1_reg(self, model):
        #This function writes the register for dec1

        #Load model variables into local variables
        dec1_value = model.vars.dec1.value
        fefilt_selected = model.vars.fefilt_selected.value

        #Dec1 register is simply one less than the value
        dec1_reg = dec1_value - 1

        #Write the registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'CF_DEC1', dec1_reg)

    def calc_dec0_actual(self,model):
        #This function calculates the actual dec0 based on the register value

        #Load model variables into local variables
        dec0_reg = Calc_Utilities_Sol().get_fefilt_actual(model,'CF_DEC0')

        #Define a constant list for the (register data, value pairs)
        dec0_list = [(0, 3), (1, 4), (2, 4), (3, 8), (4, 8), (5, 5)]
        #Search for the value in the list
        for dec0_pair in dec0_list:
            if (dec0_pair[0]==dec0_reg):
                dec0_value = dec0_pair[1]

        #Load local variables back into model variables
        model.vars.dec0_actual.value = dec0_value

    def calc_dec1_actual(self, model):
        #This function calculates the actual dec1 based on the register value

        #Load model variables into local variables
        dec1_reg = Calc_Utilities_Sol().get_fefilt_actual(model, 'CF_DEC1')

        #Dec1 value is simply one more than the register setting
        dec1_value = dec1_reg + 1

        #Load local variables back into model variables
        model.vars.dec1_actual.value = dec1_value

    def calc_src2_reg(self,model):
        #This function calculates the src2 register writes

        # Load model variables into local variables
        src2_value = model.vars.src2_ratio.value
        min_src2 = model.vars.min_src2.value  # min value for SRC2
        max_src2 = model.vars.max_src2.value  # max value for SRC2
        fefilt_selected = model.vars.fefilt_selected.value
        srcratio_value_forced = Calc_Utilities_Sol().get_fefilt_value_forced(model, 'SRC_SRCRATIO')

        if srcratio_value_forced != None:
            srcratio_value = Calc_Utilities_Sol().get_fefilt_actual(model, 'SRC_SRCRATIO')
            src2_forced =  srcratio_value / self.SRC2DENUM
            if (src2_forced > max_src2):
                max_src2 = src2_forced
            if (src2_forced < min_src2):
                min_src2 = src2_forced

        if (src2_value) >= min_src2 and (src2_value <= max_src2):
            src2_reg = int(round(self.SRC2DENUM/src2_value))
        else:
            raise CalculationException('WARNING: src2 value out of range in calc_src2_reg()')

        if (src2_reg != self.SRC2DENUM):
            src2_en = 1
        else:
            src2_en = 0

        #Write to registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'SRC_SRCRATIO', src2_reg)
        self._reg_write_by_name_concat(model, fefilt_selected, 'SRC_SRCENABLE', src2_en)

    def calc_dec2_reg(self,model):
        #This function calculates the dec2 register value

        #Load model variables into local variables
        dec2_value = model.vars.dec2.value

        # The dec2 register is one less than the decimation value
        dec2_reg = dec2_value - 1

        #Write to register
        self._reg_write(model.vars.MODEM_CTRL5_DEC2, dec2_reg)

    def calc_src2_actual(self,model):
        #This function calculates the actual SRC2 ratio from the register value

        #Load model variables into local variables
        src2_en_reg = Calc_Utilities_Sol().get_fefilt_actual(model, 'SRC_SRCENABLE')
        src2_reg = Calc_Utilities_Sol().get_fefilt_actual(model, 'SRC_SRCRATIO')

        if src2_en_reg:
            #The src2 ratio is simply 524288 divided by the register value
            src2_ratio_actual = self.SRC2DENUM / src2_reg
        else:
            src2_ratio_actual = 1.0

        #Load local variables back into model variables
        model.vars.src2_ratio_actual.value = src2_ratio_actual

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

    def calc_dec2_actual(self,model):
        #This function calculates the actual dec2 ratio from the register value
        demod_select = model.vars.demod_select.value

        # SRCSRD is a new decimator that we put into FEFILT only in the soft demod path
        # it works very similiar to DEC2 and to avoid replicating the code form Ocelot related to this
        # we just reuse dec2 to calculate it. At one point we might want to modify Ocelot code to have a more
        # generic name for model.vars.dec2.val and the corresponding actual variables to hold the decimation value
        # for DEC2 in Ocelot/Hard demod or SRCSRD for the soft demod.
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            dec2_actual = model.vars.srcsrd_actual.value
        else:
            #Load model variables into local variables
            dec2_reg = model.vars.MODEM_CTRL5_DEC2.value

            #The actual dec2 value is the dec2 register plus one
            dec2_actual = dec2_reg + 1

        #Load local variables back into model variables
        model.vars.dec2_actual.value = dec2_actual

    def calc_bitrate_gross(self, model):

        mod_type = model.vars.modulation_type.value
        bitrate = model.vars.bitrate.value

        if mod_type == model.vars.modulation_type.var_enum.OFDM:
            model.vars.bitrate_gross.value = bitrate
        else:
            super().calc_bitrate_gross(model)

    def calc_baudrate(self, model):
        # This function calculates baudrate based on the input bitrate and modulation/encoding settings

        # Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        bitrate_gross = model.vars.bitrate_gross.value
        encoding = model.vars.symbol_encoding.value
        spreading_factor = model.vars.dsss_spreading_factor.value

        if (mod_type == model.vars.modulation_type.var_enum.OFDM):
            ofdm_fft_size = model.vars.ofdm_fft_size.value
            baudrate = 1.0/self.ofdm_tsym_useful*ofdm_fft_size
        else:
            # Based on modulation type calculate baudrate from bitrate
            if (mod_type == model.vars.modulation_type.var_enum.OQPSK) or \
                    (mod_type == model.vars.modulation_type.var_enum.OOK) or \
                    (mod_type == model.vars.modulation_type.var_enum.ASK) or \
                    (mod_type == model.vars.modulation_type.var_enum.FSK2) or \
                    (mod_type == model.vars.modulation_type.var_enum.MSK) or \
                    (mod_type == model.vars.modulation_type.var_enum.BPSK) or \
                    (mod_type == model.vars.modulation_type.var_enum.DBPSK):
                baudrate = bitrate_gross * 1.0
            elif (mod_type == model.vars.modulation_type.var_enum.FSK4):
                baudrate = bitrate_gross / 2.0
            else:
                raise CalculationException('ERROR: modulation type not supported in calc_baudrate()')

            # Account for the DSSS spreading factor
            if (encoding == model.vars.symbol_encoding.var_enum.DSSS):
                baudrate *= spreading_factor

        # Load local variables back into model variables
        model.vars.baudrate.value = baudrate


    def calc_chfilt_reg(self,model):

        #This function calculates the channel filter registers

        #Load model variables into local variables
        bwsel = model.vars.bwsel.value
        fefilt_selected = model.vars.fefilt_selected.value

        coeffs = self.return_coeffs(bwsel)

        bit_widths = [10,10,10,11,11,11,12,12,12,14,14,14,16,16,16]

        # replace negative numbers with 2s complement
        for i in range(15):
            if coeffs[i] < 0:
                coeffs[i] = coeffs[i] + 2**bit_widths[i]

        # Write registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE00_SET0COEFF0', coeffs[0])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE00_SET0COEFF1', coeffs[1])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE00_SET0COEFF2', coeffs[2])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE01_SET0COEFF3', coeffs[3])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE01_SET0COEFF4', coeffs[4])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE02_SET0COEFF5', coeffs[5])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE02_SET0COEFF6', coeffs[6])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE03_SET0COEFF7', coeffs[7])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE03_SET0COEFF8', coeffs[8])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE04_SET0COEFF9', coeffs[9])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE04_SET0COEFF10', coeffs[10])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE05_SET0COEFF11', coeffs[11])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE05_SET0COEFF12', coeffs[12])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE06_SET0COEFF13', coeffs[13])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE06_SET0COEFF14', coeffs[14])

        # Load model variables into local variables
        bwsel = model.vars.lock_bwsel.value
        coeffs = self.return_coeffs(bwsel)

        # replace negative numbers with 2s complement
        for i in range(15):
            if coeffs[i] < 0:
                coeffs[i] = coeffs[i] + 2**bit_widths[i]

        # TODO: calculate the second set separately
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE10_SET1COEFF0', coeffs[0])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE10_SET1COEFF1', coeffs[1])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE10_SET1COEFF2', coeffs[2])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE11_SET1COEFF3', coeffs[3])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE11_SET1COEFF4', coeffs[4])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE12_SET1COEFF5', coeffs[5])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE12_SET1COEFF6', coeffs[6])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE13_SET1COEFF7', coeffs[7])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE13_SET1COEFF8', coeffs[8])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE14_SET1COEFF9', coeffs[9])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE14_SET1COEFF10', coeffs[10])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE15_SET1COEFF11', coeffs[11])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE15_SET1COEFF12', coeffs[12])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE16_SET1COEFF13', coeffs[13])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCOE16_SET1COEFF14', coeffs[14])


    def calc_digmixfreq_reg(self,model):
        #This function calculates the digital mixer register
        fefilt_selected = model.vars.fefilt_selected.value
        digmixfreq = model.vars.digmixfreq.value

        # Write register
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGMIXCTRL_DIGMIXFREQ', digmixfreq)

    def calc_digmixfreq_actual(self,model):

        #This function calculates the actual digital mixer frequency based on the register

        #Load model variables into local variables
        digmixfreq_reg = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFREQ')
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value

        #Calculate the actual mixer frequency
        digmixfreq_actual = int(digmixfreq_reg * model.vars.digmix_res_actual.value)

        #Load local variables back into model variables
        model.vars.digmixfreq_actual.value = digmixfreq_actual


    def calc_intosr_reg(self, model):
        # This function sets INTOSR register field
        osr = model.vars.oversampling_rate_actual.value

        if abs(round(osr) - osr) < 0.001:
            intosr = 1
        else:
            intosr = 0

        self._reg_write(model.vars.MODEM_CTRL5_INTOSR, intosr)

    def calc_dec1gain_reg(self, model):
        """set DEC1GAIN register based on calculated value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = model.vars.dec1gain.value
        fefilt_selected = model.vars.fefilt_selected.value

        if val == 12:
            reg = 2
        elif val == 6:
            reg = 1
        else:
            reg = 0

        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGIGAINCTRL_DEC1GAIN', reg)


    def calc_chflatency_actual(self, model):
        chflatency = Calc_Utilities_Sol().get_fefilt_actual(model, 'CHFCTRL_CHFLATENCY')
        model.vars.chflatency_actual.value = chflatency


    def calc_srcsrd_reg(self, model):

        mod_type = model.vars.modulation_type.value
        dec2 = model.vars.dec2.value
        demod_select = model.vars.demod_select.value
        fefilt_selected = model.vars.fefilt_selected.value

        # can only be used in the SOFT_DEMOD
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            # This acts like the new DEC2
            reg = dec2 - 1
        else:
            reg = 0

        self._reg_write_by_name_concat(model, fefilt_selected, 'SRC_SRCSRD', reg)

    def calc_srcsrd_actual(self, model):

        reg = Calc_Utilities_Sol().get_fefilt_actual(model, 'SRC_SRCSRD')
        model.vars.srcsrd_actual.value = int(reg+1)

    def calc_dccomp_misc_reg(self, model):

        #Read in model vars
        fefilt_selected = model.vars.fefilt_selected.value
        if_frequency_hz = model.vars.if_frequency_hz.value

        #If using zero-IF, disable DC comp.
        if if_frequency_hz == 0:
            dccompen = 0
        else:
            dccompen = 1

        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCCOMPEN', dccompen)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCESTIEN', 1)

        # don't reset at every packet
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCRSTEN', 0)

        # always enable gear shifting option
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCGAINGEAREN', 1)

        # when AGC gain change happens set the gear to fastest
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCGAINGEAR', 10)

        # final gear setting after settling
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCCOMPGEAR', 6)

        # limit max DC to 1V
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCLIMIT', 0)

        # don't freeze state of DC comp filters
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCCOMPFREEZE', 0)

        # time between gear shifts - set to fixed value for now
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCGAINGEARSMPS', 40)

    def calc_rx_restart_reg(self, model):
        antdivmode = model.vars.antdivmode.value
        fltrsten = 0
        antswrstfltdis = 1
        rxrestartb4predet = 0
        rxrestartuponmarssi = 0

        if antdivmode == model.vars.antdivmode.var_enum.DISABLE or \
                antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_FLTRSTEN)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_ANTSWRSTFLTTDIS)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTB4PREDET)
            self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTUPONMARSSI)
        else:
            self._reg_write(model.vars.MODEM_RXRESTART_FLTRSTEN, fltrsten)
            self._reg_write(model.vars.MODEM_RXRESTART_ANTSWRSTFLTTDIS, antswrstfltdis)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTB4PREDET, rxrestartb4predet)
            self._reg_write(model.vars.MODEM_RXRESTART_RXRESTARTUPONMARSSI, rxrestartuponmarssi)

    #FIXME: probably can avoid replicating this in Sol if we modify it slightly in Ocelot to avoid reference to src register
    def gen_frequency_signal(self, x, sf, cf, sfosr, model):
        # get parameters
        deviation = model.vars.deviation.value
        baudrate = model.vars.baudrate.value
        demodosr = round(model.vars.oversampling_rate_actual.value)
        src2 = Calc_Utilities_Sol().get_fefilt_actual(model, 'SRC_SRCRATIO')
        datafilter = model.vars.MODEM_CTRL2_DATAFILTER.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remodoutsel = model.vars.MODEM_PHDMODCTRL_REMODOUTSEL.value
        rawfltsel = model.vars.MODEM_BCRDEMODCTRL_RAWFLTSEL.value
        demod_select = model.vars.demod_select.value
        rawgain = model.vars.MODEM_BCRDEMODOOK_RAWGAIN.value
        dec2 = model.vars.dec2_actual.value
        rawndec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value

        remodpath = True if remoden or demod_select == model.vars.demod_select.var_enum.BCR else False
        softdemod = model.vars.demod_select.value == model.vars.demod_select.var_enum.SOFT_DEMOD

        if demod_select == model.vars.demod_select.var_enum.BCR:
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
        if remodpath or softdemod:
            osr = demodosr * dec2
        else:
            osr = demodosr

        # scaling by 32 to avoid resampling by very large ratios which takes too much time
        # we don't need that precision here
        u2 = sp.resample_poly(u,round(osr*src2/32), round(sfosr*self.SRC2DENUM/32))

        # channel filter OSR = chflt_osr * src2
        v = sp.lfilter(cf, 1, u2)

        # src2 - resample to target OSR rate OSR = target_osr * dec2
        v2 = sp.resample_poly(v, round(self.SRC2DENUM/32), round(src2/32))

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

    def calc_bandwdith_tol(self, model):
        # define tolerance in bandwidth calculation used in return_osr_dec0_dec1()
        # setting it to 0.025 allow a 2.5% variation in the BW when finding possible solution in decimators
        # reducing this number reduces possible solutions - maybe to zero
        # increasing it increases potential error in the actual bandwidth
        # may want to set this based on modulation method or other parameters in the future
        model.vars.bandwidth_tol.value = 0.025

    def get_limits(self, demod_select, withremod, relaxsrc2, model):

        # Load model variables into local variables
        modtype = model.vars.modulation_type.value
        mi = model.vars.modulation_index.value

        min_chfilt_osr = None
        max_chfilt_osr = None

        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:

            # If we are here we targeting the soft demod. On the soft demod side FSK is demodulated using the TRECS-VITERBI
            # approach and OQPSK is demodualted using the COHERENT demod approach therefore we set the FEFILT settings
            # accordingly. Note that in the FSK case we limit OSR to 4 and 5 as oppose to going up to 7 as in the hard demod
            # since our CORREL COPRO can only support up to 5. We make up for the difference using the new SRCSRD decimation
            # block.
            if modtype == model.vars.modulation_type.var_enum.FSK2:
                osr_list = [4, 5]
                min_osr = 4
                max_osr = 5
                min_src2 = 0.79
                max_src2 = 1.0
                min_dec2 = 1
                max_dec2 = 2
                min_bwsel = 0.2
                target_bwsel = 0.4
                max_bwsel = 0.4
            elif modtype == model.vars.modulation_type.var_enum.OQPSK or modtype == model.vars.modulation_type.var_enum.MSK:
                osr_list = [5]
                min_osr = 5
                max_osr = 5
                min_src2 = 0.8
                max_src2 = 1.2
                min_dec2 = 1
                max_dec2 = 1
                min_bwsel = 0.2
                target_bwsel = 0.4
                max_bwsel = 0.4
            elif modtype == model.vars.modulation_type.var_enum.OFDM:
                osr_list = [2] # this is OSR after the SRCSRD decimation and defined as OSR wrt FFT sampling rate
                min_osr = 2
                max_osr = 2
                min_src2 = 0.6
                max_src2 = 1.2
                min_dec2 = 1
                max_dec2 = 2
                min_bwsel = 0.17
                target_bwsel = 0.2
                max_bwsel = 0.4
            else:
                raise CalculationException('ERROR: invalid modtype in sol get_limits')

            # save to use in other functions
            model.vars.min_bwsel.value = min_bwsel  # min value for normalized channel filter bandwidth
            model.vars.max_bwsel.value = max_bwsel  # max value for normalized channel filter bandwidth
            model.vars.min_src2.value = min_src2  # min value for SRC2
            model.vars.max_src2.value = max_src2  # max value for SRC2
            model.vars.max_dec2.value = max_dec2
            model.vars.min_dec2.value = min_dec2

        # Modifying TRECS demod settings from Ocelot by allowing max_src2 to be 1.2 instead of 1.0 as we fixed a HW issue
        elif demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            if relaxsrc2 == True:
                min_src2 = 0.55
                max_src2 = 1.2
            else:
                min_src2 = 0.8
                max_src2 = 1.2
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
            if withremod == True:
                min_dec2 = 1
                max_dec2 = 64
                min_osr = 4
                max_osr = 32
                osr_list = [4, 5, 6, 7]
            elif mi > 2.5:  # FIXME: arbitrary threshold here - for zwave 9.6kbps with mi=2.1 we prefer not to use int/diff path but at some point we will have to
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

            # save to use in other functions
            model.vars.min_bwsel.value = min_bwsel  # min value for normalized channel filter bandwidth
            model.vars.max_bwsel.value = max_bwsel  # max value for normalized channel filter bandwidth
            model.vars.min_src2.value = min_src2  # min value for SRC2
            model.vars.max_src2.value = max_src2  # max value for SRC2
            model.vars.max_dec2.value = max_dec2
            model.vars.min_dec2.value = min_dec2

        else:

            # call ocelot version of this function
            min_bwsel, max_bwsel, min_chfilt_osr, max_chfilt_osr, min_src2, max_src2, min_dec2, max_dec2, min_osr, \
            max_osr, target_bwsel, osr_list = CALC_Demodulator_ocelot.get_limits(self, demod_select, withremod, relaxsrc2, model)

        return min_bwsel, max_bwsel, min_chfilt_osr, max_chfilt_osr, min_src2, max_src2, min_dec2, max_dec2, min_osr, max_osr, target_bwsel, osr_list

    def calc_remoden_reg(self, model):

        demod_select = model.vars.demod_select.value
        osr = model.vars.oversampling_rate_actual.value
        dec2_actual = model.vars.dec2_actual.value
        trecs_enabled = model.vars.trecs_enabled.value

        # Current assumption is that we are going to use the REMOD path only for Viterbi/TRecS
        if trecs_enabled and (osr > 7 or dec2_actual > 1) and demod_select is not model.vars.demod_select.var_enum.SOFT_DEMOD:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_PHDMODCTRL_REMODEN, reg)

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
            dec0_priority_list = [8,4,3,5]
        elif ratio_d4 < 0.27:
            # 0.27 = (.25-0.1825)/.25 corresponds to >60dB attenuation on d0=4 response
            dec0_priority_list = [4,3,5]
        else:
            dec0_priority_list = [3,4,5]

        return dec0_priority_list

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

        if demod_sel==model.vars.demod_select.var_enum.SOFT_DEMOD:
            datafilter_taps = 0
        elif demod_sel==model.vars.demod_select.var_enum.COHERENT and \
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

    def calc_upgaps(self, model):
        #This function calculates the UPGAPS field, controlling clk spacing for SRC interpolated samples
        demod_sel = model.vars.demod_select.value
        src2_ratio = model.vars.src2_ratio.value

        if (demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER or
            demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI) and \
            src2_ratio > 1.0:
            # IPMCUSRW-668: With UPGAPs fix on sol, still a restriction if sample rate exceeds the required
            model.vars.upgaps.value = 1
        else:
            model.vars.upgaps.value = 0

    def calc_upgaps_reg(self, model):
        # This function writes the registers for upgaps
        upgaps = model.vars.upgaps.value
        fefilt_selected = model.vars.fefilt_selected.value

        self._reg_write_by_name_concat(model, fefilt_selected, 'SRC_UPGAPS', upgaps)

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
        TRECS_REQUIRED_CLKS_PER_SAMPLE = 3

        if src_ratio > 1:
            # sample clk is not equi-spaced, interpolated samples have best-case spacing of 1/2 relative to src disabled
            # while using UPGAPS clk delay adjustment in the SRW CMU
            cycles_per_sample = floor(xtal_frequency_hz / src_freq / 2)
        else:
            cycles_per_sample = floor(xtal_frequency_hz / src_freq)

        if cycles_per_sample < TRECS_REQUIRED_CLKS_PER_SAMPLE:
            return False
        else:
            return True

    def calc_target_bandwidth(self, model):

        #Read in model vars
        modulation_type = model.vars.modulation_type.value
        freq_offset_hz = model.vars.freq_offset_hz.value
        ofdm_subcarrier_spacing_hz = model.vars.ofdm_subcarrier_spacing_hz.value
        ofdm_subcarrier_count = model.vars.ofdm_subcarrier_count.value

        #Add case for OFDM
        if modulation_type == model.vars.modulation_type.var_enum.OFDM:

            #Calculate the pre-AFC BW
            #Factor of 1.1 is to account for additional flatness requirement of OFDM
            bandwidth_hz = int((ofdm_subcarrier_spacing_hz * (ofdm_subcarrier_count+1) * 1.1)) + (2 * freq_offset_hz)

            #Write the model var
            model.vars.bandwidth_hz.value = bandwidth_hz
        else:
            super().calc_target_bandwidth(model)

    def calc_target_demod_bandwidth(self, model):

        # Read in model vars
        modulation_type = model.vars.modulation_type.value
        ofdm_subcarrier_spacing_hz = model.vars.ofdm_subcarrier_spacing_hz.value
        ofdm_subcarrier_count = model.vars.ofdm_subcarrier_count.value

        # Add case for OFDM
        if modulation_type == model.vars.modulation_type.var_enum.OFDM:

            # Calculate the bandwidth
            demod_bandwidth_hz = int((ofdm_subcarrier_spacing_hz * (ofdm_subcarrier_count+1) * 1.1)) #Extra factor is for flatness

            # Write the model var
            model.vars.demod_bandwidth_hz.value = demod_bandwidth_hz

        else:
            super().calc_target_demod_bandwidth(model)

    def calc_ofdm_subcarriers_fftsize(self, model):

        #Read in model vars
        modulation_type = model.vars.modulation_type.value
        ofdm_option = model.vars.ofdm_option.value

        #Lookup the number of subcarriers
        if modulation_type == model.vars.modulation_type.var_enum.OFDM:
            if ofdm_option == model.vars.ofdm_option.var_enum.OPT1:
                ofdm_subcarrier_count = 104
                ofdm_fft_size = 128
            elif ofdm_option == model.vars.ofdm_option.var_enum.OPT2:
                ofdm_subcarrier_count = 52
                ofdm_fft_size = 64
            elif ofdm_option == model.vars.ofdm_option.var_enum.OPT3:
                ofdm_subcarrier_count = 26
                ofdm_fft_size = 32
            else:
                ofdm_subcarrier_count = 14
                ofdm_fft_size = 16
        else:
            ofdm_subcarrier_count = 0
            ofdm_fft_size = 0

        #Write the model var
        model.vars.ofdm_subcarrier_count.value = ofdm_subcarrier_count
        model.vars.ofdm_fft_size.value = ofdm_fft_size

    def calc_ofdm_subcarrier_spacing_hz(self, model):

        #Read in model vars
        modulation_type = model.vars.modulation_type.value

        # Calculate the subcarrier spacing
        if modulation_type == model.vars.modulation_type.var_enum.OFDM:
            ofdm_subcarrier_spacing_hz = 1.0/self.ofdm_tsym_useful
        else:
            ofdm_subcarrier_spacing_hz = 0.0

        #Write the model var
        model.vars.ofdm_subcarrier_spacing_hz.value = ofdm_subcarrier_spacing_hz

    def calc_baudrate_actual(self, model, disable_subfrac_divider=False):

        #Read in model vars
        demod_select = model.vars.demod_select.value
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        demod_rate_actual = model.vars.demod_rate_actual.value

        if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OFDM:
            baudrate = demod_rate_actual/2.0
            model.vars.rx_baud_rate_actual.value = baudrate
        else:
            if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
                disable_subfrac_divider=True
            super().calc_baudrate_actual(model, disable_subfrac_divider=disable_subfrac_divider)

    def calc_bwsel(self, model, softmodem_narrowing=False):

        #Read in model vars
        demod_select = model.vars.demod_select.value

        #Allow narrowing for softmodem PHYs
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            #We support narrowing in this case
            softmodem_narrowing=True

        super().calc_bwsel(model, softmodem_narrowing=softmodem_narrowing)

    def calc_lock_bandwidth(self, model, softmodem_narrowing=False):

        # Read in model vars
        demod_select = model.vars.demod_select.value

        # Allow narrowing for softmodem PHYs
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            # We support narrowing in this case
            softmodem_narrowing = True

        super().calc_lock_bandwidth(model, softmodem_narrowing=softmodem_narrowing)

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

        # match HW truncation
        interpolation_gain = floor(interpolation_gain)

        # calculate phase interpolation gain for OQPSK cases
        if modformat == model.vars.modulation_type.var_enum.OQPSK:
            interpolation_gain = 2 ** (ceil(log(interpolation_gain, 2)))

        #Load local variables back into model variables
        model.vars.interpolation_gain_actual.value = float(interpolation_gain)

    def calc_dual_fefilt(self, model):
        #Set to False by default

        model.vars.dual_fefilt.value = False

    def calc_fefilt_dont_cares(self, model):
        #This method reads all of the variables related to the unused FEFILT, and sets them to do not care

        dual_fefilt = model.vars.dual_fefilt.value
        fefilt_selected = model.vars.fefilt_selected.value

        if fefilt_selected == "FEFILT1":
            fefilt_unused = "FEFILT0"
        else:
            fefilt_unused = "FEFILT1"

        if not dual_fefilt:
            for model_var in model.vars:
                if (fefilt_unused in model_var.name) and (model_var.svd_mapping is not None):
                    model_var.value_do_not_care = True

    def calc_digigainctrl_bbss_reg(self, model):

        #Read in model vars
        fefilt_selected = model.vars.fefilt_selected.value

        #Set to 0 for now, this is overridden for coh PHYs that need it
        bbss = 0

        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGIGAINCTRL_BBSS', bbss)

    def calc_dec1gain_actual(self, model):
        #given register settings return actual DEC1GAIN used

        reg = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGIGAINCTRL_DEC1GAIN')

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
        dec0gain = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGIGAINCTRL_DEC0GAIN')
        dec1_actual = model.vars.dec1_actual.value
        dec1gain_actual = model.vars.dec1gain_actual.value
        digigainen = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGIGAINCTRL_DIGIGAINEN')
        digigain = Calc_Utilities_Sol().get_fefilt_actual(model, 'DIGIGAINCTRL_DIGIGAIN')

        #Calculate gains

        dec0_gain_db = 6.0*dec0gain

        dec1_gain_linear = (dec1_actual**4) * (2**(-1*floor(4*log2(dec1_actual)-4)))
        dec1_gain_db = 20*log10(dec1_gain_linear/16) + dec1gain_actual #Normalize so that dec1=0 gives gain=16

        if digigainen:
            digigain_db = -3+(digigain*0.25)
        else:
            digigain_db = 0

        rssi_dig_adjust_db = dec0_gain_db + dec1_gain_db + digigain_db

        #Write the vars
        model.vars.rssi_dig_adjust_db.value = rssi_dig_adjust_db

    def calc_rssi_rf_adjust_db(self, model):
        #Temporary, to be entered based on measured validation data
        model.vars.rssi_rf_adjust_db.value = 0.0

    def calc_phscale_derate_factor(self, model):
        #This function calculates the derating factor for PHSCALE for TRECS PHYs with large freq offset tol

        #Read in model vars
        freq_offset_hz = model.vars.freq_offset_hz.value
        deviation = model.vars.deviation.value
        demod_select = model.vars.demod_select.value
        remoden = (model.vars.MODEM_PHDMODCTRL_REMODEN.value == 1)

        if deviation != 0:
            large_freq_offset = (freq_offset_hz/deviation) > 2
        else:
            large_freq_offset = False

        if large_freq_offset and not remoden and \
            (demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or
            demod_select == model.vars.demod_select.var_enum.TRECS_SLICER):
            phscale_derate_factor = 2
        else:
            phscale_derate_factor = 1

        #Write the model var
        model.vars.phscale_derate_factor.value = phscale_derate_factor
