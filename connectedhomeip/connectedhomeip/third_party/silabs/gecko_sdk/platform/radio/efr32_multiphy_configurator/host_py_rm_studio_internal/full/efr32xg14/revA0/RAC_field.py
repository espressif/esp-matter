
# -*- coding: utf-8 -*-

from . static import Base_RM_Field


class RM_Field_RAC_RXENSRCEN_SWRXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_SWRXEN, self).__init__(register,
            'SWRXEN', 'RAC.RXENSRCEN.SWRXEN', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_CHANNELBUSYEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_CHANNELBUSYEN, self).__init__(register,
            'CHANNELBUSYEN', 'RAC.RXENSRCEN.CHANNELBUSYEN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_TIMDETEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_TIMDETEN, self).__init__(register,
            'TIMDETEN', 'RAC.RXENSRCEN.TIMDETEN', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_PREDETEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_PREDETEN, self).__init__(register,
            'PREDETEN', 'RAC.RXENSRCEN.PREDETEN', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_FRAMEDETEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_FRAMEDETEN, self).__init__(register,
            'FRAMEDETEN', 'RAC.RXENSRCEN.FRAMEDETEN', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_DEMODRXREQEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_DEMODRXREQEN, self).__init__(register,
            'DEMODRXREQEN', 'RAC.RXENSRCEN.DEMODRXREQEN', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_PRSRXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_PRSRXEN, self).__init__(register,
            'PRSRXEN', 'RAC.RXENSRCEN.PRSRXEN', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_PRSRXENSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_PRSRXENSEL, self).__init__(register,
            'PRSRXENSEL', 'RAC.RXENSRCEN.PRSRXENSEL', 'read-write',
            u"",
            14, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RXENSRCEN_PRSRXENEM2WU(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RXENSRCEN_PRSRXENEM2WU, self).__init__(register,
            'PRSRXENEM2WU', 'RAC.RXENSRCEN.PRSRXENEM2WU', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_RXMASK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_RXMASK, self).__init__(register,
            'RXMASK', 'RAC.STATUS.RXMASK', 'read-only',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_FORCESTATEACTIVE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_FORCESTATEACTIVE, self).__init__(register,
            'FORCESTATEACTIVE', 'RAC.STATUS.FORCESTATEACTIVE', 'read-only',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_TXAFTERFRAMEPEND(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_TXAFTERFRAMEPEND, self).__init__(register,
            'TXAFTERFRAMEPEND', 'RAC.STATUS.TXAFTERFRAMEPEND', 'read-only',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_TXAFTERFRAMEACTIVE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_TXAFTERFRAMEACTIVE, self).__init__(register,
            'TXAFTERFRAMEACTIVE', 'RAC.STATUS.TXAFTERFRAMEACTIVE', 'read-only',
            u"",
            21, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_STATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_STATE, self).__init__(register,
            'STATE', 'RAC.STATUS.STATE', 'read-only',
            u"",
            24, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_DEMODENS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_DEMODENS, self).__init__(register,
            'DEMODENS', 'RAC.STATUS.DEMODENS', 'read-only',
            u"",
            29, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_TXENS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_TXENS, self).__init__(register,
            'TXENS', 'RAC.STATUS.TXENS', 'read-only',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS_RXENS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS_RXENS, self).__init__(register,
            'RXENS', 'RAC.STATUS.RXENS', 'read-only',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_TXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_TXEN, self).__init__(register,
            'TXEN', 'RAC.CMD.TXEN', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_FORCETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_FORCETX, self).__init__(register,
            'FORCETX', 'RAC.CMD.FORCETX', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_TXONCCA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_TXONCCA, self).__init__(register,
            'TXONCCA', 'RAC.CMD.TXONCCA', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_CLEARTXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_CLEARTXEN, self).__init__(register,
            'CLEARTXEN', 'RAC.CMD.CLEARTXEN', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_TXAFTERFRAME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_TXAFTERFRAME, self).__init__(register,
            'TXAFTERFRAME', 'RAC.CMD.TXAFTERFRAME', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_TXDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_TXDIS, self).__init__(register,
            'TXDIS', 'RAC.CMD.TXDIS', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_CLEARRXOVERFLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_CLEARRXOVERFLOW, self).__init__(register,
            'CLEARRXOVERFLOW', 'RAC.CMD.CLEARRXOVERFLOW', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_RXCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_RXCAL, self).__init__(register,
            'RXCAL', 'RAC.CMD.RXCAL', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_RXDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_RXDIS, self).__init__(register,
            'RXDIS', 'RAC.CMD.RXDIS', 'write-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_PAENSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_PAENSET, self).__init__(register,
            'PAENSET', 'RAC.CMD.PAENSET', 'write-only',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_PAENCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_PAENCLEAR, self).__init__(register,
            'PAENCLEAR', 'RAC.CMD.PAENCLEAR', 'write-only',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_LNAENSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_LNAENSET, self).__init__(register,
            'LNAENSET', 'RAC.CMD.LNAENSET', 'write-only',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_LNAENCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_LNAENCLEAR, self).__init__(register,
            'LNAENCLEAR', 'RAC.CMD.LNAENCLEAR', 'write-only',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_DEMODENSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_DEMODENSET, self).__init__(register,
            'DEMODENSET', 'RAC.CMD.DEMODENSET', 'write-only',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CMD_DEMODENCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CMD_DEMODENCLEAR, self).__init__(register,
            'DEMODENCLEAR', 'RAC.CMD.DEMODENCLEAR', 'write-only',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_FORCEDISABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_FORCEDISABLE, self).__init__(register,
            'FORCEDISABLE', 'RAC.CTRL.FORCEDISABLE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSTXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSTXEN, self).__init__(register,
            'PRSTXEN', 'RAC.CTRL.PRSTXEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_TXAFTERRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_TXAFTERRX, self).__init__(register,
            'TXAFTERRX', 'RAC.CTRL.TXAFTERRX', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSMODE, self).__init__(register,
            'PRSMODE', 'RAC.CTRL.PRSMODE', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSCLR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSCLR, self).__init__(register,
            'PRSCLR', 'RAC.CTRL.PRSCLR', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_TXPOSTPONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_TXPOSTPONE, self).__init__(register,
            'TXPOSTPONE', 'RAC.CTRL.TXPOSTPONE', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_ACTIVEPOL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_ACTIVEPOL, self).__init__(register,
            'ACTIVEPOL', 'RAC.CTRL.ACTIVEPOL', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PAENPOL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PAENPOL, self).__init__(register,
            'PAENPOL', 'RAC.CTRL.PAENPOL', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_LNAENPOL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_LNAENPOL, self).__init__(register,
            'LNAENPOL', 'RAC.CTRL.LNAENPOL', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSRXDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSRXDIS, self).__init__(register,
            'PRSRXDIS', 'RAC.CTRL.PRSRXDIS', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSRXDISSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSRXDISSEL, self).__init__(register,
            'PRSRXDISSEL', 'RAC.CTRL.PRSRXDISSEL', 'read-write',
            u"",
            11, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSFORCETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSFORCETX, self).__init__(register,
            'PRSFORCETX', 'RAC.CTRL.PRSFORCETX', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSFORCETXSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSFORCETXSEL, self).__init__(register,
            'PRSFORCETXSEL', 'RAC.CTRL.PRSFORCETXSEL', 'read-write',
            u"",
            17, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSTXENSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSTXENSEL, self).__init__(register,
            'PRSTXENSEL', 'RAC.CTRL.PRSTXENSEL', 'read-write',
            u"",
            22, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CTRL_PRSCLRSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CTRL_PRSCLRSEL, self).__init__(register,
            'PRSCLRSEL', 'RAC.CTRL.PRSCLRSEL', 'read-write',
            u"",
            27, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_FORCESTATE_FORCESTATE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_FORCESTATE_FORCESTATE, self).__init__(register,
            'FORCESTATE', 'RAC.FORCESTATE.FORCESTATE', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_STATECHANGE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_STATECHANGE, self).__init__(register,
            'STATECHANGE', 'RAC.IF.STATECHANGE', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_STIMCMPEV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_STIMCMPEV, self).__init__(register,
            'STIMCMPEV', 'RAC.IF.STIMCMPEV', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_BUSERROR, self).__init__(register,
            'BUSERROR', 'RAC.IF.BUSERROR', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_SEQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_SEQ, self).__init__(register,
            'SEQ', 'RAC.IF.SEQ', 'read-only',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_PAVHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_PAVHIGH, self).__init__(register,
            'PAVHIGH', 'RAC.IF.PAVHIGH', 'read-only',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_PAVLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_PAVLOW, self).__init__(register,
            'PAVLOW', 'RAC.IF.PAVLOW', 'read-only',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IF_PABATHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IF_PABATHIGH, self).__init__(register,
            'PABATHIGH', 'RAC.IF.PABATHIGH', 'read-only',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_STATECHANGE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_STATECHANGE, self).__init__(register,
            'STATECHANGE', 'RAC.IFS.STATECHANGE', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_STIMCMPEV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_STIMCMPEV, self).__init__(register,
            'STIMCMPEV', 'RAC.IFS.STIMCMPEV', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_BUSERROR, self).__init__(register,
            'BUSERROR', 'RAC.IFS.BUSERROR', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_SEQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_SEQ, self).__init__(register,
            'SEQ', 'RAC.IFS.SEQ', 'write-only',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_PAVHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_PAVHIGH, self).__init__(register,
            'PAVHIGH', 'RAC.IFS.PAVHIGH', 'write-only',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_PAVLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_PAVLOW, self).__init__(register,
            'PAVLOW', 'RAC.IFS.PAVLOW', 'write-only',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFS_PABATHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFS_PABATHIGH, self).__init__(register,
            'PABATHIGH', 'RAC.IFS.PABATHIGH', 'write-only',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_STATECHANGE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_STATECHANGE, self).__init__(register,
            'STATECHANGE', 'RAC.IFC.STATECHANGE', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_STIMCMPEV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_STIMCMPEV, self).__init__(register,
            'STIMCMPEV', 'RAC.IFC.STIMCMPEV', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_BUSERROR, self).__init__(register,
            'BUSERROR', 'RAC.IFC.BUSERROR', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_SEQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_SEQ, self).__init__(register,
            'SEQ', 'RAC.IFC.SEQ', 'write-only',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_PAVHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_PAVHIGH, self).__init__(register,
            'PAVHIGH', 'RAC.IFC.PAVHIGH', 'write-only',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_PAVLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_PAVLOW, self).__init__(register,
            'PAVLOW', 'RAC.IFC.PAVLOW', 'write-only',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFC_PABATHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFC_PABATHIGH, self).__init__(register,
            'PABATHIGH', 'RAC.IFC.PABATHIGH', 'write-only',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_STATECHANGE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_STATECHANGE, self).__init__(register,
            'STATECHANGE', 'RAC.IEN.STATECHANGE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_STIMCMPEV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_STIMCMPEV, self).__init__(register,
            'STIMCMPEV', 'RAC.IEN.STIMCMPEV', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_BUSERROR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_BUSERROR, self).__init__(register,
            'BUSERROR', 'RAC.IEN.BUSERROR', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_SEQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_SEQ, self).__init__(register,
            'SEQ', 'RAC.IEN.SEQ', 'read-write',
            u"",
            16, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_PAVHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_PAVHIGH, self).__init__(register,
            'PAVHIGH', 'RAC.IEN.PAVHIGH', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_PAVLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_PAVLOW, self).__init__(register,
            'PAVLOW', 'RAC.IEN.PAVLOW', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IEN_PABATHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IEN_PABATHIGH, self).__init__(register,
            'PABATHIGH', 'RAC.IEN.PABATHIGH', 'read-write',
            u"",
            27, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSCTRL_LVDSEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSCTRL_LVDSEN, self).__init__(register,
            'LVDSEN', 'RAC.LVDSCTRL.LVDSEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSCTRL_LVDSCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSCTRL_LVDSCURR, self).__init__(register,
            'LVDSCURR', 'RAC.LVDSCTRL.LVDSCURR', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSCTRL_LVDSTESTMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSCTRL_LVDSTESTMODE, self).__init__(register,
            'LVDSTESTMODE', 'RAC.LVDSCTRL.LVDSTESTMODE', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSCTRL_LVDSTESTDATA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSCTRL_LVDSTESTDATA, self).__init__(register,
            'LVDSTESTDATA', 'RAC.LVDSCTRL.LVDSTESTDATA', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSCTRL_LVDSCMCONFIG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSCTRL_LVDSCMCONFIG, self).__init__(register,
            'LVDSCMCONFIG', 'RAC.LVDSCTRL.LVDSCMCONFIG', 'read-write',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSIDLESEQ_LVDSIDLESEQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSIDLESEQ_LVDSIDLESEQ, self).__init__(register,
            'LVDSIDLESEQ', 'RAC.LVDSIDLESEQ.LVDSIDLESEQ', 'read-write',
            u"",
            0, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSROUTE_LVDSPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSROUTE_LVDSPEN, self).__init__(register,
            'LVDSPEN', 'RAC.LVDSROUTE.LVDSPEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LVDSROUTE_LVDSLOCATION(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LVDSROUTE_LVDSLOCATION, self).__init__(register,
            'LVDSLOCATION', 'RAC.LVDSROUTE.LVDSLOCATION', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMECTRL_EN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMECTRL_EN, self).__init__(register,
            'EN', 'RAC.HFXORETIMECTRL.EN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMECTRL_DIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMECTRL_DIS, self).__init__(register,
            'DIS', 'RAC.HFXORETIMECTRL.DIS', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMECTRL_RESET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMECTRL_RESET, self).__init__(register,
            'RESET', 'RAC.HFXORETIMECTRL.RESET', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMECTRL_LIMITH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMECTRL_LIMITH, self).__init__(register,
            'LIMITH', 'RAC.HFXORETIMECTRL.LIMITH', 'read-write',
            u"",
            4, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMECTRL_LIMITL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMECTRL_LIMITL, self).__init__(register,
            'LIMITL', 'RAC.HFXORETIMECTRL.LIMITL', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMESTATUS_CLKSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMESTATUS_CLKSEL, self).__init__(register,
            'CLKSEL', 'RAC.HFXORETIMESTATUS.CLKSEL', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_HFXORETIMESTATUS_RERESETN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_HFXORETIMESTATUS_RERESETN, self).__init__(register,
            'RERESETN', 'RAC.HFXORETIMESTATUS.RERESETN', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_TESTCTRL_MODEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_TESTCTRL_MODEN, self).__init__(register,
            'MODEN', 'RAC.TESTCTRL.MODEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_TESTCTRL_DEMODEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_TESTCTRL_DEMODEN, self).__init__(register,
            'DEMODEN', 'RAC.TESTCTRL.DEMODEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_TESTCTRL_AUX2RFSENSE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_TESTCTRL_AUX2RFSENSE, self).__init__(register,
            'AUX2RFSENSE', 'RAC.TESTCTRL.AUX2RFSENSE', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_TESTCTRL_LOOPBACK2LNAINPUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_TESTCTRL_LOOPBACK2LNAINPUT, self).__init__(register,
            'LOOPBACK2LNAINPUT', 'RAC.TESTCTRL.LOOPBACK2LNAINPUT', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_TESTCTRL_LOOPBACK2LNAOUTPUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_TESTCTRL_LOOPBACK2LNAOUTPUT, self).__init__(register,
            'LOOPBACK2LNAOUTPUT', 'RAC.TESTCTRL.LOOPBACK2LNAOUTPUT', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_STOPPED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_STOPPED, self).__init__(register,
            'STOPPED', 'RAC.SEQSTATUS.STOPPED', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_BKPT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_BKPT, self).__init__(register,
            'BKPT', 'RAC.SEQSTATUS.BKPT', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_WAITING(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_WAITING, self).__init__(register,
            'WAITING', 'RAC.SEQSTATUS.WAITING', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_WAITMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_WAITMODE, self).__init__(register,
            'WAITMODE', 'RAC.SEQSTATUS.WAITMODE', 'read-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_DONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_DONE, self).__init__(register,
            'DONE', 'RAC.SEQSTATUS.DONE', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_NEG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_NEG, self).__init__(register,
            'NEG', 'RAC.SEQSTATUS.NEG', 'read-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_POS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_POS, self).__init__(register,
            'POS', 'RAC.SEQSTATUS.POS', 'read-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_ZERO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_ZERO, self).__init__(register,
            'ZERO', 'RAC.SEQSTATUS.ZERO', 'read-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_CARRY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_CARRY, self).__init__(register,
            'CARRY', 'RAC.SEQSTATUS.CARRY', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQSTATUS_ABORTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQSTATUS_ABORTEN, self).__init__(register,
            'ABORTEN', 'RAC.SEQSTATUS.ABORTEN', 'read-only',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_HALT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_HALT, self).__init__(register,
            'HALT', 'RAC.SEQCMD.HALT', 'write-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_STEP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_STEP, self).__init__(register,
            'STEP', 'RAC.SEQCMD.STEP', 'write-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_RESUME(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_RESUME, self).__init__(register,
            'RESUME', 'RAC.SEQCMD.RESUME', 'write-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_BKPTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_BKPTEN, self).__init__(register,
            'BKPTEN', 'RAC.SEQCMD.BKPTEN', 'write-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_BKPTDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_BKPTDIS, self).__init__(register,
            'BKPTDIS', 'RAC.SEQCMD.BKPTDIS', 'write-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_ABORT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_ABORT, self).__init__(register,
            'ABORT', 'RAC.SEQCMD.ABORT', 'write-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_ABORTENSET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_ABORTENSET, self).__init__(register,
            'ABORTENSET', 'RAC.SEQCMD.ABORTENSET', 'write-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCMD_ABORTENCLEAR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCMD_ABORTENCLEAR, self).__init__(register,
            'ABORTENCLEAR', 'RAC.SEQCMD.ABORTENCLEAR', 'write-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_BREAKPOINT_BKPADDR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_BREAKPOINT_BKPADDR, self).__init__(register,
            'BKPADDR', 'RAC.BREAKPOINT.BKPADDR', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R0_R0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R0_R0, self).__init__(register,
            'R0', 'RAC.R0.R0', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R1_R1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R1_R1, self).__init__(register,
            'R1', 'RAC.R1.R1', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R2_R2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R2_R2, self).__init__(register,
            'R2', 'RAC.R2.R2', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R3_R3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R3_R3, self).__init__(register,
            'R3', 'RAC.R3.R3', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R4_R4(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R4_R4, self).__init__(register,
            'R4', 'RAC.R4.R4', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R5_R5(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R5_R5, self).__init__(register,
            'R5', 'RAC.R5.R5', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R6_R6(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R6_R6, self).__init__(register,
            'R6', 'RAC.R6.R6', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_R7_R7(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_R7_R7, self).__init__(register,
            'R7', 'RAC.R7.R7', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_STCMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_STCMP, self).__init__(register,
            'STCMP', 'RAC.WAITMASK.STCMP', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_FRCRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_FRCRX, self).__init__(register,
            'FRCRX', 'RAC.WAITMASK.FRCRX', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_FRCTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_FRCTX, self).__init__(register,
            'FRCTX', 'RAC.WAITMASK.FRCTX', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_PRSEVENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_PRSEVENT, self).__init__(register,
            'PRSEVENT', 'RAC.WAITMASK.PRSEVENT', 'read-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_DEMODRXREQCLR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_DEMODRXREQCLR, self).__init__(register,
            'DEMODRXREQCLR', 'RAC.WAITMASK.DEMODRXREQCLR', 'read-only',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_SYNTHRDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_SYNTHRDY, self).__init__(register,
            'SYNTHRDY', 'RAC.WAITMASK.SYNTHRDY', 'read-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_RAMPDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_RAMPDONE, self).__init__(register,
            'RAMPDONE', 'RAC.WAITMASK.RAMPDONE', 'read-only',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_HFXORDY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_HFXORDY, self).__init__(register,
            'HFXORDY', 'RAC.WAITMASK.HFXORDY', 'read-only',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITMASK_FRCPAUSED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITMASK_FRCPAUSED, self).__init__(register,
            'FRCPAUSED', 'RAC.WAITMASK.FRCPAUSED', 'read-only',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_WAITSNSH_WAITSNSH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_WAITSNSH_WAITSNSH, self).__init__(register,
            'WAITSNSH', 'RAC.WAITSNSH.WAITSNSH', 'read-only',
            u"",
            0, 10)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STIMER_STIMER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STIMER_STIMER, self).__init__(register,
            'STIMER', 'RAC.STIMER.STIMER', 'read-only',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STIMERCOMP_STIMERCOMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STIMERCOMP_STIMERCOMP, self).__init__(register,
            'STIMERCOMP', 'RAC.STIMERCOMP.STIMERCOMP', 'read-write',
            u"",
            0, 16)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VECTADDR_VECTADDR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VECTADDR_VECTADDR, self).__init__(register,
            'VECTADDR', 'RAC.VECTADDR.VECTADDR', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCTRL_COMPACT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCTRL_COMPACT, self).__init__(register,
            'COMPACT', 'RAC.SEQCTRL.COMPACT', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCTRL_COMPINVALMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCTRL_COMPINVALMODE, self).__init__(register,
            'COMPINVALMODE', 'RAC.SEQCTRL.COMPINVALMODE', 'read-write',
            u"",
            1, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCTRL_PRSSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCTRL_PRSSEL, self).__init__(register,
            'PRSSEL', 'RAC.SEQCTRL.PRSSEL', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCTRL_STIMERDEBUGRUN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCTRL_STIMERDEBUGRUN, self).__init__(register,
            'STIMERDEBUGRUN', 'RAC.SEQCTRL.STIMERDEBUGRUN', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCTRL_CPUHALTREQEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCTRL_CPUHALTREQEN, self).__init__(register,
            'CPUHALTREQEN', 'RAC.SEQCTRL.CPUHALTREQEN', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SEQCTRL_SEQHALTUPONCPUHALTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SEQCTRL_SEQHALTUPONCPUHALTEN, self).__init__(register,
            'SEQHALTUPONCPUHALTEN', 'RAC.SEQCTRL.SEQHALTUPONCPUHALTEN', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PRESC_STIMER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PRESC_STIMER, self).__init__(register,
            'STIMER', 'RAC.PRESC.STIMER', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SR0_SR0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SR0_SR0, self).__init__(register,
            'SR0', 'RAC.SR0.SR0', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SR1_SR1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SR1_SR1, self).__init__(register,
            'SR1', 'RAC.SR1.SR1', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SR2_SR2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SR2_SR2, self).__init__(register,
            'SR2', 'RAC.SR2.SR2', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SR3_SR3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SR3_SR3, self).__init__(register,
            'SR3', 'RAC.SR3.SR3', 'read-write',
            u"",
            0, 32)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_VCOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_VCOEN, self).__init__(register,
            'VCOEN', 'RAC.SYNTHENCTRL.VCOEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_VCOSTARTUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_VCOSTARTUP, self).__init__(register,
            'VCOSTARTUP', 'RAC.SYNTHENCTRL.VCOSTARTUP', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_LODIVEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_LODIVEN, self).__init__(register,
            'LODIVEN', 'RAC.SYNTHENCTRL.LODIVEN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_CHPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_CHPEN, self).__init__(register,
            'CHPEN', 'RAC.SYNTHENCTRL.CHPEN', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_LPFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_LPFEN, self).__init__(register,
            'LPFEN', 'RAC.SYNTHENCTRL.LPFEN', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_LPFQUICKSTART(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_LPFQUICKSTART, self).__init__(register,
            'LPFQUICKSTART', 'RAC.SYNTHENCTRL.LPFQUICKSTART', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_VCBUFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_VCBUFEN, self).__init__(register,
            'VCBUFEN', 'RAC.SYNTHENCTRL.VCBUFEN', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_SYNTHCLKEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_SYNTHCLKEN, self).__init__(register,
            'SYNTHCLKEN', 'RAC.SYNTHENCTRL.SYNTHCLKEN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_SYNTHSTARTREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_SYNTHSTARTREQ, self).__init__(register,
            'SYNTHSTARTREQ', 'RAC.SYNTHENCTRL.SYNTHSTARTREQ', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_CHPLDOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_CHPLDOEN, self).__init__(register,
            'CHPLDOEN', 'RAC.SYNTHENCTRL.CHPLDOEN', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_LODIVSYNCCLKEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_LODIVSYNCCLKEN, self).__init__(register,
            'LODIVSYNCCLKEN', 'RAC.SYNTHENCTRL.LODIVSYNCCLKEN', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_MMDLDOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_MMDLDOEN, self).__init__(register,
            'MMDLDOEN', 'RAC.SYNTHENCTRL.MMDLDOEN', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_VCOLDOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_VCOLDOEN, self).__init__(register,
            'VCOLDOEN', 'RAC.SYNTHENCTRL.VCOLDOEN', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_VCODIVEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_VCODIVEN, self).__init__(register,
            'VCODIVEN', 'RAC.SYNTHENCTRL.VCODIVEN', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHENCTRL_LPFBWSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHENCTRL_LPFBWSEL, self).__init__(register,
            'LPFBWSEL', 'RAC.SYNTHENCTRL.LPFBWSEL', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_MMDLDOAMPBWRED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_MMDLDOAMPBWRED, self).__init__(register,
            'MMDLDOAMPBWRED', 'RAC.SYNTHREGCTRL.MMDLDOAMPBWRED', 'read-write',
            u"",
            5, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_MMDLDOAMPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_MMDLDOAMPCURR, self).__init__(register,
            'MMDLDOAMPCURR', 'RAC.SYNTHREGCTRL.MMDLDOAMPCURR', 'read-write',
            u"",
            7, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_MMDLDOVREFTRIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_MMDLDOVREFTRIM, self).__init__(register,
            'MMDLDOVREFTRIM', 'RAC.SYNTHREGCTRL.MMDLDOVREFTRIM', 'read-write',
            u"",
            10, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_VCOLDOAMPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_VCOLDOAMPCURR, self).__init__(register,
            'VCOLDOAMPCURR', 'RAC.SYNTHREGCTRL.VCOLDOAMPCURR', 'read-write',
            u"",
            13, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_VCOLDOVREFTRIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_VCOLDOVREFTRIM, self).__init__(register,
            'VCOLDOVREFTRIM', 'RAC.SYNTHREGCTRL.VCOLDOVREFTRIM', 'read-write',
            u"",
            16, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_CHPLDOAMPBWRED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_CHPLDOAMPBWRED, self).__init__(register,
            'CHPLDOAMPBWRED', 'RAC.SYNTHREGCTRL.CHPLDOAMPBWRED', 'read-write',
            u"",
            19, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_CHPLDOAMPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_CHPLDOAMPCURR, self).__init__(register,
            'CHPLDOAMPCURR', 'RAC.SYNTHREGCTRL.CHPLDOAMPCURR', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHREGCTRL_CHPLDOVREFTRIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHREGCTRL_CHPLDOVREFTRIM, self).__init__(register,
            'CHPLDOVREFTRIM', 'RAC.SYNTHREGCTRL.CHPLDOVREFTRIM', 'read-write',
            u"",
            24, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCOAMPLITUDE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCOAMPLITUDE, self).__init__(register,
            'VCOAMPLITUDE', 'RAC.VCOCTRL.VCOAMPLITUDE', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCODETAMPLITUDE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCODETAMPLITUDE, self).__init__(register,
            'VCODETAMPLITUDE', 'RAC.VCOCTRL.VCODETAMPLITUDE', 'read-write',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCODETEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCODETEN, self).__init__(register,
            'VCODETEN', 'RAC.VCOCTRL.VCODETEN', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCODETMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCODETMODE, self).__init__(register,
            'VCODETMODE', 'RAC.VCOCTRL.VCODETMODE', 'read-write',
            u"",
            9, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCOAREGCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCOAREGCURR, self).__init__(register,
            'VCOAREGCURR', 'RAC.VCOCTRL.VCOAREGCURR', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCOCREGCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCOCREGCURR, self).__init__(register,
            'VCOCREGCURR', 'RAC.VCOCTRL.VCOCREGCURR', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_VCOCTRL_VCODIVCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_VCOCTRL_VCODIVCURR, self).__init__(register,
            'VCODIVCURR', 'RAC.VCOCTRL.VCODIVCURR', 'read-write',
            u"",
            24, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL_MMDDIVDCDC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL_MMDDIVDCDC, self).__init__(register,
            'MMDDIVDCDC', 'RAC.MMDCTRL.MMDDIVDCDC', 'read-write',
            u"",
            0, 9)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL_MMDDIVRSDCDC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL_MMDDIVRSDCDC, self).__init__(register,
            'MMDDIVRSDCDC', 'RAC.MMDCTRL.MMDDIVRSDCDC', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL_MMDDIVRSDIG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL_MMDDIVRSDIG, self).__init__(register,
            'MMDDIVRSDIG', 'RAC.MMDCTRL.MMDDIVRSDIG', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL_MMDENDCDC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL_MMDENDCDC, self).__init__(register,
            'MMDENDCDC', 'RAC.MMDCTRL.MMDENDCDC', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL_MMDENRSDCDC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL_MMDENRSDCDC, self).__init__(register,
            'MMDENRSDCDC', 'RAC.MMDCTRL.MMDENRSDCDC', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL_MMDENRSDIG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL_MMDENRSDIG, self).__init__(register,
            'MMDENRSDIG', 'RAC.MMDCTRL.MMDENRSDIG', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL_CHPBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL_CHPBIAS, self).__init__(register,
            'CHPBIAS', 'RAC.CHPCTRL.CHPBIAS', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL_CHPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL_CHPCURR, self).__init__(register,
            'CHPCURR', 'RAC.CHPCTRL.CHPCURR', 'read-write',
            u"",
            3, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL_CHPOUTPUTTRISTATEMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL_CHPOUTPUTTRISTATEMODE, self).__init__(register,
            'CHPOUTPUTTRISTATEMODE', 'RAC.CHPCTRL.CHPOUTPUTTRISTATEMODE', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL_CHPSWITCHEDBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL_CHPSWITCHEDBIASEN, self).__init__(register,
            'CHPSWITCHEDBIASEN', 'RAC.CHPCTRL.CHPSWITCHEDBIASEN', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCAL_PSRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCAL_PSRC, self).__init__(register,
            'PSRC', 'RAC.CHPCAL.PSRC', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCAL_NSRC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCAL_NSRC, self).__init__(register,
            'NSRC', 'RAC.CHPCAL.NSRC', 'read-write',
            u"",
            3, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LPFCTRL_LPFBWRX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LPFCTRL_LPFBWRX, self).__init__(register,
            'LPFBWRX', 'RAC.LPFCTRL.LPFBWRX', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LPFCTRL_MIRACLEMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LPFCTRL_MIRACLEMODE, self).__init__(register,
            'MIRACLEMODE', 'RAC.LPFCTRL.MIRACLEMODE', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LPFCTRL_LPFINPUTCAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LPFCTRL_LPFINPUTCAP, self).__init__(register,
            'LPFINPUTCAP', 'RAC.LPFCTRL.LPFINPUTCAP', 'read-write',
            u"",
            14, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LPFCTRL_LPFSWITCHINGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LPFCTRL_LPFSWITCHINGEN, self).__init__(register,
            'LPFSWITCHINGEN', 'RAC.LPFCTRL.LPFSWITCHINGEN', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LPFCTRL_LPFGNDSWITCHINGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LPFCTRL_LPFGNDSWITCHINGEN, self).__init__(register,
            'LPFGNDSWITCHINGEN', 'RAC.LPFCTRL.LPFGNDSWITCHINGEN', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LPFCTRL_LPFBWTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LPFCTRL_LPFBWTX, self).__init__(register,
            'LPFBWTX', 'RAC.LPFCTRL.LPFBWTX', 'read-write',
            u"",
            20, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHCTRL_LODIVTXEN0DBM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHCTRL_LODIVTXEN0DBM, self).__init__(register,
            'LODIVTXEN0DBM', 'RAC.SYNTHCTRL.LODIVTXEN0DBM', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHCTRL_LODIVTXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHCTRL_LODIVTXEN, self).__init__(register,
            'LODIVTXEN', 'RAC.SYNTHCTRL.LODIVTXEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHCTRL_PFDTRADMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHCTRL_PFDTRADMODE, self).__init__(register,
            'PFDTRADMODE', 'RAC.SYNTHCTRL.PFDTRADMODE', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE, self).__init__(register,
            'MMDPOWERBALANCEDISABLE', 'RAC.SYNTHCTRL.MMDPOWERBALANCEDISABLE', 'read-write',
            u"",
            10, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_AUXREGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_AUXREGEN, self).__init__(register,
            'AUXREGEN', 'RAC.AUXCTRL.AUXREGEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_VCOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_VCOEN, self).__init__(register,
            'VCOEN', 'RAC.AUXCTRL.VCOEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_VCOSTARTUP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_VCOSTARTUP, self).__init__(register,
            'VCOSTARTUP', 'RAC.AUXCTRL.VCOSTARTUP', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_CHPEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_CHPEN, self).__init__(register,
            'CHPEN', 'RAC.AUXCTRL.CHPEN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_CHPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_CHPCURR, self).__init__(register,
            'CHPCURR', 'RAC.AUXCTRL.CHPCURR', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_LODIVEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_LODIVEN, self).__init__(register,
            'LODIVEN', 'RAC.AUXCTRL.LODIVEN', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_AUXSYNTHCLKEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_AUXSYNTHCLKEN, self).__init__(register,
            'AUXSYNTHCLKEN', 'RAC.AUXCTRL.AUXSYNTHCLKEN', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_LODIVSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_LODIVSEL, self).__init__(register,
            'LODIVSEL', 'RAC.AUXCTRL.LODIVSEL', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_RXAMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_RXAMP, self).__init__(register,
            'RXAMP', 'RAC.AUXCTRL.RXAMP', 'read-write',
            u"",
            11, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_LDOAMPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_LDOAMPCURR, self).__init__(register,
            'LDOAMPCURR', 'RAC.AUXCTRL.LDOAMPCURR', 'read-write',
            u"",
            17, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_LDOVREFTRIM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_LDOVREFTRIM, self).__init__(register,
            'LDOVREFTRIM', 'RAC.AUXCTRL.LDOVREFTRIM', 'read-write',
            u"",
            20, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_FPLLDIGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_FPLLDIGEN, self).__init__(register,
            'FPLLDIGEN', 'RAC.AUXCTRL.FPLLDIGEN', 'read-write',
            u"",
            23, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXCTRL_LPFRES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXCTRL_LPFRES, self).__init__(register,
            'LPFRES', 'RAC.AUXCTRL.LPFRES', 'read-write',
            u"",
            24, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXENCTRL_LDOEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXENCTRL_LDOEN, self).__init__(register,
            'LDOEN', 'RAC.AUXENCTRL.LDOEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXENCTRL_VCBUFEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXENCTRL_VCBUFEN, self).__init__(register,
            'VCBUFEN', 'RAC.AUXENCTRL.VCBUFEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXENCTRL_LODIVAUXEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXENCTRL_LODIVAUXEN, self).__init__(register,
            'LODIVAUXEN', 'RAC.AUXENCTRL.LODIVAUXEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_AUXENCTRL_LODIVAUXEN0DBM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_AUXENCTRL_LODIVAUXEN0DBM, self).__init__(register,
            'LODIVAUXEN0DBM', 'RAC.AUXENCTRL.LODIVAUXEN0DBM', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_IFFILTSTANDBY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_IFFILTSTANDBY, self).__init__(register,
            'IFFILTSTANDBY', 'RAC.RFENCTRL.IFFILTSTANDBY', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_IFFILTEREN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_IFFILTEREN, self).__init__(register,
            'IFFILTEREN', 'RAC.RFENCTRL.IFFILTEREN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_PKDEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_PKDEN, self).__init__(register,
            'PKDEN', 'RAC.RFENCTRL.PKDEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_IFADCEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_IFADCEN, self).__init__(register,
            'IFADCEN', 'RAC.RFENCTRL.IFADCEN', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_IFADCCAPRESET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_IFADCCAPRESET, self).__init__(register,
            'IFADCCAPRESET', 'RAC.RFENCTRL.IFADCCAPRESET', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_IFADCCLKEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_IFADCCLKEN, self).__init__(register,
            'IFADCCLKEN', 'RAC.RFENCTRL.IFADCCLKEN', 'read-write',
            u"",
            6, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_RFBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_RFBIASEN, self).__init__(register,
            'RFBIASEN', 'RAC.RFENCTRL.RFBIASEN', 'read-write',
            u"",
            11, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_DEMEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_DEMEN, self).__init__(register,
            'DEMEN', 'RAC.RFENCTRL.DEMEN', 'read-write',
            u"",
            12, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENABLEI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENABLEI, self).__init__(register,
            'ENABLEI', 'RAC.RFENCTRL.ENABLEI', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENABLEQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENABLEQ, self).__init__(register,
            'ENABLEQ', 'RAC.RFENCTRL.ENABLEQ', 'read-write',
            u"",
            14, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENLDOCLK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENLDOCLK, self).__init__(register,
            'ENLDOCLK', 'RAC.RFENCTRL.ENLDOCLK', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENLDOSERIES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENLDOSERIES, self).__init__(register,
            'ENLDOSERIES', 'RAC.RFENCTRL.ENLDOSERIES', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENLDOSHUNTI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENLDOSHUNTI, self).__init__(register,
            'ENLDOSHUNTI', 'RAC.RFENCTRL.ENLDOSHUNTI', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENLDOSHUNTQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENLDOSHUNTQ, self).__init__(register,
            'ENLDOSHUNTQ', 'RAC.RFENCTRL.ENLDOSHUNTQ', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENLDOPGALNA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENLDOPGALNA, self).__init__(register,
            'ENLDOPGALNA', 'RAC.RFENCTRL.ENLDOPGALNA', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_ENPGA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_ENPGA, self).__init__(register,
            'ENPGA', 'RAC.RFENCTRL.ENPGA', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_PACTUNESEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_PACTUNESEL, self).__init__(register,
            'PACTUNESEL', 'RAC.RFENCTRL.PACTUNESEL', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL_SGPACTUNESEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL_SGPACTUNESEL, self).__init__(register,
            'SGPACTUNESEL', 'RAC.RFENCTRL.SGPACTUNESEL', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_LNAMIXSTANDBY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_LNAMIXSTANDBY, self).__init__(register,
            'LNAMIXSTANDBY', 'RAC.RFENCTRL0.LNAMIXSTANDBY', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_LNAMIXBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_LNAMIXBIASEN, self).__init__(register,
            'LNAMIXBIASEN', 'RAC.RFENCTRL0.LNAMIXBIASEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_LNAMIXLOBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_LNAMIXLOBIASEN, self).__init__(register,
            'LNAMIXLOBIASEN', 'RAC.RFENCTRL0.LNAMIXLOBIASEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_LNAMIXRFBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_LNAMIXRFBIASEN, self).__init__(register,
            'LNAMIXRFBIASEN', 'RAC.RFENCTRL0.LNAMIXRFBIASEN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_PASTANDBY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_PASTANDBY, self).__init__(register,
            'PASTANDBY', 'RAC.RFENCTRL0.PASTANDBY', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_PAEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_PAEN, self).__init__(register,
            'PAEN', 'RAC.RFENCTRL0.PAEN', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_PAOUTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_PAOUTEN, self).__init__(register,
            'PAOUTEN', 'RAC.RFENCTRL0.PAOUTEN', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_TRSW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_TRSW, self).__init__(register,
            'TRSW', 'RAC.RFENCTRL0.TRSW', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_CASCODEDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_CASCODEDIS, self).__init__(register,
            'CASCODEDIS', 'RAC.RFENCTRL0.CASCODEDIS', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFENCTRL0_STRIPESLICEDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFENCTRL0_STRIPESLICEDIS, self).__init__(register,
            'STRIPESLICEDIS', 'RAC.RFENCTRL0.STRIPESLICEDIS', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_RF2P4BANDWIDTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_RF2P4BANDWIDTH, self).__init__(register,
            'RF2P4BANDWIDTH', 'RAC.LNAMIXCTRL.RF2P4BANDWIDTH', 'read-write',
            u"",
            0, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_CASCODEBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_CASCODEBIAS, self).__init__(register,
            'CASCODEBIAS', 'RAC.LNAMIXCTRL.CASCODEBIAS', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_LOBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_LOBIAS, self).__init__(register,
            'LOBIAS', 'RAC.LNAMIXCTRL.LOBIAS', 'read-write',
            u"",
            11, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_VREG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_VREG, self).__init__(register,
            'VREG', 'RAC.LNAMIXCTRL.VREG', 'read-write',
            u"",
            14, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_RFBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_RFBIAS, self).__init__(register,
            'RFBIAS', 'RAC.LNAMIXCTRL.RFBIAS', 'read-write',
            u"",
            17, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_RISEFALLATT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_RISEFALLATT, self).__init__(register,
            'RISEFALLATT', 'RAC.LNAMIXCTRL.RISEFALLATT', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_LNAMIXAUXSYNTHINPUTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_LNAMIXAUXSYNTHINPUTEN, self).__init__(register,
            'LNAMIXAUXSYNTHINPUTEN', 'RAC.LNAMIXCTRL.LNAMIXAUXSYNTHINPUTEN', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL_ENLOAD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL_ENLOAD, self).__init__(register,
            'ENLOAD', 'RAC.LNAMIXCTRL.ENLOAD', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_RF2P4PASEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_RF2P4PASEL, self).__init__(register,
            'RF2P4PASEL', 'RAC.PACTRL0.RF2P4PASEL', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_RF2P4PAVDDSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_RF2P4PAVDDSEL, self).__init__(register,
            'RF2P4PAVDDSEL', 'RAC.PACTRL0.RF2P4PAVDDSEL', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_RF2P4RFVDDSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_RF2P4RFVDDSEL, self).__init__(register,
            'RF2P4RFVDDSEL', 'RAC.PACTRL0.RF2P4RFVDDSEL', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_BOOTSTRAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_BOOTSTRAP, self).__init__(register,
            'BOOTSTRAP', 'RAC.PACTRL0.BOOTSTRAP', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_RF2P4VDDPADET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_RF2P4VDDPADET, self).__init__(register,
            'RF2P4VDDPADET', 'RAC.PACTRL0.RF2P4VDDPADET', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_CASCODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_CASCODE, self).__init__(register,
            'CASCODE', 'RAC.PACTRL0.CASCODE', 'read-write',
            u"",
            6, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_SLICE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_SLICE, self).__init__(register,
            'SLICE', 'RAC.PACTRL0.SLICE', 'read-write',
            u"",
            14, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_STRIPE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_STRIPE, self).__init__(register,
            'STRIPE', 'RAC.PACTRL0.STRIPE', 'read-write',
            u"",
            24, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_DACGLITCHCTRL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_DACGLITCHCTRL, self).__init__(register,
            'DACGLITCHCTRL', 'RAC.PACTRL0.DACGLITCHCTRL', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTRL0_CASCODEBYPASSEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTRL0_CASCODEBYPASSEN, self).__init__(register,
            'CASCODEBYPASSEN', 'RAC.PACTRL0.CASCODEBYPASSEN', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAPKDCTRL_PKDEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAPKDCTRL_PKDEN, self).__init__(register,
            'PKDEN', 'RAC.PAPKDCTRL.PKDEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAPKDCTRL_VTLSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAPKDCTRL_VTLSEL, self).__init__(register,
            'VTLSEL', 'RAC.PAPKDCTRL.VTLSEL', 'read-write',
            u"",
            2, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAPKDCTRL_VTHSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAPKDCTRL_VTHSEL, self).__init__(register,
            'VTHSEL', 'RAC.PAPKDCTRL.VTHSEL', 'read-write',
            u"",
            8, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAPKDCTRL_CAPSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAPKDCTRL_CAPSEL, self).__init__(register,
            'CAPSEL', 'RAC.PAPKDCTRL.CAPSEL', 'read-write',
            u"",
            14, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAPKDCTRL_I2VCM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAPKDCTRL_I2VCM, self).__init__(register,
            'I2VCM', 'RAC.PAPKDCTRL.I2VCM', 'read-write',
            u"",
            18, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAPKDCTRL_PKDBIASTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAPKDCTRL_PKDBIASTH, self).__init__(register,
            'PKDBIASTH', 'RAC.PAPKDCTRL.PKDBIASTH', 'read-write',
            u"",
            22, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL0_LDOBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL0_LDOBIAS, self).__init__(register,
            'LDOBIAS', 'RAC.PABIASCTRL0.LDOBIAS', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL0_PABIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL0_PABIAS, self).__init__(register,
            'PABIAS', 'RAC.PABIASCTRL0.PABIAS', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL0_BUF0BIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL0_BUF0BIAS, self).__init__(register,
            'BUF0BIAS', 'RAC.PABIASCTRL0.BUF0BIAS', 'read-write',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL0_BUF12BIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL0_BUF12BIAS, self).__init__(register,
            'BUF12BIAS', 'RAC.PABIASCTRL0.BUF12BIAS', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL0_TXPOWER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL0_TXPOWER, self).__init__(register,
            'TXPOWER', 'RAC.PABIASCTRL0.TXPOWER', 'read-write',
            u"",
            22, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL0_CMGAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL0_CMGAIN, self).__init__(register,
            'CMGAIN', 'RAC.PABIASCTRL0.CMGAIN', 'read-write',
            u"",
            23, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL1_VLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL1_VLDO, self).__init__(register,
            'VLDO', 'RAC.PABIASCTRL1.VLDO', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL1_VLDOFB(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL1_VLDOFB, self).__init__(register,
            'VLDOFB', 'RAC.PABIASCTRL1.VLDOFB', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL1_VCASCODEHV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL1_VCASCODEHV, self).__init__(register,
            'VCASCODEHV', 'RAC.PABIASCTRL1.VCASCODEHV', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL1_VCASCODELV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL1_VCASCODELV, self).__init__(register,
            'VCASCODELV', 'RAC.PABIASCTRL1.VCASCODELV', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PABIASCTRL1_RF2P4VDDPATHRESHOLD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PABIASCTRL1_RF2P4VDDPATHRESHOLD, self).__init__(register,
            'RF2P4VDDPATHRESHOLD', 'RAC.PABIASCTRL1.RF2P4VDDPATHRESHOLD', 'read-write',
            u"",
            16, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_LNAMIXBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_LNAMIXBIASEN, self).__init__(register,
            'LNAMIXBIASEN', 'RAC.SGRFENCTRL0.LNAMIXBIASEN', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_LNAMIXLOBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_LNAMIXLOBIASEN, self).__init__(register,
            'LNAMIXLOBIASEN', 'RAC.SGRFENCTRL0.LNAMIXLOBIASEN', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_LNAMIXRFBIASEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_LNAMIXRFBIASEN, self).__init__(register,
            'LNAMIXRFBIASEN', 'RAC.SGRFENCTRL0.LNAMIXRFBIASEN', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_PASTANDBY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_PASTANDBY, self).__init__(register,
            'PASTANDBY', 'RAC.SGRFENCTRL0.PASTANDBY', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_PAEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_PAEN, self).__init__(register,
            'PAEN', 'RAC.SGRFENCTRL0.PAEN', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_PAOUTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_PAOUTEN, self).__init__(register,
            'PAOUTEN', 'RAC.SGRFENCTRL0.PAOUTEN', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_TRSW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_TRSW, self).__init__(register,
            'TRSW', 'RAC.SGRFENCTRL0.TRSW', 'read-write',
            u"",
            19, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_CASCODEDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_CASCODEDIS, self).__init__(register,
            'CASCODEDIS', 'RAC.SGRFENCTRL0.CASCODEDIS', 'read-write',
            u"",
            24, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGRFENCTRL0_STRIPESLICEDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGRFENCTRL0_STRIPESLICEDIS, self).__init__(register,
            'STRIPESLICEDIS', 'RAC.SGRFENCTRL0.STRIPESLICEDIS', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_CASCODEBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_CASCODEBIAS, self).__init__(register,
            'CASCODEBIAS', 'RAC.SGLNAMIXCTRL.CASCODEBIAS', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_LOBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_LOBIAS, self).__init__(register,
            'LOBIAS', 'RAC.SGLNAMIXCTRL.LOBIAS', 'read-write',
            u"",
            11, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_VREG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_VREG, self).__init__(register,
            'VREG', 'RAC.SGLNAMIXCTRL.VREG', 'read-write',
            u"",
            14, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_RFBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_RFBIAS, self).__init__(register,
            'RFBIAS', 'RAC.SGLNAMIXCTRL.RFBIAS', 'read-write',
            u"",
            17, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_RISEFALLATT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_RISEFALLATT, self).__init__(register,
            'RISEFALLATT', 'RAC.SGLNAMIXCTRL.RISEFALLATT', 'read-write',
            u"",
            20, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_SGREGAMPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_SGREGAMPCURR, self).__init__(register,
            'SGREGAMPCURR', 'RAC.SGLNAMIXCTRL.SGREGAMPCURR', 'read-write',
            u"",
            23, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_LNAMIXAUXSYNTHINPUTEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_LNAMIXAUXSYNTHINPUTEN, self).__init__(register,
            'LNAMIXAUXSYNTHINPUTEN', 'RAC.SGLNAMIXCTRL.LNAMIXAUXSYNTHINPUTEN', 'read-write',
            u"",
            26, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_SGREGAMPBWRED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_SGREGAMPBWRED, self).__init__(register,
            'SGREGAMPBWRED', 'RAC.SGLNAMIXCTRL.SGREGAMPBWRED', 'read-write',
            u"",
            28, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL_ENLOAD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL_ENLOAD, self).__init__(register,
            'ENLOAD', 'RAC.SGLNAMIXCTRL.ENLOAD', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_BOOTSTRAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_BOOTSTRAP, self).__init__(register,
            'BOOTSTRAP', 'RAC.SGPACTRL0.BOOTSTRAP', 'read-write',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_SGVBATDET(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_SGVBATDET, self).__init__(register,
            'SGVBATDET', 'RAC.SGPACTRL0.SGVBATDET', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_CASCODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_CASCODE, self).__init__(register,
            'CASCODE', 'RAC.SGPACTRL0.CASCODE', 'read-write',
            u"",
            6, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_SLICE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_SLICE, self).__init__(register,
            'SLICE', 'RAC.SGPACTRL0.SLICE', 'read-write',
            u"",
            14, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_STRIPE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_STRIPE, self).__init__(register,
            'STRIPE', 'RAC.SGPACTRL0.STRIPE', 'read-write',
            u"",
            24, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_DACGLITCHCTRL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_DACGLITCHCTRL, self).__init__(register,
            'DACGLITCHCTRL', 'RAC.SGPACTRL0.DACGLITCHCTRL', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPACTRL0_CASCODEBYPASSEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPACTRL0_CASCODEBYPASSEN, self).__init__(register,
            'CASCODEBYPASSEN', 'RAC.SGPACTRL0.CASCODEBYPASSEN', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPAPKDCTRL_PKDEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPAPKDCTRL_PKDEN, self).__init__(register,
            'PKDEN', 'RAC.SGPAPKDCTRL.PKDEN', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPAPKDCTRL_VTLSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPAPKDCTRL_VTLSEL, self).__init__(register,
            'VTLSEL', 'RAC.SGPAPKDCTRL.VTLSEL', 'read-write',
            u"",
            2, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPAPKDCTRL_VTHSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPAPKDCTRL_VTHSEL, self).__init__(register,
            'VTHSEL', 'RAC.SGPAPKDCTRL.VTHSEL', 'read-write',
            u"",
            8, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPAPKDCTRL_CAPSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPAPKDCTRL_CAPSEL, self).__init__(register,
            'CAPSEL', 'RAC.SGPAPKDCTRL.CAPSEL', 'read-write',
            u"",
            14, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPAPKDCTRL_I2VCM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPAPKDCTRL_I2VCM, self).__init__(register,
            'I2VCM', 'RAC.SGPAPKDCTRL.I2VCM', 'read-write',
            u"",
            18, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPAPKDCTRL_PKDBIASTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPAPKDCTRL_PKDBIASTH, self).__init__(register,
            'PKDBIASTH', 'RAC.SGPAPKDCTRL.PKDBIASTH', 'read-write',
            u"",
            22, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_LDOBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_LDOBIAS, self).__init__(register,
            'LDOBIAS', 'RAC.SGPABIASCTRL0.LDOBIAS', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_PABIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_PABIAS, self).__init__(register,
            'PABIAS', 'RAC.SGPABIASCTRL0.PABIAS', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_BUF0BIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_BUF0BIAS, self).__init__(register,
            'BUF0BIAS', 'RAC.SGPABIASCTRL0.BUF0BIAS', 'read-write',
            u"",
            6, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_BUF12BIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_BUF12BIAS, self).__init__(register,
            'BUF12BIAS', 'RAC.SGPABIASCTRL0.BUF12BIAS', 'read-write',
            u"",
            10, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_TXPOWER(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_TXPOWER, self).__init__(register,
            'TXPOWER', 'RAC.SGPABIASCTRL0.TXPOWER', 'read-write',
            u"",
            22, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_CMGAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_CMGAIN, self).__init__(register,
            'CMGAIN', 'RAC.SGPABIASCTRL0.CMGAIN', 'read-write',
            u"",
            23, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_SGDACFILTBANDWIDTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_SGDACFILTBANDWIDTH, self).__init__(register,
            'SGDACFILTBANDWIDTH', 'RAC.SGPABIASCTRL0.SGDACFILTBANDWIDTH', 'read-write',
            u"",
            24, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL0_SGPAOOKINCRMODDEPTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL0_SGPAOOKINCRMODDEPTH, self).__init__(register,
            'SGPAOOKINCRMODDEPTH', 'RAC.SGPABIASCTRL0.SGPAOOKINCRMODDEPTH', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL1_VLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL1_VLDO, self).__init__(register,
            'VLDO', 'RAC.SGPABIASCTRL1.VLDO', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL1_VLDOFB(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL1_VLDOFB, self).__init__(register,
            'VLDOFB', 'RAC.SGPABIASCTRL1.VLDOFB', 'read-write',
            u"",
            4, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL1_VCASCODEHV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL1_VCASCODEHV, self).__init__(register,
            'VCASCODEHV', 'RAC.SGPABIASCTRL1.VCASCODEHV', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL1_VCASCODELV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL1_VCASCODELV, self).__init__(register,
            'VCASCODELV', 'RAC.SGPABIASCTRL1.VCASCODELV', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL1_SGVBATDETTHRESHOLD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL1_SGVBATDETTHRESHOLD, self).__init__(register,
            'SGVBATDETTHRESHOLD', 'RAC.SGPABIASCTRL1.SGVBATDETTHRESHOLD', 'read-write',
            u"",
            18, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGPABIASCTRL1_SGTRIMLOWVBATCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGPABIASCTRL1_SGTRIMLOWVBATCURR, self).__init__(register,
            'SGTRIMLOWVBATCURR', 'RAC.SGPABIASCTRL1.SGTRIMLOWVBATCURR', 'read-write',
            u"",
            20, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFSTATUS_MODRAMPUPDONE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFSTATUS_MODRAMPUPDONE, self).__init__(register,
            'MODRAMPUPDONE', 'RAC.RFSTATUS.MODRAMPUPDONE', 'read-only',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFSTATUS_PAVHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFSTATUS_PAVHIGH, self).__init__(register,
            'PAVHIGH', 'RAC.RFSTATUS.PAVHIGH', 'read-only',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFSTATUS_PAVLOW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFSTATUS_PAVLOW, self).__init__(register,
            'PAVLOW', 'RAC.RFSTATUS.PAVLOW', 'read-only',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFSTATUS_PABATHIGH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFSTATUS_PABATHIGH, self).__init__(register,
            'PABATHIGH', 'RAC.RFSTATUS.PABATHIGH', 'read-only',
            u"",
            3, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCTRL_LDOVREF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCTRL_LDOVREF, self).__init__(register,
            'LDOVREF', 'RAC.RFBIASCTRL.LDOVREF', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCTRL_LDOAMPCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCTRL_LDOAMPCURR, self).__init__(register,
            'LDOAMPCURR', 'RAC.RFBIASCTRL.LDOAMPCURR', 'read-write',
            u"",
            4, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCTRL_STARTUPSUPPLY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCTRL_STARTUPSUPPLY, self).__init__(register,
            'STARTUPSUPPLY', 'RAC.RFBIASCTRL.STARTUPSUPPLY', 'read-write',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCTRL_STARTUPCORE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCTRL_STARTUPCORE, self).__init__(register,
            'STARTUPCORE', 'RAC.RFBIASCTRL.STARTUPCORE', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCTRL_DISBOOTSTRAP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCTRL_DISBOOTSTRAP, self).__init__(register,
            'DISBOOTSTRAP', 'RAC.RFBIASCTRL.DISBOOTSTRAP', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCAL_VREF(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCAL_VREF, self).__init__(register,
            'VREF', 'RAC.RFBIASCAL.VREF', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCAL_BIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCAL_BIAS, self).__init__(register,
            'BIAS', 'RAC.RFBIASCAL.BIAS', 'read-write',
            u"",
            8, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RFBIASCAL_TEMPCO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RFBIASCAL_TEMPCO, self).__init__(register,
            'TEMPCO', 'RAC.RFBIASCAL.TEMPCO', 'read-write',
            u"",
            16, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMAUXPLLCLK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMAUXPLLCLK, self).__init__(register,
            'TRIMAUXPLLCLK', 'RAC.LNAMIXCTRL1.TRIMAUXPLLCLK', 'read-write',
            u"",
            1, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMTRSWGATEV(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMTRSWGATEV, self).__init__(register,
            'TRIMTRSWGATEV', 'RAC.LNAMIXCTRL1.TRIMTRSWGATEV', 'read-write',
            u"",
            5, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMVCASLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMVCASLDO, self).__init__(register,
            'TRIMVCASLDO', 'RAC.LNAMIXCTRL1.TRIMVCASLDO', 'read-write',
            u"",
            7, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMVREFLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMVREFLDO, self).__init__(register,
            'TRIMVREFLDO', 'RAC.LNAMIXCTRL1.TRIMVREFLDO', 'read-write',
            u"",
            8, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMVREGMIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMVREGMIN, self).__init__(register,
            'TRIMVREGMIN', 'RAC.LNAMIXCTRL1.TRIMVREGMIN', 'read-write',
            u"",
            11, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMAUXBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMAUXBIAS, self).__init__(register,
            'TRIMAUXBIAS', 'RAC.LNAMIXCTRL1.TRIMAUXBIAS', 'read-write',
            u"",
            13, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_ENBIASCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_ENBIASCAL, self).__init__(register,
            'ENBIASCAL', 'RAC.LNAMIXCTRL1.ENBIASCAL', 'read-write',
            u"",
            15, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_STATUSBIASCAL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_STATUSBIASCAL, self).__init__(register,
            'STATUSBIASCAL', 'RAC.LNAMIXCTRL1.STATUSBIASCAL', 'read-only',
            u"",
            16, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMAUXPLLGAIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMAUXPLLGAIN, self).__init__(register,
            'TRIMAUXPLLGAIN', 'RAC.LNAMIXCTRL1.TRIMAUXPLLGAIN', 'read-write',
            u"",
            17, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_LNAMIXCTRL1_TRIMLDOAMPBWRED(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_LNAMIXCTRL1_TRIMLDOAMPBWRED, self).__init__(register,
            'TRIMLDOAMPBWRED', 'RAC.LNAMIXCTRL1.TRIMLDOAMPBWRED', 'read-write',
            u"",
            21, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_VLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_VLDO, self).__init__(register,
            'VLDO', 'RAC.IFPGACTRL.VLDO', 'read-write',
            u"",
            1, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_BANDSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_BANDSEL, self).__init__(register,
            'BANDSEL', 'RAC.IFPGACTRL.BANDSEL', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_CASCBIAS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_CASCBIAS, self).__init__(register,
            'CASCBIAS', 'RAC.IFPGACTRL.CASCBIAS', 'read-write',
            u"",
            5, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_TRIMVCASLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_TRIMVCASLDO, self).__init__(register,
            'TRIMVCASLDO', 'RAC.IFPGACTRL.TRIMVCASLDO', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_TRIMVCM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_TRIMVCM, self).__init__(register,
            'TRIMVCM', 'RAC.IFPGACTRL.TRIMVCM', 'read-write',
            u"",
            9, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_TRIMVREFLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_TRIMVREFLDO, self).__init__(register,
            'TRIMVREFLDO', 'RAC.IFPGACTRL.TRIMVREFLDO', 'read-write',
            u"",
            12, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_TRIMVREGMIN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_TRIMVREGMIN, self).__init__(register,
            'TRIMVREGMIN', 'RAC.IFPGACTRL.TRIMVREGMIN', 'read-write',
            u"",
            15, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_ENHYST(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_ENHYST, self).__init__(register,
            'ENHYST', 'RAC.IFPGACTRL.ENHYST', 'read-write',
            u"",
            17, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL_ENOFFD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL_ENOFFD, self).__init__(register,
            'ENOFFD', 'RAC.IFPGACTRL.ENOFFD', 'read-write',
            u"",
            18, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACAL_IRAMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACAL_IRAMP, self).__init__(register,
            'IRAMP', 'RAC.IFPGACAL.IRAMP', 'read-write',
            u"",
            0, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACAL_IRPHASE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACAL_IRPHASE, self).__init__(register,
            'IRPHASE', 'RAC.IFPGACAL.IRPHASE', 'read-write',
            u"",
            8, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACAL_OFFSETI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACAL_OFFSETI, self).__init__(register,
            'OFFSETI', 'RAC.IFPGACAL.OFFSETI', 'read-write',
            u"",
            16, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACAL_OFFSETQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACAL_OFFSETQ, self).__init__(register,
            'OFFSETQ', 'RAC.IFPGACAL.OFFSETQ', 'read-write',
            u"",
            24, 7)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFFILTCTRL_BANDWIDTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFFILTCTRL_BANDWIDTH, self).__init__(register,
            'BANDWIDTH', 'RAC.IFFILTCTRL.BANDWIDTH', 'read-write',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFFILTCTRL_CENTFREQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFFILTCTRL_CENTFREQ, self).__init__(register,
            'CENTFREQ', 'RAC.IFFILTCTRL.CENTFREQ', 'read-write',
            u"",
            5, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFFILTCTRL_VCM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFFILTCTRL_VCM, self).__init__(register,
            'VCM', 'RAC.IFFILTCTRL.VCM', 'read-write',
            u"",
            18, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFFILTCTRL_VREG(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFFILTCTRL_VREG, self).__init__(register,
            'VREG', 'RAC.IFFILTCTRL.VREG', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFFILTCTRL_DROOP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFFILTCTRL_DROOP, self).__init__(register,
            'DROOP', 'RAC.IFFILTCTRL.DROOP', 'read-write',
            u"",
            25, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFFILTCTRL_INPUTSHORT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFFILTCTRL_INPUTSHORT, self).__init__(register,
            'INPUTSHORT', 'RAC.IFFILTCTRL.INPUTSHORT', 'read-write',
            u"",
            31, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_REALMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_REALMODE, self).__init__(register,
            'REALMODE', 'RAC.IFADCCTRL.REALMODE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_INPUTSEL(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_INPUTSEL, self).__init__(register,
            'INPUTSEL', 'RAC.IFADCCTRL.INPUTSEL', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_INPUTSCALE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_INPUTSCALE, self).__init__(register,
            'INPUTSCALE', 'RAC.IFADCCTRL.INPUTSCALE', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_SHORTI(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_SHORTI, self).__init__(register,
            'SHORTI', 'RAC.IFADCCTRL.SHORTI', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_SHORTQ(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_SHORTQ, self).__init__(register,
            'SHORTQ', 'RAC.IFADCCTRL.SHORTQ', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_VLDOSERIES(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_VLDOSERIES, self).__init__(register,
            'VLDOSERIES', 'RAC.IFADCCTRL.VLDOSERIES', 'read-write',
            u"",
            6, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_VLDOSERIESCURR(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_VLDOSERIESCURR, self).__init__(register,
            'VLDOSERIESCURR', 'RAC.IFADCCTRL.VLDOSERIESCURR', 'read-write',
            u"",
            9, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_VLDOSHUNT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_VLDOSHUNT, self).__init__(register,
            'VLDOSHUNT', 'RAC.IFADCCTRL.VLDOSHUNT', 'read-write',
            u"",
            12, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_VLDOCLKGEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_VLDOCLKGEN, self).__init__(register,
            'VLDOCLKGEN', 'RAC.IFADCCTRL.VLDOCLKGEN', 'read-write',
            u"",
            14, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_VCM(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_VCM, self).__init__(register,
            'VCM', 'RAC.IFADCCTRL.VCM', 'read-write',
            u"",
            16, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_OTA1CURRENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_OTA1CURRENT, self).__init__(register,
            'OTA1CURRENT', 'RAC.IFADCCTRL.OTA1CURRENT', 'read-write',
            u"",
            19, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_OTA2CURRENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_OTA2CURRENT, self).__init__(register,
            'OTA2CURRENT', 'RAC.IFADCCTRL.OTA2CURRENT', 'read-write',
            u"",
            21, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_OTA3CURRENT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_OTA3CURRENT, self).__init__(register,
            'OTA3CURRENT', 'RAC.IFADCCTRL.OTA3CURRENT', 'read-write',
            u"",
            23, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_SATURATIONDETDIS(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_SATURATIONDETDIS, self).__init__(register,
            'SATURATIONDETDIS', 'RAC.IFADCCTRL.SATURATIONDETDIS', 'read-write',
            u"",
            25, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_REGENCLKDELAY(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_REGENCLKDELAY, self).__init__(register,
            'REGENCLKDELAY', 'RAC.IFADCCTRL.REGENCLKDELAY', 'read-write',
            u"",
            26, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_ENABLECLK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_ENABLECLK, self).__init__(register,
            'ENABLECLK', 'RAC.IFADCCTRL.ENABLECLK', 'read-write',
            u"",
            29, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCTRL_INVERTCLK(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCTRL_INVERTCLK, self).__init__(register,
            'INVERTCLK', 'RAC.IFADCCTRL.INVERTCLK', 'read-write',
            u"",
            30, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCAL_CALEN(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCAL_CALEN, self).__init__(register,
            'CALEN', 'RAC.IFADCCAL.CALEN', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFADCCAL_RCCALOUT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFADCCAL_RCCALOUT, self).__init__(register,
            'RCCALOUT', 'RAC.IFADCCAL.RCCALOUT', 'read-only',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PAENCTRL_PARAMP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PAENCTRL_PARAMP, self).__init__(register,
            'PARAMP', 'RAC.PAENCTRL.PARAMP', 'read-write',
            u"",
            8, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTUNECTRL_PACTUNERX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTUNECTRL_PACTUNERX, self).__init__(register,
            'PACTUNERX', 'RAC.PACTUNECTRL.PACTUNERX', 'read-write',
            u"",
            0, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTUNECTRL_SGPACTUNERX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTUNECTRL_SGPACTUNERX, self).__init__(register,
            'SGPACTUNERX', 'RAC.PACTUNECTRL.SGPACTUNERX', 'read-write',
            u"",
            4, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTUNECTRL_PACTUNETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTUNECTRL_PACTUNETX, self).__init__(register,
            'PACTUNETX', 'RAC.PACTUNECTRL.PACTUNETX', 'read-write',
            u"",
            16, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_PACTUNECTRL_SGPACTUNETX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_PACTUNECTRL_SGPACTUNETX, self).__init__(register,
            'SGPACTUNETX', 'RAC.PACTUNECTRL.SGPACTUNETX', 'read-write',
            u"",
            20, 5)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RCTUNE_IFADCRCTUNE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RCTUNE_IFADCRCTUNE, self).__init__(register,
            'IFADCRCTUNE', 'RAC.RCTUNE.IFADCRCTUNE', 'read-write',
            u"",
            0, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_RCTUNE_IFFILT(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_RCTUNE_IFFILT, self).__init__(register,
            'IFFILT', 'RAC.RCTUNE.IFFILT', 'read-write',
            u"",
            16, 6)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_APC_ENPASTATUSVDDPA(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_APC_ENPASTATUSVDDPA, self).__init__(register,
            'ENPASTATUSVDDPA', 'RAC.APC.ENPASTATUSVDDPA', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_APC_ENPASTATUSPKDVTH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_APC_ENPASTATUSPKDVTH, self).__init__(register,
            'ENPASTATUSPKDVTH', 'RAC.APC.ENPASTATUSPKDVTH', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_APC_ENAPCSW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_APC_ENAPCSW, self).__init__(register,
            'ENAPCSW', 'RAC.APC.ENAPCSW', 'read-write',
            u"",
            2, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_APC_AMPCONTROLLIMITSW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_APC_AMPCONTROLLIMITSW, self).__init__(register,
            'AMPCONTROLLIMITSW', 'RAC.APC.AMPCONTROLLIMITSW', 'read-write',
            u"",
            24, 8)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_LNAMIXERSPARE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_LNAMIXERSPARE, self).__init__(register,
            'LNAMIXERSPARE', 'RAC.SPARE.LNAMIXERSPARE', 'read-write',
            u"",
            8, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_IFFILTSPARE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_IFFILTSPARE, self).__init__(register,
            'IFFILTSPARE', 'RAC.SPARE.IFFILTSPARE', 'read-write',
            u"",
            10, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_IFPGASPARE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_IFPGASPARE, self).__init__(register,
            'IFPGASPARE', 'RAC.SPARE.IFPGASPARE', 'read-write',
            u"",
            13, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_IFFILTSPARE0(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_IFFILTSPARE0, self).__init__(register,
            'IFFILTSPARE0', 'RAC.SPARE.IFFILTSPARE0', 'read-write',
            u"",
            15, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_IFFILTSPARE1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_IFFILTSPARE1, self).__init__(register,
            'IFFILTSPARE1', 'RAC.SPARE.IFFILTSPARE1', 'read-write',
            u"",
            17, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_IFFILTSPARE2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_IFFILTSPARE2, self).__init__(register,
            'IFFILTSPARE2', 'RAC.SPARE.IFFILTSPARE2', 'read-write',
            u"",
            19, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_IFADCSPARE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_IFADCSPARE, self).__init__(register,
            'IFADCSPARE', 'RAC.SPARE.IFADCSPARE', 'read-write',
            u"",
            21, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_PASPARE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_PASPARE, self).__init__(register,
            'PASPARE', 'RAC.SPARE.PASPARE', 'read-write',
            u"",
            24, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SPARE_SGPASPARE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SPARE_SGPASPARE, self).__init__(register,
            'SGPASPARE', 'RAC.SPARE.SGPASPARE', 'read-write',
            u"",
            27, 3)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_IFPGACTRL1_LBWMODE(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_IFPGACTRL1_LBWMODE, self).__init__(register,
            'LBWMODE', 'RAC.IFPGACTRL1.LBWMODE', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL1_ENRFPKD(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL1_ENRFPKD, self).__init__(register,
            'ENRFPKD', 'RAC.SGLNAMIXCTRL1.ENRFPKD', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL1_TRIMACC(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL1_TRIMACC, self).__init__(register,
            'TRIMACC', 'RAC.SGLNAMIXCTRL1.TRIMACC', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL1_TRIMNPATHBW(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL1_TRIMNPATHBW, self).__init__(register,
            'TRIMNPATHBW', 'RAC.SGLNAMIXCTRL1.TRIMNPATHBW', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL1_TRIMRESP(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL1_TRIMRESP, self).__init__(register,
            'TRIMRESP', 'RAC.SGLNAMIXCTRL1.TRIMRESP', 'read-write',
            u"",
            4, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_SGLNAMIXCTRL1_TRIMTHRESH(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_SGLNAMIXCTRL1_TRIMTHRESH, self).__init__(register,
            'TRIMTHRESH', 'RAC.SGLNAMIXCTRL1.TRIMTHRESH', 'read-write',
            u"",
            5, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL1_BYPREPLDOTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL1_BYPREPLDOTX, self).__init__(register,
            'BYPREPLDOTX', 'RAC.CHPCTRL1.BYPREPLDOTX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL1_BYPREPLDORX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL1_BYPREPLDORX, self).__init__(register,
            'BYPREPLDORX', 'RAC.CHPCTRL1.BYPREPLDORX', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_CHPCTRL1_TRIMREPLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_CHPCTRL1_TRIMREPLDO, self).__init__(register,
            'TRIMREPLDO', 'RAC.CHPCTRL1.TRIMREPLDO', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL1_BYPREPLDOTX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL1_BYPREPLDOTX, self).__init__(register,
            'BYPREPLDOTX', 'RAC.MMDCTRL1.BYPREPLDOTX', 'read-write',
            u"",
            0, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL1_BYPREPLDORX(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL1_BYPREPLDORX, self).__init__(register,
            'BYPREPLDORX', 'RAC.MMDCTRL1.BYPREPLDORX', 'read-write',
            u"",
            1, 1)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_MMDCTRL1_TRIMREPLDO(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_MMDCTRL1_TRIMREPLDO, self).__init__(register,
            'TRIMREPLDO', 'RAC.MMDCTRL1.TRIMREPLDO', 'read-write',
            u"",
            2, 2)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS2_PREVSTATE1(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS2_PREVSTATE1, self).__init__(register,
            'PREVSTATE1', 'RAC.STATUS2.PREVSTATE1', 'read-only',
            u"",
            0, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS2_PREVSTATE2(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS2_PREVSTATE2, self).__init__(register,
            'PREVSTATE2', 'RAC.STATUS2.PREVSTATE2', 'read-only',
            u"",
            4, 4)
        self.__dict__['zz_frozen'] = True


class RM_Field_RAC_STATUS2_PREVSTATE3(Base_RM_Field):
    def __init__(self, register):
        self.__dict__['zz_frozen'] = False
        super(RM_Field_RAC_STATUS2_PREVSTATE3, self).__init__(register,
            'PREVSTATE3', 'RAC.STATUS2.PREVSTATE3', 'read-only',
            u"",
            8, 4)
        self.__dict__['zz_frozen'] = True


