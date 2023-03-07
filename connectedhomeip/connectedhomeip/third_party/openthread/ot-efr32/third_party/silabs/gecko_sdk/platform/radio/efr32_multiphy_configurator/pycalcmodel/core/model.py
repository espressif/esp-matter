from pycalcmodel.core.log import LogMsg, LogType

try:
    from StringIO import StringIO
except ModuleNotFoundError:
    from io import StringIO

try:
    from pycalcmodel import __version__, model_instance
    from pycalcmodel import model_type as model_type
except ImportError as ex:
    print('ImportError')
from pycalcmodel import model_instance as model_inst

from pycalcmodel.core.phy import *
from pycalcmodel.core.parser import ModelParser
from pycalcmodel.core.profile import ModelProfileContainer, ModelProfileTypeXml, ModelProfileInstanceXml
from pycalcmodel.core.variable import ModelVariableContainer, ModelVariableTypeXml, ModelVariableInstanceXml
from pycalcmodel.core.reg_model import RegModel
from pycalcmodel.core.default_phy import *
from pycalcmodel.core.feature import *
from datetime import datetime
import sys
from py_2_and_3_compatibility import *

__all__ = [ 'ModelRoot', 'ModelRootTypeXml', 'ModelRootInstanceXml' ]


class ModelRoot(object):

    def __init__(self, part_family, calc_version, target=None, part_revision='ANY'):
        assert isinstance(part_family, basestring)
        assert isinstance(calc_version, basestring)
        self.part_family = part_family
        self.part_revision = part_revision
        self.calc_version = calc_version
        self._xsd_version = __version__
        self.parser = ModelParser()
        self.features = ModelFeatureContainer(self.parser)
        self.phys = ModelPhyContainer(self.parser)
        self.profiles = ModelProfileContainer(self.parser)
        self.vars = ModelVariableContainer()
        self.logs = None
        self.calc_routine_execution = None  # This variable is not serialized/deserialized to XML, only used for unit tests
        self._reg_model = RegModel.get_reg_model(part_family)  # Reg model object is passed by reference
        if target is not None:
            self.target = target

    @property
    def part_family(self):
        return self._part_family

    @part_family.setter
    def part_family(self, value):
        assert isinstance(value, basestring)
        self._part_family = value

    @property
    def xsd_version(self):
        return self._xsd_version

    @property
    def calc_version(self):
        return self._calc_version

    @calc_version.setter
    def calc_version(self, value):
        assert isinstance(value, basestring)
        self._calc_version = value

    @property
    def target(self):
        return self._target

    @target.setter
    def target(self, value):
        assert isinstance(value, basestring)
        self._target = value

    @property
    def reg_model(self):
        return self._reg_model

    def validate(self):
        """
        First validate variables, then the profiles.

        :return: True for valid, False for invalid.
        """
        return self.vars.validate() and self.profiles.validate(self.vars) and self.phys.validate(self.vars)

    def to_type_xml(self, filename, phy_group_incl_list=None):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        with open(filename, 'w') as outfile:
            return self._write_to_stream(outfile, phy_group_incl_list)

    def to_type_xml_str(self, phy_group_incl_list=None):
        xml_string = None
        outfile = StringIO()  # this is an in memory file
        try:
            retur_val = self._write_to_stream(outfile, phy_group_incl_list, pretty_print=False)
            if retur_val is not None:
                xml_string = outfile.getvalue()
        finally:
            outfile.close()
        return xml_string

    def _write_to_stream(self, outstream, phy_group_incl_list=None, pretty_print=True):
        if self.validate():
            outstream.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")

            profile_incl_list = self.getProfilesInclList(phy_group_incl_list, self.phys)

            model_obj = model_type.model(part_family=self.part_family,
                                         calc_version=self.calc_version,
                                         xsd_version=self.xsd_version,
                                         phys=self.phys.to_type_xml(phy_group_incl_list),
                                         profiles=self.profiles.to_type_xml(profile_incl_list, phy_group_incl_list),
                                         variables=self.vars.to_type_xml(),
                                         features=self.features.to_type_xml())
            model_obj.export(outstream, 0, pretty_print=pretty_print)
            return 0
        else:
            sys.stderr.write("ERROR: Type data model is invalid. Unable to create XML.")
            return -1

    def getProfilesInclList(self, phy_group_incl_list, phys):
        if phy_group_incl_list:
            profile_list = []
            for phy in phys:
                if phy.group_name in phy_group_incl_list:
                    if phy.profile_name not in profile_list:
                        profile_list.append(phy.profile_name)
        else:
            profile_list = None
        return profile_list

    def to_instance_xml(self, filename, part_revision, desc, processed, result_code, error_message, profile, phy=None, timestamp=None, phy_group_incl_list=None):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        with open(filename, 'w') as outfile:
            return self._write_instance_to_stream(outfile, part_revision, desc, processed, result_code, error_message, profile, phy, timestamp, phy_group_incl_list)

    def to_instance_xml_str(self, part_revision, desc, processed, result_code, error_message, profile, phy=None, timestamp=None, phy_group_incl_list=None):
        xml_string = None
        outfile = StringIO()  # this is an in memory file
        try:
            retur_val = self._write_instance_to_stream(outfile, part_revision, desc, processed, result_code, error_message, profile, phy, timestamp, phy_group_incl_list, pretty_print=False)
            if retur_val is not None:
                xml_string = outfile.getvalue()
        finally:
            outfile.close()
        return xml_string

    def _write_instance_to_stream(self, outstream, part_revision, desc, processed, result_code, error_message, profile, phy=None, timestamp=None, phy_group_incl_list=None, pretty_print=True):
        if self.validate():
            outstream.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
            if processed:
                if timestamp is None:
                    timestamp = datetime.now().isoformat().split('.')[0]
            else:
                timestamp = None
            phy_name = None
            if phy is not None:
                if phy_group_incl_list:
                    if phy.group_name in phy_group_incl_list:
                        phy_name = phy.name
                else:
                    phy_name = phy.name

            if self.logs is not None and len(self.logs) > 0:
                logs_xml = model_instance.logsType()
                for log in self.logs:
                    log_xml = model_instance.logType3(str(log.log_type.name), log.message)
                    logs_xml.add_log(log_xml)
            else:
                logs_xml = None

            model_obj = model_inst.model(part_family=self.part_family,
                                         part_revision=part_revision,
                                         calc_version=self.calc_version,
                                         xsd_version=self.xsd_version,
                                         desc=desc,
                                         processed=processed,
                                         result_code=result_code,
                                         error_message=error_message,
                                         timestamp=timestamp,
                                         phys=self.phys.to_instance_xml(phy_name),
                                         profiles=self.profiles.to_instance_xml(profile.name, phy_group_incl_list),
                                         variables=self.vars.to_instance_xml(),
                                         features=self.features.to_instance_xml(),
                                         logs=logs_xml,
                                         target=self.target)
            model_obj.export(outstream, 0, pretty_print=pretty_print)
            return 0
        else:
            sys.stderr.write("ERROR: Model instance is invalid. Unable to create XML.")
            return -1

    def get_profile_names(self):
        profile_names = []
        for profile in self.profiles:
            if profile.is_active():
                profile_names.append(profile.name)
        return profile_names

    def get_phy_names(self):
        phy_names = []
        for phy in self.phys:
            if phy.is_active():
                phy_names.append(phy.name)
        return phy_names

    def reset_all_variables(self):
        for var in self.vars:
            var.value_calc = None
            var.value_forced = None

    #
    # Used for instance models, where there is only one
    #
    @property
    def profile(self):
        profile = next(x for x in self.profiles) # gets the first one
        return profile

    #
    # Used for instance models, where there is only zero or one
    #
    @property
    def phy(self):
        phy = None
        try:
            phy = next(x for x in self.phys) # gets the first one
        except StopIteration:
            pass  # assuming no PHY's found
        return phy

    def __str__(self):
        out = '\nModel Part Family {}:\n'.format(self.part_family)
        out += '  Calculator Version: {}\n'.format(self.calc_version)
        out += '  XSD Version : {}\n'.format(self.xsd_version)
        for phy in self.phys:
            out += str(phy)
        for profile in self.profiles:
            out += str(profile)
        for var in self.vars:
            out += str(var)
        return out


