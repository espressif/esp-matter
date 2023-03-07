import copy
import tempfile

import os

import pylib_multi_phy_model

import sys

from pylib_multi_phy_model.multi_phy_configuration_model import Bindings
from pylib_multi_phy_model.multi_phy_configuration_model.Bindings import *
from pylib_multi_phy_model.register_diff_tool.model_diff_codes import ModelDiffCodes

if sys.version_info[0] < 3:
    from StringIO import StringIO
else:
    from io import StringIO
    basestring = str

from pylib_multi_phy_model.register_model_to_xml.rm_to_xml import RMtoXML
from pylib_multi_phy_model.multi_phy_configuration_model._version import __xsd_version__
from py_2_and_3_compatibility import *

__all__ = ['MultiPHYConfig']


class MultiPHYConfig(object):
    """
    THis is a wrapper around multi_phy_configuration data model
    """

    _xsd_version = __xsd_version__

    def __init__(self, model=None, xml_file=None):
        self._model = model

        if xml_file is not None:
            self.load_xml(xml_file)

    @staticmethod
    def xsd_version():
        return MultiPHYConfig._xsd_version

    @property
    def model(self):
        return self._model

    @model.setter
    def model(self, model):
        self._model = model

    def validate(self):
        # TODO: Add Jython compatible validation here
        return True

    def to_xml(self, filename):
        """
        Save XML model to file
        :param filename:
        :param model:
        :return:
        """
        with open(filename, 'w') as outfile:
            return self._write_to_stream(outfile)

    def to_xml_string(self):
        """
        Save XML model to file, and reads back as string (limitation of generateDS())
        :param model:
        :return:
        """
        xml_string = None
        outfile = StringIO()  # this is an in memory file
        try:
            retur_val = self._write_to_stream(outfile, pretty_print=False)
            if retur_val is not None:
                xml_string = outfile.getvalue()
        finally:
            outfile.close()
        return xml_string

    def _write_to_stream(self, outstream, pretty_print=True):
        if self.validate():
            outstream.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
            processed_model = self._preprocess_model(self._model)
            processed_model.export(outstream, 0, pretty_print=pretty_print)
            return 0
        else:
            sys.stderr.write("ERROR: Type data model is invalid. Unable to create XML.")
            return -1


    def load_xml(self, filename):
        self._model = Bindings.parse(inFileName=filename)

        if MultiPHYConfig.xsd_version() != self._model.xsd_version:
            print("WARNING: Parsing XML with XSD version {} in MultiPHYConfig {}".format(MultiPHYConfig.xsd_version(),
                                                                                      self._model.xsd_version))
        return self._model

    def load_xml_string(self, xml_string):
        try:
            self._model = Bindings.parse(xml_string=xml_string)
        except ValueError as ve:
            self._model = Bindings.parse(xml_string=xml_string.encode('utf-8'))

        if MultiPHYConfig.xsd_version() != self._model.xsd_version:
            print("WARNING: Parsing XML with XSD version {} in MultiPHYConfig {}".format(MultiPHYConfig.xsd_version(),
                                                                                      self._model.xsd_version))

        # Hack for channel power, which cna have strings and integers
        for base_channel_configuration in self._model.base_channel_configurations.base_channel_configuration:
            for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                # check if is numeric, isnumeric() doesn't handle negative values
                try:
                    channel_config_entry.max_power = int(channel_config_entry.max_power)
                except ValueError:
                    pass

        return self._model


    def _preprocess_model(self, model):
        # clear out some non-compatible object types before serialization
        processed_model = copy.deepcopy(model)

        # Loop through each base channel configuration
        for base_channel_configuration in processed_model.base_channel_configurations.base_channel_configuration:
            # process phy_config_base
            if not isinstance(base_channel_configuration.phy_config_base, phy_config_baseType):
                phy_config_base_dict = base_channel_configuration.phy_config_base
                phy_config_base = phy_config_baseType()
                for register_name, regsiter_obj in phy_config_base_dict.items():
                    phy_config_base.add_register(self._conver_register_obj_to_xml(regsiter_obj))
                base_channel_configuration.phy_config_base = phy_config_base

            # process phy_config_delta_subtract
            if not isinstance(base_channel_configuration.phy_config_delta_subtract, phy_config_delta_subtractType):
                phy_config_delta_subtract_dict = base_channel_configuration.phy_config_delta_subtract
                phy_config_delta_subtract = phy_config_delta_subtractType()
                for register_name, regsiter_obj in phy_config_delta_subtract_dict.items():
                    phy_config_delta_subtract.add_register(self._conver_register_obj_to_xml(regsiter_obj))
                base_channel_configuration.phy_config_delta_subtract = phy_config_delta_subtract

            # Loop through each channel config entry
            for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                if not isinstance(channel_config_entry.phy_config_delta_add, phy_config_delta_addType):
                    # process phy_config_delta_add
                    phy_config_delta_add_dict = channel_config_entry.phy_config_delta_add
                    phy_config_delta_add = phy_config_delta_addType()
                    for register_name, regsiter_obj in phy_config_delta_add_dict.items():
                        phy_config_delta_add.add_register(self._conver_register_obj_to_xml(regsiter_obj))
                    channel_config_entry.phy_config_delta_add = phy_config_delta_add

                if (channel_config_entry.radio_configurator_output_model is not None) and not type(channel_config_entry.radio_configurator_output_model) in [str, basestring, unicode]:
                    # process radio_configurator_output_model
                    radio_configurator_output_model_obj = channel_config_entry.radio_configurator_output_model
                    channel_config_entry.radio_configurator_output_model = self.__radio_configurator_output_model_to_XML_str(radio_configurator_output_model_obj)

                if (channel_config_entry.full_register_model is not None) and not type(channel_config_entry.full_register_model) in [str, basestring, unicode]:
                    # process full_register_model
                    rm2xml = RMtoXML()
                    channel_config_entry.full_register_model = rm2xml.export_to_string(channel_config_entry.full_register_model)

        return processed_model


    def _conver_register_obj_to_xml(self, regsiter_obj):
        return registerType(name=regsiter_obj.name,
                            value=regsiter_obj.io,
                            baseAddress=regsiter_obj.baseAddress,
                            addressOffset=regsiter_obj.addressOffset,
                            fullname=regsiter_obj.fullname,
                            access=regsiter_obj.access,
                            description=regsiter_obj.description,
                            resetValue=regsiter_obj.resetValue,
                            resetMask=regsiter_obj.resetMask)


    def __radio_configurator_output_model_to_XML_str(self, model):
        """Converts model instance to XML instance file

        Args:
            model (ModelRoot) : Data model instance
            output_path (str) : Path to which XML instance is written to
            phy_name (str) : PHY name to create insance of (Optional, default = None)
            profile_name (str) : Profile name to create insance of (Optional, default = None)
        """
        phy = None
        try:
            phy = model.phy
        except StopIteration:
            pass

        if phy is not None:
            profile = getattr(model.profiles, phy.profile_name)
        else:
            profile = model.profile

        if hasattr(model, 'processed'):
            processed = model.processed
        else:
            processed = True

        if hasattr(model, 'result_code'):
            result_code = model.result_code
        else:
            result_code = 0

        if hasattr(model, 'error_message'):
            error_message = model.error_message
        else:
            error_message = ''

        return model.to_instance_xml_str(model.part_revision, '', processed, result_code, error_message, profile, phy)

    @staticmethod
    def wrap_single_phy_model_with_multi_phy(single_phy_model):
        multi_phy_model = multi_phy_configuration(part_family=single_phy_model.part_family.lower(),
                                              part_revision=single_phy_model.part_revision,
                                              rail_adapter_version="rail_api_2.x",
                                              xsd_version=MultiPHYConfig.xsd_version(),
                                              status_code=ModelDiffCodes.OK.value,
                                              target=single_phy_model.target)

        base_channel_configuration = base_channel_configurationType()
        base_channel_configuration.name = "UNKNOWN"
        multi_phy_model.base_channel_configurations.add_base_channel_configuration(base_channel_configuration)

        channel_config_entry = channel_config_entryType(name="channel_0",
                                                        base_frequency=single_phy_model.vars.base_frequency_hz.value,
                                                        channel_spacing=single_phy_model.vars.channel_spacing_hz.value,
                                                        physical_channel_offset=0,
                                                        channel_number_start=0,
                                                        channel_number_end=0,
                                                        max_power="RAIL_TX_POWER_MAX")

        channel_config_entry.radio_configurator_output_model = single_phy_model

        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        from pylib_multi_phy_model.register_diff_tool.model_diff import ModelDiff
        ModelDiff.process_diffs(multi_phy_model, skip_running_radio_config_on_channel=True)

        return multi_phy_model
