
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . RFFPLL0_field import *


class RM_Register_RFFPLL0_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_IPVERSION, self).__init__(rmio, label,
            0x400c8000, 0x000,
            'IPVERSION', 'RFFPLL0.IPVERSION', 'read-only',
            u"",
            0x00000001, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_RFFPLL0_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_CFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_CFG, self).__init__(rmio, label,
            0x400c8000, 0x010,
            'CFG', 'RFFPLL0.CFG', 'read-write',
            u"",
            0x00000000, 0xC0000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.DISFSMRFBIAS = RM_Field_RFFPLL0_CFG_DISFSMRFBIAS(self)
        self.zz_fdict['DISFSMRFBIAS'] = self.DISFSMRFBIAS
        self.DISFSMRFFPLL = RM_Field_RFFPLL0_CFG_DISFSMRFFPLL(self)
        self.zz_fdict['DISFSMRFFPLL'] = self.DISFSMRFFPLL
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_CTRL, self).__init__(rmio, label,
            0x400c8000, 0x014,
            'CTRL', 'RFFPLL0.CTRL', 'read-write',
            u"",
            0x00000000, 0x80000103,
            0x00001000, 0x00002000,
            0x00003000)

        self.FORCEENRFBIAS = RM_Field_RFFPLL0_CTRL_FORCEENRFBIAS(self)
        self.zz_fdict['FORCEENRFBIAS'] = self.FORCEENRFBIAS
        self.FORCEEN = RM_Field_RFFPLL0_CTRL_FORCEEN(self)
        self.zz_fdict['FORCEEN'] = self.FORCEEN
        self.DISONDEMAND = RM_Field_RFFPLL0_CTRL_DISONDEMAND(self)
        self.zz_fdict['DISONDEMAND'] = self.DISONDEMAND
        self.FORCERAWCLK = RM_Field_RFFPLL0_CTRL_FORCERAWCLK(self)
        self.zz_fdict['FORCERAWCLK'] = self.FORCERAWCLK
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_STATUS, self).__init__(rmio, label,
            0x400c8000, 0x020,
            'STATUS', 'RFFPLL0.STATUS', 'read-only',
            u"",
            0x00000000, 0x800F0113,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFFPLLRADIORDY = RM_Field_RFFPLL0_STATUS_RFFPLLRADIORDY(self)
        self.zz_fdict['RFFPLLRADIORDY'] = self.RFFPLLRADIORDY
        self.RFFPLLSYSRDY = RM_Field_RFFPLL0_STATUS_RFFPLLSYSRDY(self)
        self.zz_fdict['RFFPLLSYSRDY'] = self.RFFPLLSYSRDY
        self.RFBIASRDY = RM_Field_RFFPLL0_STATUS_RFBIASRDY(self)
        self.zz_fdict['RFBIASRDY'] = self.RFBIASRDY
        self.ENS = RM_Field_RFFPLL0_STATUS_ENS(self)
        self.zz_fdict['ENS'] = self.ENS
        self.HWREQRADIO = RM_Field_RFFPLL0_STATUS_HWREQRADIO(self)
        self.zz_fdict['HWREQRADIO'] = self.HWREQRADIO
        self.HWREQSYS = RM_Field_RFFPLL0_STATUS_HWREQSYS(self)
        self.zz_fdict['HWREQSYS'] = self.HWREQSYS
        self.ISFORCEDRADIO = RM_Field_RFFPLL0_STATUS_ISFORCEDRADIO(self)
        self.zz_fdict['ISFORCEDRADIO'] = self.ISFORCEDRADIO
        self.ISFORCEDSYS = RM_Field_RFFPLL0_STATUS_ISFORCEDSYS(self)
        self.zz_fdict['ISFORCEDSYS'] = self.ISFORCEDSYS
        self.LOCK = RM_Field_RFFPLL0_STATUS_LOCK(self)
        self.zz_fdict['LOCK'] = self.LOCK
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFBIASFSMCFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFBIASFSMCFG, self).__init__(rmio, label,
            0x400c8000, 0x030,
            'RFBIASFSMCFG', 'RFFPLL0.RFBIASFSMCFG', 'read-write',
            u"",
            0x0000FA03, 0x0001FF07,
            0x00001000, 0x00002000,
            0x00003000)

        self.WAIT0P1 = RM_Field_RFFPLL0_RFBIASFSMCFG_WAIT0P1(self)
        self.zz_fdict['WAIT0P1'] = self.WAIT0P1
        self.WAIT10P0 = RM_Field_RFFPLL0_RFBIASFSMCFG_WAIT10P0(self)
        self.zz_fdict['WAIT10P0'] = self.WAIT10P0
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFBIASCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFBIASCTRL, self).__init__(rmio, label,
            0x400c8000, 0x034,
            'RFBIASCTRL', 'RFFPLL0.RFBIASCTRL', 'read-write',
            u"",
            0x00000001, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_RFFPLL0_RFBIASCTRL_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFBIASCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFBIASCAL, self).__init__(rmio, label,
            0x400c8000, 0x038,
            'RFBIASCAL', 'RFFPLL0.RFBIASCAL', 'read-write',
            u"",
            0x81306A00, 0xFFFFFFF7,
            0x00001000, 0x00002000,
            0x00003000)

        self.DISABLEBOOTSTRAP = RM_Field_RFFPLL0_RFBIASCAL_DISABLEBOOTSTRAP(self)
        self.zz_fdict['DISABLEBOOTSTRAP'] = self.DISABLEBOOTSTRAP
        self.STARTUPCORE = RM_Field_RFFPLL0_RFBIASCAL_STARTUPCORE(self)
        self.zz_fdict['STARTUPCORE'] = self.STARTUPCORE
        self.STARTUPSUPPLY = RM_Field_RFFPLL0_RFBIASCAL_STARTUPSUPPLY(self)
        self.zz_fdict['STARTUPSUPPLY'] = self.STARTUPSUPPLY
        self.CALBIAS = RM_Field_RFFPLL0_RFBIASCAL_CALBIAS(self)
        self.zz_fdict['CALBIAS'] = self.CALBIAS
        self.CALTC = RM_Field_RFFPLL0_RFBIASCAL_CALTC(self)
        self.zz_fdict['CALTC'] = self.CALTC
        self.CALVREFSTARTUP = RM_Field_RFFPLL0_RFBIASCAL_CALVREFSTARTUP(self)
        self.zz_fdict['CALVREFSTARTUP'] = self.CALVREFSTARTUP
        self.LDOVREFTRIM = RM_Field_RFFPLL0_RFBIASCAL_LDOVREFTRIM(self)
        self.zz_fdict['LDOVREFTRIM'] = self.LDOVREFTRIM
        self.CALVREF = RM_Field_RFFPLL0_RFBIASCAL_CALVREF(self)
        self.zz_fdict['CALVREF'] = self.CALVREF
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFBIASCAL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFBIASCAL1, self).__init__(rmio, label,
            0x400c8000, 0x03C,
            'RFBIASCAL1', 'RFFPLL0.RFBIASCAL1', 'read-write',
            u"",
            0x00000002, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.NONFLASHMODE = RM_Field_RFFPLL0_RFBIASCAL1_NONFLASHMODE(self)
        self.zz_fdict['NONFLASHMODE'] = self.NONFLASHMODE
        self.LDOHIGHCURRENT = RM_Field_RFFPLL0_RFBIASCAL1_LDOHIGHCURRENT(self)
        self.zz_fdict['LDOHIGHCURRENT'] = self.LDOHIGHCURRENT
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFFPLLFSMCFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFFPLLFSMCFG, self).__init__(rmio, label,
            0x400c8000, 0x050,
            'RFFPLLFSMCFG', 'RFFPLL0.RFFPLLFSMCFG', 'read-write',
            u"",
            0x09632003, 0x1FF7F007,
            0x00001000, 0x00002000,
            0x00003000)

        self.WAIT0P1 = RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT0P1(self)
        self.zz_fdict['WAIT0P1'] = self.WAIT0P1
        self.WAIT2P0 = RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT2P0(self)
        self.zz_fdict['WAIT2P0'] = self.WAIT2P0
        self.WAIT6P0 = RM_Field_RFFPLL0_RFFPLLFSMCFG_WAIT6P0(self)
        self.zz_fdict['WAIT6P0'] = self.WAIT6P0
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFFPLLCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFFPLLCTRL, self).__init__(rmio, label,
            0x400c8000, 0x054,
            'RFFPLLCTRL', 'RFFPLL0.RFFPLLCTRL', 'read-write',
            u"",
            0x0003DFE6, 0x0003FFE7,
            0x00001000, 0x00002000,
            0x00003000)

        self.RESYNCEN = RM_Field_RFFPLL0_RFFPLLCTRL_RESYNCEN(self)
        self.zz_fdict['RESYNCEN'] = self.RESYNCEN
        self.DIVYEN = RM_Field_RFFPLL0_RFFPLLCTRL_DIVYEN(self)
        self.zz_fdict['DIVYEN'] = self.DIVYEN
        self.DIVXEN = RM_Field_RFFPLL0_RFFPLLCTRL_DIVXEN(self)
        self.zz_fdict['DIVXEN'] = self.DIVXEN
        self.PFDEN = RM_Field_RFFPLL0_RFFPLLCTRL_PFDEN(self)
        self.zz_fdict['PFDEN'] = self.PFDEN
        self.REFDIVEN = RM_Field_RFFPLL0_RFFPLLCTRL_REFDIVEN(self)
        self.zz_fdict['REFDIVEN'] = self.REFDIVEN
        self.FBDIVEN = RM_Field_RFFPLL0_RFFPLLCTRL_FBDIVEN(self)
        self.zz_fdict['FBDIVEN'] = self.FBDIVEN
        self.CHPEN = RM_Field_RFFPLL0_RFFPLLCTRL_CHPEN(self)
        self.zz_fdict['CHPEN'] = self.CHPEN
        self.VCOEN = RM_Field_RFFPLL0_RFFPLLCTRL_VCOEN(self)
        self.zz_fdict['VCOEN'] = self.VCOEN
        self.LDO1EN = RM_Field_RFFPLL0_RFFPLLCTRL_LDO1EN(self)
        self.zz_fdict['LDO1EN'] = self.LDO1EN
        self.LDO2EN = RM_Field_RFFPLL0_RFFPLLCTRL_LDO2EN(self)
        self.zz_fdict['LDO2EN'] = self.LDO2EN
        self.LDO3EN = RM_Field_RFFPLL0_RFFPLLCTRL_LDO3EN(self)
        self.zz_fdict['LDO3EN'] = self.LDO3EN
        self.VALID = RM_Field_RFFPLL0_RFFPLLCTRL_VALID(self)
        self.zz_fdict['VALID'] = self.VALID
        self.ADCBUFFEN = RM_Field_RFFPLL0_RFFPLLCTRL_ADCBUFFEN(self)
        self.zz_fdict['ADCBUFFEN'] = self.ADCBUFFEN
        self.DACBUFFEN = RM_Field_RFFPLL0_RFFPLLCTRL_DACBUFFEN(self)
        self.zz_fdict['DACBUFFEN'] = self.DACBUFFEN
        self.MODEMBUFFEN = RM_Field_RFFPLL0_RFFPLLCTRL_MODEMBUFFEN(self)
        self.zz_fdict['MODEMBUFFEN'] = self.MODEMBUFFEN
        self.DIGBUFFEN = RM_Field_RFFPLL0_RFFPLLCTRL_DIGBUFFEN(self)
        self.zz_fdict['DIGBUFFEN'] = self.DIGBUFFEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFFPLLCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFFPLLCTRL1, self).__init__(rmio, label,
            0x400c8000, 0x058,
            'RFFPLLCTRL1', 'RFFPLL0.RFFPLLCTRL1', 'read-write',
            u"",
            0x17116057, 0xFF1FF07F,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIVN = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVN(self)
        self.zz_fdict['DIVN'] = self.DIVN
        self.DIVX = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVX(self)
        self.zz_fdict['DIVX'] = self.DIVX
        self.DIVY = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVY(self)
        self.zz_fdict['DIVY'] = self.DIVY
        self.DIVXDACSEL = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXDACSEL(self)
        self.zz_fdict['DIVXDACSEL'] = self.DIVXDACSEL
        self.DIVXMODEMSEL = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXMODEMSEL(self)
        self.zz_fdict['DIVXMODEMSEL'] = self.DIVXMODEMSEL
        self.DIVXADCSEL = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVXADCSEL(self)
        self.zz_fdict['DIVXADCSEL'] = self.DIVXADCSEL
        self.DIVYSEL = RM_Field_RFFPLL0_RFFPLLCTRL1_DIVYSEL(self)
        self.zz_fdict['DIVYSEL'] = self.DIVYSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFFPLLCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFFPLLCAL, self).__init__(rmio, label,
            0x400c8000, 0x060,
            'RFFPLLCAL', 'RFFPLL0.RFFPLLCAL', 'read-write',
            u"",
            0x400AA2A8, 0xC00FF3FD,
            0x00001000, 0x00002000,
            0x00003000)

        self.PFDRSTADJ = RM_Field_RFFPLL0_RFFPLLCAL_PFDRSTADJ(self)
        self.zz_fdict['PFDRSTADJ'] = self.PFDRSTADJ
        self.LDO1VCAL = RM_Field_RFFPLL0_RFFPLLCAL_LDO1VCAL(self)
        self.zz_fdict['LDO1VCAL'] = self.LDO1VCAL
        self.LDO2VCAL = RM_Field_RFFPLL0_RFFPLLCAL_LDO2VCAL(self)
        self.zz_fdict['LDO2VCAL'] = self.LDO2VCAL
        self.LDO2ICAL = RM_Field_RFFPLL0_RFFPLLCAL_LDO2ICAL(self)
        self.zz_fdict['LDO2ICAL'] = self.LDO2ICAL
        self.LDO3VCAL = RM_Field_RFFPLL0_RFFPLLCAL_LDO3VCAL(self)
        self.zz_fdict['LDO3VCAL'] = self.LDO3VCAL
        self.VCOFCAL = RM_Field_RFFPLL0_RFFPLLCAL_VCOFCAL(self)
        self.zz_fdict['VCOFCAL'] = self.VCOFCAL
        self.LPFCZADJ = RM_Field_RFFPLL0_RFFPLLCAL_LPFCZADJ(self)
        self.zz_fdict['LPFCZADJ'] = self.LPFCZADJ
        self.LPFRZADJ = RM_Field_RFFPLL0_RFFPLLCAL_LPFRZADJ(self)
        self.zz_fdict['LPFRZADJ'] = self.LPFRZADJ
        self.ICPICAL = RM_Field_RFFPLL0_RFFPLLCAL_ICPICAL(self)
        self.zz_fdict['ICPICAL'] = self.ICPICAL
        self.ADCPHASEFLIP = RM_Field_RFFPLL0_RFFPLLCAL_ADCPHASEFLIP(self)
        self.zz_fdict['ADCPHASEFLIP'] = self.ADCPHASEFLIP
        self.DIGPHASEFLIP = RM_Field_RFFPLL0_RFFPLLCAL_DIGPHASEFLIP(self)
        self.zz_fdict['DIGPHASEFLIP'] = self.DIGPHASEFLIP
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFFPLLCAL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFFPLLCAL1, self).__init__(rmio, label,
            0x400c8000, 0x064,
            'RFFPLLCAL1', 'RFFPLL0.RFFPLLCAL1', 'read-write',
            u"",
            0x00000200, 0x00000700,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIVR = RM_Field_RFFPLL0_RFFPLLCAL1_DIVR(self)
        self.zz_fdict['DIVR'] = self.DIVR
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_RFFPLLSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_RFFPLLSTATUS, self).__init__(rmio, label,
            0x400c8000, 0x070,
            'RFFPLLSTATUS', 'RFFPLL0.RFFPLLSTATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.ACKVALID = RM_Field_RFFPLL0_RFFPLLSTATUS_ACKVALID(self)
        self.zz_fdict['ACKVALID'] = self.ACKVALID
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_IF, self).__init__(rmio, label,
            0x400c8000, 0x080,
            'IF', 'RFFPLL0.IF', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFFPLLRADIORDY = RM_Field_RFFPLL0_IF_RFFPLLRADIORDY(self)
        self.zz_fdict['RFFPLLRADIORDY'] = self.RFFPLLRADIORDY
        self.RFFPLLSYSRDY = RM_Field_RFFPLL0_IF_RFFPLLSYSRDY(self)
        self.zz_fdict['RFFPLLSYSRDY'] = self.RFFPLLSYSRDY
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_IEN, self).__init__(rmio, label,
            0x400c8000, 0x084,
            'IEN', 'RFFPLL0.IEN', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFFPLLRADIORDY = RM_Field_RFFPLL0_IEN_RFFPLLRADIORDY(self)
        self.zz_fdict['RFFPLLRADIORDY'] = self.RFFPLLRADIORDY
        self.RFFPLLSYSRDY = RM_Field_RFFPLL0_IEN_RFFPLLSYSRDY(self)
        self.zz_fdict['RFFPLLSYSRDY'] = self.RFFPLLSYSRDY
        self.__dict__['zz_frozen'] = True


class RM_Register_RFFPLL0_LOCK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFFPLL0_LOCK, self).__init__(rmio, label,
            0x400c8000, 0x0B0,
            'LOCK', 'RFFPLL0.LOCK', 'write-only',
            u"",
            0x000085A9, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCKKEY = RM_Field_RFFPLL0_LOCK_LOCKKEY(self)
        self.zz_fdict['LOCKKEY'] = self.LOCKKEY
        self.__dict__['zz_frozen'] = True


