from pyradioconfig.parts.ocelot.phys.Phys_Studio_Connect import PHYS_connect_Ocelot
from pyradioconfig.calculator_model_framework.decorators.phy_decorators import do_not_inherit_phys

@do_not_inherit_phys
class PHYS_Studio_Connect_Sol(PHYS_connect_Ocelot):

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-45
    def PHY_Studio_Connect_920MHz_2GFSK_100kbps(self, model, phy_name=None):
        phy = super().PHY_Studio_Connect_920MHz_2GFSK_100kbps(model)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-2192
    def PHY_Studio_Connect_915mhz_oqpsk_2Mcps_250kbps(self, model, phy_name=None):
        phy = super().PHY_Studio_Connect_915mhz_oqpsk_2Mcps_250kbps(model)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-43
    def PHY_Studio_Connect_915MHz_2GFSK_500kbps(self, model, phy_name=None):
        phy = super().PHY_Studio_Connect_915MHz_2GFSK_500kbps(model)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-44
    def PHY_Studio_Connect_863MHz_2GFSK_100kbps(self, model, phy_name=None):
        phy = super().PHY_Studio_Connect_863MHz_2GFSK_100kbps(model)
        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-42
    def PHY_Studio_Connect_490MHz_2GFSK_200kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Connect, phy_description='China 490',
                      readable_name="Connect 490MHz 2GFSK 200kbps", phy_name=phy_name)

        # Common base function for all connect PHYs
        PHYS_connect_Ocelot().Connect_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 200000
        phy.profile_inputs.deviation.value = 100000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 490000000
        phy.profile_inputs.channel_spacing_hz.value = 500000

        return phy
