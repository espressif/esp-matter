from pyradioconfig.parts.ocelot.phys.Phys_Studio_Base_Standard_SUNFSK import PHYS_Studio_Base_Standard_SUNFSK_Ocelot
from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys

@do_not_inherit_phys
class PHYS_Studio_Base_Standard_SUNFSK_Sol(PHYS_Studio_Base_Standard_SUNFSK_Ocelot):

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-41
    def PHY_IEEE802154_SUN_FSK_920MHz_4FSK_400kbps(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_SUN_FSK_920MHz_4FSK_400kbps(model, phy_name=phy_name)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-2182
    def PHY_IEEE802154_SUN_FSK_915MHz_2FSK_10kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base,
                            readable_name='IEEE 802.15.4 SUN FSK 915MHz 2FSK 10kbps mi=1.0', phy_name=phy_name)

        # Start with the SUN FSK base function
        self.SUN_FSK_base(phy, model)

        # Select the modulation type
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2

        # Select the unique parameters for this PHY
        phy.profile_inputs.base_frequency_hz.value = 902200000  # Ch0 Frequency
        phy.profile_inputs.bitrate.value = 10000
        phy.profile_inputs.deviation.value = 5000
        phy.profile_inputs.channel_spacing_hz.value = 50000

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 7
        phy.profile_inputs.tx_xtal_error_ppm.value = 7

        return phy


    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-55
    def PHY_IEEE802154_SUN_FSK_896MHz_2FSK_40kbps(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_SUN_FSK_896MHz_2FSK_40kbps(model, phy_name=phy_name)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-56
    def PHY_IEEE802154_SUN_FSK_450MHz_4FSK_9p6kbps(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_SUN_FSK_450MHz_4FSK_9p6kbps(model, phy_name=phy_name)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-57
    def PHY_IEEE802154_SUN_FSK_450MHz_2FSK_4p8kbps(self, model, phy_name=None):
        phy = super().PHY_IEEE802154_SUN_FSK_450MHz_2FSK_4p8kbps(model, phy_name=phy_name)
        return phy