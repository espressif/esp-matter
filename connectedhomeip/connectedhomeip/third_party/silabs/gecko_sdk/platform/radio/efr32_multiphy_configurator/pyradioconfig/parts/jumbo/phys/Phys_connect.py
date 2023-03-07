from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

from py_2_and_3_compatibility import *

class PHYS_connect(IPhy):

    def Connect_base(self, phy, model):

        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.syncword_0.value = long(11732)
        phy.profile_inputs.syncword_1.value = long(0)
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.PN9

    def PHY_Connect_915MHz_2GFSK_500kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='US FCC 915, Brazil 915', readable_name="Connect 915MHz 2GFSK 500kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.deviation.value = 175000
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.preamble_length.value = 40
        phy.profile_inputs.syncword_tx_skip.value = False
        phy.profile_inputs.asynchronous_rx_enable.value = False
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.test_ber.value = False
        phy.profile_inputs.fec_en.value = model.vars.fec_en.var_enum.NONE
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default

        return phy

    def PHY_Connect_902MHz_2GFSK_200kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='US FCC 902, Brazil 902', readable_name="Connect 902MHz 2GFSK 200kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.base_frequency_hz.value = long(902000000)
        phy.profile_inputs.bitrate.value = 200000
        phy.profile_inputs.deviation.value = 50000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.rx_xtal_error_ppm.value = 20
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 20
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT

        return phy

    def PHY_Connect_434MHz_2GFSK_200kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='US FCC 434', readable_name="Connect 434MHz 2GFSK 200kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.bandwidth_hz.value = 400000
        phy.profile_inputs.base_frequency_hz.value =  long(434000000)
        phy.profile_inputs.bitrate.value = 200000
        phy.profile_inputs.deviation.value = 100000
        phy.profile_inputs.channel_spacing_hz.value = 500000
        phy.profile_inputs.rx_xtal_error_ppm.value = 20
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 20
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT

        return phy

    def PHY_Connect_863MHz_2GFSK_100kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Europe 868', readable_name="Connect 863MHz 2GFSK 100kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.bandwidth_hz.value = 200000
        phy.profile_inputs.base_frequency_hz.value = long(863000000)
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.deviation.value = 50000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.rx_xtal_error_ppm.value = 20
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 20
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT

        return phy

    def PHY_Connect_169MHz_2GFSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Europe 169', readable_name="Connect 169MHz 2GFSK 4.8kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.base_frequency_hz.value =  long(169000000)
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 1200
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.number_of_timing_windows.value = 2
        phy.profile_inputs.rx_xtal_error_ppm.value = 7
        phy.profile_inputs.symbols_in_timing_window.value = 6
        phy.profile_inputs.timing_detection_threshold.value = 10
        phy.profile_inputs.tx_xtal_error_ppm.value = 7
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT

        return phy

    def PHY_Connect_490MHz_2GFSK_10kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='China 490', readable_name="Connect 490MHz 2GFSK 10kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.base_frequency_hz.value =  long(490000000)
        phy.profile_inputs.bitrate.value = 10000
        phy.profile_inputs.deviation.value = 25000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.rx_xtal_error_ppm.value = 20
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 20
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        

        return phy

    def PHY_Connect_920MHz_2GFSK_100kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Japan 915', readable_name="Connect 920MHz 2GFSK 100kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.bandwidth_hz.value = 198000
        phy.profile_inputs.base_frequency_hz.value = long(920000000)
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.deviation.value = 50000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT

        return phy

    def PHY_Connect_424MHz_2GFSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Korea 424', readable_name="Connect 424MHz 2GFSK 4.8kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.bandwidth_hz.value = 12000
        phy.profile_inputs.base_frequency_hz.value = long(424700000)
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 2400
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.rx_xtal_error_ppm.value = 20
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 20
        phy.profile_inputs.freq_offset_hz.value = 1450
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian

        return phy

    def PHY_Connect_447MHz_2GFSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Korea 447', readable_name="Connect 447MHz 2GFSK 4.8kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.bandwidth_hz.value = 12000
        phy.profile_inputs.base_frequency_hz.value = long(447000000)
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 2400
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.rx_xtal_error_ppm.value = 20
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.tx_xtal_error_ppm.value = 20
        phy.profile_inputs.freq_offset_hz.value = 1450
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian

        return phy

    def PHY_Connect_917MHz_2GFSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Korea 915', readable_name="Connect 917MHz 2GFSK 4.8kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.base_frequency_hz.value = long(917100000)
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 2400
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.number_of_timing_windows.value = 10
        phy.profile_inputs.rx_xtal_error_ppm.value = 2
        phy.profile_inputs.symbols_in_timing_window.value = 1
        phy.profile_inputs.tx_xtal_error_ppm.value = 3
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.NONE
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT

        return phy

    def PHY_Connect_915MHz_OQPSK_500kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='DSSS 500', readable_name="Connect 915MHz OQPSK 500Kbps", phy_name=phy_name)

        self.Connect_base(phy, model)

        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.deviation.value = 250000
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.dsss_chipping_code.value = 31433
        phy.profile_inputs.dsss_len.value = 16
        phy.profile_inputs.dsss_spreading_factor.value = 4
        phy.profile_inputs.preamble_length.value = 40
        phy.profile_inputs.syncword_tx_skip.value = False
        phy.profile_inputs.asynchronous_rx_enable.value = False
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.test_ber.value = False
        phy.profile_inputs.fec_en.value = model.vars.fec_en.var_enum.NONE
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default

        return phy

    def PHY_Connect_915mhz_oqpsk_800kcps_100kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='DSSS 100', readable_name="Connect 915MHz OQPSK 800kcps 100kbps", phy_name=phy_name)

        self.Connect_base(phy, model)

        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_power_target.value = -6
        phy.profile_inputs.agc_settling_delay.value = 40
        phy.profile_inputs.base_frequency_hz.value = long(902000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 4000
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.LSB_FIRST
        phy.profile_inputs.deviation.value = 200000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value =  long(1951056795)
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        if model.part_family.lower() in ['dumbo', 'jumbo', 'nerio', 'nixi']:
            # Series 1
            phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2520KHz
        else:
            # Series 2
            phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2500KHz
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.rssi_period.value = 8
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.NONE
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.syncword_0.value =  long(167)
        phy.profile_inputs.syncword_1.value = long(0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.timing_detection_threshold.value = 65
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        return phy

    #+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    # def debug_PHY_Connect_IEEE802154_2p4GHz_coh(self, model):
    #     phy = self._makePhy(model, model.profiles.Base, readable_name='test phy for porting coh demod from ieee to connect', phy_name=phy_name)
    #
    #     # Stuff from connect_base routine.
    #     phy.profile_inputs.baudrate_tol_ppm.value = 0
    #     phy.profile_inputs.dsss_chipping_code.value = long(0)
    #     phy.profile_inputs.dsss_len.value = 0
    #     phy.profile_inputs.dsss_spreading_factor.value = 0
    #     phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
    #     #phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
    #     phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
    #     phy.profile_inputs.shaping_filter_param.value = 0.5
    #     phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
    #     phy.profile_inputs.xtal_frequency_hz.value = 38400000
    #     phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
    #     phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
    #     phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
    #     phy.profile_inputs.preamble_pattern_len.value = 2
    #     phy.profile_inputs.preamble_length.value = 32
    #     phy.profile_inputs.preamble_pattern.value = 1
    #     phy.profile_inputs.syncword_0.value = long(11732)
    #     phy.profile_inputs.syncword_1.value = long(0)
    #     phy.profile_inputs.syncword_length.value = 16
    #     phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.PN9
    #
    #     # Stuff from original Connect phy
    #     phy.profile_inputs.agc_hysteresis.value = 0
    #     phy.profile_inputs.agc_power_target.value = -6
    #     phy.profile_inputs.agc_settling_delay.value = 40
    #     phy.profile_inputs.base_frequency_hz.value = long(902000000)
    #     phy.profile_inputs.baudrate_tol_ppm.value = 4000
    #     phy.profile_inputs.bitrate.value = 100000
    #     phy.profile_inputs.channel_spacing_hz.value = 2000000
    #     #phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
    #     #phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.LSB_FIRST
    #     phy.profile_inputs.deviation.value = 200000
    #     phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
    #     phy.profile_inputs.dsss_chipping_code.value =  long(1951056795)
    #     phy.profile_inputs.dsss_len.value = 32
    #     phy.profile_inputs.dsss_spreading_factor.value = 8
    #     phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
    #     phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default
    #     phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
    #     phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2520KHz
    #     phy.profile_inputs.preamble_length.value = 32
    #     phy.profile_inputs.preamble_pattern.value = 0
    #     phy.profile_inputs.preamble_pattern_len.value = 4
    #     phy.profile_inputs.rssi_period.value = 8
    #     phy.profile_inputs.rx_xtal_error_ppm.value = 0
    #     phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.NONE
    #     phy.profile_inputs.shaping_filter_param.value = 0.5
    #     phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
    #     phy.profile_inputs.syncword_0.value =  long(167)
    #     phy.profile_inputs.syncword_1.value = long(0)
    #     phy.profile_inputs.syncword_length.value = 8
    #     phy.profile_inputs.timing_detection_threshold.value = 65
    #     phy.profile_inputs.timing_sample_threshold.value = 0
    #     phy.profile_inputs.tx_xtal_error_ppm.value = 0
    #     phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
    #     phy.profile_inputs.xtal_frequency_hz.value = 38400000
    #
    #     # the rest of the stuff from the ieee phy...
    #     phy.profile_inputs.agc_power_target.value = -11
    #     phy.profile_inputs.agc_scheme.value = model.vars.agc_scheme.var_enum.SCHEME_3
    #     phy.profile_inputs.agc_settling_delay.value = 40
    #     phy.profile_inputs.bandwidth_hz.value = 2524800
    #     phy.profile_inputs.baudrate_tol_ppm.value = 0
    #     phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.DISABLED
    #     phy.profile_inputs.number_of_timing_windows.value = 7
    #     phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
    #     phy.profile_inputs.symbols_in_timing_window.value = 12
    #     phy.profile_inputs.timing_detection_threshold.value = 65
    #
    #     phy.profile_outputs.AGC_CTRL2_ADCRSTSTARTUP.override = 0
    #     phy.profile_outputs.AGC_CTRL2_FASTLOOPDEL.override = 5
    #     phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.override = 4
    #     phy.profile_outputs.AGC_GAINSTEPLIM_SLOWDECAYCNT.override = 0
    #     phy.profile_outputs.AGC_LOOPDEL_IFPGADEL.override = 7
    #     phy.profile_outputs.AGC_LOOPDEL_LNASLICESDEL.override = 7
    #     phy.profile_outputs.AGC_LOOPDEL_PKDWAIT.override = 15
    #     #phy.profile_outputs.AGC_MANGAIN_MANGAINLNAATTEN.override = 0
    #     phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTPER.override = 5
    #     phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTTHR.override = 0xab
    #     phy.profile_outputs.AGC_RSSISTEPTHR_POSSTEPTHR.override = 3
    #     phy.profile_outputs.MODEM_AFC_AFCRXCLR.override = 1
    #     phy.profile_outputs.MODEM_AFC_AFCSCALEM.override = 3
    #     phy.profile_outputs.MODEM_AFCADJLIM_AFCADJLIM.override = 2750
    #     phy.profile_outputs.MODEM_CTRL1_PHASEDEMOD.override = 3
    #     phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 7
    #     phy.profile_outputs.MODEM_CTRL3_TSAMPDEL.override = 2
    #     phy.profile_outputs.MODEM_CTRL5_BBSS.override = 5
    #     phy.profile_outputs.MODEM_CTRL5_FOEPREAVG.override = 7
    #     phy.profile_outputs.MODEM_CTRL5_LINCORR.override = 1
    #     phy.profile_outputs.MODEM_CTRL5_POEPER.override = 1
    #     phy.profile_outputs.MODEM_CTRL6_ARW.override = 1
    #     phy.profile_outputs.MODEM_CTRL6_PSTIMABORT0.override = 1
    #     phy.profile_outputs.MODEM_CTRL6_PSTIMABORT1.override = 1
    #     phy.profile_outputs.MODEM_CTRL6_PSTIMABORT2.override = 1
    #     phy.profile_outputs.MODEM_CTRL6_RXBRCALCDIS.override = 1
    #     phy.profile_outputs.MODEM_CTRL6_TDREW.override = 64
    #     phy.profile_outputs.MODEM_INTAFC_FOEPREAVG0.override = 1
    #     phy.profile_outputs.MODEM_INTAFC_FOEPREAVG1.override = 3
    #     phy.profile_outputs.MODEM_INTAFC_FOEPREAVG2.override = 5
    #     phy.profile_outputs.MODEM_INTAFC_FOEPREAVG3.override = 5
    #     phy.profile_outputs.MODEM_TIMING_OFFSUBDEN.override = 5
    #     phy.profile_outputs.MODEM_TIMING_OFFSUBNUM.override = 12
    #     phy.profile_outputs.MODEM_TIMING_TIMTHRESH.override = 80
    #     phy.profile_outputs.MODEM_TIMING_TIMSEQSYNC.override = 1
    #     phy.profile_outputs.MODEM_PRE_TXBASES.override = 7
    #
    #     # Enables Clock gating to reduce current consumption
    #     phy.profile_outputs.MODEM_CGCLKSTOP_FORCEOFF.override = 0x1E00  # 9,10,11,12



    def PHY_Connect_915mhz_oqpsk_2Mcps_250kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='DSSS 250', readable_name="Connect 915MHz OQPSK 2Mcps 250kbps", phy_name=phy_name)

        self.Connect_base(phy, model)
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.syncword_0.value =  long(167)
        phy.profile_inputs.syncword_1.value = long(0)
        phy.profile_inputs.syncword_tx_skip.value = False
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.baudrate_tol_ppm.value = 4000
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.NONE
        phy.profile_inputs.base_frequency_hz.value = long(902000000)
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default
        phy.profile_inputs.dsss_chipping_code.value =  long(1951056795)
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8
        phy.profile_inputs.asynchronous_rx_enable.value = False
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.LSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
        phy.profile_inputs.timing_detection_threshold.value = 65
        phy.profile_inputs.timing_sample_threshold.value = 0
        if model.part_family.lower() in ['dumbo', 'jumbo', 'nerio', 'nixi']:
            # Series 1
            phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2520KHz
        else:
            # Series 2
            phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2500KHz
        phy.profile_inputs.agc_power_target.value = -6
        phy.profile_inputs.rssi_period.value = 8
        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_settling_delay.value = 40

        return phy

    def PHY_Connect_2_4GHz_OQPSK_2Mcps_250kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='2.4GHz OQPSK 2Mcps 250kbps', readable_name="Connect 2.4GHz OQPSK 2Mcps 250kbps", phy_name=phy_name)
    
        self.Connect_base(phy, model)
        phy.profile_inputs.base_frequency_hz.value =  long(2405000000)
        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_power_target.value = -6
        phy.profile_inputs.agc_settling_delay.value = 40
        phy.profile_inputs.asynchronous_rx_enable.value = False
        phy.profile_inputs.baudrate_tol_ppm.value = 4000
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.channel_spacing_hz.value = 5000000
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value =  long(1951056795)
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        if model.part_family.lower() in ['dumbo', 'jumbo', 'nerio', 'nixi']:
            # Series 1
            phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2520KHz
        else:
            # Series 2
            phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2500KHz
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.rssi_period.value = 8
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.syncword_0.value = long(229)
        phy.profile_inputs.syncword_1.value = long(0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.timing_detection_threshold.value = 65
        phy.profile_inputs.timing_resync_period.value = 2
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        return phy
