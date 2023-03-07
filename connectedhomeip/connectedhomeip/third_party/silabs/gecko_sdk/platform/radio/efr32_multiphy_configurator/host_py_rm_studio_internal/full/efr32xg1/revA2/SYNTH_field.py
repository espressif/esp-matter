
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


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


class RM_Field_SYNTH_STATUS_AUXINLOCK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_AUXINLOCK, self).__init__(register,
            'AUXINLOCK', 'SYNTH.STATUS.AUXINLOCK', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_AUXRUNNING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_AUXRUNNING, self).__init__(register,
            'AUXRUNNING', 'SYNTH.STATUS.AUXRUNNING', 'read-only',
            u"",
            5, 1)
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


class RM_Field_SYNTH_STATUS_AUXCAPCALRUNNING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_AUXCAPCALRUNNING, self).__init__(register,
            'AUXCAPCALRUNNING', 'SYNTH.STATUS.AUXCAPCALRUNNING', 'read-only',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_AUXCALBITRESULT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_AUXCALBITRESULT, self).__init__(register,
            'AUXCALBITRESULT', 'SYNTH.STATUS.AUXCALBITRESULT', 'read-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_AUXCALFULLWAIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_AUXCALFULLWAIT, self).__init__(register,
            'AUXCALFULLWAIT', 'SYNTH.STATUS.AUXCALFULLWAIT', 'read-only',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_CAPCALERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_CAPCALERROR, self).__init__(register,
            'CAPCALERROR', 'SYNTH.STATUS.CAPCALERROR', 'read-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_AUXCALERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_AUXCALERROR, self).__init__(register,
            'AUXCALERROR', 'SYNTH.STATUS.AUXCALERROR', 'read-only',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_VCOFREQACTIVE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_VCOFREQACTIVE, self).__init__(register,
            'VCOFREQACTIVE', 'SYNTH.STATUS.VCOFREQACTIVE', 'read-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_VCOVARBANK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_VCOVARBANK, self).__init__(register,
            'VCOVARBANK', 'SYNTH.STATUS.VCOVARBANK', 'read-only',
            u"",
            17, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_STATUS_VCOAMPOK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_STATUS_VCOAMPOK, self).__init__(register,
            'VCOAMPOK', 'SYNTH.STATUS.VCOAMPOK', 'read-only',
            u"",
            24, 1)
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


class RM_Field_SYNTH_CMD_AUXSTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_AUXSTART, self).__init__(register,
            'AUXSTART', 'SYNTH.CMD.AUXSTART', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_AUXSTOP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_AUXSTOP, self).__init__(register,
            'AUXSTOP', 'SYNTH.CMD.AUXSTOP', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_AUXCAPCALSTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_AUXCAPCALSTART, self).__init__(register,
            'AUXCAPCALSTART', 'SYNTH.CMD.AUXCAPCALSTART', 'write-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_VCOUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_VCOUP, self).__init__(register,
            'VCOUP', 'SYNTH.CMD.VCOUP', 'write-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CMD_VCODOWN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CMD_VCODOWN, self).__init__(register,
            'VCODOWN', 'SYNTH.CMD.VCODOWN', 'write-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_DITHERDSMINPUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_DITHERDSMINPUT, self).__init__(register,
            'DITHERDSMINPUT', 'SYNTH.CTRL.DITHERDSMINPUT', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_DITHERDSMOUTPUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_DITHERDSMOUTPUT, self).__init__(register,
            'DITHERDSMOUTPUT', 'SYNTH.CTRL.DITHERDSMOUTPUT', 'read-write',
            u"",
            1, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_DITHERDAC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_DITHERDAC, self).__init__(register,
            'DITHERDAC', 'SYNTH.CTRL.DITHERDAC', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_DSMMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_DSMMODE, self).__init__(register,
            'DSMMODE', 'SYNTH.CTRL.DSMMODE', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_LSBFORCE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_LSBFORCE, self).__init__(register,
            'LSBFORCE', 'SYNTH.CTRL.LSBFORCE', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_LOCKTHRESHOLD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_LOCKTHRESHOLD, self).__init__(register,
            'LOCKTHRESHOLD', 'SYNTH.CTRL.LOCKTHRESHOLD', 'read-write',
            u"",
            10, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_AUXLOCKTHRESHOLD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_AUXLOCKTHRESHOLD, self).__init__(register,
            'AUXLOCKTHRESHOLD', 'SYNTH.CTRL.AUXLOCKTHRESHOLD', 'read-write',
            u"",
            13, 3)
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


