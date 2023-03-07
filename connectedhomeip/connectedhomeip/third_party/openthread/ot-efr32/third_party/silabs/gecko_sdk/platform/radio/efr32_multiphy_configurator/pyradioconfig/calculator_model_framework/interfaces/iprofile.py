from abc import ABCMeta, abstractmethod
from pycalcmodel.core.profile import ModelProfile
from pycalcmodel.core.variable import ModelVariable
from pycalcmodel.core.input import ModelInput, ModelInputType, ModelInputDefaultVisibilityType
from pycalcmodel.core.output import ModelOutput, ModelOutputType

from py_2_and_3_compatibility import *

"""
Profile interface file
"""
class IProfile(object):

    _profileName = ""
    _category = ""
    _description = ""
    _default = False
    _readable_name = ""
    _activation_logic = ""

    """
    Returns profile readable and searchable nmae
    """
    @abstractmethod
    def getName(self):
        # Since this is used in code to reference the profile, it cannot have white space or spaces
        name = self._profileName.strip()
        name = name.replace(" ", "_")
        return name

    """
    Builds inputs, forced, outputs into modem model
    """
    @abstractmethod
    def buildProfileModel(self, modem_model):
        raise NotImplementedError('Call to abstract method getName()')

    """
    Builds empty profile model
    Returns: Populated profile model
    """
    @abstractmethod
    def _makeProfile(self, modem_model, category=None, description=None,
                     default=None, readable_name=None, activation_logic=None):
        # Build profile from name, category, and description
        if category is None:
            category = self._category
        if description is None:
            description = self._description
        if default is None:
            default = self._default
        if readable_name is None:
            readable_name = self._readable_name
        if activation_logic is None:
            activation_logic = self._activation_logic
        profile_model = ModelProfile(self.getName(), category, description,
                                     default=default, readable_name=readable_name, act_logic=activation_logic)
        modem_model.profiles.append(profile_model)
        return profile_model

    """
    Moves a variable from input group and inputs forces
    """
    def _removeVariableFromInputs(self, profile, varInput, value=None):
        assert isinstance(varInput, ModelVariable), "FATAL ERROR: var is not ModelVariable"
        # Remove from inputs
        # Loop through aliases and find the right var name
        for input in profile.inputs:
            if input.var_name == varInput.name:
                input_alias = input.var_name
                profile.inputs.remove_input(input_alias)
                break

    """
    Helper function to create lined input/output types
    """
    @staticmethod
    def make_linked_io(profile, var, category=None, readable_name=None,
                 value_limit_min=None, value_limit_max=None, fractional_digits=None, deprecated=False,
                 units_multiplier=None):

        if category is None:
            raise Exception("Category not defined for profile input %s" % var._name)

        if readable_name is None:
            raise Exception("Readable name not defined for profile input %s" % var._name)

        if var.var_type == int or var.var_type == long or var.var_type == int:
            if value_limit_min is None or value_limit_max is None:
                raise Exception("Min/max values not defined for profile input %s" % var._name)

        # Make input
        profile_input = ModelInput(var, category, ModelInputType.LINKED_IO, default=None,
                        readable_name=readable_name, var_value=None,
                        value_limit_min=value_limit_min, value_limit_max=value_limit_max,
                        fractional_digits=fractional_digits, deprecated=deprecated,
                        default_visibility=ModelInputDefaultVisibilityType.VISIBLE,
                        units_multiplier=units_multiplier)
        profile.inputs.append(profile_input)

        # Make output
        output = ModelOutput(var, category, ModelOutputType.LINKED_IO, readable_name,
                             value_limit_min, value_limit_max, override=None)
        profile.outputs.append(output)


    @staticmethod
    def make_hidden_input(profile, var, category=None, readable_name=None,
                          value_limit_min=None, value_limit_max=None,
                          fractional_digits = None, units_multiplier = None):

        if category is None:
            raise Exception("Category not defined for profile input %s" % var._name)

        if readable_name is None:
            raise Exception("Readable name not defined for profile input %s" % var._name)

        if var.var_type == int or var.var_type == long or var.var_type == int:
            if value_limit_min is None or value_limit_max is None:
                raise Exception("Min/max values not defined for profile input %s" % var._name)

        profile_input = ModelInput(var, category=category, input_type=ModelInputType.OPTIONAL, default=None,
                        readable_name=readable_name, var_value=None,
                        value_limit_min=value_limit_min, value_limit_max=value_limit_max,
                        fractional_digits=fractional_digits, deprecated=False,
                        default_visibility=ModelInputDefaultVisibilityType.HIDDEN,
                        units_multiplier=units_multiplier)
        profile.inputs.append(profile_input)

    @staticmethod
    def make_required_input(profile, var, category=None, readable_name=None,
                        value_limit_min=None, value_limit_max=None,
                        fractional_digits=None, units_multiplier=None):

        if category is None:
            raise Exception("Category not defined for profile input %s" % var._name)

        if readable_name is None:
            raise Exception("Readable name not defined for profile input %s" % var._name)

        if var.var_type == int or var.var_type == long or var.var_type == int:
            if value_limit_min is None or value_limit_max is None:
                raise Exception("Min/max values not defined for profile input %s" % var._name)

        profile_input = ModelInput(var, category=category, input_type=ModelInputType.REQUIRED, default=None,
                        readable_name = readable_name, var_value = None,
                        value_limit_min = value_limit_min, value_limit_max = value_limit_max,
                        fractional_digits = fractional_digits, deprecated = False,
                        default_visibility=ModelInputDefaultVisibilityType.VISIBLE,
                        units_multiplier = units_multiplier)
        profile.inputs.append(profile_input)

    #
    # From our perspective, an input is considered optional if some isc files may not have that input and we need
    # to specify a default value that will be used if that input does not exist.  This happens when an input is added
    # to a profile after it has been defined and is out there in use.
    #
    @staticmethod
    def make_optional_input(profile, var, category=None, readable_name=None, default=None,
                        value_limit_min=None, value_limit_max=None,
                        fractional_digits=None, units_multiplier=None):

        if category is None:
            raise Exception("Category not defined for profile input %s" % var._name)

        if readable_name is None:
            raise Exception("Readable name not defined for profile input %s" % var._name)

        if default is None:
            raise Exception("Optional input defined with no default value for profile input %s" % var._name)

        if var.var_type == int or var.var_type == long or var.var_type == int:
            if value_limit_min is None or value_limit_max is None:
                raise Exception("Min/max values not defined for profile input %s" % var._name)

        profile_input = ModelInput(var, category=category, input_type=ModelInputType.REQUIRED, default=default,
                        readable_name = readable_name, var_value = None,
                        value_limit_min = value_limit_min, value_limit_max = value_limit_max,
                        fractional_digits = fractional_digits, deprecated = False,
                        default_visibility=ModelInputDefaultVisibilityType.VISIBLE,
                        units_multiplier = units_multiplier)
        profile.inputs.append(profile_input)


    @staticmethod
    def make_deprecated_input(profile, var, category=None, readable_name=None, default=None,
                        value_limit_min=None, value_limit_max=None,
                        fractional_digits=None, units_multiplier=None):

        #We don't really need a category for deprecated inputs, as they will not show in Studio GUI
        if category is None:
            category = 'None'

        profile_input = ModelInput(var, category=category, input_type=ModelInputType.LINKED_IO, default=default,
                        readable_name = readable_name, var_value = None,
                        value_limit_min = value_limit_min, value_limit_max = value_limit_max,
                        fractional_digits = fractional_digits, deprecated = True,
                        default_visibility=ModelInputDefaultVisibilityType.HIDDEN,
                        units_multiplier = units_multiplier)
        profile.inputs.append(profile_input)



