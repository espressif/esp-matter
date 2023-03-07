
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . RFCRC_field import *


class RM_Register_RFCRC_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_IPVERSION, self).__init__(rmio, label,
            0xa8010000, 0x000,
            'IPVERSION', 'RFCRC.IPVERSION', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_RFCRC_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_EN, self).__init__(rmio, label,
            0xa8010000, 0x004,
            'EN', 'RFCRC.EN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_RFCRC_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_CTRL, self).__init__(rmio, label,
            0xa8010000, 0x008,
            'CTRL', 'RFCRC.CTRL', 'read-write',
            u"",
            0x00000704, 0x00001FEF,
            0x00001000, 0x00002000,
            0x00003000)

        self.INPUTINV = RM_Field_RFCRC_CTRL_INPUTINV(self)
        self.zz_fdict['INPUTINV'] = self.INPUTINV
        self.OUTPUTINV = RM_Field_RFCRC_CTRL_OUTPUTINV(self)
        self.zz_fdict['OUTPUTINV'] = self.OUTPUTINV
        self.CRCWIDTH = RM_Field_RFCRC_CTRL_CRCWIDTH(self)
        self.zz_fdict['CRCWIDTH'] = self.CRCWIDTH
        self.INPUTBITORDER = RM_Field_RFCRC_CTRL_INPUTBITORDER(self)
        self.zz_fdict['INPUTBITORDER'] = self.INPUTBITORDER
        self.BYTEREVERSE = RM_Field_RFCRC_CTRL_BYTEREVERSE(self)
        self.zz_fdict['BYTEREVERSE'] = self.BYTEREVERSE
        self.BITREVERSE = RM_Field_RFCRC_CTRL_BITREVERSE(self)
        self.zz_fdict['BITREVERSE'] = self.BITREVERSE
        self.BITSPERWORD = RM_Field_RFCRC_CTRL_BITSPERWORD(self)
        self.zz_fdict['BITSPERWORD'] = self.BITSPERWORD
        self.PADCRCINPUT = RM_Field_RFCRC_CTRL_PADCRCINPUT(self)
        self.zz_fdict['PADCRCINPUT'] = self.PADCRCINPUT
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_STATUS, self).__init__(rmio, label,
            0xa8010000, 0x00C,
            'STATUS', 'RFCRC.STATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.BUSY = RM_Field_RFCRC_STATUS_BUSY(self)
        self.zz_fdict['BUSY'] = self.BUSY
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_CMD, self).__init__(rmio, label,
            0xa8010000, 0x010,
            'CMD', 'RFCRC.CMD', 'write-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.INITIALIZE = RM_Field_RFCRC_CMD_INITIALIZE(self)
        self.zz_fdict['INITIALIZE'] = self.INITIALIZE
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_INPUTDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_INPUTDATA, self).__init__(rmio, label,
            0xa8010000, 0x014,
            'INPUTDATA', 'RFCRC.INPUTDATA', 'write-only',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.INPUTDATA = RM_Field_RFCRC_INPUTDATA_INPUTDATA(self)
        self.zz_fdict['INPUTDATA'] = self.INPUTDATA
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_INIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_INIT, self).__init__(rmio, label,
            0xa8010000, 0x018,
            'INIT', 'RFCRC.INIT', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.INIT = RM_Field_RFCRC_INIT_INIT(self)
        self.zz_fdict['INIT'] = self.INIT
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_DATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_DATA, self).__init__(rmio, label,
            0xa8010000, 0x01C,
            'DATA', 'RFCRC.DATA', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DATA = RM_Field_RFCRC_DATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_RFCRC_POLY(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RFCRC_POLY, self).__init__(rmio, label,
            0xa8010000, 0x020,
            'POLY', 'RFCRC.POLY', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POLY = RM_Field_RFCRC_POLY_POLY(self)
        self.zz_fdict['POLY'] = self.POLY
        self.__dict__['zz_frozen'] = True


