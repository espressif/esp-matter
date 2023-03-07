from pyradioconfig.parts.common.profiles.dumbo_regs import *
from pyradioconfig.parts.common.profiles.jumbo_regs import *
from pyradioconfig.parts.common.profiles.nixi_regs import *
from pyradioconfig.parts.common.profiles.ocelot_regs import *
from pyradioconfig.parts.common.profiles.profile_common import *
from pyradioconfig.parts.common.profiles.profile_modem import *

from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier

from pyradioconfig.parts.common.profiles.profile_mbus_modes import *


class Profile_Mbus(IProfile):

    """
    Init internal variables
    """
    def __init__(self):
        self._profileName = "Mbus"
        self._readable_name = "Mbus Profile"
        self._category = ""
        self._description = "Profile used for Mbus phys"
        self._default = False
        self._activation_logic = ""


    """
    Builds inputs, forced, outputs into modem model
    """
    def buildProfileModel(self, model):

        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        IProfile.make_required_input(profile, model.vars.mbus_mode,            category="general", readable_name="Mbus Mode")
        IProfile.make_required_input(profile, model.vars.mbus_frame_format,    category="general", readable_name="Mbus Frame Format")
        IProfile.make_required_input(profile, model.vars.mbus_symbol_encoding, category="general", readable_name="Symbol Encoding")
        IProfile.make_required_input(profile, model.vars.syncword_dualsync,    category="general", readable_name="Enable Dual Syncword Detection")

        IProfile.make_required_input(profile, model.vars.base_frequency_hz,    category="operational_frequency", readable_name="Base Channel Frequency",  value_limit_min=long(100000000), value_limit_max=long(2480000000), units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz,   category="operational_frequency", readable_name="Channel Spacing",         value_limit_min=0,          value_limit_max=10000000,    units_multiplier=UnitsMultiplier.KILO)

        # Hidden test inputs
        IProfile.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length', readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)
        IProfile.make_hidden_input(profile, model.vars.payload_crc_en,    category='frame_payload',      readable_name="Insert/Check CRC after payload")

        IProfile.make_hidden_input(profile, model.vars.crc_poly,        category='crc', readable_name="CRC Polynomial")
        #IProfile.make_hidden_input(profile, model.vars.crc_seed,        category='crc', readable_name="CRC Seed")
        IProfile.make_hidden_input(profile, model.vars.crc_byte_endian, category='crc', readable_name="CRC Byte Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_bit_endian,  category='crc', readable_name="CRC Output Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_pad_input,   category='crc', readable_name="CRC Input Padding")
        IProfile.make_hidden_input(profile, model.vars.crc_input_order, category='crc', readable_name="CRC Input Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_invert,      category='crc', readable_name="CRC Invert")

        IProfile.make_optional_input(profile, model.vars.test_ber,      category="testing", readable_name="Reconfigure for BER testing", default=False)

        # TODO:  Change this to MSB_FIRst in the lab phys, then remove this input and move it to the forced section below
        IProfile.make_hidden_input(profile, model.vars.frame_bitendian, category='frame_general', readable_name="Frame Bit Endian")

        # Informational output
        buildModemInfoOutputs(model, profile)

        # RAIL Outputs
        buildRailOutputs(model, profile)

        # Profile does not include advanced inputs, but we do need an output for IR cal power level for rail_scripts to consume
        # So add that here. It will default to 0 (which triggers to change to IR cal).
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.RAIL_CONFIG, readable_name='IR cal power level (amplitude)'))

        # Output fields
        buildFrameOutputs(model, profile, family=family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

        if family == "dumbo":
            build_modem_regs_dumbo(model, profile, family = family)
        elif family == "jumbo":
            build_modem_regs_jumbo(model, profile, family = family)
        elif family == "nerio":
            build_modem_regs_jumbo(model, profile, family = family)
        elif family == "nixi":
            build_modem_regs_nixi(model, profile, family = family)

        if family == "nerio" or family == "nixi":
            buildLongRangeOutputs(model, profile)

        build_ircal_sw_vars(model, profile)

        return profile


    def mbus_profile_frame_format_common(self, model):
        family = self._family

        # Whitening
        model.vars.header_white_en.value_forced = False
        model.vars.payload_white_en.value_forced = False
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.NONE
        model.vars.white_seed.value_forced = 0
        model.vars.white_output_bit.value_forced = 0

        # General frame format
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.MSB_FIRST

        # -- Payload --
        if family != 'dumbo':
            model.vars.payload_addtrailtxdata_en.value_forced = False
        if family == 'nerio' or family == 'nixi':           # TODO Is this correct?  Do we include this in Nixi?
            model.vars.payload_excludesubframewcnt_en.value_forced = False

        # -- Header --

        if family != 'dumbo':
            model.vars.header_addtrailtxdata_en.value_forced = False
        if family == 'nerio' or family == 'nixi':  # TODO Is this correct?  Do we include this in Nixi?
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

        model.vars.xtal_frequency_hz.value_forced = 38400000

        model.vars.preamble_pattern.value_forced = 1
        model.vars.preamble_pattern_len.value_forced = 2

        model.vars.asynchronous_rx_enable.value_forced = False

        model.vars.syncword_tx_skip.value_forced = False

        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Inverted


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

    def mbus_profile_postamble_calc(self, model):
        # We should be able to create an input variable in the base profile that selects the type of
        # postamble that will be sent.  That variable would be read by a calculation in the calculator (calc_fec right now).
        # Then this code would just force that base profile input variable instead of forcing registers.
        # The profile really should only be forcing base profile input variables, so this code should be cleaned up
        if model.profile.inputs.mbus_symbol_encoding.var_value == model.vars.mbus_symbol_encoding.var_enum.Manchester:
            model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATA.value_forced = 0xff
            model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT.value_forced = 0  # shortest allowed postamble is 2 chip
            model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE.value_forced = 1


    def mbus_profile_symbol_encoding_calc(self, model):
        mbus_symbol_encoding = model.profile.inputs.mbus_symbol_encoding.var_value
        if mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.NRZ:
            model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.NRZ
            model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        elif mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.Manchester:
            model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.Manchester
            model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        elif mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6:
            model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.NRZ
            model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.MBUS_3OF6
        else:
            raise Exception("Unexpected value found for mbus_symbol_encoding")


    def profile_calculate(self, model):
        self.mbus_profile_frame_format_common(model)
        self.mbus_profile_frame_format_calc(model)
        self.mbus_profile_radio_common(model)
        self.mbus_profile_mode_calc(model)
        self.mbus_profile_postamble_calc(model)
        self.mbus_profile_symbol_encoding_calc(model)
