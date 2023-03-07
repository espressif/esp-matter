
"""
This module contains the ModelProfile class to define a profile with
input, forced, and output variables.
"""
from .. import model_type as model_type
from .. import model_instance as model_inst
from pycalcmodel.core.common import *
from pycalcmodel.core.input import ModelInputContainer, ModelInput, ModelInputType, ModelInputDefaultVisibilityType
from pycalcmodel.core.force import ModelForceContainer, ModelForce
from pycalcmodel.core.output import ModelOutputContainer, ModelOutput, ModelOutputType
from collections import OrderedDict
from pycalcmodel.core.parser import ModelParser
from pycalcmodel.core import default_phy
from pycalcmodel.core.variable import ModelVariableContainer
import sys

from pycalcmodel.py2_and_3_compatibility import *

__all__ = [ 'ModelProfileMissingVariable',
            'ModelProfile',
            'ModelProfileContainer',
            'ModelProfileTypeXml',
            'ModelProfileInstanceXml'
            ]

class ModelProfileMissingVariable(Exception):
    pass


class ModelProfile(object):

    def __init__(self, name, category, desc, default=False, readable_name=None, act_logic=''):
        self.name = name
        if readable_name is None:
            self.readable_name = name
        else:
            self.readable_name = readable_name
        self.category = category
        self.desc = desc
        self.default = default
        self.act_logic = act_logic
        self.inputs = ModelInputContainer()
        self.forces = ModelForceContainer()
        self.outputs = ModelOutputContainer()
        self.default_phys = default_phy.ModelDefaultPhyContainer()
        self.zz_eval_act_logic = None

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, value):
        assert isinstance(value, basestring)
        self._name = value

    @property
    def readable_name(self):
        return self._readable_name

    @readable_name.setter
    def readable_name(self, value):
        assert isinstance(value, basestring)
        self._readable_name = value

    @property
    def category(self):
        return self._category

    @category.setter
    def category(self, value):
        assert isinstance(value, basestring), "FATAL ERROR: value must be str"
        self._category = value

    @property
    def version(self):
        return self._version

    @version.setter
    def version(self, value):
        assert isinstance(value, basestring)
        self._version = value

    @property
    def desc(self):
        return self._desc

    @desc.setter
    def desc(self, value):
        assert isinstance(value, basestring)
        self._desc = value

    @property
    def default(self):
        return self._default

    @default.setter
    def default(self, value):
        assert isinstance(value, bool)
        self._default = value

    @property
    def act_logic(self):
        return self._act_logic

    @act_logic.setter
    def act_logic(self, value):
        assert isinstance(value, basestring)
        self._act_logic = value

    def validate(self, variables):
        assert isinstance(variables, ModelVariableContainer)
        for input in self.inputs:
            if input.var_name not in variables:
                sys.stderr.write("ERROR: Unable to locate input variable '{}'".format(input.var_name))
                return False
            object = getattr(self.inputs, input.var_name)
            if not isinstance(object, ModelInput):
                sys.stderr.write("ERROR: Invalid profile input variable '{}'".format(input.var_name))
                return False
        for force in self.forces:
            if force.var_name not in variables:
                sys.stderr.write("ERROR: Unable to locate force variable '{}'".format(force.var_name))
                return False
            object = getattr(self.forces, force.var_name)
            if not isinstance(object, ModelForce):
                sys.stderr.write("ERROR: Invalid profile force variable '{}'".format(force.var_name))
                return False
        for output in self.outputs:
            if output.var_name not in variables:
                sys.stderr.write("ERROR: Unable to locate output variable '{}'".format(output.var_name))
                return False
            object = getattr(self.outputs, output.var_name)
            if not isinstance(object, ModelOutput):
                sys.stderr.write("ERROR: Invalid profile output variable '{}'".format(output.var_name))
                return False
        return True

    def get_outputs(self, output_types=[]):
        if len(output_types) == 0:
            # return all
            for output in self.outputs:
                yield output
        else:
            # return only requested
            for output in self.outputs:
                for output_type in output_types:
                    if output.output_type == output_type:
                        yield output
                        break

    def register_parser(self, eval_act_logic_func):
        # called by the append() method in ModelProfileContainer
        assert callable(eval_act_logic_func)
        self.zz_eval_act_logic = eval_act_logic_func

    def is_active(self):
        if self.zz_eval_act_logic is None:
            raise AttributeError("Please append profile to model to enable evaluation.")
        else:
            return self.zz_eval_act_logic(self.act_logic)

    def to_type_xml(self, phy_group_incl_list):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        return model_type.profileType(name=self.name,
                                      readable_name=self.readable_name,
                                      category=self.category,
                                      desc=self.desc,
                                      default=self.default,
                                      act_logic=self.act_logic,
                                      inputs=self.inputs.to_type_xml(),
                                      forces=self.forces.to_type_xml(),
                                      outputs=self.outputs.to_type_xml(),
                                      default_phys=self.default_phys.to_type_xml(phy_group_incl_list))

    def to_instance_xml(self, phy_group_incl_list):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        return model_inst.profileType(name=self.name,
                                      category=self.category,
                                      readable_name=self.readable_name,
                                      desc=self.desc,
                                      default=self.default,
                                      act_logic=self.act_logic,
                                      inputs=self.inputs.to_instance_xml(),
                                      forces=self.forces.to_instance_xml(),
                                      outputs=self.outputs.to_instance_xml(),
                                      default_phys=self.default_phys.to_instance_xml(phy_group_incl_list))

    def __str__(self):
        out = '\n  Profile -  {}:\n'.format(self.name)
        out += '    Readable Name: {}\n'.format(self.readable_name)
        out += '    Category:  {}\n'.format(self.category)
        out += '    Desc:      {}\n'.format(self.desc)
        out += '    Act Logic: {}\n'.format(self.act_logic)
        out += '    Default:   {}\n'.format(self.default)
        out += '    Inputs:\n'
        for input in self.inputs:
            out += str(input)
        out += '    Forces:\n'
        for force in self.forces:
            out += str(force)
        out += '    Outputs:\n'
        for output in self.outputs:
            out += str(output)
        out += '    Default Phys:\n'
        for default_phy in self.default_phys:
            out += str(default_phy)
        return out


