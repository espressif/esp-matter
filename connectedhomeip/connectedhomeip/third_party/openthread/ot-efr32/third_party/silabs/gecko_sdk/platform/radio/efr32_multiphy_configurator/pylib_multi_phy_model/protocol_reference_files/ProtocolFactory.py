import importlib
import inspect
import os

from pylib_multi_phy_model.multi_phy_configuration_model import MultiPHYConfig, overrideType
from pylib_multi_phy_model.multi_phy_configuration_model.rail_scripts_wrapper import RAILScriptsWrapper
from pylib_multi_phy_model.protocol_reference_files.utils.iProtocol import IProtocol
from pylib_multi_phy_model.register_diff_tool.model_diff import ModelDiff


class ProtocolFactory(object):

    @staticmethod
    def create_multi_phy_model(part_family, protocol_name):
        part_path = ProtocolFactory._get_part_family_path(part_family)

        multi_phy_model = None

        for file in os.listdir(part_path):
            if os.path.isfile(os.path.join(part_path, file)):
                file_name, file_extention = os.path.splitext(file)

                if protocol_name == file_name:
                    if file_extention.lower() == ".xml":
                        # Load model from XML
                        mphy_config_import = MultiPHYConfig()
                        multi_phy_model = mphy_config_import.load_xml(os.path.join(part_path, file))
                        break
                    elif file_extention.lower() == ".py" or file_extention.lower() == ".pyc":
                        multi_phy_model = ProtocolFactory._get_multi_phy_model_from_src(part_family, file_name)
                        if multi_phy_model is not None:
                            break
                    elif file_extention.lower() == ".class":  # Jython file
                        multi_phy_model = ProtocolFactory._get_multi_phy_model_from_src(part_family, file_name)
                        if multi_phy_model is not None:
                            break

        return multi_phy_model

    @staticmethod
    def _get_part_family_path(part_family):
        part_family = part_family.lower()
        part_path = os.path.join(os.path.dirname(__file__), "parts", part_family)

        if not os.path.exists(part_path):
            raise Exception("Protocol path for part {} does not exist!".format(part_family))

        return part_path

    @staticmethod
    def _get_multi_phy_model_from_src(part_family, file_name):
        multi_phy_model = None
        # Load from python code
        part_family = part_family.lower()
        protocol_module = importlib.import_module('pylib_multi_phy_model.protocol_reference_files.parts.{0}.{1}'.format(part_family, file_name))
        for cls in dir(protocol_module):
            cls = getattr(protocol_module, cls)
            if (inspect.isclass(cls)  # Make sure it is a class
                and inspect.getmodule(cls) == protocol_module):  # Make sure it was defined in module, not just imported
                if issubclass(cls, IProtocol):  # Make sure it is a IProtocol class
                    protocol = cls()
                    multi_phy_model = protocol.get_model()
                    break

        return multi_phy_model


    @staticmethod
    def process_protocol(part_family, protocol_name, output_filename=None, override_channel_config_name=None, overrides=None):
        multi_phy_model = ProtocolFactory.create_multi_phy_model(part_family, protocol_name)

        if multi_phy_model is None:
            raise Exception('Unable to find {} protocol: {} multi-phy reference file'.format(part_family, protocol_name))

        # Handle overrides
        if override_channel_config_name is not None and len(override_channel_config_name) > 0:
            if overrides is None:
                overrides = dict()

            # Find channel under test
            channel_config_entry_found = False
            for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
                for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                    if channel_config_entry.name == override_channel_config_name:
                        for key, value in overrides.items():
                            input_override = overrideType(key, value)
                            channel_config_entry.profile_input_overrides.add_override(input_override)

                        channel_config_entry_found = True
                        break
                if channel_config_entry_found:
                    break

            if not channel_config_entry_found:
                raise Exception("Unable to find channel config named: {}".format(override_channel_config_name))


        ModelDiff.process_diffs(multi_phy_model)
        RAILScriptsWrapper.run_rail_scripts(multi_phy_model)

        if output_filename is not None:
            mphy_config = MultiPHYConfig(model=multi_phy_model)
            mphy_config.to_xml(output_filename)

        return multi_phy_model
