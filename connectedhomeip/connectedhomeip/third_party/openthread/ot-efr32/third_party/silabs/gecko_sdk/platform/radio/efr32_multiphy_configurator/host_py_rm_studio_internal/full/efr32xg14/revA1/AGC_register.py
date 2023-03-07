
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . AGC_field import *


class RM_Register_AGC_STATUS0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STATUS0, self).__init__(rmio, label,
            0x40087000, 0x000,
            'STATUS0', 'AGC.STATUS0', 'read-only',
            u"",
            0x00300000, 0x3EFFF43F)

        self.GAININDEX = RM_Field_AGC_STATUS0_GAININDEX(self)
        self.zz_fdict['GAININDEX'] = self.GAININDEX
        self.CCA = RM_Field_AGC_STATUS0_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.GAINOK = RM_Field_AGC_STATUS0_GAINOK(self)
        self.zz_fdict['GAINOK'] = self.GAINOK
        self.GAINIFPGA = RM_Field_AGC_STATUS0_GAINIFPGA(self)
        self.zz_fdict['GAINIFPGA'] = self.GAINIFPGA
        self.GAINLNADEGEN = RM_Field_AGC_STATUS0_GAINLNADEGEN(self)
        self.zz_fdict['GAINLNADEGEN'] = self.GAINLNADEGEN
        self.GAINLNASLICE = RM_Field_AGC_STATUS0_GAINLNASLICE(self)
        self.zz_fdict['GAINLNASLICE'] = self.GAINLNASLICE
        self.GAINLNAATTEN = RM_Field_AGC_STATUS0_GAINLNAATTEN(self)
        self.zz_fdict['GAINLNAATTEN'] = self.GAINLNAATTEN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_STATUS1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_STATUS1, self).__init__(rmio, label,
            0x40087000, 0x004,
            'STATUS1', 'AGC.STATUS1', 'read-only',
            u"",
            0x00000000, 0x000FFEFF)

        self.CHPWR = RM_Field_AGC_STATUS1_CHPWR(self)
        self.zz_fdict['CHPWR'] = self.CHPWR
        self.ADCSTABILITYDET = RM_Field_AGC_STATUS1_ADCSTABILITYDET(self)
        self.zz_fdict['ADCSTABILITYDET'] = self.ADCSTABILITYDET
        self.PGAPEAKDET2 = RM_Field_AGC_STATUS1_PGAPEAKDET2(self)
        self.zz_fdict['PGAPEAKDET2'] = self.PGAPEAKDET2
        self.PGAPEAKDET1 = RM_Field_AGC_STATUS1_PGAPEAKDET1(self)
        self.zz_fdict['PGAPEAKDET1'] = self.PGAPEAKDET1
        self.CFLOOPSTATE = RM_Field_AGC_STATUS1_CFLOOPSTATE(self)
        self.zz_fdict['CFLOOPSTATE'] = self.CFLOOPSTATE
        self.FASTLOOPSTATE = RM_Field_AGC_STATUS1_FASTLOOPSTATE(self)
        self.zz_fdict['FASTLOOPSTATE'] = self.FASTLOOPSTATE
        self.RFPEAKDETRAW = RM_Field_AGC_STATUS1_RFPEAKDETRAW(self)
        self.zz_fdict['RFPEAKDETRAW'] = self.RFPEAKDETRAW
        self.RFPEAKDET2 = RM_Field_AGC_STATUS1_RFPEAKDET2(self)
        self.zz_fdict['RFPEAKDET2'] = self.RFPEAKDET2
        self.RFPEAKDET1 = RM_Field_AGC_STATUS1_RFPEAKDET1(self)
        self.zz_fdict['RFPEAKDET1'] = self.RFPEAKDET1
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RSSI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSI, self).__init__(rmio, label,
            0x40087000, 0x008,
            'RSSI', 'AGC.RSSI', 'read-only',
            u"",
            0x00008000, 0x0000FFC0)

        self.RSSIFRAC = RM_Field_AGC_RSSI_RSSIFRAC(self)
        self.zz_fdict['RSSIFRAC'] = self.RSSIFRAC
        self.RSSIINT = RM_Field_AGC_RSSI_RSSIINT(self)
        self.zz_fdict['RSSIINT'] = self.RSSIINT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_FRAMERSSI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_FRAMERSSI, self).__init__(rmio, label,
            0x40087000, 0x00C,
            'FRAMERSSI', 'AGC.FRAMERSSI', 'read-only',
            u"",
            0x00008000, 0x0000FFC0)

        self.FRAMERSSIFRAC = RM_Field_AGC_FRAMERSSI_FRAMERSSIFRAC(self)
        self.zz_fdict['FRAMERSSIFRAC'] = self.FRAMERSSIFRAC
        self.FRAMERSSIINT = RM_Field_AGC_FRAMERSSI_FRAMERSSIINT(self)
        self.zz_fdict['FRAMERSSIINT'] = self.FRAMERSSIINT
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL0, self).__init__(rmio, label,
            0x40087000, 0x014,
            'CTRL0', 'AGC.CTRL0', 'read-write',
            u"",
            0x0082707F, 0x03FFFFFF)

        self.PWRTARGET = RM_Field_AGC_CTRL0_PWRTARGET(self)
        self.zz_fdict['PWRTARGET'] = self.PWRTARGET
        self.MODE = RM_Field_AGC_CTRL0_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.RSSISHIFT = RM_Field_AGC_CTRL0_RSSISHIFT(self)
        self.zz_fdict['RSSISHIFT'] = self.RSSISHIFT
        self.DISCFLOOPADJ = RM_Field_AGC_CTRL0_DISCFLOOPADJ(self)
        self.zz_fdict['DISCFLOOPADJ'] = self.DISCFLOOPADJ
        self.USEADCSATDET = RM_Field_AGC_CTRL0_USEADCSATDET(self)
        self.zz_fdict['USEADCSATDET'] = self.USEADCSATDET
        self.AGCCLKUNDIVREQ = RM_Field_AGC_CTRL0_AGCCLKUNDIVREQ(self)
        self.zz_fdict['AGCCLKUNDIVREQ'] = self.AGCCLKUNDIVREQ
        self.DISRESETCHPWR = RM_Field_AGC_CTRL0_DISRESETCHPWR(self)
        self.zz_fdict['DISRESETCHPWR'] = self.DISRESETCHPWR
        self.ADCRESETDURATION = RM_Field_AGC_CTRL0_ADCRESETDURATION(self)
        self.zz_fdict['ADCRESETDURATION'] = self.ADCRESETDURATION
        self.ENRSSIRESET = RM_Field_AGC_CTRL0_ENRSSIRESET(self)
        self.zz_fdict['ENRSSIRESET'] = self.ENRSSIRESET
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_CTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_CTRL1, self).__init__(rmio, label,
            0x40087000, 0x018,
            'CTRL1', 'AGC.CTRL1', 'read-write',
            u"",
            0x00001300, 0xFFFFFFFF)

        self.CCATHRSH = RM_Field_AGC_CTRL1_CCATHRSH(self)
        self.zz_fdict['CCATHRSH'] = self.CCATHRSH
        self.RSSIPERIOD = RM_Field_AGC_CTRL1_RSSIPERIOD(self)
        self.zz_fdict['RSSIPERIOD'] = self.RSSIPERIOD
        self.AGCPERIOD = RM_Field_AGC_CTRL1_AGCPERIOD(self)
        self.zz_fdict['AGCPERIOD'] = self.AGCPERIOD
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
            0x40087000, 0x01C,
            'CTRL2', 'AGC.CTRL2', 'read-write',
            u"",
            0x05050060, 0xFFFFFFE0)

        self.HYST = RM_Field_AGC_CTRL2_HYST(self)
        self.zz_fdict['HYST'] = self.HYST
        self.DMASEL = RM_Field_AGC_CTRL2_DMASEL(self)
        self.zz_fdict['DMASEL'] = self.DMASEL
        self.MAXPWRVAR = RM_Field_AGC_CTRL2_MAXPWRVAR(self)
        self.zz_fdict['MAXPWRVAR'] = self.MAXPWRVAR
        self.FASTLOOPDEL = RM_Field_AGC_CTRL2_FASTLOOPDEL(self)
        self.zz_fdict['FASTLOOPDEL'] = self.FASTLOOPDEL
        self.CFLOOPDEL = RM_Field_AGC_CTRL2_CFLOOPDEL(self)
        self.zz_fdict['CFLOOPDEL'] = self.CFLOOPDEL
        self.RSSIFAST = RM_Field_AGC_CTRL2_RSSIFAST(self)
        self.zz_fdict['RSSIFAST'] = self.RSSIFAST
        self.ADCRSTFASTLOOP = RM_Field_AGC_CTRL2_ADCRSTFASTLOOP(self)
        self.zz_fdict['ADCRSTFASTLOOP'] = self.ADCRSTFASTLOOP
        self.ADCRSTSTARTUP = RM_Field_AGC_CTRL2_ADCRSTSTARTUP(self)
        self.zz_fdict['ADCRSTSTARTUP'] = self.ADCRSTSTARTUP
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RSSISTEPTHR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RSSISTEPTHR, self).__init__(rmio, label,
            0x40087000, 0x020,
            'RSSISTEPTHR', 'AGC.RSSISTEPTHR', 'read-write',
            u"",
            0x00000000, 0xFFF1FFFF)

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
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IFPEAKDET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IFPEAKDET, self).__init__(rmio, label,
            0x40087000, 0x024,
            'IFPEAKDET', 'AGC.IFPEAKDET', 'read-write',
            u"",
            0x00000082, 0x000003FF)

        self.PKDTHRESH1 = RM_Field_AGC_IFPEAKDET_PKDTHRESH1(self)
        self.zz_fdict['PKDTHRESH1'] = self.PKDTHRESH1
        self.PKDTHRESH2 = RM_Field_AGC_IFPEAKDET_PKDTHRESH2(self)
        self.zz_fdict['PKDTHRESH2'] = self.PKDTHRESH2
        self.MODEPKD = RM_Field_AGC_IFPEAKDET_MODEPKD(self)
        self.zz_fdict['MODEPKD'] = self.MODEPKD
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_MANGAIN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_MANGAIN, self).__init__(rmio, label,
            0x40087000, 0x028,
            'MANGAIN', 'AGC.MANGAIN', 'read-write',
            u"",
            0x00000000, 0x7FEFFEBF)

        self.MANGAININDEX = RM_Field_AGC_MANGAIN_MANGAININDEX(self)
        self.zz_fdict['MANGAININDEX'] = self.MANGAININDEX
        self.MANGAININDEXEN = RM_Field_AGC_MANGAIN_MANGAININDEXEN(self)
        self.zz_fdict['MANGAININDEXEN'] = self.MANGAININDEXEN
        self.MANGAINIFPGA = RM_Field_AGC_MANGAIN_MANGAINIFPGA(self)
        self.zz_fdict['MANGAINIFPGA'] = self.MANGAINIFPGA
        self.MANGAINLNADEGEN = RM_Field_AGC_MANGAIN_MANGAINLNADEGEN(self)
        self.zz_fdict['MANGAINLNADEGEN'] = self.MANGAINLNADEGEN
        self.MANGAINLNASLICES = RM_Field_AGC_MANGAIN_MANGAINLNASLICES(self)
        self.zz_fdict['MANGAINLNASLICES'] = self.MANGAINLNASLICES
        self.MANGAINLNAATTEN = RM_Field_AGC_MANGAIN_MANGAINLNAATTEN(self)
        self.zz_fdict['MANGAINLNAATTEN'] = self.MANGAINLNAATTEN
        self.MANGAINLNASLICESREG = RM_Field_AGC_MANGAIN_MANGAINLNASLICESREG(self)
        self.zz_fdict['MANGAINLNASLICESREG'] = self.MANGAINLNASLICESREG
        self.MANGAINEN = RM_Field_AGC_MANGAIN_MANGAINEN(self)
        self.zz_fdict['MANGAINEN'] = self.MANGAINEN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_RFPEAKDET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_RFPEAKDET, self).__init__(rmio, label,
            0x40087000, 0x02C,
            'RFPEAKDET', 'AGC.RFPEAKDET', 'read-write',
            u"",
            0x000000D5, 0x000001FF)

        self.RFPKDTHRESH1 = RM_Field_AGC_RFPEAKDET_RFPKDTHRESH1(self)
        self.zz_fdict['RFPKDTHRESH1'] = self.RFPKDTHRESH1
        self.RFPKDTHRESH2 = RM_Field_AGC_RFPEAKDET_RFPKDTHRESH2(self)
        self.zz_fdict['RFPKDTHRESH2'] = self.RFPKDTHRESH2
        self.RFPKDSWITCHDEL = RM_Field_AGC_RFPEAKDET_RFPKDSWITCHDEL(self)
        self.zz_fdict['RFPKDSWITCHDEL'] = self.RFPKDSWITCHDEL
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IF, self).__init__(rmio, label,
            0x40087000, 0x030,
            'IF', 'AGC.IF', 'read-only',
            u"",
            0x00000000, 0x0000007D)

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


