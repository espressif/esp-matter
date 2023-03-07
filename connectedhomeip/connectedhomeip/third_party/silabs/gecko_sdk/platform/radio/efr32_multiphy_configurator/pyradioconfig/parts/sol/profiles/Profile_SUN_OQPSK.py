from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.sol.profiles.sw_profile_outputs_common import sw_profile_outputs_common_sol

class Profile_SUN_OQPSK_Sol(IProfile):

    def __init__(self):
        self._profileName = "SUN_OQPSK"
        self._readable_name = "SUN OQPSK Profile"
        self._category = ""
        self._description = "Profile used for SUN OQPSK PHYs"
        self._default = False
        self._activation_logic = ""
        self._family = "sol"
        self._sw_profile_outputs_common = sw_profile_outputs_common_sol()

    def buildProfileModel(self, model):
        # Build profile object and append it to the model
        profile = self._makeProfile(model)

        # Build inputs
        self.build_required_profile_inputs(model, profile)
        self.build_optional_profile_inputs(model, profile)
        self.build_advanced_profile_inputs(model, profile)
        self.build_hidden_profile_inputs(model, profile)
        self.build_deprecated_profile_inputs(model, profile)

        # Build outputs
        self.build_register_profile_outputs(model, profile)
        self.build_variable_profile_outputs(model, profile)
        self.build_info_profile_outputs(model, profile)

    def build_required_profile_inputs(self, model, profile):
        self.make_required_input(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=358000000,
                                     value_limit_max=956000000, units_multiplier=UnitsMultiplier.MEGA)
        self.make_required_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0,
                                     value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)
        self.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        self.make_required_input(profile, model.vars.sun_oqpsk_chiprate, "SUN",
                                     readable_name="SUN OQPSK Chiprate", value_limit_min=model.vars.sun_oqpsk_chiprate.var_enum._100_KCPS, value_limit_max=model.vars.sun_oqpsk_chiprate.var_enum._2000_KCPS)
        self.make_required_input(profile, model.vars.fcs_type_802154, 'SUN', readable_name="FCS Type (CRC)")

    def build_optional_profile_inputs(self, model, profile):
        pass

    def build_advanced_profile_inputs(self, model, profile):
        self.make_linked_io(profile, model.vars.fpll_band, 'crystal', readable_name="RF Frequency Planning Band")

    def build_hidden_profile_inputs(self, model, profile):
        # Hidden inputs to allow for fixed frame length testing
        self.make_hidden_input(profile, model.vars.frame_length_type, 'frame_general',
                               readable_name="Frame Length Algorithm")
        self.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length',
                               readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)

        # Hidden inputs to allow for keeping absolute tolerance the same when testing at 915M
        self.make_hidden_input(profile, model.vars.freq_offset_hz, 'Advanced',
                               readable_name="Frequency Offset Compensation (AFC) Limit", value_limit_min=0,
                               value_limit_max=500000, units_multiplier=UnitsMultiplier.KILO)

        #Hidden input for dual front-end filter support
        self.make_hidden_input(profile, model.vars.dual_fefilt, "Advanced",
                               readable_name="Dual front-end filter enable")

    def build_deprecated_profile_inputs(self, model, profile):
        pass

    def build_register_profile_outputs(self, model, profile):
        family = self._family

        build_modem_regs_ocelot(model, profile, family)
        buildFrameOutputs(model, profile, family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

    def build_variable_profile_outputs(self, model, profile):
        self._sw_profile_outputs_common.build_rail_outputs(model, profile)
        self._sw_profile_outputs_common.build_ircal_outputs(model, profile)

    def build_info_profile_outputs(self, model, profile):
        self._sw_profile_outputs_common.build_info_outputs(model, profile)

    def profile_calculate(self, model):
        self._fixed_sun_oqpsk_vars(model)
        self._lookup_from_oqpsk_chiprate(model)

    def _fixed_sun_oqpsk_vars(self, model):

        #AGC
        self._fixed_sun_oqpsk_agc(model)

        #OQPSK modulation on softmodem
        model.vars.modulation_type.value_forced = model.vars.modulation_type.var_enum.OQPSK
        model.vars.demod_select.value_forced = model.vars.demod_select.var_enum.SOFT_DEMOD

        #Tolerance
        model.vars.rx_xtal_error_ppm.value_forced = 0
        model.vars.tx_xtal_error_ppm.value_forced = 0
        model.vars.baudrate_tol_ppm.value_forced = 40
        model.vars.deviation_tol_ppm.value_forced = 0

        #Encoding and Whitening (unused)
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED
        model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.NRZ
        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Default
        model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.NONE
        model.vars.white_seed.value_forced = 0
        model.vars.white_output_bit.value_forced = 0

        #Preamble and syncword (unused)
        model.vars.preamble_length.value_forced = 32
        model.vars.preamble_pattern.value_forced = 0
        model.vars.preamble_pattern_len.value_forced = 4
        model.vars.syncword_0.value_forced = 0xe5
        model.vars.syncword_1.value_forced = 0
        model.vars.syncword_length.value_forced = 8

        #Shaping (unused)
        model.vars.shaping_filter.value_forced = model.vars.shaping_filter.var_enum.Custom_OQPSK
        model.vars.shaping_filter_param.value_forced = 0.0

        #Modulation parameters (unused)
        model.vars.deviation.value_forced = 0
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0

        #Frame settings (unused)
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.LSB_FIRST
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.FIXED_LENGTH
        model.vars.payload_white_en.value_forced = False
        model.vars.payload_crc_en.value_forced = False
        model.vars.header_en.value_forced = True
        model.vars.header_size.value_forced = 1
        model.vars.header_calc_crc.value_forced = False
        model.vars.header_white_en.value_forced = False
        model.vars.var_length_numbits.value_forced = 8
        model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        model.vars.var_length_shift.value_forced = 0
        model.vars.var_length_minlength.value_forced = 5
        model.vars.var_length_maxlength.value_forced = 0x7F
        model.vars.var_length_includecrc.value_forced = False
        model.vars.var_length_adjust.value_forced = 0
        model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.LSB_FIRST
        model.vars.crc_seed.value_forced = 0x00000000
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.LSB_FIRST
        model.vars.crc_bit_endian.value_forced = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        model.vars.crc_byte_endian.value_forced = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        model.vars.crc_pad_input.value_forced = False
        model.vars.crc_invert.value_forced = False
        model.vars.fixed_length_size.value_forced = 1
        model.vars.frame_type_0_filter.value_forced = True
        model.vars.frame_type_0_length.value_forced = 0
        model.vars.frame_type_0_valid.value_forced = False
        model.vars.frame_type_1_filter.value_forced = True
        model.vars.frame_type_1_length.value_forced = 0
        model.vars.frame_type_1_valid.value_forced = False
        model.vars.frame_type_2_filter.value_forced = True
        model.vars.frame_type_2_length.value_forced = 0
        model.vars.frame_type_2_valid.value_forced = False
        model.vars.frame_type_3_filter.value_forced = True
        model.vars.frame_type_3_length.value_forced = 0
        model.vars.frame_type_3_valid.value_forced = False
        model.vars.frame_type_4_filter.value_forced = True
        model.vars.frame_type_4_length.value_forced = 0
        model.vars.frame_type_4_valid.value_forced = False
        model.vars.frame_type_5_filter.value_forced = True
        model.vars.frame_type_5_length.value_forced = 0
        model.vars.frame_type_5_valid.value_forced = False
        model.vars.frame_type_6_filter.value_forced = True
        model.vars.frame_type_6_length.value_forced = 0
        model.vars.frame_type_6_valid.value_forced = False
        model.vars.frame_type_7_filter.value_forced = True
        model.vars.frame_type_7_length.value_forced = 0
        model.vars.frame_type_7_valid.value_forced = False
        model.vars.frame_type_bits.value_forced = 3
        model.vars.frame_type_loc.value_forced = 0
        model.vars.frame_type_lsbit.value_forced = 0

        #Other
        model.vars.asynchronous_rx_enable.value_forced = False
        model.vars.syncword_tx_skip.value_forced = False

    def _fixed_sun_oqpsk_agc(self, model):
        pass

    def _lookup_from_oqpsk_chiprate(self, model):

        #Read in the chiprate
        sun_oqpsk_chiprate = model.profile.inputs.sun_oqpsk_chiprate.var_value

        #Set the bitrate for SF1 (actual bitrate handled in softmodem since we don't know SF)
        if sun_oqpsk_chiprate == model.vars.sun_oqpsk_chiprate.var_enum._100_KCPS:
            bitrate = 100000
        elif sun_oqpsk_chiprate == model.vars.sun_oqpsk_chiprate.var_enum._400_KCPS:
            bitrate = 400000
        elif sun_oqpsk_chiprate == model.vars.sun_oqpsk_chiprate.var_enum._1000_KCPS:
            bitrate = 1000000
        else:
            bitrate = 2000000

        model.vars.bitrate.value_forced = bitrate
        model.vars.dsss_spreading_factor.value_forced = 1
        model.vars.dsss_chipping_code.value_forced = 0xA47C
        model.vars.dsss_len.value_forced = 16