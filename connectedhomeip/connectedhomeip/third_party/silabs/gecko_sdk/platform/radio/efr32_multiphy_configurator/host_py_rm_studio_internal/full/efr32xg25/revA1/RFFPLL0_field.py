
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_RFFPLL0_IPVERSION_IPVERSION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_IPVERSION_IPVERSION, self).__init__(register,
            'IPVERSION', 'RFFPLL0.IPVERSION.IPVERSION', 'read-only',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_CFG_DISFSMRFBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_CFG_DISFSMRFBIAS, self).__init__(register,
            'DISFSMRFBIAS', 'RFFPLL0.CFG.DISFSMRFBIAS', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_CFG_DISFSMRFFPLL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_CFG_DISFSMRFFPLL, self).__init__(register,
            'DISFSMRFFPLL', 'RFFPLL0.CFG.DISFSMRFFPLL', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_CTRL_FORCEENRFBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_CTRL_FORCEENRFBIAS, self).__init__(register,
            'FORCEENRFBIAS', 'RFFPLL0.CTRL.FORCEENRFBIAS', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_CTRL_FORCEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_CTRL_FORCEEN, self).__init__(register,
            'FORCEEN', 'RFFPLL0.CTRL.FORCEEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_CTRL_DISONDEMAND(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_CTRL_DISONDEMAND, self).__init__(register,
            'DISONDEMAND', 'RFFPLL0.CTRL.DISONDEMAND', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_CTRL_FORCERAWCLK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_CTRL_FORCERAWCLK, self).__init__(register,
            'FORCERAWCLK', 'RFFPLL0.CTRL.FORCERAWCLK', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_RFFPLLRADIORDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_RFFPLLRADIORDY, self).__init__(register,
            'RFFPLLRADIORDY', 'RFFPLL0.STATUS.RFFPLLRADIORDY', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_RFFPLLSYSRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_RFFPLLSYSRDY, self).__init__(register,
            'RFFPLLSYSRDY', 'RFFPLL0.STATUS.RFFPLLSYSRDY', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_RFBIASRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_RFBIASRDY, self).__init__(register,
            'RFBIASRDY', 'RFFPLL0.STATUS.RFBIASRDY', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_ENS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_ENS, self).__init__(register,
            'ENS', 'RFFPLL0.STATUS.ENS', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_HWREQRADIO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_HWREQRADIO, self).__init__(register,
            'HWREQRADIO', 'RFFPLL0.STATUS.HWREQRADIO', 'read-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_HWREQSYS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_HWREQSYS, self).__init__(register,
            'HWREQSYS', 'RFFPLL0.STATUS.HWREQSYS', 'read-only',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_ISFORCEDRADIO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_ISFORCEDRADIO, self).__init__(register,
            'ISFORCEDRADIO', 'RFFPLL0.STATUS.ISFORCEDRADIO', 'read-only',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_ISFORCEDSYS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_ISFORCEDSYS, self).__init__(register,
            'ISFORCEDSYS', 'RFFPLL0.STATUS.ISFORCEDSYS', 'read-only',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_STATUS_LOCK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_STATUS_LOCK, self).__init__(register,
            'LOCK', 'RFFPLL0.STATUS.LOCK', 'read-only',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASFSMCFG_WAIT0P1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASFSMCFG_WAIT0P1, self).__init__(register,
            'WAIT0P1', 'RFFPLL0.RFBIASFSMCFG.WAIT0P1', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASFSMCFG_WAIT10P0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASFSMCFG_WAIT10P0, self).__init__(register,
            'WAIT10P0', 'RFFPLL0.RFBIASFSMCFG.WAIT10P0', 'read-write',
            u"",
            8, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCTRL_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCTRL_EN, self).__init__(register,
            'EN', 'RFFPLL0.RFBIASCTRL.EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_DISABLEBOOTSTRAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_DISABLEBOOTSTRAP, self).__init__(register,
            'DISABLEBOOTSTRAP', 'RFFPLL0.RFBIASCAL.DISABLEBOOTSTRAP', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_STARTUPCORE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_STARTUPCORE, self).__init__(register,
            'STARTUPCORE', 'RFFPLL0.RFBIASCAL.STARTUPCORE', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_STARTUPSUPPLY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_STARTUPSUPPLY, self).__init__(register,
            'STARTUPSUPPLY', 'RFFPLL0.RFBIASCAL.STARTUPSUPPLY', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_CALBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_CALBIAS, self).__init__(register,
            'CALBIAS', 'RFFPLL0.RFBIASCAL.CALBIAS', 'read-write',
            u"",
            4, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_CALTC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_CALTC, self).__init__(register,
            'CALTC', 'RFFPLL0.RFBIASCAL.CALTC', 'read-write',
            u"",
            10, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_CALVREFSTARTUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_CALVREFSTARTUP, self).__init__(register,
            'CALVREFSTARTUP', 'RFFPLL0.RFBIASCAL.CALVREFSTARTUP', 'read-write',
            u"",
            16, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_LDOVREFTRIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_LDOVREFTRIM, self).__init__(register,
            'LDOVREFTRIM', 'RFFPLL0.RFBIASCAL.LDOVREFTRIM', 'read-write',
            u"",
            22, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL_CALVREF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL_CALVREF, self).__init__(register,
            'CALVREF', 'RFFPLL0.RFBIASCAL.CALVREF', 'read-write',
            u"",
            26, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL1_NONFLASHMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL1_NONFLASHMODE, self).__init__(register,
            'NONFLASHMODE', 'RFFPLL0.RFBIASCAL1.NONFLASHMODE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFBIASCAL1_LDOHIGHCURRENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFBIASCAL1_LDOHIGHCURRENT, self).__init__(register,
            'LDOHIGHCURRENT', 'RFFPLL0.RFBIASCAL1.LDOHIGHCURRENT', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT0P1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT0P1, self).__init__(register,
            'WAIT0P1', 'RFFPLL0.RFFPLLFSMCFG.WAIT0P1', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT2P0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT2P0, self).__init__(register,
            'WAIT2P0', 'RFFPLL0.RFFPLLFSMCFG.WAIT2P0', 'read-write',
            u"",
            12, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT6P0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT6P0, self).__init__(register,
            'WAIT6P0', 'RFFPLL0.RFFPLLFSMCFG.WAIT6P0', 'read-write',
            u"",
            20, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_RESYNCEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_RESYNCEN, self).__init__(register,
            'RESYNCEN', 'RFFPLL0.RFFPLLCTRL.RESYNCEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_DIVYEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_DIVYEN, self).__init__(register,
            'DIVYEN', 'RFFPLL0.RFFPLLCTRL.DIVYEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_DIVXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_DIVXEN, self).__init__(register,
            'DIVXEN', 'RFFPLL0.RFFPLLCTRL.DIVXEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_PFDEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_PFDEN, self).__init__(register,
            'PFDEN', 'RFFPLL0.RFFPLLCTRL.PFDEN', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_REFDIVEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_REFDIVEN, self).__init__(register,
            'REFDIVEN', 'RFFPLL0.RFFPLLCTRL.REFDIVEN', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_FBDIVEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_FBDIVEN, self).__init__(register,
            'FBDIVEN', 'RFFPLL0.RFFPLLCTRL.FBDIVEN', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_CHPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_CHPEN, self).__init__(register,
            'CHPEN', 'RFFPLL0.RFFPLLCTRL.CHPEN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_VCOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_VCOEN, self).__init__(register,
            'VCOEN', 'RFFPLL0.RFFPLLCTRL.VCOEN', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_LDO1EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_LDO1EN, self).__init__(register,
            'LDO1EN', 'RFFPLL0.RFFPLLCTRL.LDO1EN', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_LDO2EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_LDO2EN, self).__init__(register,
            'LDO2EN', 'RFFPLL0.RFFPLLCTRL.LDO2EN', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_LDO3EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_LDO3EN, self).__init__(register,
            'LDO3EN', 'RFFPLL0.RFFPLLCTRL.LDO3EN', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_VALID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_VALID, self).__init__(register,
            'VALID', 'RFFPLL0.RFFPLLCTRL.VALID', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_ADCBUFFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_ADCBUFFEN, self).__init__(register,
            'ADCBUFFEN', 'RFFPLL0.RFFPLLCTRL.ADCBUFFEN', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_DACBUFFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_DACBUFFEN, self).__init__(register,
            'DACBUFFEN', 'RFFPLL0.RFFPLLCTRL.DACBUFFEN', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_MODEMBUFFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_MODEMBUFFEN, self).__init__(register,
            'MODEMBUFFEN', 'RFFPLL0.RFFPLLCTRL.MODEMBUFFEN', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL_DIGBUFFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL_DIGBUFFEN, self).__init__(register,
            'DIGBUFFEN', 'RFFPLL0.RFFPLLCTRL.DIGBUFFEN', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVN, self).__init__(register,
            'DIVN', 'RFFPLL0.RFFPLLCTRL1.DIVN', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVX, self).__init__(register,
            'DIVX', 'RFFPLL0.RFFPLLCTRL1.DIVX', 'read-write',
            u"",
            12, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVY, self).__init__(register,
            'DIVY', 'RFFPLL0.RFFPLLCTRL1.DIVY', 'read-write',
            u"",
            16, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXDACSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXDACSEL, self).__init__(register,
            'DIVXDACSEL', 'RFFPLL0.RFFPLLCTRL1.DIVXDACSEL', 'read-write',
            u"",
            24, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXMODEMSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXMODEMSEL, self).__init__(register,
            'DIVXMODEMSEL', 'RFFPLL0.RFFPLLCTRL1.DIVXMODEMSEL', 'read-write',
            u"",
            28, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXADCSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXADCSEL, self).__init__(register,
            'DIVXADCSEL', 'RFFPLL0.RFFPLLCTRL1.DIVXADCSEL', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCTRL1_DIVYSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCTRL1_DIVYSEL, self).__init__(register,
            'DIVYSEL', 'RFFPLL0.RFFPLLCTRL1.DIVYSEL', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_PFDRSTADJ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_PFDRSTADJ, self).__init__(register,
            'PFDRSTADJ', 'RFFPLL0.RFFPLLCAL.PFDRSTADJ', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_LDO1VCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_LDO1VCAL, self).__init__(register,
            'LDO1VCAL', 'RFFPLL0.RFFPLLCAL.LDO1VCAL', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_LDO2VCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_LDO2VCAL, self).__init__(register,
            'LDO2VCAL', 'RFFPLL0.RFFPLLCAL.LDO2VCAL', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_LDO2ICAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_LDO2ICAL, self).__init__(register,
            'LDO2ICAL', 'RFFPLL0.RFFPLLCAL.LDO2ICAL', 'read-write',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_LDO3VCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_LDO3VCAL, self).__init__(register,
            'LDO3VCAL', 'RFFPLL0.RFFPLLCAL.LDO3VCAL', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_VCOFCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_VCOFCAL, self).__init__(register,
            'VCOFCAL', 'RFFPLL0.RFFPLLCAL.VCOFCAL', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_LPFCZADJ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_LPFCZADJ, self).__init__(register,
            'LPFCZADJ', 'RFFPLL0.RFFPLLCAL.LPFCZADJ', 'read-write',
            u"",
            14, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_LPFRZADJ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_LPFRZADJ, self).__init__(register,
            'LPFRZADJ', 'RFFPLL0.RFFPLLCAL.LPFRZADJ', 'read-write',
            u"",
            16, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_ICPICAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_ICPICAL, self).__init__(register,
            'ICPICAL', 'RFFPLL0.RFFPLLCAL.ICPICAL', 'read-write',
            u"",
            18, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_ADCPHASEFLIP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_ADCPHASEFLIP, self).__init__(register,
            'ADCPHASEFLIP', 'RFFPLL0.RFFPLLCAL.ADCPHASEFLIP', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL_DIGPHASEFLIP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL_DIGPHASEFLIP, self).__init__(register,
            'DIGPHASEFLIP', 'RFFPLL0.RFFPLLCAL.DIGPHASEFLIP', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLCAL1_DIVR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLCAL1_DIVR, self).__init__(register,
            'DIVR', 'RFFPLL0.RFFPLLCAL1.DIVR', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_RFFPLLSTATUS_ACKVALID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_RFFPLLSTATUS_ACKVALID, self).__init__(register,
            'ACKVALID', 'RFFPLL0.RFFPLLSTATUS.ACKVALID', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_IF_RFFPLLRADIORDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_IF_RFFPLLRADIORDY, self).__init__(register,
            'RFFPLLRADIORDY', 'RFFPLL0.IF.RFFPLLRADIORDY', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_IF_RFFPLLSYSRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_IF_RFFPLLSYSRDY, self).__init__(register,
            'RFFPLLSYSRDY', 'RFFPLL0.IF.RFFPLLSYSRDY', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_IEN_RFFPLLRADIORDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_IEN_RFFPLLRADIORDY, self).__init__(register,
            'RFFPLLRADIORDY', 'RFFPLL0.IEN.RFFPLLRADIORDY', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_IEN_RFFPLLSYSRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_IEN_RFFPLLSYSRDY, self).__init__(register,
            'RFFPLLSYSRDY', 'RFFPLL0.IEN.RFFPLLSYSRDY', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RFFPLL0_LOCK_LOCKKEY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RFFPLL0_LOCK_LOCKKEY, self).__init__(register,
            'LOCKKEY', 'RFFPLL0.LOCK.LOCKKEY', 'write-only',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


