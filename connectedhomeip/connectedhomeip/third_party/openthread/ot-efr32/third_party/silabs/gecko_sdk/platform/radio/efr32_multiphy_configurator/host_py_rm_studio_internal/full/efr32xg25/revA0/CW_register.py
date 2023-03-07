
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . CW_field import *


class RM_Register_CW_CFG1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CW_CFG1, self).__init__(rmio, label,
            0xb500fc00, 0x000,
            'CFG1', 'CW.CFG1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODULATION = RM_Field_CW_CFG1_MODULATION(self)
        self.zz_fdict['MODULATION'] = self.MODULATION
        self.NFFTLOG2 = RM_Field_CW_CFG1_NFFTLOG2(self)
        self.zz_fdict['NFFTLOG2'] = self.NFFTLOG2
        self.DUALPATH = RM_Field_CW_CFG1_DUALPATH(self)
        self.zz_fdict['DUALPATH'] = self.DUALPATH
        self.FFTANALYZE = RM_Field_CW_CFG1_FFTANALYZE(self)
        self.zz_fdict['FFTANALYZE'] = self.FFTANALYZE
        self.SIGTYPE = RM_Field_CW_CFG1_SIGTYPE(self)
        self.zz_fdict['SIGTYPE'] = self.SIGTYPE
        self.__dict__['zz_frozen'] = True


class RM_Register_CW_CFG2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CW_CFG2, self).__init__(rmio, label,
            0xb500fc00, 0x004,
            'CFG2', 'CW.CFG2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FREQ = RM_Field_CW_CFG2_FREQ(self)
        self.zz_fdict['FREQ'] = self.FREQ
        self.AMP = RM_Field_CW_CFG2_AMP(self)
        self.zz_fdict['AMP'] = self.AMP
        self.__dict__['zz_frozen'] = True


class RM_Register_CW_CFG3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CW_CFG3, self).__init__(rmio, label,
            0xb500fc00, 0x008,
            'CFG3', 'CW.CFG3', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.XTALFREQ = RM_Field_CW_CFG3_XTALFREQ(self)
        self.zz_fdict['XTALFREQ'] = self.XTALFREQ
        self.__dict__['zz_frozen'] = True


