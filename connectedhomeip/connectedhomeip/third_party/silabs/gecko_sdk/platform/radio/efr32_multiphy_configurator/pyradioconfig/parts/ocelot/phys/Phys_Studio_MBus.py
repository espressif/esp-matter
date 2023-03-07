from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy


class PHYS_Mbus_Studio_Ocelot(IPhy):

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-30
    def PHY_wMbus_ModeNabef_4p8K_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 2/6/8/11, frameA (4.8k)', phy_name=phy_name)

        #Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN1a_4p8K

        #Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 16

        #Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 169406250
        phy.profile_inputs.channel_spacing_hz.value = 12500

        #Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        #Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-31
    def PHY_wMbus_ModeN2g_19p2k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 4/13, frameA (19.2k)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeNg

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 16

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 169437500
        phy.profile_inputs.channel_spacing_hz.value = 50000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-184
    def PHY_wMbus_ModeT_M2O_100k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus T M2O (100k, 3of6)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeT_M2O_100k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 38

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 868950000
        phy.profile_inputs.channel_spacing_hz.value = 1000000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6
        phy.profile_inputs.syncword_dualsync.value = False

        # Postamble length: Always set to 0, except for Mode T, Mode R or Mode S
        phy.profile_inputs.mbus_postamble_length.value = 1

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-179
    def PHY_wMbus_ModeC_M2O_100k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C M2O frameA (100k)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeC_M2O_100k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 38

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 868950000
        phy.profile_inputs.channel_spacing_hz.value = 1000000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-179
    def PHY_wMbus_ModeC_M2O_100k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C M2O frameB (100k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeC_M2O_100k_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-181
    def PHY_wMbus_ModeC_O2M_50k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C O2M frameA (50k)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeC_O2M_50k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 38

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 869525000
        phy.profile_inputs.channel_spacing_hz.value = 1000000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-181
    def PHY_wMbus_ModeC_O2M_50k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C O2M frameB (50k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeC_O2M_50k_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-183
    def PHY_wMbus_ModeS_32p768k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus S (32.768k, Manchester)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeS_32p768k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 30

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 868300000
        phy.profile_inputs.channel_spacing_hz.value = 300000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.Manchester
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Postamble length: Always set to 0, except for Mode T, Mode R or Mode S
        phy.profile_inputs.mbus_postamble_length.value = 1

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-30
    def PHY_wMbus_ModeNabef_4p8K_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 2/6/8/11, frameB (4.8k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeNabef_4p8K_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-33
    def PHY_wMbus_ModeNcd_2p4K_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 1/5/7/10, frameA (2.4k)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN1c_2p4K

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 16

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 169431250
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-33
    def PHY_wMbus_ModeNcd_2p4K_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 1/5/7/10, frameB (2.4k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeNcd_2p4K_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-31
    def PHY_wMbus_ModeN2g_19p2k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 4/13, frameB (19.2k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeN2g_19p2k_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-182
    def PHY_wMbus_ModeR_4p8k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus R2 (4.8k, Manchester)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeR_4p8k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 78

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 868330000
        phy.profile_inputs.channel_spacing_hz.value = 60000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.Manchester
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Postamble length: Always set to 0, except for Mode T, Mode R or Mode S
        phy.profile_inputs.mbus_postamble_length.value = 1

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-64
    def PHY_wMbus_ModeF_2p4k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus F, frameA (2.4k)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeF_2p4k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 78

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 433820000
        phy.profile_inputs.channel_spacing_hz.value = 50000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-64
    def PHY_wMbus_ModeF_2p4k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus F, frameB (2.4k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeF_2p4k_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-1131
    def PHY_wMbus_ModeN_6p4k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 3/9/12, frameA (6.4k)', phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN_6p4k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 16

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 169406250
        phy.profile_inputs.channel_spacing_hz.value = 12500

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-1131
    def PHY_wMbus_ModeN_6p4k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 3/9/12, frameB (6.4k)', phy_name=phy_name)

        # Start with the FrameA version
        self.PHY_wMbus_ModeN_6p4k_frameA(model)

        # Specify FrameB
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/MCUW_RADIO_CFG-1414
    def PHY_wMbus_ModeT_O2M_32p768k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus T O2M (32.768k, Manchester)',
                            phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeT_O2M_32p768k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 30

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value =  868300000
        phy.profile_inputs.channel_spacing_hz.value = 300000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.Manchester
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        # Postamble length: Always set to 0, except for Mode T, Mode R or Mode S
        phy.profile_inputs.mbus_postamble_length.value = 1

        return phy

    # Owner: Efrain Gaxiola
    # JIRA Link: https://jira.silabs.com/browse/MCUW_RADIO_CFG-1414
    # same as T mode, but with disabled block coder, dfl and crc. Needs software decoder, dfl, crc, decision can be made by first 6 bits of payload
    def PHY_wMbus_ModeTC_M2O_100k_noFrame(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus T M2O (100k, no framing)',
                            phy_name=phy_name)

        # Define the Mbus Mode
        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeT_M2O_100k

        # Set the TX preamble length
        phy.profile_inputs.preamble_length.value = 38

        # Center frequency and channel spacing
        phy.profile_inputs.base_frequency_hz.value = 868950000
        phy.profile_inputs.channel_spacing_hz.value = 1000000

        # Frame formatting and encoding
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.NoFormat
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        # Xtal frequency
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy