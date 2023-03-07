import os.path

#
# Write all variables in a model with the svd_mapping
# field set.
#
# fixme:  Change this to write_model_to_rm and use the same routine in field_to_regs and in load_phy_to_dut
#                                
def write_model_to_dut(dut, model):
    from py_dut_control_rf.configurators.radio_configurator_wrapper import RadioConfigurator as dut_control_radio_config
    dut_control_radio_config.downloadModelInstance(dut, model)

#
# Used externally
#    
def load_phy_to_dut(dut, phy_name, input_dir):
    import os.path

    model_input_path = input_dir + "/" + phy_name + ".xml"
    if os.path.exists(model_input_path):
        from py_dut_control_rf.configurators.radio_configurator_wrapper import \
            RadioConfigurator as dut_control_radio_config
        dut_control_radio_config.downloadInstance(dut, model_input_path)
    else:
        raise Exception("Input file not found at: " + input_dir)

#
# This is a reasonable "top level" routine for the calculator.  Call it with a phy
# name and it runs the calculator and writes it to the dut.
#
def run_calc_and_write_to_dut(dut, phy_name):
    # Hack to force part family and revision
    part_family = dut.info.hw.CHIPREVHW_FAMILY_NAME  # "dumbo"
    part_rev = "ANY"
    from pyradioconfig.calculator_model_framework.CalcManager import CalcManager
    radio_configurator = CalcManager(part_family, part_rev)
    model_instance = radio_configurator.calculate_phy(phy_name)

    # Now write the registers back to the dut
    write_model_to_dut(dut, model_instance)

    return model_instance
    


