from pyradioconfig.parts.jumbo.phys.Phys_IEEE802154_GB868 import PHYS_IEEE802154_GB868_Jumbo
from py_2_and_3_compatibility import *

class PHYS_IEEE802154_GB868_Nerio(PHYS_IEEE802154_GB868_Jumbo):
    # inherit from Jumbo phy

    def PHY_IEEE802154_915MHz_2GFSK_R23_NA(self, model):

        # refer to spec: \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\ZigBee\Zigbee Pro R23 Spec 0.5 PDF.pdf
        phy = self._makePhy(model, model.profiles.Base, 'IEEE 802.15.4 915MHz 2GFSK R23 North America')

        # inherit base from Nerio/Jumbo:
        self.IEEE802154_GB868_Base(phy, model)
        self.IEEE802154_GB868_154G_PHR(phy, model)


        ### new settings below as per Terry's email on 2019-05-15 20:47
        phy.profile_inputs.base_frequency_hz.value = long(903e6)
        phy.profile_inputs.white_seed.value = 0x1FF
        phy.profile_inputs.var_length_minlength.value = 5
        phy.profile_inputs.baudrate_tol_ppm.value = 300
        phy.profile_inputs.bitrate.value = 500000
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.deviation.value = 190000
        phy.profile_inputs.tx_xtal_error_ppm.value = 40
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.rx_xtal_error_ppm.value = 40


