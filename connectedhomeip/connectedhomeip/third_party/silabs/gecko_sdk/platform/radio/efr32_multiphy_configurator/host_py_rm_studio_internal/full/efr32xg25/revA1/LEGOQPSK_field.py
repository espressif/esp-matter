
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_LEGOQPSK_CFG1_MODULATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_LEGOQPSK_CFG1_MODULATION, self).__init__(register,
            'MODULATION', 'LEGOQPSK.CFG1.MODULATION', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_LEGOQPSK_CFG1_CHIPRATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_LEGOQPSK_CFG1_CHIPRATE, self).__init__(register,
            'CHIPRATE', 'LEGOQPSK.CFG1.CHIPRATE', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_LEGOQPSK_CFG1_BANDFREQMHZ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_LEGOQPSK_CFG1_BANDFREQMHZ, self).__init__(register,
            'BANDFREQMHZ', 'LEGOQPSK.CFG1.BANDFREQMHZ', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_LEGOQPSK_CFG2_XTALFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_LEGOQPSK_CFG2_XTALFREQ, self).__init__(register,
            'XTALFREQ', 'LEGOQPSK.CFG2.XTALFREQ', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