class RM_Field_SYNTH_CTRL_TRISTATEPOSTPONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_TRISTATEPOSTPONE, self).__init__(register,
            'TRISTATEPOSTPONE', 'SYNTH.CTRL.TRISTATEPOSTPONE', 'read-write',
            u"",
            29, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_INTEGERMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_INTEGERMODE, self).__init__(register,
            'INTEGERMODE', 'SYNTH.CTRL.INTEGERMODE', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CTRL_MMDSCANTESTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CTRL_MMDSCANTESTEN, self).__init__(register,
            'MMDSCANTESTEN', 'SYNTH.CTRL.MMDSCANTESTEN', 'read-write',
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


class RM_Field_SYNTH_CALCTRL_AUXCALCYCLES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_AUXCALCYCLES, self).__init__(register,
            'AUXCALCYCLES', 'SYNTH.CALCTRL.AUXCALCYCLES', 'read-write',
            u"",
            16, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_AUXCALACCURACY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_AUXCALACCURACY, self).__init__(register,
            'AUXCALACCURACY', 'SYNTH.CALCTRL.AUXCALACCURACY', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_AUXCALCYCLEWAIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_AUXCALCYCLEWAIT, self).__init__(register,
            'AUXCALCYCLEWAIT', 'SYNTH.CALCTRL.AUXCALCYCLEWAIT', 'read-write',
            u"",
            20, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_AUXCALSINGLESTEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_AUXCALSINGLESTEP, self).__init__(register,
            'AUXCALSINGLESTEP', 'SYNTH.CALCTRL.AUXCALSINGLESTEP', 'read-write',
            u"",
            22, 1)
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


class RM_Field_SYNTH_CALCTRL_AUXPLLOVERRIDEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_AUXPLLOVERRIDEEN, self).__init__(register,
            'AUXPLLOVERRIDEEN', 'SYNTH.CALCTRL.AUXPLLOVERRIDEEN', 'read-write',
            u"",
            28, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_CALCTRL_AUXPLLMMDRESETVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_CALCTRL_AUXPLLMMDRESETVAL, self).__init__(register,
            'AUXPLLMMDRESETVAL', 'SYNTH.CALCTRL.AUXPLLMMDRESETVAL', 'read-write',
            u"",
            29, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCDACCTRL_VCDACVAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCDACCTRL_VCDACVAL, self).__init__(register,
            'VCDACVAL', 'SYNTH.VCDACCTRL.VCDACVAL', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCDACCTRL_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCDACCTRL_EN, self).__init__(register,
            'EN', 'SYNTH.VCDACCTRL.EN', 'read-write',
            u"",
            6, 1)
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


class RM_Field_SYNTH_DIVCTRL_AUXLODIVFREQCTRL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_DIVCTRL_AUXLODIVFREQCTRL, self).__init__(register,
            'AUXLODIVFREQCTRL', 'SYNTH.DIVCTRL.AUXLODIVFREQCTRL', 'read-write',
            u"",
            16, 6)
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
            0, 8)
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
            3, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCORANGE_LOWTHRESH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCORANGE_LOWTHRESH, self).__init__(register,
            'LOWTHRESH', 'SYNTH.VCORANGE.LOWTHRESH', 'read-write',
            u"",
            5, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_VCOGAIN_VCOGAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_VCOGAIN_VCOGAIN, self).__init__(register,
            'VCOGAIN', 'SYNTH.VCOGAIN.VCOGAIN', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_AUXVCOTUNING_AUXVCOTUNING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_AUXVCOTUNING_AUXVCOTUNING, self).__init__(register,
            'AUXVCOTUNING', 'SYNTH.AUXVCOTUNING.AUXVCOTUNING', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_AUXFREQ_MMDDENOM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_AUXFREQ_MMDDENOM, self).__init__(register,
            'MMDDENOM', 'SYNTH.AUXFREQ.MMDDENOM', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_AUXVCDACCTRL_VALUE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_AUXVCDACCTRL_VALUE, self).__init__(register,
            'VALUE', 'SYNTH.AUXVCDACCTRL.VALUE', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_AUXVCDACCTRL_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_AUXVCDACCTRL_EN, self).__init__(register,
            'EN', 'SYNTH.AUXVCDACCTRL.EN', 'read-write',
            u"",
            4, 1)
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


