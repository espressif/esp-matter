from pyradioconfig.parts.ocelot.profiles.Profile_Mbus import Profile_Mbus_Ocelot
from pyradioconfig.parts.sol.profiles.sw_profile_outputs_common import sw_profile_outputs_common_sol

class Profile_Mbus_Sol(Profile_Mbus_Ocelot):

    def __init__(self):
        super().__init__()
        self._family = "sol"
        self._sw_profile_outputs_common = sw_profile_outputs_common_sol()

    def buildProfileModel(self, model):
        profile = super().buildProfileModel(model)
        self.make_linked_io(profile, model.vars.fpll_band, 'crystal', readable_name="RF Frequency Planning Band")

        return profile

    def mbus_profile_radio_common(self, model):
        super().mbus_profile_radio_common(model)
        model.vars.ofdm_option.value_forced = model.vars.ofdm_option.var_enum.OPT1