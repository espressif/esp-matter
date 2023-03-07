
"""
This module contains the ModelOutput class to define outputs for a
calculation flow.
"""

from .. import model_type as model_type
from .. import model_instance as model_inst
from pycalcmodel.core.common import *
from pycalcmodel.core.variable import ModelVariable, ModelVariableEmptyValue
from collections import OrderedDict
from enum import Enum, unique

from pycalcmodel.py2_and_3_compatibility import *

__all__ = [ 'ModelOutput', 'ModelOutputContainer', 'ModelOutputType',
            'ModelOutputOverride', 'ModelOutputOverrideContainer']


@unique
class ModelOutputType(Enum):
    #: This output maps to a CMSIS SVD register field.
    SVD_REG_FIELD = 0
    #: This output maps to a virtual memory-mapped register field in the sequencer RAM, which is accessed
    #:   via a virtual SEQ peripheral.
    SEQ_REG_FIELD = 1
    #: A purely informational output that is not directly used in the part configuration.
    INFO = 2
    #: This output is used in the RAIL configuration structure.
    RAIL_CONFIG = 3
    #: This output translates to a firmware configuration function, where the variable name is
    #:   the function name and the variable value is the single parameter passed into the function.
    CONFIG_FUNCTION = 4
    #: This output is automatically calculated and shown in GUI, use checkbox to explicitly set,
    #:   ensures this output is tied to the input in a single control in the GUI.
    LINKED_IO = 5
    #: This output is used as an input to the DUT as a variable (e.g. not a register).
    SW_VAR = 6


class ModelOutput(object):
    def __init__(self, var, category, output_type, readable_name=None,
                 value_limit_min=None, value_limit_max=None, override=None,
                 fractional_digits=None):
        assert isinstance(var, ModelVariable), "FATAL ERROR: var is not ModelVariable"
        self._var = var
        if readable_name is None:
            self.readable_name = self._var.name
        else:
            self.readable_name = readable_name
        self.category = category
        self.output_type = output_type
        self.override = override
        #: The minimum limit of the value (inclusive)
        self.value_limit_min = value_limit_min
        #: The maximum limit of the value (inclusive)
        self.value_limit_max = value_limit_max
        self.fractional_digits = fractional_digits

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
    def output_type(self):
        return self._output_type

    @output_type.setter
    def output_type(self, value):
        assert isinstance(value, ModelOutputType), "FATAL ERROR: value must be ModelOutputType enum"
        self._output_type = value

    @property
    def is_array(self):
        return self._var.is_array

    @property
    def var_value(self):
        return self._var.value

    @property
    def override(self):
        return self._override

    @override.setter
    def override(self, value):
        self._var.validate_type(value)
        self._override = value

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

    @property
    def fractional_digits(self):
        return self._fractional_digits

    @fractional_digits.setter
    def fractional_digits(self, value):
        assert isinstance(value, (int, type(None))), "FATAL ERROR: Invalid fractional_digits type"
        self._fractional_digits = value

    @property
    def var(self):
        return self._var

    def to_type_xml(self):
        return model_type.outputType2(var_name=self.var_name,
                                      readable_name=self.readable_name,
                                      category=self.category,
                                      is_array=self.is_array,
                                      value_limit_min=self.value_limit_min,
                                      value_limit_max=self.value_limit_max,
                                      output_type=self.output_type.name,
                                      fractional_digits=self.fractional_digits)

    def to_instance_xml(self):
        try:
            var_values_obj = model_inst.var_values(get_xml_str_values(self.is_array, self.var_value))
        except ModelVariableEmptyValue as err:
            var_values_obj = model_inst.var_values(None)
        try:
            var_overrides_obj = model_inst.var_overrides(get_xml_str_values(self.is_array, self.override))
        except ModelVariableEmptyValue as err:
            var_overrides_obj = model_inst.var_overrides(None)
        return model_inst.outputType2(var_name=self.var_name,
                                      readable_name=self.readable_name,
                                      category=self.category,
                                      is_array=self.is_array,
                                      value_limit_min=self.value_limit_min,
                                      value_limit_max=self.value_limit_max,
                                      output_type=self.output_type.name,
                                      var_values=var_values_obj,
                                      var_overrides=var_overrides_obj,
                                      fractional_digits=self.fractional_digits)

    def __str__(self):
        out = '        Output:\n'
        out += '            readable_name: {}\n'.format(self.readable_name)
        out += '            category:      {}\n'.format(self.category)
        out += '            var_name:      {}\n'.format(self.var_name)
        try:
            out += '            var_value:     {}\n'.format(self.var_value)
        except ModelVariableEmptyValue:
            out += '            var_value:     {}\n'.format(None)
        out += '            var_overrides: {}\n'.format(self.override)
        out += '            is_array:      {}\n'.format(self.is_array)
        out += '            output_type:   {}\n'.format(self.output_type.name)
        if self.value_limit_min is not None:
            out += '            value_limit_min:  {}\n'.format(self.value_limit_min)
        if self.value_limit_max is not None:
            out += '            value_limit_max:  {}\n'.format(self.value_limit_max)
        if self.fractional_digits is not None:
            out += '            fractional_digits: {}\n'.format(self.fractional_digits)
        return out


