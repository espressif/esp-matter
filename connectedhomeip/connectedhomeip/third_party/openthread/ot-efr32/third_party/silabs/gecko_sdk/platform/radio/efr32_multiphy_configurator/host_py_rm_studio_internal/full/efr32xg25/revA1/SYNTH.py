
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_SYNTH' ]

from . static import Base_RM_Peripheral
from . SYNTH_register import *

class RM_Peripheral_SYNTH(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_SYNTH, self).__init__(rmio, label,
            0xA8018000, 'SYNTH',
            u"",
            [])
        self.IPVERSION = RM_Register_SYNTH_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_SYNTH_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.STATUS = RM_Register_SYNTH_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.CMD = RM_Register_SYNTH_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.CTRL = RM_Register_SYNTH_CTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL'] = self.CTRL
        self.CALCTRL = RM_Register_SYNTH_CALCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CALCTRL'] = self.CALCTRL
        self.VCDACCTRL = RM_Register_SYNTH_VCDACCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCDACCTRL'] = self.VCDACCTRL
        self.FREQ = RM_Register_SYNTH_FREQ(self.zz_rmio, self.zz_label)
        self.zz_rdict['FREQ'] = self.FREQ
        self.IFFREQ = RM_Register_SYNTH_IFFREQ(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFFREQ'] = self.IFFREQ
        self.DIVCTRL = RM_Register_SYNTH_DIVCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIVCTRL'] = self.DIVCTRL
        self.CHCTRL = RM_Register_SYNTH_CHCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHCTRL'] = self.CHCTRL
        self.CHSP = RM_Register_SYNTH_CHSP(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHSP'] = self.CHSP
        self.CALOFFSET = RM_Register_SYNTH_CALOFFSET(self.zz_rmio, self.zz_label)
        self.zz_rdict['CALOFFSET'] = self.CALOFFSET
        self.VCOTUNING = RM_Register_SYNTH_VCOTUNING(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCOTUNING'] = self.VCOTUNING
        self.VCORANGE = RM_Register_SYNTH_VCORANGE(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCORANGE'] = self.VCORANGE
        self.VCOGAIN = RM_Register_SYNTH_VCOGAIN(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCOGAIN'] = self.VCOGAIN
        self.CHPDACCTRL = RM_Register_SYNTH_CHPDACCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHPDACCTRL'] = self.CHPDACCTRL
        self.CAPCALCYCLECNT = RM_Register_SYNTH_CAPCALCYCLECNT(self.zz_rmio, self.zz_label)
        self.zz_rdict['CAPCALCYCLECNT'] = self.CAPCALCYCLECNT
        self.VCOFRCCAL = RM_Register_SYNTH_VCOFRCCAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['VCOFRCCAL'] = self.VCOFRCCAL
        self.IF = RM_Register_SYNTH_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IEN = RM_Register_SYNTH_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.LOCNTCTRL = RM_Register_SYNTH_LOCNTCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOCNTCTRL'] = self.LOCNTCTRL
        self.LOCNTSTATUS = RM_Register_SYNTH_LOCNTSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOCNTSTATUS'] = self.LOCNTSTATUS
        self.LOCNTTARGET = RM_Register_SYNTH_LOCNTTARGET(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOCNTTARGET'] = self.LOCNTTARGET
        self.MMDDENOMINIT = RM_Register_SYNTH_MMDDENOMINIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['MMDDENOMINIT'] = self.MMDDENOMINIT
        self.CHPDACINIT = RM_Register_SYNTH_CHPDACINIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHPDACINIT'] = self.CHPDACINIT
        self.LPFCTRL1CAL = RM_Register_SYNTH_LPFCTRL1CAL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LPFCTRL1CAL'] = self.LPFCTRL1CAL
        self.LPFCTRL1RX = RM_Register_SYNTH_LPFCTRL1RX(self.zz_rmio, self.zz_label)
        self.zz_rdict['LPFCTRL1RX'] = self.LPFCTRL1RX
        self.LPFCTRL1TX = RM_Register_SYNTH_LPFCTRL1TX(self.zz_rmio, self.zz_label)
        self.zz_rdict['LPFCTRL1TX'] = self.LPFCTRL1TX
        self.LPFCTRL2RX = RM_Register_SYNTH_LPFCTRL2RX(self.zz_rmio, self.zz_label)
        self.zz_rdict['LPFCTRL2RX'] = self.LPFCTRL2RX
        self.LPFCTRL2TX = RM_Register_SYNTH_LPFCTRL2TX(self.zz_rmio, self.zz_label)
        self.zz_rdict['LPFCTRL2TX'] = self.LPFCTRL2TX
        self.DSMCTRLRX = RM_Register_SYNTH_DSMCTRLRX(self.zz_rmio, self.zz_label)
        self.zz_rdict['DSMCTRLRX'] = self.DSMCTRLRX
        self.DSMCTRLTX = RM_Register_SYNTH_DSMCTRLTX(self.zz_rmio, self.zz_label)
        self.zz_rdict['DSMCTRLTX'] = self.DSMCTRLTX
        self.SEQIF = RM_Register_SYNTH_SEQIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIF'] = self.SEQIF
        self.SEQIEN = RM_Register_SYNTH_SEQIEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIEN'] = self.SEQIEN
        self.SFMIF = RM_Register_SYNTH_SFMIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['SFMIF'] = self.SFMIF
        self.SFMIEN = RM_Register_SYNTH_SFMIEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SFMIEN'] = self.SFMIEN
        self.__dict__['zz_frozen'] = True