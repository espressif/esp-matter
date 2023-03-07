from pyradioconfig.parts.lynx.profiles.Profile_Base import Profile_Base_Lynx
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.common.profiles.profile_common import buildCrcInputs, buildFecInputs, buildWhiteInputs, \
    buildFrameInputs, buildFrameOutputs, buildCrcOutputs, buildWhiteOutputs, buildFecOutputs, build_ircal_sw_vars
from pyradioconfig.parts.common.profiles.profile_modem import buildModemAdvancedInputs, buildModemInfoOutputs, \
    buildRailOutputs
from pyradioconfig.parts.common.profiles.leopard_regs import build_modem_regs_leopard

class profile_base_leopard(Profile_Base_Lynx):

    def __init__(self):
        self._profileName = "Base"
        self._readable_name = "Base Profile"
        self._category = ""
        self._description = "Profile used for most PHYs"
        self._default = True
        self._activation_logic = ""
        self._family = "leopard"

    def buildProfileModel(self, model):

        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        # Add inputs
        self.build_required_profile_inputs(model, profile)
        self.build_optional_profile_inputs(model, profile)
        self.build_advanced_profile_inputs(model, profile)
        self.build_hidden_profile_inputs(model, profile)
        self.build_deprecated_profile_inputs(model, profile)

        # Frame configuration Inputs
        self.build_frame_configuration_inputs(model, profile)

        # Modem Advanced Inputs
        buildModemAdvancedInputs(model, profile, family)

        # Informational Output
        buildModemInfoOutputs(model, profile)

        # RAIL Outputs
        buildRailOutputs(model, profile)

        # Output fields
        buildFrameOutputs(model, profile, family=family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

        self.buildRegisterOutputs(model, profile)

        build_ircal_sw_vars(model, profile)

        return profile

    def build_required_profile_inputs(self, model, profile):

        self.make_required_input(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=100000000,
                                     value_limit_max=2480000000, units_multiplier=UnitsMultiplier.MEGA)
        self.make_required_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0, value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)
        self.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        self.make_required_input(profile, model.vars.rx_xtal_error_ppm, "crystal",
                                     readable_name="RX Crystal Accuracy", value_limit_min=0, value_limit_max=200)
        self.make_required_input(profile, model.vars.tx_xtal_error_ppm, "crystal",
                                     readable_name="TX Crystal Accuracy", value_limit_min=0, value_limit_max=200)
        self.make_required_input(profile, model.vars.syncword_0, "syncword", readable_name="Sync Word 0",
                                     value_limit_min=0, value_limit_max=0xffffffff)
        self.make_required_input(profile, model.vars.syncword_1, "syncword", readable_name="Sync Word 1",
                                     value_limit_min=0, value_limit_max=0xffffffff)
        self.make_required_input(profile, model.vars.syncword_length, "syncword",
                                     readable_name="Sync Word Length", value_limit_min=0, value_limit_max=32)
        self.make_required_input(profile, model.vars.preamble_pattern_len, "preamble",
                                     readable_name="Preamble Pattern Length", value_limit_min=0, value_limit_max=4)
        self.make_required_input(profile, model.vars.preamble_length, "preamble",
                                     readable_name="Preamble Length Total", value_limit_min=0,
                                     value_limit_max=2097151)
        self.make_required_input(profile, model.vars.preamble_pattern, "preamble",
                                     readable_name="Preamble Base Pattern", value_limit_min=0, value_limit_max=15)
        self.make_required_input(profile, model.vars.modulation_type, "modem", readable_name="Modulation Type")
        self.make_required_input(profile, model.vars.deviation, "modem", readable_name="Deviation",
                                     value_limit_min=0, value_limit_max=1000000,
                                     units_multiplier=UnitsMultiplier.KILO)
        self.make_required_input(profile, model.vars.bitrate, "modem", readable_name="Bitrate",
                                     value_limit_min=100, value_limit_max=2000000,
                                     units_multiplier=UnitsMultiplier.KILO)
        self.make_required_input(profile, model.vars.baudrate_tol_ppm, "modem",
                                     readable_name="Baudrate Tolerance", value_limit_min=0, value_limit_max=200000)
        self.make_required_input(profile, model.vars.shaping_filter, "modem", readable_name="Shaping Filter")
        self.make_required_input(profile, model.vars.fsk_symbol_map, "modem", readable_name="FSK symbol map")
        self.make_required_input(profile, model.vars.diff_encoding_mode, "symbol_coding",
                                 readable_name="Differential Encoding Mode")
        self.make_required_input(profile, model.vars.dsss_chipping_code, "symbol_coding",
                                 readable_name="DSSS Chipping Code Base", value_limit_min=0,
                                 value_limit_max=0xffffffff)
        self.make_required_input(profile, model.vars.dsss_len, "symbol_coding",
                                 readable_name="DSSS Chipping Code Length", value_limit_min=0,
                                 value_limit_max=32)
        self.make_required_input(profile, model.vars.dsss_spreading_factor, "symbol_coding",
                                 readable_name="DSSS Spreading Factor", value_limit_min=0, value_limit_max=100)

    def build_optional_profile_inputs(self, model, profile):

        self.make_optional_input(profile, model.vars.syncword_tx_skip, "syncword",
                                     readable_name="Sync Word TX Skip", default=False)
        self.make_optional_input(profile, model.vars.asynchronous_rx_enable, "modem",
                                     readable_name="Enable Asynchronous direct mode", default=False)
        self.make_optional_input(profile, model.vars.symbol_encoding, "symbol_coding",
                                     readable_name="Symbol Encoding",
                                     default=model.vars.symbol_encoding.var_enum.NRZ)
        self.make_optional_input(profile, model.vars.manchester_mapping, "symbol_coding",
                                     readable_name="Manchester Code Mapping",
                                     default=model.vars.manchester_mapping.var_enum.Default)
        self.make_optional_input(profile, model.vars.test_ber, "testing",
                                     readable_name="Reconfigure for BER testing", default=False)
        self.make_optional_input(profile, model.vars.shaping_filter_param, "modem",
                                 readable_name="Shaping Filter Parameter (BT or R)", value_limit_min=0.3,
                                 value_limit_max=1.5, fractional_digits=2, default=0.5)

    def build_advanced_profile_inputs(self, model, profile):
        self.make_linked_io(profile, model.vars.lo_injection_side, "Advanced",
                            readable_name="Injection side")

    def build_hidden_profile_inputs(self, model, profile):

        self.make_hidden_input(profile, model.vars.src1_range_available_minimum, "modem",
                                   readable_name="SRC range minimum", value_limit_min=125, value_limit_max=155)
        self.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec3, "modem",
                                   readable_name="1=Allow input decimation filter decimate by 3 in cost function",
                                   value_limit_min=0, value_limit_max=1)
        self.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec8, "modem",
                                   readable_name="1=Allow input decimation filter decimate by 8 in cost function",
                                   value_limit_min=0, value_limit_max=1)

    def build_deprecated_profile_inputs(self, model, profile):
        pass

    def build_frame_configuration_inputs(self, model, profile):

        buildFrameInputs(model, profile, family=self._family)
        buildCrcInputs(model, profile)
        buildWhiteInputs(model, profile)
        buildFecInputs(model, profile)

    def buildRegisterOutputs(self, model, profile):

        build_modem_regs_leopard(model, profile, family=self._family)
