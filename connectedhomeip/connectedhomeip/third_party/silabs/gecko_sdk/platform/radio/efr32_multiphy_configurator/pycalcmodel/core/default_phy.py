
"""
This module contains the ModelPhyDefault class to define a phy default
for a profile.
"""

from .. import model_type as model_type
from .. import model_instance as model_inst
from collections import OrderedDict
try:
    import phy as PHY
except ModuleNotFoundError:
    from pycalcmodel.core import phy as PHY

__all__ = [ 'ModelDefaultPhy', 'ModelDefaultPhyContainer',
            'ModelDefaultPhyTypeXml', 'ModelDefaultPhyInstanceXml' ]


class ModelDefaultPhy(object):

    def __init__(self, phy):
        assert isinstance(phy, PHY.ModelPhy), \
            "FATAL ERROR: phy must be ModelPhy"
        self._phy = phy

    @property
    def phy_name(self):
        return self._phy.name

    @property
    def group_name(self):
        return self._phy.group_name

    def is_active(self):
        return self._phy.is_active()

    def to_type_xml(self):
        return model_type.default_phyType(phy_name=self.phy_name)

    def to_instance_xml(self):
        return model_inst.default_phyType(phy_name=self.phy_name)

    def __str__(self):
        out = '\n      Default Phy - {}:\n'.format(self.phy_name)
        return out


class ModelDefaultPhyContainer(object):

    def __init__(self):
        self.ZZ_DEFAULT_PHY_KEYS = OrderedDict()

    def __iter__(self):
        for key in self.ZZ_DEFAULT_PHY_KEYS:
            yield self.ZZ_DEFAULT_PHY_KEYS[key]

    def __contains__(self, phy_name):
        return phy_name in self.ZZ_DEFAULT_PHY_KEYS

    def get_default_phy_name(self, phy_name):
        return self.ZZ_DEFAULT_PHY_KEYS[phy_name]

    def get_active_default(self):
        for default_phy in self:
            if default_phy.is_active():
                return default_phy
        return None

    def append(self, default_phy):
        assert isinstance(default_phy, ModelDefaultPhy), \
            "FATAL ERROR: default_default_phy must be ModelDefaultPhy"
        self.ZZ_DEFAULT_PHY_KEYS[default_phy.phy_name] = default_phy
        setattr(self, default_phy.phy_name, default_phy)

    def extend(self, default_phy_list):
        for default_phy in default_phy_list:
            self.append(default_phy)

    def to_type_xml(self, phy_group_incl_list):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        default_phys = model_type.default_physType()
        for default_phy in self:
            if phy_group_incl_list:
                if default_phy.group_name in phy_group_incl_list:
                    default_phys.add_default_phy(default_phy.to_type_xml())
            else:
                default_phys.add_default_phy(default_phy.to_type_xml())
        return default_phys

    def to_instance_xml(self, phy_group_incl_list):
        assert isinstance(phy_group_incl_list, (list, type(None)))
        default_phys = model_inst.default_physType()
        active_default_phy = self.get_active_default()
        if active_default_phy is not None:
            if phy_group_incl_list:
                if active_default_phy.group_name in phy_group_incl_list:
                    default_phys.add_default_phy(active_default_phy.to_instance_xml())
            else:
                default_phys.add_default_phy(active_default_phy.to_instance_xml())
        return default_phys


class ModelDefaultPhyTypeXml(ModelDefaultPhy):

    def __init__(self, phys, default_phy_root):
        assert isinstance(phys, PHY.ModelPhyContainer)
        assert isinstance(default_phy_root, model_type.default_phyType)
        phy = phys.get_phy(default_phy_root.phy_name)
        super(ModelDefaultPhyTypeXml, self).__init__(phy)


class ModelDefaultPhyInstanceXml(ModelDefaultPhy):

    def __init__(self, phys, default_phy_root):
        assert isinstance(phys, PHY.ModelPhyContainer)
        assert isinstance(default_phy_root, model_inst.default_phyType)
        phy = phys.get_phy(default_phy_root.phy_name)
        super(ModelDefaultPhyInstanceXml, self).__init__(phy)
