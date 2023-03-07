
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_CW' ]

from . static import Base_RM_Peripheral
from . CW_register import *

class RM_Peripheral_CW(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_CW, self).__init__(rmio, label,
            0xB500FC00, 'CW',
            u"",
            ['LEGOQPSK', 'SUNFSK', 'SUNOFDM', 'SUNOQPSK'])
        self.CFG1 = RM_Register_CW_CFG1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG1'] = self.CFG1
        self.CFG2 = RM_Register_CW_CFG2(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG2'] = self.CFG2
        self.CFG3 = RM_Register_CW_CFG3(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG3'] = self.CFG3
        self.__dict__['zz_frozen'] = True