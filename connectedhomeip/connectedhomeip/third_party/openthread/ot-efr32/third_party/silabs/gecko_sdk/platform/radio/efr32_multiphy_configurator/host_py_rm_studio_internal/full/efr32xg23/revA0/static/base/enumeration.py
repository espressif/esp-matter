
__all__ = ['Base_RM_Enum', 'Base_RM_EnumInt']

from .. interface import IRegMapEnum
from .. interface import IRegMapEnumInt


class Base_RM_EnumInt(IRegMapEnumInt):
    def __init__(self, zz_field, value):
        self.zz_field = zz_field
        self.value = value

    def __new__(cls, zz_field, value, *args, **kwargs):
        return super(Base_RM_EnumInt, cls).__new__(cls, value)

    def write(self):
        self.zz_field.zz_reg.zz_rmio.writeRegisterField(self.zz_field.zz_reg, self.zz_field, self.value)

    def __getnewargs__(self):
        return self.zz_field, self.value


class Base_RM_Enum(IRegMapEnum):

    def __init__(self, edict, desc):
        self.__dict__['zz_frozen'] = False
        self.zz_edict = edict
        self.zz_desc = desc

    def __setattr__(self, name, value):
        if self.__dict__['zz_frozen']:
            raise AttributeError(u"FATAL ERROR: Unable to set '{}' to '{}'".format(name,
                                                                                   value))
        else:
            self.__dict__[name] = value

    def __repr__(self):
        out = u""
        for key in self.zz_edict.keys():
            out += u"        {}  {}: {}\n".format(self.zz_edict[key],
                                                  key,
                                                  self.zz_desc[key])
        return out

    def getDescByValue(self, value):
        try:
            desc = self.zz_desc[self.getNameByValue(value)]
            return desc
        except KeyError:
            return None

    def getNameByValue(self, value):
        for key in self.zz_edict.keys():
            if self.zz_edict[key] == value:
                return key
        return None
