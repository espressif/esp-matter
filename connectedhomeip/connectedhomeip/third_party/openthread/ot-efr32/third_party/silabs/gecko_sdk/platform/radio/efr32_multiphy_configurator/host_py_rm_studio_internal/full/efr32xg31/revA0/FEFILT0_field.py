
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_FEFILT0_IPVERSION_IPVERSION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IPVERSION_IPVERSION, self).__init__(register,
            'IPVERSION', 'FEFILT0.IPVERSION.IPVERSION', 'read-only',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_EN_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_EN_EN, self).__init__(register,
            'EN', 'FEFILT0.EN.EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_STATUS_FEFILTLOCKSTATUS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_STATUS_FEFILTLOCKSTATUS, self).__init__(register,
            'FEFILTLOCKSTATUS', 'FEFILT0.STATUS.FEFILTLOCKSTATUS', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_LOCK_FEFILTLOCKKEY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_LOCK_FEFILTLOCKKEY, self).__init__(register,
            'FEFILTLOCKKEY', 'FEFILT0.LOCK.FEFILTLOCKKEY', 'write-only',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CFG_DEC1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CFG_DEC1, self).__init__(register,
            'DEC1', 'FEFILT0.CFG.DEC1', 'read-write',
            u"",
            0, 14)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CFG_CHFGAINREDUCTION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CFG_CHFGAINREDUCTION, self).__init__(register,
            'CHFGAINREDUCTION', 'FEFILT0.CFG.CHFGAINREDUCTION', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CFG_CHFCOEFFSWEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CFG_CHFCOEFFSWEN, self).__init__(register,
            'CHFCOEFFSWEN', 'FEFILT0.CFG.CHFCOEFFSWEN', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CFG_CHFCOEFFSWSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CFG_CHFCOEFFSWSEL, self).__init__(register,
            'CHFCOEFFSWSEL', 'FEFILT0.CFG.CHFCOEFFSWSEL', 'read-write',
            u"",
            28, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CFG_CHFCOEFFFWSWEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CFG_CHFCOEFFFWSWEN, self).__init__(register,
            'CHFCOEFFFWSWEN', 'FEFILT0.CFG.CHFCOEFFFWSWEN', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CFG_CHFCOEFFFWSWSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CFG_CHFCOEFFFWSWSEL, self).__init__(register,
            'CHFCOEFFFWSWSEL', 'FEFILT0.CFG.CHFCOEFFFWSWSEL', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_SRC2_SRC2RATIO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_SRC2_SRC2RATIO, self).__init__(register,
            'SRC2RATIO', 'FEFILT0.SRC2.SRC2RATIO', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_SRC2_UPGAPS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_SRC2_UPGAPS, self).__init__(register,
            'UPGAPS', 'FEFILT0.SRC2.UPGAPS', 'read-write',
            u"",
            22, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_SRC2_SRC2ENABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_SRC2_SRC2ENABLE, self).__init__(register,
            'SRC2ENABLE', 'FEFILT0.SRC2.SRC2ENABLE', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_GAINCTRL_DEC0GAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_GAINCTRL_DEC0GAIN, self).__init__(register,
            'DEC0GAIN', 'FEFILT0.GAINCTRL.DEC0GAIN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_GAINCTRL_DEC1GAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_GAINCTRL_DEC1GAIN, self).__init__(register,
            'DEC1GAIN', 'FEFILT0.GAINCTRL.DEC1GAIN', 'read-write',
            u"",
            9, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF0, self).__init__(register,
            'SET0CSDCOEFF0', 'FEFILT0.CHFCSDCOE00.SET0CSDCOEFF0', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF1, self).__init__(register,
            'SET0CSDCOEFF1', 'FEFILT0.CHFCSDCOE00.SET0CSDCOEFF1', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF2, self).__init__(register,
            'SET0CSDCOEFF2', 'FEFILT0.CHFCSDCOE00.SET0CSDCOEFF2', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00_SET0CSDCOEFF3, self).__init__(register,
            'SET0CSDCOEFF3', 'FEFILT0.CHFCSDCOE00.SET0CSDCOEFF3', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF4, self).__init__(register,
            'SET0CSDCOEFF4', 'FEFILT0.CHFCSDCOE01.SET0CSDCOEFF4', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF5, self).__init__(register,
            'SET0CSDCOEFF5', 'FEFILT0.CHFCSDCOE01.SET0CSDCOEFF5', 'read-write',
            u"",
            8, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01_SET0CSDCOEFF6, self).__init__(register,
            'SET0CSDCOEFF6', 'FEFILT0.CHFCSDCOE01.SET0CSDCOEFF6', 'read-write',
            u"",
            17, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF7(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF7, self).__init__(register,
            'SET0CSDCOEFF7', 'FEFILT0.CHFCSDCOE02.SET0CSDCOEFF7', 'read-write',
            u"",
            0, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF8(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF8, self).__init__(register,
            'SET0CSDCOEFF8', 'FEFILT0.CHFCSDCOE02.SET0CSDCOEFF8', 'read-write',
            u"",
            9, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF9(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE02_SET0CSDCOEFF9, self).__init__(register,
            'SET0CSDCOEFF9', 'FEFILT0.CHFCSDCOE02.SET0CSDCOEFF9', 'read-write',
            u"",
            19, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE03_SET0CSDCOEFF10(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE03_SET0CSDCOEFF10, self).__init__(register,
            'SET0CSDCOEFF10', 'FEFILT0.CHFCSDCOE03.SET0CSDCOEFF10', 'read-write',
            u"",
            0, 11)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE03_SET0CSDCOEFF11(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE03_SET0CSDCOEFF11, self).__init__(register,
            'SET0CSDCOEFF11', 'FEFILT0.CHFCSDCOE03.SET0CSDCOEFF11', 'read-write',
            u"",
            11, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF0, self).__init__(register,
            'SET1CSDCOEFF0', 'FEFILT0.CHFCSDCOE10.SET1CSDCOEFF0', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF1, self).__init__(register,
            'SET1CSDCOEFF1', 'FEFILT0.CHFCSDCOE10.SET1CSDCOEFF1', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF2, self).__init__(register,
            'SET1CSDCOEFF2', 'FEFILT0.CHFCSDCOE10.SET1CSDCOEFF2', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10_SET1CSDCOEFF3, self).__init__(register,
            'SET1CSDCOEFF3', 'FEFILT0.CHFCSDCOE10.SET1CSDCOEFF3', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF4, self).__init__(register,
            'SET1CSDCOEFF4', 'FEFILT0.CHFCSDCOE11.SET1CSDCOEFF4', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF5, self).__init__(register,
            'SET1CSDCOEFF5', 'FEFILT0.CHFCSDCOE11.SET1CSDCOEFF5', 'read-write',
            u"",
            8, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11_SET1CSDCOEFF6, self).__init__(register,
            'SET1CSDCOEFF6', 'FEFILT0.CHFCSDCOE11.SET1CSDCOEFF6', 'read-write',
            u"",
            17, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF7(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF7, self).__init__(register,
            'SET1CSDCOEFF7', 'FEFILT0.CHFCSDCOE12.SET1CSDCOEFF7', 'read-write',
            u"",
            0, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF8(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF8, self).__init__(register,
            'SET1CSDCOEFF8', 'FEFILT0.CHFCSDCOE12.SET1CSDCOEFF8', 'read-write',
            u"",
            9, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF9(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE12_SET1CSDCOEFF9, self).__init__(register,
            'SET1CSDCOEFF9', 'FEFILT0.CHFCSDCOE12.SET1CSDCOEFF9', 'read-write',
            u"",
            19, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE13_SET1CSDCOEFF10(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE13_SET1CSDCOEFF10, self).__init__(register,
            'SET1CSDCOEFF10', 'FEFILT0.CHFCSDCOE13.SET1CSDCOEFF10', 'read-write',
            u"",
            0, 11)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE13_SET1CSDCOEFF11(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE13_SET1CSDCOEFF11, self).__init__(register,
            'SET1CSDCOEFF11', 'FEFILT0.CHFCSDCOE13.SET1CSDCOEFF11', 'read-write',
            u"",
            11, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF0S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF0S, self).__init__(register,
            'SET0CSDCOEFF0S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF0S', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF1S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF1S, self).__init__(register,
            'SET0CSDCOEFF1S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF1S', 'read-write',
            u"",
            3, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF2S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF2S, self).__init__(register,
            'SET0CSDCOEFF2S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF2S', 'read-write',
            u"",
            7, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF3S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF3S, self).__init__(register,
            'SET0CSDCOEFF3S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF3S', 'read-write',
            u"",
            11, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF4S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF4S, self).__init__(register,
            'SET0CSDCOEFF4S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF4S', 'read-write',
            u"",
            15, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF5S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF5S, self).__init__(register,
            'SET0CSDCOEFF5S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF5S', 'read-write',
            u"",
            19, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF6S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE00S_SET0CSDCOEFF6S, self).__init__(register,
            'SET0CSDCOEFF6S', 'FEFILT0.CHFCSDCOE00S.SET0CSDCOEFF6S', 'read-write',
            u"",
            24, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF7S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF7S, self).__init__(register,
            'SET0CSDCOEFF7S', 'FEFILT0.CHFCSDCOE01S.SET0CSDCOEFF7S', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF8S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF8S, self).__init__(register,
            'SET0CSDCOEFF8S', 'FEFILT0.CHFCSDCOE01S.SET0CSDCOEFF8S', 'read-write',
            u"",
            5, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF9S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF9S, self).__init__(register,
            'SET0CSDCOEFF9S', 'FEFILT0.CHFCSDCOE01S.SET0CSDCOEFF9S', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF10S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF10S, self).__init__(register,
            'SET0CSDCOEFF10S', 'FEFILT0.CHFCSDCOE01S.SET0CSDCOEFF10S', 'read-write',
            u"",
            15, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF11S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE01S_SET0CSDCOEFF11S, self).__init__(register,
            'SET0CSDCOEFF11S', 'FEFILT0.CHFCSDCOE01S.SET0CSDCOEFF11S', 'read-write',
            u"",
            21, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF0S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF0S, self).__init__(register,
            'SET1CSDCOEFF0S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF0S', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF1S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF1S, self).__init__(register,
            'SET1CSDCOEFF1S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF1S', 'read-write',
            u"",
            3, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF2S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF2S, self).__init__(register,
            'SET1CSDCOEFF2S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF2S', 'read-write',
            u"",
            7, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF3S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF3S, self).__init__(register,
            'SET1CSDCOEFF3S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF3S', 'read-write',
            u"",
            11, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF4S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF4S, self).__init__(register,
            'SET1CSDCOEFF4S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF4S', 'read-write',
            u"",
            15, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF5S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF5S, self).__init__(register,
            'SET1CSDCOEFF5S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF5S', 'read-write',
            u"",
            19, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF6S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE10S_SET1CSDCOEFF6S, self).__init__(register,
            'SET1CSDCOEFF6S', 'FEFILT0.CHFCSDCOE10S.SET1CSDCOEFF6S', 'read-write',
            u"",
            24, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF7S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF7S, self).__init__(register,
            'SET1CSDCOEFF7S', 'FEFILT0.CHFCSDCOE11S.SET1CSDCOEFF7S', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF8S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF8S, self).__init__(register,
            'SET1CSDCOEFF8S', 'FEFILT0.CHFCSDCOE11S.SET1CSDCOEFF8S', 'read-write',
            u"",
            5, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF9S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF9S, self).__init__(register,
            'SET1CSDCOEFF9S', 'FEFILT0.CHFCSDCOE11S.SET1CSDCOEFF9S', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF10S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF10S, self).__init__(register,
            'SET1CSDCOEFF10S', 'FEFILT0.CHFCSDCOE11S.SET1CSDCOEFF10S', 'read-write',
            u"",
            15, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF11S(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_CHFCSDCOE11S_SET1CSDCOEFF11S, self).__init__(register,
            'SET1CSDCOEFF11S', 'FEFILT0.CHFCSDCOE11S.SET1CSDCOEFF11S', 'read-write',
            u"",
            21, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DIGMIXCTRL_DIGMIXFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DIGMIXCTRL_DIGMIXFREQ, self).__init__(register,
            'DIGMIXFREQ', 'FEFILT0.DIGMIXCTRL.DIGMIXFREQ', 'read-write',
            u"",
            0, 20)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DIGMIXCTRL_DIGIQSWAPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DIGMIXCTRL_DIGIQSWAPEN, self).__init__(register,
            'DIGIQSWAPEN', 'FEFILT0.DIGMIXCTRL.DIGIQSWAPEN', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DIGMIXCTRL_MIXERCONJ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DIGMIXCTRL_MIXERCONJ, self).__init__(register,
            'MIXERCONJ', 'FEFILT0.DIGMIXCTRL.MIXERCONJ', 'read-write',
            u"",
            21, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DIGMIXCTRL_DIGMIXFBENABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DIGMIXCTRL_DIGMIXFBENABLE, self).__init__(register,
            'DIGMIXFBENABLE', 'FEFILT0.DIGMIXCTRL.DIGMIXFBENABLE', 'read-write',
            u"",
            22, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DIGMIXCTRL_ZIFMODEENABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DIGMIXCTRL_ZIFMODEENABLE, self).__init__(register,
            'ZIFMODEENABLE', 'FEFILT0.DIGMIXCTRL.ZIFMODEENABLE', 'read-write',
            u"",
            23, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCESTIEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCESTIEN, self).__init__(register,
            'DCESTIEN', 'FEFILT0.DCCOMP.DCESTIEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCCOMPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCCOMPEN, self).__init__(register,
            'DCCOMPEN', 'FEFILT0.DCCOMP.DCCOMPEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCRSTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCRSTEN, self).__init__(register,
            'DCRSTEN', 'FEFILT0.DCCOMP.DCRSTEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCCOMPFREEZE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCCOMPFREEZE, self).__init__(register,
            'DCCOMPFREEZE', 'FEFILT0.DCCOMP.DCCOMPFREEZE', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCCOMPGEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCCOMPGEAR, self).__init__(register,
            'DCCOMPGEAR', 'FEFILT0.DCCOMP.DCCOMPGEAR', 'read-write',
            u"",
            4, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCLIMIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCLIMIT, self).__init__(register,
            'DCLIMIT', 'FEFILT0.DCCOMP.DCLIMIT', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCGAINGEAREN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCGAINGEAREN, self).__init__(register,
            'DCGAINGEAREN', 'FEFILT0.DCCOMP.DCGAINGEAREN', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCGAINGEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCGAINGEAR, self).__init__(register,
            'DCGAINGEAR', 'FEFILT0.DCCOMP.DCGAINGEAR', 'read-write',
            u"",
            11, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMP_DCGAINGEARSMPS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMP_DCGAINGEARSMPS, self).__init__(register,
            'DCGAINGEARSMPS', 'FEFILT0.DCCOMP.DCGAINGEARSMPS', 'read-write',
            u"",
            15, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALI, self).__init__(register,
            'DCCOMPINITVALI', 'FEFILT0.DCCOMPFILTINIT.DCCOMPINITVALI', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALQ, self).__init__(register,
            'DCCOMPINITVALQ', 'FEFILT0.DCCOMPFILTINIT.DCCOMPINITVALQ', 'read-write',
            u"",
            15, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCCOMPFILTINIT_DCCOMPINIT, self).__init__(register,
            'DCCOMPINIT', 'FEFILT0.DCCOMPFILTINIT.DCCOMPINIT', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCESTI_DCCOMPESTIVALI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCESTI_DCCOMPESTIVALI, self).__init__(register,
            'DCCOMPESTIVALI', 'FEFILT0.DCESTI.DCCOMPESTIVALI', 'read-only',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_DCESTI_DCCOMPESTIVALQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_DCESTI_DCCOMPESTIVALQ, self).__init__(register,
            'DCCOMPESTIVALQ', 'FEFILT0.DCESTI.DCCOMPESTIVALQ', 'read-only',
            u"",
            15, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCAL_IRCALEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCAL_IRCALEN, self).__init__(register,
            'IRCALEN', 'FEFILT0.IRCAL.IRCALEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCAL_MURSHF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCAL_MURSHF, self).__init__(register,
            'MURSHF', 'FEFILT0.IRCAL.MURSHF', 'read-write',
            u"",
            1, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCAL_MUISHF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCAL_MUISHF, self).__init__(register,
            'MUISHF', 'FEFILT0.IRCAL.MUISHF', 'read-write',
            u"",
            7, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCAL_IRCORREN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCAL_IRCORREN, self).__init__(register,
            'IRCORREN', 'FEFILT0.IRCAL.IRCORREN', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCAL_IRCALCOEFRSTCMD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCAL_IRCALCOEFRSTCMD, self).__init__(register,
            'IRCALCOEFRSTCMD', 'FEFILT0.IRCAL.IRCALCOEFRSTCMD', 'write-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCAL_IRCALIFADCDBG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCAL_IRCALIFADCDBG, self).__init__(register,
            'IRCALIFADCDBG', 'FEFILT0.IRCAL.IRCALIFADCDBG', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCALCOEF_CRV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCALCOEF_CRV, self).__init__(register,
            'CRV', 'FEFILT0.IRCALCOEF.CRV', 'read-only',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCALCOEF_CIV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCALCOEF_CIV, self).__init__(register,
            'CIV', 'FEFILT0.IRCALCOEF.CIV', 'read-only',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCALCOEFWR0_CRVWD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCALCOEFWR0_CRVWD, self).__init__(register,
            'CRVWD', 'FEFILT0.IRCALCOEFWR0.CRVWD', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCALCOEFWR0_CRVWEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCALCOEFWR0_CRVWEN, self).__init__(register,
            'CRVWEN', 'FEFILT0.IRCALCOEFWR0.CRVWEN', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCALCOEFWR0_CIVWD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCALCOEFWR0_CIVWD, self).__init__(register,
            'CIVWD', 'FEFILT0.IRCALCOEFWR0.CIVWD', 'read-write',
            u"",
            16, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_FEFILT0_IRCALCOEFWR0_CIVWEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FEFILT0_IRCALCOEFWR0_CIVWEN, self).__init__(register,
            'CIVWEN', 'FEFILT0.IRCALCOEFWR0.CIVWEN', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


