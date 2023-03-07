from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.dumbo.phys.phy_internal_base import Phy_Internal_Base

from py_2_and_3_compatibility import *

class PHYS_Datasheet(IPhy):

    ###Unique PHYs###

    def PHY_Datasheet_2450M_2GFSK_2Mbps_1M(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 2Mbps 1M', phy_name=phy_name)

        Phy_Internal_Base.GFSK_2400M_base(phy, model)
        phy.profile_inputs.base_frequency_hz.value = long(2440000000)

        phy.profile_inputs.bitrate.value = 2000000
        phy.profile_inputs.deviation.value = 1000000
        phy.profile_inputs.agc_settling_delay.value = 39 #Allowing this as no DEC values have changed

    def PHY_Datasheet_915M_2GFSK_2Mbps_500K(self, model, phy_name=None):
        # Formerly PHY_Internal_915M_2GFSK_2Mbps_500K in Package/pyradioconfig/parts/dumbo/phys/Phys_Internal.py
        # Migrated to here during MCUW_RADIO_CFG-815 Purge stale internal PHYs
        # and restored as a datasheet PHY in
        # MCUW_RADIO_CFG-1017 restore Internal PHY as Datasheet PHY on Dumbo and Jumbo PHY_Internal_915M_2GFSK_2Mbps_500K
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M 2GFSK 2Mbps 500K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        phy.profile_inputs.bitrate.value = 2000000
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.agc_power_target.value = -8
        phy.profile_inputs.agc_settling_delay.value = 34 #Allowing this as no DEC values have changed

    def PHY_Datasheet_2450M_2GFSK_1Mbps_500K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 1Mbps 500K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_2400M_base(phy, model)

        phy.profile_inputs.bitrate.value = 1000000
        phy.profile_inputs.deviation.value = 500000
        phy.profile_inputs.agc_settling_delay.value = 39 #Allowing this as no DEC values have changed

    def PHY_Datasheet_915M_2GFSK_500Kbps_175K_mi0p7(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M 2GFSK 500Kbps 175K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        # Add values to existing inputs
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.deviation.value = 175000
        phy.profile_inputs.agc_settling_delay.value = 29 #Allowing this as no DEC values have changed

    def PHY_Datasheet_2450M_2GFSK_250Kbps_125K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='2450M 2GFSK 250Kbps 125K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_2400M_base(phy, model)

        phy.profile_inputs.bitrate.value = 250000
        phy.profile_inputs.deviation.value = 125000
        phy.profile_inputs.agc_settling_delay.value = 39 #Allowing this as no DEC values have changed

    def PHY_Datasheet_915M_2GFSK_50Kbps_25K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M 2GFSK 50Kbps 25K', phy_name=phy_name)

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        # Add values to existing inputs
        phy.profile_inputs.bitrate.value = 50000
        phy.profile_inputs.deviation.value = 25000
        phy.profile_inputs.agc_power_target.value = -8

    def PHY_Datasheet_490M_2GFSK_10Kbps_5K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='490MHz 2GFSK 10Kbps 5KHz', phy_name=phy_name)

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(490000000)
        phy.profile_inputs.bitrate.value = 10000
        phy.profile_inputs.deviation.value = 5000

    ###Base Functions###

    def Datasheet_2GFSK_500Kbps_125K_base(self, phy, model):

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.deviation.value = 125000

    def Datasheet_2GFSK_100Kbps_50K_base(self, phy, model):

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.deviation.value = 50000

    def Datasheet_2GFSK_38p4Kbps_20k_base(self, phy, model):

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        phy.profile_inputs.bitrate.value = 38400
        phy.profile_inputs.deviation.value = 20000

    def Datasheet_2GFSK_2p4Kbps_1p2K_base(self, phy, model):

        Phy_Internal_Base.GFSK_915M_base(phy, model)

        phy.profile_inputs.bitrate.value = 2400
        phy.profile_inputs.deviation.value = 1200

    ###Derivative PHYs###

    def PHY_Datasheet_868M_2GFSK_500Kbps_125K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='868M 2GFSK 500Kbps 125K', phy_name=phy_name)

        self.Datasheet_2GFSK_500Kbps_125K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(868000000)
        phy.profile_inputs.agc_settling_delay.value = 29 #Allowing this as no DEC values have changed

    def PHY_Datasheet_315M_2GFSK_500Kbps_125K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='315MHz 2GFSK 500Kbps 125KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_500Kbps_125K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(315000000)
        phy.profile_inputs.agc_settling_delay.value = 29 #Allowing this as no DEC values have changed

    def PHY_Datasheet_915M_2GFSK_100Kbps_50K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, phy_name=phy_name, readable_name='915M 2GFSK 100Kbps 50K')

        self.Datasheet_2GFSK_100Kbps_50K_base(phy, model)

        phy.profile_inputs.agc_power_target.value = -8
        phy.profile_inputs.agc_settling_delay.value = 34 #Allowing this as no DEC values have changed

    def PHY_Datasheet_490M_2GFSK_100Kbps_50K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='490MHz 2GFSK 100Kbps 50KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_100Kbps_50K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(490000000)

    def PHY_Datasheet_868M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='868M 2GFSK 38.4Kbps 20K', phy_name=phy_name)

        self.Datasheet_2GFSK_38p4Kbps_20k_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(868000000)

    def PHY_Datasheet_490M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='490MHz 2GFSK 38.4Kbps 20KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_38p4Kbps_20k_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(490000000)

    def PHY_Datasheet_315M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='315MHz 2GFSK 38.4Kbps 20KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_38p4Kbps_20k_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(315000000)

    def PHY_Datasheet_169M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='169MHz 2GFSK 38.4Kbps 20KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_38p4Kbps_20k_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(169000000)

    def PHY_Datasheet_868M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='868M 2GFSK 2.4Kbps 1.2K', phy_name=phy_name)

        self.Datasheet_2GFSK_2p4Kbps_1p2K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(868000000)
        phy.profile_inputs.if_frequency_hz.value = 400000
        phy.profile_inputs.agc_power_target.value = -8

    def PHY_Datasheet_490M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='490MHz 2GFSK 2.4Kbps 1.2KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_2p4Kbps_1p2K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(490000000)
        phy.profile_inputs.if_frequency_hz.value = 300000

    def PHY_Datasheet_315M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='315MHz 2GFSK 2.4Kbps 1.2KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_2p4Kbps_1p2K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(315000000)
        phy.profile_inputs.if_frequency_hz.value = 300000
        phy.profile_inputs.agc_settling_delay.value = 34  # Allowing this as no DEC values have changed

    def PHY_Datasheet_285M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='285MHz 2GFSK 2.4Kbps 1.2KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_2p4Kbps_1p2K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(285000000)
        phy.profile_inputs.if_frequency_hz.value = 300000
        phy.profile_inputs.agc_settling_delay.value = 34  # Allowing this as no DEC values have changed

    def PHY_Datasheet_169M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='169MHz 2GFSK 2.4Kbps 1.2KHz', phy_name=phy_name)

        self.Datasheet_2GFSK_2p4Kbps_1p2K_base(phy, model)

        phy.profile_inputs.base_frequency_hz.value = long(169000000)
        phy.profile_inputs.if_frequency_hz.value = 300000
        phy.profile_inputs.agc_settling_delay.value = 34  # Allowing this as no DEC values have changed