from pyradioconfig.calculator_model_framework.Utils.FileUtilities import FileUtilities
from pyradioconfig.calculator_model_framework.CalcManager import CalcManager
from pyradioconfig.calculator_model_framework.model_serializers.human_readable import Human_Readable

from dut_init import *

from print_profile import *
from extract import *

import os.path
import shutil

#
# Extract all fields from efrtest_setup.c
#
def extract_modem_config(dut, phy_name):

    # Make sure the firmware runs basic init code to set analog registers before reading them
    dut.mcu.ResetNoHalt()

    # Get the list of phy types that were defined in the firmware
    dut.rf.rpc.RFDUT_ModemConfig(getattr(dut.rf.RPC_TYPES.PHY_Select_t, phy_name))

    # Skip protocols that didn't update the RX baud rate denominator.  They're not valid.
    rxbrden = dut.rm.MODEM.RXBR.RXBRDEN.io
    if (rxbrden == 0):
        print("Skipping uninitialized phy: %s" % phy_name)
        return None
    else:
        print("Extracting configuration fields: %s" % phy_name)


        # There is one phy that has a different clock frequency.  Just special case it here.
        if (phy_name == 'Phy_PHY_OOK_9p6kbps'):
            xtal_frequency = 40000000
        else:
            xtal_frequency = 38400000

        model_extracted = extract_model_from_rm(dut.rm, xtal_frequency)
        return model_extracted


#
# Extract all phy's in efrtest_setup.c to .cfg files
#
def extract_all():

    dut = dut_init()


    current_path = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(current_path, 'extracted')
    FileUtilities.cleandir(output_dir)

    # Read all registers for all configurations.
    phy_names_from_s029 = sorted(dut.rf.RPC_TYPES.PHY_Select_t.__members__.keys())
    for phy_name in phy_names_from_s029:
        model_extracted = extract_modem_config(dut, phy_name)
        if model_extracted is not None:
            # Output it to a file
            Human_Readable.print_modem_model_values_v2(output_dir + "/" + phy_name + ".cfg", phy_name, model_extracted)

    print("Finished!")


#
# Print all extracted fields to valid Python source code
#
def print_python_source(phy_name, model, source_lines):

    source_lines.append(' ')
    source_lines.append(' ')
    source_lines.append('    def %s(self, model):' % phy_name)
    source_lines.append('        phy = self._makePhy(model, model.profiles.Base, \'%s\')' % phy_name)
    source_lines.append(' ')
    source_lines.append('        self.sim_test_case_base(phy, model)')

    # There is one phy that has a different clock frequency.  Just special case it here.
    if (model.profile.inputs.xtal_frequency_hz.var_value != 38400000):
        source_lines.append('        phy.profile_inputs.xtal_frequency.value = %r' % model.profile.inputs.xtal_frequency.var_value)

    for output in model.profile.get_outputs([ModelOutputType.SVD_REG_FIELD, ModelOutputType.SEQ_REG_FIELD]):
        (block, reg, field) = output.var.svd_mapping.split('.')
        if block == 'CRC':
            continue
        if block == 'FRC':
            continue

        if output.override is not None:
            source_lines.append('        phy.profile_outputs.%s_%s_%s.override = %r' % (block, reg, field, output.override))
        elif output.var_value is not None:
            source_lines.append('        phy.profile_outputs.%s_%s_%s.override = %r' % (block, reg, field, output.var_value))   # Leave on two lines to simplify changing the text if needed
            pass

