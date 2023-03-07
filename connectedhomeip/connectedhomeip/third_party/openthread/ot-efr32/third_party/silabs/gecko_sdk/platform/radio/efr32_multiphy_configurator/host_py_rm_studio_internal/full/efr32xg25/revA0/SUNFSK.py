
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_SUNFSK' ]

from . static import Base_RM_Peripheral
from . SUNFSK_register import *

class RM_Peripheral_SUNFSK(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_SUNFSK, self).__init__(rmio, label,
            0xB500FC00, 'SUNFSK',
            u"",
            ['CW', 'LEGOQPSK', 'SUNOFDM', 'SUNOQPSK'])
        self.CFG1 = RM_Register_SUNFSK_CFG1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG1'] = self.CFG1
        self.CFG2 = RM_Register_SUNFSK_CFG2(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG2'] = self.CFG2
        self.CFG3 = RM_Register_SUNFSK_CFG3(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG3'] = self.CFG3
        self.CFG4 = RM_Register_SUNFSK_CFG4(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG4'] = self.CFG4
        self.CFG5 = RM_Register_SUNFSK_CFG5(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG5'] = self.CFG5
        self.CFG6 = RM_Register_SUNFSK_CFG6(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG6'] = self.CFG6
        self.CFG7 = RM_Register_SUNFSK_CFG7(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG7'] = self.CFG7
        self.CFG8 = RM_Register_SUNFSK_CFG8(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG8'] = self.CFG8
        self.CFG9 = RM_Register_SUNFSK_CFG9(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG9'] = self.CFG9
        self.CFG10 = RM_Register_SUNFSK_CFG10(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG10'] = self.CFG10
        self.CFG11 = RM_Register_SUNFSK_CFG11(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG11'] = self.CFG11
        self.CFG12 = RM_Register_SUNFSK_CFG12(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG12'] = self.CFG12
        self.CFG13 = RM_Register_SUNFSK_CFG13(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG13'] = self.CFG13
        self.CFG14 = RM_Register_SUNFSK_CFG14(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG14'] = self.CFG14
        self.CFG15 = RM_Register_SUNFSK_CFG15(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG15'] = self.CFG15
        self.CFG16 = RM_Register_SUNFSK_CFG16(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG16'] = self.CFG16
        self.__dict__['zz_frozen'] = True