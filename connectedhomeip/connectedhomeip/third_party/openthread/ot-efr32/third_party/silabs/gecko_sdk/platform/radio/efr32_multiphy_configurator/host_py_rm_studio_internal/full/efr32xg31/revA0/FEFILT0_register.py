
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . FEFILT0_field import *


class RM_Register_FEFILT0_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_IPVERSION, self).__init__(rmio, label,
            0xa803c000, 0x000,
            'IPVERSION', 'FEFILT0.IPVERSION', 'read-only',
            u"",
            0x00000002, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_FEFILT0_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_EN, self).__init__(rmio, label,
            0xa803c000, 0x004,
            'EN', 'FEFILT0.EN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_FEFILT0_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_STATUS, self).__init__(rmio, label,
            0xa803c000, 0x00C,
            'STATUS', 'FEFILT0.STATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.FEFILTLOCKSTATUS = RM_Field_FEFILT0_STATUS_FEFILTLOCKSTATUS(self)
        self.zz_fdict['FEFILTLOCKSTATUS'] = self.FEFILTLOCKSTATUS
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_LOCK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_LOCK, self).__init__(rmio, label,
            0xa803c000, 0x010,
            'LOCK', 'FEFILT0.LOCK', 'write-only',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FEFILTLOCKKEY = RM_Field_FEFILT0_LOCK_FEFILTLOCKKEY(self)
        self.zz_fdict['FEFILTLOCKKEY'] = self.FEFILTLOCKKEY
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CFG, self).__init__(rmio, label,
            0xa803c000, 0x100,
            'CFG', 'FEFILT0.CFG', 'read-write',
            u"",
            0x00000000, 0xFC003FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DEC1 = RM_Field_FEFILT0_CFG_DEC1(self)
        self.zz_fdict['DEC1'] = self.DEC1
        self.CHFGAINREDUCTION = RM_Field_FEFILT0_CFG_CHFGAINREDUCTION(self)
        self.zz_fdict['CHFGAINREDUCTION'] = self.CHFGAINREDUCTION
        self.CHFCOEFFSWEN = RM_Field_FEFILT0_CFG_CHFCOEFFSWEN(self)
        self.zz_fdict['CHFCOEFFSWEN'] = self.CHFCOEFFSWEN
        self.CHFCOEFFSWSEL = RM_Field_FEFILT0_CFG_CHFCOEFFSWSEL(self)
        self.zz_fdict['CHFCOEFFSWSEL'] = self.CHFCOEFFSWSEL
        self.CHFCOEFFFWSWEN = RM_Field_FEFILT0_CFG_CHFCOEFFFWSWEN(self)
        self.zz_fdict['CHFCOEFFFWSWEN'] = self.CHFCOEFFFWSWEN
        self.CHFCOEFFFWSWSEL = RM_Field_FEFILT0_CFG_CHFCOEFFFWSWSEL(self)
        self.zz_fdict['CHFCOEFFFWSWSEL'] = self.CHFCOEFFFWSWSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_SRC2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_SRC2, self).__init__(rmio, label,
            0xa803c000, 0x104,
            'SRC2', 'FEFILT0.SRC2', 'read-write',
            u"",
            0x00000000, 0x09C07FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SRC2RATIO = RM_Field_FEFILT0_SRC2_SRC2RATIO(self)
        self.zz_fdict['SRC2RATIO'] = self.SRC2RATIO
        self.UPGAPS = RM_Field_FEFILT0_SRC2_UPGAPS(self)
        self.zz_fdict['UPGAPS'] = self.UPGAPS
        self.SRC2ENABLE = RM_Field_FEFILT0_SRC2_SRC2ENABLE(self)
        self.zz_fdict['SRC2ENABLE'] = self.SRC2ENABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_GAINCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_GAINCTRL, self).__init__(rmio, label,
            0xa803c000, 0x108,
            'GAINCTRL', 'FEFILT0.GAINCTRL', 'read-write',
            u"",
            0x00000000, 0x00000700,
            0x00001000, 0x00002000,
            0x00003000)

        self.DEC0GAIN = RM_Field_FEFILT0_GAINCTRL_DEC0GAIN(self)
        self.zz_fdict['DEC0GAIN'] = self.DEC0GAIN
        self.DEC1GAIN = RM_Field_FEFILT0_GAINCTRL_DEC1GAIN(self)
        self.zz_fdict['DEC1GAIN'] = self.DEC1GAIN
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE00(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE00, self).__init__(rmio, label,
            0xa803c000, 0x10C,
            'CHFCSDCOE00', 'FEFILT0.CHFCSDCOE00', 'read-write',
            u"",
            0x00000000, 0xFFFFFF3F,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0CSDCOEFF0 = RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF0(self)
        self.zz_fdict['SET0CSDCOEFF0'] = self.SET0CSDCOEFF0
        self.SET0CSDCOEFF1 = RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF1(self)
        self.zz_fdict['SET0CSDCOEFF1'] = self.SET0CSDCOEFF1
        self.SET0CSDCOEFF2 = RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF2(self)
        self.zz_fdict['SET0CSDCOEFF2'] = self.SET0CSDCOEFF2
        self.SET0CSDCOEFF3 = RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF3(self)
        self.zz_fdict['SET0CSDCOEFF3'] = self.SET0CSDCOEFF3
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE01(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE01, self).__init__(rmio, label,
            0xa803c000, 0x110,
            'CHFCSDCOE01', 'FEFILT0.CHFCSDCOE01', 'read-write',
            u"",
            0x00000000, 0x03FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0CSDCOEFF4 = RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF4(self)
        self.zz_fdict['SET0CSDCOEFF4'] = self.SET0CSDCOEFF4
        self.SET0CSDCOEFF5 = RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF5(self)
        self.zz_fdict['SET0CSDCOEFF5'] = self.SET0CSDCOEFF5
        self.SET0CSDCOEFF6 = RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF6(self)
        self.zz_fdict['SET0CSDCOEFF6'] = self.SET0CSDCOEFF6
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE02(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE02, self).__init__(rmio, label,
            0xa803c000, 0x114,
            'CHFCSDCOE02', 'FEFILT0.CHFCSDCOE02', 'read-write',
            u"",
            0x00000000, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0CSDCOEFF7 = RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF7(self)
        self.zz_fdict['SET0CSDCOEFF7'] = self.SET0CSDCOEFF7
        self.SET0CSDCOEFF8 = RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF8(self)
        self.zz_fdict['SET0CSDCOEFF8'] = self.SET0CSDCOEFF8
        self.SET0CSDCOEFF9 = RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF9(self)
        self.zz_fdict['SET0CSDCOEFF9'] = self.SET0CSDCOEFF9
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE03(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE03, self).__init__(rmio, label,
            0xa803c000, 0x118,
            'CHFCSDCOE03', 'FEFILT0.CHFCSDCOE03', 'read-write',
            u"",
            0x00000000, 0x007FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0CSDCOEFF10 = RM_Field_FEFILT0_CHFCSDCOE03_SET0CSDCOEFF10(self)
        self.zz_fdict['SET0CSDCOEFF10'] = self.SET0CSDCOEFF10
        self.SET0CSDCOEFF11 = RM_Field_FEFILT0_CHFCSDCOE03_SET0CSDCOEFF11(self)
        self.zz_fdict['SET0CSDCOEFF11'] = self.SET0CSDCOEFF11
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE10(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE10, self).__init__(rmio, label,
            0xa803c000, 0x11C,
            'CHFCSDCOE10', 'FEFILT0.CHFCSDCOE10', 'read-write',
            u"",
            0x00000000, 0xFFFFFF3F,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1CSDCOEFF0 = RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF0(self)
        self.zz_fdict['SET1CSDCOEFF0'] = self.SET1CSDCOEFF0
        self.SET1CSDCOEFF1 = RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF1(self)
        self.zz_fdict['SET1CSDCOEFF1'] = self.SET1CSDCOEFF1
        self.SET1CSDCOEFF2 = RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF2(self)
        self.zz_fdict['SET1CSDCOEFF2'] = self.SET1CSDCOEFF2
        self.SET1CSDCOEFF3 = RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF3(self)
        self.zz_fdict['SET1CSDCOEFF3'] = self.SET1CSDCOEFF3
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE11(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE11, self).__init__(rmio, label,
            0xa803c000, 0x120,
            'CHFCSDCOE11', 'FEFILT0.CHFCSDCOE11', 'read-write',
            u"",
            0x00000000, 0x03FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1CSDCOEFF4 = RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF4(self)
        self.zz_fdict['SET1CSDCOEFF4'] = self.SET1CSDCOEFF4
        self.SET1CSDCOEFF5 = RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF5(self)
        self.zz_fdict['SET1CSDCOEFF5'] = self.SET1CSDCOEFF5
        self.SET1CSDCOEFF6 = RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF6(self)
        self.zz_fdict['SET1CSDCOEFF6'] = self.SET1CSDCOEFF6
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE12(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE12, self).__init__(rmio, label,
            0xa803c000, 0x124,
            'CHFCSDCOE12', 'FEFILT0.CHFCSDCOE12', 'read-write',
            u"",
            0x00000000, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1CSDCOEFF7 = RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF7(self)
        self.zz_fdict['SET1CSDCOEFF7'] = self.SET1CSDCOEFF7
        self.SET1CSDCOEFF8 = RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF8(self)
        self.zz_fdict['SET1CSDCOEFF8'] = self.SET1CSDCOEFF8
        self.SET1CSDCOEFF9 = RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF9(self)
        self.zz_fdict['SET1CSDCOEFF9'] = self.SET1CSDCOEFF9
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE13(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE13, self).__init__(rmio, label,
            0xa803c000, 0x128,
            'CHFCSDCOE13', 'FEFILT0.CHFCSDCOE13', 'read-write',
            u"",
            0x00000000, 0x007FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1CSDCOEFF10 = RM_Field_FEFILT0_CHFCSDCOE13_SET1CSDCOEFF10(self)
        self.zz_fdict['SET1CSDCOEFF10'] = self.SET1CSDCOEFF10
        self.SET1CSDCOEFF11 = RM_Field_FEFILT0_CHFCSDCOE13_SET1CSDCOEFF11(self)
        self.zz_fdict['SET1CSDCOEFF11'] = self.SET1CSDCOEFF11
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE00S(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE00S, self).__init__(rmio, label,
            0xa803c000, 0x12C,
            'CHFCSDCOE00S', 'FEFILT0.CHFCSDCOE00S', 'read-write',
            u"",
            0x00000000, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0CSDCOEFF0S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF0S(self)
        self.zz_fdict['SET0CSDCOEFF0S'] = self.SET0CSDCOEFF0S
        self.SET0CSDCOEFF1S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF1S(self)
        self.zz_fdict['SET0CSDCOEFF1S'] = self.SET0CSDCOEFF1S
        self.SET0CSDCOEFF2S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF2S(self)
        self.zz_fdict['SET0CSDCOEFF2S'] = self.SET0CSDCOEFF2S
        self.SET0CSDCOEFF3S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF3S(self)
        self.zz_fdict['SET0CSDCOEFF3S'] = self.SET0CSDCOEFF3S
        self.SET0CSDCOEFF4S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF4S(self)
        self.zz_fdict['SET0CSDCOEFF4S'] = self.SET0CSDCOEFF4S
        self.SET0CSDCOEFF5S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF5S(self)
        self.zz_fdict['SET0CSDCOEFF5S'] = self.SET0CSDCOEFF5S
        self.SET0CSDCOEFF6S = RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF6S(self)
        self.zz_fdict['SET0CSDCOEFF6S'] = self.SET0CSDCOEFF6S
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE01S(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE01S, self).__init__(rmio, label,
            0xa803c000, 0x130,
            'CHFCSDCOE01S', 'FEFILT0.CHFCSDCOE01S', 'read-write',
            u"",
            0x00000000, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0CSDCOEFF7S = RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF7S(self)
        self.zz_fdict['SET0CSDCOEFF7S'] = self.SET0CSDCOEFF7S
        self.SET0CSDCOEFF8S = RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF8S(self)
        self.zz_fdict['SET0CSDCOEFF8S'] = self.SET0CSDCOEFF8S
        self.SET0CSDCOEFF9S = RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF9S(self)
        self.zz_fdict['SET0CSDCOEFF9S'] = self.SET0CSDCOEFF9S
        self.SET0CSDCOEFF10S = RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF10S(self)
        self.zz_fdict['SET0CSDCOEFF10S'] = self.SET0CSDCOEFF10S
        self.SET0CSDCOEFF11S = RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF11S(self)
        self.zz_fdict['SET0CSDCOEFF11S'] = self.SET0CSDCOEFF11S
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE10S(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE10S, self).__init__(rmio, label,
            0xa803c000, 0x134,
            'CHFCSDCOE10S', 'FEFILT0.CHFCSDCOE10S', 'read-write',
            u"",
            0x00000000, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1CSDCOEFF0S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF0S(self)
        self.zz_fdict['SET1CSDCOEFF0S'] = self.SET1CSDCOEFF0S
        self.SET1CSDCOEFF1S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF1S(self)
        self.zz_fdict['SET1CSDCOEFF1S'] = self.SET1CSDCOEFF1S
        self.SET1CSDCOEFF2S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF2S(self)
        self.zz_fdict['SET1CSDCOEFF2S'] = self.SET1CSDCOEFF2S
        self.SET1CSDCOEFF3S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF3S(self)
        self.zz_fdict['SET1CSDCOEFF3S'] = self.SET1CSDCOEFF3S
        self.SET1CSDCOEFF4S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF4S(self)
        self.zz_fdict['SET1CSDCOEFF4S'] = self.SET1CSDCOEFF4S
        self.SET1CSDCOEFF5S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF5S(self)
        self.zz_fdict['SET1CSDCOEFF5S'] = self.SET1CSDCOEFF5S
        self.SET1CSDCOEFF6S = RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF6S(self)
        self.zz_fdict['SET1CSDCOEFF6S'] = self.SET1CSDCOEFF6S
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_CHFCSDCOE11S(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_CHFCSDCOE11S, self).__init__(rmio, label,
            0xa803c000, 0x138,
            'CHFCSDCOE11S', 'FEFILT0.CHFCSDCOE11S', 'read-write',
            u"",
            0x00000000, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1CSDCOEFF7S = RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF7S(self)
        self.zz_fdict['SET1CSDCOEFF7S'] = self.SET1CSDCOEFF7S
        self.SET1CSDCOEFF8S = RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF8S(self)
        self.zz_fdict['SET1CSDCOEFF8S'] = self.SET1CSDCOEFF8S
        self.SET1CSDCOEFF9S = RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF9S(self)
        self.zz_fdict['SET1CSDCOEFF9S'] = self.SET1CSDCOEFF9S
        self.SET1CSDCOEFF10S = RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF10S(self)
        self.zz_fdict['SET1CSDCOEFF10S'] = self.SET1CSDCOEFF10S
        self.SET1CSDCOEFF11S = RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF11S(self)
        self.zz_fdict['SET1CSDCOEFF11S'] = self.SET1CSDCOEFF11S
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_DIGMIXCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_DIGMIXCTRL, self).__init__(rmio, label,
            0xa803c000, 0x180,
            'DIGMIXCTRL', 'FEFILT0.DIGMIXCTRL', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIGMIXFREQ = RM_Field_FEFILT0_DIGMIXCTRL_DIGMIXFREQ(self)
        self.zz_fdict['DIGMIXFREQ'] = self.DIGMIXFREQ
        self.DIGIQSWAPEN = RM_Field_FEFILT0_DIGMIXCTRL_DIGIQSWAPEN(self)
        self.zz_fdict['DIGIQSWAPEN'] = self.DIGIQSWAPEN
        self.MIXERCONJ = RM_Field_FEFILT0_DIGMIXCTRL_MIXERCONJ(self)
        self.zz_fdict['MIXERCONJ'] = self.MIXERCONJ
        self.DIGMIXFBENABLE = RM_Field_FEFILT0_DIGMIXCTRL_DIGMIXFBENABLE(self)
        self.zz_fdict['DIGMIXFBENABLE'] = self.DIGMIXFBENABLE
        self.ZIFMODEENABLE = RM_Field_FEFILT0_DIGMIXCTRL_ZIFMODEENABLE(self)
        self.zz_fdict['ZIFMODEENABLE'] = self.ZIFMODEENABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_DCCOMP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_DCCOMP, self).__init__(rmio, label,
            0xa803c000, 0x200,
            'DCCOMP', 'FEFILT0.DCCOMP', 'read-write',
            u"",
            0x00000030, 0x007FFF7F,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCESTIEN = RM_Field_FEFILT0_DCCOMP_DCESTIEN(self)
        self.zz_fdict['DCESTIEN'] = self.DCESTIEN
        self.DCCOMPEN = RM_Field_FEFILT0_DCCOMP_DCCOMPEN(self)
        self.zz_fdict['DCCOMPEN'] = self.DCCOMPEN
        self.DCRSTEN = RM_Field_FEFILT0_DCCOMP_DCRSTEN(self)
        self.zz_fdict['DCRSTEN'] = self.DCRSTEN
        self.DCCOMPFREEZE = RM_Field_FEFILT0_DCCOMP_DCCOMPFREEZE(self)
        self.zz_fdict['DCCOMPFREEZE'] = self.DCCOMPFREEZE
        self.DCCOMPGEAR = RM_Field_FEFILT0_DCCOMP_DCCOMPGEAR(self)
        self.zz_fdict['DCCOMPGEAR'] = self.DCCOMPGEAR
        self.DCLIMIT = RM_Field_FEFILT0_DCCOMP_DCLIMIT(self)
        self.zz_fdict['DCLIMIT'] = self.DCLIMIT
        self.DCGAINGEAREN = RM_Field_FEFILT0_DCCOMP_DCGAINGEAREN(self)
        self.zz_fdict['DCGAINGEAREN'] = self.DCGAINGEAREN
        self.DCGAINGEAR = RM_Field_FEFILT0_DCCOMP_DCGAINGEAR(self)
        self.zz_fdict['DCGAINGEAR'] = self.DCGAINGEAR
        self.DCGAINGEARSMPS = RM_Field_FEFILT0_DCCOMP_DCGAINGEARSMPS(self)
        self.zz_fdict['DCGAINGEARSMPS'] = self.DCGAINGEARSMPS
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_DCCOMPFILTINIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_DCCOMPFILTINIT, self).__init__(rmio, label,
            0xa803c000, 0x204,
            'DCCOMPFILTINIT', 'FEFILT0.DCCOMPFILTINIT', 'read-write',
            u"",
            0x00000000, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCCOMPINITVALI = RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALI(self)
        self.zz_fdict['DCCOMPINITVALI'] = self.DCCOMPINITVALI
        self.DCCOMPINITVALQ = RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALQ(self)
        self.zz_fdict['DCCOMPINITVALQ'] = self.DCCOMPINITVALQ
        self.DCCOMPINIT = RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINIT(self)
        self.zz_fdict['DCCOMPINIT'] = self.DCCOMPINIT
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_DCESTI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_DCESTI, self).__init__(rmio, label,
            0xa803c000, 0x208,
            'DCESTI', 'FEFILT0.DCESTI', 'read-only',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCCOMPESTIVALI = RM_Field_FEFILT0_DCESTI_DCCOMPESTIVALI(self)
        self.zz_fdict['DCCOMPESTIVALI'] = self.DCCOMPESTIVALI
        self.DCCOMPESTIVALQ = RM_Field_FEFILT0_DCESTI_DCCOMPESTIVALQ(self)
        self.zz_fdict['DCCOMPESTIVALQ'] = self.DCCOMPESTIVALQ
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_IRCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_IRCAL, self).__init__(rmio, label,
            0xa803c000, 0x250,
            'IRCAL', 'FEFILT0.IRCAL', 'read-write',
            u"",
            0x00000000, 0x0000FFBF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IRCALEN = RM_Field_FEFILT0_IRCAL_IRCALEN(self)
        self.zz_fdict['IRCALEN'] = self.IRCALEN
        self.MURSHF = RM_Field_FEFILT0_IRCAL_MURSHF(self)
        self.zz_fdict['MURSHF'] = self.MURSHF
        self.MUISHF = RM_Field_FEFILT0_IRCAL_MUISHF(self)
        self.zz_fdict['MUISHF'] = self.MUISHF
        self.IRCORREN = RM_Field_FEFILT0_IRCAL_IRCORREN(self)
        self.zz_fdict['IRCORREN'] = self.IRCORREN
        self.IRCALCOEFRSTCMD = RM_Field_FEFILT0_IRCAL_IRCALCOEFRSTCMD(self)
        self.zz_fdict['IRCALCOEFRSTCMD'] = self.IRCALCOEFRSTCMD
        self.IRCALIFADCDBG = RM_Field_FEFILT0_IRCAL_IRCALIFADCDBG(self)
        self.zz_fdict['IRCALIFADCDBG'] = self.IRCALIFADCDBG
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_IRCALCOEF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_IRCALCOEF, self).__init__(rmio, label,
            0xa803c000, 0x254,
            'IRCALCOEF', 'FEFILT0.IRCALCOEF', 'read-only',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CRV = RM_Field_FEFILT0_IRCALCOEF_CRV(self)
        self.zz_fdict['CRV'] = self.CRV
        self.CIV = RM_Field_FEFILT0_IRCALCOEF_CIV(self)
        self.zz_fdict['CIV'] = self.CIV
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT0_IRCALCOEFWR0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT0_IRCALCOEFWR0, self).__init__(rmio, label,
            0xa803c000, 0x300,
            'IRCALCOEFWR0', 'FEFILT0.IRCALCOEFWR0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CRVWD = RM_Field_FEFILT0_IRCALCOEFWR0_CRVWD(self)
        self.zz_fdict['CRVWD'] = self.CRVWD
        self.CRVWEN = RM_Field_FEFILT0_IRCALCOEFWR0_CRVWEN(self)
        self.zz_fdict['CRVWEN'] = self.CRVWEN
        self.CIVWD = RM_Field_FEFILT0_IRCALCOEFWR0_CIVWD(self)
        self.zz_fdict['CIVWD'] = self.CIVWD
        self.CIVWEN = RM_Field_FEFILT0_IRCALCOEFWR0_CIVWEN(self)
        self.zz_fdict['CIVWEN'] = self.CIVWEN
        self.__dict__['zz_frozen'] = True


