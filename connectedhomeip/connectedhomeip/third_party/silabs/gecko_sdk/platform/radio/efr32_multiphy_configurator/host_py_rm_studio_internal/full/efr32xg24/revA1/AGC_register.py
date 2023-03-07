
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . AGC_field import *


class RM_Register_AGC_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IPVERSION, self).__init__(rmio, label,
            0xa800c000, 0x000,
            'IPVERSION', 'AGC.IPVERSION', 'read-only',
            u"",
            0x00000003, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_AGC_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_EN, self).__init__(rmio, label,
            0xa800c000, 0x004,
            'EN', 'AGC.EN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_AGC_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_STATUS0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STATUS0, self).__init__(rmio, label,
            0xa800c000, 0x008,
            'STATUS0', 'AGC.STATUS0', 'read-only',
            u"",
            0x00000000, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.GAININDEX = RM_Field_AGC_STATUS0_GAININDEX(self)
        self.zz_fdict['GAININDEX'] = self.GAININDEX
        self.RFPKDLOWLAT = RM_Field_AGC_STATUS0_RFPKDLOWLAT(self)
        self.zz_fdict['RFPKDLOWLAT'] = self.RFPKDLOWLAT
        self.RFPKDHILAT = RM_Field_AGC_STATUS0_RFPKDHILAT(self)
        self.zz_fdict['RFPKDHILAT'] = self.RFPKDHILAT
        self.IFPKDLOLAT = RM_Field_AGC_STATUS0_IFPKDLOLAT(self)
        self.zz_fdict['IFPKDLOLAT'] = self.IFPKDLOLAT
        self.IFPKDHILAT = RM_Field_AGC_STATUS0_IFPKDHILAT(self)
        self.zz_fdict['IFPKDHILAT'] = self.IFPKDHILAT
        self.CCA = RM_Field_AGC_STATUS0_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.GAINOK = RM_Field_AGC_STATUS0_GAINOK(self)
        self.zz_fdict['GAINOK'] = self.GAINOK
        self.PGAINDEX = RM_Field_AGC_STATUS0_PGAINDEX(self)
        self.zz_fdict['PGAINDEX'] = self.PGAINDEX
        self.LNAINDEX = RM_Field_AGC_STATUS0_LNAINDEX(self)
        self.zz_fdict['LNAINDEX'] = self.LNAINDEX
        self.PNINDEX = RM_Field_AGC_STATUS0_PNINDEX(self)
        self.zz_fdict['PNINDEX'] = self.PNINDEX
        self.ADCINDEX = RM_Field_AGC_STATUS0_ADCINDEX(self)
        self.zz_fdict['ADCINDEX'] = self.ADCINDEX
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_STATUS1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STATUS1, self).__init__(rmio, label,
            0xa800c000, 0x00C,
            'STATUS1', 'AGC.STATUS1', 'read-only',
            u"",
            0x00000000, 0x3FFFFEFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHPWR = RM_Field_AGC_STATUS1_CHPWR(self)
        self.zz_fdict['CHPWR'] = self.CHPWR
        self.FASTLOOPSTATE = RM_Field_AGC_STATUS1_FASTLOOPSTATE(self)
        self.zz_fdict['FASTLOOPSTATE'] = self.FASTLOOPSTATE
        self.CFLOOPSTATE = RM_Field_AGC_STATUS1_CFLOOPSTATE(self)
        self.zz_fdict['CFLOOPSTATE'] = self.CFLOOPSTATE
        self.RSSISTATE = RM_Field_AGC_STATUS1_RSSISTATE(self)
        self.zz_fdict['RSSISTATE'] = self.RSSISTATE
        self.RFPKDLOWLATCNT = RM_Field_AGC_STATUS1_RFPKDLOWLATCNT(self)
        self.zz_fdict['RFPKDLOWLATCNT'] = self.RFPKDLOWLATCNT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_STATUS2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STATUS2, self).__init__(rmio, label,
            0xa800c000, 0x010,
            'STATUS2', 'AGC.STATUS2', 'read-only',
            u"",
            0x00000000, 0xFFFF4FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFPKDHILATCNT = RM_Field_AGC_STATUS2_RFPKDHILATCNT(self)
        self.zz_fdict['RFPKDHILATCNT'] = self.RFPKDHILATCNT
        self.PNDWNUP = RM_Field_AGC_STATUS2_PNDWNUP(self)
        self.zz_fdict['PNDWNUP'] = self.PNDWNUP
        self.RFPKDPRDCNT = RM_Field_AGC_STATUS2_RFPKDPRDCNT(self)
        self.zz_fdict['RFPKDPRDCNT'] = self.RFPKDPRDCNT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RSSI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSI, self).__init__(rmio, label,
            0xa800c000, 0x018,
            'RSSI', 'AGC.RSSI', 'read-only',
            u"",
            0x00008000, 0x0000FFC0,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIFRAC = RM_Field_AGC_RSSI_RSSIFRAC(self)
        self.zz_fdict['RSSIFRAC'] = self.RSSIFRAC
        self.RSSIINT = RM_Field_AGC_RSSI_RSSIINT(self)
        self.zz_fdict['RSSIINT'] = self.RSSIINT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FRAMERSSI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FRAMERSSI, self).__init__(rmio, label,
            0xa800c000, 0x01C,
            'FRAMERSSI', 'AGC.FRAMERSSI', 'read-only',
            u"",
            0x00008000, 0x0000FFC0,
            0x00001000, 0x00002000,
            0x00003000)

        self.FRAMERSSIFRAC = RM_Field_AGC_FRAMERSSI_FRAMERSSIFRAC(self)
        self.zz_fdict['FRAMERSSIFRAC'] = self.FRAMERSSIFRAC
        self.FRAMERSSIINT = RM_Field_AGC_FRAMERSSI_FRAMERSSIINT(self)
        self.zz_fdict['FRAMERSSIINT'] = self.FRAMERSSIINT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL0, self).__init__(rmio, label,
            0xa800c000, 0x020,
            'CTRL0', 'AGC.CTRL0', 'read-write',
            u"",
            0x2132727F, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PWRTARGET = RM_Field_AGC_CTRL0_PWRTARGET(self)
        self.zz_fdict['PWRTARGET'] = self.PWRTARGET
        self.MODE = RM_Field_AGC_CTRL0_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.RSSISHIFT = RM_Field_AGC_CTRL0_RSSISHIFT(self)
        self.zz_fdict['RSSISHIFT'] = self.RSSISHIFT
        self.DISCFLOOPADJ = RM_Field_AGC_CTRL0_DISCFLOOPADJ(self)
        self.zz_fdict['DISCFLOOPADJ'] = self.DISCFLOOPADJ
        self.CFLOOPNFADJ = RM_Field_AGC_CTRL0_CFLOOPNFADJ(self)
        self.zz_fdict['CFLOOPNFADJ'] = self.CFLOOPNFADJ
        self.CFLOOPNEWCALC = RM_Field_AGC_CTRL0_CFLOOPNEWCALC(self)
        self.zz_fdict['CFLOOPNEWCALC'] = self.CFLOOPNEWCALC
        self.DISRESETCHPWR = RM_Field_AGC_CTRL0_DISRESETCHPWR(self)
        self.zz_fdict['DISRESETCHPWR'] = self.DISRESETCHPWR
        self.ADCATTENMODE = RM_Field_AGC_CTRL0_ADCATTENMODE(self)
        self.zz_fdict['ADCATTENMODE'] = self.ADCATTENMODE
        self.FENOTCHMODESEL = RM_Field_AGC_CTRL0_FENOTCHMODESEL(self)
        self.zz_fdict['FENOTCHMODESEL'] = self.FENOTCHMODESEL
        self.ADCATTENCODE = RM_Field_AGC_CTRL0_ADCATTENCODE(self)
        self.zz_fdict['ADCATTENCODE'] = self.ADCATTENCODE
        self.ENRSSIRESET = RM_Field_AGC_CTRL0_ENRSSIRESET(self)
        self.zz_fdict['ENRSSIRESET'] = self.ENRSSIRESET
        self.DSADISCFLOOP = RM_Field_AGC_CTRL0_DSADISCFLOOP(self)
        self.zz_fdict['DSADISCFLOOP'] = self.DSADISCFLOOP
        self.DISPNGAINUP = RM_Field_AGC_CTRL0_DISPNGAINUP(self)
        self.zz_fdict['DISPNGAINUP'] = self.DISPNGAINUP
        self.DISPNDWNCOMP = RM_Field_AGC_CTRL0_DISPNDWNCOMP(self)
        self.zz_fdict['DISPNDWNCOMP'] = self.DISPNDWNCOMP
        self.AGCRST = RM_Field_AGC_CTRL0_AGCRST(self)
        self.zz_fdict['AGCRST'] = self.AGCRST
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL1, self).__init__(rmio, label,
            0xa800c000, 0x024,
            'CTRL1', 'AGC.CTRL1', 'read-write',
            u"",
            0x00001300, 0x0007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CCATHRSH = RM_Field_AGC_CTRL1_CCATHRSH(self)
        self.zz_fdict['CCATHRSH'] = self.CCATHRSH
        self.RSSIPERIOD = RM_Field_AGC_CTRL1_RSSIPERIOD(self)
        self.zz_fdict['RSSIPERIOD'] = self.RSSIPERIOD
        self.PWRPERIOD = RM_Field_AGC_CTRL1_PWRPERIOD(self)
        self.zz_fdict['PWRPERIOD'] = self.PWRPERIOD
        self.CCAMODE = RM_Field_AGC_CTRL1_CCAMODE(self)
        self.zz_fdict['CCAMODE'] = self.CCAMODE
        self.CCAMODE3LOGIC = RM_Field_AGC_CTRL1_CCAMODE3LOGIC(self)
        self.zz_fdict['CCAMODE3LOGIC'] = self.CCAMODE3LOGIC
        self.CCASWCTRL = RM_Field_AGC_CTRL1_CCASWCTRL(self)
        self.zz_fdict['CCASWCTRL'] = self.CCASWCTRL
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL2, self).__init__(rmio, label,
            0xa800c000, 0x028,
            'CTRL2', 'AGC.CTRL2', 'read-write',
            u"",
            0x0000610A, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DMASEL = RM_Field_AGC_CTRL2_DMASEL(self)
        self.zz_fdict['DMASEL'] = self.DMASEL
        self.SAFEMODE = RM_Field_AGC_CTRL2_SAFEMODE(self)
        self.zz_fdict['SAFEMODE'] = self.SAFEMODE
        self.SAFEMODETHD = RM_Field_AGC_CTRL2_SAFEMODETHD(self)
        self.zz_fdict['SAFEMODETHD'] = self.SAFEMODETHD
        self.REHICNTTHD = RM_Field_AGC_CTRL2_REHICNTTHD(self)
        self.zz_fdict['REHICNTTHD'] = self.REHICNTTHD
        self.RELOTHD = RM_Field_AGC_CTRL2_RELOTHD(self)
        self.zz_fdict['RELOTHD'] = self.RELOTHD
        self.RELBYCHPWR = RM_Field_AGC_CTRL2_RELBYCHPWR(self)
        self.zz_fdict['RELBYCHPWR'] = self.RELBYCHPWR
        self.RELTARGETPWR = RM_Field_AGC_CTRL2_RELTARGETPWR(self)
        self.zz_fdict['RELTARGETPWR'] = self.RELTARGETPWR
        self.RSSICCASUB = RM_Field_AGC_CTRL2_RSSICCASUB(self)
        self.zz_fdict['RSSICCASUB'] = self.RSSICCASUB
        self.DEBCNTRST = RM_Field_AGC_CTRL2_DEBCNTRST(self)
        self.zz_fdict['DEBCNTRST'] = self.DEBCNTRST
        self.PRSDEBUGEN = RM_Field_AGC_CTRL2_PRSDEBUGEN(self)
        self.zz_fdict['PRSDEBUGEN'] = self.PRSDEBUGEN
        self.DISRFPKD = RM_Field_AGC_CTRL2_DISRFPKD(self)
        self.zz_fdict['DISRFPKD'] = self.DISRFPKD
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL3, self).__init__(rmio, label,
            0xa800c000, 0x02C,
            'CTRL3', 'AGC.CTRL3', 'read-write',
            u"",
            0x5140A800, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFPKDDEB = RM_Field_AGC_CTRL3_IFPKDDEB(self)
        self.zz_fdict['IFPKDDEB'] = self.IFPKDDEB
        self.IFPKDDEBTHD = RM_Field_AGC_CTRL3_IFPKDDEBTHD(self)
        self.zz_fdict['IFPKDDEBTHD'] = self.IFPKDDEBTHD
        self.IFPKDDEBPRD = RM_Field_AGC_CTRL3_IFPKDDEBPRD(self)
        self.zz_fdict['IFPKDDEBPRD'] = self.IFPKDDEBPRD
        self.IFPKDDEBRST = RM_Field_AGC_CTRL3_IFPKDDEBRST(self)
        self.zz_fdict['IFPKDDEBRST'] = self.IFPKDDEBRST
        self.RFPKDDEB = RM_Field_AGC_CTRL3_RFPKDDEB(self)
        self.zz_fdict['RFPKDDEB'] = self.RFPKDDEB
        self.RFPKDDEBTHD = RM_Field_AGC_CTRL3_RFPKDDEBTHD(self)
        self.zz_fdict['RFPKDDEBTHD'] = self.RFPKDDEBTHD
        self.RFPKDDEBPRD = RM_Field_AGC_CTRL3_RFPKDDEBPRD(self)
        self.zz_fdict['RFPKDDEBPRD'] = self.RFPKDDEBPRD
        self.RFPKDDEBRST = RM_Field_AGC_CTRL3_RFPKDDEBRST(self)
        self.zz_fdict['RFPKDDEBRST'] = self.RFPKDDEBRST
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL4, self).__init__(rmio, label,
            0xa800c000, 0x030,
            'CTRL4', 'AGC.CTRL4', 'read-write',
            u"",
            0x0000000E, 0xFF80FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PERIODRFPKD = RM_Field_AGC_CTRL4_PERIODRFPKD(self)
        self.zz_fdict['PERIODRFPKD'] = self.PERIODRFPKD
        self.DISRFPKDCNTRST = RM_Field_AGC_CTRL4_DISRFPKDCNTRST(self)
        self.zz_fdict['DISRFPKDCNTRST'] = self.DISRFPKDCNTRST
        self.DISRSTCONDI = RM_Field_AGC_CTRL4_DISRSTCONDI(self)
        self.zz_fdict['DISRSTCONDI'] = self.DISRSTCONDI
        self.RFPKDPRDGEAR = RM_Field_AGC_CTRL4_RFPKDPRDGEAR(self)
        self.zz_fdict['RFPKDPRDGEAR'] = self.RFPKDPRDGEAR
        self.RFPKDSYNCSEL = RM_Field_AGC_CTRL4_RFPKDSYNCSEL(self)
        self.zz_fdict['RFPKDSYNCSEL'] = self.RFPKDSYNCSEL
        self.RFPKDSEL = RM_Field_AGC_CTRL4_RFPKDSEL(self)
        self.zz_fdict['RFPKDSEL'] = self.RFPKDSEL
        self.FRZPKDEN = RM_Field_AGC_CTRL4_FRZPKDEN(self)
        self.zz_fdict['FRZPKDEN'] = self.FRZPKDEN
        self.RFPKDCNTEN = RM_Field_AGC_CTRL4_RFPKDCNTEN(self)
        self.zz_fdict['RFPKDCNTEN'] = self.RFPKDCNTEN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL5, self).__init__(rmio, label,
            0xa800c000, 0x034,
            'CTRL5', 'AGC.CTRL5', 'read-write',
            u"",
            0x00000000, 0xC0FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PNUPDISTHD = RM_Field_AGC_CTRL5_PNUPDISTHD(self)
        self.zz_fdict['PNUPDISTHD'] = self.PNUPDISTHD
        self.PNUPRELTHD = RM_Field_AGC_CTRL5_PNUPRELTHD(self)
        self.zz_fdict['PNUPRELTHD'] = self.PNUPRELTHD
        self.SEQPNUPALLOW = RM_Field_AGC_CTRL5_SEQPNUPALLOW(self)
        self.zz_fdict['SEQPNUPALLOW'] = self.SEQPNUPALLOW
        self.SEQRFPKDEN = RM_Field_AGC_CTRL5_SEQRFPKDEN(self)
        self.zz_fdict['SEQRFPKDEN'] = self.SEQRFPKDEN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL6, self).__init__(rmio, label,
            0xa800c000, 0x038,
            'CTRL6', 'AGC.CTRL6', 'read-write',
            u"",
            0x0003AAA8, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DUALRFPKDDEC = RM_Field_AGC_CTRL6_DUALRFPKDDEC(self)
        self.zz_fdict['DUALRFPKDDEC'] = self.DUALRFPKDDEC
        self.ENDUALRFPKD = RM_Field_AGC_CTRL6_ENDUALRFPKD(self)
        self.zz_fdict['ENDUALRFPKD'] = self.ENDUALRFPKD
        self.GAINDETTHD = RM_Field_AGC_CTRL6_GAINDETTHD(self)
        self.zz_fdict['GAINDETTHD'] = self.GAINDETTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL7, self).__init__(rmio, label,
            0xa800c000, 0x03C,
            'CTRL7', 'AGC.CTRL7', 'read-write',
            u"",
            0x00000000, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SUBDEN = RM_Field_AGC_CTRL7_SUBDEN(self)
        self.zz_fdict['SUBDEN'] = self.SUBDEN
        self.SUBINT = RM_Field_AGC_CTRL7_SUBINT(self)
        self.zz_fdict['SUBINT'] = self.SUBINT
        self.SUBNUM = RM_Field_AGC_CTRL7_SUBNUM(self)
        self.zz_fdict['SUBNUM'] = self.SUBNUM
        self.SUBPERIOD = RM_Field_AGC_CTRL7_SUBPERIOD(self)
        self.zz_fdict['SUBPERIOD'] = self.SUBPERIOD
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RSSISTEPTHR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSISTEPTHR, self).__init__(rmio, label,
            0xa800c000, 0x040,
            'RSSISTEPTHR', 'AGC.RSSISTEPTHR', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.POSSTEPTHR = RM_Field_AGC_RSSISTEPTHR_POSSTEPTHR(self)
        self.zz_fdict['POSSTEPTHR'] = self.POSSTEPTHR
        self.NEGSTEPTHR = RM_Field_AGC_RSSISTEPTHR_NEGSTEPTHR(self)
        self.zz_fdict['NEGSTEPTHR'] = self.NEGSTEPTHR
        self.STEPPER = RM_Field_AGC_RSSISTEPTHR_STEPPER(self)
        self.zz_fdict['STEPPER'] = self.STEPPER
        self.DEMODRESTARTPER = RM_Field_AGC_RSSISTEPTHR_DEMODRESTARTPER(self)
        self.zz_fdict['DEMODRESTARTPER'] = self.DEMODRESTARTPER
        self.DEMODRESTARTTHR = RM_Field_AGC_RSSISTEPTHR_DEMODRESTARTTHR(self)
        self.zz_fdict['DEMODRESTARTTHR'] = self.DEMODRESTARTTHR
        self.RSSIFAST = RM_Field_AGC_RSSISTEPTHR_RSSIFAST(self)
        self.zz_fdict['RSSIFAST'] = self.RSSIFAST
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_MANGAIN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_MANGAIN, self).__init__(rmio, label,
            0xa800c000, 0x044,
            'MANGAIN', 'AGC.MANGAIN', 'read-write',
            u"",
            0x00000000, 0x0001FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MANGAINEN = RM_Field_AGC_MANGAIN_MANGAINEN(self)
        self.zz_fdict['MANGAINEN'] = self.MANGAINEN
        self.MANGAINIFPGA = RM_Field_AGC_MANGAIN_MANGAINIFPGA(self)
        self.zz_fdict['MANGAINIFPGA'] = self.MANGAINIFPGA
        self.MANGAINLNA = RM_Field_AGC_MANGAIN_MANGAINLNA(self)
        self.zz_fdict['MANGAINLNA'] = self.MANGAINLNA
        self.MANGAINPN = RM_Field_AGC_MANGAIN_MANGAINPN(self)
        self.zz_fdict['MANGAINPN'] = self.MANGAINPN
        self.MANRFLATRST = RM_Field_AGC_MANGAIN_MANRFLATRST(self)
        self.zz_fdict['MANRFLATRST'] = self.MANRFLATRST
        self.MANIFLOLATRST = RM_Field_AGC_MANGAIN_MANIFLOLATRST(self)
        self.zz_fdict['MANIFLOLATRST'] = self.MANIFLOLATRST
        self.MANIFHILATRST = RM_Field_AGC_MANGAIN_MANIFHILATRST(self)
        self.zz_fdict['MANIFHILATRST'] = self.MANIFHILATRST
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IF, self).__init__(rmio, label,
            0xa800c000, 0x048,
            'IF', 'AGC.IF', 'read-write',
            u"",
            0x00000000, 0x00007F7D,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIVALID = RM_Field_AGC_IF_RSSIVALID(self)
        self.zz_fdict['RSSIVALID'] = self.RSSIVALID
        self.CCA = RM_Field_AGC_IF_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.RSSIPOSSTEP = RM_Field_AGC_IF_RSSIPOSSTEP(self)
        self.zz_fdict['RSSIPOSSTEP'] = self.RSSIPOSSTEP
        self.RSSINEGSTEP = RM_Field_AGC_IF_RSSINEGSTEP(self)
        self.zz_fdict['RSSINEGSTEP'] = self.RSSINEGSTEP
        self.RSSIDONE = RM_Field_AGC_IF_RSSIDONE(self)
        self.zz_fdict['RSSIDONE'] = self.RSSIDONE
        self.SHORTRSSIPOSSTEP = RM_Field_AGC_IF_SHORTRSSIPOSSTEP(self)
        self.zz_fdict['SHORTRSSIPOSSTEP'] = self.SHORTRSSIPOSSTEP
        self.RFPKDPRDDONE = RM_Field_AGC_IF_RFPKDPRDDONE(self)
        self.zz_fdict['RFPKDPRDDONE'] = self.RFPKDPRDDONE
        self.RFPKDCNTDONE = RM_Field_AGC_IF_RFPKDCNTDONE(self)
        self.zz_fdict['RFPKDCNTDONE'] = self.RFPKDCNTDONE
        self.RSSIHIGH = RM_Field_AGC_IF_RSSIHIGH(self)
        self.zz_fdict['RSSIHIGH'] = self.RSSIHIGH
        self.RSSILOW = RM_Field_AGC_IF_RSSILOW(self)
        self.zz_fdict['RSSILOW'] = self.RSSILOW
        self.CCANODET = RM_Field_AGC_IF_CCANODET(self)
        self.zz_fdict['CCANODET'] = self.CCANODET
        self.GAINBELOWGAINTHD = RM_Field_AGC_IF_GAINBELOWGAINTHD(self)
        self.zz_fdict['GAINBELOWGAINTHD'] = self.GAINBELOWGAINTHD
        self.GAINUPDATEFRZ = RM_Field_AGC_IF_GAINUPDATEFRZ(self)
        self.zz_fdict['GAINUPDATEFRZ'] = self.GAINUPDATEFRZ
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IEN, self).__init__(rmio, label,
            0xa800c000, 0x04C,
            'IEN', 'AGC.IEN', 'read-write',
            u"",
            0x00000000, 0x00007F7D,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIVALID = RM_Field_AGC_IEN_RSSIVALID(self)
        self.zz_fdict['RSSIVALID'] = self.RSSIVALID
        self.CCA = RM_Field_AGC_IEN_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.RSSIPOSSTEP = RM_Field_AGC_IEN_RSSIPOSSTEP(self)
        self.zz_fdict['RSSIPOSSTEP'] = self.RSSIPOSSTEP
        self.RSSINEGSTEP = RM_Field_AGC_IEN_RSSINEGSTEP(self)
        self.zz_fdict['RSSINEGSTEP'] = self.RSSINEGSTEP
        self.RSSIDONE = RM_Field_AGC_IEN_RSSIDONE(self)
        self.zz_fdict['RSSIDONE'] = self.RSSIDONE
        self.SHORTRSSIPOSSTEP = RM_Field_AGC_IEN_SHORTRSSIPOSSTEP(self)
        self.zz_fdict['SHORTRSSIPOSSTEP'] = self.SHORTRSSIPOSSTEP
        self.RFPKDPRDDONE = RM_Field_AGC_IEN_RFPKDPRDDONE(self)
        self.zz_fdict['RFPKDPRDDONE'] = self.RFPKDPRDDONE
        self.RFPKDCNTDONE = RM_Field_AGC_IEN_RFPKDCNTDONE(self)
        self.zz_fdict['RFPKDCNTDONE'] = self.RFPKDCNTDONE
        self.RSSIHIGH = RM_Field_AGC_IEN_RSSIHIGH(self)
        self.zz_fdict['RSSIHIGH'] = self.RSSIHIGH
        self.RSSILOW = RM_Field_AGC_IEN_RSSILOW(self)
        self.zz_fdict['RSSILOW'] = self.RSSILOW
        self.CCANODET = RM_Field_AGC_IEN_CCANODET(self)
        self.zz_fdict['CCANODET'] = self.CCANODET
        self.GAINBELOWGAINTHD = RM_Field_AGC_IEN_GAINBELOWGAINTHD(self)
        self.zz_fdict['GAINBELOWGAINTHD'] = self.GAINBELOWGAINTHD
        self.GAINUPDATEFRZ = RM_Field_AGC_IEN_GAINUPDATEFRZ(self)
        self.zz_fdict['GAINUPDATEFRZ'] = self.GAINUPDATEFRZ
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CMD, self).__init__(rmio, label,
            0xa800c000, 0x050,
            'CMD', 'AGC.CMD', 'write-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSISTART = RM_Field_AGC_CMD_RSSISTART(self)
        self.zz_fdict['RSSISTART'] = self.RSSISTART
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINRANGE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINRANGE, self).__init__(rmio, label,
            0xa800c000, 0x054,
            'GAINRANGE', 'AGC.GAINRANGE', 'read-write',
            u"",
            0x00813187, 0x03FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAINDEXBORDER = RM_Field_AGC_GAINRANGE_LNAINDEXBORDER(self)
        self.zz_fdict['LNAINDEXBORDER'] = self.LNAINDEXBORDER
        self.PGAINDEXBORDER = RM_Field_AGC_GAINRANGE_PGAINDEXBORDER(self)
        self.zz_fdict['PGAINDEXBORDER'] = self.PGAINDEXBORDER
        self.GAININCSTEP = RM_Field_AGC_GAINRANGE_GAININCSTEP(self)
        self.zz_fdict['GAININCSTEP'] = self.GAININCSTEP
        self.PNGAINSTEP = RM_Field_AGC_GAINRANGE_PNGAINSTEP(self)
        self.zz_fdict['PNGAINSTEP'] = self.PNGAINSTEP
        self.LATCHEDHISTEP = RM_Field_AGC_GAINRANGE_LATCHEDHISTEP(self)
        self.zz_fdict['LATCHEDHISTEP'] = self.LATCHEDHISTEP
        self.HIPWRTHD = RM_Field_AGC_GAINRANGE_HIPWRTHD(self)
        self.zz_fdict['HIPWRTHD'] = self.HIPWRTHD
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_AGCPERIOD0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_AGCPERIOD0, self).__init__(rmio, label,
            0xa800c000, 0x058,
            'AGCPERIOD0', 'AGC.AGCPERIOD0', 'read-write',
            u"",
            0xD607000E, 0xFFFF01FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PERIODHI = RM_Field_AGC_AGCPERIOD0_PERIODHI(self)
        self.zz_fdict['PERIODHI'] = self.PERIODHI
        self.MAXHICNTTHD = RM_Field_AGC_AGCPERIOD0_MAXHICNTTHD(self)
        self.zz_fdict['MAXHICNTTHD'] = self.MAXHICNTTHD
        self.SETTLETIMEIF = RM_Field_AGC_AGCPERIOD0_SETTLETIMEIF(self)
        self.zz_fdict['SETTLETIMEIF'] = self.SETTLETIMEIF
        self.SETTLETIMERF = RM_Field_AGC_AGCPERIOD0_SETTLETIMERF(self)
        self.zz_fdict['SETTLETIMERF'] = self.SETTLETIMERF
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_AGCPERIOD1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_AGCPERIOD1, self).__init__(rmio, label,
            0xa800c000, 0x05C,
            'AGCPERIOD1', 'AGC.AGCPERIOD1', 'read-write',
            u"",
            0x00000037, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PERIODLOW = RM_Field_AGC_AGCPERIOD1_PERIODLOW(self)
        self.zz_fdict['PERIODLOW'] = self.PERIODLOW
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_HICNTREGION0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_HICNTREGION0, self).__init__(rmio, label,
            0xa800c000, 0x060,
            'HICNTREGION0', 'AGC.HICNTREGION0', 'read-write',
            u"",
            0x06050403, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.HICNTREGION0 = RM_Field_AGC_HICNTREGION0_HICNTREGION0(self)
        self.zz_fdict['HICNTREGION0'] = self.HICNTREGION0
        self.HICNTREGION1 = RM_Field_AGC_HICNTREGION0_HICNTREGION1(self)
        self.zz_fdict['HICNTREGION1'] = self.HICNTREGION1
        self.HICNTREGION2 = RM_Field_AGC_HICNTREGION0_HICNTREGION2(self)
        self.zz_fdict['HICNTREGION2'] = self.HICNTREGION2
        self.HICNTREGION3 = RM_Field_AGC_HICNTREGION0_HICNTREGION3(self)
        self.zz_fdict['HICNTREGION3'] = self.HICNTREGION3
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_HICNTREGION1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_HICNTREGION1, self).__init__(rmio, label,
            0xa800c000, 0x064,
            'HICNTREGION1', 'AGC.HICNTREGION1', 'read-write',
            u"",
            0x00000008, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.HICNTREGION4 = RM_Field_AGC_HICNTREGION1_HICNTREGION4(self)
        self.zz_fdict['HICNTREGION4'] = self.HICNTREGION4
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_STEPDWN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STEPDWN, self).__init__(rmio, label,
            0xa800c000, 0x068,
            'STEPDWN', 'AGC.STEPDWN', 'read-write',
            u"",
            0x00036D11, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STEPDWN0 = RM_Field_AGC_STEPDWN_STEPDWN0(self)
        self.zz_fdict['STEPDWN0'] = self.STEPDWN0
        self.STEPDWN1 = RM_Field_AGC_STEPDWN_STEPDWN1(self)
        self.zz_fdict['STEPDWN1'] = self.STEPDWN1
        self.STEPDWN2 = RM_Field_AGC_STEPDWN_STEPDWN2(self)
        self.zz_fdict['STEPDWN2'] = self.STEPDWN2
        self.STEPDWN3 = RM_Field_AGC_STEPDWN_STEPDWN3(self)
        self.zz_fdict['STEPDWN3'] = self.STEPDWN3
        self.STEPDWN4 = RM_Field_AGC_STEPDWN_STEPDWN4(self)
        self.zz_fdict['STEPDWN4'] = self.STEPDWN4
        self.STEPDWN5 = RM_Field_AGC_STEPDWN_STEPDWN5(self)
        self.zz_fdict['STEPDWN5'] = self.STEPDWN5
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINSTEPLIM0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINSTEPLIM0, self).__init__(rmio, label,
            0xa800c000, 0x06C,
            'GAINSTEPLIM0', 'AGC.GAINSTEPLIM0', 'read-write',
            u"",
            0x00003144, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CFLOOPSTEPMAX = RM_Field_AGC_GAINSTEPLIM0_CFLOOPSTEPMAX(self)
        self.zz_fdict['CFLOOPSTEPMAX'] = self.CFLOOPSTEPMAX
        self.CFLOOPDEL = RM_Field_AGC_GAINSTEPLIM0_CFLOOPDEL(self)
        self.zz_fdict['CFLOOPDEL'] = self.CFLOOPDEL
        self.HYST = RM_Field_AGC_GAINSTEPLIM0_HYST(self)
        self.zz_fdict['HYST'] = self.HYST
        self.MAXPWRVAR = RM_Field_AGC_GAINSTEPLIM0_MAXPWRVAR(self)
        self.zz_fdict['MAXPWRVAR'] = self.MAXPWRVAR
        self.TRANRSTAGC = RM_Field_AGC_GAINSTEPLIM0_TRANRSTAGC(self)
        self.zz_fdict['TRANRSTAGC'] = self.TRANRSTAGC
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINSTEPLIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINSTEPLIM1, self).__init__(rmio, label,
            0xa800c000, 0x070,
            'GAINSTEPLIM1', 'AGC.GAINSTEPLIM1', 'read-write',
            u"",
            0x000011BA, 0x00001FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAINDEXMAX = RM_Field_AGC_GAINSTEPLIM1_LNAINDEXMAX(self)
        self.zz_fdict['LNAINDEXMAX'] = self.LNAINDEXMAX
        self.PGAINDEXMAX = RM_Field_AGC_GAINSTEPLIM1_PGAINDEXMAX(self)
        self.zz_fdict['PGAINDEXMAX'] = self.PGAINDEXMAX
        self.PNINDEXMAX = RM_Field_AGC_GAINSTEPLIM1_PNINDEXMAX(self)
        self.zz_fdict['PNINDEXMAX'] = self.PNINDEXMAX
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT0, self).__init__(rmio, label,
            0xa800c000, 0x074,
            'PNRFATT0', 'AGC.PNRFATT0', 'read-write',
            u"",
            0x00200400, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT1 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT1(self)
        self.zz_fdict['LNAMIXRFATT1'] = self.LNAMIXRFATT1
        self.LNAMIXRFATT2 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT2(self)
        self.zz_fdict['LNAMIXRFATT2'] = self.LNAMIXRFATT2
        self.LNAMIXRFATT3 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT3(self)
        self.zz_fdict['LNAMIXRFATT3'] = self.LNAMIXRFATT3
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT1, self).__init__(rmio, label,
            0xa800c000, 0x078,
            'PNRFATT1', 'AGC.PNRFATT1', 'read-write',
            u"",
            0x00801804, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT4 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT4(self)
        self.zz_fdict['LNAMIXRFATT4'] = self.LNAMIXRFATT4
        self.LNAMIXRFATT5 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT5(self)
        self.zz_fdict['LNAMIXRFATT5'] = self.LNAMIXRFATT5
        self.LNAMIXRFATT6 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT6(self)
        self.zz_fdict['LNAMIXRFATT6'] = self.LNAMIXRFATT6
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT2, self).__init__(rmio, label,
            0xa800c000, 0x07C,
            'PNRFATT2', 'AGC.PNRFATT2', 'read-write',
            u"",
            0x01203C0B, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT7 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT7(self)
        self.zz_fdict['LNAMIXRFATT7'] = self.LNAMIXRFATT7
        self.LNAMIXRFATT8 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT8(self)
        self.zz_fdict['LNAMIXRFATT8'] = self.LNAMIXRFATT8
        self.LNAMIXRFATT9 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT9(self)
        self.zz_fdict['LNAMIXRFATT9'] = self.LNAMIXRFATT9
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT3, self).__init__(rmio, label,
            0xa800c000, 0x080,
            'PNRFATT3', 'AGC.PNRFATT3', 'read-write',
            u"",
            0x02107C18, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT10 = RM_Field_AGC_PNRFATT3_LNAMIXRFATT10(self)
        self.zz_fdict['LNAMIXRFATT10'] = self.LNAMIXRFATT10
        self.LNAMIXRFATT11 = RM_Field_AGC_PNRFATT3_LNAMIXRFATT11(self)
        self.zz_fdict['LNAMIXRFATT11'] = self.LNAMIXRFATT11
        self.LNAMIXRFATT12 = RM_Field_AGC_PNRFATT3_LNAMIXRFATT12(self)
        self.zz_fdict['LNAMIXRFATT12'] = self.LNAMIXRFATT12
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT4, self).__init__(rmio, label,
            0xa800c000, 0x084,
            'PNRFATT4', 'AGC.PNRFATT4', 'read-write',
            u"",
            0x06E0FC2F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT13 = RM_Field_AGC_PNRFATT4_LNAMIXRFATT13(self)
        self.zz_fdict['LNAMIXRFATT13'] = self.LNAMIXRFATT13
        self.LNAMIXRFATT14 = RM_Field_AGC_PNRFATT4_LNAMIXRFATT14(self)
        self.zz_fdict['LNAMIXRFATT14'] = self.LNAMIXRFATT14
        self.LNAMIXRFATT15 = RM_Field_AGC_PNRFATT4_LNAMIXRFATT15(self)
        self.zz_fdict['LNAMIXRFATT15'] = self.LNAMIXRFATT15
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT5, self).__init__(rmio, label,
            0xa800c000, 0x088,
            'PNRFATT5', 'AGC.PNRFATT5', 'read-write',
            u"",
            0x0180480F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT16 = RM_Field_AGC_PNRFATT5_LNAMIXRFATT16(self)
        self.zz_fdict['LNAMIXRFATT16'] = self.LNAMIXRFATT16
        self.LNAMIXRFATT17 = RM_Field_AGC_PNRFATT5_LNAMIXRFATT17(self)
        self.zz_fdict['LNAMIXRFATT17'] = self.LNAMIXRFATT17
        self.LNAMIXRFATT18 = RM_Field_AGC_PNRFATT5_LNAMIXRFATT18(self)
        self.zz_fdict['LNAMIXRFATT18'] = self.LNAMIXRFATT18
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT6, self).__init__(rmio, label,
            0xa800c000, 0x08C,
            'PNRFATT6', 'AGC.PNRFATT6', 'read-write',
            u"",
            0x02F0841F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT19 = RM_Field_AGC_PNRFATT6_LNAMIXRFATT19(self)
        self.zz_fdict['LNAMIXRFATT19'] = self.LNAMIXRFATT19
        self.LNAMIXRFATT20 = RM_Field_AGC_PNRFATT6_LNAMIXRFATT20(self)
        self.zz_fdict['LNAMIXRFATT20'] = self.LNAMIXRFATT20
        self.LNAMIXRFATT21 = RM_Field_AGC_PNRFATT6_LNAMIXRFATT21(self)
        self.zz_fdict['LNAMIXRFATT21'] = self.LNAMIXRFATT21
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT7, self).__init__(rmio, label,
            0xa800c000, 0x090,
            'PNRFATT7', 'AGC.PNRFATT7', 'read-write',
            u"",
            0x07F1B83F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT22 = RM_Field_AGC_PNRFATT7_LNAMIXRFATT22(self)
        self.zz_fdict['LNAMIXRFATT22'] = self.LNAMIXRFATT22
        self.LNAMIXRFATT23 = RM_Field_AGC_PNRFATT7_LNAMIXRFATT23(self)
        self.zz_fdict['LNAMIXRFATT23'] = self.LNAMIXRFATT23
        self.LNAMIXRFATT24 = RM_Field_AGC_PNRFATT7_LNAMIXRFATT24(self)
        self.zz_fdict['LNAMIXRFATT24'] = self.LNAMIXRFATT24
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATTALT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATTALT, self).__init__(rmio, label,
            0xa800c000, 0x0A4,
            'PNRFATTALT', 'AGC.PNRFATTALT', 'read-write',
            u"",
            0x0000007F, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATTALT = RM_Field_AGC_PNRFATTALT_LNAMIXRFATTALT(self)
        self.zz_fdict['LNAMIXRFATTALT'] = self.LNAMIXRFATTALT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LNAMIXCODE0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LNAMIXCODE0, self).__init__(rmio, label,
            0xa800c000, 0x0A8,
            'LNAMIXCODE0', 'AGC.LNAMIXCODE0', 'read-write',
            u"",
            0x15724BBD, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXSLICE1 = RM_Field_AGC_LNAMIXCODE0_LNAMIXSLICE1(self)
        self.zz_fdict['LNAMIXSLICE1'] = self.LNAMIXSLICE1
        self.LNAMIXSLICE2 = RM_Field_AGC_LNAMIXCODE0_LNAMIXSLICE2(self)
        self.zz_fdict['LNAMIXSLICE2'] = self.LNAMIXSLICE2
        self.LNAMIXSLICE3 = RM_Field_AGC_LNAMIXCODE0_LNAMIXSLICE3(self)
        self.zz_fdict['LNAMIXSLICE3'] = self.LNAMIXSLICE3
        self.LNAMIXSLICE4 = RM_Field_AGC_LNAMIXCODE0_LNAMIXSLICE4(self)
        self.zz_fdict['LNAMIXSLICE4'] = self.LNAMIXSLICE4
        self.LNAMIXSLICE5 = RM_Field_AGC_LNAMIXCODE0_LNAMIXSLICE5(self)
        self.zz_fdict['LNAMIXSLICE5'] = self.LNAMIXSLICE5
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LNAMIXCODE1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LNAMIXCODE1, self).__init__(rmio, label,
            0xa800c000, 0x0AC,
            'LNAMIXCODE1', 'AGC.LNAMIXCODE1', 'read-write',
            u"",
            0x0518A311, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXSLICE6 = RM_Field_AGC_LNAMIXCODE1_LNAMIXSLICE6(self)
        self.zz_fdict['LNAMIXSLICE6'] = self.LNAMIXSLICE6
        self.LNAMIXSLICE7 = RM_Field_AGC_LNAMIXCODE1_LNAMIXSLICE7(self)
        self.zz_fdict['LNAMIXSLICE7'] = self.LNAMIXSLICE7
        self.LNAMIXSLICE8 = RM_Field_AGC_LNAMIXCODE1_LNAMIXSLICE8(self)
        self.zz_fdict['LNAMIXSLICE8'] = self.LNAMIXSLICE8
        self.LNAMIXSLICE9 = RM_Field_AGC_LNAMIXCODE1_LNAMIXSLICE9(self)
        self.zz_fdict['LNAMIXSLICE9'] = self.LNAMIXSLICE9
        self.LNAMIXSLICE10 = RM_Field_AGC_LNAMIXCODE1_LNAMIXSLICE10(self)
        self.zz_fdict['LNAMIXSLICE10'] = self.LNAMIXSLICE10
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PGACODE0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PGACODE0, self).__init__(rmio, label,
            0xa800c000, 0x0B0,
            'PGACODE0', 'AGC.PGACODE0', 'read-write',
            u"",
            0x76543210, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGAGAIN1 = RM_Field_AGC_PGACODE0_PGAGAIN1(self)
        self.zz_fdict['PGAGAIN1'] = self.PGAGAIN1
        self.PGAGAIN2 = RM_Field_AGC_PGACODE0_PGAGAIN2(self)
        self.zz_fdict['PGAGAIN2'] = self.PGAGAIN2
        self.PGAGAIN3 = RM_Field_AGC_PGACODE0_PGAGAIN3(self)
        self.zz_fdict['PGAGAIN3'] = self.PGAGAIN3
        self.PGAGAIN4 = RM_Field_AGC_PGACODE0_PGAGAIN4(self)
        self.zz_fdict['PGAGAIN4'] = self.PGAGAIN4
        self.PGAGAIN5 = RM_Field_AGC_PGACODE0_PGAGAIN5(self)
        self.zz_fdict['PGAGAIN5'] = self.PGAGAIN5
        self.PGAGAIN6 = RM_Field_AGC_PGACODE0_PGAGAIN6(self)
        self.zz_fdict['PGAGAIN6'] = self.PGAGAIN6
        self.PGAGAIN7 = RM_Field_AGC_PGACODE0_PGAGAIN7(self)
        self.zz_fdict['PGAGAIN7'] = self.PGAGAIN7
        self.PGAGAIN8 = RM_Field_AGC_PGACODE0_PGAGAIN8(self)
        self.zz_fdict['PGAGAIN8'] = self.PGAGAIN8
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PGACODE1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PGACODE1, self).__init__(rmio, label,
            0xa800c000, 0x0B4,
            'PGACODE1', 'AGC.PGACODE1', 'read-write',
            u"",
            0x00000A98, 0x00000FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGAGAIN9 = RM_Field_AGC_PGACODE1_PGAGAIN9(self)
        self.zz_fdict['PGAGAIN9'] = self.PGAGAIN9
        self.PGAGAIN10 = RM_Field_AGC_PGACODE1_PGAGAIN10(self)
        self.zz_fdict['PGAGAIN10'] = self.PGAGAIN10
        self.PGAGAIN11 = RM_Field_AGC_PGACODE1_PGAGAIN11(self)
        self.zz_fdict['PGAGAIN11'] = self.PGAGAIN11
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LBT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LBT, self).__init__(rmio, label,
            0xa800c000, 0x0B8,
            'LBT', 'AGC.LBT', 'read-write',
            u"",
            0x00000000, 0x0000007F,
            0x00001000, 0x00002000,
            0x00003000)

        self.CCARSSIPERIOD = RM_Field_AGC_LBT_CCARSSIPERIOD(self)
        self.zz_fdict['CCARSSIPERIOD'] = self.CCARSSIPERIOD
        self.ENCCARSSIPERIOD = RM_Field_AGC_LBT_ENCCARSSIPERIOD(self)
        self.zz_fdict['ENCCARSSIPERIOD'] = self.ENCCARSSIPERIOD
        self.ENCCAGAINREDUCED = RM_Field_AGC_LBT_ENCCAGAINREDUCED(self)
        self.zz_fdict['ENCCAGAINREDUCED'] = self.ENCCAGAINREDUCED
        self.ENCCARSSIMAX = RM_Field_AGC_LBT_ENCCARSSIMAX(self)
        self.zz_fdict['ENCCARSSIMAX'] = self.ENCCARSSIMAX
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_MIRRORIF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_MIRRORIF, self).__init__(rmio, label,
            0xa800c000, 0x0BC,
            'MIRRORIF', 'AGC.MIRRORIF', 'read-write',
            u"",
            0x00000000, 0x0000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIPOSSTEPM = RM_Field_AGC_MIRRORIF_RSSIPOSSTEPM(self)
        self.zz_fdict['RSSIPOSSTEPM'] = self.RSSIPOSSTEPM
        self.RSSINEGSTEPM = RM_Field_AGC_MIRRORIF_RSSINEGSTEPM(self)
        self.zz_fdict['RSSINEGSTEPM'] = self.RSSINEGSTEPM
        self.SHORTRSSIPOSSTEPM = RM_Field_AGC_MIRRORIF_SHORTRSSIPOSSTEPM(self)
        self.zz_fdict['SHORTRSSIPOSSTEPM'] = self.SHORTRSSIPOSSTEPM
        self.IFMIRRORCLEAR = RM_Field_AGC_MIRRORIF_IFMIRRORCLEAR(self)
        self.zz_fdict['IFMIRRORCLEAR'] = self.IFMIRRORCLEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_SEQIF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_SEQIF, self).__init__(rmio, label,
            0xa800c000, 0x0C0,
            'SEQIF', 'AGC.SEQIF', 'read-write',
            u"",
            0x00000000, 0x00007F7D,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIVALID = RM_Field_AGC_SEQIF_RSSIVALID(self)
        self.zz_fdict['RSSIVALID'] = self.RSSIVALID
        self.CCA = RM_Field_AGC_SEQIF_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.RSSIPOSSTEP = RM_Field_AGC_SEQIF_RSSIPOSSTEP(self)
        self.zz_fdict['RSSIPOSSTEP'] = self.RSSIPOSSTEP
        self.RSSINEGSTEP = RM_Field_AGC_SEQIF_RSSINEGSTEP(self)
        self.zz_fdict['RSSINEGSTEP'] = self.RSSINEGSTEP
        self.RSSIDONE = RM_Field_AGC_SEQIF_RSSIDONE(self)
        self.zz_fdict['RSSIDONE'] = self.RSSIDONE
        self.SHORTRSSIPOSSTEP = RM_Field_AGC_SEQIF_SHORTRSSIPOSSTEP(self)
        self.zz_fdict['SHORTRSSIPOSSTEP'] = self.SHORTRSSIPOSSTEP
        self.RFPKDPRDDONE = RM_Field_AGC_SEQIF_RFPKDPRDDONE(self)
        self.zz_fdict['RFPKDPRDDONE'] = self.RFPKDPRDDONE
        self.RFPKDCNTDONE = RM_Field_AGC_SEQIF_RFPKDCNTDONE(self)
        self.zz_fdict['RFPKDCNTDONE'] = self.RFPKDCNTDONE
        self.RSSIHIGH = RM_Field_AGC_SEQIF_RSSIHIGH(self)
        self.zz_fdict['RSSIHIGH'] = self.RSSIHIGH
        self.RSSILOW = RM_Field_AGC_SEQIF_RSSILOW(self)
        self.zz_fdict['RSSILOW'] = self.RSSILOW
        self.CCANODET = RM_Field_AGC_SEQIF_CCANODET(self)
        self.zz_fdict['CCANODET'] = self.CCANODET
        self.GAINBELOWGAINTHD = RM_Field_AGC_SEQIF_GAINBELOWGAINTHD(self)
        self.zz_fdict['GAINBELOWGAINTHD'] = self.GAINBELOWGAINTHD
        self.GAINUPDATEFRZ = RM_Field_AGC_SEQIF_GAINUPDATEFRZ(self)
        self.zz_fdict['GAINUPDATEFRZ'] = self.GAINUPDATEFRZ
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_SEQIEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_SEQIEN, self).__init__(rmio, label,
            0xa800c000, 0x0C4,
            'SEQIEN', 'AGC.SEQIEN', 'read-write',
            u"",
            0x00000000, 0x00007F7D,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIVALID = RM_Field_AGC_SEQIEN_RSSIVALID(self)
        self.zz_fdict['RSSIVALID'] = self.RSSIVALID
        self.CCA = RM_Field_AGC_SEQIEN_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.RSSIPOSSTEP = RM_Field_AGC_SEQIEN_RSSIPOSSTEP(self)
        self.zz_fdict['RSSIPOSSTEP'] = self.RSSIPOSSTEP
        self.RSSINEGSTEP = RM_Field_AGC_SEQIEN_RSSINEGSTEP(self)
        self.zz_fdict['RSSINEGSTEP'] = self.RSSINEGSTEP
        self.RSSIDONE = RM_Field_AGC_SEQIEN_RSSIDONE(self)
        self.zz_fdict['RSSIDONE'] = self.RSSIDONE
        self.SHORTRSSIPOSSTEP = RM_Field_AGC_SEQIEN_SHORTRSSIPOSSTEP(self)
        self.zz_fdict['SHORTRSSIPOSSTEP'] = self.SHORTRSSIPOSSTEP
        self.RFPKDPRDDONE = RM_Field_AGC_SEQIEN_RFPKDPRDDONE(self)
        self.zz_fdict['RFPKDPRDDONE'] = self.RFPKDPRDDONE
        self.RFPKDCNTDONE = RM_Field_AGC_SEQIEN_RFPKDCNTDONE(self)
        self.zz_fdict['RFPKDCNTDONE'] = self.RFPKDCNTDONE
        self.RSSIHIGH = RM_Field_AGC_SEQIEN_RSSIHIGH(self)
        self.zz_fdict['RSSIHIGH'] = self.RSSIHIGH
        self.RSSILOW = RM_Field_AGC_SEQIEN_RSSILOW(self)
        self.zz_fdict['RSSILOW'] = self.RSSILOW
        self.CCANODET = RM_Field_AGC_SEQIEN_CCANODET(self)
        self.zz_fdict['CCANODET'] = self.CCANODET
        self.GAINBELOWGAINTHD = RM_Field_AGC_SEQIEN_GAINBELOWGAINTHD(self)
        self.zz_fdict['GAINBELOWGAINTHD'] = self.GAINBELOWGAINTHD
        self.GAINUPDATEFRZ = RM_Field_AGC_SEQIEN_GAINUPDATEFRZ(self)
        self.zz_fdict['GAINUPDATEFRZ'] = self.GAINUPDATEFRZ
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RSSIABSTHR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSIABSTHR, self).__init__(rmio, label,
            0xa800c000, 0x0C8,
            'RSSIABSTHR', 'AGC.RSSIABSTHR', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RSSIHIGHTHRSH = RM_Field_AGC_RSSIABSTHR_RSSIHIGHTHRSH(self)
        self.zz_fdict['RSSIHIGHTHRSH'] = self.RSSIHIGHTHRSH
        self.RSSILOWTHRSH = RM_Field_AGC_RSSIABSTHR_RSSILOWTHRSH(self)
        self.zz_fdict['RSSILOWTHRSH'] = self.RSSILOWTHRSH
        self.SIRSSIHIGHTHR = RM_Field_AGC_RSSIABSTHR_SIRSSIHIGHTHR(self)
        self.zz_fdict['SIRSSIHIGHTHR'] = self.SIRSSIHIGHTHR
        self.SIRSSINEGSTEPTHR = RM_Field_AGC_RSSIABSTHR_SIRSSINEGSTEPTHR(self)
        self.zz_fdict['SIRSSINEGSTEPTHR'] = self.SIRSSINEGSTEPTHR
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LNABOOST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LNABOOST, self).__init__(rmio, label,
            0xa800c000, 0x0CC,
            'LNABOOST', 'AGC.LNABOOST', 'read-write',
            u"",
            0x000001FE, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BOOSTLNA = RM_Field_AGC_LNABOOST_BOOSTLNA(self)
        self.zz_fdict['BOOSTLNA'] = self.BOOSTLNA
        self.LNABWADJ = RM_Field_AGC_LNABOOST_LNABWADJ(self)
        self.zz_fdict['LNABWADJ'] = self.LNABWADJ
        self.LNABWADJBOOST = RM_Field_AGC_LNABOOST_LNABWADJBOOST(self)
        self.zz_fdict['LNABWADJBOOST'] = self.LNABWADJBOOST
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_ANTDIV(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_ANTDIV, self).__init__(rmio, label,
            0xa800c000, 0x0D0,
            'ANTDIV', 'AGC.ANTDIV', 'read-write',
            u"",
            0x00000000, 0x000007FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.GAINMODE = RM_Field_AGC_ANTDIV_GAINMODE(self)
        self.zz_fdict['GAINMODE'] = self.GAINMODE
        self.DEBOUNCECNTTHD = RM_Field_AGC_ANTDIV_DEBOUNCECNTTHD(self)
        self.zz_fdict['DEBOUNCECNTTHD'] = self.DEBOUNCECNTTHD
        self.DISRSSIANTDIVFIX = RM_Field_AGC_ANTDIV_DISRSSIANTDIVFIX(self)
        self.zz_fdict['DISRSSIANTDIVFIX'] = self.DISRSSIANTDIVFIX
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_DUALRFPKDTHD0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_DUALRFPKDTHD0, self).__init__(rmio, label,
            0xa800c000, 0x0D4,
            'DUALRFPKDTHD0', 'AGC.DUALRFPKDTHD0', 'read-write',
            u"",
            0x000A0001, 0x0FFF0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFPKDLOWTHD0 = RM_Field_AGC_DUALRFPKDTHD0_RFPKDLOWTHD0(self)
        self.zz_fdict['RFPKDLOWTHD0'] = self.RFPKDLOWTHD0
        self.RFPKDLOWTHD1 = RM_Field_AGC_DUALRFPKDTHD0_RFPKDLOWTHD1(self)
        self.zz_fdict['RFPKDLOWTHD1'] = self.RFPKDLOWTHD1
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_DUALRFPKDTHD1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_DUALRFPKDTHD1, self).__init__(rmio, label,
            0xa800c000, 0x0D8,
            'DUALRFPKDTHD1', 'AGC.DUALRFPKDTHD1', 'read-write',
            u"",
            0x00280001, 0x0FFF0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFPKDHITHD0 = RM_Field_AGC_DUALRFPKDTHD1_RFPKDHITHD0(self)
        self.zz_fdict['RFPKDHITHD0'] = self.RFPKDHITHD0
        self.RFPKDHITHD1 = RM_Field_AGC_DUALRFPKDTHD1_RFPKDHITHD1(self)
        self.zz_fdict['RFPKDHITHD1'] = self.RFPKDHITHD1
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_SPARE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_SPARE, self).__init__(rmio, label,
            0xa800c000, 0x0DC,
            'SPARE', 'AGC.SPARE', 'read-write',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SPAREREG = RM_Field_AGC_SPARE_SPAREREG(self)
        self.zz_fdict['SPAREREG'] = self.SPAREREG
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT0, self).__init__(rmio, label,
            0xa800c000, 0x0E0,
            'PNRFFILT0', 'AGC.PNRFFILT0', 'read-write',
            u"",
            0x00200400, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT1 = RM_Field_AGC_PNRFFILT0_LNAMIXRFATT1(self)
        self.zz_fdict['LNAMIXRFATT1'] = self.LNAMIXRFATT1
        self.LNAMIXRFATT2 = RM_Field_AGC_PNRFFILT0_LNAMIXRFATT2(self)
        self.zz_fdict['LNAMIXRFATT2'] = self.LNAMIXRFATT2
        self.LNAMIXRFATT3 = RM_Field_AGC_PNRFFILT0_LNAMIXRFATT3(self)
        self.zz_fdict['LNAMIXRFATT3'] = self.LNAMIXRFATT3
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT1, self).__init__(rmio, label,
            0xa800c000, 0x0E4,
            'PNRFFILT1', 'AGC.PNRFFILT1', 'read-write',
            u"",
            0x00801804, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT4 = RM_Field_AGC_PNRFFILT1_LNAMIXRFATT4(self)
        self.zz_fdict['LNAMIXRFATT4'] = self.LNAMIXRFATT4
        self.LNAMIXRFATT5 = RM_Field_AGC_PNRFFILT1_LNAMIXRFATT5(self)
        self.zz_fdict['LNAMIXRFATT5'] = self.LNAMIXRFATT5
        self.LNAMIXRFATT6 = RM_Field_AGC_PNRFFILT1_LNAMIXRFATT6(self)
        self.zz_fdict['LNAMIXRFATT6'] = self.LNAMIXRFATT6
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT2, self).__init__(rmio, label,
            0xa800c000, 0x0E8,
            'PNRFFILT2', 'AGC.PNRFFILT2', 'read-write',
            u"",
            0x01203C0B, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT7 = RM_Field_AGC_PNRFFILT2_LNAMIXRFATT7(self)
        self.zz_fdict['LNAMIXRFATT7'] = self.LNAMIXRFATT7
        self.LNAMIXRFATT8 = RM_Field_AGC_PNRFFILT2_LNAMIXRFATT8(self)
        self.zz_fdict['LNAMIXRFATT8'] = self.LNAMIXRFATT8
        self.LNAMIXRFATT9 = RM_Field_AGC_PNRFFILT2_LNAMIXRFATT9(self)
        self.zz_fdict['LNAMIXRFATT9'] = self.LNAMIXRFATT9
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT3, self).__init__(rmio, label,
            0xa800c000, 0x0EC,
            'PNRFFILT3', 'AGC.PNRFFILT3', 'read-write',
            u"",
            0x02107C18, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT10 = RM_Field_AGC_PNRFFILT3_LNAMIXRFATT10(self)
        self.zz_fdict['LNAMIXRFATT10'] = self.LNAMIXRFATT10
        self.LNAMIXRFATT11 = RM_Field_AGC_PNRFFILT3_LNAMIXRFATT11(self)
        self.zz_fdict['LNAMIXRFATT11'] = self.LNAMIXRFATT11
        self.LNAMIXRFATT12 = RM_Field_AGC_PNRFFILT3_LNAMIXRFATT12(self)
        self.zz_fdict['LNAMIXRFATT12'] = self.LNAMIXRFATT12
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT4, self).__init__(rmio, label,
            0xa800c000, 0x0F0,
            'PNRFFILT4', 'AGC.PNRFFILT4', 'read-write',
            u"",
            0x06E0FC2F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT13 = RM_Field_AGC_PNRFFILT4_LNAMIXRFATT13(self)
        self.zz_fdict['LNAMIXRFATT13'] = self.LNAMIXRFATT13
        self.LNAMIXRFATT14 = RM_Field_AGC_PNRFFILT4_LNAMIXRFATT14(self)
        self.zz_fdict['LNAMIXRFATT14'] = self.LNAMIXRFATT14
        self.LNAMIXRFATT15 = RM_Field_AGC_PNRFFILT4_LNAMIXRFATT15(self)
        self.zz_fdict['LNAMIXRFATT15'] = self.LNAMIXRFATT15
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT5, self).__init__(rmio, label,
            0xa800c000, 0x0F4,
            'PNRFFILT5', 'AGC.PNRFFILT5', 'read-write',
            u"",
            0x0180480F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT16 = RM_Field_AGC_PNRFFILT5_LNAMIXRFATT16(self)
        self.zz_fdict['LNAMIXRFATT16'] = self.LNAMIXRFATT16
        self.LNAMIXRFATT17 = RM_Field_AGC_PNRFFILT5_LNAMIXRFATT17(self)
        self.zz_fdict['LNAMIXRFATT17'] = self.LNAMIXRFATT17
        self.LNAMIXRFATT18 = RM_Field_AGC_PNRFFILT5_LNAMIXRFATT18(self)
        self.zz_fdict['LNAMIXRFATT18'] = self.LNAMIXRFATT18
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT6, self).__init__(rmio, label,
            0xa800c000, 0x0F8,
            'PNRFFILT6', 'AGC.PNRFFILT6', 'read-write',
            u"",
            0x02F0841F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT19 = RM_Field_AGC_PNRFFILT6_LNAMIXRFATT19(self)
        self.zz_fdict['LNAMIXRFATT19'] = self.LNAMIXRFATT19
        self.LNAMIXRFATT20 = RM_Field_AGC_PNRFFILT6_LNAMIXRFATT20(self)
        self.zz_fdict['LNAMIXRFATT20'] = self.LNAMIXRFATT20
        self.LNAMIXRFATT21 = RM_Field_AGC_PNRFFILT6_LNAMIXRFATT21(self)
        self.zz_fdict['LNAMIXRFATT21'] = self.LNAMIXRFATT21
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFFILT7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFFILT7, self).__init__(rmio, label,
            0xa800c000, 0x0FC,
            'PNRFFILT7', 'AGC.PNRFFILT7', 'read-write',
            u"",
            0x07F1B83F, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT22 = RM_Field_AGC_PNRFFILT7_LNAMIXRFATT22(self)
        self.zz_fdict['LNAMIXRFATT22'] = self.LNAMIXRFATT22
        self.LNAMIXRFATT23 = RM_Field_AGC_PNRFFILT7_LNAMIXRFATT23(self)
        self.zz_fdict['LNAMIXRFATT23'] = self.LNAMIXRFATT23
        self.LNAMIXRFATT24 = RM_Field_AGC_PNRFFILT7_LNAMIXRFATT24(self)
        self.zz_fdict['LNAMIXRFATT24'] = self.LNAMIXRFATT24
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT0, self).__init__(rmio, label,
            0xa800c000, 0x100,
            'FENOTCHATT0', 'AGC.FENOTCHATT0', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL1 = RM_Field_AGC_FENOTCHATT0_FENOTCHATTNSEL1(self)
        self.zz_fdict['FENOTCHATTNSEL1'] = self.FENOTCHATTNSEL1
        self.FENOTCHCAPCRSE1 = RM_Field_AGC_FENOTCHATT0_FENOTCHCAPCRSE1(self)
        self.zz_fdict['FENOTCHCAPCRSE1'] = self.FENOTCHCAPCRSE1
        self.FENOTCHCAPFINE1 = RM_Field_AGC_FENOTCHATT0_FENOTCHCAPFINE1(self)
        self.zz_fdict['FENOTCHCAPFINE1'] = self.FENOTCHCAPFINE1
        self.FENOTCHRATTNEN1 = RM_Field_AGC_FENOTCHATT0_FENOTCHRATTNEN1(self)
        self.zz_fdict['FENOTCHRATTNEN1'] = self.FENOTCHRATTNEN1
        self.FENOTCHEN1 = RM_Field_AGC_FENOTCHATT0_FENOTCHEN1(self)
        self.zz_fdict['FENOTCHEN1'] = self.FENOTCHEN1
        self.FENOTCHATTNSEL2 = RM_Field_AGC_FENOTCHATT0_FENOTCHATTNSEL2(self)
        self.zz_fdict['FENOTCHATTNSEL2'] = self.FENOTCHATTNSEL2
        self.FENOTCHCAPCRSE2 = RM_Field_AGC_FENOTCHATT0_FENOTCHCAPCRSE2(self)
        self.zz_fdict['FENOTCHCAPCRSE2'] = self.FENOTCHCAPCRSE2
        self.FENOTCHCAPFINE2 = RM_Field_AGC_FENOTCHATT0_FENOTCHCAPFINE2(self)
        self.zz_fdict['FENOTCHCAPFINE2'] = self.FENOTCHCAPFINE2
        self.FENOTCHRATTNEN2 = RM_Field_AGC_FENOTCHATT0_FENOTCHRATTNEN2(self)
        self.zz_fdict['FENOTCHRATTNEN2'] = self.FENOTCHRATTNEN2
        self.FENOTCHEN2 = RM_Field_AGC_FENOTCHATT0_FENOTCHEN2(self)
        self.zz_fdict['FENOTCHEN2'] = self.FENOTCHEN2
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT1, self).__init__(rmio, label,
            0xa800c000, 0x104,
            'FENOTCHATT1', 'AGC.FENOTCHATT1', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL3 = RM_Field_AGC_FENOTCHATT1_FENOTCHATTNSEL3(self)
        self.zz_fdict['FENOTCHATTNSEL3'] = self.FENOTCHATTNSEL3
        self.FENOTCHCAPCRSE3 = RM_Field_AGC_FENOTCHATT1_FENOTCHCAPCRSE3(self)
        self.zz_fdict['FENOTCHCAPCRSE3'] = self.FENOTCHCAPCRSE3
        self.FENOTCHCAPFINE3 = RM_Field_AGC_FENOTCHATT1_FENOTCHCAPFINE3(self)
        self.zz_fdict['FENOTCHCAPFINE3'] = self.FENOTCHCAPFINE3
        self.FENOTCHRATTNEN3 = RM_Field_AGC_FENOTCHATT1_FENOTCHRATTNEN3(self)
        self.zz_fdict['FENOTCHRATTNEN3'] = self.FENOTCHRATTNEN3
        self.FENOTCHEN3 = RM_Field_AGC_FENOTCHATT1_FENOTCHEN3(self)
        self.zz_fdict['FENOTCHEN3'] = self.FENOTCHEN3
        self.FENOTCHATTNSEL4 = RM_Field_AGC_FENOTCHATT1_FENOTCHATTNSEL4(self)
        self.zz_fdict['FENOTCHATTNSEL4'] = self.FENOTCHATTNSEL4
        self.FENOTCHCAPCRSE4 = RM_Field_AGC_FENOTCHATT1_FENOTCHCAPCRSE4(self)
        self.zz_fdict['FENOTCHCAPCRSE4'] = self.FENOTCHCAPCRSE4
        self.FENOTCHCAPFINE4 = RM_Field_AGC_FENOTCHATT1_FENOTCHCAPFINE4(self)
        self.zz_fdict['FENOTCHCAPFINE4'] = self.FENOTCHCAPFINE4
        self.FENOTCHRATTNEN4 = RM_Field_AGC_FENOTCHATT1_FENOTCHRATTNEN4(self)
        self.zz_fdict['FENOTCHRATTNEN4'] = self.FENOTCHRATTNEN4
        self.FENOTCHEN4 = RM_Field_AGC_FENOTCHATT1_FENOTCHEN4(self)
        self.zz_fdict['FENOTCHEN4'] = self.FENOTCHEN4
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT2, self).__init__(rmio, label,
            0xa800c000, 0x108,
            'FENOTCHATT2', 'AGC.FENOTCHATT2', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL5 = RM_Field_AGC_FENOTCHATT2_FENOTCHATTNSEL5(self)
        self.zz_fdict['FENOTCHATTNSEL5'] = self.FENOTCHATTNSEL5
        self.FENOTCHCAPCRSE5 = RM_Field_AGC_FENOTCHATT2_FENOTCHCAPCRSE5(self)
        self.zz_fdict['FENOTCHCAPCRSE5'] = self.FENOTCHCAPCRSE5
        self.FENOTCHCAPFINE5 = RM_Field_AGC_FENOTCHATT2_FENOTCHCAPFINE5(self)
        self.zz_fdict['FENOTCHCAPFINE5'] = self.FENOTCHCAPFINE5
        self.FENOTCHRATTNEN5 = RM_Field_AGC_FENOTCHATT2_FENOTCHRATTNEN5(self)
        self.zz_fdict['FENOTCHRATTNEN5'] = self.FENOTCHRATTNEN5
        self.FENOTCHEN5 = RM_Field_AGC_FENOTCHATT2_FENOTCHEN5(self)
        self.zz_fdict['FENOTCHEN5'] = self.FENOTCHEN5
        self.FENOTCHATTNSEL6 = RM_Field_AGC_FENOTCHATT2_FENOTCHATTNSEL6(self)
        self.zz_fdict['FENOTCHATTNSEL6'] = self.FENOTCHATTNSEL6
        self.FENOTCHCAPCRSE6 = RM_Field_AGC_FENOTCHATT2_FENOTCHCAPCRSE6(self)
        self.zz_fdict['FENOTCHCAPCRSE6'] = self.FENOTCHCAPCRSE6
        self.FENOTCHCAPFINE6 = RM_Field_AGC_FENOTCHATT2_FENOTCHCAPFINE6(self)
        self.zz_fdict['FENOTCHCAPFINE6'] = self.FENOTCHCAPFINE6
        self.FENOTCHRATTNEN6 = RM_Field_AGC_FENOTCHATT2_FENOTCHRATTNEN6(self)
        self.zz_fdict['FENOTCHRATTNEN6'] = self.FENOTCHRATTNEN6
        self.FENOTCHEN6 = RM_Field_AGC_FENOTCHATT2_FENOTCHEN6(self)
        self.zz_fdict['FENOTCHEN6'] = self.FENOTCHEN6
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT3, self).__init__(rmio, label,
            0xa800c000, 0x10C,
            'FENOTCHATT3', 'AGC.FENOTCHATT3', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL7 = RM_Field_AGC_FENOTCHATT3_FENOTCHATTNSEL7(self)
        self.zz_fdict['FENOTCHATTNSEL7'] = self.FENOTCHATTNSEL7
        self.FENOTCHCAPCRSE7 = RM_Field_AGC_FENOTCHATT3_FENOTCHCAPCRSE7(self)
        self.zz_fdict['FENOTCHCAPCRSE7'] = self.FENOTCHCAPCRSE7
        self.FENOTCHCAPFINE7 = RM_Field_AGC_FENOTCHATT3_FENOTCHCAPFINE7(self)
        self.zz_fdict['FENOTCHCAPFINE7'] = self.FENOTCHCAPFINE7
        self.FENOTCHRATTNEN7 = RM_Field_AGC_FENOTCHATT3_FENOTCHRATTNEN7(self)
        self.zz_fdict['FENOTCHRATTNEN7'] = self.FENOTCHRATTNEN7
        self.FENOTCHEN7 = RM_Field_AGC_FENOTCHATT3_FENOTCHEN7(self)
        self.zz_fdict['FENOTCHEN7'] = self.FENOTCHEN7
        self.FENOTCHATTNSEL8 = RM_Field_AGC_FENOTCHATT3_FENOTCHATTNSEL8(self)
        self.zz_fdict['FENOTCHATTNSEL8'] = self.FENOTCHATTNSEL8
        self.FENOTCHCAPCRSE8 = RM_Field_AGC_FENOTCHATT3_FENOTCHCAPCRSE8(self)
        self.zz_fdict['FENOTCHCAPCRSE8'] = self.FENOTCHCAPCRSE8
        self.FENOTCHCAPFINE8 = RM_Field_AGC_FENOTCHATT3_FENOTCHCAPFINE8(self)
        self.zz_fdict['FENOTCHCAPFINE8'] = self.FENOTCHCAPFINE8
        self.FENOTCHRATTNEN8 = RM_Field_AGC_FENOTCHATT3_FENOTCHRATTNEN8(self)
        self.zz_fdict['FENOTCHRATTNEN8'] = self.FENOTCHRATTNEN8
        self.FENOTCHEN8 = RM_Field_AGC_FENOTCHATT3_FENOTCHEN8(self)
        self.zz_fdict['FENOTCHEN8'] = self.FENOTCHEN8
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT4, self).__init__(rmio, label,
            0xa800c000, 0x110,
            'FENOTCHATT4', 'AGC.FENOTCHATT4', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL9 = RM_Field_AGC_FENOTCHATT4_FENOTCHATTNSEL9(self)
        self.zz_fdict['FENOTCHATTNSEL9'] = self.FENOTCHATTNSEL9
        self.FENOTCHCAPCRSE9 = RM_Field_AGC_FENOTCHATT4_FENOTCHCAPCRSE9(self)
        self.zz_fdict['FENOTCHCAPCRSE9'] = self.FENOTCHCAPCRSE9
        self.FENOTCHCAPFINE9 = RM_Field_AGC_FENOTCHATT4_FENOTCHCAPFINE9(self)
        self.zz_fdict['FENOTCHCAPFINE9'] = self.FENOTCHCAPFINE9
        self.FENOTCHRATTNEN9 = RM_Field_AGC_FENOTCHATT4_FENOTCHRATTNEN9(self)
        self.zz_fdict['FENOTCHRATTNEN9'] = self.FENOTCHRATTNEN9
        self.FENOTCHEN9 = RM_Field_AGC_FENOTCHATT4_FENOTCHEN9(self)
        self.zz_fdict['FENOTCHEN9'] = self.FENOTCHEN9
        self.FENOTCHATTNSEL10 = RM_Field_AGC_FENOTCHATT4_FENOTCHATTNSEL10(self)
        self.zz_fdict['FENOTCHATTNSEL10'] = self.FENOTCHATTNSEL10
        self.FENOTCHCAPCRSE10 = RM_Field_AGC_FENOTCHATT4_FENOTCHCAPCRSE10(self)
        self.zz_fdict['FENOTCHCAPCRSE10'] = self.FENOTCHCAPCRSE10
        self.FENOTCHCAPFINE10 = RM_Field_AGC_FENOTCHATT4_FENOTCHCAPFINE10(self)
        self.zz_fdict['FENOTCHCAPFINE10'] = self.FENOTCHCAPFINE10
        self.FENOTCHRATTNEN10 = RM_Field_AGC_FENOTCHATT4_FENOTCHRATTNEN10(self)
        self.zz_fdict['FENOTCHRATTNEN10'] = self.FENOTCHRATTNEN10
        self.FENOTCHEN10 = RM_Field_AGC_FENOTCHATT4_FENOTCHEN10(self)
        self.zz_fdict['FENOTCHEN10'] = self.FENOTCHEN10
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT5, self).__init__(rmio, label,
            0xa800c000, 0x114,
            'FENOTCHATT5', 'AGC.FENOTCHATT5', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL11 = RM_Field_AGC_FENOTCHATT5_FENOTCHATTNSEL11(self)
        self.zz_fdict['FENOTCHATTNSEL11'] = self.FENOTCHATTNSEL11
        self.FENOTCHCAPCRSE11 = RM_Field_AGC_FENOTCHATT5_FENOTCHCAPCRSE11(self)
        self.zz_fdict['FENOTCHCAPCRSE11'] = self.FENOTCHCAPCRSE11
        self.FENOTCHCAPFINE11 = RM_Field_AGC_FENOTCHATT5_FENOTCHCAPFINE11(self)
        self.zz_fdict['FENOTCHCAPFINE11'] = self.FENOTCHCAPFINE11
        self.FENOTCHRATTNEN11 = RM_Field_AGC_FENOTCHATT5_FENOTCHRATTNEN11(self)
        self.zz_fdict['FENOTCHRATTNEN11'] = self.FENOTCHRATTNEN11
        self.FENOTCHEN11 = RM_Field_AGC_FENOTCHATT5_FENOTCHEN11(self)
        self.zz_fdict['FENOTCHEN11'] = self.FENOTCHEN11
        self.FENOTCHATTNSEL12 = RM_Field_AGC_FENOTCHATT5_FENOTCHATTNSEL12(self)
        self.zz_fdict['FENOTCHATTNSEL12'] = self.FENOTCHATTNSEL12
        self.FENOTCHCAPCRSE12 = RM_Field_AGC_FENOTCHATT5_FENOTCHCAPCRSE12(self)
        self.zz_fdict['FENOTCHCAPCRSE12'] = self.FENOTCHCAPCRSE12
        self.FENOTCHCAPFINE12 = RM_Field_AGC_FENOTCHATT5_FENOTCHCAPFINE12(self)
        self.zz_fdict['FENOTCHCAPFINE12'] = self.FENOTCHCAPFINE12
        self.FENOTCHRATTNEN12 = RM_Field_AGC_FENOTCHATT5_FENOTCHRATTNEN12(self)
        self.zz_fdict['FENOTCHRATTNEN12'] = self.FENOTCHRATTNEN12
        self.FENOTCHEN12 = RM_Field_AGC_FENOTCHATT5_FENOTCHEN12(self)
        self.zz_fdict['FENOTCHEN12'] = self.FENOTCHEN12
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT6, self).__init__(rmio, label,
            0xa800c000, 0x118,
            'FENOTCHATT6', 'AGC.FENOTCHATT6', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL13 = RM_Field_AGC_FENOTCHATT6_FENOTCHATTNSEL13(self)
        self.zz_fdict['FENOTCHATTNSEL13'] = self.FENOTCHATTNSEL13
        self.FENOTCHCAPCRSE13 = RM_Field_AGC_FENOTCHATT6_FENOTCHCAPCRSE13(self)
        self.zz_fdict['FENOTCHCAPCRSE13'] = self.FENOTCHCAPCRSE13
        self.FENOTCHCAPFINE13 = RM_Field_AGC_FENOTCHATT6_FENOTCHCAPFINE13(self)
        self.zz_fdict['FENOTCHCAPFINE13'] = self.FENOTCHCAPFINE13
        self.FENOTCHRATTNEN13 = RM_Field_AGC_FENOTCHATT6_FENOTCHRATTNEN13(self)
        self.zz_fdict['FENOTCHRATTNEN13'] = self.FENOTCHRATTNEN13
        self.FENOTCHEN13 = RM_Field_AGC_FENOTCHATT6_FENOTCHEN13(self)
        self.zz_fdict['FENOTCHEN13'] = self.FENOTCHEN13
        self.FENOTCHATTNSEL14 = RM_Field_AGC_FENOTCHATT6_FENOTCHATTNSEL14(self)
        self.zz_fdict['FENOTCHATTNSEL14'] = self.FENOTCHATTNSEL14
        self.FENOTCHCAPCRSE14 = RM_Field_AGC_FENOTCHATT6_FENOTCHCAPCRSE14(self)
        self.zz_fdict['FENOTCHCAPCRSE14'] = self.FENOTCHCAPCRSE14
        self.FENOTCHCAPFINE14 = RM_Field_AGC_FENOTCHATT6_FENOTCHCAPFINE14(self)
        self.zz_fdict['FENOTCHCAPFINE14'] = self.FENOTCHCAPFINE14
        self.FENOTCHRATTNEN14 = RM_Field_AGC_FENOTCHATT6_FENOTCHRATTNEN14(self)
        self.zz_fdict['FENOTCHRATTNEN14'] = self.FENOTCHRATTNEN14
        self.FENOTCHEN14 = RM_Field_AGC_FENOTCHATT6_FENOTCHEN14(self)
        self.zz_fdict['FENOTCHEN14'] = self.FENOTCHEN14
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT7, self).__init__(rmio, label,
            0xa800c000, 0x11C,
            'FENOTCHATT7', 'AGC.FENOTCHATT7', 'read-write',
            u"",
            0x20080000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL15 = RM_Field_AGC_FENOTCHATT7_FENOTCHATTNSEL15(self)
        self.zz_fdict['FENOTCHATTNSEL15'] = self.FENOTCHATTNSEL15
        self.FENOTCHCAPCRSE15 = RM_Field_AGC_FENOTCHATT7_FENOTCHCAPCRSE15(self)
        self.zz_fdict['FENOTCHCAPCRSE15'] = self.FENOTCHCAPCRSE15
        self.FENOTCHCAPFINE15 = RM_Field_AGC_FENOTCHATT7_FENOTCHCAPFINE15(self)
        self.zz_fdict['FENOTCHCAPFINE15'] = self.FENOTCHCAPFINE15
        self.FENOTCHRATTNEN15 = RM_Field_AGC_FENOTCHATT7_FENOTCHRATTNEN15(self)
        self.zz_fdict['FENOTCHRATTNEN15'] = self.FENOTCHRATTNEN15
        self.FENOTCHEN15 = RM_Field_AGC_FENOTCHATT7_FENOTCHEN15(self)
        self.zz_fdict['FENOTCHEN15'] = self.FENOTCHEN15
        self.FENOTCHATTNSEL16 = RM_Field_AGC_FENOTCHATT7_FENOTCHATTNSEL16(self)
        self.zz_fdict['FENOTCHATTNSEL16'] = self.FENOTCHATTNSEL16
        self.FENOTCHCAPCRSE16 = RM_Field_AGC_FENOTCHATT7_FENOTCHCAPCRSE16(self)
        self.zz_fdict['FENOTCHCAPCRSE16'] = self.FENOTCHCAPCRSE16
        self.FENOTCHCAPFINE16 = RM_Field_AGC_FENOTCHATT7_FENOTCHCAPFINE16(self)
        self.zz_fdict['FENOTCHCAPFINE16'] = self.FENOTCHCAPFINE16
        self.FENOTCHRATTNEN16 = RM_Field_AGC_FENOTCHATT7_FENOTCHRATTNEN16(self)
        self.zz_fdict['FENOTCHRATTNEN16'] = self.FENOTCHRATTNEN16
        self.FENOTCHEN16 = RM_Field_AGC_FENOTCHATT7_FENOTCHEN16(self)
        self.zz_fdict['FENOTCHEN16'] = self.FENOTCHEN16
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT8(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT8, self).__init__(rmio, label,
            0xa800c000, 0x120,
            'FENOTCHATT8', 'AGC.FENOTCHATT8', 'read-write',
            u"",
            0x200B200A, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL17 = RM_Field_AGC_FENOTCHATT8_FENOTCHATTNSEL17(self)
        self.zz_fdict['FENOTCHATTNSEL17'] = self.FENOTCHATTNSEL17
        self.FENOTCHCAPCRSE17 = RM_Field_AGC_FENOTCHATT8_FENOTCHCAPCRSE17(self)
        self.zz_fdict['FENOTCHCAPCRSE17'] = self.FENOTCHCAPCRSE17
        self.FENOTCHCAPFINE17 = RM_Field_AGC_FENOTCHATT8_FENOTCHCAPFINE17(self)
        self.zz_fdict['FENOTCHCAPFINE17'] = self.FENOTCHCAPFINE17
        self.FENOTCHRATTNEN17 = RM_Field_AGC_FENOTCHATT8_FENOTCHRATTNEN17(self)
        self.zz_fdict['FENOTCHRATTNEN17'] = self.FENOTCHRATTNEN17
        self.FENOTCHEN17 = RM_Field_AGC_FENOTCHATT8_FENOTCHEN17(self)
        self.zz_fdict['FENOTCHEN17'] = self.FENOTCHEN17
        self.FENOTCHATTNSEL18 = RM_Field_AGC_FENOTCHATT8_FENOTCHATTNSEL18(self)
        self.zz_fdict['FENOTCHATTNSEL18'] = self.FENOTCHATTNSEL18
        self.FENOTCHCAPCRSE18 = RM_Field_AGC_FENOTCHATT8_FENOTCHCAPCRSE18(self)
        self.zz_fdict['FENOTCHCAPCRSE18'] = self.FENOTCHCAPCRSE18
        self.FENOTCHCAPFINE18 = RM_Field_AGC_FENOTCHATT8_FENOTCHCAPFINE18(self)
        self.zz_fdict['FENOTCHCAPFINE18'] = self.FENOTCHCAPFINE18
        self.FENOTCHRATTNEN18 = RM_Field_AGC_FENOTCHATT8_FENOTCHRATTNEN18(self)
        self.zz_fdict['FENOTCHRATTNEN18'] = self.FENOTCHRATTNEN18
        self.FENOTCHEN18 = RM_Field_AGC_FENOTCHATT8_FENOTCHEN18(self)
        self.zz_fdict['FENOTCHEN18'] = self.FENOTCHEN18
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT9(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT9, self).__init__(rmio, label,
            0xa800c000, 0x124,
            'FENOTCHATT9', 'AGC.FENOTCHATT9', 'read-write',
            u"",
            0x200B200B, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL19 = RM_Field_AGC_FENOTCHATT9_FENOTCHATTNSEL19(self)
        self.zz_fdict['FENOTCHATTNSEL19'] = self.FENOTCHATTNSEL19
        self.FENOTCHCAPCRSE19 = RM_Field_AGC_FENOTCHATT9_FENOTCHCAPCRSE19(self)
        self.zz_fdict['FENOTCHCAPCRSE19'] = self.FENOTCHCAPCRSE19
        self.FENOTCHCAPFINE19 = RM_Field_AGC_FENOTCHATT9_FENOTCHCAPFINE19(self)
        self.zz_fdict['FENOTCHCAPFINE19'] = self.FENOTCHCAPFINE19
        self.FENOTCHRATTNEN19 = RM_Field_AGC_FENOTCHATT9_FENOTCHRATTNEN19(self)
        self.zz_fdict['FENOTCHRATTNEN19'] = self.FENOTCHRATTNEN19
        self.FENOTCHEN19 = RM_Field_AGC_FENOTCHATT9_FENOTCHEN19(self)
        self.zz_fdict['FENOTCHEN19'] = self.FENOTCHEN19
        self.FENOTCHATTNSEL20 = RM_Field_AGC_FENOTCHATT9_FENOTCHATTNSEL20(self)
        self.zz_fdict['FENOTCHATTNSEL20'] = self.FENOTCHATTNSEL20
        self.FENOTCHCAPCRSE20 = RM_Field_AGC_FENOTCHATT9_FENOTCHCAPCRSE20(self)
        self.zz_fdict['FENOTCHCAPCRSE20'] = self.FENOTCHCAPCRSE20
        self.FENOTCHCAPFINE20 = RM_Field_AGC_FENOTCHATT9_FENOTCHCAPFINE20(self)
        self.zz_fdict['FENOTCHCAPFINE20'] = self.FENOTCHCAPFINE20
        self.FENOTCHRATTNEN20 = RM_Field_AGC_FENOTCHATT9_FENOTCHRATTNEN20(self)
        self.zz_fdict['FENOTCHRATTNEN20'] = self.FENOTCHRATTNEN20
        self.FENOTCHEN20 = RM_Field_AGC_FENOTCHATT9_FENOTCHEN20(self)
        self.zz_fdict['FENOTCHEN20'] = self.FENOTCHEN20
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT10(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT10, self).__init__(rmio, label,
            0xa800c000, 0x128,
            'FENOTCHATT10', 'AGC.FENOTCHATT10', 'read-write',
            u"",
            0x200B200B, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL21 = RM_Field_AGC_FENOTCHATT10_FENOTCHATTNSEL21(self)
        self.zz_fdict['FENOTCHATTNSEL21'] = self.FENOTCHATTNSEL21
        self.FENOTCHCAPCRSE21 = RM_Field_AGC_FENOTCHATT10_FENOTCHCAPCRSE21(self)
        self.zz_fdict['FENOTCHCAPCRSE21'] = self.FENOTCHCAPCRSE21
        self.FENOTCHCAPFINE21 = RM_Field_AGC_FENOTCHATT10_FENOTCHCAPFINE21(self)
        self.zz_fdict['FENOTCHCAPFINE21'] = self.FENOTCHCAPFINE21
        self.FENOTCHRATTNEN21 = RM_Field_AGC_FENOTCHATT10_FENOTCHRATTNEN21(self)
        self.zz_fdict['FENOTCHRATTNEN21'] = self.FENOTCHRATTNEN21
        self.FENOTCHEN21 = RM_Field_AGC_FENOTCHATT10_FENOTCHEN21(self)
        self.zz_fdict['FENOTCHEN21'] = self.FENOTCHEN21
        self.FENOTCHATTNSEL22 = RM_Field_AGC_FENOTCHATT10_FENOTCHATTNSEL22(self)
        self.zz_fdict['FENOTCHATTNSEL22'] = self.FENOTCHATTNSEL22
        self.FENOTCHCAPCRSE22 = RM_Field_AGC_FENOTCHATT10_FENOTCHCAPCRSE22(self)
        self.zz_fdict['FENOTCHCAPCRSE22'] = self.FENOTCHCAPCRSE22
        self.FENOTCHCAPFINE22 = RM_Field_AGC_FENOTCHATT10_FENOTCHCAPFINE22(self)
        self.zz_fdict['FENOTCHCAPFINE22'] = self.FENOTCHCAPFINE22
        self.FENOTCHRATTNEN22 = RM_Field_AGC_FENOTCHATT10_FENOTCHRATTNEN22(self)
        self.zz_fdict['FENOTCHRATTNEN22'] = self.FENOTCHRATTNEN22
        self.FENOTCHEN22 = RM_Field_AGC_FENOTCHATT10_FENOTCHEN22(self)
        self.zz_fdict['FENOTCHEN22'] = self.FENOTCHEN22
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHATT11(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHATT11, self).__init__(rmio, label,
            0xa800c000, 0x12C,
            'FENOTCHATT11', 'AGC.FENOTCHATT11', 'read-write',
            u"",
            0x200B200B, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL23 = RM_Field_AGC_FENOTCHATT11_FENOTCHATTNSEL23(self)
        self.zz_fdict['FENOTCHATTNSEL23'] = self.FENOTCHATTNSEL23
        self.FENOTCHCAPCRSE23 = RM_Field_AGC_FENOTCHATT11_FENOTCHCAPCRSE23(self)
        self.zz_fdict['FENOTCHCAPCRSE23'] = self.FENOTCHCAPCRSE23
        self.FENOTCHCAPFINE23 = RM_Field_AGC_FENOTCHATT11_FENOTCHCAPFINE23(self)
        self.zz_fdict['FENOTCHCAPFINE23'] = self.FENOTCHCAPFINE23
        self.FENOTCHRATTNEN23 = RM_Field_AGC_FENOTCHATT11_FENOTCHRATTNEN23(self)
        self.zz_fdict['FENOTCHRATTNEN23'] = self.FENOTCHRATTNEN23
        self.FENOTCHEN23 = RM_Field_AGC_FENOTCHATT11_FENOTCHEN23(self)
        self.zz_fdict['FENOTCHEN23'] = self.FENOTCHEN23
        self.FENOTCHATTNSEL24 = RM_Field_AGC_FENOTCHATT11_FENOTCHATTNSEL24(self)
        self.zz_fdict['FENOTCHATTNSEL24'] = self.FENOTCHATTNSEL24
        self.FENOTCHCAPCRSE24 = RM_Field_AGC_FENOTCHATT11_FENOTCHCAPCRSE24(self)
        self.zz_fdict['FENOTCHCAPCRSE24'] = self.FENOTCHCAPCRSE24
        self.FENOTCHCAPFINE24 = RM_Field_AGC_FENOTCHATT11_FENOTCHCAPFINE24(self)
        self.zz_fdict['FENOTCHCAPFINE24'] = self.FENOTCHCAPFINE24
        self.FENOTCHRATTNEN24 = RM_Field_AGC_FENOTCHATT11_FENOTCHRATTNEN24(self)
        self.zz_fdict['FENOTCHRATTNEN24'] = self.FENOTCHRATTNEN24
        self.FENOTCHEN24 = RM_Field_AGC_FENOTCHATT11_FENOTCHEN24(self)
        self.zz_fdict['FENOTCHEN24'] = self.FENOTCHEN24
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT0, self).__init__(rmio, label,
            0xa800c000, 0x130,
            'FENOTCHFILT0', 'AGC.FENOTCHFILT0', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL1 = RM_Field_AGC_FENOTCHFILT0_FENOTCHATTNSEL1(self)
        self.zz_fdict['FENOTCHATTNSEL1'] = self.FENOTCHATTNSEL1
        self.FENOTCHCAPCRSE1 = RM_Field_AGC_FENOTCHFILT0_FENOTCHCAPCRSE1(self)
        self.zz_fdict['FENOTCHCAPCRSE1'] = self.FENOTCHCAPCRSE1
        self.FENOTCHCAPFINE1 = RM_Field_AGC_FENOTCHFILT0_FENOTCHCAPFINE1(self)
        self.zz_fdict['FENOTCHCAPFINE1'] = self.FENOTCHCAPFINE1
        self.FENOTCHRATTNEN1 = RM_Field_AGC_FENOTCHFILT0_FENOTCHRATTNEN1(self)
        self.zz_fdict['FENOTCHRATTNEN1'] = self.FENOTCHRATTNEN1
        self.FENOTCHEN1 = RM_Field_AGC_FENOTCHFILT0_FENOTCHEN1(self)
        self.zz_fdict['FENOTCHEN1'] = self.FENOTCHEN1
        self.FENOTCHATTNSEL2 = RM_Field_AGC_FENOTCHFILT0_FENOTCHATTNSEL2(self)
        self.zz_fdict['FENOTCHATTNSEL2'] = self.FENOTCHATTNSEL2
        self.FENOTCHCAPCRSE2 = RM_Field_AGC_FENOTCHFILT0_FENOTCHCAPCRSE2(self)
        self.zz_fdict['FENOTCHCAPCRSE2'] = self.FENOTCHCAPCRSE2
        self.FENOTCHCAPFINE2 = RM_Field_AGC_FENOTCHFILT0_FENOTCHCAPFINE2(self)
        self.zz_fdict['FENOTCHCAPFINE2'] = self.FENOTCHCAPFINE2
        self.FENOTCHRATTNEN2 = RM_Field_AGC_FENOTCHFILT0_FENOTCHRATTNEN2(self)
        self.zz_fdict['FENOTCHRATTNEN2'] = self.FENOTCHRATTNEN2
        self.FENOTCHEN2 = RM_Field_AGC_FENOTCHFILT0_FENOTCHEN2(self)
        self.zz_fdict['FENOTCHEN2'] = self.FENOTCHEN2
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT1, self).__init__(rmio, label,
            0xa800c000, 0x134,
            'FENOTCHFILT1', 'AGC.FENOTCHFILT1', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL3 = RM_Field_AGC_FENOTCHFILT1_FENOTCHATTNSEL3(self)
        self.zz_fdict['FENOTCHATTNSEL3'] = self.FENOTCHATTNSEL3
        self.FENOTCHCAPCRSE3 = RM_Field_AGC_FENOTCHFILT1_FENOTCHCAPCRSE3(self)
        self.zz_fdict['FENOTCHCAPCRSE3'] = self.FENOTCHCAPCRSE3
        self.FENOTCHCAPFINE3 = RM_Field_AGC_FENOTCHFILT1_FENOTCHCAPFINE3(self)
        self.zz_fdict['FENOTCHCAPFINE3'] = self.FENOTCHCAPFINE3
        self.FENOTCHRATTNEN3 = RM_Field_AGC_FENOTCHFILT1_FENOTCHRATTNEN3(self)
        self.zz_fdict['FENOTCHRATTNEN3'] = self.FENOTCHRATTNEN3
        self.FENOTCHEN3 = RM_Field_AGC_FENOTCHFILT1_FENOTCHEN3(self)
        self.zz_fdict['FENOTCHEN3'] = self.FENOTCHEN3
        self.FENOTCHATTNSEL4 = RM_Field_AGC_FENOTCHFILT1_FENOTCHATTNSEL4(self)
        self.zz_fdict['FENOTCHATTNSEL4'] = self.FENOTCHATTNSEL4
        self.FENOTCHCAPCRSE4 = RM_Field_AGC_FENOTCHFILT1_FENOTCHCAPCRSE4(self)
        self.zz_fdict['FENOTCHCAPCRSE4'] = self.FENOTCHCAPCRSE4
        self.FENOTCHCAPFINE4 = RM_Field_AGC_FENOTCHFILT1_FENOTCHCAPFINE4(self)
        self.zz_fdict['FENOTCHCAPFINE4'] = self.FENOTCHCAPFINE4
        self.FENOTCHRATTNEN4 = RM_Field_AGC_FENOTCHFILT1_FENOTCHRATTNEN4(self)
        self.zz_fdict['FENOTCHRATTNEN4'] = self.FENOTCHRATTNEN4
        self.FENOTCHEN4 = RM_Field_AGC_FENOTCHFILT1_FENOTCHEN4(self)
        self.zz_fdict['FENOTCHEN4'] = self.FENOTCHEN4
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT2, self).__init__(rmio, label,
            0xa800c000, 0x138,
            'FENOTCHFILT2', 'AGC.FENOTCHFILT2', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL5 = RM_Field_AGC_FENOTCHFILT2_FENOTCHATTNSEL5(self)
        self.zz_fdict['FENOTCHATTNSEL5'] = self.FENOTCHATTNSEL5
        self.FENOTCHCAPCRSE5 = RM_Field_AGC_FENOTCHFILT2_FENOTCHCAPCRSE5(self)
        self.zz_fdict['FENOTCHCAPCRSE5'] = self.FENOTCHCAPCRSE5
        self.FENOTCHCAPFINE5 = RM_Field_AGC_FENOTCHFILT2_FENOTCHCAPFINE5(self)
        self.zz_fdict['FENOTCHCAPFINE5'] = self.FENOTCHCAPFINE5
        self.FENOTCHRATTNEN5 = RM_Field_AGC_FENOTCHFILT2_FENOTCHRATTNEN5(self)
        self.zz_fdict['FENOTCHRATTNEN5'] = self.FENOTCHRATTNEN5
        self.FENOTCHEN5 = RM_Field_AGC_FENOTCHFILT2_FENOTCHEN5(self)
        self.zz_fdict['FENOTCHEN5'] = self.FENOTCHEN5
        self.FENOTCHATTNSEL6 = RM_Field_AGC_FENOTCHFILT2_FENOTCHATTNSEL6(self)
        self.zz_fdict['FENOTCHATTNSEL6'] = self.FENOTCHATTNSEL6
        self.FENOTCHCAPCRSE6 = RM_Field_AGC_FENOTCHFILT2_FENOTCHCAPCRSE6(self)
        self.zz_fdict['FENOTCHCAPCRSE6'] = self.FENOTCHCAPCRSE6
        self.FENOTCHCAPFINE6 = RM_Field_AGC_FENOTCHFILT2_FENOTCHCAPFINE6(self)
        self.zz_fdict['FENOTCHCAPFINE6'] = self.FENOTCHCAPFINE6
        self.FENOTCHRATTNEN6 = RM_Field_AGC_FENOTCHFILT2_FENOTCHRATTNEN6(self)
        self.zz_fdict['FENOTCHRATTNEN6'] = self.FENOTCHRATTNEN6
        self.FENOTCHEN6 = RM_Field_AGC_FENOTCHFILT2_FENOTCHEN6(self)
        self.zz_fdict['FENOTCHEN6'] = self.FENOTCHEN6
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT3, self).__init__(rmio, label,
            0xa800c000, 0x13C,
            'FENOTCHFILT3', 'AGC.FENOTCHFILT3', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL7 = RM_Field_AGC_FENOTCHFILT3_FENOTCHATTNSEL7(self)
        self.zz_fdict['FENOTCHATTNSEL7'] = self.FENOTCHATTNSEL7
        self.FENOTCHCAPCRSE7 = RM_Field_AGC_FENOTCHFILT3_FENOTCHCAPCRSE7(self)
        self.zz_fdict['FENOTCHCAPCRSE7'] = self.FENOTCHCAPCRSE7
        self.FENOTCHCAPFINE7 = RM_Field_AGC_FENOTCHFILT3_FENOTCHCAPFINE7(self)
        self.zz_fdict['FENOTCHCAPFINE7'] = self.FENOTCHCAPFINE7
        self.FENOTCHRATTNEN7 = RM_Field_AGC_FENOTCHFILT3_FENOTCHRATTNEN7(self)
        self.zz_fdict['FENOTCHRATTNEN7'] = self.FENOTCHRATTNEN7
        self.FENOTCHEN7 = RM_Field_AGC_FENOTCHFILT3_FENOTCHEN7(self)
        self.zz_fdict['FENOTCHEN7'] = self.FENOTCHEN7
        self.FENOTCHATTNSEL8 = RM_Field_AGC_FENOTCHFILT3_FENOTCHATTNSEL8(self)
        self.zz_fdict['FENOTCHATTNSEL8'] = self.FENOTCHATTNSEL8
        self.FENOTCHCAPCRSE8 = RM_Field_AGC_FENOTCHFILT3_FENOTCHCAPCRSE8(self)
        self.zz_fdict['FENOTCHCAPCRSE8'] = self.FENOTCHCAPCRSE8
        self.FENOTCHCAPFINE8 = RM_Field_AGC_FENOTCHFILT3_FENOTCHCAPFINE8(self)
        self.zz_fdict['FENOTCHCAPFINE8'] = self.FENOTCHCAPFINE8
        self.FENOTCHRATTNEN8 = RM_Field_AGC_FENOTCHFILT3_FENOTCHRATTNEN8(self)
        self.zz_fdict['FENOTCHRATTNEN8'] = self.FENOTCHRATTNEN8
        self.FENOTCHEN8 = RM_Field_AGC_FENOTCHFILT3_FENOTCHEN8(self)
        self.zz_fdict['FENOTCHEN8'] = self.FENOTCHEN8
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT4, self).__init__(rmio, label,
            0xa800c000, 0x140,
            'FENOTCHFILT4', 'AGC.FENOTCHFILT4', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL9 = RM_Field_AGC_FENOTCHFILT4_FENOTCHATTNSEL9(self)
        self.zz_fdict['FENOTCHATTNSEL9'] = self.FENOTCHATTNSEL9
        self.FENOTCHCAPCRSE9 = RM_Field_AGC_FENOTCHFILT4_FENOTCHCAPCRSE9(self)
        self.zz_fdict['FENOTCHCAPCRSE9'] = self.FENOTCHCAPCRSE9
        self.FENOTCHCAPFINE9 = RM_Field_AGC_FENOTCHFILT4_FENOTCHCAPFINE9(self)
        self.zz_fdict['FENOTCHCAPFINE9'] = self.FENOTCHCAPFINE9
        self.FENOTCHRATTNEN9 = RM_Field_AGC_FENOTCHFILT4_FENOTCHRATTNEN9(self)
        self.zz_fdict['FENOTCHRATTNEN9'] = self.FENOTCHRATTNEN9
        self.FENOTCHEN9 = RM_Field_AGC_FENOTCHFILT4_FENOTCHEN9(self)
        self.zz_fdict['FENOTCHEN9'] = self.FENOTCHEN9
        self.FENOTCHATTNSEL10 = RM_Field_AGC_FENOTCHFILT4_FENOTCHATTNSEL10(self)
        self.zz_fdict['FENOTCHATTNSEL10'] = self.FENOTCHATTNSEL10
        self.FENOTCHCAPCRSE10 = RM_Field_AGC_FENOTCHFILT4_FENOTCHCAPCRSE10(self)
        self.zz_fdict['FENOTCHCAPCRSE10'] = self.FENOTCHCAPCRSE10
        self.FENOTCHCAPFINE10 = RM_Field_AGC_FENOTCHFILT4_FENOTCHCAPFINE10(self)
        self.zz_fdict['FENOTCHCAPFINE10'] = self.FENOTCHCAPFINE10
        self.FENOTCHRATTNEN10 = RM_Field_AGC_FENOTCHFILT4_FENOTCHRATTNEN10(self)
        self.zz_fdict['FENOTCHRATTNEN10'] = self.FENOTCHRATTNEN10
        self.FENOTCHEN10 = RM_Field_AGC_FENOTCHFILT4_FENOTCHEN10(self)
        self.zz_fdict['FENOTCHEN10'] = self.FENOTCHEN10
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT5, self).__init__(rmio, label,
            0xa800c000, 0x144,
            'FENOTCHFILT5', 'AGC.FENOTCHFILT5', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL11 = RM_Field_AGC_FENOTCHFILT5_FENOTCHATTNSEL11(self)
        self.zz_fdict['FENOTCHATTNSEL11'] = self.FENOTCHATTNSEL11
        self.FENOTCHCAPCRSE11 = RM_Field_AGC_FENOTCHFILT5_FENOTCHCAPCRSE11(self)
        self.zz_fdict['FENOTCHCAPCRSE11'] = self.FENOTCHCAPCRSE11
        self.FENOTCHCAPFINE11 = RM_Field_AGC_FENOTCHFILT5_FENOTCHCAPFINE11(self)
        self.zz_fdict['FENOTCHCAPFINE11'] = self.FENOTCHCAPFINE11
        self.FENOTCHRATTNEN11 = RM_Field_AGC_FENOTCHFILT5_FENOTCHRATTNEN11(self)
        self.zz_fdict['FENOTCHRATTNEN11'] = self.FENOTCHRATTNEN11
        self.FENOTCHEN11 = RM_Field_AGC_FENOTCHFILT5_FENOTCHEN11(self)
        self.zz_fdict['FENOTCHEN11'] = self.FENOTCHEN11
        self.FENOTCHATTNSEL12 = RM_Field_AGC_FENOTCHFILT5_FENOTCHATTNSEL12(self)
        self.zz_fdict['FENOTCHATTNSEL12'] = self.FENOTCHATTNSEL12
        self.FENOTCHCAPCRSE12 = RM_Field_AGC_FENOTCHFILT5_FENOTCHCAPCRSE12(self)
        self.zz_fdict['FENOTCHCAPCRSE12'] = self.FENOTCHCAPCRSE12
        self.FENOTCHCAPFINE12 = RM_Field_AGC_FENOTCHFILT5_FENOTCHCAPFINE12(self)
        self.zz_fdict['FENOTCHCAPFINE12'] = self.FENOTCHCAPFINE12
        self.FENOTCHRATTNEN12 = RM_Field_AGC_FENOTCHFILT5_FENOTCHRATTNEN12(self)
        self.zz_fdict['FENOTCHRATTNEN12'] = self.FENOTCHRATTNEN12
        self.FENOTCHEN12 = RM_Field_AGC_FENOTCHFILT5_FENOTCHEN12(self)
        self.zz_fdict['FENOTCHEN12'] = self.FENOTCHEN12
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT6, self).__init__(rmio, label,
            0xa800c000, 0x148,
            'FENOTCHFILT6', 'AGC.FENOTCHFILT6', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL13 = RM_Field_AGC_FENOTCHFILT6_FENOTCHATTNSEL13(self)
        self.zz_fdict['FENOTCHATTNSEL13'] = self.FENOTCHATTNSEL13
        self.FENOTCHCAPCRSE13 = RM_Field_AGC_FENOTCHFILT6_FENOTCHCAPCRSE13(self)
        self.zz_fdict['FENOTCHCAPCRSE13'] = self.FENOTCHCAPCRSE13
        self.FENOTCHCAPFINE13 = RM_Field_AGC_FENOTCHFILT6_FENOTCHCAPFINE13(self)
        self.zz_fdict['FENOTCHCAPFINE13'] = self.FENOTCHCAPFINE13
        self.FENOTCHRATTNEN13 = RM_Field_AGC_FENOTCHFILT6_FENOTCHRATTNEN13(self)
        self.zz_fdict['FENOTCHRATTNEN13'] = self.FENOTCHRATTNEN13
        self.FENOTCHEN13 = RM_Field_AGC_FENOTCHFILT6_FENOTCHEN13(self)
        self.zz_fdict['FENOTCHEN13'] = self.FENOTCHEN13
        self.FENOTCHATTNSEL14 = RM_Field_AGC_FENOTCHFILT6_FENOTCHATTNSEL14(self)
        self.zz_fdict['FENOTCHATTNSEL14'] = self.FENOTCHATTNSEL14
        self.FENOTCHCAPCRSE14 = RM_Field_AGC_FENOTCHFILT6_FENOTCHCAPCRSE14(self)
        self.zz_fdict['FENOTCHCAPCRSE14'] = self.FENOTCHCAPCRSE14
        self.FENOTCHCAPFINE14 = RM_Field_AGC_FENOTCHFILT6_FENOTCHCAPFINE14(self)
        self.zz_fdict['FENOTCHCAPFINE14'] = self.FENOTCHCAPFINE14
        self.FENOTCHRATTNEN14 = RM_Field_AGC_FENOTCHFILT6_FENOTCHRATTNEN14(self)
        self.zz_fdict['FENOTCHRATTNEN14'] = self.FENOTCHRATTNEN14
        self.FENOTCHEN14 = RM_Field_AGC_FENOTCHFILT6_FENOTCHEN14(self)
        self.zz_fdict['FENOTCHEN14'] = self.FENOTCHEN14
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT7, self).__init__(rmio, label,
            0xa800c000, 0x14C,
            'FENOTCHFILT7', 'AGC.FENOTCHFILT7', 'read-write',
            u"",
            0x20080000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL15 = RM_Field_AGC_FENOTCHFILT7_FENOTCHATTNSEL15(self)
        self.zz_fdict['FENOTCHATTNSEL15'] = self.FENOTCHATTNSEL15
        self.FENOTCHCAPCRSE15 = RM_Field_AGC_FENOTCHFILT7_FENOTCHCAPCRSE15(self)
        self.zz_fdict['FENOTCHCAPCRSE15'] = self.FENOTCHCAPCRSE15
        self.FENOTCHCAPFINE15 = RM_Field_AGC_FENOTCHFILT7_FENOTCHCAPFINE15(self)
        self.zz_fdict['FENOTCHCAPFINE15'] = self.FENOTCHCAPFINE15
        self.FENOTCHRATTNEN15 = RM_Field_AGC_FENOTCHFILT7_FENOTCHRATTNEN15(self)
        self.zz_fdict['FENOTCHRATTNEN15'] = self.FENOTCHRATTNEN15
        self.FENOTCHEN15 = RM_Field_AGC_FENOTCHFILT7_FENOTCHEN15(self)
        self.zz_fdict['FENOTCHEN15'] = self.FENOTCHEN15
        self.FENOTCHATTNSEL16 = RM_Field_AGC_FENOTCHFILT7_FENOTCHATTNSEL16(self)
        self.zz_fdict['FENOTCHATTNSEL16'] = self.FENOTCHATTNSEL16
        self.FENOTCHCAPCRSE16 = RM_Field_AGC_FENOTCHFILT7_FENOTCHCAPCRSE16(self)
        self.zz_fdict['FENOTCHCAPCRSE16'] = self.FENOTCHCAPCRSE16
        self.FENOTCHCAPFINE16 = RM_Field_AGC_FENOTCHFILT7_FENOTCHCAPFINE16(self)
        self.zz_fdict['FENOTCHCAPFINE16'] = self.FENOTCHCAPFINE16
        self.FENOTCHRATTNEN16 = RM_Field_AGC_FENOTCHFILT7_FENOTCHRATTNEN16(self)
        self.zz_fdict['FENOTCHRATTNEN16'] = self.FENOTCHRATTNEN16
        self.FENOTCHEN16 = RM_Field_AGC_FENOTCHFILT7_FENOTCHEN16(self)
        self.zz_fdict['FENOTCHEN16'] = self.FENOTCHEN16
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT8(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT8, self).__init__(rmio, label,
            0xa800c000, 0x150,
            'FENOTCHFILT8', 'AGC.FENOTCHFILT8', 'read-write',
            u"",
            0x200B200A, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL17 = RM_Field_AGC_FENOTCHFILT8_FENOTCHATTNSEL17(self)
        self.zz_fdict['FENOTCHATTNSEL17'] = self.FENOTCHATTNSEL17
        self.FENOTCHCAPCRSE17 = RM_Field_AGC_FENOTCHFILT8_FENOTCHCAPCRSE17(self)
        self.zz_fdict['FENOTCHCAPCRSE17'] = self.FENOTCHCAPCRSE17
        self.FENOTCHCAPFINE17 = RM_Field_AGC_FENOTCHFILT8_FENOTCHCAPFINE17(self)
        self.zz_fdict['FENOTCHCAPFINE17'] = self.FENOTCHCAPFINE17
        self.FENOTCHRATTNEN17 = RM_Field_AGC_FENOTCHFILT8_FENOTCHRATTNEN17(self)
        self.zz_fdict['FENOTCHRATTNEN17'] = self.FENOTCHRATTNEN17
        self.FENOTCHEN17 = RM_Field_AGC_FENOTCHFILT8_FENOTCHEN17(self)
        self.zz_fdict['FENOTCHEN17'] = self.FENOTCHEN17
        self.FENOTCHATTNSEL18 = RM_Field_AGC_FENOTCHFILT8_FENOTCHATTNSEL18(self)
        self.zz_fdict['FENOTCHATTNSEL18'] = self.FENOTCHATTNSEL18
        self.FENOTCHCAPCRSE18 = RM_Field_AGC_FENOTCHFILT8_FENOTCHCAPCRSE18(self)
        self.zz_fdict['FENOTCHCAPCRSE18'] = self.FENOTCHCAPCRSE18
        self.FENOTCHCAPFINE18 = RM_Field_AGC_FENOTCHFILT8_FENOTCHCAPFINE18(self)
        self.zz_fdict['FENOTCHCAPFINE18'] = self.FENOTCHCAPFINE18
        self.FENOTCHRATTNEN18 = RM_Field_AGC_FENOTCHFILT8_FENOTCHRATTNEN18(self)
        self.zz_fdict['FENOTCHRATTNEN18'] = self.FENOTCHRATTNEN18
        self.FENOTCHEN18 = RM_Field_AGC_FENOTCHFILT8_FENOTCHEN18(self)
        self.zz_fdict['FENOTCHEN18'] = self.FENOTCHEN18
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT9(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT9, self).__init__(rmio, label,
            0xa800c000, 0x154,
            'FENOTCHFILT9', 'AGC.FENOTCHFILT9', 'read-write',
            u"",
            0x200B200B, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL19 = RM_Field_AGC_FENOTCHFILT9_FENOTCHATTNSEL19(self)
        self.zz_fdict['FENOTCHATTNSEL19'] = self.FENOTCHATTNSEL19
        self.FENOTCHCAPCRSE19 = RM_Field_AGC_FENOTCHFILT9_FENOTCHCAPCRSE19(self)
        self.zz_fdict['FENOTCHCAPCRSE19'] = self.FENOTCHCAPCRSE19
        self.FENOTCHCAPFINE19 = RM_Field_AGC_FENOTCHFILT9_FENOTCHCAPFINE19(self)
        self.zz_fdict['FENOTCHCAPFINE19'] = self.FENOTCHCAPFINE19
        self.FENOTCHRATTNEN19 = RM_Field_AGC_FENOTCHFILT9_FENOTCHRATTNEN19(self)
        self.zz_fdict['FENOTCHRATTNEN19'] = self.FENOTCHRATTNEN19
        self.FENOTCHEN19 = RM_Field_AGC_FENOTCHFILT9_FENOTCHEN19(self)
        self.zz_fdict['FENOTCHEN19'] = self.FENOTCHEN19
        self.FENOTCHATTNSEL20 = RM_Field_AGC_FENOTCHFILT9_FENOTCHATTNSEL20(self)
        self.zz_fdict['FENOTCHATTNSEL20'] = self.FENOTCHATTNSEL20
        self.FENOTCHCAPCRSE20 = RM_Field_AGC_FENOTCHFILT9_FENOTCHCAPCRSE20(self)
        self.zz_fdict['FENOTCHCAPCRSE20'] = self.FENOTCHCAPCRSE20
        self.FENOTCHCAPFINE20 = RM_Field_AGC_FENOTCHFILT9_FENOTCHCAPFINE20(self)
        self.zz_fdict['FENOTCHCAPFINE20'] = self.FENOTCHCAPFINE20
        self.FENOTCHRATTNEN20 = RM_Field_AGC_FENOTCHFILT9_FENOTCHRATTNEN20(self)
        self.zz_fdict['FENOTCHRATTNEN20'] = self.FENOTCHRATTNEN20
        self.FENOTCHEN20 = RM_Field_AGC_FENOTCHFILT9_FENOTCHEN20(self)
        self.zz_fdict['FENOTCHEN20'] = self.FENOTCHEN20
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT10(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT10, self).__init__(rmio, label,
            0xa800c000, 0x158,
            'FENOTCHFILT10', 'AGC.FENOTCHFILT10', 'read-write',
            u"",
            0x200B200B, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL21 = RM_Field_AGC_FENOTCHFILT10_FENOTCHATTNSEL21(self)
        self.zz_fdict['FENOTCHATTNSEL21'] = self.FENOTCHATTNSEL21
        self.FENOTCHCAPCRSE21 = RM_Field_AGC_FENOTCHFILT10_FENOTCHCAPCRSE21(self)
        self.zz_fdict['FENOTCHCAPCRSE21'] = self.FENOTCHCAPCRSE21
        self.FENOTCHCAPFINE21 = RM_Field_AGC_FENOTCHFILT10_FENOTCHCAPFINE21(self)
        self.zz_fdict['FENOTCHCAPFINE21'] = self.FENOTCHCAPFINE21
        self.FENOTCHRATTNEN21 = RM_Field_AGC_FENOTCHFILT10_FENOTCHRATTNEN21(self)
        self.zz_fdict['FENOTCHRATTNEN21'] = self.FENOTCHRATTNEN21
        self.FENOTCHEN21 = RM_Field_AGC_FENOTCHFILT10_FENOTCHEN21(self)
        self.zz_fdict['FENOTCHEN21'] = self.FENOTCHEN21
        self.FENOTCHATTNSEL22 = RM_Field_AGC_FENOTCHFILT10_FENOTCHATTNSEL22(self)
        self.zz_fdict['FENOTCHATTNSEL22'] = self.FENOTCHATTNSEL22
        self.FENOTCHCAPCRSE22 = RM_Field_AGC_FENOTCHFILT10_FENOTCHCAPCRSE22(self)
        self.zz_fdict['FENOTCHCAPCRSE22'] = self.FENOTCHCAPCRSE22
        self.FENOTCHCAPFINE22 = RM_Field_AGC_FENOTCHFILT10_FENOTCHCAPFINE22(self)
        self.zz_fdict['FENOTCHCAPFINE22'] = self.FENOTCHCAPFINE22
        self.FENOTCHRATTNEN22 = RM_Field_AGC_FENOTCHFILT10_FENOTCHRATTNEN22(self)
        self.zz_fdict['FENOTCHRATTNEN22'] = self.FENOTCHRATTNEN22
        self.FENOTCHEN22 = RM_Field_AGC_FENOTCHFILT10_FENOTCHEN22(self)
        self.zz_fdict['FENOTCHEN22'] = self.FENOTCHEN22
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FENOTCHFILT11(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FENOTCHFILT11, self).__init__(rmio, label,
            0xa800c000, 0x15C,
            'FENOTCHFILT11', 'AGC.FENOTCHFILT11', 'read-write',
            u"",
            0x200B200B, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FENOTCHATTNSEL23 = RM_Field_AGC_FENOTCHFILT11_FENOTCHATTNSEL23(self)
        self.zz_fdict['FENOTCHATTNSEL23'] = self.FENOTCHATTNSEL23
        self.FENOTCHCAPCRSE23 = RM_Field_AGC_FENOTCHFILT11_FENOTCHCAPCRSE23(self)
        self.zz_fdict['FENOTCHCAPCRSE23'] = self.FENOTCHCAPCRSE23
        self.FENOTCHCAPFINE23 = RM_Field_AGC_FENOTCHFILT11_FENOTCHCAPFINE23(self)
        self.zz_fdict['FENOTCHCAPFINE23'] = self.FENOTCHCAPFINE23
        self.FENOTCHRATTNEN23 = RM_Field_AGC_FENOTCHFILT11_FENOTCHRATTNEN23(self)
        self.zz_fdict['FENOTCHRATTNEN23'] = self.FENOTCHRATTNEN23
        self.FENOTCHEN23 = RM_Field_AGC_FENOTCHFILT11_FENOTCHEN23(self)
        self.zz_fdict['FENOTCHEN23'] = self.FENOTCHEN23
        self.FENOTCHATTNSEL24 = RM_Field_AGC_FENOTCHFILT11_FENOTCHATTNSEL24(self)
        self.zz_fdict['FENOTCHATTNSEL24'] = self.FENOTCHATTNSEL24
        self.FENOTCHCAPCRSE24 = RM_Field_AGC_FENOTCHFILT11_FENOTCHCAPCRSE24(self)
        self.zz_fdict['FENOTCHCAPCRSE24'] = self.FENOTCHCAPCRSE24
        self.FENOTCHCAPFINE24 = RM_Field_AGC_FENOTCHFILT11_FENOTCHCAPFINE24(self)
        self.zz_fdict['FENOTCHCAPFINE24'] = self.FENOTCHCAPFINE24
        self.FENOTCHRATTNEN24 = RM_Field_AGC_FENOTCHFILT11_FENOTCHRATTNEN24(self)
        self.zz_fdict['FENOTCHRATTNEN24'] = self.FENOTCHRATTNEN24
        self.FENOTCHEN24 = RM_Field_AGC_FENOTCHFILT11_FENOTCHEN24(self)
        self.zz_fdict['FENOTCHEN24'] = self.FENOTCHEN24
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CCADEBUG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CCADEBUG, self).__init__(rmio, label,
            0xa800c000, 0x160,
            'CCADEBUG', 'AGC.CCADEBUG', 'read-only',
            u"",
            0x00000000, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DEBUGCCARSSI = RM_Field_AGC_CCADEBUG_DEBUGCCARSSI(self)
        self.zz_fdict['DEBUGCCARSSI'] = self.DEBUGCCARSSI
        self.DEBUGCCAM1 = RM_Field_AGC_CCADEBUG_DEBUGCCAM1(self)
        self.zz_fdict['DEBUGCCAM1'] = self.DEBUGCCAM1
        self.DEBUGCCASIGDET = RM_Field_AGC_CCADEBUG_DEBUGCCASIGDET(self)
        self.zz_fdict['DEBUGCCASIGDET'] = self.DEBUGCCASIGDET
        self.__dict__['zz_frozen'] = True


