
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . RAC_field import *


class RM_Register_RAC_RXENSRCEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RXENSRCEN, self).__init__(rmio, label,
            0x40084000, 0x000,
            'RXENSRCEN', 'RAC.RXENSRCEN', 'read-write',
            u"",
            0x00000000, 0x0013FFFF)

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
        self.PRSRXENSEL = RM_Field_RAC_RXENSRCEN_PRSRXENSEL(self)
        self.zz_fdict['PRSRXENSEL'] = self.PRSRXENSEL
        self.PRSRXENEM2WU = RM_Field_RAC_RXENSRCEN_PRSRXENEM2WU(self)
        self.zz_fdict['PRSRXENEM2WU'] = self.PRSRXENEM2WU
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STATUS, self).__init__(rmio, label,
            0x40084000, 0x004,
            'STATUS', 'RAC.STATUS', 'read-only',
            u"",
            0x00000000, 0xEF38FFFF)

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
            0x40084000, 0x008,
            'CMD', 'RAC.CMD', 'write-only',
            u"",
            0x00000000, 0xC000F1FF)

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
            0x40084000, 0x00C,
            'CTRL', 'RAC.CTRL', 'read-write',
            u"",
            0x00000000, 0x7BDF7FEF)

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
        self.PRSRXDISSEL = RM_Field_RAC_CTRL_PRSRXDISSEL(self)
        self.zz_fdict['PRSRXDISSEL'] = self.PRSRXDISSEL
        self.PRSFORCETX = RM_Field_RAC_CTRL_PRSFORCETX(self)
        self.zz_fdict['PRSFORCETX'] = self.PRSFORCETX
        self.PRSFORCETXSEL = RM_Field_RAC_CTRL_PRSFORCETXSEL(self)
        self.zz_fdict['PRSFORCETXSEL'] = self.PRSFORCETXSEL
        self.PRSTXENSEL = RM_Field_RAC_CTRL_PRSTXENSEL(self)
        self.zz_fdict['PRSTXENSEL'] = self.PRSTXENSEL
        self.PRSCLRSEL = RM_Field_RAC_CTRL_PRSCLRSEL(self)
        self.zz_fdict['PRSCLRSEL'] = self.PRSCLRSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_FORCESTATE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_FORCESTATE, self).__init__(rmio, label,
            0x40084000, 0x010,
            'FORCESTATE', 'RAC.FORCESTATE', 'read-write',
            u"",
            0x00000000, 0x0000000F)

        self.FORCESTATE = RM_Field_RAC_FORCESTATE_FORCESTATE(self)
        self.zz_fdict['FORCESTATE'] = self.FORCESTATE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IF, self).__init__(rmio, label,
            0x40084000, 0x014,
            'IF', 'RAC.IF', 'read-only',
            u"",
            0x00000000, 0x0EFF0007)

        self.STATECHANGE = RM_Field_RAC_IF_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IF_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.BUSERROR = RM_Field_RAC_IF_BUSERROR(self)
        self.zz_fdict['BUSERROR'] = self.BUSERROR
        self.SEQ = RM_Field_RAC_IF_SEQ(self)
        self.zz_fdict['SEQ'] = self.SEQ
        self.PAVHIGH = RM_Field_RAC_IF_PAVHIGH(self)
        self.zz_fdict['PAVHIGH'] = self.PAVHIGH
        self.PAVLOW = RM_Field_RAC_IF_PAVLOW(self)
        self.zz_fdict['PAVLOW'] = self.PAVLOW
        self.PABATHIGH = RM_Field_RAC_IF_PABATHIGH(self)
        self.zz_fdict['PABATHIGH'] = self.PABATHIGH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFS, self).__init__(rmio, label,
            0x40084000, 0x018,
            'IFS', 'RAC.IFS', 'write-only',
            u"",
            0x00000000, 0x0EFF0007)

        self.STATECHANGE = RM_Field_RAC_IFS_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IFS_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.BUSERROR = RM_Field_RAC_IFS_BUSERROR(self)
        self.zz_fdict['BUSERROR'] = self.BUSERROR
        self.SEQ = RM_Field_RAC_IFS_SEQ(self)
        self.zz_fdict['SEQ'] = self.SEQ
        self.PAVHIGH = RM_Field_RAC_IFS_PAVHIGH(self)
        self.zz_fdict['PAVHIGH'] = self.PAVHIGH
        self.PAVLOW = RM_Field_RAC_IFS_PAVLOW(self)
        self.zz_fdict['PAVLOW'] = self.PAVLOW
        self.PABATHIGH = RM_Field_RAC_IFS_PABATHIGH(self)
        self.zz_fdict['PABATHIGH'] = self.PABATHIGH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFC, self).__init__(rmio, label,
            0x40084000, 0x01C,
            'IFC', 'RAC.IFC', 'write-only',
            u"",
            0x00000000, 0x0EFF0007)

        self.STATECHANGE = RM_Field_RAC_IFC_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IFC_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.BUSERROR = RM_Field_RAC_IFC_BUSERROR(self)
        self.zz_fdict['BUSERROR'] = self.BUSERROR
        self.SEQ = RM_Field_RAC_IFC_SEQ(self)
        self.zz_fdict['SEQ'] = self.SEQ
        self.PAVHIGH = RM_Field_RAC_IFC_PAVHIGH(self)
        self.zz_fdict['PAVHIGH'] = self.PAVHIGH
        self.PAVLOW = RM_Field_RAC_IFC_PAVLOW(self)
        self.zz_fdict['PAVLOW'] = self.PAVLOW
        self.PABATHIGH = RM_Field_RAC_IFC_PABATHIGH(self)
        self.zz_fdict['PABATHIGH'] = self.PABATHIGH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IEN, self).__init__(rmio, label,
            0x40084000, 0x020,
            'IEN', 'RAC.IEN', 'read-write',
            u"",
            0x00000000, 0x0EFF0007)

        self.STATECHANGE = RM_Field_RAC_IEN_STATECHANGE(self)
        self.zz_fdict['STATECHANGE'] = self.STATECHANGE
        self.STIMCMPEV = RM_Field_RAC_IEN_STIMCMPEV(self)
        self.zz_fdict['STIMCMPEV'] = self.STIMCMPEV
        self.BUSERROR = RM_Field_RAC_IEN_BUSERROR(self)
        self.zz_fdict['BUSERROR'] = self.BUSERROR
        self.SEQ = RM_Field_RAC_IEN_SEQ(self)
        self.zz_fdict['SEQ'] = self.SEQ
        self.PAVHIGH = RM_Field_RAC_IEN_PAVHIGH(self)
        self.zz_fdict['PAVHIGH'] = self.PAVHIGH
        self.PAVLOW = RM_Field_RAC_IEN_PAVLOW(self)
        self.zz_fdict['PAVLOW'] = self.PAVLOW
        self.PABATHIGH = RM_Field_RAC_IEN_PABATHIGH(self)
        self.zz_fdict['PABATHIGH'] = self.PABATHIGH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LVDSCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LVDSCTRL, self).__init__(rmio, label,
            0x40084000, 0x024,
            'LVDSCTRL', 'RAC.LVDSCTRL', 'read-write',
            u"",
            0x0000000C, 0x000000FD)

        self.LVDSEN = RM_Field_RAC_LVDSCTRL_LVDSEN(self)
        self.zz_fdict['LVDSEN'] = self.LVDSEN
        self.LVDSCURR = RM_Field_RAC_LVDSCTRL_LVDSCURR(self)
        self.zz_fdict['LVDSCURR'] = self.LVDSCURR
        self.LVDSTESTMODE = RM_Field_RAC_LVDSCTRL_LVDSTESTMODE(self)
        self.zz_fdict['LVDSTESTMODE'] = self.LVDSTESTMODE
        self.LVDSTESTDATA = RM_Field_RAC_LVDSCTRL_LVDSTESTDATA(self)
        self.zz_fdict['LVDSTESTDATA'] = self.LVDSTESTDATA
        self.LVDSCMCONFIG = RM_Field_RAC_LVDSCTRL_LVDSCMCONFIG(self)
        self.zz_fdict['LVDSCMCONFIG'] = self.LVDSCMCONFIG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LVDSIDLESEQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LVDSIDLESEQ, self).__init__(rmio, label,
            0x40084000, 0x028,
            'LVDSIDLESEQ', 'RAC.LVDSIDLESEQ', 'read-write',
            u"",
            0x000000BC, 0x000000FF)

        self.LVDSIDLESEQ = RM_Field_RAC_LVDSIDLESEQ_LVDSIDLESEQ(self)
        self.zz_fdict['LVDSIDLESEQ'] = self.LVDSIDLESEQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LVDSROUTE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LVDSROUTE, self).__init__(rmio, label,
            0x40084000, 0x02C,
            'LVDSROUTE', 'RAC.LVDSROUTE', 'read-write',
            u"",
            0x00000000, 0x00000101)

        self.LVDSPEN = RM_Field_RAC_LVDSROUTE_LVDSPEN(self)
        self.zz_fdict['LVDSPEN'] = self.LVDSPEN
        self.LVDSLOCATION = RM_Field_RAC_LVDSROUTE_LVDSLOCATION(self)
        self.zz_fdict['LVDSLOCATION'] = self.LVDSLOCATION
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_HFXORETIMECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_HFXORETIMECTRL, self).__init__(rmio, label,
            0x40084000, 0x030,
            'HFXORETIMECTRL', 'RAC.HFXORETIMECTRL', 'read-write',
            u"",
            0x00000000, 0x00000777)

        self.EN = RM_Field_RAC_HFXORETIMECTRL_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.DIS = RM_Field_RAC_HFXORETIMECTRL_DIS(self)
        self.zz_fdict['DIS'] = self.DIS
        self.RESET = RM_Field_RAC_HFXORETIMECTRL_RESET(self)
        self.zz_fdict['RESET'] = self.RESET
        self.LIMITH = RM_Field_RAC_HFXORETIMECTRL_LIMITH(self)
        self.zz_fdict['LIMITH'] = self.LIMITH
        self.LIMITL = RM_Field_RAC_HFXORETIMECTRL_LIMITL(self)
        self.zz_fdict['LIMITL'] = self.LIMITL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_HFXORETIMESTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_HFXORETIMESTATUS, self).__init__(rmio, label,
            0x40084000, 0x034,
            'HFXORETIMESTATUS', 'RAC.HFXORETIMESTATUS', 'read-only',
            u"",
            0x00000000, 0x00000003)

        self.CLKSEL = RM_Field_RAC_HFXORETIMESTATUS_CLKSEL(self)
        self.zz_fdict['CLKSEL'] = self.CLKSEL
        self.RERESETN = RM_Field_RAC_HFXORETIMESTATUS_RERESETN(self)
        self.zz_fdict['RERESETN'] = self.RERESETN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_TESTCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_TESTCTRL, self).__init__(rmio, label,
            0x40084000, 0x038,
            'TESTCTRL', 'RAC.TESTCTRL', 'read-write',
            u"",
            0x00000000, 0x0000001F)

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
            0x40084000, 0x03C,
            'SEQSTATUS', 'RAC.SEQSTATUS', 'read-only',
            u"",
            0x00000010, 0x000005FF)

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
            0x40084000, 0x040,
            'SEQCMD', 'RAC.SEQCMD', 'write-only',
            u"",
            0x00000000, 0x000000FF)

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
            0x40084000, 0x044,
            'BREAKPOINT', 'RAC.BREAKPOINT', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.BKPADDR = RM_Field_RAC_BREAKPOINT_BKPADDR(self)
        self.zz_fdict['BKPADDR'] = self.BKPADDR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R0, self).__init__(rmio, label,
            0x40084000, 0x048,
            'R0', 'RAC.R0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R0 = RM_Field_RAC_R0_R0(self)
        self.zz_fdict['R0'] = self.R0
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R1, self).__init__(rmio, label,
            0x40084000, 0x04C,
            'R1', 'RAC.R1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R1 = RM_Field_RAC_R1_R1(self)
        self.zz_fdict['R1'] = self.R1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R2, self).__init__(rmio, label,
            0x40084000, 0x050,
            'R2', 'RAC.R2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R2 = RM_Field_RAC_R2_R2(self)
        self.zz_fdict['R2'] = self.R2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R3, self).__init__(rmio, label,
            0x40084000, 0x054,
            'R3', 'RAC.R3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R3 = RM_Field_RAC_R3_R3(self)
        self.zz_fdict['R3'] = self.R3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R4(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R4, self).__init__(rmio, label,
            0x40084000, 0x058,
            'R4', 'RAC.R4', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R4 = RM_Field_RAC_R4_R4(self)
        self.zz_fdict['R4'] = self.R4
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R5(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R5, self).__init__(rmio, label,
            0x40084000, 0x05C,
            'R5', 'RAC.R5', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R5 = RM_Field_RAC_R5_R5(self)
        self.zz_fdict['R5'] = self.R5
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R6(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R6, self).__init__(rmio, label,
            0x40084000, 0x060,
            'R6', 'RAC.R6', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R6 = RM_Field_RAC_R6_R6(self)
        self.zz_fdict['R6'] = self.R6
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_R7(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_R7, self).__init__(rmio, label,
            0x40084000, 0x064,
            'R7', 'RAC.R7', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.R7 = RM_Field_RAC_R7_R7(self)
        self.zz_fdict['R7'] = self.R7
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_WAITMASK(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_WAITMASK, self).__init__(rmio, label,
            0x40084000, 0x06C,
            'WAITMASK', 'RAC.WAITMASK', 'read-only',
            u"",
            0x00000000, 0x000001FF)

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
        self.HFXORDY = RM_Field_RAC_WAITMASK_HFXORDY(self)
        self.zz_fdict['HFXORDY'] = self.HFXORDY
        self.FRCPAUSED = RM_Field_RAC_WAITMASK_FRCPAUSED(self)
        self.zz_fdict['FRCPAUSED'] = self.FRCPAUSED
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_WAITSNSH(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_WAITSNSH, self).__init__(rmio, label,
            0x40084000, 0x070,
            'WAITSNSH', 'RAC.WAITSNSH', 'read-only',
            u"",
            0x00000000, 0x000003FF)

        self.WAITSNSH = RM_Field_RAC_WAITSNSH_WAITSNSH(self)
        self.zz_fdict['WAITSNSH'] = self.WAITSNSH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STIMER(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STIMER, self).__init__(rmio, label,
            0x40084000, 0x074,
            'STIMER', 'RAC.STIMER', 'read-only',
            u"",
            0x00000000, 0x0000FFFF)

        self.STIMER = RM_Field_RAC_STIMER_STIMER(self)
        self.zz_fdict['STIMER'] = self.STIMER
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STIMERCOMP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STIMERCOMP, self).__init__(rmio, label,
            0x40084000, 0x078,
            'STIMERCOMP', 'RAC.STIMERCOMP', 'read-write',
            u"",
            0x00000000, 0x0000FFFF)

        self.STIMERCOMP = RM_Field_RAC_STIMERCOMP_STIMERCOMP(self)
        self.zz_fdict['STIMERCOMP'] = self.STIMERCOMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_VECTADDR(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_VECTADDR, self).__init__(rmio, label,
            0x40084000, 0x07C,
            'VECTADDR', 'RAC.VECTADDR', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.VECTADDR = RM_Field_RAC_VECTADDR_VECTADDR(self)
        self.zz_fdict['VECTADDR'] = self.VECTADDR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SEQCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SEQCTRL, self).__init__(rmio, label,
            0x40084000, 0x080,
            'SEQCTRL', 'RAC.SEQCTRL', 'read-write',
            u"",
            0x00000000, 0x00001CF7)

        self.COMPACT = RM_Field_RAC_SEQCTRL_COMPACT(self)
        self.zz_fdict['COMPACT'] = self.COMPACT
        self.COMPINVALMODE = RM_Field_RAC_SEQCTRL_COMPINVALMODE(self)
        self.zz_fdict['COMPINVALMODE'] = self.COMPINVALMODE
        self.PRSSEL = RM_Field_RAC_SEQCTRL_PRSSEL(self)
        self.zz_fdict['PRSSEL'] = self.PRSSEL
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
            0x40084000, 0x084,
            'PRESC', 'RAC.PRESC', 'read-write',
            u"",
            0x00000007, 0x0000007F)

        self.STIMER = RM_Field_RAC_PRESC_STIMER(self)
        self.zz_fdict['STIMER'] = self.STIMER
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR0, self).__init__(rmio, label,
            0x40084000, 0x088,
            'SR0', 'RAC.SR0', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.SR0 = RM_Field_RAC_SR0_SR0(self)
        self.zz_fdict['SR0'] = self.SR0
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR1, self).__init__(rmio, label,
            0x40084000, 0x08C,
            'SR1', 'RAC.SR1', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.SR1 = RM_Field_RAC_SR1_SR1(self)
        self.zz_fdict['SR1'] = self.SR1
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR2, self).__init__(rmio, label,
            0x40084000, 0x090,
            'SR2', 'RAC.SR2', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.SR2 = RM_Field_RAC_SR2_SR2(self)
        self.zz_fdict['SR2'] = self.SR2
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SR3(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SR3, self).__init__(rmio, label,
            0x40084000, 0x094,
            'SR3', 'RAC.SR3', 'read-write',
            u"",
            0x00000000, 0xFFFFFFFF)

        self.SR3 = RM_Field_RAC_SR3_SR3(self)
        self.zz_fdict['SR3'] = self.SR3
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHENCTRL, self).__init__(rmio, label,
            0x40084000, 0x098,
            'SYNTHENCTRL', 'RAC.SYNTHENCTRL', 'read-write',
            u"",
            0x00000000, 0x0010EFFB)

        self.VCOEN = RM_Field_RAC_SYNTHENCTRL_VCOEN(self)
        self.zz_fdict['VCOEN'] = self.VCOEN
        self.VCOSTARTUP = RM_Field_RAC_SYNTHENCTRL_VCOSTARTUP(self)
        self.zz_fdict['VCOSTARTUP'] = self.VCOSTARTUP
        self.LODIVEN = RM_Field_RAC_SYNTHENCTRL_LODIVEN(self)
        self.zz_fdict['LODIVEN'] = self.LODIVEN
        self.CHPEN = RM_Field_RAC_SYNTHENCTRL_CHPEN(self)
        self.zz_fdict['CHPEN'] = self.CHPEN
        self.LPFEN = RM_Field_RAC_SYNTHENCTRL_LPFEN(self)
        self.zz_fdict['LPFEN'] = self.LPFEN
        self.LPFQUICKSTART = RM_Field_RAC_SYNTHENCTRL_LPFQUICKSTART(self)
        self.zz_fdict['LPFQUICKSTART'] = self.LPFQUICKSTART
        self.VCBUFEN = RM_Field_RAC_SYNTHENCTRL_VCBUFEN(self)
        self.zz_fdict['VCBUFEN'] = self.VCBUFEN
        self.SYNTHCLKEN = RM_Field_RAC_SYNTHENCTRL_SYNTHCLKEN(self)
        self.zz_fdict['SYNTHCLKEN'] = self.SYNTHCLKEN
        self.SYNTHSTARTREQ = RM_Field_RAC_SYNTHENCTRL_SYNTHSTARTREQ(self)
        self.zz_fdict['SYNTHSTARTREQ'] = self.SYNTHSTARTREQ
        self.CHPLDOEN = RM_Field_RAC_SYNTHENCTRL_CHPLDOEN(self)
        self.zz_fdict['CHPLDOEN'] = self.CHPLDOEN
        self.LODIVSYNCCLKEN = RM_Field_RAC_SYNTHENCTRL_LODIVSYNCCLKEN(self)
        self.zz_fdict['LODIVSYNCCLKEN'] = self.LODIVSYNCCLKEN
        self.MMDLDOEN = RM_Field_RAC_SYNTHENCTRL_MMDLDOEN(self)
        self.zz_fdict['MMDLDOEN'] = self.MMDLDOEN
        self.VCOLDOEN = RM_Field_RAC_SYNTHENCTRL_VCOLDOEN(self)
        self.zz_fdict['VCOLDOEN'] = self.VCOLDOEN
        self.VCODIVEN = RM_Field_RAC_SYNTHENCTRL_VCODIVEN(self)
        self.zz_fdict['VCODIVEN'] = self.VCODIVEN
        self.LPFBWSEL = RM_Field_RAC_SYNTHENCTRL_LPFBWSEL(self)
        self.zz_fdict['LPFBWSEL'] = self.LPFBWSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHREGCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHREGCTRL, self).__init__(rmio, label,
            0x40084000, 0x09C,
            'SYNTHREGCTRL', 'RAC.SYNTHREGCTRL', 'read-write',
            u"",
            0x03636D80, 0x07FFFFE0)

        self.MMDLDOAMPBWRED = RM_Field_RAC_SYNTHREGCTRL_MMDLDOAMPBWRED(self)
        self.zz_fdict['MMDLDOAMPBWRED'] = self.MMDLDOAMPBWRED
        self.MMDLDOAMPCURR = RM_Field_RAC_SYNTHREGCTRL_MMDLDOAMPCURR(self)
        self.zz_fdict['MMDLDOAMPCURR'] = self.MMDLDOAMPCURR
        self.MMDLDOVREFTRIM = RM_Field_RAC_SYNTHREGCTRL_MMDLDOVREFTRIM(self)
        self.zz_fdict['MMDLDOVREFTRIM'] = self.MMDLDOVREFTRIM
        self.VCOLDOAMPCURR = RM_Field_RAC_SYNTHREGCTRL_VCOLDOAMPCURR(self)
        self.zz_fdict['VCOLDOAMPCURR'] = self.VCOLDOAMPCURR
        self.VCOLDOVREFTRIM = RM_Field_RAC_SYNTHREGCTRL_VCOLDOVREFTRIM(self)
        self.zz_fdict['VCOLDOVREFTRIM'] = self.VCOLDOVREFTRIM
        self.CHPLDOAMPBWRED = RM_Field_RAC_SYNTHREGCTRL_CHPLDOAMPBWRED(self)
        self.zz_fdict['CHPLDOAMPBWRED'] = self.CHPLDOAMPBWRED
        self.CHPLDOAMPCURR = RM_Field_RAC_SYNTHREGCTRL_CHPLDOAMPCURR(self)
        self.zz_fdict['CHPLDOAMPCURR'] = self.CHPLDOAMPCURR
        self.CHPLDOVREFTRIM = RM_Field_RAC_SYNTHREGCTRL_CHPLDOVREFTRIM(self)
        self.zz_fdict['CHPLDOVREFTRIM'] = self.CHPLDOVREFTRIM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_VCOCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_VCOCTRL, self).__init__(rmio, label,
            0x40084000, 0x0A0,
            'VCOCTRL', 'RAC.VCOCTRL', 'read-write',
            u"",
            0x0A0003AA, 0x0F007FFF)

        self.VCOAMPLITUDE = RM_Field_RAC_VCOCTRL_VCOAMPLITUDE(self)
        self.zz_fdict['VCOAMPLITUDE'] = self.VCOAMPLITUDE
        self.VCODETAMPLITUDE = RM_Field_RAC_VCOCTRL_VCODETAMPLITUDE(self)
        self.zz_fdict['VCODETAMPLITUDE'] = self.VCODETAMPLITUDE
        self.VCODETEN = RM_Field_RAC_VCOCTRL_VCODETEN(self)
        self.zz_fdict['VCODETEN'] = self.VCODETEN
        self.VCODETMODE = RM_Field_RAC_VCOCTRL_VCODETMODE(self)
        self.zz_fdict['VCODETMODE'] = self.VCODETMODE
        self.VCOAREGCURR = RM_Field_RAC_VCOCTRL_VCOAREGCURR(self)
        self.zz_fdict['VCOAREGCURR'] = self.VCOAREGCURR
        self.VCOCREGCURR = RM_Field_RAC_VCOCTRL_VCOCREGCURR(self)
        self.zz_fdict['VCOCREGCURR'] = self.VCOCREGCURR
        self.VCODIVCURR = RM_Field_RAC_VCOCTRL_VCODIVCURR(self)
        self.zz_fdict['VCODIVCURR'] = self.VCODIVCURR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_MMDCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_MMDCTRL, self).__init__(rmio, label,
            0x40084000, 0x0A4,
            'MMDCTRL', 'RAC.MMDCTRL', 'read-write',
            u"",
            0x00002563, 0x0001FDFF)

        self.MMDDIVDCDC = RM_Field_RAC_MMDCTRL_MMDDIVDCDC(self)
        self.zz_fdict['MMDDIVDCDC'] = self.MMDDIVDCDC
        self.MMDDIVRSDCDC = RM_Field_RAC_MMDCTRL_MMDDIVRSDCDC(self)
        self.zz_fdict['MMDDIVRSDCDC'] = self.MMDDIVRSDCDC
        self.MMDDIVRSDIG = RM_Field_RAC_MMDCTRL_MMDDIVRSDIG(self)
        self.zz_fdict['MMDDIVRSDIG'] = self.MMDDIVRSDIG
        self.MMDENDCDC = RM_Field_RAC_MMDCTRL_MMDENDCDC(self)
        self.zz_fdict['MMDENDCDC'] = self.MMDENDCDC
        self.MMDENRSDCDC = RM_Field_RAC_MMDCTRL_MMDENRSDCDC(self)
        self.zz_fdict['MMDENRSDCDC'] = self.MMDENRSDCDC
        self.MMDENRSDIG = RM_Field_RAC_MMDCTRL_MMDENRSDIG(self)
        self.zz_fdict['MMDENRSDIG'] = self.MMDENRSDIG
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CHPCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CHPCTRL, self).__init__(rmio, label,
            0x40084000, 0x0B0,
            'CHPCTRL', 'RAC.CHPCTRL', 'read-write',
            u"",
            0x0000002E, 0x0000C03F)

        self.CHPBIAS = RM_Field_RAC_CHPCTRL_CHPBIAS(self)
        self.zz_fdict['CHPBIAS'] = self.CHPBIAS
        self.CHPCURR = RM_Field_RAC_CHPCTRL_CHPCURR(self)
        self.zz_fdict['CHPCURR'] = self.CHPCURR
        self.CHPOUTPUTTRISTATEMODE = RM_Field_RAC_CHPCTRL_CHPOUTPUTTRISTATEMODE(self)
        self.zz_fdict['CHPOUTPUTTRISTATEMODE'] = self.CHPOUTPUTTRISTATEMODE
        self.CHPSWITCHEDBIASEN = RM_Field_RAC_CHPCTRL_CHPSWITCHEDBIASEN(self)
        self.zz_fdict['CHPSWITCHEDBIASEN'] = self.CHPSWITCHEDBIASEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CHPCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CHPCAL, self).__init__(rmio, label,
            0x40084000, 0x0B4,
            'CHPCAL', 'RAC.CHPCAL', 'read-write',
            u"",
            0x00000024, 0x0000003F)

        self.PSRC = RM_Field_RAC_CHPCAL_PSRC(self)
        self.zz_fdict['PSRC'] = self.PSRC
        self.NSRC = RM_Field_RAC_CHPCAL_NSRC(self)
        self.zz_fdict['NSRC'] = self.NSRC
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LPFCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LPFCTRL, self).__init__(rmio, label,
            0x40084000, 0x0B8,
            'LPFCTRL', 'RAC.LPFCTRL', 'read-write',
            u"",
            0x00B3C00B, 0x00F3C01F)

        self.LPFBWRX = RM_Field_RAC_LPFCTRL_LPFBWRX(self)
        self.zz_fdict['LPFBWRX'] = self.LPFBWRX
        self.MIRACLEMODE = RM_Field_RAC_LPFCTRL_MIRACLEMODE(self)
        self.zz_fdict['MIRACLEMODE'] = self.MIRACLEMODE
        self.LPFINPUTCAP = RM_Field_RAC_LPFCTRL_LPFINPUTCAP(self)
        self.zz_fdict['LPFINPUTCAP'] = self.LPFINPUTCAP
        self.LPFSWITCHINGEN = RM_Field_RAC_LPFCTRL_LPFSWITCHINGEN(self)
        self.zz_fdict['LPFSWITCHINGEN'] = self.LPFSWITCHINGEN
        self.LPFGNDSWITCHINGEN = RM_Field_RAC_LPFCTRL_LPFGNDSWITCHINGEN(self)
        self.zz_fdict['LPFGNDSWITCHINGEN'] = self.LPFGNDSWITCHINGEN
        self.LPFBWTX = RM_Field_RAC_LPFCTRL_LPFBWTX(self)
        self.zz_fdict['LPFBWTX'] = self.LPFBWTX
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SYNTHCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SYNTHCTRL, self).__init__(rmio, label,
            0x40084000, 0x0BC,
            'SYNTHCTRL', 'RAC.SYNTHCTRL', 'read-write',
            u"",
            0x00000000, 0x0000040E)

        self.LODIVTXEN0DBM = RM_Field_RAC_SYNTHCTRL_LODIVTXEN0DBM(self)
        self.zz_fdict['LODIVTXEN0DBM'] = self.LODIVTXEN0DBM
        self.LODIVTXEN = RM_Field_RAC_SYNTHCTRL_LODIVTXEN(self)
        self.zz_fdict['LODIVTXEN'] = self.LODIVTXEN
        self.PFDTRADMODE = RM_Field_RAC_SYNTHCTRL_PFDTRADMODE(self)
        self.zz_fdict['PFDTRADMODE'] = self.PFDTRADMODE
        self.MMDPOWERBALANCEDISABLE = RM_Field_RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE(self)
        self.zz_fdict['MMDPOWERBALANCEDISABLE'] = self.MMDPOWERBALANCEDISABLE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXCTRL, self).__init__(rmio, label,
            0x40084000, 0x0C0,
            'AUXCTRL', 'RAC.AUXCTRL', 'read-write',
            u"",
            0x01360010, 0x03FFFFFF)

        self.AUXREGEN = RM_Field_RAC_AUXCTRL_AUXREGEN(self)
        self.zz_fdict['AUXREGEN'] = self.AUXREGEN
        self.VCOEN = RM_Field_RAC_AUXCTRL_VCOEN(self)
        self.zz_fdict['VCOEN'] = self.VCOEN
        self.VCOSTARTUP = RM_Field_RAC_AUXCTRL_VCOSTARTUP(self)
        self.zz_fdict['VCOSTARTUP'] = self.VCOSTARTUP
        self.CHPEN = RM_Field_RAC_AUXCTRL_CHPEN(self)
        self.zz_fdict['CHPEN'] = self.CHPEN
        self.CHPCURR = RM_Field_RAC_AUXCTRL_CHPCURR(self)
        self.zz_fdict['CHPCURR'] = self.CHPCURR
        self.LODIVEN = RM_Field_RAC_AUXCTRL_LODIVEN(self)
        self.zz_fdict['LODIVEN'] = self.LODIVEN
        self.AUXSYNTHCLKEN = RM_Field_RAC_AUXCTRL_AUXSYNTHCLKEN(self)
        self.zz_fdict['AUXSYNTHCLKEN'] = self.AUXSYNTHCLKEN
        self.LODIVSEL = RM_Field_RAC_AUXCTRL_LODIVSEL(self)
        self.zz_fdict['LODIVSEL'] = self.LODIVSEL
        self.RXAMP = RM_Field_RAC_AUXCTRL_RXAMP(self)
        self.zz_fdict['RXAMP'] = self.RXAMP
        self.LDOAMPCURR = RM_Field_RAC_AUXCTRL_LDOAMPCURR(self)
        self.zz_fdict['LDOAMPCURR'] = self.LDOAMPCURR
        self.LDOVREFTRIM = RM_Field_RAC_AUXCTRL_LDOVREFTRIM(self)
        self.zz_fdict['LDOVREFTRIM'] = self.LDOVREFTRIM
        self.FPLLDIGEN = RM_Field_RAC_AUXCTRL_FPLLDIGEN(self)
        self.zz_fdict['FPLLDIGEN'] = self.FPLLDIGEN
        self.LPFRES = RM_Field_RAC_AUXCTRL_LPFRES(self)
        self.zz_fdict['LPFRES'] = self.LPFRES
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_AUXENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_AUXENCTRL, self).__init__(rmio, label,
            0x40084000, 0x0C4,
            'AUXENCTRL', 'RAC.AUXENCTRL', 'read-write',
            u"",
            0x00000000, 0x0000000F)

        self.LDOEN = RM_Field_RAC_AUXENCTRL_LDOEN(self)
        self.zz_fdict['LDOEN'] = self.LDOEN
        self.VCBUFEN = RM_Field_RAC_AUXENCTRL_VCBUFEN(self)
        self.zz_fdict['VCBUFEN'] = self.VCBUFEN
        self.LODIVAUXEN = RM_Field_RAC_AUXENCTRL_LODIVAUXEN(self)
        self.zz_fdict['LODIVAUXEN'] = self.LODIVAUXEN
        self.LODIVAUXEN0DBM = RM_Field_RAC_AUXENCTRL_LODIVAUXEN0DBM(self)
        self.zz_fdict['LODIVAUXEN0DBM'] = self.LODIVAUXEN0DBM
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFENCTRL, self).__init__(rmio, label,
            0x40084000, 0x0F4,
            'RFENCTRL', 'RAC.RFENCTRL', 'read-write',
            u"",
            0x00001020, 0x031FF87F)

        self.IFFILTSTANDBY = RM_Field_RAC_RFENCTRL_IFFILTSTANDBY(self)
        self.zz_fdict['IFFILTSTANDBY'] = self.IFFILTSTANDBY
        self.IFFILTEREN = RM_Field_RAC_RFENCTRL_IFFILTEREN(self)
        self.zz_fdict['IFFILTEREN'] = self.IFFILTEREN
        self.PKDEN = RM_Field_RAC_RFENCTRL_PKDEN(self)
        self.zz_fdict['PKDEN'] = self.PKDEN
        self.IFPGAEN = RM_Field_RAC_RFENCTRL_IFPGAEN(self)
        self.zz_fdict['IFPGAEN'] = self.IFPGAEN
        self.IFADCEN = RM_Field_RAC_RFENCTRL_IFADCEN(self)
        self.zz_fdict['IFADCEN'] = self.IFADCEN
        self.IFADCCAPRESET = RM_Field_RAC_RFENCTRL_IFADCCAPRESET(self)
        self.zz_fdict['IFADCCAPRESET'] = self.IFADCCAPRESET
        self.IFADCCLKEN = RM_Field_RAC_RFENCTRL_IFADCCLKEN(self)
        self.zz_fdict['IFADCCLKEN'] = self.IFADCCLKEN
        self.RFBIASEN = RM_Field_RAC_RFENCTRL_RFBIASEN(self)
        self.zz_fdict['RFBIASEN'] = self.RFBIASEN
        self.DEMEN = RM_Field_RAC_RFENCTRL_DEMEN(self)
        self.zz_fdict['DEMEN'] = self.DEMEN
        self.ENABLEI = RM_Field_RAC_RFENCTRL_ENABLEI(self)
        self.zz_fdict['ENABLEI'] = self.ENABLEI
        self.ENABLEQ = RM_Field_RAC_RFENCTRL_ENABLEQ(self)
        self.zz_fdict['ENABLEQ'] = self.ENABLEQ
        self.ENLDOCLK = RM_Field_RAC_RFENCTRL_ENLDOCLK(self)
        self.zz_fdict['ENLDOCLK'] = self.ENLDOCLK
        self.ENLDOSERIES = RM_Field_RAC_RFENCTRL_ENLDOSERIES(self)
        self.zz_fdict['ENLDOSERIES'] = self.ENLDOSERIES
        self.ENLDOSHUNTI = RM_Field_RAC_RFENCTRL_ENLDOSHUNTI(self)
        self.zz_fdict['ENLDOSHUNTI'] = self.ENLDOSHUNTI
        self.ENLDOSHUNTQ = RM_Field_RAC_RFENCTRL_ENLDOSHUNTQ(self)
        self.zz_fdict['ENLDOSHUNTQ'] = self.ENLDOSHUNTQ
        self.ENLDOPGALNA = RM_Field_RAC_RFENCTRL_ENLDOPGALNA(self)
        self.zz_fdict['ENLDOPGALNA'] = self.ENLDOPGALNA
        self.ENPGA = RM_Field_RAC_RFENCTRL_ENPGA(self)
        self.zz_fdict['ENPGA'] = self.ENPGA
        self.PACTUNESEL = RM_Field_RAC_RFENCTRL_PACTUNESEL(self)
        self.zz_fdict['PACTUNESEL'] = self.PACTUNESEL
        self.SGPACTUNESEL = RM_Field_RAC_RFENCTRL_SGPACTUNESEL(self)
        self.zz_fdict['SGPACTUNESEL'] = self.SGPACTUNESEL
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFENCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFENCTRL0, self).__init__(rmio, label,
            0x40084000, 0x0F8,
            'RFENCTRL0', 'RAC.RFENCTRL0', 'read-write',
            u"",
            0x00000000, 0x030F000F)

        self.LNAMIXSTANDBY = RM_Field_RAC_RFENCTRL0_LNAMIXSTANDBY(self)
        self.zz_fdict['LNAMIXSTANDBY'] = self.LNAMIXSTANDBY
        self.LNAMIXBIASEN = RM_Field_RAC_RFENCTRL0_LNAMIXBIASEN(self)
        self.zz_fdict['LNAMIXBIASEN'] = self.LNAMIXBIASEN
        self.LNAMIXLOBIASEN = RM_Field_RAC_RFENCTRL0_LNAMIXLOBIASEN(self)
        self.zz_fdict['LNAMIXLOBIASEN'] = self.LNAMIXLOBIASEN
        self.LNAMIXRFBIASEN = RM_Field_RAC_RFENCTRL0_LNAMIXRFBIASEN(self)
        self.zz_fdict['LNAMIXRFBIASEN'] = self.LNAMIXRFBIASEN
        self.PASTANDBY = RM_Field_RAC_RFENCTRL0_PASTANDBY(self)
        self.zz_fdict['PASTANDBY'] = self.PASTANDBY
        self.PAEN = RM_Field_RAC_RFENCTRL0_PAEN(self)
        self.zz_fdict['PAEN'] = self.PAEN
        self.PAOUTEN = RM_Field_RAC_RFENCTRL0_PAOUTEN(self)
        self.zz_fdict['PAOUTEN'] = self.PAOUTEN
        self.TRSW = RM_Field_RAC_RFENCTRL0_TRSW(self)
        self.zz_fdict['TRSW'] = self.TRSW
        self.CASCODEDIS = RM_Field_RAC_RFENCTRL0_CASCODEDIS(self)
        self.zz_fdict['CASCODEDIS'] = self.CASCODEDIS
        self.STRIPESLICEDIS = RM_Field_RAC_RFENCTRL0_STRIPESLICEDIS(self)
        self.zz_fdict['STRIPESLICEDIS'] = self.STRIPESLICEDIS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXCTRL, self).__init__(rmio, label,
            0x40084000, 0x0FC,
            'LNAMIXCTRL', 'RAC.LNAMIXCTRL', 'read-write',
            u"",
            0x00000000, 0x44FFFF1F)

        self.RF2P4BANDWIDTH = RM_Field_RAC_LNAMIXCTRL_RF2P4BANDWIDTH(self)
        self.zz_fdict['RF2P4BANDWIDTH'] = self.RF2P4BANDWIDTH
        self.CASCODEBIAS = RM_Field_RAC_LNAMIXCTRL_CASCODEBIAS(self)
        self.zz_fdict['CASCODEBIAS'] = self.CASCODEBIAS
        self.LOBIAS = RM_Field_RAC_LNAMIXCTRL_LOBIAS(self)
        self.zz_fdict['LOBIAS'] = self.LOBIAS
        self.VREG = RM_Field_RAC_LNAMIXCTRL_VREG(self)
        self.zz_fdict['VREG'] = self.VREG
        self.RFBIAS = RM_Field_RAC_LNAMIXCTRL_RFBIAS(self)
        self.zz_fdict['RFBIAS'] = self.RFBIAS
        self.RISEFALLATT = RM_Field_RAC_LNAMIXCTRL_RISEFALLATT(self)
        self.zz_fdict['RISEFALLATT'] = self.RISEFALLATT
        self.LNAMIXAUXSYNTHINPUTEN = RM_Field_RAC_LNAMIXCTRL_LNAMIXAUXSYNTHINPUTEN(self)
        self.zz_fdict['LNAMIXAUXSYNTHINPUTEN'] = self.LNAMIXAUXSYNTHINPUTEN
        self.ENLOAD = RM_Field_RAC_LNAMIXCTRL_ENLOAD(self)
        self.zz_fdict['ENLOAD'] = self.ENLOAD
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PACTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PACTRL0, self).__init__(rmio, label,
            0x40084000, 0x100,
            'PACTRL0', 'RAC.PACTRL0', 'read-write',
            u"",
            0x0000000C, 0xDF3FFFDF)

        self.RF2P4PASEL = RM_Field_RAC_PACTRL0_RF2P4PASEL(self)
        self.zz_fdict['RF2P4PASEL'] = self.RF2P4PASEL
        self.RF2P4PAVDDSEL = RM_Field_RAC_PACTRL0_RF2P4PAVDDSEL(self)
        self.zz_fdict['RF2P4PAVDDSEL'] = self.RF2P4PAVDDSEL
        self.RF2P4RFVDDSEL = RM_Field_RAC_PACTRL0_RF2P4RFVDDSEL(self)
        self.zz_fdict['RF2P4RFVDDSEL'] = self.RF2P4RFVDDSEL
        self.BOOTSTRAP = RM_Field_RAC_PACTRL0_BOOTSTRAP(self)
        self.zz_fdict['BOOTSTRAP'] = self.BOOTSTRAP
        self.RF2P4VDDPADET = RM_Field_RAC_PACTRL0_RF2P4VDDPADET(self)
        self.zz_fdict['RF2P4VDDPADET'] = self.RF2P4VDDPADET
        self.CASCODE = RM_Field_RAC_PACTRL0_CASCODE(self)
        self.zz_fdict['CASCODE'] = self.CASCODE
        self.SLICE = RM_Field_RAC_PACTRL0_SLICE(self)
        self.zz_fdict['SLICE'] = self.SLICE
        self.STRIPE = RM_Field_RAC_PACTRL0_STRIPE(self)
        self.zz_fdict['STRIPE'] = self.STRIPE
        self.DACGLITCHCTRL = RM_Field_RAC_PACTRL0_DACGLITCHCTRL(self)
        self.zz_fdict['DACGLITCHCTRL'] = self.DACGLITCHCTRL
        self.CASCODEBYPASSEN = RM_Field_RAC_PACTRL0_CASCODEBYPASSEN(self)
        self.zz_fdict['CASCODEBYPASSEN'] = self.CASCODEBYPASSEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PAPKDCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PAPKDCTRL, self).__init__(rmio, label,
            0x40084000, 0x104,
            'PAPKDCTRL', 'RAC.PAPKDCTRL', 'read-write',
            u"",
            0x0104C000, 0x01CCDF7D)

        self.PKDEN = RM_Field_RAC_PAPKDCTRL_PKDEN(self)
        self.zz_fdict['PKDEN'] = self.PKDEN
        self.VTLSEL = RM_Field_RAC_PAPKDCTRL_VTLSEL(self)
        self.zz_fdict['VTLSEL'] = self.VTLSEL
        self.VTHSEL = RM_Field_RAC_PAPKDCTRL_VTHSEL(self)
        self.zz_fdict['VTHSEL'] = self.VTHSEL
        self.CAPSEL = RM_Field_RAC_PAPKDCTRL_CAPSEL(self)
        self.zz_fdict['CAPSEL'] = self.CAPSEL
        self.I2VCM = RM_Field_RAC_PAPKDCTRL_I2VCM(self)
        self.zz_fdict['I2VCM'] = self.I2VCM
        self.PKDBIASTH = RM_Field_RAC_PAPKDCTRL_PKDBIASTH(self)
        self.zz_fdict['PKDBIASTH'] = self.PKDBIASTH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PABIASCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PABIASCTRL0, self).__init__(rmio, label,
            0x40084000, 0x108,
            'PABIASCTRL0', 'RAC.PABIASCTRL0', 'read-write',
            u"",
            0x00000445, 0x00C00CCD)

        self.LDOBIAS = RM_Field_RAC_PABIASCTRL0_LDOBIAS(self)
        self.zz_fdict['LDOBIAS'] = self.LDOBIAS
        self.PABIAS = RM_Field_RAC_PABIASCTRL0_PABIAS(self)
        self.zz_fdict['PABIAS'] = self.PABIAS
        self.BUF0BIAS = RM_Field_RAC_PABIASCTRL0_BUF0BIAS(self)
        self.zz_fdict['BUF0BIAS'] = self.BUF0BIAS
        self.BUF12BIAS = RM_Field_RAC_PABIASCTRL0_BUF12BIAS(self)
        self.zz_fdict['BUF12BIAS'] = self.BUF12BIAS
        self.TXPOWER = RM_Field_RAC_PABIASCTRL0_TXPOWER(self)
        self.zz_fdict['TXPOWER'] = self.TXPOWER
        self.CMGAIN = RM_Field_RAC_PABIASCTRL0_CMGAIN(self)
        self.zz_fdict['CMGAIN'] = self.CMGAIN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PABIASCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PABIASCTRL1, self).__init__(rmio, label,
            0x40084000, 0x10C,
            'PABIASCTRL1', 'RAC.PABIASCTRL1', 'read-write',
            u"",
            0x00024523, 0x00037737)

        self.VLDO = RM_Field_RAC_PABIASCTRL1_VLDO(self)
        self.zz_fdict['VLDO'] = self.VLDO
        self.VLDOFB = RM_Field_RAC_PABIASCTRL1_VLDOFB(self)
        self.zz_fdict['VLDOFB'] = self.VLDOFB
        self.VCASCODEHV = RM_Field_RAC_PABIASCTRL1_VCASCODEHV(self)
        self.zz_fdict['VCASCODEHV'] = self.VCASCODEHV
        self.VCASCODELV = RM_Field_RAC_PABIASCTRL1_VCASCODELV(self)
        self.zz_fdict['VCASCODELV'] = self.VCASCODELV
        self.RF2P4VDDPATHRESHOLD = RM_Field_RAC_PABIASCTRL1_RF2P4VDDPATHRESHOLD(self)
        self.zz_fdict['RF2P4VDDPATHRESHOLD'] = self.RF2P4VDDPATHRESHOLD
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGRFENCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGRFENCTRL0, self).__init__(rmio, label,
            0x40084000, 0x110,
            'SGRFENCTRL0', 'RAC.SGRFENCTRL0', 'read-write',
            u"",
            0x00000000, 0x030F000E)

        self.LNAMIXBIASEN = RM_Field_RAC_SGRFENCTRL0_LNAMIXBIASEN(self)
        self.zz_fdict['LNAMIXBIASEN'] = self.LNAMIXBIASEN
        self.LNAMIXLOBIASEN = RM_Field_RAC_SGRFENCTRL0_LNAMIXLOBIASEN(self)
        self.zz_fdict['LNAMIXLOBIASEN'] = self.LNAMIXLOBIASEN
        self.LNAMIXRFBIASEN = RM_Field_RAC_SGRFENCTRL0_LNAMIXRFBIASEN(self)
        self.zz_fdict['LNAMIXRFBIASEN'] = self.LNAMIXRFBIASEN
        self.PASTANDBY = RM_Field_RAC_SGRFENCTRL0_PASTANDBY(self)
        self.zz_fdict['PASTANDBY'] = self.PASTANDBY
        self.PAEN = RM_Field_RAC_SGRFENCTRL0_PAEN(self)
        self.zz_fdict['PAEN'] = self.PAEN
        self.PAOUTEN = RM_Field_RAC_SGRFENCTRL0_PAOUTEN(self)
        self.zz_fdict['PAOUTEN'] = self.PAOUTEN
        self.TRSW = RM_Field_RAC_SGRFENCTRL0_TRSW(self)
        self.zz_fdict['TRSW'] = self.TRSW
        self.CASCODEDIS = RM_Field_RAC_SGRFENCTRL0_CASCODEDIS(self)
        self.zz_fdict['CASCODEDIS'] = self.CASCODEDIS
        self.STRIPESLICEDIS = RM_Field_RAC_SGRFENCTRL0_STRIPESLICEDIS(self)
        self.zz_fdict['STRIPESLICEDIS'] = self.STRIPESLICEDIS
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGLNAMIXCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGLNAMIXCTRL, self).__init__(rmio, label,
            0x40084000, 0x114,
            'SGLNAMIXCTRL', 'RAC.SGLNAMIXCTRL', 'read-write',
            u"",
            0x0186DB00, 0x77FFFF00)

        self.CASCODEBIAS = RM_Field_RAC_SGLNAMIXCTRL_CASCODEBIAS(self)
        self.zz_fdict['CASCODEBIAS'] = self.CASCODEBIAS
        self.LOBIAS = RM_Field_RAC_SGLNAMIXCTRL_LOBIAS(self)
        self.zz_fdict['LOBIAS'] = self.LOBIAS
        self.VREG = RM_Field_RAC_SGLNAMIXCTRL_VREG(self)
        self.zz_fdict['VREG'] = self.VREG
        self.RFBIAS = RM_Field_RAC_SGLNAMIXCTRL_RFBIAS(self)
        self.zz_fdict['RFBIAS'] = self.RFBIAS
        self.RISEFALLATT = RM_Field_RAC_SGLNAMIXCTRL_RISEFALLATT(self)
        self.zz_fdict['RISEFALLATT'] = self.RISEFALLATT
        self.SGREGAMPCURR = RM_Field_RAC_SGLNAMIXCTRL_SGREGAMPCURR(self)
        self.zz_fdict['SGREGAMPCURR'] = self.SGREGAMPCURR
        self.LNAMIXAUXSYNTHINPUTEN = RM_Field_RAC_SGLNAMIXCTRL_LNAMIXAUXSYNTHINPUTEN(self)
        self.zz_fdict['LNAMIXAUXSYNTHINPUTEN'] = self.LNAMIXAUXSYNTHINPUTEN
        self.SGREGAMPBWRED = RM_Field_RAC_SGLNAMIXCTRL_SGREGAMPBWRED(self)
        self.zz_fdict['SGREGAMPBWRED'] = self.SGREGAMPBWRED
        self.ENLOAD = RM_Field_RAC_SGLNAMIXCTRL_ENLOAD(self)
        self.zz_fdict['ENLOAD'] = self.ENLOAD
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGPACTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGPACTRL0, self).__init__(rmio, label,
            0x40084000, 0x118,
            'SGPACTRL0', 'RAC.SGPACTRL0', 'read-write',
            u"",
            0x40000008, 0xDF3FFFE8)

        self.BOOTSTRAP = RM_Field_RAC_SGPACTRL0_BOOTSTRAP(self)
        self.zz_fdict['BOOTSTRAP'] = self.BOOTSTRAP
        self.SGVBATDET = RM_Field_RAC_SGPACTRL0_SGVBATDET(self)
        self.zz_fdict['SGVBATDET'] = self.SGVBATDET
        self.CASCODE = RM_Field_RAC_SGPACTRL0_CASCODE(self)
        self.zz_fdict['CASCODE'] = self.CASCODE
        self.SLICE = RM_Field_RAC_SGPACTRL0_SLICE(self)
        self.zz_fdict['SLICE'] = self.SLICE
        self.STRIPE = RM_Field_RAC_SGPACTRL0_STRIPE(self)
        self.zz_fdict['STRIPE'] = self.STRIPE
        self.DACGLITCHCTRL = RM_Field_RAC_SGPACTRL0_DACGLITCHCTRL(self)
        self.zz_fdict['DACGLITCHCTRL'] = self.DACGLITCHCTRL
        self.CASCODEBYPASSEN = RM_Field_RAC_SGPACTRL0_CASCODEBYPASSEN(self)
        self.zz_fdict['CASCODEBYPASSEN'] = self.CASCODEBYPASSEN
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGPAPKDCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGPAPKDCTRL, self).__init__(rmio, label,
            0x40084000, 0x11C,
            'SGPAPKDCTRL', 'RAC.SGPAPKDCTRL', 'read-write',
            u"",
            0x0108C000, 0x01CCDF7D)

        self.PKDEN = RM_Field_RAC_SGPAPKDCTRL_PKDEN(self)
        self.zz_fdict['PKDEN'] = self.PKDEN
        self.VTLSEL = RM_Field_RAC_SGPAPKDCTRL_VTLSEL(self)
        self.zz_fdict['VTLSEL'] = self.VTLSEL
        self.VTHSEL = RM_Field_RAC_SGPAPKDCTRL_VTHSEL(self)
        self.zz_fdict['VTHSEL'] = self.VTHSEL
        self.CAPSEL = RM_Field_RAC_SGPAPKDCTRL_CAPSEL(self)
        self.zz_fdict['CAPSEL'] = self.CAPSEL
        self.I2VCM = RM_Field_RAC_SGPAPKDCTRL_I2VCM(self)
        self.zz_fdict['I2VCM'] = self.I2VCM
        self.PKDBIASTH = RM_Field_RAC_SGPAPKDCTRL_PKDBIASTH(self)
        self.zz_fdict['PKDBIASTH'] = self.PKDBIASTH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGPABIASCTRL0(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGPABIASCTRL0, self).__init__(rmio, label,
            0x40084000, 0x120,
            'SGPABIASCTRL0', 'RAC.SGPABIASCTRL0', 'read-write',
            u"",
            0x07000445, 0x87C00CCD)

        self.LDOBIAS = RM_Field_RAC_SGPABIASCTRL0_LDOBIAS(self)
        self.zz_fdict['LDOBIAS'] = self.LDOBIAS
        self.PABIAS = RM_Field_RAC_SGPABIASCTRL0_PABIAS(self)
        self.zz_fdict['PABIAS'] = self.PABIAS
        self.BUF0BIAS = RM_Field_RAC_SGPABIASCTRL0_BUF0BIAS(self)
        self.zz_fdict['BUF0BIAS'] = self.BUF0BIAS
        self.BUF12BIAS = RM_Field_RAC_SGPABIASCTRL0_BUF12BIAS(self)
        self.zz_fdict['BUF12BIAS'] = self.BUF12BIAS
        self.TXPOWER = RM_Field_RAC_SGPABIASCTRL0_TXPOWER(self)
        self.zz_fdict['TXPOWER'] = self.TXPOWER
        self.CMGAIN = RM_Field_RAC_SGPABIASCTRL0_CMGAIN(self)
        self.zz_fdict['CMGAIN'] = self.CMGAIN
        self.SGDACFILTBANDWIDTH = RM_Field_RAC_SGPABIASCTRL0_SGDACFILTBANDWIDTH(self)
        self.zz_fdict['SGDACFILTBANDWIDTH'] = self.SGDACFILTBANDWIDTH
        self.SGPAOOKINCRMODDEPTH = RM_Field_RAC_SGPABIASCTRL0_SGPAOOKINCRMODDEPTH(self)
        self.zz_fdict['SGPAOOKINCRMODDEPTH'] = self.SGPAOOKINCRMODDEPTH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGPABIASCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGPABIASCTRL1, self).__init__(rmio, label,
            0x40084000, 0x124,
            'SGPABIASCTRL1', 'RAC.SGPABIASCTRL1', 'read-write',
            u"",
            0x00084523, 0x001C7737)

        self.VLDO = RM_Field_RAC_SGPABIASCTRL1_VLDO(self)
        self.zz_fdict['VLDO'] = self.VLDO
        self.VLDOFB = RM_Field_RAC_SGPABIASCTRL1_VLDOFB(self)
        self.zz_fdict['VLDOFB'] = self.VLDOFB
        self.VCASCODEHV = RM_Field_RAC_SGPABIASCTRL1_VCASCODEHV(self)
        self.zz_fdict['VCASCODEHV'] = self.VCASCODEHV
        self.VCASCODELV = RM_Field_RAC_SGPABIASCTRL1_VCASCODELV(self)
        self.zz_fdict['VCASCODELV'] = self.VCASCODELV
        self.SGVBATDETTHRESHOLD = RM_Field_RAC_SGPABIASCTRL1_SGVBATDETTHRESHOLD(self)
        self.zz_fdict['SGVBATDETTHRESHOLD'] = self.SGVBATDETTHRESHOLD
        self.SGTRIMLOWVBATCURR = RM_Field_RAC_SGPABIASCTRL1_SGTRIMLOWVBATCURR(self)
        self.zz_fdict['SGTRIMLOWVBATCURR'] = self.SGTRIMLOWVBATCURR
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFSTATUS, self).__init__(rmio, label,
            0x40084000, 0x128,
            'RFSTATUS', 'RAC.RFSTATUS', 'read-only',
            u"",
            0x00000000, 0x0000000F)

        self.MODRAMPUPDONE = RM_Field_RAC_RFSTATUS_MODRAMPUPDONE(self)
        self.zz_fdict['MODRAMPUPDONE'] = self.MODRAMPUPDONE
        self.PAVHIGH = RM_Field_RAC_RFSTATUS_PAVHIGH(self)
        self.zz_fdict['PAVHIGH'] = self.PAVHIGH
        self.PAVLOW = RM_Field_RAC_RFSTATUS_PAVLOW(self)
        self.zz_fdict['PAVLOW'] = self.PAVLOW
        self.PABATHIGH = RM_Field_RAC_RFSTATUS_PABATHIGH(self)
        self.zz_fdict['PABATHIGH'] = self.PABATHIGH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFBIASCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFBIASCTRL, self).__init__(rmio, label,
            0x40084000, 0x12C,
            'RFBIASCTRL', 'RAC.RFBIASCTRL', 'read-write',
            u"",
            0x00000034, 0x00070077)

        self.LDOVREF = RM_Field_RAC_RFBIASCTRL_LDOVREF(self)
        self.zz_fdict['LDOVREF'] = self.LDOVREF
        self.LDOAMPCURR = RM_Field_RAC_RFBIASCTRL_LDOAMPCURR(self)
        self.zz_fdict['LDOAMPCURR'] = self.LDOAMPCURR
        self.STARTUPSUPPLY = RM_Field_RAC_RFBIASCTRL_STARTUPSUPPLY(self)
        self.zz_fdict['STARTUPSUPPLY'] = self.STARTUPSUPPLY
        self.STARTUPCORE = RM_Field_RAC_RFBIASCTRL_STARTUPCORE(self)
        self.zz_fdict['STARTUPCORE'] = self.STARTUPCORE
        self.DISBOOTSTRAP = RM_Field_RAC_RFBIASCTRL_DISBOOTSTRAP(self)
        self.zz_fdict['DISBOOTSTRAP'] = self.DISBOOTSTRAP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RFBIASCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RFBIASCAL, self).__init__(rmio, label,
            0x40084000, 0x130,
            'RFBIASCAL', 'RAC.RFBIASCAL', 'read-write',
            u"",
            0x001F1F1F, 0x003F3F3F)

        self.VREF = RM_Field_RAC_RFBIASCAL_VREF(self)
        self.zz_fdict['VREF'] = self.VREF
        self.BIAS = RM_Field_RAC_RFBIASCAL_BIAS(self)
        self.zz_fdict['BIAS'] = self.BIAS
        self.TEMPCO = RM_Field_RAC_RFBIASCAL_TEMPCO(self)
        self.zz_fdict['TEMPCO'] = self.TEMPCO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_LNAMIXCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_LNAMIXCTRL1, self).__init__(rmio, label,
            0x40084000, 0x134,
            'LNAMIXCTRL1', 'RAC.LNAMIXCTRL1', 'read-write',
            u"",
            0x00000880, 0x007FFFFE)

        self.TRIMAUXPLLCLK = RM_Field_RAC_LNAMIXCTRL1_TRIMAUXPLLCLK(self)
        self.zz_fdict['TRIMAUXPLLCLK'] = self.TRIMAUXPLLCLK
        self.TRIMTRSWGATEV = RM_Field_RAC_LNAMIXCTRL1_TRIMTRSWGATEV(self)
        self.zz_fdict['TRIMTRSWGATEV'] = self.TRIMTRSWGATEV
        self.TRIMVCASLDO = RM_Field_RAC_LNAMIXCTRL1_TRIMVCASLDO(self)
        self.zz_fdict['TRIMVCASLDO'] = self.TRIMVCASLDO
        self.TRIMVREFLDO = RM_Field_RAC_LNAMIXCTRL1_TRIMVREFLDO(self)
        self.zz_fdict['TRIMVREFLDO'] = self.TRIMVREFLDO
        self.TRIMVREGMIN = RM_Field_RAC_LNAMIXCTRL1_TRIMVREGMIN(self)
        self.zz_fdict['TRIMVREGMIN'] = self.TRIMVREGMIN
        self.TRIMAUXBIAS = RM_Field_RAC_LNAMIXCTRL1_TRIMAUXBIAS(self)
        self.zz_fdict['TRIMAUXBIAS'] = self.TRIMAUXBIAS
        self.ENBIASCAL = RM_Field_RAC_LNAMIXCTRL1_ENBIASCAL(self)
        self.zz_fdict['ENBIASCAL'] = self.ENBIASCAL
        self.STATUSBIASCAL = RM_Field_RAC_LNAMIXCTRL1_STATUSBIASCAL(self)
        self.zz_fdict['STATUSBIASCAL'] = self.STATUSBIASCAL
        self.TRIMAUXPLLGAIN = RM_Field_RAC_LNAMIXCTRL1_TRIMAUXPLLGAIN(self)
        self.zz_fdict['TRIMAUXPLLGAIN'] = self.TRIMAUXPLLGAIN
        self.TRIMLDOAMPBWRED = RM_Field_RAC_LNAMIXCTRL1_TRIMLDOAMPBWRED(self)
        self.zz_fdict['TRIMLDOAMPBWRED'] = self.TRIMLDOAMPBWRED
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFPGACTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFPGACTRL, self).__init__(rmio, label,
            0x40084000, 0x138,
            'IFPGACTRL', 'RAC.IFPGACTRL', 'read-write',
            u"",
            0x000087E0, 0x0007FFFE)

        self.VLDO = RM_Field_RAC_IFPGACTRL_VLDO(self)
        self.zz_fdict['VLDO'] = self.VLDO
        self.BANDSEL = RM_Field_RAC_IFPGACTRL_BANDSEL(self)
        self.zz_fdict['BANDSEL'] = self.BANDSEL
        self.CASCBIAS = RM_Field_RAC_IFPGACTRL_CASCBIAS(self)
        self.zz_fdict['CASCBIAS'] = self.CASCBIAS
        self.TRIMVCASLDO = RM_Field_RAC_IFPGACTRL_TRIMVCASLDO(self)
        self.zz_fdict['TRIMVCASLDO'] = self.TRIMVCASLDO
        self.TRIMVCM = RM_Field_RAC_IFPGACTRL_TRIMVCM(self)
        self.zz_fdict['TRIMVCM'] = self.TRIMVCM
        self.TRIMVREFLDO = RM_Field_RAC_IFPGACTRL_TRIMVREFLDO(self)
        self.zz_fdict['TRIMVREFLDO'] = self.TRIMVREFLDO
        self.TRIMVREGMIN = RM_Field_RAC_IFPGACTRL_TRIMVREGMIN(self)
        self.zz_fdict['TRIMVREGMIN'] = self.TRIMVREGMIN
        self.ENHYST = RM_Field_RAC_IFPGACTRL_ENHYST(self)
        self.zz_fdict['ENHYST'] = self.ENHYST
        self.ENOFFD = RM_Field_RAC_IFPGACTRL_ENOFFD(self)
        self.zz_fdict['ENOFFD'] = self.ENOFFD
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFPGACAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFPGACAL, self).__init__(rmio, label,
            0x40084000, 0x13C,
            'IFPGACAL', 'RAC.IFPGACAL', 'read-write',
            u"",
            0x00000000, 0x7F7F7F7F)

        self.IRAMP = RM_Field_RAC_IFPGACAL_IRAMP(self)
        self.zz_fdict['IRAMP'] = self.IRAMP
        self.IRPHASE = RM_Field_RAC_IFPGACAL_IRPHASE(self)
        self.zz_fdict['IRPHASE'] = self.IRPHASE
        self.OFFSETI = RM_Field_RAC_IFPGACAL_OFFSETI(self)
        self.zz_fdict['OFFSETI'] = self.OFFSETI
        self.OFFSETQ = RM_Field_RAC_IFPGACAL_OFFSETQ(self)
        self.zz_fdict['OFFSETQ'] = self.OFFSETQ
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFFILTCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFFILTCTRL, self).__init__(rmio, label,
            0x40084000, 0x140,
            'IFFILTCTRL', 'RAC.IFFILTCTRL', 'read-write',
            u"",
            0x00880000, 0x86EC00EF)

        self.BANDWIDTH = RM_Field_RAC_IFFILTCTRL_BANDWIDTH(self)
        self.zz_fdict['BANDWIDTH'] = self.BANDWIDTH
        self.CENTFREQ = RM_Field_RAC_IFFILTCTRL_CENTFREQ(self)
        self.zz_fdict['CENTFREQ'] = self.CENTFREQ
        self.VCM = RM_Field_RAC_IFFILTCTRL_VCM(self)
        self.zz_fdict['VCM'] = self.VCM
        self.VREG = RM_Field_RAC_IFFILTCTRL_VREG(self)
        self.zz_fdict['VREG'] = self.VREG
        self.DROOP = RM_Field_RAC_IFFILTCTRL_DROOP(self)
        self.zz_fdict['DROOP'] = self.DROOP
        self.INPUTSHORT = RM_Field_RAC_IFFILTCTRL_INPUTSHORT(self)
        self.zz_fdict['INPUTSHORT'] = self.INPUTSHORT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCCTRL, self).__init__(rmio, label,
            0x40084000, 0x144,
            'IFADCCTRL', 'RAC.IFADCCTRL', 'read-write',
            u"",
            0x0D52A6C0, 0x7FFFFFFF)

        self.REALMODE = RM_Field_RAC_IFADCCTRL_REALMODE(self)
        self.zz_fdict['REALMODE'] = self.REALMODE
        self.INPUTSEL = RM_Field_RAC_IFADCCTRL_INPUTSEL(self)
        self.zz_fdict['INPUTSEL'] = self.INPUTSEL
        self.INPUTSCALE = RM_Field_RAC_IFADCCTRL_INPUTSCALE(self)
        self.zz_fdict['INPUTSCALE'] = self.INPUTSCALE
        self.SHORTI = RM_Field_RAC_IFADCCTRL_SHORTI(self)
        self.zz_fdict['SHORTI'] = self.SHORTI
        self.SHORTQ = RM_Field_RAC_IFADCCTRL_SHORTQ(self)
        self.zz_fdict['SHORTQ'] = self.SHORTQ
        self.VLDOSERIES = RM_Field_RAC_IFADCCTRL_VLDOSERIES(self)
        self.zz_fdict['VLDOSERIES'] = self.VLDOSERIES
        self.VLDOSERIESCURR = RM_Field_RAC_IFADCCTRL_VLDOSERIESCURR(self)
        self.zz_fdict['VLDOSERIESCURR'] = self.VLDOSERIESCURR
        self.VLDOSHUNT = RM_Field_RAC_IFADCCTRL_VLDOSHUNT(self)
        self.zz_fdict['VLDOSHUNT'] = self.VLDOSHUNT
        self.VLDOCLKGEN = RM_Field_RAC_IFADCCTRL_VLDOCLKGEN(self)
        self.zz_fdict['VLDOCLKGEN'] = self.VLDOCLKGEN
        self.VCM = RM_Field_RAC_IFADCCTRL_VCM(self)
        self.zz_fdict['VCM'] = self.VCM
        self.OTA1CURRENT = RM_Field_RAC_IFADCCTRL_OTA1CURRENT(self)
        self.zz_fdict['OTA1CURRENT'] = self.OTA1CURRENT
        self.OTA2CURRENT = RM_Field_RAC_IFADCCTRL_OTA2CURRENT(self)
        self.zz_fdict['OTA2CURRENT'] = self.OTA2CURRENT
        self.OTA3CURRENT = RM_Field_RAC_IFADCCTRL_OTA3CURRENT(self)
        self.zz_fdict['OTA3CURRENT'] = self.OTA3CURRENT
        self.SATURATIONDETDIS = RM_Field_RAC_IFADCCTRL_SATURATIONDETDIS(self)
        self.zz_fdict['SATURATIONDETDIS'] = self.SATURATIONDETDIS
        self.REGENCLKDELAY = RM_Field_RAC_IFADCCTRL_REGENCLKDELAY(self)
        self.zz_fdict['REGENCLKDELAY'] = self.REGENCLKDELAY
        self.ENABLECLK = RM_Field_RAC_IFADCCTRL_ENABLECLK(self)
        self.zz_fdict['ENABLECLK'] = self.ENABLECLK
        self.INVERTCLK = RM_Field_RAC_IFADCCTRL_INVERTCLK(self)
        self.zz_fdict['INVERTCLK'] = self.INVERTCLK
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFADCCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFADCCAL, self).__init__(rmio, label,
            0x40084000, 0x148,
            'IFADCCAL', 'RAC.IFADCCAL', 'read-write',
            u"",
            0x00000000, 0x00000030)

        self.CALEN = RM_Field_RAC_IFADCCAL_CALEN(self)
        self.zz_fdict['CALEN'] = self.CALEN
        self.RCCALOUT = RM_Field_RAC_IFADCCAL_RCCALOUT(self)
        self.zz_fdict['RCCALOUT'] = self.RCCALOUT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PAENCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PAENCTRL, self).__init__(rmio, label,
            0x40084000, 0x14C,
            'PAENCTRL', 'RAC.PAENCTRL', 'read-write',
            u"",
            0x00000000, 0x00000100)

        self.PARAMP = RM_Field_RAC_PAENCTRL_PARAMP(self)
        self.zz_fdict['PARAMP'] = self.PARAMP
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_PACTUNECTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_PACTUNECTRL, self).__init__(rmio, label,
            0x40084000, 0x150,
            'PACTUNECTRL', 'RAC.PACTUNECTRL', 'read-write',
            u"",
            0x00000000, 0x01F701F7)

        self.PACTUNERX = RM_Field_RAC_PACTUNECTRL_PACTUNERX(self)
        self.zz_fdict['PACTUNERX'] = self.PACTUNERX
        self.SGPACTUNERX = RM_Field_RAC_PACTUNECTRL_SGPACTUNERX(self)
        self.zz_fdict['SGPACTUNERX'] = self.SGPACTUNERX
        self.PACTUNETX = RM_Field_RAC_PACTUNECTRL_PACTUNETX(self)
        self.zz_fdict['PACTUNETX'] = self.PACTUNETX
        self.SGPACTUNETX = RM_Field_RAC_PACTUNECTRL_SGPACTUNETX(self)
        self.zz_fdict['SGPACTUNETX'] = self.SGPACTUNETX
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_RCTUNE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_RCTUNE, self).__init__(rmio, label,
            0x40084000, 0x154,
            'RCTUNE', 'RAC.RCTUNE', 'read-write',
            u"",
            0x00000000, 0x003F003F)

        self.IFADCRCTUNE = RM_Field_RAC_RCTUNE_IFADCRCTUNE(self)
        self.zz_fdict['IFADCRCTUNE'] = self.IFADCRCTUNE
        self.IFFILT = RM_Field_RAC_RCTUNE_IFFILT(self)
        self.zz_fdict['IFFILT'] = self.IFFILT
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_APC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_APC, self).__init__(rmio, label,
            0x40084000, 0x158,
            'APC', 'RAC.APC', 'read-write',
            u"",
            0xFF000003, 0xFF000007)

        self.ENPASTATUSVDDPA = RM_Field_RAC_APC_ENPASTATUSVDDPA(self)
        self.zz_fdict['ENPASTATUSVDDPA'] = self.ENPASTATUSVDDPA
        self.ENPASTATUSPKDVTH = RM_Field_RAC_APC_ENPASTATUSPKDVTH(self)
        self.zz_fdict['ENPASTATUSPKDVTH'] = self.ENPASTATUSPKDVTH
        self.ENAPCSW = RM_Field_RAC_APC_ENAPCSW(self)
        self.zz_fdict['ENAPCSW'] = self.ENAPCSW
        self.AMPCONTROLLIMITSW = RM_Field_RAC_APC_AMPCONTROLLIMITSW(self)
        self.zz_fdict['AMPCONTROLLIMITSW'] = self.AMPCONTROLLIMITSW
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SPARE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SPARE, self).__init__(rmio, label,
            0x40084000, 0x15C,
            'SPARE', 'RAC.SPARE', 'read-write',
            u"",
            0x00000000, 0x3FFFBF00)

        self.LNAMIXERSPARE = RM_Field_RAC_SPARE_LNAMIXERSPARE(self)
        self.zz_fdict['LNAMIXERSPARE'] = self.LNAMIXERSPARE
        self.IFFILTSPARE = RM_Field_RAC_SPARE_IFFILTSPARE(self)
        self.zz_fdict['IFFILTSPARE'] = self.IFFILTSPARE
        self.IFPGASPARE = RM_Field_RAC_SPARE_IFPGASPARE(self)
        self.zz_fdict['IFPGASPARE'] = self.IFPGASPARE
        self.IFFILTSPARE0 = RM_Field_RAC_SPARE_IFFILTSPARE0(self)
        self.zz_fdict['IFFILTSPARE0'] = self.IFFILTSPARE0
        self.IFFILTSPARE1 = RM_Field_RAC_SPARE_IFFILTSPARE1(self)
        self.zz_fdict['IFFILTSPARE1'] = self.IFFILTSPARE1
        self.IFFILTSPARE2 = RM_Field_RAC_SPARE_IFFILTSPARE2(self)
        self.zz_fdict['IFFILTSPARE2'] = self.IFFILTSPARE2
        self.IFADCSPARE = RM_Field_RAC_SPARE_IFADCSPARE(self)
        self.zz_fdict['IFADCSPARE'] = self.IFADCSPARE
        self.PASPARE = RM_Field_RAC_SPARE_PASPARE(self)
        self.zz_fdict['PASPARE'] = self.PASPARE
        self.SGPASPARE = RM_Field_RAC_SPARE_SGPASPARE(self)
        self.zz_fdict['SGPASPARE'] = self.SGPASPARE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_IFPGACTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_IFPGACTRL1, self).__init__(rmio, label,
            0x40084000, 0x160,
            'IFPGACTRL1', 'RAC.IFPGACTRL1', 'read-write',
            u"",
            0x00000000, 0x00000001)

        self.LBWMODE = RM_Field_RAC_IFPGACTRL1_LBWMODE(self)
        self.zz_fdict['LBWMODE'] = self.LBWMODE
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_SGLNAMIXCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_SGLNAMIXCTRL1, self).__init__(rmio, label,
            0x40084000, 0x164,
            'SGLNAMIXCTRL1', 'RAC.SGLNAMIXCTRL1', 'read-write',
            u"",
            0x00000000, 0x0000003F)

        self.ENRFPKD = RM_Field_RAC_SGLNAMIXCTRL1_ENRFPKD(self)
        self.zz_fdict['ENRFPKD'] = self.ENRFPKD
        self.TRIMACC = RM_Field_RAC_SGLNAMIXCTRL1_TRIMACC(self)
        self.zz_fdict['TRIMACC'] = self.TRIMACC
        self.TRIMNPATHBW = RM_Field_RAC_SGLNAMIXCTRL1_TRIMNPATHBW(self)
        self.zz_fdict['TRIMNPATHBW'] = self.TRIMNPATHBW
        self.TRIMRESP = RM_Field_RAC_SGLNAMIXCTRL1_TRIMRESP(self)
        self.zz_fdict['TRIMRESP'] = self.TRIMRESP
        self.TRIMTHRESH = RM_Field_RAC_SGLNAMIXCTRL1_TRIMTHRESH(self)
        self.zz_fdict['TRIMTHRESH'] = self.TRIMTHRESH
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_CHPCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_CHPCTRL1, self).__init__(rmio, label,
            0x40084000, 0x168,
            'CHPCTRL1', 'RAC.CHPCTRL1', 'read-write',
            u"",
            0x00000004, 0x0000000F)

        self.BYPREPLDOTX = RM_Field_RAC_CHPCTRL1_BYPREPLDOTX(self)
        self.zz_fdict['BYPREPLDOTX'] = self.BYPREPLDOTX
        self.BYPREPLDORX = RM_Field_RAC_CHPCTRL1_BYPREPLDORX(self)
        self.zz_fdict['BYPREPLDORX'] = self.BYPREPLDORX
        self.TRIMREPLDO = RM_Field_RAC_CHPCTRL1_TRIMREPLDO(self)
        self.zz_fdict['TRIMREPLDO'] = self.TRIMREPLDO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_MMDCTRL1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_MMDCTRL1, self).__init__(rmio, label,
            0x40084000, 0x16C,
            'MMDCTRL1', 'RAC.MMDCTRL1', 'read-write',
            u"",
            0x00000004, 0x0000000F)

        self.BYPREPLDOTX = RM_Field_RAC_MMDCTRL1_BYPREPLDOTX(self)
        self.zz_fdict['BYPREPLDOTX'] = self.BYPREPLDOTX
        self.BYPREPLDORX = RM_Field_RAC_MMDCTRL1_BYPREPLDORX(self)
        self.zz_fdict['BYPREPLDORX'] = self.BYPREPLDORX
        self.TRIMREPLDO = RM_Field_RAC_MMDCTRL1_TRIMREPLDO(self)
        self.zz_fdict['TRIMREPLDO'] = self.TRIMREPLDO
        self.__dict__['zz_frozen'] = True


class RM_Register_RAC_STATUS2(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_RAC_STATUS2, self).__init__(rmio, label,
            0x40084000, 0x170,
            'STATUS2', 'RAC.STATUS2', 'read-only',
            u"",
            0x00000000, 0x00000FFF)

        self.PREVSTATE1 = RM_Field_RAC_STATUS2_PREVSTATE1(self)
        self.zz_fdict['PREVSTATE1'] = self.PREVSTATE1
        self.PREVSTATE2 = RM_Field_RAC_STATUS2_PREVSTATE2(self)
        self.zz_fdict['PREVSTATE2'] = self.PREVSTATE2
        self.PREVSTATE3 = RM_Field_RAC_STATUS2_PREVSTATE3(self)
        self.zz_fdict['PREVSTATE3'] = self.PREVSTATE3
        self.__dict__['zz_frozen'] = True