class RM_Register_AGC_IFS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IFS, self).__init__(rmio, label,
            0x40087000, 0x034,
            'IFS', 'AGC.IFS', 'write-only',
            u"",
            0x00000000, 0x0000007D)

        self.RSSIVALID = RM_Field_AGC_IFS_RSSIVALID(self)
        self.zz_fdict['RSSIVALID'] = self.RSSIVALID
        self.CCA = RM_Field_AGC_IFS_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.RSSIPOSSTEP = RM_Field_AGC_IFS_RSSIPOSSTEP(self)
        self.zz_fdict['RSSIPOSSTEP'] = self.RSSIPOSSTEP
        self.RSSINEGSTEP = RM_Field_AGC_IFS_RSSINEGSTEP(self)
        self.zz_fdict['RSSINEGSTEP'] = self.RSSINEGSTEP
        self.RSSIDONE = RM_Field_AGC_IFS_RSSIDONE(self)
        self.zz_fdict['RSSIDONE'] = self.RSSIDONE
        self.SHORTRSSIPOSSTEP = RM_Field_AGC_IFS_SHORTRSSIPOSSTEP(self)
        self.zz_fdict['SHORTRSSIPOSSTEP'] = self.SHORTRSSIPOSSTEP
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IFC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IFC, self).__init__(rmio, label,
            0x40087000, 0x038,
            'IFC', 'AGC.IFC', 'write-only',
            u"",
            0x00000000, 0x0000007D)

        self.RSSIVALID = RM_Field_AGC_IFC_RSSIVALID(self)
        self.zz_fdict['RSSIVALID'] = self.RSSIVALID
        self.CCA = RM_Field_AGC_IFC_CCA(self)
        self.zz_fdict['CCA'] = self.CCA
        self.RSSIPOSSTEP = RM_Field_AGC_IFC_RSSIPOSSTEP(self)
        self.zz_fdict['RSSIPOSSTEP'] = self.RSSIPOSSTEP
        self.RSSINEGSTEP = RM_Field_AGC_IFC_RSSINEGSTEP(self)
        self.zz_fdict['RSSINEGSTEP'] = self.RSSINEGSTEP
        self.RSSIDONE = RM_Field_AGC_IFC_RSSIDONE(self)
        self.zz_fdict['RSSIDONE'] = self.RSSIDONE
        self.SHORTRSSIPOSSTEP = RM_Field_AGC_IFC_SHORTRSSIPOSSTEP(self)
        self.zz_fdict['SHORTRSSIPOSSTEP'] = self.SHORTRSSIPOSSTEP
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_IEN, self).__init__(rmio, label,
            0x40087000, 0x03C,
            'IEN', 'AGC.IEN', 'read-write',
            u"",
            0x00000000, 0x0000007D)

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
            0x40087000, 0x040,
            'CMD', 'AGC.CMD', 'write-only',
            u"",
            0x00000000, 0x00000001)

        self.RSSISTART = RM_Field_AGC_CMD_RSSISTART(self)
        self.zz_fdict['RSSISTART'] = self.RSSISTART
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINRANGE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINRANGE, self).__init__(rmio, label,
            0x40087000, 0x048,
            'GAINRANGE', 'AGC.GAINRANGE', 'read-write',
            u"",
            0x00003D3C, 0x00003FFF)

        self.MAXGAIN = RM_Field_AGC_GAINRANGE_MAXGAIN(self)
        self.zz_fdict['MAXGAIN'] = self.MAXGAIN
        self.MINGAIN = RM_Field_AGC_GAINRANGE_MINGAIN(self)
        self.zz_fdict['MINGAIN'] = self.MINGAIN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAININDEX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAININDEX, self).__init__(rmio, label,
            0x40087000, 0x04C,
            'GAININDEX', 'AGC.GAININDEX', 'read-write',
            u"",
            0x000019BC, 0x000FFFFF)

        self.NUMINDEXPGA = RM_Field_AGC_GAININDEX_NUMINDEXPGA(self)
        self.zz_fdict['NUMINDEXPGA'] = self.NUMINDEXPGA
        self.NUMINDEXDEGEN = RM_Field_AGC_GAININDEX_NUMINDEXDEGEN(self)
        self.zz_fdict['NUMINDEXDEGEN'] = self.NUMINDEXDEGEN
        self.NUMINDEXSLICES = RM_Field_AGC_GAININDEX_NUMINDEXSLICES(self)
        self.zz_fdict['NUMINDEXSLICES'] = self.NUMINDEXSLICES
        self.NUMINDEXATTEN = RM_Field_AGC_GAININDEX_NUMINDEXATTEN(self)
        self.zz_fdict['NUMINDEXATTEN'] = self.NUMINDEXATTEN
        self.MININDEXPGA = RM_Field_AGC_GAININDEX_MININDEXPGA(self)
        self.zz_fdict['MININDEXPGA'] = self.MININDEXPGA
        self.MININDEXDEGEN = RM_Field_AGC_GAININDEX_MININDEXDEGEN(self)
        self.zz_fdict['MININDEXDEGEN'] = self.MININDEXDEGEN
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_SLICECODE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_SLICECODE, self).__init__(rmio, label,
            0x40087000, 0x050,
            'SLICECODE', 'AGC.SLICECODE', 'read-write',
            u"",
            0x0CA86543, 0x0FFFFFFF)

        self.SLICECODEINDEX0 = RM_Field_AGC_SLICECODE_SLICECODEINDEX0(self)
        self.zz_fdict['SLICECODEINDEX0'] = self.SLICECODEINDEX0
        self.SLICECODEINDEX1 = RM_Field_AGC_SLICECODE_SLICECODEINDEX1(self)
        self.zz_fdict['SLICECODEINDEX1'] = self.SLICECODEINDEX1
        self.SLICECODEINDEX2 = RM_Field_AGC_SLICECODE_SLICECODEINDEX2(self)
        self.zz_fdict['SLICECODEINDEX2'] = self.SLICECODEINDEX2
        self.SLICECODEINDEX3 = RM_Field_AGC_SLICECODE_SLICECODEINDEX3(self)
        self.zz_fdict['SLICECODEINDEX3'] = self.SLICECODEINDEX3
        self.SLICECODEINDEX4 = RM_Field_AGC_SLICECODE_SLICECODEINDEX4(self)
        self.zz_fdict['SLICECODEINDEX4'] = self.SLICECODEINDEX4
        self.SLICECODEINDEX5 = RM_Field_AGC_SLICECODE_SLICECODEINDEX5(self)
        self.zz_fdict['SLICECODEINDEX5'] = self.SLICECODEINDEX5
        self.SLICECODEINDEX6 = RM_Field_AGC_SLICECODE_SLICECODEINDEX6(self)
        self.zz_fdict['SLICECODEINDEX6'] = self.SLICECODEINDEX6
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_ATTENCODE1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_ATTENCODE1, self).__init__(rmio, label,
            0x40087000, 0x054,
            'ATTENCODE1', 'AGC.ATTENCODE1', 'read-write',
            u"",
            0x06543210, 0x0FFFFFFF)

        self.ATTENCODEINDEX0 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX0(self)
        self.zz_fdict['ATTENCODEINDEX0'] = self.ATTENCODEINDEX0
        self.ATTENCODEINDEX1 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX1(self)
        self.zz_fdict['ATTENCODEINDEX1'] = self.ATTENCODEINDEX1
        self.ATTENCODEINDEX2 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX2(self)
        self.zz_fdict['ATTENCODEINDEX2'] = self.ATTENCODEINDEX2
        self.ATTENCODEINDEX3 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX3(self)
        self.zz_fdict['ATTENCODEINDEX3'] = self.ATTENCODEINDEX3
        self.ATTENCODEINDEX4 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX4(self)
        self.zz_fdict['ATTENCODEINDEX4'] = self.ATTENCODEINDEX4
        self.ATTENCODEINDEX5 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX5(self)
        self.zz_fdict['ATTENCODEINDEX5'] = self.ATTENCODEINDEX5
        self.ATTENCODEINDEX6 = RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX6(self)
        self.zz_fdict['ATTENCODEINDEX6'] = self.ATTENCODEINDEX6
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_ATTENCODE2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_ATTENCODE2, self).__init__(rmio, label,
            0x40087000, 0x058,
            'ATTENCODE2', 'AGC.ATTENCODE2', 'read-write',
            u"",
            0x18B52507, 0x3FFFFFFF)

        self.ATTENCODEINDEX7 = RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX7(self)
        self.zz_fdict['ATTENCODEINDEX7'] = self.ATTENCODEINDEX7
        self.ATTENCODEINDEX8 = RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX8(self)
        self.zz_fdict['ATTENCODEINDEX8'] = self.ATTENCODEINDEX8
        self.ATTENCODEINDEX9 = RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX9(self)
        self.zz_fdict['ATTENCODEINDEX9'] = self.ATTENCODEINDEX9
        self.ATTENCODEINDEX10 = RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX10(self)
        self.zz_fdict['ATTENCODEINDEX10'] = self.ATTENCODEINDEX10
        self.ATTENCODEINDEX11 = RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX11(self)
        self.zz_fdict['ATTENCODEINDEX11'] = self.ATTENCODEINDEX11
        self.ATTENCODEINDEX12 = RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX12(self)
        self.zz_fdict['ATTENCODEINDEX12'] = self.ATTENCODEINDEX12
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_ATTENCODE3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_ATTENCODE3, self).__init__(rmio, label,
            0x40087000, 0x05C,
            'ATTENCODE3', 'AGC.ATTENCODE3', 'read-write',
            u"",
            0x25183DCD, 0x3FFFFFFF)

        self.ATTENCODEINDEX13 = RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX13(self)
        self.zz_fdict['ATTENCODEINDEX13'] = self.ATTENCODEINDEX13
        self.ATTENCODEINDEX14 = RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX14(self)
        self.zz_fdict['ATTENCODEINDEX14'] = self.ATTENCODEINDEX14
        self.ATTENCODEINDEX15 = RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX15(self)
        self.zz_fdict['ATTENCODEINDEX15'] = self.ATTENCODEINDEX15
        self.ATTENCODEINDEX16 = RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX16(self)
        self.zz_fdict['ATTENCODEINDEX16'] = self.ATTENCODEINDEX16
        self.ATTENCODEINDEX17 = RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX17(self)
        self.zz_fdict['ATTENCODEINDEX17'] = self.ATTENCODEINDEX17
        self.ATTENCODEINDEX18 = RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX18(self)
        self.zz_fdict['ATTENCODEINDEX18'] = self.ATTENCODEINDEX18
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINERROR1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINERROR1, self).__init__(rmio, label,
            0x40087000, 0x060,
            'GAINERROR1', 'AGC.GAINERROR1', 'read-write',
            u"",
            0x00000000, 0x07FFFFFF)

        self.DEGENERRINDEX0 = RM_Field_AGC_GAINERROR1_DEGENERRINDEX0(self)
        self.zz_fdict['DEGENERRINDEX0'] = self.DEGENERRINDEX0
        self.DEGENERRINDEX1 = RM_Field_AGC_GAINERROR1_DEGENERRINDEX1(self)
        self.zz_fdict['DEGENERRINDEX1'] = self.DEGENERRINDEX1
        self.DEGENERRINDEX2 = RM_Field_AGC_GAINERROR1_DEGENERRINDEX2(self)
        self.zz_fdict['DEGENERRINDEX2'] = self.DEGENERRINDEX2
        self.SLICEERRINDEX0 = RM_Field_AGC_GAINERROR1_SLICEERRINDEX0(self)
        self.zz_fdict['SLICEERRINDEX0'] = self.SLICEERRINDEX0
        self.SLICEERRINDEX1 = RM_Field_AGC_GAINERROR1_SLICEERRINDEX1(self)
        self.zz_fdict['SLICEERRINDEX1'] = self.SLICEERRINDEX1
        self.SLICEERRINDEX2 = RM_Field_AGC_GAINERROR1_SLICEERRINDEX2(self)
        self.zz_fdict['SLICEERRINDEX2'] = self.SLICEERRINDEX2
        self.SLICEERRINDEX3 = RM_Field_AGC_GAINERROR1_SLICEERRINDEX3(self)
        self.zz_fdict['SLICEERRINDEX3'] = self.SLICEERRINDEX3
        self.SLICEERRINDEX4 = RM_Field_AGC_GAINERROR1_SLICEERRINDEX4(self)
        self.zz_fdict['SLICEERRINDEX4'] = self.SLICEERRINDEX4
        self.SLICEERRINDEX5 = RM_Field_AGC_GAINERROR1_SLICEERRINDEX5(self)
        self.zz_fdict['SLICEERRINDEX5'] = self.SLICEERRINDEX5
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINERROR2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINERROR2, self).__init__(rmio, label,
            0x40087000, 0x064,
            'GAINERROR2', 'AGC.GAINERROR2', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF)

        self.ATTENERRINDEX0 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX0(self)
        self.zz_fdict['ATTENERRINDEX0'] = self.ATTENERRINDEX0
        self.ATTENERRINDEX1 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX1(self)
        self.zz_fdict['ATTENERRINDEX1'] = self.ATTENERRINDEX1
        self.ATTENERRINDEX2 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX2(self)
        self.zz_fdict['ATTENERRINDEX2'] = self.ATTENERRINDEX2
        self.ATTENERRINDEX3 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX3(self)
        self.zz_fdict['ATTENERRINDEX3'] = self.ATTENERRINDEX3
        self.ATTENERRINDEX4 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX4(self)
        self.zz_fdict['ATTENERRINDEX4'] = self.ATTENERRINDEX4
        self.ATTENERRINDEX5 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX5(self)
        self.zz_fdict['ATTENERRINDEX5'] = self.ATTENERRINDEX5
        self.ATTENERRINDEX6 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX6(self)
        self.zz_fdict['ATTENERRINDEX6'] = self.ATTENERRINDEX6
        self.ATTENERRINDEX7 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX7(self)
        self.zz_fdict['ATTENERRINDEX7'] = self.ATTENERRINDEX7
        self.ATTENERRINDEX8 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX8(self)
        self.zz_fdict['ATTENERRINDEX8'] = self.ATTENERRINDEX8
        self.ATTENERRINDEX9 = RM_Field_AGC_GAINERROR2_ATTENERRINDEX9(self)
        self.zz_fdict['ATTENERRINDEX9'] = self.ATTENERRINDEX9
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINERROR3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINERROR3, self).__init__(rmio, label,
            0x40087000, 0x068,
            'GAINERROR3', 'AGC.GAINERROR3', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF)

        self.ATTENERRINDEX10 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX10(self)
        self.zz_fdict['ATTENERRINDEX10'] = self.ATTENERRINDEX10
        self.ATTENERRINDEX11 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX11(self)
        self.zz_fdict['ATTENERRINDEX11'] = self.ATTENERRINDEX11
        self.ATTENERRINDEX12 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX12(self)
        self.zz_fdict['ATTENERRINDEX12'] = self.ATTENERRINDEX12
        self.ATTENERRINDEX13 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX13(self)
        self.zz_fdict['ATTENERRINDEX13'] = self.ATTENERRINDEX13
        self.ATTENERRINDEX14 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX14(self)
        self.zz_fdict['ATTENERRINDEX14'] = self.ATTENERRINDEX14
        self.ATTENERRINDEX15 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX15(self)
        self.zz_fdict['ATTENERRINDEX15'] = self.ATTENERRINDEX15
        self.ATTENERRINDEX16 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX16(self)
        self.zz_fdict['ATTENERRINDEX16'] = self.ATTENERRINDEX16
        self.ATTENERRINDEX17 = RM_Field_AGC_GAINERROR3_ATTENERRINDEX17(self)
        self.zz_fdict['ATTENERRINDEX17'] = self.ATTENERRINDEX17
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_MANUALCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_MANUALCTRL, self).__init__(rmio, label,
            0x40087000, 0x06C,
            'MANUALCTRL', 'AGC.MANUALCTRL', 'read-write',
            u"",
            0x00000000, 0x000001FF)

        self.ENMANUALRSTPKD = RM_Field_AGC_MANUALCTRL_ENMANUALRSTPKD(self)
        self.zz_fdict['ENMANUALRSTPKD'] = self.ENMANUALRSTPKD
        self.MANUALRSTPKD = RM_Field_AGC_MANUALCTRL_MANUALRSTPKD(self)
        self.zz_fdict['MANUALRSTPKD'] = self.MANUALRSTPKD
        self.ENMANUALSLICESSEL = RM_Field_AGC_MANUALCTRL_ENMANUALSLICESSEL(self)
        self.zz_fdict['ENMANUALSLICESSEL'] = self.ENMANUALSLICESSEL
        self.MANUALSLICESSEL = RM_Field_AGC_MANUALCTRL_MANUALSLICESSEL(self)
        self.zz_fdict['MANUALSLICESSEL'] = self.MANUALSLICESSEL
        self.ENMANUALSLICESDUMP = RM_Field_AGC_MANUALCTRL_ENMANUALSLICESDUMP(self)
        self.zz_fdict['ENMANUALSLICESDUMP'] = self.ENMANUALSLICESDUMP
        self.MANUALSLICESDUMP = RM_Field_AGC_MANUALCTRL_MANUALSLICESDUMP(self)
        self.zz_fdict['MANUALSLICESDUMP'] = self.MANUALSLICESDUMP
        self.ENMANUALRSTRFPKD = RM_Field_AGC_MANUALCTRL_ENMANUALRSTRFPKD(self)
        self.zz_fdict['ENMANUALRSTRFPKD'] = self.ENMANUALRSTRFPKD
        self.MANUALRSTRFPKD = RM_Field_AGC_MANUALCTRL_MANUALRSTRFPKD(self)
        self.zz_fdict['MANUALRSTRFPKD'] = self.MANUALRSTRFPKD
        self.ENMANUALRFPKDTHRESH = RM_Field_AGC_MANUALCTRL_ENMANUALRFPKDTHRESH(self)
        self.zz_fdict['ENMANUALRFPKDTHRESH'] = self.ENMANUALRFPKDTHRESH
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_GAINSTEPLIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_GAINSTEPLIM, self).__init__(rmio, label,
            0x40087000, 0x070,
            'GAINSTEPLIM', 'AGC.GAINSTEPLIM', 'read-write',
            u"",
            0x00010103, 0x7FFFFFFF)

        self.FASTSTEPDOWN = RM_Field_AGC_GAINSTEPLIM_FASTSTEPDOWN(self)
        self.zz_fdict['FASTSTEPDOWN'] = self.FASTSTEPDOWN
        self.EN2XFASTSTEPDOWN = RM_Field_AGC_GAINSTEPLIM_EN2XFASTSTEPDOWN(self)
        self.zz_fdict['EN2XFASTSTEPDOWN'] = self.EN2XFASTSTEPDOWN
        self.FASTSTEPUP = RM_Field_AGC_GAINSTEPLIM_FASTSTEPUP(self)
        self.zz_fdict['FASTSTEPUP'] = self.FASTSTEPUP
        self.EN2XFASTSTEPUP = RM_Field_AGC_GAINSTEPLIM_EN2XFASTSTEPUP(self)
        self.zz_fdict['EN2XFASTSTEPUP'] = self.EN2XFASTSTEPUP
        self.CFLOOPSTEPMAX = RM_Field_AGC_GAINSTEPLIM_CFLOOPSTEPMAX(self)
        self.zz_fdict['CFLOOPSTEPMAX'] = self.CFLOOPSTEPMAX
        self.SLOWDECAYCNT = RM_Field_AGC_GAINSTEPLIM_SLOWDECAYCNT(self)
        self.zz_fdict['SLOWDECAYCNT'] = self.SLOWDECAYCNT
        self.ADCATTENCODE = RM_Field_AGC_GAINSTEPLIM_ADCATTENCODE(self)
        self.zz_fdict['ADCATTENCODE'] = self.ADCATTENCODE
        self.ADCATTENMODE = RM_Field_AGC_GAINSTEPLIM_ADCATTENMODE(self)
        self.zz_fdict['ADCATTENMODE'] = self.ADCATTENMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LOOPDEL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LOOPDEL, self).__init__(rmio, label,
            0x40087000, 0x074,
            'LOOPDEL', 'AGC.LOOPDEL', 'read-write',
            u"",
            0x00039C0F, 0x000FFFFF)

        self.PKDWAIT = RM_Field_AGC_LOOPDEL_PKDWAIT(self)
        self.zz_fdict['PKDWAIT'] = self.PKDWAIT
        self.IFPGADEL = RM_Field_AGC_LOOPDEL_IFPGADEL(self)
        self.zz_fdict['IFPGADEL'] = self.IFPGADEL
        self.LNASLICESDEL = RM_Field_AGC_LOOPDEL_LNASLICESDEL(self)
        self.zz_fdict['LNASLICESDEL'] = self.LNASLICESDEL
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_MININDEX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_MININDEX, self).__init__(rmio, label,
            0x40087000, 0x078,
            'MININDEX', 'AGC.MININDEX', 'read-write',
            u"",
            0x00552300, 0x00FFFFFF)

        self.INDEXMINATTEN = RM_Field_AGC_MININDEX_INDEXMINATTEN(self)
        self.zz_fdict['INDEXMINATTEN'] = self.INDEXMINATTEN
        self.INDEXMINSLICES = RM_Field_AGC_MININDEX_INDEXMINSLICES(self)
        self.zz_fdict['INDEXMINSLICES'] = self.INDEXMINSLICES
        self.INDEXMINDEGEN = RM_Field_AGC_MININDEX_INDEXMINDEGEN(self)
        self.zz_fdict['INDEXMINDEGEN'] = self.INDEXMINDEGEN
        self.INDEXMINPGA = RM_Field_AGC_MININDEX_INDEXMINPGA(self)
        self.zz_fdict['INDEXMINPGA'] = self.INDEXMINPGA
        self.__dict__['zz_frozen'] = True


