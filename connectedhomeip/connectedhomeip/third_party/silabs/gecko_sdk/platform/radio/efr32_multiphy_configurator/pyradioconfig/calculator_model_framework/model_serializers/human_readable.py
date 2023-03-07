from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.output import ModelVariableEmptyValue
from pycalcmodel.core.output import ModelOutputType
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
import warnings
from math import log10, floor

from py_2_and_3_compatibility import *
import os
from pyradioconfig._version import __version__
import subprocess

class Human_Readable(object):

    git_branch = None
    git_commit = None

    #
    # Print a modem_model object to a humanly readable file.
    #
    @staticmethod
    def print_modem_model_values_v2(outputfilename, phy_name, modem_model, show_do_not_care=True, phy_guid=None):

        output_lines = list()

        # meta data here
        output_lines.append('metadata.version = {}'.format(__version__))
        Human_Readable._get_git_branch_and_commit()
        output_lines.append('metadata.git_branch = {}'.format(Human_Readable.git_branch))
        output_lines.append('metadata.git_commit = {}'.format(Human_Readable.git_commit))

        #Show the PHY name
        line = 'info.config_name = ' + str(phy_name)
        output_lines.append(line)

        if phy_guid is not None:
            line = 'info.phy_guid = ' + str(phy_guid)
            output_lines.append(line)

        #Show the part family and revision
        line = 'info.part_family = ' + str(modem_model.part_family)
        output_lines.append(line)
        line = 'info.part_revision = ' + str(modem_model.part_revision)
        output_lines.append(line)

        #Show the execution target
        line = 'info.target = ' + str(modem_model.target)
        output_lines.append(line)

        if hasattr(modem_model, 'phy') and modem_model.phy is not None:
            if not hasattr(modem_model.phy, "locked"):
                modem_model.phy.locked = False
            phy_locked = modem_model.phy.locked
            line = 'info.phy.locked = ' + str(phy_locked)
            output_lines.append(line)

        #If this is a PROD PHY, then also print the PHY this points to
        phy_obj = getattr(modem_model,'phy',None)  #When processing ISC files, phy may be None
        if phy_obj is not None:
            phy_points_to = phy_obj.phy_points_to
            if phy_points_to is not None:
                line = 'info.phy_points_to = ' + phy_points_to
                output_lines.append(line)

        profile = modem_model.profile # should only be one profile!

        #Print all non-register calculator variables that are forced as "config" (includes profile inputs, other forced vars)
        for var in modem_model.vars:
            if var._value_forced is not None and var.svd_mapping is None:
                line = "config.%s = %s" % (var.name, Human_Readable._py2_str_format(var._value_forced))
                output_lines.append(line)

        for output in profile.outputs:

            # Print all overridden Profile Outputs as "forced"
            if output.override is not None:
                if (output.output_type is ModelOutputType.SVD_REG_FIELD) or (output.output_type is ModelOutputType.SEQ_REG_FIELD):
                    var = getattr(modem_model.vars, output.var_name)
                    if var.value_do_not_care:
                        LogMgr.Warning("%s is forcing register %s even though it is set to do not care" % (phy_name, var.svd_mapping))
                    line = "forced.%s = %s" % (var.svd_mapping, Human_Readable._py2_str_format(output.override))
                else:
                    line = "forced.%s = %s" % (output.var_name, Human_Readable._py2_str_format(output.override))
                output_lines.append(line)

            if (output.var_value is not None and not output.var.value_do_not_care) or (output.var.value_do_not_care and show_do_not_care):
                var = getattr(modem_model.vars, output.var_name)

                # Print all non-overridden register Profile Outputs as "rm"
                if (output.output_type is ModelOutputType.SVD_REG_FIELD) or (output.output_type is ModelOutputType.SEQ_REG_FIELD):
                    if var.value_do_not_care:
                        line = "rm.%s = X" % (var.svd_mapping)
                    else:
                        line = "rm.%s = %s" % (var.svd_mapping, Human_Readable._py2_str_format(output.var_value))

                # Print all non-overridden non-register Profile Outputs as "info" (e.g. RAIL outputs)
                else:
                    line = "info.%s = %s" % (output.var_name, Human_Readable._py2_str_format(output.var_value, output.var.var_type))
                if len(var._access_write) > 0:
                    tab_column = 80
                    if len(line) >= tab_column:
                        tab_column = len(line) + 1
                    line = line.ljust(tab_column) + "[" + var._access_write[0] + "]"
                output_lines.append(line)

        for var in modem_model.vars:
            if var.svd_mapping is None:
                try:
                    if var._value_calc is not None:

                        #Only output actual or calculated information if the variable is not forced
                        if not ((var._value_forced is not None) or
                           hasattr(profile.forces, var.name) or
                           hasattr(profile.outputs, var.name)):

                            #Print all _actual variables as "actual"
                            if var.name.endswith(ICalculator.actual_suffix):
                                line = "actual.%s = %s" % (var.name, Human_Readable._py2_str_format(var._value_calc))

                            #Print all other non-register variables as "calculated"
                            else:
                                line = "calculated.%s = %s" % (var.name, Human_Readable._py2_str_format(var._value_calc, name=var.name))

                            if len(var._access_write) > 0:
                                tab_column = 80
                                if len(line) >= tab_column:
                                    tab_column = len(line) + 1
                                line = line.ljust(tab_column) + "[" + var._access_write[0] + "]"
                            output_lines.append(line)
                except ModelVariableEmptyValue:
                    pass

        # Sort the lines and write them to a file
        outputfile = open(outputfilename, 'w')
        for line in sorted(output_lines):
            outputfile.write('%s\n' % line)
        outputfile.close()

        # For easier diffing of values only between part families, output same data without [access_write] data to a second file
        outputfile = open(outputfilename.replace('.cfg','.cfg_values_only'), 'w')
        for line in sorted(output_lines):
            if (line.find('[')) > 0:
                idx = (line.find('['))
            else:
                idx = len(line)
            outputfile.write('%s\n' % line[0:idx].strip())
        outputfile.close()

        # For easier diffing of values only between part families, output same data but with _jumbo, _dumbo, etc, remapped to _FAMILY
        outputfile = open(outputfilename.replace('.cfg', '.cfg_scrub_fam'), 'w')
        for line in sorted(output_lines):
            line_scrub_fam = line
            for fam in ["dumbo", "jumbo", "nerio", "nixi", "panther", "lynx", "ocelot", "Dumbo", "Jumbo", "Nerio", "Nixi", "Panther", "Lynx", "Ocelot"]:
                line_scrub_fam = line_scrub_fam.replace("_"+fam,"_FAMILY")
            outputfile.write('%s\n' % line_scrub_fam)
        outputfile.close()

    #
    #
    #
    @staticmethod
    def compare_forced_to_calculated(modem_model):
        output_lines = list()
        for var in modem_model.vars:
            if var._value_forced is not None:       # gdc:  What's the right way to check this?
                if var._value_calc is not None:
                    if var._value_calc == var._value_forced:
                        line = "        %s = %d:  Calculated value matches forced value." % (var.name, var.value_forced)
                        #print(line)
                        output_lines.append(line)

        return output_lines

    @staticmethod
    def _py2_str_format(var_value, var_type=None, name=None):
        # special function catch differences in Python 2 and 3
        # Python 2 rounds floats to 12 significant digits when converting to string, actual values are identical
        value = var_value
        if sys.version_info[0] > 2:
            if type(var_value) == float:
                if var_value != 0.0:
                    try:
                        value = str(round(var_value, 12 - int(floor(log10(abs(var_value)))) - 1))
                    except ValueError:
                        value = var_value
            elif isinstance(var_value, list):
                if var_type == long:
                    if any(abs(n)>(0x7fffffff) for n in var_value):
                        # Py3 doesn't add the "L" suffix to long values, since there are no longs, everything is an int
                        value = str(var_value)
                        if 'L' not in value:
                            value = value.replace(',', 'L,')
                            value = value.replace(']', 'L]')
        return value

    @staticmethod
    def _get_git_branch_and_commit():
        if Human_Readable.git_branch is None or Human_Readable.git_commit is None:
            git_branch = 'unknown'
            git_commit = 'unknown'
            try:
                current_path = os.path.dirname(os.path.abspath(__file__))
                base_project_dir = os.path.realpath(current_path + '/../../../..')
                git_commit = subprocess.check_output(['git', 'describe', '--tags'], cwd=base_project_dir)
                git_commit = git_commit.strip()
                git_commit = str(git_commit.decode('utf-8')).replace('\n', '')

                git_branch = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD'], cwd=base_project_dir)
                git_branch = git_branch.strip()
                git_branch = str(git_branch.decode('utf-8')).replace('\n', '')
            except Exception as ex:
                pass
            Human_Readable.git_branch = git_branch
            Human_Readable.git_commit = git_commit

