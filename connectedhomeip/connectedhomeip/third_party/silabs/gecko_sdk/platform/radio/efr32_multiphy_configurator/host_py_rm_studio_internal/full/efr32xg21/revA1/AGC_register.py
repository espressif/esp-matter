
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
            0x00000000, 0xFFFFFFFF,
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
            'STATUS0', 'AGC.STATUS0', 'read-write',
            u"",
            0x00000000, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.GAININDEX = RM_Field_AGC_STATUS0_GAININDEX(self)
        self.zz_fdict['GAININDEX'] = self.GAININDEX
        self.RFPKDLAT = RM_Field_AGC_STATUS0_RFPKDLAT(self)
        self.zz_fdict['RFPKDLAT'] = self.RFPKDLAT
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
            0x00000000, 0x0003FEFF,
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
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RSSI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSI, self).__init__(rmio, label,
            0xa800c000, 0x010,
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
            0xa800c000, 0x014,
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
            0xa800c000, 0x01C,
            'CTRL0', 'AGC.CTRL0', 'read-write',
            u"",
            0x2002727F, 0xFEEFFFFF,
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
        self.AGCCLKUNDIVREQ = RM_Field_AGC_CTRL0_AGCCLKUNDIVREQ(self)
        self.zz_fdict['AGCCLKUNDIVREQ'] = self.AGCCLKUNDIVREQ
        self.DISRESETCHPWR = RM_Field_AGC_CTRL0_DISRESETCHPWR(self)
        self.zz_fdict['DISRESETCHPWR'] = self.DISRESETCHPWR
        self.ADCATTENMODE = RM_Field_AGC_CTRL0_ADCATTENMODE(self)
        self.zz_fdict['ADCATTENMODE'] = self.ADCATTENMODE
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
            0xa800c000, 0x020,
            'CTRL1', 'AGC.CTRL1', 'read-write',
            u"",
            0x00001300, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CCATHRSH = RM_Field_AGC_CTRL1_CCATHRSH(self)
        self.zz_fdict['CCATHRSH'] = self.CCATHRSH
        self.RSSIPERIOD = RM_Field_AGC_CTRL1_RSSIPERIOD(self)
        self.zz_fdict['RSSIPERIOD'] = self.RSSIPERIOD
        self.PWRPERIOD = RM_Field_AGC_CTRL1_PWRPERIOD(self)
        self.zz_fdict['PWRPERIOD'] = self.PWRPERIOD
        self.SUBPERIOD = RM_Field_AGC_CTRL1_SUBPERIOD(self)
        self.zz_fdict['SUBPERIOD'] = self.SUBPERIOD
        self.SUBNUM = RM_Field_AGC_CTRL1_SUBNUM(self)
        self.zz_fdict['SUBNUM'] = self.SUBNUM
        self.SUBDEN = RM_Field_AGC_CTRL1_SUBDEN(self)
        self.zz_fdict['SUBDEN'] = self.SUBDEN
        self.SUBINT = RM_Field_AGC_CTRL1_SUBINT(self)
        self.zz_fdict['SUBINT'] = self.SUBINT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL2, self).__init__(rmio, label,
            0xa800c000, 0x024,
            'CTRL2', 'AGC.CTRL2', 'read-write',
            u"",
            0x0000610A, 0xC3FFFFFF,
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
        self.PRSDEBUGEN = RM_Field_AGC_CTRL2_PRSDEBUGEN(self)
        self.zz_fdict['PRSDEBUGEN'] = self.PRSDEBUGEN
        self.DISRFPKD = RM_Field_AGC_CTRL2_DISRFPKD(self)
        self.zz_fdict['DISRFPKD'] = self.DISRFPKD
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL3, self).__init__(rmio, label,
            0xa800c000, 0x028,
            'CTRL3', 'AGC.CTRL3', 'read-write',
            u"",
            0x02A8A800, 0x03FFFFFF,
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


class RM_Register_AGC_RSSISTEPTHR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSISTEPTHR, self).__init__(rmio, label,
            0xa800c000, 0x02C,
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
            0xa800c000, 0x030,
            'MANGAIN', 'AGC.MANGAIN', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
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
            0xa800c000, 0x038,
            'IF', 'AGC.IF', 'read-write',
            u"",
            0x00000000, 0x0000007D,
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
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IEN, self).__init__(rmio, label,
            0xa800c000, 0x044,
            'IEN', 'AGC.IEN', 'read-write',
            u"",
            0x00000000, 0x0000007D,
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
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CMD, self).__init__(rmio, label,
            0xa800c000, 0x048,
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
            0xa800c000, 0x050,
            'GAINRANGE', 'AGC.GAINRANGE', 'read-write',
            u"",
            0x08813187, 0x0FFFFFFF,
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
        self.BOOSTLNA = RM_Field_AGC_GAINRANGE_BOOSTLNA(self)
        self.zz_fdict['BOOSTLNA'] = self.BOOSTLNA
        self.LNABWADJ = RM_Field_AGC_GAINRANGE_LNABWADJ(self)
        self.zz_fdict['LNABWADJ'] = self.LNABWADJ
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_AGCPERIOD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_AGCPERIOD, self).__init__(rmio, label,
            0xa800c000, 0x054,
            'AGCPERIOD', 'AGC.AGCPERIOD', 'read-write',
            u"",
            0xD607370E, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PERIODHI = RM_Field_AGC_AGCPERIOD_PERIODHI(self)
        self.zz_fdict['PERIODHI'] = self.PERIODHI
        self.PERIODLO = RM_Field_AGC_AGCPERIOD_PERIODLO(self)
        self.zz_fdict['PERIODLO'] = self.PERIODLO
        self.MAXHICNTTHD = RM_Field_AGC_AGCPERIOD_MAXHICNTTHD(self)
        self.zz_fdict['MAXHICNTTHD'] = self.MAXHICNTTHD
        self.SETTLETIMEIF = RM_Field_AGC_AGCPERIOD_SETTLETIMEIF(self)
        self.zz_fdict['SETTLETIMEIF'] = self.SETTLETIMEIF
        self.SETTLETIMERF = RM_Field_AGC_AGCPERIOD_SETTLETIMERF(self)
        self.zz_fdict['SETTLETIMERF'] = self.SETTLETIMERF
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_HICNTREGION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_HICNTREGION, self).__init__(rmio, label,
            0xa800c000, 0x058,
            'HICNTREGION', 'AGC.HICNTREGION', 'read-write',
            u"",
            0x08060543, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.HICNTREGION0 = RM_Field_AGC_HICNTREGION_HICNTREGION0(self)
        self.zz_fdict['HICNTREGION0'] = self.HICNTREGION0
        self.HICNTREGION1 = RM_Field_AGC_HICNTREGION_HICNTREGION1(self)
        self.zz_fdict['HICNTREGION1'] = self.HICNTREGION1
        self.HICNTREGION2 = RM_Field_AGC_HICNTREGION_HICNTREGION2(self)
        self.zz_fdict['HICNTREGION2'] = self.HICNTREGION2
        self.HICNTREGION3 = RM_Field_AGC_HICNTREGION_HICNTREGION3(self)
        self.zz_fdict['HICNTREGION3'] = self.HICNTREGION3
        self.HICNTREGION4 = RM_Field_AGC_HICNTREGION_HICNTREGION4(self)
        self.zz_fdict['HICNTREGION4'] = self.HICNTREGION4
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_STEPDWN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STEPDWN, self).__init__(rmio, label,
            0xa800c000, 0x05C,
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


class RM_Register_AGC_GAINSTEPLIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINSTEPLIM, self).__init__(rmio, label,
            0xa800c000, 0x064,
            'GAINSTEPLIM', 'AGC.GAINSTEPLIM', 'read-write',
            u"",
            0x00003144, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CFLOOPSTEPMAX = RM_Field_AGC_GAINSTEPLIM_CFLOOPSTEPMAX(self)
        self.zz_fdict['CFLOOPSTEPMAX'] = self.CFLOOPSTEPMAX
        self.CFLOOPDEL = RM_Field_AGC_GAINSTEPLIM_CFLOOPDEL(self)
        self.zz_fdict['CFLOOPDEL'] = self.CFLOOPDEL
        self.HYST = RM_Field_AGC_GAINSTEPLIM_HYST(self)
        self.zz_fdict['HYST'] = self.HYST
        self.MAXPWRVAR = RM_Field_AGC_GAINSTEPLIM_MAXPWRVAR(self)
        self.zz_fdict['MAXPWRVAR'] = self.MAXPWRVAR
        self.TRANRSTAGC = RM_Field_AGC_GAINSTEPLIM_TRANRSTAGC(self)
        self.zz_fdict['TRANRSTAGC'] = self.TRANRSTAGC
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT0, self).__init__(rmio, label,
            0xa800c000, 0x068,
            'PNRFATT0', 'AGC.PNRFATT0', 'read-write',
            u"",
            0x06102040, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT1 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT1(self)
        self.zz_fdict['LNAMIXRFATT1'] = self.LNAMIXRFATT1
        self.LNAMIXRFATT2 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT2(self)
        self.zz_fdict['LNAMIXRFATT2'] = self.LNAMIXRFATT2
        self.LNAMIXRFATT3 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT3(self)
        self.zz_fdict['LNAMIXRFATT3'] = self.LNAMIXRFATT3
        self.LNAMIXRFATT4 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT4(self)
        self.zz_fdict['LNAMIXRFATT4'] = self.LNAMIXRFATT4
        self.LNAMIXRFATT5 = RM_Field_AGC_PNRFATT0_LNAMIXRFATT5(self)
        self.zz_fdict['LNAMIXRFATT5'] = self.LNAMIXRFATT5
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT1, self).__init__(rmio, label,
            0xa800c000, 0x06C,
            'PNRFATT1', 'AGC.PNRFATT1', 'read-write',
            u"",
            0x1848F2C8, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT6 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT6(self)
        self.zz_fdict['LNAMIXRFATT6'] = self.LNAMIXRFATT6
        self.LNAMIXRFATT7 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT7(self)
        self.zz_fdict['LNAMIXRFATT7'] = self.LNAMIXRFATT7
        self.LNAMIXRFATT8 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT8(self)
        self.zz_fdict['LNAMIXRFATT8'] = self.LNAMIXRFATT8
        self.LNAMIXRFATT9 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT9(self)
        self.zz_fdict['LNAMIXRFATT9'] = self.LNAMIXRFATT9
        self.LNAMIXRFATT10 = RM_Field_AGC_PNRFATT1_LNAMIXRFATT10(self)
        self.zz_fdict['LNAMIXRFATT10'] = self.LNAMIXRFATT10
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_PNRFATT2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_PNRFATT2, self).__init__(rmio, label,
            0xa800c000, 0x070,
            'PNRFATT2', 'AGC.PNRFATT2', 'read-write',
            u"",
            0x00F6E81F, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFATT11 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT11(self)
        self.zz_fdict['LNAMIXRFATT11'] = self.LNAMIXRFATT11
        self.LNAMIXRFATT12 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT12(self)
        self.zz_fdict['LNAMIXRFATT12'] = self.LNAMIXRFATT12
        self.LNAMIXRFATT13 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT13(self)
        self.zz_fdict['LNAMIXRFATT13'] = self.LNAMIXRFATT13
        self.LNAMIXRFATT14 = RM_Field_AGC_PNRFATT2_LNAMIXRFATT14(self)
        self.zz_fdict['LNAMIXRFATT14'] = self.LNAMIXRFATT14
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LNAMIXCODE0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LNAMIXCODE0, self).__init__(rmio, label,
            0xa800c000, 0x074,
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
            0xa800c000, 0x078,
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
            0xa800c000, 0x07C,
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
            0xa800c000, 0x080,
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
            0xa800c000, 0x088,
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
            0xa800c000, 0x08C,
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


