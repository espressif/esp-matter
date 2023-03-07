
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_CRC' ]

from . static import Base_RM_Peripheral
from . CRC_register import *

class RM_Peripheral_CRC(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_CRC, self).__init__(rmio, label,
            0x40082000, 'CRC',
            u"",
            [])
        self.CTRL = RM_Register_CRC_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.STATUS = RM_Register_CRC_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.CMD = RM_Register_CRC_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.INPUTDATA = RM_Register_CRC_INPUTDATA(self.zz_rmio, self.zz_label)
        self.zz_rdict['INPUTDATA'] = self.INPUTDATA
        self.INIT = RM_Register_CRC_INIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['INIT'] = self.INIT
        self.DATA = RM_Register_CRC_DATA(self.zz_rmio, self.zz_label)
        self.zz_rdict['DATA'] = self.DATA
        self.POLY = RM_Register_CRC_POLY(self.zz_rmio, self.zz_label)
        self.zz_rdict['POLY'] = self.POLY
        self.__dict__['zz_frozen'] = True