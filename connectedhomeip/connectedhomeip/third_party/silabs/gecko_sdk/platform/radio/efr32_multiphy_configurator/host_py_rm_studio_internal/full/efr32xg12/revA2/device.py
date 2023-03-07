
# -*- coding: utf-8 -*-

"""
Device
^^^^^^
This package provides a top-level device class, ``RM_Device_EFR32XG12XFULL_RevA2``,
which is instantiated in either an offline mode for stand-alone simulated
register I/O or in a live session with a real part.

  .. autoclass::  RM_Device_EFR32XG12XFULL_RevA2
     :members:
     :show-inheritance:
     :inherited-members:

The ARM CMSIS System View Description (SVD) XML hierarchy is reflected in
this package module composition. The ``device.py`` imports a number of
peripheral modules, which in turn import register, field, and optional enum
modules.

"""


__all__ = [ 'RM_Device_EFR32XG12XFULL_RevA2' ]

from . static import Base_RM_Device, RM_SVD_Info
from . AGC import *
from . CRC import *
from . FRC import *
from . MODEM import *
from . RAC import *
from . SEQ import *
from . SYNTH import *

import imp
import os

_PKG_PATH = os.path.dirname(os.path.realpath(__file__))
_EXCLUDED_REGS_MOD_NAME = 'excluded_regs'
_EXCLUDED_REGS_LIST_NAME = 'EXCLUDED_REGS'


class RM_Device_EFR32XG12XFULL_RevA2(Base_RM_Device):
    """
    The top level register map class. This class can be used in an offline
    mode for stand-alone simulated register I/O. In this case the class will
    use a dictionary initialized with reset values from the CMSIS SVD XML.

    .. code-block:: py

        # Example: Use register map package in offline mode
        import sys
        sys.path.append("path/to/this/package")

        from host_py_rm_studio_internal import RM_Factory
        rm_device_class = RM_Factory("JUMBO", "A0")
        rm = rm_device_class(label='offline_rm')

    The class is used by the base application classes in the Python Debug Bench
    to control the part. This requires an RPC interface provided by the
    pyvsrpc package. If you want to use live outside the PDB, you can use the
    JLINK_AccessManager class from the pyjlinklib package.

    .. code-block:: py

        # Example: Use register map with live J-Link connection
        import sys
        sys.path.append("path/to/the/packages")

        from pyrmsvd import *
        from pyjlinklib import *
        from host_py_rm_studio_internal import RM_Factory

        dut_label = 'dut_1'
        accessMgr = JLINK_AccessManager(dut_label, JLINK_ARM_OPTIONS(), False)
        accessMgr.Connect()

        # auto detect part and request corresponding RM class
        hwinfo = EFRDetectPlatform(self.accessMgr)
        rm_device_class = RM_Factory(hwinfo.family_name, hwinfo.rev_name)
        rmIO = RegisterMapInterface(accessMgr.ReadRegister,
                                    accessMgr.WriteRegister)
        rm = rm_device_class(rmIO, dut_label)

    The ``svdInfo`` property is an instance of :class:`pyrmsvd.static.common.svdinfo.RM_SVD_Info`.

    """

    def __init__(self, rmio=None, label='offline_rm'):
        """
        Register the ``RegisterMapInterface`` instance and the logging label.

        :type  rmio: :class:`pyrmsvd.static.common.regmapio.RegisterMapInterface` or ``NoneType``
        :param rmio: The register map I/O instance, which contains the \
                     read/write functions for the registers. If ``None``, then \
                     the class will automatically create an internal ``RegisterMapInterface`` \
                     with an :class:`pyrmsvd.static.common.accessmgr.Offline_AccessManager` \
                     instance for operation in offline mode.
        :type  label: ``str``
        :param label: The register map DUT label to use in the system logger.
        """
        self.__dict__['zz_frozen'] = False
        super(RM_Device_EFR32XG12XFULL_RevA2, self).__init__(rmio, label,
            'EFR32XG12XFULL',
            RM_SVD_Info('EFR32XG12XFULL_SEQ.svd', 'f50f054f8c847bcf4639864e55bdf3f3'))

        self.AGC = RM_Peripheral_AGC(self.zz_rmio, self.zz_label)
        self.zz_pdict['AGC'] = self.AGC
        self.CRC = RM_Peripheral_CRC(self.zz_rmio, self.zz_label)
        self.zz_pdict['CRC'] = self.CRC
        self.FRC = RM_Peripheral_FRC(self.zz_rmio, self.zz_label)
        self.zz_pdict['FRC'] = self.FRC
        self.MODEM = RM_Peripheral_MODEM(self.zz_rmio, self.zz_label)
        self.zz_pdict['MODEM'] = self.MODEM
        self.RAC = RM_Peripheral_RAC(self.zz_rmio, self.zz_label)
        self.zz_pdict['RAC'] = self.RAC
        self.SEQ = RM_Peripheral_SEQ(self.zz_rmio, self.zz_label)
        self.zz_pdict['SEQ'] = self.SEQ
        self.SYNTH = RM_Peripheral_SYNTH(self.zz_rmio, self.zz_label)
        self.zz_pdict['SYNTH'] = self.SYNTH

        if self.offline:
            # assign the default reset values to the simulated register dictionary
            for key in self.zz_pdict:
                self.zz_pdict[key].assignRegDefault()
        # build the register address-to-names mapping
        for pkey in self.zz_pdict:
            per_obj = self.zz_pdict[pkey]
            # TODO: Remove deprecated self.zz_reg_addr_to_name in future pyrmsvd 2.x release
            per_obj.getAddressNameMap(self.zz_reg_addr_to_name)
            per_obj.getAddressNamesMap(self.zz_reg_addr_to_names)
            if per_obj.isAliased():
                # register device-level method to query the self.zz_reg_addr_to_names dict
                per_obj.registerAddressToNamesCb(self.addressToNames)

        self.__dict__['zz_frozen'] = True

        # Process an optional exclusion list that will remove a register
        # from a unfiltered register dump operation performed in the dump()
        # method. The register is also removed from the generated list in
        # the buildRegFilterList() method. Requires pyrmsvd support, so
        # check if this version of the base class supports register exclusion.

        excluded_regs_fn = os.path.join(_PKG_PATH, _EXCLUDED_REGS_MOD_NAME + '.py')
        if os.path.exists(excluded_regs_fn) and hasattr(self, 'excludeFromDumpByName'):
            mod = imp.load_source(_EXCLUDED_REGS_MOD_NAME, excluded_regs_fn)
            excluded_regs = getattr(mod, _EXCLUDED_REGS_LIST_NAME)
            self.excludeFromDumpByName(excluded_regs)