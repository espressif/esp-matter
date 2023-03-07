
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SEQ_DYNAMIC_CHPWR_TABLE_ADDRESS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_DYNAMIC_CHPWR_TABLE_ADDRESS, self).__init__(register,
            'ADDRESS', 'SEQ.DYNAMIC_CHPWR_TABLE.ADDRESS', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTRX, self).__init__(register,
            'DITHERDSMINPUTRX', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS.DITHERDSMINPUTRX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTRX, self).__init__(register,
            'DITHERDSMOUTPUTRX', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS.DITHERDSMOUTPUTRX', 'read-write',
            u"",
            1, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACRX, self).__init__(register,
            'DITHERDACRX', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS.DITHERDACRX', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTTX, self).__init__(register,
            'DITHERDSMINPUTTX', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS.DITHERDSMINPUTTX', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTTX, self).__init__(register,
            'DITHERDSMOUTPUTTX', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS.DITHERDSMOUTPUTTX', 'read-write',
            u"",
            17, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACTX, self).__init__(register,
            'DITHERDACTX', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS.DITHERDACTX', 'read-write',
            u"",
            20, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_SQBMODETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_SQBMODETX, self).__init__(register,
            'SQBMODETX', 'SEQ.MISC.SQBMODETX', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_PERIODIC_DEMOD_RST_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_PERIODIC_DEMOD_RST_EN, self).__init__(register,
            'PERIODIC_DEMOD_RST_EN', 'SEQ.MISC.PERIODIC_DEMOD_RST_EN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN, self).__init__(register,
            'DYNAMIC_SLICER_SW_EN', 'SEQ.MISC.DYNAMIC_SLICER_SW_EN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_PHYINFO_ADDRESS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_PHYINFO_ADDRESS, self).__init__(register,
            'ADDRESS', 'SEQ.PHYINFO.ADDRESS', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_PERIODICDEMODRSTPERIOD_PERIOD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_PERIODICDEMODRSTPERIOD_PERIOD, self).__init__(register,
            'PERIOD', 'SEQ.PERIODICDEMODRSTPERIOD.PERIOD', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


