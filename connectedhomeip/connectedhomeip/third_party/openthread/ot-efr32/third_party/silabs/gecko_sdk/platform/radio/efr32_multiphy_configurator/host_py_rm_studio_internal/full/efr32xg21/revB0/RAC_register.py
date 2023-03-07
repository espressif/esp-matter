
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
            0x00000000, 0xFFFFFFFF,
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
            0x00000000, 0xEF38FFFF,
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
        self.STATE = RM_Field_RAC_STATUS_STATE(self)
        self.zz_fdict['STATE'] = self.STATE
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
            0x00000000, 0xC000F1FF,
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
            0x00000000, 0x000107EF,
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
            0x00000000, 0x00FF0007,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGE = RM_Field_RAC_IF_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IF_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.BUSERROR = RM_Field_RAC_IF_BUSERROR(self)
        self.zz_fdict['BUSERROR'] = self.BUSERROR
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
            0x00000000, 0x00FF0007,
            0x00001000, 0x00002000,
            0x00003000)

        self.STATECHANGE = RM_Field_RAC_IEN_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IEN_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.BUSERROR = RM_Field_RAC_IEN_BUSERROR(self)
        self.zz_fdict['BUSERROR'] = self.BUSERROR
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
            0x00000000, 0x0000001F,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODEN = RM_Field_RAC_TESTCTRL_MODEN(self)
        self.zz_fdict['MODEN'] = self.MODEN
        self.DEMODEN = RM_Field_RAC_TESTCTRL_DEMODEN(self)
        self.zz_fdict['DEMODEN'] = self.DEMODEN
        self.AUX2RFSENSE = RM_Field_RAC_TESTCTRL_AUX2RFSENSE(self)
        self.zz_fdict['AUX2RFSENSE'] = self.AUX2RFSENSE
        self.LOOPBACK2LNAINPUT = RM_Field_RAC_TESTCTRL_LOOPBACK2LNAINPUT(self)
        self.zz_fdict['LOOPBACK2LNAINPUT'] = self.LOOPBACK2LNAINPUT
        self.LOOPBACK2LNAOUTPUT = RM_Field_RAC_TESTCTRL_LOOPBACK2LNAOUTPUT(self)
        self.zz_fdict['LOOPBACK2LNAOUTPUT'] = self.LOOPBACK2LNAOUTPUT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x028,
            'SEQSTATUS', 'RAC.SEQSTATUS', 'read-only',
            u"",
            0x00000010, 0x000005FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STOPPED = RM_Field_RAC_SEQSTATUS_STOPPED(self)
        self.zz_fdict['STOPPED'] = self.STOPPED
        self.BKPT = RM_Field_RAC_SEQSTATUS_BKPT(self)
        self.zz_fdict['BKPT'] = self.BKPT
        self.WAITING = RM_Field_RAC_SEQSTATUS_WAITING(self)
        self.zz_fdict['WAITING'] = self.WAITING
        self.WAITMODE = RM_Field_RAC_SEQSTATUS_WAITMODE(self)
        self.zz_fdict['WAITMODE'] = self.WAITMODE
        self.DONE = RM_Field_RAC_SEQSTATUS_DONE(self)
        self.zz_fdict['DONE'] = self.DONE
        self.NEG = RM_Field_RAC_SEQSTATUS_NEG(self)
        self.zz_fdict['NEG'] = self.NEG
        self.POS = RM_Field_RAC_SEQSTATUS_POS(self)
        self.zz_fdict['POS'] = self.POS
        self.ZERO = RM_Field_RAC_SEQSTATUS_ZERO(self)
        self.zz_fdict['ZERO'] = self.ZERO
        self.CARRY = RM_Field_RAC_SEQSTATUS_CARRY(self)
        self.zz_fdict['CARRY'] = self.CARRY
        self.ABORTEN = RM_Field_RAC_SEQSTATUS_ABORTEN(self)
        self.zz_fdict['ABORTEN'] = self.ABORTEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQCMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQCMD, self).__init__(rmio, label,
            0xa8020000, 0x02C,
            'SEQCMD', 'RAC.SEQCMD', 'write-only',
            u"",
            0x00000000, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.HALT = RM_Field_RAC_SEQCMD_HALT(self)
        self.zz_fdict['HALT'] = self.HALT
        self.STEP = RM_Field_RAC_SEQCMD_STEP(self)
        self.zz_fdict['STEP'] = self.STEP
        self.RESUME = RM_Field_RAC_SEQCMD_RESUME(self)
        self.zz_fdict['RESUME'] = self.RESUME
        self.BKPTEN = RM_Field_RAC_SEQCMD_BKPTEN(self)
        self.zz_fdict['BKPTEN'] = self.BKPTEN
        self.BKPTDIS = RM_Field_RAC_SEQCMD_BKPTDIS(self)
        self.zz_fdict['BKPTDIS'] = self.BKPTDIS
        self.ABORT = RM_Field_RAC_SEQCMD_ABORT(self)
        self.zz_fdict['ABORT'] = self.ABORT
        self.ABORTENSET = RM_Field_RAC_SEQCMD_ABORTENSET(self)
        self.zz_fdict['ABORTENSET'] = self.ABORTENSET
        self.ABORTENCLEAR = RM_Field_RAC_SEQCMD_ABORTENCLEAR(self)
        self.zz_fdict['ABORTENCLEAR'] = self.ABORTENCLEAR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_BREAKPOINT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_BREAKPOINT, self).__init__(rmio, label,
            0xa8020000, 0x030,
            'BREAKPOINT', 'RAC.BREAKPOINT', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.BKPADDR = RM_Field_RAC_BREAKPOINT_BKPADDR(self)
        self.zz_fdict['BKPADDR'] = self.BKPADDR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R0, self).__init__(rmio, label,
            0xa8020000, 0x034,
            'R0', 'RAC.R0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R0 = RM_Field_RAC_R0_R0(self)
        self.zz_fdict['R0'] = self.R0
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R1, self).__init__(rmio, label,
            0xa8020000, 0x038,
            'R1', 'RAC.R1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R1 = RM_Field_RAC_R1_R1(self)
        self.zz_fdict['R1'] = self.R1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R2, self).__init__(rmio, label,
            0xa8020000, 0x03C,
            'R2', 'RAC.R2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R2 = RM_Field_RAC_R2_R2(self)
        self.zz_fdict['R2'] = self.R2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R3, self).__init__(rmio, label,
            0xa8020000, 0x040,
            'R3', 'RAC.R3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R3 = RM_Field_RAC_R3_R3(self)
        self.zz_fdict['R3'] = self.R3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R4, self).__init__(rmio, label,
            0xa8020000, 0x044,
            'R4', 'RAC.R4', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R4 = RM_Field_RAC_R4_R4(self)
        self.zz_fdict['R4'] = self.R4
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R5, self).__init__(rmio, label,
            0xa8020000, 0x048,
            'R5', 'RAC.R5', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R5 = RM_Field_RAC_R5_R5(self)
        self.zz_fdict['R5'] = self.R5
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R6, self).__init__(rmio, label,
            0xa8020000, 0x04C,
            'R6', 'RAC.R6', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R6 = RM_Field_RAC_R6_R6(self)
        self.zz_fdict['R6'] = self.R6
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R7, self).__init__(rmio, label,
            0xa8020000, 0x050,
            'R7', 'RAC.R7', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.R7 = RM_Field_RAC_R7_R7(self)
        self.zz_fdict['R7'] = self.R7
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_WAITMASK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_WAITMASK, self).__init__(rmio, label,
            0xa8020000, 0x054,
            'WAITMASK', 'RAC.WAITMASK', 'read-only',
            u"",
            0x00000000, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STCMP = RM_Field_RAC_WAITMASK_STCMP(self)
        self.zz_fdict['STCMP'] = self.STCMP
        self.FRCRX = RM_Field_RAC_WAITMASK_FRCRX(self)
        self.zz_fdict['FRCRX'] = self.FRCRX
        self.FRCTX = RM_Field_RAC_WAITMASK_FRCTX(self)
        self.zz_fdict['FRCTX'] = self.FRCTX
        self.PRSEVENT = RM_Field_RAC_WAITMASK_PRSEVENT(self)
        self.zz_fdict['PRSEVENT'] = self.PRSEVENT
        self.DEMODRXREQCLR = RM_Field_RAC_WAITMASK_DEMODRXREQCLR(self)
        self.zz_fdict['DEMODRXREQCLR'] = self.DEMODRXREQCLR
        self.SYNTHRDY = RM_Field_RAC_WAITMASK_SYNTHRDY(self)
        self.zz_fdict['SYNTHRDY'] = self.SYNTHRDY
        self.RAMPDONE = RM_Field_RAC_WAITMASK_RAMPDONE(self)
        self.zz_fdict['RAMPDONE'] = self.RAMPDONE
        self.FRCPAUSED = RM_Field_RAC_WAITMASK_FRCPAUSED(self)
        self.zz_fdict['FRCPAUSED'] = self.FRCPAUSED
        self.ANTSWITCH = RM_Field_RAC_WAITMASK_ANTSWITCH(self)
        self.zz_fdict['ANTSWITCH'] = self.ANTSWITCH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_WAITSNSH(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_WAITSNSH, self).__init__(rmio, label,
            0xa8020000, 0x058,
            'WAITSNSH', 'RAC.WAITSNSH', 'read-only',
            u"",
            0x00000000, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.WAITSNSH = RM_Field_RAC_WAITSNSH_WAITSNSH(self)
        self.zz_fdict['WAITSNSH'] = self.WAITSNSH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STIMER(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STIMER, self).__init__(rmio, label,
            0xa8020000, 0x05C,
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
            0xa8020000, 0x060,
            'STIMERCOMP', 'RAC.STIMERCOMP', 'read-write',
            u"",
            0x00000000, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.STIMERCOMP = RM_Field_RAC_STIMERCOMP_STIMERCOMP(self)
        self.zz_fdict['STIMERCOMP'] = self.STIMERCOMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_VECTADDR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_VECTADDR, self).__init__(rmio, label,
            0xa8020000, 0x064,
            'VECTADDR', 'RAC.VECTADDR', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VECTADDR = RM_Field_RAC_VECTADDR_VECTADDR(self)
        self.zz_fdict['VECTADDR'] = self.VECTADDR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQCTRL, self).__init__(rmio, label,
            0xa8020000, 0x068,
            'SEQCTRL', 'RAC.SEQCTRL', 'read-write',
            u"",
            0x00000000, 0x00001C07,
            0x00001000, 0x00002000,
            0x00003000)

        self.COMPACT = RM_Field_RAC_SEQCTRL_COMPACT(self)
        self.zz_fdict['COMPACT'] = self.COMPACT
        self.COMPINVALMODE = RM_Field_RAC_SEQCTRL_COMPINVALMODE(self)
        self.zz_fdict['COMPINVALMODE'] = self.COMPINVALMODE
        self.STIMERDEBUGRUN = RM_Field_RAC_SEQCTRL_STIMERDEBUGRUN(self)
        self.zz_fdict['STIMERDEBUGRUN'] = self.STIMERDEBUGRUN
        self.CPUHALTREQEN = RM_Field_RAC_SEQCTRL_CPUHALTREQEN(self)
        self.zz_fdict['CPUHALTREQEN'] = self.CPUHALTREQEN
        self.SEQHALTUPONCPUHALTEN = RM_Field_RAC_SEQCTRL_SEQHALTUPONCPUHALTEN(self)
        self.zz_fdict['SEQHALTUPONCPUHALTEN'] = self.SEQHALTUPONCPUHALTEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PRESC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PRESC, self).__init__(rmio, label,
            0xa8020000, 0x06C,
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
            0xa8020000, 0x070,
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
            0xa8020000, 0x074,
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
            0xa8020000, 0x078,
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
            0xa8020000, 0x07C,
            'SR3', 'RAC.SR3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SR3 = RM_Field_RAC_SR3_SR3(self)
        self.zz_fdict['SR3'] = self.SR3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_WUREQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_WUREQ, self).__init__(rmio, label,
            0xa8020000, 0x080,
            'WUREQ', 'RAC.WUREQ', 'read-write',
            u"",
            0x00000000, 0x00000007,
            0x00001000, 0x00002000,
            0x00003000)

        self.MANUAL = RM_Field_RAC_WUREQ_MANUAL(self)
        self.zz_fdict['MANUAL'] = self.MANUAL
        self.OVERRIDE = RM_Field_RAC_WUREQ_OVERRIDE(self)
        self.zz_fdict['OVERRIDE'] = self.OVERRIDE
        self.STATUS = RM_Field_RAC_WUREQ_STATUS(self)
        self.zz_fdict['STATUS'] = self.STATUS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHENCTRL, self).__init__(rmio, label,
            0xa8020000, 0x084,
            'SYNTHENCTRL', 'RAC.SYNTHENCTRL', 'read-write',
            u"",
            0x00000000, 0x00100282,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCOSTARTUP = RM_Field_RAC_SYNTHENCTRL_VCOSTARTUP(self)
        self.zz_fdict['VCOSTARTUP'] = self.VCOSTARTUP
        self.VCBUFEN = RM_Field_RAC_SYNTHENCTRL_VCBUFEN(self)
        self.zz_fdict['VCBUFEN'] = self.VCBUFEN
        self.SYNTHSTARTREQ = RM_Field_RAC_SYNTHENCTRL_SYNTHSTARTREQ(self)
        self.zz_fdict['SYNTHSTARTREQ'] = self.SYNTHSTARTREQ
        self.LPFBWSEL = RM_Field_RAC_SYNTHENCTRL_LPFBWSEL(self)
        self.zz_fdict['LPFBWSEL'] = self.LPFBWSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHREGCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHREGCTRL, self).__init__(rmio, label,
            0xa8020000, 0x088,
            'SYNTHREGCTRL', 'RAC.SYNTHREGCTRL', 'read-write',
            u"",
            0x04000000, 0x07001C00,
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
            0xa8020000, 0x08C,
            'VCOCTRL', 'RAC.VCOCTRL', 'read-write',
            u"",
            0x0000004C, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCOAMPLITUDE = RM_Field_RAC_VCOCTRL_VCOAMPLITUDE(self)
        self.zz_fdict['VCOAMPLITUDE'] = self.VCOAMPLITUDE
        self.VCODETAMPLITUDE = RM_Field_RAC_VCOCTRL_VCODETAMPLITUDE(self)
        self.zz_fdict['VCODETAMPLITUDE'] = self.VCODETAMPLITUDE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CHPCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CHPCTRL, self).__init__(rmio, label,
            0xa8020000, 0x090,
            'CHPCTRL', 'RAC.CHPCTRL', 'read-write',
            u"",
            0x00000000, 0x00004000,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHPOUTPUTTRISTATEMODE = RM_Field_RAC_CHPCTRL_CHPOUTPUTTRISTATEMODE(self)
        self.zz_fdict['CHPOUTPUTTRISTATEMODE'] = self.CHPOUTPUTTRISTATEMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHCTRL, self).__init__(rmio, label,
            0xa8020000, 0x094,
            'SYNTHCTRL', 'RAC.SYNTHCTRL', 'read-write',
            u"",
            0x00000000, 0x00000400,
            0x00001000, 0x00002000,
            0x00003000)

        self.MMDPOWERBALANCEDISABLE = RM_Field_RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE(self)
        self.zz_fdict['MMDPOWERBALANCEDISABLE'] = self.MMDPOWERBALANCEDISABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x098,
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
            0xa8020000, 0x09C,
            'STATUS2', 'RAC.STATUS2', 'read-only',
            u"",
            0x00000000, 0x00000FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PREVSTATE1 = RM_Field_RAC_STATUS2_PREVSTATE1(self)
        self.zz_fdict['PREVSTATE1'] = self.PREVSTATE1
        self.PREVSTATE2 = RM_Field_RAC_STATUS2_PREVSTATE2(self)
        self.zz_fdict['PREVSTATE2'] = self.PREVSTATE2
        self.PREVSTATE3 = RM_Field_RAC_STATUS2_PREVSTATE3(self)
        self.zz_fdict['PREVSTATE3'] = self.PREVSTATE3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFPGACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFPGACTRL, self).__init__(rmio, label,
            0xa8020000, 0x0A0,
            'IFPGACTRL', 'RAC.IFPGACTRL', 'read-write',
            u"",
            0x00000000, 0x0FF80000,
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
            0xa8020000, 0x0A4,
            'PAENCTRL', 'RAC.PAENCTRL', 'read-write',
            u"",
            0x00000000, 0x00000100,
            0x00001000, 0x00002000,
            0x00003000)

        self.PARAMP = RM_Field_RAC_PAENCTRL_PARAMP(self)
        self.zz_fdict['PARAMP'] = self.PARAMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_APC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_APC, self).__init__(rmio, label,
            0xa8020000, 0x0A8,
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


class RM_Register_RAC_AUXADCTRIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCTRIM, self).__init__(rmio, label,
            0xa8020000, 0x0AC,
            'AUXADCTRIM', 'RAC.AUXADCTRIM', 'read-write',
            u"",
            0x06D55502, 0x1FFFFFFF,
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
            0xa8020000, 0x0B0,
            'AUXADCEN', 'RAC.AUXADCEN', 'read-write',
            u"",
            0x00000000, 0x000001FF,
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
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCCTRL0, self).__init__(rmio, label,
            0xa8020000, 0x0B4,
            'AUXADCCTRL0', 'RAC.AUXADCCTRL0', 'read-write',
            u"",
            0x00000100, 0x0003FFFF,
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
        self.INPUTRESSEL = RM_Field_RAC_AUXADCCTRL0_INPUTRESSEL(self)
        self.zz_fdict['INPUTRESSEL'] = self.INPUTRESSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCCTRL1, self).__init__(rmio, label,
            0xa8020000, 0x0B8,
            'AUXADCCTRL1', 'RAC.AUXADCCTRL1', 'read-write',
            u"",
            0x00000000, 0x031F0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCINPUTRESSEL = RM_Field_RAC_AUXADCCTRL1_AUXADCINPUTRESSEL(self)
        self.zz_fdict['AUXADCINPUTRESSEL'] = self.AUXADCINPUTRESSEL
        self.AUXADCINPUTSELECT = RM_Field_RAC_AUXADCCTRL1_AUXADCINPUTSELECT(self)
        self.zz_fdict['AUXADCINPUTSELECT'] = self.AUXADCINPUTSELECT
        self.AUXADCPMONSELECT = RM_Field_RAC_AUXADCCTRL1_AUXADCPMONSELECT(self)
        self.zz_fdict['AUXADCPMONSELECT'] = self.AUXADCPMONSELECT
        self.AUXADCTSENSESELCURR = RM_Field_RAC_AUXADCCTRL1_AUXADCTSENSESELCURR(self)
        self.zz_fdict['AUXADCTSENSESELCURR'] = self.AUXADCTSENSESELCURR
        self.AUXADCRESET = RM_Field_RAC_AUXADCCTRL1_AUXADCRESET(self)
        self.zz_fdict['AUXADCRESET'] = self.AUXADCRESET
        self.AUXADCTSENSESELVBE = RM_Field_RAC_AUXADCCTRL1_AUXADCTSENSESELVBE(self)
        self.zz_fdict['AUXADCTSENSESELVBE'] = self.AUXADCTSENSESELVBE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXADCOUT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXADCOUT, self).__init__(rmio, label,
            0xa8020000, 0x0BC,
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
            0xa8020000, 0x0C0,
            'CLKMULTEN0', 'RAC.CLKMULTEN0', 'read-write',
            u"",
            0x02A40005, 0x03FFFFFF,
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
        self.CLKMULTENDRVDIFF = RM_Field_RAC_CLKMULTEN0_CLKMULTENDRVDIFF(self)
        self.zz_fdict['CLKMULTENDRVDIFF'] = self.CLKMULTENDRVDIFF
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
        self.CLKMULTENROTDET = RM_Field_RAC_CLKMULTEN0_CLKMULTENROTDET(self)
        self.zz_fdict['CLKMULTENROTDET'] = self.CLKMULTENROTDET
        self.CLKMULTFREQCAL = RM_Field_RAC_CLKMULTEN0_CLKMULTFREQCAL(self)
        self.zz_fdict['CLKMULTFREQCAL'] = self.CLKMULTFREQCAL
        self.CLKMULTREG1ADJV = RM_Field_RAC_CLKMULTEN0_CLKMULTREG1ADJV(self)
        self.zz_fdict['CLKMULTREG1ADJV'] = self.CLKMULTREG1ADJV
        self.CLKMULTREG2ADJI = RM_Field_RAC_CLKMULTEN0_CLKMULTREG2ADJI(self)
        self.zz_fdict['CLKMULTREG2ADJI'] = self.CLKMULTREG2ADJI
        self.CLKMULTREG2ADJV = RM_Field_RAC_CLKMULTEN0_CLKMULTREG2ADJV(self)
        self.zz_fdict['CLKMULTREG2ADJV'] = self.CLKMULTREG2ADJV
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CLKMULTEN1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CLKMULTEN1, self).__init__(rmio, label,
            0xa8020000, 0x0C4,
            'CLKMULTEN1', 'RAC.CLKMULTEN1', 'read-write',
            u"",
            0x00000188, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CLKMULTINNIBBLE = RM_Field_RAC_CLKMULTEN1_CLKMULTINNIBBLE(self)
        self.zz_fdict['CLKMULTINNIBBLE'] = self.CLKMULTINNIBBLE
        self.CLKMULTLDCNIB = RM_Field_RAC_CLKMULTEN1_CLKMULTLDCNIB(self)
        self.zz_fdict['CLKMULTLDCNIB'] = self.CLKMULTLDCNIB
        self.CLKMULTLDFNIB = RM_Field_RAC_CLKMULTEN1_CLKMULTLDFNIB(self)
        self.zz_fdict['CLKMULTLDFNIB'] = self.CLKMULTLDFNIB
        self.CLKMULTLDMNIB = RM_Field_RAC_CLKMULTEN1_CLKMULTLDMNIB(self)
        self.zz_fdict['CLKMULTLDMNIB'] = self.CLKMULTLDMNIB
        self.CLKMULTRDNIBBLE = RM_Field_RAC_CLKMULTEN1_CLKMULTRDNIBBLE(self)
        self.zz_fdict['CLKMULTRDNIBBLE'] = self.CLKMULTRDNIBBLE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CLKMULTCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CLKMULTCTRL, self).__init__(rmio, label,
            0xa8020000, 0x0C8,
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
            0xa8020000, 0x0CC,
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
            0xa8020000, 0x0D0,
            'IFADCDBG', 'RAC.IFADCDBG', 'read-write',
            u"",
            0x0000000E, 0x0000003F,
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
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCTRIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCTRIM, self).__init__(rmio, label,
            0xa8020000, 0x0D4,
            'IFADCTRIM', 'RAC.IFADCTRIM', 'read-write',
            u"",
            0x08965290, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCCLKSEL = RM_Field_RAC_IFADCTRIM_IFADCCLKSEL(self)
        self.zz_fdict['IFADCCLKSEL'] = self.IFADCCLKSEL
        self.IFADCENHALFMODE = RM_Field_RAC_IFADCTRIM_IFADCENHALFMODE(self)
        self.zz_fdict['IFADCENHALFMODE'] = self.IFADCENHALFMODE
        self.IFADCLDOSERIESAMPLVL = RM_Field_RAC_IFADCTRIM_IFADCLDOSERIESAMPLVL(self)
        self.zz_fdict['IFADCLDOSERIESAMPLVL'] = self.IFADCLDOSERIESAMPLVL
        self.IFADCLDOSHUNTAMPLVL = RM_Field_RAC_IFADCTRIM_IFADCLDOSHUNTAMPLVL(self)
        self.zz_fdict['IFADCLDOSHUNTAMPLVL'] = self.IFADCLDOSHUNTAMPLVL
        self.IFADCLDOSHUNTCURLP = RM_Field_RAC_IFADCTRIM_IFADCLDOSHUNTCURLP(self)
        self.zz_fdict['IFADCLDOSHUNTCURLP'] = self.IFADCLDOSHUNTCURLP
        self.IFADCLDOSHUNTCURLVL = RM_Field_RAC_IFADCTRIM_IFADCLDOSHUNTCURLVL(self)
        self.zz_fdict['IFADCLDOSHUNTCURLVL'] = self.IFADCLDOSHUNTCURLVL
        self.IFADCOTAST1CURRENT = RM_Field_RAC_IFADCTRIM_IFADCOTAST1CURRENT(self)
        self.zz_fdict['IFADCOTAST1CURRENT'] = self.IFADCOTAST1CURRENT
        self.IFADCOTAST2CURRENT = RM_Field_RAC_IFADCTRIM_IFADCOTAST2CURRENT(self)
        self.zz_fdict['IFADCOTAST2CURRENT'] = self.IFADCOTAST2CURRENT
        self.IFADCREFBUFAMPLVL = RM_Field_RAC_IFADCTRIM_IFADCREFBUFAMPLVL(self)
        self.zz_fdict['IFADCREFBUFAMPLVL'] = self.IFADCREFBUFAMPLVL
        self.IFADCREFBUFCURLVL = RM_Field_RAC_IFADCTRIM_IFADCREFBUFCURLVL(self)
        self.zz_fdict['IFADCREFBUFCURLVL'] = self.IFADCREFBUFCURLVL
        self.IFADCSIDETONEAMP = RM_Field_RAC_IFADCTRIM_IFADCSIDETONEAMP(self)
        self.zz_fdict['IFADCSIDETONEAMP'] = self.IFADCSIDETONEAMP
        self.IFADCSIDETONEFREQ = RM_Field_RAC_IFADCTRIM_IFADCSIDETONEFREQ(self)
        self.zz_fdict['IFADCSIDETONEFREQ'] = self.IFADCSIDETONEFREQ
        self.IFADCTUNEZERO = RM_Field_RAC_IFADCTRIM_IFADCTUNEZERO(self)
        self.zz_fdict['IFADCTUNEZERO'] = self.IFADCTUNEZERO
        self.IFADCVCMLVL = RM_Field_RAC_IFADCTRIM_IFADCVCMLVL(self)
        self.zz_fdict['IFADCVCMLVL'] = self.IFADCVCMLVL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCCAL, self).__init__(rmio, label,
            0xa8020000, 0x0D8,
            'IFADCCAL', 'RAC.IFADCCAL', 'read-write',
            u"",
            0x00001000, 0x00001F03,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFADCENRCCAL = RM_Field_RAC_IFADCCAL_IFADCENRCCAL(self)
        self.zz_fdict['IFADCENRCCAL'] = self.IFADCENRCCAL
        self.IFADCTUNERCCALMODE = RM_Field_RAC_IFADCCAL_IFADCTUNERCCALMODE(self)
        self.zz_fdict['IFADCTUNERCCALMODE'] = self.IFADCTUNERCCALMODE
        self.IFADCTUNERC = RM_Field_RAC_IFADCCAL_IFADCTUNERC(self)
        self.zz_fdict['IFADCTUNERC'] = self.IFADCTUNERC
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCSTATUS, self).__init__(rmio, label,
            0xa8020000, 0x0DC,
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
            0xa8020000, 0x0E0,
            'LNAMIXDEBUG', 'RAC.LNAMIXDEBUG', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXDISMXR1 = RM_Field_RAC_LNAMIXDEBUG_LNAMIXDISMXR1(self)
        self.zz_fdict['LNAMIXDISMXR1'] = self.LNAMIXDISMXR1
        self.LNAMIXDISMXR2 = RM_Field_RAC_LNAMIXDEBUG_LNAMIXDISMXR2(self)
        self.zz_fdict['LNAMIXDISMXR2'] = self.LNAMIXDISMXR2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM0, self).__init__(rmio, label,
            0xa8020000, 0x0E4,
            'LNAMIXTRIM0', 'RAC.LNAMIXTRIM0', 'read-write',
            u"",
            0x0442093D, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXCURCTRL = RM_Field_RAC_LNAMIXTRIM0_LNAMIXCURCTRL(self)
        self.zz_fdict['LNAMIXCURCTRL'] = self.LNAMIXCURCTRL
        self.LNAMIXHIGHCUR = RM_Field_RAC_LNAMIXTRIM0_LNAMIXHIGHCUR(self)
        self.zz_fdict['LNAMIXHIGHCUR'] = self.LNAMIXHIGHCUR
        self.LNAMIXLOWCUR = RM_Field_RAC_LNAMIXTRIM0_LNAMIXLOWCUR(self)
        self.zz_fdict['LNAMIXLOWCUR'] = self.LNAMIXLOWCUR
        self.LNAMIXRFPKDBWSEL = RM_Field_RAC_LNAMIXTRIM0_LNAMIXRFPKDBWSEL(self)
        self.zz_fdict['LNAMIXRFPKDBWSEL'] = self.LNAMIXRFPKDBWSEL
        self.LNAMIXRFPKDCALCM = RM_Field_RAC_LNAMIXTRIM0_LNAMIXRFPKDCALCM(self)
        self.zz_fdict['LNAMIXRFPKDCALCM'] = self.LNAMIXRFPKDCALCM
        self.LNAMIXRFPKDCALDM = RM_Field_RAC_LNAMIXTRIM0_LNAMIXRFPKDCALDM(self)
        self.zz_fdict['LNAMIXRFPKDCALDM'] = self.LNAMIXRFPKDCALDM
        self.LNAMIXTRIMVREG = RM_Field_RAC_LNAMIXTRIM0_LNAMIXTRIMVREG(self)
        self.zz_fdict['LNAMIXTRIMVREG'] = self.LNAMIXTRIMVREG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM1, self).__init__(rmio, label,
            0xa8020000, 0x0E8,
            'LNAMIXTRIM1', 'RAC.LNAMIXTRIM1', 'read-write',
            u"",
            0x00011508, 0x0001FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXIBIAS1ADJ = RM_Field_RAC_LNAMIXTRIM1_LNAMIXIBIAS1ADJ(self)
        self.zz_fdict['LNAMIXIBIAS1ADJ'] = self.LNAMIXIBIAS1ADJ
        self.LNAMIXLNA1CAPSEL = RM_Field_RAC_LNAMIXTRIM1_LNAMIXLNA1CAPSEL(self)
        self.zz_fdict['LNAMIXLNA1CAPSEL'] = self.LNAMIXLNA1CAPSEL
        self.LNAMIXMXRBIAS1 = RM_Field_RAC_LNAMIXTRIM1_LNAMIXMXRBIAS1(self)
        self.zz_fdict['LNAMIXMXRBIAS1'] = self.LNAMIXMXRBIAS1
        self.LNAMIXNCAS1ADJ = RM_Field_RAC_LNAMIXTRIM1_LNAMIXNCAS1ADJ(self)
        self.zz_fdict['LNAMIXNCAS1ADJ'] = self.LNAMIXNCAS1ADJ
        self.LNAMIXPCAS1ADJ = RM_Field_RAC_LNAMIXTRIM1_LNAMIXPCAS1ADJ(self)
        self.zz_fdict['LNAMIXPCAS1ADJ'] = self.LNAMIXPCAS1ADJ
        self.LNAMIXVOUT1ADJ = RM_Field_RAC_LNAMIXTRIM1_LNAMIXVOUT1ADJ(self)
        self.zz_fdict['LNAMIXVOUT1ADJ'] = self.LNAMIXVOUT1ADJ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXTRIM2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXTRIM2, self).__init__(rmio, label,
            0xa8020000, 0x0EC,
            'LNAMIXTRIM2', 'RAC.LNAMIXTRIM2', 'read-write',
            u"",
            0x00011508, 0x0001FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXIBIAS2ADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXIBIAS2ADJ(self)
        self.zz_fdict['LNAMIXIBIAS2ADJ'] = self.LNAMIXIBIAS2ADJ
        self.LNAMIXLNA2CAPSEL = RM_Field_RAC_LNAMIXTRIM2_LNAMIXLNA2CAPSEL(self)
        self.zz_fdict['LNAMIXLNA2CAPSEL'] = self.LNAMIXLNA2CAPSEL
        self.LNAMIXMXRBIAS2 = RM_Field_RAC_LNAMIXTRIM2_LNAMIXMXRBIAS2(self)
        self.zz_fdict['LNAMIXMXRBIAS2'] = self.LNAMIXMXRBIAS2
        self.LNAMIXNCAS2ADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXNCAS2ADJ(self)
        self.zz_fdict['LNAMIXNCAS2ADJ'] = self.LNAMIXNCAS2ADJ
        self.LNAMIXPCAS2ADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXPCAS2ADJ(self)
        self.zz_fdict['LNAMIXPCAS2ADJ'] = self.LNAMIXPCAS2ADJ
        self.LNAMIXVOUT2ADJ = RM_Field_RAC_LNAMIXTRIM2_LNAMIXVOUT2ADJ(self)
        self.zz_fdict['LNAMIXVOUT2ADJ'] = self.LNAMIXVOUT2ADJ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXCAL, self).__init__(rmio, label,
            0xa8020000, 0x0F0,
            'LNAMIXCAL', 'RAC.LNAMIXCAL', 'read-write',
            u"",
            0x000003F0, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXCALEN = RM_Field_RAC_LNAMIXCAL_LNAMIXCALEN(self)
        self.zz_fdict['LNAMIXCALEN'] = self.LNAMIXCALEN
        self.LNAMIXCALVMODE = RM_Field_RAC_LNAMIXCAL_LNAMIXCALVMODE(self)
        self.zz_fdict['LNAMIXCALVMODE'] = self.LNAMIXCALVMODE
        self.LNAMIXENIRCAL1 = RM_Field_RAC_LNAMIXCAL_LNAMIXENIRCAL1(self)
        self.zz_fdict['LNAMIXENIRCAL1'] = self.LNAMIXENIRCAL1
        self.LNAMIXENIRCAL2 = RM_Field_RAC_LNAMIXCAL_LNAMIXENIRCAL2(self)
        self.zz_fdict['LNAMIXENIRCAL2'] = self.LNAMIXENIRCAL2
        self.LNAMIXIRCAL1AMP = RM_Field_RAC_LNAMIXCAL_LNAMIXIRCAL1AMP(self)
        self.zz_fdict['LNAMIXIRCAL1AMP'] = self.LNAMIXIRCAL1AMP
        self.LNAMIXIRCAL2AMP = RM_Field_RAC_LNAMIXCAL_LNAMIXIRCAL2AMP(self)
        self.zz_fdict['LNAMIXIRCAL2AMP'] = self.LNAMIXIRCAL2AMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXEN, self).__init__(rmio, label,
            0xa8020000, 0x0F4,
            'LNAMIXEN', 'RAC.LNAMIXEN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXENLDO = RM_Field_RAC_LNAMIXEN_LNAMIXENLDO(self)
        self.zz_fdict['LNAMIXENLDO'] = self.LNAMIXENLDO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PRECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PRECTRL, self).__init__(rmio, label,
            0xa8020000, 0x0F8,
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


class RM_Register_RAC_PATRIM0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM0, self).__init__(rmio, label,
            0xa8020000, 0x0FC,
            'PATRIM0', 'RAC.PATRIM0', 'read-write',
            u"",
            0x20088D93, 0x3FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PATRIMDRVREGIBCORE0DBM = RM_Field_RAC_PATRIM0_PATRIMDRVREGIBCORE0DBM(self)
        self.zz_fdict['PATRIMDRVREGIBCORE0DBM'] = self.PATRIMDRVREGIBCORE0DBM
        self.PATRIMDRVREGIBNDIO0DBM = RM_Field_RAC_PATRIM0_PATRIMDRVREGIBNDIO0DBM(self)
        self.zz_fdict['PATRIMDRVREGIBNDIO0DBM'] = self.PATRIMDRVREGIBNDIO0DBM
        self.PATRIMDRVREGPSR0DBM = RM_Field_RAC_PATRIM0_PATRIMDRVREGPSR0DBM(self)
        self.zz_fdict['PATRIMDRVREGPSR0DBM'] = self.PATRIMDRVREGPSR0DBM
        self.PATRIMDRVSLICE0DBM = RM_Field_RAC_PATRIM0_PATRIMDRVSLICE0DBM(self)
        self.zz_fdict['PATRIMDRVSLICE0DBM'] = self.PATRIMDRVSLICE0DBM
        self.PATRIMFB0DBM = RM_Field_RAC_PATRIM0_PATRIMFB0DBM(self)
        self.zz_fdict['PATRIMFB0DBM'] = self.PATRIMFB0DBM
        self.PATRIMPABIASN0DBM = RM_Field_RAC_PATRIM0_PATRIMPABIASN0DBM(self)
        self.zz_fdict['PATRIMPABIASN0DBM'] = self.PATRIMPABIASN0DBM
        self.PATRIMPABIASP0DBM = RM_Field_RAC_PATRIM0_PATRIMPABIASP0DBM(self)
        self.zz_fdict['PATRIMPABIASP0DBM'] = self.PATRIMPABIASP0DBM
        self.PATRIMPASLICE0DBM = RM_Field_RAC_PATRIM0_PATRIMPASLICE0DBM(self)
        self.zz_fdict['PATRIMPASLICE0DBM'] = self.PATRIMPASLICE0DBM
        self.PATRIMVREF0DBM = RM_Field_RAC_PATRIM0_PATRIMVREF0DBM(self)
        self.zz_fdict['PATRIMVREF0DBM'] = self.PATRIMVREF0DBM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM1, self).__init__(rmio, label,
            0xa8020000, 0x100,
            'PATRIM1', 'RAC.PATRIM1', 'read-write',
            u"",
            0x40000980, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PATRIM10DBMDUTYCYN = RM_Field_RAC_PATRIM1_PATRIM10DBMDUTYCYN(self)
        self.zz_fdict['PATRIM10DBMDUTYCYN'] = self.PATRIM10DBMDUTYCYN
        self.PATRIM10DBMDUTYCYP = RM_Field_RAC_PATRIM1_PATRIM10DBMDUTYCYP(self)
        self.zz_fdict['PATRIM10DBMDUTYCYP'] = self.PATRIM10DBMDUTYCYP
        self.PATRIM20DBMPREDRV = RM_Field_RAC_PATRIM1_PATRIM20DBMPREDRV(self)
        self.zz_fdict['PATRIM20DBMPREDRV'] = self.PATRIM20DBMPREDRV
        self.PATRIMANTSWBIAS = RM_Field_RAC_PATRIM1_PATRIMANTSWBIAS(self)
        self.zz_fdict['PATRIMANTSWBIAS'] = self.PATRIMANTSWBIAS
        self.PATRIMBLEEDAUTOPREREG = RM_Field_RAC_PATRIM1_PATRIMBLEEDAUTOPREREG(self)
        self.zz_fdict['PATRIMBLEEDAUTOPREREG'] = self.PATRIMBLEEDAUTOPREREG
        self.PATRIMCAPPAOUTM = RM_Field_RAC_PATRIM1_PATRIMCAPPAOUTM(self)
        self.zz_fdict['PATRIMCAPPAOUTM'] = self.PATRIMCAPPAOUTM
        self.PATRIMCAPPAOUTP = RM_Field_RAC_PATRIM1_PATRIMCAPPAOUTP(self)
        self.zz_fdict['PATRIMCAPPAOUTP'] = self.PATRIMCAPPAOUTP
        self.PATRIMCMGAIN = RM_Field_RAC_PATRIM1_PATRIMCMGAIN(self)
        self.zz_fdict['PATRIMCMGAIN'] = self.PATRIMCMGAIN
        self.PATRIMDLY0 = RM_Field_RAC_PATRIM1_PATRIMDLY0(self)
        self.zz_fdict['PATRIMDLY0'] = self.PATRIMDLY0
        self.PATRIMDLY1 = RM_Field_RAC_PATRIM1_PATRIMDLY1(self)
        self.zz_fdict['PATRIMDLY1'] = self.PATRIMDLY1
        self.PATRIMFBKATTPDRVLDO = RM_Field_RAC_PATRIM1_PATRIMFBKATTPDRVLDO(self)
        self.zz_fdict['PATRIMFBKATTPDRVLDO'] = self.PATRIMFBKATTPDRVLDO
        self.PATRIMIBIASMASTER = RM_Field_RAC_PATRIM1_PATRIMIBIASMASTER(self)
        self.zz_fdict['PATRIMIBIASMASTER'] = self.PATRIMIBIASMASTER
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PATRIM2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PATRIM2, self).__init__(rmio, label,
            0xa8020000, 0x104,
            'PATRIM2', 'RAC.PATRIM2', 'read-write',
            u"",
            0x30C0F87A, 0x7FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PATRIMLDOFBHVPDRVLDO = RM_Field_RAC_PATRIM2_PATRIMLDOFBHVPDRVLDO(self)
        self.zz_fdict['PATRIMLDOFBHVPDRVLDO'] = self.PATRIMLDOFBHVPDRVLDO
        self.PATRIMLDOFBHVPREREG = RM_Field_RAC_PATRIM2_PATRIMLDOFBHVPREREG(self)
        self.zz_fdict['PATRIMLDOFBHVPREREG'] = self.PATRIMLDOFBHVPREREG
        self.PATRIMLDOHVPDRVLDO = RM_Field_RAC_PATRIM2_PATRIMLDOHVPDRVLDO(self)
        self.zz_fdict['PATRIMLDOHVPDRVLDO'] = self.PATRIMLDOHVPDRVLDO
        self.PATRIMLDOHVPREREG = RM_Field_RAC_PATRIM2_PATRIMLDOHVPREREG(self)
        self.zz_fdict['PATRIMLDOHVPREREG'] = self.PATRIMLDOHVPREREG
        self.PATRIMLDOPSRPDRVLDO = RM_Field_RAC_PATRIM2_PATRIMLDOPSRPDRVLDO(self)
        self.zz_fdict['PATRIMLDOPSRPDRVLDO'] = self.PATRIMLDOPSRPDRVLDO
        self.PATRIMLDOPSRPREREG = RM_Field_RAC_PATRIM2_PATRIMLDOPSRPREREG(self)
        self.zz_fdict['PATRIMLDOPSRPREREG'] = self.PATRIMLDOPSRPREREG
        self.PATRIMLDOSLICESPDRVLDO = RM_Field_RAC_PATRIM2_PATRIMLDOSLICESPDRVLDO(self)
        self.zz_fdict['PATRIMLDOSLICESPDRVLDO'] = self.PATRIMLDOSLICESPDRVLDO
        self.PATRIMLDOSLICESPREREG = RM_Field_RAC_PATRIM2_PATRIMLDOSLICESPREREG(self)
        self.zz_fdict['PATRIMLDOSLICESPREREG'] = self.PATRIMLDOSLICESPREREG
        self.PATRIMPADACGLITCH = RM_Field_RAC_PATRIM2_PATRIMPADACGLITCH(self)
        self.zz_fdict['PATRIMPADACGLITCH'] = self.PATRIMPADACGLITCH
        self.PATRIMNBIAS = RM_Field_RAC_PATRIM2_PATRIMNBIAS(self)
        self.zz_fdict['PATRIMNBIAS'] = self.PATRIMNBIAS
        self.PATRIMNCASC = RM_Field_RAC_PATRIM2_PATRIMNCASC(self)
        self.zz_fdict['PATRIMNCASC'] = self.PATRIMNCASC
        self.PATRIMPBIAS = RM_Field_RAC_PATRIM2_PATRIMPBIAS(self)
        self.zz_fdict['PATRIMPBIAS'] = self.PATRIMPBIAS
        self.PATRIMPCASC = RM_Field_RAC_PATRIM2_PATRIMPCASC(self)
        self.zz_fdict['PATRIMPCASC'] = self.PATRIMPCASC
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PACTRL, self).__init__(rmio, label,
            0xa8020000, 0x108,
            'PACTRL', 'RAC.PACTRL', 'read-write',
            u"",
            0x00000000, 0x00FF07FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PAEN10DBMVMID = RM_Field_RAC_PACTRL_PAEN10DBMVMID(self)
        self.zz_fdict['PAEN10DBMVMID'] = self.PAEN10DBMVMID
        self.PAEN20DBMVMID = RM_Field_RAC_PACTRL_PAEN20DBMVMID(self)
        self.zz_fdict['PAEN20DBMVMID'] = self.PAEN20DBMVMID
        self.PAENCAPATT = RM_Field_RAC_PACTRL_PAENCAPATT(self)
        self.zz_fdict['PAENCAPATT'] = self.PAENCAPATT
        self.PAENLATCHBYPASS = RM_Field_RAC_PACTRL_PAENLATCHBYPASS(self)
        self.zz_fdict['PAENLATCHBYPASS'] = self.PAENLATCHBYPASS
        self.PAENPOWERRAMPINGCLK = RM_Field_RAC_PACTRL_PAENPOWERRAMPINGCLK(self)
        self.zz_fdict['PAENPOWERRAMPINGCLK'] = self.PAENPOWERRAMPINGCLK
        self.PAPULLDOWNVDDPA = RM_Field_RAC_PACTRL_PAPULLDOWNVDDPA(self)
        self.zz_fdict['PAPULLDOWNVDDPA'] = self.PAPULLDOWNVDDPA
        self.PAREGBYPASSPDRVLDO = RM_Field_RAC_PACTRL_PAREGBYPASSPDRVLDO(self)
        self.zz_fdict['PAREGBYPASSPDRVLDO'] = self.PAREGBYPASSPDRVLDO
        self.PAREGBYPASSPREREG = RM_Field_RAC_PACTRL_PAREGBYPASSPREREG(self)
        self.zz_fdict['PAREGBYPASSPREREG'] = self.PAREGBYPASSPREREG
        self.PASELLDOVDDPA = RM_Field_RAC_PACTRL_PASELLDOVDDPA(self)
        self.zz_fdict['PASELLDOVDDPA'] = self.PASELLDOVDDPA
        self.PASELLDOVDDRF = RM_Field_RAC_PACTRL_PASELLDOVDDRF(self)
        self.zz_fdict['PASELLDOVDDRF'] = self.PASELLDOVDDRF
        self.PASLICERST = RM_Field_RAC_PACTRL_PASLICERST(self)
        self.zz_fdict['PASLICERST'] = self.PASLICERST
        self.PAPOWER = RM_Field_RAC_PACTRL_PAPOWER(self)
        self.zz_fdict['PAPOWER'] = self.PAPOWER
        self.PASELSLICE = RM_Field_RAC_PACTRL_PASELSLICE(self)
        self.zz_fdict['PASELSLICE'] = self.PASELSLICE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGADEBUG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGADEBUG, self).__init__(rmio, label,
            0xa8020000, 0x10C,
            'PGADEBUG', 'RAC.PGADEBUG', 'read-write',
            u"",
            0x00000004, 0x00000007,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGACCTRIM = RM_Field_RAC_PGADEBUG_PGACCTRIM(self)
        self.zz_fdict['PGACCTRIM'] = self.PGACCTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGATRIM(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGATRIM, self).__init__(rmio, label,
            0xa8020000, 0x110,
            'PGATRIM', 'RAC.PGATRIM', 'read-write',
            u"",
            0x00000547, 0x000007FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGACTUNE = RM_Field_RAC_PGATRIM_PGACTUNE(self)
        self.zz_fdict['PGACTUNE'] = self.PGACTUNE
        self.PGADISANTILOCK = RM_Field_RAC_PGATRIM_PGADISANTILOCK(self)
        self.zz_fdict['PGADISANTILOCK'] = self.PGADISANTILOCK
        self.PGAVCMOUTTRIM = RM_Field_RAC_PGATRIM_PGAVCMOUTTRIM(self)
        self.zz_fdict['PGAVCMOUTTRIM'] = self.PGAVCMOUTTRIM
        self.PGAVLDOTRIM = RM_Field_RAC_PGATRIM_PGAVLDOTRIM(self)
        self.zz_fdict['PGAVLDOTRIM'] = self.PGAVLDOTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGACAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGACAL, self).__init__(rmio, label,
            0xa8020000, 0x114,
            'PGACAL', 'RAC.PGACAL', 'read-write',
            u"",
            0x20202020, 0x3F3F3F3F,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGAOFFNCALI = RM_Field_RAC_PGACAL_PGAOFFNCALI(self)
        self.zz_fdict['PGAOFFNCALI'] = self.PGAOFFNCALI
        self.PGAOFFNCALQ = RM_Field_RAC_PGACAL_PGAOFFNCALQ(self)
        self.zz_fdict['PGAOFFNCALQ'] = self.PGAOFFNCALQ
        self.PGAOFFPCALI = RM_Field_RAC_PGACAL_PGAOFFPCALI(self)
        self.zz_fdict['PGAOFFPCALI'] = self.PGAOFFPCALI
        self.PGAOFFPCALQ = RM_Field_RAC_PGACAL_PGAOFFPCALQ(self)
        self.zz_fdict['PGAOFFPCALQ'] = self.PGAOFFPCALQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PGACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PGACTRL, self).__init__(rmio, label,
            0xa8020000, 0x118,
            'PGACTRL', 'RAC.PGACTRL', 'read-write',
            u"",
            0x04000000, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PGABWMODE = RM_Field_RAC_PGACTRL_PGABWMODE(self)
        self.zz_fdict['PGABWMODE'] = self.PGABWMODE
        self.PGAENBIAS = RM_Field_RAC_PGACTRL_PGAENBIAS(self)
        self.zz_fdict['PGAENBIAS'] = self.PGAENBIAS
        self.PGAENGHZ = RM_Field_RAC_PGACTRL_PGAENGHZ(self)
        self.zz_fdict['PGAENGHZ'] = self.PGAENGHZ
        self.PGAENHYST = RM_Field_RAC_PGACTRL_PGAENHYST(self)
        self.zz_fdict['PGAENHYST'] = self.PGAENHYST
        self.PGAENLATCHI = RM_Field_RAC_PGACTRL_PGAENLATCHI(self)
        self.zz_fdict['PGAENLATCHI'] = self.PGAENLATCHI
        self.PGAENLATCHQ = RM_Field_RAC_PGACTRL_PGAENLATCHQ(self)
        self.zz_fdict['PGAENLATCHQ'] = self.PGAENLATCHQ
        self.PGAENLDOLOAD = RM_Field_RAC_PGACTRL_PGAENLDOLOAD(self)
        self.zz_fdict['PGAENLDOLOAD'] = self.PGAENLDOLOAD
        self.PGAENOFFD = RM_Field_RAC_PGACTRL_PGAENOFFD(self)
        self.zz_fdict['PGAENOFFD'] = self.PGAENOFFD
        self.PGAENPGAI = RM_Field_RAC_PGACTRL_PGAENPGAI(self)
        self.zz_fdict['PGAENPGAI'] = self.PGAENPGAI
        self.PGAENPGAQ = RM_Field_RAC_PGACTRL_PGAENPGAQ(self)
        self.zz_fdict['PGAENPGAQ'] = self.PGAENPGAQ
        self.PGAENPKD = RM_Field_RAC_PGACTRL_PGAENPKD(self)
        self.zz_fdict['PGAENPKD'] = self.PGAENPKD
        self.PGAENRCMOUT = RM_Field_RAC_PGACTRL_PGAENRCMOUT(self)
        self.zz_fdict['PGAENRCMOUT'] = self.PGAENRCMOUT
        self.PGAENSUBG = RM_Field_RAC_PGACTRL_PGAENSUBG(self)
        self.zz_fdict['PGAENSUBG'] = self.PGAENSUBG
        self.PGAPOWERMODE = RM_Field_RAC_PGACTRL_PGAPOWERMODE(self)
        self.zz_fdict['PGAPOWERMODE'] = self.PGAPOWERMODE
        self.PGATHRPKDLOSEL = RM_Field_RAC_PGACTRL_PGATHRPKDLOSEL(self)
        self.zz_fdict['PGATHRPKDLOSEL'] = self.PGATHRPKDLOSEL
        self.PGATHRPKDHISEL = RM_Field_RAC_PGACTRL_PGATHRPKDHISEL(self)
        self.zz_fdict['PGATHRPKDHISEL'] = self.PGATHRPKDHISEL
        self.LNAMIXRFPKDTHRESHSEL = RM_Field_RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL(self)
        self.zz_fdict['LNAMIXRFPKDTHRESHSEL'] = self.LNAMIXRFPKDTHRESHSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFBIASCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFBIASCAL, self).__init__(rmio, label,
            0xa8020000, 0x11C,
            'RFBIASCAL', 'RAC.RFBIASCAL', 'read-write',
            u"",
            0x30202020, 0x3F3F3F3F,
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
            0xa8020000, 0x120,
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
            0xa8020000, 0x124,
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


class RM_Register_RAC_RFPATHEN1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFPATHEN1, self).__init__(rmio, label,
            0xa8020000, 0x128,
            'RFPATHEN1', 'RAC.RFPATHEN1', 'read-write',
            u"",
            0x00000004, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXEN0DBMPA1 = RM_Field_RAC_RFPATHEN1_LNAMIXEN0DBMPA1(self)
        self.zz_fdict['LNAMIXEN0DBMPA1'] = self.LNAMIXEN0DBMPA1
        self.LNAMIXEN1 = RM_Field_RAC_RFPATHEN1_LNAMIXEN1(self)
        self.zz_fdict['LNAMIXEN1'] = self.LNAMIXEN1
        self.LNAMIXRFATTDCEN1 = RM_Field_RAC_RFPATHEN1_LNAMIXRFATTDCEN1(self)
        self.zz_fdict['LNAMIXRFATTDCEN1'] = self.LNAMIXRFATTDCEN1
        self.LNAMIXRFPKDENRF1 = RM_Field_RAC_RFPATHEN1_LNAMIXRFPKDENRF1(self)
        self.zz_fdict['LNAMIXRFPKDENRF1'] = self.LNAMIXRFPKDENRF1
        self.LNAMIXTRSW1 = RM_Field_RAC_RFPATHEN1_LNAMIXTRSW1(self)
        self.zz_fdict['LNAMIXTRSW1'] = self.LNAMIXTRSW1
        self.PAENANT1 = RM_Field_RAC_RFPATHEN1_PAENANT1(self)
        self.zz_fdict['PAENANT1'] = self.PAENANT1
        self.PAENPA10DBM = RM_Field_RAC_RFPATHEN1_PAENPA10DBM(self)
        self.zz_fdict['PAENPA10DBM'] = self.PAENPA10DBM
        self.PAENPAPREDRV10DBM = RM_Field_RAC_RFPATHEN1_PAENPAPREDRV10DBM(self)
        self.zz_fdict['PAENPAPREDRV10DBM'] = self.PAENPAPREDRV10DBM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFPATHEN2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFPATHEN2, self).__init__(rmio, label,
            0xa8020000, 0x12C,
            'RFPATHEN2', 'RAC.RFPATHEN2', 'read-write',
            u"",
            0x00000004, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LNAMIXEN0DBMPA2 = RM_Field_RAC_RFPATHEN2_LNAMIXEN0DBMPA2(self)
        self.zz_fdict['LNAMIXEN0DBMPA2'] = self.LNAMIXEN0DBMPA2
        self.LNAMIXEN2 = RM_Field_RAC_RFPATHEN2_LNAMIXEN2(self)
        self.zz_fdict['LNAMIXEN2'] = self.LNAMIXEN2
        self.LNAMIXRFATTDCEN2 = RM_Field_RAC_RFPATHEN2_LNAMIXRFATTDCEN2(self)
        self.zz_fdict['LNAMIXRFATTDCEN2'] = self.LNAMIXRFATTDCEN2
        self.LNAMIXRFPKDENRF2 = RM_Field_RAC_RFPATHEN2_LNAMIXRFPKDENRF2(self)
        self.zz_fdict['LNAMIXRFPKDENRF2'] = self.LNAMIXRFPKDENRF2
        self.LNAMIXTRSW2 = RM_Field_RAC_RFPATHEN2_LNAMIXTRSW2(self)
        self.zz_fdict['LNAMIXTRSW2'] = self.LNAMIXTRSW2
        self.PAENANT2 = RM_Field_RAC_RFPATHEN2_PAENANT2(self)
        self.zz_fdict['PAENANT2'] = self.PAENANT2
        self.PAENPA20DBM = RM_Field_RAC_RFPATHEN2_PAENPA20DBM(self)
        self.zz_fdict['PAENPA20DBM'] = self.PAENPA20DBM
        self.PAENPAPREDRV20DBM = RM_Field_RAC_RFPATHEN2_PAENPAPREDRV20DBM(self)
        self.zz_fdict['PAENPAPREDRV20DBM'] = self.PAENPAPREDRV20DBM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RX, self).__init__(rmio, label,
            0xa8020000, 0x130,
            'RX', 'RAC.RX', 'read-write',
            u"",
            0x00000410, 0x00003FFF,
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
        self.LNAMIXLDOLOWCUR = RM_Field_RAC_RX_LNAMIXLDOLOWCUR(self)
        self.zz_fdict['LNAMIXLDOLOWCUR'] = self.LNAMIXLDOLOWCUR
        self.LNAMIXREGLOADEN = RM_Field_RAC_RX_LNAMIXREGLOADEN(self)
        self.zz_fdict['LNAMIXREGLOADEN'] = self.LNAMIXREGLOADEN
        self.PGAENLDO = RM_Field_RAC_RX_PGAENLDO(self)
        self.zz_fdict['PGAENLDO'] = self.PGAENLDO
        self.SYCHPBIASTRIMBUF = RM_Field_RAC_RX_SYCHPBIASTRIMBUF(self)
        self.zz_fdict['SYCHPBIASTRIMBUF'] = self.SYCHPBIASTRIMBUF
        self.SYCHPQNC3EN = RM_Field_RAC_RX_SYCHPQNC3EN(self)
        self.zz_fdict['SYCHPQNC3EN'] = self.SYCHPQNC3EN
        self.SYMMDMODE = RM_Field_RAC_RX_SYMMDMODE(self)
        self.zz_fdict['SYMMDMODE'] = self.SYMMDMODE
        self.SYPFDCHPLPEN = RM_Field_RAC_RX_SYPFDCHPLPEN(self)
        self.zz_fdict['SYPFDCHPLPEN'] = self.SYPFDCHPLPEN
        self.SYPFDFPWEN = RM_Field_RAC_RX_SYPFDFPWEN(self)
        self.zz_fdict['SYPFDFPWEN'] = self.SYPFDFPWEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TX, self).__init__(rmio, label,
            0xa8020000, 0x134,
            'TX', 'RAC.TX', 'read-write',
            u"",
            0x00000000, 0xFF3707FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.PABLEEDDRVREG0DBM = RM_Field_RAC_TX_PABLEEDDRVREG0DBM(self)
        self.zz_fdict['PABLEEDDRVREG0DBM'] = self.PABLEEDDRVREG0DBM
        self.PABLEEDREG0DBM = RM_Field_RAC_TX_PABLEEDREG0DBM(self)
        self.zz_fdict['PABLEEDREG0DBM'] = self.PABLEEDREG0DBM
        self.PAENBIAS0DBM = RM_Field_RAC_TX_PAENBIAS0DBM(self)
        self.zz_fdict['PAENBIAS0DBM'] = self.PAENBIAS0DBM
        self.PAENDRVREG0DBM = RM_Field_RAC_TX_PAENDRVREG0DBM(self)
        self.zz_fdict['PAENDRVREG0DBM'] = self.PAENDRVREG0DBM
        self.PAENDRVREGBIAS0DBM = RM_Field_RAC_TX_PAENDRVREGBIAS0DBM(self)
        self.zz_fdict['PAENDRVREGBIAS0DBM'] = self.PAENDRVREGBIAS0DBM
        self.PAENLO0DBM = RM_Field_RAC_TX_PAENLO0DBM(self)
        self.zz_fdict['PAENLO0DBM'] = self.PAENLO0DBM
        self.PAENREG0DBM = RM_Field_RAC_TX_PAENREG0DBM(self)
        self.zz_fdict['PAENREG0DBM'] = self.PAENREG0DBM
        self.PAENTAPCAP0DBM = RM_Field_RAC_TX_PAENTAPCAP0DBM(self)
        self.zz_fdict['PAENTAPCAP0DBM'] = self.PAENTAPCAP0DBM
        self.ENPATRIMPASLICE0DBM = RM_Field_RAC_TX_ENPATRIMPASLICE0DBM(self)
        self.zz_fdict['ENPATRIMPASLICE0DBM'] = self.ENPATRIMPASLICE0DBM
        self.PAEN10DBMM = RM_Field_RAC_TX_PAEN10DBMM(self)
        self.zz_fdict['PAEN10DBMM'] = self.PAEN10DBMM
        self.PAEN10DBMP = RM_Field_RAC_TX_PAEN10DBMP(self)
        self.zz_fdict['PAEN10DBMP'] = self.PAEN10DBMP
        self.PAEN10DBMPDRV = RM_Field_RAC_TX_PAEN10DBMPDRV(self)
        self.zz_fdict['PAEN10DBMPDRV'] = self.PAEN10DBMPDRV
        self.PAEN20DBM = RM_Field_RAC_TX_PAEN20DBM(self)
        self.zz_fdict['PAEN20DBM'] = self.PAEN20DBM
        self.PAEN20DBMPDRV = RM_Field_RAC_TX_PAEN20DBMPDRV(self)
        self.zz_fdict['PAEN20DBMPDRV'] = self.PAEN20DBMPDRV
        self.PAENBLEEDPDRVLDO = RM_Field_RAC_TX_PAENBLEEDPDRVLDO(self)
        self.zz_fdict['PAENBLEEDPDRVLDO'] = self.PAENBLEEDPDRVLDO
        self.PAENBLEEDPREREG = RM_Field_RAC_TX_PAENBLEEDPREREG(self)
        self.zz_fdict['PAENBLEEDPREREG'] = self.PAENBLEEDPREREG
        self.PAENLDOHVPDRVLDO = RM_Field_RAC_TX_PAENLDOHVPDRVLDO(self)
        self.zz_fdict['PAENLDOHVPDRVLDO'] = self.PAENLDOHVPDRVLDO
        self.PAENLDOHVPREREG = RM_Field_RAC_TX_PAENLDOHVPREREG(self)
        self.zz_fdict['PAENLDOHVPREREG'] = self.PAENLDOHVPREREG
        self.PAENPAOUT = RM_Field_RAC_TX_PAENPAOUT(self)
        self.zz_fdict['PAENPAOUT'] = self.PAENPAOUT
        self.ENXOSQBUFFILT = RM_Field_RAC_TX_ENXOSQBUFFILT(self)
        self.zz_fdict['ENXOSQBUFFILT'] = self.ENXOSQBUFFILT
        self.ENPAPOWER = RM_Field_RAC_TX_ENPAPOWER(self)
        self.zz_fdict['ENPAPOWER'] = self.ENPAPOWER
        self.ENPASELSLICE = RM_Field_RAC_TX_ENPASELSLICE(self)
        self.zz_fdict['ENPASELSLICE'] = self.ENPASELSLICE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYDEBUG(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYDEBUG, self).__init__(rmio, label,
            0xa8020000, 0x138,
            'SYDEBUG', 'RAC.SYDEBUG', 'read-write',
            u"",
            0x00000020, 0x0000017F,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYCHPSWITCHEDBIASEN = RM_Field_RAC_SYDEBUG_SYCHPSWITCHEDBIASEN(self)
        self.zz_fdict['SYCHPSWITCHEDBIASEN'] = self.SYCHPSWITCHEDBIASEN
        self.SYENMMDDREGBIASSTB = RM_Field_RAC_SYDEBUG_SYENMMDDREGBIASSTB(self)
        self.zz_fdict['SYENMMDDREGBIASSTB'] = self.SYENMMDDREGBIASSTB
        self.SYLODIVAUXLODIS = RM_Field_RAC_SYDEBUG_SYLODIVAUXLODIS(self)
        self.zz_fdict['SYLODIVAUXLODIS'] = self.SYLODIVAUXLODIS
        self.SYLODIVCLKSYNCSGEN = RM_Field_RAC_SYDEBUG_SYLODIVCLKSYNCSGEN(self)
        self.zz_fdict['SYLODIVCLKSYNCSGEN'] = self.SYLODIVCLKSYNCSGEN
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
            0xa8020000, 0x13C,
            'SYTRIM0', 'RAC.SYTRIM0', 'read-write',
            u"",
            0x00062E29, 0x003FEFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYCHPBIAS = RM_Field_RAC_SYTRIM0_SYCHPBIAS(self)
        self.zz_fdict['SYCHPBIAS'] = self.SYCHPBIAS
        self.SYCHPCURR = RM_Field_RAC_SYTRIM0_SYCHPCURR(self)
        self.zz_fdict['SYCHPCURR'] = self.SYCHPCURR
        self.SYCHPLEVNSRC = RM_Field_RAC_SYTRIM0_SYCHPLEVNSRC(self)
        self.zz_fdict['SYCHPLEVNSRC'] = self.SYCHPLEVNSRC
        self.SYCHPLEVPSRC = RM_Field_RAC_SYTRIM0_SYCHPLEVPSRC(self)
        self.zz_fdict['SYCHPLEVPSRC'] = self.SYCHPLEVPSRC
        self.SYCHPSRCEN = RM_Field_RAC_SYTRIM0_SYCHPSRCEN(self)
        self.zz_fdict['SYCHPSRCEN'] = self.SYCHPSRCEN
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
            0xa8020000, 0x140,
            'SYTRIM1', 'RAC.SYTRIM1', 'read-write',
            u"",
            0x00003FD0, 0x0001FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYLODIVLDOTRIMCORE = RM_Field_RAC_SYTRIM1_SYLODIVLDOTRIMCORE(self)
        self.zz_fdict['SYLODIVLDOTRIMCORE'] = self.SYLODIVLDOTRIMCORE
        self.SYLODIVLDOTRIMNDIO = RM_Field_RAC_SYTRIM1_SYLODIVLDOTRIMNDIO(self)
        self.zz_fdict['SYLODIVLDOTRIMNDIO'] = self.SYLODIVLDOTRIMNDIO
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
            0xa8020000, 0x144,
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
            0xa8020000, 0x148,
            'SYEN', 'RAC.SYEN', 'read-write',
            u"",
            0x00000000, 0x00007FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYCHPEN = RM_Field_RAC_SYEN_SYCHPEN(self)
        self.zz_fdict['SYCHPEN'] = self.SYCHPEN
        self.SYCHPLPEN = RM_Field_RAC_SYEN_SYCHPLPEN(self)
        self.zz_fdict['SYCHPLPEN'] = self.SYCHPLPEN
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
        self.SYLODIVEN = RM_Field_RAC_SYEN_SYLODIVEN(self)
        self.zz_fdict['SYLODIVEN'] = self.SYLODIVEN
        self.SYLODIVLDOBIASEN = RM_Field_RAC_SYEN_SYLODIVLDOBIASEN(self)
        self.zz_fdict['SYLODIVLDOBIASEN'] = self.SYLODIVLDOBIASEN
        self.SYLODIVLDOEN = RM_Field_RAC_SYEN_SYLODIVLDOEN(self)
        self.zz_fdict['SYLODIVLDOEN'] = self.SYLODIVLDOEN
        self.SYSTARTCHPREG = RM_Field_RAC_SYEN_SYSTARTCHPREG(self)
        self.zz_fdict['SYSTARTCHPREG'] = self.SYSTARTCHPREG
        self.SYSTARTMMDREG = RM_Field_RAC_SYEN_SYSTARTMMDREG(self)
        self.zz_fdict['SYSTARTMMDREG'] = self.SYSTARTMMDREG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYLOEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYLOEN, self).__init__(rmio, label,
            0xa8020000, 0x14C,
            'SYLOEN', 'RAC.SYLOEN', 'read-write',
            u"",
            0x00000000, 0x00001FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYLODIVRLOADCCLK2G4EN = RM_Field_RAC_SYLOEN_SYLODIVRLOADCCLK2G4EN(self)
        self.zz_fdict['SYLODIVRLOADCCLK2G4EN'] = self.SYLODIVRLOADCCLK2G4EN
        self.SYLODIVRLO12G4EN = RM_Field_RAC_SYLOEN_SYLODIVRLO12G4EN(self)
        self.zz_fdict['SYLODIVRLO12G4EN'] = self.SYLODIVRLO12G4EN
        self.SYLODIVRLO1SGEN = RM_Field_RAC_SYLOEN_SYLODIVRLO1SGEN(self)
        self.zz_fdict['SYLODIVRLO1SGEN'] = self.SYLODIVRLO1SGEN
        self.SYLODIVRLO22G4EN = RM_Field_RAC_SYLOEN_SYLODIVRLO22G4EN(self)
        self.zz_fdict['SYLODIVRLO22G4EN'] = self.SYLODIVRLO22G4EN
        self.SYLODIVRLO2SGEN = RM_Field_RAC_SYLOEN_SYLODIVRLO2SGEN(self)
        self.zz_fdict['SYLODIVRLO2SGEN'] = self.SYLODIVRLO2SGEN
        self.SYLODIVTLO0DBM2G4AUXEN = RM_Field_RAC_SYLOEN_SYLODIVTLO0DBM2G4AUXEN(self)
        self.zz_fdict['SYLODIVTLO0DBM2G4AUXEN'] = self.SYLODIVTLO0DBM2G4AUXEN
        self.SYLODIVTLO0DBM2G4EN = RM_Field_RAC_SYLOEN_SYLODIVTLO0DBM2G4EN(self)
        self.zz_fdict['SYLODIVTLO0DBM2G4EN'] = self.SYLODIVTLO0DBM2G4EN
        self.SYLODIVTLO0DBMSGAUXEN = RM_Field_RAC_SYLOEN_SYLODIVTLO0DBMSGAUXEN(self)
        self.zz_fdict['SYLODIVTLO0DBMSGAUXEN'] = self.SYLODIVTLO0DBMSGAUXEN
        self.SYLODIVTLO0DBMSGEN = RM_Field_RAC_SYLOEN_SYLODIVTLO0DBMSGEN(self)
        self.zz_fdict['SYLODIVTLO0DBMSGEN'] = self.SYLODIVTLO0DBMSGEN
        self.SYLODIVTLO20DBM2G4AUXEN = RM_Field_RAC_SYLOEN_SYLODIVTLO20DBM2G4AUXEN(self)
        self.zz_fdict['SYLODIVTLO20DBM2G4AUXEN'] = self.SYLODIVTLO20DBM2G4AUXEN
        self.SYLODIVTLO20DBM2G4EN = RM_Field_RAC_SYLOEN_SYLODIVTLO20DBM2G4EN(self)
        self.zz_fdict['SYLODIVTLO20DBM2G4EN'] = self.SYLODIVTLO20DBM2G4EN
        self.SYLODIVTLO20DBMSGAUXEN = RM_Field_RAC_SYLOEN_SYLODIVTLO20DBMSGAUXEN(self)
        self.zz_fdict['SYLODIVTLO20DBMSGAUXEN'] = self.SYLODIVTLO20DBMSGAUXEN
        self.SYLODIVTLO20DBMSGEN = RM_Field_RAC_SYLOEN_SYLODIVTLO20DBMSGEN(self)
        self.zz_fdict['SYLODIVTLO20DBMSGEN'] = self.SYLODIVTLO20DBMSGEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYMMDCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYMMDCTRL, self).__init__(rmio, label,
            0xa8020000, 0x158,
            'SYMMDCTRL', 'RAC.SYMMDCTRL', 'read-write',
            u"",
            0x00000000, 0x00000007,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYMMDENRSDIG = RM_Field_RAC_SYMMDCTRL_SYMMDENRSDIG(self)
        self.zz_fdict['SYMMDENRSDIG'] = self.SYMMDENRSDIG
        self.SYMMDDIVRSDIG = RM_Field_RAC_SYMMDCTRL_SYMMDDIVRSDIG(self)
        self.zz_fdict['SYMMDDIVRSDIG'] = self.SYMMDDIVRSDIG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_DIGCLKRETIMECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_DIGCLKRETIMECTRL, self).__init__(rmio, label,
            0xa8020000, 0x15C,
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
            0xa8020000, 0x160,
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
            0xa8020000, 0x164,
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
            0xa8020000, 0x168,
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


class RM_Register_RAC_XOSQBUFFILT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_XOSQBUFFILT, self).__init__(rmio, label,
            0xa8020000, 0x16C,
            'XOSQBUFFILT', 'RAC.XOSQBUFFILT', 'read-write',
            u"",
            0x00000000, 0x00000003,
            0x00001000, 0x00002000,
            0x00003000)

        self.XOSQBUFFILT = RM_Field_RAC_XOSQBUFFILT_XOSQBUFFILT(self)
        self.zz_fdict['XOSQBUFFILT'] = self.XOSQBUFFILT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SPARE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SPARE, self).__init__(rmio, label,
            0xa8020000, 0x170,
            'SPARE', 'RAC.SPARE', 'read-write',
            u"",
            0x00000000, 0x0007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.AUXADCSPARE = RM_Field_RAC_SPARE_AUXADCSPARE(self)
        self.zz_fdict['AUXADCSPARE'] = self.AUXADCSPARE
        self.CLKMULTSPARE = RM_Field_RAC_SPARE_CLKMULTSPARE(self)
        self.zz_fdict['CLKMULTSPARE'] = self.CLKMULTSPARE
        self.IFADCSPARE = RM_Field_RAC_SPARE_IFADCSPARE(self)
        self.zz_fdict['IFADCSPARE'] = self.IFADCSPARE
        self.PATRIMSPARE = RM_Field_RAC_SPARE_PATRIMSPARE(self)
        self.zz_fdict['PATRIMSPARE'] = self.PATRIMSPARE
        self.PGASPACER0 = RM_Field_RAC_SPARE_PGASPACER0(self)
        self.zz_fdict['PGASPACER0'] = self.PGASPACER0
        self.SYSPARE = RM_Field_RAC_SPARE_SYSPARE(self)
        self.zz_fdict['SYSPARE'] = self.SYSPARE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_ANTDIV(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_ANTDIV, self).__init__(rmio, label,
            0xa8020000, 0x174,
            'ANTDIV', 'RAC.ANTDIV', 'read-write',
            u"",
            0x00000000, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_RAC_ANTDIV_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.STATUS = RM_Field_RAC_ANTDIV_STATUS(self)
        self.zz_fdict['STATUS'] = self.STATUS
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


class RM_Register_RAC_RFLOCK0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFLOCK0, self).__init__(rmio, label,
            0xa8020000, 0x7F8,
            'RFLOCK0', 'RAC.RFLOCK0', 'read-write',
            u"",
            0x80000000, 0xFF1F01FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNTHLODIVFREQCTRL = RM_Field_RAC_RFLOCK0_SYNTHLODIVFREQCTRL(self)
        self.zz_fdict['SYNTHLODIVFREQCTRL'] = self.SYNTHLODIVFREQCTRL
        self.RACIFPGAEN = RM_Field_RAC_RFLOCK0_RACIFPGAEN(self)
        self.zz_fdict['RACIFPGAEN'] = self.RACIFPGAEN
        self.RACPAEN10DBMM = RM_Field_RAC_RFLOCK0_RACPAEN10DBMM(self)
        self.zz_fdict['RACPAEN10DBMM'] = self.RACPAEN10DBMM
        self.RACPAEN10DBMP = RM_Field_RAC_RFLOCK0_RACPAEN10DBMP(self)
        self.zz_fdict['RACPAEN10DBMP'] = self.RACPAEN10DBMP
        self.RACPAEN20DBM = RM_Field_RAC_RFLOCK0_RACPAEN20DBM(self)
        self.zz_fdict['RACPAEN20DBM'] = self.RACPAEN20DBM
        self.RACSGPAEN = RM_Field_RAC_RFLOCK0_RACSGPAEN(self)
        self.zz_fdict['RACSGPAEN'] = self.RACSGPAEN
        self.FRCCONVMODE = RM_Field_RAC_RFLOCK0_FRCCONVMODE(self)
        self.zz_fdict['FRCCONVMODE'] = self.FRCCONVMODE
        self.FRCPAUSING = RM_Field_RAC_RFLOCK0_FRCPAUSING(self)
        self.zz_fdict['FRCPAUSING'] = self.FRCPAUSING
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
            0x00FFFFFF, 0x00FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.RACPASLICE0DBM = RM_Field_RAC_RFLOCK1_RACPASLICE0DBM(self)
        self.zz_fdict['RACPASLICE0DBM'] = self.RACPASLICE0DBM
        self.RACDRVSLICE0DBM = RM_Field_RAC_RFLOCK1_RACDRVSLICE0DBM(self)
        self.zz_fdict['RACDRVSLICE0DBM'] = self.RACDRVSLICE0DBM
        self.RACPASTRIPE10DBM = RM_Field_RAC_RFLOCK1_RACPASTRIPE10DBM(self)
        self.zz_fdict['RACPASTRIPE10DBM'] = self.RACPASTRIPE10DBM
        self.RACPASLICE10DBM = RM_Field_RAC_RFLOCK1_RACPASLICE10DBM(self)
        self.zz_fdict['RACPASLICE10DBM'] = self.RACPASLICE10DBM
        self.RACPASTRIPE20DBM = RM_Field_RAC_RFLOCK1_RACPASTRIPE20DBM(self)
        self.zz_fdict['RACPASTRIPE20DBM'] = self.RACPASTRIPE20DBM
        self.RACPASLICE20DBM = RM_Field_RAC_RFLOCK1_RACPASLICE20DBM(self)
        self.zz_fdict['RACPASLICE20DBM'] = self.RACPASLICE20DBM
        self.__dict__['zz_frozen'] = True


