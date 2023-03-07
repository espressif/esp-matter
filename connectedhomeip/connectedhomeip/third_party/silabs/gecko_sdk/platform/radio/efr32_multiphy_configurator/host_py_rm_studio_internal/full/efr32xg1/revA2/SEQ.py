
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_SEQ' ]

from . static import Base_RM_Peripheral
from . SEQ_register import *

class RM_Peripheral_SEQ(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_SEQ, self).__init__(rmio, label,
            0x21000FEC, 'SEQ',
            u"",
            [])
        self.DYNAMIC_CHPWR_TABLE = RM_Register_SEQ_DYNAMIC_CHPWR_TABLE(self.zz_rmio, self.zz_label)
        self.zz_rdict['DYNAMIC_CHPWR_TABLE'] = self.DYNAMIC_CHPWR_TABLE
        self.PHYINFO = RM_Register_SEQ_PHYINFO(self.zz_rmio, self.zz_label)
        self.zz_rdict['PHYINFO'] = self.PHYINFO
        self.MISC = RM_Register_SEQ_MISC(self.zz_rmio, self.zz_label)
        self.zz_rdict['MISC'] = self.MISC
        self.SYNTHLPFCTRLRX = RM_Register_SEQ_SYNTHLPFCTRLRX(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHLPFCTRLRX'] = self.SYNTHLPFCTRLRX
        self.SYNTHLPFCTRLTX = RM_Register_SEQ_SYNTHLPFCTRLTX(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNTHLPFCTRLTX'] = self.SYNTHLPFCTRLTX
        self.__dict__['zz_frozen'] = True