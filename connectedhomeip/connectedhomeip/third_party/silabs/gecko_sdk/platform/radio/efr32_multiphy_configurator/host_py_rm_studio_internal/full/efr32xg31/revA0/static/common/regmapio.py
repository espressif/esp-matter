
"""
Register Map I/O
^^^^^^^^^^^^^^^^
The register map I/O occurs via the :class:`pyrmsvd.static.common.regmapio.RegisterMapInterface`
instance, which is registered in the top-level device class..

  .. autoclass:: RegisterMapInterface
     :members:
     :show-inheritance:
     :inherited-members:

"""

__all__ = [
    'GetFieldValue',
    'SetFieldValue',
    'RegisterMapInterface',
    'RegisterRangeOverride'
]
try:
    unicode = unicode
except NameError:
    # 'unicode' is undefined, must be Python 3
    str = str
    unicode = str
    bytes = bytes
    basestring = (str,bytes)
else:
    # 'unicode' exists, must be Python 2
    str = str
    unicode = unicode
    bytes = str
    basestring = basestring
import os
import logging
import logging.handlers

from .. interface import IRegMapIO
from .. common import RegMapAccessError, RegMapLoggingError


def GetFieldValue(regValue, lsb, fsize):
    """
    An internal utility function to parse a field value from a register value.

    :type  regValue: ``int`` or ``long``
    :param regValue: The value of the register to parse
    :type  lsb: ``int``
    :param lsb: The least significant bit of the field
    :type  fsize: ``int``
    :param fsize: The size of the field in bits
    :rtype: ``int`` or ``long``
    :return: The field value
    """
    msb = lsb + fsize
    mask = (pow(2, msb) - 1) & ~(pow(2, lsb) - 1)
    return (regValue & mask) >> lsb


def SetFieldValue(regValue, lsb, fsize, fieldValue):
    """
    An internal utility function to assign a field value to a register value.
    Perform range check on fieldValue using fsize.

    :type  regValue: ``int`` or ``long``
    :param regValue: The value of the register to parse
    :type  lsb: ``int``
    :param lsb: The least significant bit of the field
    :type  fsize: ``int``
    :param fsize: The size of the field in bits
    :type  fieldValue: ``int``
    :param fieldValue: The new field value
    :rtype: ``int`` or ``long``
    :return: The new register value
    """
    if (-1 << fsize) & fieldValue:
        raise ValueError("field value '{}' exceeds range of {} bits".format(fieldValue,
                                                                            fsize))
    msb = lsb + fsize
    mask = ~((pow(2, msb) - 1) & ~(pow(2, lsb) - 1))
    return (regValue & mask) | (fieldValue << lsb)


class RegisterRangeOverride(object):
    """
    This class provides a way to override the default reader/writer methods
    for a given address range.
    """
    def __init__(self, name, start_addr, end_addr, reader, writer):
        """Initialize register override information.

        Args:
            name (str): The name of this override range. Used as key in register map.
            start_addr (int): The starting address range to override.
            end_addr (int): The ending address range to override.
            reader(function): The reader function with address parameter.
            writer(function): The writer function with address and data parameters.

        """
        assert isinstance(name, basestring), "name must be type str"
        assert isinstance(start_addr, (int, long)), "addr_range must be int or long"
        assert isinstance(end_addr, (int, long)), "addr_range must be int or long"
        assert callable(reader), "reader must be function or method"
        assert callable(writer), "writer must be function or method"
        self.name = name
        self.start_addr = start_addr
        self.end_addr = end_addr
        self._read = reader
        self._write = writer

    def read(self, address):
        """The address to read."""
        return self._read(address)

    def write(self, address, value):
        """The address and value to write."""
        return self._write(address, value)

    def __repr__(self):
        out = ""
        out += "Name:   {}\n".format(self.name)
        out += "Range:  {:#010x}-{:#010x}\n".format(self.start_addr,
                                                    self.end_addr)
        out += "Reader: {}\n".format(self._read)
        out += "Writer: {}\n".format(self._write)
        return out


