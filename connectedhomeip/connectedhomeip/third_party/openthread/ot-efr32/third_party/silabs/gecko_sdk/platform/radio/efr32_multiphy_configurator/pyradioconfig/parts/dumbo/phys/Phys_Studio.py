from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

from py_2_and_3_compatibility import *

class PHYS_Studio(IPhy):

    def PHY_Sigfox_868MHz_DBPSK_100bps_Studio(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Sigfox_TX, readable_name='Sigfox 868MHz DBPSK 100bps', phy_description="Transmit only configuration for Sigfox DBPSK modulation.", phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value =  long(868000000)
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.bitrate.value = 100
        phy.profile_inputs.preamble_length.value = 16

        return phy

    def PHY_Sigfox_915MHz_DBPSK_600bps_Studio(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Sigfox_TX, readable_name='Sigfox 915MHz DBPSK 600bps', phy_description="Transmit only configuration for Sigfox DBPSK modulation.",phy_name=phy_name)
        phy.profile_inputs.base_frequency_hz.value =  long(915000000)
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.bitrate.value = 600
        phy.profile_inputs.preamble_length.value = 16

        return phy