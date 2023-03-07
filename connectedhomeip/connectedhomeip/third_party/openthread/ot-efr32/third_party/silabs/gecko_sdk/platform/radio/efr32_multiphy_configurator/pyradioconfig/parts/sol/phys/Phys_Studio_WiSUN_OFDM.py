from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

class Phys_Studio_WiSUN_OFDM_Sol(IPhy):

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-2 through https://jira.silabs.com/browse/PGSOLVALTEST-8
    def PHY_IEEE802154_WISUN_915MHz_OFDM_OPT1_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN NA-915MHz, OFDM OPTION 1',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 32-38 (MCS0-6)
        # ChanPlanID: 5 (902_928_1200, 1200kHz spacing, Ch0 903.2MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT1

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 903200000
        phy.profile_inputs.channel_spacing_hz.value = 1200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-9 through https://jira.silabs.com/browse/PGSOLVALTEST-15
    def PHY_IEEE802154_WISUN_915MHz_OFDM_OPT2_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN NA-915MHz, OFDM OPTION 2',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 48-54 (MCS0-6)
        # ChanPlanID: 4 (902_928_800, 800kHz spacing, Ch0 902.8MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT2

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902800000
        phy.profile_inputs.channel_spacing_hz.value = 800000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-9 through https://jira.silabs.com/browse/PGSOLVALTEST-15
    def PHY_IEEE802154_WISUN_920MHz_OFDM_OPT2_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN JP-920MHz, OFDM OPTION 2',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 48-54 (MCS0-6)
        # ChanPlanID: 24 (920_928_800, 800kHz spacing, Ch0 921.1MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT2

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 921100000
        phy.profile_inputs.channel_spacing_hz.value = 800000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-16 through https://jira.silabs.com/browse/PGSOLVALTEST-22
    def PHY_IEEE802154_WISUN_915MHz_OFDM_OPT3_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN NA-915MHz, OFDM OPTION 3',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 64-70 (MCS0-6)
        # ChanPlanID: 2 (902_928_400, 400kHz spacing, Ch0 902.4MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT3

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902400000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

        # Owner: Casey Weltzin
        # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-16 through https://jira.silabs.com/browse/PGSOLVALTEST-22

    def PHY_IEEE802154_WISUN_920MHz_OFDM_OPT3_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN JP-920MHz, OFDM OPTION 3',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 64-70 (MCS0-6)
        # ChanPlanID: 22 (920_928_400, 400kHz spacing, Ch0 920.9MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT3

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 920900000
        phy.profile_inputs.channel_spacing_hz.value = 400000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-23 through https://jira.silabs.com/browse/PGSOLVALTEST-29
    def PHY_IEEE802154_WISUN_915MHz_OFDM_OPT4_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN NA-915MHz, OFDM OPTION 4',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 80-86 (MCS0-6)
        # ChanPlanID: 1 (902_928_200, 200kHz spacing, Ch0 902.2MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT4

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902200000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-23 through https://jira.silabs.com/browse/PGSOLVALTEST-29
    def PHY_IEEE802154_WISUN_868MHz_OFDM_OPT4_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN EU-868MHz, OFDM OPTION 4',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 80-86 (MCS0-6)
        # ChanPlanID: 33 (863_870_200, 200kHz spacing, Ch0 863.1MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT4

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 863100000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # Jira Link: https://jira.silabs.com/browse/PGSOLVALTEST-23 through https://jira.silabs.com/browse/PGSOLVALTEST-29
    def PHY_IEEE802154_WISUN_920MHz_OFDM_OPT4_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN_OFDM, readable_name='Wi-SUN JP-920MHz, OFDM OPTION 4',
                            phy_name=phy_name)

        ### Frequency Band and Channel Parameters ###
        # PhyModeID: 80-86 (MCS0-6)
        # ChanPlanID: 21 (920_928_200, 200kHz spacing, Ch0 920.6MHz)

        # Select the correct Wi-SUN OFDM Option
        phy.profile_inputs.ofdm_option.value = model.vars.ofdm_option.var_enum.OPT4

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 920600000
        phy.profile_inputs.channel_spacing_hz.value = 200000
        phy.profile_inputs.fcs_type_802154.value = model.vars.fcs_type_802154.var_enum.FOUR_BYTE

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy