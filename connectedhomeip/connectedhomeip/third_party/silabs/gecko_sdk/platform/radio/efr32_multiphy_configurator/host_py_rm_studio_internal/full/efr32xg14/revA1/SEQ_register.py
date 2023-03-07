
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SEQ_field import *


class RM_Register_SEQ_DYNAMIC_CHPWR_TABLE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_DYNAMIC_CHPWR_TABLE, self).__init__(rmio, label,
            0x21001fec, 0x000,
            'DYNAMIC_CHPWR_TABLE', 'SEQ.DYNAMIC_CHPWR_TABLE', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.ADDRESS = RM_Field_SEQ_DYNAMIC_CHPWR_TABLE_ADDRESS(self)
        self.zz_fdict['ADDRESS'] = self.ADDRESS
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SYNTH_CTRL_DITHER_SETTINGS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SYNTH_CTRL_DITHER_SETTINGS, self).__init__(rmio, label,
            0x21001fec, 0x004,
            'SYNTH_CTRL_DITHER_SETTINGS', 'SEQ.SYNTH_CTRL_DITHER_SETTINGS', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.DITHERDSMINPUTRX = RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTRX(self)
        self.zz_fdict['DITHERDSMINPUTRX'] = self.DITHERDSMINPUTRX
        self.DITHERDSMOUTPUTRX = RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTRX(self)
        self.zz_fdict['DITHERDSMOUTPUTRX'] = self.DITHERDSMOUTPUTRX
        self.DITHERDACRX = RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACRX(self)
        self.zz_fdict['DITHERDACRX'] = self.DITHERDACRX
        self.DITHERDSMINPUTTX = RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTTX(self)
        self.zz_fdict['DITHERDSMINPUTTX'] = self.DITHERDSMINPUTTX
        self.DITHERDSMOUTPUTTX = RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTTX(self)
        self.zz_fdict['DITHERDSMOUTPUTTX'] = self.DITHERDSMOUTPUTTX
        self.DITHERDACTX = RM_Field_SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACTX(self)
        self.zz_fdict['DITHERDACTX'] = self.DITHERDACTX
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_MISC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_MISC, self).__init__(rmio, label,
            0x21001fec, 0x008,
            'MISC', 'SEQ.MISC', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.SQBMODETX = RM_Field_SEQ_MISC_SQBMODETX(self)
        self.zz_fdict['SQBMODETX'] = self.SQBMODETX
        self.PERIODIC_DEMOD_RST_EN = RM_Field_SEQ_MISC_PERIODIC_DEMOD_RST_EN(self)
        self.zz_fdict['PERIODIC_DEMOD_RST_EN'] = self.PERIODIC_DEMOD_RST_EN
        self.DYNAMIC_SLICER_SW_EN = RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN(self)
        self.zz_fdict['DYNAMIC_SLICER_SW_EN'] = self.DYNAMIC_SLICER_SW_EN
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_PHYINFO(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_PHYINFO, self).__init__(rmio, label,
            0x21001fec, 0x00C,
            'PHYINFO', 'SEQ.PHYINFO', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.ADDRESS = RM_Field_SEQ_PHYINFO_ADDRESS(self)
        self.zz_fdict['ADDRESS'] = self.ADDRESS
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_PERIODICDEMODRSTPERIOD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_PERIODICDEMODRSTPERIOD, self).__init__(rmio, label,
            0x21001fec, 0x010,
            'PERIODICDEMODRSTPERIOD', 'SEQ.PERIODICDEMODRSTPERIOD', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.PERIOD = RM_Field_SEQ_PERIODICDEMODRSTPERIOD_PERIOD(self)
        self.zz_fdict['PERIOD'] = self.PERIOD
        self.__dict__['zz_frozen'] = True


