import re
import traceback

from pylib_multi_phy_model.multi_phy_configuration_model import overrideType
from pylib_multi_phy_model.register_diff_tool.model_diff_codes import ModelDiffCodes
from pyradioconfig import CalcManager
from pycalcmodel.core.output import ModelOutputType
from pyradioconfig.calculator_model_framework.Utils.CalcStatus import CalcStatus
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

try:
  from host_py_rm_studio_internal import RM_Factory
except ImportError:
  from host_py_rm_pdb_internal import RM_Factory

from py_2_and_3_compatibility import *

from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from rail_scripts.rail_adapter_multi_phy import RAIL_ConcPhy
from rail_scripts.rail_adapter_multi_phy import RAIL_OptArgInput

class ModelDiff(object):
    @staticmethod
    def process_diffs(multi_phy_model, skip_running_radio_config_on_channel=False):
        """
        More information on the diff be found at:
        https://confluence.silabs.com/pages/viewpage.action?spaceKey=RCS&title=Register+Diff+Tool

        More Information on the data model can be found at:
        https://confluence.silabs.com/display/RCS/EFR+Multi-PHY+Configurator+Data+Models


        :param multi_phy_model: Fully populated multi phy data model
        :return:  Processed multi phy data model
        """
        try:
            # Verify uniqueness of protocol and channel names
            if not ModelDiff._verify_protocol_channel_naming_uniqueness(multi_phy_model):
                raise Exception('Protocol and channel names are not unique')

            # Loop through each base channel configuration
            for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
                base_channel_configuration.phy_config_delta_subtract = dict()  # Init dictionary
                #LogMgr.Debug("Processing: {}".format(base_channel_configuration.name))
                channel_config_entry_counter = 0
                # Loop through each channel config entry
                for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                    channel_config_entry_registers = dict()  # Init dictionary

                    # Create default register model for the part_family
                    register_model = RM_Factory(multi_phy_model.part_family.upper())()
                    # channel_config_entry.full_register_model = register_model

                    # Run radio configurator for this channel config entry
                    if not skip_running_radio_config_on_channel:
                        radio_config_model = ModelDiff._run_radio_configurator(multi_phy_model.part_family, multi_phy_model.part_revision, base_channel_configuration, channel_config_entry, multi_phy_model.target)
                    else:
                        radio_config_model = channel_config_entry.radio_configurator_output_model

                    if radio_config_model.result_code != CalcStatus.Success.value:
                        channel_config_entry.radio_configurator_output_model = radio_config_model
                        raise CalculationException('channel_config_entry {} : {}'.format(channel_config_entry.name, radio_config_model.error_message))

                    # clean some fat off the radio config model
                    for var in radio_config_model.vars:
                        var._access_read.clear()
                        var._access_write.clear()

                    channel_config_entry.radio_configurator_output_model = radio_config_model

                    # Loop through radio configurator profile outputs that match
                    # "register type" (i.e. SVD_REG_FIELD or SEQ_REG_FIELD)
                    for profile_output in radio_config_model.profile.get_outputs([ModelOutputType.SVD_REG_FIELD,
                                                                                  ModelOutputType.SEQ_REG_FIELD]):
                        reg_var = radio_config_model.vars.get_var(profile_output.var_name)
                        field_name = reg_var.svd_mapping
                        if profile_output.var_value is not None:
                            if profile_output._var.value_do_not_care != True:
                                # Load profile output value into register model
                                try:
                                    field = register_model.getObjectByName(field_name)
                                    field.io = profile_output.var_value
                                except BaseException as e:
                                    error_message = "Error Executing field write: {}".format(e)
                                    LogMgr.Error(error_message)
                                    if hasattr(e, 'message'):
                                        e.message = error_message + '\r\n' + e.message
                                    else:
                                        e.message = error_message
                                    raise e
                        # Get register object
                        register_name = ModelDiff._getRegNameFromFieldName(field_name)
                        register = register_model.getObjectByName(register_name)

                        # Save register for later diff use
                        channel_config_entry_registers[register_name] = register

                    if channel_config_entry_counter == 0:
                        # if this is the first entry in collection (e.g. this is the first iteration) then use this as the base configuration set of registers
                        base_channel_configuration.phy_config_base = channel_config_entry_registers
                        channel_config_entry.phy_config_delta_add = dict()
                    else:
                        # find differences in [base_channel_config.phy_config_base] vs [channel_config_entry_registers]
                        add_registers, subtract_registers = ModelDiff._find_diff_and_reset_values_from_reference(base_channel_configuration.phy_config_base, channel_config_entry_registers)

                        # store [register] differences into [channel_config_entry.phy_config_delta_add]
                        channel_config_entry.phy_config_delta_add = add_registers

                        # merge [base_channel_config.phy_config_base] registers into [base_channel_config.phy_config_delta_subtract]
                        base_channel_configuration.phy_config_delta_subtract.update(subtract_registers)

                    channel_config_entry_counter += 1  # increment loop counter

                if base_channel_configuration.force_empty_phy_config_delta_subtract:
                    # When true, copy all missing values back into "add" dictionary.  And then empty out "subtract" dictionary.
                    # This is used as a speed optimization, at the cost of a larger "add" size.
                    # Remove registers from "baseline" that already exist in the "add" collection.
                    for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                        phy_config_delta_add = channel_config_entry.phy_config_delta_add
                        for key, subtract in base_channel_configuration.phy_config_delta_subtract.items():
                            if not key in phy_config_delta_add:
                                phy_config_delta_add[key] = subtract

                    phy_config_base = base_channel_configuration.phy_config_base
                    for key, subtract in base_channel_configuration.phy_config_delta_subtract.items():
                        if key in phy_config_base:
                            del phy_config_base[key]

                    base_channel_configuration.phy_config_delta_subtract.clear()

            # Now that we've gone through and calculated all the base channel configs, let's ee if we need to do optimize any base_channel_references
            base_channel_diffs = dict()
            ind_base_channel_diffs = dict()
            for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
                # if force_empty_phy_config_delta_subtract is True, and base_channel_reference is not an empty string.. then this name is the reference regsiter set used to optimize space across multiple base_channel_configs
                if base_channel_configuration.force_empty_phy_config_delta_subtract:
                    if base_channel_configuration.base_channel_reference:
                        if len(base_channel_configuration.base_channel_reference) > 0:
                            base_channel_reference_name = base_channel_configuration.base_channel_reference
                            base_channel_config_ref = ModelDiff._get_base_channel_config_by_name(multi_phy_model, base_channel_reference_name)

                            if base_channel_config_ref is None:
                                raise Exception("Unable to find base_channel_reference with name = {}".format(base_channel_reference_name))

                            if not base_channel_reference_name in base_channel_diffs:
                                base_channel_diffs[base_channel_reference_name] = list()
                                ind_base_channel_diffs[base_channel_reference_name] = dict()

                            ind_base_channel_diffs[base_channel_reference_name][base_channel_configuration.name] = ModelDiff._generate_collection_of_diff_register_names(base_channel_config_ref.phy_config_base, base_channel_configuration.phy_config_base)

                            # merge lists
                            base_channel_diffs[base_channel_reference_name] = list(set().union(base_channel_diffs[base_channel_reference_name], ind_base_channel_diffs[base_channel_reference_name][base_channel_configuration.name]))
                            #LogMgr.Debug("register {} diff merge: {}, original {}".format(base_channel_configuration.phy.name, len(ind_base_channel_diffs[base_channel_reference_name][base_channel_configuration.name]), len(base_channel_diffs[base_channel_reference_name])))

            for base_channel_reference_name, list_diff_reg_names in base_channel_diffs.items():
                for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
                    phy_config_base = base_channel_configuration.phy_config_base
                    phy_config_base_orig_len = len(phy_config_base)
                    if base_channel_configuration.name == base_channel_reference_name or base_channel_configuration.base_channel_reference == base_channel_reference_name:
                        conc_phy_opt_hop_found = False
                        if (len(base_channel_configuration.optional_arguments.argument) > 0):
                            # Let's see if this is a concurrent PHY
                            conc_phy_opt_hop_found = RAIL_ConcPhy.RAIL_IsConcPhyVt(base_channel_configuration.optional_arguments.argument)

                        if (conc_phy_opt_hop_found == True) and (base_channel_configuration.base_channel_reference is not None):
                            # if virtual concurrent PHY, only apply the diff compare to the base channel
                            #LogMgr.Debug("Found virtual PHY {}".format(base_channel_configuration.phy.name))
                            # First, make sure MODEM.VITERBIDEMOD is on the diffs
                            if not RAIL_OptArgInput.viterbidemod in ind_base_channel_diffs[base_channel_reference_name][base_channel_configuration.name]:
                                ind_base_channel_diffs[base_channel_reference_name][base_channel_configuration.name].append(RAIL_OptArgInput.viterbidemod)
                            for list_diff_reg in ind_base_channel_diffs[base_channel_reference_name][base_channel_configuration.name]:
                                for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                                    phy_config_delta_add = channel_config_entry.phy_config_delta_add
                                    if not list_diff_reg in phy_config_delta_add:
                                        phy_config_delta_add[list_diff_reg] = phy_config_base[list_diff_reg]
                                del phy_config_base[list_diff_reg]
                        else:
                            # if non-(virtual concurrent) PHY, apply the diff compare to the list_diff_reg_names
                            for list_diff_reg in list_diff_reg_names:
                                for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                                    phy_config_delta_add = channel_config_entry.phy_config_delta_add
                                    if not list_diff_reg in phy_config_delta_add:
                                        phy_config_delta_add[list_diff_reg] = phy_config_base[list_diff_reg]
                                del phy_config_base[list_diff_reg]

                        #LogMgr.Debug("register base {} orig: {}, final: {}".format(base_channel_configuration.phy.name, phy_config_base_orig_len, len(phy_config_base)))
                        if base_channel_configuration.base_channel_reference == base_channel_reference_name:
                            # Clear out all the base registers, except the base reference list
                            phy_config_base.clear()

            if isinstance(multi_phy_model.status_code, str):
                multi_phy_model.status_code = int(multi_phy_model.status_code)
        except Exception as ex:
            # LogMgr.Error(ex)
            LogMgr.Error(traceback.print_exc())
            multi_phy_model.status_message = str(ex)
            multi_phy_model.status_code = ModelDiffCodes.UNKNOWN_ERROR.value

        return multi_phy_model

    @staticmethod
    def _run_radio_configurator(part_family, part_rev, base_channel_configuration, channel_config_entry, part_target=None):
        # Hack to pass in base_frequency_hz from channel config entry as an override
        channel_config_properties = dict()
        if channel_config_entry.base_frequency:
            channel_config_properties["base_frequency_hz"] = channel_config_entry.base_frequency

        channel_entry_input_overrides = channel_config_entry.profile_input_overrides
        channel_entry_output_overrides = channel_config_entry.profile_output_overrides

        phy_name_override = channel_config_entry.phy_name_override

        if phy_name_override is not None and len(phy_name_override) > 0:
            phy_name_override = ModelDiff._strip_studio_var_value_name_declarations(phy_name_override)
            radio_config_model = ModelDiff._run_radio_configurator_phy(part_family, part_rev, phy_name_override, None, channel_entry_input_overrides.override, channel_config_properties, None, channel_entry_output_overrides.override, part_target)

            ModelDiff._fill_missing_model_values_as_needed(channel_config_entry, radio_config_model)

            return radio_config_model

        phy_name = base_channel_configuration.phy.name

        if phy_name is not None and len(phy_name) > 0:
            phy_name = ModelDiff._strip_studio_var_value_name_declarations(phy_name)
            input_overrides = base_channel_configuration.phy.profile_input_overrides
            output_overrides = base_channel_configuration.profile_output_overrides
            radio_config_model = ModelDiff._run_radio_configurator_phy(part_family, part_rev, phy_name, input_overrides.override, channel_entry_input_overrides.override, channel_config_properties, output_overrides.override, channel_entry_output_overrides.override, part_target)

            ModelDiff._fill_missing_model_values_as_needed(channel_config_entry, radio_config_model)

            return radio_config_model

        profile = base_channel_configuration.profile

        if profile is not None and len(profile) > 0:
            profile = ModelDiff._strip_studio_var_value_name_declarations(profile)
            profile_inputs = base_channel_configuration.profile_inputs
            output_overrides = base_channel_configuration.profile_output_overrides
            radio_config_model = ModelDiff._run_radio_configurator_profile(part_family, part_rev, profile, profile_inputs.input, channel_entry_input_overrides.override, channel_config_properties, output_overrides.override, channel_entry_output_overrides.override, part_target)

            ModelDiff._fill_missing_model_values_as_needed(channel_config_entry, radio_config_model)

            return radio_config_model

        raise Exception("No profile or phy submitted for channel config entry: {}.".format(channel_config_entry.name))

    @staticmethod
    def _run_radio_configurator_profile(part_family, part_rev, profile_name, inputs, channel_entry_input_overrides, channel_config_properties, output_overrides, channel_entry_output_overrides, part_target):
        inputs = ModelDiff._convert_xml_inputs_to_dict(inputs)
        channel_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(channel_entry_input_overrides)

        # merge override dictionaries
        inputs.update(channel_config_properties)  # merge channel config properties to be passed in as overrides
        inputs.update(channel_overrides_dict)

        if output_overrides is not None:
            output_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(output_overrides)
        else:
            output_overrides_dict = dict()
        channel_output_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(channel_entry_output_overrides)
        inputs.update(output_overrides_dict)
        inputs.update(channel_output_overrides_dict)

        radio_configurator = CalcManager(part_family, part_rev, part_target)
        radio_configurator._CalcManager__developer_mode = False
        radio_config_model = radio_configurator.calc_config_profile(profile_name, optional_inputs=inputs)
        return radio_config_model

    @staticmethod
    def _run_radio_configurator_phy(part_family, part_rev, phy_name, input_overrides, channel_entry_input_overrides, channel_config_properties, output_overrides, channel_entry_output_overrides, part_target):
        if input_overrides is not None:
            input_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(input_overrides)
        else:
            input_overrides_dict = dict()

        channel_input_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(channel_entry_input_overrides)

        # merge override dictionaries
        input_overrides_dict.update(channel_config_properties) # merge channel config properties to be passed in as overrides
        input_overrides_dict.update(channel_input_overrides_dict)

        if output_overrides is not None:
            output_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(output_overrides)
        else:
            output_overrides_dict = dict()
        channel_output_overrides_dict = ModelDiff._convert_xml_inputs_to_dict(channel_entry_output_overrides)
        input_overrides_dict.update(output_overrides_dict)
        input_overrides_dict.update(channel_output_overrides_dict)

        radio_configurator = CalcManager(part_family, part_rev, part_target)
        radio_configurator._CalcManager__developer_mode = False
        radio_config_model = radio_configurator.calculate_phy(phy_name, input_overrides_dict)

        return radio_config_model

    @staticmethod
    def _convert_xml_inputs_to_dict(xml_input_list):
        input_dict = dict()
        if xml_input_list:
            for input in xml_input_list:
                # key = input.key.lower()
                key = input.key
                if '.' in key:  # check for [category].[variable name]
                    # remove category
                    key = key[key.index(".")+1:]

                input_value = input.value
                if isinstance(input_value, basestring):
                    if ':' in input_value:
                        input_value = input_value.split(':')[-1]

                    if input_value != 'null':  # don't allow NULL inputs to pass through
                        if input_value.lower() == 'false' or input_value.lower() == 'no' or input_value.lower() == 'off':  # Hack: Fix Java 'false' to Python False conversion
                            input_dict[key] = False
                        else:
                            input_dict[key] = input_value
                else:
                    input_dict[key] = input_value

        return input_dict

    @staticmethod
    def _getRegNameFromFieldName(field_name):
        (block, reg, field) = field_name.split('.')
        return block + '.' + reg

    @staticmethod
    def _find_diff_and_reset_values_from_reference(common_register_set, register_set_to_diff):
        add_registers = dict()
        diff_keys = [k for k in register_set_to_diff if common_register_set[k].io != register_set_to_diff[k].io]
        for key in diff_keys:
            add_registers[key] = register_set_to_diff[key]

        subtract_registers = dict()
        for key in diff_keys:
            subtract_registers[key] = common_register_set[key]

        return add_registers, subtract_registers

    @staticmethod
    def _get_base_channel_config_by_name(multi_phy_model, base_channel_config_name):
        for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
            if base_channel_configuration.name == base_channel_config_name:
                return base_channel_configuration

        return None

    @staticmethod
    def _generate_collection_of_diff_register_names(base_channel_config_ref, base_channel_configuration):
        list_diff_reg_names = list()

        for base_reg_name, base_reg in base_channel_config_ref.items():
            if base_reg_name in base_channel_configuration:
                if base_reg.io != base_channel_configuration[base_reg_name].io:
                    list_diff_reg_names.append(base_reg_name)

        return list_diff_reg_names

    @staticmethod
    def _strip_studio_var_value_name_declarations(var_value):
        if var_value.startswith('serializableObject:'):
            var_value = var_value.split(':')[-1]
        return var_value

    @staticmethod
    def _fill_missing_model_values_as_needed(channel_config_entry, radio_config_model):
        if channel_config_entry.base_frequency is None:
            channel_config_entry.base_frequency = radio_config_model.vars.base_frequency_hz.value
        if channel_config_entry.channel_spacing is None:
            channel_config_entry.channel_spacing = radio_config_model.vars.channel_spacing_hz.value

    @staticmethod
    def _verify_protocol_channel_naming_uniqueness(multi_phy_model):
        base_channel_configuration_names = []
        for base_channel_configuration in multi_phy_model.base_channel_configurations.base_channel_configuration:
            base_channel_configuration.name = ModelDiff._fix_protocol_channel_naming(base_channel_configuration.name)
            if base_channel_configuration.name not in base_channel_configuration_names:
                base_channel_configuration_names.append(base_channel_configuration.name)
            else:
                LogMgr.Error('base_channel_configuration name: {} is duplicated'.format(base_channel_configuration.name))
                return False

            channel_config_entries = []
            for channel_config_entry in base_channel_configuration.channel_config_entries.channel_config_entry:
                channel_config_entry.name = ModelDiff._fix_protocol_channel_naming(channel_config_entry.name)
                if channel_config_entry.name not in channel_config_entries:
                    channel_config_entries.append(channel_config_entry.name)
                else:
                    LogMgr.Error('channel_config_entry name: {} is duplicated'.format(channel_config_entry.name))
                    return False
        return True

    @staticmethod
    def _fix_protocol_channel_naming(nameStr):
        # Make names compatible with ANSI C variable naming conventions (e.g. no spaces, cant start with a number)
        if nameStr is not None:
            # nameStr = nameStr.replace(" ", "_")
            nameStr = re.sub("[^0-9a-zA-Z_]", "_", nameStr)
            if nameStr[0].isdigit():
                nameStr = "_" + nameStr
        return nameStr

