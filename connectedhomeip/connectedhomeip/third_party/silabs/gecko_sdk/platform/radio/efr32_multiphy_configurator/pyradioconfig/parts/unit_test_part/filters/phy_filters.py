from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter

#
# Implements iphy_filter.py
#
class PhyFilters(IPhyFilter):

    customer_phy_groups = ['Phys_Essence', 'Phys_internal']
    sim_tests_phy_groups = ['Phys_test']
    simplicity_studio_phy_groups = ['Phys_internal']

