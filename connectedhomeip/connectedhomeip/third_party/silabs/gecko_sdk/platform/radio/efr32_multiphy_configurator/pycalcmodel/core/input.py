
"""
This module contains the ModelInput class to define inputs for a
profile definition.
"""

from .. import model_type as model_type
from .. import model_instance as model_inst
from pycalcmodel.core.common import *
from pycalcmodel.core.variable import ModelVariable
from collections import OrderedDict
from enum import Enum, unique

from pycalcmodel.py2_and_3_compatibility import *

__all__ = [ 'ModelInput', 'ModelInputContainer', 'ModelInputType',
            'ModelInputValue', 'ModelInputValueContainer', 'ModelInputDefaultVisibilityType' ]


@unique
class ModelInputType(Enum):
    #: This is a required variable that has to be set before a calculation can be run.
    REQUIRED = 0
    #: This is an optional value. If not specified, a default is used. Depending on a key required input,
    #:   this input may even be ignored.
    OPTIONAL = 1
    #: A purely informational input, useful in description or logging output.
    INFO = 2
    #: An optional input automatically calculated and shown in GUI, use checkbox to explicitly set,
    #:   show "auto" in the absence of a calculated value.
    LINKED_IO = 3

class ModelInputDefaultVisibilityType(Enum):
    #: This profile input is visible by default, in a GUI
    VISIBLE = 0
    #: This profile input is hidden by default, in a GUI
    HIDDEN = 1


