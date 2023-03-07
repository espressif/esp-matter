from pyradioconfig.parts.ocelot.profiles.Profile_LongRange import Profile_Long_Range_Ocelot
from pyradioconfig.parts.common.profiles.margay_regs import build_modem_regs_margay
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs
from pyradioconfig.parts.margay.profiles.sw_profile_outputs_common import sw_profile_outputs_common_margay


class Profile_Long_Range_Margay(Profile_Long_Range_Ocelot):

    def __init__(self):
        super(Profile_Long_Range_Ocelot, self).__init__()
        self._family = 'margay'
        self._sw_profile_outputs_common = sw_profile_outputs_common_margay()

    def build_register_profile_outputs(self, model, profile):
        family = self._family
        build_modem_regs_margay(model, profile)
        buildFrameOutputs(model, profile, family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
