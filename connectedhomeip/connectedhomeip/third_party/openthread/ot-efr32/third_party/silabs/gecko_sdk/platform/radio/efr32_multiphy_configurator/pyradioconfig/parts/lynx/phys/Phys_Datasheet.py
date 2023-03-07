from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.panther.phys.PHY_internal_base import Phy_Internal_Base
from py_2_and_3_compatibility import *

class PHYS_Datasheet_Lynx(IPhy):
    # duplicated from Package/pyradioconfig/parts/jumbo/phys/Phys_Datasheet.py
    # did not inherit as we only want 2.4GHz PHYs

    def PHY_Datasheet_2450M_2GFSK_1Mbps_500K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 1Mbps 500K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_2400M_base(phy, model)

        phy.profile_inputs.bitrate.value = 1000000
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.timing_sample_threshold.value = 12
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1000KHz

    def PHY_Datasheet_2450M_2GFSK_250Kbps_125K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 250Kbps 125K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_2400M_base(phy, model)

        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.deviation.value = 125000
        phy.profile_inputs.timing_sample_threshold.value = 12
        phy.profile_inputs.bandwidth_hz.value = 350000
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1000KHz
        phy.profile_inputs.if_frequency_hz.value = 1370000

    def PHY_Datasheet_2450M_2GFSK_2Mbps_1M(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 2Mbps 1M', phy_name=phy_name)

        Phy_Internal_Base.GFSK_2400M_base(phy, model)
        phy.profile_inputs.base_frequency_hz.value = long(2400000000)

        phy.profile_inputs.bitrate.value = 2000000
        phy.profile_inputs.deviation.value = 1000000
        phy.profile_inputs.timing_sample_threshold.value = 12
        phy.profile_inputs.agc_settling_delay.value = 39
        phy.profile_inputs.bandwidth_hz.value = 2400000
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1500KHz