class ModelInput(object):

    def __init__(self, var, category, input_type=ModelInputType.REQUIRED, default=None, readable_name=None, var_value=None,
                 value_limit_min=None, value_limit_max=None, fractional_digits=None, deprecated=False, default_visibility=ModelInputDefaultVisibilityType.VISIBLE,
                 units_multiplier=None):
        assert isinstance(var, ModelVariable), "FATAL ERROR: var is not ModelVariable"
        self._var = var
        if readable_name is None:
            self.readable_name = self._var.name
        else:
            self.readable_name = readable_name
        self.category = category
        self.input_type = input_type
        self.default = default
        #if var_value is None:
        #    if self.default is None:
        #        # ensure var_value is always present so it can be assigned
        #        self.var_value = None # get_dummy_var_value(self._var)
        #    else:
        #        self.var_value = self.default
        #else:
        #    self.var_value = var_value
        self.var_value = var_value
        #: The minimum limit of the value (inclusive)
        self.value_limit_min = value_limit_min
        #: The maximum limit of the value (inclusive)
        self.value_limit_max = value_limit_max
        self.fractional_digits = fractional_digits

        # backward compatibility
        if deprecated is None:
            deprecated = False
        self.deprecated = deprecated

        # backward compatibility
        if default_visibility is None:
           default_visibility = ModelInputDefaultVisibilityType.VISIBLE
        self.default_visibility = default_visibility

        #: The units multiplier string for the variable
        self._units_multiplier = None
        if units_multiplier is not None:
            self.units_multiplier = units_multiplier

    @property
    def var_name(self):
        return self._var.name

    @property
    def readable_name(self):
        return self._readable_name

    @readable_name.setter
    def readable_name(self, value):
        assert isinstance(value, basestring), "FATAL ERROR: value must be str"
        self._readable_name = value

    @property
    def category(self):
        return self._category

    @category.setter
    def category(self, value):
        assert isinstance(value, basestring), "FATAL ERROR: value must be str"
        self._category = value

    @property
    def is_array(self):
        return self._var.is_array

    @property
    def input_type(self):
        return self._input_type

    @input_type.setter
    def input_type(self, value):
        assert isinstance(value, ModelInputType), "FATAL ERROR: value must be ModelInputType enum"
        self._input_type = value

    @property
    def default(self):
        return self._default

    @default.setter
    def default(self, value):
        self._var.validate_type(value)
        self._default = value

    @property
    def var_value(self):
        return self._var_value

    @var_value.setter
    def var_value(self, value):
        self._var.validate_type(value)
        self._var_value = value

    @property
    def value_limit_min(self):
        return self._value_limit_min

    @value_limit_min.setter
    def value_limit_min(self, value):
        self._var.validate_type(value)
        self._value_limit_min = value

    @property
    def value_limit_max(self):
        return self._value_limit_max

    @value_limit_max.setter
    def value_limit_max(self, value):
        self._var.validate_type(value)
        self._value_limit_max = value

    def validate_limits(self, value):
        # test optional max limit
        if self._value_limit_max is not None:
            if value > self._value_limit_max:
                return False
        # test optional min limit
        if self._value_limit_min is not None:
            if value < self._value_limit_min:
                return False
        return True

    @property
    def fractional_digits(self):
        return self._fractional_digits

    @fractional_digits.setter
    def fractional_digits(self, value):
        assert isinstance(value, (int, type(None))), "FATAL ERROR: Invalid fractional_digits type"
        self._fractional_digits = value

    @property
    def deprecated(self):
        return self._deprecated

    @deprecated.setter
    def deprecated(self, value):
        assert isinstance(value, (bool)), "FATAL ERROR: Invalid deprecated value"
        self._deprecated = value

    @property
    def default_visibility(self):
        return self._default_visibility

    @default_visibility.setter
    def default_visibility(self, value):
        assert isinstance(value, ModelInputDefaultVisibilityType), "FATAL ERROR: value must be ModelInputDefaultVisibilityType enum"
        self._default_visibility = value

    @property
    def units_multiplier(self):
        return self._units_multiplier

    @units_multiplier.setter
    def units_multiplier(self, value):
        assert isinstance(value, basestring)
        self._units_multiplier = value

    def to_type_xml(self):
        defaults_obj = model_type.defaults(get_xml_str_values(self.is_array, self.default))
        return model_type.inputType1(var_name=self.var_name,
                                     readable_name=self.readable_name,
                                     category=self.category,
                                     is_array=self.is_array,
                                     input_type=self.input_type.name,
                                     defaults=defaults_obj,
                                     value_limit_min=self.value_limit_min,
                                     value_limit_max=self.value_limit_max,
                                     fractional_digits=self.fractional_digits,
                                     deprecated=self.deprecated,
                                     default_visiblity=self.default_visibility.name,
                                     units_multiplier=self.units_multiplier)

    def to_instance_xml(self):
        var_values_obj = model_inst.var_values(get_xml_str_values(self.is_array, self.var_value))
        defaults_obj = model_inst.defaults(get_xml_str_values(self.is_array, self.default))
        return model_inst.inputType1(var_name=self.var_name,
                                     readable_name=self.readable_name,
                                     category=self.category,
                                     is_array=self.is_array,
                                     input_type=self.input_type.name,
                                     defaults=defaults_obj,
                                     var_values=var_values_obj,
                                     value_limit_min=self.value_limit_min,
                                     value_limit_max=self.value_limit_max,
                                     fractional_digits=self.fractional_digits,
                                     deprecated=self.deprecated,
                                     default_visiblity=self.default_visibility.name,
                                     units_multiplier=self.units_multiplier)

    def __str__(self):
        out = '        Input:\n'
        out += '            readable_name: {}\n'.format(self.readable_name)
        out += '            category:      {}\n'.format(self.category)
        out += '            var_name:      {}\n'.format(self.var_name)
        out += '            var_value:     {}\n'.format(self.var_value)
        out += '            is_array:      {}\n'.format(self.is_array)
        out += '            input_type:    {}\n'.format(self.input_type.name)
        out += '            default:       {}\n'.format(self.default)
        if self.value_limit_min is not None:
            out += '            value_limit_min:  {}\n'.format(self.value_limit_min)
        if self.value_limit_max is not None:
            out += '            value_limit_max:  {}\n'.format(self.value_limit_max)
        if self.fractional_digits is not None:
            out += '            fractional_digits: {}\n'.format(self.fractional_digits)
        out += '            deprecated:       {}\n'.format(self.deprecated)
        out += '            default_visiblity:    {}\n'.format(self.default_visibility.name)
        if self.units_multiplier is not None:
            out += '    Units Multiplier:      {}\n'.format(self.units_multiplier)
        return out


