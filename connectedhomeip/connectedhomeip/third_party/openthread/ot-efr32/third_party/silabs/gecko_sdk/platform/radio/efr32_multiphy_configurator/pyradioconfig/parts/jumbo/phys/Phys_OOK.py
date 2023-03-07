from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_INTERNAL

from py_2_and_3_compatibility import *

class PHYS_OOK(IPhy):

    def OOK_Base(self, phy, model):
        # Add values to existing inputs
        phy.profile_inputs.base_frequency_hz.value =  long(915000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 1000
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.deviation.value = 0
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OOK
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 40
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.NONE
        phy.profile_inputs.shaping_filter_param.value = 1.5
        phy.profile_inputs.syncword_0.value = long(0xf68d)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.errors_in_timing_window.value = 0

        PHY_COMMON_FRAME_INTERNAL(phy, model)

    # def PHY_Internal_915M_OOK_100kbps(self, model):
    #     phy = self._makePhy(model, model.profiles.Base, '915M OOK 100kbps')
    #
    #     self.OOK_Base(phy, model)
    #
    #     # Add values to existing inputs
    #     phy.profile_inputs.bitrate.value = 100000
    #     phy.profile_inputs.bandwidth_hz.value = 600000

    def PHY_Datasheet_915M_OOK_4p8kbps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M OOK 4.8kbps', phy_name=phy_name)

        self.OOK_Base(phy, model)

        # Add values to existing inputs
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.bandwidth_hz.value = 306000

        # Other overrides needed to maintain validated PHY despite OOK calculator changes
        phy.profile_inputs.symbols_in_timing_window.value = 16
        phy.profile_inputs.timing_resync_period.value = 2
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_FRAME_DETECT
        phy.profile_inputs.ook_slicer_level.value = 2
        phy.profile_inputs.errors_in_timing_window.value = 1

        model.vars.dynamic_slicer_enabled.value_forced = False

        phy.profile_outputs.MODEM_CF_DEC0.override = 0
        phy.profile_outputs.MODEM_CF_DEC1.override = 10
        phy.profile_outputs.MODEM_CF_DEC2.override = 47
        phy.profile_outputs.MODEM_SRCCHF_SRCRATIO1.override = 128
        phy.profile_outputs.MODEM_SRCCHF_SRCRATIO2.override = 862
        phy.profile_outputs.MODEM_SRCCHF_SRCENABLE1.override = 0
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 4
        phy.profile_outputs.MODEM_TIMING_OFFSUBNUM.override = 3
        phy.profile_outputs.MODEM_TIMING_OFFSUBDEN.override = 2
        phy.profile_outputs.MODEM_RXBR_RXBRNUM.override = 0
        phy.profile_outputs.MODEM_CTRL5_RESYNCBAUDTRANS.override = 0
        phy.profile_outputs.AGC_CTRL0_MODE.override = 1
        phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.override = 5

    def PHY_Datasheet_433M_OOK_4p8kbps(self,model, phy_name=None):
        phy=self._makePhy(model,model.profiles.Base,readable_name='434M OOK 4.8kbps', phy_name=phy_name)

        self.OOK_Base(phy,model)

        #Addvaluestoexistinginputs
        phy.profile_inputs.base_frequency_hz.value= long(433000000)
        phy.profile_inputs.bitrate.value=4800
        phy.profile_inputs.bandwidth_hz.value=306000

        # Other overrides needed to maintain validated PHY despite OOK calculator changes
        phy.profile_inputs.symbols_in_timing_window.value = 16
        phy.profile_inputs.timing_resync_period.value = 2
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_FRAME_DETECT
        phy.profile_inputs.ook_slicer_level.value = 2
        phy.profile_inputs.errors_in_timing_window.value = 1

        model.vars.dynamic_slicer_enabled.value_forced = False

        phy.profile_outputs.MODEM_CF_DEC0.override = 0
        phy.profile_outputs.MODEM_CF_DEC1.override = 10
        phy.profile_outputs.MODEM_CF_DEC2.override = 47
        phy.profile_outputs.MODEM_SRCCHF_SRCRATIO1.override = 128
        phy.profile_outputs.MODEM_SRCCHF_SRCRATIO2.override = 862
        phy.profile_outputs.MODEM_SRCCHF_SRCENABLE1.override = 0
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 4
        phy.profile_outputs.MODEM_TIMING_OFFSUBNUM.override = 3
        phy.profile_outputs.MODEM_TIMING_OFFSUBDEN.override = 2
        phy.profile_outputs.MODEM_RXBR_RXBRNUM.override = 0
        phy.profile_outputs.MODEM_CTRL5_RESYNCBAUDTRANS.override = 0
        phy.profile_outputs.AGC_CTRL0_MODE.override = 1
        phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.override = 5

    ### Customer OOK reference PHYs, use dynamic slicing ###

    def PHY_Reference_433M_OOK_4p8kbps(self,model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='433M OOK 4.8kbps', phy_name=phy_name)
        self.OOK_Base(phy, model)
        phy.profile_inputs.base_frequency_hz.value = long(433000000)
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.bandwidth_hz.value = 360000

        return phy

    def PHY_Reference_433M_OOK_10kbps(self,model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='433M OOK 10kbps', phy_name=phy_name)
        self.OOK_Base(phy, model)
        phy.profile_inputs.base_frequency_hz.value = long(433000000)
        phy.profile_inputs.bitrate.value = 10000
        phy.profile_inputs.bandwidth_hz.value = 750000

        return phy

    def PHY_Reference_915M_OOK_4p8kbps(self,model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M OOK 4.8kbps', phy_name=phy_name)
        self.OOK_Base(phy, model)
        phy.profile_inputs.base_frequency_hz.value = long(915000000)
        phy.profile_inputs.bitrate.value = 4800
        phy.profile_inputs.bandwidth_hz.value = 360000

        return phy

    def PHY_Reference_915M_OOK_10kbps(self,model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M OOK 10kbps', phy_name=phy_name)
        self.OOK_Base(phy, model)
        phy.profile_inputs.base_frequency_hz.value = long(915000000)
        phy.profile_inputs.bitrate.value = 10000
        phy.profile_inputs.bandwidth_hz.value = 750000

        return phy