#
# Extract all phy's in efrtest_setup.c to Python source code
#
def extract_to_source():

    dut = dut_init()

    source_lines = []

    source_lines.append("from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy")
    source_lines.append("from pycalcmodel.core.input import ModelInput")
    source_lines.append("from pycalcmodel.core.output import ModelOutput")
    source_lines.append("from pyradioconfig.parts.dumbo.phys.phy_common import *")
    source_lines.append(" ")
    source_lines.append("class PHYS_Sim_Tests(IPhy):")
    source_lines.append(" ")
    source_lines.append("    def sim_test_case_base(self, phy, model):")

    # For all of these phy's, we'll use a fixed frame format configuration
    source_lines.append('        ')
    source_lines.append('        # Put frame config function call here...')
    source_lines.append('        ')
    source_lines.append('        phy.profile_inputs.xtal_frequency.value = 38400000')
    source_lines.append("        ")
    source_lines.append("        ")

    # Read all registers for all configurations.
    # Hack to force part family and revision
    part_family = "jumbo"
    part_rev = "A0"
    radio_configurator = CalcManager(part_family, part_rev)
    phy_names_from_calculator = radio_configurator.getPhyNames()
    phy_names_from_s029 = sorted(dut.rf.RPC_TYPES.PHY_Select_t.__members__.keys())
    for phy_name in phy_names_from_s029:

        # Skip the phy's that are already in source code
        if phy_name in phy_names_from_calculator:
            print("Skipping %s.  Phy already in calculator" % phy_name)
            continue

        model_extracted = extract_modem_config(dut, phy_name)
        if model_extracted is not None:
            print_python_source(phy_name, model_extracted, source_lines)

    # Write the lines to a file if one was specified.
    # Otherwise, we'll just return the list to the calling function
    # to do what it wants with the list.
    current_path = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(current_path, '../../parts/jumbo/phys')
    output_dir = 'C:/validation/host_py_radio_config/Package/pyradioconfig/parts/jumbo/phys'
    output_filename = output_dir + "/Phys_sim_tests.py"

    if output_filename is not None:
        outputfile = open(output_filename, 'w')
        for line in source_lines:
            outputfile.write('%s\n' % line)
        outputfile.close()
    else:
        return source_lines

    print("Finished!")



def get_phys_in_group(group_name):
    phys = list()
    radio_configurator = CalcManager("jumbo", "")
    types_model = radio_configurator.create_modem_model_type()
    for phy in types_model.phys:
        #print("{0} {1}".format(phy.group_name, phy.name))
        if phy.group_name == group_name:
            phys.append(phy.name)
            #print("%s in selected group" % phy.name)
    return sorted(phys)

#
# Extract all phy's in efrtest_setup.c to Python source code
#
def phys_to_python_source():

    source_lines = []

    source_lines.append("from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy")
    source_lines.append("from pycalcmodel.core.input import ModelInput")
    source_lines.append("from pycalcmodel.core.output import ModelOutput")
    source_lines.append("from pyradioconfig.parts.dumbo.phys.phy_common import *")
    source_lines.append(" ")
    source_lines.append("class PHYS_Sim_Tests(IPhy):")
    source_lines.append(" ")
    source_lines.append("    def sim_test_case_base(self, phy, model):")

    # For all of these phy's, we'll use a fixed frame format configuration
    source_lines.append('        ')
    source_lines.append('        # Put frame config function call here...')
    source_lines.append('        ')
    source_lines.append('        phy.profile_inputs.xtal_frequency_hz.value = 38400000')
    source_lines.append("        ")
    source_lines.append("        ")

    # Read all registers for all configurations.
    phys_from_sim_tests = get_phys_in_group('Phys_sim_tests')

    for phy_name in phys_from_sim_tests:
        print("Adding phy: %s" % phy_name)
        # Hack to force part family and revision
        part_family = "jumbo"
        part_rev = "A0"
        radio_configurator = CalcManager(part_family, part_rev)
        model_instance = radio_configurator.calculate_phy(phy_name)
        print_python_source(phy_name, model_instance, source_lines)

    # Write the lines to a file if one was specified.
    # Otherwise, we'll just return the list to the calling function
    # to do what it wants with the list.
    current_path = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(current_path, '../../parts/jumbo/phys')
    output_dir = 'C:/workspaces'
    output_filename = output_dir + "/Phys_sim_tests.py"

    if output_filename is not None:
        outputfile = open(output_filename, 'w')
        for line in source_lines:
            outputfile.write('%s\n' % line)
        outputfile.close()
    else:
        return source_lines

    print("Finished!")


