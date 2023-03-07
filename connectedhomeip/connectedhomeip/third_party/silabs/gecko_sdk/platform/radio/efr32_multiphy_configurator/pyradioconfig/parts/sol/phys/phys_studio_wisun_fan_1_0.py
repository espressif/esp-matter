from pyradioconfig.parts.ocelot.phys.phys_studio_wisun_fan_1_0 import PhysStudioWisunFanOcelot
from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys


@do_not_inherit_phys
class PhysStudioWisunFanSol(PhysStudioWisunFanOcelot):

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-30
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-31
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-33
    def PHY_IEEE802154_WISUN_470MHz_2GFSK_50kbps_1b_CN(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_470MHz_2GFSK_50kbps_1b_CN(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-34
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-35
    def PHY_IEEE802154_WISUN_470MHz_2GFSK_100kbps_2a_CN(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_470MHz_2GFSK_100kbps_2a_CN(model)

        return phy

    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-37
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-85
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_150kbps_3_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_868MHz_2GFSK_150kbps_3_EU(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-38
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_200kbps_4a_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_200kbps_4a_NA(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-39
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_200kbps_4b_JP(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_200kbps_4b_JP(model)

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-40
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_300kbps_5_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_300kbps_5_NA(model)

        return phy
