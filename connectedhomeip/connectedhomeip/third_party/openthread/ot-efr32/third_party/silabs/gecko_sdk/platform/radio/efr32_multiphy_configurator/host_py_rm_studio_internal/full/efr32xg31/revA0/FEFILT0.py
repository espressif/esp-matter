
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_FEFILT0' ]

from . static import Base_RM_Peripheral
from . FEFILT0_register import *

class RM_Peripheral_FEFILT0(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_FEFILT0, self).__init__(rmio, label,
            0xA803C000, 'FEFILT0',
            u"",
            [])
        self.IPVERSION = RM_Register_FEFILT0_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_FEFILT0_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.STATUS = RM_Register_FEFILT0_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.LOCK = RM_Register_FEFILT0_LOCK(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOCK'] = self.LOCK
        self.CFG = RM_Register_FEFILT0_CFG(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG'] = self.CFG
        self.SRC2 = RM_Register_FEFILT0_SRC2(self.zz_rmio, self.zz_label)
        self.zz_rdict['SRC2'] = self.SRC2
        self.GAINCTRL = RM_Register_FEFILT0_GAINCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINCTRL'] = self.GAINCTRL
        self.CHFCSDCOE00 = RM_Register_FEFILT0_CHFCSDCOE00(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE00'] = self.CHFCSDCOE00
        self.CHFCSDCOE01 = RM_Register_FEFILT0_CHFCSDCOE01(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE01'] = self.CHFCSDCOE01
        self.CHFCSDCOE02 = RM_Register_FEFILT0_CHFCSDCOE02(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE02'] = self.CHFCSDCOE02
        self.CHFCSDCOE03 = RM_Register_FEFILT0_CHFCSDCOE03(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE03'] = self.CHFCSDCOE03
        self.CHFCSDCOE10 = RM_Register_FEFILT0_CHFCSDCOE10(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE10'] = self.CHFCSDCOE10
        self.CHFCSDCOE11 = RM_Register_FEFILT0_CHFCSDCOE11(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE11'] = self.CHFCSDCOE11
        self.CHFCSDCOE12 = RM_Register_FEFILT0_CHFCSDCOE12(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE12'] = self.CHFCSDCOE12
        self.CHFCSDCOE13 = RM_Register_FEFILT0_CHFCSDCOE13(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE13'] = self.CHFCSDCOE13
        self.CHFCSDCOE00S = RM_Register_FEFILT0_CHFCSDCOE00S(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE00S'] = self.CHFCSDCOE00S
        self.CHFCSDCOE01S = RM_Register_FEFILT0_CHFCSDCOE01S(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE01S'] = self.CHFCSDCOE01S
        self.CHFCSDCOE10S = RM_Register_FEFILT0_CHFCSDCOE10S(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE10S'] = self.CHFCSDCOE10S
        self.CHFCSDCOE11S = RM_Register_FEFILT0_CHFCSDCOE11S(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCSDCOE11S'] = self.CHFCSDCOE11S
        self.DIGMIXCTRL = RM_Register_FEFILT0_DIGMIXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGMIXCTRL'] = self.DIGMIXCTRL
        self.DCCOMP = RM_Register_FEFILT0_DCCOMP(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCCOMP'] = self.DCCOMP
        self.DCCOMPFILTINIT = RM_Register_FEFILT0_DCCOMPFILTINIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCCOMPFILTINIT'] = self.DCCOMPFILTINIT
        self.DCESTI = RM_Register_FEFILT0_DCESTI(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCESTI'] = self.DCESTI
        self.IRCAL = RM_Register_FEFILT0_IRCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCAL'] = self.IRCAL
        self.IRCALCOEF = RM_Register_FEFILT0_IRCALCOEF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCALCOEF'] = self.IRCALCOEF
        self.IRCALCOEFWR0 = RM_Register_FEFILT0_IRCALCOEFWR0(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCALCOEFWR0'] = self.IRCALCOEFWR0
        self.__dict__['zz_frozen'] = True