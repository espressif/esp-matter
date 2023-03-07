from pyradioconfig.parts.ocelot.profiles.profile_wisun_han import ProfileWisunHanOcelot
from pyradioconfig.parts.common.profiles.margay_regs import build_modem_regs_margay
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs
from pyradioconfig.parts.margay.profiles.sw_profile_outputs_common import sw_profile_outputs_common_margay


class ProfileWisunHanMargay(ProfileWisunHanOcelot):

    def __init__(self):
        super().__init__()
        self._family = "margay"
        self._sw_profile_outputs_common = sw_profile_outputs_common_margay()

    def build_register_profile_outputs(self, model, profile):
        build_modem_regs_margay(model, profile)
        buildFrameOutputs(model, profile, self._family)
        buildCrcOutputs(model, profile, self._family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
