from pyradioconfig.parts.common.profiles.dumbo_regs import *
from pyradioconfig.parts.common.profiles.jumbo_regs import *
from pyradioconfig.parts.common.profiles.nixi_regs import *
from pyradioconfig.parts.common.profiles.panther_regs import *
from pyradioconfig.parts.common.profiles.lynx_regs import *
from pyradioconfig.parts.common.profiles.ocelot_regs import *
from pyradioconfig.parts.common.profiles.profile_common import *
from pyradioconfig.parts.common.profiles.profile_modem import *

from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier

from py_2_and_3_compatibility import *


class Profile_Base(IProfile):

    """
    Init internal variables
    """
    def __init__(self):
        self._profileName = "Base"
        self._readable_name = "Base Profile"
        self._category = ""
        self._description = "Profile used for most phy's"
        self._default = True
        self._activation_logic = ""

    """
    Builds inputs, forced, outputs into modem model
    """
    def buildProfileModel(self, model):

        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        IProfile.make_required_input(profile, model.vars.base_frequency_hz,      "operational_frequency", readable_name="Base Channel Frequency",     value_limit_min=long(100000000), value_limit_max=long(2480000000), units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz,     "operational_frequency", readable_name="Channel Spacing",            value_limit_min=0,         value_limit_max=10000000, units_multiplier=UnitsMultiplier.KILO)

        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz,      "crystal",  readable_name="Crystal Frequency",          value_limit_min=38000000,  value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.rx_xtal_error_ppm,      "crystal",  readable_name="RX Crystal Accuracy",        value_limit_min=0,         value_limit_max=200)
        IProfile.make_required_input(profile, model.vars.tx_xtal_error_ppm,      "crystal",  readable_name="TX Crystal Accuracy",        value_limit_min=0,         value_limit_max=200)

        IProfile.make_required_input(profile, model.vars.syncword_0,             "syncword", readable_name="Sync Word 0",                value_limit_min=long(0),        value_limit_max=long(0xffffffff))
        IProfile.make_required_input(profile, model.vars.syncword_1,             "syncword", readable_name="Sync Word 1",                value_limit_min=long(0),        value_limit_max=long(0xffffffff))
        IProfile.make_optional_input(profile, model.vars.syncword_tx_skip,       "syncword", readable_name="Sync Word TX Skip",          default=False)
        IProfile.make_required_input(profile, model.vars.syncword_length,        "syncword", readable_name="Sync Word Length",           value_limit_min=0,         value_limit_max=32)

        IProfile.make_required_input(profile, model.vars.preamble_pattern_len,   "preamble", readable_name="Preamble Pattern Length",    value_limit_min=0,         value_limit_max=4)
        IProfile.make_required_input(profile, model.vars.preamble_length,        "preamble", readable_name="Preamble Length Total",      value_limit_min=0,         value_limit_max=2097151)
        IProfile.make_required_input(profile, model.vars.preamble_pattern,       "preamble", readable_name="Preamble Base Pattern",      value_limit_min=0,         value_limit_max=15)

        IProfile.make_required_input(profile, model.vars.modulation_type,        "modem",    readable_name="Modulation Type"             )
        IProfile.make_required_input(profile, model.vars.deviation,              "modem",    readable_name="Deviation",                  value_limit_min=0,         value_limit_max=1000000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.bitrate,                "modem",    readable_name="Chiprate",                    value_limit_min=100,       value_limit_max=2000000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.baudrate_tol_ppm,       "modem",    readable_name="Baudrate Tolerance",         value_limit_min=0,         value_limit_max=200000)
        IProfile.make_required_input(profile, model.vars.shaping_filter,         "modem",    readable_name="Shaping Filter"              )
        IProfile.make_required_input(profile, model.vars.fsk_symbol_map,         "modem",    readable_name="FSK symbol map"              )
        IProfile.make_optional_input(profile, model.vars.shaping_filter_param, "modem", readable_name="Shaping Filter Parameter (BT or R)", value_limit_min=0.3, value_limit_max=1.5, fractional_digits=2, default=0.5)
        IProfile.make_optional_input(profile, model.vars.asynchronous_rx_enable, "modem",    readable_name="Enable Asynchronous direct mode",    default=False )

        IProfile.make_required_input(profile, model.vars.diff_encoding_mode,     "symbol_coding",  readable_name="Differential Encoding Mode"  )
        IProfile.make_optional_input(profile, model.vars.symbol_encoding,        "symbol_coding",  readable_name="Symbol Encoding", default=model.vars.symbol_encoding.var_enum.NRZ)
        IProfile.make_optional_input(profile, model.vars.manchester_mapping,     "symbol_coding",  readable_name="Manchester Code Mapping", default=model.vars.manchester_mapping.var_enum.Default)
        IProfile.make_required_input(profile, model.vars.dsss_chipping_code,     "symbol_coding",  readable_name="DSSS Chipping Code Base",    value_limit_min=long(0),        value_limit_max=long(0xffffffff))
        IProfile.make_required_input(profile, model.vars.dsss_len,               "symbol_coding",  readable_name="DSSS Chipping Code Length",  value_limit_min=0,         value_limit_max=32)
        IProfile.make_required_input(profile, model.vars.dsss_spreading_factor,  "symbol_coding",  readable_name="DSSS Spreading Factor",      value_limit_min=0,         value_limit_max=100)

        IProfile.make_optional_input(profile, model.vars.test_ber,               "testing", readable_name="Reconfigure for BER testing",  default=False)

        IProfile.make_linked_io(profile, model.vars.lo_injection_side,      "Advanced", readable_name="Injection side")

        if family.lower() not in ["dumbo", "jumbo", "nerio", "nixi"]:
            # 40nm Series 2 enable optional input for SRC range minimum
            IProfile.make_hidden_input(profile, model.vars.src1_range_available_minimum, "modem", readable_name="SRC range minimum",      value_limit_min=125,         value_limit_max=155)

            # Series 2: optional input to disallow DEC0 enums of DEC3/DEC8 (any width)
            # Not allowed to use bools per Attila, so int flag instead
            IProfile.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec3, "modem", readable_name="1=Allow input decimation filter decimate by 3 in cost function",      value_limit_min=0,         value_limit_max=1)
            IProfile.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec8, "modem", readable_name="1=Allow input decimation filter decimate by 8 in cost function",      value_limit_min=0,         value_limit_max=1)


        # Frame configuration Inputs
        buildFrameInputs(model, profile, family=family)
        buildCrcInputs(model, profile)
        buildWhiteInputs(model, profile)
        buildFecInputs(model, profile)

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

        if family == "dumbo":
            build_modem_regs_dumbo(model, profile, family = family)   # Clean this up as a next step...
        elif family == "jumbo":
            build_modem_regs_jumbo(model, profile, family = family)   # Clean this up as a next step...
        elif family == "nerio":
            build_modem_regs_jumbo(model, profile, family = family)   # Clean this up as a next step...
            buildLongRangeOutputs(model, profile)
        elif family == "nixi":
            build_modem_regs_nixi(model, profile, family = family)    # Clean this up as a next step...
            buildLongRangeOutputs(model, profile)
        elif family == "panther":
            build_modem_regs_panther(model, profile, family = family)    # Clean this up as a next step...
            # Already defined elsewhere for Panther
            # buildLongRangeOutputs(model, profile)
        elif family == "lynx":
            build_modem_regs_lynx(model, profile, family = family)    # Clean this up as a next step...

        build_ircal_sw_vars(model, profile)

        return profile


    def profile_calculate(self, model):
        pass