class ModelProfileContainer(object):

    def __init__(self, parser):
        assert isinstance(parser, ModelParser)
        self.zz_parser = parser
        self.ZZ_PROFILE_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_PROFILE_KEYS:
            yield self.ZZ_PROFILE_KEYS[key]

    def __contains__(self, name):
        return name in self.ZZ_PROFILE_KEYS

    def get_profile(self, profile_name):
        return self.ZZ_PROFILE_KEYS[profile_name]

    def get_active(self):
        active = []
        for profile in self:
            if profile.is_active():
                active.append(profile)
        return active

    def append(self, profile):
        assert isinstance(profile, ModelProfile), "FATAL ERROR: profile must be ModelProfile"
        self.ZZ_PROFILE_KEYS[profile.name] = profile
        profile.register_parser(self.zz_parser.eval_act_logic)
        setattr(self, profile.name, profile)

    def extend(self, profile_list):
        for profile in profile_list:
            self.append(profile)

    def validate(self, vars):
        assert isinstance(vars, ModelVariableContainer)
        for profile in self:
            if not profile.validate(vars):
                return False
        return True

    def to_type_xml(self, profile_incl_list, phy_group_incl_list):
        assert isinstance(profile_incl_list, (list, type(None)))
        assert isinstance(phy_group_incl_list, (list, type(None)))
        profiles = model_type.profilesType()
        for profile in self:
            if profile_incl_list:
                if profile.name in profile_incl_list:
                    profiles.add_profile(profile.to_type_xml(phy_group_incl_list))
            else:
                profiles.add_profile(profile.to_type_xml(phy_group_incl_list))
        return profiles

    def to_instance_xml(self, name, phy_group_incl_list):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        profiles = model_inst.profilesType()
        if name not in self:
            raise NameError("Profile '{}' does not exist".format(name))
        else:
            profiles.profile = self.get_profile(name).to_instance_xml(phy_group_incl_list)
        return profiles

    def clear(self):
        # clear attributes
        for key in self.ZZ_PROFILE_KEYS:
            if hasattr(self, key):
                delattr(self, key)
        # clear dictionary
        self.ZZ_PROFILE_KEYS.clear()


