from pylib_multi_phy_model.isc_import.MultiPHYStudioPropFileImport import MultiPHYStudioPropFileImport
from pylib_multi_phy_model.multi_phy_configuration_model import MultiPHYConfig
from pylib_multi_phy_model.multi_phy_configuration_model.rail_scripts_wrapper import RAILScriptsWrapper
from pylib_multi_phy_model.register_diff_tool.model_diff import ModelDiff
import os

class MultiPHYIscImport(object):
    """
    Runs Multi-PHYISC files through calculator and rail_scripts to generate a C/H, XML, and CFG file
    """

    # ------------------------------------------------------------------------
    def __init__(self):
        pass

    def run(self, isc_filename, output_directory):
        xml_string = self.parse_file(isc_filename)
        # print xml_string

        mphy_config = MultiPHYConfig()
        multi_phy_model = mphy_config.load_xml_string(xml_string)

        if multi_phy_model is None:
            raise Exception('Error parsing XML from ISC file.')

        multi_phy_model = MultiPHYStudioPropFileImport.hack_to_account_for_capitalization_crap_from_studio(multi_phy_model)

        ModelDiff.process_diffs(multi_phy_model)
        RAILScriptsWrapper.run_rail_scripts(multi_phy_model)

        # Renaming the files for uniqueness
        file_name = os.path.splitext(os.path.basename(isc_filename))[0]
        for file in multi_phy_model.output_files.file:
            if file.name.endswith('.h'):
                file.name = 'rail_config_{}.h'.format(file_name)
            elif file.name.endswith('.c'):
                file.name = 'rail_config_{}.c'.format(file_name)

            # check if output for each file is not a null string
            if len(file.source_code) == 0:
                raise Exception('Error RAIL source file {}.'.format(file.name))

        RAILScriptsWrapper.dump_output_files(multi_phy_model, output_directory)

        return multi_phy_model


    def parse_file(self, file_path):

        found_start = False
        found_end = False

        str_list = []

        # Loop through file line by line until start 'setupId:multiPhyRadioConfigurator' is found
        with open(file_path, "r") as f:
            for line in f:
                line_strip = line.strip()
                if len(line_strip) > 0:
                    if line_strip == '{setupId:multiPhyRadioConfigurator':
                        # found start of input section
                        found_start = True
                    elif line_strip == '}' and found_start:
                        # found end of input section
                        found_end = True
                        break
                    else:
                        # We found start and are in input section
                        if found_start and not found_end:
                            str_list.append(line)

        if not str_list:
            raise Exception('XML string not found!')

        xml_str = ''.join(str_list)

        # Return XML string
        return xml_str