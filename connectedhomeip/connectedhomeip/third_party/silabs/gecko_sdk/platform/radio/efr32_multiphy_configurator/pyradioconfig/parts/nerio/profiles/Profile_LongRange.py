from pyradioconfig.parts.jumbo.profiles.Profile_LongRange import Profile_Long_Range_Jumbo
from pyradioconfig.parts.common.profiles.jumbo_regs import build_modem_regs_jumbo
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs, buildLongRangeOutputs

class Profile_Long_Range_Nerio(Profile_Long_Range_Jumbo):

    def __init__(self):
        super(Profile_Long_Range_Nerio, self).__init__()
        self._family = 'nerio'

    def build_register_profile_outputs(self, model, profile):
        family = self._family
        build_modem_regs_jumbo(model, profile, family)
        buildFrameOutputs(model, profile, family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
        buildLongRangeOutputs(model, profile)
        pass