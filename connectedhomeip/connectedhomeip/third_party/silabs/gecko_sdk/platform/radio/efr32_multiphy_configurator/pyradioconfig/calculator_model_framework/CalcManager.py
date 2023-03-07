"""
Radio Configurator
"""
import copy
import os
import traceback
import types
from enum import Enum

from pyradioconfig._version import __version__
from pyradioconfig.calculator_model_framework.Utils.CalcStatus import CalcStatus
from pyradioconfig.calculator_model_framework.Utils.ClassManager import ClassManager
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import UnknownOPNTypeException, \
    InvalidOptionOverride, UnknownProfileException
from pyradioconfig.calculator_model_framework.Utils.FileUtilities import FileUtilities
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.calculator_model_framework.exceptions.exceptions import *
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.calculator_model_framework.interfaces.idefault_phy import IDefaultPhy
from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter, PhyFilterGroupTypes
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget
from pyradioconfig.features.features import Features
from pycalcmodel.core.model import ModelRoot
from pycalcmodel.core.variable import ModelVariableEmptyValue
from pycalcmodel.core.variable import ModelVariableWriteAccess
from pycalcmodel.core.variable_access_name import VariableAccess
from pycalcmodel.core.input import ModelInputType
from pyradioconfig.calculator_model_framework.model_serializers.human_readable import Human_Readable
from pyradioconfig.calculator_model_framework.model_serializers.static_timestamp_xml import Static_TimeStamp_XML
from pyradioconfig.calculator_model_framework.model_serializers.import_isc_files import ImportISCFiles
from pyradioconfig import parts
from pycalcmodel.core.model import ModelRootInstanceXml
from pycalcmodel.core.output import ModelOutputType

from py_2_and_3_compatibility import *


