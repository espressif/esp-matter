
"""

"""
from .. import model_type as model_type
from .. import model_instance as model_inst
from collections import OrderedDict
from pycalcmodel.core.parser import ModelParser
from pycalcmodel.py2_and_3_compatibility import *

__all__ = [ 'ModelFeature', 'ModelFeatureContainer',
            'ModelFeatureTypeXml', 'ModelFeatureInstanceXml' ]

# ===========================================================================
#  CONSTANTS
# ===========================================================================


# ===========================================================================
#  CLASSES
# ===========================================================================

# ---------------------------------------------------------------------------
class ModelFeature(object):
    """
    Defines a feature used as a feature variable in activation logic
    attributes.
    """

    def __init__(self, name, desc='', value=False):
        self.zz_parser_update = None
        #: The feature name
        self.name = name
        #: A string description of the feature
        self.desc = desc
        self.value = value

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, value):
        assert isinstance(value, basestring)
        self._name = value

    @property
    def desc(self):
        return self._desc

    @desc.setter
    def desc(self, value):
        assert isinstance(value, basestring)
        self._desc = value

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        assert isinstance(value, bool)
        self._value = value
        if self.zz_parser_update:
            self.zz_parser_update(self.name, value)

    def register_parser(self, parser_update_func):
        assert callable(parser_update_func)
        self.zz_parser_update = parser_update_func

    def to_type_xml(self):
        return model_type.featureType(name=self.name,
                                      desc=self.desc)

    def to_instance_xml(self):
        return model_inst.featureType(name=self.name,
                                      desc=self.desc,
                                      value=self.value)

    def __str__(self):
        out = '\n  Feature - {}\n'.format(self.name)
        out += '    Desc:             {}\n'.format(self.desc)
        out += '    Value:            {}\n'.format(self.value)
        return out


class ModelFeatureContainer(object):

    def __init__(self, parser):
        assert isinstance(parser, ModelParser)
        self.zz_parser = parser
        self.ZZ_FEATURE_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_FEATURE_KEYS:
            yield self.ZZ_FEATURE_KEYS[key]

    def __contains__(self, name):
        return name in self.ZZ_FEATURE_KEYS

    def get_feature(self, var_name):
        return self.ZZ_FEATURE_KEYS[var_name]

    def get_act_logic_dict(self):
        act_logic_features = {}
        for feature in self:
            act_logic_features['feature_{}'.format(feature.name)] = feature.value
        return act_logic_features

    def append(self, feature):
        assert isinstance(feature, ModelFeature), \
            "FATAL ERROR: feature must be ModelFeature"
        if feature.name in self:
            raise NameError("'{}' is already defined!".format(feature.name))
        feature.register_parser(self.zz_parser.update_feature)
        self.ZZ_FEATURE_KEYS[feature.name] = feature
        self.zz_parser.update_feature(feature.name, feature.value)
        setattr(self, feature.name, feature)

    def extend(self, feature_list):
        for feature in feature_list:
            self.append(feature)

    def to_type_xml(self):
        features = model_type.features()
        for feature in self:
            features.add_feature(feature.to_type_xml())
        return features

    def to_instance_xml(self):
        features = model_inst.features()
        for feature in self:
            features.add_feature(feature.to_instance_xml())
        return features

    def remove_feature(self, name):
        # Remove attribute
        if hasattr(self, name):
            delattr(self, name)

        # Remove from dictionary
        if name in self.ZZ_FEATURE_KEYS:
            self.ZZ_FEATURE_KEYS.pop(name)


class ModelFeatureTypeXml(ModelFeature):

    def __init__(self, feature_root):
        assert isinstance(feature_root, model_type.featureType)
        super(ModelFeatureTypeXml, self).__init__(feature_root.name,
                                                  feature_root.desc)

class ModelFeatureInstanceXml(ModelFeature):

    def __init__(self, feature_root):
        assert isinstance(feature_root, model_inst.featureType)
        super(ModelFeatureInstanceXml, self).__init__(feature_root.name,
                                                      feature_root.desc,
                                                      feature_root.value)
