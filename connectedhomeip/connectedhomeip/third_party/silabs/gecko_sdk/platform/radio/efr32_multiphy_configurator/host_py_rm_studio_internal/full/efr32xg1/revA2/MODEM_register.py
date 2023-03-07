
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . MODEM_field import *


class RM_Register_MODEM_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_STATUS, self).__init__(rmio, label,
            0x40086000, 0x000,
            'STATUS', 'MODEM.STATUS', 'read-only',
            u"",
            0x00000000, 0xFFFF00F7)

        self.DEMODSTATE = RM_Field_MODEM_STATUS_DEMODSTATE(self)
        self.zz_fdict['DEMODSTATE'] = self.DEMODSTATE
        self.FRAMEDETID = RM_Field_MODEM_STATUS_FRAMEDETID(self)
        self.zz_fdict['FRAMEDETID'] = self.FRAMEDETID
        self.ANTSEL = RM_Field_MODEM_STATUS_ANTSEL(self)
        self.zz_fdict['ANTSEL'] = self.ANTSEL
        self.TIMSEQINV = RM_Field_MODEM_STATUS_TIMSEQINV(self)
        self.zz_fdict['TIMSEQINV'] = self.TIMSEQINV
        self.TIMLOSTCAUSE = RM_Field_MODEM_STATUS_TIMLOSTCAUSE(self)
        self.zz_fdict['TIMLOSTCAUSE'] = self.TIMLOSTCAUSE
        self.CORR = RM_Field_MODEM_STATUS_CORR(self)
        self.zz_fdict['CORR'] = self.CORR
        self.WEAKSYMBOLS = RM_Field_MODEM_STATUS_WEAKSYMBOLS(self)
        self.zz_fdict['WEAKSYMBOLS'] = self.WEAKSYMBOLS
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TIMDETSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TIMDETSTATUS, self).__init__(rmio, label,
            0x40086000, 0x004,
            'TIMDETSTATUS', 'MODEM.TIMDETSTATUS', 'read-only',
            u"",
            0x00000000, 0x1F0FFFFF)

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


