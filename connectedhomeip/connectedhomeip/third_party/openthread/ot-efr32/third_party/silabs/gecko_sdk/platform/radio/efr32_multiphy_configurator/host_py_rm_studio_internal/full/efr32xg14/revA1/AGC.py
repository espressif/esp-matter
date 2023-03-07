
# -*- coding: utf-8 -*-

__all__ = [ 'RM_Peripheral_AGC' ]

from . static import Base_RM_Peripheral
from . AGC_register import *

class RM_Peripheral_AGC(Base_RM_Peripheral):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Peripheral_AGC, self).__init__(rmio, label,
            0x40087000, 'AGC',
            u"",
            [])
        self.STATUS0 = RM_Register_AGC_STATUS0(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS0'] = self.STATUS0
        self.STATUS1 = RM_Register_AGC_STATUS1(self.zz_rmio, self.zz_label)
        self.zz_rdict['STATUS1'] = self.STATUS1
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
        self.RSSISTEPTHR = RM_Register_AGC_RSSISTEPTHR(self.zz_rmio, self.zz_label)
        self.zz_rdict['RSSISTEPTHR'] = self.RSSISTEPTHR
        self.IFPEAKDET = RM_Register_AGC_IFPEAKDET(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFPEAKDET'] = self.IFPEAKDET
        self.MANGAIN = RM_Register_AGC_MANGAIN(self.zz_rmio, self.zz_label)
        self.zz_rdict['MANGAIN'] = self.MANGAIN
        self.RFPEAKDET = RM_Register_AGC_RFPEAKDET(self.zz_rmio, self.zz_label)
        self.zz_rdict['RFPEAKDET'] = self.RFPEAKDET
        self.IF = RM_Register_AGC_IF(self.zz_rmio, self.zz_label)
        self.zz_rdict['IF'] = self.IF
        self.IFS = RM_Register_AGC_IFS(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFS'] = self.IFS
        self.IFC = RM_Register_AGC_IFC(self.zz_rmio, self.zz_label)
        self.zz_rdict['IFC'] = self.IFC
        self.IEN = RM_Register_AGC_IEN(self.zz_rmio, self.zz_label)
        self.zz_rdict['IEN'] = self.IEN
        self.CMD = RM_Register_AGC_CMD(self.zz_rmio, self.zz_label)
        self.zz_rdict['CMD'] = self.CMD
        self.GAINRANGE = RM_Register_AGC_GAINRANGE(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINRANGE'] = self.GAINRANGE
        self.GAININDEX = RM_Register_AGC_GAININDEX(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAININDEX'] = self.GAININDEX
        self.SLICECODE = RM_Register_AGC_SLICECODE(self.zz_rmio, self.zz_label)
        self.zz_rdict['SLICECODE'] = self.SLICECODE
        self.ATTENCODE1 = RM_Register_AGC_ATTENCODE1(self.zz_rmio, self.zz_label)
        self.zz_rdict['ATTENCODE1'] = self.ATTENCODE1
        self.ATTENCODE2 = RM_Register_AGC_ATTENCODE2(self.zz_rmio, self.zz_label)
        self.zz_rdict['ATTENCODE2'] = self.ATTENCODE2
        self.ATTENCODE3 = RM_Register_AGC_ATTENCODE3(self.zz_rmio, self.zz_label)
        self.zz_rdict['ATTENCODE3'] = self.ATTENCODE3
        self.GAINERROR1 = RM_Register_AGC_GAINERROR1(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINERROR1'] = self.GAINERROR1
        self.GAINERROR2 = RM_Register_AGC_GAINERROR2(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINERROR2'] = self.GAINERROR2
        self.GAINERROR3 = RM_Register_AGC_GAINERROR3(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINERROR3'] = self.GAINERROR3
        self.MANUALCTRL = RM_Register_AGC_MANUALCTRL(self.zz_rmio, self.zz_label)
        self.zz_rdict['MANUALCTRL'] = self.MANUALCTRL
        self.GAINSTEPLIM = RM_Register_AGC_GAINSTEPLIM(self.zz_rmio, self.zz_label)
        self.zz_rdict['GAINSTEPLIM'] = self.GAINSTEPLIM
        self.LOOPDEL = RM_Register_AGC_LOOPDEL(self.zz_rmio, self.zz_label)
        self.zz_rdict['LOOPDEL'] = self.LOOPDEL
        self.MININDEX = RM_Register_AGC_MININDEX(self.zz_rmio, self.zz_label)
        self.zz_rdict['MININDEX'] = self.MININDEX
        self.LBT = RM_Register_AGC_LBT(self.zz_rmio, self.zz_label)
        self.zz_rdict['LBT'] = self.LBT
        self.MIRRORIF = RM_Register_AGC_MIRRORIF(self.zz_rmio, self.zz_label)
        self.zz_rdict['MIRRORIF'] = self.MIRRORIF
        self.__dict__['zz_frozen'] = True