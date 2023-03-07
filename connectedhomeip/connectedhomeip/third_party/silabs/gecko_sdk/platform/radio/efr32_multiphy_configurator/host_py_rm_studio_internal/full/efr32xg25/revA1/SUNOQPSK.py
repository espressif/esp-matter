
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_SUNOQPSK' ]

from . static import Base_RM_Peripheral
from . SUNOQPSK_register import *

class RM_Peripheral_SUNOQPSK(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_SUNOQPSK, self).__init__(rmio, label,
            0xB500FC00, 'SUNOQPSK',
            u"",
            ['CW', 'LEGOQPSK', 'SUNFSK', 'SUNOFDM'])
        self.CFG1 = RM_Register_SUNOQPSK_CFG1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG1'] = self.CFG1
        self.CFG2 = RM_Register_SUNOQPSK_CFG2(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG2'] = self.CFG2
        self.__dict__['zz_frozen'] = True