class RM_Field_SYNTH_IF_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.IF.LOCKED', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.IF.UNLOCKED', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_CAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_CAPCALDONE, self).__init__(register,
            'CAPCALDONE', 'SYNTH.IF.CAPCALDONE', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.IF.VCOHIGH', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.IF.VCOLOW', 'read-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_AUXCAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_AUXCAPCALDONE, self).__init__(register,
            'AUXCAPCALDONE', 'SYNTH.IF.AUXCAPCALDONE', 'read-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_AUXLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_AUXLOCKED, self).__init__(register,
            'AUXLOCKED', 'SYNTH.IF.AUXLOCKED', 'read-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IF_AUXUNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IF_AUXUNLOCKED, self).__init__(register,
            'AUXUNLOCKED', 'SYNTH.IF.AUXUNLOCKED', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.IFS.LOCKED', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.IFS.UNLOCKED', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_CAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_CAPCALDONE, self).__init__(register,
            'CAPCALDONE', 'SYNTH.IFS.CAPCALDONE', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.IFS.VCOHIGH', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.IFS.VCOLOW', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_AUXCAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_AUXCAPCALDONE, self).__init__(register,
            'AUXCAPCALDONE', 'SYNTH.IFS.AUXCAPCALDONE', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_AUXLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_AUXLOCKED, self).__init__(register,
            'AUXLOCKED', 'SYNTH.IFS.AUXLOCKED', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFS_AUXUNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFS_AUXUNLOCKED, self).__init__(register,
            'AUXUNLOCKED', 'SYNTH.IFS.AUXUNLOCKED', 'write-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_LOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_LOCKED, self).__init__(register,
            'LOCKED', 'SYNTH.IFC.LOCKED', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_UNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_UNLOCKED, self).__init__(register,
            'UNLOCKED', 'SYNTH.IFC.UNLOCKED', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_CAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_CAPCALDONE, self).__init__(register,
            'CAPCALDONE', 'SYNTH.IFC.CAPCALDONE', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_VCOHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_VCOHIGH, self).__init__(register,
            'VCOHIGH', 'SYNTH.IFC.VCOHIGH', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_VCOLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_VCOLOW, self).__init__(register,
            'VCOLOW', 'SYNTH.IFC.VCOLOW', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_AUXCAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_AUXCAPCALDONE, self).__init__(register,
            'AUXCAPCALDONE', 'SYNTH.IFC.AUXCAPCALDONE', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_AUXLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_AUXLOCKED, self).__init__(register,
            'AUXLOCKED', 'SYNTH.IFC.AUXLOCKED', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IFC_AUXUNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IFC_AUXUNLOCKED, self).__init__(register,
            'AUXUNLOCKED', 'SYNTH.IFC.AUXUNLOCKED', 'write-only',
            u"",
            8, 1)
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


class RM_Field_SYNTH_IEN_CAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_CAPCALDONE, self).__init__(register,
            'CAPCALDONE', 'SYNTH.IEN.CAPCALDONE', 'read-write',
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


class RM_Field_SYNTH_IEN_AUXCAPCALDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_AUXCAPCALDONE, self).__init__(register,
            'AUXCAPCALDONE', 'SYNTH.IEN.AUXCAPCALDONE', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_AUXLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_AUXLOCKED, self).__init__(register,
            'AUXLOCKED', 'SYNTH.IEN.AUXLOCKED', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_SYNTH_IEN_AUXUNLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_SYNTH_IEN_AUXUNLOCKED, self).__init__(register,
            'AUXUNLOCKED', 'SYNTH.IEN.AUXUNLOCKED', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


