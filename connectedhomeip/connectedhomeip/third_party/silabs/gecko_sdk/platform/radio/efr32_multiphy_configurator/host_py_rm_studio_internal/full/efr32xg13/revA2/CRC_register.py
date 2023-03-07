
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . CRC_field import *


class RM_Register_CRC_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_CTRL, self).__init__(rmio, label,
            0x40082000, 0x000,
            'CTRL', 'CRC.CTRL', 'read-write',
            u"",
            0x00000704, 0x00001FEF)

        self.INPUTINV = RM_Field_CRC_CTRL_INPUTINV(self)
        self.zz_fdict['INPUTINV'] = self.INPUTINV
        self.OUTPUTINV = RM_Field_CRC_CTRL_OUTPUTINV(self)
        self.zz_fdict['OUTPUTINV'] = self.OUTPUTINV
        self.CRCWIDTH = RM_Field_CRC_CTRL_CRCWIDTH(self)
        self.zz_fdict['CRCWIDTH'] = self.CRCWIDTH
        self.INPUTBITORDER = RM_Field_CRC_CTRL_INPUTBITORDER(self)
        self.zz_fdict['INPUTBITORDER'] = self.INPUTBITORDER
        self.BYTEREVERSE = RM_Field_CRC_CTRL_BYTEREVERSE(self)
        self.zz_fdict['BYTEREVERSE'] = self.BYTEREVERSE
        self.BITREVERSE = RM_Field_CRC_CTRL_BITREVERSE(self)
        self.zz_fdict['BITREVERSE'] = self.BITREVERSE
        self.BITSPERWORD = RM_Field_CRC_CTRL_BITSPERWORD(self)
        self.zz_fdict['BITSPERWORD'] = self.BITSPERWORD
        self.PADCRCINPUT = RM_Field_CRC_CTRL_PADCRCINPUT(self)
        self.zz_fdict['PADCRCINPUT'] = self.PADCRCINPUT
        self.__dict__['zz_frozen'] = True


class RM_Register_CRC_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_STATUS, self).__init__(rmio, label,
            0x40082000, 0x004,
            'STATUS', 'CRC.STATUS', 'read-only',
            u"",
            0x00000000, 0x00000001)

        self.BUSY = RM_Field_CRC_STATUS_BUSY(self)
        self.zz_fdict['BUSY'] = self.BUSY
        self.__dict__['zz_frozen'] = True


class RM_Register_CRC_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_CMD, self).__init__(rmio, label,
            0x40082000, 0x008,
            'CMD', 'CRC.CMD', 'write-only',
            u"",
            0x00000000, 0x00000001)

        self.INITIALIZE = RM_Field_CRC_CMD_INITIALIZE(self)
        self.zz_fdict['INITIALIZE'] = self.INITIALIZE
        self.__dict__['zz_frozen'] = True


class RM_Register_CRC_INPUTDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_INPUTDATA, self).__init__(rmio, label,
            0x40082000, 0x00C,
            'INPUTDATA', 'CRC.INPUTDATA', 'read-write',
            u"",
            0x00000000, 0x0000FFFF)

        self.INPUTDATA = RM_Field_CRC_INPUTDATA_INPUTDATA(self)
        self.zz_fdict['INPUTDATA'] = self.INPUTDATA
        self.__dict__['zz_frozen'] = True


class RM_Register_CRC_INIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_INIT, self).__init__(rmio, label,
            0x40082000, 0x010,
            'INIT', 'CRC.INIT', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.INIT = RM_Field_CRC_INIT_INIT(self)
        self.zz_fdict['INIT'] = self.INIT
        self.__dict__['zz_frozen'] = True


class RM_Register_CRC_DATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_DATA, self).__init__(rmio, label,
            0x40082000, 0x014,
            'DATA', 'CRC.DATA', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.DATA = RM_Field_CRC_DATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_CRC_POLY(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_CRC_POLY, self).__init__(rmio, label,
            0x40082000, 0x018,
            'POLY', 'CRC.POLY', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.POLY = RM_Field_CRC_POLY_POLY(self)
        self.zz_fdict['POLY'] = self.POLY
        self.__dict__['zz_frozen'] = True


