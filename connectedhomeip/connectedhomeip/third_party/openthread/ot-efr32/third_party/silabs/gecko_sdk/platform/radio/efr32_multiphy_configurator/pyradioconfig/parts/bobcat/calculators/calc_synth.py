from pyradioconfig.parts.ocelot.calculators.calc_synth import CALC_Synth_ocelot
from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

class Calc_Synth_Bobcat(CALC_Synth_ocelot):

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator
        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Build variables from Lynx
        calc_synth_ocelot_obj = CALC_Synth_ocelot()
        calc_synth_ocelot_obj.buildVariables(model)

        # : Modify synth RX modes from Ocelot
        model.vars.synth_rx_mode.var_enum = CreateModelVariableEnum(
            enum_name='SynthRxModeEnum',
            enum_desc='Defined Synth RX Mode',
            member_data=[
                ['MODE1', 0, 'RX Mode 1'],
                ['MODE2', 1, 'RX Mode 2'],
            ])
        model.vars.synth_rx_mode_actual.var_enum = model.vars.synth_rx_mode.var_enum

        # : Modify synth settling modes from Ocelot
        model.vars.synth_settling_mode.var_enum = CreateModelVariableEnum(
            enum_name='SynthSettlingMode',
            enum_desc='Synth Settling Mode',
            member_data=[
                ['NORMAL', 0, 'Normal Operation Mode (Recommended)'],
                ['BLE_LR', 1, 'BLE Longrange Mode'],
            ])

        # : Modify tx synth modes from Ocelot
        model.vars.synth_tx_mode.var_enum = CreateModelVariableEnum(
            enum_name='SynthTxModeEnum',
            enum_desc='Defined Synth TX Mode',
            member_data=[
                ['MODE1', 0, 'TX Mode 1'],
                ['MODE2', 1, 'TX Mode 2'],
                ['MODE3', 2, 'TX Mode 3'],
                ['MODE4', 3, 'TX Mode 4'],
                ['MODE_BLE', 0, 'TX BLE Mode'],
                ['MODE_BLE_FULLRATE', 1, 'TX BLE Fullrate Mode'],
                ['MODE_IEEE802154', 1, 'TX IEEE802154 Mode'],
            ])
        model.vars.synth_tx_mode_actual.var_enum = model.vars.synth_tx_mode.var_enum

    def calc_synth_settling_mode(self, model):
        #Set synth settling mode to Normal for now
        model.vars.synth_settling_mode.value = model.vars.synth_settling_mode.var_enum.NORMAL

    def calc_rx_mode(self, model):
        synth_settling_mode = model.vars.synth_settling_mode.value

        if synth_settling_mode == model.vars.synth_settling_mode.var_enum.BLE_LR:
            model.vars.synth_rx_mode.value = model.vars.synth_rx_mode.var_enum.MODE1
        else:
            model.vars.synth_rx_mode.value = model.vars.synth_rx_mode.var_enum.MODE2

    def calc_tx_mode(self, model):
        baudrate = model.vars.baudrate.value
        modulation_type = model.vars.modulation_type.value

        # Set FSK and OQPSK settings based on baudrate
        if modulation_type == model.vars.modulation_type.var_enum.FSK2 \
                or modulation_type == model.vars.modulation_type.var_enum.FSK4 \
                or modulation_type == model.vars.modulation_type.var_enum.OQPSK:
            if baudrate > 1250e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE4  # 3 MHz
            elif baudrate > 1000e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE3  # 2.5 MHz
            elif baudrate > 500e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE2  # 1.5 MHz
            else:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE1  # 1 MHz
        # Fixed settings for OOK, DBPSK, BPSK - transferred from Ocelot
        if modulation_type == model.vars.modulation_type.var_enum.DBPSK:
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE4  # 3 MHz
        elif modulation_type == model.vars.modulation_type.var_enum.BPSK:
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE3  # 2.5 MHz
        elif modulation_type == model.vars.modulation_type.var_enum.OOK:
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE1  # 1000 kHz

    # overwrite this function from Ocelot to make VCODIV the default clock for ADC
    def calc_adc_clock_config(self, model):
        # This function calculates both the ADC mode (e.g. fullrate, halfrate, etc) as well as the ADC clock divider path

        # Load model values into local variables
        bandwidth_hz = model.vars.bandwidth_hz.value

        if (bandwidth_hz < 1.25e6):
            # 1/2 rate mode
            # Use the HFXO along with DPLL for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.HALFRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.VCODIV
        else:
            # Full rate mode
            # Use the divided down VCO for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.FULLRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.VCODIV

        # Load local variables back into model variables
        model.vars.adc_clock_mode.value = adc_clock_mode
        model.vars.adc_rate_mode.value = adc_rate_mode

    def calc_sylodivrloadcclk_reg(self, model):
        adc_rate_mode = model.vars.adc_rate_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_SYLOEN_SYLODIVRLOADCCLKSEL, reg)

    def calc_sytrim0_sychpcurrtx_reg(self, model):
        pass

    def calc_rx_mode_reg(self, model):

        rx_mode = model.vars.synth_rx_mode.value
        ind = rx_mode.value

        # Synth settings https://jira.silabs.com/browse/MCUW_RADIO_CFG-1529
        # Settings copied over from Lynx Assert
        # {workspace}\shared_files\lynx\radio_validation\ASSERTS
        # BLK_SYNTH_RX_LP_BW_200KHZ.csv (BLE_LR mode)
        # BLK_SYNTH_RX_LP_BW_250KHZ.csv (NORMAL mode)

        rx_mode_settings = {
            'SYNTH.LPFCTRL2RX.CASELRX':         [1,   1],
            'SYNTH.LPFCTRL2RX.CAVALRX':         [22,  16],
            'SYNTH.LPFCTRL2RX.CZSELRX':         [1,   1],
            'SYNTH.LPFCTRL2RX.CZVALRX':         [254, 229],
            'SYNTH.LPFCTRL2RX.CFBSELRX':        [0,   0],
            'SYNTH.LPFCTRL2RX.LPFGNDSWENRX':    [0,   0],
            'SYNTH.LPFCTRL2RX.MODESELRX':       [0,   0],
            'SYNTH.LPFCTRL1RX.OP1BWRX':         [0,   0],
            'SYNTH.LPFCTRL1RX.OP1COMPRX':       [7,   7],
            'SYNTH.LPFCTRL1RX.RFBVALRX':        [0,   0],
            'SYNTH.LPFCTRL1RX.RPVALRX':         [7,   7],
            'SYNTH.LPFCTRL1RX.RZVALRX':         [12,  13],
            'SYNTH.LPFCTRL2RX.LPFSWENRX':       [1,   1],
            'SYNTH.LPFCTRL2RX.LPFINCAPRX':      [2,   2],
            'SYNTH.DSMCTRLRX.MASHORDERRX':      [1,   1],
            'SYNTH.DSMCTRLRX.LSBFORCERX':       [1,   1],
            'SYNTH.DSMCTRLRX.DSMMODERX':        [1,   1],
            'SYNTH.DSMCTRLRX.DITHERDACRX':      [0,   0],
            'SYNTH.DSMCTRLRX.DITHERDSMOUTPUTRX':[0,   0],
            'SYNTH.DSMCTRLRX.DITHERDSMINPUTRX': [0,   0],
            'RAC.SYMMDCTRL.SYMMDMODERX':        [4,   4],
            'RAC.SYTRIM1.SYLODIVLDOTRIMNDIORX': [1,   1],
            'RAC.SYEN.SYCHPLPENRX':             [1,   1],
            'RAC.SYTRIM0.SYCHPREPLICACURRADJ':  [1,   1],
            'RAC.SYTRIM0.SYCHPSRCENRX':         [0,   0]
        }

        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CASELRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CASELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CAVALRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CAVALRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CZSELRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CZSELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CZVALRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CZVALRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CFBSELRX,         rx_mode_settings['SYNTH.LPFCTRL2RX.CFBSELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_LPFGNDSWENRX,     rx_mode_settings['SYNTH.LPFCTRL2RX.LPFGNDSWENRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_MODESELRX,        rx_mode_settings['SYNTH.LPFCTRL2RX.MODESELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_OP1BWRX,          rx_mode_settings['SYNTH.LPFCTRL1RX.OP1BWRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_OP1COMPRX,        rx_mode_settings['SYNTH.LPFCTRL1RX.OP1COMPRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_RFBVALRX,         rx_mode_settings['SYNTH.LPFCTRL1RX.RFBVALRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_RPVALRX,          rx_mode_settings['SYNTH.LPFCTRL1RX.RPVALRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_RZVALRX,          rx_mode_settings['SYNTH.LPFCTRL1RX.RZVALRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_LPFSWENRX,        rx_mode_settings['SYNTH.LPFCTRL2RX.LPFSWENRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_LPFINCAPRX,       rx_mode_settings['SYNTH.LPFCTRL2RX.LPFINCAPRX'][ind])

        # : Following registers are PTE Set & Forget but needs to be set by RC since they are different from reset value
        # : See https://jira.silabs.com/browse/MCUW_RADIO_CFG-1610
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_MASHORDERRX,       rx_mode_settings['SYNTH.DSMCTRLRX.MASHORDERRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_LSBFORCERX,        rx_mode_settings['SYNTH.DSMCTRLRX.LSBFORCERX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DSMMODERX,         rx_mode_settings['SYNTH.DSMCTRLRX.DSMMODERX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DITHERDACRX,       rx_mode_settings['SYNTH.DSMCTRLRX.DITHERDACRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DITHERDSMOUTPUTRX, rx_mode_settings['SYNTH.DSMCTRLRX.DITHERDSMOUTPUTRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DITHERDSMINPUTRX,  rx_mode_settings['SYNTH.DSMCTRLRX.DITHERDSMINPUTRX'][ind])
        self._reg_write(model.vars.RAC_SYMMDCTRL_SYMMDMODERX,         rx_mode_settings['RAC.SYMMDCTRL.SYMMDMODERX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMNDIORX,  rx_mode_settings['RAC.SYTRIM1.SYLODIVLDOTRIMNDIORX'][ind])
        self._reg_write(model.vars.RAC_SYEN_SYCHPLPENRX,              rx_mode_settings['RAC.SYEN.SYCHPLPENRX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPREPLICACURRADJ,   rx_mode_settings['RAC.SYTRIM0.SYCHPREPLICACURRADJ'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPSRCENRX,          rx_mode_settings['RAC.SYTRIM0.SYCHPSRCENRX'][ind])

    def calc_tx_mode_reg(self, model):

        tx_mode = model.vars.synth_tx_mode.value
        ind = tx_mode.value

        # Synth settings https://jira.silabs.com/browse/MCUW_RADIO_CFG-1529
        # Settings copied over from Lynx Assert
        # {workspace}\shared_files\lynx\radio_validation\ASSERTS
        # BLK_SYNTH_TX_BW_1000KHZ.csv (MODE 1)
        # BLK_SYNTH_TX_BW_1500KHZ.csv (MODE 2)
        # BLK_SYNTH_TX_BW_2500KHZ.csv (MODE 3)
        # BLK_SYNTh_TX_BW_3000KHZ.csv (MODE 4)

        tx_mode_settings = {
            'SYNTH.LPFCTRL2TX.CASELTX':         [1,   1,   1,   1],
            'SYNTH.LPFCTRL2TX.CAVALTX':         [15,  8,   4,   2],
            'SYNTH.LPFCTRL2TX.CZSELTX':         [1,   1,   1,   1],
            'SYNTH.LPFCTRL2TX.CZVALTX':         [126, 70,  22,  6],
            'SYNTH.LPFCTRL2TX.CFBSELTX':        [0,   0,   0,   0],
            'SYNTH.LPFCTRL2TX.LPFGNDSWENTX':    [0,   0,   0,   0],
            'SYNTH.LPFCTRL2TX.MODESELTX':       [0,   0,   0,   0],
            'SYNTH.LPFCTRL1TX.OP1BWTX':         [0,   5,   11,  15],
            'SYNTH.LPFCTRL1TX.OP1COMPTX':       [13,  13,  14,  15],
            'SYNTH.LPFCTR1TX.RFBVALTX':         [0,   0,   0,   0],
            'SYNTH.LPFCTRL1TX.RPVALTX':         [0,   0,   0,   0],
            'SYNTH.LPFCTRL1TX.RZVALTX':         [0,   3,   9,   11],
            'SYNTH.LPFCTRL2TX.LPFSWENTX':       [1,   1,   1,   1],
            'SYNTH.LPFCTRL2TX.LPFINCAPTX':      [2,   2,   2,   2],
            'SYNTH.DSMCTRLTX.DEMMODETX':        [1,   1,   1,   1],
            'SYNTH.DSMCTRLTX.DITHERDACTX':      [3,   3,   3,   3],
            'SYNTH.DSMCTRLTX.DITHERDSMOUTPUTTX':[3,   3,   3,   3]
        }

        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CASELTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CASELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CAVALTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CAVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CZSELTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CZSELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CZVALTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CZVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CFBSELTX,           tx_mode_settings['SYNTH.LPFCTRL2TX.CFBSELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_LPFGNDSWENTX,       tx_mode_settings['SYNTH.LPFCTRL2TX.LPFGNDSWENTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_MODESELTX,          tx_mode_settings['SYNTH.LPFCTRL2TX.MODESELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_OP1BWTX,            tx_mode_settings['SYNTH.LPFCTRL1TX.OP1BWTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_OP1COMPTX,          tx_mode_settings['SYNTH.LPFCTRL1TX.OP1COMPTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_RFBVALTX,           tx_mode_settings['SYNTH.LPFCTR1TX.RFBVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_RPVALTX,            tx_mode_settings['SYNTH.LPFCTRL1TX.RPVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_RZVALTX,            tx_mode_settings['SYNTH.LPFCTRL1TX.RZVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_LPFSWENTX,          tx_mode_settings['SYNTH.LPFCTRL2TX.LPFSWENTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_LPFINCAPTX,         tx_mode_settings['SYNTH.LPFCTRL2TX.LPFINCAPTX'][ind])

        # : Following registers are PTE Set & Forget but needs to be set by RC since they are different from reset value
        # : See https://jira.silabs.com/browse/MCUW_RADIO_CFG-1610
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DEMMODETX,           tx_mode_settings['SYNTH.DSMCTRLTX.DEMMODETX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DITHERDACTX,         tx_mode_settings['SYNTH.DSMCTRLTX.DITHERDACTX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DITHERDSMOUTPUTTX,   tx_mode_settings['SYNTH.DSMCTRLTX.DITHERDSMOUTPUTTX'][ind])

    def calc_boostlna_reg(self, model):

        #Read in model varsF
        lo_injection_side = model.vars.lo_injection_side.value

        # Calculating the boostlna value
        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            boostlna = 1
        else:
            boostlna = 0

        # Write the register
        self._reg_write(model.vars.AGC_LNABOOST_BOOSTLNA, boostlna)
        
    def calc_hfxo_retiming_table(self, model):
        # Inherit Ocelot retiming
        super().calc_hfxo_retiming_table(model)

        #setting for 2x HFXO frequency
        self.retime_print("calculating 2x HFXO freq")
        model.vars.lut_table_index.value.append(0)
        self.retime_main(model, model.vars.xtal_frequency_hz.value * 2, 1, model.vars.lut_table_index.value[0])

    def calc_synth_misc(self, model):

        # Setting this value to POR
        self._reg_write_default(model.vars.SYNTH_LPFCTRL2TX_VCMLVLTX)

        # : Set this value to 3 regardless of modem in OFF/TX/RX states
        # : https://jira.silabs.com/browse/MCUW_RADIO_CFG-1745
        self._reg_write(model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM, 3)

        # Fairly confident that these CAL values are not actually used, but Design can not confirm these are unused.
        # So if the value isn't forced then these are a do not care.
        if model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL)

    def calc_txramp_txmodephaseflip_reg(self, model):
        pass

    def calc_adc_rate_mode_reg(self,model):
        pass

    def calc_adc_freq_actual(self,model):
        #This function calculates the actual ADC sample frequency and error based on the registers

        #Load model variables into local variables
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value
        adc_vco_div_actual = model.vars.adc_vco_div_actual.value
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        fsynth = model.vars.rx_synth_freq_actual.value
        fadc_target = model.vars.adc_target_freq.value #The target frequency is used for computing error
        ifadc_halfrate = model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value

        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            #the ordering of the if statements is important - keep ifadcpll_en_xo_bypass on top
            if 1 == ifadc_halfrate:
                adc_freq_actual = xtal_frequency_hz * 4
            else:
                adc_freq_actual = xtal_frequency_hz * 8
        else:
            adc_freq_actual = int(fsynth / adc_vco_div_actual)

        # Compute the final ADC frequency percent error
        ferror = 100 * (fadc_target - adc_freq_actual) / float(fadc_target)

        #Load local variables back into model variables
        model.vars.adc_freq_actual.value = adc_freq_actual
        model.vars.adc_freq_error.value = ferror

    def calc_ifadcplldcofilter_reg(self, model):
        pass

    def calc_ifadcpolldcobiashalf_reg(self, model):
        pass

    def calc_ifadctrim1_reg(self, model):
        negres = 1
        self._reg_write(model.vars.RAC_IFADCTRIM1_IFADCENNEGRES, negres)

    def calc_adc_vco_div_reg(self,model):
        pass

    def calc_adc_vco_div_actual(self, model):
        adc_vco_div = model.vars.adc_vco_div.value
        model.vars.adc_vco_div_actual.value = adc_vco_div
    def calc_pga_lna_bw_reg(self, model):
        # : Bobcat calculation based on Lynx BW configuration of 5 MHz for all PHYs
        # : Revert ocelot change that used narrower bandwidth in the baseband to improve blocking performance
        # : https://jira.silabs.com/browse/MCUW_RADIO_CFG-1692

        lnaboost_boostlna = model.vars.AGC_LNABOOST_BOOSTLNA.value
        pgabwmode = 1

        if lnaboost_boostlna == 1:
            lnabwadj = 0
            lnabwadjboost = 0
        else:
            lnabwadj = 1
            lnabwadjboost = 1

        self._reg_write(model.vars.RAC_PGACTRL_PGABWMODE, pgabwmode)
        self._reg_write(model.vars.AGC_LNABOOST_LNABWADJ, lnabwadj)
        self._reg_write(model.vars.AGC_LNABOOST_LNABWADJBOOST, lnabwadjboost)

    def calc_clkmult_div_reg(self, model):
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value
        ifadc_halfrate = model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value

        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            if ifadc_halfrate == 0:
                # adc_full_speed from dpll_utils.py (xo * 8); 8 = 48 / (3 * 2)
                self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVR, 1)
                self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVN, 48)
                self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVX, 3)
                self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTFREQCAL, 1)
                self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTBWCAL, 1)
            else:
                # adc_half_speed from dpll_utils.py (xo * 4); 4 = 40 / (5 * 2)
                self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVR, 1)
                self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVN, 40)
                self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVX, 5)
                self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTFREQCAL, 0)
                self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTBWCAL, 0)
        else:
            # reset values
            self._reg_write_default(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVR)
            self._reg_write_default(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVN)
            self._reg_write_default(model.vars.RAC_CLKMULTCTRL_CLKMULTDIVX)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTFREQCAL)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTBWCAL)

    def calc_clkmulten_reg(self, model):
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value

        # To enable clkmult the following additional registers must also be set, but are handled in RAIL
        # SYXO0.INTERNALCTRL.ENCLKMULTANA = 1 # enable XO output to CLKMULT
        # RAC.SYLOEN.SYLODIVRLO2P4GENEN = 0 # disable LODIV output buffer from SYLODIV (power saving)

        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            # unless otherwise specified, the values are taken from dualbclk_mult_validation_20190516_lynx_revA0.pptx > dpll_utils.py > dualbclk_mult spec sheet
            # based on the common settings for adc_full_speed, adc_full_speed_lp, adc_half_speed, adc_half_speed_lp
            # in dpll_utils.py
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTDISICO, 0) # 0 = ENABLE ICO, 1 = DISABLE ICO
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENBBDET, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENBBXLDET, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENBBXMDET, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENCFDET, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDITHER, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVADC, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVN, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVP, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVRX2P4G, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVRXSUBG, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVTXDUALB, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENFBDIV, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENREFDIV, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENREG1, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENREG2, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENREG3, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENROTDET, 1)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTENBYPASS40MHZ, 0)

            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTREG1ADJV, 2)
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTREG2ADJV, 3) # from dpll_utils.py
            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTREG2ADJI, 1) # from dpll_utils.py

            self._reg_write(model.vars.RAC_CLKMULTEN0_CLKMULTREG3ADJV, 2)

            self._reg_write(model.vars.RAC_CLKMULTEN1_CLKMULTINNIBBLE, 8)
            self._reg_write(model.vars.RAC_CLKMULTEN1_CLKMULTLDFNIB, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN1_CLKMULTLDMNIB, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN1_CLKMULTRDNIBBLE, 3)
            self._reg_write(model.vars.RAC_CLKMULTEN1_CLKMULTLDCNIB, 0)
            self._reg_write(model.vars.RAC_CLKMULTEN1_CLKMULTDRVAMPSEL, 7) # based on dpll_utils.py

            self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTENRESYNC, 0)
            self._reg_write(model.vars.RAC_CLKMULTCTRL_CLKMULTVALID, 0)
        else:
            # when using lodiv, turn off dualbclk_mult to reset values
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTDISICO)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENBBDET)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENBBXLDET)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENBBXMDET)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENCFDET)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDITHER)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVADC)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVN)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVP)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVRX2P4G)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVRXSUBG)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVTXDUALB)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENFBDIV)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENREFDIV)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENREG1)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENREG2)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENREG3)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENROTDET)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENBYPASS40MHZ)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTREG1ADJV)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTREG2ADJV)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTREG2ADJI)
            self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTREG3ADJV)

            self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTINNIBBLE)
            self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTLDFNIB)
            self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTLDMNIB)
            self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTRDNIBBLE)
            self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTLDCNIB)
            self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTDRVAMPSEL)

            self._reg_write_default(model.vars.RAC_CLKMULTCTRL_CLKMULTENRESYNC)
            self._reg_write_default(model.vars.RAC_CLKMULTCTRL_CLKMULTVALID)