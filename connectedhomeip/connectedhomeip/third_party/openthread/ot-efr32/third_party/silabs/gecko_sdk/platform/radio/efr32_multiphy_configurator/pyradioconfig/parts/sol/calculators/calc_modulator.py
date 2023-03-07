from pyradioconfig.parts.ocelot.calculators.calc_modulator import CALC_Modulator_Ocelot
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from math import ceil,log2
from enum import Enum

class Calc_Modulator_Sol(CALC_Modulator_Ocelot):

    def buildVariables(self, model):
        super().buildVariables(model)
        self._addModelVariable(model, 'ofdm_rate_hz', float, ModelVariableFormat.DECIMAL,
                               desc='OFDM rate for softmodem')
        self._addModelVariable(model, 'softmodem_tx_interpolation1',int,ModelVariableFormat.DECIMAL,desc='interpolation rate 1 for softmodem TX')
        self._addModelVariable(model, 'softmodem_tx_interpolation2', int, ModelVariableFormat.DECIMAL,
                               desc='interpolation rate 2 for softmodem TX')
        self._addModelVariable(model, 'softmodem_modulator_select', Enum, ModelVariableFormat.DECIMAL,
                               desc='determines modulator path for softmodem PHYs')
        model.vars.softmodem_modulator_select.var_enum = CreateModelVariableEnum(
            'SoftmodemModSelEnum',
            'List of softmodem modulator paths',
            [['IQ_MOD',0,'OFDM modulator'],
            ['LEGACY_MOD',1,'Legacy modulator'],
             ['NONE',2,'Not using softmodem']])
        self._addModelVariable(model, 'softmodem_txircal_params', int, ModelVariableFormat.DECIMAL,
                               desc='TX IRCal parameters [kt, int2ratio, int2gain]', is_array=True)
        self._addModelVariable(model, 'softmodem_txircal_freq', int, ModelVariableFormat.DECIMAL,
                               desc='TX IRCal tone freq')

    def calc_softmodem_modulator_select(self, model):

        softmodem_modulation_type = model.vars.softmodem_modulation_type.value

        if (softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OFDM) or \
            (softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.CW):
            softmodem_modulator_select = model.vars.softmodem_modulator_select.var_enum.IQ_MOD
        elif softmodem_modulation_type != model.vars.softmodem_modulation_type.var_enum.NONE:
            softmodem_modulator_select =  model.vars.softmodem_modulator_select.var_enum.LEGACY_MOD
        else:
            softmodem_modulator_select = model.vars.softmodem_modulator_select.var_enum.NONE

        #Write the model var
        model.vars.softmodem_modulator_select.value = softmodem_modulator_select

    def calc_txmodsel_reg(self, model):

        #Read in model vars
        softmodem_modulator_select = model.vars.softmodem_modulator_select.value

        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD:
            txmodsel = 0
        elif softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.LEGACY_MOD:
            txmodsel = 1
        else:
            txmodsel = 0

        #Write the register
        self._reg_write(model.vars.RAC_SOFTMCTRL_TXMODSEL, txmodsel)

    def calc_softmodem_tx_interpolation(self, model):
        #This method calculates the interpolation rates for softmodem PHYs

        #Read in model vars
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        ofdm_option = model.vars.ofdm_option.value
        softmodem_modulator_select = model.vars.softmodem_modulator_select.value

        #Only need to set interpolation values for OFDM MOD
        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD:
            if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OFDM:
                softmodem_tx_interpolation1 = 7 #Static for now
                if ofdm_option == model.vars.ofdm_option.var_enum.OPT1:
                    softmodem_tx_interpolation2 = 2
                elif ofdm_option == model.vars.ofdm_option.var_enum.OPT2:
                    softmodem_tx_interpolation2 = 4
                elif ofdm_option == model.vars.ofdm_option.var_enum.OPT3:
                    softmodem_tx_interpolation2 = 8
                else:
                    softmodem_tx_interpolation2 = 16
            else:
                softmodem_tx_interpolation1 = 1
                softmodem_tx_interpolation2 = 5
        else:
            softmodem_tx_interpolation1 = 0
            softmodem_tx_interpolation2 = 0

        #Write the model vars
        model.vars.softmodem_tx_interpolation1.value = softmodem_tx_interpolation1
        model.vars.softmodem_tx_interpolation2.value = softmodem_tx_interpolation2


    def calc_int1cfg_reg(self, model):
        #This method calculates the int1cfg register fields

        #Read in model vars
        softmodem_modulator_select = model.vars.softmodem_modulator_select.value
        softmodem_tx_interpolation1 = model.vars.softmodem_tx_interpolation1.value

        # Set only when OFDM modulator is used
        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD:
            ratio = softmodem_tx_interpolation1-1
            gainshift = 12 #Static for now
        else:
            ratio=0
            gainshift=0

        #Write the registers
        self._reg_write(model.vars.TXFRONT_INT1CFG_RATIO,ratio)
        self._reg_write(model.vars.TXFRONT_INT1CFG_GAINSHIFT, gainshift)

    def calc_int2cfg_reg(self, model):
        #This method calculates the int2cfg register fields

        #Read in model vars
        softmodem_modulator_select = model.vars.softmodem_modulator_select.value
        softmodem_tx_interpolation2 = model.vars.softmodem_tx_interpolation2.value

        # Set only when OFDM modulator is used
        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD:
            ratio = softmodem_tx_interpolation2-1
            gainshift = ceil(log2(softmodem_tx_interpolation2**2))
        else:
            ratio = 0
            gainshift = 0

        #Write the registers
        self._reg_write(model.vars.TXFRONT_INT2CFG_RATIO, ratio)
        self._reg_write(model.vars.TXFRONT_INT2CFG_GAINSHIFT, gainshift)

    def calc_softmodem_txircal(self, model):

        #Read in model vars
        divn = model.vars.fpll_divn.value
        divx = model.vars.fpll_divx.value

        #divn: [kt, int2_ratio, int2_gainshift]
        txircal_dict = {
            80:[5,5,5],
            81:[9,9,7],
            85:[5,17,9],
            86:[5,43,11],
            93:[3,31,10],
            97:[2,97,14],
            98:[7,7,6],
            99:[9,11,7],
            100:[5,5,5],
            101:[2,101,14],
            103:[2,103,14],
            111:[3,37,11],
            114:[3,19,9],
            115:[5,23,10],
            116:[5,29,10],
            117:[9,13,8],
            119:[3,59,12]
        }

        try:
            softmodem_txircal_params = txircal_dict[divn]
        except KeyError:
            LogMgr.Error('Invalid RFFPLL divn for softmodem tx calibration')
            softmodem_txircal_params = []
            softmodem_txircal_freq = 0
        else:
            fft_size_log2 = 6
            txircal_dec0 = 4
            softmodem_txircal_freq = ((1 << (17 - fft_size_log2)) * softmodem_txircal_params[0] * divx * softmodem_txircal_params[1]) / txircal_dec0 / divn

        model.vars.softmodem_txircal_params.value = softmodem_txircal_params
        model.vars.softmodem_txircal_freq.value = int(softmodem_txircal_freq)

    def calc_srccfg_ratio_reg(self, model):
        #This method calulates the softmodem SRCCFG RATIO value

        #Read in model vars
        softmodem_modulator_select = model.vars.softmodem_modulator_select.value
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        dac_freq_actual = model.vars.dac_freq_actual.value
        baudrate = model.vars.baudrate.value
        softmodem_tx_interpolation1 = model.vars.softmodem_tx_interpolation1.value
        softmodem_tx_interpolation2 = model.vars.softmodem_tx_interpolation2.value

        # Set only when OFDM modulator is used
        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD:
            if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OFDM:
                ratio = (2**18) * (2.0*baudrate*softmodem_tx_interpolation1*softmodem_tx_interpolation2)/dac_freq_actual #2^18 * (2*OFDM_RATE*INT1*INT2)/DAC_FREQ
            else:
                ratio = (2**18) #Ratio of 1 for CW
        else:
            ratio = 0

        #Write the reg
        self._reg_write(model.vars.TXFRONT_SRCCFG_RATIO, int(ratio))

    def calc_tx_baud_rate_actual(self, model):

        #Read in model vars
        softmodem_modulator_select = model.vars.softmodem_modulator_select.value
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        softmodem_tx_interpolation1_actual = model.vars.TXFRONT_INT1CFG_RATIO.value + 1
        softmodem_tx_interpolation2_actual = model.vars.TXFRONT_INT2CFG_RATIO.value + 1
        txfront_srccfg_ratio_actual = model.vars.TXFRONT_SRCCFG_RATIO.value/(2**18)
        dac_freq_actual = model.vars.dac_freq_actual.value

        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD and\
                softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_OFDM:
            tx_baud_rate_actual = dac_freq_actual/softmodem_tx_interpolation1_actual/softmodem_tx_interpolation2_actual*txfront_srccfg_ratio_actual/2.0
            model.vars.tx_baud_rate_actual.value = tx_baud_rate_actual
        else:
            super().calc_tx_baud_rate_actual(model)

    def calc_txmix_regs(self, model):
        #This method calculates the RAC_TXMIX fields as well as the RAC_PATRIM6_TXTRIMFILGAIN field

        softmodem_modulator_select = model.vars.softmodem_modulator_select.value
        base_frequency_hz = model.vars.base_frequency_hz.value

        #Choose regsiter settings based on RF band
        if softmodem_modulator_select == model.vars.softmodem_modulator_select.var_enum.IQ_MOD:

            if base_frequency_hz < 450e6:
                #430M Band
                txtrimfilgain = 2
                txselmixctune = 43
                txselmixgmslicei = 9
                txselmixgmsliceq = 9
                txselmixrload = 0
                txselmixband = 0
                txmixcappulldown = 1

            elif base_frequency_hz < 520e6:
                # 470M Band
                txtrimfilgain = 2
                txselmixctune = 31
                txselmixgmslicei = 8
                txselmixgmsliceq = 8
                txselmixrload = 0
                txselmixband = 0
                txmixcappulldown = 1

            elif base_frequency_hz < 625e6:
                # 570M Band
                txtrimfilgain = 2
                txselmixctune = 13
                txselmixgmslicei = 6
                txselmixgmsliceq = 6
                txselmixrload = 2
                txselmixband = 0
                txmixcappulldown = 1

            elif base_frequency_hz < 730e6:
                # 680M Band
                txtrimfilgain = 2
                txselmixctune = 3
                txselmixgmslicei = 6
                txselmixgmsliceq = 6
                txselmixrload = 5
                txselmixband = 0
                txmixcappulldown = 1

            elif base_frequency_hz < 825e6:
                # 780M Band
                txtrimfilgain = 2
                txselmixctune = 24
                txselmixgmslicei = 7
                txselmixgmsliceq = 7
                txselmixrload = 1
                txselmixband = 1
                txmixcappulldown = 0

            elif base_frequency_hz < 895e6:
                # 870M Band
                txtrimfilgain = 2
                txselmixctune = 14
                txselmixgmslicei = 6
                txselmixgmsliceq = 6
                txselmixrload = 2
                txselmixband = 1
                txmixcappulldown = 0

            elif base_frequency_hz < 940e6:
                # 920M Band (settings from Eric Vapillon)
                txtrimfilgain = 2
                txselmixctune = 9
                txselmixgmslicei = 6
                txselmixgmsliceq = 6
                txselmixrload = 4
                txselmixband = 1
                txmixcappulldown = 0

            else:
                # 960M Band
                txtrimfilgain = 2
                txselmixctune = 5
                txselmixgmslicei = 6
                txselmixgmsliceq = 6
                txselmixrload = 5
                txselmixband = 1
                txmixcappulldown = 0

        else:
            #Use POR values
            txtrimfilgain = 1
            txselmixctune = 0
            txselmixgmslicei = 4
            txselmixgmsliceq = 4
            txselmixrload = 0
            txselmixband = 0
            txmixcappulldown = 0

        #Write the register fields
        self._reg_write(model.vars.RAC_PATRIM6_TXTRIMFILGAIN, txtrimfilgain)
        self._reg_write(model.vars.RAC_TXMIX_TXSELMIXCTUNE, txselmixctune)
        self._reg_write(model.vars.RAC_TXMIX_TXSELMIXGMSLICEI, txselmixgmslicei)
        self._reg_write(model.vars.RAC_TXMIX_TXSELMIXGMSLICEQ, txselmixgmsliceq)
        self._reg_write(model.vars.RAC_TXMIX_TXSELMIXRLOAD, txselmixrload)
        self._reg_write(model.vars.RAC_TXMIX_TXSELMIXBAND, txselmixband)
        self._reg_write(model.vars.RAC_TXMIX_TXMIXCAPPULLDOWN, txmixcappulldown)

    def calc_symbol_rates_actual(self, model):
        modulation_type = model.vars.modulation_type.value
        max_bit_rate = model.vars.bitrate.value #We already store the max bitrate here

        if (modulation_type == model.vars.modulation_type.var_enum.OFDM):
            # Symbol rate is constant for OFDM: 1/120us
            ofdm_tsym = 120e-6
            ofdm_symbol_rate = 1/ofdm_tsym

            # baud per symbol is not used in OFDM
            baud_per_symbol = 1

            # bits_per_symbol corresponds to the maximum bit rate (MCS6) for a given option over the symbol rate:
            bits_per_symbol = int(max_bit_rate / ofdm_symbol_rate)

            # Update model variables
            model.vars.ofdm_symbol_rate.value = ofdm_symbol_rate
            model.vars.baud_per_symbol_actual.value = baud_per_symbol
            model.vars.bits_per_symbol_actual.value = bits_per_symbol
        else:
            # Call Ocelot version
            super().calc_symbol_rates_actual(model)

    def calc_txbases_reg(self, model):

        # Read in model variables
        preamble_length = model.vars.preamble_length.value  # This is the TX preamble length
        preamble_pattern_len_actual = model.vars.preamble_pattern_len_actual.value
        softmodem_active = (model.vars.softmodem_modulation_type.value != model.vars.softmodem_modulation_type.var_enum.NONE)

        if softmodem_active:
            #If the softmodem is active, the preamble bits will come from the softmodem so set TXBASES=0
            txbases = 0
        else:
            txbases = preamble_length / preamble_pattern_len_actual
            if (txbases) > 0xffff:
                LogMgr.Error("Calculated TX preamble sequences (TXBASE) value of %s exceeds limit of 65535! Adjust preamble inputs." % txbases)

        # Write the register
        self._reg_write(model.vars.MODEM_PRE_TXBASES, int(txbases))
