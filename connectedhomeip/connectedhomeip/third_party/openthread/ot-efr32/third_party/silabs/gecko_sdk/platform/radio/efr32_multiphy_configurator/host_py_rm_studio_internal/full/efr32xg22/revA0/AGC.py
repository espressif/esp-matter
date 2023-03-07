
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_AGC' ]

from . static import Base_RM_Peripheral
from . AGC_register import *

class RM_Peripheral_AGC(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_AGC, self).__init__(rmio, label,
            0xA800C000, 'AGC',
            u"",
            [])
        self.IPVERSION = RM_Register_AGC_IPVERSION(self.zz_rmio, self.zz_label)
        self.zz_rdict['IPVERSION'] = self.IPVERSION
        self.EN = RM_Register_AGC_EN(self.zz_rmio, self.zz_label)
        self.zz_rdict['EN'] = self.EN
        self.STATUS0 = RM_Register_AGC_STATUS0(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS0'] = self.STATUS0
        self.STATUS1 = RM_Register_AGC_STATUS1(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS1'] = self.STATUS1
        self.STATUS2 = RM_Register_AGC_STATUS2(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS2'] = self.STATUS2
        self.RSSI = RM_Register_AGC_RSSI(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSI'] = self.RSSI
        self.FRAMERSSI = RM_Register_AGC_FRAMERSSI(self.zz_rmio, self.zz_label)
        self.zz_rdict['FRAMERSSI'] = self.FRAMERSSI
        self.CTRL0 = RM_Register_AGC_CTRL0(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL0'] = self.CTRL0
        self.CTRL1 = RM_Register_AGC_CTRL1(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL1'] = self.CTRL1
        self.CTRL2 = RM_Register_AGC_CTRL2(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL2'] = self.CTRL2
        self.CTRL3 = RM_Register_AGC_CTRL3(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL3'] = self.CTRL3
        self.CTRL4 = RM_Register_AGC_CTRL4(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL4'] = self.CTRL4
        self.CTRL5 = RM_Register_AGC_CTRL5(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL5'] = self.CTRL5
        self.CTRL6 = RM_Register_AGC_CTRL6(self.zz_rmio, self.zz_label)
        self.zz_rdict['CTRL6'] = self.CTRL6
        self.RSSISTEPTHR = RM_Register_AGC_RSSISTEPTHR(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSISTEPTHR'] = self.RSSISTEPTHR
        self.MANGAIN = RM_Register_AGC_MANGAIN(self.zz_rmio, self.zz_label)
        self.zz_rdict['MANGAIN'] = self.MANGAIN
        self.IF = RM_Register_AGC_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IEN = RM_Register_AGC_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.CMD = RM_Register_AGC_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.GAINRANGE = RM_Register_AGC_GAINRANGE(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINRANGE'] = self.GAINRANGE
        self.AGCPERIOD = RM_Register_AGC_AGCPERIOD(self.zz_rmio, self.zz_label)
        self.zz_rdict['AGCPERIOD'] = self.AGCPERIOD
        self.HICNTREGION = RM_Register_AGC_HICNTREGION(self.zz_rmio, self.zz_label)
        self.zz_rdict['HICNTREGION'] = self.HICNTREGION
        self.STEPDWN = RM_Register_AGC_STEPDWN(self.zz_rmio, self.zz_label)
        self.zz_rdict['STEPDWN'] = self.STEPDWN
        self.GAINSTEPLIM = RM_Register_AGC_GAINSTEPLIM(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINSTEPLIM'] = self.GAINSTEPLIM
        self.PNRFATT0 = RM_Register_AGC_PNRFATT0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT0'] = self.PNRFATT0
        self.PNRFATT1 = RM_Register_AGC_PNRFATT1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT1'] = self.PNRFATT1
        self.PNRFATT2 = RM_Register_AGC_PNRFATT2(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT2'] = self.PNRFATT2
        self.PNRFATT3 = RM_Register_AGC_PNRFATT3(self.zz_rmio, self.zz_label)
        self.zz_rdict['PNRFATT3'] = self.PNRFATT3
        self.LNAMIXCODE0 = RM_Register_AGC_LNAMIXCODE0(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCODE0'] = self.LNAMIXCODE0
        self.LNAMIXCODE1 = RM_Register_AGC_LNAMIXCODE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['LNAMIXCODE1'] = self.LNAMIXCODE1
        self.PGACODE0 = RM_Register_AGC_PGACODE0(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGACODE0'] = self.PGACODE0
        self.PGACODE1 = RM_Register_AGC_PGACODE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['PGACODE1'] = self.PGACODE1
        self.LBT = RM_Register_AGC_LBT(self.zz_rmio, self.zz_label)
        self.zz_rdict['LBT'] = self.LBT
        self.MIRRORIF = RM_Register_AGC_MIRRORIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['MIRRORIF'] = self.MIRRORIF
        self.SEQIF = RM_Register_AGC_SEQIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIF'] = self.SEQIF
        self.SEQIEN = RM_Register_AGC_SEQIEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['SEQIEN'] = self.SEQIEN
        self.__dict__['zz_frozen'] = True