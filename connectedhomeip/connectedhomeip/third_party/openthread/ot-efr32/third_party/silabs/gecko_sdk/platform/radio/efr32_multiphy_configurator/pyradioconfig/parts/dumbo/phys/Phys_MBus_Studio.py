from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

from py_2_and_3_compatibility import *

class PHYS_MBus_Studio(IPhy):

    #
    #
    #
    def PHY_wMbus_ModeT_M2O_100k_no_postamble_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus T M2O, no postamble (100k, 3of6)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeT_M2O_100k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(868950000)
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        #Does need some workarounds, but we do need it
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    def PHY_wMbus_ModeT_O2M_32p768k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus T O2M (32.768k, Manchester)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeT_O2M_32p768k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(868300000)
        phy.profile_inputs.channel_spacing_hz.value = 300000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.Manchester
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    #same as T mode, but with disabled block coder, dfl and crc. Needs software decoder, dfl, crc, decision can be made by first 6 bits of payload
    def PHY_wMbus_ModeTC_M2O_100k_noFrame(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus T M2O (100k, no framing)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeT_M2O_100k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.NoFormat
        phy.profile_inputs.base_frequency_hz.value = long(868950000)
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        #phy.profile_inputs.fixed_length_size.value = 18 #first block in C mode: 2B sync + 10B +2B crc=14B. in T mode: (10B+2B crc)*1.5=18
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeC_M2O_100k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C M2O frameA (100k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeC_M2O_100k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(868950000)
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    def PHY_wMbus_ModeC_M2O_100k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C M2O frameB (100k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeC_M2O_100k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB
        phy.profile_inputs.base_frequency_hz.value = long(868950000)
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeC_O2M_50k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C O2M frameA (50k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeC_O2M_50k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(869525000)
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    def PHY_wMbus_ModeC_O2M_50k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus C O2M frameB (50k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeC_O2M_50k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB
        phy.profile_inputs.base_frequency_hz.value = long(869525000)
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeS_32p768k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus S (32.768k, Manchester)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeS_32p768k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(868300000)
        phy.profile_inputs.channel_spacing_hz.value = 300000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.Manchester
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeNabef_4p8K_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 2/6/8/11, frameA (4.8k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN1a_4p8K
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(169406250)
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    def PHY_wMbus_ModeNabef_4p8K_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 2/6/8/11, frameB (4.8k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN1a_4p8K
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB
        phy.profile_inputs.base_frequency_hz.value = long(169406250)
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeNcd_2p4K_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 1/5/7/10, frameA (2.4k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN1c_2p4K
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(169431250)
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    def PHY_wMbus_ModeNcd_2p4K_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 1/5/7/10, frameB (2.4k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeN1c_2p4K
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB
        phy.profile_inputs.base_frequency_hz.value = long(169431250)
        phy.profile_inputs.channel_spacing_hz.value = 12500
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeN2g_19p2k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 4/13, frameA (19.2k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeNg
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(169437500)
        phy.profile_inputs.channel_spacing_hz.value = 50000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy

    def PHY_wMbus_ModeN2g_19p2k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus N, index 4/13, frameB (19.2k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeNg
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB
        phy.profile_inputs.base_frequency_hz.value = long(169437500)
        phy.profile_inputs.channel_spacing_hz.value = 50000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeR_4p8k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus R2 (4.8k, Manchester)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeR_4p8k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(868330000)
        phy.profile_inputs.channel_spacing_hz.value = 60000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.Manchester
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeF_2p4k_frameA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus F, frameA (2.4k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeF_2p4k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameA
        phy.profile_inputs.base_frequency_hz.value = long(433820000)
        phy.profile_inputs.channel_spacing_hz.value = 50000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy
    #
    #
    #
    def PHY_wMbus_ModeF_2p4k_frameB(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Mbus, readable_name='WMbus F, frameB (2.4k)', phy_name=phy_name)

        phy.profile_inputs.mbus_mode.value = model.vars.mbus_mode.var_enum.ModeF_2p4k
        phy.profile_inputs.mbus_frame_format.value = model.vars.mbus_frame_format.var_enum.FrameB
        phy.profile_inputs.base_frequency_hz.value = long(433820000)
        phy.profile_inputs.channel_spacing_hz.value = 50000
        phy.profile_inputs.mbus_symbol_encoding.value = model.vars.mbus_symbol_encoding.var_enum.NRZ
        phy.profile_inputs.syncword_dualsync.value = False

        return phy