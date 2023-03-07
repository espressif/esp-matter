from pyradioconfig.parts.nixi.phys.Phys_connect import PHYS_connect_Nixi
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from py_2_and_3_compatibility import *

class PHYS_connect_Bobcat(IPhy):
    # inherit from Nixi (Jumbo)

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
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 10
        phy.profile_inputs.tx_xtal_error_ppm.value = 10
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


    # Owner     : Young-Joon Choi
    # Jira Link : https://jira.silabs.com/browse/PGBOBCATVALTEST-211
    def PHY_Studio_Connect_2_4GHz_OQPSK_2Mcps_250kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='2.4GHz OQPSK 2Mcps 250kbps',
                                readable_name="Connect 2.4GHz OQPSK 2Mcps 250kbps", phy_name=phy_name)

        self.Connect_base(phy, model)

        """ Channel and Bandwidth Settings """
        phy.profile_inputs.base_frequency_hz.value = long(2405000000)
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.channel_spacing_hz.value = 5000000
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
        phy.profile_inputs.syncword_0.value = long(229)
        phy.profile_inputs.syncword_1.value = long(0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.syncword_tx_skip.value = False

        """ Frame Configurations """
        # Packet Inputs
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
        phy.profile_inputs.asynchronous_rx_enable.value = False
        return phy