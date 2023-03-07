
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SUNOQPSK_field import *


class RM_Register_SUNOQPSK_CFG1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNOQPSK_CFG1, self).__init__(rmio, label,
            0xb500fc00, 0x000,
            'CFG1', 'SUNOQPSK.CFG1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODULATION = RM_Field_SUNOQPSK_CFG1_MODULATION(self)
        self.zz_fdict['MODULATION'] = self.MODULATION
        self.CHIPRATE = RM_Field_SUNOQPSK_CFG1_CHIPRATE(self)
        self.zz_fdict['CHIPRATE'] = self.CHIPRATE
        self.BANDFREQMHZ = RM_Field_SUNOQPSK_CFG1_BANDFREQMHZ(self)
        self.zz_fdict['BANDFREQMHZ'] = self.BANDFREQMHZ
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNOQPSK_CFG2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNOQPSK_CFG2, self).__init__(rmio, label,
            0xb500fc00, 0x004,
            'CFG2', 'SUNOQPSK.CFG2', 'read-write',
            u"",
            0x00000000, 0xFFFF0001,
            0x00001000, 0x00002000,
            0x00003000)

        self.MACFCSTYPE = RM_Field_SUNOQPSK_CFG2_MACFCSTYPE(self)
        self.zz_fdict['MACFCSTYPE'] = self.MACFCSTYPE
        self.XTALFREQ = RM_Field_SUNOQPSK_CFG2_XTALFREQ(self)
        self.zz_fdict['XTALFREQ'] = self.XTALFREQ
        self.__dict__['zz_frozen'] = True