class ModelOutputContainer(object):

    def __init__(self):
        self.ZZ_OUTPUT_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_OUTPUT_KEYS:
            yield self.ZZ_OUTPUT_KEYS[key]

    def __contains__(self, var_name):
        return var_name in self.ZZ_OUTPUT_KEYS

    def get_output(self, var_name):
        return self.ZZ_OUTPUT_KEYS[var_name]

    def append(self, output):
        assert isinstance(output, ModelOutput), \
            "FATAL ERROR: output must be ModelOutput"
        if output.var_name in self:
            raise NameError("'{}' is already defined!".format(output.var_name))
        self.ZZ_OUTPUT_KEYS[output.var_name] = output
        setattr(self, output.var_name, output)

    def extend(self, output_list):
        for output in output_list:
            self.append(output)

    def to_type_xml(self):
        outputs = model_type.outputsType()
        for output in self:
            outputs.add_output(output.to_type_xml())
        return outputs

    def to_instance_xml(self):
        outputs = model_inst.outputsType()
        for output in self:
            outputs.add_output(output.to_instance_xml())
        return outputs


class ModelOutputOverride(ModelOutput):

    def __init__(self, profile_output):
        assert isinstance(profile_output, ModelOutput)
        super(ModelOutputOverride, self).__init__(
            profile_output._var,
            profile_output.category,
            profile_output.output_type,
            profile_output.readable_name,
            profile_output.value_limit_min,
            profile_output.value_limit_max,
            profile_output.override)

    def to_type_xml(self):
        if self.override is None:
            return None
        else:
            overrides_obj = model_type.overrides(get_xml_str_values(self.is_array, self.override))
            return model_type.profile_outputType(var_name=self.var_name,
                                                 readable_name=self.readable_name,
                                                 category=self.category,
                                                 is_array=self.is_array,
                                                 overrides=overrides_obj)

    def to_instance_xml(self):
        if self.override is None:
            return None
        else:
            overrides_obj = model_inst.overrides(get_xml_str_values(self.is_array, self.override))
            return model_inst.profile_outputType(var_name=self.var_name,
                                                 readable_name=self.readable_name,
                                                 category=self.category,
                                                 is_array=self.is_array,
                                                 overrides=overrides_obj)

    def __str__(self):
        out = '        Profile Output:\n'
        out += '            var_name:      {}\n'.format(self.var_name)
        out += '            readable_name: {}\n'.format(self.readable_name)
        out += '            category:      {}\n'.format(self.category)
        out += '            is_array:      {}\n'.format(self.is_array)
        out += '            override:      {}\n'.format(self.override)
        return out


class ModelOutputOverrideContainer(object):

    def __init__(self):
        self.ZZ_OUTPUT_OVR_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_OUTPUT_OVR_KEYS:
            yield self.ZZ_OUTPUT_OVR_KEYS[key]

    def __contains__(self, var_name):
        return var_name in self.ZZ_OUTPUT_OVR_KEYS

    def get_output_override(self, var_name):
        return self.ZZ_OUTPUT_OVR_KEYS[var_name]

    def append(self, output):
        assert isinstance(output, ModelOutputOverride), \
            "FATAL ERROR: output must be ModelOutputOverride"
        if output.var_name in self:
            raise NameError("'{}' is already defined!".format(output.var_name))
        self.ZZ_OUTPUT_OVR_KEYS[output.var_name] = output
        setattr(self, output.var_name, output)

    def extend(self, output_list):
        for output in output_list:
            self.append(output)

    def to_type_xml(self):
        outputs = model_type.profile_outputsType()
        for output in self:
            out = output.to_type_xml()
            if out is not None:
                outputs.add_profile_output(out)
        return outputs

    def to_instance_xml(self):
        outputs = model_inst.profile_outputsType()
        for output in self:
            out = output.to_instance_xml()
            if out is not None:
                outputs.add_profile_output(out)
        return outputs
