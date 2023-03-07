from pyradioconfig.parts.ocelot.phys.phys_studio_wisun_han import PhysStudioWisunHanOcelot
from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys


@do_not_inherit_phys
class PhysStudioWisunHanSol(PhysStudioWisunHanOcelot):

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-32
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-36
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET(model)

        return phy
