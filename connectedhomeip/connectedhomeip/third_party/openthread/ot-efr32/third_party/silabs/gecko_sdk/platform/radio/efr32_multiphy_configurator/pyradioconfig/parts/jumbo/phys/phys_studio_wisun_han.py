from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy


class PhysStudioWisunHanJumbo(IPhy):

    ### PHYs Tested by Apps ###

    def PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_han,
                            readable_name='Wi-SUN ECHONET, JP-920MHz, 1b (2FSK 50kbps mi=1.0)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_han_mode.value = model.vars.wisun_han_mode.var_enum.Mode1b
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.JP
        phy.profile_inputs.wisun_operating_class.value = 1

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 920600000
        phy.profile_inputs.channel_spacing_hz.value = 200000

        return phy

    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.wisun_han,
                            readable_name='Wi-SUN ECHONET, JP-920MHz, 2b (2FSK 100kbps mi=1.0)', phy_name=phy_name)

        # Wi-SUN Inputs
        phy.profile_inputs.wisun_han_mode.value = model.vars.wisun_han_mode.var_enum.Mode2b
        phy.profile_inputs.wisun_reg_domain.value = model.vars.wisun_reg_domain.var_enum.JP
        phy.profile_inputs.wisun_operating_class.value = 2

        # Default xtal frequency of 38.4MHz
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        # Temporary redundant inputs for base frequency and channel spacing (required due to Studio UI limitations)
        phy.profile_inputs.base_frequency_hz.value = 920900000
        phy.profile_inputs.channel_spacing_hz.value = 400000

        return phy