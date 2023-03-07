from pyradioconfig.parts.ocelot.phys.Phys_Studio_Base import PHYS_Studio_Base_Ocelot
from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

class PHYS_Studio_Base_Bobcat(IPhy):

    #Copy the 2FSK base function from Ocelot
    def Studio_2GFSK_base(self, phy, model):
        PHYS_Studio_Base_Ocelot().Studio_2GFSK_base(phy, model)

    # Owner     : Young-Joon Choi
    # Jira Link : https://jira.silabs.com/browse/PGBOBCATVALTEST-208
    def PHY_Studio_2450M_2GFSK_1Mbps_500K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 1Mbps 500K', phy_name=phy_name)

        # Start with the base function
        self.Studio_2GFSK_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 1000000
        phy.profile_inputs.deviation.value = 500000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 2450000000

        return phy

    # Owner     : Young-Joon Choi
    # Jira Link : https://jira.silabs.com/browse/PGBOBCATVALTEST-209
    def PHY_Studio_2450M_2GFSK_250Kbps_125K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 250Kbps 125K', phy_name=phy_name)

        # Start with the base function
        self.Studio_2GFSK_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.deviation.value = 125000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 2450000000

        return phy

    # Owner     : Young-Joon Choi
    # Jira Link : https://jira.silabs.com/browse/PGBOBCATVALTEST-210
    def PHY_Studio_2450M_2GFSK_2Mbps_1M(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 2Mbps 1M', phy_name=phy_name)

        # Start with the base function
        self.Studio_2GFSK_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 2000000
        phy.profile_inputs.deviation.value = 1000000

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 2450000000

        return phy
