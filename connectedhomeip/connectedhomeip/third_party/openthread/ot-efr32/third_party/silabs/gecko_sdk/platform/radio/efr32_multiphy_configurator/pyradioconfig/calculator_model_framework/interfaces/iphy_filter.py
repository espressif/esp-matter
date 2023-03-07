from abc import abstractmethod
from enum import IntEnum

class PhyFilterGroupTypes(IntEnum):
    customer_phys = 1
    sim_tests_phys = 2
    simplicity_studio_phys = 3
    non_functional_phys = 4


class IPhyFilter(object):

    customer_phy_groups = []
    sim_tests_phy_groups = []
    simplicity_studio_phy_groups = []
    non_functional_phy_groups = []

    def __init__(self):
        pass

    @abstractmethod
    def get_phy_filter_groups(self, filter_type):
        if filter_type == PhyFilterGroupTypes.customer_phys:
            return self.customer_phy_groups
        elif filter_type == PhyFilterGroupTypes.sim_tests_phys:
            return self.sim_tests_phy_groups
        elif filter_type == PhyFilterGroupTypes.simplicity_studio_phys:
            return self.simplicity_studio_phy_groups
        elif filter_type == PhyFilterGroupTypes.non_functional_phys:
            return self.non_functional_phy_groups
        else:
            raise Exception("Unknown phy filter group type: " + str(filter_type))

