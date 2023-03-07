
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_SYNTH' ]

from . static import Base_RM_Peripheral
from . SYNTH_register import *

class RM_Peripheral_SYNTH(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_SYNTH, self).__init__(rmio, label,
            0x40083000, 'SYNTH',
            u"",
            [])
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
        self.AUXVCOTUNING = RM_Register_SYNTH_AUXVCOTUNING(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXVCOTUNING'] = self.AUXVCOTUNING
        self.AUXFREQ = RM_Register_SYNTH_AUXFREQ(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXFREQ'] = self.AUXFREQ
        self.AUXVCDACCTRL = RM_Register_SYNTH_AUXVCDACCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUXVCDACCTRL'] = self.AUXVCDACCTRL
        self.CHPDACCTRL = RM_Register_SYNTH_CHPDACCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['CHPDACCTRL'] = self.CHPDACCTRL
        self.CAPCALCYCLECNT = RM_Register_SYNTH_CAPCALCYCLECNT(self.zz_rmio, self.zz_label)
        self.zz_rdict['CAPCALCYCLECNT'] = self.CAPCALCYCLECNT
        self.IF = RM_Register_SYNTH_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IFS = RM_Register_SYNTH_IFS(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFS'] = self.IFS
        self.IFC = RM_Register_SYNTH_IFC(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFC'] = self.IFC
        self.IEN = RM_Register_SYNTH_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.__dict__['zz_frozen'] = True