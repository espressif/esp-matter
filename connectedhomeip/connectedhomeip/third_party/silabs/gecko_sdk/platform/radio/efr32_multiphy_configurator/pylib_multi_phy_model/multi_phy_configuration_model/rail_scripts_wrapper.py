import os

import yaml

from pylib_multi_phy_model.multi_phy_configuration_model import fileType
from rail_scripts.generators.railConfig_sourceCodeGenerator import RAILConfig_generator
from rail_scripts.generators.railTest_rmrCommandGenerator import RailTest_rmrConfigGenerator
from rail_scripts.rail_adapter import RAILAdapter

class RAILScriptsWrapper(object):
    rail_signature_function = None

    @staticmethod
    def run_rail_scripts(multi_phy_model, generate_debug_yaml=False, output_filename="rail_config", internal=False, sign=False):
        railAdapter = RAILAdapter(mphyConfig=multi_phy_model, adapter_name=multi_phy_model.rail_adapter_version)
        railAdapter.populateModel()

        if railAdapter._railModelPopulated == False:
            return

        # Debug yaml file
        if generate_debug_yaml:
            railModelContext = railAdapter.generateRailModelContext()
            rail_model_out = yaml.dump(railModelContext)
            multi_phy_model.output_files.file.append(fileType("rail_model.yml", rail_model_out))

        # Setup the context for the rail config generator
        context = {'rail_internal': internal,
                   'enable_timing': internal,
                   'filename': output_filename}
        # Add commit information to internal builds assuming we're in a git
        # repo and have access to this.
        if internal:
            railAdapterPath = os.path.abspath(RAILAdapter.current_dir)
            context['ra_commit'] = os.popen("git -C {} rev-parse HEAD".format(os.path.join(railAdapterPath))).read()[0:10]
            context['rc_commit'] = os.popen("git -C {} rev-parse HEAD".format(os.path.join(railAdapterPath, "..", ".."))).read()[0:10]

        # If a signature is requested then use the rail_signature_function to do
        # that. We need to ensure that something has set this function because
        # it is kept external to this code intentionally.
        if sign:
            assert RAILScriptsWrapper.rail_signature_function != None, "No signature function available!"
            railModelContext = railAdapter.generateRailModelContext()
            signatures = RAILScriptsWrapper.rail_signature_function(railModelContext)
            railAdapter.setSignatures(signatures)

        # Create and override the generator
        generator = RAILConfig_generator(railAdapter)
        generator.overrideContext(**context)

        rail_config_h = generator.render(generator.template_path_h)
        multi_phy_model.output_files.file.append(fileType("{}.h".format(output_filename), rail_config_h))

        rail_config_c = generator.render(generator.template_path_c)
        multi_phy_model.output_files.file.append(fileType("{}.c".format(output_filename), rail_config_c))

        railtest_generator = RailTest_rmrConfigGenerator(railAdapter)
        rail_railtest_commands = railtest_generator.render(railtest_generator.template_path_railtest)
        multi_phy_model.output_files.file.append(fileType("rail_test_commands.txt", rail_railtest_commands))

    @staticmethod
    def dump_output_files(multi_phy_model, output_path):

        if not os.path.exists(output_path):
            os.makedirs(output_path, exist_ok=True)

        for file in multi_phy_model.output_files.file:
            file_path = os.path.join(output_path, file.name)

            if os.path.exists(file_path):
                os.remove(file_path)

            with open(file_path, 'w') as fc:
                print ("Creating '{}'...".format(file_path))
                fc.write(file.source_code)
                fc.close()