class RegisterMapInterface(IRegMapIO):
    """
    This class defines the register map I/O instance (``rmIO``) that
    is passed to the top-level device class.

    It implements the :class:`pyrmsvd.static.interface.iregmapio.IRegMapIO` interface.
    """

    def __init__(self, Reader, Writer, simulated=False, range_overrides=None, Dumper=None):
        """
        Register the reader and writer functions that the top-level
        device class will use for the peripheral register and register
        field 'io' properties.

        :type Reader: T(<address>)
        :param Reader: The read address function pointer
        :type  Writer: T(<address, value>)
        :param Writer: The write address function pointer
        :type simulated: ``bool``
        :param simulated: Indicates the interface is offline stand-alone \
                          register map
        :type range_overrides: list of RegisterRangeOverrides
        :param range_overrides: The optional list of override ranges.
        :type Dumper: T(<address>)
        :param Dumper: The read address function pointer for dumping.  To log failed reads.
        """
        # register the reader and writer functions
        self._ReadAddress = Reader
        self._WriteAddress = Writer
        self.simulated = simulated
        self.accessed_flag_active = False
        self.py_logger = None
        self.py_rm_obj_prefix = ''
        self.py_logger_label = None
        self.py_logger_footer = ''
        self.py_logger_active = False
        self.c_logger = None
        self.c_logger_label = None
        self.c_logger_footer = ''
        self.c_logger_active = False
        self._address_overrides = {}
        if range_overrides:
            for override in range_overrides:
                self.addRegisterRangeOverride(override)
        if Dumper is None:
            self._DumpAddress = Reader
        else:
            self._DumpAddress = Dumper

    def _read(self, address, reg=None):
        # print("Reading address {0:#010x}".format(address))
        try:
            if self._address_overrides:
                for override in self._address_overrides.values():
                    if override.start_addr <= address <= override.end_addr:
                        return override.read(address)
                return self._ReadAddress(address)
            else:
                return self._ReadAddress(address)
        except BaseException as exc:
            if reg is not None and 'ReadMem32: Failure' in exc.args[0]:
                raise RegMapAccessError("Read Failure: Register '{}' is not accessible. Check power/clock config and "
                                        "access restrictions to the {} peripheral.".format(reg.name, reg.pername))
            else:
                raise exc

    def _write(self, address, value, reg=None):
        # print("Writing '{:#010x}' to address {:#010x}".format(value, address))
        try:
            if self._address_overrides:
                for override in self._address_overrides.values():
                    if override.start_addr <= address <= override.end_addr:
                        return override.write(address, value)
                return self._WriteAddress(address, value)
            else:
                return self._WriteAddress(address, value)
        except BaseException as exc:
            if reg is not None and 'WriteMem32: Failure' in exc.args[0]:
                raise RegMapAccessError("Write Failure: Register '{}' is not accessible. Check power/clock config and "
                                        "access restrictions to the {} peripheral.".format(reg.name, reg.pername))
            else:
                raise exc

    def assignRegisterDefault(self, reg):
        if not self.simulated:
            raise RegMapAccessError("Unable to assign default in live connection")
        return self._write(reg.baseAddress + reg.addressOffset, reg.resetValue, reg)

    def forceRegister(self, reg, value):
        if not self.simulated:
            raise RegMapAccessError("Unable to force value in live connection")
        return self._write(reg.baseAddress + reg.addressOffset, value, reg)

    def dumpRegister(self, reg):
        return self._DumpAddress(reg.baseAddress + reg.addressOffset)

    def readRegister(self, reg):
        # Per LABATEPDB-642, allow reads of all access types
        return self._read(reg.baseAddress + reg.addressOffset, reg)

    def readRegisterField(self, reg, field):
        # Per LABATEPDB-642, allow reads of all access types
        return GetFieldValue(self._read(reg.baseAddress + reg.addressOffset, reg),
                             field.bitOffset, field.bitWidth)

    def writeRegister(self, reg, regValue):
        if reg.access in [None, 'write-only', 'read-write', 'writeOnce', 'read-writeOnce']:
            return self._write(reg.baseAddress + reg.addressOffset, regValue, reg)
        else:
            raise RegMapAccessError("Write Failure: Register '{}' access is '{}'".format(reg.name, reg.access))

    def setRegister(self, reg, maskValue):
        if reg.access in [None, 'write-only', 'read-write', 'writeOnce', 'read-writeOnce']:
            if reg.setOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.setOffset + reg.addressOffset, maskValue, reg)
            else:
                if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                    regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                else:
                    regValue = 0
                return self._write(reg.baseAddress + reg.addressOffset, regValue | maskValue, reg)
        else:
            raise RegMapAccessError("setRegister Failure: Register '{}' access is '{}'".format(reg.name, reg.access))

    def clrRegister(self, reg, maskValue):
        if reg.access in [None, 'write-only', 'read-write', 'writeOnce', 'read-writeOnce']:
            if reg.clrOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.clrOffset + reg.addressOffset, maskValue, reg)
            else:
                if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                    regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                else:
                    regValue = 0
                return self._write(reg.baseAddress + reg.addressOffset, (~maskValue) & regValue, reg)
        else:
            raise RegMapAccessError("clrRegister Failure: Register '{}' access is '{}'".format(reg.name, reg.access))

    def tglRegister(self, reg, maskValue):
        if reg.access in [None, 'write-only', 'read-write', 'writeOnce', 'read-writeOnce']:
            if reg.tglOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.tglOffset + reg.addressOffset, maskValue, reg)
            else:
                if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                    regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                else:
                    regValue = 0
                return self._write(reg.baseAddress + reg.addressOffset, maskValue ^ regValue, reg)
        else:
            raise RegMapAccessError("tglRegister Failure: Register '{}' access is '{}'".format(reg.name, reg.access))

    def writeRegisterField(self, reg, field, fieldValue):
        if field.access in [None, 'read-write', 'read-writeOnce']:
            regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
            return self._write(reg.baseAddress + reg.addressOffset,
                               SetFieldValue(regValue, field.bitOffset, field.bitWidth, fieldValue), reg)
        elif field.access in ['write-only', 'writeOnce']:
            if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                # PSC40NMP-31, a write-only field may live in read-write register
                # Any other write-only fields will be lost, no logger available here
                regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
            else:
                # normal case, parent register is write-only so use zero
                regValue = 0
            return self._write(reg.baseAddress + reg.addressOffset,
                               SetFieldValue(regValue, field.bitOffset, field.bitWidth, fieldValue), reg)
        else:
            raise RegMapAccessError("Write Failure: Register '{}:{}' access is '{}'".format(reg.name, field.name,
                                                                                            field.access))

    def setRegisterField(self, reg, field, maskValue):
        if field.access in [None, 'read-write', 'read-writeOnce']:
            if reg.setOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.setOffset + reg.addressOffset,
                                   SetFieldValue(0, field.bitOffset, field.bitWidth, maskValue), reg)
            else:
                regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                return self._write(reg.baseAddress + reg.addressOffset,
                                   SetFieldValue(regValue, field.bitOffset, field.bitWidth, maskValue | fieldValue), reg)
        elif field.access in ['write-only', 'writeOnce']:
            if reg.setOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.setOffset + reg.addressOffset,
                                   SetFieldValue(0, field.bitOffset, field.bitWidth, maskValue), reg)
            else:
                if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                    # PSC40NMP-31, a write-only field may live in read-write register
                    # Any other write-only fields will be lost, no logger available here
                    regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                else:
                    # normal case, parent register is write-only so use zero
                    regValue = 0
                fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                return self._write(reg.baseAddress + reg.addressOffset,
                                   SetFieldValue(regValue, field.bitOffset, field.bitWidth, maskValue | fieldValue), reg)
        else:
            raise RegMapAccessError("Write Failure: Register '{}:{}' access is '{}'".format(reg.name, field.name,
                                                                                            field.access))

    def clrRegisterField(self, reg, field, maskValue):
        if field.access in [None, 'read-write', 'read-writeOnce']:
            if reg.clrOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.clrOffset + reg.addressOffset,
                                   SetFieldValue(0, field.bitOffset, field.bitWidth, maskValue), reg)
            else:
                regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                fieldValue = ((~maskValue)&(0xFFFFFFFF>>(32-field.bitWidth))) & fieldValue
                return self._write(reg.baseAddress + reg.addressOffset,
                                   SetFieldValue(regValue, field.bitOffset, field.bitWidth, fieldValue), reg)
        elif field.access in ['write-only', 'writeOnce']:
            if reg.clrOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.clrOffset + reg.addressOffset,
                                   SetFieldValue(0, field.bitOffset, field.bitWidth, maskValue), reg)
            else:
                if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                    # PSC40NMP-31, a write-only field may live in read-write register
                    # Any other write-only fields will be lost, no logger available here
                    regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                else:
                    # normal case, parent register is write-only so use zero
                    regValue = 0
                fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                fieldValue = ((~maskValue) & (0xFFFFFFFF >> (32 - field.bitWidth))) & fieldValue
                return self._write(reg.baseAddress + reg.addressOffset,
                                   SetFieldValue(regValue, field.bitOffset, field.bitWidth, fieldValue), reg)
        else:
            raise RegMapAccessError("Write Failure: Register '{}:{}' access is '{}'".format(reg.name, field.name,
                                                                                            field.access))

    def tglRegisterField(self, reg, field, maskValue):
        if field.access in [None, 'read-write', 'read-writeOnce']:
            if reg.tglOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.tglOffset + reg.addressOffset,
                                   SetFieldValue(0, field.bitOffset, field.bitWidth, maskValue), reg)
            else:
                regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                return self._write(reg.baseAddress + reg.addressOffset,
                                   SetFieldValue(regValue, field.bitOffset, field.bitWidth,
                                                 ((fieldValue ^ maskValue)&(0xFFFFFFFF>>(32-field.bitWidth)))), reg)
        elif field.access in ['write-only', 'writeOnce']:
            if reg.tglOffset is not None and not self.simulated:
                return self._write(reg.baseAddress + reg.tglOffset + reg.addressOffset,
                                   SetFieldValue(0, field.bitOffset, field.bitWidth, maskValue), reg)
            else:
                if reg.access in [None, 'read-only', 'read-write', 'read-writeOnce']:
                    # PSC40NMP-31, a write-only field may live in read-write register
                    # Any other write-only fields will be lost, no logger available here
                    regValue = self._read(reg.baseAddress + reg.addressOffset, reg)
                    fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                else:
                    # normal case, parent register is write-only so use zero
                    regValue = 0
                    fieldValue = GetFieldValue(regValue, field.bitOffset, field.bitWidth)
                return self._write(reg.baseAddress + reg.addressOffset,
                                   SetFieldValue(regValue, field.bitOffset, field.bitWidth,
                                                 ((fieldValue ^ maskValue)&(0xFFFFFFFF>>(32-field.bitWidth)))), reg)
        else:
            raise RegMapAccessError("Write Failure: Register '{}:{}' access is '{}'".format(reg.name, field.name,
                                                                                            field.access))

    def enableAccessedFlags(self):
        self.accessed_flag_active = True

    def disableAccessedFlags(self):
        self.accessed_flag_active = False

    def openCLangLogger(self, logger_fn, logger_header='',
                        logger_footer='', logger_label='rm_c_source'):
        if self.c_logger is not None:
            raise RegMapLoggingError("C source logging is already initialized. "
                                     "Call closeCLangLogger() to start new session.")
        else:
            if not isinstance(logger_fn, basestring):
                raise RegMapLoggingError("Invalid logger_fn of '{}'".format(logger_fn))
            if not isinstance(logger_header, basestring):
                raise RegMapLoggingError("Invalid logger_header of '{}'".format(logger_header))
            if not isinstance(logger_footer, basestring):
                raise RegMapLoggingError("Invalid logger_footer of '{}'".format(logger_footer))
            if not isinstance(logger_label, basestring):
                raise RegMapLoggingError("Invalid logger_label of '{}'".format(logger_label))
            self.c_logger_footer = logger_footer
            self.c_logger_label = logger_label
            self.c_logger = logging.getLogger(self.c_logger_label)
            self.c_logger.setLevel(logging.DEBUG)
            dirname = os.path.dirname(logger_fn)
            if not os.path.exists(dirname):
                os.makedirs(dirname)
            handler = logging.FileHandler(logger_fn, mode='w')
            formatter = logging.Formatter("%(message)s")
            handler.setFormatter(formatter)
            self.c_logger.addHandler(handler)
            if logger_header:
                self.c_logger.debug(logger_header)

    def enableCLangLogger(self):
        if self.c_logger is None:
            raise RegMapLoggingError("C logger is not initialized. Call openCLangLogger()")
        self.c_logger_active = True

    def disableCLangLogger(self):
        if self.c_logger is None:
            raise RegMapLoggingError("C logger is not initialized. Call openCLangLogger()")
        self.c_logger_active = False

    def closeCLangLogger(self):
        if self.c_logger is None:
            raise RegMapLoggingError("C logger is not initialized. Nothing to close.")
        else:
            if self.c_logger_footer:
                self.c_logger.debug(self.c_logger_footer)
            c_handlers = self.c_logger.handlers
            for handler in c_handlers:
                if isinstance(handler, logging.FileHandler):
                    handler.flush()
                    handler.close()
                self.c_logger.removeHandler(handler)
            self.c_logger_label = None
            self.c_logger = None
            self.c_logger_active = False
            self.c_logger_footer = ''

    def openPyLangLogger(self, logger_fn, rm_obj_prefix='dut.rm.',
                         logger_header='', logger_footer='',
                         logger_label='rm_py_source'):
        if self.py_logger is not None:
            raise RegMapLoggingError("Python source logging is already initialized. "
                                     "Call closePyLangLogger() to start new session.")
        else:
            if not isinstance(logger_fn, basestring):
                raise RegMapLoggingError("Invalid logger_fn of '{}'".format(logger_fn))
            if not isinstance(rm_obj_prefix, basestring):
                raise RegMapLoggingError("Invalid rm_obj_prefix of '{}'".format(rm_obj_prefix))
            if not isinstance(logger_header, basestring):
                raise RegMapLoggingError("Invalid logger_header of '{}'".format(logger_header))
            if not isinstance(logger_footer, basestring):
                raise RegMapLoggingError("Invalid logger_footer of '{}'".format(logger_footer))
            if not isinstance(logger_label, basestring):
                raise RegMapLoggingError("Invalid logger_label of '{}'".format(logger_label))
            self.py_rm_obj_prefix = rm_obj_prefix
            if self.py_rm_obj_prefix[-1] != '.':
                self.py_rm_obj_prefix += '.'
            self.py_logger_footer = logger_footer
            self.py_logger_label = logger_label
            self.py_logger = logging.getLogger(self.py_logger_label)
            self.py_logger.setLevel(logging.DEBUG)
            dirname = os.path.dirname(logger_fn)
            if not os.path.exists(dirname):
                os.makedirs(dirname)
            handler = logging.FileHandler(logger_fn, mode='w')
            formatter = logging.Formatter("%(message)s")
            handler.setFormatter(formatter)
            self.py_logger.addHandler(handler)
            if logger_header:
                self.py_logger.debug(logger_header)

    def enablePyLangLogger(self):
        if self.py_logger is None:
            raise RegMapLoggingError("Python logger is not initialized. Call openPyLangLogger()")
        self.py_logger_active = True

    def disablePyLangLogger(self):
        if self.py_logger is None:
            raise RegMapLoggingError("Python logger is not initialized. Call openPyLangLogger()")
        self.py_logger_active = False

    def closePyLangLogger(self):
        if self.py_logger is None:
            raise RegMapLoggingError("Python logger is not initialized. Nothing to close.")
        else:
            if self.py_logger_footer:
                self.py_logger.debug(self.py_logger_footer)
            py_handlers = self.py_logger.handlers
            for handler in py_handlers:
                if isinstance(handler, logging.FileHandler):
                    handler.flush()
                    handler.close()
                self.py_logger.removeHandler(handler)
            self.py_logger_label = None
            self.py_logger = None
            self.py_rm_obj_prefix = ''
            self.py_logger_active = False
            self.py_logger_footer = ''

    def addRegisterRangeOverride(self, override):
        """Store override instance for use in read and write operations.

        The override is keyed by the name attribute into an internal
        dictionary. Use getRegisterRangeOverrides() to review current
        values. To update an override, use removeRegisterRangeOverride()
        to remove and then add update.

        Args:
            override (RegisterRangeOverride): The override instance.

        Raises:
            ValueError: On attempts to register an override with existing name.

        """
        if not isinstance(override, RegisterRangeOverride):
            raise TypeError("override must be type RegisterRangeOverride")

        if override.name in self._address_overrides:
            raise ValueError("Override '{}' is already assigned, remove "
                             "existing value first".format(override.name))
        self._address_overrides[override.name] = override

    def removeRegisterRangeOverride(self, name):
        """Remove the address override name key.

        Use getRegisterRangeOverrides() to review current values.

        """
        if name in self._address_overrides:
            del self._address_overrides[name]

    def getRegisterRangeOverrides(self):
        """Return the address override dictionary, which is keyed by name.

        Use this method to query current overrides.

        """
        return self._address_overrides

    def assignRawWordBlock(self, address, wordList):
        """
        Write a number of 32-words starting at given address.

        This is a raw operation which will not check the parent reg object.
        It exists to initialize the simulated offline register map from
        an efficient bulk library read operation.

        Args:
            address (int or long): The starting memory mapped register address
            wordList (list[int or long]): The 32-bit register values array

        Raises:
            RegMapAccessError: Only allowed for offline simulated mode.

        """
        if not self.simulated:
            raise RegMapAccessError("Raw word assignment is limited to offline"
                                    "simulated register map.")
        addr = address
        for data_word in wordList:
            self._write(addr, data_word)
            addr += 4
