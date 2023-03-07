
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_SEQ' ]

from . static import Base_RM_Peripheral
from . SEQ_register import *

class RM_Peripheral_SEQ(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_SEQ, self).__init__(rmio, label,
            0xB0006FE0, 'SEQ',
            u"",
            [])
        self.SPARE0 = RM_Register_SEQ_SPARE0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE0'] = self.SPARE0
        self.SPARE1 = RM_Register_SEQ_SPARE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE1'] = self.SPARE1
        self.SPARE2 = RM_Register_SEQ_SPARE2(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE2'] = self.SPARE2
        self.SPARE3 = RM_Register_SEQ_SPARE3(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE3'] = self.SPARE3
        self.SPARE4 = RM_Register_SEQ_SPARE4(self.zz_rmio, self.zz_label)
        self.zz_rdict['SPARE4'] = self.SPARE4
        self.MODEMINFO = RM_Register_SEQ_MODEMINFO(self.zz_rmio, self.zz_label)
        self.zz_rdict['MODEMINFO'] = self.MODEMINFO
        self.MISC = RM_Register_SEQ_MISC(self.zz_rmio, self.zz_label)
        self.zz_rdict['MISC'] = self.MISC
        self.PHYINFO = RM_Register_SEQ_PHYINFO(self.zz_rmio, self.zz_label)
        self.zz_rdict['PHYINFO'] = self.PHYINFO
        self.__dict__['zz_frozen'] = True