from pyradioconfig.parts.common.phys.default_phys import DefaultPhys, ModelDefaultPhy

class DefaultPhys_ocelot(DefaultPhys):

    def build(self, model):
        """
        Assigns default PHYs.  There must be one default phy defined here for every profile defined
        """
        model.profiles.Base.default_phys.append(ModelDefaultPhy(model.phys.PHY_Studio_868M_2GFSK_38p4Kbps_20K))
        model.profiles.Connect.default_phys.append(ModelDefaultPhy(model.phys.PHY_Studio_Connect_920MHz_2GFSK_100kbps))
        model.profiles.Mbus.default_phys.append(ModelDefaultPhy(model.phys.PHY_wMbus_ModeS_32p768k_frameA))