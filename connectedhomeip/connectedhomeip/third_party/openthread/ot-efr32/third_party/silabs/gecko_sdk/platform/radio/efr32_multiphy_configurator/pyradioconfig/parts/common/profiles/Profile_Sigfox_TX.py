from pyradioconfig.parts.common.profiles.dumbo_regs import *
from pyradioconfig.parts.common.profiles.jumbo_regs import *
from pyradioconfig.parts.common.profiles.nixi_regs import *
from pyradioconfig.parts.common.profiles.ocelot_regs import *
from pyradioconfig.parts.common.profiles.profile_common import *
from pyradioconfig.parts.common.profiles.profile_modem import *

from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier


class Profile_Sigfox_TX(IProfile):

    def __init__(self):
        self._profileName = "Sigfox_TX"
        self._readable_name = "Sigfox TX Profile"
        self._category = ""
        self._description = "Profile used for Transmit only Sigfox configurations"
        self._default = False
        self._activation_logic = ""
        self._family = "dumbo"


    """
    Builds inputs, forced, outputs into modem model
    """
    def buildProfileModel(self, model):


        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        # Add required and optional inputs
        self.build_required_profile_inputs(model, profile)
        self.build_optional_profile_inputs(model, profile)

        #Frame configuration inputs
        self.build_frame_configuration_inputs(model, profile)

        # Sigfox profile does not include advanced inputs, but we do need an output for IR cal power level for rail_scripts to consume
        # So add that here. It will default to 0 (which triggers to change to IR cal).
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.RAIL_CONFIG, readable_name='IR cal power level (amplitude)'))

        # Informational Output
        buildModemInfoOutputs(model, profile)

        # RAIL Outputs
        buildRailOutputs(model, profile)

        # Output fields
        buildFrameOutputs(model, profile, family=family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

        if family == "dumbo":
            build_modem_regs_dumbo(model, profile, family = family)   # Clean this up as a next step...
        elif family == "jumbo":
            build_modem_regs_jumbo(model, profile, family = family)   # Clean this up as a next step...
        elif family == "nerio":
            build_modem_regs_jumbo(model, profile, family = family)   # Clean this up as a next step...
        elif family == "nixi":
            build_modem_regs_nixi(model, profile, family = family)    # Clean this up as a next step...

        if family == 'nerio' or family == "nixi":
            buildLongRangeOutputs(model, profile)

        build_ircal_sw_vars(model, profile)

        # The above code is really just a copy/paste of the Base profile with some inputs commnted out.
        # It should be diff'ed against the Base profile for accuracy.
        # Now set default values for all of the above inputs.  Assume any defaults above
        # were just copy/pasted from the base profile and set the correct values here

        # Modem related input defaults
        profile.inputs.bitrate.value_limit_min = 100
        profile.inputs.bitrate.value_limit_max = 600
        profile.inputs.xtal_frequency_hz.default = 38400000
        profile.inputs.preamble_length.default = 16

        # Frame configuration related defaults

        # Packet Inputs
        profile.inputs.frame_bitendian.default = model.vars.frame_bitendian.var_enum.LSB_FIRST
        profile.inputs.frame_length_type.default = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        profile.inputs.payload_white_en.default = False
        profile.inputs.payload_crc_en.default = True

        # Variable length includes header
        profile.inputs.header_en.default = True
        profile.inputs.header_size.default = 1
        profile.inputs.header_calc_crc.default = False
        profile.inputs.header_white_en.default = False

        profile.inputs.var_length_numbits.default = 7
        profile.inputs.var_length_bitendian.default = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        profile.inputs.var_length_shift.default = 0
        profile.inputs.var_length_minlength.default = 5
        profile.inputs.var_length_maxlength.default = 0x7F
        profile.inputs.var_length_includecrc.default = True
        profile.inputs.var_length_adjust.default = 0

        # CRC Inputs
        profile.inputs.crc_poly.default = model.vars.crc_poly.var_enum.CCITT_16
        profile.inputs.crc_seed.default = long(0x00000000)
        profile.inputs.crc_input_order.default = model.vars.crc_input_order.var_enum.LSB_FIRST
        profile.inputs.crc_bit_endian.default = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        profile.inputs.crc_byte_endian.default = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        profile.inputs.crc_pad_input.default = False
        profile.inputs.crc_invert.default = False


        # Misc default inputs that now have to be forced
        profile.inputs.fec_en.default = model.vars.fec_en.var_enum.NONE
        profile.inputs.fixed_length_size.default = 1

        self._set_frame_coding(model, profile)

        profile.inputs.frame_type_0_length.default = 0
        profile.inputs.frame_type_0_valid.default = False
        profile.inputs.frame_type_1_length.default = 0
        profile.inputs.frame_type_1_valid.default = False
        profile.inputs.frame_type_2_length.default = 0
        profile.inputs.frame_type_2_valid.default = False
        profile.inputs.frame_type_3_length.default = 0
        profile.inputs.frame_type_3_valid.default = False
        profile.inputs.frame_type_4_length.default = 0
        profile.inputs.frame_type_4_valid.default = False
        profile.inputs.frame_type_5_length.default = 0
        profile.inputs.frame_type_5_valid.default = False
        profile.inputs.frame_type_6_length.default = 0
        profile.inputs.frame_type_6_valid.default = False
        profile.inputs.frame_type_7_length.default = 0
        profile.inputs.frame_type_7_valid.default = False
        profile.inputs.frame_type_bits.default = 3
        profile.inputs.frame_type_loc.default = 0
        profile.inputs.frame_type_lsbit.default = 0

        profile.inputs.var_length_byteendian.default = model.vars.var_length_byteendian.var_enum.LSB_FIRST

        profile.inputs.white_output_bit.default = 0
        profile.inputs.white_poly.default = model.vars.white_poly.var_enum.NONE
        profile.inputs.white_seed.default = 0

        return profile

    def _set_frame_coding(self, model, profile):
        profile.inputs.frame_coding.default = model.vars.frame_coding.var_enum.NONE

    def build_required_profile_inputs(self, model, profile):
        IProfile.make_required_input(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=long(100000000),
                                     value_limit_max=long(2480000000), units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0, value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)

        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.preamble_length, "preamble",
                                     readable_name="Preamble Length Total", value_limit_min=0, value_limit_max=2097151)
        IProfile.make_required_input(profile, model.vars.bitrate, "modem", readable_name="Chiprate",
                                     value_limit_min=100, value_limit_max=2000000,
                                     units_multiplier=UnitsMultiplier.KILO)

    def build_optional_profile_inputs(self, model, profile):
        IProfile.make_optional_input(profile, model.vars.syncword_tx_skip, "syncword",
                                     readable_name="Sync Word TX Skip", default=False)
        IProfile.make_optional_input(profile, model.vars.test_ber, "testing", default=False,
                                     readable_name="Reconfigure for BER testing")

    def build_frame_configuration_inputs(self, model, profile):
        buildFrameInputs(model, profile, family=self._family)
        buildCrcInputs(model, profile)
        buildWhiteInputs(model, profile)
        buildFecInputs(model, profile)

    def profile_calculate(self, model):
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED
        model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.NRZ
        model.vars.modulation_type.value_forced = model.vars.modulation_type.var_enum.DBPSK
        model.vars.shaping_filter.value_forced = model.vars.shaping_filter.var_enum.Root_Raised_Cosine
        model.vars.shaping_filter_param.value_forced = 0.0
        model.vars.dsss_chipping_code.value_forced = long(0)
        model.vars.dsss_len.value_forced = 0
        model.vars.dsss_spreading_factor.value_forced = 0
        model.vars.agc_power_target.value_forced = 4
        model.vars.errors_in_timing_window.value_forced = 1
        model.vars.rx_xtal_error_ppm.value_forced = 0
        model.vars.tx_xtal_error_ppm.value_forced = 0
        model.vars.deviation.value_forced = 0
        model.vars.agc_hysteresis.value_forced = 0
        model.vars.agc_settling_delay.value_forced = 40
        model.vars.baudrate_tol_ppm.value_forced = 0
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0
        model.vars.timing_sample_threshold.value_forced = 0
        model.vars.number_of_timing_windows.value_forced = 0
        model.vars.timing_detection_threshold.value_forced = 75
        #profile.outputs.SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX.override = 0x3C00F
        model.vars.pll_bandwidth_tx.value_forced = model.vars.pll_bandwidth_tx.var_enum.BW_3000KHz
        model.vars.asynchronous_rx_enable.value_forced = False

        model.vars.channel_spacing_hz.value_forced = 0

        model.vars.preamble_pattern.value_forced = 1
        model.vars.preamble_pattern_len.value_forced = 2
        model.vars.syncword_length.value_forced = 13
        model.vars.syncword_0.value_forced = long(0x6B)
        model.vars.syncword_1.value_forced = long(0x0)

        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Default