class ModelProfileTypeXml(ModelProfile):

    def __init__(self, vars, profile_root):
        assert isinstance(vars, ModelVariableContainer)
        assert isinstance(profile_root, model_type.profileType)
        super(ModelProfileTypeXml, self).__init__(profile_root.name,
                                                  profile_root.category,
                                                  profile_root.desc,
                                                  profile_root.default,
                                                  profile_root.readable_name,
                                                  profile_root.act_logic)
        for input_root in profile_root.get_inputs().get_input():
            var = vars.get_var(input_root.var_name)

            # backward compatibilit
            if input_root.default_visiblity is not None:
                default_visiblity = ModelInputDefaultVisibilityType[input_root.default_visiblity]
            else:
                default_visiblity = None

            self.inputs.append(ModelInput(var,
                                          input_root.category,
                                          ModelInputType[input_root.input_type],
                                          cast_value_from_xml(var, input_root.defaults.get_value()),
                                          readable_name=input_root.readable_name,
                                          value_limit_min=cast_value_from_xml(var, [input_root.value_limit_min]),
                                          value_limit_max=cast_value_from_xml(var, [input_root.value_limit_max]),
                                          fractional_digits=input_root.fractional_digits,
                                          deprecated=input_root.deprecated,
                                          default_visibility=default_visiblity,
                                          units_multiplier=input_root.units_multiplier
                                          ))

        for force_root in profile_root.get_forces().get_force():
            var = vars.get_var(force_root.var_name)
            self.forces.append(ModelForce(var,
                                          cast_value_from_xml(var, force_root.values.get_value())))
        for output_root in profile_root.get_outputs().get_output():
            var = vars.get_var(output_root.var_name)
            self.outputs.append(ModelOutput(var,
                                            output_root.category,
                                            ModelOutputType[output_root.output_type],
                                            readable_name=output_root.readable_name,
                                            value_limit_min=cast_value_from_xml(var, [output_root.value_limit_min]),
                                            value_limit_max=cast_value_from_xml(var, [output_root.value_limit_max])))


class ModelProfileInstanceXml(ModelProfile):

    def __init__(self, vars, profile_root):
        assert isinstance(vars, ModelVariableContainer)
        assert isinstance(profile_root, model_inst.profileType)
        super(ModelProfileInstanceXml, self).__init__(profile_root.name,
                                                      profile_root.category,
                                                      profile_root.desc,
                                                      profile_root.default,
                                                      profile_root.readable_name,
                                                      profile_root.act_logic)
        for input_root in profile_root.get_inputs().get_input():
            var = vars.get_var(input_root.var_name)
            if input_root.defaults is None:
                default = None
            else:
                default = cast_value_from_xml(var, input_root.defaults.get_value())

            # Handle backward compatibility
            if hasattr(input_root, 'default_visiblity'):
                if input_root.default_visiblity is not None:
                    default_visiblity = ModelInputDefaultVisibilityType[input_root.default_visiblity]
                else:
                    default_visiblity = None
            else:
                default_visiblity = None

            self.inputs.append(ModelInput(var,
                                          input_root.category,
                                          ModelInputType[input_root.input_type],
                                          default,
                                          readable_name=input_root.readable_name,
                                          var_value=cast_value_from_xml(var, input_root.var_values.get_value()),
                                          value_limit_min=cast_value_from_xml(var, [input_root.value_limit_min]),
                                          value_limit_max=cast_value_from_xml(var, [input_root.value_limit_max]),
                                          fractional_digits=input_root.fractional_digits,
                                          deprecated=input_root.deprecated,
                                          default_visibility=default_visiblity,
                                          units_multiplier=input_root.units_multiplier
                                          ))
        for force_root in profile_root.get_forces().get_force():
            var = vars.get_var(force_root.var_name)
            self.forces.append(ModelForce(var,
                                          cast_value_from_xml(var, force_root.values.get_value())))
        for output_root in profile_root.get_outputs().get_output():
            var = vars.get_var(output_root.var_name)
            if output_root.var_overrides is None:
                override = None
            else:
                override = cast_value_from_xml(var, output_root.var_overrides.get_value())
            # note that var_values is handled in the overall variables assignment, as it is
            # only a read property to the var.value
            self.outputs.append(ModelOutput(var,
                                            output_root.category,
                                            ModelOutputType[output_root.output_type],
                                            readable_name=output_root.readable_name,
                                            value_limit_min=cast_value_from_xml(var, [output_root.value_limit_min]),
                                            value_limit_max=cast_value_from_xml(var, [output_root.value_limit_max]),
                                            override=override,
                                            fractional_digits=output_root.fractional_digits))
