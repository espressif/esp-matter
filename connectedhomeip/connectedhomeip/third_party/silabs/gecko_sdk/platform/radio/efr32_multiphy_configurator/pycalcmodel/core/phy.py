
"""
This module contains the ModelPhy class to define a phy with
phy versions.
"""
from .. import model_type as model_type
from .. import model_instance as model_inst
from pycalcmodel.core.common import *
from collections import OrderedDict
from pycalcmodel.core.profile import ModelProfile, ModelProfileContainer
from pycalcmodel.core.variable import ModelVariableContainer
from pycalcmodel.core.input import ModelInputValueContainer, ModelInputValue
from pycalcmodel.core.output import ModelOutputOverrideContainer, ModelOutputOverride
from pycalcmodel.core.parser import ModelParser
import sys

from pycalcmodel.py2_and_3_compatibility import *


__all__ = [ 'ModelPhy', 'ModelPhyContainer', 'ModelPhyTypeXml', 'ModelPhyInstanceXml' ]


class ModelPhy(object):

    def __init__(self, name, desc, profile, group_name, readable_name=None, act_logic='', tags='', phy_points_to=None, locked=False, guid=None):
        self.name = name
        if readable_name is None:
            self.readable_name = name
        else:
            self.readable_name = readable_name
        self.desc = desc
        self.phy_points_to = phy_points_to
        assert isinstance(profile, ModelProfile), "FATAL ERROR: profile must be ModelProfile"
        self._profile = profile
        self.group_name = group_name
        self.act_logic = act_logic
        self.zz_eval_act_logic = None
        self.profile_inputs = ModelInputValueContainer()
        for profile_input in self._profile.inputs:
            self.profile_inputs.append(ModelInputValue(profile_input))
        for profile_input in self.profile_inputs:
            profile_input.value = None
        self.profile_outputs = ModelOutputOverrideContainer()
        for profile_output in self._profile.outputs:
            self.profile_outputs.append(ModelOutputOverride(profile_output))

        self.locked = locked

        # backward compatibility
        if tags is None:
            tags = ''
        self.tags = tags

        self.guid = guid

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
    def desc(self):
        return self._desc

    @desc.setter
    def desc(self, value):
        assert isinstance(value, basestring)
        self._desc = value

    @property
    def profile_name(self):
        return self._profile.name

    @property
    def profile_version(self):
        return self._profile.version

    @property
    def group_name(self):
        return self._group_name

    @group_name.setter
    def group_name(self, value):
        assert isinstance(value, basestring)
        self._group_name = value

    @property
    def act_logic(self):
        return self._act_logic

    @act_logic.setter
    def act_logic(self, value):
        assert isinstance(value, basestring)
        self._act_logic = value

    @property
    def tags(self):
        return self._tags

    @tags.setter
    def tags(self, value):
        assert isinstance(value, basestring)
        self._tags = value

    @property
    def locked(self):
        return self._locked

    @locked.setter
    def locked(self, value):
        assert isinstance(value, bool)
        self._locked = value

    @property
    def guid(self):
        return self._guid

    @guid.setter
    def guid(self, value):
        if value is not None:
            assert isinstance(value, basestring)
        self._guid = value

    def register_parser(self, eval_act_logic_func):
        # called by the append() method in ModelPhyContainer
        assert callable(eval_act_logic_func)
        self.zz_eval_act_logic = eval_act_logic_func

    def is_active(self):
        if self.zz_eval_act_logic is None:
            raise AttributeError("Please append phy to model to enable evaluation.")
        else:
            return self.zz_eval_act_logic(self.act_logic)

    def to_type_xml(self):
        return model_type.phyType(name=self.name,
                                  desc=self.desc,
                                  act_logic=self.act_logic,
                                  group_name=self.group_name,
                                  readable_name=self.readable_name,
                                  profile_name=self.profile_name,
                                  profile_inputs=self.profile_inputs.to_type_xml(exclude_none_values=True),
                                  profile_outputs=self.profile_outputs.to_type_xml(),
                                  tags=self.tags,
                                  locked=self.locked,
                                  guid=self.guid)

    def to_instance_xml(self):
        return model_inst.phyType(name=self.name,
                                  desc=self.desc,
                                  act_logic=self.act_logic,
                                  group_name=self.group_name,
                                  readable_name=self.readable_name,
                                  profile_name=self.profile_name,
                                  profile_inputs=self.profile_inputs.to_instance_xml(),
                                  profile_outputs=self.profile_outputs.to_instance_xml(),
                                  tags=self.tags,
                                  locked=self.locked,
                                  guid=self.guid)

    def __str__(self):
        out = '\n  Phy - {}:\n'.format(self.name)
        out += '    Readable Name: {}\n'.format(self.readable_name)
        out += '    Desc:          {}\n'.format(self.desc)
        out += '    Act Logic:     {}\n'.format(self.act_logic)
        out += '    Group Name:    {}\n'.format(self.group_name)
        out += '    Profile Name:  {}\n'.format(self.profile_name)
        out += '    Tags:          {}\n'.format(self.tags)
        out += '    Locked:        {}\n'.format(self.locked)
        out += '    GUID:          {}\n'.format(self.guid)
        for profile_input in self.profile_inputs:
            out += str(profile_input)
        for profile_output in self.profile_outputs:
            out += str(profile_output)
        return out

    def validate(self, variables):
        assert isinstance(variables, ModelVariableContainer)
        for input in self.profile_inputs:
            if input.var_name not in variables:
                sys.stderr.write("ERROR: Unable to locate input variable '{}'".format(input.var_name))
                return False
            object = getattr(self.profile_inputs, input.var_name)
            if not isinstance(object, ModelInputValue):
                sys.stderr.write("ERROR: Invalid phy input variable '{}'".format(input.var_name))
                return False
        for output in self.profile_outputs:
            if output.var_name not in variables:
                sys.stderr.write("ERROR: Unable to locate output variable '{}'".format(output.var_name))
                return False
            object = getattr(self.profile_outputs, output.var_name)
            if not isinstance(object, ModelOutputOverride):
                sys.stderr.write("ERROR: Invalid phy output override variable '{}'".format(output.var_name))
                return False
        return True

