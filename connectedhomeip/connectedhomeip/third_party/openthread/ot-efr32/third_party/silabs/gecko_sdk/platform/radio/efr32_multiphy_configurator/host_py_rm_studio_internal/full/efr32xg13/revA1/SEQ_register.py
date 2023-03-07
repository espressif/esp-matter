
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SEQ_field import *


class RM_Register_SEQ_BBSS_GROUP_1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_BBSS_GROUP_1, self).__init__(rmio, label,
            0x21001fdc, 0x000,
            'BBSS_GROUP_1', 'SEQ.BBSS_GROUP_1', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.IIR_SHIFT = RM_Field_SEQ_BBSS_GROUP_1_IIR_SHIFT(self)
        self.zz_fdict['IIR_SHIFT'] = self.IIR_SHIFT
        self.BBSS_HYSTERESIS = RM_Field_SEQ_BBSS_GROUP_1_BBSS_HYSTERESIS(self)
        self.zz_fdict['BBSS_HYSTERESIS'] = self.BBSS_HYSTERESIS
        self.BBSS_LEVEL_LOW = RM_Field_SEQ_BBSS_GROUP_1_BBSS_LEVEL_LOW(self)
        self.zz_fdict['BBSS_LEVEL_LOW'] = self.BBSS_LEVEL_LOW
        self.BBS_LEVEL_1_OFFSET = RM_Field_SEQ_BBSS_GROUP_1_BBS_LEVEL_1_OFFSET(self)
        self.zz_fdict['BBS_LEVEL_1_OFFSET'] = self.BBS_LEVEL_1_OFFSET
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_BBSS_GROUP_2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_BBSS_GROUP_2, self).__init__(rmio, label,
            0x21001fdc, 0x004,
            'BBSS_GROUP_2', 'SEQ.BBSS_GROUP_2', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.BBSS_LEVEL_2_OFFSET = RM_Field_SEQ_BBSS_GROUP_2_BBSS_LEVEL_2_OFFSET(self)
        self.zz_fdict['BBSS_LEVEL_2_OFFSET'] = self.BBSS_LEVEL_2_OFFSET
        self.SYNC_THRESH_MID = RM_Field_SEQ_BBSS_GROUP_2_SYNC_THRESH_MID(self)
        self.zz_fdict['SYNC_THRESH_MID'] = self.SYNC_THRESH_MID
        self.BBSS_LEVEL_1_VALUE = RM_Field_SEQ_BBSS_GROUP_2_BBSS_LEVEL_1_VALUE(self)
        self.zz_fdict['BBSS_LEVEL_1_VALUE'] = self.BBSS_LEVEL_1_VALUE
        self.SYNC_THRESH_LOW = RM_Field_SEQ_BBSS_GROUP_2_SYNC_THRESH_LOW(self)
        self.zz_fdict['SYNC_THRESH_LOW'] = self.SYNC_THRESH_LOW
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_BBSS_GROUP_3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_BBSS_GROUP_3, self).__init__(rmio, label,
            0x21001fdc, 0x008,
            'BBSS_GROUP_3', 'SEQ.BBSS_GROUP_3', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.BBSS_LEVEL_3_VALUE = RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_VALUE(self)
        self.zz_fdict['BBSS_LEVEL_3_VALUE'] = self.BBSS_LEVEL_3_VALUE
        self.BBSS_LEVEL_3_OFFSET = RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_OFFSET(self)
        self.zz_fdict['BBSS_LEVEL_3_OFFSET'] = self.BBSS_LEVEL_3_OFFSET
        self.BBSS_TIM_DETECTED_TIM_THRESH = RM_Field_SEQ_BBSS_GROUP_3_BBSS_TIM_DETECTED_TIM_THRESH(self)
        self.zz_fdict['BBSS_TIM_DETECTED_TIM_THRESH'] = self.BBSS_TIM_DETECTED_TIM_THRESH
        self.BBSS_LEVEL_2_VALUE = RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_2_VALUE(self)
        self.zz_fdict['BBSS_LEVEL_2_VALUE'] = self.BBSS_LEVEL_2_VALUE
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_BBSS_GROUP_4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_BBSS_GROUP_4, self).__init__(rmio, label,
            0x21001fdc, 0x00C,
            'BBSS_GROUP_4', 'SEQ.BBSS_GROUP_4', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.DEFAULT_TIM_THRESH = RM_Field_SEQ_BBSS_GROUP_4_DEFAULT_TIM_THRESH(self)
        self.zz_fdict['DEFAULT_TIM_THRESH'] = self.DEFAULT_TIM_THRESH
        self.BBSS_LEVEL_5_VALUE = RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_5_VALUE(self)
        self.zz_fdict['BBSS_LEVEL_5_VALUE'] = self.BBSS_LEVEL_5_VALUE
        self.BBSS_LEVEL_4_OFFSET = RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_OFFSET(self)
        self.zz_fdict['BBSS_LEVEL_4_OFFSET'] = self.BBSS_LEVEL_4_OFFSET
        self.BBSS_LEVEL_4_VALUE = RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_VALUE(self)
        self.zz_fdict['BBSS_LEVEL_4_VALUE'] = self.BBSS_LEVEL_4_VALUE
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_DYNAMIC_CHPWR_TABLE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_DYNAMIC_CHPWR_TABLE, self).__init__(rmio, label,
            0x21001fdc, 0x010,
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
            0x21001fdc, 0x014,
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
            0x21001fdc, 0x018,
            'MISC', 'SEQ.MISC', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.SQBMODETX = RM_Field_SEQ_MISC_SQBMODETX(self)
        self.zz_fdict['SQBMODETX'] = self.SQBMODETX
        self.BLE_VITERBI_FIX_EN = RM_Field_SEQ_MISC_BLE_VITERBI_FIX_EN(self)
        self.zz_fdict['BLE_VITERBI_FIX_EN'] = self.BLE_VITERBI_FIX_EN
        self.PERIODIC_DEMOD_RST_EN = RM_Field_SEQ_MISC_PERIODIC_DEMOD_RST_EN(self)
        self.zz_fdict['PERIODIC_DEMOD_RST_EN'] = self.PERIODIC_DEMOD_RST_EN
        self.DYNAMIC_SLICER_SW_EN = RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN(self)
        self.zz_fdict['DYNAMIC_SLICER_SW_EN'] = self.DYNAMIC_SLICER_SW_EN
        self.DYNAMIC_BBSS_SW_EN = RM_Field_SEQ_MISC_DYNAMIC_BBSS_SW_EN(self)
        self.zz_fdict['DYNAMIC_BBSS_SW_EN'] = self.DYNAMIC_BBSS_SW_EN
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_PHYINFO(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_PHYINFO, self).__init__(rmio, label,
            0x21001fdc, 0x01C,
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
            0x21001fdc, 0x020,
            'PERIODICDEMODRSTPERIOD', 'SEQ.PERIODICDEMODRSTPERIOD', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.PERIOD = RM_Field_SEQ_PERIODICDEMODRSTPERIOD_PERIOD(self)
        self.zz_fdict['PERIOD'] = self.PERIOD
        self.__dict__['zz_frozen'] = True