class ModelRootTypeXml(ModelRoot):

    def __init__(self, filename):

        _root = model_type.parse(filename)
        assert isinstance(_root, model_type.model)

        super(ModelRootTypeXml, self).__init__(_root.part_family,
                                               _root.calc_version)
        if self.xsd_version != _root.xsd_version:
            print("WARNING: Parsing XML with XSD version {} in pycalcmodel {}".format(self.xsd_version,
                                                                                      _root.xsd_version))
        for feature in _root.get_features().get_feature():
            self.features.append(ModelFeatureTypeXml(feature))

        for var in _root.get_variables().get_variable():
            self.vars.append(ModelVariableTypeXml(var))

        for profile in _root.get_profiles().get_profile():
            self.profiles.append(ModelProfileTypeXml(self.vars, profile))

        for phy in _root.get_phys().get_phy():
            self.phys.append(ModelPhyTypeXml(self.vars, self.profiles, phy))

        # with phys built, update the profile default phys
        for profile_root in _root.get_profiles().get_profile():
            profile = self.profiles.get_profile(profile_root.name)
            for default_phy_root in profile_root.get_default_phys().get_default_phy():
                phy = self.phys.get_phy(default_phy_root.phy_name)
                profile.default_phys.append(ModelDefaultPhy(phy))


class ModelRootInstanceXml(ModelRoot):

    def __init__(self, filename):

        _root = model_inst.parse(filename)
        assert isinstance(_root, model_inst.model)

        super(ModelRootInstanceXml, self).__init__(_root.part_family,
                                                   _root.calc_version,
                                                   _root.target)
        if self.xsd_version != _root.xsd_version:
            print("WARNING: Parsing XML with XSD version {} in pycalcmodel {}".format(self.xsd_version,
                                                                                      _root.xsd_version))

        self.part_revision = _root.part_revision
        self.desc = _root.desc
        self.processed = _root.processed
        self.result_code = _root.result_code
        self.error_message = _root.error_message
        self.timestamp = _root.timestamp

        for feature in _root.get_features().get_feature():
            self.features.append(ModelFeatureInstanceXml(feature))

        for var in _root.get_variables().get_variable():
            self.vars.append(ModelVariableInstanceXml(var))

        profile = _root.get_profiles().get_profile()
        self.profiles.append(ModelProfileInstanceXml(self.vars, profile))

        phy = _root.get_phys().get_phy()
        if phy is not None:
            self.phys.append(ModelPhyInstanceXml(self.vars, self.profiles, phy))

        # with phys built, update the profile default phys
        profile_root = _root.get_profiles().get_profile()
        profile = self.profiles.get_profile(profile_root.name)
        for default_phy_root in profile_root.get_default_phys().get_default_phy():
            try:
                phy = self.phys.get_phy(default_phy_root.phy_name)
            except KeyError as err:
                print("WARNING: Phy '{}' is not available".format(default_phy_root.phy_name))
                phy = None
            if phy:
                profile.default_phys.append(ModelDefaultPhy(phy))

        if _root.logs is not None:
            for log in _root.get_logs().get_log():
                if self.logs is None:
                    self.logs = list()
                self.logs.append(LogMsg(LogType[log.get_type()], log.get_valueOf_()))
