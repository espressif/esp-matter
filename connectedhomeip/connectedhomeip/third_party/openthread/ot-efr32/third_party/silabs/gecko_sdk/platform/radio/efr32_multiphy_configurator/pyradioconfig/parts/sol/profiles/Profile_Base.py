from pyradioconfig.parts.ocelot.profiles.Profile_Base import Profile_Base_Ocelot
from pyradioconfig.parts.sol.profiles.frame_profile_inputs_common import frame_profile_inputs_common_sol
from pyradioconfig.parts.sol.profiles.sw_profile_outputs_common import sw_profile_outputs_common_sol

class Profile_Base_Sol(Profile_Base_Ocelot):

    def __init__(self):
        super().__init__()
        self._description = "Profile used for most PHYs"
        self._family = "sol"
        self._sw_profile_outputs_common = sw_profile_outputs_common_sol()
        self._frame_profile_inputs_common_sol = frame_profile_inputs_common_sol()

    def profile_calculate(self, model):

        #Call existing profile calculations
        super().profile_calculate(model)

        #Fixed variables (required inputs for other Profiles)
        #Using .value only so that we can force this in internal test PHYs
        model.vars.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT1

    def build_advanced_profile_inputs(self, model, profile):
        super().build_advanced_profile_inputs(model, profile)
        self.make_linked_io(profile, model.vars.fpll_band, 'crystal', readable_name="RF Frequency Planning Band")

    def build_hidden_profile_inputs(self, model, profile):
        super().build_hidden_profile_inputs(model, profile)
        self.make_hidden_input(profile, model.vars.dual_fefilt, "Advanced",
                                   readable_name="Dual front-end filter enable")
