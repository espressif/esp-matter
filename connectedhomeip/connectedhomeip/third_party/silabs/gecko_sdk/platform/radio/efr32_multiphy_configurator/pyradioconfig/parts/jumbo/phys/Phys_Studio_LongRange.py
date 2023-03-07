from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

class PHYS_OQPSK_LoRa_Jumbo(IPhy):
    ####################################################################################################################
    # These PHYs originally developed for Nixi, then enabled for Jumbo and Nerio. Linked JIRA tickets points to Nixi PHYs
    #
    # ------------------------------------------------------------------------------------------------------------------
    # 434 MHz Basic PHYs by Tibor Palvolgyi: IOTPA_PROP-914, IOTPA_PROP-916, IOTPA_PROP-882, IOTPA_PROP-922
    # ------------------------------------------------------------------------------------------------------------------
    def PHY_Datasheet_LongRange_434MHz_1p2k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='434 MHz 1.2 kbps OQPSK DSSS SF8',
                            phy_description = 'Sensitivity is -127.5 dBm at 1% PER on 19 byte long packets. Frequency \
                            offset tolerance to 1 dB sensitivity degradation is +/ 1.2 kHz which translates to +/- \
                            1 ppm reference frequency (XO) accuracy at either side of the link at 434 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 434000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_1p2k

    def PHY_Datasheet_LongRange_434MHz_2p4k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='434 MHz 2.4 kbps OQPSK DSSS SF8',
                            phy_description = 'Sensitivity is -124.5 dBm at 1% PER on 19 byte long packets. Frequency \
                            offset tolerance to 1 dB sensitivity degradation is +/ 2.5 kHz which translates to +/- \
                            2.5 ppm reference frequency (XO) accuracy at either side of the link at 434 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 434000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_2p4k

    def PHY_Datasheet_LongRange_434MHz_4p8k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='434 MHz 4.8 kbps OQPSK DSSS SF8',
                            phy_description = 'Sensitivity is -122.5 dBm at 1% PER on 19 byte long packets. Frequency \
                            offset tolerance to 1 dB sensitivity degradation is +/ 5 kHz which translates to +/- \
                            5 ppm reference frequency (XO) accuracy at either side of the link at 434 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 434000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_4p8k

    def PHY_Datasheet_LongRange_434MHz_9p6k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='434 MHz 9.6 kbps OQPSK DSSS SF8',
                            phy_description = 'Sensitivity is -119 dBm at 1% PER on 19 byte long packets. Frequency \
                            offset tolerance to 1 dB sensitivity degradation is +/ 10 kHz which translates to +/- \
                            11.5 ppm reference frequency (XO) accuracy at either side of the link at 434 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 434000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_9p6k

    def PHY_Datasheet_LongRange_434MHz_19p2k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='434 MHz 19.2 kbps OQPSK DSSS SF8',
                            phy_description='Sensitivity is -117 dBm at 1% PER on 19 byte long packets. Frequency \
                            offset tolerance to 1 dB sensitivity degradation is +/ 20 kHz which translates to +/- \
                            20 ppm reference frequency (TCXO) accuracy at either side of the link at 915 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 434000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_19p2k

    # -----------------------------------------------------------------------------------------------
    # 915 MHz Basic PHYs
    # -----------------------------------------------------------------------------------------------

    # -----------------------------------------------------------------------------------------------
    # 1p2k w/o FEC Basic -- original Long Range PHY by Lijun
    # -----------------------------------------------------------------------------------------------

    def PHY_Datasheet_LongRange_915MHz_1p2k_NoFEC_Basic(self, model, phy_name = None):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915 MHz 1.2 kbps OQPSK DSSS SF8', phy_name=phy_name,
                            phy_description='Sensitivity is -127 dBm at 1% PER on 19 byte long packets. Frequency \
                             offset tolerance to 1 dB sensitivity degradation is +/ 5 kHz which translates to +/- \
                             0.5 ppm reference frequency (TCXO) accuracy at either side of the link at 915 MHz.')
        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 25000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_1p2k

     # -----------------------------------------------------------------------------------------------
     # Upscaling 1p2k PHYs to other data rates by Tibor
     # -----------------------------------------------------------------------------------------------

    def PHY_Datasheet_LongRange_915MHz_4p8k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915 MHz 4.8 kbps OQPSK DSSS SF8',
                            phy_description='Sensitivity is -120.5 dBm at 1% PER on 19 byte long packets. Frequency \
                             offset tolerance to 1 dB sensitivity degradation is +/ 5 kHz which translates to +/- \
                             2.5 ppm reference frequency (TCXO) accuracy at either side of the link at 915 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_4p8k

    # -----------------------------------------------------------------------------------------------
    # Upscaling 9p6k PHY to 19p2k w/o FEC by Tibor Palvolgyi - https://jira.silabs.com/browse/IOTPA_PROP-712
    # -----------------------------------------------------------------------------------------------

    def PHY_Datasheet_LongRange_915MHz_9p6k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915 MHz 9.6 kbps OQPSK DSSS SF8',
                            phy_description='Sensitivity is -118 dBm at 1% PER on 19 byte long packets. Frequency \
                             offset tolerance to 1 dB sensitivity degradation is +/ 10 kHz which translates to +/- \
                             5.5 ppm reference frequency (TCXO) accuracy at either side of the link at 915 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_9p6k

    # -----------------------------------------------------------------------------------------------
    # Upscaling 9p6k PHY to 19p2k w/o FEC by Tibor Palvolgyi - https://jira.silabs.com/browse/IOTPA_PROP-880
    # -----------------------------------------------------------------------------------------------

    def PHY_Datasheet_LongRange_915MHz_19p2k_NoFEC_Basic(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915 MHz 19.2 kbps OQPSK DSSS SF8',
                            phy_description='Sensitivity is -114.5 dBm at 1% PER on 19 byte long packets. Frequency \
                            offset tolerance to 1 dB sensitivity degradation is +/ 18 kHz which translates to +/- \
                            10 ppm reference frequency (TCXO) accuracy at either side of the link at 915 MHz.')

        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_19p2k

    pass

    # -----------------------------------------------------------------------------------------------
    # By Tibor Palvolgyi - https://jira.silabs.com/browse/MCUW_RADIO_CFG-1377
    # -----------------------------------------------------------------------------------------------

    def PHY_Studio_LongRange_915MHz_80k_FCC(self, model):
        phy = self._makePhy(model, model.profiles.Long_Range, readable_name='915 MHz 80kbps OQPSK DSSS SF8',
                            phy_description='Sensitivity is -107.5 dBm at 1% PER on 19 byte long packets. \
                                             This PHY is suitable for FCC 15.247 operation without frequency hopping.')

        phy.profile_inputs.base_frequency_hz.value = 915000000
        phy.profile_inputs.channel_spacing_hz.value = 35000
        phy.profile_inputs.longrange_mode.value = model.vars.longrange_mode.var_enum.LR_80k

    pass