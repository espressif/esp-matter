from pyradioconfig.parts.ocelot.profiles.Profile_Base import Profile_Base_Ocelot
from pyradioconfig.parts.margay.profiles.sw_profile_outputs_common import sw_profile_outputs_common_margay
from pyradioconfig.parts.margay.profiles.frame_profile_inputs_common import frame_profile_inputs_common_margay
from pyradioconfig.parts.common.profiles.margay_regs import build_modem_regs_margay


class Profile_Base_Margay(Profile_Base_Ocelot):

    """
    Init internal variables
    """
    def __init__(self):
        super().__init__()
        self._description = "Profile used for most PHYs on EFR32xG23 parts"
        self._family = "margay"
        self._sw_profile_outputs_common = sw_profile_outputs_common_margay()
        self._frame_profile_inputs_common = frame_profile_inputs_common_margay()

    def buildRegisterOutputs(self, model, profile):
        build_modem_regs_margay(model, profile)

