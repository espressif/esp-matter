from pycalcmodel.core.output import ModelOutputType
from pyradioconfig.calculator_model_framework.Utils.FileUtilities import FileUtilities
from pyradioconfig.calculator_model_framework.CalcManager import CalcManager





#
# Returns a list of overrides for a given model
#
def get_overrides(model):

    overrides = []
    for output in model.profile.get_outputs([ModelOutputType.SVD_REG_FIELD, ModelOutputType.SVD_REG_FIELD]):
            if output.override is not None:
                overrides.append(output.var_name)
    
    return overrides
                

#
# This runs the calculator for all phys (just to get the
# profile inputs and outputs built, and returns a 
# dictionary of lists of all overrides in any phys
#    
def get_overrides_each_phy(phy_names):                

    phy_overrides = dict()

    for phy_name in phy_names:
        # Run the calculator for the given phy
        # Hack to force part family and revision
        part_family = "dumbo"
        part_rev = "A0"
        radio_configurator = CalcManager(part_family, part_rev)
        #model = radio_configurator.calculate_phy(phy_name)
        model = radio_configurator.create_modem_model_instance_and_load_phy(phy_name)
        
        phy_overrides[phy_name] = get_overrides(model)

    # Now we have a dictionary of lists.  One key per phy,
    # and each element has a list of overrides for that phy
    
    return phy_overrides
    
#    
# This looks at a dictionary of all overrides in each phy, 
# and returns a list of all overrides in any phy
# 
def get_all_overrides(phy_overrides):

    override_list = []
    
    for phy_name in phy_overrides.keys():
        overrides = phy_overrides[phy_name]
        
        for override in overrides:
            if override not in override_list:
                override_list.append(override)
    
    return override_list
    
    
#
# Create a dictionary with keys for each override field, and each
# key will be a list of phys that override that field
#    
def make_dict_of_phys_per_override(phy_overrides):    
    
    override_dict = dict()

    for phy_name in phy_overrides.keys():           # Loop over all phys
        overrides = phy_overrides[phy_name]         
        for override in overrides:                  # Loop over all overrides in each phy
            if override not in override_dict.keys():
                override_dict[override] = list()    
            override_dict[override].append(phy_name)

    return override_dict

    
#
# This prints a table where each row has a field name, and for each
# field, it lists the phys that force that field
#    
def print_overrides():
    # Hack to force part family and revision
    part_family = "dumbo"
    part_rev = "A0"
    radio_configurator = CalcManager(part_family, part_rev)
    types_model = radio_configurator.create_modem_model_type()
    sim_tests_phy_groups = radio_configurator.get_sim_tests_phy_groups()
    # Filter out the sim tests phys
    filtered_phys = radio_configurator.filter_out_phy_group_names(types_model, sim_tests_phy_groups)
    phy_names = [x.name for x in filtered_phys]

    ignored_phys = [
         'PHY_Bluetooth_LE',
         'PHY_Bluetooth_LE_Narrow_CHBW',
         'PHY_Bluetooth_LE_Narrow_CHBW_2458',
         'PHY_Bluetooth_LE_Test',
         'PHY_Bluetooth_LE_Tx17tap',
         'PHY_IEEE802154_2p4GHz',
         'PHY_IEEE802154_2p4GHz_2415',
         'PHY_IEEE802154_2p4GHz_Narrow_CHBW',
         'PHY_IEEE802154_780MHz_OQPSK',
         'PHY_IEEE802154_868MHz_BPSK',
         'PHY_IEEE802154_915MHz_OQPSK',
         'PHY_WMbus_Ng',
         'PHY_WMbus_T_100k',
         'PHY_CTUNE_ONLY',
         'PHY_Test_BPSK'
    ]
    
    # Remove the phy's we don't care about
    for ignored in ignored_phys:
        phy_names.remove(ignored)
    
    phy_overrides = get_overrides_each_phy(phy_names)

    override_dict = make_dict_of_phys_per_override(phy_overrides)
    
    fields = override_dict.keys()
    fields.sort()
    
    for field in fields:
        print(field.ljust(40),)
        for phy in override_dict[field]:
            print(phy, )
        print(" ")
    
