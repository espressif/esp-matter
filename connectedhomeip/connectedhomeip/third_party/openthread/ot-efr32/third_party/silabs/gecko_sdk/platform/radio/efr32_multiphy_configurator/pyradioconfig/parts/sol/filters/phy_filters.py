from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters_Sol(IPhyFilter):

    # Studio black list (files removed before Studio distribution)
    customer_phy_groups = ['Phys_Internal_Base_Customer_Chamberlain',
                           'Phys_Internal_Base_Customer_Landis_Gyr',
                           'Phys_Internal_Base_Customer_Lutron',
                           'Phys_Internal_Base_Customer_Sigfox',
                           'Phys_Internal_Base_Experimental',
                           'Phys_Internal_Base_Standard_SUNFSK',
                           'Phys_Internal_Base_Utility',
                           'Phys_Internal_Base_ValOnly',
                           'Phys_Internal_Connect',
                           'Phys_Internal_SUN_OQPSK',
                           'Phys_Internal_WiSUN_FSK',
                           'Phys_Internal_WiSUN_OFDM',
                           'Phys_RAIL_Base_Standard_BLE',
                           'Phys_RAIL_Base_Standard_IEEE802154',
                           'Phys_RAIL_Base_Standard_IEEE802154GB',
                           'Phys_RAIL_Base_Standard_ZWave',
                        ]
    # Studio white list (these PHYs show in Studio as proprietary starting points)
    simplicity_studio_phy_groups = ['Phys_Studio_Base',
                                    'Phys_Studio_Base_Standard_SUNFSK',
                                    'Phys_Studio_Connect',
                                    'Phys_Studio_SUN_OQPSK',
                                    'phys_studio_wisun_fan_1_0',
                                    'phys_studio_wisun_han',
                                    'Phys_Studio_WiSUN_OFDM',
                                    ]

    # Special designation for simulation PHYs
    sim_tests_phy_groups = []

    # Special designation for non-functional PHYs
    non_functional_phy_groups = []
