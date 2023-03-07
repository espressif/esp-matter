from pyradioconfig.parts.nixi.phys.Phys_connect import PHYS_connect_Nixi
from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy

class PHYS_connect_Lynx(IPhy):

    # importing only one PHY from Nixi
    # did not inherit as we only want 2.4GHz PHY

    def PHY_Connect_2_4GHz_OQPSK_2Mcps_250kbps(self, model):
        phy = PHYS_connect_Nixi().PHY_Connect_2_4GHz_OQPSK_2Mcps_250kbps(model)

