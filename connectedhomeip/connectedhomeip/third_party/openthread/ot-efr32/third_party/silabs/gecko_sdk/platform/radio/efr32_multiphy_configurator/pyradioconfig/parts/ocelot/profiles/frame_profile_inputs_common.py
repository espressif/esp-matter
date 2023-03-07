from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile

class frame_profile_inputs_common_ocelot(object):

    def build_frame_inputs(self, model, profile):
        MIN_FRAME_LENGTH = 1
        MAX_FRAME_LENGTH = 0x7fffffff
        MIN_HEADER_LENGTH = 1
        MAX_HEADER_LENGTH = 254
        MAX_FRAME_TYPE_LENGTH = 0xffff

        IProfile.make_optional_input(profile, model.vars.frame_bitendian, 'frame_general',
                                     default=model.vars.frame_bitendian.var_enum.LSB_FIRST,
                                     readable_name="Frame Bit Endian")
        IProfile.make_optional_input(profile, model.vars.frame_length_type, 'frame_general',
                                     default=model.vars.frame_length_type.var_enum.FIXED_LENGTH,
                                     readable_name="Frame Length Algorithm")
        IProfile.make_optional_input(profile, model.vars.header_en, 'frame_general', default=False,
                                     readable_name="Header Enable")
        IProfile.make_optional_input(profile, model.vars.payload_white_en, 'frame_payload', default=False,
                                     readable_name="Payload Whitening Enable")
        IProfile.make_optional_input(profile, model.vars.payload_crc_en, 'frame_payload', default=True,
                                     readable_name="Insert/Check CRC after payload")
        IProfile.make_hidden_input(profile, model.vars.payload_addtrailtxdata_en, 'frame_payload',
                                   readable_name="Add Trail TX Data to subframe")
        IProfile.make_optional_input(profile, model.vars.header_size, 'frame_header', default=1,
                                     readable_name="Header Size", value_limit_min=MIN_HEADER_LENGTH,
                                     value_limit_max=MAX_HEADER_LENGTH)
        IProfile.make_optional_input(profile, model.vars.header_calc_crc, 'frame_header', default=False,
                                     readable_name="CRC Header")
        IProfile.make_optional_input(profile, model.vars.header_white_en, 'frame_header', default=False,
                                     readable_name="Whiten Header")
        IProfile.make_hidden_input(profile, model.vars.header_addtrailtxdata_en, 'frame_header',
                                   readable_name="Add Trail TX Data to subframe")
        IProfile.make_hidden_input(profile, model.vars.header_excludesubframewcnt_en, 'frame_payload',
                                   readable_name="Exclude words in subframe from Word Couunter")
        IProfile.make_optional_input(profile, model.vars.fixed_length_size, 'frame_fixed_length', default=1,
                                     readable_name="Fixed Payload Size", value_limit_min=MIN_FRAME_LENGTH,
                                     value_limit_max=MAX_FRAME_LENGTH)
        IProfile.make_optional_input(profile, model.vars.var_length_numbits, 'frame_var_length', default=8,
                                     readable_name="Variable Length Bit Size", value_limit_min=1, value_limit_max=12)
        IProfile.make_optional_input(profile, model.vars.var_length_bitendian, 'frame_var_length',
                                     default=model.vars.var_length_bitendian.var_enum.LSB_FIRST,
                                     readable_name="Variable Length Bit Endian")
        IProfile.make_optional_input(profile, model.vars.var_length_byteendian, 'frame_var_length',
                                     default=model.vars.var_length_byteendian.var_enum.LSB_FIRST,
                                     readable_name="Variable Length Byte Endian")
        IProfile.make_optional_input(profile, model.vars.var_length_shift, 'frame_var_length', default=0,
                                     readable_name="Variable Length Bit Location", value_limit_min=0, value_limit_max=7)
        IProfile.make_optional_input(profile, model.vars.var_length_minlength, 'frame_var_length', default=0,
                                     readable_name="Minimum Length", value_limit_min=0, value_limit_max=4095)
        IProfile.make_optional_input(profile, model.vars.var_length_maxlength, 'frame_var_length', default=255,
                                     readable_name="Maximum Length", value_limit_min=0, value_limit_max=4095)
        IProfile.make_optional_input(profile, model.vars.var_length_includecrc, 'frame_var_length', default=False,
                                     readable_name="Length Includes CRC Bytes")
        IProfile.make_optional_input(profile, model.vars.var_length_adjust, 'frame_var_length', default=0,
                                     readable_name="Variable Frame Length Adjust", value_limit_min=-4096,
                                     value_limit_max=4095)
        IProfile.make_optional_input(profile, model.vars.frame_type_loc, 'frame_type_length', default=0,
                                     readable_name="Frame Type Location", value_limit_min=0, value_limit_max=255)
        IProfile.make_optional_input(profile, model.vars.frame_type_bits, 'frame_type_length', default=3,
                                     readable_name="Number of Frame Type Bits", value_limit_min=1, value_limit_max=3)
        IProfile.make_optional_input(profile, model.vars.frame_type_lsbit, 'frame_type_length', default=0,
                                     readable_name="Frame Type Bit 0 Location", value_limit_min=0, value_limit_max=0x7)
        IProfile.make_optional_input(profile, model.vars.frame_type_0_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 0 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_1_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 1 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_2_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 2 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_3_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 3 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_4_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 4 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_5_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 5 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_6_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 6 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_7_length, 'frame_type_length', default=16,
                                     readable_name="Frame Type 7 Length", value_limit_min=0,
                                     value_limit_max=MAX_FRAME_TYPE_LENGTH)
        IProfile.make_optional_input(profile, model.vars.frame_type_0_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 0")
        IProfile.make_optional_input(profile, model.vars.frame_type_1_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 1")
        IProfile.make_optional_input(profile, model.vars.frame_type_2_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 2")
        IProfile.make_optional_input(profile, model.vars.frame_type_3_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 3")
        IProfile.make_optional_input(profile, model.vars.frame_type_4_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 4")
        IProfile.make_optional_input(profile, model.vars.frame_type_5_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 5")
        IProfile.make_optional_input(profile, model.vars.frame_type_6_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 6")
        IProfile.make_optional_input(profile, model.vars.frame_type_7_valid, 'frame_type_length', default=True,
                                     readable_name="Accept Frame Type 7")
        IProfile.make_optional_input(profile, model.vars.frame_type_0_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 0")
        IProfile.make_optional_input(profile, model.vars.frame_type_1_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 1")
        IProfile.make_optional_input(profile, model.vars.frame_type_2_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 2")
        IProfile.make_optional_input(profile, model.vars.frame_type_3_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 3")
        IProfile.make_optional_input(profile, model.vars.frame_type_4_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 4")
        IProfile.make_optional_input(profile, model.vars.frame_type_5_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 5")
        IProfile.make_optional_input(profile, model.vars.frame_type_6_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 6")
        IProfile.make_optional_input(profile, model.vars.frame_type_7_filter, 'frame_type_length', default=False,
                                     readable_name="Apply Address Filter for Frame Type 7")
