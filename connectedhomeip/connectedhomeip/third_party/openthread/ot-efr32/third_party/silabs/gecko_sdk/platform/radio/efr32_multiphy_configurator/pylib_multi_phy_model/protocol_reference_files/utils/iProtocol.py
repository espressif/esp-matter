import abc
from pylib_multi_phy_model.multi_phy_configuration_model import *
from pylib_multi_phy_model.register_diff_tool.model_diff_codes import ModelDiffCodes

class IProtocol(object):
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def get_model(self):
        """
        Child class must implement this function and return a basic multi PHY model, with the minimim data needed
        """
        pass
