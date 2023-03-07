
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_CW_CFG1_MODULATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG1_MODULATION, self).__init__(register,
            'MODULATION', 'CW.CFG1.MODULATION', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG1_NFFTLOG2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG1_NFFTLOG2, self).__init__(register,
            'NFFTLOG2', 'CW.CFG1.NFFTLOG2', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG1_DUALPATH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG1_DUALPATH, self).__init__(register,
            'DUALPATH', 'CW.CFG1.DUALPATH', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG1_FFTANALYZE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG1_FFTANALYZE, self).__init__(register,
            'FFTANALYZE', 'CW.CFG1.FFTANALYZE', 'read-write',
            u"",
            24, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG1_SIGTYPE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG1_SIGTYPE, self).__init__(register,
            'SIGTYPE', 'CW.CFG1.SIGTYPE', 'read-write',
            u"",
            28, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG2_FREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG2_FREQ, self).__init__(register,
            'FREQ', 'CW.CFG2.FREQ', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG2_AMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG2_AMP, self).__init__(register,
            'AMP', 'CW.CFG2.AMP', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_CW_CFG3_XTALFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_CW_CFG3_XTALFREQ, self).__init__(register,
            'XTALFREQ', 'CW.CFG3.XTALFREQ', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


