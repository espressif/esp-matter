from pyradioconfig.parts.ocelot.profiles.Profile_Mbus import Profile_Mbus_Ocelot
from pyradioconfig.parts.margay.profiles.sw_profile_outputs_common import sw_profile_outputs_common_margay
from pyradioconfig.parts.common.profiles.margay_regs import build_modem_regs_margay


class Profile_Mbus_Margay(Profile_Mbus_Ocelot):

    """
    Init internal variables
    """
    def __init__(self):
        self._family = "margay"
        self._profileName = "Mbus"
        self._readable_name = "Mbus Profile"
        self._category = ""
        self._description = "Profile used for Mbus phys"
        self._default = False
        self._activation_logic = ""
        self._sw_profile_outputs_common = sw_profile_outputs_common_margay()


    def _add_reg_profile_outputs(self, model, profile):
        build_modem_regs_margay(model, profile)