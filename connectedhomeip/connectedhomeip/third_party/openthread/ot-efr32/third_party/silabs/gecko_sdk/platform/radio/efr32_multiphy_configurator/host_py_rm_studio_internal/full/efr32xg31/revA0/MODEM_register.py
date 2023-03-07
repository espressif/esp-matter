
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . MODEM_field import *


class RM_Register_MODEM_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IPVERSION, self).__init__(rmio, label,
            0xa8014000, 0x000,
            'IPVERSION', 'MODEM.IPVERSION', 'read-only',
            u"",
            0x00000003, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_MODEM_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_EN, self).__init__(rmio, label,
            0xa8014000, 0x004,
            'EN', 'MODEM.EN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_MODEM_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IF, self).__init__(rmio, label,
            0xa8014000, 0x008,
            'IF', 'MODEM.IF', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXFRAMESENT = RM_Field_MODEM_IF_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_IF_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_IF_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
        self.TXRAMPDONE = RM_Field_MODEM_IF_TXRAMPDONE(self)
        self.zz_fdict['TXRAMPDONE'] = self.TXRAMPDONE
        self.LDTNOARR = RM_Field_MODEM_IF_LDTNOARR(self)
        self.zz_fdict['LDTNOARR'] = self.LDTNOARR
        self.PHDSADET = RM_Field_MODEM_IF_PHDSADET(self)
        self.zz_fdict['PHDSADET'] = self.PHDSADET
        self.PHYUNCODEDET = RM_Field_MODEM_IF_PHYUNCODEDET(self)
        self.zz_fdict['PHYUNCODEDET'] = self.PHYUNCODEDET
        self.PHYCODEDET = RM_Field_MODEM_IF_PHYCODEDET(self)
        self.zz_fdict['PHYCODEDET'] = self.PHYCODEDET
        self.RXTIMDET = RM_Field_MODEM_IF_RXTIMDET(self)
        self.zz_fdict['RXTIMDET'] = self.RXTIMDET
        self.RXPREDET = RM_Field_MODEM_IF_RXPREDET(self)
        self.zz_fdict['RXPREDET'] = self.RXPREDET
        self.RXFRAMEDET0 = RM_Field_MODEM_IF_RXFRAMEDET0(self)
        self.zz_fdict['RXFRAMEDET0'] = self.RXFRAMEDET0
        self.RXFRAMEDET1 = RM_Field_MODEM_IF_RXFRAMEDET1(self)
        self.zz_fdict['RXFRAMEDET1'] = self.RXFRAMEDET1
        self.RXTIMLOST = RM_Field_MODEM_IF_RXTIMLOST(self)
        self.zz_fdict['RXTIMLOST'] = self.RXTIMLOST
        self.RXPRELOST = RM_Field_MODEM_IF_RXPRELOST(self)
        self.zz_fdict['RXPRELOST'] = self.RXPRELOST
        self.RXFRAMEDETOF = RM_Field_MODEM_IF_RXFRAMEDETOF(self)
        self.zz_fdict['RXFRAMEDETOF'] = self.RXFRAMEDETOF
        self.RXTIMNF = RM_Field_MODEM_IF_RXTIMNF(self)
        self.zz_fdict['RXTIMNF'] = self.RXTIMNF
        self.FRCTIMOUT = RM_Field_MODEM_IF_FRCTIMOUT(self)
        self.zz_fdict['FRCTIMOUT'] = self.FRCTIMOUT
        self.ETS = RM_Field_MODEM_IF_ETS(self)
        self.zz_fdict['ETS'] = self.ETS
        self.CFGANTPATTRD = RM_Field_MODEM_IF_CFGANTPATTRD(self)
        self.zz_fdict['CFGANTPATTRD'] = self.CFGANTPATTRD
        self.RXRESTARTRSSIMAPRE = RM_Field_MODEM_IF_RXRESTARTRSSIMAPRE(self)
        self.zz_fdict['RXRESTARTRSSIMAPRE'] = self.RXRESTARTRSSIMAPRE
        self.RXRESTARTRSSIMASYNC = RM_Field_MODEM_IF_RXRESTARTRSSIMASYNC(self)
        self.zz_fdict['RXRESTARTRSSIMASYNC'] = self.RXRESTARTRSSIMASYNC
        self.SQDET = RM_Field_MODEM_IF_SQDET(self)
        self.zz_fdict['SQDET'] = self.SQDET
        self.SQNOTDET = RM_Field_MODEM_IF_SQNOTDET(self)
        self.zz_fdict['SQNOTDET'] = self.SQNOTDET
        self.ANTDIVRDY = RM_Field_MODEM_IF_ANTDIVRDY(self)
        self.zz_fdict['ANTDIVRDY'] = self.ANTDIVRDY
        self.SOFTRESETDONE = RM_Field_MODEM_IF_SOFTRESETDONE(self)
        self.zz_fdict['SOFTRESETDONE'] = self.SOFTRESETDONE
        self.SQPRENOTDET = RM_Field_MODEM_IF_SQPRENOTDET(self)
        self.zz_fdict['SQPRENOTDET'] = self.SQPRENOTDET
        self.SQFRAMENOTDET = RM_Field_MODEM_IF_SQFRAMENOTDET(self)
        self.zz_fdict['SQFRAMENOTDET'] = self.SQFRAMENOTDET
        self.SQAFCOUTOFBAND = RM_Field_MODEM_IF_SQAFCOUTOFBAND(self)
        self.zz_fdict['SQAFCOUTOFBAND'] = self.SQAFCOUTOFBAND
        self.SIDET = RM_Field_MODEM_IF_SIDET(self)
        self.zz_fdict['SIDET'] = self.SIDET
        self.SIRESET = RM_Field_MODEM_IF_SIRESET(self)
        self.zz_fdict['SIRESET'] = self.SIRESET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IEN, self).__init__(rmio, label,
            0xa8014000, 0x00C,
            'IEN', 'MODEM.IEN', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXFRAMESENT = RM_Field_MODEM_IEN_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_IEN_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_IEN_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
        self.TXRAMPDONE = RM_Field_MODEM_IEN_TXRAMPDONE(self)
        self.zz_fdict['TXRAMPDONE'] = self.TXRAMPDONE
        self.LDTNOARR = RM_Field_MODEM_IEN_LDTNOARR(self)
        self.zz_fdict['LDTNOARR'] = self.LDTNOARR
        self.PHDSADET = RM_Field_MODEM_IEN_PHDSADET(self)
        self.zz_fdict['PHDSADET'] = self.PHDSADET
        self.PHYUNCODEDET = RM_Field_MODEM_IEN_PHYUNCODEDET(self)
        self.zz_fdict['PHYUNCODEDET'] = self.PHYUNCODEDET
        self.PHYCODEDET = RM_Field_MODEM_IEN_PHYCODEDET(self)
        self.zz_fdict['PHYCODEDET'] = self.PHYCODEDET
        self.RXTIMDET = RM_Field_MODEM_IEN_RXTIMDET(self)
        self.zz_fdict['RXTIMDET'] = self.RXTIMDET
        self.RXPREDET = RM_Field_MODEM_IEN_RXPREDET(self)
        self.zz_fdict['RXPREDET'] = self.RXPREDET
        self.RXFRAMEDET0 = RM_Field_MODEM_IEN_RXFRAMEDET0(self)
        self.zz_fdict['RXFRAMEDET0'] = self.RXFRAMEDET0
        self.RXFRAMEDET1 = RM_Field_MODEM_IEN_RXFRAMEDET1(self)
        self.zz_fdict['RXFRAMEDET1'] = self.RXFRAMEDET1
        self.RXTIMLOST = RM_Field_MODEM_IEN_RXTIMLOST(self)
        self.zz_fdict['RXTIMLOST'] = self.RXTIMLOST
        self.RXPRELOST = RM_Field_MODEM_IEN_RXPRELOST(self)
        self.zz_fdict['RXPRELOST'] = self.RXPRELOST
        self.RXFRAMEDETOF = RM_Field_MODEM_IEN_RXFRAMEDETOF(self)
        self.zz_fdict['RXFRAMEDETOF'] = self.RXFRAMEDETOF
        self.RXTIMNF = RM_Field_MODEM_IEN_RXTIMNF(self)
        self.zz_fdict['RXTIMNF'] = self.RXTIMNF
        self.FRCTIMOUT = RM_Field_MODEM_IEN_FRCTIMOUT(self)
        self.zz_fdict['FRCTIMOUT'] = self.FRCTIMOUT
        self.ETS = RM_Field_MODEM_IEN_ETS(self)
        self.zz_fdict['ETS'] = self.ETS
        self.CFGANTPATTRD = RM_Field_MODEM_IEN_CFGANTPATTRD(self)
        self.zz_fdict['CFGANTPATTRD'] = self.CFGANTPATTRD
        self.RXRESTARTRSSIMAPRE = RM_Field_MODEM_IEN_RXRESTARTRSSIMAPRE(self)
        self.zz_fdict['RXRESTARTRSSIMAPRE'] = self.RXRESTARTRSSIMAPRE
        self.RXRESTARTRSSIMASYNC = RM_Field_MODEM_IEN_RXRESTARTRSSIMASYNC(self)
        self.zz_fdict['RXRESTARTRSSIMASYNC'] = self.RXRESTARTRSSIMASYNC
        self.SQDET = RM_Field_MODEM_IEN_SQDET(self)
        self.zz_fdict['SQDET'] = self.SQDET
        self.SQNOTDET = RM_Field_MODEM_IEN_SQNOTDET(self)
        self.zz_fdict['SQNOTDET'] = self.SQNOTDET
        self.ANTDIVRDY = RM_Field_MODEM_IEN_ANTDIVRDY(self)
        self.zz_fdict['ANTDIVRDY'] = self.ANTDIVRDY
        self.SOFTRESETDONE = RM_Field_MODEM_IEN_SOFTRESETDONE(self)
        self.zz_fdict['SOFTRESETDONE'] = self.SOFTRESETDONE
        self.SQPRENOTDET = RM_Field_MODEM_IEN_SQPRENOTDET(self)
        self.zz_fdict['SQPRENOTDET'] = self.SQPRENOTDET
        self.SQFRAMENOTDET = RM_Field_MODEM_IEN_SQFRAMENOTDET(self)
        self.zz_fdict['SQFRAMENOTDET'] = self.SQFRAMENOTDET
        self.SQAFCOUTOFBAND = RM_Field_MODEM_IEN_SQAFCOUTOFBAND(self)
        self.zz_fdict['SQAFCOUTOFBAND'] = self.SQAFCOUTOFBAND
        self.SIDET = RM_Field_MODEM_IEN_SIDET(self)
        self.zz_fdict['SIDET'] = self.SIDET
        self.SIRESET = RM_Field_MODEM_IEN_SIRESET(self)
        self.zz_fdict['SIRESET'] = self.SIRESET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SEQIF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SEQIF, self).__init__(rmio, label,
            0xa8014000, 0x010,
            'SEQIF', 'MODEM.SEQIF', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXFRAMESENT = RM_Field_MODEM_SEQIF_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_SEQIF_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_SEQIF_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
        self.TXRAMPDONE = RM_Field_MODEM_SEQIF_TXRAMPDONE(self)
        self.zz_fdict['TXRAMPDONE'] = self.TXRAMPDONE
        self.LDTNOARR = RM_Field_MODEM_SEQIF_LDTNOARR(self)
        self.zz_fdict['LDTNOARR'] = self.LDTNOARR
        self.PHDSADET = RM_Field_MODEM_SEQIF_PHDSADET(self)
        self.zz_fdict['PHDSADET'] = self.PHDSADET
        self.PHYUNCODEDET = RM_Field_MODEM_SEQIF_PHYUNCODEDET(self)
        self.zz_fdict['PHYUNCODEDET'] = self.PHYUNCODEDET
        self.PHYCODEDET = RM_Field_MODEM_SEQIF_PHYCODEDET(self)
        self.zz_fdict['PHYCODEDET'] = self.PHYCODEDET
        self.RXTIMDET = RM_Field_MODEM_SEQIF_RXTIMDET(self)
        self.zz_fdict['RXTIMDET'] = self.RXTIMDET
        self.RXPREDET = RM_Field_MODEM_SEQIF_RXPREDET(self)
        self.zz_fdict['RXPREDET'] = self.RXPREDET
        self.RXFRAMEDET0 = RM_Field_MODEM_SEQIF_RXFRAMEDET0(self)
        self.zz_fdict['RXFRAMEDET0'] = self.RXFRAMEDET0
        self.RXFRAMEDET1 = RM_Field_MODEM_SEQIF_RXFRAMEDET1(self)
        self.zz_fdict['RXFRAMEDET1'] = self.RXFRAMEDET1
        self.RXTIMLOST = RM_Field_MODEM_SEQIF_RXTIMLOST(self)
        self.zz_fdict['RXTIMLOST'] = self.RXTIMLOST
        self.RXPRELOST = RM_Field_MODEM_SEQIF_RXPRELOST(self)
        self.zz_fdict['RXPRELOST'] = self.RXPRELOST
        self.RXFRAMEDETOF = RM_Field_MODEM_SEQIF_RXFRAMEDETOF(self)
        self.zz_fdict['RXFRAMEDETOF'] = self.RXFRAMEDETOF
        self.RXTIMNF = RM_Field_MODEM_SEQIF_RXTIMNF(self)
        self.zz_fdict['RXTIMNF'] = self.RXTIMNF
        self.FRCTIMOUT = RM_Field_MODEM_SEQIF_FRCTIMOUT(self)
        self.zz_fdict['FRCTIMOUT'] = self.FRCTIMOUT
        self.ETS = RM_Field_MODEM_SEQIF_ETS(self)
        self.zz_fdict['ETS'] = self.ETS
        self.CFGANTPATTRD = RM_Field_MODEM_SEQIF_CFGANTPATTRD(self)
        self.zz_fdict['CFGANTPATTRD'] = self.CFGANTPATTRD
        self.RXRESTARTRSSIMAPRE = RM_Field_MODEM_SEQIF_RXRESTARTRSSIMAPRE(self)
        self.zz_fdict['RXRESTARTRSSIMAPRE'] = self.RXRESTARTRSSIMAPRE
        self.RXRESTARTRSSIMASYNC = RM_Field_MODEM_SEQIF_RXRESTARTRSSIMASYNC(self)
        self.zz_fdict['RXRESTARTRSSIMASYNC'] = self.RXRESTARTRSSIMASYNC
        self.SQDET = RM_Field_MODEM_SEQIF_SQDET(self)
        self.zz_fdict['SQDET'] = self.SQDET
        self.SQNOTDET = RM_Field_MODEM_SEQIF_SQNOTDET(self)
        self.zz_fdict['SQNOTDET'] = self.SQNOTDET
        self.ANTDIVRDY = RM_Field_MODEM_SEQIF_ANTDIVRDY(self)
        self.zz_fdict['ANTDIVRDY'] = self.ANTDIVRDY
        self.SOFTRESETDONE = RM_Field_MODEM_SEQIF_SOFTRESETDONE(self)
        self.zz_fdict['SOFTRESETDONE'] = self.SOFTRESETDONE
        self.SQPRENOTDET = RM_Field_MODEM_SEQIF_SQPRENOTDET(self)
        self.zz_fdict['SQPRENOTDET'] = self.SQPRENOTDET
        self.SQFRAMENOTDET = RM_Field_MODEM_SEQIF_SQFRAMENOTDET(self)
        self.zz_fdict['SQFRAMENOTDET'] = self.SQFRAMENOTDET
        self.SQAFCOUTOFBAND = RM_Field_MODEM_SEQIF_SQAFCOUTOFBAND(self)
        self.zz_fdict['SQAFCOUTOFBAND'] = self.SQAFCOUTOFBAND
        self.SIDET = RM_Field_MODEM_SEQIF_SIDET(self)
        self.zz_fdict['SIDET'] = self.SIDET
        self.SIRESET = RM_Field_MODEM_SEQIF_SIRESET(self)
        self.zz_fdict['SIRESET'] = self.SIRESET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SEQIEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SEQIEN, self).__init__(rmio, label,
            0xa8014000, 0x014,
            'SEQIEN', 'MODEM.SEQIEN', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXFRAMESENT = RM_Field_MODEM_SEQIEN_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_SEQIEN_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_SEQIEN_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
        self.TXRAMPDONE = RM_Field_MODEM_SEQIEN_TXRAMPDONE(self)
        self.zz_fdict['TXRAMPDONE'] = self.TXRAMPDONE
        self.LDTNOARR = RM_Field_MODEM_SEQIEN_LDTNOARR(self)
        self.zz_fdict['LDTNOARR'] = self.LDTNOARR
        self.PHDSADET = RM_Field_MODEM_SEQIEN_PHDSADET(self)
        self.zz_fdict['PHDSADET'] = self.PHDSADET
        self.PHYUNCODEDET = RM_Field_MODEM_SEQIEN_PHYUNCODEDET(self)
        self.zz_fdict['PHYUNCODEDET'] = self.PHYUNCODEDET
        self.PHYCODEDET = RM_Field_MODEM_SEQIEN_PHYCODEDET(self)
        self.zz_fdict['PHYCODEDET'] = self.PHYCODEDET
        self.RXTIMDET = RM_Field_MODEM_SEQIEN_RXTIMDET(self)
        self.zz_fdict['RXTIMDET'] = self.RXTIMDET
        self.RXPREDET = RM_Field_MODEM_SEQIEN_RXPREDET(self)
        self.zz_fdict['RXPREDET'] = self.RXPREDET
        self.RXFRAMEDET0 = RM_Field_MODEM_SEQIEN_RXFRAMEDET0(self)
        self.zz_fdict['RXFRAMEDET0'] = self.RXFRAMEDET0
        self.RXFRAMEDET1 = RM_Field_MODEM_SEQIEN_RXFRAMEDET1(self)
        self.zz_fdict['RXFRAMEDET1'] = self.RXFRAMEDET1
        self.RXTIMLOST = RM_Field_MODEM_SEQIEN_RXTIMLOST(self)
        self.zz_fdict['RXTIMLOST'] = self.RXTIMLOST
        self.RXPRELOST = RM_Field_MODEM_SEQIEN_RXPRELOST(self)
        self.zz_fdict['RXPRELOST'] = self.RXPRELOST
        self.RXFRAMEDETOF = RM_Field_MODEM_SEQIEN_RXFRAMEDETOF(self)
        self.zz_fdict['RXFRAMEDETOF'] = self.RXFRAMEDETOF
        self.RXTIMNF = RM_Field_MODEM_SEQIEN_RXTIMNF(self)
        self.zz_fdict['RXTIMNF'] = self.RXTIMNF
        self.FRCTIMOUT = RM_Field_MODEM_SEQIEN_FRCTIMOUT(self)
        self.zz_fdict['FRCTIMOUT'] = self.FRCTIMOUT
        self.ETS = RM_Field_MODEM_SEQIEN_ETS(self)
        self.zz_fdict['ETS'] = self.ETS
        self.CFGANTPATTRD = RM_Field_MODEM_SEQIEN_CFGANTPATTRD(self)
        self.zz_fdict['CFGANTPATTRD'] = self.CFGANTPATTRD
        self.RXRESTARTRSSIMAPRE = RM_Field_MODEM_SEQIEN_RXRESTARTRSSIMAPRE(self)
        self.zz_fdict['RXRESTARTRSSIMAPRE'] = self.RXRESTARTRSSIMAPRE
        self.RXRESTARTRSSIMASYNC = RM_Field_MODEM_SEQIEN_RXRESTARTRSSIMASYNC(self)
        self.zz_fdict['RXRESTARTRSSIMASYNC'] = self.RXRESTARTRSSIMASYNC
        self.SQDET = RM_Field_MODEM_SEQIEN_SQDET(self)
        self.zz_fdict['SQDET'] = self.SQDET
        self.SQNOTDET = RM_Field_MODEM_SEQIEN_SQNOTDET(self)
        self.zz_fdict['SQNOTDET'] = self.SQNOTDET
        self.ANTDIVRDY = RM_Field_MODEM_SEQIEN_ANTDIVRDY(self)
        self.zz_fdict['ANTDIVRDY'] = self.ANTDIVRDY
        self.SOFTRESETDONE = RM_Field_MODEM_SEQIEN_SOFTRESETDONE(self)
        self.zz_fdict['SOFTRESETDONE'] = self.SOFTRESETDONE
        self.SQPRENOTDET = RM_Field_MODEM_SEQIEN_SQPRENOTDET(self)
        self.zz_fdict['SQPRENOTDET'] = self.SQPRENOTDET
        self.SQFRAMENOTDET = RM_Field_MODEM_SEQIEN_SQFRAMENOTDET(self)
        self.zz_fdict['SQFRAMENOTDET'] = self.SQFRAMENOTDET
        self.SQAFCOUTOFBAND = RM_Field_MODEM_SEQIEN_SQAFCOUTOFBAND(self)
        self.zz_fdict['SQAFCOUTOFBAND'] = self.SQAFCOUTOFBAND
        self.SIDET = RM_Field_MODEM_SEQIEN_SIDET(self)
        self.zz_fdict['SIDET'] = self.SIDET
        self.SIRESET = RM_Field_MODEM_SEQIEN_SIRESET(self)
        self.zz_fdict['SIRESET'] = self.SIRESET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS, self).__init__(rmio, label,
            0xa8014000, 0x018,
            'STATUS', 'MODEM.STATUS', 'read-only',
            u"",
            0x00000000, 0xFFFFFFDF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DEMODSTATE = RM_Field_MODEM_STATUS_DEMODSTATE(self)
        self.zz_fdict['DEMODSTATE'] = self.DEMODSTATE
        self.BCRCFEDSADET = RM_Field_MODEM_STATUS_BCRCFEDSADET(self)
        self.zz_fdict['BCRCFEDSADET'] = self.BCRCFEDSADET
        self.FRAMEDETID = RM_Field_MODEM_STATUS_FRAMEDETID(self)
        self.zz_fdict['FRAMEDETID'] = self.FRAMEDETID
        self.TIMSEQINV = RM_Field_MODEM_STATUS_TIMSEQINV(self)
        self.zz_fdict['TIMSEQINV'] = self.TIMSEQINV
        self.TIMLOSTCAUSE = RM_Field_MODEM_STATUS_TIMLOSTCAUSE(self)
        self.zz_fdict['TIMLOSTCAUSE'] = self.TIMLOSTCAUSE
        self.DSADETECTED = RM_Field_MODEM_STATUS_DSADETECTED(self)
        self.zz_fdict['DSADETECTED'] = self.DSADETECTED
        self.DSAFREQESTDONE = RM_Field_MODEM_STATUS_DSAFREQESTDONE(self)
        self.zz_fdict['DSAFREQESTDONE'] = self.DSAFREQESTDONE
        self.VITERBIDEMODTIMDET = RM_Field_MODEM_STATUS_VITERBIDEMODTIMDET(self)
        self.zz_fdict['VITERBIDEMODTIMDET'] = self.VITERBIDEMODTIMDET
        self.VITERBIDEMODFRAMEDET = RM_Field_MODEM_STATUS_VITERBIDEMODFRAMEDET(self)
        self.zz_fdict['VITERBIDEMODFRAMEDET'] = self.VITERBIDEMODFRAMEDET
        self.STAMPSTATE = RM_Field_MODEM_STATUS_STAMPSTATE(self)
        self.zz_fdict['STAMPSTATE'] = self.STAMPSTATE
        self.TRECSDSAADET = RM_Field_MODEM_STATUS_TRECSDSAADET(self)
        self.zz_fdict['TRECSDSAADET'] = self.TRECSDSAADET
        self.CORR = RM_Field_MODEM_STATUS_CORR(self)
        self.zz_fdict['CORR'] = self.CORR
        self.WEAKSYMBOLS = RM_Field_MODEM_STATUS_WEAKSYMBOLS(self)
        self.zz_fdict['WEAKSYMBOLS'] = self.WEAKSYMBOLS
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS2, self).__init__(rmio, label,
            0xa8014000, 0x01C,
            'STATUS2', 'MODEM.STATUS2', 'read-only',
            u"",
            0x00000000, 0xFFFCF0FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHPWRACCUMUX = RM_Field_MODEM_STATUS2_CHPWRACCUMUX(self)
        self.zz_fdict['CHPWRACCUMUX'] = self.CHPWRACCUMUX
        self.LRBLECI = RM_Field_MODEM_STATUS2_LRBLECI(self)
        self.zz_fdict['LRBLECI'] = self.LRBLECI
        self.UNCODEDPHY = RM_Field_MODEM_STATUS2_UNCODEDPHY(self)
        self.zz_fdict['UNCODEDPHY'] = self.UNCODEDPHY
        self.CODEDPHY = RM_Field_MODEM_STATUS2_CODEDPHY(self)
        self.zz_fdict['CODEDPHY'] = self.CODEDPHY
        self.RTCOST = RM_Field_MODEM_STATUS2_RTCOST(self)
        self.zz_fdict['RTCOST'] = self.RTCOST
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS3, self).__init__(rmio, label,
            0xa8014000, 0x020,
            'STATUS3', 'MODEM.STATUS3', 'read-only',
            u"",
            0x00000000, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BBPFOUTABS1 = RM_Field_MODEM_STATUS3_BBPFOUTABS1(self)
        self.zz_fdict['BBPFOUTABS1'] = self.BBPFOUTABS1
        self.BBPFOUTABS = RM_Field_MODEM_STATUS3_BBPFOUTABS(self)
        self.zz_fdict['BBPFOUTABS'] = self.BBPFOUTABS
        self.LRDSALIVE = RM_Field_MODEM_STATUS3_LRDSALIVE(self)
        self.zz_fdict['LRDSALIVE'] = self.LRDSALIVE
        self.COHDSALIVE = RM_Field_MODEM_STATUS3_COHDSALIVE(self)
        self.zz_fdict['COHDSALIVE'] = self.COHDSALIVE
        self.LRDSADET = RM_Field_MODEM_STATUS3_LRDSADET(self)
        self.zz_fdict['LRDSADET'] = self.LRDSADET
        self.COHDSADET = RM_Field_MODEM_STATUS3_COHDSADET(self)
        self.zz_fdict['COHDSADET'] = self.COHDSADET
        self.SYNCSECPEAKABTH = RM_Field_MODEM_STATUS3_SYNCSECPEAKABTH(self)
        self.zz_fdict['SYNCSECPEAKABTH'] = self.SYNCSECPEAKABTH
        self.SOFTRSTDONE = RM_Field_MODEM_STATUS3_SOFTRSTDONE(self)
        self.zz_fdict['SOFTRSTDONE'] = self.SOFTRSTDONE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS4, self).__init__(rmio, label,
            0xa8014000, 0x024,
            'STATUS4', 'MODEM.STATUS4', 'read-only',
            u"",
            0x00000000, 0x01FF01FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ANT0RSSI = RM_Field_MODEM_STATUS4_ANT0RSSI(self)
        self.zz_fdict['ANT0RSSI'] = self.ANT0RSSI
        self.ANT1RSSI = RM_Field_MODEM_STATUS4_ANT1RSSI(self)
        self.zz_fdict['ANT1RSSI'] = self.ANT1RSSI
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS5, self).__init__(rmio, label,
            0xa8014000, 0x028,
            'STATUS5', 'MODEM.STATUS5', 'read-only',
            u"",
            0x00000000, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RXRESTARTMAFLTDOUT = RM_Field_MODEM_STATUS5_RXRESTARTMAFLTDOUT(self)
        self.zz_fdict['RXRESTARTMAFLTDOUT'] = self.RXRESTARTMAFLTDOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS6, self).__init__(rmio, label,
            0xa8014000, 0x02C,
            'STATUS6', 'MODEM.STATUS6', 'read-only',
            u"",
            0x00000000, 0xC00FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ANT0CORR = RM_Field_MODEM_STATUS6_ANT0CORR(self)
        self.zz_fdict['ANT0CORR'] = self.ANT0CORR
        self.ANT1CORR = RM_Field_MODEM_STATUS6_ANT1CORR(self)
        self.zz_fdict['ANT1CORR'] = self.ANT1CORR
        self.ANT0OUT = RM_Field_MODEM_STATUS6_ANT0OUT(self)
        self.zz_fdict['ANT0OUT'] = self.ANT0OUT
        self.ANT1OUT = RM_Field_MODEM_STATUS6_ANT1OUT(self)
        self.zz_fdict['ANT1OUT'] = self.ANT1OUT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_STATUS7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS7, self).__init__(rmio, label,
            0xa8014000, 0x030,
            'STATUS7', 'MODEM.STATUS7', 'read-only',
            u"",
            0x00000000, 0xBFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FDEVEST = RM_Field_MODEM_STATUS7_FDEVEST(self)
        self.zz_fdict['FDEVEST'] = self.FDEVEST
        self.DEMODSOFT = RM_Field_MODEM_STATUS7_DEMODSOFT(self)
        self.zz_fdict['DEMODSOFT'] = self.DEMODSOFT
        self.CFEPHDIFF = RM_Field_MODEM_STATUS7_CFEPHDIFF(self)
        self.zz_fdict['CFEPHDIFF'] = self.CFEPHDIFF
        self.MINCOSTPASS = RM_Field_MODEM_STATUS7_MINCOSTPASS(self)
        self.zz_fdict['MINCOSTPASS'] = self.MINCOSTPASS
        self.CFEDSADET = RM_Field_MODEM_STATUS7_CFEDSADET(self)
        self.zz_fdict['CFEDSADET'] = self.CFEDSADET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TIMDETSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TIMDETSTATUS, self).__init__(rmio, label,
            0xa8014000, 0x034,
            'TIMDETSTATUS', 'MODEM.TIMDETSTATUS', 'read-only',
            u"",
            0x00000000, 0x1F0FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TIMDETCORR = RM_Field_MODEM_TIMDETSTATUS_TIMDETCORR(self)
        self.zz_fdict['TIMDETCORR'] = self.TIMDETCORR
        self.TIMDETFREQOFFEST = RM_Field_MODEM_TIMDETSTATUS_TIMDETFREQOFFEST(self)
        self.zz_fdict['TIMDETFREQOFFEST'] = self.TIMDETFREQOFFEST
        self.TIMDETPREERRORS = RM_Field_MODEM_TIMDETSTATUS_TIMDETPREERRORS(self)
        self.zz_fdict['TIMDETPREERRORS'] = self.TIMDETPREERRORS
        self.TIMDETPASS = RM_Field_MODEM_TIMDETSTATUS_TIMDETPASS(self)
        self.zz_fdict['TIMDETPASS'] = self.TIMDETPASS
        self.TIMDETINDEX = RM_Field_MODEM_TIMDETSTATUS_TIMDETINDEX(self)
        self.zz_fdict['TIMDETINDEX'] = self.TIMDETINDEX
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_FSMSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_FSMSTATUS, self).__init__(rmio, label,
            0xa8014000, 0x038,
            'FSMSTATUS', 'MODEM.FSMSTATUS', 'read-only',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DETSTATE = RM_Field_MODEM_FSMSTATUS_DETSTATE(self)
        self.zz_fdict['DETSTATE'] = self.DETSTATE
        self.DSASTATE = RM_Field_MODEM_FSMSTATUS_DSASTATE(self)
        self.zz_fdict['DSASTATE'] = self.DSASTATE
        self.LRBLESTATE = RM_Field_MODEM_FSMSTATUS_LRBLESTATE(self)
        self.zz_fdict['LRBLESTATE'] = self.LRBLESTATE
        self.NBBLESTATE = RM_Field_MODEM_FSMSTATUS_NBBLESTATE(self)
        self.zz_fdict['NBBLESTATE'] = self.NBBLESTATE
        self.ANTDIVSTATE = RM_Field_MODEM_FSMSTATUS_ANTDIVSTATE(self)
        self.zz_fdict['ANTDIVSTATE'] = self.ANTDIVSTATE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_FREQOFFEST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_FREQOFFEST, self).__init__(rmio, label,
            0xa8014000, 0x03C,
            'FREQOFFEST', 'MODEM.FREQOFFEST', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FREQOFFEST = RM_Field_MODEM_FREQOFFEST_FREQOFFEST(self)
        self.zz_fdict['FREQOFFEST'] = self.FREQOFFEST
        self.CORRVAL = RM_Field_MODEM_FREQOFFEST_CORRVAL(self)
        self.zz_fdict['CORRVAL'] = self.CORRVAL
        self.SOFTVAL = RM_Field_MODEM_FREQOFFEST_SOFTVAL(self)
        self.zz_fdict['SOFTVAL'] = self.SOFTVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFCADJRX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFCADJRX, self).__init__(rmio, label,
            0xa8014000, 0x040,
            'AFCADJRX', 'MODEM.AFCADJRX', 'read-write',
            u"",
            0x00000000, 0xF1F7FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AFCADJRX = RM_Field_MODEM_AFCADJRX_AFCADJRX(self)
        self.zz_fdict['AFCADJRX'] = self.AFCADJRX
        self.AFCSCALEM = RM_Field_MODEM_AFCADJRX_AFCSCALEM(self)
        self.zz_fdict['AFCSCALEM'] = self.AFCSCALEM
        self.AFCSCALEE = RM_Field_MODEM_AFCADJRX_AFCSCALEE(self)
        self.zz_fdict['AFCSCALEE'] = self.AFCSCALEE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFCADJTX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFCADJTX, self).__init__(rmio, label,
            0xa8014000, 0x044,
            'AFCADJTX', 'MODEM.AFCADJTX', 'read-write',
            u"",
            0x00000000, 0xF1F7FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AFCADJTX = RM_Field_MODEM_AFCADJTX_AFCADJTX(self)
        self.zz_fdict['AFCADJTX'] = self.AFCADJTX
        self.AFCSCALEM = RM_Field_MODEM_AFCADJTX_AFCSCALEM(self)
        self.zz_fdict['AFCSCALEM'] = self.AFCSCALEM
        self.AFCSCALEE = RM_Field_MODEM_AFCADJTX_AFCSCALEE(self)
        self.zz_fdict['AFCSCALEE'] = self.AFCSCALEE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL0, self).__init__(rmio, label,
            0xa8014000, 0x04C,
            'CTRL0', 'MODEM.CTRL0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FDM0DIFFDIS = RM_Field_MODEM_CTRL0_FDM0DIFFDIS(self)
        self.zz_fdict['FDM0DIFFDIS'] = self.FDM0DIFFDIS
        self.MAPFSK = RM_Field_MODEM_CTRL0_MAPFSK(self)
        self.zz_fdict['MAPFSK'] = self.MAPFSK
        self.CODING = RM_Field_MODEM_CTRL0_CODING(self)
        self.zz_fdict['CODING'] = self.CODING
        self.MODFORMAT = RM_Field_MODEM_CTRL0_MODFORMAT(self)
        self.zz_fdict['MODFORMAT'] = self.MODFORMAT
        self.DUALCORROPTDIS = RM_Field_MODEM_CTRL0_DUALCORROPTDIS(self)
        self.zz_fdict['DUALCORROPTDIS'] = self.DUALCORROPTDIS
        self.OOKASYNCPIN = RM_Field_MODEM_CTRL0_OOKASYNCPIN(self)
        self.zz_fdict['OOKASYNCPIN'] = self.OOKASYNCPIN
        self.DSSSLEN = RM_Field_MODEM_CTRL0_DSSSLEN(self)
        self.zz_fdict['DSSSLEN'] = self.DSSSLEN
        self.DSSSSHIFTS = RM_Field_MODEM_CTRL0_DSSSSHIFTS(self)
        self.zz_fdict['DSSSSHIFTS'] = self.DSSSSHIFTS
        self.DSSSDOUBLE = RM_Field_MODEM_CTRL0_DSSSDOUBLE(self)
        self.zz_fdict['DSSSDOUBLE'] = self.DSSSDOUBLE
        self.DETDIS = RM_Field_MODEM_CTRL0_DETDIS(self)
        self.zz_fdict['DETDIS'] = self.DETDIS
        self.DIFFENCMODE = RM_Field_MODEM_CTRL0_DIFFENCMODE(self)
        self.zz_fdict['DIFFENCMODE'] = self.DIFFENCMODE
        self.SHAPING = RM_Field_MODEM_CTRL0_SHAPING(self)
        self.zz_fdict['SHAPING'] = self.SHAPING
        self.DEMODRAWDATASEL = RM_Field_MODEM_CTRL0_DEMODRAWDATASEL(self)
        self.zz_fdict['DEMODRAWDATASEL'] = self.DEMODRAWDATASEL
        self.FRAMEDETDEL = RM_Field_MODEM_CTRL0_FRAMEDETDEL(self)
        self.zz_fdict['FRAMEDETDEL'] = self.FRAMEDETDEL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL1, self).__init__(rmio, label,
            0xa8014000, 0x050,
            'CTRL1', 'MODEM.CTRL1', 'read-write',
            u"",
            0x00000000, 0xFFFFDFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNCBITS = RM_Field_MODEM_CTRL1_SYNCBITS(self)
        self.zz_fdict['SYNCBITS'] = self.SYNCBITS
        self.SYNCERRORS = RM_Field_MODEM_CTRL1_SYNCERRORS(self)
        self.zz_fdict['SYNCERRORS'] = self.SYNCERRORS
        self.DUALSYNC = RM_Field_MODEM_CTRL1_DUALSYNC(self)
        self.zz_fdict['DUALSYNC'] = self.DUALSYNC
        self.TXSYNC = RM_Field_MODEM_CTRL1_TXSYNC(self)
        self.zz_fdict['TXSYNC'] = self.TXSYNC
        self.SYNCDATA = RM_Field_MODEM_CTRL1_SYNCDATA(self)
        self.zz_fdict['SYNCDATA'] = self.SYNCDATA
        self.SYNC1INV = RM_Field_MODEM_CTRL1_SYNC1INV(self)
        self.zz_fdict['SYNC1INV'] = self.SYNC1INV
        self.COMPMODE = RM_Field_MODEM_CTRL1_COMPMODE(self)
        self.zz_fdict['COMPMODE'] = self.COMPMODE
        self.RESYNCPER = RM_Field_MODEM_CTRL1_RESYNCPER(self)
        self.zz_fdict['RESYNCPER'] = self.RESYNCPER
        self.PHASEDEMOD = RM_Field_MODEM_CTRL1_PHASEDEMOD(self)
        self.zz_fdict['PHASEDEMOD'] = self.PHASEDEMOD
        self.FREQOFFESTPER = RM_Field_MODEM_CTRL1_FREQOFFESTPER(self)
        self.zz_fdict['FREQOFFESTPER'] = self.FREQOFFESTPER
        self.FREQOFFESTLIM = RM_Field_MODEM_CTRL1_FREQOFFESTLIM(self)
        self.zz_fdict['FREQOFFESTLIM'] = self.FREQOFFESTLIM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL2, self).__init__(rmio, label,
            0xa8014000, 0x054,
            'CTRL2', 'MODEM.CTRL2', 'read-write',
            u"",
            0x00001000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SQITHRESH = RM_Field_MODEM_CTRL2_SQITHRESH(self)
        self.zz_fdict['SQITHRESH'] = self.SQITHRESH
        self.RXFRCDIS = RM_Field_MODEM_CTRL2_RXFRCDIS(self)
        self.zz_fdict['RXFRCDIS'] = self.RXFRCDIS
        self.RXPINMODE = RM_Field_MODEM_CTRL2_RXPINMODE(self)
        self.zz_fdict['RXPINMODE'] = self.RXPINMODE
        self.TXPINMODE = RM_Field_MODEM_CTRL2_TXPINMODE(self)
        self.zz_fdict['TXPINMODE'] = self.TXPINMODE
        self.DATAFILTER = RM_Field_MODEM_CTRL2_DATAFILTER(self)
        self.zz_fdict['DATAFILTER'] = self.DATAFILTER
        self.BRDIVA = RM_Field_MODEM_CTRL2_BRDIVA(self)
        self.zz_fdict['BRDIVA'] = self.BRDIVA
        self.BRDIVB = RM_Field_MODEM_CTRL2_BRDIVB(self)
        self.zz_fdict['BRDIVB'] = self.BRDIVB
        self.DEVMULA = RM_Field_MODEM_CTRL2_DEVMULA(self)
        self.zz_fdict['DEVMULA'] = self.DEVMULA
        self.DEVMULB = RM_Field_MODEM_CTRL2_DEVMULB(self)
        self.zz_fdict['DEVMULB'] = self.DEVMULB
        self.RATESELMODE = RM_Field_MODEM_CTRL2_RATESELMODE(self)
        self.zz_fdict['RATESELMODE'] = self.RATESELMODE
        self.DEVWEIGHTDIS = RM_Field_MODEM_CTRL2_DEVWEIGHTDIS(self)
        self.zz_fdict['DEVWEIGHTDIS'] = self.DEVWEIGHTDIS
        self.DMASEL = RM_Field_MODEM_CTRL2_DMASEL(self)
        self.zz_fdict['DMASEL'] = self.DMASEL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL3, self).__init__(rmio, label,
            0xa8014000, 0x058,
            'CTRL3', 'MODEM.CTRL3', 'read-write',
            u"",
            0x00008000, 0xFFFFFF8F,
            0x00001000, 0x00002000,
            0x00003000)

        self.PRSDINEN = RM_Field_MODEM_CTRL3_PRSDINEN(self)
        self.zz_fdict['PRSDINEN'] = self.PRSDINEN
        self.TIMINGBASESGAIN = RM_Field_MODEM_CTRL3_TIMINGBASESGAIN(self)
        self.zz_fdict['TIMINGBASESGAIN'] = self.TIMINGBASESGAIN
        self.DEVMULBCW = RM_Field_MODEM_CTRL3_DEVMULBCW(self)
        self.zz_fdict['DEVMULBCW'] = self.DEVMULBCW
        self.RAMTESTEN = RM_Field_MODEM_CTRL3_RAMTESTEN(self)
        self.zz_fdict['RAMTESTEN'] = self.RAMTESTEN
        self.ANTDIVMODE = RM_Field_MODEM_CTRL3_ANTDIVMODE(self)
        self.zz_fdict['ANTDIVMODE'] = self.ANTDIVMODE
        self.ANTDIVREPEATDIS = RM_Field_MODEM_CTRL3_ANTDIVREPEATDIS(self)
        self.zz_fdict['ANTDIVREPEATDIS'] = self.ANTDIVREPEATDIS
        self.TSAMPMODE = RM_Field_MODEM_CTRL3_TSAMPMODE(self)
        self.zz_fdict['TSAMPMODE'] = self.TSAMPMODE
        self.TSAMPDEL = RM_Field_MODEM_CTRL3_TSAMPDEL(self)
        self.zz_fdict['TSAMPDEL'] = self.TSAMPDEL
        self.TSAMPLIM = RM_Field_MODEM_CTRL3_TSAMPLIM(self)
        self.zz_fdict['TSAMPLIM'] = self.TSAMPLIM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL4, self).__init__(rmio, label,
            0xa8014000, 0x05C,
            'CTRL4', 'MODEM.CTRL4', 'read-write',
            u"",
            0x03000000, 0xBFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ISICOMP = RM_Field_MODEM_CTRL4_ISICOMP(self)
        self.zz_fdict['ISICOMP'] = self.ISICOMP
        self.DEVOFFCOMP = RM_Field_MODEM_CTRL4_DEVOFFCOMP(self)
        self.zz_fdict['DEVOFFCOMP'] = self.DEVOFFCOMP
        self.PREDISTGAIN = RM_Field_MODEM_CTRL4_PREDISTGAIN(self)
        self.zz_fdict['PREDISTGAIN'] = self.PREDISTGAIN
        self.PREDISTDEB = RM_Field_MODEM_CTRL4_PREDISTDEB(self)
        self.zz_fdict['PREDISTDEB'] = self.PREDISTDEB
        self.PREDISTAVG = RM_Field_MODEM_CTRL4_PREDISTAVG(self)
        self.zz_fdict['PREDISTAVG'] = self.PREDISTAVG
        self.PREDISTRST = RM_Field_MODEM_CTRL4_PREDISTRST(self)
        self.zz_fdict['PREDISTRST'] = self.PREDISTRST
        self.PHASECLICKFILT = RM_Field_MODEM_CTRL4_PHASECLICKFILT(self)
        self.zz_fdict['PHASECLICKFILT'] = self.PHASECLICKFILT
        self.SOFTDSSSMODE = RM_Field_MODEM_CTRL4_SOFTDSSSMODE(self)
        self.zz_fdict['SOFTDSSSMODE'] = self.SOFTDSSSMODE
        self.ADCSATLEVEL = RM_Field_MODEM_CTRL4_ADCSATLEVEL(self)
        self.zz_fdict['ADCSATLEVEL'] = self.ADCSATLEVEL
        self.ADCSATDENS = RM_Field_MODEM_CTRL4_ADCSATDENS(self)
        self.zz_fdict['ADCSATDENS'] = self.ADCSATDENS
        self.OFFSETPHASEMASKING = RM_Field_MODEM_CTRL4_OFFSETPHASEMASKING(self)
        self.zz_fdict['OFFSETPHASEMASKING'] = self.OFFSETPHASEMASKING
        self.OFFSETPHASESCALING = RM_Field_MODEM_CTRL4_OFFSETPHASESCALING(self)
        self.zz_fdict['OFFSETPHASESCALING'] = self.OFFSETPHASESCALING
        self.CLKUNDIVREQ = RM_Field_MODEM_CTRL4_CLKUNDIVREQ(self)
        self.zz_fdict['CLKUNDIVREQ'] = self.CLKUNDIVREQ
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL5, self).__init__(rmio, label,
            0xa8014000, 0x060,
            'CTRL5', 'MODEM.CTRL5', 'read-write',
            u"",
            0x00000000, 0x7FFFFFFE,
            0x00001000, 0x00002000,
            0x00003000)

        self.BRCALEN = RM_Field_MODEM_CTRL5_BRCALEN(self)
        self.zz_fdict['BRCALEN'] = self.BRCALEN
        self.BRCALMODE = RM_Field_MODEM_CTRL5_BRCALMODE(self)
        self.zz_fdict['BRCALMODE'] = self.BRCALMODE
        self.BRCALAVG = RM_Field_MODEM_CTRL5_BRCALAVG(self)
        self.zz_fdict['BRCALAVG'] = self.BRCALAVG
        self.DETDEL = RM_Field_MODEM_CTRL5_DETDEL(self)
        self.zz_fdict['DETDEL'] = self.DETDEL
        self.TDEDGE = RM_Field_MODEM_CTRL5_TDEDGE(self)
        self.zz_fdict['TDEDGE'] = self.TDEDGE
        self.TREDGE = RM_Field_MODEM_CTRL5_TREDGE(self)
        self.zz_fdict['TREDGE'] = self.TREDGE
        self.DSSSCTD = RM_Field_MODEM_CTRL5_DSSSCTD(self)
        self.zz_fdict['DSSSCTD'] = self.DSSSCTD
        self.RESYNCLIMIT = RM_Field_MODEM_CTRL5_RESYNCLIMIT(self)
        self.zz_fdict['RESYNCLIMIT'] = self.RESYNCLIMIT
        self.FOEPREAVG = RM_Field_MODEM_CTRL5_FOEPREAVG(self)
        self.zz_fdict['FOEPREAVG'] = self.FOEPREAVG
        self.POEPER = RM_Field_MODEM_CTRL5_POEPER(self)
        self.zz_fdict['POEPER'] = self.POEPER
        self.DEMODRAWDATASEL2 = RM_Field_MODEM_CTRL5_DEMODRAWDATASEL2(self)
        self.zz_fdict['DEMODRAWDATASEL2'] = self.DEMODRAWDATASEL2
        self.LINCORR = RM_Field_MODEM_CTRL5_LINCORR(self)
        self.zz_fdict['LINCORR'] = self.LINCORR
        self.DEC2 = RM_Field_MODEM_CTRL5_DEC2(self)
        self.zz_fdict['DEC2'] = self.DEC2
        self.RESYNCBAUDTRANS = RM_Field_MODEM_CTRL5_RESYNCBAUDTRANS(self)
        self.zz_fdict['RESYNCBAUDTRANS'] = self.RESYNCBAUDTRANS
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL6, self).__init__(rmio, label,
            0xa8014000, 0x064,
            'CTRL6', 'MODEM.CTRL6', 'read-write',
            u"",
            0x00000000, 0xF6FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TDREW = RM_Field_MODEM_CTRL6_TDREW(self)
        self.zz_fdict['TDREW'] = self.TDREW
        self.PREBASES = RM_Field_MODEM_CTRL6_PREBASES(self)
        self.zz_fdict['PREBASES'] = self.PREBASES
        self.PSTIMABORT0 = RM_Field_MODEM_CTRL6_PSTIMABORT0(self)
        self.zz_fdict['PSTIMABORT0'] = self.PSTIMABORT0
        self.PSTIMABORT1 = RM_Field_MODEM_CTRL6_PSTIMABORT1(self)
        self.zz_fdict['PSTIMABORT1'] = self.PSTIMABORT1
        self.PSTIMABORT2 = RM_Field_MODEM_CTRL6_PSTIMABORT2(self)
        self.zz_fdict['PSTIMABORT2'] = self.PSTIMABORT2
        self.PSTIMABORT3 = RM_Field_MODEM_CTRL6_PSTIMABORT3(self)
        self.zz_fdict['PSTIMABORT3'] = self.PSTIMABORT3
        self.ARW = RM_Field_MODEM_CTRL6_ARW(self)
        self.zz_fdict['ARW'] = self.ARW
        self.TIMTHRESHGAIN = RM_Field_MODEM_CTRL6_TIMTHRESHGAIN(self)
        self.zz_fdict['TIMTHRESHGAIN'] = self.TIMTHRESHGAIN
        self.CPLXCORREN = RM_Field_MODEM_CTRL6_CPLXCORREN(self)
        self.zz_fdict['CPLXCORREN'] = self.CPLXCORREN
        self.DSSS3SYMBOLSYNCEN = RM_Field_MODEM_CTRL6_DSSS3SYMBOLSYNCEN(self)
        self.zz_fdict['DSSS3SYMBOLSYNCEN'] = self.DSSS3SYMBOLSYNCEN
        self.TXDBPSKINV = RM_Field_MODEM_CTRL6_TXDBPSKINV(self)
        self.zz_fdict['TXDBPSKINV'] = self.TXDBPSKINV
        self.TXDBPSKRAMPEN = RM_Field_MODEM_CTRL6_TXDBPSKRAMPEN(self)
        self.zz_fdict['TXDBPSKRAMPEN'] = self.TXDBPSKRAMPEN
        self.CODINGB = RM_Field_MODEM_CTRL6_CODINGB(self)
        self.zz_fdict['CODINGB'] = self.CODINGB
        self.RXRESTARTUPONRSSI = RM_Field_MODEM_CTRL6_RXRESTARTUPONRSSI(self)
        self.zz_fdict['RXRESTARTUPONRSSI'] = self.RXRESTARTUPONRSSI
        self.RXRESTARTUPONSHORTRSSI = RM_Field_MODEM_CTRL6_RXRESTARTUPONSHORTRSSI(self)
        self.zz_fdict['RXRESTARTUPONSHORTRSSI'] = self.RXRESTARTUPONSHORTRSSI
        self.RXBRCALCDIS = RM_Field_MODEM_CTRL6_RXBRCALCDIS(self)
        self.zz_fdict['RXBRCALCDIS'] = self.RXBRCALCDIS
        self.DEMODRESTARTALL = RM_Field_MODEM_CTRL6_DEMODRESTARTALL(self)
        self.zz_fdict['DEMODRESTARTALL'] = self.DEMODRESTARTALL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TXBR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TXBR, self).__init__(rmio, label,
            0xa8014000, 0x068,
            'TXBR', 'MODEM.TXBR', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXBRNUM = RM_Field_MODEM_TXBR_TXBRNUM(self)
        self.zz_fdict['TXBRNUM'] = self.TXBRNUM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RXBR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RXBR, self).__init__(rmio, label,
            0xa8014000, 0x06C,
            'RXBR', 'MODEM.RXBR', 'read-write',
            u"",
            0x00000000, 0x00001FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RXBRNUM = RM_Field_MODEM_RXBR_RXBRNUM(self)
        self.zz_fdict['RXBRNUM'] = self.RXBRNUM
        self.RXBRDEN = RM_Field_MODEM_RXBR_RXBRDEN(self)
        self.zz_fdict['RXBRDEN'] = self.RXBRDEN
        self.RXBRINT = RM_Field_MODEM_RXBR_RXBRINT(self)
        self.zz_fdict['RXBRINT'] = self.RXBRINT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PRE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PRE, self).__init__(rmio, label,
            0xa8014000, 0x074,
            'PRE', 'MODEM.PRE', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BASE = RM_Field_MODEM_PRE_BASE(self)
        self.zz_fdict['BASE'] = self.BASE
        self.BASEBITS = RM_Field_MODEM_PRE_BASEBITS(self)
        self.zz_fdict['BASEBITS'] = self.BASEBITS
        self.PRESYMB4FSK = RM_Field_MODEM_PRE_PRESYMB4FSK(self)
        self.zz_fdict['PRESYMB4FSK'] = self.PRESYMB4FSK
        self.PREERRORS = RM_Field_MODEM_PRE_PREERRORS(self)
        self.zz_fdict['PREERRORS'] = self.PREERRORS
        self.DSSSPRE = RM_Field_MODEM_PRE_DSSSPRE(self)
        self.zz_fdict['DSSSPRE'] = self.DSSSPRE
        self.SYNCSYMB4FSK = RM_Field_MODEM_PRE_SYNCSYMB4FSK(self)
        self.zz_fdict['SYNCSYMB4FSK'] = self.SYNCSYMB4FSK
        self.PREAMBDETEN = RM_Field_MODEM_PRE_PREAMBDETEN(self)
        self.zz_fdict['PREAMBDETEN'] = self.PREAMBDETEN
        self.PREWNDERRORS = RM_Field_MODEM_PRE_PREWNDERRORS(self)
        self.zz_fdict['PREWNDERRORS'] = self.PREWNDERRORS
        self.TXBASES = RM_Field_MODEM_PRE_TXBASES(self)
        self.zz_fdict['TXBASES'] = self.TXBASES
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SYNC0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SYNC0, self).__init__(rmio, label,
            0xa8014000, 0x078,
            'SYNC0', 'MODEM.SYNC0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNC0 = RM_Field_MODEM_SYNC0_SYNC0(self)
        self.zz_fdict['SYNC0'] = self.SYNC0
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SYNC1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SYNC1, self).__init__(rmio, label,
            0xa8014000, 0x07C,
            'SYNC1', 'MODEM.SYNC1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNC1 = RM_Field_MODEM_SYNC1_SYNC1(self)
        self.zz_fdict['SYNC1'] = self.SYNC1
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TIMING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TIMING, self).__init__(rmio, label,
            0xa8014000, 0x080,
            'TIMING', 'MODEM.TIMING', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TIMTHRESH = RM_Field_MODEM_TIMING_TIMTHRESH(self)
        self.zz_fdict['TIMTHRESH'] = self.TIMTHRESH
        self.TIMINGBASES = RM_Field_MODEM_TIMING_TIMINGBASES(self)
        self.zz_fdict['TIMINGBASES'] = self.TIMINGBASES
        self.ADDTIMSEQ = RM_Field_MODEM_TIMING_ADDTIMSEQ(self)
        self.zz_fdict['ADDTIMSEQ'] = self.ADDTIMSEQ
        self.TIMSEQINVEN = RM_Field_MODEM_TIMING_TIMSEQINVEN(self)
        self.zz_fdict['TIMSEQINVEN'] = self.TIMSEQINVEN
        self.TIMSEQSYNC = RM_Field_MODEM_TIMING_TIMSEQSYNC(self)
        self.zz_fdict['TIMSEQSYNC'] = self.TIMSEQSYNC
        self.FDM0THRESH = RM_Field_MODEM_TIMING_FDM0THRESH(self)
        self.zz_fdict['FDM0THRESH'] = self.FDM0THRESH
        self.OFFSUBNUM = RM_Field_MODEM_TIMING_OFFSUBNUM(self)
        self.zz_fdict['OFFSUBNUM'] = self.OFFSUBNUM
        self.OFFSUBDEN = RM_Field_MODEM_TIMING_OFFSUBDEN(self)
        self.zz_fdict['OFFSUBDEN'] = self.OFFSUBDEN
        self.TSAGCDEL = RM_Field_MODEM_TIMING_TSAGCDEL(self)
        self.zz_fdict['TSAGCDEL'] = self.TSAGCDEL
        self.FASTRESYNC = RM_Field_MODEM_TIMING_FASTRESYNC(self)
        self.zz_fdict['FASTRESYNC'] = self.FASTRESYNC
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSSS0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSSS0, self).__init__(rmio, label,
            0xa8014000, 0x084,
            'DSSS0', 'MODEM.DSSS0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DSSS0 = RM_Field_MODEM_DSSS0_DSSS0(self)
        self.zz_fdict['DSSS0'] = self.DSSS0
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_MODINDEX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_MODINDEX, self).__init__(rmio, label,
            0xa8014000, 0x088,
            'MODINDEX', 'MODEM.MODINDEX', 'read-write',
            u"",
            0x00000000, 0x003F1FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODINDEXM = RM_Field_MODEM_MODINDEX_MODINDEXM(self)
        self.zz_fdict['MODINDEXM'] = self.MODINDEXM
        self.MODINDEXE = RM_Field_MODEM_MODINDEX_MODINDEXE(self)
        self.zz_fdict['MODINDEXE'] = self.MODINDEXE
        self.FREQGAINE = RM_Field_MODEM_MODINDEX_FREQGAINE(self)
        self.zz_fdict['FREQGAINE'] = self.FREQGAINE
        self.FREQGAINM = RM_Field_MODEM_MODINDEX_FREQGAINM(self)
        self.zz_fdict['FREQGAINM'] = self.FREQGAINM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFC, self).__init__(rmio, label,
            0xa8014000, 0x08C,
            'AFC', 'MODEM.AFC', 'read-write',
            u"",
            0x00000000, 0xFFFFFC00,
            0x00001000, 0x00002000,
            0x00003000)

        self.AFCRXMODE = RM_Field_MODEM_AFC_AFCRXMODE(self)
        self.zz_fdict['AFCRXMODE'] = self.AFCRXMODE
        self.AFCTXMODE = RM_Field_MODEM_AFC_AFCTXMODE(self)
        self.zz_fdict['AFCTXMODE'] = self.AFCTXMODE
        self.AFCRXCLR = RM_Field_MODEM_AFC_AFCRXCLR(self)
        self.zz_fdict['AFCRXCLR'] = self.AFCRXCLR
        self.AFCDEL = RM_Field_MODEM_AFC_AFCDEL(self)
        self.zz_fdict['AFCDEL'] = self.AFCDEL
        self.AFCAVGPER = RM_Field_MODEM_AFC_AFCAVGPER(self)
        self.zz_fdict['AFCAVGPER'] = self.AFCAVGPER
        self.AFCLIMRESET = RM_Field_MODEM_AFC_AFCLIMRESET(self)
        self.zz_fdict['AFCLIMRESET'] = self.AFCLIMRESET
        self.AFCONESHOT = RM_Field_MODEM_AFC_AFCONESHOT(self)
        self.zz_fdict['AFCONESHOT'] = self.AFCONESHOT
        self.AFCENINTCOMP = RM_Field_MODEM_AFC_AFCENINTCOMP(self)
        self.zz_fdict['AFCENINTCOMP'] = self.AFCENINTCOMP
        self.AFCDSAFREQOFFEST = RM_Field_MODEM_AFC_AFCDSAFREQOFFEST(self)
        self.zz_fdict['AFCDSAFREQOFFEST'] = self.AFCDSAFREQOFFEST
        self.AFCDELDET = RM_Field_MODEM_AFC_AFCDELDET(self)
        self.zz_fdict['AFCDELDET'] = self.AFCDELDET
        self.AFCGEAR = RM_Field_MODEM_AFC_AFCGEAR(self)
        self.zz_fdict['AFCGEAR'] = self.AFCGEAR
        self.DISAFCCTE = RM_Field_MODEM_AFC_DISAFCCTE(self)
        self.zz_fdict['DISAFCCTE'] = self.DISAFCCTE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFCADJLIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFCADJLIM, self).__init__(rmio, label,
            0xa8014000, 0x090,
            'AFCADJLIM', 'MODEM.AFCADJLIM', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AFCADJLIM = RM_Field_MODEM_AFCADJLIM_AFCADJLIM(self)
        self.zz_fdict['AFCADJLIM'] = self.AFCADJLIM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SHAPING0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SHAPING0, self).__init__(rmio, label,
            0xa8014000, 0x094,
            'SHAPING0', 'MODEM.SHAPING0', 'read-write',
            u"",
            0x22130A04, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF0 = RM_Field_MODEM_SHAPING0_COEFF0(self)
        self.zz_fdict['COEFF0'] = self.COEFF0
        self.COEFF1 = RM_Field_MODEM_SHAPING0_COEFF1(self)
        self.zz_fdict['COEFF1'] = self.COEFF1
        self.COEFF2 = RM_Field_MODEM_SHAPING0_COEFF2(self)
        self.zz_fdict['COEFF2'] = self.COEFF2
        self.COEFF3 = RM_Field_MODEM_SHAPING0_COEFF3(self)
        self.zz_fdict['COEFF3'] = self.COEFF3
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SHAPING1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SHAPING1, self).__init__(rmio, label,
            0xa8014000, 0x098,
            'SHAPING1', 'MODEM.SHAPING1', 'read-write',
            u"",
            0x4F4A4132, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF4 = RM_Field_MODEM_SHAPING1_COEFF4(self)
        self.zz_fdict['COEFF4'] = self.COEFF4
        self.COEFF5 = RM_Field_MODEM_SHAPING1_COEFF5(self)
        self.zz_fdict['COEFF5'] = self.COEFF5
        self.COEFF6 = RM_Field_MODEM_SHAPING1_COEFF6(self)
        self.zz_fdict['COEFF6'] = self.COEFF6
        self.COEFF7 = RM_Field_MODEM_SHAPING1_COEFF7(self)
        self.zz_fdict['COEFF7'] = self.COEFF7
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SHAPING2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SHAPING2, self).__init__(rmio, label,
            0xa8014000, 0x09C,
            'SHAPING2', 'MODEM.SHAPING2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF8 = RM_Field_MODEM_SHAPING2_COEFF8(self)
        self.zz_fdict['COEFF8'] = self.COEFF8
        self.COEFF9 = RM_Field_MODEM_SHAPING2_COEFF9(self)
        self.zz_fdict['COEFF9'] = self.COEFF9
        self.COEFF10 = RM_Field_MODEM_SHAPING2_COEFF10(self)
        self.zz_fdict['COEFF10'] = self.COEFF10
        self.COEFF11 = RM_Field_MODEM_SHAPING2_COEFF11(self)
        self.zz_fdict['COEFF11'] = self.COEFF11
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SHAPING3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SHAPING3, self).__init__(rmio, label,
            0xa8014000, 0x0A0,
            'SHAPING3', 'MODEM.SHAPING3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COEFF12 = RM_Field_MODEM_SHAPING3_COEFF12(self)
        self.zz_fdict['COEFF12'] = self.COEFF12
        self.COEFF13 = RM_Field_MODEM_SHAPING3_COEFF13(self)
        self.zz_fdict['COEFF13'] = self.COEFF13
        self.COEFF14 = RM_Field_MODEM_SHAPING3_COEFF14(self)
        self.zz_fdict['COEFF14'] = self.COEFF14
        self.COEFF15 = RM_Field_MODEM_SHAPING3_COEFF15(self)
        self.zz_fdict['COEFF15'] = self.COEFF15
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAMPCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAMPCTRL, self).__init__(rmio, label,
            0xa8014000, 0x0D8,
            'RAMPCTRL', 'MODEM.RAMPCTRL', 'read-write',
            u"",
            0x00000555, 0x00FF1FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RAMPRATE0 = RM_Field_MODEM_RAMPCTRL_RAMPRATE0(self)
        self.zz_fdict['RAMPRATE0'] = self.RAMPRATE0
        self.RAMPRATE1 = RM_Field_MODEM_RAMPCTRL_RAMPRATE1(self)
        self.zz_fdict['RAMPRATE1'] = self.RAMPRATE1
        self.RAMPRATE2 = RM_Field_MODEM_RAMPCTRL_RAMPRATE2(self)
        self.zz_fdict['RAMPRATE2'] = self.RAMPRATE2
        self.RAMPDIS = RM_Field_MODEM_RAMPCTRL_RAMPDIS(self)
        self.zz_fdict['RAMPDIS'] = self.RAMPDIS
        self.RAMPVAL = RM_Field_MODEM_RAMPCTRL_RAMPVAL(self)
        self.zz_fdict['RAMPVAL'] = self.RAMPVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAMPLEV(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAMPLEV, self).__init__(rmio, label,
            0xa8014000, 0x0DC,
            'RAMPLEV', 'MODEM.RAMPLEV', 'read-write',
            u"",
            0x009F9F9F, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RAMPLEV0 = RM_Field_MODEM_RAMPLEV_RAMPLEV0(self)
        self.zz_fdict['RAMPLEV0'] = self.RAMPLEV0
        self.RAMPLEV1 = RM_Field_MODEM_RAMPLEV_RAMPLEV1(self)
        self.zz_fdict['RAMPLEV1'] = self.RAMPLEV1
        self.RAMPLEV2 = RM_Field_MODEM_RAMPLEV_RAMPLEV2(self)
        self.zz_fdict['RAMPLEV2'] = self.RAMPLEV2
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANARAMPCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANARAMPCTRL, self).__init__(rmio, label,
            0xa8014000, 0x0E0,
            'ANARAMPCTRL', 'MODEM.ANARAMPCTRL', 'read-write',
            u"",
            0x00000A00, 0x00001E06,
            0x00001000, 0x00002000,
            0x00003000)

        self.RAMPOVREN = RM_Field_MODEM_ANARAMPCTRL_RAMPOVREN(self)
        self.zz_fdict['RAMPOVREN'] = self.RAMPOVREN
        self.RAMPOVRUPD = RM_Field_MODEM_ANARAMPCTRL_RAMPOVRUPD(self)
        self.zz_fdict['RAMPOVRUPD'] = self.RAMPOVRUPD
        self.VMIDCTRL = RM_Field_MODEM_ANARAMPCTRL_VMIDCTRL(self)
        self.zz_fdict['VMIDCTRL'] = self.VMIDCTRL
        self.MUTEDLY = RM_Field_MODEM_ANARAMPCTRL_MUTEDLY(self)
        self.zz_fdict['MUTEDLY'] = self.MUTEDLY
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_INTAFC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_INTAFC, self).__init__(rmio, label,
            0xa8014000, 0x120,
            'INTAFC', 'MODEM.INTAFC', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FOEPREAVG0 = RM_Field_MODEM_INTAFC_FOEPREAVG0(self)
        self.zz_fdict['FOEPREAVG0'] = self.FOEPREAVG0
        self.FOEPREAVG1 = RM_Field_MODEM_INTAFC_FOEPREAVG1(self)
        self.zz_fdict['FOEPREAVG1'] = self.FOEPREAVG1
        self.FOEPREAVG2 = RM_Field_MODEM_INTAFC_FOEPREAVG2(self)
        self.zz_fdict['FOEPREAVG2'] = self.FOEPREAVG2
        self.FOEPREAVG3 = RM_Field_MODEM_INTAFC_FOEPREAVG3(self)
        self.zz_fdict['FOEPREAVG3'] = self.FOEPREAVG3
        self.FOEPREAVG4 = RM_Field_MODEM_INTAFC_FOEPREAVG4(self)
        self.zz_fdict['FOEPREAVG4'] = self.FOEPREAVG4
        self.FOEPREAVG5 = RM_Field_MODEM_INTAFC_FOEPREAVG5(self)
        self.zz_fdict['FOEPREAVG5'] = self.FOEPREAVG5
        self.FOEPREAVG6 = RM_Field_MODEM_INTAFC_FOEPREAVG6(self)
        self.zz_fdict['FOEPREAVG6'] = self.FOEPREAVG6
        self.FOEPREAVG7 = RM_Field_MODEM_INTAFC_FOEPREAVG7(self)
        self.zz_fdict['FOEPREAVG7'] = self.FOEPREAVG7
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSATHD0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSATHD0, self).__init__(rmio, label,
            0xa8014000, 0x124,
            'DSATHD0', 'MODEM.DSATHD0', 'read-write',
            u"",
            0x07830464, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPIKETHD = RM_Field_MODEM_DSATHD0_SPIKETHD(self)
        self.zz_fdict['SPIKETHD'] = self.SPIKETHD
        self.UNMODTHD = RM_Field_MODEM_DSATHD0_UNMODTHD(self)
        self.zz_fdict['UNMODTHD'] = self.UNMODTHD
        self.FDEVMINTHD = RM_Field_MODEM_DSATHD0_FDEVMINTHD(self)
        self.zz_fdict['FDEVMINTHD'] = self.FDEVMINTHD
        self.FDEVMAXTHD = RM_Field_MODEM_DSATHD0_FDEVMAXTHD(self)
        self.zz_fdict['FDEVMAXTHD'] = self.FDEVMAXTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSATHD1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSATHD1, self).__init__(rmio, label,
            0xa8014000, 0x128,
            'DSATHD1', 'MODEM.DSATHD1', 'read-write',
            u"",
            0x3AC81388, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POWABSTHD = RM_Field_MODEM_DSATHD1_POWABSTHD(self)
        self.zz_fdict['POWABSTHD'] = self.POWABSTHD
        self.POWRELTHD = RM_Field_MODEM_DSATHD1_POWRELTHD(self)
        self.zz_fdict['POWRELTHD'] = self.POWRELTHD
        self.DSARSTCNT = RM_Field_MODEM_DSATHD1_DSARSTCNT(self)
        self.zz_fdict['DSARSTCNT'] = self.DSARSTCNT
        self.RSSIJMPTHD = RM_Field_MODEM_DSATHD1_RSSIJMPTHD(self)
        self.zz_fdict['RSSIJMPTHD'] = self.RSSIJMPTHD
        self.FREQLATDLY = RM_Field_MODEM_DSATHD1_FREQLATDLY(self)
        self.zz_fdict['FREQLATDLY'] = self.FREQLATDLY
        self.PWRFLTBYP = RM_Field_MODEM_DSATHD1_PWRFLTBYP(self)
        self.zz_fdict['PWRFLTBYP'] = self.PWRFLTBYP
        self.AMPFLTBYP = RM_Field_MODEM_DSATHD1_AMPFLTBYP(self)
        self.zz_fdict['AMPFLTBYP'] = self.AMPFLTBYP
        self.PWRDETDIS = RM_Field_MODEM_DSATHD1_PWRDETDIS(self)
        self.zz_fdict['PWRDETDIS'] = self.PWRDETDIS
        self.FREQSCALE = RM_Field_MODEM_DSATHD1_FREQSCALE(self)
        self.zz_fdict['FREQSCALE'] = self.FREQSCALE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSATHD2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSATHD2, self).__init__(rmio, label,
            0xa8014000, 0x12C,
            'DSATHD2', 'MODEM.DSATHD2', 'read-write',
            u"",
            0x0C660664, 0x7FFFFEFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POWABSTHDLOG = RM_Field_MODEM_DSATHD2_POWABSTHDLOG(self)
        self.zz_fdict['POWABSTHDLOG'] = self.POWABSTHDLOG
        self.JUMPDETEN = RM_Field_MODEM_DSATHD2_JUMPDETEN(self)
        self.zz_fdict['JUMPDETEN'] = self.JUMPDETEN
        self.FDADJTHD = RM_Field_MODEM_DSATHD2_FDADJTHD(self)
        self.zz_fdict['FDADJTHD'] = self.FDADJTHD
        self.PMDETPASSTHD = RM_Field_MODEM_DSATHD2_PMDETPASSTHD(self)
        self.zz_fdict['PMDETPASSTHD'] = self.PMDETPASSTHD
        self.FREQESTTHD = RM_Field_MODEM_DSATHD2_FREQESTTHD(self)
        self.zz_fdict['FREQESTTHD'] = self.FREQESTTHD
        self.INTERFERDET = RM_Field_MODEM_DSATHD2_INTERFERDET(self)
        self.zz_fdict['INTERFERDET'] = self.INTERFERDET
        self.PMDETFORCE = RM_Field_MODEM_DSATHD2_PMDETFORCE(self)
        self.zz_fdict['PMDETFORCE'] = self.PMDETFORCE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSATHD3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSATHD3, self).__init__(rmio, label,
            0xa8014000, 0x130,
            'DSATHD3', 'MODEM.DSATHD3', 'read-write',
            u"",
            0x07830464, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPIKETHDLO = RM_Field_MODEM_DSATHD3_SPIKETHDLO(self)
        self.zz_fdict['SPIKETHDLO'] = self.SPIKETHDLO
        self.UNMODTHDLO = RM_Field_MODEM_DSATHD3_UNMODTHDLO(self)
        self.zz_fdict['UNMODTHDLO'] = self.UNMODTHDLO
        self.FDEVMINTHDLO = RM_Field_MODEM_DSATHD3_FDEVMINTHDLO(self)
        self.zz_fdict['FDEVMINTHDLO'] = self.FDEVMINTHDLO
        self.FDEVMAXTHDLO = RM_Field_MODEM_DSATHD3_FDEVMAXTHDLO(self)
        self.zz_fdict['FDEVMAXTHDLO'] = self.FDEVMAXTHDLO
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSATHD4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSATHD4, self).__init__(rmio, label,
            0xa8014000, 0x134,
            'DSATHD4', 'MODEM.DSATHD4', 'read-write',
            u"",
            0x00821388, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POWABSTHDLO = RM_Field_MODEM_DSATHD4_POWABSTHDLO(self)
        self.zz_fdict['POWABSTHDLO'] = self.POWABSTHDLO
        self.ARRTOLERTHD0LO = RM_Field_MODEM_DSATHD4_ARRTOLERTHD0LO(self)
        self.zz_fdict['ARRTOLERTHD0LO'] = self.ARRTOLERTHD0LO
        self.ARRTOLERTHD1LO = RM_Field_MODEM_DSATHD4_ARRTOLERTHD1LO(self)
        self.zz_fdict['ARRTOLERTHD1LO'] = self.ARRTOLERTHD1LO
        self.SWTHD = RM_Field_MODEM_DSATHD4_SWTHD(self)
        self.zz_fdict['SWTHD'] = self.SWTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DSACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DSACTRL, self).__init__(rmio, label,
            0xa8014000, 0x138,
            'DSACTRL', 'MODEM.DSACTRL', 'read-write',
            u"",
            0x000A2090, 0xFFEFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DSAMODE = RM_Field_MODEM_DSACTRL_DSAMODE(self)
        self.zz_fdict['DSAMODE'] = self.DSAMODE
        self.ARRTHD = RM_Field_MODEM_DSACTRL_ARRTHD(self)
        self.zz_fdict['ARRTHD'] = self.ARRTHD
        self.ARRTOLERTHD0 = RM_Field_MODEM_DSACTRL_ARRTOLERTHD0(self)
        self.zz_fdict['ARRTOLERTHD0'] = self.ARRTOLERTHD0
        self.ARRTOLERTHD1 = RM_Field_MODEM_DSACTRL_ARRTOLERTHD1(self)
        self.zz_fdict['ARRTOLERTHD1'] = self.ARRTOLERTHD1
        self.SCHPRD = RM_Field_MODEM_DSACTRL_SCHPRD(self)
        self.zz_fdict['SCHPRD'] = self.SCHPRD
        self.FREQAVGSYM = RM_Field_MODEM_DSACTRL_FREQAVGSYM(self)
        self.zz_fdict['FREQAVGSYM'] = self.FREQAVGSYM
        self.TRANRSTDSA = RM_Field_MODEM_DSACTRL_TRANRSTDSA(self)
        self.zz_fdict['TRANRSTDSA'] = self.TRANRSTDSA
        self.DSARSTON = RM_Field_MODEM_DSACTRL_DSARSTON(self)
        self.zz_fdict['DSARSTON'] = self.DSARSTON
        self.GAINREDUCDLY = RM_Field_MODEM_DSACTRL_GAINREDUCDLY(self)
        self.zz_fdict['GAINREDUCDLY'] = self.GAINREDUCDLY
        self.LOWDUTY = RM_Field_MODEM_DSACTRL_LOWDUTY(self)
        self.zz_fdict['LOWDUTY'] = self.LOWDUTY
        self.RESTORE = RM_Field_MODEM_DSACTRL_RESTORE(self)
        self.zz_fdict['RESTORE'] = self.RESTORE
        self.AGCBAUDEN = RM_Field_MODEM_DSACTRL_AGCBAUDEN(self)
        self.zz_fdict['AGCBAUDEN'] = self.AGCBAUDEN
        self.AMPJUPTHD = RM_Field_MODEM_DSACTRL_AMPJUPTHD(self)
        self.zz_fdict['AMPJUPTHD'] = self.AMPJUPTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_VITERBIDEMOD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_VITERBIDEMOD, self).__init__(rmio, label,
            0xa8014000, 0x140,
            'VITERBIDEMOD', 'MODEM.VITERBIDEMOD', 'read-write',
            u"",
            0x00206100, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VTDEMODEN = RM_Field_MODEM_VITERBIDEMOD_VTDEMODEN(self)
        self.zz_fdict['VTDEMODEN'] = self.VTDEMODEN
        self.HARDDECISION = RM_Field_MODEM_VITERBIDEMOD_HARDDECISION(self)
        self.zz_fdict['HARDDECISION'] = self.HARDDECISION
        self.VITERBIKSI1 = RM_Field_MODEM_VITERBIDEMOD_VITERBIKSI1(self)
        self.zz_fdict['VITERBIKSI1'] = self.VITERBIKSI1
        self.VITERBIKSI2 = RM_Field_MODEM_VITERBIDEMOD_VITERBIKSI2(self)
        self.zz_fdict['VITERBIKSI2'] = self.VITERBIKSI2
        self.VITERBIKSI3 = RM_Field_MODEM_VITERBIDEMOD_VITERBIKSI3(self)
        self.zz_fdict['VITERBIKSI3'] = self.VITERBIKSI3
        self.SYNTHAFC = RM_Field_MODEM_VITERBIDEMOD_SYNTHAFC(self)
        self.zz_fdict['SYNTHAFC'] = self.SYNTHAFC
        self.CORRCYCLE = RM_Field_MODEM_VITERBIDEMOD_CORRCYCLE(self)
        self.zz_fdict['CORRCYCLE'] = self.CORRCYCLE
        self.CORRSTPSIZE = RM_Field_MODEM_VITERBIDEMOD_CORRSTPSIZE(self)
        self.zz_fdict['CORRSTPSIZE'] = self.CORRSTPSIZE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_VTCORRCFG0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_VTCORRCFG0, self).__init__(rmio, label,
            0xa8014000, 0x144,
            'VTCORRCFG0', 'MODEM.VTCORRCFG0', 'read-write',
            u"",
            0x123556B7, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.EXPECTPATT = RM_Field_MODEM_VTCORRCFG0_EXPECTPATT(self)
        self.zz_fdict['EXPECTPATT'] = self.EXPECTPATT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_VTCORRCFG1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_VTCORRCFG1, self).__init__(rmio, label,
            0xa8014000, 0x148,
            'VTCORRCFG1', 'MODEM.VTCORRCFG1', 'read-write',
            u"",
            0x5020C000, 0xF7FDFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VITERBIKSI3WB = RM_Field_MODEM_VTCORRCFG1_VITERBIKSI3WB(self)
        self.zz_fdict['VITERBIKSI3WB'] = self.VITERBIKSI3WB
        self.KSI3SWENABLE = RM_Field_MODEM_VTCORRCFG1_KSI3SWENABLE(self)
        self.zz_fdict['KSI3SWENABLE'] = self.KSI3SWENABLE
        self.VTFRQLIM = RM_Field_MODEM_VTCORRCFG1_VTFRQLIM(self)
        self.zz_fdict['VTFRQLIM'] = self.VTFRQLIM
        self.EXPSYNCLEN = RM_Field_MODEM_VTCORRCFG1_EXPSYNCLEN(self)
        self.zz_fdict['EXPSYNCLEN'] = self.EXPSYNCLEN
        self.EXPECTHT = RM_Field_MODEM_VTCORRCFG1_EXPECTHT(self)
        self.zz_fdict['EXPECTHT'] = self.EXPECTHT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_VTTRACK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_VTTRACK, self).__init__(rmio, label,
            0xa8014000, 0x14C,
            'VTTRACK', 'MODEM.VTTRACK', 'read-write',
            u"",
            0x0D803B88, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FREQTRACKMODE = RM_Field_MODEM_VTTRACK_FREQTRACKMODE(self)
        self.zz_fdict['FREQTRACKMODE'] = self.FREQTRACKMODE
        self.TIMTRACKTHD = RM_Field_MODEM_VTTRACK_TIMTRACKTHD(self)
        self.zz_fdict['TIMTRACKTHD'] = self.TIMTRACKTHD
        self.TIMEACQUTHD = RM_Field_MODEM_VTTRACK_TIMEACQUTHD(self)
        self.zz_fdict['TIMEACQUTHD'] = self.TIMEACQUTHD
        self.TIMGEAR = RM_Field_MODEM_VTTRACK_TIMGEAR(self)
        self.zz_fdict['TIMGEAR'] = self.TIMGEAR
        self.FREQBIAS = RM_Field_MODEM_VTTRACK_FREQBIAS(self)
        self.zz_fdict['FREQBIAS'] = self.FREQBIAS
        self.HIPWRTHD = RM_Field_MODEM_VTTRACK_HIPWRTHD(self)
        self.zz_fdict['HIPWRTHD'] = self.HIPWRTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_VTBLETIMING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_VTBLETIMING, self).__init__(rmio, label,
            0xa8014000, 0x150,
            'VTBLETIMING', 'MODEM.VTBLETIMING', 'read-write',
            u"",
            0x00000000, 0x8000FFF7,
            0x00001000, 0x00002000,
            0x00003000)

        self.VTBLETIMINGSEL = RM_Field_MODEM_VTBLETIMING_VTBLETIMINGSEL(self)
        self.zz_fdict['VTBLETIMINGSEL'] = self.VTBLETIMINGSEL
        self.VTBLETIMINGCLKSEL = RM_Field_MODEM_VTBLETIMING_VTBLETIMINGCLKSEL(self)
        self.zz_fdict['VTBLETIMINGCLKSEL'] = self.VTBLETIMINGCLKSEL
        self.TIMINGDELAY = RM_Field_MODEM_VTBLETIMING_TIMINGDELAY(self)
        self.zz_fdict['TIMINGDELAY'] = self.TIMINGDELAY
        self.FLENOFF = RM_Field_MODEM_VTBLETIMING_FLENOFF(self)
        self.zz_fdict['FLENOFF'] = self.FLENOFF
        self.DISDEMODOF = RM_Field_MODEM_VTBLETIMING_DISDEMODOF(self)
        self.zz_fdict['DISDEMODOF'] = self.DISDEMODOF
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_BREST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_BREST, self).__init__(rmio, label,
            0xa8014000, 0x154,
            'BREST', 'MODEM.BREST', 'read-only',
            u"",
            0x00000000, 0x000007FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BRESTINT = RM_Field_MODEM_BREST_BRESTINT(self)
        self.zz_fdict['BRESTINT'] = self.BRESTINT
        self.BRESTNUM = RM_Field_MODEM_BREST_BRESTNUM(self)
        self.zz_fdict['BRESTNUM'] = self.BRESTNUM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AUTOCG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AUTOCG, self).__init__(rmio, label,
            0xa8014000, 0x158,
            'AUTOCG', 'MODEM.AUTOCG', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUTOCGEN = RM_Field_MODEM_AUTOCG_AUTOCGEN(self)
        self.zz_fdict['AUTOCGEN'] = self.AUTOCGEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CGCLKSTOP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CGCLKSTOP, self).__init__(rmio, label,
            0xa8014000, 0x15C,
            'CGCLKSTOP', 'MODEM.CGCLKSTOP', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FORCEOFF = RM_Field_MODEM_CGCLKSTOP_FORCEOFF(self)
        self.zz_fdict['FORCEOFF'] = self.FORCEOFF
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_POE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_POE, self).__init__(rmio, label,
            0xa8014000, 0x160,
            'POE', 'MODEM.POE', 'read-only',
            u"",
            0x00000000, 0x03FF03FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POEI = RM_Field_MODEM_POE_POEI(self)
        self.zz_fdict['POEI'] = self.POEI
        self.POEQ = RM_Field_MODEM_POE_POEQ(self)
        self.zz_fdict['POEQ'] = self.POEQ
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DIRECTMODE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DIRECTMODE, self).__init__(rmio, label,
            0xa8014000, 0x164,
            'DIRECTMODE', 'MODEM.DIRECTMODE', 'read-write',
            u"",
            0x0000010C, 0x00001F0F,
            0x00001000, 0x00002000,
            0x00003000)

        self.DMENABLE = RM_Field_MODEM_DIRECTMODE_DMENABLE(self)
        self.zz_fdict['DMENABLE'] = self.DMENABLE
        self.SYNCASYNC = RM_Field_MODEM_DIRECTMODE_SYNCASYNC(self)
        self.zz_fdict['SYNCASYNC'] = self.SYNCASYNC
        self.SYNCPREAM = RM_Field_MODEM_DIRECTMODE_SYNCPREAM(self)
        self.zz_fdict['SYNCPREAM'] = self.SYNCPREAM
        self.CLKWIDTH = RM_Field_MODEM_DIRECTMODE_CLKWIDTH(self)
        self.zz_fdict['CLKWIDTH'] = self.CLKWIDTH
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE, self).__init__(rmio, label,
            0xa8014000, 0x168,
            'LONGRANGE', 'MODEM.LONGRANGE', 'read-write',
            u"",
            0x00FA53E8, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRCORRTHD = RM_Field_MODEM_LONGRANGE_LRCORRTHD(self)
        self.zz_fdict['LRCORRTHD'] = self.LRCORRTHD
        self.LRCORRSCHWIN = RM_Field_MODEM_LONGRANGE_LRCORRSCHWIN(self)
        self.zz_fdict['LRCORRSCHWIN'] = self.LRCORRSCHWIN
        self.LRBLE = RM_Field_MODEM_LONGRANGE_LRBLE(self)
        self.zz_fdict['LRBLE'] = self.LRBLE
        self.LRTIMCORRTHD = RM_Field_MODEM_LONGRANGE_LRTIMCORRTHD(self)
        self.zz_fdict['LRTIMCORRTHD'] = self.LRTIMCORRTHD
        self.LRBLEDSA = RM_Field_MODEM_LONGRANGE_LRBLEDSA(self)
        self.zz_fdict['LRBLEDSA'] = self.LRBLEDSA
        self.LRDEC = RM_Field_MODEM_LONGRANGE_LRDEC(self)
        self.zz_fdict['LRDEC'] = self.LRDEC
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE1, self).__init__(rmio, label,
            0xa8014000, 0x16C,
            'LONGRANGE1', 'MODEM.LONGRANGE1', 'read-write',
            u"",
            0x40000000, 0xFFFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRSS = RM_Field_MODEM_LONGRANGE1_LRSS(self)
        self.zz_fdict['LRSS'] = self.LRSS
        self.LRTIMEOUTTHD = RM_Field_MODEM_LONGRANGE1_LRTIMEOUTTHD(self)
        self.zz_fdict['LRTIMEOUTTHD'] = self.LRTIMEOUTTHD
        self.CHPWRACCUDEL = RM_Field_MODEM_LONGRANGE1_CHPWRACCUDEL(self)
        self.zz_fdict['CHPWRACCUDEL'] = self.CHPWRACCUDEL
        self.HYSVAL = RM_Field_MODEM_LONGRANGE1_HYSVAL(self)
        self.zz_fdict['HYSVAL'] = self.HYSVAL
        self.AVGWIN = RM_Field_MODEM_LONGRANGE1_AVGWIN(self)
        self.zz_fdict['AVGWIN'] = self.AVGWIN
        self.LRSPIKETHADD = RM_Field_MODEM_LONGRANGE1_LRSPIKETHADD(self)
        self.zz_fdict['LRSPIKETHADD'] = self.LRSPIKETHADD
        self.LOGICBASEDPUGATE = RM_Field_MODEM_LONGRANGE1_LOGICBASEDPUGATE(self)
        self.zz_fdict['LOGICBASEDPUGATE'] = self.LOGICBASEDPUGATE
        self.LOGICBASEDLRDEMODGATE = RM_Field_MODEM_LONGRANGE1_LOGICBASEDLRDEMODGATE(self)
        self.zz_fdict['LOGICBASEDLRDEMODGATE'] = self.LOGICBASEDLRDEMODGATE
        self.PREFILTLEN = RM_Field_MODEM_LONGRANGE1_PREFILTLEN(self)
        self.zz_fdict['PREFILTLEN'] = self.PREFILTLEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE2, self).__init__(rmio, label,
            0xa8014000, 0x170,
            'LONGRANGE2', 'MODEM.LONGRANGE2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRCHPWRTH1 = RM_Field_MODEM_LONGRANGE2_LRCHPWRTH1(self)
        self.zz_fdict['LRCHPWRTH1'] = self.LRCHPWRTH1
        self.LRCHPWRTH2 = RM_Field_MODEM_LONGRANGE2_LRCHPWRTH2(self)
        self.zz_fdict['LRCHPWRTH2'] = self.LRCHPWRTH2
        self.LRCHPWRTH3 = RM_Field_MODEM_LONGRANGE2_LRCHPWRTH3(self)
        self.zz_fdict['LRCHPWRTH3'] = self.LRCHPWRTH3
        self.LRCHPWRTH4 = RM_Field_MODEM_LONGRANGE2_LRCHPWRTH4(self)
        self.zz_fdict['LRCHPWRTH4'] = self.LRCHPWRTH4
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE3, self).__init__(rmio, label,
            0xa8014000, 0x174,
            'LONGRANGE3', 'MODEM.LONGRANGE3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRCHPWRTH5 = RM_Field_MODEM_LONGRANGE3_LRCHPWRTH5(self)
        self.zz_fdict['LRCHPWRTH5'] = self.LRCHPWRTH5
        self.LRCHPWRTH6 = RM_Field_MODEM_LONGRANGE3_LRCHPWRTH6(self)
        self.zz_fdict['LRCHPWRTH6'] = self.LRCHPWRTH6
        self.LRCHPWRTH7 = RM_Field_MODEM_LONGRANGE3_LRCHPWRTH7(self)
        self.zz_fdict['LRCHPWRTH7'] = self.LRCHPWRTH7
        self.LRCHPWRTH8 = RM_Field_MODEM_LONGRANGE3_LRCHPWRTH8(self)
        self.zz_fdict['LRCHPWRTH8'] = self.LRCHPWRTH8
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE4, self).__init__(rmio, label,
            0xa8014000, 0x178,
            'LONGRANGE4', 'MODEM.LONGRANGE4', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRCHPWRTH9 = RM_Field_MODEM_LONGRANGE4_LRCHPWRTH9(self)
        self.zz_fdict['LRCHPWRTH9'] = self.LRCHPWRTH9
        self.LRCHPWRTH10 = RM_Field_MODEM_LONGRANGE4_LRCHPWRTH10(self)
        self.zz_fdict['LRCHPWRTH10'] = self.LRCHPWRTH10
        self.LRCHPWRSH1 = RM_Field_MODEM_LONGRANGE4_LRCHPWRSH1(self)
        self.zz_fdict['LRCHPWRSH1'] = self.LRCHPWRSH1
        self.LRCHPWRSH2 = RM_Field_MODEM_LONGRANGE4_LRCHPWRSH2(self)
        self.zz_fdict['LRCHPWRSH2'] = self.LRCHPWRSH2
        self.LRCHPWRSH3 = RM_Field_MODEM_LONGRANGE4_LRCHPWRSH3(self)
        self.zz_fdict['LRCHPWRSH3'] = self.LRCHPWRSH3
        self.LRCHPWRSH4 = RM_Field_MODEM_LONGRANGE4_LRCHPWRSH4(self)
        self.zz_fdict['LRCHPWRSH4'] = self.LRCHPWRSH4
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE5, self).__init__(rmio, label,
            0xa8014000, 0x17C,
            'LONGRANGE5', 'MODEM.LONGRANGE5', 'read-write',
            u"",
            0x00000000, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRCHPWRSH5 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH5(self)
        self.zz_fdict['LRCHPWRSH5'] = self.LRCHPWRSH5
        self.LRCHPWRSH6 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH6(self)
        self.zz_fdict['LRCHPWRSH6'] = self.LRCHPWRSH6
        self.LRCHPWRSH7 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH7(self)
        self.zz_fdict['LRCHPWRSH7'] = self.LRCHPWRSH7
        self.LRCHPWRSH8 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH8(self)
        self.zz_fdict['LRCHPWRSH8'] = self.LRCHPWRSH8
        self.LRCHPWRSH9 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH9(self)
        self.zz_fdict['LRCHPWRSH9'] = self.LRCHPWRSH9
        self.LRCHPWRSH10 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH10(self)
        self.zz_fdict['LRCHPWRSH10'] = self.LRCHPWRSH10
        self.LRCHPWRSH11 = RM_Field_MODEM_LONGRANGE5_LRCHPWRSH11(self)
        self.zz_fdict['LRCHPWRSH11'] = self.LRCHPWRSH11
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LONGRANGE6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LONGRANGE6, self).__init__(rmio, label,
            0xa8014000, 0x180,
            'LONGRANGE6', 'MODEM.LONGRANGE6', 'read-write',
            u"",
            0x00000000, 0xFFF7FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LRCHPWRSPIKETH = RM_Field_MODEM_LONGRANGE6_LRCHPWRSPIKETH(self)
        self.zz_fdict['LRCHPWRSPIKETH'] = self.LRCHPWRSPIKETH
        self.LRSPIKETHD = RM_Field_MODEM_LONGRANGE6_LRSPIKETHD(self)
        self.zz_fdict['LRSPIKETHD'] = self.LRSPIKETHD
        self.LRCHPWRTH11 = RM_Field_MODEM_LONGRANGE6_LRCHPWRTH11(self)
        self.zz_fdict['LRCHPWRTH11'] = self.LRCHPWRTH11
        self.LRCHPWRSH12 = RM_Field_MODEM_LONGRANGE6_LRCHPWRSH12(self)
        self.zz_fdict['LRCHPWRSH12'] = self.LRCHPWRSH12
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_LRFRC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_LRFRC, self).__init__(rmio, label,
            0xa8014000, 0x184,
            'LRFRC', 'MODEM.LRFRC', 'read-write',
            u"",
            0x00000101, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CI500 = RM_Field_MODEM_LRFRC_CI500(self)
        self.zz_fdict['CI500'] = self.CI500
        self.FRCACKTIMETHD = RM_Field_MODEM_LRFRC_FRCACKTIMETHD(self)
        self.zz_fdict['FRCACKTIMETHD'] = self.FRCACKTIMETHD
        self.LRCORRMODE = RM_Field_MODEM_LRFRC_LRCORRMODE(self)
        self.zz_fdict['LRCORRMODE'] = self.LRCORRMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_COH0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_COH0, self).__init__(rmio, label,
            0xa8014000, 0x188,
            'COH0', 'MODEM.COH0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COHDYNAMICBBSSEN = RM_Field_MODEM_COH0_COHDYNAMICBBSSEN(self)
        self.zz_fdict['COHDYNAMICBBSSEN'] = self.COHDYNAMICBBSSEN
        self.COHDYNAMICSYNCTHRESH = RM_Field_MODEM_COH0_COHDYNAMICSYNCTHRESH(self)
        self.zz_fdict['COHDYNAMICSYNCTHRESH'] = self.COHDYNAMICSYNCTHRESH
        self.COHDYNAMICPRETHRESH = RM_Field_MODEM_COH0_COHDYNAMICPRETHRESH(self)
        self.zz_fdict['COHDYNAMICPRETHRESH'] = self.COHDYNAMICPRETHRESH
        self.COHCHPWRLOCK = RM_Field_MODEM_COH0_COHCHPWRLOCK(self)
        self.zz_fdict['COHCHPWRLOCK'] = self.COHCHPWRLOCK
        self.COHCHPWRRESTART = RM_Field_MODEM_COH0_COHCHPWRRESTART(self)
        self.zz_fdict['COHCHPWRRESTART'] = self.COHCHPWRRESTART
        self.COHDYNAMICPRETHRESHSEL = RM_Field_MODEM_COH0_COHDYNAMICPRETHRESHSEL(self)
        self.zz_fdict['COHDYNAMICPRETHRESHSEL'] = self.COHDYNAMICPRETHRESHSEL
        self.COHCHPWRTH0 = RM_Field_MODEM_COH0_COHCHPWRTH0(self)
        self.zz_fdict['COHCHPWRTH0'] = self.COHCHPWRTH0
        self.COHCHPWRTH1 = RM_Field_MODEM_COH0_COHCHPWRTH1(self)
        self.zz_fdict['COHCHPWRTH1'] = self.COHCHPWRTH1
        self.COHCHPWRTH2 = RM_Field_MODEM_COH0_COHCHPWRTH2(self)
        self.zz_fdict['COHCHPWRTH2'] = self.COHCHPWRTH2
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_COH1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_COH1, self).__init__(rmio, label,
            0xa8014000, 0x18C,
            'COH1', 'MODEM.COH1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNCTHRESH0 = RM_Field_MODEM_COH1_SYNCTHRESH0(self)
        self.zz_fdict['SYNCTHRESH0'] = self.SYNCTHRESH0
        self.SYNCTHRESH1 = RM_Field_MODEM_COH1_SYNCTHRESH1(self)
        self.zz_fdict['SYNCTHRESH1'] = self.SYNCTHRESH1
        self.SYNCTHRESH2 = RM_Field_MODEM_COH1_SYNCTHRESH2(self)
        self.zz_fdict['SYNCTHRESH2'] = self.SYNCTHRESH2
        self.SYNCTHRESH3 = RM_Field_MODEM_COH1_SYNCTHRESH3(self)
        self.zz_fdict['SYNCTHRESH3'] = self.SYNCTHRESH3
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_COH2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_COH2, self).__init__(rmio, label,
            0xa8014000, 0x190,
            'COH2', 'MODEM.COH2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNCTHRESHDELTA0 = RM_Field_MODEM_COH2_SYNCTHRESHDELTA0(self)
        self.zz_fdict['SYNCTHRESHDELTA0'] = self.SYNCTHRESHDELTA0
        self.SYNCTHRESHDELTA1 = RM_Field_MODEM_COH2_SYNCTHRESHDELTA1(self)
        self.zz_fdict['SYNCTHRESHDELTA1'] = self.SYNCTHRESHDELTA1
        self.SYNCTHRESHDELTA2 = RM_Field_MODEM_COH2_SYNCTHRESHDELTA2(self)
        self.zz_fdict['SYNCTHRESHDELTA2'] = self.SYNCTHRESHDELTA2
        self.SYNCTHRESHDELTA3 = RM_Field_MODEM_COH2_SYNCTHRESHDELTA3(self)
        self.zz_fdict['SYNCTHRESHDELTA3'] = self.SYNCTHRESHDELTA3
        self.DSAPEAKCHPWRTH = RM_Field_MODEM_COH2_DSAPEAKCHPWRTH(self)
        self.zz_fdict['DSAPEAKCHPWRTH'] = self.DSAPEAKCHPWRTH
        self.FIXEDCDTHFORIIR = RM_Field_MODEM_COH2_FIXEDCDTHFORIIR(self)
        self.zz_fdict['FIXEDCDTHFORIIR'] = self.FIXEDCDTHFORIIR
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_COH3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_COH3, self).__init__(rmio, label,
            0xa8014000, 0x194,
            'COH3', 'MODEM.COH3', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COHDSAEN = RM_Field_MODEM_COH3_COHDSAEN(self)
        self.zz_fdict['COHDSAEN'] = self.COHDSAEN
        self.COHDSAADDWNDSIZE = RM_Field_MODEM_COH3_COHDSAADDWNDSIZE(self)
        self.zz_fdict['COHDSAADDWNDSIZE'] = self.COHDSAADDWNDSIZE
        self.CDSS = RM_Field_MODEM_COH3_CDSS(self)
        self.zz_fdict['CDSS'] = self.CDSS
        self.DSAPEAKCHKEN = RM_Field_MODEM_COH3_DSAPEAKCHKEN(self)
        self.zz_fdict['DSAPEAKCHKEN'] = self.DSAPEAKCHKEN
        self.DSAPEAKINDLEN = RM_Field_MODEM_COH3_DSAPEAKINDLEN(self)
        self.zz_fdict['DSAPEAKINDLEN'] = self.DSAPEAKINDLEN
        self.DSAPEAKCHPWREN = RM_Field_MODEM_COH3_DSAPEAKCHPWREN(self)
        self.zz_fdict['DSAPEAKCHPWREN'] = self.DSAPEAKCHPWREN
        self.LOGICBASEDCOHDEMODGATE = RM_Field_MODEM_COH3_LOGICBASEDCOHDEMODGATE(self)
        self.zz_fdict['LOGICBASEDCOHDEMODGATE'] = self.LOGICBASEDCOHDEMODGATE
        self.DYNIIRCOEFOPTION = RM_Field_MODEM_COH3_DYNIIRCOEFOPTION(self)
        self.zz_fdict['DYNIIRCOEFOPTION'] = self.DYNIIRCOEFOPTION
        self.ONEPEAKQUALEN = RM_Field_MODEM_COH3_ONEPEAKQUALEN(self)
        self.zz_fdict['ONEPEAKQUALEN'] = self.ONEPEAKQUALEN
        self.PEAKCHKTIMOUT = RM_Field_MODEM_COH3_PEAKCHKTIMOUT(self)
        self.zz_fdict['PEAKCHKTIMOUT'] = self.PEAKCHKTIMOUT
        self.COHDSADETDIS = RM_Field_MODEM_COH3_COHDSADETDIS(self)
        self.zz_fdict['COHDSADETDIS'] = self.COHDSADETDIS
        self.COHDSACMPLX = RM_Field_MODEM_COH3_COHDSACMPLX(self)
        self.zz_fdict['COHDSACMPLX'] = self.COHDSACMPLX
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CMD, self).__init__(rmio, label,
            0xa8014000, 0x198,
            'CMD', 'MODEM.CMD', 'write-only',
            u"",
            0x00000000, 0x0000003B,
            0x00001000, 0x00002000,
            0x00003000)

        self.PRESTOP = RM_Field_MODEM_CMD_PRESTOP(self)
        self.zz_fdict['PRESTOP'] = self.PRESTOP
        self.CHPWRACCUCLR = RM_Field_MODEM_CMD_CHPWRACCUCLR(self)
        self.zz_fdict['CHPWRACCUCLR'] = self.CHPWRACCUCLR
        self.AFCTXLOCK = RM_Field_MODEM_CMD_AFCTXLOCK(self)
        self.zz_fdict['AFCTXLOCK'] = self.AFCTXLOCK
        self.AFCTXCLEAR = RM_Field_MODEM_CMD_AFCTXCLEAR(self)
        self.zz_fdict['AFCTXCLEAR'] = self.AFCTXCLEAR
        self.AFCRXCLEAR = RM_Field_MODEM_CMD_AFCRXCLEAR(self)
        self.zz_fdict['AFCRXCLEAR'] = self.AFCRXCLEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SYNCPROPERTIES(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SYNCPROPERTIES, self).__init__(rmio, label,
            0xa8014000, 0x1A4,
            'SYNCPROPERTIES', 'MODEM.SYNCPROPERTIES', 'read-write',
            u"",
            0x00000000, 0x0001FF00,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATICSYNCTHRESHEN = RM_Field_MODEM_SYNCPROPERTIES_STATICSYNCTHRESHEN(self)
        self.zz_fdict['STATICSYNCTHRESHEN'] = self.STATICSYNCTHRESHEN
        self.STATICSYNCTHRESH = RM_Field_MODEM_SYNCPROPERTIES_STATICSYNCTHRESH(self)
        self.zz_fdict['STATICSYNCTHRESH'] = self.STATICSYNCTHRESH
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PRSCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PRSCTRL, self).__init__(rmio, label,
            0xa8014000, 0x1AC,
            'PRSCTRL', 'MODEM.PRSCTRL', 'read-write',
            u"",
            0x00000000, 0x000FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POSTPONESEL = RM_Field_MODEM_PRSCTRL_POSTPONESEL(self)
        self.zz_fdict['POSTPONESEL'] = self.POSTPONESEL
        self.ADVANCESEL = RM_Field_MODEM_PRSCTRL_ADVANCESEL(self)
        self.zz_fdict['ADVANCESEL'] = self.ADVANCESEL
        self.NEWWNDSEL = RM_Field_MODEM_PRSCTRL_NEWWNDSEL(self)
        self.zz_fdict['NEWWNDSEL'] = self.NEWWNDSEL
        self.WEAKSEL = RM_Field_MODEM_PRSCTRL_WEAKSEL(self)
        self.zz_fdict['WEAKSEL'] = self.WEAKSEL
        self.SYNCSENTSEL = RM_Field_MODEM_PRSCTRL_SYNCSENTSEL(self)
        self.zz_fdict['SYNCSENTSEL'] = self.SYNCSENTSEL
        self.PRESENTSEL = RM_Field_MODEM_PRSCTRL_PRESENTSEL(self)
        self.zz_fdict['PRESENTSEL'] = self.PRESENTSEL
        self.LOWCORRSEL = RM_Field_MODEM_PRSCTRL_LOWCORRSEL(self)
        self.zz_fdict['LOWCORRSEL'] = self.LOWCORRSEL
        self.ANT0SEL = RM_Field_MODEM_PRSCTRL_ANT0SEL(self)
        self.zz_fdict['ANT0SEL'] = self.ANT0SEL
        self.ANT1SEL = RM_Field_MODEM_PRSCTRL_ANT1SEL(self)
        self.zz_fdict['ANT1SEL'] = self.ANT1SEL
        self.IFADCCLKSEL = RM_Field_MODEM_PRSCTRL_IFADCCLKSEL(self)
        self.zz_fdict['IFADCCLKSEL'] = self.IFADCCLKSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PADEBUG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PADEBUG, self).__init__(rmio, label,
            0xa8014000, 0x1B0,
            'PADEBUG', 'MODEM.PADEBUG', 'read-write',
            u"",
            0x00000000, 0x00001E00,
            0x00001000, 0x00002000,
            0x00003000)

        self.MANPACLKAMPCTRL = RM_Field_MODEM_PADEBUG_MANPACLKAMPCTRL(self)
        self.zz_fdict['MANPACLKAMPCTRL'] = self.MANPACLKAMPCTRL
        self.ENMANPACLKAMPCTRL = RM_Field_MODEM_PADEBUG_ENMANPACLKAMPCTRL(self)
        self.zz_fdict['ENMANPACLKAMPCTRL'] = self.ENMANPACLKAMPCTRL
        self.ENMANPAPOWER = RM_Field_MODEM_PADEBUG_ENMANPAPOWER(self)
        self.zz_fdict['ENMANPAPOWER'] = self.ENMANPAPOWER
        self.ENMANPASELSLICE = RM_Field_MODEM_PADEBUG_ENMANPASELSLICE(self)
        self.zz_fdict['ENMANPASELSLICE'] = self.ENMANPASELSLICE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_REALTIMCFE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_REALTIMCFE, self).__init__(rmio, label,
            0xa8014000, 0x1B4,
            'REALTIMCFE', 'MODEM.REALTIMCFE', 'read-write',
            u"",
            0x001F81F4, 0xE03FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MINCOSTTHD = RM_Field_MODEM_REALTIMCFE_MINCOSTTHD(self)
        self.zz_fdict['MINCOSTTHD'] = self.MINCOSTTHD
        self.RTSCHWIN = RM_Field_MODEM_REALTIMCFE_RTSCHWIN(self)
        self.zz_fdict['RTSCHWIN'] = self.RTSCHWIN
        self.RTSCHMODE = RM_Field_MODEM_REALTIMCFE_RTSCHMODE(self)
        self.zz_fdict['RTSCHMODE'] = self.RTSCHMODE
        self.TRACKINGWIN = RM_Field_MODEM_REALTIMCFE_TRACKINGWIN(self)
        self.zz_fdict['TRACKINGWIN'] = self.TRACKINGWIN
        self.SYNCACQWIN = RM_Field_MODEM_REALTIMCFE_SYNCACQWIN(self)
        self.zz_fdict['SYNCACQWIN'] = self.SYNCACQWIN
        self.EXTENSCHBYP = RM_Field_MODEM_REALTIMCFE_EXTENSCHBYP(self)
        self.zz_fdict['EXTENSCHBYP'] = self.EXTENSCHBYP
        self.SINEWEN = RM_Field_MODEM_REALTIMCFE_SINEWEN(self)
        self.zz_fdict['SINEWEN'] = self.SINEWEN
        self.VTAFCFRAME = RM_Field_MODEM_REALTIMCFE_VTAFCFRAME(self)
        self.zz_fdict['VTAFCFRAME'] = self.VTAFCFRAME
        self.RTCFEEN = RM_Field_MODEM_REALTIMCFE_RTCFEEN(self)
        self.zz_fdict['RTCFEEN'] = self.RTCFEEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ETSCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ETSCTRL, self).__init__(rmio, label,
            0xa8014000, 0x1B8,
            'ETSCTRL', 'MODEM.ETSCTRL', 'read-write',
            u"",
            0x00000000, 0x3FFFF7FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ETSLOC = RM_Field_MODEM_ETSCTRL_ETSLOC(self)
        self.zz_fdict['ETSLOC'] = self.ETSLOC
        self.CAPSIGONPRS = RM_Field_MODEM_ETSCTRL_CAPSIGONPRS(self)
        self.zz_fdict['CAPSIGONPRS'] = self.CAPSIGONPRS
        self.CAPTRIG = RM_Field_MODEM_ETSCTRL_CAPTRIG(self)
        self.zz_fdict['CAPTRIG'] = self.CAPTRIG
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ETSTIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ETSTIM, self).__init__(rmio, label,
            0xa8014000, 0x1BC,
            'ETSTIM', 'MODEM.ETSTIM', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ETSTIMVAL = RM_Field_MODEM_ETSTIM_ETSTIMVAL(self)
        self.zz_fdict['ETSTIMVAL'] = self.ETSTIMVAL
        self.ETSCOUNTEREN = RM_Field_MODEM_ETSTIM_ETSCOUNTEREN(self)
        self.zz_fdict['ETSCOUNTEREN'] = self.ETSCOUNTEREN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANTSWCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANTSWCTRL, self).__init__(rmio, label,
            0xa8014000, 0x1C0,
            'ANTSWCTRL', 'MODEM.ANTSWCTRL', 'read-write',
            u"",
            0x003C0000, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ANTDFLTSEL = RM_Field_MODEM_ANTSWCTRL_ANTDFLTSEL(self)
        self.zz_fdict['ANTDFLTSEL'] = self.ANTDFLTSEL
        self.ANTCOUNT = RM_Field_MODEM_ANTSWCTRL_ANTCOUNT(self)
        self.zz_fdict['ANTCOUNT'] = self.ANTCOUNT
        self.ANTSWTYPE = RM_Field_MODEM_ANTSWCTRL_ANTSWTYPE(self)
        self.zz_fdict['ANTSWTYPE'] = self.ANTSWTYPE
        self.ANTSWRST = RM_Field_MODEM_ANTSWCTRL_ANTSWRST(self)
        self.zz_fdict['ANTSWRST'] = self.ANTSWRST
        self.CFGANTPATTEN = RM_Field_MODEM_ANTSWCTRL_CFGANTPATTEN(self)
        self.zz_fdict['CFGANTPATTEN'] = self.CFGANTPATTEN
        self.ANTSWENABLE = RM_Field_MODEM_ANTSWCTRL_ANTSWENABLE(self)
        self.zz_fdict['ANTSWENABLE'] = self.ANTSWENABLE
        self.EXTDSTOPPULSECNT = RM_Field_MODEM_ANTSWCTRL_EXTDSTOPPULSECNT(self)
        self.zz_fdict['EXTDSTOPPULSECNT'] = self.EXTDSTOPPULSECNT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANTSWCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANTSWCTRL1, self).__init__(rmio, label,
            0xa8014000, 0x1C4,
            'ANTSWCTRL1', 'MODEM.ANTSWCTRL1', 'read-write',
            u"",
            0x0006AAAA, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TIMEPERIOD = RM_Field_MODEM_ANTSWCTRL1_TIMEPERIOD(self)
        self.zz_fdict['TIMEPERIOD'] = self.TIMEPERIOD
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANTSWSTART(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANTSWSTART, self).__init__(rmio, label,
            0xa8014000, 0x1C8,
            'ANTSWSTART', 'MODEM.ANTSWSTART', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ANTSWSTARTTIM = RM_Field_MODEM_ANTSWSTART_ANTSWSTARTTIM(self)
        self.zz_fdict['ANTSWSTARTTIM'] = self.ANTSWSTARTTIM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANTSWEND(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANTSWEND, self).__init__(rmio, label,
            0xa8014000, 0x1CC,
            'ANTSWEND', 'MODEM.ANTSWEND', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ANTSWENDTIM = RM_Field_MODEM_ANTSWEND_ANTSWENDTIM(self)
        self.zz_fdict['ANTSWENDTIM'] = self.ANTSWENDTIM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TRECPMPATT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TRECPMPATT, self).__init__(rmio, label,
            0xa8014000, 0x1D0,
            'TRECPMPATT', 'MODEM.TRECPMPATT', 'read-write',
            u"",
            0x55555555, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PMEXPECTPATT = RM_Field_MODEM_TRECPMPATT_PMEXPECTPATT(self)
        self.zz_fdict['PMEXPECTPATT'] = self.PMEXPECTPATT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TRECPMDET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TRECPMDET, self).__init__(rmio, label,
            0xa8014000, 0x1D4,
            'TRECPMDET', 'MODEM.TRECPMDET', 'read-write',
            u"",
            0x00000017, 0xBEFFC3FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PMACQUINGWIN = RM_Field_MODEM_TRECPMDET_PMACQUINGWIN(self)
        self.zz_fdict['PMACQUINGWIN'] = self.PMACQUINGWIN
        self.PMCOSTVALTHD = RM_Field_MODEM_TRECPMDET_PMCOSTVALTHD(self)
        self.zz_fdict['PMCOSTVALTHD'] = self.PMCOSTVALTHD
        self.PMTIMEOUTSEL = RM_Field_MODEM_TRECPMDET_PMTIMEOUTSEL(self)
        self.zz_fdict['PMTIMEOUTSEL'] = self.PMTIMEOUTSEL
        self.PHSCALE = RM_Field_MODEM_TRECPMDET_PHSCALE(self)
        self.zz_fdict['PHSCALE'] = self.PHSCALE
        self.PMMINCOSTTHD = RM_Field_MODEM_TRECPMDET_PMMINCOSTTHD(self)
        self.zz_fdict['PMMINCOSTTHD'] = self.PMMINCOSTTHD
        self.COSTHYST = RM_Field_MODEM_TRECPMDET_COSTHYST(self)
        self.zz_fdict['COSTHYST'] = self.COSTHYST
        self.PREAMSCH = RM_Field_MODEM_TRECPMDET_PREAMSCH(self)
        self.zz_fdict['PREAMSCH'] = self.PREAMSCH
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TRECSCFG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TRECSCFG, self).__init__(rmio, label,
            0xa8014000, 0x1D8,
            'TRECSCFG', 'MODEM.TRECSCFG', 'read-write',
            u"",
            0x00020004, 0x01FF5FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TRECSOSR = RM_Field_MODEM_TRECSCFG_TRECSOSR(self)
        self.zz_fdict['TRECSOSR'] = self.TRECSOSR
        self.DTIMLOSSTHD = RM_Field_MODEM_TRECSCFG_DTIMLOSSTHD(self)
        self.zz_fdict['DTIMLOSSTHD'] = self.DTIMLOSSTHD
        self.DTIMLOSSEN = RM_Field_MODEM_TRECSCFG_DTIMLOSSEN(self)
        self.zz_fdict['DTIMLOSSEN'] = self.DTIMLOSSEN
        self.PMOFFSET = RM_Field_MODEM_TRECSCFG_PMOFFSET(self)
        self.zz_fdict['PMOFFSET'] = self.PMOFFSET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CFGANTPATT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CFGANTPATT, self).__init__(rmio, label,
            0xa8014000, 0x1DC,
            'CFGANTPATT', 'MODEM.CFGANTPATT', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CFGANTPATTVAL = RM_Field_MODEM_CFGANTPATT_CFGANTPATTVAL(self)
        self.zz_fdict['CFGANTPATTVAL'] = self.CFGANTPATTVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_COCURRMODE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_COCURRMODE, self).__init__(rmio, label,
            0xa8014000, 0x1E0,
            'COCURRMODE', 'MODEM.COCURRMODE', 'read-write',
            u"",
            0x00000000, 0x80000000,
            0x00001000, 0x00002000,
            0x00003000)

        self.CONCURRENT = RM_Field_MODEM_COCURRMODE_CONCURRENT(self)
        self.zz_fdict['CONCURRENT'] = self.CONCURRENT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_FRMSCHTIME(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_FRMSCHTIME, self).__init__(rmio, label,
            0xa8014000, 0x224,
            'FRMSCHTIME', 'MODEM.FRMSCHTIME', 'read-write',
            u"",
            0x00000040, 0xE000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FRMSCHTIME = RM_Field_MODEM_FRMSCHTIME_FRMSCHTIME(self)
        self.zz_fdict['FRMSCHTIME'] = self.FRMSCHTIME
        self.PMRSTSYCNEN = RM_Field_MODEM_FRMSCHTIME_PMRSTSYCNEN(self)
        self.zz_fdict['PMRSTSYCNEN'] = self.PMRSTSYCNEN
        self.DSARSTSYCNEN = RM_Field_MODEM_FRMSCHTIME_DSARSTSYCNEN(self)
        self.zz_fdict['DSARSTSYCNEN'] = self.DSARSTSYCNEN
        self.PMENDSCHEN = RM_Field_MODEM_FRMSCHTIME_PMENDSCHEN(self)
        self.zz_fdict['PMENDSCHEN'] = self.PMENDSCHEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PREFILTCOEFF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PREFILTCOEFF, self).__init__(rmio, label,
            0xa8014000, 0x228,
            'PREFILTCOEFF', 'MODEM.PREFILTCOEFF', 'read-write',
            u"",
            0x3B3B3B3B, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREFILTCOEFF = RM_Field_MODEM_PREFILTCOEFF_PREFILTCOEFF(self)
        self.zz_fdict['PREFILTCOEFF'] = self.PREFILTCOEFF
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RXRESTART(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RXRESTART, self).__init__(rmio, label,
            0xa8014000, 0x22C,
            'RXRESTART', 'MODEM.RXRESTART', 'read-write',
            u"",
            0x00001860, 0x00011FF1,
            0x00001000, 0x00002000,
            0x00003000)

        self.RXRESTARTUPONMARSSI = RM_Field_MODEM_RXRESTART_RXRESTARTUPONMARSSI(self)
        self.zz_fdict['RXRESTARTUPONMARSSI'] = self.RXRESTARTUPONMARSSI
        self.RXRESTARTMATHRESHOLD = RM_Field_MODEM_RXRESTART_RXRESTARTMATHRESHOLD(self)
        self.zz_fdict['RXRESTARTMATHRESHOLD'] = self.RXRESTARTMATHRESHOLD
        self.RXRESTARTMALATCHSEL = RM_Field_MODEM_RXRESTART_RXRESTARTMALATCHSEL(self)
        self.zz_fdict['RXRESTARTMALATCHSEL'] = self.RXRESTARTMALATCHSEL
        self.RXRESTARTMACOMPENSEL = RM_Field_MODEM_RXRESTART_RXRESTARTMACOMPENSEL(self)
        self.zz_fdict['RXRESTARTMACOMPENSEL'] = self.RXRESTARTMACOMPENSEL
        self.RXRESTARTMATAP = RM_Field_MODEM_RXRESTART_RXRESTARTMATAP(self)
        self.zz_fdict['RXRESTARTMATAP'] = self.RXRESTARTMATAP
        self.RXRESTARTB4PREDET = RM_Field_MODEM_RXRESTART_RXRESTARTB4PREDET(self)
        self.zz_fdict['RXRESTARTB4PREDET'] = self.RXRESTARTB4PREDET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SQ, self).__init__(rmio, label,
            0xa8014000, 0x230,
            'SQ', 'MODEM.SQ', 'read-write',
            u"",
            0x00000000, 0xFFFF0003,
            0x00001000, 0x00002000,
            0x00003000)

        self.SQEN = RM_Field_MODEM_SQ_SQEN(self)
        self.zz_fdict['SQEN'] = self.SQEN
        self.SQSWRST = RM_Field_MODEM_SQ_SQSWRST(self)
        self.zz_fdict['SQSWRST'] = self.SQSWRST
        self.SQTIMOUT = RM_Field_MODEM_SQ_SQTIMOUT(self)
        self.zz_fdict['SQTIMOUT'] = self.SQTIMOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SQEXT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SQEXT, self).__init__(rmio, label,
            0xa8014000, 0x234,
            'SQEXT', 'MODEM.SQEXT', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SQSTG2TIMOUT = RM_Field_MODEM_SQEXT_SQSTG2TIMOUT(self)
        self.zz_fdict['SQSTG2TIMOUT'] = self.SQSTG2TIMOUT
        self.SQSTG3TIMOUT = RM_Field_MODEM_SQEXT_SQSTG3TIMOUT(self)
        self.zz_fdict['SQSTG3TIMOUT'] = self.SQSTG3TIMOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SQI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SQI, self).__init__(rmio, label,
            0xa8014000, 0x238,
            'SQI', 'MODEM.SQI', 'read-write',
            u"",
            0x00000000, 0x00FF0001,
            0x00001000, 0x00002000,
            0x00003000)

        self.SQISELECT = RM_Field_MODEM_SQI_SQISELECT(self)
        self.zz_fdict['SQISELECT'] = self.SQISELECT
        self.CHIPERROR = RM_Field_MODEM_SQI_CHIPERROR(self)
        self.zz_fdict['CHIPERROR'] = self.CHIPERROR
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANTDIVCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANTDIVCTRL, self).__init__(rmio, label,
            0xa8014000, 0x23C,
            'ANTDIVCTRL', 'MODEM.ANTDIVCTRL', 'read-write',
            u"",
            0x00000000, 0x000007FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ADPRETHRESH = RM_Field_MODEM_ANTDIVCTRL_ADPRETHRESH(self)
        self.zz_fdict['ADPRETHRESH'] = self.ADPRETHRESH
        self.ENADPRETHRESH = RM_Field_MODEM_ANTDIVCTRL_ENADPRETHRESH(self)
        self.zz_fdict['ENADPRETHRESH'] = self.ENADPRETHRESH
        self.ANTDIVDISCCA = RM_Field_MODEM_ANTDIVCTRL_ANTDIVDISCCA(self)
        self.zz_fdict['ANTDIVDISCCA'] = self.ANTDIVDISCCA
        self.ANTDIVSELCCA = RM_Field_MODEM_ANTDIVCTRL_ANTDIVSELCCA(self)
        self.zz_fdict['ANTDIVSELCCA'] = self.ANTDIVSELCCA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ANTDIVFW(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ANTDIVFW, self).__init__(rmio, label,
            0xa8014000, 0x240,
            'ANTDIVFW', 'MODEM.ANTDIVFW', 'read-write',
            u"",
            0x00000000, 0x80000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.FWSELANT = RM_Field_MODEM_ANTDIVFW_FWSELANT(self)
        self.zz_fdict['FWSELANT'] = self.FWSELANT
        self.FWANTSWCMD = RM_Field_MODEM_ANTDIVFW_FWANTSWCMD(self)
        self.zz_fdict['FWANTSWCMD'] = self.FWANTSWCMD
        self.FWANTDIVEN = RM_Field_MODEM_ANTDIVFW_FWANTDIVEN(self)
        self.zz_fdict['FWANTDIVEN'] = self.FWANTDIVEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PHDMODANTDIV(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PHDMODANTDIV, self).__init__(rmio, label,
            0xa8014000, 0x244,
            'PHDMODANTDIV', 'MODEM.PHDMODANTDIV', 'read-write',
            u"",
            0x0000000F, 0x40FF1FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ANTWAIT = RM_Field_MODEM_PHDMODANTDIV_ANTWAIT(self)
        self.zz_fdict['ANTWAIT'] = self.ANTWAIT
        self.SKIPRSSITHD = RM_Field_MODEM_PHDMODANTDIV_SKIPRSSITHD(self)
        self.zz_fdict['SKIPRSSITHD'] = self.SKIPRSSITHD
        self.SKIPCORRTHD = RM_Field_MODEM_PHDMODANTDIV_SKIPCORRTHD(self)
        self.zz_fdict['SKIPCORRTHD'] = self.SKIPCORRTHD
        self.SKIP2ANT = RM_Field_MODEM_PHDMODANTDIV_SKIP2ANT(self)
        self.zz_fdict['SKIP2ANT'] = self.SKIP2ANT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PHANTDECSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PHANTDECSION, self).__init__(rmio, label,
            0xa8014000, 0x248,
            'PHANTDECSION', 'MODEM.PHANTDECSION', 'read-write',
            u"",
            0x00000000, 0xF007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CORRANDDIVTHD = RM_Field_MODEM_PHANTDECSION_CORRANDDIVTHD(self)
        self.zz_fdict['CORRANDDIVTHD'] = self.CORRANDDIVTHD
        self.RSSIANDDIVTHD = RM_Field_MODEM_PHANTDECSION_RSSIANDDIVTHD(self)
        self.zz_fdict['RSSIANDDIVTHD'] = self.RSSIANDDIVTHD
        self.RSSICORR0 = RM_Field_MODEM_PHANTDECSION_RSSICORR0(self)
        self.zz_fdict['RSSICORR0'] = self.RSSICORR0
        self.RSSICORR1 = RM_Field_MODEM_PHANTDECSION_RSSICORR1(self)
        self.zz_fdict['RSSICORR1'] = self.RSSICORR1
        self.RSSICORR2 = RM_Field_MODEM_PHANTDECSION_RSSICORR2(self)
        self.zz_fdict['RSSICORR2'] = self.RSSICORR2
        self.RSSICORR3 = RM_Field_MODEM_PHANTDECSION_RSSICORR3(self)
        self.zz_fdict['RSSICORR3'] = self.RSSICORR3
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PHDMODCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PHDMODCTRL, self).__init__(rmio, label,
            0xa8014000, 0x24C,
            'PHDMODCTRL', 'MODEM.PHDMODCTRL', 'read-write',
            u"",
            0x01DF0004, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PMDETTHD = RM_Field_MODEM_PHDMODCTRL_PMDETTHD(self)
        self.zz_fdict['PMDETTHD'] = self.PMDETTHD
        self.PMTIMLOSTHD = RM_Field_MODEM_PHDMODCTRL_PMTIMLOSTHD(self)
        self.zz_fdict['PMTIMLOSTHD'] = self.PMTIMLOSTHD
        self.PMTIMLOSEN = RM_Field_MODEM_PHDMODCTRL_PMTIMLOSEN(self)
        self.zz_fdict['PMTIMLOSEN'] = self.PMTIMLOSEN
        self.RSSIFLTBYP = RM_Field_MODEM_PHDMODCTRL_RSSIFLTBYP(self)
        self.zz_fdict['RSSIFLTBYP'] = self.RSSIFLTBYP
        self.PMDETEN = RM_Field_MODEM_PHDMODCTRL_PMDETEN(self)
        self.zz_fdict['PMDETEN'] = self.PMDETEN
        self.REMODOSR = RM_Field_MODEM_PHDMODCTRL_REMODOSR(self)
        self.zz_fdict['REMODOSR'] = self.REMODOSR
        self.REMODDWN = RM_Field_MODEM_PHDMODCTRL_REMODDWN(self)
        self.zz_fdict['REMODDWN'] = self.REMODDWN
        self.REMODOUTSEL = RM_Field_MODEM_PHDMODCTRL_REMODOUTSEL(self)
        self.zz_fdict['REMODOUTSEL'] = self.REMODOUTSEL
        self.REMODEN = RM_Field_MODEM_PHDMODCTRL_REMODEN(self)
        self.zz_fdict['REMODEN'] = self.REMODEN
        self.BCRDETECTOR = RM_Field_MODEM_PHDMODCTRL_BCRDETECTOR(self)
        self.zz_fdict['BCRDETECTOR'] = self.BCRDETECTOR
        self.BCRTRECSCONC = RM_Field_MODEM_PHDMODCTRL_BCRTRECSCONC(self)
        self.zz_fdict['BCRTRECSCONC'] = self.BCRTRECSCONC
        self.BCRLEGACYCONC = RM_Field_MODEM_PHDMODCTRL_BCRLEGACYCONC(self)
        self.zz_fdict['BCRLEGACYCONC'] = self.BCRLEGACYCONC
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SICTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SICTRL0, self).__init__(rmio, label,
            0xa8014000, 0x250,
            'SICTRL0', 'MODEM.SICTRL0', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODE = RM_Field_MODEM_SICTRL0_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.NOISETHRESH = RM_Field_MODEM_SICTRL0_NOISETHRESH(self)
        self.zz_fdict['NOISETHRESH'] = self.NOISETHRESH
        self.PEAKNUMTHRESHLW = RM_Field_MODEM_SICTRL0_PEAKNUMTHRESHLW(self)
        self.zz_fdict['PEAKNUMTHRESHLW'] = self.PEAKNUMTHRESHLW
        self.PEAKNUMTHRESHSW = RM_Field_MODEM_SICTRL0_PEAKNUMTHRESHSW(self)
        self.zz_fdict['PEAKNUMTHRESHSW'] = self.PEAKNUMTHRESHSW
        self.SMALLSAMPLETHRESH = RM_Field_MODEM_SICTRL0_SMALLSAMPLETHRESH(self)
        self.zz_fdict['SMALLSAMPLETHRESH'] = self.SMALLSAMPLETHRESH
        self.FREQNOMINAL = RM_Field_MODEM_SICTRL0_FREQNOMINAL(self)
        self.zz_fdict['FREQNOMINAL'] = self.FREQNOMINAL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SICTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SICTRL1, self).__init__(rmio, label,
            0xa8014000, 0x254,
            'SICTRL1', 'MODEM.SICTRL1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SUPERCHIPTOLERANCE = RM_Field_MODEM_SICTRL1_SUPERCHIPTOLERANCE(self)
        self.zz_fdict['SUPERCHIPTOLERANCE'] = self.SUPERCHIPTOLERANCE
        self.SUPERCHIPMEDIAN = RM_Field_MODEM_SICTRL1_SUPERCHIPMEDIAN(self)
        self.zz_fdict['SUPERCHIPMEDIAN'] = self.SUPERCHIPMEDIAN
        self.CORRTHRESH = RM_Field_MODEM_SICTRL1_CORRTHRESH(self)
        self.zz_fdict['CORRTHRESH'] = self.CORRTHRESH
        self.CORRNUM = RM_Field_MODEM_SICTRL1_CORRNUM(self)
        self.zz_fdict['CORRNUM'] = self.CORRNUM
        self.FASTMODE = RM_Field_MODEM_SICTRL1_FASTMODE(self)
        self.zz_fdict['FASTMODE'] = self.FASTMODE
        self.NARROWPULSETHRESH = RM_Field_MODEM_SICTRL1_NARROWPULSETHRESH(self)
        self.zz_fdict['NARROWPULSETHRESH'] = self.NARROWPULSETHRESH
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SISTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SISTATUS, self).__init__(rmio, label,
            0xa8014000, 0x258,
            'SISTATUS', 'MODEM.SISTATUS', 'read-only',
            u"",
            0x00000000, 0x67FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SISTATE = RM_Field_MODEM_SISTATUS_SISTATE(self)
        self.zz_fdict['SISTATE'] = self.SISTATE
        self.NOISE = RM_Field_MODEM_SISTATUS_NOISE(self)
        self.zz_fdict['NOISE'] = self.NOISE
        self.LWPEAKCOUNT = RM_Field_MODEM_SISTATUS_LWPEAKCOUNT(self)
        self.zz_fdict['LWPEAKCOUNT'] = self.LWPEAKCOUNT
        self.NARROWCOUNT = RM_Field_MODEM_SISTATUS_NARROWCOUNT(self)
        self.zz_fdict['NARROWCOUNT'] = self.NARROWCOUNT
        self.TIMELOCK = RM_Field_MODEM_SISTATUS_TIMELOCK(self)
        self.zz_fdict['TIMELOCK'] = self.TIMELOCK
        self.SUPERCHIPFAIL = RM_Field_MODEM_SISTATUS_SUPERCHIPFAIL(self)
        self.zz_fdict['SUPERCHIPFAIL'] = self.SUPERCHIPFAIL
        self.SUPERCHIPPASS = RM_Field_MODEM_SISTATUS_SUPERCHIPPASS(self)
        self.zz_fdict['SUPERCHIPPASS'] = self.SUPERCHIPPASS
        self.TIMEOFFSET = RM_Field_MODEM_SISTATUS_TIMEOFFSET(self)
        self.zz_fdict['TIMEOFFSET'] = self.TIMEOFFSET
        self.CORRPASSNUM = RM_Field_MODEM_SISTATUS_CORRPASSNUM(self)
        self.zz_fdict['CORRPASSNUM'] = self.CORRPASSNUM
        self.SNIFFDONE = RM_Field_MODEM_SISTATUS_SNIFFDONE(self)
        self.zz_fdict['SNIFFDONE'] = self.SNIFFDONE
        self.SIDET = RM_Field_MODEM_SISTATUS_SIDET(self)
        self.zz_fdict['SIDET'] = self.SIDET
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CFGANTPATTEXT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CFGANTPATTEXT, self).__init__(rmio, label,
            0xa8014000, 0x25C,
            'CFGANTPATTEXT', 'MODEM.CFGANTPATTEXT', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CFGANTPATTVALEXT = RM_Field_MODEM_CFGANTPATTEXT_CFGANTPATTVALEXT(self)
        self.zz_fdict['CFGANTPATTVALEXT'] = self.CFGANTPATTVALEXT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SICTRL2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SICTRL2, self).__init__(rmio, label,
            0xa8014000, 0x264,
            'SICTRL2', 'MODEM.SICTRL2', 'read-write',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SIRSTAGCMODE = RM_Field_MODEM_SICTRL2_SIRSTAGCMODE(self)
        self.zz_fdict['SIRSTAGCMODE'] = self.SIRSTAGCMODE
        self.SIRSTPRSMODE = RM_Field_MODEM_SICTRL2_SIRSTPRSMODE(self)
        self.zz_fdict['SIRSTPRSMODE'] = self.SIRSTPRSMODE
        self.SIRSTCCAMODE = RM_Field_MODEM_SICTRL2_SIRSTCCAMODE(self)
        self.zz_fdict['SIRSTCCAMODE'] = self.SIRSTCCAMODE
        self.SUPERCHIPTHRESH = RM_Field_MODEM_SICTRL2_SUPERCHIPTHRESH(self)
        self.zz_fdict['SUPERCHIPTHRESH'] = self.SUPERCHIPTHRESH
        self.DISSIFRAMEDET = RM_Field_MODEM_SICTRL2_DISSIFRAMEDET(self)
        self.zz_fdict['DISSIFRAMEDET'] = self.DISSIFRAMEDET
        self.AGCRSTUPONSI = RM_Field_MODEM_SICTRL2_AGCRSTUPONSI(self)
        self.zz_fdict['AGCRSTUPONSI'] = self.AGCRSTUPONSI
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CHFSWCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CHFSWCTRL, self).__init__(rmio, label,
            0xa8014000, 0x268,
            'CHFSWCTRL', 'MODEM.CHFSWCTRL', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHFSWTIME = RM_Field_MODEM_CHFSWCTRL_CHFSWTIME(self)
        self.zz_fdict['CHFSWTIME'] = self.CHFSWTIME
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMCTRL0, self).__init__(rmio, label,
            0xa8014000, 0x3B0,
            'HADMCTRL0', 'MODEM.HADMCTRL0', 'read-write',
            u"",
            0x00000000, 0xFC00703F,
            0x00001000, 0x00002000,
            0x00003000)

        self.HADMEN = RM_Field_MODEM_HADMCTRL0_HADMEN(self)
        self.zz_fdict['HADMEN'] = self.HADMEN
        self.TESEN = RM_Field_MODEM_HADMCTRL0_TESEN(self)
        self.zz_fdict['TESEN'] = self.TESEN
        self.PESEN = RM_Field_MODEM_HADMCTRL0_PESEN(self)
        self.zz_fdict['PESEN'] = self.PESEN
        self.SNDSEQEN = RM_Field_MODEM_HADMCTRL0_SNDSEQEN(self)
        self.zz_fdict['SNDSEQEN'] = self.SNDSEQEN
        self.ROLE = RM_Field_MODEM_HADMCTRL0_ROLE(self)
        self.zz_fdict['ROLE'] = self.ROLE
        self.RTTPHY = RM_Field_MODEM_HADMCTRL0_RTTPHY(self)
        self.zz_fdict['RTTPHY'] = self.RTTPHY
        self.PM = RM_Field_MODEM_HADMCTRL0_PM(self)
        self.zz_fdict['PM'] = self.PM
        self.AVGMODE = RM_Field_MODEM_HADMCTRL0_AVGMODE(self)
        self.zz_fdict['AVGMODE'] = self.AVGMODE
        self.DFTSCALE = RM_Field_MODEM_HADMCTRL0_DFTSCALE(self)
        self.zz_fdict['DFTSCALE'] = self.DFTSCALE
        self.PKTSENTSEL = RM_Field_MODEM_HADMCTRL0_PKTSENTSEL(self)
        self.zz_fdict['PKTSENTSEL'] = self.PKTSENTSEL
        self.TXUPSAMPOSR4 = RM_Field_MODEM_HADMCTRL0_TXUPSAMPOSR4(self)
        self.zz_fdict['TXUPSAMPOSR4'] = self.TXUPSAMPOSR4
        self.SSAFCGEAR = RM_Field_MODEM_HADMCTRL0_SSAFCGEAR(self)
        self.zz_fdict['SSAFCGEAR'] = self.SSAFCGEAR
        self.SRC2AUTOSCALE = RM_Field_MODEM_HADMCTRL0_SRC2AUTOSCALE(self)
        self.zz_fdict['SRC2AUTOSCALE'] = self.SRC2AUTOSCALE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMCTRL1, self).__init__(rmio, label,
            0xa8014000, 0x3B4,
            'HADMCTRL1', 'MODEM.HADMCTRL1', 'read-write',
            u"",
            0x00040000, 0xFFC7FF07,
            0x00001000, 0x00002000,
            0x00003000)

        self.STEPSTATE = RM_Field_MODEM_HADMCTRL1_STEPSTATE(self)
        self.zz_fdict['STEPSTATE'] = self.STEPSTATE
        self.DFTSTARTOFF = RM_Field_MODEM_HADMCTRL1_DFTSTARTOFF(self)
        self.zz_fdict['DFTSTARTOFF'] = self.DFTSTARTOFF
        self.MAXSCHWIN = RM_Field_MODEM_HADMCTRL1_MAXSCHWIN(self)
        self.zz_fdict['MAXSCHWIN'] = self.MAXSCHWIN
        self.AVGSTARTOFF = RM_Field_MODEM_HADMCTRL1_AVGSTARTOFF(self)
        self.zz_fdict['AVGSTARTOFF'] = self.AVGSTARTOFF
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS0, self).__init__(rmio, label,
            0xa8014000, 0x3B8,
            'HADMSTATUS0', 'MODEM.HADMSTATUS0', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AVG0 = RM_Field_MODEM_HADMSTATUS0_AVG0(self)
        self.zz_fdict['AVG0'] = self.AVG0
        self.AVG1 = RM_Field_MODEM_HADMSTATUS0_AVG1(self)
        self.zz_fdict['AVG1'] = self.AVG1
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS1, self).__init__(rmio, label,
            0xa8014000, 0x3BC,
            'HADMSTATUS1', 'MODEM.HADMSTATUS1', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FREQOFFSET = RM_Field_MODEM_HADMSTATUS1_FREQOFFSET(self)
        self.zz_fdict['FREQOFFSET'] = self.FREQOFFSET
        self.TIMETOX = RM_Field_MODEM_HADMSTATUS1_TIMETOX(self)
        self.zz_fdict['TIMETOX'] = self.TIMETOX
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS2, self).__init__(rmio, label,
            0xa8014000, 0x3C0,
            'HADMSTATUS2', 'MODEM.HADMSTATUS2', 'read-only',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COSTLATE1 = RM_Field_MODEM_HADMSTATUS2_COSTLATE1(self)
        self.zz_fdict['COSTLATE1'] = self.COSTLATE1
        self.COSTCURR1 = RM_Field_MODEM_HADMSTATUS2_COSTCURR1(self)
        self.zz_fdict['COSTCURR1'] = self.COSTCURR1
        self.COSTEARL1 = RM_Field_MODEM_HADMSTATUS2_COSTEARL1(self)
        self.zz_fdict['COSTEARL1'] = self.COSTEARL1
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS3, self).__init__(rmio, label,
            0xa8014000, 0x3C4,
            'HADMSTATUS3', 'MODEM.HADMSTATUS3', 'read-only',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.COSTLATE0 = RM_Field_MODEM_HADMSTATUS3_COSTLATE0(self)
        self.zz_fdict['COSTLATE0'] = self.COSTLATE0
        self.COSTCURR0 = RM_Field_MODEM_HADMSTATUS3_COSTCURR0(self)
        self.zz_fdict['COSTCURR0'] = self.COSTCURR0
        self.COSTEARL0 = RM_Field_MODEM_HADMSTATUS3_COSTEARL0(self)
        self.zz_fdict['COSTEARL0'] = self.COSTEARL0
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS4, self).__init__(rmio, label,
            0xa8014000, 0x3C8,
            'HADMSTATUS4', 'MODEM.HADMSTATUS4', 'read-only',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SBSP500I = RM_Field_MODEM_HADMSTATUS4_SBSP500I(self)
        self.zz_fdict['SBSP500I'] = self.SBSP500I
        self.SBSP500Q = RM_Field_MODEM_HADMSTATUS4_SBSP500Q(self)
        self.zz_fdict['SBSP500Q'] = self.SBSP500Q
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS5, self).__init__(rmio, label,
            0xa8014000, 0x3CC,
            'HADMSTATUS5', 'MODEM.HADMSTATUS5', 'read-only',
            u"",
            0x00000000, 0x7FFF7FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SBSM500I = RM_Field_MODEM_HADMSTATUS5_SBSM500I(self)
        self.zz_fdict['SBSM500I'] = self.SBSM500I
        self.SBSM500Q = RM_Field_MODEM_HADMSTATUS5_SBSM500Q(self)
        self.zz_fdict['SBSM500Q'] = self.SBSM500Q
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_HADMSTATUS6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_HADMSTATUS6, self).__init__(rmio, label,
            0xa8014000, 0x3D0,
            'HADMSTATUS6', 'MODEM.HADMSTATUS6', 'read-only',
            u"",
            0x00000000, 0xFFF0FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FREQMEAS = RM_Field_MODEM_HADMSTATUS6_FREQMEAS(self)
        self.zz_fdict['FREQMEAS'] = self.FREQMEAS
        self.SBSPSCALE = RM_Field_MODEM_HADMSTATUS6_SBSPSCALE(self)
        self.zz_fdict['SBSPSCALE'] = self.SBSPSCALE
        self.SBSMSCALE = RM_Field_MODEM_HADMSTATUS6_SBSMSCALE(self)
        self.zz_fdict['SBSMSCALE'] = self.SBSMSCALE
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TXCORR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TXCORR, self).__init__(rmio, label,
            0xa8014000, 0x3F0,
            'TXCORR', 'MODEM.TXCORR', 'read-write',
            u"",
            0x00000000, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXDGAIN6DB = RM_Field_MODEM_TXCORR_TXDGAIN6DB(self)
        self.zz_fdict['TXDGAIN6DB'] = self.TXDGAIN6DB
        self.TXDGAIN = RM_Field_MODEM_TXCORR_TXDGAIN(self)
        self.zz_fdict['TXDGAIN'] = self.TXDGAIN
        self.TXGAINIMB = RM_Field_MODEM_TXCORR_TXGAINIMB(self)
        self.zz_fdict['TXGAINIMB'] = self.TXGAINIMB
        self.TXPHSIMB = RM_Field_MODEM_TXCORR_TXPHSIMB(self)
        self.zz_fdict['TXPHSIMB'] = self.TXPHSIMB
        self.TXFREQCORR = RM_Field_MODEM_TXCORR_TXFREQCORR(self)
        self.zz_fdict['TXFREQCORR'] = self.TXFREQCORR
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TXMISC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TXMISC, self).__init__(rmio, label,
            0xa8014000, 0x3F4,
            'TXMISC', 'MODEM.TXMISC', 'read-write',
            u"",
            0x0000000E, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FORCECLKEN = RM_Field_MODEM_TXMISC_FORCECLKEN(self)
        self.zz_fdict['FORCECLKEN'] = self.FORCECLKEN
        self.TXIQIMBEN = RM_Field_MODEM_TXMISC_TXIQIMBEN(self)
        self.zz_fdict['TXIQIMBEN'] = self.TXIQIMBEN
        self.TXINTPEN = RM_Field_MODEM_TXMISC_TXINTPEN(self)
        self.zz_fdict['TXINTPEN'] = self.TXINTPEN
        self.TXDSEN = RM_Field_MODEM_TXMISC_TXDSEN(self)
        self.zz_fdict['TXDSEN'] = self.TXDSEN
        self.TXIQSWAP = RM_Field_MODEM_TXMISC_TXIQSWAP(self)
        self.zz_fdict['TXIQSWAP'] = self.TXIQSWAP
        self.TXDACFORMAT = RM_Field_MODEM_TXMISC_TXDACFORMAT(self)
        self.zz_fdict['TXDACFORMAT'] = self.TXDACFORMAT
        self.TXDACFORCE = RM_Field_MODEM_TXMISC_TXDACFORCE(self)
        self.zz_fdict['TXDACFORCE'] = self.TXDACFORCE
        self.TXDCI = RM_Field_MODEM_TXMISC_TXDCI(self)
        self.zz_fdict['TXDCI'] = self.TXDCI
        self.TXDCQ = RM_Field_MODEM_TXMISC_TXDCQ(self)
        self.zz_fdict['TXDCQ'] = self.TXDCQ
        self.BR2M = RM_Field_MODEM_TXMISC_BR2M(self)
        self.zz_fdict['BR2M'] = self.BR2M
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SPARE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SPARE, self).__init__(rmio, label,
            0xa8014000, 0x400,
            'SPARE', 'MODEM.SPARE', 'read-write',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPARE = RM_Field_MODEM_SPARE_SPARE(self)
        self.zz_fdict['SPARE'] = self.SPARE
        self.__dict__['zz_frozen'] = True