class CalcManager(object):
    """
    Main interfaces to pyradioconfig.

    Args:
       part_family (str):  Part family name (e.g. 'dumbo').
       part_rev (str):  Part revision name (e.g. 'A0').

    Example usages:
        Create Radio Configurator Object:

        >>> radio_configurator = CalcManager(part_family='dumbo', part_rev='A0')

        Calculate PHY, generates output instance XML at output_dir and returns handle to model instance:

        >>> radio_configurator.calc_config_phy(phy_name = 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K', output_dir='c:/some_directory')

        Calculate PHY, generates output instance XML at output_dir and returns handle to model instance, with optional inputs:

        >>> optional_inputs=dict()
        >>> optional_inputs['base_frequency_hz'] = 2402000000
        >>> radio_configurator.calc_config_phy(phy_name = 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K', output_dir='c:/some_directory', optional_inputs=optional_inputs)

        Calculate Profile with inputs, generates output model XML instance:

        >>> model = radio_configurator.create_modem_model_instance(profile_name='Base')
        >>> model.inputs.base_frequency_hz.var_value =  long(915000000)
        >>> radio_configurator.calculateXMLInstance(model, output_path='c:/some_dir/output_instance.xml')

        Alternative calculate Profile with inputs, generates output model instance:

        >>> model = radio_configurator.create_modem_model_instance(profile_name='Base')
        >>> model.inputs.base_frequency_hz.var_value =  long(915000000)
        >>> radio_configurator.calculate(model_instance_profile)

        How to import .ISC file:

        >>> from pyradioconfig.calculator_model_framework.model_serializers.import_isc_files import ImportISCFiles
        >>> isc = ImportISCFiles()
        >>> inputs = isc.parse_file(test_isc_file_path)
        >>>
        >>> # Create model and fill with ISC file values
        >>> model_instance_input = radio_configurator.calc_config_profile(isc.profile, output_instance_file_path, inputs)
        >>>
        >>> # Calculate the model
        >>> result_code, error_message = radio_configurator.calculate(model_instance_input)

        Convert output instance model to XML file:

        >>> radio_configurator.processed_model_to_xml(model_instance_output, output_file='c:/temp/output.xml')

        For more details example usages, please review unittests.
    """

    # Static variables
    __MODEL_NAME = __name__
    __MODEL_VER = __version__
    __developer_mode = True
    IC_str = ITarget.IC_str

    # Private variables
    __part_family = None
    __part_revision = None
    __target = None

    def __init__(self, part_family=None, part_rev=None, target=None):
        """CalcManager Constructor.

        Args:
           part_family (str):  Part family name (e.g. 'dumbo').
           part_rev (str):  Part revision name (e.g. 'A0').

        Returns:
           CalcManager.
        """
        if part_family is not None:
            part_family = part_family.strip()
        self.__part_family = part_family
        if part_rev is not None:
            part_rev = part_rev.strip()
        self.__part_revision = part_rev
        if target is None:
            target = 'IC'  # default target
        self.__target = target
        LogMgr.reset()

    @property
    def part_family(self):
        """Part family getter

        Returns:
           part_family (str): Part family name (e.g. 'dumbo').

        """
        return self.__part_family

    @part_family.setter
    def part_family(self, part_family):
        """Part family setter

        Args:
           part_family (str): Part family name (e.g. 'dumbo').

        """
        if part_family is not None:
            part_family = part_family.strip()
        self.__part_family = part_family

    @property
    def part_revision(self):
        """Part revision getter

        Returns:
           part_revision (str): Part revision (e.g. 'A0').

        """
        return self.__part_revision

    @part_revision.setter
    def part_revision(self, part_revision):
        """Part revision setter

        Args:
           part_revision (str): Part revision (e.g. 'A0').

        """
        if part_revision is not None:
            part_revision = part_revision.strip()
        self.__part_revision = part_revision

    @property
    def target(self):
        """Part target getter

        Returns:
           target (str): Target (e.g. 'IC', 'Sim', 'FGPA').

        """
        return self.__target

    @target.setter
    def target(self, target):
        """Part target setter

        Args:
           target (str): Target (e.g. 'IC', 'Sim', 'FGPA').

        """
        if target is not None:
            target = target.strip()
        self.__target = target

    @property
    def version(self):
        """pyradioconfig version getter

        Returns:
           version (str): pyradioconfig version (e.g. '1.0.2').

        """
        return self.__MODEL_VER

    @staticmethod
    def parseOPN(opn):
        """Parses OPN string into part_family, part_revision

        Args:
            opn (str) : Orderable Part Number,  (e.g. "EFR32fg1p133f256gm32-A0")

        Returns:
           part_family (str): Part family name (e.g. 'dumbo').
           part_revision (str): Part revision (e.g. 'A0').

        """
        # Used for unit testing
        if opn.lower().startswith('unit_test_part'):
            tempList = [x.strip() for x in opn.split('-')]
            partFamily = 'unit_test_part'
            partRevision = tempList[1].upper()
            return partFamily, partRevision
        # OPN decoder for EFR32
        elif opn.lower().startswith('efr32'):
            tempList = [x.strip() for x in opn.split('-')]
            partFamily = opn[5]
            if partFamily.lower() == 'f':
                partFamily = 'dumbo'
            partRevision = tempList[1].upper()
            return partFamily, partRevision
        # Unkown OPN type
        else:
            raise UnknownOPNTypeException('Unknown OPN type: ' + str(opn))

    def getProfiles(self):
        """Returns a list of profile objects

        Returns:
           uniqueProfiles (dict): Uniquely named profile objects

        """
        profileList = self.__getProfileList()
        uniqueProfiles = dict()

        for profile in profileList:
            # Check if profile is already in the dictionary
            key  = profile.getName()
            #if not uniqueProfiles.__contains__(key):
            # Add to dictionary
            uniqueProfiles[key] = profile

        return uniqueProfiles

    def __getProfileList(self):
        """Returns a list of all profile names for part family and part revision

        Returns:
           list (list): List of uniquely named profile

        """
        self.__verifyPartFamilyPartRevisionIsSet()
        part_family = self.__part_family
        part_revision = self.__part_revision
        try:
            list = []

            # Find all .py files for this family
            path = self.__getPartFamilyPath(part_family) + "/profiles/" + "__init__.py"
            list.extend(self.__getProfileListFromPath(path))

            return list
        except ImportError:
            LogMgr.Error("Unable to import modules at: %s" % (path,))
        except Exception:
            LogMgr.Error(traceback.print_exc())

    def __getProfileListFromPath(self, path):
        """Returns a list of all profiles for part family and part revision

        Args:
            path (str) : Path to search for profile classes

        Returns:
           list (list): List of uniquely named profile classes
        """
        list = []

        # Import profile modules and classes
        classes = ClassManager.getClasses(path)
        for cls in classes:
            if issubclass(cls, IProfile):  # Make sure it is a Profile class
                # Add everything to list
                list.append(cls())

        return list

    def __getPartFamilyPath(self, part_family):
        """Helper function to build local path from part_family

        Args:
           part_family (str): Part family name (e.g. 'dumbo').

        Returns:
           path (str): Path to part family specific files
        """
        #path = os.path.join(os.path.dirname(__file__),  '..', 'parts', part_family)
        path = os.path.join(FileUtilities.resource_path(os.path.dirname(__file__)),  '..', 'parts', part_family)
        return path

    def __getPartCommonPath(self):
        path = os.path.join(FileUtilities.resource_path(os.path.dirname(__file__)),  '..', 'parts', 'common')
        return path

    def createXMLInstance(self, input_path, phy_name = None, profile_name = None):
        """Create model XML instance file

        Args:
            input_path (str) : Path to which XML instance is written to
            phy_name (str) : PHY name to create insance of (Optional, default = None)
            profile_name (str) : Profile name to create insance of (Optional, default = None)

        Returns:
           status (boolean): Returns True if no exceptions are caught
        """
        status = False
        try:
            # Convert version string to object
            #version = Version.fromString(version_string)

            modem_instance_model = self.create_modem_model_instance(phy_name)
            self.convert_model_instance_to_XML(modem_instance_model, input_path, phy_name = phy_name, profile_name = profile_name)

            status = True
        except:
            raise

        # return success / fail
        return status

    def convert_model_instance_to_XML(self, model, output_path, phy_name = None, profile_name = None):
        """Converts model instance to XML instance file

        Args:
            model (ModelRoot) : Data model instance
            output_path (str) : Path to which XML instance is written to
            phy_name (str) : PHY name to create insance of (Optional, default = None)
            profile_name (str) : Profile name to create insance of (Optional, default = None)
        """
        if phy_name is not None:
            phy = getattr(model.phys, phy_name)
        else:
            phy = None

        # Find profile requested
        #profile = self._findProfile(profile_name, profile_version_string)
        #if (profile == None):
        #    raise Exception("Profile: " +  profile_name + ", Version: " + profile_version_string + " not found.")

        # Build profile model
        #profile.buildProfileModel(modem_model)

        # Send modem model to file
        if profile_name is None:
            profile = getattr(model.profiles, phy.profile_name)
        else:
            profile = getattr(model.profiles, profile_name)

        if hasattr(model, 'processed'):
            processed = model.processed
        else:
            processed = False

        if hasattr(model, 'result_code'):
            result_code = model.result_code
        else:
            result_code = 0

        if hasattr(model, 'error_message'):
            error_message = model.error_message
        else:
            error_message = ''

        model.to_instance_xml(output_path, self.__part_revision, '', processed, result_code, error_message, profile, phy)  # New way

    def _findProfile(self, profile_name):
        """Finds profile for profile_name, version, part_family, part_revision

        Args:
            profile_name (str) : Profile name to create insance of (Optional, default = None)

        Returns:
           profile (Profile): Returns profile model object
        """
        profileFound = False
        profileList = self.__getProfileList()
        for profile in profileList:
            key  = profile.getName()
            if (profile_name == key):
                profileFound = True
                break

        if profileFound:
            return profile
        else:
            return None

    def calculateXMLInstance(self, modem_instance_model, output_path):
        """Create Input XML profile instance file

        Args:
            modem_instance_model (ModelRoot) : Data model instance
            output_path (str) : File path that the output calculated XML instance is written to

        Returns:
           result_code (int): Calculation return status codes
           error_message (str) : Calculation error message, if any (default = '')

        Example Usage:
            Calculate Profile with inputs, generates output model instance:

            >>> model = radio_configurator.create_modem_model_instance(profile_name='Base')
            >>> model.inputs.base_frequency_hz.var_value =  long(915000000)
            >>> radio_configurator.calculateXMLInstance(model, output_path='c:/some_dir/output_instance.xml')

            Alternative calculate Profile with inputs, generates output model instance:

            >>> model = radio_configurator.create_modem_model_instance(profile_name='Base')
            >>> model.inputs.base_frequency_hz.var_value =  long(915000000)
            >>> radio_configurator.calculate(model_instance_profile)
        """

        # Should only be one profile?  Should be only one
        result_code = CalcStatus.Success.value
        model_profile = None
        try:
            model_profile = modem_instance_model.profile
        except StopIteration:
            raise Exception("No profile in modem model!  Needs to have atleast one.")

        if model_profile is not None:
            try:
                # Create new model to absorb any new inputs, defaults, etc...
                modem_instance_model_new = self.create_modem_model_instance(profile_name=model_profile.name)
                self.read_profile_inputs_into_model(modem_instance_model_new, model_profile)
            except (SystemExit, SystemError, MemoryError, KeyboardInterrupt):
                raise
            except BaseException as e:
                # Let the calculate function catch the error
                result_code = CalcStatus.Failure.value
                error_message = str(e)
                pass

        # Run calculators
        if (result_code == CalcStatus.Success.value):
            result_code, error_message = self.execute_calc_fuctions(modem_instance_model_new)

        # Any phy's in here?  Could be one or none
        model_phy = None
        try:
            if result_code == CalcStatus.Success.value:
                model_phy = modem_instance_model.phy
                if model_phy is not None:
                    # add phy to new model
                    phy = self._findPhy(model_phy.name)
                    if not phy is None:
                        phy(modem_instance_model_new)
                        model_phy = modem_instance_model_new.phy
                    else:
                        model_phy = None
        except StopIteration:
            # ignore no phy found
            pass

        model_profile = modem_instance_model_new.profile

        # Send calculated modem model to file
        processed = True
        modem_instance_model_new.to_instance_xml(output_path,
                                                 modem_instance_model_new.part_revision,
                                                 '',
                                                 processed,
                                                 modem_instance_model_new.result_code,
                                                 modem_instance_model_new.error_message,
                                                 model_profile, model_phy)

        return result_code, error_message

    def execute_calc_fuctions(self, model_instance):
        """Executes all calacaultor functions on data model

        Args:
            modem_instance_model (ModelRoot) : Data model instance

        Returns:
           result (int): Calculation return status codes
           error_message (str) : Calculation error message, if any (default = '')
        """

        result = CalcStatus.Success.value
        error_message = ''

        try:
            # Read profile variables into variables
            self.read_profile_into_variables(model_instance)

            # Get list of functions to perform calculators
            calcFunctions = self._getCalculatorFunctionList()
            self.calculateOverList(calcFunctions, model_instance)

        except ModelVariableWriteAccess as e:
            # This is a show stopper (assert) for the calc functions!
            raise
        except (SystemExit, SystemError, MemoryError, KeyboardInterrupt):
            raise
        except Exception as e:
            result = CalcStatus.Failure.value
            error_message = str(e)
            if self.__developer_mode:
                raise
            else:
                LogMgr.Error(error_message)
                LogMgr.Debug(traceback.format_exc())

        model_instance.part_revision = self.part_revision
        model_instance.result_code = result
        model_instance.error_message = error_message
        model_instance.processed = True
        model_instance.logs = copy.deepcopy(LogMgr.get_queue())
        LogMgr.reset()
        return result, error_message

    def _getCalculatorFunctionList(self):
        """Returns a list of all calculator functions for part family and part revision

        Returns:
           list (list): List of calculation function references
        """
        calculators = self._getCalculatorsList()
        list = []
        for calculator in calculators:
            list.extend(calculator.getCalculationList())

        return list

    def _getCalculatorsList(self):
        """Returns a list of all calculator objects for part family and part revision

        Returns:
           list (list): List of calculation object references
        """
        self.__verifyPartFamilyPartRevisionIsSet()
        part_family = self.__part_family
        part_revision = self.__part_revision

        # Find all part rev specific calculator .py files for this family
        try:
            list = []
            class_type = ICalculator

            if part_family.lower() in ['dumbo', 'jumbo', 'lynx', 'nerio', 'nixi', 'panther', 'leopard', 'unit_test_part']:
                # Find all .py files for this part in common
                path = self.__getPartCommonPath() + "/calculators/" + "__init__.py"
                common_part_list = ClassManager.getClassListFromPath(path, class_type)
            else:
                # As of ocelot, we are no longer merging 'commmon' calculators
                common_part_list = []

            # Find all .py files for this family and revision in part revision
            path = self.__getPartFamilyPath(part_family) + "/calculators/" + "__init__.py"
            rev_list = ClassManager.getClassListFromPath(path, class_type)

            # Loop through common phys and remove and parent classes from part rev specific instance
            list = ClassManager.merge_lists_classes(common_part_list, rev_list)

            return list
        except ImportError:
            LogMgr.Error("Unable to import modules at: %s" % (path,))
        except Exception:
            LogMgr.Error(traceback.print_exc())

        return list

    def calculateOverList(self, calc_routine_list, modem_model):
        """Loop through all function pointers and execute calculators on model

        Args:
            calc_routine_list (list) : List of calcaultor functions to execute
            modem_model (ModelRoot) : Data model instance

        """
        # Validate model
        if not modem_model.validate():
            raise Exception('Model does not pass validation.  Please review vars and profile var definitions.')

        initial_calc_routine_name_list = []
        for calc_routine in calc_routine_list:
            initial_calc_routine_name_list.append(calc_routine.__name__)

        function  = VariableAccess()
        while True:
            num_calc_routines = calc_routine_list.__len__()
            #print("Number of routines = %d" % num_calc_routines)

            for calc_routine in calc_routine_list[:]:       # The [:] thing is a trick to make a copy of the list.
                                                            # Don't iterate over a list while items are being removed from it.
                #print("About to call %s" % calc_routine.func_name)
                try:
                    if isinstance(calc_routine, types.MethodType):
                        try:
                            function.name = calc_routine.im_class.__name__ + "." + calc_routine.__name__ + "()"
                        except AttributeError:
                            # function.name = calc_routine.__qualname__ + "()"
                            function.name = calc_routine.__self__.__class__.__name__ + "." + calc_routine.__name__ + "()"
                        calc_routine(modem_model)
                        #print("     Success!  %d left" % calc_routine_list.__len__())
                        calc_routine_list.remove(calc_routine)              # Remove the calc routine from the list if it didn't have errors
                        function.name = None
                except ModelVariableEmptyValue as e:
                    # Ignore when variable value has not been set
                    #print("     Undefined inputs found in %s.  Will try again on next pass" % calc_routine.func_name)
                    pass
                # except (SystemExit, SystemError, MemoryError, KeyboardInterrupt):
                #     raise
                #except Exception as e:
                #    traceback.print_exc(file=sys.stdout)
                #    print('DEBUG : ' + str(e))
                #    raise e
                finally:
                    function.name = None

            #print("************** Pass complete with %d routines left ******************" % calc_routine_list.__len__())

            if num_calc_routines == calc_routine_list.__len__():
                #We are finished calculating when the number of remaining calc routines is the same as we started with
                #on this pass

                modem_model.calc_routine_execution = dict()

                final_calc_routine_name_list = []
                for calc_routine in calc_routine_list:
                    final_calc_routine_name_list.append(calc_routine.__name__)

                for calc_routine_name in initial_calc_routine_name_list:
                    if calc_routine_name in final_calc_routine_name_list:
                        #This means we failed to execute the routine
                        modem_model.calc_routine_execution[calc_routine_name] = False
                    else:
                        modem_model.calc_routine_execution[calc_routine_name] = True

                break

    def getPhys(self):
        """Returns a list of phys

        Returns:
           phyList (list) : List of Phy reference object functions
        """
        phyList = self._getPhyFunctionList()
        return phyList

    def _getPhyFunctionList(self):
        """Returns a list of all phy functions for part family and part revision

        Returns:
           list (list) : List of Phy reference object functions
        """
        phy_class_list = self.__getPhyList()
        phy_function_list = []
        phy_name_list = []
        for phy_class in phy_class_list:
            phy_functions = phy_class.getPhyList()
            for phy_function in phy_functions:
                phy_function_name = phy_function.__name__
                if (phy_function_name in phy_name_list) and (phy_function_name != '_phypass'):
                    #Found a duplicate PHY definition, raise exception
                    raise Exception("Error! Found duplicate definition for %s" % phy_function_name)
                else:
                    phy_function_list.append(phy_function)
                    phy_name_list.append(phy_function_name)

        return phy_function_list

    def __getPhyList(self):
        """Returns a list of all phys for part family and part revision

        Returns:
           list (list) : List of Phy reference objects
        """
        self.__verifyPartFamilyPartRevisionIsSet()
        part_family = self.__part_family
        part_revision = self.__part_revision

        # Find all .py files for this family and revision in part revision
        partfamily_path = self.__getPartFamilyPath(part_family) + "/phys/" + "__init__.py"
        list = ClassManager.getClassListFromPath(partfamily_path, IPhy)
        return list

    def read_phy_into_profile(self, phy_name, modem_model):
        """Load PHY values into Profile inputs in data model

        Args:
            phy_name (str) : Key name of the PHY in the model
            modem_model (ModelRoot) : Data model instance

        """
        phy = getattr(modem_model.phys, phy_name)
        profile_name = phy.profile_name
        profile = self._findProfile(profile_name)
        if profile is not None:
            profile.buildProfileModel(modem_model)

        if phy is None:
            phyFound = self._findPhy(phy_name)
            if not phyFound is None:
                phyFound(modem_model)

        # Load profile defaults into inputs
        profile_reference = getattr(modem_model.profiles, profile_name)
        for input in profile_reference.inputs:
            # Do not load in default values for deprecated inputs (we want to leave as None if unused)
            if input.default is not None and input.deprecated == False:
                input.var_value = input.default

        # Assign Phy inputs to Profile Inputs
        for profile_input in phy.profile_inputs:
            variable = getattr(profile_reference.inputs, profile_input.var_name)
            value = profile_input.value
            if value is not None:
                variable.var_value = value

        # Assign Phy output overrides to Profile outputs overrides
        profile_reference = getattr(modem_model.profiles, profile_name)
        for profile_output in phy.profile_outputs:
            variable = getattr(profile_reference.outputs, profile_output.var_name)
            value = profile_output.override
            if value is not None:
                variable.override = value


    def read_profile_into_variables(self, modem_model):
        """Load Profile values into variables in data model

        Args:
            modem_model (ModelRoot) : Data model instance
        """

        profile = modem_model.profile

        # Fill profile inputs with default values if they are empty
        for input in profile.inputs:
            # Do not load deprecated values into model
            if input.var_value is None and input.default is not None and input.deprecated == False:
                input.var_value = input.default

        # Call any profile specific calculate functions, prior to running all the calc functions
        # This function is responsible for taking care of deprecating of input variables,
        # converting profile inputs into more commonly used calculator inputs, or whatever.
        self._call_profile_calculate(modem_model)

        # Assign user inputs from Profile to Variables, but skip deprecated Profile Inputs
        # as they should have no direct impact on Model Variables
        for input in profile.inputs:
            if input.var_value is not None and input.deprecated == False:
                variable = getattr(modem_model.vars, input.var_name)
                variable.value_forced = input.var_value

        # Assign user outputs from Profile to Variables
        for output in profile.outputs:
            if output.override is not None:
                variable = getattr(modem_model.vars, output.var_name)
                if (output.output_type is ModelOutputType.SVD_REG_FIELD) or (output.output_type is ModelOutputType.SEQ_REG_FIELD):
                    if output.override < 0:
                        #Convert to 2s compliment and warn
                        field_width = variable.get_bit_width()
                        variable.value_forced = (1 << field_width) + output.override
                        LogMgr.Warning("Negative value for override of %s, assuming 2s compliment" % output.var_name)
                    else:
                        # Use positive value
                        variable.value_forced = output.override
                else:
                    variable.value_forced = output.override

        # Call any target specific calculate functions last (these overwrite all other settings)
        self._call_target_calculate(modem_model)

    def create_modem_model_instance(self, phy_name=None, profile_name=None):
        """Creates an empty model instance for a PHY or Profile

        Args:
            phy_name (str) : PHY name to create insance of (Optional, default = None)
            profile_name (str) : Profile name to create insance of (Optional, default = None)
            Note: You must specify either a PHY name or Profile name.

        Returns:
            modem_model (ModelRoot) : Data model instance

        Example Usage:
            Calculate Profile with inputs, generates output model instance:

            >>> model = radio_configurator.create_modem_model_instance(profile_name='Base')
            >>> model.inputs.base_frequency_hz.var_value =  long(915000000)
            >>> radio_configurator.calculateXMLInstance(model, output_path='c:/some_dir/output_instance.xml')

        """
        self.__verifyPartFamilyPartRevisionIsSet()

        # Create empty modem model
        modem_model_instance = ModelRoot(self.__part_family, self.__MODEL_VER, self.__target, part_revision=self.__part_revision)

        # Build features
        Features.build(modem_model_instance)

        # Build calculator variables
        calculators = self._getCalculatorsList()
        for calculator in calculators:
            calculator.buildVariables(modem_model_instance)

        if profile_name is None:
            # Populate all profiles, since we don't know which one the Phy will need yet
            profiles = self.__getProfileList()
            for profile in profiles:
                profile.buildProfileModel(modem_model_instance)
        else:
            # Only make the profile needed
            profile = self._findProfile(profile_name)
            if profile is not None:
                profile.buildProfileModel(modem_model_instance)
            else:
                raise UnknownProfileException('Profile %s is not available in the Radio Configurator for this part. Please use an available Profile.' % profile_name)

        # Find and build phy
        if phy_name is not None:
            phy = self._findPhy(phy_name)
            if not phy is None:
                phy(modem_model_instance)
            else:
                phy = self._findPhyGUID(phy_guid=phy_name)
                if not phy is None:
                    phy(modem_model_instance)
                    phy_name = modem_model_instance.phy.name  # phy.__name__

            if profile_name is None:
                # Hack to remove profiles we don't need for this phy
                phy_object = getattr(modem_model_instance.phys, phy_name)
                profile_name = phy_object.profile_name
                modem_model_instance.profiles.clear()
                profile = self._findProfile(profile_name)
                if profile is not None:
                    profile.buildProfileModel(modem_model_instance)

        # Add default pnys
        #self._buildDefaultPhys(modem_model_instance)

        return modem_model_instance

    def create_modem_model_type(self):
        """Creates a type model for current part family and revision

        Returns:
            modem_model (ModelRoot) : Data model type (lists all phys, profiles, varaibles, etc...)
        """
        self.__verifyPartFamilyPartRevisionIsSet()

        # Create empty modem model
        modem_type_model = ModelRoot(self.__part_family, self.__MODEL_VER, target=self.__target, part_revision=self.__part_revision)

        # Build features
        Features.build(modem_type_model)

        # Build calculator variables
        calculators = self._getCalculatorsList()
        for calculator in calculators:
            calculator.buildVariables(modem_type_model)

        # Build model with all profiles
        profiles = self.__getProfileList()
        for profile in profiles:
            profile.buildProfileModel(modem_type_model)

        # Build model with all phys
        phys = self._getPhyFunctionList()
        for phy in phys:
            phy(modem_type_model)

        # Add default pnys
        self._buildDefaultPhys(modem_type_model)

        return modem_type_model

    def _findPhy(self, phy_name):
        """Finds phy for phy_name, version, part_family, part_revision

        Args:
            phy_name (str) : PHY name to find

        Returns:
            phy_function (function) : PHY function reference
        """
        phyFound = False
        phyList = self.__getPhyList()
        for phy in phyList:
            phy_functions = phy.getPhyList()
            for phy_function in phy_functions:
                key = phy_function.__name__
                if (phy_name == key):
                    phyFound = True
                    break

            if phyFound:
                break

        if phyFound:
            return phy_function
        else:
            return None

    def read_profile_inputs_into_model(self, modem_model, model_profile):
        """Reads inputs from profile object into model

        Args:
            modem_model (ModelRoot) : Data model to read into
            model_profile (Profile) : Profile object with input values
        """

        profile_name = model_profile.name
        profile = self._findProfile(profile_name)
        if profile is not None:
            profile.buildProfileModel(modem_model)

        profile_new = getattr(modem_model.profiles, profile_name)
        # Copy over input values
        for input in model_profile.inputs:
            if input.var_value is not None:
                input_new = getattr(profile_new.inputs, input.var_name)
                input_new.var_value = input.var_value

        # Copy over output overrides
        for output in model_profile.outputs:
            if output.override is not None:
                output_new = getattr(profile_new.outputs, output.var_name)
                output_new.override = output.override

        # Assign user inputs from Model Profile to Variables
        #for input in profile_new.inputs:
        #    if input.var_value is not None:
        #        variable = getattr(modem_model.vars, input.var_name)
        #        variable.value_forced = input.var_value

        # Assign user outputs from Model Profile to Variables
        #for output in profile_new.outputs:
        #    if output.override is not None:
        #        variable = getattr(modem_model.vars, output.var_name)
        #        variable.value_forced = output.override

    def __verifyPartFamilyPartRevisionIsSet(self):
        """Verifies part family and revision is set

        Returns:
            Succesful (boolean) : Both part family and revision are not None.

        Raises:
            Exception is either part family or revision is None
        """
        if self.__part_family is None:
            raise Exception("Part family must be set!")
        if self.__part_revision is None:
            raise Exception("Part revision must be set!")
        if len(self.__part_family) == 0:
            raise Exception("Invalid Part family = {}!".format(self.__part_family))
        return True

    def getPhyNames(self):
        """Gets a list of PHY names

        Returns:
            phy_names (list) : List of PHY name strings
        """

        phy_names = list()
        for phy in self.getPhys():
            phy_names.append(phy.__name__)

        return phy_names

    def create_modem_model_instance_and_load_phy(self, phy_name=None):
        """Creates a modem model instance and loads PHY

        Args:
            phy_name (str) : PHY name to load

        Returns:
            model_instance (MOdelRoot) : New instance of data model with single PHY
        """
        model_instance = self.create_modem_model_instance(phy_name)
        self.read_phy_into_profile(model_instance.phy.name, model_instance)
        return model_instance


    def calculate(self, model_instance):
        """Executes all calaualtor functions agains modek variables

        Args:
            model_instance (ModelRoot) : Data model with variables to run through calcaultions

        Returns:
           result_code (int): Calculation return status codes
           error_message (str) : Calculation error message, if any (default = '')
        """

        sim_test_phy_groups = self.get_sim_tests_phy_groups()

        model_has_phy = hasattr(model_instance, 'phy')
        if model_has_phy:
            phy_group_name = getattr(model_instance.phy, 'group_name', 'No_Group')
            phy_name = getattr(model_instance.phy, 'name', 'No_PHY_Name')
        else:
            phy_group_name = 'No_Group'
            phy_name = 'No_PHY_Name'

        #First check to make sure all required Profile Inputs are present
        #We do not check for unit_test_part because it intentionally does not follow this rule
        #Also skip the old sim PHYs as they are not well formed (just pokes)
        if (model_instance.part_family.lower() != "unit_test_part") and (phy_group_name not in sim_test_phy_groups):
            for profile_input in model_instance.profile.inputs:
                if profile_input.input_type == ModelInputType.REQUIRED and profile_input.default is None:
                    assert profile_input.var_value is not None, "Required Profile Input %s is not populated for %s" % (profile_input.var_name, phy_name)

        result_code, error_message = self.execute_calc_fuctions(model_instance)
        return result_code, error_message

    def calculate_phy(self, phy_name=None, optional_inputs=None):
        if optional_inputs is None: optional_inputs = dict()
        model_instance = self.create_modem_model_instance_and_load_phy(phy_name)

        if not self.check_phy_supported_on_target(phy_name, model=model_instance):
            raise PHYNotSupportedOnTargetException("PHY: {} not supported on target: {}".format(phy_name, self.target))

        if not model_instance.phy.locked:
            model_instance = self.load_input_dictionary_into_model(model_instance, optional_inputs)
            result_code, error_message = self.calculate(model_instance)
        else:
            static_phy_path = self.find_static_phy_path(phy_name)
            if os.path.exists(static_phy_path):
                LogMgr.Info('Loading {} from STATIC PHY.'.format(phy_name))
                model_instance = ModelRootInstanceXml(static_phy_path)
            else:
                # No static file found, going to try and calculate and save to XML
                result_code, error_message = self.calculate(model_instance)
                Static_TimeStamp_XML.print_modem_model_values_xml(static_phy_path, phy_name, model_instance)
                # TODO: Add hash or checksum generation here

            model_instance = self.load_input_dictionary_into_model(model_instance, optional_inputs)

        return model_instance

    def load_input_dictionary_into_model(self, model_instance, inputs=None):
        """Loads input dictionary into model instance

        Args:
            model_instance (ModelRoot) : Data model to load inputds into
            inputs (dict) : Dictionary of input values.  Key is var_name and Value is var_value

        Returns:
            model_instance (ModelRoot) : Updated data model
        """
        if inputs is None: inputs = dict()
        for key, value in inputs.items():
            if hasattr(model_instance.profile.inputs, key):
                # process option inputs into profile inputs
                input = getattr(model_instance.profile.inputs, key)

                if model_instance.phy and model_instance.phy.locked and value is not None and input.var_value != value:
                    raise StaticPHYInputException("This model has a static 'locked' PHY.  Cannot supply input overrides: {}!".format(key))

                if input._var.var_type != Enum:
                    if value is not None:
                        input.var_value = (input._var.var_type)(value)
                    else:
                        input.var_value = value
                else:
                    if isinstance(value, basestring):
                        if value.isdigit():
                            value = int(value)
                            enum_val = input._var.var_enum(value)
                        else:
                            enum_val = getattr(input._var.var_enum, value)
                    elif isinstance(value, int):
                        enum_val = input._var.var_enum(value)
                    elif isinstance(value, float):
                        value = int(value)
                        enum_val = input._var.var_enum(value)
                    input.var_value = enum_val
            elif hasattr(model_instance.profile.outputs, key):
                self.__override_profile_output(model_instance, key, value)
            elif hasattr(model_instance.profile.outputs, key.upper()):
                self.__override_profile_output(model_instance, key.upper(), value)
            else:
                raise InvalidOptionOverride(key + ' is not a valid option input or output for {} profile.'.format(model_instance.profile.name))

        return model_instance

    def calc_config_phy(self, phy_name, output_dir=None, optional_inputs=None, optional_filename=None, show_do_not_care=True):
        """Run the calculator and generate the calculated outputs for a single PHY

        Args:
            phy_name (str) : Name of PHY to run through the calcaultor.
            output_dir (str) : Directory path where output files are generate.
            optional_inputs (dict) : Dictionary of input values.  Key is var_name and Value is var_value [Optional Input].
            optional_filename (string) : Filename to use when generating the output files. [Optional Input: Defaults to phy_name if not specified.].

        Returns:
            model_instance (ModelRoot) : calculated data model instance

        Example usages:
            Calculate PHY, generates output instance XML at output_dir and returns handle to model instance:

            >>> radio_configurator.calc_config_phy(phy_name = 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K', output_dir='c:/some_directory')

            Calculate PHY, generates output instance XML at output_dir and returns handle to model instance, with optional inputs:

            >>> optional_inputs=dict()
            >>> optional_inputs['base_frequency_hz'] = 2402000000
            >>> radio_configurator.calc_config_phy(phy_name = 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K', output_dir='c:/some_directory', optional_inputs=optional_inputs)
        """
        if optional_inputs is None: optional_inputs = dict()

        # Force path to end with "/"
        output_dir = os.path.normpath(output_dir) + os.sep

        cfg_location, store_config_output = self.getTargetCFGInfo()

        if optional_filename is None:
            optional_filename = phy_name + ".xml"
        elif not optional_filename.lower().endswith('.xml'):
            optional_filename = optional_filename + ".xml"

        output_path = output_dir + optional_filename
        model_instance = self.calc_config(phy_name, output_path, optional_inputs, show_do_not_care=show_do_not_care)

        return model_instance

    def calc_config(self, phy_name, output_path, optional_inputs=None, show_do_not_care=True):
        """Run the calculator and generate the calculated outputs for a single PHY

        Args:
            phy_name (str) : Name of PHY to run through the calcaultor.
            output_path (str) : Full path to output file
            optional_inputs (dict) : Dictionary of input values.  Key is var_name and Value is var_value [Optional Input].

        Returns:
            model_instance (ModelRoot) : calculated data model instance

        Example usages:
            Calculate PHY, generates output instance XML at output_dir and returns handle to model instance:

            >>> radio_configurator.calc_config(phy_name = 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K', output_path='c:/some_directory/phy_name.xml')

            Calculate PHY, generates output instance XML at output_dir and returns handle to model instance, with optional inputs:

            >>> optional_inputs=dict()
            >>> optional_inputs['base_frequency_hz'] = 2402000000
            >>> radio_configurator.calc_config(phy_name = 'PHY_Datasheet_2450M_2GFSK_1Mbps_500K', output_path='c:/some_directory', optional_inputs=optional_inputs)
        """
        if optional_inputs is None: optional_inputs = dict()

        # Load instance model with phy variables
        # call top level calculator function
        model_instance = self.calculate_phy(phy_name, optional_inputs)

        if bool(optional_inputs):
            # Not an empty dictionary of optional input overrides
            #optional_filename = optional_filename + "_with_input_overrides"
            comment = phy_name + " instance with overrides"
        else:
            comment = phy_name + " instance"

        if model_instance.result_code != CalcStatus.Failure.value:
            # print the results to a .cfg file
            self.processed_model_to_cfg(model_instance, output_path, model_instance.phy.name, show_do_not_care=show_do_not_care)

            # Now look for variables that were forced unnecessarily
            output_lines = Human_Readable.compare_forced_to_calculated(model_instance)

            # enable following lines to see settings for which forced values match calculated ones
            # print('\n'.join(map(str,output_lines)))

            # print to XML file
            Static_TimeStamp_XML.print_modem_model_values_xml(output_path, model_instance.phy.name, model_instance, comment)
        else:
            # Some error happened, letting that bubble up
            pass

        return model_instance


    def calc_config_profile(self, profile_name, output_file_path=None, optional_inputs=None):
        """Run the calculator and generate the calculated outputs

        Args:
            profile_name (str) : Name of Profile to run through the calculator
            output_file_path (str) : Full file name of output file
            optional_inputs (dict) : Dictionary of input values.  Key is var_name and Value is var_value [Optional Input]

        Returns:
            model_instance (ModelRoot) : calculated data model instance
        """
        if optional_inputs is None: optional_inputs = dict()

        model_instance = self.create_modem_model_instance(phy_name=None, profile_name=profile_name)
        model_instance = self.load_input_dictionary_into_model(model_instance, optional_inputs)

        # Run calculations
        result_code, error_message = self.calculate(model_instance)

        if model_instance.result_code != CalcStatus.Failure.value:
            if output_file_path is not None:
                # Create output instance XML
                self.convert_model_instance_to_XML(model_instance, output_file_path, phy_name = None, profile_name = profile_name)
        else:
            # Some error happened, letting that bubble up
            return model_instance

        return model_instance

    def filter_out_phy_group_names(self, model, phy_group_to_exclude):
        """Get list of PHYs from model, filtering out the phy_group_to_exclude

        Args:
            model (ModelRoot) : Data model to filter out phys from
            phy_group_to_exclude (list) : List of PHYs to exclude

        Returns:
            filtered_phys (list) : List of PHY's with filters excluded
        """
        filtered_phys = list()
        for phy in model.phys:
            if phy.group_name not in phy_group_to_exclude:
                filtered_phys.append(phy)
            #else:
            #    print("skipping {0}.{1}".format(phy.group_name, phy.name))

        return filtered_phys

    def find_all_phys_of_group_name(self, model, phy_group_to_include):
        """Get list of PHYs from model, including only phy_group_to_include

        Args:
            model (ModelRoot) : Data model to filter out phys from
            phy_group_to_include (list) : List of PHYs to include

        Returns:
            filtered_phys (list) : List of PHY's with filters included
        """
        filtered_phys = list()
        for phy in model.phys:
            if phy.group_name in phy_group_to_include:
                filtered_phys.append(phy)

        return filtered_phys

    def filter_out_phy_group_names_to_phy_group_name_list(self, model, phy_group_to_exclude):
        """Get list of PHYs from model, excluding phys with groups in phy_group_to_exclude

        Args:
            model (ModelRoot) : Data model to filter out phys from
            phy_group_to_exclude (list) : List of PHY groups to exclude

        Returns:
            filtered_phy_group_names (list) : List of PHY's with group filters excluded
        """
        filtered_phy_group_names = list()
        for phy in model.phys:
            if phy.group_name not in phy_group_to_exclude:
                filtered_phy_group_names.append(phy.group_name)

        return filtered_phy_group_names

    def find_all_phy_group_names_in_phy_group_name_list(self, model, phy_group_to_include):
        """Get list of PHYs from model, including phys with groups in phy_group_to_include

        Args:
            model (ModelRoot) : Data model to filter out phys from
            phy_group_to_include (list) : List of PHY groups to include

        Returns:
            filtered_phy_group_names (list) : List of PHY's with group filters included
        """
        filtered_phy_group_names = list()
        for phy in model.phys:
            if phy.group_name in phy_group_to_include:
                filtered_phy_group_names.append(phy.group_name)

        # Hack fix: If no PHY's are found, then return a warning entry
        if not filtered_phy_group_names:
            filtered_phy_group_names.append("NO PHYS FOUND!")

        return filtered_phy_group_names

    def _get_filter_phy_groups(self):
        """Gets list of PHY filter groups for part family and revision

        Returns:
            classlist (list) : List of PHY groups needed to be filtered
        """
        self.__verifyPartFamilyPartRevisionIsSet()
        part_family = self.__part_family
        part_revision = self.__part_revision

        # Find all .py files for this family and revision in part revision
        partfamily_path = self.__getPartFamilyPath(part_family) + "/filters/" + "__init__.py"
        classlist = ClassManager.getClassListFromPath(partfamily_path, IPhyFilter)
        return classlist

    def _get_phy_groups(self, phy_filter_group_types):
        """Gets list of PHY's base on group types

        Args:
            phy_filter_group_types (Enum: PhyFilterGroupTypes) : PHY group type to get

        Returns:
            filterList (list) : List of PHYs in group
        """
        classlist = self._get_filter_phy_groups()

        filterList = []
        for filter in classlist:
            tempList = filter.get_phy_filter_groups(phy_filter_group_types)
            filterList = list(set(filterList + tempList))

        return filterList

    def get_customer_phy_groups(self):
        """Gets list customer PHYs

        Returns:
            filterList (list) : List of PHYs of group type
        """
        return self._get_phy_groups(PhyFilterGroupTypes.customer_phys)

    def get_sim_tests_phy_groups(self):
        """Gets list simulation PHYs

        Returns:
            filterList (list) : List of PHYs of group type
        """
        return self._get_phy_groups(PhyFilterGroupTypes.sim_tests_phys)

    def get_simplicity_studio_phy_groups(self):
        """Gets list Simplicity Studio PHYs

        Returns:
            filterList (list) : List of PHYs of group type
        """
        return self._get_phy_groups(PhyFilterGroupTypes.simplicity_studio_phys)

    def get_non_functional_phy_groups(self):
        """Gets list non-functional PHYs

        Returns:
            filterList (list) : List of PHYs of group type
        """
        return self._get_phy_groups(PhyFilterGroupTypes.non_functional_phys)

    def _buildDefaultPhys(self, modem_type_model):
        """Builds all PHY's into empty data model

        Args:
            modem_type_model (ModelRoot) : Empty data model to fill
        """
        list = self.__getDefaultPhyList()
        for default_phy in list:
            default_phy.build(modem_type_model)

    def __getDefaultPhyList(self):
        """Gets a list of default PHYs

        Returns:
            list (list) : List of PHY objects for part family and revision
        """
        self.__verifyPartFamilyPartRevisionIsSet()
        part_family = self.__part_family
        part_revision = self.__part_revision
        try:
            list = []

            # Find all .py files for this family
            path = self.__getPartFamilyPath(part_family) + "/phys/" + "__init__.py"
            list.extend(self.__getDefaultPhyListFromPath(path))

            return list
        except ImportError:
            LogMgr.Error("Unable to import modules at: %s" % (path,))
        except Exception:
            LogMgr.Error(traceback.print_exc())

    def __getDefaultPhyListFromPath(self, path):
        """Returns a list of all profiles for part family and part revision

        Args:
            path (str) : Path to search for PHY listing

        Returns:
            list (list) : List of PHY objects for part family and revision
        """
        list = []

        # Import profile modules and classes
        classes = ClassManager.getClasses(path)
        for cls in classes:
            if issubclass(cls, IDefaultPhy):  # Make sure it is a Profile class
                # Add everything to list
                list.append(cls())

        return list

    def processed_model_to_xml(self, model_instance, output_file):
        """Converts output instance model to an XML file

        Args:
            model_instance (ModelRoot) : Data model instance
            output_file (str) : Full fiel path name to marshal to XML file

        Returns:
            list (list) : List of PHY objects for part family and revision
        """
        if hasattr(model_instance, 'phy'):
            phy = model_instance.phy
            desc = phy.name
        else:
            desc = os.path.basename(output_file)
            phy = None
        profile = model_instance.profile
        result_code = model_instance.result_code
        error_message = model_instance.error_message
        model_instance.to_instance_xml(output_file, self.__part_revision, desc, True, result_code, error_message, profile, phy)

    def convert_isc_to_cfg(self, isc_filename, xml_filename, cfg_filename):
        # Create ISC Parser
        isc = ImportISCFiles()
        inputs = isc.parse_file(isc_filename)

        # Create model and fill with ISC file values
        if (isc.phy is not None) and (isc.phy != 'Custom settings') and (isc.phy != 'Custom_settings'):
            model_instance = self.calc_config('PHY_' + isc.phy, xml_filename, inputs)
        else:
            model_instance = self.calc_config_profile(isc.profile, xml_filename, inputs)
        Human_Readable.print_modem_model_values_v2(cfg_filename, None, model_instance)

        # print out example profile python code
        profile = model_instance.profile
        for input in profile.inputs:
            if input._var.var_type is Enum:
                if input.var_value is not None:
                    LogMgr.Debug("phy.profile_inputs.{0}.value = model.vars.{0}.var_enum.{1}".format(input.var_name, input.var_value.name))
                else:
                    LogMgr.Debug("phy.profile_inputs.{0}.value = None".format(input.var_name))
            else:
                LogMgr.Debug("phy.profile_inputs.{0}.value = {1}{2}".format(input.var_name, input.var_value, 'L' if input._var.var_type is long else ''))


    def convert_phy_to_isc_section(self, phy_name, isc_filename, xml_filename = None):
        if xml_filename is None:
            model_instance = self.calculate_phy(phy_name)
        else:
            model_instance = self.calc_config(phy_name, xml_filename)

        output_string = self.convert_model_to_isc_section(model_instance, isc_filename)
        return output_string


    def convert_model_to_isc_section(self, model_instance, isc_filename = None):
        # Create ISC Parser
        isc = ImportISCFiles()
        output_string = isc.export_modem_model_to_isc_section(model_instance, isc_filename)
        return output_string

    def _call_profile_calculate(self, model_instance):
        profile_name = model_instance.profile.name
        profile = self._findProfile(profile_name)

        if hasattr(profile, "profile_calculate"):
            profile.profile_calculate(model_instance)
        else:
            # we could ignore this, but I'd like to make sure each profile has a placeholder routine to allow
            # compatiblity functions to be added in case inputs become deprecated.
            raise Exception('Profile {} does not have a profile_calculate() function.'.format(profile_name))

    def check_phy_has_tag(self, phy_name, tag, model=None):
        tag_found_in_tags = False

        if model is None:
            model = self.create_modem_model_instance(phy_name=phy_name)

        #Try to access the PHY from the model.phys object (exists in types model as well)
        #If that fails then assume a populated model for a specific PHY (required to support GUIDs)
        try:
            phy = getattr(model.phys, phy_name)
        except:
            phy = model.phy

        if phy.tags is not None:
            if tag in phy.tags:
                tag_found_in_tags = True
        return tag_found_in_tags

    def processed_model_to_cfg(self, model_instance, output_file, phy_name=None, show_do_not_care=True):
        """Converts output instance model to a CFG file

        Args:
            model_instance (ModelRoot) : Data model instance
            output_file (str) : Full file path name to marshal to XML file

        Returns:
            None
        """
        output_file_dir = os.path.dirname(output_file)
        if not os.path.exists(output_file_dir):
            os.mkdir(output_file_dir)

        output_path_cfg = output_file.replace(".xml", "")
        output_path_cfg = output_path_cfg.replace(".XML", "")
        output_path_cfg = output_path_cfg + ".cfg"

        phy_guid = None
        if phy_name is None:
            if hasattr(model_instance, 'phy'):
                phy = model_instance.phy
                phy_name = phy.name
                if hasattr(phy, 'guid'):
                    phy_guid = phy.guid
            else:
                phy_name = os.path.basename(output_file)

        Human_Readable.print_modem_model_values_v2(output_path_cfg, phy_name, model_instance, show_do_not_care=show_do_not_care, phy_guid=phy_guid)

    @staticmethod
    def get_list_of_parts_supported():
        parts_list = []
        exclude_list = ['common', 'unit_test_part']
        parts_location = os.path.dirname(parts.__file__)
        for dirname in os.listdir(parts_location):
            if not dirname.startswith('_') and dirname not in exclude_list:
                parts_list.append(dirname)
        return parts_list

    def _call_target_calculate(self, model_instance):
        target_name = model_instance.target
        target = self._findTarget(target_name)  # type: ITarget
        if target is not None:
            target.target_calculate(model_instance)
        else:
            raise Exception('Target {} does not exists for part: {}.'.format(target_name, model_instance.part_family))

    def _findTarget(self, target_name):
        targetList = self.__getTargetList()
        for target in targetList:
            key = target.getName()
            if target_name == key:
                return target

        return None

    def __getTargetList(self):
        self.__verifyPartFamilyPartRevisionIsSet()
        part_family = self.__part_family
        try:
            list = []

            # Find all .py files for this family
            path = self.__getPartFamilyPath(part_family) + "/targets/" + "__init__.py"
            list.extend(self.__getTargetListFromPath(path))

            return list
        except ImportError:
            LogMgr.Error("Unable to import modules at: %s" % (path,))
        except Exception:
            LogMgr.Error(traceback.print_exc())

    def getTargetNameList(self):
        target_name_list = []
        target_obj_list = self.__getTargetList()
        for target in target_obj_list:
            name = target.getName()
            target_name_list.append(name)

        return target_name_list

    def __getTargetListFromPath(self, path):
        target_list = []

        # Import profile modules and classes
        classes = ClassManager.getClasses(path)
        for cls in classes:
            if issubclass(cls, ITarget):  # Make sure it is a Profile class
                # Add everything to list
                target_list.append(cls())

        return target_list

    def getTargetCFGInfo(self):
        #Return the CFG output path and whether or not we track config output for this target
        target = self._findTarget(self.target)
        if target is not None:
            return target.getCFGInfo()
        else:
            return None

    def getTargetTag(self):
        # Return the target tag
        target = self._findTarget(self.target)
        if target is not None:
            return target.getTargetTag()
        else:
            return None

    def find_static_phy_path(self, phy_name):
        cfg_location, store_config_output = self.getTargetCFGInfo()
        root_path = os.path.realpath("{}/../unit_tests/fixed_phy_xml/{}".format(FileUtilities.resource_path(os.path.dirname(__file__)), cfg_location))
        static_phy_name = "{}.xml".format(phy_name)
        static_phy_path = os.path.realpath(os.path.join(root_path, static_phy_name))
        return static_phy_path

    def check_phy_supported_on_target(self, phy_name, target_tag=None, model=None):

        if target_tag is None:
            #This can be retrieved automatically but there is a performance penalty
            target_tag = self.getTargetTag()

        blacklist_tag = "-"+target_tag
        target_blacklisted = self.check_phy_has_tag(phy_name,blacklist_tag,model)
        if target_blacklisted:
            return False
        else:
            return True

    def _findPhyGUID(self, phy_guid):
        """Finds phy for phy_guid, version, part_family, part_revision

        Args:
            phy_guid (str) : PHY GUID to find

        Returns:
            phy_function (function) : PHY function reference
        """
        phyFound = False
        phyList = self.__getPhyList()
        for phy in phyList:
            phy_functions = phy.getPhyList()
            for phy_function in phy_functions:
                if hasattr(phy_function, 'phy_guid') and (phy_guid == phy_function.phy_guid):
                    phyFound = True
                    break

            if phyFound:
                break

        if phyFound:
            return phy_function
        else:
            return None

    def findPHYNameFromGUID(self, phy_guid):
        phy_func = self._findPhyGUID(phy_guid)
        if phy_func is not None:
            phy_name = phy_func.__name__
        else:
            phy_name = None
        return phy_name

    def calc_config_phy_guid(self, phy_guid, output_path, optional_inputs=None, show_do_not_care=True):
        """Run the calculator and generate the calculated outputs for a single PHY guid

        Args:
            phy_guid (str) : PHY GUID to run through the calculator.
            output_path (str) : Full path to output file
            optional_inputs (dict) : Dictionary of input values.  Key is var_name and Value is var_value [Optional Input].

        Returns:
            model_instance (ModelRoot) : calculated data model instance
        """

        phy_name = self.findPHYNameFromGUID(phy_guid)
        if phy_name is not None:
            model_instance = self.calc_config(phy_name, output_path, optional_inputs, show_do_not_care)
        else:
            raise UnableToFindPHY('phy_guid: {} not found'.format(phy_guid))
        return model_instance

    def __override_profile_output(self, model_instance, key, value):
        # process option inputs into profile output overrides
        output = getattr(model_instance.profile.outputs, key)
        if output._var.var_type != Enum:
            output.override = (output._var.var_type)(value)
        else:
            if isinstance(value, basestring):
                if value.isdigit():
                    value = int(value)
                    enum_val = output._var.var_enum(value)
                else:
                    enum_val = getattr(output._var.var_enum, value)
            output.override = enum_val
