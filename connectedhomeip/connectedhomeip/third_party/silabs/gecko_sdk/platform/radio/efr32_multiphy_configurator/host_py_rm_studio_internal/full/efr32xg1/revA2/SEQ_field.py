
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


class RM_Field_SEQ_PHYINFO_ADDRESS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_PHYINFO_ADDRESS, self).__init__(register,
            'ADDRESS', 'SEQ.PHYINFO.ADDRESS', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_SQBMODETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_SQBMODETX, self).__init__(register,
            'SQBMODETX', 'SEQ.MISC.SQBMODETX', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN, self).__init__(register,
            'DYNAMIC_SLICER_SW_EN', 'SEQ.MISC.DYNAMIC_SLICER_SW_EN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTHLPFCTRLRX_SYNTHLPFCTRLRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTHLPFCTRLRX_SYNTHLPFCTRLRX, self).__init__(register,
            'SYNTHLPFCTRLRX', 'SEQ.SYNTHLPFCTRLRX.SYNTHLPFCTRLRX', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX, self).__init__(register,
            'SYNTHLPFCTRLTX', 'SEQ.SYNTHLPFCTRLTX.SYNTHLPFCTRLTX', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