class RM_Register_MODEM_FREQOFFEST(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_FREQOFFEST, self).__init__(rmio, label,
            0x40086000, 0x008,
            'FREQOFFEST', 'MODEM.FREQOFFEST', 'read-only',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.FREQOFFEST = RM_Field_MODEM_FREQOFFEST_FREQOFFEST(self)
        self.zz_fdict['FREQOFFEST'] = self.FREQOFFEST
        self.POE = RM_Field_MODEM_FREQOFFEST_POE(self)
        self.zz_fdict['POE'] = self.POE
        self.CORRVAL = RM_Field_MODEM_FREQOFFEST_CORRVAL(self)
        self.zz_fdict['CORRVAL'] = self.CORRVAL
        self.SOFTVAL = RM_Field_MODEM_FREQOFFEST_SOFTVAL(self)
        self.zz_fdict['SOFTVAL'] = self.SOFTVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFCADJRX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFCADJRX, self).__init__(rmio, label,
            0x40086000, 0x00C,
            'AFCADJRX', 'MODEM.AFCADJRX', 'read-only',
            u"",
            0x00000000, 0x0007FFFF)

        self.AFCADJRX = RM_Field_MODEM_AFCADJRX_AFCADJRX(self)
        self.zz_fdict['AFCADJRX'] = self.AFCADJRX
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFCADJTX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFCADJTX, self).__init__(rmio, label,
            0x40086000, 0x010,
            'AFCADJTX', 'MODEM.AFCADJTX', 'read-only',
            u"",
            0x00000000, 0x0007FFFF)

        self.AFCADJTX = RM_Field_MODEM_AFCADJTX_AFCADJTX(self)
        self.zz_fdict['AFCADJTX'] = self.AFCADJTX
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_MIXCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_MIXCTRL, self).__init__(rmio, label,
            0x40086000, 0x014,
            'MIXCTRL', 'MODEM.MIXCTRL', 'read-write',
            u"",
            0x00000000, 0x0000001F)

        self.MODE = RM_Field_MODEM_MIXCTRL_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.DIGIQSWAPEN = RM_Field_MODEM_MIXCTRL_DIGIQSWAPEN(self)
        self.zz_fdict['DIGIQSWAPEN'] = self.DIGIQSWAPEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL0, self).__init__(rmio, label,
            0x40086000, 0x018,
            'CTRL0', 'MODEM.CTRL0', 'read-write',
            u"",
            0x00000000, 0xFFFFFBFF)

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
            0x40086000, 0x01C,
            'CTRL1', 'MODEM.CTRL1', 'read-write',
            u"",
            0x00000000, 0xFFFFDFFF)

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
            0x40086000, 0x020,
            'CTRL2', 'MODEM.CTRL2', 'read-write',
            u"",
            0x00001000, 0xFFFFFFFF)

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
        self.PRSDEBUG = RM_Field_MODEM_CTRL2_PRSDEBUG(self)
        self.zz_fdict['PRSDEBUG'] = self.PRSDEBUG
        self.DEVWEIGHTDIS = RM_Field_MODEM_CTRL2_DEVWEIGHTDIS(self)
        self.zz_fdict['DEVWEIGHTDIS'] = self.DEVWEIGHTDIS
        self.DMASEL = RM_Field_MODEM_CTRL2_DMASEL(self)
        self.zz_fdict['DMASEL'] = self.DMASEL
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CTRL3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CTRL3, self).__init__(rmio, label,
            0x40086000, 0x024,
            'CTRL3', 'MODEM.CTRL3', 'read-write',
            u"",
            0x00008000, 0xFFFFFF9F)

        self.PRSDINEN = RM_Field_MODEM_CTRL3_PRSDINEN(self)
        self.zz_fdict['PRSDINEN'] = self.PRSDINEN
        self.PRSDINSEL = RM_Field_MODEM_CTRL3_PRSDINSEL(self)
        self.zz_fdict['PRSDINSEL'] = self.PRSDINSEL
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
            0x40086000, 0x028,
            'CTRL4', 'MODEM.CTRL4', 'read-write',
            u"",
            0x00800000, 0xBFFFFFFF)

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
            0x40086000, 0x02C,
            'CTRL5', 'MODEM.CTRL5', 'read-write',
            u"",
            0x00000000, 0x000007FE)

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
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TXBR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TXBR, self).__init__(rmio, label,
            0x40086000, 0x030,
            'TXBR', 'MODEM.TXBR', 'read-write',
            u"",
            0x00000000, 0x00FFFFFF)

        self.TXBRNUM = RM_Field_MODEM_TXBR_TXBRNUM(self)
        self.zz_fdict['TXBRNUM'] = self.TXBRNUM
        self.TXBRDEN = RM_Field_MODEM_TXBR_TXBRDEN(self)
        self.zz_fdict['TXBRDEN'] = self.TXBRDEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RXBR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RXBR, self).__init__(rmio, label,
            0x40086000, 0x034,
            'RXBR', 'MODEM.RXBR', 'read-write',
            u"",
            0x00000000, 0x00001FFF)

        self.RXBRNUM = RM_Field_MODEM_RXBR_RXBRNUM(self)
        self.zz_fdict['RXBRNUM'] = self.RXBRNUM
        self.RXBRDEN = RM_Field_MODEM_RXBR_RXBRDEN(self)
        self.zz_fdict['RXBRDEN'] = self.RXBRDEN
        self.RXBRINT = RM_Field_MODEM_RXBR_RXBRINT(self)
        self.zz_fdict['RXBRINT'] = self.RXBRINT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CF, self).__init__(rmio, label,
            0x40086000, 0x038,
            'CF', 'MODEM.CF', 'read-write',
            u"",
            0x00000000, 0x3FFFFFFF)

        self.DEC0 = RM_Field_MODEM_CF_DEC0(self)
        self.zz_fdict['DEC0'] = self.DEC0
        self.DEC1 = RM_Field_MODEM_CF_DEC1(self)
        self.zz_fdict['DEC1'] = self.DEC1
        self.DEC2 = RM_Field_MODEM_CF_DEC2(self)
        self.zz_fdict['DEC2'] = self.DEC2
        self.CFOSR = RM_Field_MODEM_CF_CFOSR(self)
        self.zz_fdict['CFOSR'] = self.CFOSR
        self.DEC1GAIN = RM_Field_MODEM_CF_DEC1GAIN(self)
        self.zz_fdict['DEC1GAIN'] = self.DEC1GAIN
        self.RESYNCRESETTIMING = RM_Field_MODEM_CF_RESYNCRESETTIMING(self)
        self.zz_fdict['RESYNCRESETTIMING'] = self.RESYNCRESETTIMING
        self.RESYNCBYPASS = RM_Field_MODEM_CF_RESYNCBYPASS(self)
        self.zz_fdict['RESYNCBYPASS'] = self.RESYNCBYPASS
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_PRE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_PRE, self).__init__(rmio, label,
            0x40086000, 0x03C,
            'PRE', 'MODEM.PRE', 'read-write',
            u"",
            0x00000000, 0xFFFF1FFF)

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
        self.TXBASES = RM_Field_MODEM_PRE_TXBASES(self)
        self.zz_fdict['TXBASES'] = self.TXBASES
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SYNC0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SYNC0, self).__init__(rmio, label,
            0x40086000, 0x040,
            'SYNC0', 'MODEM.SYNC0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.SYNC0 = RM_Field_MODEM_SYNC0_SYNC0(self)
        self.zz_fdict['SYNC0'] = self.SYNC0
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SYNC1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SYNC1, self).__init__(rmio, label,
            0x40086000, 0x044,
            'SYNC1', 'MODEM.SYNC1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.SYNC1 = RM_Field_MODEM_SYNC1_SYNC1(self)
        self.zz_fdict['SYNC1'] = self.SYNC1
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_TIMING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_TIMING, self).__init__(rmio, label,
            0x40086000, 0x048,
            'TIMING', 'MODEM.TIMING', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

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
            0x40086000, 0x04C,
            'DSSS0', 'MODEM.DSSS0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.DSSS0 = RM_Field_MODEM_DSSS0_DSSS0(self)
        self.zz_fdict['DSSS0'] = self.DSSS0
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_MODINDEX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_MODINDEX, self).__init__(rmio, label,
            0x40086000, 0x050,
            'MODINDEX', 'MODEM.MODINDEX', 'read-write',
            u"",
            0x00000000, 0x003F03FF)

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
            0x40086000, 0x054,
            'AFC', 'MODEM.AFC', 'read-write',
            u"",
            0x00000000, 0x00FFFCFF)

        self.AFCSCALEM = RM_Field_MODEM_AFC_AFCSCALEM(self)
        self.zz_fdict['AFCSCALEM'] = self.AFCSCALEM
        self.AFCSCALEE = RM_Field_MODEM_AFC_AFCSCALEE(self)
        self.zz_fdict['AFCSCALEE'] = self.AFCSCALEE
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
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_AFCADJLIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_AFCADJLIM, self).__init__(rmio, label,
            0x40086000, 0x058,
            'AFCADJLIM', 'MODEM.AFCADJLIM', 'read-write',
            u"",
            0x00000000, 0x0003FFFF)

        self.AFCADJLIM = RM_Field_MODEM_AFCADJLIM_AFCADJLIM(self)
        self.zz_fdict['AFCADJLIM'] = self.AFCADJLIM
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_SHAPING0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_SHAPING0, self).__init__(rmio, label,
            0x40086000, 0x05C,
            'SHAPING0', 'MODEM.SHAPING0', 'read-write',
            u"",
            0x22130A04, 0xFFFFFFFF)

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
            0x40086000, 0x060,
            'SHAPING1', 'MODEM.SHAPING1', 'read-write',
            u"",
            0x4F4A4132, 0xFFFFFFFF)

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
            0x40086000, 0x064,
            'SHAPING2', 'MODEM.SHAPING2', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.COEFF8 = RM_Field_MODEM_SHAPING2_COEFF8(self)
        self.zz_fdict['COEFF8'] = self.COEFF8
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAMPCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAMPCTRL, self).__init__(rmio, label,
            0x40086000, 0x068,
            'RAMPCTRL', 'MODEM.RAMPCTRL', 'read-write',
            u"",
            0x00000555, 0xFF800FFF)

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
            0x40086000, 0x06C,
            'RAMPLEV', 'MODEM.RAMPLEV', 'read-write',
            u"",
            0x00FFFFFF, 0x00FFFFFF)

        self.RAMPLEV0 = RM_Field_MODEM_RAMPLEV_RAMPLEV0(self)
        self.zz_fdict['RAMPLEV0'] = self.RAMPLEV0
        self.RAMPLEV1 = RM_Field_MODEM_RAMPLEV_RAMPLEV1(self)
        self.zz_fdict['RAMPLEV1'] = self.RAMPLEV1
        self.RAMPLEV2 = RM_Field_MODEM_RAMPLEV_RAMPLEV2(self)
        self.zz_fdict['RAMPLEV2'] = self.RAMPLEV2
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ROUTEPEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ROUTEPEN, self).__init__(rmio, label,
            0x40086000, 0x070,
            'ROUTEPEN', 'MODEM.ROUTEPEN', 'read-write',
            u"",
            0x00000000, 0x0000001F)

        self.DINPEN = RM_Field_MODEM_ROUTEPEN_DINPEN(self)
        self.zz_fdict['DINPEN'] = self.DINPEN
        self.DOUTPEN = RM_Field_MODEM_ROUTEPEN_DOUTPEN(self)
        self.zz_fdict['DOUTPEN'] = self.DOUTPEN
        self.DCLKPEN = RM_Field_MODEM_ROUTEPEN_DCLKPEN(self)
        self.zz_fdict['DCLKPEN'] = self.DCLKPEN
        self.ANT0PEN = RM_Field_MODEM_ROUTEPEN_ANT0PEN(self)
        self.zz_fdict['ANT0PEN'] = self.ANT0PEN
        self.ANT1PEN = RM_Field_MODEM_ROUTEPEN_ANT1PEN(self)
        self.zz_fdict['ANT1PEN'] = self.ANT1PEN
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ROUTELOC0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ROUTELOC0, self).__init__(rmio, label,
            0x40086000, 0x074,
            'ROUTELOC0', 'MODEM.ROUTELOC0', 'read-write',
            u"",
            0x00000000, 0x003F3F3F)

        self.DINLOC = RM_Field_MODEM_ROUTELOC0_DINLOC(self)
        self.zz_fdict['DINLOC'] = self.DINLOC
        self.DOUTLOC = RM_Field_MODEM_ROUTELOC0_DOUTLOC(self)
        self.zz_fdict['DOUTLOC'] = self.DOUTLOC
        self.DCLKLOC = RM_Field_MODEM_ROUTELOC0_DCLKLOC(self)
        self.zz_fdict['DCLKLOC'] = self.DCLKLOC
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_ROUTELOC1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_ROUTELOC1, self).__init__(rmio, label,
            0x40086000, 0x078,
            'ROUTELOC1', 'MODEM.ROUTELOC1', 'read-write',
            u"",
            0x00000000, 0x00003F3F)

        self.ANT0LOC = RM_Field_MODEM_ROUTELOC1_ANT0LOC(self)
        self.zz_fdict['ANT0LOC'] = self.ANT0LOC
        self.ANT1LOC = RM_Field_MODEM_ROUTELOC1_ANT1LOC(self)
        self.zz_fdict['ANT1LOC'] = self.ANT1LOC
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IF, self).__init__(rmio, label,
            0x40086000, 0x080,
            'IF', 'MODEM.IF', 'read-only',
            u"",
            0x00000000, 0x0000FF07)

        self.TXFRAMESENT = RM_Field_MODEM_IF_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_IF_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_IF_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
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
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_IFS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IFS, self).__init__(rmio, label,
            0x40086000, 0x084,
            'IFS', 'MODEM.IFS', 'write-only',
            u"",
            0x00000000, 0x0000FF07)

        self.TXFRAMESENT = RM_Field_MODEM_IFS_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_IFS_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_IFS_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
        self.RXTIMDET = RM_Field_MODEM_IFS_RXTIMDET(self)
        self.zz_fdict['RXTIMDET'] = self.RXTIMDET
        self.RXPREDET = RM_Field_MODEM_IFS_RXPREDET(self)
        self.zz_fdict['RXPREDET'] = self.RXPREDET
        self.RXFRAMEDET0 = RM_Field_MODEM_IFS_RXFRAMEDET0(self)
        self.zz_fdict['RXFRAMEDET0'] = self.RXFRAMEDET0
        self.RXFRAMEDET1 = RM_Field_MODEM_IFS_RXFRAMEDET1(self)
        self.zz_fdict['RXFRAMEDET1'] = self.RXFRAMEDET1
        self.RXTIMLOST = RM_Field_MODEM_IFS_RXTIMLOST(self)
        self.zz_fdict['RXTIMLOST'] = self.RXTIMLOST
        self.RXPRELOST = RM_Field_MODEM_IFS_RXPRELOST(self)
        self.zz_fdict['RXPRELOST'] = self.RXPRELOST
        self.RXFRAMEDETOF = RM_Field_MODEM_IFS_RXFRAMEDETOF(self)
        self.zz_fdict['RXFRAMEDETOF'] = self.RXFRAMEDETOF
        self.RXTIMNF = RM_Field_MODEM_IFS_RXTIMNF(self)
        self.zz_fdict['RXTIMNF'] = self.RXTIMNF
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_IFC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IFC, self).__init__(rmio, label,
            0x40086000, 0x088,
            'IFC', 'MODEM.IFC', 'write-only',
            u"",
            0x00000000, 0x0000FF07)

        self.TXFRAMESENT = RM_Field_MODEM_IFC_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_IFC_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_IFC_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
        self.RXTIMDET = RM_Field_MODEM_IFC_RXTIMDET(self)
        self.zz_fdict['RXTIMDET'] = self.RXTIMDET
        self.RXPREDET = RM_Field_MODEM_IFC_RXPREDET(self)
        self.zz_fdict['RXPREDET'] = self.RXPREDET
        self.RXFRAMEDET0 = RM_Field_MODEM_IFC_RXFRAMEDET0(self)
        self.zz_fdict['RXFRAMEDET0'] = self.RXFRAMEDET0
        self.RXFRAMEDET1 = RM_Field_MODEM_IFC_RXFRAMEDET1(self)
        self.zz_fdict['RXFRAMEDET1'] = self.RXFRAMEDET1
        self.RXTIMLOST = RM_Field_MODEM_IFC_RXTIMLOST(self)
        self.zz_fdict['RXTIMLOST'] = self.RXTIMLOST
        self.RXPRELOST = RM_Field_MODEM_IFC_RXPRELOST(self)
        self.zz_fdict['RXPRELOST'] = self.RXPRELOST
        self.RXFRAMEDETOF = RM_Field_MODEM_IFC_RXFRAMEDETOF(self)
        self.zz_fdict['RXFRAMEDETOF'] = self.RXFRAMEDETOF
        self.RXTIMNF = RM_Field_MODEM_IFC_RXTIMNF(self)
        self.zz_fdict['RXTIMNF'] = self.RXTIMNF
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_IEN, self).__init__(rmio, label,
            0x40086000, 0x08C,
            'IEN', 'MODEM.IEN', 'read-write',
            u"",
            0x00000000, 0x0000FF07)

        self.TXFRAMESENT = RM_Field_MODEM_IEN_TXFRAMESENT(self)
        self.zz_fdict['TXFRAMESENT'] = self.TXFRAMESENT
        self.TXSYNCSENT = RM_Field_MODEM_IEN_TXSYNCSENT(self)
        self.zz_fdict['TXSYNCSENT'] = self.TXSYNCSENT
        self.TXPRESENT = RM_Field_MODEM_IEN_TXPRESENT(self)
        self.zz_fdict['TXPRESENT'] = self.TXPRESENT
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
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_CMD, self).__init__(rmio, label,
            0x40086000, 0x090,
            'CMD', 'MODEM.CMD', 'write-only',
            u"",
            0x00000000, 0x00000039)

        self.PRESTOP = RM_Field_MODEM_CMD_PRESTOP(self)
        self.zz_fdict['PRESTOP'] = self.PRESTOP
        self.AFCTXLOCK = RM_Field_MODEM_CMD_AFCTXLOCK(self)
        self.zz_fdict['AFCTXLOCK'] = self.AFCTXLOCK
        self.AFCTXCLEAR = RM_Field_MODEM_CMD_AFCTXCLEAR(self)
        self.zz_fdict['AFCTXCLEAR'] = self.AFCTXCLEAR
        self.AFCRXCLEAR = RM_Field_MODEM_CMD_AFCRXCLEAR(self)
        self.zz_fdict['AFCRXCLEAR'] = self.AFCRXCLEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DCCOMP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DCCOMP, self).__init__(rmio, label,
            0x40086000, 0x098,
            'DCCOMP', 'MODEM.DCCOMP', 'read-write',
            u"",
            0x00000030, 0x000001FF)

        self.DCESTIEN = RM_Field_MODEM_DCCOMP_DCESTIEN(self)
        self.zz_fdict['DCESTIEN'] = self.DCESTIEN
        self.DCCOMPEN = RM_Field_MODEM_DCCOMP_DCCOMPEN(self)
        self.zz_fdict['DCCOMPEN'] = self.DCCOMPEN
        self.DCRSTEN = RM_Field_MODEM_DCCOMP_DCRSTEN(self)
        self.zz_fdict['DCRSTEN'] = self.DCRSTEN
        self.DCCOMPFREEZE = RM_Field_MODEM_DCCOMP_DCCOMPFREEZE(self)
        self.zz_fdict['DCCOMPFREEZE'] = self.DCCOMPFREEZE
        self.DCCOMPGEAR = RM_Field_MODEM_DCCOMP_DCCOMPGEAR(self)
        self.zz_fdict['DCCOMPGEAR'] = self.DCCOMPGEAR
        self.DCLIMIT = RM_Field_MODEM_DCCOMP_DCLIMIT(self)
        self.zz_fdict['DCLIMIT'] = self.DCLIMIT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DCCOMPFILTINIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DCCOMPFILTINIT, self).__init__(rmio, label,
            0x40086000, 0x09C,
            'DCCOMPFILTINIT', 'MODEM.DCCOMPFILTINIT', 'read-write',
            u"",
            0x00000000, 0x7FFFFFFF)

        self.DCCOMPINITVALI = RM_Field_MODEM_DCCOMPFILTINIT_DCCOMPINITVALI(self)
        self.zz_fdict['DCCOMPINITVALI'] = self.DCCOMPINITVALI
        self.DCCOMPINITVALQ = RM_Field_MODEM_DCCOMPFILTINIT_DCCOMPINITVALQ(self)
        self.zz_fdict['DCCOMPINITVALQ'] = self.DCCOMPINITVALQ
        self.DCCOMPINIT = RM_Field_MODEM_DCCOMPFILTINIT_DCCOMPINIT(self)
        self.zz_fdict['DCCOMPINIT'] = self.DCCOMPINIT
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_DCESTI(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_DCESTI, self).__init__(rmio, label,
            0x40086000, 0x100,
            'DCESTI', 'MODEM.DCESTI', 'read-only',
            u"",
            0x00000000, 0x3FFFFFFF)

        self.DCCOMPESTIVALI = RM_Field_MODEM_DCESTI_DCCOMPESTIVALI(self)
        self.zz_fdict['DCCOMPESTIVALI'] = self.DCCOMPESTIVALI
        self.DCCOMPESTIVALQ = RM_Field_MODEM_DCESTI_DCCOMPESTIVALQ(self)
        self.zz_fdict['DCCOMPESTIVALQ'] = self.DCCOMPESTIVALQ
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM0_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM0_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x400,
            'RAM0_RAMDATA', 'MODEM.RAM0_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM0_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM1_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM1_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x404,
            'RAM1_RAMDATA', 'MODEM.RAM1_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM1_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM2_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM2_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x408,
            'RAM2_RAMDATA', 'MODEM.RAM2_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM2_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM3_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM3_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x40C,
            'RAM3_RAMDATA', 'MODEM.RAM3_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM3_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM4_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM4_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x410,
            'RAM4_RAMDATA', 'MODEM.RAM4_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM4_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM5_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM5_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x414,
            'RAM5_RAMDATA', 'MODEM.RAM5_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM5_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM6_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM6_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x418,
            'RAM6_RAMDATA', 'MODEM.RAM6_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM6_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM7_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM7_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x41C,
            'RAM7_RAMDATA', 'MODEM.RAM7_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM7_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM8_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM8_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x420,
            'RAM8_RAMDATA', 'MODEM.RAM8_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM8_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM9_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM9_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x424,
            'RAM9_RAMDATA', 'MODEM.RAM9_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM9_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM10_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM10_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x428,
            'RAM10_RAMDATA', 'MODEM.RAM10_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM10_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM11_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM11_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x42C,
            'RAM11_RAMDATA', 'MODEM.RAM11_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM11_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM12_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM12_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x430,
            'RAM12_RAMDATA', 'MODEM.RAM12_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM12_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM13_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM13_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x434,
            'RAM13_RAMDATA', 'MODEM.RAM13_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM13_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM14_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM14_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x438,
            'RAM14_RAMDATA', 'MODEM.RAM14_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM14_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM15_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM15_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x43C,
            'RAM15_RAMDATA', 'MODEM.RAM15_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM15_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM16_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM16_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x440,
            'RAM16_RAMDATA', 'MODEM.RAM16_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM16_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM17_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM17_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x444,
            'RAM17_RAMDATA', 'MODEM.RAM17_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM17_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM18_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM18_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x448,
            'RAM18_RAMDATA', 'MODEM.RAM18_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM18_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM19_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM19_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x44C,
            'RAM19_RAMDATA', 'MODEM.RAM19_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM19_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM20_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM20_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x450,
            'RAM20_RAMDATA', 'MODEM.RAM20_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM20_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM21_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM21_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x454,
            'RAM21_RAMDATA', 'MODEM.RAM21_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM21_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM22_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM22_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x458,
            'RAM22_RAMDATA', 'MODEM.RAM22_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM22_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM23_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM23_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x45C,
            'RAM23_RAMDATA', 'MODEM.RAM23_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM23_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM24_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM24_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x460,
            'RAM24_RAMDATA', 'MODEM.RAM24_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM24_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM25_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM25_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x464,
            'RAM25_RAMDATA', 'MODEM.RAM25_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM25_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM26_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM26_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x468,
            'RAM26_RAMDATA', 'MODEM.RAM26_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM26_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM27_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM27_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x46C,
            'RAM27_RAMDATA', 'MODEM.RAM27_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM27_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM28_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM28_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x470,
            'RAM28_RAMDATA', 'MODEM.RAM28_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM28_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM29_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM29_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x474,
            'RAM29_RAMDATA', 'MODEM.RAM29_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM29_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM30_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM30_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x478,
            'RAM30_RAMDATA', 'MODEM.RAM30_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM30_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM31_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM31_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x47C,
            'RAM31_RAMDATA', 'MODEM.RAM31_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM31_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM32_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM32_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x480,
            'RAM32_RAMDATA', 'MODEM.RAM32_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM32_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM33_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM33_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x484,
            'RAM33_RAMDATA', 'MODEM.RAM33_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM33_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM34_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM34_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x488,
            'RAM34_RAMDATA', 'MODEM.RAM34_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM34_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM35_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM35_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x48C,
            'RAM35_RAMDATA', 'MODEM.RAM35_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM35_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM36_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM36_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x490,
            'RAM36_RAMDATA', 'MODEM.RAM36_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM36_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM37_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM37_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x494,
            'RAM37_RAMDATA', 'MODEM.RAM37_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM37_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM38_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM38_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x498,
            'RAM38_RAMDATA', 'MODEM.RAM38_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM38_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM39_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM39_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x49C,
            'RAM39_RAMDATA', 'MODEM.RAM39_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM39_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM40_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM40_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4A0,
            'RAM40_RAMDATA', 'MODEM.RAM40_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM40_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM41_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM41_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4A4,
            'RAM41_RAMDATA', 'MODEM.RAM41_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM41_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM42_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM42_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4A8,
            'RAM42_RAMDATA', 'MODEM.RAM42_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM42_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM43_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM43_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4AC,
            'RAM43_RAMDATA', 'MODEM.RAM43_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM43_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM44_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM44_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4B0,
            'RAM44_RAMDATA', 'MODEM.RAM44_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM44_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM45_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM45_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4B4,
            'RAM45_RAMDATA', 'MODEM.RAM45_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM45_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM46_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM46_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4B8,
            'RAM46_RAMDATA', 'MODEM.RAM46_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM46_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM47_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM47_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4BC,
            'RAM47_RAMDATA', 'MODEM.RAM47_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM47_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM48_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM48_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4C0,
            'RAM48_RAMDATA', 'MODEM.RAM48_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM48_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM49_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM49_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4C4,
            'RAM49_RAMDATA', 'MODEM.RAM49_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM49_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM50_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM50_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4C8,
            'RAM50_RAMDATA', 'MODEM.RAM50_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM50_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM51_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM51_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4CC,
            'RAM51_RAMDATA', 'MODEM.RAM51_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM51_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM52_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM52_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4D0,
            'RAM52_RAMDATA', 'MODEM.RAM52_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM52_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM53_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM53_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4D4,
            'RAM53_RAMDATA', 'MODEM.RAM53_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM53_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM54_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM54_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4D8,
            'RAM54_RAMDATA', 'MODEM.RAM54_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM54_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM55_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM55_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4DC,
            'RAM55_RAMDATA', 'MODEM.RAM55_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM55_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM56_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM56_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4E0,
            'RAM56_RAMDATA', 'MODEM.RAM56_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM56_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM57_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM57_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4E4,
            'RAM57_RAMDATA', 'MODEM.RAM57_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM57_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM58_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM58_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4E8,
            'RAM58_RAMDATA', 'MODEM.RAM58_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM58_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM59_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM59_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4EC,
            'RAM59_RAMDATA', 'MODEM.RAM59_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM59_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM60_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM60_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4F0,
            'RAM60_RAMDATA', 'MODEM.RAM60_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM60_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM61_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM61_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4F4,
            'RAM61_RAMDATA', 'MODEM.RAM61_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM61_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM62_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM62_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4F8,
            'RAM62_RAMDATA', 'MODEM.RAM62_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM62_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM63_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM63_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x4FC,
            'RAM63_RAMDATA', 'MODEM.RAM63_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM63_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM64_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM64_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x500,
            'RAM64_RAMDATA', 'MODEM.RAM64_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM64_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM65_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM65_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x504,
            'RAM65_RAMDATA', 'MODEM.RAM65_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM65_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM66_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM66_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x508,
            'RAM66_RAMDATA', 'MODEM.RAM66_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM66_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM67_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM67_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x50C,
            'RAM67_RAMDATA', 'MODEM.RAM67_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM67_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM68_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM68_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x510,
            'RAM68_RAMDATA', 'MODEM.RAM68_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM68_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM69_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM69_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x514,
            'RAM69_RAMDATA', 'MODEM.RAM69_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM69_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM70_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM70_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x518,
            'RAM70_RAMDATA', 'MODEM.RAM70_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM70_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM71_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM71_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x51C,
            'RAM71_RAMDATA', 'MODEM.RAM71_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM71_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM72_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM72_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x520,
            'RAM72_RAMDATA', 'MODEM.RAM72_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM72_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM73_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM73_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x524,
            'RAM73_RAMDATA', 'MODEM.RAM73_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM73_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM74_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM74_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x528,
            'RAM74_RAMDATA', 'MODEM.RAM74_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM74_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM75_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM75_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x52C,
            'RAM75_RAMDATA', 'MODEM.RAM75_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM75_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM76_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM76_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x530,
            'RAM76_RAMDATA', 'MODEM.RAM76_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM76_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM77_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM77_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x534,
            'RAM77_RAMDATA', 'MODEM.RAM77_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM77_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM78_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM78_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x538,
            'RAM78_RAMDATA', 'MODEM.RAM78_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM78_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM79_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM79_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x53C,
            'RAM79_RAMDATA', 'MODEM.RAM79_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM79_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM80_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM80_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x540,
            'RAM80_RAMDATA', 'MODEM.RAM80_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM80_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM81_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM81_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x544,
            'RAM81_RAMDATA', 'MODEM.RAM81_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM81_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM82_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM82_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x548,
            'RAM82_RAMDATA', 'MODEM.RAM82_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM82_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM83_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM83_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x54C,
            'RAM83_RAMDATA', 'MODEM.RAM83_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM83_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM84_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM84_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x550,
            'RAM84_RAMDATA', 'MODEM.RAM84_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM84_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM85_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM85_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x554,
            'RAM85_RAMDATA', 'MODEM.RAM85_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM85_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM86_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM86_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x558,
            'RAM86_RAMDATA', 'MODEM.RAM86_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM86_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM87_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM87_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x55C,
            'RAM87_RAMDATA', 'MODEM.RAM87_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM87_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM88_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM88_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x560,
            'RAM88_RAMDATA', 'MODEM.RAM88_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM88_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM89_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM89_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x564,
            'RAM89_RAMDATA', 'MODEM.RAM89_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM89_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM90_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM90_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x568,
            'RAM90_RAMDATA', 'MODEM.RAM90_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM90_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM91_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM91_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x56C,
            'RAM91_RAMDATA', 'MODEM.RAM91_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM91_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM92_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM92_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x570,
            'RAM92_RAMDATA', 'MODEM.RAM92_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM92_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM93_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM93_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x574,
            'RAM93_RAMDATA', 'MODEM.RAM93_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM93_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM94_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM94_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x578,
            'RAM94_RAMDATA', 'MODEM.RAM94_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM94_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM95_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM95_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x57C,
            'RAM95_RAMDATA', 'MODEM.RAM95_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM95_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM96_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM96_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x580,
            'RAM96_RAMDATA', 'MODEM.RAM96_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM96_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM97_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM97_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x584,
            'RAM97_RAMDATA', 'MODEM.RAM97_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM97_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM98_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM98_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x588,
            'RAM98_RAMDATA', 'MODEM.RAM98_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM98_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM99_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM99_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x58C,
            'RAM99_RAMDATA', 'MODEM.RAM99_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM99_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM100_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM100_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x590,
            'RAM100_RAMDATA', 'MODEM.RAM100_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM100_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM101_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM101_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x594,
            'RAM101_RAMDATA', 'MODEM.RAM101_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM101_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM102_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM102_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x598,
            'RAM102_RAMDATA', 'MODEM.RAM102_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM102_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM103_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM103_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x59C,
            'RAM103_RAMDATA', 'MODEM.RAM103_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM103_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM104_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM104_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5A0,
            'RAM104_RAMDATA', 'MODEM.RAM104_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM104_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM105_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM105_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5A4,
            'RAM105_RAMDATA', 'MODEM.RAM105_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM105_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM106_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM106_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5A8,
            'RAM106_RAMDATA', 'MODEM.RAM106_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM106_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM107_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM107_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5AC,
            'RAM107_RAMDATA', 'MODEM.RAM107_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM107_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM108_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM108_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5B0,
            'RAM108_RAMDATA', 'MODEM.RAM108_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM108_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM109_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM109_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5B4,
            'RAM109_RAMDATA', 'MODEM.RAM109_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM109_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM110_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM110_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5B8,
            'RAM110_RAMDATA', 'MODEM.RAM110_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM110_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM111_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM111_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5BC,
            'RAM111_RAMDATA', 'MODEM.RAM111_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM111_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM112_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM112_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5C0,
            'RAM112_RAMDATA', 'MODEM.RAM112_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM112_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM113_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM113_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5C4,
            'RAM113_RAMDATA', 'MODEM.RAM113_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM113_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM114_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM114_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5C8,
            'RAM114_RAMDATA', 'MODEM.RAM114_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM114_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM115_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM115_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5CC,
            'RAM115_RAMDATA', 'MODEM.RAM115_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM115_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM116_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM116_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5D0,
            'RAM116_RAMDATA', 'MODEM.RAM116_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM116_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM117_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM117_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5D4,
            'RAM117_RAMDATA', 'MODEM.RAM117_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM117_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM118_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM118_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5D8,
            'RAM118_RAMDATA', 'MODEM.RAM118_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM118_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM119_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM119_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5DC,
            'RAM119_RAMDATA', 'MODEM.RAM119_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM119_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM120_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM120_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5E0,
            'RAM120_RAMDATA', 'MODEM.RAM120_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM120_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM121_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM121_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5E4,
            'RAM121_RAMDATA', 'MODEM.RAM121_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM121_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM122_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM122_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5E8,
            'RAM122_RAMDATA', 'MODEM.RAM122_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM122_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM123_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM123_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5EC,
            'RAM123_RAMDATA', 'MODEM.RAM123_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM123_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM124_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM124_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5F0,
            'RAM124_RAMDATA', 'MODEM.RAM124_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM124_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM125_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM125_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5F4,
            'RAM125_RAMDATA', 'MODEM.RAM125_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM125_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM126_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM126_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5F8,
            'RAM126_RAMDATA', 'MODEM.RAM126_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM126_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM127_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM127_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x5FC,
            'RAM127_RAMDATA', 'MODEM.RAM127_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM127_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM128_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM128_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x600,
            'RAM128_RAMDATA', 'MODEM.RAM128_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM128_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM129_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM129_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x604,
            'RAM129_RAMDATA', 'MODEM.RAM129_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM129_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM130_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM130_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x608,
            'RAM130_RAMDATA', 'MODEM.RAM130_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM130_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM131_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM131_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x60C,
            'RAM131_RAMDATA', 'MODEM.RAM131_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM131_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM132_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM132_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x610,
            'RAM132_RAMDATA', 'MODEM.RAM132_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM132_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM133_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM133_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x614,
            'RAM133_RAMDATA', 'MODEM.RAM133_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM133_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM134_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM134_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x618,
            'RAM134_RAMDATA', 'MODEM.RAM134_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM134_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM135_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM135_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x61C,
            'RAM135_RAMDATA', 'MODEM.RAM135_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM135_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM136_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM136_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x620,
            'RAM136_RAMDATA', 'MODEM.RAM136_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM136_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM137_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM137_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x624,
            'RAM137_RAMDATA', 'MODEM.RAM137_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM137_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM138_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM138_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x628,
            'RAM138_RAMDATA', 'MODEM.RAM138_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM138_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM139_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM139_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x62C,
            'RAM139_RAMDATA', 'MODEM.RAM139_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM139_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM140_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM140_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x630,
            'RAM140_RAMDATA', 'MODEM.RAM140_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM140_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM141_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM141_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x634,
            'RAM141_RAMDATA', 'MODEM.RAM141_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM141_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM142_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM142_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x638,
            'RAM142_RAMDATA', 'MODEM.RAM142_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM142_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM143_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM143_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x63C,
            'RAM143_RAMDATA', 'MODEM.RAM143_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM143_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM144_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM144_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x640,
            'RAM144_RAMDATA', 'MODEM.RAM144_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM144_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM145_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM145_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x644,
            'RAM145_RAMDATA', 'MODEM.RAM145_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM145_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM146_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM146_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x648,
            'RAM146_RAMDATA', 'MODEM.RAM146_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM146_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM147_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM147_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x64C,
            'RAM147_RAMDATA', 'MODEM.RAM147_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM147_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM148_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM148_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x650,
            'RAM148_RAMDATA', 'MODEM.RAM148_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM148_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM149_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM149_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x654,
            'RAM149_RAMDATA', 'MODEM.RAM149_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM149_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM150_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM150_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x658,
            'RAM150_RAMDATA', 'MODEM.RAM150_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM150_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM151_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM151_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x65C,
            'RAM151_RAMDATA', 'MODEM.RAM151_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM151_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM152_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM152_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x660,
            'RAM152_RAMDATA', 'MODEM.RAM152_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM152_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM153_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM153_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x664,
            'RAM153_RAMDATA', 'MODEM.RAM153_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM153_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM154_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM154_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x668,
            'RAM154_RAMDATA', 'MODEM.RAM154_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM154_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM155_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM155_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x66C,
            'RAM155_RAMDATA', 'MODEM.RAM155_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM155_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM156_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM156_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x670,
            'RAM156_RAMDATA', 'MODEM.RAM156_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM156_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM157_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM157_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x674,
            'RAM157_RAMDATA', 'MODEM.RAM157_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM157_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM158_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM158_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x678,
            'RAM158_RAMDATA', 'MODEM.RAM158_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM158_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM159_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM159_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x67C,
            'RAM159_RAMDATA', 'MODEM.RAM159_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM159_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM160_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM160_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x680,
            'RAM160_RAMDATA', 'MODEM.RAM160_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM160_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM161_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM161_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x684,
            'RAM161_RAMDATA', 'MODEM.RAM161_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM161_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM162_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM162_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x688,
            'RAM162_RAMDATA', 'MODEM.RAM162_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM162_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM163_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM163_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x68C,
            'RAM163_RAMDATA', 'MODEM.RAM163_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM163_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM164_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM164_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x690,
            'RAM164_RAMDATA', 'MODEM.RAM164_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM164_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM165_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM165_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x694,
            'RAM165_RAMDATA', 'MODEM.RAM165_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM165_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM166_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM166_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x698,
            'RAM166_RAMDATA', 'MODEM.RAM166_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM166_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM167_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM167_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x69C,
            'RAM167_RAMDATA', 'MODEM.RAM167_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM167_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM168_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM168_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6A0,
            'RAM168_RAMDATA', 'MODEM.RAM168_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM168_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM169_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM169_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6A4,
            'RAM169_RAMDATA', 'MODEM.RAM169_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM169_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM170_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM170_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6A8,
            'RAM170_RAMDATA', 'MODEM.RAM170_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM170_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM171_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM171_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6AC,
            'RAM171_RAMDATA', 'MODEM.RAM171_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM171_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM172_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM172_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6B0,
            'RAM172_RAMDATA', 'MODEM.RAM172_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM172_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM173_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM173_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6B4,
            'RAM173_RAMDATA', 'MODEM.RAM173_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM173_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM174_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM174_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6B8,
            'RAM174_RAMDATA', 'MODEM.RAM174_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM174_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM175_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM175_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6BC,
            'RAM175_RAMDATA', 'MODEM.RAM175_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM175_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM176_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM176_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6C0,
            'RAM176_RAMDATA', 'MODEM.RAM176_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM176_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM177_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM177_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6C4,
            'RAM177_RAMDATA', 'MODEM.RAM177_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM177_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM178_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM178_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6C8,
            'RAM178_RAMDATA', 'MODEM.RAM178_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM178_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM179_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM179_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6CC,
            'RAM179_RAMDATA', 'MODEM.RAM179_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM179_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM180_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM180_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6D0,
            'RAM180_RAMDATA', 'MODEM.RAM180_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM180_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM181_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM181_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6D4,
            'RAM181_RAMDATA', 'MODEM.RAM181_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM181_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM182_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM182_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6D8,
            'RAM182_RAMDATA', 'MODEM.RAM182_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM182_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM183_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM183_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6DC,
            'RAM183_RAMDATA', 'MODEM.RAM183_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM183_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM184_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM184_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6E0,
            'RAM184_RAMDATA', 'MODEM.RAM184_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM184_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM185_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM185_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6E4,
            'RAM185_RAMDATA', 'MODEM.RAM185_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM185_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM186_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM186_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6E8,
            'RAM186_RAMDATA', 'MODEM.RAM186_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM186_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM187_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM187_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6EC,
            'RAM187_RAMDATA', 'MODEM.RAM187_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM187_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM188_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM188_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6F0,
            'RAM188_RAMDATA', 'MODEM.RAM188_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM188_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM189_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM189_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6F4,
            'RAM189_RAMDATA', 'MODEM.RAM189_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM189_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM190_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM190_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6F8,
            'RAM190_RAMDATA', 'MODEM.RAM190_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM190_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM191_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM191_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x6FC,
            'RAM191_RAMDATA', 'MODEM.RAM191_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM191_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM192_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM192_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x700,
            'RAM192_RAMDATA', 'MODEM.RAM192_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM192_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM193_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM193_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x704,
            'RAM193_RAMDATA', 'MODEM.RAM193_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM193_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM194_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM194_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x708,
            'RAM194_RAMDATA', 'MODEM.RAM194_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM194_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM195_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM195_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x70C,
            'RAM195_RAMDATA', 'MODEM.RAM195_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM195_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM196_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM196_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x710,
            'RAM196_RAMDATA', 'MODEM.RAM196_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM196_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM197_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM197_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x714,
            'RAM197_RAMDATA', 'MODEM.RAM197_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM197_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM198_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM198_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x718,
            'RAM198_RAMDATA', 'MODEM.RAM198_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM198_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM199_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM199_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x71C,
            'RAM199_RAMDATA', 'MODEM.RAM199_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM199_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM200_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM200_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x720,
            'RAM200_RAMDATA', 'MODEM.RAM200_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM200_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM201_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM201_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x724,
            'RAM201_RAMDATA', 'MODEM.RAM201_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM201_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM202_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM202_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x728,
            'RAM202_RAMDATA', 'MODEM.RAM202_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM202_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM203_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM203_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x72C,
            'RAM203_RAMDATA', 'MODEM.RAM203_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM203_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM204_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM204_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x730,
            'RAM204_RAMDATA', 'MODEM.RAM204_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM204_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM205_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM205_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x734,
            'RAM205_RAMDATA', 'MODEM.RAM205_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM205_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM206_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM206_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x738,
            'RAM206_RAMDATA', 'MODEM.RAM206_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM206_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM207_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM207_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x73C,
            'RAM207_RAMDATA', 'MODEM.RAM207_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM207_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM208_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM208_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x740,
            'RAM208_RAMDATA', 'MODEM.RAM208_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM208_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM209_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM209_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x744,
            'RAM209_RAMDATA', 'MODEM.RAM209_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM209_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM210_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM210_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x748,
            'RAM210_RAMDATA', 'MODEM.RAM210_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM210_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM211_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM211_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x74C,
            'RAM211_RAMDATA', 'MODEM.RAM211_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM211_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM212_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM212_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x750,
            'RAM212_RAMDATA', 'MODEM.RAM212_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM212_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM213_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM213_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x754,
            'RAM213_RAMDATA', 'MODEM.RAM213_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM213_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM214_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM214_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x758,
            'RAM214_RAMDATA', 'MODEM.RAM214_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM214_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM215_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM215_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x75C,
            'RAM215_RAMDATA', 'MODEM.RAM215_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM215_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM216_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM216_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x760,
            'RAM216_RAMDATA', 'MODEM.RAM216_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM216_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM217_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM217_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x764,
            'RAM217_RAMDATA', 'MODEM.RAM217_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM217_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM218_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM218_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x768,
            'RAM218_RAMDATA', 'MODEM.RAM218_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM218_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM219_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM219_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x76C,
            'RAM219_RAMDATA', 'MODEM.RAM219_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM219_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM220_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM220_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x770,
            'RAM220_RAMDATA', 'MODEM.RAM220_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM220_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM221_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM221_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x774,
            'RAM221_RAMDATA', 'MODEM.RAM221_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM221_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM222_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM222_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x778,
            'RAM222_RAMDATA', 'MODEM.RAM222_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM222_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM223_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM223_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x77C,
            'RAM223_RAMDATA', 'MODEM.RAM223_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM223_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM224_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM224_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x780,
            'RAM224_RAMDATA', 'MODEM.RAM224_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM224_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM225_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM225_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x784,
            'RAM225_RAMDATA', 'MODEM.RAM225_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM225_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM226_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM226_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x788,
            'RAM226_RAMDATA', 'MODEM.RAM226_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM226_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM227_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM227_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x78C,
            'RAM227_RAMDATA', 'MODEM.RAM227_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM227_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM228_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM228_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x790,
            'RAM228_RAMDATA', 'MODEM.RAM228_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM228_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM229_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM229_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x794,
            'RAM229_RAMDATA', 'MODEM.RAM229_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM229_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM230_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM230_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x798,
            'RAM230_RAMDATA', 'MODEM.RAM230_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM230_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM231_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM231_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x79C,
            'RAM231_RAMDATA', 'MODEM.RAM231_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM231_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM232_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM232_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7A0,
            'RAM232_RAMDATA', 'MODEM.RAM232_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM232_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM233_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM233_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7A4,
            'RAM233_RAMDATA', 'MODEM.RAM233_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM233_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM234_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM234_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7A8,
            'RAM234_RAMDATA', 'MODEM.RAM234_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM234_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM235_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM235_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7AC,
            'RAM235_RAMDATA', 'MODEM.RAM235_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM235_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM236_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM236_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7B0,
            'RAM236_RAMDATA', 'MODEM.RAM236_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM236_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM237_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM237_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7B4,
            'RAM237_RAMDATA', 'MODEM.RAM237_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM237_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM238_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM238_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7B8,
            'RAM238_RAMDATA', 'MODEM.RAM238_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM238_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM239_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM239_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7BC,
            'RAM239_RAMDATA', 'MODEM.RAM239_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM239_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM240_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM240_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7C0,
            'RAM240_RAMDATA', 'MODEM.RAM240_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM240_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM241_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM241_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7C4,
            'RAM241_RAMDATA', 'MODEM.RAM241_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM241_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM242_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM242_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7C8,
            'RAM242_RAMDATA', 'MODEM.RAM242_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM242_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM243_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM243_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7CC,
            'RAM243_RAMDATA', 'MODEM.RAM243_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM243_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM244_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM244_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7D0,
            'RAM244_RAMDATA', 'MODEM.RAM244_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM244_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM245_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM245_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7D4,
            'RAM245_RAMDATA', 'MODEM.RAM245_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM245_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM246_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM246_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7D8,
            'RAM246_RAMDATA', 'MODEM.RAM246_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM246_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM247_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM247_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7DC,
            'RAM247_RAMDATA', 'MODEM.RAM247_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM247_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM248_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM248_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7E0,
            'RAM248_RAMDATA', 'MODEM.RAM248_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM248_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM249_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM249_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7E4,
            'RAM249_RAMDATA', 'MODEM.RAM249_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM249_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM250_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM250_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7E8,
            'RAM250_RAMDATA', 'MODEM.RAM250_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM250_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM251_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM251_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7EC,
            'RAM251_RAMDATA', 'MODEM.RAM251_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM251_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM252_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM252_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7F0,
            'RAM252_RAMDATA', 'MODEM.RAM252_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM252_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM253_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM253_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7F4,
            'RAM253_RAMDATA', 'MODEM.RAM253_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM253_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM254_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM254_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7F8,
            'RAM254_RAMDATA', 'MODEM.RAM254_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM254_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


class RM_Register_MODEM_RAM255_RAMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_MODEM_RAM255_RAMDATA, self).__init__(rmio, label,
            0x40086000, 0x7FC,
            'RAM255_RAMDATA', 'MODEM.RAM255_RAMDATA', 'read-write',
            u"",
            0x00000000, 0x000000FF)

        self.DATA = RM_Field_MODEM_RAM255_RAMDATA_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.__dict__['zz_frozen'] = True


