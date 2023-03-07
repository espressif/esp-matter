
"""
This module contains the ModelForce class to define forces for a
profile definition.
"""

from .. import model_type as model_type
from .. import model_instance as model_inst
from pycalcmodel.core.common import *
from pycalcmodel.core.variable import ModelVariable
from collections import OrderedDict

__all__ = [ 'ModelForce', 'ModelForceContainer' ]


class ModelForce(object):

    def __init__(self, var, value):
        assert isinstance(var, ModelVariable), "FATAL ERROR: var is not ModelVariable"
        self._var = var
        self.value = value

    @property
    def var_name(self):
        return self._var.name

    @property
    def is_array(self):
        return self._var.is_array

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        self._var.validate_type(value)
        self._value = value

    def to_type_xml(self):
        value_obj = model_type.values(get_xml_str_values(self.is_array, self.value))
        return model_type.forceType(var_name=self.var_name,
                                    is_array=self.is_array,
                                    values=value_obj)

    def to_instance_xml(self):
        value_obj = model_inst.values(get_xml_str_values(self.is_array, self.value))
        return model_inst.forceType(var_name=self.var_name,
                                    is_array=self.is_array,
                                    values=value_obj)

    def __str__(self):
        out = '        Force:\n'
        out += '            var_name: {}\n'.format(self.var_name)
        out += '            is_array: {}\n'.format(self.is_array)
        out += '            value:  {}\n'.format(self.value)
        return out


class ModelForceContainer(object):

    def __init__(self):
        self.ZZ_FORCE_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_FORCE_KEYS:
            yield self.ZZ_FORCE_KEYS[key]

    def __contains__(self, var_name):
        return var_name in self.ZZ_FORCE_KEYS

    def append(self, force):
        assert isinstance(force, ModelForce), \
            "FATAL ERROR: force must be ModelForce"
        self.ZZ_FORCE_KEYS[force.var_name] = force
        setattr(self, force.var_name, force)

    def extend(self, force_list):
        for force in force_list:
            self.append(force)

    def to_type_xml(self):
        forces = model_type.forcesType()
        for force in self:
            forces.add_force(force.to_type_xml())
        return forces

    def to_instance_xml(self):
        forces = model_inst.forcesType()
        for force in self:
            forces.add_force(force.to_instance_xml())
        return forces

    def remove_force(self, var_name):
        # Remove attribute
        if hasattr(self, var_name):
            delattr(self, var_name)

        #Remove from dictionary
        if var_name in self.ZZ_FORCE_KEYS:
            force = self.ZZ_FORCE_KEYS.pop(var_name)
            force = None # Tag for garbage collector
