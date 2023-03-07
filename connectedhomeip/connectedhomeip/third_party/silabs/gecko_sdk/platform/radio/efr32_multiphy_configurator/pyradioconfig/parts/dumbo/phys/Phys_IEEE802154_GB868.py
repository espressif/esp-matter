from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_154

from py_2_and_3_compatibility import *

class PHYS_IEEE802154_GB868(IPhy):

    def IEEE802154_GB868_154G_PHR(self, phy, model):
        # Great Britain smart metering PHY from 802.15.4g
        # refer to spec:
        # \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\ZigBee\docs-13-0373-12-0mwg-868-gb-smart-meter-han-technical-requirements.doc

        # Override settings for 15.4g PHR and GB868 PN9 whitening
        phy.profile_inputs.white_seed.value            = 0x000000FF
        phy.profile_inputs.white_output_bit.value      = 8
        phy.profile_inputs.white_poly.value            = model.vars.white_poly.var_enum.PN9_BYTE
        phy.profile_inputs.payload_white_en.value      = True
        # The whitening config above will calculate FRC->FECCTRL.BLOCKWHITEMODE
        # = 2 (BYTEWHITE), but it seems we need it to be 1 (WHITE) to work as
        # 15.4g has specifiesd it, so must override:
        phy.profile_outputs.FRC_FECCTRL_BLOCKWHITEMODE.override = 1

        phy.profile_inputs.header_size.value           = 2
        phy.profile_inputs.var_length_numbits.value    = 11
        phy.profile_inputs.var_length_byteendian.value = model.vars.var_length_byteendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_bitendian.value  = model.vars.var_length_bitendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_shift.value      = 0
        #@TODO: Bump min/maxlength by -1 after MCUW_RADIO_CFG-325 is fixed:
        phy.profile_inputs.var_length_minlength.value  = 4    # 15.4e's 4
        phy.profile_inputs.var_length_maxlength.value  = 127  # NOT 15.4g's 2047
        # GB868 does NOT support 15.4g 4-byte CRC, mode switching, or FEC

    def IEEE802154_GB868_Base(self, phy, model):
        # Great Britain smart metering PHY from 802.15.4g
        # refer to spec:
        # \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\ZigBee\docs-13-0373-12-0mwg-868-gb-smart-meter-han-technical-requirements.doc

        # Copied from phy_internal_base.py::GFSK_915M_base() at commit 110a85d7
        phy.profile_inputs.base_frequency_hz.value =  long(915350000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.deviation.value = 35000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.preamble_pattern.value = 1   # 010101...
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 64
        phy.profile_inputs.rx_xtal_error_ppm.value = 40
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.syncword_0.value =  long(0x904E)  # 15.4g SFD0 7209 non-FEC
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        phy.profile_inputs.symbols_in_timing_window.value = 14  # ???
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.agc_speed.value = model.vars.agc_speed.var_enum.FAST

        # Copied from Phys_Datasheet.py::PHY_Datasheet_915M_2GFSK_100Kbps_50K()
        # at commit 4bc304d1
        phy.profile_inputs.timing_detection_threshold.value = 20
        phy.profile_inputs.agc_power_target.value = -8
        phy.profile_inputs.errors_in_timing_window.value = 1
        phy.profile_inputs.timing_sample_threshold.value = 12
        phy.profile_inputs.agc_settling_delay.value = 34

        # Add 15.4 Packet Configuration
        PHY_COMMON_FRAME_154(phy, model)

        # Additional settings
        phy.profile_inputs.rssi_period.value = 3    # 2^3 = 8 bits(symbols)
        phy.profile_inputs.in_2fsk_opt_scope.value = False

        # RFVALREQ-42
        phy.profile_inputs.symbols_in_timing_window.value = 8
        phy.profile_inputs.number_of_timing_windows.value = 3

        phy.profile_outputs.rx_sync_delay_ns.override = 49000
        phy.profile_outputs.rx_eof_delay_ns.override = 49000

    def PHY_IEEE802154_GB868_863MHz_PHR2(self, model, phy_name=None):
        # Great Britain smart metering PHY from 802.15.4g
        # refer to spec:
        # \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\ZigBee\docs-13-0373-12-0mwg-868-gb-smart-meter-han-technical-requirements.doc
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_IEEE802154_GB868_863MHz_PHR2', phy_name=phy_name)
        self.IEEE802154_GB868_Base(phy, model)
        self.IEEE802154_GB868_154G_PHR(phy, model)
        phy.profile_inputs.base_frequency_hz.value =  long(863250000)

    def PHY_IEEE802154_GB868_915MHz_PHR2(self, model, phy_name=None):
        # Great Britain smart metering PHY from 802.15.4g
        # refer to spec:
        # \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\ZigBee\docs-13-0373-12-0mwg-868-gb-smart-meter-han-technical-requirements.doc
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_IEEE802154_GB868_915MHz_PHR2', phy_name=phy_name)
        self.IEEE802154_GB868_Base(phy, model)
        self.IEEE802154_GB868_154G_PHR(phy, model)
