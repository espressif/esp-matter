
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_RFCRC' ]

from . static import Base_RM_Peripheral
from . RFCRC_register import *

class RM_Peripheral_RFCRC(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_RFCRC, self).__init__(rmio, label,
            0xA8010000, 'RFCRC',
            u"",
            [])
        self.IPVERSION = RM_Register_RFCRC_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_RFCRC_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.CTRL = RM_Register_RFCRC_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.STATUS = RM_Register_RFCRC_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.CMD = RM_Register_RFCRC_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.INPUTDATA = RM_Register_RFCRC_INPUTDATA(self.zz_rmio, self.zz_label)
        self.zz_rdict['INPUTDATA'] = self.INPUTDATA
        self.INIT = RM_Register_RFCRC_INIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['INIT'] = self.INIT
        self.DATA = RM_Register_RFCRC_DATA(self.zz_rmio, self.zz_label)
        self.zz_rdict['DATA'] = self.DATA
        self.POLY = RM_Register_RFCRC_POLY(self.zz_rmio, self.zz_label)
        self.zz_rdict['POLY'] = self.POLY
        self.__dict__['zz_frozen'] = True