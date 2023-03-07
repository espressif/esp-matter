
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SUNFSK_field import *


class RM_Register_SUNFSK_CFG1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG1, self).__init__(rmio, label,
            0xb500fc00, 0x000,
            'CFG1', 'SUNFSK.CFG1', 'read-write',
            u"",
            0x00000000, 0xFF3FF7FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODULATION = RM_Field_SUNFSK_CFG1_MODULATION(self)
        self.zz_fdict['MODULATION'] = self.MODULATION
        self.FECSEL = RM_Field_SUNFSK_CFG1_FECSEL(self)
        self.zz_fdict['FECSEL'] = self.FECSEL
        self.PHYSUNFSKSFD = RM_Field_SUNFSK_CFG1_PHYSUNFSKSFD(self)
        self.zz_fdict['PHYSUNFSKSFD'] = self.PHYSUNFSKSFD
        self.MODSCHEME = RM_Field_SUNFSK_CFG1_MODSCHEME(self)
        self.zz_fdict['MODSCHEME'] = self.MODSCHEME
        self.BT = RM_Field_SUNFSK_CFG1_BT(self)
        self.zz_fdict['BT'] = self.BT
        self.MODINDEX = RM_Field_SUNFSK_CFG1_MODINDEX(self)
        self.zz_fdict['MODINDEX'] = self.MODINDEX
        self.FSKPREAMBLELENGTH = RM_Field_SUNFSK_CFG1_FSKPREAMBLELENGTH(self)
        self.zz_fdict['FSKPREAMBLELENGTH'] = self.FSKPREAMBLELENGTH
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG2, self).__init__(rmio, label,
            0xb500fc00, 0x004,
            'CFG2', 'SUNFSK.CFG2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VEQEN = RM_Field_SUNFSK_CFG2_VEQEN(self)
        self.zz_fdict['VEQEN'] = self.VEQEN
        self.KSI1 = RM_Field_SUNFSK_CFG2_KSI1(self)
        self.zz_fdict['KSI1'] = self.KSI1
        self.KSI2 = RM_Field_SUNFSK_CFG2_KSI2(self)
        self.zz_fdict['KSI2'] = self.KSI2
        self.KSI3 = RM_Field_SUNFSK_CFG2_KSI3(self)
        self.zz_fdict['KSI3'] = self.KSI3
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG3, self).__init__(rmio, label,
            0xb500fc00, 0x008,
            'CFG3', 'SUNFSK.CFG3', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.KSI3W = RM_Field_SUNFSK_CFG3_KSI3W(self)
        self.zz_fdict['KSI3W'] = self.KSI3W
        self.PHASESCALE = RM_Field_SUNFSK_CFG3_PHASESCALE(self)
        self.zz_fdict['PHASESCALE'] = self.PHASESCALE
        self.OSR = RM_Field_SUNFSK_CFG3_OSR(self)
        self.zz_fdict['OSR'] = self.OSR
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG4, self).__init__(rmio, label,
            0xb500fc00, 0x00C,
            'CFG4', 'SUNFSK.CFG4', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREAMBLECOSTTHD = RM_Field_SUNFSK_CFG4_PREAMBLECOSTTHD(self)
        self.zz_fdict['PREAMBLECOSTTHD'] = self.PREAMBLECOSTTHD
        self.PREAMBLECNTWIN = RM_Field_SUNFSK_CFG4_PREAMBLECNTWIN(self)
        self.zz_fdict['PREAMBLECNTWIN'] = self.PREAMBLECNTWIN
        self.PREAMBLECNTTHD = RM_Field_SUNFSK_CFG4_PREAMBLECNTTHD(self)
        self.zz_fdict['PREAMBLECNTTHD'] = self.PREAMBLECNTTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG5, self).__init__(rmio, label,
            0xb500fc00, 0x010,
            'CFG5', 'SUNFSK.CFG5', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREAMBLELEN = RM_Field_SUNFSK_CFG5_PREAMBLELEN(self)
        self.zz_fdict['PREAMBLELEN'] = self.PREAMBLELEN
        self.PREAMBLEERRORS = RM_Field_SUNFSK_CFG5_PREAMBLEERRORS(self)
        self.zz_fdict['PREAMBLEERRORS'] = self.PREAMBLEERRORS
        self.PREAMBLETIMEOUT = RM_Field_SUNFSK_CFG5_PREAMBLETIMEOUT(self)
        self.zz_fdict['PREAMBLETIMEOUT'] = self.PREAMBLETIMEOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG6, self).__init__(rmio, label,
            0xb500fc00, 0x014,
            'CFG6', 'SUNFSK.CFG6', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREAMBLEPATT = RM_Field_SUNFSK_CFG6_PREAMBLEPATT(self)
        self.zz_fdict['PREAMBLEPATT'] = self.PREAMBLEPATT
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG7, self).__init__(rmio, label,
            0xb500fc00, 0x018,
            'CFG7', 'SUNFSK.CFG7', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREAMBLEFREQLIM = RM_Field_SUNFSK_CFG7_PREAMBLEFREQLIM(self)
        self.zz_fdict['PREAMBLEFREQLIM'] = self.PREAMBLEFREQLIM
        self.PREAMBLECOSTMAX = RM_Field_SUNFSK_CFG7_PREAMBLECOSTMAX(self)
        self.zz_fdict['PREAMBLECOSTMAX'] = self.PREAMBLECOSTMAX
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG8(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG8, self).__init__(rmio, label,
            0xb500fc00, 0x01C,
            'CFG8', 'SUNFSK.CFG8', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SFD1 = RM_Field_SUNFSK_CFG8_SFD1(self)
        self.zz_fdict['SFD1'] = self.SFD1
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG9(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG9, self).__init__(rmio, label,
            0xb500fc00, 0x020,
            'CFG9', 'SUNFSK.CFG9', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SFD2 = RM_Field_SUNFSK_CFG9_SFD2(self)
        self.zz_fdict['SFD2'] = self.SFD2
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG10(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG10, self).__init__(rmio, label,
            0xb500fc00, 0x024,
            'CFG10', 'SUNFSK.CFG10', 'read-write',
            u"",
            0x00000000, 0xFFFF00FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SFDLEN = RM_Field_SUNFSK_CFG10_SFDLEN(self)
        self.zz_fdict['SFDLEN'] = self.SFDLEN
        self.SFDCOSTTHD = RM_Field_SUNFSK_CFG10_SFDCOSTTHD(self)
        self.zz_fdict['SFDCOSTTHD'] = self.SFDCOSTTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG11(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG11, self).__init__(rmio, label,
            0xb500fc00, 0x028,
            'CFG11', 'SUNFSK.CFG11', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SFDTIMEOUT = RM_Field_SUNFSK_CFG11_SFDTIMEOUT(self)
        self.zz_fdict['SFDTIMEOUT'] = self.SFDTIMEOUT
        self.SFDERRORS = RM_Field_SUNFSK_CFG11_SFDERRORS(self)
        self.zz_fdict['SFDERRORS'] = self.SFDERRORS
        self.TIMINGTRACKGEAR = RM_Field_SUNFSK_CFG11_TIMINGTRACKGEAR(self)
        self.zz_fdict['TIMINGTRACKGEAR'] = self.TIMINGTRACKGEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG12(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG12, self).__init__(rmio, label,
            0xb500fc00, 0x02C,
            'CFG12', 'SUNFSK.CFG12', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TIMINGTRACKSYMB = RM_Field_SUNFSK_CFG12_TIMINGTRACKSYMB(self)
        self.zz_fdict['TIMINGTRACKSYMB'] = self.TIMINGTRACKSYMB
        self.TIMINGMINTRANS = RM_Field_SUNFSK_CFG12_TIMINGMINTRANS(self)
        self.zz_fdict['TIMINGMINTRANS'] = self.TIMINGMINTRANS
        self.REALTIMESEARCH = RM_Field_SUNFSK_CFG12_REALTIMESEARCH(self)
        self.zz_fdict['REALTIMESEARCH'] = self.REALTIMESEARCH
        self.NBSYMBBATCH = RM_Field_SUNFSK_CFG12_NBSYMBBATCH(self)
        self.zz_fdict['NBSYMBBATCH'] = self.NBSYMBBATCH
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG13(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG13, self).__init__(rmio, label,
            0xb500fc00, 0x030,
            'CFG13', 'SUNFSK.CFG13', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHFILTERSWEN = RM_Field_SUNFSK_CFG13_CHFILTERSWEN(self)
        self.zz_fdict['CHFILTERSWEN'] = self.CHFILTERSWEN
        self.AFCMODE = RM_Field_SUNFSK_CFG13_AFCMODE(self)
        self.zz_fdict['AFCMODE'] = self.AFCMODE
        self.AFCSCALE = RM_Field_SUNFSK_CFG13_AFCSCALE(self)
        self.zz_fdict['AFCSCALE'] = self.AFCSCALE
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG14(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG14, self).__init__(rmio, label,
            0xb500fc00, 0x034,
            'CFG14', 'SUNFSK.CFG14', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AFCADJLIM = RM_Field_SUNFSK_CFG14_AFCADJLIM(self)
        self.zz_fdict['AFCADJLIM'] = self.AFCADJLIM
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG15(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG15, self).__init__(rmio, label,
            0xb500fc00, 0x038,
            'CFG15', 'SUNFSK.CFG15', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AFCADJPERIOD = RM_Field_SUNFSK_CFG15_AFCADJPERIOD(self)
        self.zz_fdict['AFCADJPERIOD'] = self.AFCADJPERIOD
        self.AFCCOSTTHD = RM_Field_SUNFSK_CFG15_AFCCOSTTHD(self)
        self.zz_fdict['AFCCOSTTHD'] = self.AFCCOSTTHD
        self.XTALFREQ = RM_Field_SUNFSK_CFG15_XTALFREQ(self)
        self.zz_fdict['XTALFREQ'] = self.XTALFREQ
        self.__dict__['zz_frozen'] = True


class RM_Register_SUNFSK_CFG16(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNFSK_CFG16, self).__init__(rmio, label,
            0xb500fc00, 0x03C,
            'CFG16', 'SUNFSK.CFG16', 'read-write',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODE = RM_Field_SUNFSK_CFG16_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.__dict__['zz_frozen'] = True


