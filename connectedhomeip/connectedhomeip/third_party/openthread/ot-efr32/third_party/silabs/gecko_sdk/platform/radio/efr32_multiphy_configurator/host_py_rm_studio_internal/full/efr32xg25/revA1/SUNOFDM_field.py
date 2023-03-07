
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SUNOFDM_CFG1_MODULATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOFDM_CFG1_MODULATION, self).__init__(register,
            'MODULATION', 'SUNOFDM.CFG1.MODULATION', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOFDM_CFG1_OFDMOPTION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOFDM_CFG1_OFDMOPTION, self).__init__(register,
            'OFDMOPTION', 'SUNOFDM.CFG1.OFDMOPTION', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOFDM_CFG1_INTERLEAVING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOFDM_CFG1_INTERLEAVING, self).__init__(register,
            'INTERLEAVING', 'SUNOFDM.CFG1.INTERLEAVING', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOFDM_CFG1_MACFCSTYPE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOFDM_CFG1_MACFCSTYPE, self).__init__(register,
            'MACFCSTYPE', 'SUNOFDM.CFG1.MACFCSTYPE', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOFDM_CFG1_XTALFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOFDM_CFG1_XTALFREQ, self).__init__(register,
            'XTALFREQ', 'SUNOFDM.CFG1.XTALFREQ', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


