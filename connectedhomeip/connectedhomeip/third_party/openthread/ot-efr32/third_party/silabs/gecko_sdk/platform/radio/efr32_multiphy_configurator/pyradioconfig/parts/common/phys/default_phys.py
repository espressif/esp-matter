"""
This class is where the default phys's are defined
"""
from pyradioconfig.calculator_model_framework.interfaces.idefault_phy import IDefaultPhy
from pycalcmodel.core.default_phy import ModelDefaultPhy


class DefaultPhys(IDefaultPhy):
    def build(self, model):
        """
        Assigns default PHYs.  There must be one default phy defined here for every profile defined
        """
        model.profiles.Base.default_phys.append(ModelDefaultPhy(model.phys.PHY_Datasheet_868M_2GFSK_500Kbps_125K))
        model.profiles.Connect.default_phys.append(ModelDefaultPhy(model.phys.PHY_Connect_902MHz_2GFSK_200kbps))
        model.profiles.Sigfox_TX.default_phys.append(ModelDefaultPhy(model.phys.PHY_Sigfox_868MHz_DBPSK_100bps_Studio))
        model.profiles.Mbus.default_phys.append(ModelDefaultPhy(model.phys.PHY_wMbus_ModeS_32p768k_frameA))