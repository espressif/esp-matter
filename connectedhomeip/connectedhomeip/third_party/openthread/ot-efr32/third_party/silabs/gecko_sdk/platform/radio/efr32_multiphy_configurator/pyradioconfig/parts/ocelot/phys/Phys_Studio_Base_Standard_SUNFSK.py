from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.ocelot.profiles.profile_wisun_fan_1_0 import ProfileWisunFanOcelot

##########SUN FSK PHYs (exposed using Base Profile)##########

class PHYS_Studio_Base_Standard_SUNFSK_Ocelot(IPhy):

    def SUN_FSK_base(self, phy, model):

        #These are taken directly from the WiSUN Profile. Noteable items:
        #Data whitening is optional for SUN FSK but we will enable it by default here
        #Dynamic FEC support will be enabled for these PHYs via advanced input (but TX off by default)
        #Xtal tolerance will be defined per-PHY as the SUN FSK standard defines this per band
        #Using 4-octet CRC (FCS) by default

        # Shaping filter
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian  # 17.2.4 GFSK modulation
        phy.profile_inputs.shaping_filter_param.value = 2.0 #Match our decision for WiSUN

        # Variable Length and Endianness Inputs
        phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST  # 802154 17.2.2 (Reference Modulator Diagram)
        phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH  # 802154 20.2.2 (PHR Field Format)
        phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.MSB_FIRST  # 802154 20.2.2 (PHR Field Format)
        phy.profile_inputs.var_length_byteendian.value = model.vars.var_length_byteendian.var_enum.MSB_FIRST  # Consistent with var_length_bitendian above
        phy.profile_inputs.var_length_numbits.value = 11  # Frame Length contained in bits 5-15 (11 total bits) of PHR
        phy.profile_inputs.var_length_shift.value = 0  # Since the length is stored MSB_FIRST, we don't need to shift as the MSB is in bit 15
        phy.profile_inputs.var_length_maxlength.value = 2047  # 11-bit field for Frame Length
        phy.profile_inputs.var_length_minlength.value = 0
        phy.profile_inputs.var_length_includecrc.value = True  # Length is the total number of octets contained in the PSDU
        phy.profile_inputs.var_length_adjust.value = 0

        # Header Inputs
        phy.profile_inputs.header_calc_crc.value = False  # No CRC on the header itself
        phy.profile_inputs.header_en.value = True  # 802154 20.2.2 (PHR Field Format)
        phy.profile_inputs.header_size.value = 2  # Header size is 16 bits
        phy.profile_inputs.header_white_en.value = False  # When whitening is enabled it shall be applied only over the PSDU

        # FEC settings (note that there is a separate variable fec_tx_enable that determines if FEC is on/off in TX)
        phy.profile_inputs.fec_en.value = model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING
        phy.profile_inputs.fec_tx_enable.value = model.vars.fec_tx_enable.var_enum.DISABLED #Disable FEC on TX by default

        # Whitening Inputs set consistent with 802154 17.2.3
        phy.profile_inputs.payload_white_en.value = True
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.PN9_802154
        phy.profile_inputs.white_seed.value = 0xFF  # This has been tested, need a 0 in position 8 due to EFR32 whitening architecture
        phy.profile_inputs.white_output_bit.value = 8

        # CRC Inputs set to match 802.15.4 settings in PHY_COMMON_FRAME_154
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979 #Use 4-octet CRC
        phy.profile_inputs.payload_crc_en.value = True
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.LSB_FIRST
        phy.profile_inputs.crc_invert.value = True
        phy.profile_inputs.crc_pad_input.value = True
        phy.profile_inputs.crc_seed.value = 0xFFFFFFFF

        # Symbol mapping and encoding
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = 0
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0  # 802154 Table 20.8 and 20.9

        # Preamble and syncword definition
        phy.profile_inputs.preamble_length.value = 8 * 8  # default to 64 bit preamble
        phy.profile_inputs.preamble_pattern.value = 1  # 802154 20.2.1.1 (Preamble field)
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.syncword_0.value = ProfileWisunFanOcelot.phySunFskSfd_0_uncoded
        phy.profile_inputs.syncword_1.value = ProfileWisunFanOcelot.phySunFskSfd_0_uncoded
        phy.profile_inputs.syncword_length.value = 16

        # Tolerance values
        phy.profile_inputs.baudrate_tol_ppm.value = 300
        phy.profile_inputs.deviation_tol_ppm.value = 300000

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-27
    def PHY_IEEE802154_SUN_FSK_169MHz_2FSK_2p4kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 SUN FSK 169MHz 2FSK 2.4kbps mi=2.0', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        #Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2

        #Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 169406250 # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 2400
        phy.profile_inputs.deviation.value = 2400
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 16  # From SUN FSK tolerance equation
        phy.profile_inputs.tx_xtal_error_ppm.value = 16

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-28
    def PHY_IEEE802154_SUN_FSK_169MHz_2FSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='IEEE 802.15.4 SUN FSK 169MHz 2FSK 4.8kbps mi=0.5', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 169406250 # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 1200
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 8  # From SUN FSK tolerance equation
        phy.profile_inputs.tx_xtal_error_ppm.value = 8

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-29
    def PHY_IEEE802154_SUN_FSK_169MHz_4FSK_9p6kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base,
                            readable_name='IEEE 802.15.4 SUN FSK 169MHz 4FSK 9.6kbps mi=0.33', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK4

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 169406250  # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 9600
        phy.profile_inputs.deviation.value = 800
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 6  # 20.6.3 Equation for Rsymb = 4800, mi=0.333, rnd up to 6
        phy.profile_inputs.tx_xtal_error_ppm.value = 6  # 20.6.3 Equation for Rsymb = 4800, mi=0.333, rnd up to 6

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-61
    def PHY_IEEE802154_SUN_FSK_450MHz_2FSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base,
                            readable_name='IEEE 802.15.4 SUN FSK 450MHz 2FSK 4.8kbps mi=1.0', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 450006250  # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 2400
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 6
        phy.profile_inputs.tx_xtal_error_ppm.value = 6

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-62
    def PHY_IEEE802154_SUN_FSK_450MHz_4FSK_9p6kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base,
                            readable_name='IEEE 802.15.4 SUN FSK 450MHz 4FSK 9.6kbps mi=0.33', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK4

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 450006250  # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 9600
        phy.profile_inputs.deviation.value = 800
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 2
        phy.profile_inputs.tx_xtal_error_ppm.value = 2

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-163
    def PHY_IEEE802154_SUN_FSK_896MHz_2FSK_40kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base,
                            readable_name='IEEE 802.15.4 SUN FSK 896MHz 2FSK 40kbps mi=0.5', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 896050000  # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 40000
        phy.profile_inputs.deviation.value = 10000
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 13
        phy.profile_inputs.tx_xtal_error_ppm.value = 13

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-164
    def PHY_IEEE802154_SUN_FSK_920MHz_4FSK_400kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base,
                            readable_name='IEEE 802.15.4 SUN FSK 920MHz 4FSK 400kbps mi=0.33', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK4

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 920800000  # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 400000
        phy.profile_inputs.deviation.value = 33333
        phy.profile_inputs.channel_spacing_hz.value = 600000

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 40
        phy.profile_inputs.tx_xtal_error_ppm.value = 40

        return phy