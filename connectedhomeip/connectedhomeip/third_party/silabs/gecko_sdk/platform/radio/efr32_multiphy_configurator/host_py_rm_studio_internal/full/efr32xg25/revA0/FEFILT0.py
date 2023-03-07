
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_FEFILT0' ]

from . static import Base_RM_Peripheral
from . FEFILT0_register import *

class RM_Peripheral_FEFILT0(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_FEFILT0, self).__init__(rmio, label,
            0xA8040000, 'FEFILT0',
            u"",
            [])
        self.IPVERSION = RM_Register_FEFILT0_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_FEFILT0_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.SWRST = RM_Register_FEFILT0_SWRST(self.zz_rmio, self.zz_label)
        self.zz_rdict['SWRST'] = self.SWRST
        self.STATUS = RM_Register_FEFILT0_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.LOCK = RM_Register_FEFILT0_LOCK(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOCK'] = self.LOCK
        self.CFG = RM_Register_FEFILT0_CFG(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG'] = self.CFG
        self.CMD = RM_Register_FEFILT0_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.CF = RM_Register_FEFILT0_CF(self.zz_rmio, self.zz_label)
        self.zz_rdict['CF'] = self.CF
        self.SRC = RM_Register_FEFILT0_SRC(self.zz_rmio, self.zz_label)
        self.zz_rdict['SRC'] = self.SRC
        self.DIGIGAINCTRL = RM_Register_FEFILT0_DIGIGAINCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGIGAINCTRL'] = self.DIGIGAINCTRL
        self.CHFCOE00 = RM_Register_FEFILT0_CHFCOE00(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE00'] = self.CHFCOE00
        self.CHFCOE01 = RM_Register_FEFILT0_CHFCOE01(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE01'] = self.CHFCOE01
        self.CHFCOE02 = RM_Register_FEFILT0_CHFCOE02(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE02'] = self.CHFCOE02
        self.CHFCOE03 = RM_Register_FEFILT0_CHFCOE03(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE03'] = self.CHFCOE03
        self.CHFCOE04 = RM_Register_FEFILT0_CHFCOE04(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE04'] = self.CHFCOE04
        self.CHFCOE05 = RM_Register_FEFILT0_CHFCOE05(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE05'] = self.CHFCOE05
        self.CHFCOE06 = RM_Register_FEFILT0_CHFCOE06(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE06'] = self.CHFCOE06
        self.CHFCOE10 = RM_Register_FEFILT0_CHFCOE10(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE10'] = self.CHFCOE10
        self.CHFCOE11 = RM_Register_FEFILT0_CHFCOE11(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE11'] = self.CHFCOE11
        self.CHFCOE12 = RM_Register_FEFILT0_CHFCOE12(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE12'] = self.CHFCOE12
        self.CHFCOE13 = RM_Register_FEFILT0_CHFCOE13(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE13'] = self.CHFCOE13
        self.CHFCOE15 = RM_Register_FEFILT0_CHFCOE15(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE15'] = self.CHFCOE15
        self.CHFCOE14 = RM_Register_FEFILT0_CHFCOE14(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE14'] = self.CHFCOE14
        self.CHFCOE16 = RM_Register_FEFILT0_CHFCOE16(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE16'] = self.CHFCOE16
        self.CHFCTRL = RM_Register_FEFILT0_CHFCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCTRL'] = self.CHFCTRL
        self.CTRL = RM_Register_FEFILT0_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.BLANKING = RM_Register_FEFILT0_BLANKING(self.zz_rmio, self.zz_label)
        self.zz_rdict['BLANKING'] = self.BLANKING
        self.RSSIFILT = RM_Register_FEFILT0_RSSIFILT(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSIFILT'] = self.RSSIFILT
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
        self.IRCALCOEFWR1 = RM_Register_FEFILT0_IRCALCOEFWR1(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCALCOEFWR1'] = self.IRCALCOEFWR1
        self.__dict__['zz_frozen'] = True