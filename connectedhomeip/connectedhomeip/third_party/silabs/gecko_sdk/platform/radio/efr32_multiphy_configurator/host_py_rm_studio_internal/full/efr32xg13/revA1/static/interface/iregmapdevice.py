
"""
Device
------
The top-level device class subclasses :class:`~Base_RM_Device`, which
implements this interface.

  .. autoclass:: IRegMapDevice
     :members:
     :show-inheritance:
     :inherited-members:

"""


__all__ = ['IRegMapDevice']


from abc import ABCMeta, abstractmethod


class IRegMapDevice(object):
    """
    The interface for the dut.rm device component. This component will also
    contain a list of peripheral name attributes in uppercase.
    """
    __metaclass__ = ABCMeta

    # SVD Info Property

    @property
    @abstractmethod
    def svdInfo(self):
        """
        :rtype: :class:`pyrmsvd.static.common.svdinfo.RM_SVD_Info`
        :return: An instance of the RM_SVD_Info with filename, md5sum, and
                 aliased list for registers, fields, and enums.
        """
        pass

    # Name Access Methods

    @abstractmethod
    def addressToName(self, address):
        """
        Returns the register string for the given address.

        DEPRECATED by addressToNames()

        :type  address: ``int`` or ``long``
        :param address: The register address
        :rtype: ``str``
        :return: The register name in 'PERIPHERAL.REGISTER' format
        :raises: RegMapAddressError
        """
        pass

    @abstractmethod
    def addressToNames(self, address):
        """
        Returns the list of register strings for the given address.

        :type  address: ``int`` or ``long``
        :param address: The register address
        :rtype: ``list``
        :return: The register names list in 'PERIPHERAL.REGISTER' format
        :raises: RegMapAddressError
        """
        pass

    @abstractmethod
    def nameToAddress(self, name):
        """
        Get the address of the register or register field.

        :type  name: ``str``
        :param name: The 'PERIPHERAL.REGISTER' or 'PERIPHERAL.REGISTER.FIELD' name
        :rtype: ``long``
        :return: The address of the register
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def writeByName(self, name, value):
        """
        Write the value to register or register field.

        :type  name: ``str``
        :param name: The 'PERIPHERAL.REGISTER' or 'PERIPHERAL.REGISTER.FIELD' name
        :type  value: ``int`` or ``long``
        :param value: The value of the register or register field
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def readByName(self, name):
        """
        Return the value for register or register field.

        :type  name: ``str``
        :param name: The 'PERIPHERAL.REGISTER' or 'PERIPHERAL.REGISTER.FIELD' name
        :rtype: ``long``
        :return: The value of the register or register field
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def getObjectByName(self, name):
        """
        A helper function used to get peripheral, register or register field object.

        :type  name: ``str``
        :param name: The 'PERHIPHERAL', 'PERIPHERAL.REGISTER' or \
            'PERIPHERAL.REGISTER.FIELD' name
        :rtype: :class: Base_RM_Peripheral`, :class:`Base_RM_Register` or \
            :class:`Base_RM_Field` instance
        :return: The peripheral, register or register field object
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def getRegisterNameFromFieldName(self, name):
        """
        A helper function use to get the 'PERIPHERAL.REGISTER' name from
        a 'PERIPHERAL.REGISTER.FIELD' name. Will accept a 'PERIPHERAL.REGISTER'
        name without complaint.

        :type  name: ``str``
        :param name: The 'PERIPHERAL.REGISTER' or 'PERIPHERAL.REGISTER.FIELD' name
        :rtype: ``str``
        :return: The 'PERIPHERAL.REGISTER' name
        """
        pass

    @abstractmethod
    def getPeripheralNames(self):
        """
        Return a list of all peripheral names in this device.

        :rtype: ``list`` of ``str``
        :return: The 'PERIPHERAL' names in this device.
        """
        pass

    @abstractmethod
    def getRegisterNames(self):
        """
        Return a list of all register names in this device.

        :rtype: ``list`` of ``str``
        :return: The 'PERIPHERAL.REGISTER' names in this device.
        """
        pass

    @abstractmethod
    def isReadable(self, name):
        """
        A helper function use to determine if 'PERIPHERAL.REGISTER' or
        'PERIPHERAL.REGISTER.FIELD' name is readable.

        :type  name: ``str``
        :param name: The 'PERIPHERAL.REGISTER' or 'PERIPHERAL.REGISTER.FIELD' name
        :rtype: ``bool``
        :return: ``True`` if readable, else ``False``
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def isWriteable(self, name):
        """
        A helper function use to determine if 'PERIPHERAL.REGISTER' or
        'PERIPHERAL.REGISTER.FIELD' name is writeable.

        :type  name: ``str``
        :param name: The 'PERIPHERAL.REGISTER' or 'PERIPHERAL.REGISTER.FIELD' name
        :rtype: ``bool``
        :return: ``True`` if writeable, else ``False``
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def forceRegister(self, name, value):
        """
        Used to ignore the register access when running in simulation mode,
        where the connection is offline. Allows the user to assign a default
        value to a read-only register.

        :type  name: ``str``
        :param name:  The 'PERIPHERAL.REGISTER' name
        :type  value: ``int`` or ``long``
        :param value: The value to assign to the register
        :raises: RegMapAccessError, RegMapNameError
        """
        pass

    # Accessed Flag Methods

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
    def clearAccessedFlags(self):
        """
        Clear the accessed flag for all registers in all peripherals, recursively.
        Note that the accessed flag is set whenever the register is written.
        """
        pass

    @abstractmethod
    def setAccessedFlags(self):
        """
        Set the accessed flag for all registers in all peripherals, recursively.
        Note that the accessed flag is set whenever the register is written.
        """
        pass

    @abstractmethod
    def getAccessedRegisterNames(self):
        """
        Return a list of 'PERIPHERAL.REGISTER' names with accessed flag set.

        :rtype: ``list`` of ``str``
        :return: A list of register names
        """
        pass

    @abstractmethod
    def getAccessedFieldNames(self):
        """
        Return a list of 'PERIPHERAL.REGISTER.FIELD' names with accessed flag set.

        :rtype: ``list`` of ``str``
        :return: A list of register field names
        """
        pass

    # Data Dictionary Methods

    @staticmethod
    @abstractmethod
    def convertListToOrderedDict(inList, defaultValue=0):
        """
        Create an empty ordered dict from a list for use in the readData(),
        writeData(), or verifyData() methods.

        :type  inList: ``list`` of ``str``
        :param inList: A list of 'PERIPHERAL.REGISTER' or \
                      'PERIPHERAL.REGISTER.FIELD' names.
        :type  defaultValue: ``int`` or ``long``
        :param defaultValue: The default value to assign to each key.
        :rtype: :class:`collections.OrderedDict`
        :return: An ordered dict version of the list.
        """

    @abstractmethod
    def writeData(self, dataDict):
        """
        Write register or register field item value from dictionary to part.

        .. note:: To ensure consistent write order, use an OrderedDict()

        :type  dataDict: ``dict`` of ``str`` : ``int`` or ``long`` item
        :param dataDict: The items of 'PERIPHERAL.REGISTER' or \
                         'PERIPHERAL.REGISTER.FIELD' name key and value
        :raises: RegMapNameError, RegMapValueError
        """
        pass

    @abstractmethod
    def readData(self, dataDict):
        """
        Read register or register field items from part and assign
        values in dictionary.

        .. note:: To ensure consistent read order, use an OrderedDict()

        :type  dataDict: ``dict`` of ``str`` : ``long`` item
        :param dataDict: The items of 'PERIPHERAL.REGISTER' or \
                         'PERIPHERAL.REGISTER.FIELD' name key and value
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def verifyData(self, expectedDict):
        """
        Read register or register field items from part and compare against
        values in dictionary. Create an item in a difference dictionary for a
        mismatch.

        :type  expectedDict: ``dict`` of ``str`` = ``long`` item
        :param expectedDict: The items of 'PERIPHERAL.REGISTER' or \
                            'PERIPHERAL.REGISTER.FIELD' name key and value
        :rtype: :class:`collections.OrderedDict` of ``str`` : ``long``
        :return: An OrderedDict with register or register field keys and \
                actual values read. An empty dictionary is returned if no \
                differences are detected.
        :raises: RegMapNameError
        """
        pass

    @abstractmethod
    def readAccessedRegisters(self):
        """
        Read values of all registers with accessed flag set and store in a
        return dictionary. Lists are sorted alphabetically by peripheral
        name followed by register name.

        :rtype: :class:`collections.OrderedDict` of ``str`` : ``long``
        :return: An ordered dictionary with 'PERIPHERAL.REGISTER' name key \
                 and actual values read. An empty dictionary is returned \
                 if no accessed flags are set.
        """
        pass

    @abstractmethod
    def readAccessedFields(self):
        """
        Read values of all register fields with accessed flag set and store
        in a return dictionary. Lists are sorted alphabetically by peripheral
        name followed by register field name.

        .. note:: The granularity is at the field level, so even if all fields
            in a register have been read, the register name will not be
            returned.

        :rtype: :class:`collections.OrderedDict` of ``str`` : ``long``
        :return: An ordered dictionary with 'PERIPHERAL.REGISTER.FIELD' name \
                 key and actual values read. An empty dictionary is returned \
                 if no accessed flags are set.
        """
        pass

    # Dump Methods

    @abstractmethod
    def getExcludedDumpRegisterNames(self):
        """
        Get a list of register names which are currently excluded from an
        unfiltered dump operation, which occurs when the user does not
        supply a regFilterList parameter to the dump() method.

        .. note:: An excluded register can still be dumped if it is
            explicitly listed in the regFilterList. Only a readable
            register can be dumped.

        :rtype: ``list`` of ``str``
        :return: A list of 'PERIPHERAL.REGISTER' names, which are
            not dumped unless explicitly listed in a filter list.
        """
        pass

    def excludeFromDumpByName(self, nameList):
        """
        Call the excludeFromDump() method for each name item in the
        provided list. The name can be a peripheral, a register, or
        a register field. For a peripheral, all of its regiters are
        excluded. For a register field, the parent register is
        excluded.

        .. note:: An excluded register can still be dumped if it is
            explicitly listed in the regFilterList in the top-level
            device dump() method.

        :type  nameList: ``list`` of ``str``
        :param nameList: A list of 'PERIPHERAL', 'PERIPHERAL.REGISTER',
            or 'PERIPHERAL.REGISTER.FIELD' names, which are
            not dumped unless explicitly listed in a filter list.
        """
        pass

    @abstractmethod
    def includeInDump(self):
        """
        Calls the includeInDump() method for all registers in this device,
        so they can be dumped by default when a filter list is not supplied
        to the dump() method. Use this method to globally reverse a call to
        setExcludedDumpResisterNames().
        """
        pass

    @abstractmethod
    def appendDumpDescCallback(self, cb_function):
        """
        Allow the user to register a description callback function,
        which is called during the dump() method. This allows the
        user to collect relevant description strings to log in the
        Python dump file for context. Multiple calls will append to
        a callback list. String data is prepended with a '#' comment
        character if required.

        Args:
            cb_function: A function returning string desc data to log.

        Raises:
            ValueError: The cb_function is not callable

        """
        pass

    @abstractmethod
    def clearDumpDescCallbacks(self):
        """
        Clear any registered description callback functions.
        """
        pass

    @abstractmethod
    def buildRegFilterList(self, filename, listname='regFilterList'):
        """
        Create a python source filename containing a default list of all
        readable register names in 'PERIPHERAL.REGISTER' format. This list
        will exclude those registers that are excluded from dumping with a
        dumpActive flag set to False. See getExcludedDumpRegisters() for
        this list. The resulting list can be imported from the python source
        filename for use in the top-level device dump() method. For a complete
        list of all registers, see the getRegisterNames() method.

        .. code-block:: py

            # example filter listing

            regFilterList = [
             'MODEM.AFC',
             'MODEM.AFCADJLIM',
             'MODEM.AFCADJRX',
             'MODEM.AFCADJTX',
             'MODEM.CF',
             'MODEM.CMD',
             'MODEM.CTRL0',
             'MODEM.CTRL1',
             'MODEM.CTRL2',
             'MODEM.CTRL3',
             'MODEM.CTRL4',
             'MODEM.CTRL5',
             'MODEM.DCCOMP',
             'MODEM.DCCOMPFILTINIT',
             'MODEM.DCESTI',
             'MODEM.DSSS0',
             'MODEM.FREQOFFEST',
             'MODEM.IEN',
             'MODEM.IF',
             'MODEM.MIXCTRL',
             'MODEM.MODINDEX',
             'MODEM.PRE',
            ]

        :type  filename: ``str``
        :param filename: Path and filename for a filter list (i.e. 'regfilter.py')
        :type  listname: ``str``
        :param listname: The name to use in the python source file
        :rtype: ``list`` of ``str``
        :return: The list of register names that were written to the python source file
        """

    @abstractmethod
    def dump(self, filename, regFilterList=None, userMsg=''):
        """
        Read values of all registers and store in a return dictionary. This
        dictionary is dumped to filename, with 'PERIPHERAL.REGISTER' name or
        'PERIPHERAL.REGISTER.FIELD' name keys and values read from the part.

        For each register or field value, the formatted field value is listed
        in source comments with any corresponding enum name and its description
        text. This dictionary is returned for immediate use. The dumped Python
        source file can be used in a diff operation to compare settings.

        Any previous file is overwritten. The registered description callback
        functions are called and string data inserted as comments at the top
        of the log file. A user message can also be supplied and will be
        formatted with leading '#' comment characters as required for multiline
        content.

        .. warning:: If ``regFilterList`` is ``None``, then **ALL** readable
            registers with dumpActive flag set to True are dumped. Reading
            some auto-increment registers can cause serious side effects
            to a running program. The dump routine will sort peripherals
            alphabetically and then the registers based on ascending address.
            See the getExcludedDumpRegisterNames() method for a list of
            registers that are not dumped by default.

        .. note:: If ``regFilterList`` is supplied, the dump routine will
            use the order of the list to read and preserve the order in
            the dump file. A register or register field explicitly listed
            here will be dumped regardless of whether it is excluded (where
            dumpActive flag is False).

        See the following confluence HowTo for details:
          `HowTo: Dump a dictionary of register values for diffing \
              <https://confluence.silabs.com/display/LabATE/HOWTO%3A+Dump+a+dictionary+of+register+values+for+diffing>`_

        :type  filename: ``str``
        :param filename: A python source file to dump the value dictionary
        :type  regFilterList: ``list`` of ``str``
        :param regFilterList: A list of 'PERIPHERAL.REGISTER' or \
                              'PERIPHERAL.REGISTER.FIELD' names to dump
        :type  userMsg: ``str``
        :param userMsg: An optional header message to insert into the dump
        :rtype: :class:`collections.OrderedDict` of ``str`` : ``long``
        :return: An OrderedDict dictionary with 'PERIPHERAL.REGISTER' or \
                 'PERIPHERAL.REGISTER.FIELD' name key and actual values read.

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
        Use this method to suspend logging. Use enablePyLangLogger() to
        resume. Raises exception if Python logger has not been opened.

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
        pass
