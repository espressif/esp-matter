from pyradioconfig.parts.jumbo.profiles.Profile_LongRange import Profile_Long_Range_Jumbo
from pyradioconfig.parts.common.profiles.nixi_regs import *
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs, buildLongRangeOutputs

class Profile_Long_Range_Nixi(Profile_Long_Range_Jumbo):

    def __init__(self):
        super(Profile_Long_Range_Nixi, self).__init__()
        self._family = 'nixi'

    def build_register_profile_outputs(self, model, profile):
        family = self._family
        build_modem_regs_nixi(model, profile, family)
        buildFrameOutputs(model, profile, family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
        buildLongRangeOutputs(model, profile)

    def _disable_dynamic_BBSS(self, model):
        # Need only for Jumbo and Nerio
        pass



