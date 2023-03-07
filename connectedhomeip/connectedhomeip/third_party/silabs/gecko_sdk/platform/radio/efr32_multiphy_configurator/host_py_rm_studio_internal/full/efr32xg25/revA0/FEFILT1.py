
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_FEFILT1' ]

from . static import Base_RM_Peripheral
from . FEFILT1_register import *

class RM_Peripheral_FEFILT1(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_FEFILT1, self).__init__(rmio, label,
            0xA8044000, 'FEFILT1',
            u"",
            [])
        self.IPVERSION = RM_Register_FEFILT1_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_FEFILT1_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.SWRST = RM_Register_FEFILT1_SWRST(self.zz_rmio, self.zz_label)
        self.zz_rdict['SWRST'] = self.SWRST
        self.STATUS = RM_Register_FEFILT1_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.LOCK = RM_Register_FEFILT1_LOCK(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOCK'] = self.LOCK
        self.CFG = RM_Register_FEFILT1_CFG(self.zz_rmio, self.zz_label)
        self.zz_rdict['CFG'] = self.CFG
        self.CMD = RM_Register_FEFILT1_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.CF = RM_Register_FEFILT1_CF(self.zz_rmio, self.zz_label)
        self.zz_rdict['CF'] = self.CF
        self.SRC = RM_Register_FEFILT1_SRC(self.zz_rmio, self.zz_label)
        self.zz_rdict['SRC'] = self.SRC
        self.DIGIGAINCTRL = RM_Register_FEFILT1_DIGIGAINCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGIGAINCTRL'] = self.DIGIGAINCTRL
        self.CHFCOE00 = RM_Register_FEFILT1_CHFCOE00(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE00'] = self.CHFCOE00
        self.CHFCOE01 = RM_Register_FEFILT1_CHFCOE01(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE01'] = self.CHFCOE01
        self.CHFCOE02 = RM_Register_FEFILT1_CHFCOE02(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE02'] = self.CHFCOE02
        self.CHFCOE03 = RM_Register_FEFILT1_CHFCOE03(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE03'] = self.CHFCOE03
        self.CHFCOE04 = RM_Register_FEFILT1_CHFCOE04(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE04'] = self.CHFCOE04
        self.CHFCOE05 = RM_Register_FEFILT1_CHFCOE05(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE05'] = self.CHFCOE05
        self.CHFCOE06 = RM_Register_FEFILT1_CHFCOE06(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE06'] = self.CHFCOE06
        self.CHFCOE10 = RM_Register_FEFILT1_CHFCOE10(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE10'] = self.CHFCOE10
        self.CHFCOE11 = RM_Register_FEFILT1_CHFCOE11(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE11'] = self.CHFCOE11
        self.CHFCOE12 = RM_Register_FEFILT1_CHFCOE12(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE12'] = self.CHFCOE12
        self.CHFCOE13 = RM_Register_FEFILT1_CHFCOE13(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE13'] = self.CHFCOE13
        self.CHFCOE15 = RM_Register_FEFILT1_CHFCOE15(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE15'] = self.CHFCOE15
        self.CHFCOE14 = RM_Register_FEFILT1_CHFCOE14(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE14'] = self.CHFCOE14
        self.CHFCOE16 = RM_Register_FEFILT1_CHFCOE16(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCOE16'] = self.CHFCOE16
        self.CHFCTRL = RM_Register_FEFILT1_CHFCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHFCTRL'] = self.CHFCTRL
        self.CTRL = RM_Register_FEFILT1_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.BLANKING = RM_Register_FEFILT1_BLANKING(self.zz_rmio, self.zz_label)
        self.zz_rdict['BLANKING'] = self.BLANKING
        self.RSSIFILT = RM_Register_FEFILT1_RSSIFILT(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSIFILT'] = self.RSSIFILT
        self.DIGMIXCTRL = RM_Register_FEFILT1_DIGMIXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGMIXCTRL'] = self.DIGMIXCTRL
        self.DCCOMP = RM_Register_FEFILT1_DCCOMP(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCCOMP'] = self.DCCOMP
        self.DCCOMPFILTINIT = RM_Register_FEFILT1_DCCOMPFILTINIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCCOMPFILTINIT'] = self.DCCOMPFILTINIT
        self.DCESTI = RM_Register_FEFILT1_DCESTI(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCESTI'] = self.DCESTI
        self.IRCAL = RM_Register_FEFILT1_IRCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCAL'] = self.IRCAL
        self.IRCALCOEF = RM_Register_FEFILT1_IRCALCOEF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCALCOEF'] = self.IRCALCOEF
        self.IRCALCOEFWR0 = RM_Register_FEFILT1_IRCALCOEFWR0(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCALCOEFWR0'] = self.IRCALCOEFWR0
        self.IRCALCOEFWR1 = RM_Register_FEFILT1_IRCALCOEFWR1(self.zz_rmio, self.zz_label)
        self.zz_rdict['IRCALCOEFWR1'] = self.IRCALCOEFWR1
        self.__dict__['zz_frozen'] = True