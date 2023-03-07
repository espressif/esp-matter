
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_SYNTH_IPVERSION_IPVERSION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IPVERSION_IPVERSION, self).__init__(register,
            'IPVERSION', 'SYNTH.IPVERSION.IPVERSION', 'read-only',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_EN_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_EN_EN, self).__init__(register,
            'EN', 'SYNTH.EN.EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_INLOCK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_INLOCK, self).__init__(register,
            'INLOCK', 'SYNTH.STATUS.INLOCK', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_IFFREQEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_IFFREQEN, self).__init__(register,
            'IFFREQEN', 'SYNTH.STATUS.IFFREQEN', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_SYNTHREADY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_SYNTHREADY, self).__init__(register,
            'SYNTHREADY', 'SYNTH.STATUS.SYNTHREADY', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_CAPCALRUNNING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_CAPCALRUNNING, self).__init__(register,
            'CAPCALRUNNING', 'SYNTH.STATUS.CAPCALRUNNING', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_CAPCALBITRESULT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_CAPCALBITRESULT, self).__init__(register,
            'CAPCALBITRESULT', 'SYNTH.STATUS.CAPCALBITRESULT', 'read-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_CAPCALFULLWAIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_CAPCALFULLWAIT, self).__init__(register,
            'CAPCALFULLWAIT', 'SYNTH.STATUS.CAPCALFULLWAIT', 'read-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_CAPCALERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_CAPCALERROR, self).__init__(register,
            'CAPCALERROR', 'SYNTH.STATUS.CAPCALERROR', 'read-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_VCOFREQACTIVE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_VCOFREQACTIVE, self).__init__(register,
            'VCOFREQACTIVE', 'SYNTH.STATUS.VCOFREQACTIVE', 'read-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_CHPALLOWTRISTATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_CHPALLOWTRISTATE, self).__init__(register,
            'CHPALLOWTRISTATE', 'SYNTH.STATUS.CHPALLOWTRISTATE', 'read-only',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_SYNTHSTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_SYNTHSTART, self).__init__(register,
            'SYNTHSTART', 'SYNTH.CMD.SYNTHSTART', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_SYNTHSTOP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_SYNTHSTOP, self).__init__(register,
            'SYNTHSTOP', 'SYNTH.CMD.SYNTHSTOP', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_ENABLEIF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_ENABLEIF, self).__init__(register,
            'ENABLEIF', 'SYNTH.CMD.ENABLEIF', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_DISABLEIF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_DISABLEIF, self).__init__(register,
            'DISABLEIF', 'SYNTH.CMD.DISABLEIF', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_CAPCALSTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_CAPCALSTART, self).__init__(register,
            'CAPCALSTART', 'SYNTH.CMD.CAPCALSTART', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_VCOADDCAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_VCOADDCAP, self).__init__(register,
            'VCOADDCAP', 'SYNTH.CMD.VCOADDCAP', 'write-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_VCOSUBCAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_VCOSUBCAP, self).__init__(register,
            'VCOSUBCAP', 'SYNTH.CMD.VCOSUBCAP', 'write-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_LOCKTHRESHOLD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_LOCKTHRESHOLD, self).__init__(register,
            'LOCKTHRESHOLD', 'SYNTH.CTRL.LOCKTHRESHOLD', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_PRSMUX0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_PRSMUX0, self).__init__(register,
            'PRSMUX0', 'SYNTH.CTRL.PRSMUX0', 'read-write',
            u"",
            16, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_PRSMUX1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_PRSMUX1, self).__init__(register,
            'PRSMUX1', 'SYNTH.CTRL.PRSMUX1', 'read-write',
            u"",
            20, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_DISCLKSYNTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_DISCLKSYNTH, self).__init__(register,
            'DISCLKSYNTH', 'SYNTH.CTRL.DISCLKSYNTH', 'read-write',
            u"",
            23, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_INVCLKSYNTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_INVCLKSYNTH, self).__init__(register,
            'INVCLKSYNTH', 'SYNTH.CTRL.INVCLKSYNTH', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_TRISTATEPOSTPONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_TRISTATEPOSTPONE, self).__init__(register,
            'TRISTATEPOSTPONE', 'SYNTH.CTRL.TRISTATEPOSTPONE', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_INTEGERMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_INTEGERMODE, self).__init__(register,
            'INTEGERMODE', 'SYNTH.CTRL.INTEGERMODE', 'read-write',
            u"",
            28, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_MMDRSTNOVERRIDEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_MMDRSTNOVERRIDEEN, self).__init__(register,
            'MMDRSTNOVERRIDEEN', 'SYNTH.CTRL.MMDRSTNOVERRIDEEN', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_MMDMANRSTN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_MMDMANRSTN, self).__init__(register,
            'MMDMANRSTN', 'SYNTH.CTRL.MMDMANRSTN', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_NUMCYCLES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_NUMCYCLES, self).__init__(register,
            'NUMCYCLES', 'SYNTH.CALCTRL.NUMCYCLES', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_CAPCALSIDE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_CAPCALSIDE, self).__init__(register,
            'CAPCALSIDE', 'SYNTH.CALCTRL.CAPCALSIDE', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_CAPCALENDMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_CAPCALENDMODE, self).__init__(register,
            'CAPCALENDMODE', 'SYNTH.CALCTRL.CAPCALENDMODE', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_CAPCALCYCLEWAIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_CAPCALCYCLEWAIT, self).__init__(register,
            'CAPCALCYCLEWAIT', 'SYNTH.CALCTRL.CAPCALCYCLEWAIT', 'read-write',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_CAPCALWAITMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_CAPCALWAITMODE, self).__init__(register,
            'CAPCALWAITMODE', 'SYNTH.CALCTRL.CAPCALWAITMODE', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_CAPCALSINGLESTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_CAPCALSINGLESTEP, self).__init__(register,
            'CAPCALSINGLESTEP', 'SYNTH.CALCTRL.CAPCALSINGLESTEP', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_STARTUPTIMING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_STARTUPTIMING, self).__init__(register,
            'STARTUPTIMING', 'SYNTH.CALCTRL.STARTUPTIMING', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_SYNTHOVERRIDEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_SYNTHOVERRIDEEN, self).__init__(register,
            'SYNTHOVERRIDEEN', 'SYNTH.CALCTRL.SYNTHOVERRIDEEN', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_CLKLOADDENOMVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_CLKLOADDENOMVAL, self).__init__(register,
            'CLKLOADDENOMVAL', 'SYNTH.CALCTRL.CLKLOADDENOMVAL', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_PRESCALERRESETVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_PRESCALERRESETVAL, self).__init__(register,
            'PRESCALERRESETVAL', 'SYNTH.CALCTRL.PRESCALERRESETVAL', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_COUNTERRESETVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_COUNTERRESETVAL, self).__init__(register,
            'COUNTERRESETVAL', 'SYNTH.CALCTRL.COUNTERRESETVAL', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCDACCTRL_VCDACVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCDACCTRL_VCDACVAL, self).__init__(register,
            'VCDACVAL', 'SYNTH.VCDACCTRL.VCDACVAL', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCDACCTRL_VCDACEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCDACCTRL_VCDACEN, self).__init__(register,
            'VCDACEN', 'SYNTH.VCDACCTRL.VCDACEN', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCDACCTRL_LPFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCDACCTRL_LPFEN, self).__init__(register,
            'LPFEN', 'SYNTH.VCDACCTRL.LPFEN', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCDACCTRL_LPFQSEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCDACCTRL_LPFQSEN, self).__init__(register,
            'LPFQSEN', 'SYNTH.VCDACCTRL.LPFQSEN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_FREQ_FREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_FREQ_FREQ, self).__init__(register,
            'FREQ', 'SYNTH.FREQ.FREQ', 'read-write',
            u"",
            0, 28)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFFREQ_IFFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFFREQ_IFFREQ, self).__init__(register,
            'IFFREQ', 'SYNTH.IFFREQ.IFFREQ', 'read-write',
            u"",
            0, 20)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFFREQ_LOSIDE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFFREQ_LOSIDE, self).__init__(register,
            'LOSIDE', 'SYNTH.IFFREQ.LOSIDE', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DIVCTRL_LODIVFREQCTRL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DIVCTRL_LODIVFREQCTRL, self).__init__(register,
            'LODIVFREQCTRL', 'SYNTH.DIVCTRL.LODIVFREQCTRL', 'read-write',
            u"",
            0, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CHCTRL_CHNO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CHCTRL_CHNO, self).__init__(register,
            'CHNO', 'SYNTH.CHCTRL.CHNO', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CHSP_CHSP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CHSP_CHSP, self).__init__(register,
            'CHSP', 'SYNTH.CHSP.CHSP', 'read-write',
            u"",
            0, 18)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALOFFSET_CALOFFSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALOFFSET_CALOFFSET, self).__init__(register,
            'CALOFFSET', 'SYNTH.CALOFFSET.CALOFFSET', 'read-write',
            u"",
            0, 15)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOTUNING_VCOTUNING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOTUNING_VCOTUNING, self).__init__(register,
            'VCOTUNING', 'SYNTH.VCOTUNING.VCOTUNING', 'read-write',
            u"",
            0, 11)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOTUNING_VCAPSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOTUNING_VCAPSEL, self).__init__(register,
            'VCAPSEL', 'SYNTH.VCOTUNING.VCAPSEL', 'read-write',
            u"",
            11, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_MODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_MODE, self).__init__(register,
            'MODE', 'SYNTH.VCORANGE.MODE', 'read-write',
            u"",
            0, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_SWITCHMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_SWITCHMODE, self).__init__(register,
            'SWITCHMODE', 'SYNTH.VCORANGE.SWITCHMODE', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_HIGHTHRESH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_HIGHTHRESH, self).__init__(register,
            'HIGHTHRESH', 'SYNTH.VCORANGE.HIGHTHRESH', 'read-write',
            u"",
            3, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_LOWTHRESH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_LOWTHRESH, self).__init__(register,
            'LOWTHRESH', 'SYNTH.VCORANGE.LOWTHRESH', 'read-write',
            u"",
            6, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_OVERRIDEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_OVERRIDEEN, self).__init__(register,
            'OVERRIDEEN', 'SYNTH.VCORANGE.OVERRIDEEN', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_DATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_DATA, self).__init__(register,
            'DATA', 'SYNTH.VCORANGE.DATA', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_LOAD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_LOAD, self).__init__(register,
            'LOAD', 'SYNTH.VCORANGE.LOAD', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_RAMPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_RAMPEN, self).__init__(register,
            'RAMPEN', 'SYNTH.VCORANGE.RAMPEN', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_CMPENABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_CMPENABLE, self).__init__(register,
            'CMPENABLE', 'SYNTH.VCORANGE.CMPENABLE', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_CMPRESET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_CMPRESET, self).__init__(register,
            'CMPRESET', 'SYNTH.VCORANGE.CMPRESET', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_RAMPRATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_RAMPRATE, self).__init__(register,
            'RAMPRATE', 'SYNTH.VCORANGE.RAMPRATE', 'read-write',
            u"",
            15, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_THROUGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_THROUGH, self).__init__(register,
            'THROUGH', 'SYNTH.VCORANGE.THROUGH', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_INVCOMPDIR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_INVCOMPDIR, self).__init__(register,
            'INVCOMPDIR', 'SYNTH.VCORANGE.INVCOMPDIR', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOGAIN_VCOKVCOARSE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOGAIN_VCOKVCOARSE, self).__init__(register,
            'VCOKVCOARSE', 'SYNTH.VCOGAIN.VCOKVCOARSE', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOGAIN_VCOKVFINE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOGAIN_VCOKVFINE, self).__init__(register,
            'VCOKVFINE', 'SYNTH.VCOGAIN.VCOKVFINE', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CHPDACCTRL_CHPDACVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CHPDACCTRL_CHPDACVAL, self).__init__(register,
            'CHPDACVAL', 'SYNTH.CHPDACCTRL.CHPDACVAL', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CHPDACCTRL_OVERRIDE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CHPDACCTRL_OVERRIDE, self).__init__(register,
            'OVERRIDE', 'SYNTH.CHPDACCTRL.OVERRIDE', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CAPCALCYCLECNT_CAPCALCYCLECNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CAPCALCYCLECNT_CAPCALCYCLECNT, self).__init__(register,
            'CAPCALCYCLECNT', 'SYNTH.CAPCALCYCLECNT.CAPCALCYCLECNT', 'read-only',
            u"",
            0, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOFRCCAL_TIMEOUTCOUNTMAX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOFRCCAL_TIMEOUTCOUNTMAX, self).__init__(register,
            'TIMEOUTCOUNTMAX', 'SYNTH.VCOFRCCAL.TIMEOUTCOUNTMAX', 'read-write',
            u"",
            0, 14)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOFRCCAL_TIMEOUTCOUNTRESTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOFRCCAL_TIMEOUTCOUNTRESTART, self).__init__(register,
            'TIMEOUTCOUNTRESTART', 'SYNTH.VCOFRCCAL.TIMEOUTCOUNTRESTART', 'read-write',
            u"",
            16, 14)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.IF.LOCKED', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.IF.UNLOCKED', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_SYRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_SYRDY, self).__init__(register,
            'SYRDY', 'SYNTH.IF.SYRDY', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.IF.VCOHIGH', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.IF.VCOLOW', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_LOCNTDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_LOCNTDONE, self).__init__(register,
            'LOCNTDONE', 'SYNTH.IF.LOCNTDONE', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.IEN.LOCKED', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.IEN.UNLOCKED', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_SYRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_SYRDY, self).__init__(register,
            'SYRDY', 'SYNTH.IEN.SYRDY', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.IEN.VCOHIGH', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.IEN.VCOLOW', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_LOCNTDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_LOCNTDONE, self).__init__(register,
            'LOCNTDONE', 'SYNTH.IEN.LOCNTDONE', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_ENABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_ENABLE, self).__init__(register,
            'ENABLE', 'SYNTH.LOCNTCTRL.ENABLE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_CLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_CLEAR, self).__init__(register,
            'CLEAR', 'SYNTH.LOCNTCTRL.CLEAR', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_RUN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_RUN, self).__init__(register,
            'RUN', 'SYNTH.LOCNTCTRL.RUN', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_READ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_READ, self).__init__(register,
            'READ', 'SYNTH.LOCNTCTRL.READ', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_NUMCYCLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_NUMCYCLE, self).__init__(register,
            'NUMCYCLE', 'SYNTH.LOCNTCTRL.NUMCYCLE', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_LOCNTOVERRIDEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_LOCNTOVERRIDEEN, self).__init__(register,
            'LOCNTOVERRIDEEN', 'SYNTH.LOCNTCTRL.LOCNTOVERRIDEEN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_LOCNTMANCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_LOCNTMANCLEAR, self).__init__(register,
            'LOCNTMANCLEAR', 'SYNTH.LOCNTCTRL.LOCNTMANCLEAR', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_LOCNTMANRUN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_LOCNTMANRUN, self).__init__(register,
            'LOCNTMANRUN', 'SYNTH.LOCNTCTRL.LOCNTMANRUN', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTCTRL_FCALRUNCLKEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTCTRL_FCALRUNCLKEN, self).__init__(register,
            'FCALRUNCLKEN', 'SYNTH.LOCNTCTRL.FCALRUNCLKEN', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTSTATUS_LOCOUNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTSTATUS_LOCOUNT, self).__init__(register,
            'LOCOUNT', 'SYNTH.LOCNTSTATUS.LOCOUNT', 'read-only',
            u"",
            0, 19)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTSTATUS_BUSY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTSTATUS_BUSY, self).__init__(register,
            'BUSY', 'SYNTH.LOCNTSTATUS.BUSY', 'read-only',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LOCNTTARGET_TARGET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LOCNTTARGET_TARGET, self).__init__(register,
            'TARGET', 'SYNTH.LOCNTTARGET.TARGET', 'read-only',
            u"",
            0, 19)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT0, self).__init__(register,
            'DENOMINIT0', 'SYNTH.MMDDENOMINIT.DENOMINIT0', 'read-write',
            u"",
            0, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT1, self).__init__(register,
            'DENOMINIT1', 'SYNTH.MMDDENOMINIT.DENOMINIT1', 'read-write',
            u"",
            9, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_MMDDENOMINIT_DENOMINIT2, self).__init__(register,
            'DENOMINIT2', 'SYNTH.MMDDENOMINIT.DENOMINIT2', 'read-write',
            u"",
            18, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CHPDACINIT_DACINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CHPDACINIT_DACINIT, self).__init__(register,
            'DACINIT', 'SYNTH.CHPDACINIT.DACINIT', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1CAL_OP1BWCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1CAL_OP1BWCAL, self).__init__(register,
            'OP1BWCAL', 'SYNTH.LPFCTRL1CAL.OP1BWCAL', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1CAL_OP1COMPCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1CAL_OP1COMPCAL, self).__init__(register,
            'OP1COMPCAL', 'SYNTH.LPFCTRL1CAL.OP1COMPCAL', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1CAL_RFBVALCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1CAL_RFBVALCAL, self).__init__(register,
            'RFBVALCAL', 'SYNTH.LPFCTRL1CAL.RFBVALCAL', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1CAL_RPVALCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1CAL_RPVALCAL, self).__init__(register,
            'RPVALCAL', 'SYNTH.LPFCTRL1CAL.RPVALCAL', 'read-write',
            u"",
            11, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1CAL_RZVALCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1CAL_RZVALCAL, self).__init__(register,
            'RZVALCAL', 'SYNTH.LPFCTRL1CAL.RZVALCAL', 'read-write',
            u"",
            14, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1RX_OP1BWRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1RX_OP1BWRX, self).__init__(register,
            'OP1BWRX', 'SYNTH.LPFCTRL1RX.OP1BWRX', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1RX_OP1COMPRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1RX_OP1COMPRX, self).__init__(register,
            'OP1COMPRX', 'SYNTH.LPFCTRL1RX.OP1COMPRX', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1RX_RFBVALRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1RX_RFBVALRX, self).__init__(register,
            'RFBVALRX', 'SYNTH.LPFCTRL1RX.RFBVALRX', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1RX_RPVALRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1RX_RPVALRX, self).__init__(register,
            'RPVALRX', 'SYNTH.LPFCTRL1RX.RPVALRX', 'read-write',
            u"",
            11, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1RX_RZVALRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1RX_RZVALRX, self).__init__(register,
            'RZVALRX', 'SYNTH.LPFCTRL1RX.RZVALRX', 'read-write',
            u"",
            14, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1TX_OP1BWTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1TX_OP1BWTX, self).__init__(register,
            'OP1BWTX', 'SYNTH.LPFCTRL1TX.OP1BWTX', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1TX_OP1COMPTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1TX_OP1COMPTX, self).__init__(register,
            'OP1COMPTX', 'SYNTH.LPFCTRL1TX.OP1COMPTX', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1TX_RFBVALTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1TX_RFBVALTX, self).__init__(register,
            'RFBVALTX', 'SYNTH.LPFCTRL1TX.RFBVALTX', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1TX_RPVALTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1TX_RPVALTX, self).__init__(register,
            'RPVALTX', 'SYNTH.LPFCTRL1TX.RPVALTX', 'read-write',
            u"",
            11, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL1TX_RZVALTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL1TX_RZVALTX, self).__init__(register,
            'RZVALTX', 'SYNTH.LPFCTRL1TX.RZVALTX', 'read-write',
            u"",
            14, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_LPFSWENRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_LPFSWENRX, self).__init__(register,
            'LPFSWENRX', 'SYNTH.LPFCTRL2RX.LPFSWENRX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_LPFINCAPRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_LPFINCAPRX, self).__init__(register,
            'LPFINCAPRX', 'SYNTH.LPFCTRL2RX.LPFINCAPRX', 'read-write',
            u"",
            1, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_LPFGNDSWENRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_LPFGNDSWENRX, self).__init__(register,
            'LPFGNDSWENRX', 'SYNTH.LPFCTRL2RX.LPFGNDSWENRX', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_CALCRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_CALCRX, self).__init__(register,
            'CALCRX', 'SYNTH.LPFCTRL2RX.CALCRX', 'read-write',
            u"",
            4, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_CASELRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_CASELRX, self).__init__(register,
            'CASELRX', 'SYNTH.LPFCTRL2RX.CASELRX', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_CAVALRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_CAVALRX, self).__init__(register,
            'CAVALRX', 'SYNTH.LPFCTRL2RX.CAVALRX', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_CFBSELRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_CFBSELRX, self).__init__(register,
            'CFBSELRX', 'SYNTH.LPFCTRL2RX.CFBSELRX', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_CZSELRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_CZSELRX, self).__init__(register,
            'CZSELRX', 'SYNTH.LPFCTRL2RX.CZSELRX', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_CZVALRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_CZVALRX, self).__init__(register,
            'CZVALRX', 'SYNTH.LPFCTRL2RX.CZVALRX', 'read-write',
            u"",
            17, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_MODESELRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_MODESELRX, self).__init__(register,
            'MODESELRX', 'SYNTH.LPFCTRL2RX.MODESELRX', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2RX_VCMLVLRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2RX_VCMLVLRX, self).__init__(register,
            'VCMLVLRX', 'SYNTH.LPFCTRL2RX.VCMLVLRX', 'read-write',
            u"",
            26, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_LPFSWENTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_LPFSWENTX, self).__init__(register,
            'LPFSWENTX', 'SYNTH.LPFCTRL2TX.LPFSWENTX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_LPFINCAPTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_LPFINCAPTX, self).__init__(register,
            'LPFINCAPTX', 'SYNTH.LPFCTRL2TX.LPFINCAPTX', 'read-write',
            u"",
            1, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_LPFGNDSWENTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_LPFGNDSWENTX, self).__init__(register,
            'LPFGNDSWENTX', 'SYNTH.LPFCTRL2TX.LPFGNDSWENTX', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_CALCTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_CALCTX, self).__init__(register,
            'CALCTX', 'SYNTH.LPFCTRL2TX.CALCTX', 'read-write',
            u"",
            4, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_CASELTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_CASELTX, self).__init__(register,
            'CASELTX', 'SYNTH.LPFCTRL2TX.CASELTX', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_CAVALTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_CAVALTX, self).__init__(register,
            'CAVALTX', 'SYNTH.LPFCTRL2TX.CAVALTX', 'read-write',
            u"",
            10, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_CFBSELTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_CFBSELTX, self).__init__(register,
            'CFBSELTX', 'SYNTH.LPFCTRL2TX.CFBSELTX', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_CZSELTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_CZSELTX, self).__init__(register,
            'CZSELTX', 'SYNTH.LPFCTRL2TX.CZSELTX', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_CZVALTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_CZVALTX, self).__init__(register,
            'CZVALTX', 'SYNTH.LPFCTRL2TX.CZVALTX', 'read-write',
            u"",
            17, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_MODESELTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_MODESELTX, self).__init__(register,
            'MODESELTX', 'SYNTH.LPFCTRL2TX.MODESELTX', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_LPFCTRL2TX_VCMLVLTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_LPFCTRL2TX_VCMLVLTX, self).__init__(register,
            'VCMLVLTX', 'SYNTH.LPFCTRL2TX.VCMLVLTX', 'read-write',
            u"",
            26, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_DITHERDSMINPUTRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_DITHERDSMINPUTRX, self).__init__(register,
            'DITHERDSMINPUTRX', 'SYNTH.DSMCTRLRX.DITHERDSMINPUTRX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_DITHERDSMOUTPUTRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_DITHERDSMOUTPUTRX, self).__init__(register,
            'DITHERDSMOUTPUTRX', 'SYNTH.DSMCTRLRX.DITHERDSMOUTPUTRX', 'read-write',
            u"",
            1, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_DITHERDACRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_DITHERDACRX, self).__init__(register,
            'DITHERDACRX', 'SYNTH.DSMCTRLRX.DITHERDACRX', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_DSMMODERX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_DSMMODERX, self).__init__(register,
            'DSMMODERX', 'SYNTH.DSMCTRLRX.DSMMODERX', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_LSBFORCERX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_LSBFORCERX, self).__init__(register,
            'LSBFORCERX', 'SYNTH.DSMCTRLRX.LSBFORCERX', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_DEMMODERX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_DEMMODERX, self).__init__(register,
            'DEMMODERX', 'SYNTH.DSMCTRLRX.DEMMODERX', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_MASHORDERRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_MASHORDERRX, self).__init__(register,
            'MASHORDERRX', 'SYNTH.DSMCTRLRX.MASHORDERRX', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLRX_REQORDERRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLRX_REQORDERRX, self).__init__(register,
            'REQORDERRX', 'SYNTH.DSMCTRLRX.REQORDERRX', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_DITHERDSMINPUTTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_DITHERDSMINPUTTX, self).__init__(register,
            'DITHERDSMINPUTTX', 'SYNTH.DSMCTRLTX.DITHERDSMINPUTTX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_DITHERDSMOUTPUTTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_DITHERDSMOUTPUTTX, self).__init__(register,
            'DITHERDSMOUTPUTTX', 'SYNTH.DSMCTRLTX.DITHERDSMOUTPUTTX', 'read-write',
            u"",
            1, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_DITHERDACTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_DITHERDACTX, self).__init__(register,
            'DITHERDACTX', 'SYNTH.DSMCTRLTX.DITHERDACTX', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_DSMMODETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_DSMMODETX, self).__init__(register,
            'DSMMODETX', 'SYNTH.DSMCTRLTX.DSMMODETX', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_LSBFORCETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_LSBFORCETX, self).__init__(register,
            'LSBFORCETX', 'SYNTH.DSMCTRLTX.LSBFORCETX', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_DEMMODETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_DEMMODETX, self).__init__(register,
            'DEMMODETX', 'SYNTH.DSMCTRLTX.DEMMODETX', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_MASHORDERTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_MASHORDERTX, self).__init__(register,
            'MASHORDERTX', 'SYNTH.DSMCTRLTX.MASHORDERTX', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_DSMCTRLTX_REQORDERTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DSMCTRLTX_REQORDERTX, self).__init__(register,
            'REQORDERTX', 'SYNTH.DSMCTRLTX.REQORDERTX', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIF_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIF_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.SEQIF.LOCKED', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIF_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIF_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.SEQIF.UNLOCKED', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIF_SYRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIF_SYRDY, self).__init__(register,
            'SYRDY', 'SYNTH.SEQIF.SYRDY', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIF_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIF_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.SEQIF.VCOHIGH', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIF_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIF_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.SEQIF.VCOLOW', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIF_LOCNTDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIF_LOCNTDONE, self).__init__(register,
            'LOCNTDONE', 'SYNTH.SEQIF.LOCNTDONE', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIEN_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIEN_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.SEQIEN.LOCKED', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIEN_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIEN_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.SEQIEN.UNLOCKED', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIEN_SYRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIEN_SYRDY, self).__init__(register,
            'SYRDY', 'SYNTH.SEQIEN.SYRDY', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIEN_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIEN_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.SEQIEN.VCOHIGH', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIEN_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIEN_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.SEQIEN.VCOLOW', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_SEQIEN_LOCNTDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_SEQIEN_LOCNTDONE, self).__init__(register,
            'LOCNTDONE', 'SYNTH.SEQIEN.LOCNTDONE', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


