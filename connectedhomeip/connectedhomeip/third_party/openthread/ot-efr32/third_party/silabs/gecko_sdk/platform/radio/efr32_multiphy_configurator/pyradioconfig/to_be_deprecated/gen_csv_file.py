from pyradioconfig.calculator_model_framework.CalcManager import CalcManager
from pyradioconfig.calculator_model_framework.model_serializers.human_readable import Human_Readable
from pyradioconfig.calculator_model_framework.model_serializers.static_timestamp_xml import Static_TimeStamp_XML


import os.path
import shutil




    
#
# 
#
def create_csv_file(output_filename=None):
    # Hack to force part family and revision
    part_family = "dumbo"
    part_rev = "A0"
    radio_configurator = CalcManager(part_family, part_rev)
    phy_names = radio_configurator.getPhyNames()

    output_lines = list()
    
    # Generate header line
    # Create an empty model starting from the base profile
    model = radio_configurator.create_modem_model_instance(profile_name="Base")
    output_line = "phy_name"
    for var in model.vars:
        output_line = output_line + ", " + var.name
    output_lines.append(output_line)
    
    # Now print the data for each phy
    for phy_name in phy_names:
        print("Generating line in csv file for: %s" % phy_name)

        # Run the calculator to populate model
        model = radio_configurator.calculate_phy(phy_name)
        
        # Generate row of data
        output_line = phy_name
        for var in model.vars:
            output_line = output_line + ", %s" % var.value
        output_lines.append(output_line)
    

    # Write the lines to a file if one was specified.
    # Otherwise, we'll just return the list to the calling function
    # to do what it wants with the list.
    if output_filename is not None:
        outputfile = open(output_filename, 'w')
        for line in output_lines:
            outputfile.write('%s\n' % line)
        outputfile.close()
    else:
        return output_lines
