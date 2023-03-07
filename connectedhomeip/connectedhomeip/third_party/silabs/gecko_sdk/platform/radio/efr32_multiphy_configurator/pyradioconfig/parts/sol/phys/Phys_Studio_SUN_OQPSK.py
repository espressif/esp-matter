from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

class Phys_Studio_SUN_OQPSK_Sol(IPhy):

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-62, 63, 64, 69
    def PHY_IEEE802154_SUN_920MHz_OQPSK_100kcps_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.SUN_OQPSK, readable_name='SUN JP-920MHz, OQPSK 100kcps',
                            phy_name=phy_name)

        # Select the correct SUN OQPSK chiprate
        phy.profile_inputs.sun_oqpsk_chiprate.value = model.vars.sun_oqpsk_chiprate.var_enum._100_KCPS

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 920600000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000


    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-62, 63, 64, 69
    def PHY_IEEE802154_SUN_915MHz_OQPSK_100kcps_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.SUN_OQPSK, readable_name='SUN NA-915MHz, OQPSK 100kcps',
                            phy_name=phy_name)

        # Select the correct SUN OQPSK chiprate
        phy.profile_inputs.sun_oqpsk_chiprate.value = model.vars.sun_oqpsk_chiprate.var_enum._100_KCPS

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902200000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Owner: Casey Weltzin
        # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-62, 63, 64, 69

    def PHY_IEEE802154_SUN_868MHz_OQPSK_100kcps_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.SUN_OQPSK, readable_name='SUN EU-868MHz, OQPSK 100kcps',
                            phy_name=phy_name)

        # Select the correct SUN OQPSK chiprate
        phy.profile_inputs.sun_oqpsk_chiprate.value = model.vars.sun_oqpsk_chiprate.var_enum._100_KCPS

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 868300000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Owner: Casey Weltzin
        # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-62, 63, 64, 69

    def PHY_IEEE802154_SUN_470MHz_OQPSK_100kcps_CN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.SUN_OQPSK, readable_name='SUN CN-470MHz, OQPSK 100kcps',
                            phy_name=phy_name)

        # Select the correct SUN OQPSK chiprate
        phy.profile_inputs.sun_oqpsk_chiprate.value = model.vars.sun_oqpsk_chiprate.var_enum._100_KCPS

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 470200000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

    # Owner: Casey Weltzin
    ## Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-65, 66, 67, 68
    def PHY_IEEE802154_SUN_915MHz_OQPSK_1000kcps_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.SUN_OQPSK, readable_name='SUN NA-915MHz, OQPSK 1000kcps',
                            phy_name=phy_name)

        # Select the correct SUN OQPSK chiprate
        phy.profile_inputs.sun_oqpsk_chiprate.value = model.vars.sun_oqpsk_chiprate.var_enum._1000_KCPS

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 904000000
        phy.profile_inputs.channel_spacing_hz.value = 2000000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000