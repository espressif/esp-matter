"""
Lynx specific filters
"""
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters(IPhyFilter):

    #Studio black list (files removed before Studio distribution)
    customer_phy_groups = [
                            'Phys_Internal_Base_Customer_Aclara',
                            'Phys_Internal_Base_Customer_Acuity',
                            'Phys_Internal_Base_Customer_Chamberlain',
                            'Phys_Internal_Base_Customer_Essence',
                            'Phys_Internal_Base_Customer_HoneywellEnergyAxis',
                            'Phys_Internal_Base_Customer_Landis_Gyr',
                            'Phys_Internal_Base_Customer_Lutron',
                            'Phys_Internal_Base_Customer_Sigfox',
                            'Phys_Internal_Base_Experimental',
                            'Phys_Internal_Base_Utility',
                            'Phys_Internal_Base_ValOnly',
                            'Phys_Internal_Connect',
                            'Phys_Internal_Longrange',
                            'Phys_Internal_RAIL_Base_Standard_BLE',
                            'Phys_Internal_RAIL_Base_Standard_IEEE802154',
                            'Phys_RAIL_Base_Standard_BLE',
                            'Phys_RAIL_Base_Standard_IEEE802154',
                            'Phys_RAIL_Base_Standard_IEEE802154GB',
                            'Phys_RAIL_Base_Standard_ZWave',
                        ]

    #Studio white list (these PHYs show in Studio as proprietary starting points)
    simplicity_studio_phy_groups = ['Phys_Studio_Base', 'Phys_Studio_Base_Standard_SUNFSK', 'Phys_Studio_Connect',
                                    'Phys_Studio_LongRange', 'Phys_Studio_MBus','phys_studio_wisun_fan_1_0', 'phys_studio_wisun_han']

    # Special designation for simulation PHYs
    sim_tests_phy_groups = []

    # Special designation for non-functional PHYs
    non_functional_phy_groups = []