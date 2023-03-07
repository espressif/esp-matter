from pylib_multi_phy_model.multi_phy_configuration_model import MultiPHYConfig
from pylib_multi_phy_model.multi_phy_configuration_model.rail_scripts_wrapper import RAILScriptsWrapper
from pylib_multi_phy_model.register_diff_tool.model_diff import ModelDiff
import os

from pylib_multi_phy_model.register_diff_tool.model_diff_codes import ModelDiffCodes
from pyradioconfig.calculator_model_framework.model_serializers.human_readable import Human_Readable


class MultiPHYStudioPropFileImport(object):
    """
    Runs Multi-PHY radioconf files through calculator and rail_scripts to generate a C/H, XML, and CFG file
    """

    # ------------------------------------------------------------------------
    def __init__(self):
        pass

    def run(self, radioconf_filename, output_directory, output_filename=None, internal=False, sign=False):
        xml_string = self.parse_file(radioconf_filename)
        # print xml_string

        mphy_config = MultiPHYConfig()
        multi_phy_model = mphy_config.load_xml_string(xml_string)

        if multi_phy_model is None:
            raise Exception('Error parsing XML from ISC file.')

        multi_phy_model = MultiPHYStudioPropFileImport.hack_to_account_for_capitalization_crap_from_studio(multi_phy_model)

        # Create a unique output file name if one isn't given
        if not output_filename:
            output_filename = "rail_config_{}".format(os.path.splitext(os.path.basename(radioconf_filename))[0])

        ModelDiff.process_diffs(multi_phy_model)

        if int(multi_phy_model.status_code) != ModelDiffCodes.UNKNOWN_ERROR.value:
            RAILScriptsWrapper.run_rail_scripts(multi_phy_model, output_filename=output_filename, internal=internal, sign=sign)

            # Renaming the files for uniqueness
            for file in multi_phy_model.output_files.file:
                # check if output for each file is not a null string
                if len(file.source_code) == 0:
                    raise Exception('Error RAIL source file {}.'.format(file.name))

            RAILScriptsWrapper.dump_output_files(multi_phy_model, output_directory)

        return multi_phy_model

    def parse_file(self, file_path):
        with open(file_path, "r") as f:
            xml_str = f.read()
        return xml_str

    def dump_cfg_files(self, multi_phy_model, file_name_root, output_dir):
        for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
            for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                radio_config_model = channel_config_entry.radio_configurator_output_model
                cfg_output_path = os.path.join(output_dir, "{0}_{1}_{2}.cfg".format(file_name_root, base_channel_configuration.name, channel_config_entry.name))
                Human_Readable.print_modem_model_values_v2(cfg_output_path, channel_config_entry.name, radio_config_model)

    @staticmethod
    def hack_to_account_for_capitalization_crap_from_studio(multi_phy_model):
        # Hack to account for capitalization crap from Studio
        for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
            for profile_input in base_channel_configuration.profile_inputs.input:
                if profile_input.key:
                    profile_input.key = profile_input.key.lower()
            for phy_override in base_channel_configuration.phy.profile_input_overrides.override:
                if phy_override.key:
                    phy_override.key = phy_override.key.lower()
            for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                for phy_override in channel_config_entry.profile_input_overrides.override:
                    if phy_override.key:
                        phy_override.key = phy_override.key.lower()

        return multi_phy_model
