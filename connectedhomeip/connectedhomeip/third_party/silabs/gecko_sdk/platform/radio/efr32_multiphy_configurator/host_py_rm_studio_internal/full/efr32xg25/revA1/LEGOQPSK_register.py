
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . LEGOQPSK_field import *


class RM_Register_LEGOQPSK_CFG1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_LEGOQPSK_CFG1, self).__init__(rmio, label,
            0xb500fc00, 0x000,
            'CFG1', 'LEGOQPSK.CFG1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODULATION = RM_Field_LEGOQPSK_CFG1_MODULATION(self)
        self.zz_fdict['MODULATION'] = self.MODULATION
        self.CHIPRATE = RM_Field_LEGOQPSK_CFG1_CHIPRATE(self)
        self.zz_fdict['CHIPRATE'] = self.CHIPRATE
        self.BANDFREQMHZ = RM_Field_LEGOQPSK_CFG1_BANDFREQMHZ(self)
        self.zz_fdict['BANDFREQMHZ'] = self.BANDFREQMHZ
        self.__dict__['zz_frozen'] = True


class RM_Register_LEGOQPSK_CFG2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_LEGOQPSK_CFG2, self).__init__(rmio, label,
            0xb500fc00, 0x004,
            'CFG2', 'LEGOQPSK.CFG2', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.XTALFREQ = RM_Field_LEGOQPSK_CFG2_XTALFREQ(self)
        self.zz_fdict['XTALFREQ'] = self.XTALFREQ
        self.__dict__['zz_frozen'] = True


