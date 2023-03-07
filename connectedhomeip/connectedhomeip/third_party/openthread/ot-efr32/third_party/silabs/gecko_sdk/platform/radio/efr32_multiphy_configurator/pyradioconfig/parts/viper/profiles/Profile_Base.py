from pyradioconfig.parts.bobcat.profiles.Profile_Base import Profile_Base_Bobcat
from pyradioconfig.parts.common.profiles.viper_regs import build_modem_regs_viper
from pyradioconfig.parts.viper.profiles.sw_profile_outputs_common import sw_profile_outputs_common_viper

class profile_base_viper(Profile_Base_Bobcat):

    def __init__(self):
        super().__init__()
        self._description = "Profile used for most PHYs"
        self._family = "viper"
        self._sw_profile_outputs_common_viper = sw_profile_outputs_common_viper()

    def buildRegisterOutputs(self, model, profile):
        build_modem_regs_viper(model, profile, self._family)

    def build_info_profile_outputs(self, model, profile):
        self._sw_profile_outputs_common_viper.build_info_outputs(model, profile)

