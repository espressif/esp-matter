
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SUNFSK_CFG1_MODULATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_MODULATION, self).__init__(register,
            'MODULATION', 'SUNFSK.CFG1.MODULATION', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG1_FECSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_FECSEL, self).__init__(register,
            'FECSEL', 'SUNFSK.CFG1.FECSEL', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG1_PHYSUNFSKSFD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_PHYSUNFSKSFD, self).__init__(register,
            'PHYSUNFSKSFD', 'SUNFSK.CFG1.PHYSUNFSKSFD', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG1_MODSCHEME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_MODSCHEME, self).__init__(register,
            'MODSCHEME', 'SUNFSK.CFG1.MODSCHEME', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG1_BT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_BT, self).__init__(register,
            'BT', 'SUNFSK.CFG1.BT', 'read-write',
            u"",
            14, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG1_MODINDEX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_MODINDEX, self).__init__(register,
            'MODINDEX', 'SUNFSK.CFG1.MODINDEX', 'read-write',
            u"",
            16, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG1_FSKPREAMBLELENGTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG1_FSKPREAMBLELENGTH, self).__init__(register,
            'FSKPREAMBLELENGTH', 'SUNFSK.CFG1.FSKPREAMBLELENGTH', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG2_VEQEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG2_VEQEN, self).__init__(register,
            'VEQEN', 'SUNFSK.CFG2.VEQEN', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG2_KSI1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG2_KSI1, self).__init__(register,
            'KSI1', 'SUNFSK.CFG2.KSI1', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG2_KSI2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG2_KSI2, self).__init__(register,
            'KSI2', 'SUNFSK.CFG2.KSI2', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG2_KSI3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG2_KSI3, self).__init__(register,
            'KSI3', 'SUNFSK.CFG2.KSI3', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG3_KSI3W(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG3_KSI3W, self).__init__(register,
            'KSI3W', 'SUNFSK.CFG3.KSI3W', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG3_PHASESCALE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG3_PHASESCALE, self).__init__(register,
            'PHASESCALE', 'SUNFSK.CFG3.PHASESCALE', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG3_OSR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG3_OSR, self).__init__(register,
            'OSR', 'SUNFSK.CFG3.OSR', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG4_PREAMBLECOSTTHD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG4_PREAMBLECOSTTHD, self).__init__(register,
            'PREAMBLECOSTTHD', 'SUNFSK.CFG4.PREAMBLECOSTTHD', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG4_PREAMBLECNTWIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG4_PREAMBLECNTWIN, self).__init__(register,
            'PREAMBLECNTWIN', 'SUNFSK.CFG4.PREAMBLECNTWIN', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG4_PREAMBLECNTTHD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG4_PREAMBLECNTTHD, self).__init__(register,
            'PREAMBLECNTTHD', 'SUNFSK.CFG4.PREAMBLECNTTHD', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG5_PREAMBLELEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG5_PREAMBLELEN, self).__init__(register,
            'PREAMBLELEN', 'SUNFSK.CFG5.PREAMBLELEN', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG5_PREAMBLEERRORS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG5_PREAMBLEERRORS, self).__init__(register,
            'PREAMBLEERRORS', 'SUNFSK.CFG5.PREAMBLEERRORS', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG5_PREAMBLETIMEOUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG5_PREAMBLETIMEOUT, self).__init__(register,
            'PREAMBLETIMEOUT', 'SUNFSK.CFG5.PREAMBLETIMEOUT', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG6_PREAMBLEPATT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG6_PREAMBLEPATT, self).__init__(register,
            'PREAMBLEPATT', 'SUNFSK.CFG6.PREAMBLEPATT', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG7_PREAMBLEFREQLIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG7_PREAMBLEFREQLIM, self).__init__(register,
            'PREAMBLEFREQLIM', 'SUNFSK.CFG7.PREAMBLEFREQLIM', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG7_PREAMBLECOSTMAX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG7_PREAMBLECOSTMAX, self).__init__(register,
            'PREAMBLECOSTMAX', 'SUNFSK.CFG7.PREAMBLECOSTMAX', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG8_SFD1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG8_SFD1, self).__init__(register,
            'SFD1', 'SUNFSK.CFG8.SFD1', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG9_SFD2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG9_SFD2, self).__init__(register,
            'SFD2', 'SUNFSK.CFG9.SFD2', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG10_SFDLEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG10_SFDLEN, self).__init__(register,
            'SFDLEN', 'SUNFSK.CFG10.SFDLEN', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG10_SFDCOSTTHD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG10_SFDCOSTTHD, self).__init__(register,
            'SFDCOSTTHD', 'SUNFSK.CFG10.SFDCOSTTHD', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG11_SFDTIMEOUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG11_SFDTIMEOUT, self).__init__(register,
            'SFDTIMEOUT', 'SUNFSK.CFG11.SFDTIMEOUT', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG11_SFDERRORS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG11_SFDERRORS, self).__init__(register,
            'SFDERRORS', 'SUNFSK.CFG11.SFDERRORS', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG11_TIMINGTRACKGEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG11_TIMINGTRACKGEAR, self).__init__(register,
            'TIMINGTRACKGEAR', 'SUNFSK.CFG11.TIMINGTRACKGEAR', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG12_TIMINGTRACKSYMB(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG12_TIMINGTRACKSYMB, self).__init__(register,
            'TIMINGTRACKSYMB', 'SUNFSK.CFG12.TIMINGTRACKSYMB', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG12_TIMINGMINTRANS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG12_TIMINGMINTRANS, self).__init__(register,
            'TIMINGMINTRANS', 'SUNFSK.CFG12.TIMINGMINTRANS', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG12_REALTIMESEARCH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG12_REALTIMESEARCH, self).__init__(register,
            'REALTIMESEARCH', 'SUNFSK.CFG12.REALTIMESEARCH', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG12_NBSYMBBATCH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG12_NBSYMBBATCH, self).__init__(register,
            'NBSYMBBATCH', 'SUNFSK.CFG12.NBSYMBBATCH', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG13_CHFILTERSWEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG13_CHFILTERSWEN, self).__init__(register,
            'CHFILTERSWEN', 'SUNFSK.CFG13.CHFILTERSWEN', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG13_AFCMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG13_AFCMODE, self).__init__(register,
            'AFCMODE', 'SUNFSK.CFG13.AFCMODE', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG13_AFCSCALE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG13_AFCSCALE, self).__init__(register,
            'AFCSCALE', 'SUNFSK.CFG13.AFCSCALE', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG14_AFCADJLIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG14_AFCADJLIM, self).__init__(register,
            'AFCADJLIM', 'SUNFSK.CFG14.AFCADJLIM', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG15_AFCADJPERIOD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG15_AFCADJPERIOD, self).__init__(register,
            'AFCADJPERIOD', 'SUNFSK.CFG15.AFCADJPERIOD', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG15_AFCCOSTTHD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG15_AFCCOSTTHD, self).__init__(register,
            'AFCCOSTTHD', 'SUNFSK.CFG15.AFCCOSTTHD', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG15_XTALFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG15_XTALFREQ, self).__init__(register,
            'XTALFREQ', 'SUNFSK.CFG15.XTALFREQ', 'read-write',
            u"",
            16, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_SUNFSK_CFG16_MODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SUNFSK_CFG16_MODE, self).__init__(register,
            'MODE', 'SUNFSK.CFG16.MODE', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


