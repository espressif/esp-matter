from pyradioconfig.parts.ocelot.phys.Phys_Studio_MBus import PHYS_Mbus_Studio_Ocelot

class PHYS_Mbus_Studio_Sol(PHYS_Mbus_Studio_Ocelot):

    #def PHY_wMbus_ModeC_M2O_100k_frameA(self, model, phy_name=None):
        #pass

    def PHY_wMbus_ModeC_M2O_100k_frameB(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeC_O2M_50k_frameA(self, model, phy_name=None):
        #pass

    def PHY_wMbus_ModeC_O2M_50k_frameB(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeF_2p4k_frameA(self, model, phy_name=None):
        #pass

    def PHY_wMbus_ModeF_2p4k_frameB(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeN2g_19p2k_frameA(self, model, phy_name=None):
        #pass

    def PHY_wMbus_ModeN2g_19p2k_frameB(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeNabef_4p8K_frameA(self, model, phy_name=None):
        #pass

    def PHY_wMbus_ModeNabef_4p8K_frameB(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeNcd_2p4K_frameA(self, model, phy_name=None):
        #pass

    def PHY_wMbus_ModeNcd_2p4K_frameB(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeR_4p8k_frameA(self, model, phy_name=None):
        #pass

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

        # FIXME: new overrides for Sol
        phy.profile_outputs.FEFILT0_DIGMIXCTRL_DIGMIXFREQ.override     = 188200
        phy.profile_outputs.MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD.override = 100
        phy.profile_outputs.MODEM_BCRDEMODPMEXP_BCRCFESCHWIN.override  = 4
        phy.profile_outputs.SYNTH_IFFREQ_IFFREQ.override               = 28230
        return phy


    def PHY_wMbus_ModeTC_M2O_100k_noFrame(self, model, phy_name=None):
        pass

    #def PHY_wMbus_ModeT_M2O_100k_frameA(self, model, phy_name=None):
        #pass

    #def PHY_wMbus_ModeT_O2M_32p768k_frameA(self, model, phy_name=None):
        #pass
