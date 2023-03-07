from pyradioconfig.parts.common.profiles.ocelot_regs import *
from pyradioconfig.parts.common.profiles.profile_common import *

from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.ocelot.profiles.sw_profile_outputs_common import sw_profile_outputs_common_ocelot

from pyradioconfig.parts.ocelot.profiles.profile_mbus_modes import *


class Profile_Mbus_Ocelot(IProfile):

    """
    Init internal variables
    """
    def __init__(self):
        self._family = "ocelot"
        self._profileName = "Mbus"
        self._readable_name = "Mbus Profile"
        self._category = ""
        self._description = "Profile used for Mbus phys"
        self._default = False
        self._activation_logic = ""
        self._sw_profile_outputs_common = sw_profile_outputs_common_ocelot()


    """
    Builds inputs, forced, outputs into modem model
    """
    def buildProfileModel(self, model):

        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        ### Required inputs ###

        IProfile.make_required_input(profile, model.vars.preamble_length, "general", readable_name="Preamble Length Total", value_limit_min=16, value_limit_max=2097151)
        IProfile.make_required_input(profile, model.vars.mbus_mode, category="general", readable_name="Mbus Mode")
        IProfile.make_required_input(profile, model.vars.mbus_frame_format, category="general",
                                     readable_name="Mbus Frame Format")
        IProfile.make_required_input(profile, model.vars.mbus_symbol_encoding, category="general",
                                     readable_name="Symbol Encoding")
        IProfile.make_required_input(profile, model.vars.syncword_dualsync, category="general",
                                     readable_name="Enable Dual Syncword Detection")
        IProfile.make_required_input(profile, model.vars.base_frequency_hz, category="operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=long(100000000),
                                     value_limit_max=long(2480000000), units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz, category="operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0,
                                     value_limit_max=10000000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)

        ### Optional inputs ###

        IProfile.make_optional_input(profile, model.vars.test_ber, category="testing",
                                     readable_name="Reconfigure for BER testing", default=False)
        IProfile.make_optional_input(profile, model.vars.deviation_tol_ppm, 'modem', default=0,
                                     readable_name="Maximum deviation offset expected in ppm", value_limit_min=0,
                                     value_limit_max=500000)
        IProfile.make_optional_input(profile, model.vars.mbus_postamble_length, category="general", default=0,
                                     readable_name="Mbus postamble legnth", value_limit_min=0,
                                     value_limit_max=4)


        ### Hidden test inputs ###

        # Hidden inputs to allow for fixed frame length testing
        IProfile.make_hidden_input(profile, model.vars.frame_length_type, 'frame_general',
                                   readable_name="Frame Length Algorithm")
        IProfile.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length',
                                   readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)
        IProfile.make_hidden_input(profile, model.vars.payload_crc_en, category='frame_payload',
                                   readable_name="Insert/Check CRC after payload")
        IProfile.make_hidden_input(profile, model.vars.crc_poly, category='crc', readable_name="CRC Polynomial")
        IProfile.make_hidden_input(profile, model.vars.crc_byte_endian, category='crc',
                                   readable_name="CRC Byte Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_bit_endian, category='crc',
                                   readable_name="CRC Output Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_pad_input, category='crc', readable_name="CRC Input Padding")
        IProfile.make_hidden_input(profile, model.vars.crc_input_order, category='crc',
                                   readable_name="CRC Input Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_invert, category='crc', readable_name="CRC Invert")
        IProfile.make_hidden_input(profile, model.vars.target_osr, category="general",
                                   readable_name="Set desired OSR", value_limit_min=3, value_limit_max=9)
        IProfile.make_hidden_input(profile, model.vars.bitrate, category="modem",
                                   readable_name="Bitrate", value_limit_min=100, value_limit_max=2000000,
                                   units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")
        IProfile.make_hidden_input(profile, model.vars.frame_bitendian, category='frame_general',
                                   readable_name="Frame Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.synth_settling_mode, 'modem',
                                   readable_name="Synth Settling Mode")
        # Hidden inputs to allow for keeping absolute tolerance the same when testing at 915M
        IProfile.make_hidden_input(profile, model.vars.rx_xtal_error_ppm, category="general",
                                   readable_name="Set desired xtal tolerance on RX side", value_limit_min=0,
                                   value_limit_max=100)
        IProfile.make_hidden_input(profile, model.vars.tx_xtal_error_ppm, category="general",
                                   readable_name="Set desired xtal tolerance on TX side", value_limit_min=0,
                                   value_limit_max=100)
        IProfile.make_hidden_input(profile, model.vars.freq_offset_hz, 'Advanced',
                                   readable_name="Frequency Offset Compensation (AFC) Limit", value_limit_min=0,
                                   value_limit_max=500000, units_multiplier=UnitsMultiplier.KILO)

        #Deprecated inputs
        # These inputs were exposed on or after Ocelot Alpha 1 release, so they may be present in radioconf XML
        self.make_deprecated_input(profile, model.vars.max_tx_power_dbm)


        # Informational output
        self._sw_profile_outputs_common.build_info_outputs(model, profile)

        # RAIL Outputs
        self._sw_profile_outputs_common.build_rail_outputs(model, profile)

        # IRCal outputs
        self._sw_profile_outputs_common.build_ircal_outputs(model, profile)

        # Output fields
        buildFrameOutputs(model, profile, family=family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
        self._add_reg_profile_outputs(model, profile)

        return profile

    def _add_reg_profile_outputs(self, model, profile):
        build_modem_regs_ocelot(model, profile, family=self._family)

    def mbus_profile_frame_format_common(self, model):

        # Whitening
        model.vars.header_white_en.value_forced = False
        model.vars.payload_white_en.value_forced = False
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.NONE
        model.vars.white_seed.value_forced = 0
        model.vars.white_output_bit.value_forced = 0

        # General frame format
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.MSB_FIRST

        # -- Payload --
        model.vars.payload_addtrailtxdata_en.value_forced = False
        model.vars.payload_excludesubframewcnt_en.value_forced = False

        # -- Header --
        model.vars.header_addtrailtxdata_en.value_forced = False
        model.vars.header_excludesubframewcnt_en.value_forced = False

        # Frame type length variables
        model.vars.frame_type_loc.value_forced = 0
        model.vars.frame_type_bits.value_forced = 3
        model.vars.frame_type_lsbit.value_forced = 0
        model.vars.frame_type_0_length.value_forced = 0
        model.vars.frame_type_1_length.value_forced = 0
        model.vars.frame_type_2_length.value_forced = 0
        model.vars.frame_type_3_length.value_forced = 0
        model.vars.frame_type_4_length.value_forced = 0
        model.vars.frame_type_5_length.value_forced = 0
        model.vars.frame_type_6_length.value_forced = 0
        model.vars.frame_type_7_length.value_forced = 0
        model.vars.frame_type_0_valid.value_forced = False
        model.vars.frame_type_1_valid.value_forced = False
        model.vars.frame_type_2_valid.value_forced = False
        model.vars.frame_type_3_valid.value_forced = False
        model.vars.frame_type_4_valid.value_forced = False
        model.vars.frame_type_5_valid.value_forced = False
        model.vars.frame_type_6_valid.value_forced = False
        model.vars.frame_type_7_valid.value_forced = False

        # FEC
        model.vars.fec_en.value_forced = model.vars.fec_en.var_enum.NONE

        # CRC
        model.vars.crc_poly.value_forced = model.vars.crc_poly.var_enum.DNP_16
        model.vars.crc_seed.value_forced = default=long(0)
        model.vars.crc_byte_endian.value_forced = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        model.vars.crc_bit_endian.value_forced = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        model.vars.crc_pad_input.value_forced = False
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.MSB_FIRST
        model.vars.crc_invert.value_forced = True


    def mbus_profile_frame_format_calc(self, model):
        if model.profile.inputs.mbus_frame_format.var_value == model.vars.mbus_frame_format.var_enum.NoFormat:
            model.vars.header_en.value_forced = False
            model.vars.header_size.value_forced = 0
            model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.FIXED_LENGTH
            model.vars.header_calc_crc.value_forced = False

            model.vars.fixed_length_size.value_forced = 18      # This is the value Andras was using in the one phy that used this option
            model.vars.var_length_numbits.value_forced = 0
            model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.LSB_FIRST
            model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.LSB_FIRST
            model.vars.var_length_shift.value_forced = 0
            model.vars.var_length_minlength.value_forced = 0
            model.vars.var_length_maxlength.value_forced = 0
            model.vars.var_length_includecrc.value_forced = False
            model.vars.var_length_adjust.value_forced = 0

            model.vars.payload_crc_en.value_forced = False

        elif model.profile.inputs.mbus_frame_format.var_value == model.vars.mbus_frame_format.var_enum.FrameA:
            # -- Header --
            #Block 1 for frameA
            model.vars.header_en.value_forced = True
            model.vars.header_size.value_forced = 1 #This controls DFL location AND header size. We set it up for DFL loc
            model.vars.FRC_FCD0_WORDS.value_forced = 9 #and override the size to be 10B for TX
            model.vars.FRC_FCD2_WORDS.value_forced = 9 #and for RX
            model.vars.header_calc_crc.value_forced = True
            model.vars.header_include_crc.value_forced = True

            #all subsequent blocks are handled as repating 16B subframes
            model.vars.FRC_FCD1_WORDS.value_forced = 15
            model.vars.FRC_FCD3_WORDS.value_forced = 15
            #FCDMODE2 is the default, which is good for us

            # -- Variable Length --
            model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
            model.vars.var_length_numbits.value_forced = 8
            model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.MSB_FIRST
            model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.MSB_FIRST
            model.vars.var_length_shift.value_forced = 0
            model.vars.var_length_minlength.value_forced = 10
            model.vars.var_length_maxlength.value_forced = 255
            model.vars.var_length_includecrc.value_forced = False
            model.vars.var_length_adjust.value_forced = 0

            model.vars.payload_crc_en.value_forced = True

        elif model.profile.inputs.mbus_frame_format.var_value == model.vars.mbus_frame_format.var_enum.FrameB:
            # -- Header --
            #Block 1 and 2 for frameB
            model.vars.header_en.value_forced = True
            model.vars.header_size.value_forced = 1 #This controls DFL location AND header size. We set it up for DFL loc
            model.vars.FRC_FCD0_WORDS.value_forced = 125 #and override the size to be 125B for TX
            model.vars.FRC_FCD2_WORDS.value_forced = 125 #and for RX
            model.vars.header_calc_crc.value_forced = True
            model.vars.header_include_crc.value_forced = True
            #Block 3 is the remaining data, which is the payload from the configurator's perspective

            # -- Variable Length --
            model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
            model.vars.var_length_numbits.value_forced = 8
            model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.MSB_FIRST
            model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.MSB_FIRST
            model.vars.var_length_shift.value_forced = 0
            model.vars.var_length_minlength.value_forced = 12
            model.vars.var_length_maxlength.value_forced = 255
            model.vars.var_length_includecrc.value_forced = True #the big difference: frameB's length include's CRC fields.
            model.vars.var_length_adjust.value_forced = 0

            model.vars.payload_crc_en.value_forced = True

        else:
            raise Exception("Unexpected value found for mbus_frame_format")


    def mbus_profile_radio_common(self, model):

        # Set some variables common to all modes
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0
        model.vars.dsss_chipping_code.value_forced = long(0)
        model.vars.dsss_len.value_forced = 0
        model.vars.dsss_spreading_factor.value_forced = 0
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED
        model.vars.preamble_pattern.value_forced = 1
        model.vars.preamble_pattern_len.value_forced = 2
        model.vars.asynchronous_rx_enable.value_forced = False
        model.vars.syncword_tx_skip.value_forced = False

    def mbus_profile_mode_calc(self, model):
        mode = model.profile.inputs.mbus_mode.var_value
        if   mode == model.vars.mbus_mode.var_enum.ModeC_M2O_100k:
            profile_MBus_modes.profile_wMbus_ModeC_M2O_100k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeC_O2M_50k:
            profile_MBus_modes.profile_wMbus_ModeC_O2M_50k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeF_2p4k:
            profile_MBus_modes.profile_wMbus_ModeF_2p4k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeNg:
            profile_MBus_modes.profile_wMbus_ModeN2g_19p2k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeN1a_4p8K:
            profile_MBus_modes.profile_wMbus_ModeN1a_4p8K(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeN1c_2p4K:
            profile_MBus_modes.profile_wMbus_ModeN1c_2p4K(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeR_4p8k:
            profile_MBus_modes.profile_wMbus_ModeR_4p8k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeT_M2O_100k:
            profile_MBus_modes.profile_wMbus_ModeT_M2O_100k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeT_O2M_32p768k:
            profile_MBus_modes.profile_wMbus_ModeT_O2M_32p768k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeS_32p768k:
            profile_MBus_modes.profile_wMbus_ModeS_32p768k(model, self._family)
        elif mode == model.vars.mbus_mode.var_enum.ModeN_6p4k:
            profile_MBus_modes.profile_wMbus_ModeN_6p4k(model, self._family)

    def mbus_profile_symbol_encoding_calc(self, model):
        mbus_symbol_encoding = model.profile.inputs.mbus_symbol_encoding.var_value
        if mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.NRZ:
            model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.NRZ
        elif mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.Manchester:
            model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.Inv_Manchester #Always inverted
        elif mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6:
            model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.MBUS_3OF6
        else:
            raise Exception("Unexpected value found for mbus_symbol_encoding")

    def profile_calculate(self, model):
        self.mbus_profile_frame_format_common(model)
        self.mbus_profile_frame_format_calc(model)
        self.mbus_profile_radio_common(model)
        self.mbus_profile_mode_calc(model)
        self.mbus_profile_symbol_encoding_calc(model)
