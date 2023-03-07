
"""
Register Map I/O
^^^^^^^^^^^^^^^^
The top-level device class can receive a
:class:`pyrmsvd.static.common.regmapio.RegisterMapInterface` instance,
which implements this interface. This ensures a consistent address read/write
API for the low-level access to the part, whether it is a live J-Link connection
or a simulated offline dictionary.

  .. autoclass:: IRegMapIO
     :members:
     :show-inheritance:
     :inherited-members:

"""


__all__ = ['IRegMapIO']

from abc import ABCMeta, abstractmethod


class IRegMapIO(object):
    """
    The interface for low-level part access.
    """
    __metaclass__ = ABCMeta

    @abstractmethod
    def _read(self, address):
        """
        Low-level raw read function. No access checking performed.

        :type  address: ``int`` or ``long``
        :param address: The read address
        :rtype: ``long``
        :return: The 32-bit value at the address
        """
        pass

    @abstractmethod
    def _write(self, address, value):
        """
        Low-level raw write function. No access checking performed.

        :type  address: ``int`` or ``long``
        :param address: The write address
        :type  value: ``int`` or ``long``
        :param value: The value to write
        """
        pass

    @abstractmethod
    def assignRegisterDefault(self, reg):
        """
        Assign reset value of the register. Raises error if not writeable.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :raises: RegMapAccessError
        """
        pass

    @abstractmethod
    def forceRegister(self, reg, value):
        """
        Force the value of the register, regardless of the access type.
        Only allowed for simulated connections. Raises error for live
        connection.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :type  value: ``int`` or ``long``
        :param value: The register value
        :raises: RegMapAccessError
        """
        pass

    @abstractmethod
    def dumpRegister(self, reg):
        """
        Read the register. Return type will be list if Dumper function is provided,
        else the logic falls back to the Reader function with int return.

        For the Dumper function, the return type is a list of error message string and value.
        An empty error message indicates successful read.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :rtype: Union[List[str, int], int]
        :return: The value of the register
        """
        pass

    @abstractmethod
    def readRegister(self, reg):
        """
        Read the register. Raises error if not readable.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :rtype: ``long``
        :return: The value of the register
        :raises: RegMapAccessError
        """
        pass

    @abstractmethod
    def readRegisterField(self, reg, field):
        """
        Read the register's field. Raises error if not readable.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :type  field: :class:`~Base_RM_Field`
        :param field: The register's field object
        :rtype: ``long``
        :return: The value of the register's field
        :raises: RegMapAccessError
        """
        pass

    @abstractmethod
    def writeRegister(self, reg, regValue):
        """
        Write value to the register. Raises error if not writeable.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :type  regValue: ``int`` or ``long``
        :param regValue: The register value
        :raises: RegMapAccessError
        """
        pass

    @abstractmethod
    def writeRegisterField(self, reg, field, fieldValue):
        """
        Write value the register's field. Raises error if not writeable.

        :type  reg: :class:`~Base_RM_Register`
        :param reg: The register object
        :type  field: :class:`~Base_RM_Field`
        :param field: The register's field object
        :type  fieldValue: ``int`` or ``long``
        :param fieldValue: The register's field value
        :raises: RegMapAccessError
        """
        pass

    @abstractmethod
    def enableAccessedFlags(self):
        """
        Enable write tracking logic on registers and fields. Default is disabled.
        """
        pass

    @abstractmethod
    def disableAccessedFlags(self):
        """
        Disable write tracking logic on registers and fields. Default is disabled.
        """
        pass

    @abstractmethod
    def openCLangLogger(self, logger_fn, logger_header='',
                        logger_footer='', logger_label='rm_c_source'):
        """
        Initialize register map read and write logging to a C source code
        file. This allows user to record a C equivalent of a dut.rm session
        and compile it in a simulation environment. Currently supports one
        logger, but it is possible to support multiple with different labels
        and even apply different filter lists in the future.

        .. note:: User must call enableCLangLogger() after this function.

        :type  logger_fn: ``str``
        :param logger_fn: The logger path and filename.
        :type  logger_header: ``str``
        :param logger_header: Header text to insert before logging data.
        :type  logger_footer: ``str``
        :param logger_footer: Footer text to append after logging data.
        :type logger_label: ``str``
        :param logger_label: Optional label to name this logger.
        :raises: RegMapLoggingError
        """
        pass

    @abstractmethod
    def enableCLangLogger(self):
        """
        Enable register map read and write logging to C source file.
        Use this method to resume logging after it was suspended due
        to disableCLangLogger(). Raises exception if C logger has not
        been opened.

        :raises: RegMapLoggingError
        """

    @abstractmethod
    def disableCLangLogger(self):
        """
        Disable register map read and write logging to C source file.
        Use this method to suspend logging. Use enableCLangLogger() to
        resume. Raises exception if C logger has not been opened.

        :raises: RegMapLoggingError
        """
        pass

    @abstractmethod
    def closeCLangLogger(self):
        """
        End register map read and write logging to a C source file. The
        current handler is flushed and closed. The user may call
        openCLangLogger() again to reopen, for example to use a different
        filename.
        """
        pass

    @abstractmethod
    def openPyLangLogger(self, logger_fn, rm_obj_prefix='dut.rm.',
                         logger_header='', logger_footer='',
                         logger_label='rm_py_source'):
        """
        Initialize register map read and write logging to a Python source code
        file. This allows user to record a Python dut.rm session for playback.
        Currently supports one logger, but it is possible to support multiple
        with different labels and even apply different filter lists in the
        future.

        .. note:: User must call enablePyLangLogger() after this function.

        :type  logger_fn: ``str``
        :param logger_fn: The logger path and filename.
        :type  rm_obj_prefix: ``str``
        :param rm_obj_prefix: The string for the parent object of the rm.
        :type  logger_header: ``str``
        :param logger_header: Header text to insert before logging data.
        :type  logger_footer: ``str``
        :param logger_footer: Footer text to append after logging data.
        :type logger_label: ``str``
        :param logger_label: Optional label to name this logger.
        :raises: RegMapLoggingError
        """
        pass

    @abstractmethod
    def enablePyLangLogger(self):
        """
        Enable register map read and write logging to Python source file.
        Use this method to resume logging after it was suspended due
        to disablePyLangLogger(). Raises exception if Python logger has not
        been opened.

        :raises: RegMapLoggingError
        """

    @abstractmethod
    def disablePyLangLogger(self):
        """
        Disable register map read and write logging to Python source file.
        Use this method to suspend logging. Raises exception if Python logger
        has not been opened.

        :raises: RegMapLoggingError
        """
        pass

    @abstractmethod
    def closePyLangLogger(self):
        """
        End register map read and write logging to a C source file. The
        current handler is flushed and closed. The user may call
        openPyLangLogger() again to reopen, for example to use a different
        filename.
        """
        pass

    @abstractmethod
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
        pass

    @abstractmethod
    def removeRegisterRangeOverride(self, name):
        """Remove the address override name key.

        Use getRegisterRangeOverrides() to review current values.

        """
        pass

    @abstractmethod
    def getRegisterRangeOverrides(self):
        """Return the address override dictionary, which is keyed by name.

        Use this method to query current overrides.

        """
        pass

    @abstractmethod
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
