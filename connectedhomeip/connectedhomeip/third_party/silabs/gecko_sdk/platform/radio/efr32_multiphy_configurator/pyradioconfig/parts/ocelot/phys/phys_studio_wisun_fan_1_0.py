from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys
from pyradioconfig.parts.jumbo.phys.phys_studio_wisun_fan_1_0 import PhysStudioWisunFanJumbo


@do_not_inherit_phys
class PhysStudioWisunFanOcelot(PhysStudioWisunFanJumbo):

    ### EU Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-166
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(model)

        #Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    #Apps-verified
    def PHY_IEEE802154_WISUN_873MHz_2GFSK_50kbps_1a_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_873MHz_2GFSK_50kbps_1a_EU(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-165
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Apps-verified
    def PHY_IEEE802154_WISUN_873MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_873MHz_2GFSK_100kbps_2a_EU(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Apps-verified
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_150kbps_3_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_868MHz_2GFSK_150kbps_3_EU(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

    # Apps-verified
    def PHY_IEEE802154_WISUN_873MHz_2GFSK_150kbps_3_EU(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_873MHz_2GFSK_150kbps_3_EU(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    ### NA Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-168
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Apps-verified
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_100kbps_2a_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_100kbps_2a_NA(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-167
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Apps-verified
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_200kbps_4a_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_200kbps_4a_NA(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Apps-verified
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_300kbps_5_NA(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_915MHz_2GFSK_300kbps_5_NA(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    ### JP Region ###

    #Apps-verified
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    #Apps-verified
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_200kbps_4b_JP(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_920MHz_2GFSK_200kbps_4b_JP(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    ### CN Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-1218
    def PHY_IEEE802154_WISUN_470MHz_2GFSK_50kbps_1b_CN(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_WISUN_470MHz_2GFSK_50kbps_1b_CN(model)

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-1219
    def PHY_IEEE802154_WISUN_470MHz_2GFSK_100kbps_2a_CN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, CN-470MHz, 2a (2FSK 100kbps mi=0.5)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.CN
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 470200000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy