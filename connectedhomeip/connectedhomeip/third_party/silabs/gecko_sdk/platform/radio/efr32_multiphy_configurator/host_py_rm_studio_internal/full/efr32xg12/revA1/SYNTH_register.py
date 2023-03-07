
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SYNTH_field import *


class RM_Register_SYNTH_STATUS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_STATUS, self).__init__(rmio, label,
            0x40083000, 0x000,
            'STATUS', 'SYNTH.STATUS', 'read-only',
            u"",
            0x00000000, 0x057FFF37)

        self.INLOCK = RM_Field_SYNTH_STATUS_INLOCK(self)
        self.zz_fdict['INLOCK'] = self.INLOCK
        self.IFFREQEN = RM_Field_SYNTH_STATUS_IFFREQEN(self)
        self.zz_fdict['IFFREQEN'] = self.IFFREQEN
        self.SYNTHREADY = RM_Field_SYNTH_STATUS_SYNTHREADY(self)
        self.zz_fdict['SYNTHREADY'] = self.SYNTHREADY
        self.AUXINLOCK = RM_Field_SYNTH_STATUS_AUXINLOCK(self)
        self.zz_fdict['AUXINLOCK'] = self.AUXINLOCK
        self.AUXRUNNING = RM_Field_SYNTH_STATUS_AUXRUNNING(self)
        self.zz_fdict['AUXRUNNING'] = self.AUXRUNNING
        self.CAPCALRUNNING = RM_Field_SYNTH_STATUS_CAPCALRUNNING(self)
        self.zz_fdict['CAPCALRUNNING'] = self.CAPCALRUNNING
        self.CAPCALBITRESULT = RM_Field_SYNTH_STATUS_CAPCALBITRESULT(self)
        self.zz_fdict['CAPCALBITRESULT'] = self.CAPCALBITRESULT
        self.CAPCALFULLWAIT = RM_Field_SYNTH_STATUS_CAPCALFULLWAIT(self)
        self.zz_fdict['CAPCALFULLWAIT'] = self.CAPCALFULLWAIT
        self.AUXCAPCALRUNNING = RM_Field_SYNTH_STATUS_AUXCAPCALRUNNING(self)
        self.zz_fdict['AUXCAPCALRUNNING'] = self.AUXCAPCALRUNNING
        self.AUXCALBITRESULT = RM_Field_SYNTH_STATUS_AUXCALBITRESULT(self)
        self.zz_fdict['AUXCALBITRESULT'] = self.AUXCALBITRESULT
        self.AUXCALFULLWAIT = RM_Field_SYNTH_STATUS_AUXCALFULLWAIT(self)
        self.zz_fdict['AUXCALFULLWAIT'] = self.AUXCALFULLWAIT
        self.CAPCALERROR = RM_Field_SYNTH_STATUS_CAPCALERROR(self)
        self.zz_fdict['CAPCALERROR'] = self.CAPCALERROR
        self.AUXCALERROR = RM_Field_SYNTH_STATUS_AUXCALERROR(self)
        self.zz_fdict['AUXCALERROR'] = self.AUXCALERROR
        self.VCOFREQACTIVE = RM_Field_SYNTH_STATUS_VCOFREQACTIVE(self)
        self.zz_fdict['VCOFREQACTIVE'] = self.VCOFREQACTIVE
        self.VCOVARBANK = RM_Field_SYNTH_STATUS_VCOVARBANK(self)
        self.zz_fdict['VCOVARBANK'] = self.VCOVARBANK
        self.VCOAMPOK = RM_Field_SYNTH_STATUS_VCOAMPOK(self)
        self.zz_fdict['VCOAMPOK'] = self.VCOAMPOK
        self.CHPALLOWTRISTATE = RM_Field_SYNTH_STATUS_CHPALLOWTRISTATE(self)
        self.zz_fdict['CHPALLOWTRISTATE'] = self.CHPALLOWTRISTATE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CMD(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CMD, self).__init__(rmio, label,
            0x40083000, 0x004,
            'CMD', 'SYNTH.CMD', 'write-only',
            u"",
            0x00000000, 0x000007DF)

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
        self.AUXSTART = RM_Field_SYNTH_CMD_AUXSTART(self)
        self.zz_fdict['AUXSTART'] = self.AUXSTART
        self.AUXSTOP = RM_Field_SYNTH_CMD_AUXSTOP(self)
        self.zz_fdict['AUXSTOP'] = self.AUXSTOP
        self.AUXCAPCALSTART = RM_Field_SYNTH_CMD_AUXCAPCALSTART(self)
        self.zz_fdict['AUXCAPCALSTART'] = self.AUXCAPCALSTART
        self.VCOUP = RM_Field_SYNTH_CMD_VCOUP(self)
        self.zz_fdict['VCOUP'] = self.VCOUP
        self.VCODOWN = RM_Field_SYNTH_CMD_VCODOWN(self)
        self.zz_fdict['VCODOWN'] = self.VCODOWN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CTRL, self).__init__(rmio, label,
            0x40083000, 0x008,
            'CTRL', 'SYNTH.CTRL', 'read-write',
            u"",
            0x0000AC13, 0xE177FFFF)

        self.DITHERDSMINPUT = RM_Field_SYNTH_CTRL_DITHERDSMINPUT(self)
        self.zz_fdict['DITHERDSMINPUT'] = self.DITHERDSMINPUT
        self.DITHERDSMOUTPUT = RM_Field_SYNTH_CTRL_DITHERDSMOUTPUT(self)
        self.zz_fdict['DITHERDSMOUTPUT'] = self.DITHERDSMOUTPUT
        self.DITHERDAC = RM_Field_SYNTH_CTRL_DITHERDAC(self)
        self.zz_fdict['DITHERDAC'] = self.DITHERDAC
        self.DSMMODE = RM_Field_SYNTH_CTRL_DSMMODE(self)
        self.zz_fdict['DSMMODE'] = self.DSMMODE
        self.LSBFORCE = RM_Field_SYNTH_CTRL_LSBFORCE(self)
        self.zz_fdict['LSBFORCE'] = self.LSBFORCE
        self.LOCKTHRESHOLD = RM_Field_SYNTH_CTRL_LOCKTHRESHOLD(self)
        self.zz_fdict['LOCKTHRESHOLD'] = self.LOCKTHRESHOLD
        self.AUXLOCKTHRESHOLD = RM_Field_SYNTH_CTRL_AUXLOCKTHRESHOLD(self)
        self.zz_fdict['AUXLOCKTHRESHOLD'] = self.AUXLOCKTHRESHOLD
        self.PRSMUX0 = RM_Field_SYNTH_CTRL_PRSMUX0(self)
        self.zz_fdict['PRSMUX0'] = self.PRSMUX0
        self.PRSMUX1 = RM_Field_SYNTH_CTRL_PRSMUX1(self)
        self.zz_fdict['PRSMUX1'] = self.PRSMUX1
        self.DEMMODE = RM_Field_SYNTH_CTRL_DEMMODE(self)
        self.zz_fdict['DEMMODE'] = self.DEMMODE
        self.TRISTATEPOSTPONE = RM_Field_SYNTH_CTRL_TRISTATEPOSTPONE(self)
        self.zz_fdict['TRISTATEPOSTPONE'] = self.TRISTATEPOSTPONE
        self.INTEGERMODE = RM_Field_SYNTH_CTRL_INTEGERMODE(self)
        self.zz_fdict['INTEGERMODE'] = self.INTEGERMODE
        self.MMDSCANTESTEN = RM_Field_SYNTH_CTRL_MMDSCANTESTEN(self)
        self.zz_fdict['MMDSCANTESTEN'] = self.MMDSCANTESTEN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CALCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CALCTRL, self).__init__(rmio, label,
            0x40083000, 0x00C,
            'CALCTRL', 'SYNTH.CALCTRL', 'read-write',
            u"",
            0x00042801, 0x3F7F7FFF)

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
        self.AUXCALCYCLES = RM_Field_SYNTH_CALCTRL_AUXCALCYCLES(self)
        self.zz_fdict['AUXCALCYCLES'] = self.AUXCALCYCLES
        self.AUXCALACCURACY = RM_Field_SYNTH_CALCTRL_AUXCALACCURACY(self)
        self.zz_fdict['AUXCALACCURACY'] = self.AUXCALACCURACY
        self.AUXCALCYCLEWAIT = RM_Field_SYNTH_CALCTRL_AUXCALCYCLEWAIT(self)
        self.zz_fdict['AUXCALCYCLEWAIT'] = self.AUXCALCYCLEWAIT
        self.AUXCALSINGLESTEP = RM_Field_SYNTH_CALCTRL_AUXCALSINGLESTEP(self)
        self.zz_fdict['AUXCALSINGLESTEP'] = self.AUXCALSINGLESTEP
        self.SYNTHOVERRIDEEN = RM_Field_SYNTH_CALCTRL_SYNTHOVERRIDEEN(self)
        self.zz_fdict['SYNTHOVERRIDEEN'] = self.SYNTHOVERRIDEEN
        self.CLKLOADDENOMVAL = RM_Field_SYNTH_CALCTRL_CLKLOADDENOMVAL(self)
        self.zz_fdict['CLKLOADDENOMVAL'] = self.CLKLOADDENOMVAL
        self.PRESCALERRESETVAL = RM_Field_SYNTH_CALCTRL_PRESCALERRESETVAL(self)
        self.zz_fdict['PRESCALERRESETVAL'] = self.PRESCALERRESETVAL
        self.COUNTERRESETVAL = RM_Field_SYNTH_CALCTRL_COUNTERRESETVAL(self)
        self.zz_fdict['COUNTERRESETVAL'] = self.COUNTERRESETVAL
        self.AUXPLLOVERRIDEEN = RM_Field_SYNTH_CALCTRL_AUXPLLOVERRIDEEN(self)
        self.zz_fdict['AUXPLLOVERRIDEEN'] = self.AUXPLLOVERRIDEEN
        self.AUXPLLMMDRESETVAL = RM_Field_SYNTH_CALCTRL_AUXPLLMMDRESETVAL(self)
        self.zz_fdict['AUXPLLMMDRESETVAL'] = self.AUXPLLMMDRESETVAL
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCDACCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCDACCTRL, self).__init__(rmio, label,
            0x40083000, 0x024,
            'VCDACCTRL', 'SYNTH.VCDACCTRL', 'read-write',
            u"",
            0x00000020, 0x0000007F)

        self.VCDACVAL = RM_Field_SYNTH_VCDACCTRL_VCDACVAL(self)
        self.zz_fdict['VCDACVAL'] = self.VCDACVAL
        self.EN = RM_Field_SYNTH_VCDACCTRL_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_FREQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_FREQ, self).__init__(rmio, label,
            0x40083000, 0x02C,
            'FREQ', 'SYNTH.FREQ', 'read-write',
            u"",
            0x00000000, 0x0FFFFFFF)

        self.FREQ = RM_Field_SYNTH_FREQ_FREQ(self)
        self.zz_fdict['FREQ'] = self.FREQ
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IFFREQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IFFREQ, self).__init__(rmio, label,
            0x40083000, 0x030,
            'IFFREQ', 'SYNTH.IFFREQ', 'read-write',
            u"",
            0x00000000, 0x001FFFFF)

        self.IFFREQ = RM_Field_SYNTH_IFFREQ_IFFREQ(self)
        self.zz_fdict['IFFREQ'] = self.IFFREQ
        self.LOSIDE = RM_Field_SYNTH_IFFREQ_LOSIDE(self)
        self.zz_fdict['LOSIDE'] = self.LOSIDE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_DIVCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_DIVCTRL, self).__init__(rmio, label,
            0x40083000, 0x034,
            'DIVCTRL', 'SYNTH.DIVCTRL', 'read-write',
            u"",
            0x00000001, 0x003F01FF)

        self.LODIVFREQCTRL = RM_Field_SYNTH_DIVCTRL_LODIVFREQCTRL(self)
        self.zz_fdict['LODIVFREQCTRL'] = self.LODIVFREQCTRL
        self.AUXLODIVFREQCTRL = RM_Field_SYNTH_DIVCTRL_AUXLODIVFREQCTRL(self)
        self.zz_fdict['AUXLODIVFREQCTRL'] = self.AUXLODIVFREQCTRL
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHCTRL, self).__init__(rmio, label,
            0x40083000, 0x038,
            'CHCTRL', 'SYNTH.CHCTRL', 'read-write',
            u"",
            0x00000000, 0x0000003F)

        self.CHNO = RM_Field_SYNTH_CHCTRL_CHNO(self)
        self.zz_fdict['CHNO'] = self.CHNO
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHSP(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHSP, self).__init__(rmio, label,
            0x40083000, 0x03C,
            'CHSP', 'SYNTH.CHSP', 'read-write',
            u"",
            0x00000000, 0x0003FFFF)

        self.CHSP = RM_Field_SYNTH_CHSP_CHSP(self)
        self.zz_fdict['CHSP'] = self.CHSP
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CALOFFSET(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CALOFFSET, self).__init__(rmio, label,
            0x40083000, 0x040,
            'CALOFFSET', 'SYNTH.CALOFFSET', 'read-write',
            u"",
            0x00000000, 0x00007FFF)

        self.CALOFFSET = RM_Field_SYNTH_CALOFFSET_CALOFFSET(self)
        self.zz_fdict['CALOFFSET'] = self.CALOFFSET
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCOTUNING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCOTUNING, self).__init__(rmio, label,
            0x40083000, 0x044,
            'VCOTUNING', 'SYNTH.VCOTUNING', 'read-write',
            u"",
            0x00000080, 0x000000FF)

        self.VCOTUNING = RM_Field_SYNTH_VCOTUNING_VCOTUNING(self)
        self.zz_fdict['VCOTUNING'] = self.VCOTUNING
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCORANGE(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCORANGE, self).__init__(rmio, label,
            0x40083000, 0x04C,
            'VCORANGE', 'SYNTH.VCORANGE', 'read-write',
            u"",
            0x00000000, 0x000001FF)

        self.MODE = RM_Field_SYNTH_VCORANGE_MODE(self)
        self.zz_fdict['MODE'] = self.MODE
        self.SWITCHMODE = RM_Field_SYNTH_VCORANGE_SWITCHMODE(self)
        self.zz_fdict['SWITCHMODE'] = self.SWITCHMODE
        self.HIGHTHRESH = RM_Field_SYNTH_VCORANGE_HIGHTHRESH(self)
        self.zz_fdict['HIGHTHRESH'] = self.HIGHTHRESH
        self.LOWTHRESH = RM_Field_SYNTH_VCORANGE_LOWTHRESH(self)
        self.zz_fdict['LOWTHRESH'] = self.LOWTHRESH
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_VCOGAIN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_VCOGAIN, self).__init__(rmio, label,
            0x40083000, 0x050,
            'VCOGAIN', 'SYNTH.VCOGAIN', 'read-write',
            u"",
            0x0000001F, 0x0000003F)

        self.VCOGAIN = RM_Field_SYNTH_VCOGAIN_VCOGAIN(self)
        self.zz_fdict['VCOGAIN'] = self.VCOGAIN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_AUXVCOTUNING(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_AUXVCOTUNING, self).__init__(rmio, label,
            0x40083000, 0x054,
            'AUXVCOTUNING', 'SYNTH.AUXVCOTUNING', 'read-write',
            u"",
            0x00000000, 0x0000007F)

        self.AUXVCOTUNING = RM_Field_SYNTH_AUXVCOTUNING_AUXVCOTUNING(self)
        self.zz_fdict['AUXVCOTUNING'] = self.AUXVCOTUNING
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_AUXFREQ(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_AUXFREQ, self).__init__(rmio, label,
            0x40083000, 0x058,
            'AUXFREQ', 'SYNTH.AUXFREQ', 'read-write',
            u"",
            0x00000000, 0x0000007F)

        self.MMDDENOM = RM_Field_SYNTH_AUXFREQ_MMDDENOM(self)
        self.zz_fdict['MMDDENOM'] = self.MMDDENOM
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_AUXVCDACCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_AUXVCDACCTRL, self).__init__(rmio, label,
            0x40083000, 0x05C,
            'AUXVCDACCTRL', 'SYNTH.AUXVCDACCTRL', 'read-write',
            u"",
            0x00000007, 0x0000001F)

        self.VALUE = RM_Field_SYNTH_AUXVCDACCTRL_VALUE(self)
        self.zz_fdict['VALUE'] = self.VALUE
        self.EN = RM_Field_SYNTH_AUXVCDACCTRL_EN(self)
        self.zz_fdict['EN'] = self.EN
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CHPDACCTRL(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CHPDACCTRL, self).__init__(rmio, label,
            0x40083000, 0x060,
            'CHPDACCTRL', 'SYNTH.CHPDACCTRL', 'read-write',
            u"",
            0x00000000, 0x000001FF)

        self.CHPDACVAL = RM_Field_SYNTH_CHPDACCTRL_CHPDACVAL(self)
        self.zz_fdict['CHPDACVAL'] = self.CHPDACVAL
        self.OVERRIDE = RM_Field_SYNTH_CHPDACCTRL_OVERRIDE(self)
        self.zz_fdict['OVERRIDE'] = self.OVERRIDE
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_CAPCALCYCLECNT(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_CAPCALCYCLECNT, self).__init__(rmio, label,
            0x40083000, 0x064,
            'CAPCALCYCLECNT', 'SYNTH.CAPCALCYCLECNT', 'read-only',
            u"",
            0x00000000, 0x000003FF)

        self.CAPCALCYCLECNT = RM_Field_SYNTH_CAPCALCYCLECNT_CAPCALCYCLECNT(self)
        self.zz_fdict['CAPCALCYCLECNT'] = self.CAPCALCYCLECNT
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IF(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IF, self).__init__(rmio, label,
            0x40083000, 0x070,
            'IF', 'SYNTH.IF', 'read-only',
            u"",
            0x00000000, 0x000001F7)

        self.LOCKED = RM_Field_SYNTH_IF_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_IF_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.CAPCALDONE = RM_Field_SYNTH_IF_CAPCALDONE(self)
        self.zz_fdict['CAPCALDONE'] = self.CAPCALDONE
        self.VCOHIGH = RM_Field_SYNTH_IF_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_IF_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.AUXCAPCALDONE = RM_Field_SYNTH_IF_AUXCAPCALDONE(self)
        self.zz_fdict['AUXCAPCALDONE'] = self.AUXCAPCALDONE
        self.AUXLOCKED = RM_Field_SYNTH_IF_AUXLOCKED(self)
        self.zz_fdict['AUXLOCKED'] = self.AUXLOCKED
        self.AUXUNLOCKED = RM_Field_SYNTH_IF_AUXUNLOCKED(self)
        self.zz_fdict['AUXUNLOCKED'] = self.AUXUNLOCKED
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IFS(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IFS, self).__init__(rmio, label,
            0x40083000, 0x074,
            'IFS', 'SYNTH.IFS', 'write-only',
            u"",
            0x00000000, 0x000001F7)

        self.LOCKED = RM_Field_SYNTH_IFS_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_IFS_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.CAPCALDONE = RM_Field_SYNTH_IFS_CAPCALDONE(self)
        self.zz_fdict['CAPCALDONE'] = self.CAPCALDONE
        self.VCOHIGH = RM_Field_SYNTH_IFS_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_IFS_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.AUXCAPCALDONE = RM_Field_SYNTH_IFS_AUXCAPCALDONE(self)
        self.zz_fdict['AUXCAPCALDONE'] = self.AUXCAPCALDONE
        self.AUXLOCKED = RM_Field_SYNTH_IFS_AUXLOCKED(self)
        self.zz_fdict['AUXLOCKED'] = self.AUXLOCKED
        self.AUXUNLOCKED = RM_Field_SYNTH_IFS_AUXUNLOCKED(self)
        self.zz_fdict['AUXUNLOCKED'] = self.AUXUNLOCKED
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IFC(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IFC, self).__init__(rmio, label,
            0x40083000, 0x078,
            'IFC', 'SYNTH.IFC', 'write-only',
            u"",
            0x00000000, 0x000001F7)

        self.LOCKED = RM_Field_SYNTH_IFC_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_IFC_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.CAPCALDONE = RM_Field_SYNTH_IFC_CAPCALDONE(self)
        self.zz_fdict['CAPCALDONE'] = self.CAPCALDONE
        self.VCOHIGH = RM_Field_SYNTH_IFC_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_IFC_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.AUXCAPCALDONE = RM_Field_SYNTH_IFC_AUXCAPCALDONE(self)
        self.zz_fdict['AUXCAPCALDONE'] = self.AUXCAPCALDONE
        self.AUXLOCKED = RM_Field_SYNTH_IFC_AUXLOCKED(self)
        self.zz_fdict['AUXLOCKED'] = self.AUXLOCKED
        self.AUXUNLOCKED = RM_Field_SYNTH_IFC_AUXUNLOCKED(self)
        self.zz_fdict['AUXUNLOCKED'] = self.AUXUNLOCKED
        self.__dict__['zz_frozen'] = True


class RM_Register_SYNTH_IEN(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SYNTH_IEN, self).__init__(rmio, label,
            0x40083000, 0x07C,
            'IEN', 'SYNTH.IEN', 'read-write',
            u"",
            0x00000000, 0x000001F7)

        self.LOCKED = RM_Field_SYNTH_IEN_LOCKED(self)
        self.zz_fdict['LOCKED'] = self.LOCKED
        self.UNLOCKED = RM_Field_SYNTH_IEN_UNLOCKED(self)
        self.zz_fdict['UNLOCKED'] = self.UNLOCKED
        self.CAPCALDONE = RM_Field_SYNTH_IEN_CAPCALDONE(self)
        self.zz_fdict['CAPCALDONE'] = self.CAPCALDONE
        self.VCOHIGH = RM_Field_SYNTH_IEN_VCOHIGH(self)
        self.zz_fdict['VCOHIGH'] = self.VCOHIGH
        self.VCOLOW = RM_Field_SYNTH_IEN_VCOLOW(self)
        self.zz_fdict['VCOLOW'] = self.VCOLOW
        self.AUXCAPCALDONE = RM_Field_SYNTH_IEN_AUXCAPCALDONE(self)
        self.zz_fdict['AUXCAPCALDONE'] = self.AUXCAPCALDONE
        self.AUXLOCKED = RM_Field_SYNTH_IEN_AUXLOCKED(self)
        self.zz_fdict['AUXLOCKED'] = self.AUXLOCKED
        self.AUXUNLOCKED = RM_Field_SYNTH_IEN_AUXUNLOCKED(self)
        self.zz_fdict['AUXUNLOCKED'] = self.AUXUNLOCKED
        self.__dict__['zz_frozen'] = True