class RM_Register_AGC_LBT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_AGC_LBT, self).__init__(rmio, label,
            0x40087000, 0x080,
            'LBT', 'AGC.LBT', 'read-write',
            u"",
            0x00000000, 0x0000007F)

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
            0x40087000, 0x084,
            'MIRRORIF', 'AGC.MIRRORIF', 'read-write',
            u"",
            0x00000000, 0x0000000F)

        self.RSSIPOSSTEPM = RM_Field_AGC_MIRRORIF_RSSIPOSSTEPM(self)
        self.zz_fdict['RSSIPOSSTEPM'] = self.RSSIPOSSTEPM
        self.RSSINEGSTEPM = RM_Field_AGC_MIRRORIF_RSSINEGSTEPM(self)
        self.zz_fdict['RSSINEGSTEPM'] = self.RSSINEGSTEPM
        self.SHORTRSSIPOSSTEPM = RM_Field_AGC_MIRRORIF_SHORTRSSIPOSSTEPM(self)
        self.zz_fdict['SHORTRSSIPOSSTEPM'] = self.SHORTRSSIPOSSTEPM
        self.IFMIRRORCLEAR = RM_Field_AGC_MIRRORIF_IFMIRRORCLEAR(self)
        self.zz_fdict['IFMIRRORCLEAR'] = self.IFMIRRORCLEAR
        self.__dict__['zz_frozen'] = True


