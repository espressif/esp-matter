
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . FEFILT1_field import *


class RM_Register_FEFILT1_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_IPVERSION, self).__init__(rmio, label,
            0xa8044000, 0x000,
            'IPVERSION', 'FEFILT1.IPVERSION', 'read-only',
            u"",
            0x00000001, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_FEFILT1_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_EN, self).__init__(rmio, label,
            0xa8044000, 0x004,
            'EN', 'FEFILT1.EN', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_FEFILT1_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.DISABLING = RM_Field_FEFILT1_EN_DISABLING(self)
        self.zz_fdict['DISABLING'] = self.DISABLING
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_SWRST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_SWRST, self).__init__(rmio, label,
            0xa8044000, 0x008,
            'SWRST', 'FEFILT1.SWRST', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.SWRST = RM_Field_FEFILT1_SWRST_SWRST(self)
        self.zz_fdict['SWRST'] = self.SWRST
        self.RESETTING = RM_Field_FEFILT1_SWRST_RESETTING(self)
        self.zz_fdict['RESETTING'] = self.RESETTING
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_STATUS, self).__init__(rmio, label,
            0xa8044000, 0x00C,
            'STATUS', 'FEFILT1.STATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.FEFILTLOCKSTATUS = RM_Field_FEFILT1_STATUS_FEFILTLOCKSTATUS(self)
        self.zz_fdict['FEFILTLOCKSTATUS'] = self.FEFILTLOCKSTATUS
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_LOCK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_LOCK, self).__init__(rmio, label,
            0xa8044000, 0x010,
            'LOCK', 'FEFILT1.LOCK', 'write-only',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FEFILTLOCKKEY = RM_Field_FEFILT1_LOCK_FEFILTLOCKKEY(self)
        self.zz_fdict['FEFILTLOCKKEY'] = self.FEFILTLOCKKEY
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CFG, self).__init__(rmio, label,
            0xa8044000, 0x014,
            'CFG', 'FEFILT1.CFG', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOSYNCRESETENABLE = RM_Field_FEFILT1_CFG_LOSYNCRESETENABLE(self)
        self.zz_fdict['LOSYNCRESETENABLE'] = self.LOSYNCRESETENABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CMD, self).__init__(rmio, label,
            0xa8044000, 0x018,
            'CMD', 'FEFILT1.CMD', 'write-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.SWGAINCHANGE = RM_Field_FEFILT1_CMD_SWGAINCHANGE(self)
        self.zz_fdict['SWGAINCHANGE'] = self.SWGAINCHANGE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CF, self).__init__(rmio, label,
            0xa8044000, 0x100,
            'CF', 'FEFILT1.CF', 'read-write',
            u"",
            0x00000000, 0xC001FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DEC0 = RM_Field_FEFILT1_CF_DEC0(self)
        self.zz_fdict['DEC0'] = self.DEC0
        self.DEC1 = RM_Field_FEFILT1_CF_DEC1(self)
        self.zz_fdict['DEC1'] = self.DEC1
        self.ADCBITORDERI = RM_Field_FEFILT1_CF_ADCBITORDERI(self)
        self.zz_fdict['ADCBITORDERI'] = self.ADCBITORDERI
        self.ADCBITORDERQ = RM_Field_FEFILT1_CF_ADCBITORDERQ(self)
        self.zz_fdict['ADCBITORDERQ'] = self.ADCBITORDERQ
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_SRC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_SRC, self).__init__(rmio, label,
            0xa8044000, 0x104,
            'SRC', 'FEFILT1.SRC', 'read-write',
            u"",
            0x00000000, 0x39CFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SRCRATIO = RM_Field_FEFILT1_SRC_SRCRATIO(self)
        self.zz_fdict['SRCRATIO'] = self.SRCRATIO
        self.UPGAPS = RM_Field_FEFILT1_SRC_UPGAPS(self)
        self.zz_fdict['UPGAPS'] = self.UPGAPS
        self.SRCENABLE = RM_Field_FEFILT1_SRC_SRCENABLE(self)
        self.zz_fdict['SRCENABLE'] = self.SRCENABLE
        self.SRCSRD = RM_Field_FEFILT1_SRC_SRCSRD(self)
        self.zz_fdict['SRCSRD'] = self.SRCSRD
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_DIGIGAINCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_DIGIGAINCTRL, self).__init__(rmio, label,
            0xa8044000, 0x108,
            'DIGIGAINCTRL', 'FEFILT1.DIGIGAINCTRL', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIGIGAINEN = RM_Field_FEFILT1_DIGIGAINCTRL_DIGIGAINEN(self)
        self.zz_fdict['DIGIGAINEN'] = self.DIGIGAINEN
        self.DIGIGAIN = RM_Field_FEFILT1_DIGIGAINCTRL_DIGIGAIN(self)
        self.zz_fdict['DIGIGAIN'] = self.DIGIGAIN
        self.DEC0GAIN = RM_Field_FEFILT1_DIGIGAINCTRL_DEC0GAIN(self)
        self.zz_fdict['DEC0GAIN'] = self.DEC0GAIN
        self.DEC1GAIN = RM_Field_FEFILT1_DIGIGAINCTRL_DEC1GAIN(self)
        self.zz_fdict['DEC1GAIN'] = self.DEC1GAIN
        self.BBSS = RM_Field_FEFILT1_DIGIGAINCTRL_BBSS(self)
        self.zz_fdict['BBSS'] = self.BBSS
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE00(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE00, self).__init__(rmio, label,
            0xa8044000, 0x10C,
            'CHFCOE00', 'FEFILT1.CHFCOE00', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF0 = RM_Field_FEFILT1_CHFCOE00_SET0COEFF0(self)
        self.zz_fdict['SET0COEFF0'] = self.SET0COEFF0
        self.SET0COEFF1 = RM_Field_FEFILT1_CHFCOE00_SET0COEFF1(self)
        self.zz_fdict['SET0COEFF1'] = self.SET0COEFF1
        self.SET0COEFF2 = RM_Field_FEFILT1_CHFCOE00_SET0COEFF2(self)
        self.zz_fdict['SET0COEFF2'] = self.SET0COEFF2
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE01(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE01, self).__init__(rmio, label,
            0xa8044000, 0x110,
            'CHFCOE01', 'FEFILT1.CHFCOE01', 'read-write',
            u"",
            0x00000000, 0x003FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF3 = RM_Field_FEFILT1_CHFCOE01_SET0COEFF3(self)
        self.zz_fdict['SET0COEFF3'] = self.SET0COEFF3
        self.SET0COEFF4 = RM_Field_FEFILT1_CHFCOE01_SET0COEFF4(self)
        self.zz_fdict['SET0COEFF4'] = self.SET0COEFF4
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE02(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE02, self).__init__(rmio, label,
            0xa8044000, 0x114,
            'CHFCOE02', 'FEFILT1.CHFCOE02', 'read-write',
            u"",
            0x00000000, 0x007FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF5 = RM_Field_FEFILT1_CHFCOE02_SET0COEFF5(self)
        self.zz_fdict['SET0COEFF5'] = self.SET0COEFF5
        self.SET0COEFF6 = RM_Field_FEFILT1_CHFCOE02_SET0COEFF6(self)
        self.zz_fdict['SET0COEFF6'] = self.SET0COEFF6
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE03(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE03, self).__init__(rmio, label,
            0xa8044000, 0x118,
            'CHFCOE03', 'FEFILT1.CHFCOE03', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF7 = RM_Field_FEFILT1_CHFCOE03_SET0COEFF7(self)
        self.zz_fdict['SET0COEFF7'] = self.SET0COEFF7
        self.SET0COEFF8 = RM_Field_FEFILT1_CHFCOE03_SET0COEFF8(self)
        self.zz_fdict['SET0COEFF8'] = self.SET0COEFF8
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE04(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE04, self).__init__(rmio, label,
            0xa8044000, 0x11C,
            'CHFCOE04', 'FEFILT1.CHFCOE04', 'read-write',
            u"",
            0x00000000, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF9 = RM_Field_FEFILT1_CHFCOE04_SET0COEFF9(self)
        self.zz_fdict['SET0COEFF9'] = self.SET0COEFF9
        self.SET0COEFF10 = RM_Field_FEFILT1_CHFCOE04_SET0COEFF10(self)
        self.zz_fdict['SET0COEFF10'] = self.SET0COEFF10
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE05(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE05, self).__init__(rmio, label,
            0xa8044000, 0x120,
            'CHFCOE05', 'FEFILT1.CHFCOE05', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF11 = RM_Field_FEFILT1_CHFCOE05_SET0COEFF11(self)
        self.zz_fdict['SET0COEFF11'] = self.SET0COEFF11
        self.SET0COEFF12 = RM_Field_FEFILT1_CHFCOE05_SET0COEFF12(self)
        self.zz_fdict['SET0COEFF12'] = self.SET0COEFF12
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE06(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE06, self).__init__(rmio, label,
            0xa8044000, 0x124,
            'CHFCOE06', 'FEFILT1.CHFCOE06', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET0COEFF13 = RM_Field_FEFILT1_CHFCOE06_SET0COEFF13(self)
        self.zz_fdict['SET0COEFF13'] = self.SET0COEFF13
        self.SET0COEFF14 = RM_Field_FEFILT1_CHFCOE06_SET0COEFF14(self)
        self.zz_fdict['SET0COEFF14'] = self.SET0COEFF14
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE10(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE10, self).__init__(rmio, label,
            0xa8044000, 0x128,
            'CHFCOE10', 'FEFILT1.CHFCOE10', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF0 = RM_Field_FEFILT1_CHFCOE10_SET1COEFF0(self)
        self.zz_fdict['SET1COEFF0'] = self.SET1COEFF0
        self.SET1COEFF1 = RM_Field_FEFILT1_CHFCOE10_SET1COEFF1(self)
        self.zz_fdict['SET1COEFF1'] = self.SET1COEFF1
        self.SET1COEFF2 = RM_Field_FEFILT1_CHFCOE10_SET1COEFF2(self)
        self.zz_fdict['SET1COEFF2'] = self.SET1COEFF2
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE11(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE11, self).__init__(rmio, label,
            0xa8044000, 0x12C,
            'CHFCOE11', 'FEFILT1.CHFCOE11', 'read-write',
            u"",
            0x00000000, 0x003FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF3 = RM_Field_FEFILT1_CHFCOE11_SET1COEFF3(self)
        self.zz_fdict['SET1COEFF3'] = self.SET1COEFF3
        self.SET1COEFF4 = RM_Field_FEFILT1_CHFCOE11_SET1COEFF4(self)
        self.zz_fdict['SET1COEFF4'] = self.SET1COEFF4
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE12(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE12, self).__init__(rmio, label,
            0xa8044000, 0x130,
            'CHFCOE12', 'FEFILT1.CHFCOE12', 'read-write',
            u"",
            0x00000000, 0x007FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF5 = RM_Field_FEFILT1_CHFCOE12_SET1COEFF5(self)
        self.zz_fdict['SET1COEFF5'] = self.SET1COEFF5
        self.SET1COEFF6 = RM_Field_FEFILT1_CHFCOE12_SET1COEFF6(self)
        self.zz_fdict['SET1COEFF6'] = self.SET1COEFF6
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE13(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE13, self).__init__(rmio, label,
            0xa8044000, 0x134,
            'CHFCOE13', 'FEFILT1.CHFCOE13', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF7 = RM_Field_FEFILT1_CHFCOE13_SET1COEFF7(self)
        self.zz_fdict['SET1COEFF7'] = self.SET1COEFF7
        self.SET1COEFF8 = RM_Field_FEFILT1_CHFCOE13_SET1COEFF8(self)
        self.zz_fdict['SET1COEFF8'] = self.SET1COEFF8
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE15(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE15, self).__init__(rmio, label,
            0xa8044000, 0x138,
            'CHFCOE15', 'FEFILT1.CHFCOE15', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF11 = RM_Field_FEFILT1_CHFCOE15_SET1COEFF11(self)
        self.zz_fdict['SET1COEFF11'] = self.SET1COEFF11
        self.SET1COEFF12 = RM_Field_FEFILT1_CHFCOE15_SET1COEFF12(self)
        self.zz_fdict['SET1COEFF12'] = self.SET1COEFF12
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE14(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE14, self).__init__(rmio, label,
            0xa8044000, 0x13C,
            'CHFCOE14', 'FEFILT1.CHFCOE14', 'read-write',
            u"",
            0x00000000, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF9 = RM_Field_FEFILT1_CHFCOE14_SET1COEFF9(self)
        self.zz_fdict['SET1COEFF9'] = self.SET1COEFF9
        self.SET1COEFF10 = RM_Field_FEFILT1_CHFCOE14_SET1COEFF10(self)
        self.zz_fdict['SET1COEFF10'] = self.SET1COEFF10
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCOE16(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCOE16, self).__init__(rmio, label,
            0xa8044000, 0x140,
            'CHFCOE16', 'FEFILT1.CHFCOE16', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SET1COEFF13 = RM_Field_FEFILT1_CHFCOE16_SET1COEFF13(self)
        self.zz_fdict['SET1COEFF13'] = self.SET1COEFF13
        self.SET1COEFF14 = RM_Field_FEFILT1_CHFCOE16_SET1COEFF14(self)
        self.zz_fdict['SET1COEFF14'] = self.SET1COEFF14
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CHFCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CHFCTRL, self).__init__(rmio, label,
            0xa8044000, 0x144,
            'CHFCTRL', 'FEFILT1.CHFCTRL', 'read-write',
            u"",
            0x00000000, 0xC000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.FWSWCOEFFEN = RM_Field_FEFILT1_CHFCTRL_FWSWCOEFFEN(self)
        self.zz_fdict['FWSWCOEFFEN'] = self.FWSWCOEFFEN
        self.FWSELCOEFF = RM_Field_FEFILT1_CHFCTRL_FWSELCOEFF(self)
        self.zz_fdict['FWSELCOEFF'] = self.FWSELCOEFF
        self.CHFLATENCY = RM_Field_FEFILT1_CHFCTRL_CHFLATENCY(self)
        self.zz_fdict['CHFLATENCY'] = self.CHFLATENCY
        self.SWCOEFFSELECTION = RM_Field_FEFILT1_CHFCTRL_SWCOEFFSELECTION(self)
        self.zz_fdict['SWCOEFFSELECTION'] = self.SWCOEFFSELECTION
        self.SWCOEFFEN = RM_Field_FEFILT1_CHFCTRL_SWCOEFFEN(self)
        self.zz_fdict['SWCOEFFEN'] = self.SWCOEFFEN
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_CTRL, self).__init__(rmio, label,
            0xa8044000, 0x14C,
            'CTRL', 'FEFILT1.CTRL', 'read-write',
            u"",
            0x00000000, 0x0007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SWAFCADJRX = RM_Field_FEFILT1_CTRL_SWAFCADJRX(self)
        self.zz_fdict['SWAFCADJRX'] = self.SWAFCADJRX
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_BLANKING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_BLANKING, self).__init__(rmio, label,
            0xa8044000, 0x150,
            'BLANKING', 'FEFILT1.BLANKING', 'read-write',
            u"",
            0x00000000, 0x0007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BLANKDELAY = RM_Field_FEFILT1_BLANKING_BLANKDELAY(self)
        self.zz_fdict['BLANKDELAY'] = self.BLANKDELAY
        self.BLANKDURATION = RM_Field_FEFILT1_BLANKING_BLANKDURATION(self)
        self.zz_fdict['BLANKDURATION'] = self.BLANKDURATION
        self.BLANKMODE = RM_Field_FEFILT1_BLANKING_BLANKMODE(self)
        self.zz_fdict['BLANKMODE'] = self.BLANKMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_RSSIFILT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_RSSIFILT, self).__init__(rmio, label,
            0xa8044000, 0x154,
            'RSSIFILT', 'FEFILT1.RSSIFILT', 'read-write',
            u"",
            0x00000000, 0x001FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIFILTEN = RM_Field_FEFILT1_RSSIFILT_RSSIFILTEN(self)
        self.zz_fdict['RSSIFILTEN'] = self.RSSIFILTEN
        self.RSSIFILTDEC = RM_Field_FEFILT1_RSSIFILT_RSSIFILTDEC(self)
        self.zz_fdict['RSSIFILTDEC'] = self.RSSIFILTDEC
        self.RSSIFILTGAIN = RM_Field_FEFILT1_RSSIFILT_RSSIFILTGAIN(self)
        self.zz_fdict['RSSIFILTGAIN'] = self.RSSIFILTGAIN
        self.RSSIFILTCOEF0 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF0(self)
        self.zz_fdict['RSSIFILTCOEF0'] = self.RSSIFILTCOEF0
        self.RSSIFILTCOEF1 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF1(self)
        self.zz_fdict['RSSIFILTCOEF1'] = self.RSSIFILTCOEF1
        self.RSSIFILTCOEF2 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF2(self)
        self.zz_fdict['RSSIFILTCOEF2'] = self.RSSIFILTCOEF2
        self.RSSIFILTCOEF3 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF3(self)
        self.zz_fdict['RSSIFILTCOEF3'] = self.RSSIFILTCOEF3
        self.RSSIFILTCOEF4 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF4(self)
        self.zz_fdict['RSSIFILTCOEF4'] = self.RSSIFILTCOEF4
        self.RSSIFILTCOEF5 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF5(self)
        self.zz_fdict['RSSIFILTCOEF5'] = self.RSSIFILTCOEF5
        self.RSSIFILTCOEF6 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF6(self)
        self.zz_fdict['RSSIFILTCOEF6'] = self.RSSIFILTCOEF6
        self.RSSIFILTCOEF7 = RM_Field_FEFILT1_RSSIFILT_RSSIFILTCOEF7(self)
        self.zz_fdict['RSSIFILTCOEF7'] = self.RSSIFILTCOEF7
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_DIGMIXCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_DIGMIXCTRL, self).__init__(rmio, label,
            0xa8044000, 0x180,
            'DIGMIXCTRL', 'FEFILT1.DIGMIXCTRL', 'read-write',
            u"",
            0x00000000, 0x007FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIGMIXFREQ = RM_Field_FEFILT1_DIGMIXCTRL_DIGMIXFREQ(self)
        self.zz_fdict['DIGMIXFREQ'] = self.DIGMIXFREQ
        self.DIGIQSWAPEN = RM_Field_FEFILT1_DIGMIXCTRL_DIGIQSWAPEN(self)
        self.zz_fdict['DIGIQSWAPEN'] = self.DIGIQSWAPEN
        self.MIXERCONJ = RM_Field_FEFILT1_DIGMIXCTRL_MIXERCONJ(self)
        self.zz_fdict['MIXERCONJ'] = self.MIXERCONJ
        self.DIGMIXFBENABLE = RM_Field_FEFILT1_DIGMIXCTRL_DIGMIXFBENABLE(self)
        self.zz_fdict['DIGMIXFBENABLE'] = self.DIGMIXFBENABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_DCCOMP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_DCCOMP, self).__init__(rmio, label,
            0xa8044000, 0x200,
            'DCCOMP', 'FEFILT1.DCCOMP', 'read-write',
            u"",
            0x00000030, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCESTIEN = RM_Field_FEFILT1_DCCOMP_DCESTIEN(self)
        self.zz_fdict['DCESTIEN'] = self.DCESTIEN
        self.DCCOMPEN = RM_Field_FEFILT1_DCCOMP_DCCOMPEN(self)
        self.zz_fdict['DCCOMPEN'] = self.DCCOMPEN
        self.DCRSTEN = RM_Field_FEFILT1_DCCOMP_DCRSTEN(self)
        self.zz_fdict['DCRSTEN'] = self.DCRSTEN
        self.DCCOMPFREEZE = RM_Field_FEFILT1_DCCOMP_DCCOMPFREEZE(self)
        self.zz_fdict['DCCOMPFREEZE'] = self.DCCOMPFREEZE
        self.DCCOMPGEAR = RM_Field_FEFILT1_DCCOMP_DCCOMPGEAR(self)
        self.zz_fdict['DCCOMPGEAR'] = self.DCCOMPGEAR
        self.DCLIMIT = RM_Field_FEFILT1_DCCOMP_DCLIMIT(self)
        self.zz_fdict['DCLIMIT'] = self.DCLIMIT
        self.DCGAINGEAREN = RM_Field_FEFILT1_DCCOMP_DCGAINGEAREN(self)
        self.zz_fdict['DCGAINGEAREN'] = self.DCGAINGEAREN
        self.DCGAINGEAR = RM_Field_FEFILT1_DCCOMP_DCGAINGEAR(self)
        self.zz_fdict['DCGAINGEAR'] = self.DCGAINGEAR
        self.DCGAINGEARSMPS = RM_Field_FEFILT1_DCCOMP_DCGAINGEARSMPS(self)
        self.zz_fdict['DCGAINGEARSMPS'] = self.DCGAINGEARSMPS
        self.GEARGAINCHANGESEL = RM_Field_FEFILT1_DCCOMP_GEARGAINCHANGESEL(self)
        self.zz_fdict['GEARGAINCHANGESEL'] = self.GEARGAINCHANGESEL
        self.GEARFREEZEENABLE = RM_Field_FEFILT1_DCCOMP_GEARFREEZEENABLE(self)
        self.zz_fdict['GEARFREEZEENABLE'] = self.GEARFREEZEENABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_DCCOMPFILTINIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_DCCOMPFILTINIT, self).__init__(rmio, label,
            0xa8044000, 0x204,
            'DCCOMPFILTINIT', 'FEFILT1.DCCOMPFILTINIT', 'read-write',
            u"",
            0x00000000, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCCOMPINITVALI = RM_Field_FEFILT1_DCCOMPFILTINIT_DCCOMPINITVALI(self)
        self.zz_fdict['DCCOMPINITVALI'] = self.DCCOMPINITVALI
        self.DCCOMPINITVALQ = RM_Field_FEFILT1_DCCOMPFILTINIT_DCCOMPINITVALQ(self)
        self.zz_fdict['DCCOMPINITVALQ'] = self.DCCOMPINITVALQ
        self.DCCOMPINIT = RM_Field_FEFILT1_DCCOMPFILTINIT_DCCOMPINIT(self)
        self.zz_fdict['DCCOMPINIT'] = self.DCCOMPINIT
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_DCESTI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_DCESTI, self).__init__(rmio, label,
            0xa8044000, 0x208,
            'DCESTI', 'FEFILT1.DCESTI', 'read-only',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCCOMPESTIVALI = RM_Field_FEFILT1_DCESTI_DCCOMPESTIVALI(self)
        self.zz_fdict['DCCOMPESTIVALI'] = self.DCCOMPESTIVALI
        self.DCCOMPESTIVALQ = RM_Field_FEFILT1_DCESTI_DCCOMPESTIVALQ(self)
        self.zz_fdict['DCCOMPESTIVALQ'] = self.DCCOMPESTIVALQ
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_IRCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_IRCAL, self).__init__(rmio, label,
            0xa8044000, 0x250,
            'IRCAL', 'FEFILT1.IRCAL', 'read-write',
            u"",
            0x00000000, 0x0000FFBF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IRCALEN = RM_Field_FEFILT1_IRCAL_IRCALEN(self)
        self.zz_fdict['IRCALEN'] = self.IRCALEN
        self.MURSHF = RM_Field_FEFILT1_IRCAL_MURSHF(self)
        self.zz_fdict['MURSHF'] = self.MURSHF
        self.MUISHF = RM_Field_FEFILT1_IRCAL_MUISHF(self)
        self.zz_fdict['MUISHF'] = self.MUISHF
        self.IRCORREN = RM_Field_FEFILT1_IRCAL_IRCORREN(self)
        self.zz_fdict['IRCORREN'] = self.IRCORREN
        self.IRCALCOEFRSTCMD = RM_Field_FEFILT1_IRCAL_IRCALCOEFRSTCMD(self)
        self.zz_fdict['IRCALCOEFRSTCMD'] = self.IRCALCOEFRSTCMD
        self.IRCALIFADCDBG = RM_Field_FEFILT1_IRCAL_IRCALIFADCDBG(self)
        self.zz_fdict['IRCALIFADCDBG'] = self.IRCALIFADCDBG
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_IRCALCOEF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_IRCALCOEF, self).__init__(rmio, label,
            0xa8044000, 0x254,
            'IRCALCOEF', 'FEFILT1.IRCALCOEF', 'read-only',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CRV = RM_Field_FEFILT1_IRCALCOEF_CRV(self)
        self.zz_fdict['CRV'] = self.CRV
        self.CIV = RM_Field_FEFILT1_IRCALCOEF_CIV(self)
        self.zz_fdict['CIV'] = self.CIV
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_IRCALCOEFWR0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_IRCALCOEFWR0, self).__init__(rmio, label,
            0xa8044000, 0x300,
            'IRCALCOEFWR0', 'FEFILT1.IRCALCOEFWR0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CRVWD = RM_Field_FEFILT1_IRCALCOEFWR0_CRVWD(self)
        self.zz_fdict['CRVWD'] = self.CRVWD
        self.CRVWEN = RM_Field_FEFILT1_IRCALCOEFWR0_CRVWEN(self)
        self.zz_fdict['CRVWEN'] = self.CRVWEN
        self.CIVWD = RM_Field_FEFILT1_IRCALCOEFWR0_CIVWD(self)
        self.zz_fdict['CIVWD'] = self.CIVWD
        self.CIVWEN = RM_Field_FEFILT1_IRCALCOEFWR0_CIVWEN(self)
        self.zz_fdict['CIVWEN'] = self.CIVWEN
        self.__dict__['zz_frozen'] = True


class RM_Register_FEFILT1_IRCALCOEFWR1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_FEFILT1_IRCALCOEFWR1, self).__init__(rmio, label,
            0xa8044000, 0x304,
            'IRCALCOEFWR1', 'FEFILT1.IRCALCOEFWR1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CRVWD = RM_Field_FEFILT1_IRCALCOEFWR1_CRVWD(self)
        self.zz_fdict['CRVWD'] = self.CRVWD
        self.CRVWEN = RM_Field_FEFILT1_IRCALCOEFWR1_CRVWEN(self)
        self.zz_fdict['CRVWEN'] = self.CRVWEN
        self.CIVWD = RM_Field_FEFILT1_IRCALCOEFWR1_CIVWD(self)
        self.zz_fdict['CIVWD'] = self.CIVWD
        self.CIVWEN = RM_Field_FEFILT1_IRCALCOEFWR1_CIVWEN(self)
        self.zz_fdict['CIVWEN'] = self.CIVWEN
        self.__dict__['zz_frozen'] = True


