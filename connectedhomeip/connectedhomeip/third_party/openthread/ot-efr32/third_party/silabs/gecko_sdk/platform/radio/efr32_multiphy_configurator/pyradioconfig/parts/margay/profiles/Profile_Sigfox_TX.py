from pyradioconfig.parts.ocelot.profiles.Profile_Sigfox_TX import Profile_Sigfox_TX_Ocelot
from pyradioconfig.parts.margay.profiles.frame_profile_inputs_common import frame_profile_inputs_common_margay
from pyradioconfig.parts.common.profiles.margay_regs import build_modem_regs_margay


class Profile_Sigfox_TX_Margay(Profile_Sigfox_TX_Ocelot):

    def __init__(self):
        super(self.__class__, self).__init__()
        self._family = "margay"
        self._frame_profile_inputs_common = frame_profile_inputs_common_margay()

    def _add_reg_profile_outputs(self, model, profile):
        build_modem_regs_margay(model, profile)

















