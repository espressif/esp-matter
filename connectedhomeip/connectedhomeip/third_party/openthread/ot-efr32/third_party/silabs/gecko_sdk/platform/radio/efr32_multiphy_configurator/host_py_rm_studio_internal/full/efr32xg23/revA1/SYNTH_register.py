
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SYNTH_field import *


class RM_Register_SYNTH_IPVERSION(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IPVERSION, self).__init__(rmio, label,
            0xa8018000, 0x000,
            'IPVERSION', 'SYNTH.IPVERSION', 'read-only',
            u"",
            0x00000002, 0xFFFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IPVERSION = RM_Field_SYNTH_IPVERSION_IPVERSION(self)
        self.zz_fdict['IPVERSION'] = self.IPVERSION
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_EN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_EN, self).__init__(rmio, label,
            0xa8018000, 0x004,
            'EN', 'SYNTH.EN', 'read-write',
            u"",
            0x00000000, 0x00000001,
            0x00001000, 0x00002000,
            0x00003000)

        self.EN = RM_Field_SYNTH_EN_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_STATUS, self).__init__(rmio, label,
            0xa8018000, 0x008,
            'STATUS', 'SYNTH.STATUS', 'read-only',
            u"",
            0x00000000, 0x04014707,
            0x00001000, 0x00002000,
            0x00003000)

        self.INLOCK = RM_Field_SYNTH_STATUS_INLOCK(self)
        self.zz_fdict['INLOCK'] = self.INLOCK
        self.IFFREQEN = RM_Field_SYNTH_STATUS_IFFREQEN(self)
        self.zz_fdict['IFFREQEN'] = self.IFFREQEN
        self.SYNTHREADY = RM_Field_SYNTH_STATUS_SYNTHREADY(self)
        self.zz_fdict['SYNTHREADY'] = self.SYNTHREADY
        self.CAPCALRUNNING = RM_Field_SYNTH_STATUS_CAPCALRUNNING(self)
        self.zz_fdict['CAPCALRUNNING'] = self.CAPCALRUNNING
        self.CAPCALBITRESULT = RM_Field_SYNTH_STATUS_CAPCALBITRESULT(self)
        self.zz_fdict['CAPCALBITRESULT'] = self.CAPCALBITRESULT
        self.CAPCALFULLWAIT = RM_Field_SYNTH_STATUS_CAPCALFULLWAIT(self)
        self.zz_fdict['CAPCALFULLWAIT'] = self.CAPCALFULLWAIT
        self.CAPCALERROR = RM_Field_SYNTH_STATUS_CAPCALERROR(self)
        self.zz_fdict['CAPCALERROR'] = self.CAPCALERROR
        self.VCOFREQACTIVE = RM_Field_SYNTH_STATUS_VCOFREQACTIVE(self)
        self.zz_fdict['VCOFREQACTIVE'] = self.VCOFREQACTIVE
        self.CHPALLOWTRISTATE = RM_Field_SYNTH_STATUS_CHPALLOWTRISTATE(self)
        self.zz_fdict['CHPALLOWTRISTATE'] = self.CHPALLOWTRISTATE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CMD, self).__init__(rmio, label,
            0xa8018000, 0x00C,
            'CMD', 'SYNTH.CMD', 'write-only',
            u"",
            0x00000000, 0x0000061F,
            0x00001000, 0x00002000,
            0x00003000)

        self.SYNTHSTART = RM_Field_SYNTH_CMD_SYNTHSTART(self)
        self.zz_fdict['SYNTHSTART'] = self.SYNTHSTART
        self.SYNTHSTOP = RM_Field_SYNTH_CMD_SYNTHSTOP(self)
        self.zz_fdict['SYNTHSTOP'] = self.SYNTHSTOP
        self.ENABLEIF = RM_Field_SYNTH_CMD_ENABLEIF(self)
        self.zz_fdict['ENABLEIF'] = self.ENABLEIF
        self.DISABLEIF = RM_Field_SYNTH_CMD_DISABLEIF(self)
        self.zz_fdict['DISABLEIF'] = self.DISABLEIF
        self.CAPCALSTART = RM_Field_SYNTH_CMD_CAPCALSTART(self)
        self.zz_fdict['CAPCALSTART'] = self.CAPCALSTART
        self.VCOADDCAP = RM_Field_SYNTH_CMD_VCOADDCAP(self)
        self.zz_fdict['VCOADDCAP'] = self.VCOADDCAP
        self.VCOSUBCAP = RM_Field_SYNTH_CMD_VCOSUBCAP(self)
        self.zz_fdict['VCOSUBCAP'] = self.VCOSUBCAP
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CTRL, self).__init__(rmio, label,
            0xa8018000, 0x010,
            'CTRL', 'SYNTH.CTRL', 'read-write',
            u"",
            0x00000003, 0xD9F70007,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCKTHRESHOLD = RM_Field_SYNTH_CTRL_LOCKTHRESHOLD(self)
        self.zz_fdict['LOCKTHRESHOLD'] = self.LOCKTHRESHOLD
        self.PRSMUX0 = RM_Field_SYNTH_CTRL_PRSMUX0(self)
        self.zz_fdict['PRSMUX0'] = self.PRSMUX0
        self.PRSMUX1 = RM_Field_SYNTH_CTRL_PRSMUX1(self)
        self.zz_fdict['PRSMUX1'] = self.PRSMUX1
        self.DISCLKSYNTH = RM_Field_SYNTH_CTRL_DISCLKSYNTH(self)
        self.zz_fdict['DISCLKSYNTH'] = self.DISCLKSYNTH
        self.INVCLKSYNTH = RM_Field_SYNTH_CTRL_INVCLKSYNTH(self)
        self.zz_fdict['INVCLKSYNTH'] = self.INVCLKSYNTH
        self.TRISTATEPOSTPONE = RM_Field_SYNTH_CTRL_TRISTATEPOSTPONE(self)
        self.zz_fdict['TRISTATEPOSTPONE'] = self.TRISTATEPOSTPONE
        self.INTEGERMODE = RM_Field_SYNTH_CTRL_INTEGERMODE(self)
        self.zz_fdict['INTEGERMODE'] = self.INTEGERMODE
        self.MMDRSTNOVERRIDEEN = RM_Field_SYNTH_CTRL_MMDRSTNOVERRIDEEN(self)
        self.zz_fdict['MMDRSTNOVERRIDEEN'] = self.MMDRSTNOVERRIDEEN
        self.MMDMANRSTN = RM_Field_SYNTH_CTRL_MMDMANRSTN(self)
        self.zz_fdict['MMDMANRSTN'] = self.MMDMANRSTN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CALCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CALCTRL, self).__init__(rmio, label,
            0xa8018000, 0x014,
            'CALCTRL', 'SYNTH.CALCTRL', 'read-write',
            u"",
            0x00002801, 0x0F007FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.NUMCYCLES = RM_Field_SYNTH_CALCTRL_NUMCYCLES(self)
        self.zz_fdict['NUMCYCLES'] = self.NUMCYCLES
        self.CAPCALSIDE = RM_Field_SYNTH_CALCTRL_CAPCALSIDE(self)
        self.zz_fdict['CAPCALSIDE'] = self.CAPCALSIDE
        self.CAPCALENDMODE = RM_Field_SYNTH_CALCTRL_CAPCALENDMODE(self)
        self.zz_fdict['CAPCALENDMODE'] = self.CAPCALENDMODE
        self.CAPCALCYCLEWAIT = RM_Field_SYNTH_CALCTRL_CAPCALCYCLEWAIT(self)
        self.zz_fdict['CAPCALCYCLEWAIT'] = self.CAPCALCYCLEWAIT
        self.CAPCALWAITMODE = RM_Field_SYNTH_CALCTRL_CAPCALWAITMODE(self)
        self.zz_fdict['CAPCALWAITMODE'] = self.CAPCALWAITMODE
        self.CAPCALSINGLESTEP = RM_Field_SYNTH_CALCTRL_CAPCALSINGLESTEP(self)
        self.zz_fdict['CAPCALSINGLESTEP'] = self.CAPCALSINGLESTEP
        self.STARTUPTIMING = RM_Field_SYNTH_CALCTRL_STARTUPTIMING(self)
        self.zz_fdict['STARTUPTIMING'] = self.STARTUPTIMING
        self.SYNTHOVERRIDEEN = RM_Field_SYNTH_CALCTRL_SYNTHOVERRIDEEN(self)
        self.zz_fdict['SYNTHOVERRIDEEN'] = self.SYNTHOVERRIDEEN
        self.CLKLOADDENOMVAL = RM_Field_SYNTH_CALCTRL_CLKLOADDENOMVAL(self)
        self.zz_fdict['CLKLOADDENOMVAL'] = self.CLKLOADDENOMVAL
        self.PRESCALERRESETVAL = RM_Field_SYNTH_CALCTRL_PRESCALERRESETVAL(self)
        self.zz_fdict['PRESCALERRESETVAL'] = self.PRESCALERRESETVAL
        self.COUNTERRESETVAL = RM_Field_SYNTH_CALCTRL_COUNTERRESETVAL(self)
        self.zz_fdict['COUNTERRESETVAL'] = self.COUNTERRESETVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCDACCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCDACCTRL, self).__init__(rmio, label,
            0xa8018000, 0x02C,
            'VCDACCTRL', 'SYNTH.VCDACCTRL', 'read-write',
            u"",
            0x00000020, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCDACVAL = RM_Field_SYNTH_VCDACCTRL_VCDACVAL(self)
        self.zz_fdict['VCDACVAL'] = self.VCDACVAL
        self.VCDACEN = RM_Field_SYNTH_VCDACCTRL_VCDACEN(self)
        self.zz_fdict['VCDACEN'] = self.VCDACEN
        self.LPFEN = RM_Field_SYNTH_VCDACCTRL_LPFEN(self)
        self.zz_fdict['LPFEN'] = self.LPFEN
        self.LPFQSEN = RM_Field_SYNTH_VCDACCTRL_LPFQSEN(self)
        self.zz_fdict['LPFQSEN'] = self.LPFQSEN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_FREQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_FREQ, self).__init__(rmio, label,
            0xa8018000, 0x034,
            'FREQ', 'SYNTH.FREQ', 'read-write',
            u"",
            0x00000000, 0x0FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.FREQ = RM_Field_SYNTH_FREQ_FREQ(self)
        self.zz_fdict['FREQ'] = self.FREQ
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IFFREQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IFFREQ, self).__init__(rmio, label,
            0xa8018000, 0x038,
            'IFFREQ', 'SYNTH.IFFREQ', 'read-write',
            u"",
            0x00000000, 0x001FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.IFFREQ = RM_Field_SYNTH_IFFREQ_IFFREQ(self)
        self.zz_fdict['IFFREQ'] = self.IFFREQ
        self.LOSIDE = RM_Field_SYNTH_IFFREQ_LOSIDE(self)
        self.zz_fdict['LOSIDE'] = self.LOSIDE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_DIVCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_DIVCTRL, self).__init__(rmio, label,
            0xa8018000, 0x03C,
            'DIVCTRL', 'SYNTH.DIVCTRL', 'read-write',
            u"",
            0x00000001, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LODIVFREQCTRL = RM_Field_SYNTH_DIVCTRL_LODIVFREQCTRL(self)
        self.zz_fdict['LODIVFREQCTRL'] = self.LODIVFREQCTRL
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHCTRL, self).__init__(rmio, label,
            0xa8018000, 0x040,
            'CHCTRL', 'SYNTH.CHCTRL', 'read-write',
            u"",
            0x00000000, 0x0000003F,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHNO = RM_Field_SYNTH_CHCTRL_CHNO(self)
        self.zz_fdict['CHNO'] = self.CHNO
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHSP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHSP, self).__init__(rmio, label,
            0xa8018000, 0x044,
            'CHSP', 'SYNTH.CHSP', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHSP = RM_Field_SYNTH_CHSP_CHSP(self)
        self.zz_fdict['CHSP'] = self.CHSP
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CALOFFSET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CALOFFSET, self).__init__(rmio, label,
            0xa8018000, 0x048,
            'CALOFFSET', 'SYNTH.CALOFFSET', 'read-write',
            u"",
            0x00000000, 0x00007FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CALOFFSET = RM_Field_SYNTH_CALOFFSET_CALOFFSET(self)
        self.zz_fdict['CALOFFSET'] = self.CALOFFSET
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCOTUNING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCOTUNING, self).__init__(rmio, label,
            0xa8018000, 0x04C,
            'VCOTUNING', 'SYNTH.VCOTUNING', 'read-write',
            u"",
            0x00008400, 0x0000FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCOTUNING = RM_Field_SYNTH_VCOTUNING_VCOTUNING(self)
        self.zz_fdict['VCOTUNING'] = self.VCOTUNING
        self.VCAPSEL = RM_Field_SYNTH_VCOTUNING_VCAPSEL(self)
        self.zz_fdict['VCAPSEL'] = self.VCAPSEL
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCORANGE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCORANGE, self).__init__(rmio, label,
            0xa8018000, 0x054,
            'VCORANGE', 'SYNTH.VCORANGE', 'read-write',
            u"",
            0x00000000, 0x000FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODE = RM_Field_SYNTH_VCORANGE_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.SWITCHMODE = RM_Field_SYNTH_VCORANGE_SWITCHMODE(self)
        self.zz_fdict['SWITCHMODE'] = self.SWITCHMODE
        self.HIGHTHRESH = RM_Field_SYNTH_VCORANGE_HIGHTHRESH(self)
        self.zz_fdict['HIGHTHRESH'] = self.HIGHTHRESH
        self.LOWTHRESH = RM_Field_SYNTH_VCORANGE_LOWTHRESH(self)
        self.zz_fdict['LOWTHRESH'] = self.LOWTHRESH
        self.OVERRIDEEN = RM_Field_SYNTH_VCORANGE_OVERRIDEEN(self)
        self.zz_fdict['OVERRIDEEN'] = self.OVERRIDEEN
        self.DATA = RM_Field_SYNTH_VCORANGE_DATA(self)
        self.zz_fdict['DATA'] = self.DATA
        self.LOAD = RM_Field_SYNTH_VCORANGE_LOAD(self)
        self.zz_fdict['LOAD'] = self.LOAD
        self.RAMPEN = RM_Field_SYNTH_VCORANGE_RAMPEN(self)
        self.zz_fdict['RAMPEN'] = self.RAMPEN
        self.CMPENABLE = RM_Field_SYNTH_VCORANGE_CMPENABLE(self)
        self.zz_fdict['CMPENABLE'] = self.CMPENABLE
        self.CMPRESET = RM_Field_SYNTH_VCORANGE_CMPRESET(self)
        self.zz_fdict['CMPRESET'] = self.CMPRESET
        self.RAMPRATE = RM_Field_SYNTH_VCORANGE_RAMPRATE(self)
        self.zz_fdict['RAMPRATE'] = self.RAMPRATE
        self.THROUGH = RM_Field_SYNTH_VCORANGE_THROUGH(self)
        self.zz_fdict['THROUGH'] = self.THROUGH
        self.INVCOMPDIR = RM_Field_SYNTH_VCORANGE_INVCOMPDIR(self)
        self.zz_fdict['INVCOMPDIR'] = self.INVCOMPDIR
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCOGAIN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCOGAIN, self).__init__(rmio, label,
            0xa8018000, 0x058,
            'VCOGAIN', 'SYNTH.VCOGAIN', 'read-write',
            u"",
            0x00000077, 0x000000FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.VCOKVCOARSE = RM_Field_SYNTH_VCOGAIN_VCOKVCOARSE(self)
        self.zz_fdict['VCOKVCOARSE'] = self.VCOKVCOARSE
        self.VCOKVFINE = RM_Field_SYNTH_VCOGAIN_VCOKVFINE(self)
        self.zz_fdict['VCOKVFINE'] = self.VCOKVFINE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHPDACCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHPDACCTRL, self).__init__(rmio, label,
            0xa8018000, 0x068,
            'CHPDACCTRL', 'SYNTH.CHPDACCTRL', 'read-write',
            u"",
            0x00000000, 0x000001FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CHPDACVAL = RM_Field_SYNTH_CHPDACCTRL_CHPDACVAL(self)
        self.zz_fdict['CHPDACVAL'] = self.CHPDACVAL
        self.OVERRIDE = RM_Field_SYNTH_CHPDACCTRL_OVERRIDE(self)
        self.zz_fdict['OVERRIDE'] = self.OVERRIDE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CAPCALCYCLECNT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CAPCALCYCLECNT, self).__init__(rmio, label,
            0xa8018000, 0x06C,
            'CAPCALCYCLECNT', 'SYNTH.CAPCALCYCLECNT', 'read-only',
            u"",
            0x00000000, 0x000003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.CAPCALCYCLECNT = RM_Field_SYNTH_CAPCALCYCLECNT_CAPCALCYCLECNT(self)
        self.zz_fdict['CAPCALCYCLECNT'] = self.CAPCALCYCLECNT
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCOFRCCAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCOFRCCAL, self).__init__(rmio, label,
            0xa8018000, 0x070,
            'VCOFRCCAL', 'SYNTH.VCOFRCCAL', 'read-write',
            u"",
            0x01FF0FFF, 0x3FFF3FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TIMEOUTCOUNTMAX = RM_Field_SYNTH_VCOFRCCAL_TIMEOUTCOUNTMAX(self)
        self.zz_fdict['TIMEOUTCOUNTMAX'] = self.TIMEOUTCOUNTMAX
        self.TIMEOUTCOUNTRESTART = RM_Field_SYNTH_VCOFRCCAL_TIMEOUTCOUNTRESTART(self)
        self.zz_fdict['TIMEOUTCOUNTRESTART'] = self.TIMEOUTCOUNTRESTART
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IF, self).__init__(rmio, label,
            0xa8018000, 0x078,
            'IF', 'SYNTH.IF', 'read-write',
            u"",
            0x00000000, 0x00000237,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCKED = RM_Field_SYNTH_IF_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_IF_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.SYRDY = RM_Field_SYNTH_IF_SYRDY(self)
        self.zz_fdict['SYRDY'] = self.SYRDY
        self.VCOHIGH = RM_Field_SYNTH_IF_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_IF_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.LOCNTDONE = RM_Field_SYNTH_IF_LOCNTDONE(self)
        self.zz_fdict['LOCNTDONE'] = self.LOCNTDONE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IEN, self).__init__(rmio, label,
            0xa8018000, 0x084,
            'IEN', 'SYNTH.IEN', 'read-write',
            u"",
            0x00000000, 0x00000237,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCKED = RM_Field_SYNTH_IEN_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_IEN_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.SYRDY = RM_Field_SYNTH_IEN_SYRDY(self)
        self.zz_fdict['SYRDY'] = self.SYRDY
        self.VCOHIGH = RM_Field_SYNTH_IEN_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_IEN_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.LOCNTDONE = RM_Field_SYNTH_IEN_LOCNTDONE(self)
        self.zz_fdict['LOCNTDONE'] = self.LOCNTDONE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LOCNTCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LOCNTCTRL, self).__init__(rmio, label,
            0xa8018000, 0x088,
            'LOCNTCTRL', 'SYNTH.LOCNTCTRL', 'read-write',
            u"",
            0x00000000, 0x00000FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.ENABLE = RM_Field_SYNTH_LOCNTCTRL_ENABLE(self)
        self.zz_fdict['ENABLE'] = self.ENABLE
        self.CLEAR = RM_Field_SYNTH_LOCNTCTRL_CLEAR(self)
        self.zz_fdict['CLEAR'] = self.CLEAR
        self.RUN = RM_Field_SYNTH_LOCNTCTRL_RUN(self)
        self.zz_fdict['RUN'] = self.RUN
        self.READ = RM_Field_SYNTH_LOCNTCTRL_READ(self)
        self.zz_fdict['READ'] = self.READ
        self.NUMCYCLE = RM_Field_SYNTH_LOCNTCTRL_NUMCYCLE(self)
        self.zz_fdict['NUMCYCLE'] = self.NUMCYCLE
        self.LOCNTOVERRIDEEN = RM_Field_SYNTH_LOCNTCTRL_LOCNTOVERRIDEEN(self)
        self.zz_fdict['LOCNTOVERRIDEEN'] = self.LOCNTOVERRIDEEN
        self.LOCNTMANCLEAR = RM_Field_SYNTH_LOCNTCTRL_LOCNTMANCLEAR(self)
        self.zz_fdict['LOCNTMANCLEAR'] = self.LOCNTMANCLEAR
        self.LOCNTMANRUN = RM_Field_SYNTH_LOCNTCTRL_LOCNTMANRUN(self)
        self.zz_fdict['LOCNTMANRUN'] = self.LOCNTMANRUN
        self.FCALRUNCLKEN = RM_Field_SYNTH_LOCNTCTRL_FCALRUNCLKEN(self)
        self.zz_fdict['FCALRUNCLKEN'] = self.FCALRUNCLKEN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LOCNTSTATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LOCNTSTATUS, self).__init__(rmio, label,
            0xa8018000, 0x08C,
            'LOCNTSTATUS', 'SYNTH.LOCNTSTATUS', 'read-only',
            u"",
            0x00000000, 0x000FFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCOUNT = RM_Field_SYNTH_LOCNTSTATUS_LOCOUNT(self)
        self.zz_fdict['LOCOUNT'] = self.LOCOUNT
        self.BUSY = RM_Field_SYNTH_LOCNTSTATUS_BUSY(self)
        self.zz_fdict['BUSY'] = self.BUSY
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LOCNTTARGET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LOCNTTARGET, self).__init__(rmio, label,
            0xa8018000, 0x090,
            'LOCNTTARGET', 'SYNTH.LOCNTTARGET', 'read-only',
            u"",
            0x00000000, 0x0007FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.TARGET = RM_Field_SYNTH_LOCNTTARGET_TARGET(self)
        self.zz_fdict['TARGET'] = self.TARGET
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_MMDDENOMINIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_MMDDENOMINIT, self).__init__(rmio, label,
            0xa8018000, 0x094,
            'MMDDENOMINIT', 'SYNTH.MMDDENOMINIT', 'read-write',
            u"",
            0x00000000, 0x07FFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DENOMINIT0 = RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT0(self)
        self.zz_fdict['DENOMINIT0'] = self.DENOMINIT0
        self.DENOMINIT1 = RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT1(self)
        self.zz_fdict['DENOMINIT1'] = self.DENOMINIT1
        self.DENOMINIT2 = RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT2(self)
        self.zz_fdict['DENOMINIT2'] = self.DENOMINIT2
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHPDACINIT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHPDACINIT, self).__init__(rmio, label,
            0xa8018000, 0x098,
            'CHPDACINIT', 'SYNTH.CHPDACINIT', 'read-write',
            u"",
            0x00000000, 0x00000FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DACINIT = RM_Field_SYNTH_CHPDACINIT_DACINIT(self)
        self.zz_fdict['DACINIT'] = self.DACINIT
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LPFCTRL1CAL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LPFCTRL1CAL, self).__init__(rmio, label,
            0xa8018000, 0x09C,
            'LPFCTRL1CAL', 'SYNTH.LPFCTRL1CAL', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.OP1BWCAL = RM_Field_SYNTH_LPFCTRL1CAL_OP1BWCAL(self)
        self.zz_fdict['OP1BWCAL'] = self.OP1BWCAL
        self.OP1COMPCAL = RM_Field_SYNTH_LPFCTRL1CAL_OP1COMPCAL(self)
        self.zz_fdict['OP1COMPCAL'] = self.OP1COMPCAL
        self.RFBVALCAL = RM_Field_SYNTH_LPFCTRL1CAL_RFBVALCAL(self)
        self.zz_fdict['RFBVALCAL'] = self.RFBVALCAL
        self.RPVALCAL = RM_Field_SYNTH_LPFCTRL1CAL_RPVALCAL(self)
        self.zz_fdict['RPVALCAL'] = self.RPVALCAL
        self.RZVALCAL = RM_Field_SYNTH_LPFCTRL1CAL_RZVALCAL(self)
        self.zz_fdict['RZVALCAL'] = self.RZVALCAL
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LPFCTRL1RX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LPFCTRL1RX, self).__init__(rmio, label,
            0xa8018000, 0x0A0,
            'LPFCTRL1RX', 'SYNTH.LPFCTRL1RX', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.OP1BWRX = RM_Field_SYNTH_LPFCTRL1RX_OP1BWRX(self)
        self.zz_fdict['OP1BWRX'] = self.OP1BWRX
        self.OP1COMPRX = RM_Field_SYNTH_LPFCTRL1RX_OP1COMPRX(self)
        self.zz_fdict['OP1COMPRX'] = self.OP1COMPRX
        self.RFBVALRX = RM_Field_SYNTH_LPFCTRL1RX_RFBVALRX(self)
        self.zz_fdict['RFBVALRX'] = self.RFBVALRX
        self.RPVALRX = RM_Field_SYNTH_LPFCTRL1RX_RPVALRX(self)
        self.zz_fdict['RPVALRX'] = self.RPVALRX
        self.RZVALRX = RM_Field_SYNTH_LPFCTRL1RX_RZVALRX(self)
        self.zz_fdict['RZVALRX'] = self.RZVALRX
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LPFCTRL1TX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LPFCTRL1TX, self).__init__(rmio, label,
            0xa8018000, 0x0A4,
            'LPFCTRL1TX', 'SYNTH.LPFCTRL1TX', 'read-write',
            u"",
            0x00000000, 0x0003FFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.OP1BWTX = RM_Field_SYNTH_LPFCTRL1TX_OP1BWTX(self)
        self.zz_fdict['OP1BWTX'] = self.OP1BWTX
        self.OP1COMPTX = RM_Field_SYNTH_LPFCTRL1TX_OP1COMPTX(self)
        self.zz_fdict['OP1COMPTX'] = self.OP1COMPTX
        self.RFBVALTX = RM_Field_SYNTH_LPFCTRL1TX_RFBVALTX(self)
        self.zz_fdict['RFBVALTX'] = self.RFBVALTX
        self.RPVALTX = RM_Field_SYNTH_LPFCTRL1TX_RPVALTX(self)
        self.zz_fdict['RPVALTX'] = self.RPVALTX
        self.RZVALTX = RM_Field_SYNTH_LPFCTRL1TX_RZVALTX(self)
        self.zz_fdict['RZVALTX'] = self.RZVALTX
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LPFCTRL2RX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LPFCTRL2RX, self).__init__(rmio, label,
            0xa8018000, 0x0A8,
            'LPFCTRL2RX', 'SYNTH.LPFCTRL2RX', 'read-write',
            u"",
            0x00000000, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LPFSWENRX = RM_Field_SYNTH_LPFCTRL2RX_LPFSWENRX(self)
        self.zz_fdict['LPFSWENRX'] = self.LPFSWENRX
        self.LPFINCAPRX = RM_Field_SYNTH_LPFCTRL2RX_LPFINCAPRX(self)
        self.zz_fdict['LPFINCAPRX'] = self.LPFINCAPRX
        self.LPFGNDSWENRX = RM_Field_SYNTH_LPFCTRL2RX_LPFGNDSWENRX(self)
        self.zz_fdict['LPFGNDSWENRX'] = self.LPFGNDSWENRX
        self.CALCRX = RM_Field_SYNTH_LPFCTRL2RX_CALCRX(self)
        self.zz_fdict['CALCRX'] = self.CALCRX
        self.CASELRX = RM_Field_SYNTH_LPFCTRL2RX_CASELRX(self)
        self.zz_fdict['CASELRX'] = self.CASELRX
        self.CAVALRX = RM_Field_SYNTH_LPFCTRL2RX_CAVALRX(self)
        self.zz_fdict['CAVALRX'] = self.CAVALRX
        self.CFBSELRX = RM_Field_SYNTH_LPFCTRL2RX_CFBSELRX(self)
        self.zz_fdict['CFBSELRX'] = self.CFBSELRX
        self.CZSELRX = RM_Field_SYNTH_LPFCTRL2RX_CZSELRX(self)
        self.zz_fdict['CZSELRX'] = self.CZSELRX
        self.CZVALRX = RM_Field_SYNTH_LPFCTRL2RX_CZVALRX(self)
        self.zz_fdict['CZVALRX'] = self.CZVALRX
        self.MODESELRX = RM_Field_SYNTH_LPFCTRL2RX_MODESELRX(self)
        self.zz_fdict['MODESELRX'] = self.MODESELRX
        self.VCMLVLRX = RM_Field_SYNTH_LPFCTRL2RX_VCMLVLRX(self)
        self.zz_fdict['VCMLVLRX'] = self.VCMLVLRX
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_LPFCTRL2TX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_LPFCTRL2TX, self).__init__(rmio, label,
            0xa8018000, 0x0AC,
            'LPFCTRL2TX', 'SYNTH.LPFCTRL2TX', 'read-write',
            u"",
            0x00000000, 0x1FFFFFFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.LPFSWENTX = RM_Field_SYNTH_LPFCTRL2TX_LPFSWENTX(self)
        self.zz_fdict['LPFSWENTX'] = self.LPFSWENTX
        self.LPFINCAPTX = RM_Field_SYNTH_LPFCTRL2TX_LPFINCAPTX(self)
        self.zz_fdict['LPFINCAPTX'] = self.LPFINCAPTX
        self.LPFGNDSWENTX = RM_Field_SYNTH_LPFCTRL2TX_LPFGNDSWENTX(self)
        self.zz_fdict['LPFGNDSWENTX'] = self.LPFGNDSWENTX
        self.CALCTX = RM_Field_SYNTH_LPFCTRL2TX_CALCTX(self)
        self.zz_fdict['CALCTX'] = self.CALCTX
        self.CASELTX = RM_Field_SYNTH_LPFCTRL2TX_CASELTX(self)
        self.zz_fdict['CASELTX'] = self.CASELTX
        self.CAVALTX = RM_Field_SYNTH_LPFCTRL2TX_CAVALTX(self)
        self.zz_fdict['CAVALTX'] = self.CAVALTX
        self.CFBSELTX = RM_Field_SYNTH_LPFCTRL2TX_CFBSELTX(self)
        self.zz_fdict['CFBSELTX'] = self.CFBSELTX
        self.CZSELTX = RM_Field_SYNTH_LPFCTRL2TX_CZSELTX(self)
        self.zz_fdict['CZSELTX'] = self.CZSELTX
        self.CZVALTX = RM_Field_SYNTH_LPFCTRL2TX_CZVALTX(self)
        self.zz_fdict['CZVALTX'] = self.CZVALTX
        self.MODESELTX = RM_Field_SYNTH_LPFCTRL2TX_MODESELTX(self)
        self.zz_fdict['MODESELTX'] = self.MODESELTX
        self.VCMLVLTX = RM_Field_SYNTH_LPFCTRL2TX_VCMLVLTX(self)
        self.zz_fdict['VCMLVLTX'] = self.VCMLVLTX
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_DSMCTRLRX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_DSMCTRLRX, self).__init__(rmio, label,
            0xa8018000, 0x0B0,
            'DSMCTRLRX', 'SYNTH.DSMCTRLRX', 'read-write',
            u"",
            0x00000013, 0x070003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DITHERDSMINPUTRX = RM_Field_SYNTH_DSMCTRLRX_DITHERDSMINPUTRX(self)
        self.zz_fdict['DITHERDSMINPUTRX'] = self.DITHERDSMINPUTRX
        self.DITHERDSMOUTPUTRX = RM_Field_SYNTH_DSMCTRLRX_DITHERDSMOUTPUTRX(self)
        self.zz_fdict['DITHERDSMOUTPUTRX'] = self.DITHERDSMOUTPUTRX
        self.DITHERDACRX = RM_Field_SYNTH_DSMCTRLRX_DITHERDACRX(self)
        self.zz_fdict['DITHERDACRX'] = self.DITHERDACRX
        self.DSMMODERX = RM_Field_SYNTH_DSMCTRLRX_DSMMODERX(self)
        self.zz_fdict['DSMMODERX'] = self.DSMMODERX
        self.LSBFORCERX = RM_Field_SYNTH_DSMCTRLRX_LSBFORCERX(self)
        self.zz_fdict['LSBFORCERX'] = self.LSBFORCERX
        self.DEMMODERX = RM_Field_SYNTH_DSMCTRLRX_DEMMODERX(self)
        self.zz_fdict['DEMMODERX'] = self.DEMMODERX
        self.MASHORDERRX = RM_Field_SYNTH_DSMCTRLRX_MASHORDERRX(self)
        self.zz_fdict['MASHORDERRX'] = self.MASHORDERRX
        self.REQORDERRX = RM_Field_SYNTH_DSMCTRLRX_REQORDERRX(self)
        self.zz_fdict['REQORDERRX'] = self.REQORDERRX
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_DSMCTRLTX(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_DSMCTRLTX, self).__init__(rmio, label,
            0xa8018000, 0x0B4,
            'DSMCTRLTX', 'SYNTH.DSMCTRLTX', 'read-write',
            u"",
            0x00000013, 0x070003FF,
            0x00001000, 0x00002000,
            0x00003000)

        self.DITHERDSMINPUTTX = RM_Field_SYNTH_DSMCTRLTX_DITHERDSMINPUTTX(self)
        self.zz_fdict['DITHERDSMINPUTTX'] = self.DITHERDSMINPUTTX
        self.DITHERDSMOUTPUTTX = RM_Field_SYNTH_DSMCTRLTX_DITHERDSMOUTPUTTX(self)
        self.zz_fdict['DITHERDSMOUTPUTTX'] = self.DITHERDSMOUTPUTTX
        self.DITHERDACTX = RM_Field_SYNTH_DSMCTRLTX_DITHERDACTX(self)
        self.zz_fdict['DITHERDACTX'] = self.DITHERDACTX
        self.DSMMODETX = RM_Field_SYNTH_DSMCTRLTX_DSMMODETX(self)
        self.zz_fdict['DSMMODETX'] = self.DSMMODETX
        self.LSBFORCETX = RM_Field_SYNTH_DSMCTRLTX_LSBFORCETX(self)
        self.zz_fdict['LSBFORCETX'] = self.LSBFORCETX
        self.DEMMODETX = RM_Field_SYNTH_DSMCTRLTX_DEMMODETX(self)
        self.zz_fdict['DEMMODETX'] = self.DEMMODETX
        self.MASHORDERTX = RM_Field_SYNTH_DSMCTRLTX_MASHORDERTX(self)
        self.zz_fdict['MASHORDERTX'] = self.MASHORDERTX
        self.REQORDERTX = RM_Field_SYNTH_DSMCTRLTX_REQORDERTX(self)
        self.zz_fdict['REQORDERTX'] = self.REQORDERTX
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_SEQIF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_SEQIF, self).__init__(rmio, label,
            0xa8018000, 0x0B8,
            'SEQIF', 'SYNTH.SEQIF', 'read-write',
            u"",
            0x00000000, 0x00000237,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCKED = RM_Field_SYNTH_SEQIF_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_SEQIF_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.SYRDY = RM_Field_SYNTH_SEQIF_SYRDY(self)
        self.zz_fdict['SYRDY'] = self.SYRDY
        self.VCOHIGH = RM_Field_SYNTH_SEQIF_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_SEQIF_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.LOCNTDONE = RM_Field_SYNTH_SEQIF_LOCNTDONE(self)
        self.zz_fdict['LOCNTDONE'] = self.LOCNTDONE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_SEQIEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_SEQIEN, self).__init__(rmio, label,
            0xa8018000, 0x0BC,
            'SEQIEN', 'SYNTH.SEQIEN', 'read-write',
            u"",
            0x00000000, 0x00000237,
            0x00001000, 0x00002000,
            0x00003000)

        self.LOCKED = RM_Field_SYNTH_SEQIEN_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_SEQIEN_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.SYRDY = RM_Field_SYNTH_SEQIEN_SYRDY(self)
        self.zz_fdict['SYRDY'] = self.SYRDY
        self.VCOHIGH = RM_Field_SYNTH_SEQIEN_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_SEQIEN_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.LOCNTDONE = RM_Field_SYNTH_SEQIEN_LOCNTDONE(self)
        self.zz_fdict['LOCNTDONE'] = self.LOCNTDONE
        self.__dict__['zz_frozen'] = True


