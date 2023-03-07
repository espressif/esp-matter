
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_MODEM' ]

from . static import Base_RM_Peripheral
from . MODEM_register import *

class RM_Peripheral_MODEM(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_MODEM, self).__init__(rmio, label,
            0x40086000, 'MODEM',
            u"",
            [])
        self.STATUS = RM_Register_MODEM_STATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS'] = self.STATUS
        self.TIMDETSTATUS = RM_Register_MODEM_TIMDETSTATUS(self.zz_rmio, self.zz_label)
        self.zz_rdict['TIMDETSTATUS'] = self.TIMDETSTATUS
        self.FREQOFFEST = RM_Register_MODEM_FREQOFFEST(self.zz_rmio, self.zz_label)
        self.zz_rdict['FREQOFFEST'] = self.FREQOFFEST
        self.AFCADJRX = RM_Register_MODEM_AFCADJRX(self.zz_rmio, self.zz_label)
        self.zz_rdict['AFCADJRX'] = self.AFCADJRX
        self.AFCADJTX = RM_Register_MODEM_AFCADJTX(self.zz_rmio, self.zz_label)
        self.zz_rdict['AFCADJTX'] = self.AFCADJTX
        self.MIXCTRL = RM_Register_MODEM_MIXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['MIXCTRL'] = self.MIXCTRL
        self.CTRL0 = RM_Register_MODEM_CTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL0'] = self.CTRL0
        self.CTRL1 = RM_Register_MODEM_CTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL1'] = self.CTRL1
        self.CTRL2 = RM_Register_MODEM_CTRL2(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL2'] = self.CTRL2
        self.CTRL3 = RM_Register_MODEM_CTRL3(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL3'] = self.CTRL3
        self.CTRL4 = RM_Register_MODEM_CTRL4(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL4'] = self.CTRL4
        self.CTRL5 = RM_Register_MODEM_CTRL5(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL5'] = self.CTRL5
        self.CTRL6 = RM_Register_MODEM_CTRL6(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL6'] = self.CTRL6
        self.TXBR = RM_Register_MODEM_TXBR(self.zz_rmio, self.zz_label)
        self.zz_rdict['TXBR'] = self.TXBR
        self.RXBR = RM_Register_MODEM_RXBR(self.zz_rmio, self.zz_label)
        self.zz_rdict['RXBR'] = self.RXBR
        self.CF = RM_Register_MODEM_CF(self.zz_rmio, self.zz_label)
        self.zz_rdict['CF'] = self.CF
        self.PRE = RM_Register_MODEM_PRE(self.zz_rmio, self.zz_label)
        self.zz_rdict['PRE'] = self.PRE
        self.SYNC0 = RM_Register_MODEM_SYNC0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNC0'] = self.SYNC0
        self.SYNC1 = RM_Register_MODEM_SYNC1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SYNC1'] = self.SYNC1
        self.TIMING = RM_Register_MODEM_TIMING(self.zz_rmio, self.zz_label)
        self.zz_rdict['TIMING'] = self.TIMING
        self.DSSS0 = RM_Register_MODEM_DSSS0(self.zz_rmio, self.zz_label)
        self.zz_rdict['DSSS0'] = self.DSSS0
        self.MODINDEX = RM_Register_MODEM_MODINDEX(self.zz_rmio, self.zz_label)
        self.zz_rdict['MODINDEX'] = self.MODINDEX
        self.AFC = RM_Register_MODEM_AFC(self.zz_rmio, self.zz_label)
        self.zz_rdict['AFC'] = self.AFC
        self.AFCADJLIM = RM_Register_MODEM_AFCADJLIM(self.zz_rmio, self.zz_label)
        self.zz_rdict['AFCADJLIM'] = self.AFCADJLIM
        self.SHAPING0 = RM_Register_MODEM_SHAPING0(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING0'] = self.SHAPING0
        self.SHAPING1 = RM_Register_MODEM_SHAPING1(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING1'] = self.SHAPING1
        self.SHAPING2 = RM_Register_MODEM_SHAPING2(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING2'] = self.SHAPING2
        self.SHAPING3 = RM_Register_MODEM_SHAPING3(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING3'] = self.SHAPING3
        self.SHAPING4 = RM_Register_MODEM_SHAPING4(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING4'] = self.SHAPING4
        self.SHAPING5 = RM_Register_MODEM_SHAPING5(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING5'] = self.SHAPING5
        self.SHAPING6 = RM_Register_MODEM_SHAPING6(self.zz_rmio, self.zz_label)
        self.zz_rdict['SHAPING6'] = self.SHAPING6
        self.RAMPCTRL = RM_Register_MODEM_RAMPCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['RAMPCTRL'] = self.RAMPCTRL
        self.RAMPLEV = RM_Register_MODEM_RAMPLEV(self.zz_rmio, self.zz_label)
        self.zz_rdict['RAMPLEV'] = self.RAMPLEV
        self.ROUTEPEN = RM_Register_MODEM_ROUTEPEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['ROUTEPEN'] = self.ROUTEPEN
        self.ROUTELOC0 = RM_Register_MODEM_ROUTELOC0(self.zz_rmio, self.zz_label)
        self.zz_rdict['ROUTELOC0'] = self.ROUTELOC0
        self.ROUTELOC1 = RM_Register_MODEM_ROUTELOC1(self.zz_rmio, self.zz_label)
        self.zz_rdict['ROUTELOC1'] = self.ROUTELOC1
        self.DCCOMP = RM_Register_MODEM_DCCOMP(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCCOMP'] = self.DCCOMP
        self.DCCOMPFILTINIT = RM_Register_MODEM_DCCOMPFILTINIT(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCCOMPFILTINIT'] = self.DCCOMPFILTINIT
        self.DCESTI = RM_Register_MODEM_DCESTI(self.zz_rmio, self.zz_label)
        self.zz_rdict['DCESTI'] = self.DCESTI
        self.SRCCHF = RM_Register_MODEM_SRCCHF(self.zz_rmio, self.zz_label)
        self.zz_rdict['SRCCHF'] = self.SRCCHF
        self.INTAFC = RM_Register_MODEM_INTAFC(self.zz_rmio, self.zz_label)
        self.zz_rdict['INTAFC'] = self.INTAFC
        self.DSATHD0 = RM_Register_MODEM_DSATHD0(self.zz_rmio, self.zz_label)
        self.zz_rdict['DSATHD0'] = self.DSATHD0
        self.DSATHD1 = RM_Register_MODEM_DSATHD1(self.zz_rmio, self.zz_label)
        self.zz_rdict['DSATHD1'] = self.DSATHD1
        self.DSACTRL = RM_Register_MODEM_DSACTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DSACTRL'] = self.DSACTRL
        self.VITERBIDEMOD = RM_Register_MODEM_VITERBIDEMOD(self.zz_rmio, self.zz_label)
        self.zz_rdict['VITERBIDEMOD'] = self.VITERBIDEMOD
        self.VTCORRCFG0 = RM_Register_MODEM_VTCORRCFG0(self.zz_rmio, self.zz_label)
        self.zz_rdict['VTCORRCFG0'] = self.VTCORRCFG0
        self.DIGMIXCTRL = RM_Register_MODEM_DIGMIXCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['DIGMIXCTRL'] = self.DIGMIXCTRL
        self.VTCORRCFG1 = RM_Register_MODEM_VTCORRCFG1(self.zz_rmio, self.zz_label)
        self.zz_rdict['VTCORRCFG1'] = self.VTCORRCFG1
        self.VTTRACK = RM_Register_MODEM_VTTRACK(self.zz_rmio, self.zz_label)
        self.zz_rdict['VTTRACK'] = self.VTTRACK
        self.BREST = RM_Register_MODEM_BREST(self.zz_rmio, self.zz_label)
        self.zz_rdict['BREST'] = self.BREST
        self.AUTOCG = RM_Register_MODEM_AUTOCG(self.zz_rmio, self.zz_label)
        self.zz_rdict['AUTOCG'] = self.AUTOCG
        self.CGCLKSTOP = RM_Register_MODEM_CGCLKSTOP(self.zz_rmio, self.zz_label)
        self.zz_rdict['CGCLKSTOP'] = self.CGCLKSTOP
        self.POE = RM_Register_MODEM_POE(self.zz_rmio, self.zz_label)
        self.zz_rdict['POE'] = self.POE
        self.IF = RM_Register_MODEM_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IFS = RM_Register_MODEM_IFS(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFS'] = self.IFS
        self.IFC = RM_Register_MODEM_IFC(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFC'] = self.IFC
        self.IEN = RM_Register_MODEM_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.CMD = RM_Register_MODEM_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.__dict__['zz_frozen'] = True