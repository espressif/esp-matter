from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr


class ImportISCFiles(object):

    keys_ignored = ['phy', 'profile', 'configurator_version']
    phy = None
    profile = None

    def parse_file(self, file_path):

        found_start = False
        found_end = False

        inputs = dict()

        # Loop through file line by line
        with open(file_path, "r") as f:
            for line in f:
                line = line.strip()
                if len(line) > 0:
                    if line == '{setupId:efr32RadioConfig':
                        # found start of input section
                        found_start = True
                    elif line == '}' and found_start:
                        # found end of input section
                        found_end = True
                        break
                    elif line.startswith('#') and found_start:
                        # ignore commented out line
                        LogMgr.Debug("Ignoring line: " + line)
                    elif line.startswith('RAIL_API') and found_start:
                        # ignore RAIL_API=enum:Efr32RailApi:API_V_1_X
                        # This was added for RAIL 1 vs RAIL 2--we need to ignore it
                        LogMgr.Debug("Ignoring line: " + line)
                    else:
                        # We found start and are in input section
                        if found_start and not found_end:
                            if '=' in line:
                                pair = line.split("=")
                                key = pair[0].lower()
                                value_pair = pair[1]
                                value_pair = value_pair.replace('serializableObject:', '')
                                if ':' in value_pair:
                                    value_pair = value_pair.split(':')
                                    value_type = value_pair[0]
                                    value = value_pair[-1] # get last value in array

                                    if value_type == 'bool':
                                        if str.lower(value) == 'false':
                                            value = False
                                        elif str.lower(value) == 'true':
                                            value = True

                                    # Check if we need to ignore input
                                    if key not in self.keys_ignored:
                                        # Add to key/value dictionary
                                        inputs[key] = value
                                        #print("phy.profile_inputs." + key + ".value = " + str(value))
                                    else:
                                        # Known invalid inputs are caught here
                                        if hasattr(self, key):
                                            # Store to member variable
                                            LogMgr.Info("self." + key + " = " + value)
                                            setattr(self, key, value)
                                else:
                                    LogMgr.Error("Invalid value " + value_pair)
                            else:
                                LogMgr.Error("Invalid line: " + line)
        # Return key/value dictionary
        return inputs


    def export_modem_model_to_isc_section(self, model, isc_filename=None):
        output_string = ''
        output_lines = list()
        output_lines.append('{setupId:efr32RadioConfig')
        output_lines.append('CONFIGURATOR_VERSION=string:{}'.format(model.calc_version))
        output_lines.append('PROFILE=serializableObject:Efr32RadioConfiguratorProfile:{}'.format(model.profile.name))

        phy = model.phy

        profile_inputs = list() # collect and then sort them later
        if phy is not None:
            output_lines.append('PHY=serializableObject:Efr32RadioConfiguratorPhy:{0}:{1}'.format(model.profile.name, str.replace(model.phy.name, "PHY_", "")))

            for profile_input in model.phy.profile_inputs:
                var_type = profile_input._var._get_type_str()
                value = profile_input.value
                # if value is None:
                #    value = profile_input.default

                if value is not None:
                    if var_type == 'bool':
                        value = str(value).lower()
                    elif var_type == 'enum':
                        var_type = 'serializableObject:EnumDataItem'

                    profile_inputs.append('{0}={1}:{2}'.format(str.upper(profile_input.var_name), var_type, value))
        else:
            output_lines.append('PHY=serializableObject:Efr32RadioConfiguratorPhy:null:Custom settings')

            for profile_input in model.profile.inputs:
                var_type = profile_input._var._get_type_str()
                value = profile_input.var_value
                if value is None:
                    value = profile_input.default

                if value is not None:
                    if var_type == 'bool':
                        value = str(value).lower()
                    elif var_type == 'enum':
                        var_type = 'serializableObject:EnumDataItem'

                    profile_inputs.append('{0}={1}:{2}'.format(str.upper(profile_input.var_name), var_type, value))

        for profile_input in sorted(profile_inputs):
            output_lines.append(profile_input)

        output_lines.append('}')

        if isc_filename is not None:
            outputfile = open(isc_filename, 'w')
            for line in output_lines:
                output_string += '%s\n' % line
                outputfile.write('%s\n' % line)
            outputfile.close()
        else:
            for line in output_lines:
                output_string += '%s\n' % line

        #print(output_string)
        return output_string
