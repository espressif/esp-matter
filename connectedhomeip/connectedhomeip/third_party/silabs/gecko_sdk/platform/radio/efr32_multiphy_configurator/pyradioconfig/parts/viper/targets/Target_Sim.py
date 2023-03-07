from pyradioconfig.parts.bobcat.targets.Target_Sim import Target_Sim_Bobcat
from os.path import join

class Target_Sim_Viper(Target_Sim_Bobcat):

    _targetName = "Sim"
    _description = "Supports the wired FPGA and other targets of sim PHYs"
    _store_config_output = True
    _cfg_location = join('target_sim','viper')
    _tag = "SIM"

    def MODEM_SHAPING_OVERRIDE(self, model):
        # always overwrite the calculator shaping for BLE PHYs
        model.vars.MODEM_CTRL0_SHAPING.value_forced = 3

        model.vars.MODEM_SHAPING0_COEFF0.value_forced = 6
        model.vars.MODEM_SHAPING0_COEFF1.value_forced = 15
        model.vars.MODEM_SHAPING0_COEFF2.value_forced = 30
        model.vars.MODEM_SHAPING0_COEFF3.value_forced = 52

        model.vars.MODEM_SHAPING1_COEFF4.value_forced = 76
        model.vars.MODEM_SHAPING1_COEFF5.value_forced = 97
        model.vars.MODEM_SHAPING1_COEFF6.value_forced = 112
        model.vars.MODEM_SHAPING1_COEFF7.value_forced = 120

        model.vars.MODEM_SHAPING2_COEFF8.value_forced = 120
        model.vars.MODEM_SHAPING2_COEFF9.value_forced = 112
        model.vars.MODEM_SHAPING2_COEFF10.value_forced = 97
        model.vars.MODEM_SHAPING2_COEFF11.value_forced = 76

        model.vars.MODEM_SHAPING3_COEFF12.value_forced = 52
        model.vars.MODEM_SHAPING3_COEFF13.value_forced = 30
        model.vars.MODEM_SHAPING3_COEFF14.value_forced = 15
        model.vars.MODEM_SHAPING3_COEFF15.value_forced = 6

    def target_calculate(self, model):

        #Always use fixed length in sim results
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.FIXED_LENGTH

        #Force MMD LDO TRIM values
        model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.value_forced = 3

        # Force Synth settings
        # FPGA Sim does not support the same synth settings as Bobcat Silicon
        model.vars.SYNTH_LPFCTRL2RX_CZVALRX.value_forced = 128
        model.vars.SYNTH_LPFCTRL1RX_RZVALRX.value_forced = 14
        model.vars.SYNTH_LPFCTRL2RX_CAVALRX.value_forced = 16

        # Not supported by Wired FPGA
        self.FRC_DFLCTRL_DISABLE(model)
        self.FRC_FCD_DISABLE(model)

        # : Set base frequency such that LO is integer multiple of xo frequency
        # : xo_frequency | base frequency | rx_synth_frequency
        # : 39 MHz       | 2494630023     | 2496000000
        model.vars.base_frequency_hz.value_forced = 2494630023

        # : PHY-specific rules (to be replaced globally once we figure out how calculate where they need to apply)
        # : Bluetooth PHY Specific overrides
        if model.phy.name == 'PHY_Bluetooth_LE_1M_Viterbi_917M_noDSA' or \
                        model.phy.name == 'PHY_Bluetooth_LE_2M_Viterbi_noDSA_fullrate' or \
                        model.phy.name == 'PHY_Bluetooth_LE_2M_Viterbi' or \
                        model.phy.name == 'PHY_Bluetooth_LE' or \
                        model.phy.name == 'PHY_Bluetooth_LE_Viterbi_noDSA_fullrate' or \
                        model.phy.name == 'PHY_Bluetooth_LE_Viterbi_noDSA' or \
                        model.phy.name == 'PHY_Bluetooth_LE_Viterbi' or \
                        model.phy.name == 'PHY_Bluetooth_1M_prod' or \
                        model.phy.name == 'PHY_Bluetooth_2M_prod':

            self.MODEM_SHAPING_OVERRIDE(model)

        # : Bluetooth Longrange PHY Specific Overrides
        elif model.phy.name == 'PHY_Bluetooth_LongRange_dsa_125kbps' or \
                        model.phy.name == 'PHY_Bluetooth_LongRange_dsa_500kbps' or \
                        model.phy.name == 'PHY_Bluetooth_LongRange_NOdsa_125kbps' or \
                        model.phy.name == 'PHY_Bluetooth_LongRange_NOdsa_500kbps' or \
                        model.phy.name == 'PHY_Bluetooth_LR_125k_prod' or \
                        model.phy.name == 'PHY_Bluetooth_LR_500k_prod' or \
                        model.phy.name == 'PHY_Bluetooth_1M_Concurrent':

            # : Longrange specific overrides
            self.FRC_LONGRANGE_OVERRIDE(model)

            # Bluetooth ideal gaussian filter
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Datasheet_2450M_2GFSK_250Kbps_125K':
            model.vars.base_frequency_hz.value_forced = 2495700074
#            model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        elif model.phy.name == 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K':
            model.vars.base_frequency_hz.value_forced = 2494900046
#            model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        elif model.phy.name == 'PHY_Datasheet_2450M_2GFSK_2Mbps_1M':
            model.vars.base_frequency_hz.value_forced = 2494650033
#            model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        elif model.phy.name == 'PHY_ZWave_100kbps_916MHz_viterbi':
            model.vars.base_frequency_hz.value_forced = 935300023
#            model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        elif model.phy.name == 'PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc':
            model.vars.base_frequency_hz.value_forced = 934600020
#            model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        elif model.phy.name == 'PHY_Connect_2_4GHz_OQPSK_2Mcps_250kbps':
            model.vars.base_frequency_hz.value_forced = 2494650033
#            model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        elif model.phy.name == 'PHY_IEEE802154_2p4GHz_cohdsa' or \
                        model.phy.name == 'PHY_IEEE802154_2p4GHz_coh_sensitivity_diversity' or \
                        model.phy.name == 'PHY_IEEE802154_2p4GHz_coh_interference_diversity' or \
                        model.phy.name == 'PHY_IEEE802154_2p4GHz_cohdsa_diversity':
            model.vars.MODEM_TXBR_TXBRNUM.value_forced = 53773
