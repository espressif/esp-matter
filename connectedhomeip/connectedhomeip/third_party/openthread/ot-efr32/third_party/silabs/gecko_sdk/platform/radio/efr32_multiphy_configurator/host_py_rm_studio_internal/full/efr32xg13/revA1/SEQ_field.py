
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SEQ_BBSS_GROUP_1_IIR_SHIFT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_1_IIR_SHIFT, self).__init__(register,
            'IIR_SHIFT', 'SEQ.BBSS_GROUP_1.IIR_SHIFT', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_1_BBSS_HYSTERESIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_1_BBSS_HYSTERESIS, self).__init__(register,
            'BBSS_HYSTERESIS', 'SEQ.BBSS_GROUP_1.BBSS_HYSTERESIS', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_1_BBSS_LEVEL_LOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_1_BBSS_LEVEL_LOW, self).__init__(register,
            'BBSS_LEVEL_LOW', 'SEQ.BBSS_GROUP_1.BBSS_LEVEL_LOW', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_1_BBS_LEVEL_1_OFFSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_1_BBS_LEVEL_1_OFFSET, self).__init__(register,
            'BBS_LEVEL_1_OFFSET', 'SEQ.BBSS_GROUP_1.BBS_LEVEL_1_OFFSET', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_2_BBSS_LEVEL_2_OFFSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_2_BBSS_LEVEL_2_OFFSET, self).__init__(register,
            'BBSS_LEVEL_2_OFFSET', 'SEQ.BBSS_GROUP_2.BBSS_LEVEL_2_OFFSET', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_2_SYNC_THRESH_MID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_2_SYNC_THRESH_MID, self).__init__(register,
            'SYNC_THRESH_MID', 'SEQ.BBSS_GROUP_2.SYNC_THRESH_MID', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_2_BBSS_LEVEL_1_VALUE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_2_BBSS_LEVEL_1_VALUE, self).__init__(register,
            'BBSS_LEVEL_1_VALUE', 'SEQ.BBSS_GROUP_2.BBSS_LEVEL_1_VALUE', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_2_SYNC_THRESH_LOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_2_SYNC_THRESH_LOW, self).__init__(register,
            'SYNC_THRESH_LOW', 'SEQ.BBSS_GROUP_2.SYNC_THRESH_LOW', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_VALUE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_VALUE, self).__init__(register,
            'BBSS_LEVEL_3_VALUE', 'SEQ.BBSS_GROUP_3.BBSS_LEVEL_3_VALUE', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_OFFSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_OFFSET, self).__init__(register,
            'BBSS_LEVEL_3_OFFSET', 'SEQ.BBSS_GROUP_3.BBSS_LEVEL_3_OFFSET', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_3_BBSS_TIM_DETECTED_TIM_THRESH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_3_BBSS_TIM_DETECTED_TIM_THRESH, self).__init__(register,
            'BBSS_TIM_DETECTED_TIM_THRESH', 'SEQ.BBSS_GROUP_3.BBSS_TIM_DETECTED_TIM_THRESH', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_2_VALUE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_3_BBSS_LEVEL_2_VALUE, self).__init__(register,
            'BBSS_LEVEL_2_VALUE', 'SEQ.BBSS_GROUP_3.BBSS_LEVEL_2_VALUE', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_4_DEFAULT_TIM_THRESH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_4_DEFAULT_TIM_THRESH, self).__init__(register,
            'DEFAULT_TIM_THRESH', 'SEQ.BBSS_GROUP_4.DEFAULT_TIM_THRESH', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_5_VALUE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_5_VALUE, self).__init__(register,
            'BBSS_LEVEL_5_VALUE', 'SEQ.BBSS_GROUP_4.BBSS_LEVEL_5_VALUE', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_OFFSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_OFFSET, self).__init__(register,
            'BBSS_LEVEL_4_OFFSET', 'SEQ.BBSS_GROUP_4.BBSS_LEVEL_4_OFFSET', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_VALUE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_VALUE, self).__init__(register,
            'BBSS_LEVEL_4_VALUE', 'SEQ.BBSS_GROUP_4.BBSS_LEVEL_4_VALUE', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


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


class RM_Field_SEQ_MISC_BLE_VITERBI_FIX_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_BLE_VITERBI_FIX_EN, self).__init__(register,
            'BLE_VITERBI_FIX_EN', 'SEQ.MISC.BLE_VITERBI_FIX_EN', 'read-write',
            u"",
            3, 1)
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


class RM_Field_SEQ_MISC_DYNAMIC_BBSS_SW_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_DYNAMIC_BBSS_SW_EN, self).__init__(register,
            'DYNAMIC_BBSS_SW_EN', 'SEQ.MISC.DYNAMIC_BBSS_SW_EN', 'read-write',
            u"",
            0, 1)
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


