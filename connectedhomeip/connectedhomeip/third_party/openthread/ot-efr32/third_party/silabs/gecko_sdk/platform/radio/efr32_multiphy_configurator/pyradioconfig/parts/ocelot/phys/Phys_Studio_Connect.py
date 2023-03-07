from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from py_2_and_3_compatibility import *


class PHYS_connect_Ocelot(IPhy):

    def Connect_base(self, phy, model):
        #Standard modulation, shaping, and framing settings for Connect PHYs

        #Modulation format and symbol mapping
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0

        # Preamble and syncword definition
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.syncword_0.value = 0x2DD4
        phy.profile_inputs.syncword_1.value = 0x0
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.syncword_tx_skip.value = False

        #Shaping filter
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 0.5

        #Xtal frequency and tolerance
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 10
        phy.profile_inputs.tx_xtal_error_ppm.value = 10
        phy.profile_inputs.baudrate_tol_ppm.value = 0

        #CRC settings
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST

        #Encoding and whitening
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = 0
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.PN9


    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-140
    def PHY_Studio_Connect_915MHz_2GFSK_500kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, 'US FCC 915, Brazil 915',
                            readable_name="Connect 915MHz 2GFSK 500kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        self.Connect_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.deviation.value = 175000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 400000

        # Preamble and syncword definition
        phy.profile_inputs.preamble_length.value = 40

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-23
    def PHY_Studio_Connect_169MHz_2GFSK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Europe 169',
                            readable_name="Connect 169MHz 2GFSK 4.8kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        self.Connect_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.deviation.value = 1200

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 169000000
        phy.profile_inputs.channel_spacing_hz.value = 12500

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-139
    def PHY_Studio_Connect_863MHz_2GFSK_100kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Europe 868',
                            readable_name="Connect 863MHz 2GFSK 100kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        self.Connect_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.deviation.value = 50000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 863000000
        phy.profile_inputs.channel_spacing_hz.value = 400000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-144
    def PHY_Studio_Connect_920MHz_2GFSK_100kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='Japan 915',
                            readable_name="Connect 920MHz 2GFSK 100kbps", phy_name=phy_name)

        # : Common base funtion for all connect PHYs
        self.Connect_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.deviation.value = 50000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 920000000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-56
    def PHY_Studio_Connect_434MHz_2GFSK_200kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='US FCC 434',
                            readable_name="Connect 434MHz 2GFSK 200kbps", phy_name=phy_name)

        # : Common base funtion for all connect PHYs
        self.Connect_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 200000
        phy.profile_inputs.deviation.value = 100000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 434000000
        phy.profile_inputs.channel_spacing_hz.value = 500000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-141
    def PHY_Studio_Connect_915MHz_OQPSK_500kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='DSSS 500',
                            readable_name="Connect 915MHz OQPSK 500Kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        self.Connect_base(phy, model)

        """ Channel and Bandwidth Settings """
        phy.profile_inputs.base_frequency_hz.value = long(915000000)
        phy.profile_inputs.deviation.value = 250000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.baudrate_tol_ppm.value = 0

        """ Modulation Type """
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK

        """ Symbol Mapping and Encoding """
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS

        """ DSSS Parameters """
        phy.profile_inputs.dsss_chipping_code.value = 31433
        phy.profile_inputs.dsss_len.value = 16
        phy.profile_inputs.dsss_spreading_factor.value = 4

        """ Shaping Filter Parameters """
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.5

        """ Preamble Parameters """
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.preamble_length.value = 40
        phy.profile_inputs.preamble_pattern.value = 0

        """ Syncword Parameters """
        phy.profile_inputs.syncword_0.value = 0xA7
        phy.profile_inputs.syncword_1.value = 0x0
        phy.profile_inputs.syncword_length.value = 8

        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-143
    def PHY_Studio_Connect_915mhz_oqpsk_800kcps_100kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='DSSS 100',
                            readable_name="Connect 915MHz OQPSK 800kcps 100kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        self.Connect_base(phy, model)

        """ Channel and Bandwidth Settings """
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.deviation.value = 200000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.baudrate_tol_ppm.value = 4000

        """ Modulation Type """
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK

        """ Symbol Mapping and Encoding """
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS

        """ DSSS Parameters """
        phy.profile_inputs.dsss_chipping_code.value = 1951056795
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8

        """ Shaping Filter Parameters """
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.5

        """ Preamble Parameters """
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0

        """ Syncword Parameters """
        phy.profile_inputs.syncword_0.value = 0xA7
        phy.profile_inputs.syncword_1.value = 0x0
        phy.profile_inputs.syncword_length.value = 8

        """ Frame Configurations """
        # Packet Inputs
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.LSB_FIRST
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-996
    def PHY_Studio_Connect_915mhz_oqpsk_2Mcps_250kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='DSSS 250',
                            readable_name="Connect 915MHz OQPSK 2Mcps 250kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        self.Connect_base(phy, model)

        """ Channel and Bandwidth Settings """
        phy.profile_inputs.base_frequency_hz.value = long(915000000)
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.baudrate_tol_ppm.value = 4000

        """ Modulation Type """
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK

        """ Symbol Mapping and Encoding """
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS

        """ DSSS Parameters """
        phy.profile_inputs.dsss_chipping_code.value = 1951056795
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8

        """ Shaping Filter Parameters """
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 0.5

        """ Preamble Parameters """
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.preamble_length.value = 32
        phy.profile_inputs.preamble_pattern.value = 0

        """ Syncword Parameters """
        phy.profile_inputs.syncword_0.value = 0xA7
        phy.profile_inputs.syncword_1.value = 0x0
        phy.profile_inputs.syncword_length.value = 8

        """ Frame Configurations """
        # Packet Inputs
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.LSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
        return phy