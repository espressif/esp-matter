
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SEQ_field import *


class RM_Register_SEQ_DYNAMIC_CHPWR_TABLE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_DYNAMIC_CHPWR_TABLE, self).__init__(rmio, label,
            0x21000fec, 0x000,
            'DYNAMIC_CHPWR_TABLE', 'SEQ.DYNAMIC_CHPWR_TABLE', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.ADDRESS = RM_Field_SEQ_DYNAMIC_CHPWR_TABLE_ADDRESS(self)
        self.zz_fdict['ADDRESS'] = self.ADDRESS
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_PHYINFO(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_PHYINFO, self).__init__(rmio, label,
            0x21000fec, 0x004,
            'PHYINFO', 'SEQ.PHYINFO', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.ADDRESS = RM_Field_SEQ_PHYINFO_ADDRESS(self)
        self.zz_fdict['ADDRESS'] = self.ADDRESS
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_MISC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_MISC, self).__init__(rmio, label,
            0x21000fec, 0x008,
            'MISC', 'SEQ.MISC', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.SQBMODETX = RM_Field_SEQ_MISC_SQBMODETX(self)
        self.zz_fdict['SQBMODETX'] = self.SQBMODETX
        self.DYNAMIC_SLICER_SW_EN = RM_Field_SEQ_MISC_DYNAMIC_SLICER_SW_EN(self)
        self.zz_fdict['DYNAMIC_SLICER_SW_EN'] = self.DYNAMIC_SLICER_SW_EN
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SYNTHLPFCTRLRX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SYNTHLPFCTRLRX, self).__init__(rmio, label,
            0x21000fec, 0x00C,
            'SYNTHLPFCTRLRX', 'SEQ.SYNTHLPFCTRLRX', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.SYNTHLPFCTRLRX = RM_Field_SEQ_SYNTHLPFCTRLRX_SYNTHLPFCTRLRX(self)
        self.zz_fdict['SYNTHLPFCTRLRX'] = self.SYNTHLPFCTRLRX
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SYNTHLPFCTRLTX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SYNTHLPFCTRLTX, self).__init__(rmio, label,
            0x21000fec, 0x010,
            'SYNTHLPFCTRLTX', 'SEQ.SYNTHLPFCTRLTX', 'read-write',
            u"",
            0x00000000, 0x00000000)

        self.SYNTHLPFCTRLTX = RM_Field_SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX(self)
        self.zz_fdict['SYNTHLPFCTRLTX'] = self.SYNTHLPFCTRLTX
        self.__dict__['zz_frozen'] = True