class ModelInputContainer(object):

    def __init__(self):
        self.ZZ_INPUT_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_INPUT_KEYS.copy():
            yield self.ZZ_INPUT_KEYS[key]

    def __contains__(self, var_name):
        return var_name in self.ZZ_INPUT_KEYS

    def get_input(self, var_name):
        return self.ZZ_INPUT_KEYS[var_name]

    def append(self, input):
        assert isinstance(input, ModelInput), \
            "FATAL ERROR: input must be ModelInput"
        if input.var_name in self:
            raise NameError("'{}' is already defined!".format(input.var_name))
        self.ZZ_INPUT_KEYS[input.var_name] = input
        setattr(self, input.var_name, input)

    def extend(self, input_list):
        for input in input_list:
            self.append(input)

    def to_type_xml(self):
        inputs = model_type.inputsType()
        for input in self:
            inputs.add_input(input.to_type_xml())
        return inputs

    def to_instance_xml(self):
        inputs = model_inst.inputsType()
        for input in self:
            inputs.add_input(input.to_instance_xml())
        return inputs

    def remove_input(self, var_name):
        # Remove attribute
        if hasattr(self, var_name):
            delattr(self, var_name)

        #Remove from dictionary
        if var_name in self.ZZ_INPUT_KEYS:
            force = self.ZZ_INPUT_KEYS.pop(var_name)
            force = None # Tag for garbage collector

class ModelInputValue(ModelInput):

    def __init__(self, profile_input):
        assert isinstance(profile_input, ModelInput)
        super(ModelInputValue, self).__init__(
            profile_input._var,
            profile_input.category,
            profile_input.input_type,
            profile_input.default,
            profile_input.readable_name,
            profile_input.var_value,
            profile_input.value_limit_min,
            profile_input.value_limit_max,
            profile_input.fractional_digits,
            profile_input.deprecated,
            profile_input.default_visibility,
            profile_input.units_multiplier)
        self.value = profile_input.default

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        self._var.validate_type(value)
        self._value = value

    def to_type_xml(self):
        values_obj = model_type.values(get_xml_str_values(self.is_array, self.value))
        return model_type.profile_inputType(var_name=self.var_name,
                                            readable_name=self.readable_name,
                                            category=self.category,
                                            is_array=self.is_array,
                                            values=values_obj)

    def to_instance_xml(self):
        values_obj = model_inst.values(get_xml_str_values(self.is_array, self.value))
        return model_inst.profile_inputType(var_name=self.var_name,
                                            readable_name=self.readable_name,
                                            category=self.category,
                                            is_array=self.is_array,
                                            values=values_obj)
    def __str__(self):
        out = '        Profile Input:\n'
        out += '            var_name:      {}\n'.format(self.var_name)
        out += '            readable_name: {}\n'.format(self.readable_name)
        out += '            category:      {}\n'.format(self.category)
        out += '            is_array:      {}\n'.format(self.is_array)
        out += '            value:         {}\n'.format(self.value)
        return out


class ModelInputValueContainer(object):

    def __init__(self):
        self.ZZ_INPUT_VAL_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_INPUT_VAL_KEYS:
            yield self.ZZ_INPUT_VAL_KEYS[key]

    def __contains__(self, var_name):
        return var_name in self.ZZ_INPUT_VAL_KEYS

    def get_input_value(self, var_name):
        return self.ZZ_INPUT_VAL_KEYS[var_name]

    def append(self, input):
        assert isinstance(input, ModelInputValue), \
            "FATAL ERROR: input must be ModelInputValue"
        if input.var_name in self:
            raise NameError("'{}' is already defined!".format(input.var_name))
        self.ZZ_INPUT_VAL_KEYS[input.var_name] = input
        setattr(self, input.var_name, input)

    def extend(self, input_list):
        for input in input_list:
            self.append(input)

    def to_type_xml(self, exclude_none_values=False):
        inputs = model_type.profile_inputsType()
        for input in self:
            if exclude_none_values and input.value is None:
                pass
            else:
                inputs.add_profile_input(input.to_type_xml())
        return inputs

    def to_instance_xml(self):
        inputs = model_inst.profile_inputsType()
        for input in self:
            inputs.add_profile_input(input.to_instance_xml())
        return inputs