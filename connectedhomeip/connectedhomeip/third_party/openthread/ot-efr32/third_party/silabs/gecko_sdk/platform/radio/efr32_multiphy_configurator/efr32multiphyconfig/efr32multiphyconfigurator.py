import copy
from common import ChipConfiguratorInterface
from pylib_multi_phy_model.multi_phy_configuration_model import MultiPHYConfig
from pylib_multi_phy_model.multi_phy_configuration_model.rail_scripts_wrapper import RAILScriptsWrapper
from pylib_multi_phy_model.register_diff_tool.model_diff import ModelDiff
from pylib_multi_phy_model.register_diff_tool.model_diff_codes import ModelDiffCodes
from .efr32multiphycfginput import Efr32MultiPHYCfgInput
import os
from ._version import __version__


__all__ = ["Efr32MultiPHYConfigurator"]

class Efr32MultiPHYConfigurator(ChipConfiguratorInterface):
    '''
    Main interface to efr32-radio-configurator

    Example usages:
        Set input model and calculate:

        >>> cfg = Efr32MultiPHYConfigurator()
        >>> output = cfg.configure(multi_phy_config_model=multi_phy_config_model_string)
    '''
    
    def __init__(self):
        self.setup()
        setattr(self, "version", __version__)
        
    def setup(self):
        """
        Sets up the configurator
        
        Args:
        """
        super(Efr32MultiPHYConfigurator, self).__init__()
        self.inputs = Efr32MultiPHYCfgInput()
        self.configuration = {}
        self._sync_categories_and_options()

    def configure(self, **kwargs):
        """
        kwargs are all optional.  
        kwargs phy_name is used if you'd like to run a specific phy, instead of a profile
        kwargs optional_inputs is a dictionary of inputs used when running a phy only, as specified above.  
        
        Examples:
        
        >>>   multi_phy_config_model = multi_phy_config_model_string
        """
        # Optional kwargs
        multi_phy_config_model_string = None
        if 'multi_phy_config_model' in kwargs:
            multi_phy_config_model_string = kwargs['multi_phy_config_model']
        else:
            raise Exception("multi_phy_config_model not supplied!")

        if (multi_phy_config_model_string is None) or (multi_phy_config_model_string == ''):
            raise Exception("multi_phy_config_model is empty!")

        # Load the XML string into a Python model, run the register diff
        mphy_config = MultiPHYConfig()
        multi_phy_model = mphy_config.load_xml_string(multi_phy_config_model_string)
        calc_multi_phy_model = ModelDiff.process_diffs(multi_phy_model)

        if int(multi_phy_model.status_code) == ModelDiffCodes.OK.value:
            RAILScriptsWrapper.run_rail_scripts(multi_phy_model, generate_debug_yaml=False)

        # Serialize output model to XML string to return
        mphy_config.model = calc_multi_phy_model

        calc_multi_phy_model_xml = mphy_config.to_xml_string()

        # Save to file if needed
        if 'xml_filename' in kwargs:
            xml_filename = kwargs['xml_filename']
            with open(xml_filename, 'w') as outfile:
                outfile.write(calc_multi_phy_model_xml)

        self.configuration['multi_phy_config_model'] = calc_multi_phy_model_xml
        self.configuration['multi_phy_config_model_memory_object'] = calc_multi_phy_model

        return self.configuration

