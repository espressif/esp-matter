
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . RAC_field import *


class RM_Register_RAC_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IPVERSION, self).__init__(rmio, label,
            0xa8020000, 0x000,
            'IPVERSION', 'RAC.IPVERSION', 'read-only',
            u"",
            0x00000004, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_RAC_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_EN, self).__init__(rmio, label,
            0xa8020000, 0x004,
            'EN', 'RAC.EN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_RAC_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RXENSRCEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RXENSRCEN, self).__init__(rmio, label,
            0xa8020000, 0x008,
            'RXENSRCEN', 'RAC.RXENSRCEN', 'read-write',
            u"",
            0x00000000, 0x00003FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SWRXEN = RM_Field_RAC_RXENSRCEN_SWRXEN(self)
        self.zz_fdict['SWRXEN'] = self.SWRXEN
        self.CHANNELBUSYEN = RM_Field_RAC_RXENSRCEN_CHANNELBUSYEN(self)
        self.zz_fdict['CHANNELBUSYEN'] = self.CHANNELBUSYEN
        self.TIMDETEN = RM_Field_RAC_RXENSRCEN_TIMDETEN(self)
        self.zz_fdict['TIMDETEN'] = self.TIMDETEN
        self.PREDETEN = RM_Field_RAC_RXENSRCEN_PREDETEN(self)
        self.zz_fdict['PREDETEN'] = self.PREDETEN
        self.FRAMEDETEN = RM_Field_RAC_RXENSRCEN_FRAMEDETEN(self)
        self.zz_fdict['FRAMEDETEN'] = self.FRAMEDETEN
        self.DEMODRXREQEN = RM_Field_RAC_RXENSRCEN_DEMODRXREQEN(self)
        self.zz_fdict['DEMODRXREQEN'] = self.DEMODRXREQEN
        self.PRSRXEN = RM_Field_RAC_RXENSRCEN_PRSRXEN(self)
        self.zz_fdict['PRSRXEN'] = self.PRSRXEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STATUS, self).__init__(rmio, label,
            0xa8020000, 0x00C,
            'STATUS', 'RAC.STATUS', 'read-only',
            u"",
            0x00000000, 0xFFF8FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RXMASK = RM_Field_RAC_STATUS_RXMASK(self)
        self.zz_fdict['RXMASK'] = self.RXMASK
        self.FORCESTATEACTIVE = RM_Field_RAC_STATUS_FORCESTATEACTIVE(self)
        self.zz_fdict['FORCESTATEACTIVE'] = self.FORCESTATEACTIVE
        self.TXAFTERFRAMEPEND = RM_Field_RAC_STATUS_TXAFTERFRAMEPEND(self)
        self.zz_fdict['TXAFTERFRAMEPEND'] = self.TXAFTERFRAMEPEND
        self.TXAFTERFRAMEACTIVE = RM_Field_RAC_STATUS_TXAFTERFRAMEACTIVE(self)
        self.zz_fdict['TXAFTERFRAMEACTIVE'] = self.TXAFTERFRAMEACTIVE
        self.SEQSLEEPING = RM_Field_RAC_STATUS_SEQSLEEPING(self)
        self.zz_fdict['SEQSLEEPING'] = self.SEQSLEEPING
        self.SEQSLEEPDEEP = RM_Field_RAC_STATUS_SEQSLEEPDEEP(self)
        self.zz_fdict['SEQSLEEPDEEP'] = self.SEQSLEEPDEEP
        self.STATE = RM_Field_RAC_STATUS_STATE(self)
        self.zz_fdict['STATE'] = self.STATE
        self.SEQACTIVE = RM_Field_RAC_STATUS_SEQACTIVE(self)
        self.zz_fdict['SEQACTIVE'] = self.SEQACTIVE
        self.DEMODENS = RM_Field_RAC_STATUS_DEMODENS(self)
        self.zz_fdict['DEMODENS'] = self.DEMODENS
        self.TXENS = RM_Field_RAC_STATUS_TXENS(self)
        self.zz_fdict['TXENS'] = self.TXENS
        self.RXENS = RM_Field_RAC_STATUS_RXENS(self)
        self.zz_fdict['RXENS'] = self.RXENS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CMD, self).__init__(rmio, label,
            0xa8020000, 0x010,
            'CMD', 'RAC.CMD', 'write-only',
            u"",
            0x00000000, 0xC000FDFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXEN = RM_Field_RAC_CMD_TXEN(self)
        self.zz_fdict['TXEN'] = self.TXEN
        self.FORCETX = RM_Field_RAC_CMD_FORCETX(self)
        self.zz_fdict['FORCETX'] = self.FORCETX
        self.TXONCCA = RM_Field_RAC_CMD_TXONCCA(self)
        self.zz_fdict['TXONCCA'] = self.TXONCCA
        self.CLEARTXEN = RM_Field_RAC_CMD_CLEARTXEN(self)
        self.zz_fdict['CLEARTXEN'] = self.CLEARTXEN
        self.TXAFTERFRAME = RM_Field_RAC_CMD_TXAFTERFRAME(self)
        self.zz_fdict['TXAFTERFRAME'] = self.TXAFTERFRAME
        self.TXDIS = RM_Field_RAC_CMD_TXDIS(self)
        self.zz_fdict['TXDIS'] = self.TXDIS
        self.CLEARRXOVERFLOW = RM_Field_RAC_CMD_CLEARRXOVERFLOW(self)
        self.zz_fdict['CLEARRXOVERFLOW'] = self.CLEARRXOVERFLOW
        self.RXCAL = RM_Field_RAC_CMD_RXCAL(self)
        self.zz_fdict['RXCAL'] = self.RXCAL
        self.RXDIS = RM_Field_RAC_CMD_RXDIS(self)
        self.zz_fdict['RXDIS'] = self.RXDIS
        self.FRCWR = RM_Field_RAC_CMD_FRCWR(self)
        self.zz_fdict['FRCWR'] = self.FRCWR
        self.FRCRD = RM_Field_RAC_CMD_FRCRD(self)
        self.zz_fdict['FRCRD'] = self.FRCRD
        self.PAENSET = RM_Field_RAC_CMD_PAENSET(self)
        self.zz_fdict['PAENSET'] = self.PAENSET
        self.PAENCLEAR = RM_Field_RAC_CMD_PAENCLEAR(self)
        self.zz_fdict['PAENCLEAR'] = self.PAENCLEAR
        self.LNAENSET = RM_Field_RAC_CMD_LNAENSET(self)
        self.zz_fdict['LNAENSET'] = self.LNAENSET
        self.LNAENCLEAR = RM_Field_RAC_CMD_LNAENCLEAR(self)
        self.zz_fdict['LNAENCLEAR'] = self.LNAENCLEAR
        self.DEMODENSET = RM_Field_RAC_CMD_DEMODENSET(self)
        self.zz_fdict['DEMODENSET'] = self.DEMODENSET
        self.DEMODENCLEAR = RM_Field_RAC_CMD_DEMODENCLEAR(self)
        self.zz_fdict['DEMODENCLEAR'] = self.DEMODENCLEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CTRL, self).__init__(rmio, label,
            0xa8020000, 0x014,
            'CTRL', 'RAC.CTRL', 'read-write',
            u"",
            0x00000000, 0x1F0107EF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FORCEDISABLE = RM_Field_RAC_CTRL_FORCEDISABLE(self)
        self.zz_fdict['FORCEDISABLE'] = self.FORCEDISABLE
        self.PRSTXEN = RM_Field_RAC_CTRL_PRSTXEN(self)
        self.zz_fdict['PRSTXEN'] = self.PRSTXEN
        self.TXAFTERRX = RM_Field_RAC_CTRL_TXAFTERRX(self)
        self.zz_fdict['TXAFTERRX'] = self.TXAFTERRX
        self.PRSMODE = RM_Field_RAC_CTRL_PRSMODE(self)
        self.zz_fdict['PRSMODE'] = self.PRSMODE
        self.PRSCLR = RM_Field_RAC_CTRL_PRSCLR(self)
        self.zz_fdict['PRSCLR'] = self.PRSCLR
        self.TXPOSTPONE = RM_Field_RAC_CTRL_TXPOSTPONE(self)
        self.zz_fdict['TXPOSTPONE'] = self.TXPOSTPONE
        self.ACTIVEPOL = RM_Field_RAC_CTRL_ACTIVEPOL(self)
        self.zz_fdict['ACTIVEPOL'] = self.ACTIVEPOL
        self.PAENPOL = RM_Field_RAC_CTRL_PAENPOL(self)
        self.zz_fdict['PAENPOL'] = self.PAENPOL
        self.LNAENPOL = RM_Field_RAC_CTRL_LNAENPOL(self)
        self.zz_fdict['LNAENPOL'] = self.LNAENPOL
        self.PRSRXDIS = RM_Field_RAC_CTRL_PRSRXDIS(self)
        self.zz_fdict['PRSRXDIS'] = self.PRSRXDIS
        self.PRSFORCETX = RM_Field_RAC_CTRL_PRSFORCETX(self)
        self.zz_fdict['PRSFORCETX'] = self.PRSFORCETX
        self.SEQRESET = RM_Field_RAC_CTRL_SEQRESET(self)
        self.zz_fdict['SEQRESET'] = self.SEQRESET
        self.EXITSHUTDOWNDIS = RM_Field_RAC_CTRL_EXITSHUTDOWNDIS(self)
        self.zz_fdict['EXITSHUTDOWNDIS'] = self.EXITSHUTDOWNDIS
        self.CPUWAITDIS = RM_Field_RAC_CTRL_CPUWAITDIS(self)
        self.zz_fdict['CPUWAITDIS'] = self.CPUWAITDIS
        self.SEQCLKDIS = RM_Field_RAC_CTRL_SEQCLKDIS(self)
        self.zz_fdict['SEQCLKDIS'] = self.SEQCLKDIS
        self.RXOFDIS = RM_Field_RAC_CTRL_RXOFDIS(self)
        self.zz_fdict['RXOFDIS'] = self.RXOFDIS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_FORCESTATE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_FORCESTATE, self).__init__(rmio, label,
            0xa8020000, 0x018,
            'FORCESTATE', 'RAC.FORCESTATE', 'read-write',
            u"",
            0x00000000, 0x0000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.FORCESTATE = RM_Field_RAC_FORCESTATE_FORCESTATE(self)
        self.zz_fdict['FORCESTATE'] = self.FORCESTATE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IF, self).__init__(rmio, label,
            0xa8020000, 0x01C,
            'IF', 'RAC.IF', 'read-write',
            u"",
            0x00000000, 0x00FF000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGE = RM_Field_RAC_IF_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IF_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.SEQLOCKUP = RM_Field_RAC_IF_SEQLOCKUP(self)
        self.zz_fdict['SEQLOCKUP'] = self.SEQLOCKUP
        self.SEQRESETREQ = RM_Field_RAC_IF_SEQRESETREQ(self)
        self.zz_fdict['SEQRESETREQ'] = self.SEQRESETREQ
        self.SEQ = RM_Field_RAC_IF_SEQ(self)
        self.zz_fdict['SEQ'] = self.SEQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IEN, self).__init__(rmio, label,
            0xa8020000, 0x020,
            'IEN', 'RAC.IEN', 'read-write',
            u"",
            0x00000000, 0x00FF000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGE = RM_Field_RAC_IEN_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IEN_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.SEQLOCKUP = RM_Field_RAC_IEN_SEQLOCKUP(self)
        self.zz_fdict['SEQLOCKUP'] = self.SEQLOCKUP
        self.SEQRESETREQ = RM_Field_RAC_IEN_SEQRESETREQ(self)
        self.zz_fdict['SEQRESETREQ'] = self.SEQRESETREQ
        self.SEQ = RM_Field_RAC_IEN_SEQ(self)
        self.zz_fdict['SEQ'] = self.SEQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TESTCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TESTCTRL, self).__init__(rmio, label,
            0xa8020000, 0x024,
            'TESTCTRL', 'RAC.TESTCTRL', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODEN = RM_Field_RAC_TESTCTRL_MODEN(self)
        self.zz_fdict['MODEN'] = self.MODEN
        self.DEMODEN = RM_Field_RAC_TESTCTRL_DEMODEN(self)
        self.zz_fdict['DEMODEN'] = self.DEMODEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQIF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQIF, self).__init__(rmio, label,
            0xa8020000, 0x028,
            'SEQIF', 'RAC.SEQIF', 'read-write',
            u"",
            0x00000000, 0xBFFF000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGESEQ = RM_Field_RAC_SEQIF_STATECHANGESEQ(self)
        self.zz_fdict['STATECHANGESEQ'] = self.STATECHANGESEQ
        self.STIMCMPEVSEQ = RM_Field_RAC_SEQIF_STIMCMPEVSEQ(self)
        self.zz_fdict['STIMCMPEVSEQ'] = self.STIMCMPEVSEQ
        self.DEMODRXREQCLRSEQ = RM_Field_RAC_SEQIF_DEMODRXREQCLRSEQ(self)
        self.zz_fdict['DEMODRXREQCLRSEQ'] = self.DEMODRXREQCLRSEQ
        self.PRSEVENTSEQ = RM_Field_RAC_SEQIF_PRSEVENTSEQ(self)
        self.zz_fdict['PRSEVENTSEQ'] = self.PRSEVENTSEQ
        self.STATEOFF = RM_Field_RAC_SEQIF_STATEOFF(self)
        self.zz_fdict['STATEOFF'] = self.STATEOFF
        self.STATERXWARM = RM_Field_RAC_SEQIF_STATERXWARM(self)
        self.zz_fdict['STATERXWARM'] = self.STATERXWARM
        self.STATERXSEARCH = RM_Field_RAC_SEQIF_STATERXSEARCH(self)
        self.zz_fdict['STATERXSEARCH'] = self.STATERXSEARCH
        self.STATERXFRAME = RM_Field_RAC_SEQIF_STATERXFRAME(self)
        self.zz_fdict['STATERXFRAME'] = self.STATERXFRAME
        self.STATERXPD = RM_Field_RAC_SEQIF_STATERXPD(self)
        self.zz_fdict['STATERXPD'] = self.STATERXPD
        self.STATERX2RX = RM_Field_RAC_SEQIF_STATERX2RX(self)
        self.zz_fdict['STATERX2RX'] = self.STATERX2RX
        self.STATERXOVERFLOW = RM_Field_RAC_SEQIF_STATERXOVERFLOW(self)
        self.zz_fdict['STATERXOVERFLOW'] = self.STATERXOVERFLOW
        self.STATERX2TX = RM_Field_RAC_SEQIF_STATERX2TX(self)
        self.zz_fdict['STATERX2TX'] = self.STATERX2TX
        self.STATETXWARM = RM_Field_RAC_SEQIF_STATETXWARM(self)
        self.zz_fdict['STATETXWARM'] = self.STATETXWARM
        self.STATETX = RM_Field_RAC_SEQIF_STATETX(self)
        self.zz_fdict['STATETX'] = self.STATETX
        self.STATETXPD = RM_Field_RAC_SEQIF_STATETXPD(self)
        self.zz_fdict['STATETXPD'] = self.STATETXPD
        self.STATETX2RX = RM_Field_RAC_SEQIF_STATETX2RX(self)
        self.zz_fdict['STATETX2RX'] = self.STATETX2RX
        self.STATETX2TX = RM_Field_RAC_SEQIF_STATETX2TX(self)
        self.zz_fdict['STATETX2TX'] = self.STATETX2TX
        self.STATESHUTDOWN = RM_Field_RAC_SEQIF_STATESHUTDOWN(self)
        self.zz_fdict['STATESHUTDOWN'] = self.STATESHUTDOWN
        self.RFTXPKD = RM_Field_RAC_SEQIF_RFTXPKD(self)
        self.zz_fdict['RFTXPKD'] = self.RFTXPKD
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQIEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQIEN, self).__init__(rmio, label,
            0xa8020000, 0x02C,
            'SEQIEN', 'RAC.SEQIEN', 'read-write',
            u"",
            0x00000000, 0xBFFF000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGESEQ = RM_Field_RAC_SEQIEN_STATECHANGESEQ(self)
        self.zz_fdict['STATECHANGESEQ'] = self.STATECHANGESEQ
        self.STIMCMPEVSEQ = RM_Field_RAC_SEQIEN_STIMCMPEVSEQ(self)
        self.zz_fdict['STIMCMPEVSEQ'] = self.STIMCMPEVSEQ
        self.DEMODRXREQCLRSEQ = RM_Field_RAC_SEQIEN_DEMODRXREQCLRSEQ(self)
        self.zz_fdict['DEMODRXREQCLRSEQ'] = self.DEMODRXREQCLRSEQ
        self.PRSEVENTSEQ = RM_Field_RAC_SEQIEN_PRSEVENTSEQ(self)
        self.zz_fdict['PRSEVENTSEQ'] = self.PRSEVENTSEQ
        self.STATEOFF = RM_Field_RAC_SEQIEN_STATEOFF(self)
        self.zz_fdict['STATEOFF'] = self.STATEOFF
        self.STATERXWARM = RM_Field_RAC_SEQIEN_STATERXWARM(self)
        self.zz_fdict['STATERXWARM'] = self.STATERXWARM
        self.STATERXSEARCH = RM_Field_RAC_SEQIEN_STATERXSEARCH(self)
        self.zz_fdict['STATERXSEARCH'] = self.STATERXSEARCH
        self.STATERXFRAME = RM_Field_RAC_SEQIEN_STATERXFRAME(self)
        self.zz_fdict['STATERXFRAME'] = self.STATERXFRAME
        self.STATERXPD = RM_Field_RAC_SEQIEN_STATERXPD(self)
        self.zz_fdict['STATERXPD'] = self.STATERXPD
        self.STATERX2RX = RM_Field_RAC_SEQIEN_STATERX2RX(self)
        self.zz_fdict['STATERX2RX'] = self.STATERX2RX
        self.STATERXOVERFLOW = RM_Field_RAC_SEQIEN_STATERXOVERFLOW(self)
        self.zz_fdict['STATERXOVERFLOW'] = self.STATERXOVERFLOW
        self.STATERX2TX = RM_Field_RAC_SEQIEN_STATERX2TX(self)
        self.zz_fdict['STATERX2TX'] = self.STATERX2TX
        self.STATETXWARM = RM_Field_RAC_SEQIEN_STATETXWARM(self)
        self.zz_fdict['STATETXWARM'] = self.STATETXWARM
        self.STATETX = RM_Field_RAC_SEQIEN_STATETX(self)
        self.zz_fdict['STATETX'] = self.STATETX
        self.STATETXPD = RM_Field_RAC_SEQIEN_STATETXPD(self)
        self.zz_fdict['STATETXPD'] = self.STATETXPD
        self.STATETX2RX = RM_Field_RAC_SEQIEN_STATETX2RX(self)
        self.zz_fdict['STATETX2RX'] = self.STATETX2RX
        self.STATETX2TX = RM_Field_RAC_SEQIEN_STATETX2TX(self)
        self.zz_fdict['STATETX2TX'] = self.STATETX2TX
        self.STATESHUTDOWN = RM_Field_RAC_SEQIEN_STATESHUTDOWN(self)
        self.zz_fdict['STATESHUTDOWN'] = self.STATESHUTDOWN
        self.RFTXPKD = RM_Field_RAC_SEQIEN_RFTXPKD(self)
        self.zz_fdict['RFTXPKD'] = self.RFTXPKD
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STATUS1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STATUS1, self).__init__(rmio, label,
            0xa8020000, 0x030,
            'STATUS1', 'RAC.STATUS1', 'read-only',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXMASK = RM_Field_RAC_STATUS1_TXMASK(self)
        self.zz_fdict['TXMASK'] = self.TXMASK
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_FEMCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_FEMCTRL, self).__init__(rmio, label,
            0xa8020000, 0x034,
            'FEMCTRL', 'RAC.FEMCTRL', 'read-write',
            u"",
            0x00000000, 0x0000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.FEMEN = RM_Field_RAC_FEMCTRL_FEMEN(self)
        self.zz_fdict['FEMEN'] = self.FEMEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_FEMDATA(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_FEMDATA, self).__init__(rmio, label,
            0xa8020000, 0x038,
            'FEMDATA', 'RAC.FEMDATA', 'read-write',
            u"",
            0x00000000, 0x0000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.FEMDATA = RM_Field_RAC_FEMDATA_FEMDATA(self)
        self.zz_fdict['FEMDATA'] = self.FEMDATA
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STIMER(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STIMER, self).__init__(rmio, label,
            0xa8020000, 0x050,
            'STIMER', 'RAC.STIMER', 'read-only',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STIMER = RM_Field_RAC_STIMER_STIMER(self)
        self.zz_fdict['STIMER'] = self.STIMER
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STIMERCOMP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STIMERCOMP, self).__init__(rmio, label,
            0xa8020000, 0x054,
            'STIMERCOMP', 'RAC.STIMERCOMP', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STIMERCOMP = RM_Field_RAC_STIMERCOMP_STIMERCOMP(self)
        self.zz_fdict['STIMERCOMP'] = self.STIMERCOMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQCTRL, self).__init__(rmio, label,
            0xa8020000, 0x058,
            'SEQCTRL', 'RAC.SEQCTRL', 'read-write',
            u"",
            0x00000000, 0x0300007F,
            0x00001000, 0x00002000,
            0x00003000)

        self.COMPACT = RM_Field_RAC_SEQCTRL_COMPACT(self)
        self.zz_fdict['COMPACT'] = self.COMPACT
        self.COMPINVALMODE = RM_Field_RAC_SEQCTRL_COMPINVALMODE(self)
        self.zz_fdict['COMPINVALMODE'] = self.COMPINVALMODE
        self.RELATIVE = RM_Field_RAC_SEQCTRL_RELATIVE(self)
        self.zz_fdict['RELATIVE'] = self.RELATIVE
        self.STIMERALWAYSRUN = RM_Field_RAC_SEQCTRL_STIMERALWAYSRUN(self)
        self.zz_fdict['STIMERALWAYSRUN'] = self.STIMERALWAYSRUN
        self.STIMERDEBUGRUN = RM_Field_RAC_SEQCTRL_STIMERDEBUGRUN(self)
        self.zz_fdict['STIMERDEBUGRUN'] = self.STIMERDEBUGRUN
        self.STATEDEBUGRUN = RM_Field_RAC_SEQCTRL_STATEDEBUGRUN(self)
        self.zz_fdict['STATEDEBUGRUN'] = self.STATEDEBUGRUN
        self.SWIRQ = RM_Field_RAC_SEQCTRL_SWIRQ(self)
        self.zz_fdict['SWIRQ'] = self.SWIRQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PRESC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PRESC, self).__init__(rmio, label,
            0xa8020000, 0x05C,
            'PRESC', 'RAC.PRESC', 'read-write',
            u"",
            0x00000007, 0x0000007F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STIMER = RM_Field_RAC_PRESC_STIMER(self)
        self.zz_fdict['STIMER'] = self.STIMER
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR0, self).__init__(rmio, label,
            0xa8020000, 0x060,
            'SR0', 'RAC.SR0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SR0 = RM_Field_RAC_SR0_SR0(self)
        self.zz_fdict['SR0'] = self.SR0
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR1, self).__init__(rmio, label,
            0xa8020000, 0x064,
            'SR1', 'RAC.SR1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SR1 = RM_Field_RAC_SR1_SR1(self)
        self.zz_fdict['SR1'] = self.SR1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR2, self).__init__(rmio, label,
            0xa8020000, 0x068,
            'SR2', 'RAC.SR2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SR2 = RM_Field_RAC_SR2_SR2(self)
        self.zz_fdict['SR2'] = self.SR2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR3, self).__init__(rmio, label,
            0xa8020000, 0x06C,
            'SR3', 'RAC.SR3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SR3 = RM_Field_RAC_SR3_SR3(self)
        self.zz_fdict['SR3'] = self.SR3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STCTRL, self).__init__(rmio, label,
            0xa8020000, 0x070,
            'STCTRL', 'RAC.STCTRL', 'read-write',
            u"",
            0x00000000, 0x01FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STCAL = RM_Field_RAC_STCTRL_STCAL(self)
        self.zz_fdict['STCAL'] = self.STCAL
        self.STSKEW = RM_Field_RAC_STCTRL_STSKEW(self)
        self.zz_fdict['STSKEW'] = self.STSKEW
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_FRCTXWORD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_FRCTXWORD, self).__init__(rmio, label,
            0xa8020000, 0x074,
            'FRCTXWORD', 'RAC.FRCTXWORD', 'read-write',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.WDATA = RM_Field_RAC_FRCTXWORD_WDATA(self)
        self.zz_fdict['WDATA'] = self.WDATA
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_FRCRXWORD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_FRCRXWORD, self).__init__(rmio, label,
            0xa8020000, 0x078,
            'FRCRXWORD', 'RAC.FRCRXWORD', 'read-only',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RDATA = RM_Field_RAC_FRCRXWORD_RDATA(self)
        self.zz_fdict['RDATA'] = self.RDATA
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_EM1PCSR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_EM1PCSR, self).__init__(rmio, label,
            0xa8020000, 0x07C,
            'EM1PCSR', 'RAC.EM1PCSR', 'read-write',
            u"",
            0x00000000, 0x000F0033,
            0x00001000, 0x00002000,
            0x00003000)

        self.RADIOEM1PMODE = RM_Field_RAC_EM1PCSR_RADIOEM1PMODE(self)
        self.zz_fdict['RADIOEM1PMODE'] = self.RADIOEM1PMODE
        self.RADIOEM1PDISSWREQ = RM_Field_RAC_EM1PCSR_RADIOEM1PDISSWREQ(self)
        self.zz_fdict['RADIOEM1PDISSWREQ'] = self.RADIOEM1PDISSWREQ
        self.MCUEM1PMODE = RM_Field_RAC_EM1PCSR_MCUEM1PMODE(self)
        self.zz_fdict['MCUEM1PMODE'] = self.MCUEM1PMODE
        self.MCUEM1PDISSWREQ = RM_Field_RAC_EM1PCSR_MCUEM1PDISSWREQ(self)
        self.zz_fdict['MCUEM1PDISSWREQ'] = self.MCUEM1PDISSWREQ
        self.RADIOEM1PREQ = RM_Field_RAC_EM1PCSR_RADIOEM1PREQ(self)
        self.zz_fdict['RADIOEM1PREQ'] = self.RADIOEM1PREQ
        self.RADIOEM1PACK = RM_Field_RAC_EM1PCSR_RADIOEM1PACK(self)
        self.zz_fdict['RADIOEM1PACK'] = self.RADIOEM1PACK
        self.RADIOEM1PHWREQ = RM_Field_RAC_EM1PCSR_RADIOEM1PHWREQ(self)
        self.zz_fdict['RADIOEM1PHWREQ'] = self.RADIOEM1PHWREQ
        self.MCUEM1PSWRUNNING = RM_Field_RAC_EM1PCSR_MCUEM1PSWRUNNING(self)
        self.zz_fdict['MCUEM1PSWRUNNING'] = self.MCUEM1PSWRUNNING
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHENCTRL, self).__init__(rmio, label,
            0xa8020000, 0x098,
            'SYNTHENCTRL', 'RAC.SYNTHENCTRL', 'read-write',
            u"",
            0x00000000, 0x00100682,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCOSTARTUP = RM_Field_RAC_SYNTHENCTRL_VCOSTARTUP(self)
        self.zz_fdict['VCOSTARTUP'] = self.VCOSTARTUP
        self.VCBUFEN = RM_Field_RAC_SYNTHENCTRL_VCBUFEN(self)
        self.zz_fdict['VCBUFEN'] = self.VCBUFEN
        self.SYNTHSTARTREQ = RM_Field_RAC_SYNTHENCTRL_SYNTHSTARTREQ(self)
        self.zz_fdict['SYNTHSTARTREQ'] = self.SYNTHSTARTREQ
        self.MMDPOWERBALANCEDISABLE = RM_Field_RAC_SYNTHENCTRL_MMDPOWERBALANCEDISABLE(self)
        self.zz_fdict['MMDPOWERBALANCEDISABLE'] = self.MMDPOWERBALANCEDISABLE
        self.LPFBWSEL = RM_Field_RAC_SYNTHENCTRL_LPFBWSEL(self)
        self.zz_fdict['LPFBWSEL'] = self.LPFBWSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHREGCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHREGCTRL, self).__init__(rmio, label,
            0xa8020000, 0x09C,
            'SYNTHREGCTRL', 'RAC.SYNTHREGCTRL', 'read-write',
            u"",
            0x04000C00, 0x07001C00,
            0x00001000, 0x00002000,
            0x00003000)

        self.MMDLDOVREFTRIM = RM_Field_RAC_SYNTHREGCTRL_MMDLDOVREFTRIM(self)
        self.zz_fdict['MMDLDOVREFTRIM'] = self.MMDLDOVREFTRIM
        self.CHPLDOVREFTRIM = RM_Field_RAC_SYNTHREGCTRL_CHPLDOVREFTRIM(self)
        self.zz_fdict['CHPLDOVREFTRIM'] = self.CHPLDOVREFTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_VCOCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_VCOCTRL, self).__init__(rmio, label,
            0xa8020000, 0x0A0,
            'VCOCTRL', 'RAC.VCOCTRL', 'read-write',
            u"",
            0x0000044C, 0x00000FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCOAMPLITUDE = RM_Field_RAC_VCOCTRL_VCOAMPLITUDE(self)
        self.zz_fdict['VCOAMPLITUDE'] = self.VCOAMPLITUDE
        self.VCODETAMPLITUDERX = RM_Field_RAC_VCOCTRL_VCODETAMPLITUDERX(self)
        self.zz_fdict['VCODETAMPLITUDERX'] = self.VCODETAMPLITUDERX
        self.VCODETAMPLITUDETX = RM_Field_RAC_VCOCTRL_VCODETAMPLITUDETX(self)
        self.zz_fdict['VCODETAMPLITUDETX'] = self.VCODETAMPLITUDETX
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CHPCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CHPCTRL, self).__init__(rmio, label,
            0xa8020000, 0x0A4,
            'CHPCTRL', 'RAC.CHPCTRL', 'read-write',
            u"",
            0x00000000, 0x00004000,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHPOUTPUTTRISTATEMODE = RM_Field_RAC_CHPCTRL_CHPOUTPUTTRISTATEMODE(self)
        self.zz_fdict['CHPOUTPUTTRISTATEMODE'] = self.CHPOUTPUTTRISTATEMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x0AC,
            'RFSTATUS', 'RAC.RFSTATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODRAMPUPDONE = RM_Field_RAC_RFSTATUS_MODRAMPUPDONE(self)
        self.zz_fdict['MODRAMPUPDONE'] = self.MODRAMPUPDONE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STATUS2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STATUS2, self).__init__(rmio, label,
            0xa8020000, 0x0B0,
            'STATUS2', 'RAC.STATUS2', 'read-only',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREVSTATE1 = RM_Field_RAC_STATUS2_PREVSTATE1(self)
        self.zz_fdict['PREVSTATE1'] = self.PREVSTATE1
        self.PREVSTATE2 = RM_Field_RAC_STATUS2_PREVSTATE2(self)
        self.zz_fdict['PREVSTATE2'] = self.PREVSTATE2
        self.PREVSTATE3 = RM_Field_RAC_STATUS2_PREVSTATE3(self)
        self.zz_fdict['PREVSTATE3'] = self.PREVSTATE3
        self.CURRSTATE = RM_Field_RAC_STATUS2_CURRSTATE(self)
        self.zz_fdict['CURRSTATE'] = self.CURRSTATE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFPGACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFPGACTRL, self).__init__(rmio, label,
            0xa8020000, 0x0B4,
            'IFPGACTRL', 'RAC.IFPGACTRL', 'read-write',
            u"",
            0x00000000, 0x1FF80000,
            0x00001000, 0x00002000,
            0x00003000)

        self.DCCALON = RM_Field_RAC_IFPGACTRL_DCCALON(self)
        self.zz_fdict['DCCALON'] = self.DCCALON
        self.DCRSTEN = RM_Field_RAC_IFPGACTRL_DCRSTEN(self)
        self.zz_fdict['DCRSTEN'] = self.DCRSTEN
        self.DCESTIEN = RM_Field_RAC_IFPGACTRL_DCESTIEN(self)
        self.zz_fdict['DCESTIEN'] = self.DCESTIEN
        self.DCCALDEC0 = RM_Field_RAC_IFPGACTRL_DCCALDEC0(self)
        self.zz_fdict['DCCALDEC0'] = self.DCCALDEC0
        self.DCCALDCGEAR = RM_Field_RAC_IFPGACTRL_DCCALDCGEAR(self)
        self.zz_fdict['DCCALDCGEAR'] = self.DCCALDCGEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PAENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PAENCTRL, self).__init__(rmio, label,
            0xa8020000, 0x0B8,
            'PAENCTRL', 'RAC.PAENCTRL', 'read-write',
            u"",
            0x00000000, 0x00070100,
            0x00001000, 0x00002000,
            0x00003000)

        self.PARAMP = RM_Field_RAC_PAENCTRL_PARAMP(self)
        self.zz_fdict['PARAMP'] = self.PARAMP
        self.INVRAMPCLK = RM_Field_RAC_PAENCTRL_INVRAMPCLK(self)
        self.zz_fdict['INVRAMPCLK'] = self.INVRAMPCLK
        self.DIV2RAMPCLK = RM_Field_RAC_PAENCTRL_DIV2RAMPCLK(self)
        self.zz_fdict['DIV2RAMPCLK'] = self.DIV2RAMPCLK
        self.RSTDIV2RAMPCLK = RM_Field_RAC_PAENCTRL_RSTDIV2RAMPCLK(self)
        self.zz_fdict['RSTDIV2RAMPCLK'] = self.RSTDIV2RAMPCLK
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_APC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_APC, self).__init__(rmio, label,
            0xa8020000, 0x0BC,
            'APC', 'RAC.APC', 'read-write',
            u"",
            0xFF000000, 0xFF000004,
            0x00001000, 0x00002000,
            0x00003000)

        self.ENAPCSW = RM_Field_RAC_APC_ENAPCSW(self)
        self.zz_fdict['ENAPCSW'] = self.ENAPCSW
        self.AMPCONTROLLIMITSW = RM_Field_RAC_APC_AMPCONTROLLIMITSW(self)
        self.zz_fdict['AMPCONTROLLIMITSW'] = self.AMPCONTROLLIMITSW
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_ANTDIV(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_ANTDIV, self).__init__(rmio, label,
            0xa8020000, 0x0C0,
            'ANTDIV', 'RAC.ANTDIV', 'read-write',
            u"",
            0x00000000, 0x00000FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.INTDIVLNAMIXEN0 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXEN0(self)
        self.zz_fdict['INTDIVLNAMIXEN0'] = self.INTDIVLNAMIXEN0
        self.INTDIVLNAMIXENAMP0 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXENAMP0(self)
        self.zz_fdict['INTDIVLNAMIXENAMP0'] = self.INTDIVLNAMIXENAMP0
        self.INTDIVLNAMIXRFATTDCEN0 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXRFATTDCEN0(self)
        self.zz_fdict['INTDIVLNAMIXRFATTDCEN0'] = self.INTDIVLNAMIXRFATTDCEN0
        self.INTDIVLNAMIXRFPKDENRF0 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXRFPKDENRF0(self)
        self.zz_fdict['INTDIVLNAMIXRFPKDENRF0'] = self.INTDIVLNAMIXRFPKDENRF0
        self.INTDIVSYLODIVRLO0SUBGEN = RM_Field_RAC_ANTDIV_INTDIVSYLODIVRLO0SUBGEN(self)
        self.zz_fdict['INTDIVSYLODIVRLO0SUBGEN'] = self.INTDIVSYLODIVRLO0SUBGEN
        self.INTDIVLNAMIXEN1 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXEN1(self)
        self.zz_fdict['INTDIVLNAMIXEN1'] = self.INTDIVLNAMIXEN1
        self.INTDIVLNAMIXENAMP1 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXENAMP1(self)
        self.zz_fdict['INTDIVLNAMIXENAMP1'] = self.INTDIVLNAMIXENAMP1
        self.INTDIVLNAMIXRFATTDCEN1 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXRFATTDCEN1(self)
        self.zz_fdict['INTDIVLNAMIXRFATTDCEN1'] = self.INTDIVLNAMIXRFATTDCEN1
        self.INTDIVLNAMIXRFPKDENRF1 = RM_Field_RAC_ANTDIV_INTDIVLNAMIXRFPKDENRF1(self)
        self.zz_fdict['INTDIVLNAMIXRFPKDENRF1'] = self.INTDIVLNAMIXRFPKDENRF1
        self.INTDIVSYLODIVRLO1SUBGEN = RM_Field_RAC_ANTDIV_INTDIVSYLODIVRLO1SUBGEN(self)
        self.zz_fdict['INTDIVSYLODIVRLO1SUBGEN'] = self.INTDIVSYLODIVRLO1SUBGEN
        self.ANTDIVSTATUS = RM_Field_RAC_ANTDIV_ANTDIVSTATUS(self)
        self.zz_fdict['ANTDIVSTATUS'] = self.ANTDIVSTATUS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCTRIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCTRIM, self).__init__(rmio, label,
            0xa8020000, 0x0C4,
            'AUXADCTRIM', 'RAC.AUXADCTRIM', 'read-write',
            u"",
            0x06D55504, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCCLKINVERT = RM_Field_RAC_AUXADCTRIM_AUXADCCLKINVERT(self)
        self.zz_fdict['AUXADCCLKINVERT'] = self.AUXADCCLKINVERT
        self.AUXADCLDOVREFTRIM = RM_Field_RAC_AUXADCTRIM_AUXADCLDOVREFTRIM(self)
        self.zz_fdict['AUXADCLDOVREFTRIM'] = self.AUXADCLDOVREFTRIM
        self.AUXADCOUTPUTINVERT = RM_Field_RAC_AUXADCTRIM_AUXADCOUTPUTINVERT(self)
        self.zz_fdict['AUXADCOUTPUTINVERT'] = self.AUXADCOUTPUTINVERT
        self.AUXADCRCTUNE = RM_Field_RAC_AUXADCTRIM_AUXADCRCTUNE(self)
        self.zz_fdict['AUXADCRCTUNE'] = self.AUXADCRCTUNE
        self.AUXADCTRIMADCINPUTRES = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMADCINPUTRES(self)
        self.zz_fdict['AUXADCTRIMADCINPUTRES'] = self.AUXADCTRIMADCINPUTRES
        self.AUXADCTRIMCURRINPUTBUF = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMCURRINPUTBUF(self)
        self.zz_fdict['AUXADCTRIMCURRINPUTBUF'] = self.AUXADCTRIMCURRINPUTBUF
        self.AUXADCTRIMCURROPA1 = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMCURROPA1(self)
        self.zz_fdict['AUXADCTRIMCURROPA1'] = self.AUXADCTRIMCURROPA1
        self.AUXADCTRIMCURROPA2 = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMCURROPA2(self)
        self.zz_fdict['AUXADCTRIMCURROPA2'] = self.AUXADCTRIMCURROPA2
        self.AUXADCTRIMCURRREFBUF = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMCURRREFBUF(self)
        self.zz_fdict['AUXADCTRIMCURRREFBUF'] = self.AUXADCTRIMCURRREFBUF
        self.AUXADCTRIMCURRTSENSE = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMCURRTSENSE(self)
        self.zz_fdict['AUXADCTRIMCURRTSENSE'] = self.AUXADCTRIMCURRTSENSE
        self.AUXADCTRIMCURRVCMBUF = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMCURRVCMBUF(self)
        self.zz_fdict['AUXADCTRIMCURRVCMBUF'] = self.AUXADCTRIMCURRVCMBUF
        self.AUXADCTRIMLDOHIGHCURRENT = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMLDOHIGHCURRENT(self)
        self.zz_fdict['AUXADCTRIMLDOHIGHCURRENT'] = self.AUXADCTRIMLDOHIGHCURRENT
        self.AUXADCTRIMREFP = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMREFP(self)
        self.zz_fdict['AUXADCTRIMREFP'] = self.AUXADCTRIMREFP
        self.AUXADCTRIMVREFVCM = RM_Field_RAC_AUXADCTRIM_AUXADCTRIMVREFVCM(self)
        self.zz_fdict['AUXADCTRIMVREFVCM'] = self.AUXADCTRIMVREFVCM
        self.AUXADCTSENSETRIMVBE2 = RM_Field_RAC_AUXADCTRIM_AUXADCTSENSETRIMVBE2(self)
        self.zz_fdict['AUXADCTSENSETRIMVBE2'] = self.AUXADCTSENSETRIMVBE2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCEN, self).__init__(rmio, label,
            0xa8020000, 0x0C8,
            'AUXADCEN', 'RAC.AUXADCEN', 'read-write',
            u"",
            0x00000000, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCENAUXADC = RM_Field_RAC_AUXADCEN_AUXADCENAUXADC(self)
        self.zz_fdict['AUXADCENAUXADC'] = self.AUXADCENAUXADC
        self.AUXADCENINPUTBUFFER = RM_Field_RAC_AUXADCEN_AUXADCENINPUTBUFFER(self)
        self.zz_fdict['AUXADCENINPUTBUFFER'] = self.AUXADCENINPUTBUFFER
        self.AUXADCENLDO = RM_Field_RAC_AUXADCEN_AUXADCENLDO(self)
        self.zz_fdict['AUXADCENLDO'] = self.AUXADCENLDO
        self.AUXADCENOUTPUTDRV = RM_Field_RAC_AUXADCEN_AUXADCENOUTPUTDRV(self)
        self.zz_fdict['AUXADCENOUTPUTDRV'] = self.AUXADCENOUTPUTDRV
        self.AUXADCENPMON = RM_Field_RAC_AUXADCEN_AUXADCENPMON(self)
        self.zz_fdict['AUXADCENPMON'] = self.AUXADCENPMON
        self.AUXADCENRESONDIAGA = RM_Field_RAC_AUXADCEN_AUXADCENRESONDIAGA(self)
        self.zz_fdict['AUXADCENRESONDIAGA'] = self.AUXADCENRESONDIAGA
        self.AUXADCENTSENSE = RM_Field_RAC_AUXADCEN_AUXADCENTSENSE(self)
        self.zz_fdict['AUXADCENTSENSE'] = self.AUXADCENTSENSE
        self.AUXADCENTSENSECAL = RM_Field_RAC_AUXADCEN_AUXADCENTSENSECAL(self)
        self.zz_fdict['AUXADCENTSENSECAL'] = self.AUXADCENTSENSECAL
        self.AUXADCINPUTBUFFERBYPASS = RM_Field_RAC_AUXADCEN_AUXADCINPUTBUFFERBYPASS(self)
        self.zz_fdict['AUXADCINPUTBUFFERBYPASS'] = self.AUXADCINPUTBUFFERBYPASS
        self.AUXADCENMEASTHERMISTOR = RM_Field_RAC_AUXADCEN_AUXADCENMEASTHERMISTOR(self)
        self.zz_fdict['AUXADCENMEASTHERMISTOR'] = self.AUXADCENMEASTHERMISTOR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCCTRL0, self).__init__(rmio, label,
            0xa8020000, 0x0CC,
            'AUXADCCTRL0', 'RAC.AUXADCCTRL0', 'read-write',
            u"",
            0x00000100, 0x00003FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CYCLES = RM_Field_RAC_AUXADCCTRL0_CYCLES(self)
        self.zz_fdict['CYCLES'] = self.CYCLES
        self.MUXSEL = RM_Field_RAC_AUXADCCTRL0_MUXSEL(self)
        self.zz_fdict['MUXSEL'] = self.MUXSEL
        self.CLRCOUNTER = RM_Field_RAC_AUXADCCTRL0_CLRCOUNTER(self)
        self.zz_fdict['CLRCOUNTER'] = self.CLRCOUNTER
        self.CLRFILTER = RM_Field_RAC_AUXADCCTRL0_CLRFILTER(self)
        self.zz_fdict['CLRFILTER'] = self.CLRFILTER
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCCTRL1, self).__init__(rmio, label,
            0xa8020000, 0x0D0,
            'AUXADCCTRL1', 'RAC.AUXADCCTRL1', 'read-write',
            u"",
            0x00000000, 0xF31F6FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCINPUTRESSEL = RM_Field_RAC_AUXADCCTRL1_AUXADCINPUTRESSEL(self)
        self.zz_fdict['AUXADCINPUTRESSEL'] = self.AUXADCINPUTRESSEL
        self.AUXADCINPUTSELECT = RM_Field_RAC_AUXADCCTRL1_AUXADCINPUTSELECT(self)
        self.zz_fdict['AUXADCINPUTSELECT'] = self.AUXADCINPUTSELECT
        self.AUXADCPMONSELECT = RM_Field_RAC_AUXADCCTRL1_AUXADCPMONSELECT(self)
        self.zz_fdict['AUXADCPMONSELECT'] = self.AUXADCPMONSELECT
        self.AUXADCIN6SHIMEN = RM_Field_RAC_AUXADCCTRL1_AUXADCIN6SHIMEN(self)
        self.zz_fdict['AUXADCIN6SHIMEN'] = self.AUXADCIN6SHIMEN
        self.AUXADCIN8SHIMEN = RM_Field_RAC_AUXADCCTRL1_AUXADCIN8SHIMEN(self)
        self.zz_fdict['AUXADCIN8SHIMEN'] = self.AUXADCIN8SHIMEN
        self.AUXADCTSENSESELCURR = RM_Field_RAC_AUXADCCTRL1_AUXADCTSENSESELCURR(self)
        self.zz_fdict['AUXADCTSENSESELCURR'] = self.AUXADCTSENSESELCURR
        self.AUXADCRESET = RM_Field_RAC_AUXADCCTRL1_AUXADCRESET(self)
        self.zz_fdict['AUXADCRESET'] = self.AUXADCRESET
        self.AUXADCTSENSESELVBE = RM_Field_RAC_AUXADCCTRL1_AUXADCTSENSESELVBE(self)
        self.zz_fdict['AUXADCTSENSESELVBE'] = self.AUXADCTSENSESELVBE
        self.AUXADCTHERMISTORFREQSEL = RM_Field_RAC_AUXADCCTRL1_AUXADCTHERMISTORFREQSEL(self)
        self.zz_fdict['AUXADCTHERMISTORFREQSEL'] = self.AUXADCTHERMISTORFREQSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCOUT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCOUT, self).__init__(rmio, label,
            0xa8020000, 0x0D4,
            'AUXADCOUT', 'RAC.AUXADCOUT', 'read-only',
            u"",
            0x00000000, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCOUT = RM_Field_RAC_AUXADCOUT_AUXADCOUT(self)
        self.zz_fdict['AUXADCOUT'] = self.AUXADCOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CLKMULTEN0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CLKMULTEN0, self).__init__(rmio, label,
            0xa8020000, 0x0D8,
            'CLKMULTEN0', 'RAC.CLKMULTEN0', 'read-write',
            u"",
            0xAA400005, 0xFFDFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKMULTBWCAL = RM_Field_RAC_CLKMULTEN0_CLKMULTBWCAL(self)
        self.zz_fdict['CLKMULTBWCAL'] = self.CLKMULTBWCAL
        self.CLKMULTDISICO = RM_Field_RAC_CLKMULTEN0_CLKMULTDISICO(self)
        self.zz_fdict['CLKMULTDISICO'] = self.CLKMULTDISICO
        self.CLKMULTENBBDET = RM_Field_RAC_CLKMULTEN0_CLKMULTENBBDET(self)
        self.zz_fdict['CLKMULTENBBDET'] = self.CLKMULTENBBDET
        self.CLKMULTENBBXLDET = RM_Field_RAC_CLKMULTEN0_CLKMULTENBBXLDET(self)
        self.zz_fdict['CLKMULTENBBXLDET'] = self.CLKMULTENBBXLDET
        self.CLKMULTENBBXMDET = RM_Field_RAC_CLKMULTEN0_CLKMULTENBBXMDET(self)
        self.zz_fdict['CLKMULTENBBXMDET'] = self.CLKMULTENBBXMDET
        self.CLKMULTENCFDET = RM_Field_RAC_CLKMULTEN0_CLKMULTENCFDET(self)
        self.zz_fdict['CLKMULTENCFDET'] = self.CLKMULTENCFDET
        self.CLKMULTENDITHER = RM_Field_RAC_CLKMULTEN0_CLKMULTENDITHER(self)
        self.zz_fdict['CLKMULTENDITHER'] = self.CLKMULTENDITHER
        self.CLKMULTENDRVADC = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVADC(self)
        self.zz_fdict['CLKMULTENDRVADC'] = self.CLKMULTENDRVADC
        self.CLKMULTENDRVN = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVN(self)
        self.zz_fdict['CLKMULTENDRVN'] = self.CLKMULTENDRVN
        self.CLKMULTENDRVP = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVP(self)
        self.zz_fdict['CLKMULTENDRVP'] = self.CLKMULTENDRVP
        self.CLKMULTENDRVRX2P4G = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVRX2P4G(self)
        self.zz_fdict['CLKMULTENDRVRX2P4G'] = self.CLKMULTENDRVRX2P4G
        self.CLKMULTENDRVRXSUBG = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVRXSUBG(self)
        self.zz_fdict['CLKMULTENDRVRXSUBG'] = self.CLKMULTENDRVRXSUBG
        self.CLKMULTENDRVTXDUALB = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVTXDUALB(self)
        self.zz_fdict['CLKMULTENDRVTXDUALB'] = self.CLKMULTENDRVTXDUALB
        self.CLKMULTENFBDIV = RM_Field_RAC_CLKMULTEN0_CLKMULTENFBDIV(self)
        self.zz_fdict['CLKMULTENFBDIV'] = self.CLKMULTENFBDIV
        self.CLKMULTENREFDIV = RM_Field_RAC_CLKMULTEN0_CLKMULTENREFDIV(self)
        self.zz_fdict['CLKMULTENREFDIV'] = self.CLKMULTENREFDIV
        self.CLKMULTENREG1 = RM_Field_RAC_CLKMULTEN0_CLKMULTENREG1(self)
        self.zz_fdict['CLKMULTENREG1'] = self.CLKMULTENREG1
        self.CLKMULTENREG2 = RM_Field_RAC_CLKMULTEN0_CLKMULTENREG2(self)
        self.zz_fdict['CLKMULTENREG2'] = self.CLKMULTENREG2
        self.CLKMULTENREG3 = RM_Field_RAC_CLKMULTEN0_CLKMULTENREG3(self)
        self.zz_fdict['CLKMULTENREG3'] = self.CLKMULTENREG3
        self.CLKMULTENROTDET = RM_Field_RAC_CLKMULTEN0_CLKMULTENROTDET(self)
        self.zz_fdict['CLKMULTENROTDET'] = self.CLKMULTENROTDET
        self.CLKMULTENBYPASS40MHZ = RM_Field_RAC_CLKMULTEN0_CLKMULTENBYPASS40MHZ(self)
        self.zz_fdict['CLKMULTENBYPASS40MHZ'] = self.CLKMULTENBYPASS40MHZ
        self.CLKMULTFREQCAL = RM_Field_RAC_CLKMULTEN0_CLKMULTFREQCAL(self)
        self.zz_fdict['CLKMULTFREQCAL'] = self.CLKMULTFREQCAL
        self.CLKMULTREG2ADJI = RM_Field_RAC_CLKMULTEN0_CLKMULTREG2ADJI(self)
        self.zz_fdict['CLKMULTREG2ADJI'] = self.CLKMULTREG2ADJI
        self.CLKMULTREG1ADJV = RM_Field_RAC_CLKMULTEN0_CLKMULTREG1ADJV(self)
        self.zz_fdict['CLKMULTREG1ADJV'] = self.CLKMULTREG1ADJV
        self.CLKMULTREG2ADJV = RM_Field_RAC_CLKMULTEN0_CLKMULTREG2ADJV(self)
        self.zz_fdict['CLKMULTREG2ADJV'] = self.CLKMULTREG2ADJV
        self.CLKMULTREG3ADJV = RM_Field_RAC_CLKMULTEN0_CLKMULTREG3ADJV(self)
        self.zz_fdict['CLKMULTREG3ADJV'] = self.CLKMULTREG3ADJV
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CLKMULTEN1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CLKMULTEN1, self).__init__(rmio, label,
            0xa8020000, 0x0DC,
            'CLKMULTEN1', 'RAC.CLKMULTEN1', 'read-write',
            u"",
            0x00000188, 0x0001FDEF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKMULTINNIBBLE = RM_Field_RAC_CLKMULTEN1_CLKMULTINNIBBLE(self)
        self.zz_fdict['CLKMULTINNIBBLE'] = self.CLKMULTINNIBBLE
        self.CLKMULTLDFNIB = RM_Field_RAC_CLKMULTEN1_CLKMULTLDFNIB(self)
        self.zz_fdict['CLKMULTLDFNIB'] = self.CLKMULTLDFNIB
        self.CLKMULTLDMNIB = RM_Field_RAC_CLKMULTEN1_CLKMULTLDMNIB(self)
        self.zz_fdict['CLKMULTLDMNIB'] = self.CLKMULTLDMNIB
        self.CLKMULTRDNIBBLE = RM_Field_RAC_CLKMULTEN1_CLKMULTRDNIBBLE(self)
        self.zz_fdict['CLKMULTRDNIBBLE'] = self.CLKMULTRDNIBBLE
        self.CLKMULTLDCNIB = RM_Field_RAC_CLKMULTEN1_CLKMULTLDCNIB(self)
        self.zz_fdict['CLKMULTLDCNIB'] = self.CLKMULTLDCNIB
        self.CLKMULTDRVAMPSEL = RM_Field_RAC_CLKMULTEN1_CLKMULTDRVAMPSEL(self)
        self.zz_fdict['CLKMULTDRVAMPSEL'] = self.CLKMULTDRVAMPSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CLKMULTCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CLKMULTCTRL, self).__init__(rmio, label,
            0xa8020000, 0x0E0,
            'CLKMULTCTRL', 'RAC.CLKMULTCTRL', 'read-write',
            u"",
            0x000000C0, 0x00007FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKMULTDIVN = RM_Field_RAC_CLKMULTCTRL_CLKMULTDIVN(self)
        self.zz_fdict['CLKMULTDIVN'] = self.CLKMULTDIVN
        self.CLKMULTDIVR = RM_Field_RAC_CLKMULTCTRL_CLKMULTDIVR(self)
        self.zz_fdict['CLKMULTDIVR'] = self.CLKMULTDIVR
        self.CLKMULTDIVX = RM_Field_RAC_CLKMULTCTRL_CLKMULTDIVX(self)
        self.zz_fdict['CLKMULTDIVX'] = self.CLKMULTDIVX
        self.CLKMULTENRESYNC = RM_Field_RAC_CLKMULTCTRL_CLKMULTENRESYNC(self)
        self.zz_fdict['CLKMULTENRESYNC'] = self.CLKMULTENRESYNC
        self.CLKMULTVALID = RM_Field_RAC_CLKMULTCTRL_CLKMULTVALID(self)
        self.zz_fdict['CLKMULTVALID'] = self.CLKMULTVALID
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CLKMULTSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CLKMULTSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x0E4,
            'CLKMULTSTATUS', 'RAC.CLKMULTSTATUS', 'read-only',
            u"",
            0x00000000, 0x0000001F,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKMULTOUTNIBBLE = RM_Field_RAC_CLKMULTSTATUS_CLKMULTOUTNIBBLE(self)
        self.zz_fdict['CLKMULTOUTNIBBLE'] = self.CLKMULTOUTNIBBLE
        self.CLKMULTACKVALID = RM_Field_RAC_CLKMULTSTATUS_CLKMULTACKVALID(self)
        self.zz_fdict['CLKMULTACKVALID'] = self.CLKMULTACKVALID
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCDBG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCDBG, self).__init__(rmio, label,
            0xa8020000, 0x0E8,
            'IFADCDBG', 'RAC.IFADCDBG', 'read-write',
            u"",
            0x0000000E, 0x0000007F,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCENADC = RM_Field_RAC_IFADCDBG_IFADCENADC(self)
        self.zz_fdict['IFADCENADC'] = self.IFADCENADC
        self.IFADCENCLK = RM_Field_RAC_IFADCDBG_IFADCENCLK(self)
        self.zz_fdict['IFADCENCLK'] = self.IFADCENCLK
        self.IFADCENREFBUF = RM_Field_RAC_IFADCDBG_IFADCENREFBUF(self)
        self.zz_fdict['IFADCENREFBUF'] = self.IFADCENREFBUF
        self.IFADCENSIDETONE = RM_Field_RAC_IFADCDBG_IFADCENSIDETONE(self)
        self.zz_fdict['IFADCENSIDETONE'] = self.IFADCENSIDETONE
        self.IFADCINVERTCLK = RM_Field_RAC_IFADCDBG_IFADCINVERTCLK(self)
        self.zz_fdict['IFADCINVERTCLK'] = self.IFADCINVERTCLK
        self.IFADCSHORTINPUT = RM_Field_RAC_IFADCDBG_IFADCSHORTINPUT(self)
        self.zz_fdict['IFADCSHORTINPUT'] = self.IFADCSHORTINPUT
        self.IFADCDISABLECLK = RM_Field_RAC_IFADCDBG_IFADCDISABLECLK(self)
        self.zz_fdict['IFADCDISABLECLK'] = self.IFADCDISABLECLK
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCTRIM0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCTRIM0, self).__init__(rmio, label,
            0xa8020000, 0x0EC,
            'IFADCTRIM0', 'RAC.IFADCTRIM0', 'read-write',
            u"",
            0x11512C6C, 0x7FFFFFFD,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCCLKSEL = RM_Field_RAC_IFADCTRIM0_IFADCCLKSEL(self)
        self.zz_fdict['IFADCCLKSEL'] = self.IFADCCLKSEL
        self.IFADCLDOSERIESAMPLVL = RM_Field_RAC_IFADCTRIM0_IFADCLDOSERIESAMPLVL(self)
        self.zz_fdict['IFADCLDOSERIESAMPLVL'] = self.IFADCLDOSERIESAMPLVL
        self.IFADCLDOSHUNTAMPLVL1 = RM_Field_RAC_IFADCTRIM0_IFADCLDOSHUNTAMPLVL1(self)
        self.zz_fdict['IFADCLDOSHUNTAMPLVL1'] = self.IFADCLDOSHUNTAMPLVL1
        self.IFADCLDOSHUNTAMPLVL2 = RM_Field_RAC_IFADCTRIM0_IFADCLDOSHUNTAMPLVL2(self)
        self.zz_fdict['IFADCLDOSHUNTAMPLVL2'] = self.IFADCLDOSHUNTAMPLVL2
        self.IFADCLDOSHUNTCURLVL1 = RM_Field_RAC_IFADCTRIM0_IFADCLDOSHUNTCURLVL1(self)
        self.zz_fdict['IFADCLDOSHUNTCURLVL1'] = self.IFADCLDOSHUNTCURLVL1
        self.IFADCLDOSHUNTCURLVL2 = RM_Field_RAC_IFADCTRIM0_IFADCLDOSHUNTCURLVL2(self)
        self.zz_fdict['IFADCLDOSHUNTCURLVL2'] = self.IFADCLDOSHUNTCURLVL2
        self.IFADCOTACURRENT = RM_Field_RAC_IFADCTRIM0_IFADCOTACURRENT(self)
        self.zz_fdict['IFADCOTACURRENT'] = self.IFADCOTACURRENT
        self.IFADCREFBUFAMPLVL = RM_Field_RAC_IFADCTRIM0_IFADCREFBUFAMPLVL(self)
        self.zz_fdict['IFADCREFBUFAMPLVL'] = self.IFADCREFBUFAMPLVL
        self.IFADCREFBUFCURLVL = RM_Field_RAC_IFADCTRIM0_IFADCREFBUFCURLVL(self)
        self.zz_fdict['IFADCREFBUFCURLVL'] = self.IFADCREFBUFCURLVL
        self.IFADCSIDETONEAMP = RM_Field_RAC_IFADCTRIM0_IFADCSIDETONEAMP(self)
        self.zz_fdict['IFADCSIDETONEAMP'] = self.IFADCSIDETONEAMP
        self.IFADCSIDETONEFREQ = RM_Field_RAC_IFADCTRIM0_IFADCSIDETONEFREQ(self)
        self.zz_fdict['IFADCSIDETONEFREQ'] = self.IFADCSIDETONEFREQ
        self.IFADCENHALFMODE = RM_Field_RAC_IFADCTRIM0_IFADCENHALFMODE(self)
        self.zz_fdict['IFADCENHALFMODE'] = self.IFADCENHALFMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCTRIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCTRIM1, self).__init__(rmio, label,
            0xa8020000, 0x0F0,
            'IFADCTRIM1', 'RAC.IFADCTRIM1', 'read-write',
            u"",
            0x00000123, 0x00003FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCVCMLVL = RM_Field_RAC_IFADCTRIM1_IFADCVCMLVL(self)
        self.zz_fdict['IFADCVCMLVL'] = self.IFADCVCMLVL
        self.IFADCENNEGRES = RM_Field_RAC_IFADCTRIM1_IFADCENNEGRES(self)
        self.zz_fdict['IFADCENNEGRES'] = self.IFADCENNEGRES
        self.IFADCNEGRESCURRENT = RM_Field_RAC_IFADCTRIM1_IFADCNEGRESCURRENT(self)
        self.zz_fdict['IFADCNEGRESCURRENT'] = self.IFADCNEGRESCURRENT
        self.IFADCNEGRESVCM = RM_Field_RAC_IFADCTRIM1_IFADCNEGRESVCM(self)
        self.zz_fdict['IFADCNEGRESVCM'] = self.IFADCNEGRESVCM
        self.IFADCENSUBGMODE = RM_Field_RAC_IFADCTRIM1_IFADCENSUBGMODE(self)
        self.zz_fdict['IFADCENSUBGMODE'] = self.IFADCENSUBGMODE
        self.IFADCTZ = RM_Field_RAC_IFADCTRIM1_IFADCTZ(self)
        self.zz_fdict['IFADCTZ'] = self.IFADCTZ
        self.IFADCENXOBYPASS = RM_Field_RAC_IFADCTRIM1_IFADCENXOBYPASS(self)
        self.zz_fdict['IFADCENXOBYPASS'] = self.IFADCENXOBYPASS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCCAL, self).__init__(rmio, label,
            0xa8020000, 0x0F4,
            'IFADCCAL', 'RAC.IFADCCAL', 'read-write',
            u"",
            0x00000C00, 0x00FF1F03,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCENRCCAL = RM_Field_RAC_IFADCCAL_IFADCENRCCAL(self)
        self.zz_fdict['IFADCENRCCAL'] = self.IFADCENRCCAL
        self.IFADCTUNERCCALMODE = RM_Field_RAC_IFADCCAL_IFADCTUNERCCALMODE(self)
        self.zz_fdict['IFADCTUNERCCALMODE'] = self.IFADCTUNERCCALMODE
        self.IFADCTUNERC = RM_Field_RAC_IFADCCAL_IFADCTUNERC(self)
        self.zz_fdict['IFADCTUNERC'] = self.IFADCTUNERC
        self.IFADCRCCALCOUNTERSTARTVAL = RM_Field_RAC_IFADCCAL_IFADCRCCALCOUNTERSTARTVAL(self)
        self.zz_fdict['IFADCRCCALCOUNTERSTARTVAL'] = self.IFADCRCCALCOUNTERSTARTVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x0F8,
            'IFADCSTATUS', 'RAC.IFADCSTATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCRCCALOUT = RM_Field_RAC_IFADCSTATUS_IFADCRCCALOUT(self)
        self.zz_fdict['IFADCRCCALOUT'] = self.IFADCRCCALOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXDEBUG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXDEBUG, self).__init__(rmio, label,
            0xa8020000, 0x0FC,
            'LNAMIXDEBUG', 'RAC.LNAMIXDEBUG', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXDISMXR0 = RM_Field_RAC_LNAMIXDEBUG_LNAMIXDISMXR0(self)
        self.zz_fdict['LNAMIXDISMXR0'] = self.LNAMIXDISMXR0
        self.LNAMIXDISMXR1 = RM_Field_RAC_LNAMIXDEBUG_LNAMIXDISMXR1(self)
        self.zz_fdict['LNAMIXDISMXR1'] = self.LNAMIXDISMXR1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM0, self).__init__(rmio, label,
            0xa8020000, 0x100,
            'LNAMIXTRIM0', 'RAC.LNAMIXTRIM0', 'read-write',
            u"",
            0x00000110, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXLNA0CAPSEL = RM_Field_RAC_LNAMIXTRIM0_LNAMIXLNA0CAPSEL(self)
        self.zz_fdict['LNAMIXLNA0CAPSEL'] = self.LNAMIXLNA0CAPSEL
        self.LNAMIXMXRBIAS0 = RM_Field_RAC_LNAMIXTRIM0_LNAMIXMXRBIAS0(self)
        self.zz_fdict['LNAMIXMXRBIAS0'] = self.LNAMIXMXRBIAS0
        self.LNAMIXVOUT0ADJ = RM_Field_RAC_LNAMIXTRIM0_LNAMIXVOUT0ADJ(self)
        self.zz_fdict['LNAMIXVOUT0ADJ'] = self.LNAMIXVOUT0ADJ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM1, self).__init__(rmio, label,
            0xa8020000, 0x104,
            'LNAMIXTRIM1', 'RAC.LNAMIXTRIM1', 'read-write',
            u"",
            0x00000110, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXLNA1CAPSEL = RM_Field_RAC_LNAMIXTRIM1_LNAMIXLNA1CAPSEL(self)
        self.zz_fdict['LNAMIXLNA1CAPSEL'] = self.LNAMIXLNA1CAPSEL
        self.LNAMIXMXRBIAS1 = RM_Field_RAC_LNAMIXTRIM1_LNAMIXMXRBIAS1(self)
        self.zz_fdict['LNAMIXMXRBIAS1'] = self.LNAMIXMXRBIAS1
        self.LNAMIXVOUT1ADJ = RM_Field_RAC_LNAMIXTRIM1_LNAMIXVOUT1ADJ(self)
        self.zz_fdict['LNAMIXVOUT1ADJ'] = self.LNAMIXVOUT1ADJ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM2, self).__init__(rmio, label,
            0xa8020000, 0x108,
            'LNAMIXTRIM2', 'RAC.LNAMIXTRIM2', 'read-write',
            u"",
            0x0115E3D0, 0x01FFFFF0,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXCURCTRL = RM_Field_RAC_LNAMIXTRIM2_LNAMIXCURCTRL(self)
        self.zz_fdict['LNAMIXCURCTRL'] = self.LNAMIXCURCTRL
        self.LNAMIXHIGHCUR = RM_Field_RAC_LNAMIXTRIM2_LNAMIXHIGHCUR(self)
        self.zz_fdict['LNAMIXHIGHCUR'] = self.LNAMIXHIGHCUR
        self.LNAMIXLOWCUR = RM_Field_RAC_LNAMIXTRIM2_LNAMIXLOWCUR(self)
        self.zz_fdict['LNAMIXLOWCUR'] = self.LNAMIXLOWCUR
        self.LNAMIXLNARESADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXLNARESADJ(self)
        self.zz_fdict['LNAMIXLNARESADJ'] = self.LNAMIXLNARESADJ
        self.LNAMIXNCASADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXNCASADJ(self)
        self.zz_fdict['LNAMIXNCASADJ'] = self.LNAMIXNCASADJ
        self.LNAMIXPCASADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXPCASADJ(self)
        self.zz_fdict['LNAMIXPCASADJ'] = self.LNAMIXPCASADJ
        self.LNAMIXTRIMVREG = RM_Field_RAC_LNAMIXTRIM2_LNAMIXTRIMVREG(self)
        self.zz_fdict['LNAMIXTRIMVREG'] = self.LNAMIXTRIMVREG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM3, self).__init__(rmio, label,
            0xa8020000, 0x10C,
            'LNAMIXTRIM3', 'RAC.LNAMIXTRIM3', 'read-write',
            u"",
            0x00002A10, 0x00003FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXIBIASADJN = RM_Field_RAC_LNAMIXTRIM3_LNAMIXIBIASADJN(self)
        self.zz_fdict['LNAMIXIBIASADJN'] = self.LNAMIXIBIASADJN
        self.LNAMIXIBIASADJP = RM_Field_RAC_LNAMIXTRIM3_LNAMIXIBIASADJP(self)
        self.zz_fdict['LNAMIXIBIASADJP'] = self.LNAMIXIBIASADJP
        self.LNAMIXIBIASRANGEADJN = RM_Field_RAC_LNAMIXTRIM3_LNAMIXIBIASRANGEADJN(self)
        self.zz_fdict['LNAMIXIBIASRANGEADJN'] = self.LNAMIXIBIASRANGEADJN
        self.LNAMIXIBIASRANGEADJP = RM_Field_RAC_LNAMIXTRIM3_LNAMIXIBIASRANGEADJP(self)
        self.zz_fdict['LNAMIXIBIASRANGEADJP'] = self.LNAMIXIBIASRANGEADJP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM4, self).__init__(rmio, label,
            0xa8020000, 0x110,
            'LNAMIXTRIM4', 'RAC.LNAMIXTRIM4', 'read-write',
            u"",
            0x52082002, 0xFF0FFF03,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXRFPKDBWSEL = RM_Field_RAC_LNAMIXTRIM4_LNAMIXRFPKDBWSEL(self)
        self.zz_fdict['LNAMIXRFPKDBWSEL'] = self.LNAMIXRFPKDBWSEL
        self.LNAMIXRFPKDCALCMLO = RM_Field_RAC_LNAMIXTRIM4_LNAMIXRFPKDCALCMLO(self)
        self.zz_fdict['LNAMIXRFPKDCALCMLO'] = self.LNAMIXRFPKDCALCMLO
        self.LNAMIXRFPKDCALCMHI = RM_Field_RAC_LNAMIXTRIM4_LNAMIXRFPKDCALCMHI(self)
        self.zz_fdict['LNAMIXRFPKDCALCMHI'] = self.LNAMIXRFPKDCALCMHI
        self.LNAMIXRFPKDTHRESHSELLO = RM_Field_RAC_LNAMIXTRIM4_LNAMIXRFPKDTHRESHSELLO(self)
        self.zz_fdict['LNAMIXRFPKDTHRESHSELLO'] = self.LNAMIXRFPKDTHRESHSELLO
        self.LNAMIXRFPKDTHRESHSELHI = RM_Field_RAC_LNAMIXTRIM4_LNAMIXRFPKDTHRESHSELHI(self)
        self.zz_fdict['LNAMIXRFPKDTHRESHSELHI'] = self.LNAMIXRFPKDTHRESHSELHI
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXCAL, self).__init__(rmio, label,
            0xa8020000, 0x114,
            'LNAMIXCAL', 'RAC.LNAMIXCAL', 'read-write',
            u"",
            0x000007E0, 0x000007FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXCALPMOSEN = RM_Field_RAC_LNAMIXCAL_LNAMIXCALPMOSEN(self)
        self.zz_fdict['LNAMIXCALPMOSEN'] = self.LNAMIXCALPMOSEN
        self.LNAMIXCALNMOSEN = RM_Field_RAC_LNAMIXCAL_LNAMIXCALNMOSEN(self)
        self.zz_fdict['LNAMIXCALNMOSEN'] = self.LNAMIXCALNMOSEN
        self.LNAMIXCALVMODE = RM_Field_RAC_LNAMIXCAL_LNAMIXCALVMODE(self)
        self.zz_fdict['LNAMIXCALVMODE'] = self.LNAMIXCALVMODE
        self.LNAMIXENIRCAL0 = RM_Field_RAC_LNAMIXCAL_LNAMIXENIRCAL0(self)
        self.zz_fdict['LNAMIXENIRCAL0'] = self.LNAMIXENIRCAL0
        self.LNAMIXENIRCAL1 = RM_Field_RAC_LNAMIXCAL_LNAMIXENIRCAL1(self)
        self.zz_fdict['LNAMIXENIRCAL1'] = self.LNAMIXENIRCAL1
        self.LNAMIXIRCAL0AMP = RM_Field_RAC_LNAMIXCAL_LNAMIXIRCAL0AMP(self)
        self.zz_fdict['LNAMIXIRCAL0AMP'] = self.LNAMIXIRCAL0AMP
        self.LNAMIXIRCAL1AMP = RM_Field_RAC_LNAMIXCAL_LNAMIXIRCAL1AMP(self)
        self.zz_fdict['LNAMIXIRCAL1AMP'] = self.LNAMIXIRCAL1AMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXEN, self).__init__(rmio, label,
            0xa8020000, 0x118,
            'LNAMIXEN', 'RAC.LNAMIXEN', 'read-write',
            u"",
            0x00000000, 0x0000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXENAMP0 = RM_Field_RAC_LNAMIXEN_LNAMIXENAMP0(self)
        self.zz_fdict['LNAMIXENAMP0'] = self.LNAMIXENAMP0
        self.LNAMIXENAMP1 = RM_Field_RAC_LNAMIXEN_LNAMIXENAMP1(self)
        self.zz_fdict['LNAMIXENAMP1'] = self.LNAMIXENAMP1
        self.LNAMIXENBIAS = RM_Field_RAC_LNAMIXEN_LNAMIXENBIAS(self)
        self.zz_fdict['LNAMIXENBIAS'] = self.LNAMIXENBIAS
        self.LNAMIXENLDO = RM_Field_RAC_LNAMIXEN_LNAMIXENLDO(self)
        self.zz_fdict['LNAMIXENLDO'] = self.LNAMIXENLDO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PRECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PRECTRL, self).__init__(rmio, label,
            0xa8020000, 0x11C,
            'PRECTRL', 'RAC.PRECTRL', 'read-write',
            u"",
            0x00000026, 0x0000003F,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREBYPFORCE = RM_Field_RAC_PRECTRL_PREBYPFORCE(self)
        self.zz_fdict['PREBYPFORCE'] = self.PREBYPFORCE
        self.PREREGTRIM = RM_Field_RAC_PRECTRL_PREREGTRIM(self)
        self.zz_fdict['PREREGTRIM'] = self.PREREGTRIM
        self.PREVREFTRIM = RM_Field_RAC_PRECTRL_PREVREFTRIM(self)
        self.zz_fdict['PREVREFTRIM'] = self.PREVREFTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM1, self).__init__(rmio, label,
            0xa8020000, 0x124,
            'PATRIM1', 'RAC.PATRIM1', 'read-write',
            u"",
            0x00234387, 0x00FFC78F,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMXPAVNB = RM_Field_RAC_PATRIM1_TXTRIMXPAVNB(self)
        self.zz_fdict['TXTRIMXPAVNB'] = self.TXTRIMXPAVNB
        self.TXTRIMXPAVPB = RM_Field_RAC_PATRIM1_TXTRIMXPAVPB(self)
        self.zz_fdict['TXTRIMXPAVPB'] = self.TXTRIMXPAVPB
        self.TXTRIMHPAVMID = RM_Field_RAC_PATRIM1_TXTRIMHPAVMID(self)
        self.zz_fdict['TXTRIMHPAVMID'] = self.TXTRIMHPAVMID
        self.TXTRIMHPAVNCAS = RM_Field_RAC_PATRIM1_TXTRIMHPAVNCAS(self)
        self.zz_fdict['TXTRIMHPAVNCAS'] = self.TXTRIMHPAVNCAS
        self.TXTRIMHPAVPCAS = RM_Field_RAC_PATRIM1_TXTRIMHPAVPCAS(self)
        self.zz_fdict['TXTRIMHPAVPCAS'] = self.TXTRIMHPAVPCAS
        self.TXTRIMHPAVMIDCURRENT = RM_Field_RAC_PATRIM1_TXTRIMHPAVMIDCURRENT(self)
        self.zz_fdict['TXTRIMHPAVMIDCURRENT'] = self.TXTRIMHPAVMIDCURRENT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM2, self).__init__(rmio, label,
            0xa8020000, 0x128,
            'PATRIM2', 'RAC.PATRIM2', 'read-write',
            u"",
            0x00007777, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMCLKGENDUTYN = RM_Field_RAC_PATRIM2_TXTRIMCLKGENDUTYN(self)
        self.zz_fdict['TXTRIMCLKGENDUTYN'] = self.TXTRIMCLKGENDUTYN
        self.TXTRIMCLKGENDUTYP = RM_Field_RAC_PATRIM2_TXTRIMCLKGENDUTYP(self)
        self.zz_fdict['TXTRIMCLKGENDUTYP'] = self.TXTRIMCLKGENDUTYP
        self.TXTRIMCLKGENNOVFALL = RM_Field_RAC_PATRIM2_TXTRIMCLKGENNOVFALL(self)
        self.zz_fdict['TXTRIMCLKGENNOVFALL'] = self.TXTRIMCLKGENNOVFALL
        self.TXTRIMCLKGENNOVRISE = RM_Field_RAC_PATRIM2_TXTRIMCLKGENNOVRISE(self)
        self.zz_fdict['TXTRIMCLKGENNOVRISE'] = self.TXTRIMCLKGENNOVRISE
        self.TXTRIMDRVEDGENN = RM_Field_RAC_PATRIM2_TXTRIMDRVEDGENN(self)
        self.zz_fdict['TXTRIMDRVEDGENN'] = self.TXTRIMDRVEDGENN
        self.TXTRIMDRVEDGENP = RM_Field_RAC_PATRIM2_TXTRIMDRVEDGENP(self)
        self.zz_fdict['TXTRIMDRVEDGENP'] = self.TXTRIMDRVEDGENP
        self.TXTRIMDRVEDGEPN = RM_Field_RAC_PATRIM2_TXTRIMDRVEDGEPN(self)
        self.zz_fdict['TXTRIMDRVEDGEPN'] = self.TXTRIMDRVEDGEPN
        self.TXTRIMDRVEDGEPP = RM_Field_RAC_PATRIM2_TXTRIMDRVEDGEPP(self)
        self.zz_fdict['TXTRIMDRVEDGEPP'] = self.TXTRIMDRVEDGEPP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM3, self).__init__(rmio, label,
            0xa8020000, 0x12C,
            'PATRIM3', 'RAC.PATRIM3', 'read-write',
            u"",
            0x008F0EE9, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMDREGBLEED = RM_Field_RAC_PATRIM3_TXTRIMDREGBLEED(self)
        self.zz_fdict['TXTRIMDREGBLEED'] = self.TXTRIMDREGBLEED
        self.TXTRIMDREGBLEEDAUTO = RM_Field_RAC_PATRIM3_TXTRIMDREGBLEEDAUTO(self)
        self.zz_fdict['TXTRIMDREGBLEEDAUTO'] = self.TXTRIMDREGBLEEDAUTO
        self.TXTRIMOREGBLEEDAUTO = RM_Field_RAC_PATRIM3_TXTRIMOREGBLEEDAUTO(self)
        self.zz_fdict['TXTRIMOREGBLEEDAUTO'] = self.TXTRIMOREGBLEEDAUTO
        self.TXTRIMDREGFB = RM_Field_RAC_PATRIM3_TXTRIMDREGFB(self)
        self.zz_fdict['TXTRIMDREGFB'] = self.TXTRIMDREGFB
        self.TXTRIMOREGFB = RM_Field_RAC_PATRIM3_TXTRIMOREGFB(self)
        self.zz_fdict['TXTRIMOREGFB'] = self.TXTRIMOREGFB
        self.TXTRIMDREG = RM_Field_RAC_PATRIM3_TXTRIMDREG(self)
        self.zz_fdict['TXTRIMDREG'] = self.TXTRIMDREG
        self.TXTRIMOREG = RM_Field_RAC_PATRIM3_TXTRIMOREG(self)
        self.zz_fdict['TXTRIMOREG'] = self.TXTRIMOREG
        self.TXTRIMRREG = RM_Field_RAC_PATRIM3_TXTRIMRREG(self)
        self.zz_fdict['TXTRIMRREG'] = self.TXTRIMRREG
        self.TXTRIMDREGPSR = RM_Field_RAC_PATRIM3_TXTRIMDREGPSR(self)
        self.zz_fdict['TXTRIMDREGPSR'] = self.TXTRIMDREGPSR
        self.TXTRIMOREGPSR = RM_Field_RAC_PATRIM3_TXTRIMOREGPSR(self)
        self.zz_fdict['TXTRIMOREGPSR'] = self.TXTRIMOREGPSR
        self.TXTRIMDREGSLICES = RM_Field_RAC_PATRIM3_TXTRIMDREGSLICES(self)
        self.zz_fdict['TXTRIMDREGSLICES'] = self.TXTRIMDREGSLICES
        self.TXTRIMOREGSLICES = RM_Field_RAC_PATRIM3_TXTRIMOREGSLICES(self)
        self.zz_fdict['TXTRIMOREGSLICES'] = self.TXTRIMOREGSLICES
        self.TXTRIMDREGMOREBW = RM_Field_RAC_PATRIM3_TXTRIMDREGMOREBW(self)
        self.zz_fdict['TXTRIMDREGMOREBW'] = self.TXTRIMDREGMOREBW
        self.TXTRIMOREGMOREBW = RM_Field_RAC_PATRIM3_TXTRIMOREGMOREBW(self)
        self.zz_fdict['TXTRIMOREGMOREBW'] = self.TXTRIMOREGMOREBW
        self.TXTRIMRREGCN = RM_Field_RAC_PATRIM3_TXTRIMRREGCN(self)
        self.zz_fdict['TXTRIMRREGCN'] = self.TXTRIMRREGCN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM4, self).__init__(rmio, label,
            0xa8020000, 0x130,
            'PATRIM4', 'RAC.PATRIM4', 'read-write',
            u"",
            0x00FA0075, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMXPAVNBDRV = RM_Field_RAC_PATRIM4_TXTRIMXPAVNBDRV(self)
        self.zz_fdict['TXTRIMXPAVNBDRV'] = self.TXTRIMXPAVNBDRV
        self.TXTRIMXPAVPBDRV = RM_Field_RAC_PATRIM4_TXTRIMXPAVPBDRV(self)
        self.zz_fdict['TXTRIMXPAVPBDRV'] = self.TXTRIMXPAVPBDRV
        self.TXTRIMXPAVXSCURRENT = RM_Field_RAC_PATRIM4_TXTRIMXPAVXSCURRENT(self)
        self.zz_fdict['TXTRIMXPAVXSCURRENT'] = self.TXTRIMXPAVXSCURRENT
        self.TXXPAVXSCURRENTCOARSE = RM_Field_RAC_PATRIM4_TXXPAVXSCURRENTCOARSE(self)
        self.zz_fdict['TXXPAVXSCURRENTCOARSE'] = self.TXXPAVXSCURRENTCOARSE
        self.TXXPAVXSCURRENTCOARSEMIN = RM_Field_RAC_PATRIM4_TXXPAVXSCURRENTCOARSEMIN(self)
        self.zz_fdict['TXXPAVXSCURRENTCOARSEMIN'] = self.TXXPAVXSCURRENTCOARSEMIN
        self.TXXPAVXSCURRENTFINE = RM_Field_RAC_PATRIM4_TXXPAVXSCURRENTFINE(self)
        self.zz_fdict['TXXPAVXSCURRENTFINE'] = self.TXXPAVXSCURRENTFINE
        self.TXXPAVXSCURRENTSELHPA = RM_Field_RAC_PATRIM4_TXXPAVXSCURRENTSELHPA(self)
        self.zz_fdict['TXXPAVXSCURRENTSELHPA'] = self.TXXPAVXSCURRENTSELHPA
        self.TXVXSCURRENTVREFN = RM_Field_RAC_PATRIM4_TXVXSCURRENTVREFN(self)
        self.zz_fdict['TXVXSCURRENTVREFN'] = self.TXVXSCURRENTVREFN
        self.TXVXSCURRENTVREFP = RM_Field_RAC_PATRIM4_TXVXSCURRENTVREFP(self)
        self.zz_fdict['TXVXSCURRENTVREFP'] = self.TXVXSCURRENTVREFP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM5, self).__init__(rmio, label,
            0xa8020000, 0x134,
            'PATRIM5', 'RAC.PATRIM5', 'read-write',
            u"",
            0x000003F0, 0x000003F7,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXVNSRAIL = RM_Field_RAC_PATRIM5_TXVNSRAIL(self)
        self.zz_fdict['TXVNSRAIL'] = self.TXVNSRAIL
        self.TXVPSRAIL = RM_Field_RAC_PATRIM5_TXVPSRAIL(self)
        self.zz_fdict['TXVPSRAIL'] = self.TXVPSRAIL
        self.TXVCASRAIL = RM_Field_RAC_PATRIM5_TXVCASRAIL(self)
        self.zz_fdict['TXVCASRAIL'] = self.TXVCASRAIL
        self.TXTRIMXPAVNS = RM_Field_RAC_PATRIM5_TXTRIMXPAVNS(self)
        self.zz_fdict['TXTRIMXPAVNS'] = self.TXTRIMXPAVNS
        self.TXTRIMXPAVPS = RM_Field_RAC_PATRIM5_TXTRIMXPAVPS(self)
        self.zz_fdict['TXTRIMXPAVPS'] = self.TXTRIMXPAVPS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PADEBUG0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PADEBUG0, self).__init__(rmio, label,
            0xa8020000, 0x138,
            'PADEBUG0', 'RAC.PADEBUG0', 'read-write',
            u"",
            0x00000000, 0x0000000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXOREGBYPASS = RM_Field_RAC_PADEBUG0_TXOREGBYPASS(self)
        self.zz_fdict['TXOREGBYPASS'] = self.TXOREGBYPASS
        self.TXDREGBYPASS = RM_Field_RAC_PADEBUG0_TXDREGBYPASS(self)
        self.zz_fdict['TXDREGBYPASS'] = self.TXDREGBYPASS
        self.TXRREGBYPASS = RM_Field_RAC_PADEBUG0_TXRREGBYPASS(self)
        self.zz_fdict['TXRREGBYPASS'] = self.TXRREGBYPASS
        self.TXPDNVDDPA = RM_Field_RAC_PADEBUG0_TXPDNVDDPA(self)
        self.zz_fdict['TXPDNVDDPA'] = self.TXPDNVDDPA
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXPOWER(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXPOWER, self).__init__(rmio, label,
            0xa8020000, 0x13C,
            'TXPOWER', 'RAC.TXPOWER', 'read-write',
            u"",
            0x007FE000, 0x007FEEFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXSTRIPE = RM_Field_RAC_TXPOWER_TXSTRIPE(self)
        self.zz_fdict['TXSTRIPE'] = self.TXSTRIPE
        self.TXSLICE = RM_Field_RAC_TXPOWER_TXSLICE(self)
        self.zz_fdict['TXSLICE'] = self.TXSLICE
        self.TXDRVSLICE = RM_Field_RAC_TXPOWER_TXDRVSLICE(self)
        self.zz_fdict['TXDRVSLICE'] = self.TXDRVSLICE
        self.TXANARAMPODEV = RM_Field_RAC_TXPOWER_TXANARAMPODEV(self)
        self.zz_fdict['TXANARAMPODEV'] = self.TXANARAMPODEV
        self.TXANARAMPDRVFF = RM_Field_RAC_TXPOWER_TXANARAMPDRVFF(self)
        self.zz_fdict['TXANARAMPDRVFF'] = self.TXANARAMPDRVFF
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXRAMP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXRAMP, self).__init__(rmio, label,
            0xa8020000, 0x140,
            'TXRAMP', 'RAC.TXRAMP', 'read-write',
            u"",
            0x0000003F, 0x000C0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXANARAMPDRVBW = RM_Field_RAC_TXRAMP_TXANARAMPDRVBW(self)
        self.zz_fdict['TXANARAMPDRVBW'] = self.TXANARAMPDRVBW
        self.TXANARAMPDRVEN = RM_Field_RAC_TXRAMP_TXANARAMPDRVEN(self)
        self.zz_fdict['TXANARAMPDRVEN'] = self.TXANARAMPDRVEN
        self.TXANARAMPODEVBW = RM_Field_RAC_TXRAMP_TXANARAMPODEVBW(self)
        self.zz_fdict['TXANARAMPODEVBW'] = self.TXANARAMPODEVBW
        self.TXANARAMPODEVEN = RM_Field_RAC_TXRAMP_TXANARAMPODEVEN(self)
        self.zz_fdict['TXANARAMPODEVEN'] = self.TXANARAMPODEVEN
        self.TXBYPASSRAMPCLK = RM_Field_RAC_TXRAMP_TXBYPASSRAMPCLK(self)
        self.zz_fdict['TXBYPASSRAMPCLK'] = self.TXBYPASSRAMPCLK
        self.TXMODEPHASEFLIP = RM_Field_RAC_TXRAMP_TXMODEPHASEFLIP(self)
        self.zz_fdict['TXMODEPHASEFLIP'] = self.TXMODEPHASEFLIP
        self.TXPHASEDIGITAL = RM_Field_RAC_TXRAMP_TXPHASEDIGITAL(self)
        self.zz_fdict['TXPHASEDIGITAL'] = self.TXPHASEDIGITAL
        self.TXRESETPAPOWER = RM_Field_RAC_TXRAMP_TXRESETPAPOWER(self)
        self.zz_fdict['TXRESETPAPOWER'] = self.TXRESETPAPOWER
        self.TXUNMUTEENDRVFF = RM_Field_RAC_TXRAMP_TXUNMUTEENDRVFF(self)
        self.zz_fdict['TXUNMUTEENDRVFF'] = self.TXUNMUTEENDRVFF
        self.TXBYPASSBBDIGREG = RM_Field_RAC_TXRAMP_TXBYPASSBBDIGREG(self)
        self.zz_fdict['TXBYPASSBBDIGREG'] = self.TXBYPASSBBDIGREG
        self.TXBYPASSBUFREG = RM_Field_RAC_TXRAMP_TXBYPASSBUFREG(self)
        self.zz_fdict['TXBYPASSBUFREG'] = self.TXBYPASSBUFREG
        self.TXSTARTUPITCCORE = RM_Field_RAC_TXRAMP_TXSTARTUPITCCORE(self)
        self.zz_fdict['TXSTARTUPITCCORE'] = self.TXSTARTUPITCCORE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGATRIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGATRIM, self).__init__(rmio, label,
            0xa8020000, 0x148,
            'PGATRIM', 'RAC.PGATRIM', 'read-write',
            u"",
            0x00000A87, 0x00000FDF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGACTUNE = RM_Field_RAC_PGATRIM_PGACTUNE(self)
        self.zz_fdict['PGACTUNE'] = self.PGACTUNE
        self.PGAVCMOUTTRIM = RM_Field_RAC_PGATRIM_PGAVCMOUTTRIM(self)
        self.zz_fdict['PGAVCMOUTTRIM'] = self.PGAVCMOUTTRIM
        self.PGAVLDOTRIM = RM_Field_RAC_PGATRIM_PGAVLDOTRIM(self)
        self.zz_fdict['PGAVLDOTRIM'] = self.PGAVLDOTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGACAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGACAL, self).__init__(rmio, label,
            0xa8020000, 0x14C,
            'PGACAL', 'RAC.PGACAL', 'read-write',
            u"",
            0x20200000, 0x3F3F0000,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGAOFFCALI = RM_Field_RAC_PGACAL_PGAOFFCALI(self)
        self.zz_fdict['PGAOFFCALI'] = self.PGAOFFCALI
        self.PGAOFFCALQ = RM_Field_RAC_PGACAL_PGAOFFCALQ(self)
        self.zz_fdict['PGAOFFCALQ'] = self.PGAOFFCALQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGACTRL, self).__init__(rmio, label,
            0xa8020000, 0x150,
            'PGACTRL', 'RAC.PGACTRL', 'read-write',
            u"",
            0x00000003, 0x01FFEFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGABWMODE = RM_Field_RAC_PGACTRL_PGABWMODE(self)
        self.zz_fdict['PGABWMODE'] = self.PGABWMODE
        self.PGAENBIAS = RM_Field_RAC_PGACTRL_PGAENBIAS(self)
        self.zz_fdict['PGAENBIAS'] = self.PGAENBIAS
        self.PGAENGHZ = RM_Field_RAC_PGACTRL_PGAENGHZ(self)
        self.zz_fdict['PGAENGHZ'] = self.PGAENGHZ
        self.PGAENLATCHI = RM_Field_RAC_PGACTRL_PGAENLATCHI(self)
        self.zz_fdict['PGAENLATCHI'] = self.PGAENLATCHI
        self.PGAENLATCHQ = RM_Field_RAC_PGACTRL_PGAENLATCHQ(self)
        self.zz_fdict['PGAENLATCHQ'] = self.PGAENLATCHQ
        self.PGAENLDOLOAD = RM_Field_RAC_PGACTRL_PGAENLDOLOAD(self)
        self.zz_fdict['PGAENLDOLOAD'] = self.PGAENLDOLOAD
        self.PGAENPGAI = RM_Field_RAC_PGACTRL_PGAENPGAI(self)
        self.zz_fdict['PGAENPGAI'] = self.PGAENPGAI
        self.PGAENPGAQ = RM_Field_RAC_PGACTRL_PGAENPGAQ(self)
        self.zz_fdict['PGAENPGAQ'] = self.PGAENPGAQ
        self.PGAENPKD = RM_Field_RAC_PGACTRL_PGAENPKD(self)
        self.zz_fdict['PGAENPKD'] = self.PGAENPKD
        self.PGAENSUBG = RM_Field_RAC_PGACTRL_PGAENSUBG(self)
        self.zz_fdict['PGAENSUBG'] = self.PGAENSUBG
        self.PGAPOWERMODE = RM_Field_RAC_PGACTRL_PGAPOWERMODE(self)
        self.zz_fdict['PGAPOWERMODE'] = self.PGAPOWERMODE
        self.PGATHRPKDLOSEL = RM_Field_RAC_PGACTRL_PGATHRPKDLOSEL(self)
        self.zz_fdict['PGATHRPKDLOSEL'] = self.PGATHRPKDLOSEL
        self.PGATHRPKDHISEL = RM_Field_RAC_PGACTRL_PGATHRPKDHISEL(self)
        self.zz_fdict['PGATHRPKDHISEL'] = self.PGATHRPKDHISEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFBIASCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFBIASCAL, self).__init__(rmio, label,
            0xa8020000, 0x154,
            'RFBIASCAL', 'RAC.RFBIASCAL', 'read-write',
            u"",
            0x30203020, 0x3F3F3F3F,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFBIASCALBIAS = RM_Field_RAC_RFBIASCAL_RFBIASCALBIAS(self)
        self.zz_fdict['RFBIASCALBIAS'] = self.RFBIASCALBIAS
        self.RFBIASCALTC = RM_Field_RAC_RFBIASCAL_RFBIASCALTC(self)
        self.zz_fdict['RFBIASCALTC'] = self.RFBIASCALTC
        self.RFBIASCALVREF = RM_Field_RAC_RFBIASCAL_RFBIASCALVREF(self)
        self.zz_fdict['RFBIASCALVREF'] = self.RFBIASCALVREF
        self.RFBIASCALVREFSTARTUP = RM_Field_RAC_RFBIASCAL_RFBIASCALVREFSTARTUP(self)
        self.zz_fdict['RFBIASCALVREFSTARTUP'] = self.RFBIASCALVREFSTARTUP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFBIASCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFBIASCTRL, self).__init__(rmio, label,
            0xa8020000, 0x158,
            'RFBIASCTRL', 'RAC.RFBIASCTRL', 'read-write',
            u"",
            0x00040000, 0x000F001F,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFBIASDISABLEBOOTSTRAP = RM_Field_RAC_RFBIASCTRL_RFBIASDISABLEBOOTSTRAP(self)
        self.zz_fdict['RFBIASDISABLEBOOTSTRAP'] = self.RFBIASDISABLEBOOTSTRAP
        self.RFBIASLDOHIGHCURRENT = RM_Field_RAC_RFBIASCTRL_RFBIASLDOHIGHCURRENT(self)
        self.zz_fdict['RFBIASLDOHIGHCURRENT'] = self.RFBIASLDOHIGHCURRENT
        self.RFBIASNONFLASHMODE = RM_Field_RAC_RFBIASCTRL_RFBIASNONFLASHMODE(self)
        self.zz_fdict['RFBIASNONFLASHMODE'] = self.RFBIASNONFLASHMODE
        self.RFBIASSTARTUPCORE = RM_Field_RAC_RFBIASCTRL_RFBIASSTARTUPCORE(self)
        self.zz_fdict['RFBIASSTARTUPCORE'] = self.RFBIASSTARTUPCORE
        self.RFBIASSTARTUPSUPPLY = RM_Field_RAC_RFBIASCTRL_RFBIASSTARTUPSUPPLY(self)
        self.zz_fdict['RFBIASSTARTUPSUPPLY'] = self.RFBIASSTARTUPSUPPLY
        self.RFBIASLDOVREFTRIM = RM_Field_RAC_RFBIASCTRL_RFBIASLDOVREFTRIM(self)
        self.zz_fdict['RFBIASLDOVREFTRIM'] = self.RFBIASLDOVREFTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RADIOEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RADIOEN, self).__init__(rmio, label,
            0xa8020000, 0x15C,
            'RADIOEN', 'RAC.RADIOEN', 'read-write',
            u"",
            0x00000000, 0x00000007,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREEN = RM_Field_RAC_RADIOEN_PREEN(self)
        self.zz_fdict['PREEN'] = self.PREEN
        self.PRESTB100UDIS = RM_Field_RAC_RADIOEN_PRESTB100UDIS(self)
        self.zz_fdict['PRESTB100UDIS'] = self.PRESTB100UDIS
        self.RFBIASEN = RM_Field_RAC_RADIOEN_RFBIASEN(self)
        self.zz_fdict['RFBIASEN'] = self.RFBIASEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFPATHEN0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFPATHEN0, self).__init__(rmio, label,
            0xa8020000, 0x160,
            'RFPATHEN0', 'RAC.RFPATHEN0', 'read-write',
            u"",
            0x00000042, 0x0000184F,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXEN0 = RM_Field_RAC_RFPATHEN0_LNAMIXEN0(self)
        self.zz_fdict['LNAMIXEN0'] = self.LNAMIXEN0
        self.LNAMIXRFATTDCEN0 = RM_Field_RAC_RFPATHEN0_LNAMIXRFATTDCEN0(self)
        self.zz_fdict['LNAMIXRFATTDCEN0'] = self.LNAMIXRFATTDCEN0
        self.LNAMIXRFPKDENRF0 = RM_Field_RAC_RFPATHEN0_LNAMIXRFPKDENRF0(self)
        self.zz_fdict['LNAMIXRFPKDENRF0'] = self.LNAMIXRFPKDENRF0
        self.SYLODIVRLO0SUBGEN = RM_Field_RAC_RFPATHEN0_SYLODIVRLO0SUBGEN(self)
        self.zz_fdict['SYLODIVRLO0SUBGEN'] = self.SYLODIVRLO0SUBGEN
        self.TXENVMIDHPA0 = RM_Field_RAC_RFPATHEN0_TXENVMIDHPA0(self)
        self.zz_fdict['TXENVMIDHPA0'] = self.TXENVMIDHPA0
        self.TXENFSKPA0 = RM_Field_RAC_RFPATHEN0_TXENFSKPA0(self)
        self.zz_fdict['TXENFSKPA0'] = self.TXENFSKPA0
        self.TXENOFDMPA0 = RM_Field_RAC_RFPATHEN0_TXENOFDMPA0(self)
        self.zz_fdict['TXENOFDMPA0'] = self.TXENOFDMPA0
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFPATHEN1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFPATHEN1, self).__init__(rmio, label,
            0xa8020000, 0x164,
            'RFPATHEN1', 'RAC.RFPATHEN1', 'read-write',
            u"",
            0x00000042, 0x0000184F,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXEN1 = RM_Field_RAC_RFPATHEN1_LNAMIXEN1(self)
        self.zz_fdict['LNAMIXEN1'] = self.LNAMIXEN1
        self.LNAMIXRFATTDCEN1 = RM_Field_RAC_RFPATHEN1_LNAMIXRFATTDCEN1(self)
        self.zz_fdict['LNAMIXRFATTDCEN1'] = self.LNAMIXRFATTDCEN1
        self.LNAMIXRFPKDENRF1 = RM_Field_RAC_RFPATHEN1_LNAMIXRFPKDENRF1(self)
        self.zz_fdict['LNAMIXRFPKDENRF1'] = self.LNAMIXRFPKDENRF1
        self.SYLODIVRLO1SUBGEN = RM_Field_RAC_RFPATHEN1_SYLODIVRLO1SUBGEN(self)
        self.zz_fdict['SYLODIVRLO1SUBGEN'] = self.SYLODIVRLO1SUBGEN
        self.TXENVMIDHPA1 = RM_Field_RAC_RFPATHEN1_TXENVMIDHPA1(self)
        self.zz_fdict['TXENVMIDHPA1'] = self.TXENVMIDHPA1
        self.TXENFSKPA1 = RM_Field_RAC_RFPATHEN1_TXENFSKPA1(self)
        self.zz_fdict['TXENFSKPA1'] = self.TXENFSKPA1
        self.TXENOFDMPA1 = RM_Field_RAC_RFPATHEN1_TXENOFDMPA1(self)
        self.zz_fdict['TXENOFDMPA1'] = self.TXENOFDMPA1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RX, self).__init__(rmio, label,
            0xa8020000, 0x168,
            'RX', 'RAC.RX', 'read-write',
            u"",
            0x00000000, 0x800F03FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCCAPRESET = RM_Field_RAC_RX_IFADCCAPRESET(self)
        self.zz_fdict['IFADCCAPRESET'] = self.IFADCCAPRESET
        self.IFADCENLDOSERIES = RM_Field_RAC_RX_IFADCENLDOSERIES(self)
        self.zz_fdict['IFADCENLDOSERIES'] = self.IFADCENLDOSERIES
        self.IFADCENLDOSHUNT = RM_Field_RAC_RX_IFADCENLDOSHUNT(self)
        self.zz_fdict['IFADCENLDOSHUNT'] = self.IFADCENLDOSHUNT
        self.LNAMIXENRFPKD = RM_Field_RAC_RX_LNAMIXENRFPKD(self)
        self.zz_fdict['LNAMIXENRFPKD'] = self.LNAMIXENRFPKD
        self.LNAMIXENRFPKDLOTHRESH = RM_Field_RAC_RX_LNAMIXENRFPKDLOTHRESH(self)
        self.zz_fdict['LNAMIXENRFPKDLOTHRESH'] = self.LNAMIXENRFPKDLOTHRESH
        self.LNAMIXLDOLOWCUR = RM_Field_RAC_RX_LNAMIXLDOLOWCUR(self)
        self.zz_fdict['LNAMIXLDOLOWCUR'] = self.LNAMIXLDOLOWCUR
        self.LNAMIXREGLOADEN = RM_Field_RAC_RX_LNAMIXREGLOADEN(self)
        self.zz_fdict['LNAMIXREGLOADEN'] = self.LNAMIXREGLOADEN
        self.PGAENLDO = RM_Field_RAC_RX_PGAENLDO(self)
        self.zz_fdict['PGAENLDO'] = self.PGAENLDO
        self.SYCHPQNC3EN = RM_Field_RAC_RX_SYCHPQNC3EN(self)
        self.zz_fdict['SYCHPQNC3EN'] = self.SYCHPQNC3EN
        self.SYCHPBIASTRIMBUFRX = RM_Field_RAC_RX_SYCHPBIASTRIMBUFRX(self)
        self.zz_fdict['SYCHPBIASTRIMBUFRX'] = self.SYCHPBIASTRIMBUFRX
        self.SYPFDCHPLPENRX = RM_Field_RAC_RX_SYPFDCHPLPENRX(self)
        self.zz_fdict['SYPFDCHPLPENRX'] = self.SYPFDCHPLPENRX
        self.SYPFDFPWENRX = RM_Field_RAC_RX_SYPFDFPWENRX(self)
        self.zz_fdict['SYPFDFPWENRX'] = self.SYPFDFPWENRX
        self.SYVCOVCAPFASTRAMPRX = RM_Field_RAC_RX_SYVCOVCAPFASTRAMPRX(self)
        self.zz_fdict['SYVCOVCAPFASTRAMPRX'] = self.SYVCOVCAPFASTRAMPRX
        self.FEFILTOUTPUTSEL = RM_Field_RAC_RX_FEFILTOUTPUTSEL(self)
        self.zz_fdict['FEFILTOUTPUTSEL'] = self.FEFILTOUTPUTSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TX, self).__init__(rmio, label,
            0xa8020000, 0x16C,
            'TX', 'RAC.TX', 'read-write',
            u"",
            0x00000000, 0xDF3FEFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXENDREG = RM_Field_RAC_TX_TXENDREG(self)
        self.zz_fdict['TXENDREG'] = self.TXENDREG
        self.TXENOREG = RM_Field_RAC_TX_TXENOREG(self)
        self.zz_fdict['TXENOREG'] = self.TXENOREG
        self.TXENOREGBLEED = RM_Field_RAC_TX_TXENOREGBLEED(self)
        self.zz_fdict['TXENOREGBLEED'] = self.TXENOREGBLEED
        self.TXENRREG = RM_Field_RAC_TX_TXENRREG(self)
        self.zz_fdict['TXENRREG'] = self.TXENRREG
        self.TXENCLKGEN = RM_Field_RAC_TX_TXENCLKGEN(self)
        self.zz_fdict['TXENCLKGEN'] = self.TXENCLKGEN
        self.TXENPADDCFORCE = RM_Field_RAC_TX_TXENPADDCFORCE(self)
        self.zz_fdict['TXENPADDCFORCE'] = self.TXENPADDCFORCE
        self.TXENPKDOGND = RM_Field_RAC_TX_TXENPKDOGND(self)
        self.zz_fdict['TXENPKDOGND'] = self.TXENPKDOGND
        self.TXENPKDOGNDCLK = RM_Field_RAC_TX_TXENPKDOGNDCLK(self)
        self.zz_fdict['TXENPKDOGNDCLK'] = self.TXENPKDOGNDCLK
        self.TXENPUPDNPROTECT = RM_Field_RAC_TX_TXENPUPDNPROTECT(self)
        self.zz_fdict['TXENPUPDNPROTECT'] = self.TXENPUPDNPROTECT
        self.TXENRAMPCLK = RM_Field_RAC_TX_TXENRAMPCLK(self)
        self.zz_fdict['TXENRAMPCLK'] = self.TXENRAMPCLK
        self.TXENVTRCURRENT = RM_Field_RAC_TX_TXENVTRCURRENT(self)
        self.zz_fdict['TXENVTRCURRENT'] = self.TXENVTRCURRENT
        self.TXENXDRVVMID = RM_Field_RAC_TX_TXENXDRVVMID(self)
        self.zz_fdict['TXENXDRVVMID'] = self.TXENXDRVVMID
        self.TXMODESE = RM_Field_RAC_TX_TXMODESE(self)
        self.zz_fdict['TXMODESE'] = self.TXMODESE
        self.TXENRFPKD0 = RM_Field_RAC_TX_TXENRFPKD0(self)
        self.zz_fdict['TXENRFPKD0'] = self.TXENRFPKD0
        self.TXENRFPKD1 = RM_Field_RAC_TX_TXENRFPKD1(self)
        self.zz_fdict['TXENRFPKD1'] = self.TXENRFPKD1
        self.TXENRREGBLEED = RM_Field_RAC_TX_TXENRREGBLEED(self)
        self.zz_fdict['TXENRREGBLEED'] = self.TXENRREGBLEED
        self.SYCHPBIASTRIMBUFTX = RM_Field_RAC_TX_SYCHPBIASTRIMBUFTX(self)
        self.zz_fdict['SYCHPBIASTRIMBUFTX'] = self.SYCHPBIASTRIMBUFTX
        self.SYPFDCHPLPENTX = RM_Field_RAC_TX_SYPFDCHPLPENTX(self)
        self.zz_fdict['SYPFDCHPLPENTX'] = self.SYPFDCHPLPENTX
        self.SYPFDFPWENTX = RM_Field_RAC_TX_SYPFDFPWENTX(self)
        self.zz_fdict['SYPFDFPWENTX'] = self.SYPFDFPWENTX
        self.SYVCOVCAPFASTRAMPTX = RM_Field_RAC_TX_SYVCOVCAPFASTRAMPTX(self)
        self.zz_fdict['SYVCOVCAPFASTRAMPTX'] = self.SYVCOVCAPFASTRAMPTX
        self.TXMODEPMOSOFF = RM_Field_RAC_TX_TXMODEPMOSOFF(self)
        self.zz_fdict['TXMODEPMOSOFF'] = self.TXMODEPMOSOFF
        self.TXSWPENDACI = RM_Field_RAC_TX_TXSWPENDACI(self)
        self.zz_fdict['TXSWPENDACI'] = self.TXSWPENDACI
        self.TXSWPENDACQ = RM_Field_RAC_TX_TXSWPENDACQ(self)
        self.zz_fdict['TXSWPENDACQ'] = self.TXSWPENDACQ
        self.TXENITCDAC = RM_Field_RAC_TX_TXENITCDAC(self)
        self.zz_fdict['TXENITCDAC'] = self.TXENITCDAC
        self.TXENITCPA = RM_Field_RAC_TX_TXENITCPA(self)
        self.zz_fdict['TXENITCPA'] = self.TXENITCPA
        self.TXENITCCORE = RM_Field_RAC_TX_TXENITCCORE(self)
        self.zz_fdict['TXENITCCORE'] = self.TXENITCCORE
        self.ENPAPOWER = RM_Field_RAC_TX_ENPAPOWER(self)
        self.zz_fdict['ENPAPOWER'] = self.ENPAPOWER
        self.ENPASELSLICE = RM_Field_RAC_TX_ENPASELSLICE(self)
        self.zz_fdict['ENPASELSLICE'] = self.ENPASELSLICE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYDEBUG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYDEBUG, self).__init__(rmio, label,
            0xa8020000, 0x170,
            'SYDEBUG', 'RAC.SYDEBUG', 'read-write',
            u"",
            0x00000020, 0x00000177,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYCHPSWITCHEDBIASEN = RM_Field_RAC_SYDEBUG_SYCHPSWITCHEDBIASEN(self)
        self.zz_fdict['SYCHPSWITCHEDBIASEN'] = self.SYCHPSWITCHEDBIASEN
        self.SYENMMDDREGBIASSTB = RM_Field_RAC_SYDEBUG_SYENMMDDREGBIASSTB(self)
        self.zz_fdict['SYENMMDDREGBIASSTB'] = self.SYENMMDDREGBIASSTB
        self.SYLODIVAUXLODIS = RM_Field_RAC_SYDEBUG_SYLODIVAUXLODIS(self)
        self.zz_fdict['SYLODIVAUXLODIS'] = self.SYLODIVAUXLODIS
        self.SYLODIVLDOPSRTRIM = RM_Field_RAC_SYDEBUG_SYLODIVLDOPSRTRIM(self)
        self.zz_fdict['SYLODIVLDOPSRTRIM'] = self.SYLODIVLDOPSRTRIM
        self.SYPFDLOCKWINDOWN = RM_Field_RAC_SYDEBUG_SYPFDLOCKWINDOWN(self)
        self.zz_fdict['SYPFDLOCKWINDOWN'] = self.SYPFDLOCKWINDOWN
        self.SYPFDTRADMODEEN = RM_Field_RAC_SYDEBUG_SYPFDTRADMODEEN(self)
        self.zz_fdict['SYPFDTRADMODEEN'] = self.SYPFDTRADMODEEN
        self.SYVCOREGBYPASS = RM_Field_RAC_SYDEBUG_SYVCOREGBYPASS(self)
        self.zz_fdict['SYVCOREGBYPASS'] = self.SYVCOREGBYPASS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYTRIM0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYTRIM0, self).__init__(rmio, label,
            0xa8020000, 0x174,
            'SYTRIM0', 'RAC.SYTRIM0', 'read-write',
            u"",
            0x018FF169, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYCHPBIAS = RM_Field_RAC_SYTRIM0_SYCHPBIAS(self)
        self.zz_fdict['SYCHPBIAS'] = self.SYCHPBIAS
        self.SYCHPCURRRX = RM_Field_RAC_SYTRIM0_SYCHPCURRRX(self)
        self.zz_fdict['SYCHPCURRRX'] = self.SYCHPCURRRX
        self.SYCHPCURRTX = RM_Field_RAC_SYTRIM0_SYCHPCURRTX(self)
        self.zz_fdict['SYCHPCURRTX'] = self.SYCHPCURRTX
        self.SYCHPLEVNSRC = RM_Field_RAC_SYTRIM0_SYCHPLEVNSRC(self)
        self.zz_fdict['SYCHPLEVNSRC'] = self.SYCHPLEVNSRC
        self.SYCHPLEVPSRCRX = RM_Field_RAC_SYTRIM0_SYCHPLEVPSRCRX(self)
        self.zz_fdict['SYCHPLEVPSRCRX'] = self.SYCHPLEVPSRCRX
        self.SYCHPLEVPSRCTX = RM_Field_RAC_SYTRIM0_SYCHPLEVPSRCTX(self)
        self.zz_fdict['SYCHPLEVPSRCTX'] = self.SYCHPLEVPSRCTX
        self.SYCHPSRCENRX = RM_Field_RAC_SYTRIM0_SYCHPSRCENRX(self)
        self.zz_fdict['SYCHPSRCENRX'] = self.SYCHPSRCENRX
        self.SYCHPSRCENTX = RM_Field_RAC_SYTRIM0_SYCHPSRCENTX(self)
        self.zz_fdict['SYCHPSRCENTX'] = self.SYCHPSRCENTX
        self.SYCHPREPLICACURRADJ = RM_Field_RAC_SYTRIM0_SYCHPREPLICACURRADJ(self)
        self.zz_fdict['SYCHPREPLICACURRADJ'] = self.SYCHPREPLICACURRADJ
        self.SYTRIMCHPREGAMPBIAS = RM_Field_RAC_SYTRIM0_SYTRIMCHPREGAMPBIAS(self)
        self.zz_fdict['SYTRIMCHPREGAMPBIAS'] = self.SYTRIMCHPREGAMPBIAS
        self.SYTRIMCHPREGAMPBW = RM_Field_RAC_SYTRIM0_SYTRIMCHPREGAMPBW(self)
        self.zz_fdict['SYTRIMCHPREGAMPBW'] = self.SYTRIMCHPREGAMPBW
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYTRIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYTRIM1, self).__init__(rmio, label,
            0xa8020000, 0x178,
            'SYTRIM1', 'RAC.SYTRIM1', 'read-write',
            u"",
            0x1FE00440, 0xFFFFCFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYLODIVLDOTRIMCORERX = RM_Field_RAC_SYTRIM1_SYLODIVLDOTRIMCORERX(self)
        self.zz_fdict['SYLODIVLDOTRIMCORERX'] = self.SYLODIVLDOTRIMCORERX
        self.SYLODIVLDOTRIMCORETX = RM_Field_RAC_SYTRIM1_SYLODIVLDOTRIMCORETX(self)
        self.zz_fdict['SYLODIVLDOTRIMCORETX'] = self.SYLODIVLDOTRIMCORETX
        self.SYLODIVLDOTRIMNDIORX = RM_Field_RAC_SYTRIM1_SYLODIVLDOTRIMNDIORX(self)
        self.zz_fdict['SYLODIVLDOTRIMNDIORX'] = self.SYLODIVLDOTRIMNDIORX
        self.SYLODIVLDOTRIMNDIOTX = RM_Field_RAC_SYTRIM1_SYLODIVLDOTRIMNDIOTX(self)
        self.zz_fdict['SYLODIVLDOTRIMNDIOTX'] = self.SYLODIVLDOTRIMNDIOTX
        self.SYLODIVSELOUTINVADC = RM_Field_RAC_SYTRIM1_SYLODIVSELOUTINVADC(self)
        self.zz_fdict['SYLODIVSELOUTINVADC'] = self.SYLODIVSELOUTINVADC
        self.SYLODIVTLOTRIMDELAYOFDM = RM_Field_RAC_SYTRIM1_SYLODIVTLOTRIMDELAYOFDM(self)
        self.zz_fdict['SYLODIVTLOTRIMDELAYOFDM'] = self.SYLODIVTLOTRIMDELAYOFDM
        self.SYLODIVTLOTRIMDELAY = RM_Field_RAC_SYTRIM1_SYLODIVTLOTRIMDELAY(self)
        self.zz_fdict['SYLODIVTLOTRIMDELAY'] = self.SYLODIVTLOTRIMDELAY
        self.SYMMDREPLICA1CURRADJ = RM_Field_RAC_SYTRIM1_SYMMDREPLICA1CURRADJ(self)
        self.zz_fdict['SYMMDREPLICA1CURRADJ'] = self.SYMMDREPLICA1CURRADJ
        self.SYMMDREPLICA2CURRADJ = RM_Field_RAC_SYTRIM1_SYMMDREPLICA2CURRADJ(self)
        self.zz_fdict['SYMMDREPLICA2CURRADJ'] = self.SYMMDREPLICA2CURRADJ
        self.SYTRIMMMDREGAMPBIAS = RM_Field_RAC_SYTRIM1_SYTRIMMMDREGAMPBIAS(self)
        self.zz_fdict['SYTRIMMMDREGAMPBIAS'] = self.SYTRIMMMDREGAMPBIAS
        self.SYTRIMMMDREGAMPBW = RM_Field_RAC_SYTRIM1_SYTRIMMMDREGAMPBW(self)
        self.zz_fdict['SYTRIMMMDREGAMPBW'] = self.SYTRIMMMDREGAMPBW
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYCAL, self).__init__(rmio, label,
            0xa8020000, 0x17C,
            'SYCAL', 'RAC.SYCAL', 'read-write',
            u"",
            0x01008100, 0x03018700,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYVCOMODEPKD = RM_Field_RAC_SYCAL_SYVCOMODEPKD(self)
        self.zz_fdict['SYVCOMODEPKD'] = self.SYVCOMODEPKD
        self.SYVCOMORECURRENT = RM_Field_RAC_SYCAL_SYVCOMORECURRENT(self)
        self.zz_fdict['SYVCOMORECURRENT'] = self.SYVCOMORECURRENT
        self.SYVCOSLOWNOISEFILTER = RM_Field_RAC_SYCAL_SYVCOSLOWNOISEFILTER(self)
        self.zz_fdict['SYVCOSLOWNOISEFILTER'] = self.SYVCOSLOWNOISEFILTER
        self.SYVCOVCAPVCM = RM_Field_RAC_SYCAL_SYVCOVCAPVCM(self)
        self.zz_fdict['SYVCOVCAPVCM'] = self.SYVCOVCAPVCM
        self.SYHILOADCHPREG = RM_Field_RAC_SYCAL_SYHILOADCHPREG(self)
        self.zz_fdict['SYHILOADCHPREG'] = self.SYHILOADCHPREG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYEN, self).__init__(rmio, label,
            0xa8020000, 0x180,
            'SYEN', 'RAC.SYEN', 'read-write',
            u"",
            0x00000000, 0x000067FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYCHPEN = RM_Field_RAC_SYEN_SYCHPEN(self)
        self.zz_fdict['SYCHPEN'] = self.SYCHPEN
        self.SYCHPLPENRX = RM_Field_RAC_SYEN_SYCHPLPENRX(self)
        self.zz_fdict['SYCHPLPENRX'] = self.SYCHPLPENRX
        self.SYCHPLPENTX = RM_Field_RAC_SYEN_SYCHPLPENTX(self)
        self.zz_fdict['SYCHPLPENTX'] = self.SYCHPLPENTX
        self.SYENCHPREG = RM_Field_RAC_SYEN_SYENCHPREG(self)
        self.zz_fdict['SYENCHPREG'] = self.SYENCHPREG
        self.SYENCHPREPLICA = RM_Field_RAC_SYEN_SYENCHPREPLICA(self)
        self.zz_fdict['SYENCHPREPLICA'] = self.SYENCHPREPLICA
        self.SYENMMDREG = RM_Field_RAC_SYEN_SYENMMDREG(self)
        self.zz_fdict['SYENMMDREG'] = self.SYENMMDREG
        self.SYENMMDREPLICA1 = RM_Field_RAC_SYEN_SYENMMDREPLICA1(self)
        self.zz_fdict['SYENMMDREPLICA1'] = self.SYENMMDREPLICA1
        self.SYENMMDREPLICA2 = RM_Field_RAC_SYEN_SYENMMDREPLICA2(self)
        self.zz_fdict['SYENMMDREPLICA2'] = self.SYENMMDREPLICA2
        self.SYENVCOBIAS = RM_Field_RAC_SYEN_SYENVCOBIAS(self)
        self.zz_fdict['SYENVCOBIAS'] = self.SYENVCOBIAS
        self.SYENVCOPFET = RM_Field_RAC_SYEN_SYENVCOPFET(self)
        self.zz_fdict['SYENVCOPFET'] = self.SYENVCOPFET
        self.SYENVCOREG = RM_Field_RAC_SYEN_SYENVCOREG(self)
        self.zz_fdict['SYENVCOREG'] = self.SYENVCOREG
        self.SYSTARTCHPREG = RM_Field_RAC_SYEN_SYSTARTCHPREG(self)
        self.zz_fdict['SYSTARTCHPREG'] = self.SYSTARTCHPREG
        self.SYSTARTMMDREG = RM_Field_RAC_SYEN_SYSTARTMMDREG(self)
        self.zz_fdict['SYSTARTMMDREG'] = self.SYSTARTMMDREG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYLOEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYLOEN, self).__init__(rmio, label,
            0xa8020000, 0x184,
            'SYLOEN', 'RAC.SYLOEN', 'read-write',
            u"",
            0x00000000, 0x00000B9E,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYLODIVEN = RM_Field_RAC_SYLOEN_SYLODIVEN(self)
        self.zz_fdict['SYLODIVEN'] = self.SYLODIVEN
        self.SYLODIVLDOBIASEN = RM_Field_RAC_SYLOEN_SYLODIVLDOBIASEN(self)
        self.zz_fdict['SYLODIVLDOBIASEN'] = self.SYLODIVLDOBIASEN
        self.SYLODIVLDOEN = RM_Field_RAC_SYLOEN_SYLODIVLDOEN(self)
        self.zz_fdict['SYLODIVLDOEN'] = self.SYLODIVLDOEN
        self.SYLODIVRLOSUBGRADCCLKEN = RM_Field_RAC_SYLOEN_SYLODIVRLOSUBGRADCCLKEN(self)
        self.zz_fdict['SYLODIVRLOSUBGRADCCLKEN'] = self.SYLODIVRLOSUBGRADCCLKEN
        self.SYLODIVSMUXDIVEN = RM_Field_RAC_SYLOEN_SYLODIVSMUXDIVEN(self)
        self.zz_fdict['SYLODIVSMUXDIVEN'] = self.SYLODIVSMUXDIVEN
        self.SYLODIVTLOSUBGAUXEN = RM_Field_RAC_SYLOEN_SYLODIVTLOSUBGAUXEN(self)
        self.zz_fdict['SYLODIVTLOSUBGAUXEN'] = self.SYLODIVTLOSUBGAUXEN
        self.SYLODIVTLOSUBGEN = RM_Field_RAC_SYLOEN_SYLODIVTLOSUBGEN(self)
        self.zz_fdict['SYLODIVTLOSUBGEN'] = self.SYLODIVTLOSUBGEN
        self.SYLODIVTLOOFDMSUBGEN = RM_Field_RAC_SYLOEN_SYLODIVTLOOFDMSUBGEN(self)
        self.zz_fdict['SYLODIVTLOOFDMSUBGEN'] = self.SYLODIVTLOOFDMSUBGEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYMMDCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYMMDCTRL, self).__init__(rmio, label,
            0xa8020000, 0x188,
            'SYMMDCTRL', 'RAC.SYMMDCTRL', 'read-write',
            u"",
            0x00000048, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYMMDDIVRSDIG = RM_Field_RAC_SYMMDCTRL_SYMMDDIVRSDIG(self)
        self.zz_fdict['SYMMDDIVRSDIG'] = self.SYMMDDIVRSDIG
        self.SYMMDMODERX = RM_Field_RAC_SYMMDCTRL_SYMMDMODERX(self)
        self.zz_fdict['SYMMDMODERX'] = self.SYMMDMODERX
        self.SYMMDMODETX = RM_Field_RAC_SYMMDCTRL_SYMMDMODETX(self)
        self.zz_fdict['SYMMDMODETX'] = self.SYMMDMODETX
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYDIVCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYDIVCTRL, self).__init__(rmio, label,
            0xa8020000, 0x18C,
            'SYDIVCTRL', 'RAC.SYDIVCTRL', 'read-write',
            u"",
            0x00000000, 0x00033C00,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYLODIVRADCDIV = RM_Field_RAC_SYDIVCTRL_SYLODIVRADCDIV(self)
        self.zz_fdict['SYLODIVRADCDIV'] = self.SYLODIVRADCDIV
        self.SYLODIVSMUXDIV = RM_Field_RAC_SYDIVCTRL_SYLODIVSMUXDIV(self)
        self.zz_fdict['SYLODIVSMUXDIV'] = self.SYLODIVSMUXDIV
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_DIGCLKRETIMECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_DIGCLKRETIMECTRL, self).__init__(rmio, label,
            0xa8020000, 0x190,
            'DIGCLKRETIMECTRL', 'RAC.DIGCLKRETIMECTRL', 'read-write',
            u"",
            0x00000000, 0x00000777,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIGCLKRETIMEENRETIME = RM_Field_RAC_DIGCLKRETIMECTRL_DIGCLKRETIMEENRETIME(self)
        self.zz_fdict['DIGCLKRETIMEENRETIME'] = self.DIGCLKRETIMEENRETIME
        self.DIGCLKRETIMEDISRETIME = RM_Field_RAC_DIGCLKRETIMECTRL_DIGCLKRETIMEDISRETIME(self)
        self.zz_fdict['DIGCLKRETIMEDISRETIME'] = self.DIGCLKRETIMEDISRETIME
        self.DIGCLKRETIMERESETN = RM_Field_RAC_DIGCLKRETIMECTRL_DIGCLKRETIMERESETN(self)
        self.zz_fdict['DIGCLKRETIMERESETN'] = self.DIGCLKRETIMERESETN
        self.DIGCLKRETIMELIMITH = RM_Field_RAC_DIGCLKRETIMECTRL_DIGCLKRETIMELIMITH(self)
        self.zz_fdict['DIGCLKRETIMELIMITH'] = self.DIGCLKRETIMELIMITH
        self.DIGCLKRETIMELIMITL = RM_Field_RAC_DIGCLKRETIMECTRL_DIGCLKRETIMELIMITL(self)
        self.zz_fdict['DIGCLKRETIMELIMITL'] = self.DIGCLKRETIMELIMITL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_DIGCLKRETIMESTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_DIGCLKRETIMESTATUS, self).__init__(rmio, label,
            0xa8020000, 0x194,
            'DIGCLKRETIMESTATUS', 'RAC.DIGCLKRETIMESTATUS', 'read-only',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.DIGCLKRETIMECLKSEL = RM_Field_RAC_DIGCLKRETIMESTATUS_DIGCLKRETIMECLKSEL(self)
        self.zz_fdict['DIGCLKRETIMECLKSEL'] = self.DIGCLKRETIMECLKSEL
        self.DIGCLKRETIMERESETNLO = RM_Field_RAC_DIGCLKRETIMESTATUS_DIGCLKRETIMERESETNLO(self)
        self.zz_fdict['DIGCLKRETIMERESETNLO'] = self.DIGCLKRETIMERESETNLO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_XORETIMECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_XORETIMECTRL, self).__init__(rmio, label,
            0xa8020000, 0x198,
            'XORETIMECTRL', 'RAC.XORETIMECTRL', 'read-write',
            u"",
            0x00000000, 0x00000777,
            0x00001000, 0x00002000,
            0x00003000)

        self.XORETIMEENRETIME = RM_Field_RAC_XORETIMECTRL_XORETIMEENRETIME(self)
        self.zz_fdict['XORETIMEENRETIME'] = self.XORETIMEENRETIME
        self.XORETIMEDISRETIME = RM_Field_RAC_XORETIMECTRL_XORETIMEDISRETIME(self)
        self.zz_fdict['XORETIMEDISRETIME'] = self.XORETIMEDISRETIME
        self.XORETIMERESETN = RM_Field_RAC_XORETIMECTRL_XORETIMERESETN(self)
        self.zz_fdict['XORETIMERESETN'] = self.XORETIMERESETN
        self.XORETIMELIMITH = RM_Field_RAC_XORETIMECTRL_XORETIMELIMITH(self)
        self.zz_fdict['XORETIMELIMITH'] = self.XORETIMELIMITH
        self.XORETIMELIMITL = RM_Field_RAC_XORETIMECTRL_XORETIMELIMITL(self)
        self.zz_fdict['XORETIMELIMITL'] = self.XORETIMELIMITL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_XORETIMESTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_XORETIMESTATUS, self).__init__(rmio, label,
            0xa8020000, 0x19C,
            'XORETIMESTATUS', 'RAC.XORETIMESTATUS', 'read-only',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.XORETIMECLKSEL = RM_Field_RAC_XORETIMESTATUS_XORETIMECLKSEL(self)
        self.zz_fdict['XORETIMECLKSEL'] = self.XORETIMECLKSEL
        self.XORETIMERESETNLO = RM_Field_RAC_XORETIMESTATUS_XORETIMERESETNLO(self)
        self.zz_fdict['XORETIMERESETNLO'] = self.XORETIMERESETNLO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AGCOVERWRITE0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AGCOVERWRITE0, self).__init__(rmio, label,
            0xa8020000, 0x1A0,
            'AGCOVERWRITE0', 'RAC.AGCOVERWRITE0', 'read-write',
            u"",
            0x00000000, 0x03F0FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ENMANLNAMIXRFATT = RM_Field_RAC_AGCOVERWRITE0_ENMANLNAMIXRFATT(self)
        self.zz_fdict['ENMANLNAMIXRFATT'] = self.ENMANLNAMIXRFATT
        self.ENMANLNAMIXSLICE = RM_Field_RAC_AGCOVERWRITE0_ENMANLNAMIXSLICE(self)
        self.zz_fdict['ENMANLNAMIXSLICE'] = self.ENMANLNAMIXSLICE
        self.ENMANPGAGAIN = RM_Field_RAC_AGCOVERWRITE0_ENMANPGAGAIN(self)
        self.zz_fdict['ENMANPGAGAIN'] = self.ENMANPGAGAIN
        self.ENMANIFADCSCALE = RM_Field_RAC_AGCOVERWRITE0_ENMANIFADCSCALE(self)
        self.zz_fdict['ENMANIFADCSCALE'] = self.ENMANIFADCSCALE
        self.MANLNAMIXSLICE0 = RM_Field_RAC_AGCOVERWRITE0_MANLNAMIXSLICE0(self)
        self.zz_fdict['MANLNAMIXSLICE0'] = self.MANLNAMIXSLICE0
        self.MANLNAMIXSLICE1 = RM_Field_RAC_AGCOVERWRITE0_MANLNAMIXSLICE1(self)
        self.zz_fdict['MANLNAMIXSLICE1'] = self.MANLNAMIXSLICE1
        self.MANPGAGAIN = RM_Field_RAC_AGCOVERWRITE0_MANPGAGAIN(self)
        self.zz_fdict['MANPGAGAIN'] = self.MANPGAGAIN
        self.MANIFADCSCALE = RM_Field_RAC_AGCOVERWRITE0_MANIFADCSCALE(self)
        self.zz_fdict['MANIFADCSCALE'] = self.MANIFADCSCALE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AGCOVERWRITE1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AGCOVERWRITE1, self).__init__(rmio, label,
            0xa8020000, 0x1A4,
            'AGCOVERWRITE1', 'RAC.AGCOVERWRITE1', 'read-write',
            u"",
            0x00000000, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MANLNAMIXRFATT0 = RM_Field_RAC_AGCOVERWRITE1_MANLNAMIXRFATT0(self)
        self.zz_fdict['MANLNAMIXRFATT0'] = self.MANLNAMIXRFATT0
        self.MANLNAMIXRFATT1 = RM_Field_RAC_AGCOVERWRITE1_MANLNAMIXRFATT1(self)
        self.zz_fdict['MANLNAMIXRFATT1'] = self.MANLNAMIXRFATT1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXOGNDPKD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXOGNDPKD, self).__init__(rmio, label,
            0xa8020000, 0x1BC,
            'TXOGNDPKD', 'RAC.TXOGNDPKD', 'read-write',
            u"",
            0x00000000, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXPKDOGNDTHRESH = RM_Field_RAC_TXOGNDPKD_TXPKDOGNDTHRESH(self)
        self.zz_fdict['TXPKDOGNDTHRESH'] = self.TXPKDOGNDTHRESH
        self.TXPKDOGNDBYPASSLATCH = RM_Field_RAC_TXOGNDPKD_TXPKDOGNDBYPASSLATCH(self)
        self.zz_fdict['TXPKDOGNDBYPASSLATCH'] = self.TXPKDOGNDBYPASSLATCH
        self.TXPKDOGNDTYPE = RM_Field_RAC_TXOGNDPKD_TXPKDOGNDTYPE(self)
        self.zz_fdict['TXPKDOGNDTYPE'] = self.TXPKDOGNDTYPE
        self.TXPKDOGNDRESET = RM_Field_RAC_TXOGNDPKD_TXPKDOGNDRESET(self)
        self.zz_fdict['TXPKDOGNDRESET'] = self.TXPKDOGNDRESET
        self.TXPKDOGNDATTEN = RM_Field_RAC_TXOGNDPKD_TXPKDOGNDATTEN(self)
        self.zz_fdict['TXPKDOGNDATTEN'] = self.TXPKDOGNDATTEN
        self.TXPKDOGNDCLKHIGH = RM_Field_RAC_TXOGNDPKD_TXPKDOGNDCLKHIGH(self)
        self.zz_fdict['TXPKDOGNDCLKHIGH'] = self.TXPKDOGNDCLKHIGH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXOGNDPKDSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXOGNDPKDSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x1C0,
            'TXOGNDPKDSTATUS', 'RAC.TXOGNDPKDSTATUS', 'read-only',
            u"",
            0x00000001, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXOGNDPKDN = RM_Field_RAC_TXOGNDPKDSTATUS_TXOGNDPKDN(self)
        self.zz_fdict['TXOGNDPKDN'] = self.TXOGNDPKDN
        self.TXOGNDPKDP = RM_Field_RAC_TXOGNDPKDSTATUS_TXOGNDPKDP(self)
        self.zz_fdict['TXOGNDPKDP'] = self.TXOGNDPKDP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SPARE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SPARE, self).__init__(rmio, label,
            0xa8020000, 0x1C4,
            'SPARE', 'RAC.SPARE', 'read-write',
            u"",
            0x00000000, 0xFC17FE1F,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCSPARE = RM_Field_RAC_SPARE_AUXADCSPARE(self)
        self.zz_fdict['AUXADCSPARE'] = self.AUXADCSPARE
        self.CLKMULTSPARE = RM_Field_RAC_SPARE_CLKMULTSPARE(self)
        self.zz_fdict['CLKMULTSPARE'] = self.CLKMULTSPARE
        self.SYSPARE = RM_Field_RAC_SPARE_SYSPARE(self)
        self.zz_fdict['SYSPARE'] = self.SYSPARE
        self.TXSPARE = RM_Field_RAC_SPARE_TXSPARE(self)
        self.zz_fdict['TXSPARE'] = self.TXSPARE
        self.SPARE0 = RM_Field_RAC_SPARE_SPARE0(self)
        self.zz_fdict['SPARE0'] = self.SPARE0
        self.IFADCCTRLRESERVED = RM_Field_RAC_SPARE_IFADCCTRLRESERVED(self)
        self.zz_fdict['IFADCCTRLRESERVED'] = self.IFADCCTRLRESERVED
        self.IFADCCTRLRESERVEDLV = RM_Field_RAC_SPARE_IFADCCTRLRESERVEDLV(self)
        self.zz_fdict['IFADCCTRLRESERVEDLV'] = self.IFADCCTRLRESERVEDLV
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_GPPLLCLKRETIMECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_GPPLLCLKRETIMECTRL, self).__init__(rmio, label,
            0xa8020000, 0x1D4,
            'GPPLLCLKRETIMECTRL', 'RAC.GPPLLCLKRETIMECTRL', 'read-write',
            u"",
            0x00000000, 0x00000777,
            0x00001000, 0x00002000,
            0x00003000)

        self.GPPLLCLKRETIMEENRETIME = RM_Field_RAC_GPPLLCLKRETIMECTRL_GPPLLCLKRETIMEENRETIME(self)
        self.zz_fdict['GPPLLCLKRETIMEENRETIME'] = self.GPPLLCLKRETIMEENRETIME
        self.GPPLLCLKRETIMEDISRETIME = RM_Field_RAC_GPPLLCLKRETIMECTRL_GPPLLCLKRETIMEDISRETIME(self)
        self.zz_fdict['GPPLLCLKRETIMEDISRETIME'] = self.GPPLLCLKRETIMEDISRETIME
        self.GPPLLCLKRETIMERESETN = RM_Field_RAC_GPPLLCLKRETIMECTRL_GPPLLCLKRETIMERESETN(self)
        self.zz_fdict['GPPLLCLKRETIMERESETN'] = self.GPPLLCLKRETIMERESETN
        self.GPPLLCLKRETIMELIMITH = RM_Field_RAC_GPPLLCLKRETIMECTRL_GPPLLCLKRETIMELIMITH(self)
        self.zz_fdict['GPPLLCLKRETIMELIMITH'] = self.GPPLLCLKRETIMELIMITH
        self.GPPLLCLKRETIMELIMITL = RM_Field_RAC_GPPLLCLKRETIMECTRL_GPPLLCLKRETIMELIMITL(self)
        self.zz_fdict['GPPLLCLKRETIMELIMITL'] = self.GPPLLCLKRETIMELIMITL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_GPPLLCLKRETIMESTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_GPPLLCLKRETIMESTATUS, self).__init__(rmio, label,
            0xa8020000, 0x1D8,
            'GPPLLCLKRETIMESTATUS', 'RAC.GPPLLCLKRETIMESTATUS', 'read-only',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.GPPLLCLKRETIMECLKSEL = RM_Field_RAC_GPPLLCLKRETIMESTATUS_GPPLLCLKRETIMECLKSEL(self)
        self.zz_fdict['GPPLLCLKRETIMECLKSEL'] = self.GPPLLCLKRETIMECLKSEL
        self.GPPLLCLKRETIMERESETNLO = RM_Field_RAC_GPPLLCLKRETIMESTATUS_GPPLLCLKRETIMERESETNLO(self)
        self.zz_fdict['GPPLLCLKRETIMERESETNLO'] = self.GPPLLCLKRETIMERESETNLO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM6, self).__init__(rmio, label,
            0xa8020000, 0x1E4,
            'PATRIM6', 'RAC.PATRIM6', 'read-write',
            u"",
            0x000451A0, 0xFF3FF3FE,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMBBDIGREGIOUT = RM_Field_RAC_PATRIM6_TXTRIMBBDIGREGIOUT(self)
        self.zz_fdict['TXTRIMBBDIGREGIOUT'] = self.TXTRIMBBDIGREGIOUT
        self.TXTRIMBBREGVREF = RM_Field_RAC_PATRIM6_TXTRIMBBREGVREF(self)
        self.zz_fdict['TXTRIMBBREGVREF'] = self.TXTRIMBBREGVREF
        self.TXTRIMBBREGFB = RM_Field_RAC_PATRIM6_TXTRIMBBREGFB(self)
        self.zz_fdict['TXTRIMBBREGFB'] = self.TXTRIMBBREGFB
        self.TXTRIMFILVCOM = RM_Field_RAC_PATRIM6_TXTRIMFILVCOM(self)
        self.zz_fdict['TXTRIMFILVCOM'] = self.TXTRIMFILVCOM
        self.TXTRIMFILCAP = RM_Field_RAC_PATRIM6_TXTRIMFILCAP(self)
        self.zz_fdict['TXTRIMFILCAP'] = self.TXTRIMFILCAP
        self.TXTRIMFILGAIN = RM_Field_RAC_PATRIM6_TXTRIMFILGAIN(self)
        self.zz_fdict['TXTRIMFILGAIN'] = self.TXTRIMFILGAIN
        self.TXTRIMFILRES = RM_Field_RAC_PATRIM6_TXTRIMFILRES(self)
        self.zz_fdict['TXTRIMFILRES'] = self.TXTRIMFILRES
        self.TXTRIMBIASNMAX = RM_Field_RAC_PATRIM6_TXTRIMBIASNMAX(self)
        self.zz_fdict['TXTRIMBIASNMAX'] = self.TXTRIMBIASNMAX
        self.TXTRIMBIASPMAX = RM_Field_RAC_PATRIM6_TXTRIMBIASPMAX(self)
        self.zz_fdict['TXTRIMBIASPMAX'] = self.TXTRIMBIASPMAX
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM7, self).__init__(rmio, label,
            0xa8020000, 0x1E8,
            'PATRIM7', 'RAC.PATRIM7', 'read-write',
            u"",
            0x029C8015, 0x03FFC73F,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMMIXGMBIAS = RM_Field_RAC_PATRIM7_TXTRIMMIXGMBIAS(self)
        self.zz_fdict['TXTRIMMIXGMBIAS'] = self.TXTRIMMIXGMBIAS
        self.TXTRIMMIXLOBIASI = RM_Field_RAC_PATRIM7_TXTRIMMIXLOBIASI(self)
        self.zz_fdict['TXTRIMMIXLOBIASI'] = self.TXTRIMMIXLOBIASI
        self.TXTRIMMIXLOBIASQ = RM_Field_RAC_PATRIM7_TXTRIMMIXLOBIASQ(self)
        self.zz_fdict['TXTRIMMIXLOBIASQ'] = self.TXTRIMMIXLOBIASQ
        self.TXTRIMMIXINDSW = RM_Field_RAC_PATRIM7_TXTRIMMIXINDSW(self)
        self.zz_fdict['TXTRIMMIXINDSW'] = self.TXTRIMMIXINDSW
        self.TXTRIMSHVDDREG = RM_Field_RAC_PATRIM7_TXTRIMSHVDDREG(self)
        self.zz_fdict['TXTRIMSHVDDREG'] = self.TXTRIMSHVDDREG
        self.TXTRIMSHIBINBUF = RM_Field_RAC_PATRIM7_TXTRIMSHIBINBUF(self)
        self.zz_fdict['TXTRIMSHIBINBUF'] = self.TXTRIMSHIBINBUF
        self.TXTRIMSHIBVCM = RM_Field_RAC_PATRIM7_TXTRIMSHIBVCM(self)
        self.zz_fdict['TXTRIMSHIBVCM'] = self.TXTRIMSHIBVCM
        self.TXTRIMSHVCM = RM_Field_RAC_PATRIM7_TXTRIMSHVCM(self)
        self.zz_fdict['TXTRIMSHVCM'] = self.TXTRIMSHVCM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM8(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM8, self).__init__(rmio, label,
            0xa8020000, 0x1EC,
            'PATRIM8', 'RAC.PATRIM8', 'read-write',
            u"",
            0x000501C6, 0x000FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMRFPKDVDDD = RM_Field_RAC_PATRIM8_TXTRIMRFPKDVDDD(self)
        self.zz_fdict['TXTRIMRFPKDVDDD'] = self.TXTRIMRFPKDVDDD
        self.TXTRIMRFPKDIB = RM_Field_RAC_PATRIM8_TXTRIMRFPKDIB(self)
        self.zz_fdict['TXTRIMRFPKDIB'] = self.TXTRIMRFPKDIB
        self.TXTRIMRFPKDVCM = RM_Field_RAC_PATRIM8_TXTRIMRFPKDVCM(self)
        self.zz_fdict['TXTRIMRFPKDVCM'] = self.TXTRIMRFPKDVCM
        self.TXTRIMBUFBIAS = RM_Field_RAC_PATRIM8_TXTRIMBUFBIAS(self)
        self.zz_fdict['TXTRIMBUFBIAS'] = self.TXTRIMBUFBIAS
        self.TXTRIMBUFSLICESN = RM_Field_RAC_PATRIM8_TXTRIMBUFSLICESN(self)
        self.zz_fdict['TXTRIMBUFSLICESN'] = self.TXTRIMBUFSLICESN
        self.TXTRIMBUFSLICESP = RM_Field_RAC_PATRIM8_TXTRIMBUFSLICESP(self)
        self.zz_fdict['TXTRIMBUFSLICESP'] = self.TXTRIMBUFSLICESP
        self.TXTRIMBUFVMID = RM_Field_RAC_PATRIM8_TXTRIMBUFVMID(self)
        self.zz_fdict['TXTRIMBUFVMID'] = self.TXTRIMBUFVMID
        self.TXTRIMBUFVREG = RM_Field_RAC_PATRIM8_TXTRIMBUFVREG(self)
        self.zz_fdict['TXTRIMBUFVREG'] = self.TXTRIMBUFVREG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM9(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM9, self).__init__(rmio, label,
            0xa8020000, 0x1F0,
            'PATRIM9', 'RAC.PATRIM9', 'read-write',
            u"",
            0x00000045, 0xFFFF007F,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMDACVCAS = RM_Field_RAC_PATRIM9_TXTRIMDACVCAS(self)
        self.zz_fdict['TXTRIMDACVCAS'] = self.TXTRIMDACVCAS
        self.TXTRIMDACSCALE = RM_Field_RAC_PATRIM9_TXTRIMDACSCALE(self)
        self.zz_fdict['TXTRIMDACSCALE'] = self.TXTRIMDACSCALE
        self.TXTRIMDACVDDD = RM_Field_RAC_PATRIM9_TXTRIMDACVDDD(self)
        self.zz_fdict['TXTRIMDACVDDD'] = self.TXTRIMDACVDDD
        self.TXTRIMDACIBIAS = RM_Field_RAC_PATRIM9_TXTRIMDACIBIAS(self)
        self.zz_fdict['TXTRIMDACIBIAS'] = self.TXTRIMDACIBIAS
        self.TXTRIMPASLICESN = RM_Field_RAC_PATRIM9_TXTRIMPASLICESN(self)
        self.zz_fdict['TXTRIMPASLICESN'] = self.TXTRIMPASLICESN
        self.TXTRIMPASLICESP = RM_Field_RAC_PATRIM9_TXTRIMPASLICESP(self)
        self.zz_fdict['TXTRIMPASLICESP'] = self.TXTRIMPASLICESP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM10(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM10, self).__init__(rmio, label,
            0xa8020000, 0x1F4,
            'PATRIM10', 'RAC.PATRIM10', 'read-write',
            u"",
            0x015E0579, 0x07FF1FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXTRIMPAMIRROR = RM_Field_RAC_PATRIM10_TXTRIMPAMIRROR(self)
        self.zz_fdict['TXTRIMPAMIRROR'] = self.TXTRIMPAMIRROR
        self.TXTRIMPANBIAS = RM_Field_RAC_PATRIM10_TXTRIMPANBIAS(self)
        self.zz_fdict['TXTRIMPANBIAS'] = self.TXTRIMPANBIAS
        self.TXTRIMPANCBIAS = RM_Field_RAC_PATRIM10_TXTRIMPANCBIAS(self)
        self.zz_fdict['TXTRIMPANCBIAS'] = self.TXTRIMPANCBIAS
        self.TXTRIMPAPBIAS = RM_Field_RAC_PATRIM10_TXTRIMPAPBIAS(self)
        self.zz_fdict['TXTRIMPAPBIAS'] = self.TXTRIMPAPBIAS
        self.TXTRIMPAPCBIAS = RM_Field_RAC_PATRIM10_TXTRIMPAPCBIAS(self)
        self.zz_fdict['TXTRIMPAPCBIAS'] = self.TXTRIMPAPCBIAS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXRFPKD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXRFPKD, self).__init__(rmio, label,
            0xa8020000, 0x1FC,
            'TXRFPKD', 'RAC.TXRFPKD', 'read-write',
            u"",
            0x00010011, 0x0071F71D,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXRESETRFPKD = RM_Field_RAC_TXRFPKD_TXRESETRFPKD(self)
        self.zz_fdict['TXRESETRFPKD'] = self.TXRESETRFPKD
        self.TXSELRFPKDREF = RM_Field_RAC_TXRFPKD_TXSELRFPKDREF(self)
        self.zz_fdict['TXSELRFPKDREF'] = self.TXSELRFPKDREF
        self.TXRFPKDOUTMODE = RM_Field_RAC_TXRFPKD_TXRFPKDOUTMODE(self)
        self.zz_fdict['TXRFPKDOUTMODE'] = self.TXRFPKDOUTMODE
        self.TXRFPKDCALMODE = RM_Field_RAC_TXRFPKD_TXRFPKDCALMODE(self)
        self.zz_fdict['TXRFPKDCALMODE'] = self.TXRFPKDCALMODE
        self.TXRFPKDCALCODE = RM_Field_RAC_TXRFPKD_TXRFPKDCALCODE(self)
        self.zz_fdict['TXRFPKDCALCODE'] = self.TXRFPKDCALCODE
        self.TXRFPKDCAPATT = RM_Field_RAC_TXRFPKD_TXRFPKDCAPATT(self)
        self.zz_fdict['TXRFPKDCAPATT'] = self.TXRFPKDCAPATT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x204,
            'TXSTATUS', 'RAC.TXSTATUS', 'read-only',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXRFPKDOUT = RM_Field_RAC_TXSTATUS_TXRFPKDOUT(self)
        self.zz_fdict['TXRFPKDOUT'] = self.TXRFPKDOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXOFDM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXOFDM, self).__init__(rmio, label,
            0xa8020000, 0x208,
            'TXOFDM', 'RAC.TXOFDM', 'read-write',
            u"",
            0x00200008, 0x03BFB7FB,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXENBBDIGREG = RM_Field_RAC_TXOFDM_TXENBBDIGREG(self)
        self.zz_fdict['TXENBBDIGREG'] = self.TXENBBDIGREG
        self.TXENBBREG = RM_Field_RAC_TXOFDM_TXENBBREG(self)
        self.zz_fdict['TXENBBREG'] = self.TXENBBREG
        self.TXENBBREGBLEED = RM_Field_RAC_TXOFDM_TXENBBREGBLEED(self)
        self.zz_fdict['TXENBBREGBLEED'] = self.TXENBBREGBLEED
        self.TXENBUF0 = RM_Field_RAC_TXOFDM_TXENBUF0(self)
        self.zz_fdict['TXENBUF0'] = self.TXENBUF0
        self.TXENBUF1 = RM_Field_RAC_TXOFDM_TXENBUF1(self)
        self.zz_fdict['TXENBUF1'] = self.TXENBUF1
        self.TXENBBDACI = RM_Field_RAC_TXOFDM_TXENBBDACI(self)
        self.zz_fdict['TXENBBDACI'] = self.TXENBBDACI
        self.TXENBBDACQ = RM_Field_RAC_TXOFDM_TXENBBDACQ(self)
        self.zz_fdict['TXENBBDACQ'] = self.TXENBBDACQ
        self.TXENMIX = RM_Field_RAC_TXOFDM_TXENMIX(self)
        self.zz_fdict['TXENMIX'] = self.TXENMIX
        self.TXENBUFREG = RM_Field_RAC_TXOFDM_TXENBUFREG(self)
        self.zz_fdict['TXENBUFREG'] = self.TXENBUFREG
        self.TXENBUFREGBLEED = RM_Field_RAC_TXOFDM_TXENBUFREGBLEED(self)
        self.zz_fdict['TXENBUFREGBLEED'] = self.TXENBUFREGBLEED
        self.TXENMIXRLOAD = RM_Field_RAC_TXOFDM_TXENMIXRLOAD(self)
        self.zz_fdict['TXENMIXRLOAD'] = self.TXENMIXRLOAD
        self.TXENPAFASTSTARTUP = RM_Field_RAC_TXOFDM_TXENPAFASTSTARTUP(self)
        self.zz_fdict['TXENPAFASTSTARTUP'] = self.TXENPAFASTSTARTUP
        self.TXENSH = RM_Field_RAC_TXOFDM_TXENSH(self)
        self.zz_fdict['TXENSH'] = self.TXENSH
        self.TXENSHINBUF = RM_Field_RAC_TXOFDM_TXENSHINBUF(self)
        self.zz_fdict['TXENSHINBUF'] = self.TXENSHINBUF
        self.TXENSHUGB = RM_Field_RAC_TXOFDM_TXENSHUGB(self)
        self.zz_fdict['TXENSHUGB'] = self.TXENSHUGB
        self.TXENFILI = RM_Field_RAC_TXOFDM_TXENFILI(self)
        self.zz_fdict['TXENFILI'] = self.TXENFILI
        self.TXENFILQ = RM_Field_RAC_TXOFDM_TXENFILQ(self)
        self.zz_fdict['TXENFILQ'] = self.TXENFILQ
        self.TXENBBDATA = RM_Field_RAC_TXOFDM_TXENBBDATA(self)
        self.zz_fdict['TXENBBDATA'] = self.TXENBBDATA
        self.TXENPACMOS = RM_Field_RAC_TXOFDM_TXENPACMOS(self)
        self.zz_fdict['TXENPACMOS'] = self.TXENPACMOS
        self.TXENTXBIAS = RM_Field_RAC_TXOFDM_TXENTXBIAS(self)
        self.zz_fdict['TXENTXBIAS'] = self.TXENTXBIAS
        self.TXRESETSHCLKDIV = RM_Field_RAC_TXOFDM_TXRESETSHCLKDIV(self)
        self.zz_fdict['TXRESETSHCLKDIV'] = self.TXRESETSHCLKDIV
        self.TXSELPABIASMODE = RM_Field_RAC_TXOFDM_TXSELPABIASMODE(self)
        self.zz_fdict['TXSELPABIASMODE'] = self.TXSELPABIASMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXMIX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXMIX, self).__init__(rmio, label,
            0xa8020000, 0x210,
            'TXMIX', 'RAC.TXMIX', 'read-write',
            u"",
            0x00001100, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXSELMIXCTUNE = RM_Field_RAC_TXMIX_TXSELMIXCTUNE(self)
        self.zz_fdict['TXSELMIXCTUNE'] = self.TXSELMIXCTUNE
        self.TXSELMIXGMSLICEI = RM_Field_RAC_TXMIX_TXSELMIXGMSLICEI(self)
        self.zz_fdict['TXSELMIXGMSLICEI'] = self.TXSELMIXGMSLICEI
        self.TXSELMIXGMSLICEQ = RM_Field_RAC_TXMIX_TXSELMIXGMSLICEQ(self)
        self.zz_fdict['TXSELMIXGMSLICEQ'] = self.TXSELMIXGMSLICEQ
        self.TXSELMIXRLOAD = RM_Field_RAC_TXMIX_TXSELMIXRLOAD(self)
        self.zz_fdict['TXSELMIXRLOAD'] = self.TXSELMIXRLOAD
        self.TXSELMIXBAND = RM_Field_RAC_TXMIX_TXSELMIXBAND(self)
        self.zz_fdict['TXSELMIXBAND'] = self.TXSELMIXBAND
        self.TXSELMIXRDEG = RM_Field_RAC_TXMIX_TXSELMIXRDEG(self)
        self.zz_fdict['TXSELMIXRDEG'] = self.TXSELMIXRDEG
        self.TXMIXCAPPULLDOWN = RM_Field_RAC_TXMIX_TXMIXCAPPULLDOWN(self)
        self.zz_fdict['TXMIXCAPPULLDOWN'] = self.TXMIXCAPPULLDOWN
        self.TXSELSHINPUT = RM_Field_RAC_TXMIX_TXSELSHINPUT(self)
        self.zz_fdict['TXSELSHINPUT'] = self.TXSELSHINPUT
        self.TXSELSHCAPATTN = RM_Field_RAC_TXMIX_TXSELSHCAPATTN(self)
        self.zz_fdict['TXSELSHCAPATTN'] = self.TXSELSHCAPATTN
        self.TXSELSHCLKDIV = RM_Field_RAC_TXMIX_TXSELSHCLKDIV(self)
        self.zz_fdict['TXSELSHCLKDIV'] = self.TXSELSHCLKDIV
        self.TXSELSHINBUFATTN = RM_Field_RAC_TXMIX_TXSELSHINBUFATTN(self)
        self.zz_fdict['TXSELSHINBUFATTN'] = self.TXSELSHINBUFATTN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXITC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXITC, self).__init__(rmio, label,
            0xa8020000, 0x214,
            'TXITC', 'RAC.TXITC', 'read-write',
            u"",
            0x00000040, 0x000000F3,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXSELITCDAC = RM_Field_RAC_TXITC_TXSELITCDAC(self)
        self.zz_fdict['TXSELITCDAC'] = self.TXSELITCDAC
        self.TXSELITCPA = RM_Field_RAC_TXITC_TXSELITCPA(self)
        self.zz_fdict['TXSELITCPA'] = self.TXSELITCPA
        self.TXSELITCDACLEVEL = RM_Field_RAC_TXITC_TXSELITCDACLEVEL(self)
        self.zz_fdict['TXSELITCDACLEVEL'] = self.TXSELITCDACLEVEL
        self.TXSELITCPALEVEL = RM_Field_RAC_TXITC_TXSELITCPALEVEL(self)
        self.zz_fdict['TXSELITCPALEVEL'] = self.TXSELITCPALEVEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXCALITC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXCALITC, self).__init__(rmio, label,
            0xa8020000, 0x218,
            'TXCALITC', 'RAC.TXCALITC', 'read-write',
            u"",
            0x03C20830, 0x07CF7DF3,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXCALENITCDAC = RM_Field_RAC_TXCALITC_TXCALENITCDAC(self)
        self.zz_fdict['TXCALENITCDAC'] = self.TXCALENITCDAC
        self.TXCALENITCPA = RM_Field_RAC_TXCALITC_TXCALENITCPA(self)
        self.zz_fdict['TXCALENITCPA'] = self.TXCALENITCPA
        self.TXCALITCDACIB = RM_Field_RAC_TXCALITC_TXCALITCDACIB(self)
        self.zz_fdict['TXCALITCDACIB'] = self.TXCALITCDACIB
        self.TXCALITCPAIB = RM_Field_RAC_TXCALITC_TXCALITCPAIB(self)
        self.zz_fdict['TXCALITCPAIB'] = self.TXCALITCPAIB
        self.TXCALITCCOREIB = RM_Field_RAC_TXCALITC_TXCALITCCOREIB(self)
        self.zz_fdict['TXCALITCCOREIB'] = self.TXCALITCCOREIB
        self.TXCALITCCORETC = RM_Field_RAC_TXCALITC_TXCALITCCORETC(self)
        self.zz_fdict['TXCALITCCORETC'] = self.TXCALITCCORETC
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SPACER(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SPACER, self).__init__(rmio, label,
            0xa8020000, 0x238,
            'SPACER', 'RAC.SPACER', 'read-write',
            u"",
            0x00000000, 0x00000023,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKMULTSPACER0 = RM_Field_RAC_SPACER_CLKMULTSPACER0(self)
        self.zz_fdict['CLKMULTSPACER0'] = self.CLKMULTSPACER0
        self.AUXADCSPACER1 = RM_Field_RAC_SPACER_AUXADCSPACER1(self)
        self.zz_fdict['AUXADCSPACER1'] = self.AUXADCSPACER1
        self.RXSPACER2 = RM_Field_RAC_SPACER_RXSPACER2(self)
        self.zz_fdict['RXSPACER2'] = self.RXSPACER2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXPKDCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXPKDCTRL, self).__init__(rmio, label,
            0xa8020000, 0x300,
            'TXPKDCTRL', 'RAC.TXPKDCTRL', 'read-write',
            u"",
            0x00000000, 0x0001FFF7,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXPKDCLKDIV = RM_Field_RAC_TXPKDCTRL_TXPKDCLKDIV(self)
        self.zz_fdict['TXPKDCLKDIV'] = self.TXPKDCLKDIV
        self.TXPKDCNTLEN = RM_Field_RAC_TXPKDCTRL_TXPKDCNTLEN(self)
        self.zz_fdict['TXPKDCNTLEN'] = self.TXPKDCNTLEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXPKDCMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXPKDCMD, self).__init__(rmio, label,
            0xa8020000, 0x304,
            'TXPKDCMD', 'RAC.TXPKDCMD', 'write-only',
            u"",
            0x00000000, 0x00000007,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXPKDSTART = RM_Field_RAC_TXPKDCMD_TXPKDSTART(self)
        self.zz_fdict['TXPKDSTART'] = self.TXPKDSTART
        self.TXPKDSTOP = RM_Field_RAC_TXPKDCMD_TXPKDSTOP(self)
        self.zz_fdict['TXPKDSTOP'] = self.TXPKDSTOP
        self.TXPKDCLR = RM_Field_RAC_TXPKDCMD_TXPKDCLR(self)
        self.zz_fdict['TXPKDCLR'] = self.TXPKDCLR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TXPKDSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TXPKDSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x308,
            'TXPKDSTATUS', 'RAC.TXPKDSTATUS', 'read-only',
            u"",
            0x00000000, 0x0001FFF1,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXPKDDONE = RM_Field_RAC_TXPKDSTATUS_TXPKDDONE(self)
        self.zz_fdict['TXPKDDONE'] = self.TXPKDDONE
        self.TXPKDCNT = RM_Field_RAC_TXPKDSTATUS_TXPKDCNT(self)
        self.zz_fdict['TXPKDCNT'] = self.TXPKDCNT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RADIOCLKCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RADIOCLKCTRL, self).__init__(rmio, label,
            0xa8020000, 0x3D0,
            'RADIOCLKCTRL', 'RAC.RADIOCLKCTRL', 'read-write',
            u"",
            0x00000101, 0x00000111,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKSRWUHSPRESC = RM_Field_RAC_RADIOCLKCTRL_CLKSRWUHSPRESC(self)
        self.zz_fdict['CLKSRWUHSPRESC'] = self.CLKSRWUHSPRESC
        self.CLKSRWHSPRESC = RM_Field_RAC_RADIOCLKCTRL_CLKSRWHSPRESC(self)
        self.zz_fdict['CLKSRWHSPRESC'] = self.CLKSRWHSPRESC
        self.CLKSRWLSPRESC = RM_Field_RAC_RADIOCLKCTRL_CLKSRWLSPRESC(self)
        self.zz_fdict['CLKSRWLSPRESC'] = self.CLKSRWLSPRESC
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH0, self).__init__(rmio, label,
            0xa8020000, 0x3E0,
            'SCRATCH0', 'RAC.SCRATCH0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH0 = RM_Field_RAC_SCRATCH0_SCRATCH0(self)
        self.zz_fdict['SCRATCH0'] = self.SCRATCH0
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH1, self).__init__(rmio, label,
            0xa8020000, 0x3E4,
            'SCRATCH1', 'RAC.SCRATCH1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH1 = RM_Field_RAC_SCRATCH1_SCRATCH1(self)
        self.zz_fdict['SCRATCH1'] = self.SCRATCH1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH2, self).__init__(rmio, label,
            0xa8020000, 0x3E8,
            'SCRATCH2', 'RAC.SCRATCH2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH2 = RM_Field_RAC_SCRATCH2_SCRATCH2(self)
        self.zz_fdict['SCRATCH2'] = self.SCRATCH2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH3, self).__init__(rmio, label,
            0xa8020000, 0x3EC,
            'SCRATCH3', 'RAC.SCRATCH3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH3 = RM_Field_RAC_SCRATCH3_SCRATCH3(self)
        self.zz_fdict['SCRATCH3'] = self.SCRATCH3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH4, self).__init__(rmio, label,
            0xa8020000, 0x3F0,
            'SCRATCH4', 'RAC.SCRATCH4', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH4 = RM_Field_RAC_SCRATCH4_SCRATCH4(self)
        self.zz_fdict['SCRATCH4'] = self.SCRATCH4
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH5, self).__init__(rmio, label,
            0xa8020000, 0x3F4,
            'SCRATCH5', 'RAC.SCRATCH5', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH5 = RM_Field_RAC_SCRATCH5_SCRATCH5(self)
        self.zz_fdict['SCRATCH5'] = self.SCRATCH5
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH6, self).__init__(rmio, label,
            0xa8020000, 0x3F8,
            'SCRATCH6', 'RAC.SCRATCH6', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH6 = RM_Field_RAC_SCRATCH6_SCRATCH6(self)
        self.zz_fdict['SCRATCH6'] = self.SCRATCH6
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SCRATCH7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SCRATCH7, self).__init__(rmio, label,
            0xa8020000, 0x3FC,
            'SCRATCH7', 'RAC.SCRATCH7', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SCRATCH7 = RM_Field_RAC_SCRATCH7_SCRATCH7(self)
        self.zz_fdict['SCRATCH7'] = self.SCRATCH7
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SOFTMCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SOFTMCTRL, self).__init__(rmio, label,
            0xa8020000, 0x400,
            'SOFTMCTRL', 'RAC.SOFTMCTRL', 'read-write',
            u"",
            0x00000000, 0x00000DFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TXPATHEN = RM_Field_RAC_SOFTMCTRL_TXPATHEN(self)
        self.zz_fdict['TXPATHEN'] = self.TXPATHEN
        self.RXPATHEN = RM_Field_RAC_SOFTMCTRL_RXPATHEN(self)
        self.zz_fdict['RXPATHEN'] = self.RXPATHEN
        self.FRCPATHEN = RM_Field_RAC_SOFTMCTRL_FRCPATHEN(self)
        self.zz_fdict['FRCPATHEN'] = self.FRCPATHEN
        self.CLKEN = RM_Field_RAC_SOFTMCTRL_CLKEN(self)
        self.zz_fdict['CLKEN'] = self.CLKEN
        self.CLKRUNNING = RM_Field_RAC_SOFTMCTRL_CLKRUNNING(self)
        self.zz_fdict['CLKRUNNING'] = self.CLKRUNNING
        self.TXMODSEL = RM_Field_RAC_SOFTMCTRL_TXMODSEL(self)
        self.zz_fdict['TXMODSEL'] = self.TXMODSEL
        self.ANTSELSRCSEL = RM_Field_RAC_SOFTMCTRL_ANTSELSRCSEL(self)
        self.zz_fdict['ANTSELSRCSEL'] = self.ANTSELSRCSEL
        self.FRCEN = RM_Field_RAC_SOFTMCTRL_FRCEN(self)
        self.zz_fdict['FRCEN'] = self.FRCEN
        self.DISLEGACYPATH = RM_Field_RAC_SOFTMCTRL_DISLEGACYPATH(self)
        self.zz_fdict['DISLEGACYPATH'] = self.DISLEGACYPATH
        self.TXDACDISABLECLK = RM_Field_RAC_SOFTMCTRL_TXDACDISABLECLK(self)
        self.zz_fdict['TXDACDISABLECLK'] = self.TXDACDISABLECLK
        self.TXDACINVERTCLK = RM_Field_RAC_SOFTMCTRL_TXDACINVERTCLK(self)
        self.zz_fdict['TXDACINVERTCLK'] = self.TXDACINVERTCLK
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SFMIF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SFMIF, self).__init__(rmio, label,
            0xa8020000, 0x404,
            'SFMIF', 'RAC.SFMIF', 'read-write',
            u"",
            0x00000000, 0x00FF000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGESFMIF = RM_Field_RAC_SFMIF_STATECHANGESFMIF(self)
        self.zz_fdict['STATECHANGESFMIF'] = self.STATECHANGESFMIF
        self.STIMCMPEVSFMIF = RM_Field_RAC_SFMIF_STIMCMPEVSFMIF(self)
        self.zz_fdict['STIMCMPEVSFMIF'] = self.STIMCMPEVSFMIF
        self.SEQLOCKUPSFMIF = RM_Field_RAC_SFMIF_SEQLOCKUPSFMIF(self)
        self.zz_fdict['SEQLOCKUPSFMIF'] = self.SEQLOCKUPSFMIF
        self.SEQRESETREQSFMIF = RM_Field_RAC_SFMIF_SEQRESETREQSFMIF(self)
        self.zz_fdict['SEQRESETREQSFMIF'] = self.SEQRESETREQSFMIF
        self.SEQSFMIF = RM_Field_RAC_SFMIF_SEQSFMIF(self)
        self.zz_fdict['SEQSFMIF'] = self.SEQSFMIF
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SFMIEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SFMIEN, self).__init__(rmio, label,
            0xa8020000, 0x408,
            'SFMIEN', 'RAC.SFMIEN', 'read-write',
            u"",
            0x00000000, 0x00FF000F,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGESFMIEN = RM_Field_RAC_SFMIEN_STATECHANGESFMIEN(self)
        self.zz_fdict['STATECHANGESFMIEN'] = self.STATECHANGESFMIEN
        self.STIMCMPEVSFMIEN = RM_Field_RAC_SFMIEN_STIMCMPEVSFMIEN(self)
        self.zz_fdict['STIMCMPEVSFMIEN'] = self.STIMCMPEVSFMIEN
        self.SEQLOCKUPSFMIEN = RM_Field_RAC_SFMIEN_SEQLOCKUPSFMIEN(self)
        self.zz_fdict['SEQLOCKUPSFMIEN'] = self.SEQLOCKUPSFMIEN
        self.SEQRESETREQSFMIEN = RM_Field_RAC_SFMIEN_SEQRESETREQSFMIEN(self)
        self.zz_fdict['SEQRESETREQSFMIEN'] = self.SEQRESETREQSFMIEN
        self.SEQSFMIEN = RM_Field_RAC_SFMIEN_SEQSFMIEN(self)
        self.zz_fdict['SEQSFMIEN'] = self.SEQSFMIEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_THMSW(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_THMSW, self).__init__(rmio, label,
            0xa8020000, 0x7E8,
            'THMSW', 'RAC.THMSW', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_RAC_THMSW_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.HALFSWITCH = RM_Field_RAC_THMSW_HALFSWITCH(self)
        self.zz_fdict['HALFSWITCH'] = self.HALFSWITCH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_DIAGAALTEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_DIAGAALTEN, self).__init__(rmio, label,
            0xa8020000, 0x7EC,
            'DIAGAALTEN', 'RAC.DIAGAALTEN', 'read-write',
            u"",
            0x00000000, 0x00000002,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFEN = RM_Field_RAC_DIAGAALTEN_RFEN(self)
        self.zz_fdict['RFEN'] = self.RFEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_DIAGAALTSEL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_DIAGAALTSEL, self).__init__(rmio, label,
            0xa8020000, 0x7F0,
            'DIAGAALTSEL', 'RAC.DIAGAALTSEL', 'read-write',
            u"",
            0x00000000, 0x00007F3F,
            0x00001000, 0x00002000,
            0x00003000)

        self.TP = RM_Field_RAC_DIAGAALTSEL_TP(self)
        self.zz_fdict['TP'] = self.TP
        self.BLK = RM_Field_RAC_DIAGAALTSEL_BLK(self)
        self.zz_fdict['BLK'] = self.BLK
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_DIAGAALTBRIDGECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_DIAGAALTBRIDGECTRL, self).__init__(rmio, label,
            0xa8020000, 0x7F4,
            'DIAGAALTBRIDGECTRL', 'RAC.DIAGAALTBRIDGECTRL', 'read-write',
            u"",
            0x00000000, 0x00000020,
            0x00001000, 0x00002000,
            0x00003000)

        self.RFSHORTDIS = RM_Field_RAC_DIAGAALTBRIDGECTRL_RFSHORTDIS(self)
        self.zz_fdict['RFSHORTDIS'] = self.RFSHORTDIS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFLOCK0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFLOCK0, self).__init__(rmio, label,
            0xa8020000, 0x7F8,
            'RFLOCK0', 'RAC.RFLOCK0', 'read-write',
            u"",
            0x80000000, 0xFFC7E3FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNTHLODIVFREQCTRL = RM_Field_RAC_RFLOCK0_SYNTHLODIVFREQCTRL(self)
        self.zz_fdict['SYNTHLODIVFREQCTRL'] = self.SYNTHLODIVFREQCTRL
        self.TXIQENABLE = RM_Field_RAC_RFLOCK0_TXIQENABLE(self)
        self.zz_fdict['TXIQENABLE'] = self.TXIQENABLE
        self.FFTCORPRO = RM_Field_RAC_RFLOCK0_FFTCORPRO(self)
        self.zz_fdict['FFTCORPRO'] = self.FFTCORPRO
        self.OFDMMODE = RM_Field_RAC_RFLOCK0_OFDMMODE(self)
        self.zz_fdict['OFDMMODE'] = self.OFDMMODE
        self.RACIFPGAEN = RM_Field_RAC_RFLOCK0_RACIFPGAEN(self)
        self.zz_fdict['RACIFPGAEN'] = self.RACIFPGAEN
        self.RACTXENFSKPA = RM_Field_RAC_RFLOCK0_RACTXENFSKPA(self)
        self.zz_fdict['RACTXENFSKPA'] = self.RACTXENFSKPA
        self.RACTXENOFDMPA = RM_Field_RAC_RFLOCK0_RACTXENOFDMPA(self)
        self.zz_fdict['RACTXENOFDMPA'] = self.RACTXENOFDMPA
        self.FRCCONVMODE = RM_Field_RAC_RFLOCK0_FRCCONVMODE(self)
        self.zz_fdict['FRCCONVMODE'] = self.FRCCONVMODE
        self.FRCPAUSING = RM_Field_RAC_RFLOCK0_FRCPAUSING(self)
        self.zz_fdict['FRCPAUSING'] = self.FRCPAUSING
        self.MODEMANTSWENABLE = RM_Field_RAC_RFLOCK0_MODEMANTSWENABLE(self)
        self.zz_fdict['MODEMANTSWENABLE'] = self.MODEMANTSWENABLE
        self.MODEMLRBLE = RM_Field_RAC_RFLOCK0_MODEMLRBLE(self)
        self.zz_fdict['MODEMLRBLE'] = self.MODEMLRBLE
        self.MODEMDSSS = RM_Field_RAC_RFLOCK0_MODEMDSSS(self)
        self.zz_fdict['MODEMDSSS'] = self.MODEMDSSS
        self.MODEMDEC1 = RM_Field_RAC_RFLOCK0_MODEMDEC1(self)
        self.zz_fdict['MODEMDEC1'] = self.MODEMDEC1
        self.MODEMMODFORMAT = RM_Field_RAC_RFLOCK0_MODEMMODFORMAT(self)
        self.zz_fdict['MODEMMODFORMAT'] = self.MODEMMODFORMAT
        self.MODEMDUALSYNC = RM_Field_RAC_RFLOCK0_MODEMDUALSYNC(self)
        self.zz_fdict['MODEMDUALSYNC'] = self.MODEMDUALSYNC
        self.MODEMANTDIVMODE = RM_Field_RAC_RFLOCK0_MODEMANTDIVMODE(self)
        self.zz_fdict['MODEMANTDIVMODE'] = self.MODEMANTDIVMODE
        self.UNLOCKED = RM_Field_RAC_RFLOCK0_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFLOCK1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFLOCK1, self).__init__(rmio, label,
            0xa8020000, 0x7FC,
            'RFLOCK1', 'RAC.RFLOCK1', 'read-write',
            u"",
            0x000000FF, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RACPASTRIPE = RM_Field_RAC_RFLOCK1_RACPASTRIPE(self)
        self.zz_fdict['RACPASTRIPE'] = self.RACPASTRIPE
        self.RACPASLICE = RM_Field_RAC_RFLOCK1_RACPASLICE(self)
        self.zz_fdict['RACPASLICE'] = self.RACPASLICE
        self.__dict__['zz_frozen'] = True


