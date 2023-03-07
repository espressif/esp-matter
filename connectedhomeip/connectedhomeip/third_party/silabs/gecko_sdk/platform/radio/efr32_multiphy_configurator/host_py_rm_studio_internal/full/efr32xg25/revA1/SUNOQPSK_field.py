
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SUNOQPSK_CFG1_MODULATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOQPSK_CFG1_MODULATION, self).__init__(register,
            'MODULATION', 'SUNOQPSK.CFG1.MODULATION', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOQPSK_CFG1_CHIPRATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOQPSK_CFG1_CHIPRATE, self).__init__(register,
            'CHIPRATE', 'SUNOQPSK.CFG1.CHIPRATE', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOQPSK_CFG1_BANDFREQMHZ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOQPSK_CFG1_BANDFREQMHZ, self).__init__(register,
            'BANDFREQMHZ', 'SUNOQPSK.CFG1.BANDFREQMHZ', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOQPSK_CFG2_MACFCSTYPE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOQPSK_CFG2_MACFCSTYPE, self).__init__(register,
            'MACFCSTYPE', 'SUNOQPSK.CFG2.MACFCSTYPE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNOQPSK_CFG2_XTALFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNOQPSK_CFG2_XTALFREQ, self).__init__(register,
            'XTALFREQ', 'SUNOQPSK.CFG2.XTALFREQ', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


