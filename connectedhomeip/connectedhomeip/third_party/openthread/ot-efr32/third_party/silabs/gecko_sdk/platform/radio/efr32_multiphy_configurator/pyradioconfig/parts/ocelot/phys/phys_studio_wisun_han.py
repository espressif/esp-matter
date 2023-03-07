from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys
from pyradioconfig.parts.jumbo.phys.phys_studio_wisun_han import PhysStudioWisunHanJumbo

@do_not_inherit_phys
class PhysStudioWisunHanOcelot(PhysStudioWisunHanJumbo):

    ### JP Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-170
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-169
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

