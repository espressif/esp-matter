from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy


class PHYS_OQPSK_LoRa_Ocelot(IPhy):

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-80
    def PHY_Longrange_490M_OQPSK_DSSS8_2p4kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='490M OQPSK 2.4kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 490000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_2p4k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-79
    def PHY_Longrange_490M_OQPSK_DSSS8_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='490M OQPSK 4.8kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 490000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_4p8k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-82
    def PHY_Longrange_490M_OQPSK_DSSS8_9p6kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='490M OQPSK 9.6kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 490000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_9p6k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-83
    def PHY_Longrange_490M_OQPSK_DSSS8_19p2kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='490M OQPSK 19.2kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 490000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_19p2k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-185
    def PHY_Longrange_915M_OQPSK_DSSS8_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915M OQPSK 4.8kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_4p8k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-186
    def PHY_Longrange_915M_OQPSK_DSSS8_9p6kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915M OQPSK 9.6kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_9p6k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-187
    def PHY_Longrange_915M_OQPSK_DSSS8_19p2kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915M OQPSK 19.2kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_19p2k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-188
    def PHY_Longrange_915M_OQPSK_DSSS8_38p4kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915M OQPSK 38.4kbps DSSS8',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_38p4k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy

    # Owner: Young-Joon Choi
    # Jira Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-191
    def PHY_Longrange_915M_OQPSK_DSSS8_80p0kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='FCC 15.247 80 kbps OQPSK',
                            phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_80p0k
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        return phy