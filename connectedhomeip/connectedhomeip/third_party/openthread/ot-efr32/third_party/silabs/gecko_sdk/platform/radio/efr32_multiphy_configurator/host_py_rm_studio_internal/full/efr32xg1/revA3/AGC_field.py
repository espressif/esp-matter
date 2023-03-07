
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_AGC_STATUS0_GAININDEX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_GAININDEX, self).__init__(register,
            'GAININDEX', 'AGC.STATUS0.GAININDEX', 'read-only',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS0_CCA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_CCA, self).__init__(register,
            'CCA', 'AGC.STATUS0.CCA', 'read-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS0_GAINOK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_GAINOK, self).__init__(register,
            'GAINOK', 'AGC.STATUS0.GAINOK', 'read-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS0_GAINIFPGA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_GAINIFPGA, self).__init__(register,
            'GAINIFPGA', 'AGC.STATUS0.GAINIFPGA', 'read-only',
            u"",
            13, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS0_GAINLNADEGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_GAINLNADEGEN, self).__init__(register,
            'GAINLNADEGEN', 'AGC.STATUS0.GAINLNADEGEN', 'read-only',
            u"",
            17, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS0_GAINLNASLICE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_GAINLNASLICE, self).__init__(register,
            'GAINLNASLICE', 'AGC.STATUS0.GAINLNASLICE', 'read-only',
            u"",
            20, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS0_GAINLNAATTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS0_GAINLNAATTEN, self).__init__(register,
            'GAINLNAATTEN', 'AGC.STATUS0.GAINLNAATTEN', 'read-only',
            u"",
            25, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS1_CHPWR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS1_CHPWR, self).__init__(register,
            'CHPWR', 'AGC.STATUS1.CHPWR', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS1_ADCSTABILITYDET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS1_ADCSTABILITYDET, self).__init__(register,
            'ADCSTABILITYDET', 'AGC.STATUS1.ADCSTABILITYDET', 'read-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS1_PGAPEAKDET2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS1_PGAPEAKDET2, self).__init__(register,
            'PGAPEAKDET2', 'AGC.STATUS1.PGAPEAKDET2', 'read-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS1_PGAPEAKDET1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS1_PGAPEAKDET1, self).__init__(register,
            'PGAPEAKDET1', 'AGC.STATUS1.PGAPEAKDET1', 'read-only',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS1_CFLOOPSTATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS1_CFLOOPSTATE, self).__init__(register,
            'CFLOOPSTATE', 'AGC.STATUS1.CFLOOPSTATE', 'read-only',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_STATUS1_FASTLOOPSTATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_STATUS1_FASTLOOPSTATE, self).__init__(register,
            'FASTLOOPSTATE', 'AGC.STATUS1.FASTLOOPSTATE', 'read-only',
            u"",
            14, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_RSSI_RSSIFRAC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_RSSI_RSSIFRAC, self).__init__(register,
            'RSSIFRAC', 'AGC.RSSI.RSSIFRAC', 'read-only',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_RSSI_RSSIINT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_RSSI_RSSIINT, self).__init__(register,
            'RSSIINT', 'AGC.RSSI.RSSIINT', 'read-only',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_FRAMERSSI_FRAMERSSIFRAC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_FRAMERSSI_FRAMERSSIFRAC, self).__init__(register,
            'FRAMERSSIFRAC', 'AGC.FRAMERSSI.FRAMERSSIFRAC', 'read-only',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_FRAMERSSI_FRAMERSSIINT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_FRAMERSSI_FRAMERSSIINT, self).__init__(register,
            'FRAMERSSIINT', 'AGC.FRAMERSSI.FRAMERSSIINT', 'read-only',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_PWRTARGET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_PWRTARGET, self).__init__(register,
            'PWRTARGET', 'AGC.CTRL0.PWRTARGET', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_MODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_MODE, self).__init__(register,
            'MODE', 'AGC.CTRL0.MODE', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_RSSISHIFT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_RSSISHIFT, self).__init__(register,
            'RSSISHIFT', 'AGC.CTRL0.RSSISHIFT', 'read-write',
            u"",
            11, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_DISCFLOOPADJ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_DISCFLOOPADJ, self).__init__(register,
            'DISCFLOOPADJ', 'AGC.CTRL0.DISCFLOOPADJ', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_USEADCSATDET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_USEADCSATDET, self).__init__(register,
            'USEADCSATDET', 'AGC.CTRL0.USEADCSATDET', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_AGCCLKUNDIVREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_AGCCLKUNDIVREQ, self).__init__(register,
            'AGCCLKUNDIVREQ', 'AGC.CTRL0.AGCCLKUNDIVREQ', 'read-write',
            u"",
            21, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_DISRESETCHPWR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_DISRESETCHPWR, self).__init__(register,
            'DISRESETCHPWR', 'AGC.CTRL0.DISRESETCHPWR', 'read-write',
            u"",
            22, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL0_ADCRESETDURATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL0_ADCRESETDURATION, self).__init__(register,
            'ADCRESETDURATION', 'AGC.CTRL0.ADCRESETDURATION', 'read-write',
            u"",
            23, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_CCATHRSH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_CCATHRSH, self).__init__(register,
            'CCATHRSH', 'AGC.CTRL1.CCATHRSH', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_RSSIPERIOD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_RSSIPERIOD, self).__init__(register,
            'RSSIPERIOD', 'AGC.CTRL1.RSSIPERIOD', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_AGCPERIOD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_AGCPERIOD, self).__init__(register,
            'AGCPERIOD', 'AGC.CTRL1.AGCPERIOD', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_SUBPERIOD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_SUBPERIOD, self).__init__(register,
            'SUBPERIOD', 'AGC.CTRL1.SUBPERIOD', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_SUBNUM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_SUBNUM, self).__init__(register,
            'SUBNUM', 'AGC.CTRL1.SUBNUM', 'read-write',
            u"",
            16, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_SUBDEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_SUBDEN, self).__init__(register,
            'SUBDEN', 'AGC.CTRL1.SUBDEN', 'read-write',
            u"",
            21, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL1_SUBINT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL1_SUBINT, self).__init__(register,
            'SUBINT', 'AGC.CTRL1.SUBINT', 'read-write',
            u"",
            26, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_FASTSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_FASTSTEP, self).__init__(register,
            'FASTSTEP', 'AGC.CTRL2.FASTSTEP', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_HYST(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_HYST, self).__init__(register,
            'HYST', 'AGC.CTRL2.HYST', 'read-write',
            u"",
            5, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_DMASEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_DMASEL, self).__init__(register,
            'DMASEL', 'AGC.CTRL2.DMASEL', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_MAXPWRVAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_MAXPWRVAR, self).__init__(register,
            'MAXPWRVAR', 'AGC.CTRL2.MAXPWRVAR', 'read-write',
            u"",
            10, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_FASTLOOPDEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_FASTLOOPDEL, self).__init__(register,
            'FASTLOOPDEL', 'AGC.CTRL2.FASTLOOPDEL', 'read-write',
            u"",
            16, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_CFLOOPDEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_CFLOOPDEL, self).__init__(register,
            'CFLOOPDEL', 'AGC.CTRL2.CFLOOPDEL', 'read-write',
            u"",
            20, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_RSSIFAST(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_RSSIFAST, self).__init__(register,
            'RSSIFAST', 'AGC.CTRL2.RSSIFAST', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_SLOWGAINADJ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_SLOWGAINADJ, self).__init__(register,
            'SLOWGAINADJ', 'AGC.CTRL2.SLOWGAINADJ', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_ADCRSTFASTLOOP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_ADCRSTFASTLOOP, self).__init__(register,
            'ADCRSTFASTLOOP', 'AGC.CTRL2.ADCRSTFASTLOOP', 'read-write',
            u"",
            29, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CTRL2_ADCRSTSTARTUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CTRL2_ADCRSTSTARTUP, self).__init__(register,
            'ADCRSTSTARTUP', 'AGC.CTRL2.ADCRSTSTARTUP', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_RSSISTEPTHR_POSSTEPTHR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_RSSISTEPTHR_POSSTEPTHR, self).__init__(register,
            'POSSTEPTHR', 'AGC.RSSISTEPTHR.POSSTEPTHR', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_RSSISTEPTHR_NEGSTEPTHR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_RSSISTEPTHR_NEGSTEPTHR, self).__init__(register,
            'NEGSTEPTHR', 'AGC.RSSISTEPTHR.NEGSTEPTHR', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFPEAKDET_PKDTHRESH1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFPEAKDET_PKDTHRESH1, self).__init__(register,
            'PKDTHRESH1', 'AGC.IFPEAKDET.PKDTHRESH1', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFPEAKDET_PKDTHRESH2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFPEAKDET_PKDTHRESH2, self).__init__(register,
            'PKDTHRESH2', 'AGC.IFPEAKDET.PKDTHRESH2', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFPEAKDET_MODEPKD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFPEAKDET_MODEPKD, self).__init__(register,
            'MODEPKD', 'AGC.IFPEAKDET.MODEPKD', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAININDEX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAININDEX, self).__init__(register,
            'MANGAININDEX', 'AGC.MANGAIN.MANGAININDEX', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAININDEXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAININDEXEN, self).__init__(register,
            'MANGAININDEXEN', 'AGC.MANGAIN.MANGAININDEXEN', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAINIFPGA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAINIFPGA, self).__init__(register,
            'MANGAINIFPGA', 'AGC.MANGAIN.MANGAINIFPGA', 'read-write',
            u"",
            9, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAINLNADEGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAINLNADEGEN, self).__init__(register,
            'MANGAINLNADEGEN', 'AGC.MANGAIN.MANGAINLNADEGEN', 'read-write',
            u"",
            13, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAINLNASLICES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAINLNASLICES, self).__init__(register,
            'MANGAINLNASLICES', 'AGC.MANGAIN.MANGAINLNASLICES', 'read-write',
            u"",
            16, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAINLNAATTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAINLNAATTEN, self).__init__(register,
            'MANGAINLNAATTEN', 'AGC.MANGAIN.MANGAINLNAATTEN', 'read-write',
            u"",
            21, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAINLNASLICESREG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAINLNASLICESREG, self).__init__(register,
            'MANGAINLNASLICESREG', 'AGC.MANGAIN.MANGAINLNASLICESREG', 'read-write',
            u"",
            26, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANGAIN_MANGAINEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANGAIN_MANGAINEN, self).__init__(register,
            'MANGAINEN', 'AGC.MANGAIN.MANGAINEN', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IF_RSSIVALID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IF_RSSIVALID, self).__init__(register,
            'RSSIVALID', 'AGC.IF.RSSIVALID', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IF_CCA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IF_CCA, self).__init__(register,
            'CCA', 'AGC.IF.CCA', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IF_RSSIPOSSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IF_RSSIPOSSTEP, self).__init__(register,
            'RSSIPOSSTEP', 'AGC.IF.RSSIPOSSTEP', 'read-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IF_RSSINEGSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IF_RSSINEGSTEP, self).__init__(register,
            'RSSINEGSTEP', 'AGC.IF.RSSINEGSTEP', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IF_RSSIDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IF_RSSIDONE, self).__init__(register,
            'RSSIDONE', 'AGC.IF.RSSIDONE', 'read-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFS_RSSIVALID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFS_RSSIVALID, self).__init__(register,
            'RSSIVALID', 'AGC.IFS.RSSIVALID', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFS_CCA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFS_CCA, self).__init__(register,
            'CCA', 'AGC.IFS.CCA', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFS_RSSIPOSSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFS_RSSIPOSSTEP, self).__init__(register,
            'RSSIPOSSTEP', 'AGC.IFS.RSSIPOSSTEP', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFS_RSSINEGSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFS_RSSINEGSTEP, self).__init__(register,
            'RSSINEGSTEP', 'AGC.IFS.RSSINEGSTEP', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFS_RSSIDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFS_RSSIDONE, self).__init__(register,
            'RSSIDONE', 'AGC.IFS.RSSIDONE', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFC_RSSIVALID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFC_RSSIVALID, self).__init__(register,
            'RSSIVALID', 'AGC.IFC.RSSIVALID', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFC_CCA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFC_CCA, self).__init__(register,
            'CCA', 'AGC.IFC.CCA', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFC_RSSIPOSSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFC_RSSIPOSSTEP, self).__init__(register,
            'RSSIPOSSTEP', 'AGC.IFC.RSSIPOSSTEP', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFC_RSSINEGSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFC_RSSINEGSTEP, self).__init__(register,
            'RSSINEGSTEP', 'AGC.IFC.RSSINEGSTEP', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IFC_RSSIDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IFC_RSSIDONE, self).__init__(register,
            'RSSIDONE', 'AGC.IFC.RSSIDONE', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IEN_RSSIVALID(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IEN_RSSIVALID, self).__init__(register,
            'RSSIVALID', 'AGC.IEN.RSSIVALID', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IEN_CCA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IEN_CCA, self).__init__(register,
            'CCA', 'AGC.IEN.CCA', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IEN_RSSIPOSSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IEN_RSSIPOSSTEP, self).__init__(register,
            'RSSIPOSSTEP', 'AGC.IEN.RSSIPOSSTEP', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IEN_RSSINEGSTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IEN_RSSINEGSTEP, self).__init__(register,
            'RSSINEGSTEP', 'AGC.IEN.RSSINEGSTEP', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_IEN_RSSIDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_IEN_RSSIDONE, self).__init__(register,
            'RSSIDONE', 'AGC.IEN.RSSIDONE', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_CMD_RSSISTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_CMD_RSSISTART, self).__init__(register,
            'RSSISTART', 'AGC.CMD.RSSISTART', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINRANGE_MAXGAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINRANGE_MAXGAIN, self).__init__(register,
            'MAXGAIN', 'AGC.GAINRANGE.MAXGAIN', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINRANGE_MINGAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINRANGE_MINGAIN, self).__init__(register,
            'MINGAIN', 'AGC.GAINRANGE.MINGAIN', 'read-write',
            u"",
            7, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAININDEX_NUMINDEXPGA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAININDEX_NUMINDEXPGA, self).__init__(register,
            'NUMINDEXPGA', 'AGC.GAININDEX.NUMINDEXPGA', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAININDEX_NUMINDEXDEGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAININDEX_NUMINDEXDEGEN, self).__init__(register,
            'NUMINDEXDEGEN', 'AGC.GAININDEX.NUMINDEXDEGEN', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAININDEX_NUMINDEXSLICES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAININDEX_NUMINDEXSLICES, self).__init__(register,
            'NUMINDEXSLICES', 'AGC.GAININDEX.NUMINDEXSLICES', 'read-write',
            u"",
            6, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAININDEX_NUMINDEXATTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAININDEX_NUMINDEXATTEN, self).__init__(register,
            'NUMINDEXATTEN', 'AGC.GAININDEX.NUMINDEXATTEN', 'read-write',
            u"",
            9, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAININDEX_MININDEXPGA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAININDEX_MININDEXPGA, self).__init__(register,
            'MININDEXPGA', 'AGC.GAININDEX.MININDEXPGA', 'read-write',
            u"",
            14, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAININDEX_MININDEXDEGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAININDEX_MININDEXDEGEN, self).__init__(register,
            'MININDEXDEGEN', 'AGC.GAININDEX.MININDEXDEGEN', 'read-write',
            u"",
            18, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX0, self).__init__(register,
            'SLICECODEINDEX0', 'AGC.SLICECODE.SLICECODEINDEX0', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX1, self).__init__(register,
            'SLICECODEINDEX1', 'AGC.SLICECODE.SLICECODEINDEX1', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX2, self).__init__(register,
            'SLICECODEINDEX2', 'AGC.SLICECODE.SLICECODEINDEX2', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX3, self).__init__(register,
            'SLICECODEINDEX3', 'AGC.SLICECODE.SLICECODEINDEX3', 'read-write',
            u"",
            12, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX4, self).__init__(register,
            'SLICECODEINDEX4', 'AGC.SLICECODE.SLICECODEINDEX4', 'read-write',
            u"",
            16, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX5, self).__init__(register,
            'SLICECODEINDEX5', 'AGC.SLICECODE.SLICECODEINDEX5', 'read-write',
            u"",
            20, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_SLICECODE_SLICECODEINDEX6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_SLICECODE_SLICECODEINDEX6, self).__init__(register,
            'SLICECODEINDEX6', 'AGC.SLICECODE.SLICECODEINDEX6', 'read-write',
            u"",
            24, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX0, self).__init__(register,
            'ATTENCODEINDEX0', 'AGC.ATTENCODE1.ATTENCODEINDEX0', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX1, self).__init__(register,
            'ATTENCODEINDEX1', 'AGC.ATTENCODE1.ATTENCODEINDEX1', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX2, self).__init__(register,
            'ATTENCODEINDEX2', 'AGC.ATTENCODE1.ATTENCODEINDEX2', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX3, self).__init__(register,
            'ATTENCODEINDEX3', 'AGC.ATTENCODE1.ATTENCODEINDEX3', 'read-write',
            u"",
            12, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX4, self).__init__(register,
            'ATTENCODEINDEX4', 'AGC.ATTENCODE1.ATTENCODEINDEX4', 'read-write',
            u"",
            16, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX5, self).__init__(register,
            'ATTENCODEINDEX5', 'AGC.ATTENCODE1.ATTENCODEINDEX5', 'read-write',
            u"",
            20, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE1_ATTENCODEINDEX6, self).__init__(register,
            'ATTENCODEINDEX6', 'AGC.ATTENCODE1.ATTENCODEINDEX6', 'read-write',
            u"",
            24, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX7(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX7, self).__init__(register,
            'ATTENCODEINDEX7', 'AGC.ATTENCODE2.ATTENCODEINDEX7', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX8(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX8, self).__init__(register,
            'ATTENCODEINDEX8', 'AGC.ATTENCODE2.ATTENCODEINDEX8', 'read-write',
            u"",
            5, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX9(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX9, self).__init__(register,
            'ATTENCODEINDEX9', 'AGC.ATTENCODE2.ATTENCODEINDEX9', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX10(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX10, self).__init__(register,
            'ATTENCODEINDEX10', 'AGC.ATTENCODE2.ATTENCODEINDEX10', 'read-write',
            u"",
            15, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX11(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX11, self).__init__(register,
            'ATTENCODEINDEX11', 'AGC.ATTENCODE2.ATTENCODEINDEX11', 'read-write',
            u"",
            20, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX12(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE2_ATTENCODEINDEX12, self).__init__(register,
            'ATTENCODEINDEX12', 'AGC.ATTENCODE2.ATTENCODEINDEX12', 'read-write',
            u"",
            25, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX13(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX13, self).__init__(register,
            'ATTENCODEINDEX13', 'AGC.ATTENCODE3.ATTENCODEINDEX13', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX14(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX14, self).__init__(register,
            'ATTENCODEINDEX14', 'AGC.ATTENCODE3.ATTENCODEINDEX14', 'read-write',
            u"",
            5, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX15(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX15, self).__init__(register,
            'ATTENCODEINDEX15', 'AGC.ATTENCODE3.ATTENCODEINDEX15', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX16(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX16, self).__init__(register,
            'ATTENCODEINDEX16', 'AGC.ATTENCODE3.ATTENCODEINDEX16', 'read-write',
            u"",
            15, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX17(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX17, self).__init__(register,
            'ATTENCODEINDEX17', 'AGC.ATTENCODE3.ATTENCODEINDEX17', 'read-write',
            u"",
            20, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX18(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_ATTENCODE3_ATTENCODEINDEX18, self).__init__(register,
            'ATTENCODEINDEX18', 'AGC.ATTENCODE3.ATTENCODEINDEX18', 'read-write',
            u"",
            25, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_DEGENERRINDEX0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_DEGENERRINDEX0, self).__init__(register,
            'DEGENERRINDEX0', 'AGC.GAINERROR1.DEGENERRINDEX0', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_DEGENERRINDEX1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_DEGENERRINDEX1, self).__init__(register,
            'DEGENERRINDEX1', 'AGC.GAINERROR1.DEGENERRINDEX1', 'read-write',
            u"",
            3, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_DEGENERRINDEX2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_DEGENERRINDEX2, self).__init__(register,
            'DEGENERRINDEX2', 'AGC.GAINERROR1.DEGENERRINDEX2', 'read-write',
            u"",
            6, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_SLICEERRINDEX0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_SLICEERRINDEX0, self).__init__(register,
            'SLICEERRINDEX0', 'AGC.GAINERROR1.SLICEERRINDEX0', 'read-write',
            u"",
            9, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_SLICEERRINDEX1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_SLICEERRINDEX1, self).__init__(register,
            'SLICEERRINDEX1', 'AGC.GAINERROR1.SLICEERRINDEX1', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_SLICEERRINDEX2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_SLICEERRINDEX2, self).__init__(register,
            'SLICEERRINDEX2', 'AGC.GAINERROR1.SLICEERRINDEX2', 'read-write',
            u"",
            15, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_SLICEERRINDEX3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_SLICEERRINDEX3, self).__init__(register,
            'SLICEERRINDEX3', 'AGC.GAINERROR1.SLICEERRINDEX3', 'read-write',
            u"",
            18, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_SLICEERRINDEX4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_SLICEERRINDEX4, self).__init__(register,
            'SLICEERRINDEX4', 'AGC.GAINERROR1.SLICEERRINDEX4', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR1_SLICEERRINDEX5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR1_SLICEERRINDEX5, self).__init__(register,
            'SLICEERRINDEX5', 'AGC.GAINERROR1.SLICEERRINDEX5', 'read-write',
            u"",
            24, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX0, self).__init__(register,
            'ATTENERRINDEX0', 'AGC.GAINERROR2.ATTENERRINDEX0', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX1, self).__init__(register,
            'ATTENERRINDEX1', 'AGC.GAINERROR2.ATTENERRINDEX1', 'read-write',
            u"",
            3, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX2, self).__init__(register,
            'ATTENERRINDEX2', 'AGC.GAINERROR2.ATTENERRINDEX2', 'read-write',
            u"",
            6, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX3, self).__init__(register,
            'ATTENERRINDEX3', 'AGC.GAINERROR2.ATTENERRINDEX3', 'read-write',
            u"",
            9, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX4, self).__init__(register,
            'ATTENERRINDEX4', 'AGC.GAINERROR2.ATTENERRINDEX4', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX5, self).__init__(register,
            'ATTENERRINDEX5', 'AGC.GAINERROR2.ATTENERRINDEX5', 'read-write',
            u"",
            15, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX6, self).__init__(register,
            'ATTENERRINDEX6', 'AGC.GAINERROR2.ATTENERRINDEX6', 'read-write',
            u"",
            18, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX7(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX7, self).__init__(register,
            'ATTENERRINDEX7', 'AGC.GAINERROR2.ATTENERRINDEX7', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX8(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX8, self).__init__(register,
            'ATTENERRINDEX8', 'AGC.GAINERROR2.ATTENERRINDEX8', 'read-write',
            u"",
            24, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR2_ATTENERRINDEX9(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR2_ATTENERRINDEX9, self).__init__(register,
            'ATTENERRINDEX9', 'AGC.GAINERROR2.ATTENERRINDEX9', 'read-write',
            u"",
            27, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX10(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX10, self).__init__(register,
            'ATTENERRINDEX10', 'AGC.GAINERROR3.ATTENERRINDEX10', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX11(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX11, self).__init__(register,
            'ATTENERRINDEX11', 'AGC.GAINERROR3.ATTENERRINDEX11', 'read-write',
            u"",
            3, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX12(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX12, self).__init__(register,
            'ATTENERRINDEX12', 'AGC.GAINERROR3.ATTENERRINDEX12', 'read-write',
            u"",
            6, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX13(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX13, self).__init__(register,
            'ATTENERRINDEX13', 'AGC.GAINERROR3.ATTENERRINDEX13', 'read-write',
            u"",
            9, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX14(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX14, self).__init__(register,
            'ATTENERRINDEX14', 'AGC.GAINERROR3.ATTENERRINDEX14', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX15(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX15, self).__init__(register,
            'ATTENERRINDEX15', 'AGC.GAINERROR3.ATTENERRINDEX15', 'read-write',
            u"",
            15, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX16(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX16, self).__init__(register,
            'ATTENERRINDEX16', 'AGC.GAINERROR3.ATTENERRINDEX16', 'read-write',
            u"",
            18, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINERROR3_ATTENERRINDEX17(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINERROR3_ATTENERRINDEX17, self).__init__(register,
            'ATTENERRINDEX17', 'AGC.GAINERROR3.ATTENERRINDEX17', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANUALCTRL_ENMANUALRSTPKD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANUALCTRL_ENMANUALRSTPKD, self).__init__(register,
            'ENMANUALRSTPKD', 'AGC.MANUALCTRL.ENMANUALRSTPKD', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANUALCTRL_MANUALRSTPKD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANUALCTRL_MANUALRSTPKD, self).__init__(register,
            'MANUALRSTPKD', 'AGC.MANUALCTRL.MANUALRSTPKD', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANUALCTRL_ENMANUALSLICESSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANUALCTRL_ENMANUALSLICESSEL, self).__init__(register,
            'ENMANUALSLICESSEL', 'AGC.MANUALCTRL.ENMANUALSLICESSEL', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANUALCTRL_MANUALSLICESSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANUALCTRL_MANUALSLICESSEL, self).__init__(register,
            'MANUALSLICESSEL', 'AGC.MANUALCTRL.MANUALSLICESSEL', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANUALCTRL_ENMANUALSLICESDUMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANUALCTRL_ENMANUALSLICESDUMP, self).__init__(register,
            'ENMANUALSLICESDUMP', 'AGC.MANUALCTRL.ENMANUALSLICESDUMP', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MANUALCTRL_MANUALSLICESDUMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MANUALCTRL_MANUALSLICESDUMP, self).__init__(register,
            'MANUALSLICESDUMP', 'AGC.MANUALCTRL.MANUALSLICESDUMP', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINSTEPLIM_FASTSTEPDOWN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINSTEPLIM_FASTSTEPDOWN, self).__init__(register,
            'FASTSTEPDOWN', 'AGC.GAINSTEPLIM.FASTSTEPDOWN', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINSTEPLIM_EN2XFASTSTEPDOWN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINSTEPLIM_EN2XFASTSTEPDOWN, self).__init__(register,
            'EN2XFASTSTEPDOWN', 'AGC.GAINSTEPLIM.EN2XFASTSTEPDOWN', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINSTEPLIM_FASTSTEPUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINSTEPLIM_FASTSTEPUP, self).__init__(register,
            'FASTSTEPUP', 'AGC.GAINSTEPLIM.FASTSTEPUP', 'read-write',
            u"",
            7, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINSTEPLIM_EN2XFASTSTEPUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINSTEPLIM_EN2XFASTSTEPUP, self).__init__(register,
            'EN2XFASTSTEPUP', 'AGC.GAINSTEPLIM.EN2XFASTSTEPUP', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_GAINSTEPLIM_CFLOOPSTEPMAX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_GAINSTEPLIM_CFLOOPSTEPMAX, self).__init__(register,
            'CFLOOPSTEPMAX', 'AGC.GAINSTEPLIM.CFLOOPSTEPMAX', 'read-write',
            u"",
            14, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_LOOPDEL_PKDWAIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_LOOPDEL_PKDWAIT, self).__init__(register,
            'PKDWAIT', 'AGC.LOOPDEL.PKDWAIT', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_LOOPDEL_IFPGADEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_LOOPDEL_IFPGADEL, self).__init__(register,
            'IFPGADEL', 'AGC.LOOPDEL.IFPGADEL', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_LOOPDEL_LNASLICESDEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_LOOPDEL_LNASLICESDEL, self).__init__(register,
            'LNASLICESDEL', 'AGC.LOOPDEL.LNASLICESDEL', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MININDEX_INDEXMINATTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MININDEX_INDEXMINATTEN, self).__init__(register,
            'INDEXMINATTEN', 'AGC.MININDEX.INDEXMINATTEN', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MININDEX_INDEXMINSLICES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MININDEX_INDEXMINSLICES, self).__init__(register,
            'INDEXMINSLICES', 'AGC.MININDEX.INDEXMINSLICES', 'read-write',
            u"",
            6, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MININDEX_INDEXMINDEGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MININDEX_INDEXMINDEGEN, self).__init__(register,
            'INDEXMINDEGEN', 'AGC.MININDEX.INDEXMINDEGEN', 'read-write',
            u"",
            12, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_AGC_MININDEX_INDEXMINPGA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_AGC_MININDEX_INDEXMINPGA, self).__init__(register,
            'INDEXMINPGA', 'AGC.MININDEX.INDEXMINPGA', 'read-write',
            u"",
            18, 6)
        self.__dict__['zz_frozen'] = True


