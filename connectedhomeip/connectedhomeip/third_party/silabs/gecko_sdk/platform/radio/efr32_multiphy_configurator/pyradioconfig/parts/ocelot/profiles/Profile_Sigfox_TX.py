from pyradioconfig.parts.common.profiles.Profile_Sigfox_TX import Profile_Sigfox_TX, IProfile
from pyradioconfig.parts.common.profiles.profile_common import buildCrcInputs, buildWhiteInputs, buildFecInputs
from pyradioconfig.parts.ocelot.profiles.frame_profile_inputs_common import frame_profile_inputs_common_ocelot
from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier

class Profile_Sigfox_TX_Ocelot(Profile_Sigfox_TX):

    def __init__(self):
        super().__init__()
        self._family = "ocelot"
        self._frame_profile_inputs_common = frame_profile_inputs_common_ocelot()

    def buildProfileModel(self, model):
        # Start with the inherited profile
        profile = super(Profile_Sigfox_TX_Ocelot, self).buildProfileModel(model)

        # Add Ocelot-specific variables
        IProfile.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")
        IProfile.make_hidden_input(profile, model.vars.synth_settling_mode, 'modem', readable_name="Synth Settling Mode")

        #Deprecated inputs
        # These inputs were exposed on or after Ocelot Alpha 1 release, so they may be present in radioconf XML
        self.make_deprecated_input(profile, model.vars.max_tx_power_dbm)

        #Add register profile outputs
        self._add_reg_profile_outputs(model, profile)

        return profile

    def _set_frame_coding(self, model, profile):
        #Removed frame coding input on Ocelot
        pass

    def build_required_profile_inputs(self, model, profile):
        IProfile.make_required_input(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=100000000,
                                     value_limit_max=2480000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0, value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)

        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.preamble_length, "preamble",
                                     readable_name="Preamble Length Total", value_limit_min=0,
                                     value_limit_max=2097151)
        IProfile.make_required_input(profile, model.vars.bitrate, "modem", readable_name="Bitrate", value_limit_min=100,
                                     value_limit_max=2000000, units_multiplier=UnitsMultiplier.KILO)

    def build_frame_configuration_inputs(self, model, profile):
        self._frame_profile_inputs_common.build_frame_inputs(model, profile)
        buildCrcInputs(model, profile)
        buildWhiteInputs(model, profile)
        buildFecInputs(model, profile)

    def _add_reg_profile_outputs(self, model, profile):
        build_modem_regs_ocelot(model, profile, self._family)


















