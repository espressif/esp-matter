from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy


class PhysStudioWisunFanJumbo(IPhy):

    ### PHYs Tested by Apps ###

    def PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, EU-868MHz, 1a (2FSK 50kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.EU
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 863100000
        phy.profile_inputs.channel_spacing_hz.value = 100000

        return phy

    def PHY_IEEE802154_WISUN_873MHz_2GFSK_50kbps_1a_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, EU-873MHz, 1a (2FSK 50kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.EU
        phy.profile_inputs.wisun_operating_class.value = 3

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 870100000
        phy.profile_inputs.channel_spacing_hz.value = 100000

        return phy

    def PHY_IEEE802154_WISUN_866MHz_2GFSK_50kbps_1a_IN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, IN-866MHz, 1a (2FSK 50kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.IN
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 865100000
        phy.profile_inputs.channel_spacing_hz.value = 100000

        return phy

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, NA-915MHz, 1b (2FSK 50kbps mi=1.0)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1b
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.NA
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 902200000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_470MHz_2GFSK_50kbps_1b_CN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, CN-470MHz, 1b (2FSK 50kbps mi=1.0)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1b
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.CN
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 470200000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0,
                            readable_name='Wi-SUN FAN, EU-868MHz, 2a (2FSK 100kbps mi=0.5)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.EU
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 863100000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_873MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, EU-873MHz, 2a (2FSK 100kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.EU
        phy.profile_inputs.wisun_operating_class.value = 4

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 870200000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_866MHz_2GFSK_100kbps_2a_IN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, IN-866MHz, 2a (2FSK 100kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.IN
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 865100000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_100kbps_2a_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, NA-915MHz, 2a (2FSK 100kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.NA
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 902200000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, NA-915MHz, 3 (2FSK 150kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode3
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.NA
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 902400000
        phy.profile_inputs.channel_spacing_hz.value = 400000

        return phy

    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, JP-920MHz, 2b (2FSK 100kbps mi=1.0)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2b
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.JP
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 920900000
        phy.profile_inputs.channel_spacing_hz.value = 400000

        return phy

    def PHY_IEEE802154_WISUN_868MHz_2GFSK_150kbps_3_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, EU-868MHz, 3 (2FSK 150kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode3
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.EU
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 863100000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_873MHz_2GFSK_150kbps_3_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, EU-873MHz, 3 (2FSK 150kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode3
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.EU
        phy.profile_inputs.wisun_operating_class.value = 4

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 870200000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_866MHz_2GFSK_150kbps_3_IN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, IN-866MHz, 3 (2FSK 150kbps mi=0.5)',
                            phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode3
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.IN
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 865100000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_200kbps_4a_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, NA-915MHz, 4a (2GFSK 200kbps mi=0.5)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode4a
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.NA
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 902400000
        phy.profile_inputs.channel_spacing_hz.value = 400000

        return phy

    def PHY_IEEE802154_WISUN_920MHz_2GFSK_200kbps_4b_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0, readable_name='Wi-SUN FAN, JP-920MHz, 4b (2GFSK 200kbps mi=1.0)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode4b
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.JP
        phy.profile_inputs.wisun_operating_class.value = 3

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 920800000
        phy.profile_inputs.channel_spacing_hz.value = 600000

        return phy

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_300kbps_5_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_fan_1_0,
                            readable_name='Wi-SUN FAN, NA-915MHz, 5 (2GFSK 300kbps mi=0.5)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode5
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.NA
        phy.profile_inputs.wisun_operating_class.value = 3

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 902600000
        phy.profile_inputs.channel_spacing_hz.value = 600000

        return phy