
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SEQ_field import *


class RM_Register_SEQ_SPARE0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SPARE0, self).__init__(rmio, label,
            0xb0004fe0, 0x000,
            'SPARE0', 'SEQ.SPARE0', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE0 = RM_Field_SEQ_SPARE0_SPARE0(self)
        self.zz_fdict['SPARE0'] = self.SPARE0
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SPARE1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SPARE1, self).__init__(rmio, label,
            0xb0004fe0, 0x004,
            'SPARE1', 'SEQ.SPARE1', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE1 = RM_Field_SEQ_SPARE1_SPARE1(self)
        self.zz_fdict['SPARE1'] = self.SPARE1
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SPARE2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SPARE2, self).__init__(rmio, label,
            0xb0004fe0, 0x008,
            'SPARE2', 'SEQ.SPARE2', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE2 = RM_Field_SEQ_SPARE2_SPARE2(self)
        self.zz_fdict['SPARE2'] = self.SPARE2
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SPARE3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SPARE3, self).__init__(rmio, label,
            0xb0004fe0, 0x00C,
            'SPARE3', 'SEQ.SPARE3', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE3 = RM_Field_SEQ_SPARE3_SPARE3(self)
        self.zz_fdict['SPARE3'] = self.SPARE3
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SPARE4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SPARE4, self).__init__(rmio, label,
            0xb0004fe0, 0x010,
            'SPARE4', 'SEQ.SPARE4', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE4 = RM_Field_SEQ_SPARE4_SPARE4(self)
        self.zz_fdict['SPARE4'] = self.SPARE4
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_SPARE5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_SPARE5, self).__init__(rmio, label,
            0xb0004fe0, 0x014,
            'SPARE5', 'SEQ.SPARE5', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE5 = RM_Field_SEQ_SPARE5_SPARE5(self)
        self.zz_fdict['SPARE5'] = self.SPARE5
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_MISC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_MISC, self).__init__(rmio, label,
            0xb0004fe0, 0x018,
            'MISC', 'SEQ.MISC', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.RESERVED10 = RM_Field_SEQ_MISC_RESERVED10(self)
        self.zz_fdict['RESERVED10'] = self.RESERVED10
        self.IFPKDOFF_AFTER_FRAME_EN = RM_Field_SEQ_MISC_IFPKDOFF_AFTER_FRAME_EN(self)
        self.zz_fdict['IFPKDOFF_AFTER_FRAME_EN'] = self.IFPKDOFF_AFTER_FRAME_EN
        self.RFPKDOFF_AFTER_FRAME_EN = RM_Field_SEQ_MISC_RFPKDOFF_AFTER_FRAME_EN(self)
        self.zz_fdict['RFPKDOFF_AFTER_FRAME_EN'] = self.RFPKDOFF_AFTER_FRAME_EN
        self.RESERVED2 = RM_Field_SEQ_MISC_RESERVED2(self)
        self.zz_fdict['RESERVED2'] = self.RESERVED2
        self.RESERVED1 = RM_Field_SEQ_MISC_RESERVED1(self)
        self.zz_fdict['RESERVED1'] = self.RESERVED1
        self.RESERVED0 = RM_Field_SEQ_MISC_RESERVED0(self)
        self.zz_fdict['RESERVED0'] = self.RESERVED0
        self.__dict__['zz_frozen'] = True


class RM_Register_SEQ_PHYINFO(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SEQ_PHYINFO, self).__init__(rmio, label,
            0xb0004fe0, 0x01C,
            'PHYINFO', 'SEQ.PHYINFO', 'read-write',
            u"",
            0x00000000, 0x00000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.ADDRESS = RM_Field_SEQ_PHYINFO_ADDRESS(self)
        self.zz_fdict['ADDRESS'] = self.ADDRESS
        self.__dict__['zz_frozen'] = True


