from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

class PHYS_Internal_calculator_regression_Jumbo(IPhy):


    def PHY_Jira_MCUW_RADIO_CFG_827_test_timing_window_calc_longer_preamble_pattern (self, model):
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_Jira_MCUW_RADIO_CFG_827_test_timing_window_calc_longer_preamble_pattern')

        # Zoltan found if preamble_pattern_len > 2 (such as 4), calculated timing window may violate
        # Max timing window = (preamble_length) / 2 rule
        # This PHY tests the calculation in such a case.



        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.rx_xtal_error_ppm.value = 2
        phy.profile_inputs.tx_xtal_error_ppm.value = 2
        phy.profile_inputs.deviation.value = 20000
        phy.profile_inputs.base_frequency_hz.value = 921870000
        phy.profile_inputs.bitrate.value = 38400
        phy.profile_inputs.dsss_chipping_code.value = 0
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.shaping_filter_param.value = 0.5

        # Test case
        phy.profile_inputs.preamble_pattern.value = 5
        phy.profile_inputs.preamble_pattern_len.value = 4

        # Control case should yield same timing window config
        # phy.profile_inputs.preamble_pattern.value = 1
        # phy.profile_inputs.preamble_pattern_len.value = 2


        phy.profile_inputs.preamble_length.value = 48
        phy.profile_inputs.syncword_0.value = 11732
        phy.profile_inputs.syncword_1.value = 0
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.syncword_tx_skip.value = False
        phy.profile_inputs.asynchronous_rx_enable.value = False
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.dsa_enable.value = False
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.channel_spacing_hz.value = 90000
        phy.profile_inputs.test_ber.value = False
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CRC_16
        phy.profile_inputs.crc_seed.value = 0
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_pad_input.value = False
        phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.MSB_FIRST
        phy.profile_inputs.crc_invert.value = False
        phy.profile_inputs.fec_en.value = model.vars.fec_en.var_enum.NONE
        phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.MSB_FIRST
        phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        phy.profile_inputs.payload_white_en.value = False
        phy.profile_inputs.payload_crc_en.value = True
        phy.profile_inputs.payload_addtrailtxdata_en.value = False
        phy.profile_inputs.header_en.value = True
        phy.profile_inputs.header_size.value = 1
        phy.profile_inputs.header_calc_crc.value = True
        phy.profile_inputs.header_white_en.value = False
        phy.profile_inputs.header_addtrailtxdata_en.value = False
        phy.profile_inputs.fixed_length_size.value = 16
        phy.profile_inputs.var_length_numbits.value = 8
        phy.profile_inputs.var_length_byteendian.value = model.vars.var_length_byteendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_shift.value = 0
        phy.profile_inputs.var_length_minlength.value = 10
        phy.profile_inputs.var_length_maxlength.value = 64
        phy.profile_inputs.var_length_includecrc.value = False
        phy.profile_inputs.var_length_adjust.value = 0
        phy.profile_inputs.frame_type_loc.value = 0
        phy.profile_inputs.frame_type_bits.value = 0
        phy.profile_inputs.frame_type_lsbit.value = 0
        phy.profile_inputs.frame_type_0_length.value = 0
        phy.profile_inputs.frame_type_1_length.value = 0
        phy.profile_inputs.frame_type_2_length.value = 0
        phy.profile_inputs.frame_type_3_length.value = 0
        phy.profile_inputs.frame_type_4_length.value = 0
        phy.profile_inputs.frame_type_5_length.value = 0
        phy.profile_inputs.frame_type_6_length.value = 0
        phy.profile_inputs.frame_type_7_length.value = 0
        phy.profile_inputs.frame_type_0_valid.value = False
        phy.profile_inputs.frame_type_1_valid.value = False
        phy.profile_inputs.frame_type_2_valid.value = False
        phy.profile_inputs.frame_type_3_valid.value = False
        phy.profile_inputs.frame_type_4_valid.value = False
        phy.profile_inputs.frame_type_5_valid.value = False
        phy.profile_inputs.frame_type_6_valid.value = False
        phy.profile_inputs.frame_type_7_valid.value = False
        phy.profile_inputs.frame_type_0_filter.value = True
        phy.profile_inputs.frame_type_1_filter.value = True
        phy.profile_inputs.frame_type_2_filter.value = True
        phy.profile_inputs.frame_type_3_filter.value = True
        phy.profile_inputs.frame_type_4_filter.value = True
        phy.profile_inputs.frame_type_5_filter.value = True
        phy.profile_inputs.frame_type_6_filter.value = True
        phy.profile_inputs.frame_type_7_filter.value = True
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
        phy.profile_inputs.white_seed.value = 0
        phy.profile_inputs.white_output_bit.value = 0
        phy.profile_inputs.frame_coding.value = model.vars.frame_coding.var_enum.NONE