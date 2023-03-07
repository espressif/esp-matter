
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . TXFRONT_field import *


class RM_Register_TXFRONT_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_IPVERSION, self).__init__(rmio, label,
            0xa8048000, 0x000,
            'IPVERSION', 'TXFRONT.IPVERSION', 'read-only',
            u"",
            0x00000001, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_TXFRONT_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_EN, self).__init__(rmio, label,
            0xa8048000, 0x004,
            'EN', 'TXFRONT.EN', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_TXFRONT_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.DISABLING = RM_Field_TXFRONT_EN_DISABLING(self)
        self.zz_fdict['DISABLING'] = self.DISABLING
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_SWRST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_SWRST, self).__init__(rmio, label,
            0xa8048000, 0x008,
            'SWRST', 'TXFRONT.SWRST', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.SWRST = RM_Field_TXFRONT_SWRST_SWRST(self)
        self.zz_fdict['SWRST'] = self.SWRST
        self.RESETTING = RM_Field_TXFRONT_SWRST_RESETTING(self)
        self.zz_fdict['RESETTING'] = self.RESETTING
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_DISCLK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_DISCLK, self).__init__(rmio, label,
            0xa8048000, 0x010,
            'DISCLK', 'TXFRONT.DISCLK', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.DISCLK = RM_Field_TXFRONT_DISCLK_DISCLK(self)
        self.zz_fdict['DISCLK'] = self.DISCLK
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1CFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1CFG, self).__init__(rmio, label,
            0xa8048000, 0x100,
            'INT1CFG', 'TXFRONT.INT1CFG', 'read-write',
            u"",
            0x000000C7, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RATIO = RM_Field_TXFRONT_INT1CFG_RATIO(self)
        self.zz_fdict['RATIO'] = self.RATIO
        self.GAINSHIFT = RM_Field_TXFRONT_INT1CFG_GAINSHIFT(self)
        self.zz_fdict['GAINSHIFT'] = self.GAINSHIFT
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF01(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF01, self).__init__(rmio, label,
            0xa8048000, 0x104,
            'INT1COEF01', 'TXFRONT.INT1COEF01', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF0 = RM_Field_TXFRONT_INT1COEF01_COEFF0(self)
        self.zz_fdict['COEFF0'] = self.COEFF0
        self.COEFF1 = RM_Field_TXFRONT_INT1COEF01_COEFF1(self)
        self.zz_fdict['COEFF1'] = self.COEFF1
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF23(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF23, self).__init__(rmio, label,
            0xa8048000, 0x108,
            'INT1COEF23', 'TXFRONT.INT1COEF23', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF2 = RM_Field_TXFRONT_INT1COEF23_COEFF2(self)
        self.zz_fdict['COEFF2'] = self.COEFF2
        self.COEFF3 = RM_Field_TXFRONT_INT1COEF23_COEFF3(self)
        self.zz_fdict['COEFF3'] = self.COEFF3
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF45(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF45, self).__init__(rmio, label,
            0xa8048000, 0x10C,
            'INT1COEF45', 'TXFRONT.INT1COEF45', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF4 = RM_Field_TXFRONT_INT1COEF45_COEFF4(self)
        self.zz_fdict['COEFF4'] = self.COEFF4
        self.COEFF5 = RM_Field_TXFRONT_INT1COEF45_COEFF5(self)
        self.zz_fdict['COEFF5'] = self.COEFF5
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF67(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF67, self).__init__(rmio, label,
            0xa8048000, 0x110,
            'INT1COEF67', 'TXFRONT.INT1COEF67', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF6 = RM_Field_TXFRONT_INT1COEF67_COEFF6(self)
        self.zz_fdict['COEFF6'] = self.COEFF6
        self.COEFF7 = RM_Field_TXFRONT_INT1COEF67_COEFF7(self)
        self.zz_fdict['COEFF7'] = self.COEFF7
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF89(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF89, self).__init__(rmio, label,
            0xa8048000, 0x114,
            'INT1COEF89', 'TXFRONT.INT1COEF89', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF8 = RM_Field_TXFRONT_INT1COEF89_COEFF8(self)
        self.zz_fdict['COEFF8'] = self.COEFF8
        self.COEFF9 = RM_Field_TXFRONT_INT1COEF89_COEFF9(self)
        self.zz_fdict['COEFF9'] = self.COEFF9
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF1011(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF1011, self).__init__(rmio, label,
            0xa8048000, 0x118,
            'INT1COEF1011', 'TXFRONT.INT1COEF1011', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF10 = RM_Field_TXFRONT_INT1COEF1011_COEFF10(self)
        self.zz_fdict['COEFF10'] = self.COEFF10
        self.COEFF11 = RM_Field_TXFRONT_INT1COEF1011_COEFF11(self)
        self.zz_fdict['COEFF11'] = self.COEFF11
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF1213(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF1213, self).__init__(rmio, label,
            0xa8048000, 0x11C,
            'INT1COEF1213', 'TXFRONT.INT1COEF1213', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF12 = RM_Field_TXFRONT_INT1COEF1213_COEFF12(self)
        self.zz_fdict['COEFF12'] = self.COEFF12
        self.COEFF13 = RM_Field_TXFRONT_INT1COEF1213_COEFF13(self)
        self.zz_fdict['COEFF13'] = self.COEFF13
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT1COEF1415(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT1COEF1415, self).__init__(rmio, label,
            0xa8048000, 0x120,
            'INT1COEF1415', 'TXFRONT.INT1COEF1415', 'read-write',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF14 = RM_Field_TXFRONT_INT1COEF1415_COEFF14(self)
        self.zz_fdict['COEFF14'] = self.COEFF14
        self.COEFF15 = RM_Field_TXFRONT_INT1COEF1415_COEFF15(self)
        self.zz_fdict['COEFF15'] = self.COEFF15
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_INT2CFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_INT2CFG, self).__init__(rmio, label,
            0xa8048000, 0x124,
            'INT2CFG', 'TXFRONT.INT2CFG', 'read-write',
            u"",
            0x00000802, 0x00007FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RATIO = RM_Field_TXFRONT_INT2CFG_RATIO(self)
        self.zz_fdict['RATIO'] = self.RATIO
        self.GAINSHIFT = RM_Field_TXFRONT_INT2CFG_GAINSHIFT(self)
        self.zz_fdict['GAINSHIFT'] = self.GAINSHIFT
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_SRCCFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_SRCCFG, self).__init__(rmio, label,
            0xa8048000, 0x128,
            'SRCCFG', 'TXFRONT.SRCCFG', 'read-write',
            u"",
            0x00000002, 0x0007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RATIO = RM_Field_TXFRONT_SRCCFG_RATIO(self)
        self.zz_fdict['RATIO'] = self.RATIO
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXIQIMB(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXIQIMB, self).__init__(rmio, label,
            0xa8048000, 0x12C,
            'TXIQIMB', 'TXFRONT.TXIQIMB', 'read-write',
            u"",
            0x00000000, 0x0FFF0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.EPSILON = RM_Field_TXFRONT_TXIQIMB_EPSILON(self)
        self.zz_fdict['EPSILON'] = self.EPSILON
        self.PHI = RM_Field_TXFRONT_TXIQIMB_PHI(self)
        self.zz_fdict['PHI'] = self.PHI
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXDCOFFSET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXDCOFFSET, self).__init__(rmio, label,
            0xa8048000, 0x130,
            'TXDCOFFSET', 'TXFRONT.TXDCOFFSET', 'read-write',
            u"",
            0x00000000, 0x03FF03FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCOFFSETI = RM_Field_TXFRONT_TXDCOFFSET_DCOFFSETI(self)
        self.zz_fdict['DCOFFSETI'] = self.DCOFFSETI
        self.DCOFFSETQ = RM_Field_TXFRONT_TXDCOFFSET_DCOFFSETQ(self)
        self.zz_fdict['DCOFFSETQ'] = self.DCOFFSETQ
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXRAMPUP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXRAMPUP, self).__init__(rmio, label,
            0xa8048000, 0x134,
            'TXRAMPUP', 'TXFRONT.TXRAMPUP', 'read-write',
            u"",
            0x00002088, 0x0001FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODE = RM_Field_TXFRONT_TXRAMPUP_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.DELAY = RM_Field_TXFRONT_TXRAMPUP_DELAY(self)
        self.zz_fdict['DELAY'] = self.DELAY
        self.STEP = RM_Field_TXFRONT_TXRAMPUP_STEP(self)
        self.zz_fdict['STEP'] = self.STEP
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXDCRAMPUP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXDCRAMPUP, self).__init__(rmio, label,
            0xa8048000, 0x138,
            'TXDCRAMPUP', 'TXFRONT.TXDCRAMPUP', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DELAY = RM_Field_TXFRONT_TXDCRAMPUP_DELAY(self)
        self.zz_fdict['DELAY'] = self.DELAY
        self.DCRI = RM_Field_TXFRONT_TXDCRAMPUP_DCRI(self)
        self.zz_fdict['DCRI'] = self.DCRI
        self.DCRQ = RM_Field_TXFRONT_TXDCRAMPUP_DCRQ(self)
        self.zz_fdict['DCRQ'] = self.DCRQ
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXGAIN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXGAIN, self).__init__(rmio, label,
            0xa8048000, 0x13C,
            'TXGAIN', 'TXFRONT.TXGAIN', 'read-write',
            u"",
            0x000001FF, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.GAINDIG = RM_Field_TXFRONT_TXGAIN_GAINDIG(self)
        self.zz_fdict['GAINDIG'] = self.GAINDIG
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXCLIP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXCLIP, self).__init__(rmio, label,
            0xa8048000, 0x140,
            'TXCLIP', 'TXFRONT.TXCLIP', 'read-write',
            u"",
            0x080007FF, 0x0FFF0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLIPPOS = RM_Field_TXFRONT_TXCLIP_CLIPPOS(self)
        self.zz_fdict['CLIPPOS'] = self.CLIPPOS
        self.CLIPNEG = RM_Field_TXFRONT_TXCLIP_CLIPNEG(self)
        self.zz_fdict['CLIPNEG'] = self.CLIPNEG
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_TXPREDIST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_TXPREDIST, self).__init__(rmio, label,
            0xa8048000, 0x144,
            'TXPREDIST', 'TXFRONT.TXPREDIST', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BETAAMP = RM_Field_TXFRONT_TXPREDIST_BETAAMP(self)
        self.zz_fdict['BETAAMP'] = self.BETAAMP
        self.BETAPHA = RM_Field_TXFRONT_TXPREDIST_BETAPHA(self)
        self.zz_fdict['BETAPHA'] = self.BETAPHA
        self.__dict__['zz_frozen'] = True


class RM_Register_TXFRONT_DAC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_TXFRONT_DAC, self).__init__(rmio, label,
            0xa8048000, 0x150,
            'DAC', 'TXFRONT.DAC', 'read-write',
            u"",
            0x00000002, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.CONNECTTEST = RM_Field_TXFRONT_DAC_CONNECTTEST(self)
        self.zz_fdict['CONNECTTEST'] = self.CONNECTTEST
        self.DACFORMAT = RM_Field_TXFRONT_DAC_DACFORMAT(self)
        self.zz_fdict['DACFORMAT'] = self.DACFORMAT
        self.__dict__['zz_frozen'] = True