class ModelPhyContainer(object):

    def __init__(self, parser):
        assert isinstance(parser, ModelParser)
        self.zz_parser = parser
        self.ZZ_PHY_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_PHY_KEYS:
            yield self.ZZ_PHY_KEYS[key]

    def __contains__(self, name):
        return name in self.ZZ_PHY_KEYS

    def get_phy(self, phy_name):
        return self.ZZ_PHY_KEYS[phy_name]

    def get_active(self):
        active = []
        for phy in self:
            if phy.is_active():
                active.append(phy)
        return active

    def append(self, phy):
        assert isinstance(phy, ModelPhy), "FATAL ERROR: phy must be ModelPhy"
        self.ZZ_PHY_KEYS[phy.name] = phy
        phy.register_parser(self.zz_parser.eval_act_logic)
        setattr(self, phy.name, phy)

    def extend(self, phy_list):
        for phy in phy_list:
            self.append(phy)

    def to_type_xml(self, phy_group_incl_list):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        phys = model_type.physType()
        for phy in self:
            if phy_group_incl_list:
                if phy.group_name in phy_group_incl_list:
                    phys.add_phy(phy.to_type_xml())
            else:
                phys.add_phy(phy.to_type_xml())
        return phys

    def to_instance_xml(self, name):
        phys = model_inst.physType()
        if name is None:
            return phys
        elif name not in self:
            raise NameError("Phy '{}' does not exist".format(name))
        else:
            phys.phy = self.get_phy(name).to_instance_xml()
        return phys

    def validate(self, vars):
        assert isinstance(vars, ModelVariableContainer)
        for phy in self:
            if not phy.validate(vars):
                return False
        return True

class ModelPhyTypeXml(ModelPhy):

    def __init__(self, vars, profiles, phy_root):
        assert isinstance(vars, ModelVariableContainer)
        assert isinstance(profiles, ModelProfileContainer)
        assert isinstance(phy_root, model_type.phyType)
        profile = profiles.get_profile(phy_root.profile_name)
        super(ModelPhyTypeXml, self).__init__(phy_root.name,
                                              phy_root.desc,
                                              profile,
                                              phy_root.group_name,
                                              phy_root.readable_name,
                                              phy_root.act_logic,
                                              phy_root.tags,
                                              None,  # phy_points_to
                                              phy_root.locked,
                                              phy_root.guid)
        for profile_input in phy_root.get_profile_inputs().get_profile_input():
            var = vars.get_var(profile_input.var_name)
            if profile_input.is_array:
                values = []
                for value in profile_input.values.get_value():
                    values.append((var.var_type)(value))
            else:
                xml_value = profile_input.values.get_value()
                if len(xml_value):
                    values = cast_value_from_xml(var, xml_value)
                else:
                    values = None
            # assign casted values to inputs object
            self.profile_inputs.get_input_value(var.name).value = values
        for profile_output in phy_root.get_profile_outputs().get_profile_output():
            var = vars.get_var(profile_output.var_name)
            if profile_output.is_array:
                overrides = []
                for value in profile_output.overrides.get_value():
                    overrides.append((var.var_type)(value))
            else:
                xml_value = profile_output.overrides.get_value()
                if len(xml_value):
                    overrides = cast_value_from_xml(var, xml_value)
                else:
                    overrides = None
            # assign casted values to outputs override
            self.profile_outputs.get_output_override(var.name).override = overrides




class ModelPhyInstanceXml(ModelPhy):

    def __init__(self, vars, profiles, phy_root):
        assert isinstance(vars, ModelVariableContainer)
        assert isinstance(profiles, ModelProfileContainer)
        assert isinstance(phy_root, model_inst.phyType)
        profile = profiles.get_profile(phy_root.profile_name)
        super(ModelPhyInstanceXml, self).__init__(phy_root.name,
                                                  phy_root.desc,
                                                  profile,
                                                  phy_root.group_name,
                                                  phy_root.readable_name,
                                                  phy_root.act_logic,
                                                  phy_root.tags,
                                                  None,  # phy_points_to
                                                  phy_root.locked,
                                                  phy_root.guid)
        for profile_input in phy_root.get_profile_inputs().get_profile_input():
            var = vars.get_var(profile_input.var_name)
            if profile_input.is_array:
                values = []
                for value in profile_input.values.get_value():
                    values.append((var.var_type)(value))
            else:
                xml_value = profile_input.values.get_value()
                if len(xml_value):
                    values = cast_value_from_xml(var, xml_value)
                else:
                    values = None
            # assign casted values to inputs object
            self.profile_inputs.get_input_value(var.name).value = values
        for profile_output in phy_root.get_profile_outputs().get_profile_output():
            var = vars.get_var(profile_output.var_name)

            if profile_output.is_array:
                overrides = []
                for value in profile_output.overrides.get_value():
                    overrides.append((var.var_type)(value))
            else:
                xml_value = profile_output.overrides.get_value()
                if len(xml_value):
                    overrides = cast_value_from_xml(var, xml_value)
                else:
                    overrides = None
            # assign casted values to outputs override
            self.profile_outputs.get_output_override(var.name).override = overrides
