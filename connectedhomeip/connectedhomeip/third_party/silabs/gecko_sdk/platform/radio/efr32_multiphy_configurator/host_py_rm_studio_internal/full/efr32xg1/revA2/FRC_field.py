
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_FRC_STATUS_ACTIVETXFCD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_STATUS_ACTIVETXFCD, self).__init__(register,
            'ACTIVETXFCD', 'FRC.STATUS.ACTIVETXFCD', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_STATUS_ACTIVERXFCD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_STATUS_ACTIVERXFCD, self).__init__(register,
            'ACTIVERXFCD', 'FRC.STATUS.ACTIVERXFCD', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_STATUS_RXABORTINPROGRESS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_STATUS_RXABORTINPROGRESS, self).__init__(register,
            'RXABORTINPROGRESS', 'FRC.STATUS.RXABORTINPROGRESS', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_STATUS_SNIFFDFRAME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_STATUS_SNIFFDFRAME, self).__init__(register,
            'SNIFFDFRAME', 'FRC.STATUS.SNIFFDFRAME', 'read-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_STATUS_SNIFFDCOUNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_STATUS_SNIFFDCOUNT, self).__init__(register,
            'SNIFFDCOUNT', 'FRC.STATUS.SNIFFDCOUNT', 'read-only',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_STATUS_RXRAWBLOCKED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_STATUS_RXRAWBLOCKED, self).__init__(register,
            'RXRAWBLOCKED', 'FRC.STATUS.RXRAWBLOCKED', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_DFLMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_DFLMODE, self).__init__(register,
            'DFLMODE', 'FRC.DFLCTRL.DFLMODE', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_DFLBITORDER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_DFLBITORDER, self).__init__(register,
            'DFLBITORDER', 'FRC.DFLCTRL.DFLBITORDER', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_DFLSHIFT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_DFLSHIFT, self).__init__(register,
            'DFLSHIFT', 'FRC.DFLCTRL.DFLSHIFT', 'read-write',
            u"",
            4, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_DFLOFFSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_DFLOFFSET, self).__init__(register,
            'DFLOFFSET', 'FRC.DFLCTRL.DFLOFFSET', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_DFLBITS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_DFLBITS, self).__init__(register,
            'DFLBITS', 'FRC.DFLCTRL.DFLBITS', 'read-write',
            u"",
            12, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_MINLENGTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_MINLENGTH, self).__init__(register,
            'MINLENGTH', 'FRC.DFLCTRL.MINLENGTH', 'read-write',
            u"",
            16, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DFLCTRL_DFLINCLUDECRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DFLCTRL_DFLINCLUDECRC, self).__init__(register,
            'DFLINCLUDECRC', 'FRC.DFLCTRL.DFLINCLUDECRC', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_MAXLENGTH_MAXLENGTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_MAXLENGTH_MAXLENGTH, self).__init__(register,
            'MAXLENGTH', 'FRC.MAXLENGTH.MAXLENGTH', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ADDRFILTCTRL_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ADDRFILTCTRL_EN, self).__init__(register,
            'EN', 'FRC.ADDRFILTCTRL.EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ADDRFILTCTRL_BRDCST00EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ADDRFILTCTRL_BRDCST00EN, self).__init__(register,
            'BRDCST00EN', 'FRC.ADDRFILTCTRL.BRDCST00EN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ADDRFILTCTRL_BRDCSTFFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ADDRFILTCTRL_BRDCSTFFEN, self).__init__(register,
            'BRDCSTFFEN', 'FRC.ADDRFILTCTRL.BRDCSTFFEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ADDRFILTCTRL_ADDRESS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ADDRFILTCTRL_ADDRESS, self).__init__(register,
            'ADDRESS', 'FRC.ADDRFILTCTRL.ADDRESS', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_DATABUFFER_DATABUFFER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_DATABUFFER_DATABUFFER, self).__init__(register,
            'DATABUFFER', 'FRC.DATABUFFER.DATABUFFER', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WCNT_WCNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WCNT_WCNT, self).__init__(register,
            'WCNT', 'FRC.WCNT.WCNT', 'read-only',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WCNTCMP0_FRAMELENGTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WCNTCMP0_FRAMELENGTH, self).__init__(register,
            'FRAMELENGTH', 'FRC.WCNTCMP0.FRAMELENGTH', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WCNTCMP1_LENGTHFIELDLOC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WCNTCMP1_LENGTHFIELDLOC, self).__init__(register,
            'LENGTHFIELDLOC', 'FRC.WCNTCMP1.LENGTHFIELDLOC', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WCNTCMP2_ADDRFIELDLOC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WCNTCMP2_ADDRFIELDLOC, self).__init__(register,
            'ADDRFIELDLOC', 'FRC.WCNTCMP2.ADDRFIELDLOC', 'read-write',
            u"",
            0, 12)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_RXABORT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_RXABORT, self).__init__(register,
            'RXABORT', 'FRC.CMD.RXABORT', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_FRAMEDETRESUME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_FRAMEDETRESUME, self).__init__(register,
            'FRAMEDETRESUME', 'FRC.CMD.FRAMEDETRESUME', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_INTERLEAVEWRITERESUME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_INTERLEAVEWRITERESUME, self).__init__(register,
            'INTERLEAVEWRITERESUME', 'FRC.CMD.INTERLEAVEWRITERESUME', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_INTERLEAVEREADRESUME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_INTERLEAVEREADRESUME, self).__init__(register,
            'INTERLEAVEREADRESUME', 'FRC.CMD.INTERLEAVEREADRESUME', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_CONVRESUME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_CONVRESUME, self).__init__(register,
            'CONVRESUME', 'FRC.CMD.CONVRESUME', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_CONVTERMINATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_CONVTERMINATE, self).__init__(register,
            'CONVTERMINATE', 'FRC.CMD.CONVTERMINATE', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_TXSUBFRAMERESUME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_TXSUBFRAMERESUME, self).__init__(register,
            'TXSUBFRAMERESUME', 'FRC.CMD.TXSUBFRAMERESUME', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_INTERLEAVEINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_INTERLEAVEINIT, self).__init__(register,
            'INTERLEAVEINIT', 'FRC.CMD.INTERLEAVEINIT', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_INTERLEAVECNTCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_INTERLEAVECNTCLEAR, self).__init__(register,
            'INTERLEAVECNTCLEAR', 'FRC.CMD.INTERLEAVECNTCLEAR', 'write-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_CONVINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_CONVINIT, self).__init__(register,
            'CONVINIT', 'FRC.CMD.CONVINIT', 'write-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_BLOCKINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_BLOCKINIT, self).__init__(register,
            'BLOCKINIT', 'FRC.CMD.BLOCKINIT', 'write-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_STATEINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_STATEINIT, self).__init__(register,
            'STATEINIT', 'FRC.CMD.STATEINIT', 'write-only',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CMD_RXRAWUNBLOCK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CMD_RXRAWUNBLOCK, self).__init__(register,
            'RXRAWUNBLOCK', 'FRC.CMD.RXRAWUNBLOCK', 'write-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WHITECTRL_FEEDBACKSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WHITECTRL_FEEDBACKSEL, self).__init__(register,
            'FEEDBACKSEL', 'FRC.WHITECTRL.FEEDBACKSEL', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WHITECTRL_XORFEEDBACK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WHITECTRL_XORFEEDBACK, self).__init__(register,
            'XORFEEDBACK', 'FRC.WHITECTRL.XORFEEDBACK', 'read-write',
            u"",
            5, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WHITECTRL_SHROUTPUTSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WHITECTRL_SHROUTPUTSEL, self).__init__(register,
            'SHROUTPUTSEL', 'FRC.WHITECTRL.SHROUTPUTSEL', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WHITECTRL_BLOCKERRORCORRECT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WHITECTRL_BLOCKERRORCORRECT, self).__init__(register,
            'BLOCKERRORCORRECT', 'FRC.WHITECTRL.BLOCKERRORCORRECT', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WHITEPOLY_POLY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WHITEPOLY_POLY, self).__init__(register,
            'POLY', 'FRC.WHITEPOLY.POLY', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_WHITEINIT_WHITEINIT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_WHITEINIT_WHITEINIT, self).__init__(register,
            'WHITEINIT', 'FRC.WHITEINIT.WHITEINIT', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_BLOCKWHITEMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_BLOCKWHITEMODE, self).__init__(register,
            'BLOCKWHITEMODE', 'FRC.FECCTRL.BLOCKWHITEMODE', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVMODE, self).__init__(register,
            'CONVMODE', 'FRC.FECCTRL.CONVMODE', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVDECODEMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVDECODEMODE, self).__init__(register,
            'CONVDECODEMODE', 'FRC.FECCTRL.CONVDECODEMODE', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVTRACEBACKDISABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVTRACEBACKDISABLE, self).__init__(register,
            'CONVTRACEBACKDISABLE', 'FRC.FECCTRL.CONVTRACEBACKDISABLE', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVINV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVINV, self).__init__(register,
            'CONVINV', 'FRC.FECCTRL.CONVINV', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_INTERLEAVEMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_INTERLEAVEMODE, self).__init__(register,
            'INTERLEAVEMODE', 'FRC.FECCTRL.INTERLEAVEMODE', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_INTERLEAVEFIRSTINDEX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_INTERLEAVEFIRSTINDEX, self).__init__(register,
            'INTERLEAVEFIRSTINDEX', 'FRC.FECCTRL.INTERLEAVEFIRSTINDEX', 'read-write',
            u"",
            12, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_INTERLEAVEWIDTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_INTERLEAVEWIDTH, self).__init__(register,
            'INTERLEAVEWIDTH', 'FRC.FECCTRL.INTERLEAVEWIDTH', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVBUSLOCK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVBUSLOCK, self).__init__(register,
            'CONVBUSLOCK', 'FRC.FECCTRL.CONVBUSLOCK', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVSUBFRAMETERMINATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVSUBFRAMETERMINATE, self).__init__(register,
            'CONVSUBFRAMETERMINATE', 'FRC.FECCTRL.CONVSUBFRAMETERMINATE', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_SINGLEBLOCK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_SINGLEBLOCK, self).__init__(register,
            'SINGLEBLOCK', 'FRC.FECCTRL.SINGLEBLOCK', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_FORCE2FSK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_FORCE2FSK, self).__init__(register,
            'FORCE2FSK', 'FRC.FECCTRL.FORCE2FSK', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FECCTRL_CONVHARDERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FECCTRL_CONVHARDERROR, self).__init__(register,
            'CONVHARDERROR', 'FRC.FECCTRL.CONVHARDERROR', 'read-write',
            u"",
            21, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_BLOCKRAMADDR_BLOCKRAMADDR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_BLOCKRAMADDR_BLOCKRAMADDR, self).__init__(register,
            'BLOCKRAMADDR', 'FRC.BLOCKRAMADDR.BLOCKRAMADDR', 'read-write',
            u"",
            2, 13)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CONVRAMADDR_CONVRAMADDR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CONVRAMADDR_CONVRAMADDR, self).__init__(register,
            'CONVRAMADDR', 'FRC.CONVRAMADDR.CONVRAMADDR', 'read-write',
            u"",
            2, 13)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_RANDOMTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_RANDOMTX, self).__init__(register,
            'RANDOMTX', 'FRC.CTRL.RANDOMTX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_UARTMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_UARTMODE, self).__init__(register,
            'UARTMODE', 'FRC.CTRL.UARTMODE', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_BITORDER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_BITORDER, self).__init__(register,
            'BITORDER', 'FRC.CTRL.BITORDER', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_TXFCDMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_TXFCDMODE, self).__init__(register,
            'TXFCDMODE', 'FRC.CTRL.TXFCDMODE', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_RXFCDMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_RXFCDMODE, self).__init__(register,
            'RXFCDMODE', 'FRC.CTRL.RXFCDMODE', 'read-write',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_BITSPERWORD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_BITSPERWORD, self).__init__(register,
            'BITSPERWORD', 'FRC.CTRL.BITSPERWORD', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_RATESELECT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_RATESELECT, self).__init__(register,
            'RATESELECT', 'FRC.CTRL.RATESELECT', 'read-write',
            u"",
            11, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_TXPREFETCH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_TXPREFETCH, self).__init__(register,
            'TXPREFETCH', 'FRC.CTRL.TXPREFETCH', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_SEQHANDSHAKE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_SEQHANDSHAKE, self).__init__(register,
            'SEQHANDSHAKE', 'FRC.CTRL.SEQHANDSHAKE', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CTRL_PRBSTEST(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CTRL_PRBSTEST, self).__init__(register,
            'PRBSTEST', 'FRC.CTRL.PRBSTEST', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_STORECRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_STORECRC, self).__init__(register,
            'STORECRC', 'FRC.RXCTRL.STORECRC', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_ACCEPTCRCERRORS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_ACCEPTCRCERRORS, self).__init__(register,
            'ACCEPTCRCERRORS', 'FRC.RXCTRL.ACCEPTCRCERRORS', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_ACCEPTBLOCKERRORS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_ACCEPTBLOCKERRORS, self).__init__(register,
            'ACCEPTBLOCKERRORS', 'FRC.RXCTRL.ACCEPTBLOCKERRORS', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_TRACKABFRAME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_TRACKABFRAME, self).__init__(register,
            'TRACKABFRAME', 'FRC.RXCTRL.TRACKABFRAME', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_BUFCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_BUFCLEAR, self).__init__(register,
            'BUFCLEAR', 'FRC.RXCTRL.BUFCLEAR', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_BUFRESTOREFRAMEERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_BUFRESTOREFRAMEERROR, self).__init__(register,
            'BUFRESTOREFRAMEERROR', 'FRC.RXCTRL.BUFRESTOREFRAMEERROR', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXCTRL_BUFRESTORERXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXCTRL_BUFRESTORERXABORTED, self).__init__(register,
            'BUFRESTORERXABORTED', 'FRC.RXCTRL.BUFRESTORERXABORTED', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILTXDATACTRL_TRAILTXDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILTXDATACTRL_TRAILTXDATA, self).__init__(register,
            'TRAILTXDATA', 'FRC.TRAILTXDATACTRL.TRAILTXDATA', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILTXDATACTRL_TRAILTXDATACNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILTXDATACTRL_TRAILTXDATACNT, self).__init__(register,
            'TRAILTXDATACNT', 'FRC.TRAILTXDATACTRL.TRAILTXDATACNT', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE, self).__init__(register,
            'TRAILTXDATAFORCE', 'FRC.TRAILTXDATACTRL.TRAILTXDATAFORCE', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILRXDATA_RSSI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILRXDATA_RSSI, self).__init__(register,
            'RSSI', 'FRC.TRAILRXDATA.RSSI', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILRXDATA_CRCOK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILRXDATA_CRCOK, self).__init__(register,
            'CRCOK', 'FRC.TRAILRXDATA.CRCOK', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILRXDATA_PROTIMERCC0BASE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILRXDATA_PROTIMERCC0BASE, self).__init__(register,
            'PROTIMERCC0BASE', 'FRC.TRAILRXDATA.PROTIMERCC0BASE', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILRXDATA_PROTIMERCC0WRAPL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILRXDATA_PROTIMERCC0WRAPL, self).__init__(register,
            'PROTIMERCC0WRAPL', 'FRC.TRAILRXDATA.PROTIMERCC0WRAPL', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILRXDATA_PROTIMERCC0WRAPH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILRXDATA_PROTIMERCC0WRAPH, self).__init__(register,
            'PROTIMERCC0WRAPH', 'FRC.TRAILRXDATA.PROTIMERCC0WRAPH', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_TRAILRXDATA_RTCSTAMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_TRAILRXDATA_RTCSTAMP, self).__init__(register,
            'RTCSTAMP', 'FRC.TRAILRXDATA.RTCSTAMP', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SCNT_SCNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SCNT_SCNT, self).__init__(register,
            'SCNT', 'FRC.SCNT.SCNT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CONVGENERATOR_GENERATOR0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CONVGENERATOR_GENERATOR0, self).__init__(register,
            'GENERATOR0', 'FRC.CONVGENERATOR.GENERATOR0', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CONVGENERATOR_GENERATOR1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CONVGENERATOR_GENERATOR1, self).__init__(register,
            'GENERATOR1', 'FRC.CONVGENERATOR.GENERATOR1', 'read-write',
            u"",
            8, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CONVGENERATOR_RECURSIVE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CONVGENERATOR_RECURSIVE, self).__init__(register,
            'RECURSIVE', 'FRC.CONVGENERATOR.RECURSIVE', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_CONVGENERATOR_NONSYSTEMATIC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_CONVGENERATOR_NONSYSTEMATIC, self).__init__(register,
            'NONSYSTEMATIC', 'FRC.CONVGENERATOR.NONSYSTEMATIC', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PUNCTCTRL_PUNCT0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PUNCTCTRL_PUNCT0, self).__init__(register,
            'PUNCT0', 'FRC.PUNCTCTRL.PUNCT0', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PUNCTCTRL_PUNCT1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PUNCTCTRL_PUNCT1, self).__init__(register,
            'PUNCT1', 'FRC.PUNCTCTRL.PUNCT1', 'read-write',
            u"",
            8, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_FRAMEDETPAUSEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_FRAMEDETPAUSEEN, self).__init__(register,
            'FRAMEDETPAUSEEN', 'FRC.PAUSECTRL.FRAMEDETPAUSEEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_TXINTERLEAVEWRITEPAUSEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_TXINTERLEAVEWRITEPAUSEEN, self).__init__(register,
            'TXINTERLEAVEWRITEPAUSEEN', 'FRC.PAUSECTRL.TXINTERLEAVEWRITEPAUSEEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_RXINTERLEAVEWRITEPAUSEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_RXINTERLEAVEWRITEPAUSEEN, self).__init__(register,
            'RXINTERLEAVEWRITEPAUSEEN', 'FRC.PAUSECTRL.RXINTERLEAVEWRITEPAUSEEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_INTERLEAVEREADPAUSEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_INTERLEAVEREADPAUSEEN, self).__init__(register,
            'INTERLEAVEREADPAUSEEN', 'FRC.PAUSECTRL.INTERLEAVEREADPAUSEEN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_TXSUBFRAMEPAUSEEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_TXSUBFRAMEPAUSEEN, self).__init__(register,
            'TXSUBFRAMEPAUSEEN', 'FRC.PAUSECTRL.TXSUBFRAMEPAUSEEN', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_CONVPAUSECNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_CONVPAUSECNT, self).__init__(register,
            'CONVPAUSECNT', 'FRC.PAUSECTRL.CONVPAUSECNT', 'read-write',
            u"",
            5, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_INTERLEAVEWRITEPAUSECNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_INTERLEAVEWRITEPAUSECNT, self).__init__(register,
            'INTERLEAVEWRITEPAUSECNT', 'FRC.PAUSECTRL.INTERLEAVEWRITEPAUSECNT', 'read-write',
            u"",
            11, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSECTRL_INTERLEAVEREADPAUSECNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSECTRL_INTERLEAVEREADPAUSECNT, self).__init__(register,
            'INTERLEAVEREADPAUSECNT', 'FRC.PAUSECTRL.INTERLEAVEREADPAUSECNT', 'read-write',
            u"",
            16, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXDONE, self).__init__(register,
            'TXDONE', 'FRC.IF.TXDONE', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXAFTERFRAMEDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXAFTERFRAMEDONE, self).__init__(register,
            'TXAFTERFRAMEDONE', 'FRC.IF.TXAFTERFRAMEDONE', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXABORTED, self).__init__(register,
            'TXABORTED', 'FRC.IF.TXABORTED', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXUF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXUF, self).__init__(register,
            'TXUF', 'FRC.IF.TXUF', 'read-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_RXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_RXDONE, self).__init__(register,
            'RXDONE', 'FRC.IF.RXDONE', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_RXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_RXABORTED, self).__init__(register,
            'RXABORTED', 'FRC.IF.RXABORTED', 'read-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_FRAMEERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_FRAMEERROR, self).__init__(register,
            'FRAMEERROR', 'FRC.IF.FRAMEERROR', 'read-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_BLOCKERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_BLOCKERROR, self).__init__(register,
            'BLOCKERROR', 'FRC.IF.BLOCKERROR', 'read-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_RXOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_RXOF, self).__init__(register,
            'RXOF', 'FRC.IF.RXOF', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_WCNTCMP0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_WCNTCMP0, self).__init__(register,
            'WCNTCMP0', 'FRC.IF.WCNTCMP0', 'read-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_WCNTCMP1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_WCNTCMP1, self).__init__(register,
            'WCNTCMP1', 'FRC.IF.WCNTCMP1', 'read-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_WCNTCMP2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_WCNTCMP2, self).__init__(register,
            'WCNTCMP2', 'FRC.IF.WCNTCMP2', 'read-only',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_ADDRERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_ADDRERROR, self).__init__(register,
            'ADDRERROR', 'FRC.IF.ADDRERROR', 'read-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_BUSERROR, self).__init__(register,
            'BUSERROR', 'FRC.IF.BUSERROR', 'read-only',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_RXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_RXRAWEVENT, self).__init__(register,
            'RXRAWEVENT', 'FRC.IF.RXRAWEVENT', 'read-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXRAWEVENT, self).__init__(register,
            'TXRAWEVENT', 'FRC.IF.TXRAWEVENT', 'read-only',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_SNIFFOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_SNIFFOF, self).__init__(register,
            'SNIFFOF', 'FRC.IF.SNIFFOF', 'read-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_LVDSWILLERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_LVDSWILLERROR, self).__init__(register,
            'LVDSWILLERROR', 'FRC.IF.LVDSWILLERROR', 'read-only',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_LVDSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_LVDSERROR, self).__init__(register,
            'LVDSERROR', 'FRC.IF.LVDSERROR', 'read-only',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_FRAMEDETPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_FRAMEDETPAUSED, self).__init__(register,
            'FRAMEDETPAUSED', 'FRC.IF.FRAMEDETPAUSED', 'read-only',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_INTERLEAVEWRITEPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_INTERLEAVEWRITEPAUSED, self).__init__(register,
            'INTERLEAVEWRITEPAUSED', 'FRC.IF.INTERLEAVEWRITEPAUSED', 'read-only',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_INTERLEAVEREADPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_INTERLEAVEREADPAUSED, self).__init__(register,
            'INTERLEAVEREADPAUSED', 'FRC.IF.INTERLEAVEREADPAUSED', 'read-only',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXSUBFRAMEPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXSUBFRAMEPAUSED, self).__init__(register,
            'TXSUBFRAMEPAUSED', 'FRC.IF.TXSUBFRAMEPAUSED', 'read-only',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_CONVPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_CONVPAUSED, self).__init__(register,
            'CONVPAUSED', 'FRC.IF.CONVPAUSED', 'read-only',
            u"",
            28, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_RXWORD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_RXWORD, self).__init__(register,
            'RXWORD', 'FRC.IF.RXWORD', 'read-only',
            u"",
            29, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IF_TXWORD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IF_TXWORD, self).__init__(register,
            'TXWORD', 'FRC.IF.TXWORD', 'read-only',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_TXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_TXDONE, self).__init__(register,
            'TXDONE', 'FRC.IFS.TXDONE', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_TXAFTERFRAMEDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_TXAFTERFRAMEDONE, self).__init__(register,
            'TXAFTERFRAMEDONE', 'FRC.IFS.TXAFTERFRAMEDONE', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_TXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_TXABORTED, self).__init__(register,
            'TXABORTED', 'FRC.IFS.TXABORTED', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_TXUF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_TXUF, self).__init__(register,
            'TXUF', 'FRC.IFS.TXUF', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_RXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_RXDONE, self).__init__(register,
            'RXDONE', 'FRC.IFS.RXDONE', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_RXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_RXABORTED, self).__init__(register,
            'RXABORTED', 'FRC.IFS.RXABORTED', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_FRAMEERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_FRAMEERROR, self).__init__(register,
            'FRAMEERROR', 'FRC.IFS.FRAMEERROR', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_BLOCKERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_BLOCKERROR, self).__init__(register,
            'BLOCKERROR', 'FRC.IFS.BLOCKERROR', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_RXOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_RXOF, self).__init__(register,
            'RXOF', 'FRC.IFS.RXOF', 'write-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_WCNTCMP0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_WCNTCMP0, self).__init__(register,
            'WCNTCMP0', 'FRC.IFS.WCNTCMP0', 'write-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_WCNTCMP1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_WCNTCMP1, self).__init__(register,
            'WCNTCMP1', 'FRC.IFS.WCNTCMP1', 'write-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_WCNTCMP2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_WCNTCMP2, self).__init__(register,
            'WCNTCMP2', 'FRC.IFS.WCNTCMP2', 'write-only',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_ADDRERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_ADDRERROR, self).__init__(register,
            'ADDRERROR', 'FRC.IFS.ADDRERROR', 'write-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_BUSERROR, self).__init__(register,
            'BUSERROR', 'FRC.IFS.BUSERROR', 'write-only',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_RXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_RXRAWEVENT, self).__init__(register,
            'RXRAWEVENT', 'FRC.IFS.RXRAWEVENT', 'write-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_TXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_TXRAWEVENT, self).__init__(register,
            'TXRAWEVENT', 'FRC.IFS.TXRAWEVENT', 'write-only',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_SNIFFOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_SNIFFOF, self).__init__(register,
            'SNIFFOF', 'FRC.IFS.SNIFFOF', 'write-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_LVDSWILLERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_LVDSWILLERROR, self).__init__(register,
            'LVDSWILLERROR', 'FRC.IFS.LVDSWILLERROR', 'write-only',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFS_LVDSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFS_LVDSERROR, self).__init__(register,
            'LVDSERROR', 'FRC.IFS.LVDSERROR', 'write-only',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_TXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_TXDONE, self).__init__(register,
            'TXDONE', 'FRC.IFC.TXDONE', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_TXAFTERFRAMEDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_TXAFTERFRAMEDONE, self).__init__(register,
            'TXAFTERFRAMEDONE', 'FRC.IFC.TXAFTERFRAMEDONE', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_TXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_TXABORTED, self).__init__(register,
            'TXABORTED', 'FRC.IFC.TXABORTED', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_TXUF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_TXUF, self).__init__(register,
            'TXUF', 'FRC.IFC.TXUF', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_RXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_RXDONE, self).__init__(register,
            'RXDONE', 'FRC.IFC.RXDONE', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_RXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_RXABORTED, self).__init__(register,
            'RXABORTED', 'FRC.IFC.RXABORTED', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_FRAMEERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_FRAMEERROR, self).__init__(register,
            'FRAMEERROR', 'FRC.IFC.FRAMEERROR', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_BLOCKERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_BLOCKERROR, self).__init__(register,
            'BLOCKERROR', 'FRC.IFC.BLOCKERROR', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_RXOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_RXOF, self).__init__(register,
            'RXOF', 'FRC.IFC.RXOF', 'write-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_WCNTCMP0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_WCNTCMP0, self).__init__(register,
            'WCNTCMP0', 'FRC.IFC.WCNTCMP0', 'write-only',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_WCNTCMP1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_WCNTCMP1, self).__init__(register,
            'WCNTCMP1', 'FRC.IFC.WCNTCMP1', 'write-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_WCNTCMP2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_WCNTCMP2, self).__init__(register,
            'WCNTCMP2', 'FRC.IFC.WCNTCMP2', 'write-only',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_ADDRERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_ADDRERROR, self).__init__(register,
            'ADDRERROR', 'FRC.IFC.ADDRERROR', 'write-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_BUSERROR, self).__init__(register,
            'BUSERROR', 'FRC.IFC.BUSERROR', 'write-only',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_RXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_RXRAWEVENT, self).__init__(register,
            'RXRAWEVENT', 'FRC.IFC.RXRAWEVENT', 'write-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_TXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_TXRAWEVENT, self).__init__(register,
            'TXRAWEVENT', 'FRC.IFC.TXRAWEVENT', 'write-only',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_SNIFFOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_SNIFFOF, self).__init__(register,
            'SNIFFOF', 'FRC.IFC.SNIFFOF', 'write-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_LVDSWILLERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_LVDSWILLERROR, self).__init__(register,
            'LVDSWILLERROR', 'FRC.IFC.LVDSWILLERROR', 'write-only',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IFC_LVDSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IFC_LVDSERROR, self).__init__(register,
            'LVDSERROR', 'FRC.IFC.LVDSERROR', 'write-only',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXDONE, self).__init__(register,
            'TXDONE', 'FRC.IEN.TXDONE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXAFTERFRAMEDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXAFTERFRAMEDONE, self).__init__(register,
            'TXAFTERFRAMEDONE', 'FRC.IEN.TXAFTERFRAMEDONE', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXABORTED, self).__init__(register,
            'TXABORTED', 'FRC.IEN.TXABORTED', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXUF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXUF, self).__init__(register,
            'TXUF', 'FRC.IEN.TXUF', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_RXDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_RXDONE, self).__init__(register,
            'RXDONE', 'FRC.IEN.RXDONE', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_RXABORTED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_RXABORTED, self).__init__(register,
            'RXABORTED', 'FRC.IEN.RXABORTED', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_FRAMEERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_FRAMEERROR, self).__init__(register,
            'FRAMEERROR', 'FRC.IEN.FRAMEERROR', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_BLOCKERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_BLOCKERROR, self).__init__(register,
            'BLOCKERROR', 'FRC.IEN.BLOCKERROR', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_RXOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_RXOF, self).__init__(register,
            'RXOF', 'FRC.IEN.RXOF', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_WCNTCMP0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_WCNTCMP0, self).__init__(register,
            'WCNTCMP0', 'FRC.IEN.WCNTCMP0', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_WCNTCMP1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_WCNTCMP1, self).__init__(register,
            'WCNTCMP1', 'FRC.IEN.WCNTCMP1', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_WCNTCMP2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_WCNTCMP2, self).__init__(register,
            'WCNTCMP2', 'FRC.IEN.WCNTCMP2', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_ADDRERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_ADDRERROR, self).__init__(register,
            'ADDRERROR', 'FRC.IEN.ADDRERROR', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_BUSERROR, self).__init__(register,
            'BUSERROR', 'FRC.IEN.BUSERROR', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_RXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_RXRAWEVENT, self).__init__(register,
            'RXRAWEVENT', 'FRC.IEN.RXRAWEVENT', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXRAWEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXRAWEVENT, self).__init__(register,
            'TXRAWEVENT', 'FRC.IEN.TXRAWEVENT', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_SNIFFOF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_SNIFFOF, self).__init__(register,
            'SNIFFOF', 'FRC.IEN.SNIFFOF', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_LVDSWILLERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_LVDSWILLERROR, self).__init__(register,
            'LVDSWILLERROR', 'FRC.IEN.LVDSWILLERROR', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_LVDSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_LVDSERROR, self).__init__(register,
            'LVDSERROR', 'FRC.IEN.LVDSERROR', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_FRAMEDETPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_FRAMEDETPAUSED, self).__init__(register,
            'FRAMEDETPAUSED', 'FRC.IEN.FRAMEDETPAUSED', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_INTERLEAVEWRITEPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_INTERLEAVEWRITEPAUSED, self).__init__(register,
            'INTERLEAVEWRITEPAUSED', 'FRC.IEN.INTERLEAVEWRITEPAUSED', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_INTERLEAVEREADPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_INTERLEAVEREADPAUSED, self).__init__(register,
            'INTERLEAVEREADPAUSED', 'FRC.IEN.INTERLEAVEREADPAUSED', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXSUBFRAMEPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXSUBFRAMEPAUSED, self).__init__(register,
            'TXSUBFRAMEPAUSED', 'FRC.IEN.TXSUBFRAMEPAUSED', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_CONVPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_CONVPAUSED, self).__init__(register,
            'CONVPAUSED', 'FRC.IEN.CONVPAUSED', 'read-write',
            u"",
            28, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_RXWORD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_RXWORD, self).__init__(register,
            'RXWORD', 'FRC.IEN.RXWORD', 'read-write',
            u"",
            29, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_IEN_TXWORD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_IEN_TXWORD, self).__init__(register,
            'TXWORD', 'FRC.IEN.TXWORD', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_BUFFERMODE_TXBUFFERMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_BUFFERMODE_TXBUFFERMODE, self).__init__(register,
            'TXBUFFERMODE', 'FRC.BUFFERMODE.TXBUFFERMODE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_BUFFERMODE_RXBUFFERMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_BUFFERMODE_RXBUFFERMODE, self).__init__(register,
            'RXBUFFERMODE', 'FRC.BUFFERMODE.RXBUFFERMODE', 'read-write',
            u"",
            1, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ROUTEPEN_DOUTPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ROUTEPEN_DOUTPEN, self).__init__(register,
            'DOUTPEN', 'FRC.ROUTEPEN.DOUTPEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ROUTEPEN_DCLKPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ROUTEPEN_DCLKPEN, self).__init__(register,
            'DCLKPEN', 'FRC.ROUTEPEN.DCLKPEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ROUTEPEN_DFRAMEPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ROUTEPEN_DFRAMEPEN, self).__init__(register,
            'DFRAMEPEN', 'FRC.ROUTEPEN.DFRAMEPEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ROUTELOC0_DOUTLOC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ROUTELOC0_DOUTLOC, self).__init__(register,
            'DOUTLOC', 'FRC.ROUTELOC0.DOUTLOC', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ROUTELOC0_DCLKLOC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ROUTELOC0_DCLKLOC, self).__init__(register,
            'DCLKLOC', 'FRC.ROUTELOC0.DCLKLOC', 'read-write',
            u"",
            8, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_ROUTELOC0_DFRAMELOC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_ROUTELOC0_DFRAMELOC, self).__init__(register,
            'DFRAMELOC', 'FRC.ROUTELOC0.DFRAMELOC', 'read-write',
            u"",
            16, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFMODE, self).__init__(register,
            'SNIFFMODE', 'FRC.SNIFFCTRL.SNIFFMODE', 'read-write',
            u"",
            0, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFBITS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFBITS, self).__init__(register,
            'SNIFFBITS', 'FRC.SNIFFCTRL.SNIFFBITS', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFRXDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFRXDATA, self).__init__(register,
            'SNIFFRXDATA', 'FRC.SNIFFCTRL.SNIFFRXDATA', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFTXDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFTXDATA, self).__init__(register,
            'SNIFFTXDATA', 'FRC.SNIFFCTRL.SNIFFTXDATA', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFRSSI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFRSSI, self).__init__(register,
            'SNIFFRSSI', 'FRC.SNIFFCTRL.SNIFFRSSI', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFSTATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFSTATE, self).__init__(register,
            'SNIFFSTATE', 'FRC.SNIFFCTRL.SNIFFSTATE', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFAUXDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFAUXDATA, self).__init__(register,
            'SNIFFAUXDATA', 'FRC.SNIFFCTRL.SNIFFAUXDATA', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFBR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFBR, self).__init__(register,
            'SNIFFBR', 'FRC.SNIFFCTRL.SNIFFBR', 'read-write',
            u"",
            8, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_SNIFFCTRL_SNIFFSLEEPCTRL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_SNIFFCTRL_SNIFFSLEEPCTRL, self).__init__(register,
            'SNIFFSLEEPCTRL', 'FRC.SNIFFCTRL.SNIFFSLEEPCTRL', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_AUXDATA_AUXDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_AUXDATA_AUXDATA, self).__init__(register,
            'AUXDATA', 'FRC.AUXDATA.AUXDATA', 'read-write',
            u"",
            0, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RAWCTRL_TXRAWMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RAWCTRL_TXRAWMODE, self).__init__(register,
            'TXRAWMODE', 'FRC.RAWCTRL.TXRAWMODE', 'read-write',
            u"",
            0, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RAWCTRL_RXRAWMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RAWCTRL_RXRAWMODE, self).__init__(register,
            'RXRAWMODE', 'FRC.RAWCTRL.RXRAWMODE', 'read-write',
            u"",
            2, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RAWCTRL_RXRAWRANDOM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RAWCTRL_RXRAWRANDOM, self).__init__(register,
            'RXRAWRANDOM', 'FRC.RAWCTRL.RXRAWRANDOM', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RAWCTRL_RXRAWTRIGGER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RAWCTRL_RXRAWTRIGGER, self).__init__(register,
            'RXRAWTRIGGER', 'FRC.RAWCTRL.RXRAWTRIGGER', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RAWCTRL_RXRAWPRSSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RAWCTRL_RXRAWPRSSEL, self).__init__(register,
            'RXRAWPRSSEL', 'FRC.RAWCTRL.RXRAWPRSSEL', 'read-write',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_RXRAWDATA_RXRAWDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_RXRAWDATA_RXRAWDATA, self).__init__(register,
            'RXRAWDATA', 'FRC.RXRAWDATA.RXRAWDATA', 'read-only',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_PAUSEDATA_PAUSEDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_PAUSEDATA_PAUSEDATA, self).__init__(register,
            'PAUSEDATA', 'FRC.PAUSEDATA.PAUSEDATA', 'read-only',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_LIKELYCONVSTATE_LIKELYCONVSTATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_LIKELYCONVSTATE_LIKELYCONVSTATE, self).__init__(register,
            'LIKELYCONVSTATE', 'FRC.LIKELYCONVSTATE.LIKELYCONVSTATE', 'read-only',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENTNEXT_INTELEMENTNEXT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENTNEXT_INTELEMENTNEXT, self).__init__(register,
            'INTELEMENTNEXT', 'FRC.INTELEMENTNEXT.INTELEMENTNEXT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTWRITEPOINT_INTWRITEPOINT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTWRITEPOINT_INTWRITEPOINT, self).__init__(register,
            'INTWRITEPOINT', 'FRC.INTWRITEPOINT.INTWRITEPOINT', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTREADPOINT_INTREADPOINT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTREADPOINT_INTREADPOINT, self).__init__(register,
            'INTREADPOINT', 'FRC.INTREADPOINT.INTREADPOINT', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD0_WORDS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD0_WORDS, self).__init__(register,
            'WORDS', 'FRC.FCD0.WORDS', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD0_BUFFER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD0_BUFFER, self).__init__(register,
            'BUFFER', 'FRC.FCD0.BUFFER', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD0_INCLUDECRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD0_INCLUDECRC, self).__init__(register,
            'INCLUDECRC', 'FRC.FCD0.INCLUDECRC', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD0_CALCCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD0_CALCCRC, self).__init__(register,
            'CALCCRC', 'FRC.FCD0.CALCCRC', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD0_SKIPCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD0_SKIPCRC, self).__init__(register,
            'SKIPCRC', 'FRC.FCD0.SKIPCRC', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD0_SKIPWHITE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD0_SKIPWHITE, self).__init__(register,
            'SKIPWHITE', 'FRC.FCD0.SKIPWHITE', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD1_WORDS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD1_WORDS, self).__init__(register,
            'WORDS', 'FRC.FCD1.WORDS', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD1_BUFFER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD1_BUFFER, self).__init__(register,
            'BUFFER', 'FRC.FCD1.BUFFER', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD1_INCLUDECRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD1_INCLUDECRC, self).__init__(register,
            'INCLUDECRC', 'FRC.FCD1.INCLUDECRC', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD1_CALCCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD1_CALCCRC, self).__init__(register,
            'CALCCRC', 'FRC.FCD1.CALCCRC', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD1_SKIPCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD1_SKIPCRC, self).__init__(register,
            'SKIPCRC', 'FRC.FCD1.SKIPCRC', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD1_SKIPWHITE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD1_SKIPWHITE, self).__init__(register,
            'SKIPWHITE', 'FRC.FCD1.SKIPWHITE', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD2_WORDS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD2_WORDS, self).__init__(register,
            'WORDS', 'FRC.FCD2.WORDS', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD2_BUFFER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD2_BUFFER, self).__init__(register,
            'BUFFER', 'FRC.FCD2.BUFFER', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD2_INCLUDECRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD2_INCLUDECRC, self).__init__(register,
            'INCLUDECRC', 'FRC.FCD2.INCLUDECRC', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD2_CALCCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD2_CALCCRC, self).__init__(register,
            'CALCCRC', 'FRC.FCD2.CALCCRC', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD2_SKIPCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD2_SKIPCRC, self).__init__(register,
            'SKIPCRC', 'FRC.FCD2.SKIPCRC', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD2_SKIPWHITE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD2_SKIPWHITE, self).__init__(register,
            'SKIPWHITE', 'FRC.FCD2.SKIPWHITE', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD3_WORDS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD3_WORDS, self).__init__(register,
            'WORDS', 'FRC.FCD3.WORDS', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD3_BUFFER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD3_BUFFER, self).__init__(register,
            'BUFFER', 'FRC.FCD3.BUFFER', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD3_INCLUDECRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD3_INCLUDECRC, self).__init__(register,
            'INCLUDECRC', 'FRC.FCD3.INCLUDECRC', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD3_CALCCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD3_CALCCRC, self).__init__(register,
            'CALCCRC', 'FRC.FCD3.CALCCRC', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD3_SKIPCRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD3_SKIPCRC, self).__init__(register,
            'SKIPCRC', 'FRC.FCD3.SKIPCRC', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_FCD3_SKIPWHITE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_FCD3_SKIPWHITE, self).__init__(register,
            'SKIPWHITE', 'FRC.FCD3.SKIPWHITE', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT0_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT0_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT0.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT1_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT1_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT1.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT2_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT2_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT2.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT3_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT3_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT3.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT4_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT4_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT4.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT5_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT5_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT5.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT6_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT6_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT6.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT7_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT7_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT7.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT8_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT8_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT8.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT9_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT9_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT9.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT10_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT10_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT10.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT11_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT11_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT11.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT12_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT12_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT12.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT13_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT13_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT13.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT14_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT14_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT14.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_FRC_INTELEMENT15_INTELEMENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_FRC_INTELEMENT15_INTELEMENT, self).__init__(register,
            'INTELEMENT', 'FRC.INTELEMENT15.INTELEMENT', 'read-only',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


