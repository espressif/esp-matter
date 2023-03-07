from pyradioconfig.parts.dumbo.phys.Phys_Studio import PHYS_Studio
from pyradioconfig.parts.jumbo.phys.Phys_OOK import PHYS_OOK

class PHYS_Studio_Jumbo(PHYS_Studio):

    # Publish the OOK reference PHYs in Studio
    def PHY_Studio_433M_OOK_4p8kbps(self, model):
        phys_ook_object = PHYS_OOK()
        phys_ook_object.PHY_Reference_433M_OOK_4p8kbps(model)

    def PHY_Studio_433M_OOK_10kbps(self, model):
        phys_ook_object = PHYS_OOK()
        phys_ook_object.PHY_Reference_433M_OOK_10kbps(model)

    def PHY_Studio_915M_OOK_4p8kbps(self, model):
        phys_ook_object = PHYS_OOK()
        phys_ook_object.PHY_Reference_915M_OOK_4p8kbps(model)

    def PHY_Studio_915M_OOK_10kbps(self, model):
        phys_ook_object = PHYS_OOK()
        phys_ook_object.PHY_Reference_915M_OOK_10kbps(model)
