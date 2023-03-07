from pyradioconfig.parts.nixi.profiles.Profile_LongRange import Profile_Long_Range_Nixi
from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs
from pyradioconfig.parts.ocelot.profiles.sw_profile_outputs_common import sw_profile_outputs_common_ocelot
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier

class Profile_Long_Range_Ocelot(Profile_Long_Range_Nixi):

    def __init__(self):
        super(Profile_Long_Range_Ocelot, self).__init__()
        self._family = 'ocelot'
        self._sw_profile_outputs_common = sw_profile_outputs_common_ocelot()

    def build_required_profile_inputs(self, model, profile):
        super().build_required_profile_inputs(model, profile)
        self.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)

    def build_optional_profile_inputs(self, model, profile):
        pass

    def build_advanced_profile_inputs(self, model, profile):
        self.make_linked_io(profile, model.vars.fec_en, 'Channel_Coding', readable_name="FEC Algorithm")

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

    def build_deprecated_profile_inputs(self, model, profile):
        # These inputs were exposed on or after Ocelot Alpha 1 release, so they may be present in radioconf XML
        self.make_deprecated_input(profile, model.vars.max_tx_power_dbm)

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

    def _fixed_longrange_vars(self, model):
        """ Modulation Type """
        model.vars.modulation_type.value_forced = model.vars.modulation_type.var_enum.OQPSK

        """ Symbol Mapping and Encoding """
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0
        model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.DSSS
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED

        """ DSSS Parameters """
        model.vars.dsss_chipping_code.value_forced = 0xC8DD7892
        model.vars.dsss_len.value_forced = 32
        model.vars.dsss_spreading_factor.value_forced = 8

        """ Shaping Filter Parameters """
        model.vars.shaping_filter.value_forced = model.vars.shaping_filter.var_enum.Custom_OQPSK
        model.vars.shaping_filter_param.value_forced = 0.3  # Does not matter

        """ Preamble Parameters """
        model.vars.preamble_length.value_forced = 40
        model.vars.preamble_pattern.value_forced = 0
        model.vars.preamble_pattern_len.value_forced = 4

        """ Syncword Parameters """
        model.vars.syncword_length.value_forced = 12
        model.vars.syncword_0.value_forced = 0xe5
        model.vars.syncword_1.value_forced = 0x0

        # Assorted app parameters
        model.vars.baudrate_tol_ppm.value_forced = 0
        model.vars.deviation_tol_ppm.value_forced = 0

        """ Frame Inputs """
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.LSB_FIRST
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        model.vars.payload_white_en.value_forced = False
        model.vars.payload_crc_en.value_forced = True
        model.vars.header_en.value_forced = True
        model.vars.header_size.value_forced = 1
        model.vars.header_calc_crc.value_forced = False
        model.vars.header_white_en.value_forced = False
        model.vars.var_length_numbits.value_forced = 8
        model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        model.vars.var_length_shift.value_forced = 0
        model.vars.var_length_minlength.value_forced = 5
        model.vars.var_length_maxlength.value_forced = 0x7F
        model.vars.var_length_includecrc.value_forced = True
        model.vars.crc_poly.value_forced = model.vars.crc_poly.var_enum.CCITT_16
        model.vars.crc_seed.value_forced = 0x00000000
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.LSB_FIRST
        model.vars.crc_bit_endian.value_forced = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        model.vars.crc_byte_endian.value_forced = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        model.vars.crc_pad_input.value_forced = False
        model.vars.crc_invert.value_forced = False

        # Variables that are optional inputs in the Base profile and need to be explicitly set
        model.vars.test_ber.value_forced = False
        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Default
        model.vars.syncword_tx_skip.value_forced = False
        model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.NONE
        model.vars.white_seed.value_forced = 0
        model.vars.white_output_bit.value_forced = 0
        model.vars.asynchronous_rx_enable.value_forced = False
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
        model.vars.var_length_adjust.value_forced = 0
        model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.LSB_FIRST

    def _lookup_from_longrange_mode(self, model):
        #This function calculates some variables/registers based on the longrange mode

        #Read the mode and fec_en from the profile inputs (not yet written to model vars)
        longrange_mode = model.profile.inputs.longrange_mode.var_value
        fec_en = model.profile.inputs.fec_en.var_value

        # We need to modify the net bitrate (data rate) based on whether FEC is enabled or not
        if fec_en is not None and fec_en != model.vars.fec_en.var_enum.NONE:
            fec_factor=2
        else:
            fec_factor=1

        if longrange_mode == model.vars.longrange_mode.var_enum.LR_1p2k:
            model.vars.bitrate.value_forced = 1200//fec_factor
            model.vars.deviation.value_forced = 2400

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_2p4k:
            model.vars.bitrate.value_forced = 2400//fec_factor
            model.vars.deviation.value_forced = 4800
            if model.profile.inputs.base_frequency_hz.var_value <= 500e6:
                model.vars.AGC_GAINSTEPLIM0_CFLOOPDEL.value_forced = 25

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_4p8k:
            model.vars.bitrate.value_forced = 4800//fec_factor
            model.vars.deviation.value_forced = 9600

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_9p6k:
            model.vars.bitrate.value_forced = 9600//fec_factor
            model.vars.deviation.value_forced = 19200

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_19p2k:
            model.vars.bitrate.value_forced = 19200//fec_factor
            model.vars.deviation.value_forced = 38400

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_38p4k:
            model.vars.bitrate.value_forced = 38400//fec_factor
            model.vars.deviation.value_forced = 76800

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_80p0k:
            model.vars.bitrate.value_forced = 80000//fec_factor
            model.vars.deviation.value_forced = 160000