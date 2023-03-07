
"""
Register
--------
All rm.<PERIPHERAL>.<REGISTER> components subclass :class:`~Base_RM_Register`,
which implements this interface.

  .. autoclass:: IRegMapRegister
     :members:
     :show-inheritance:
     :inherited-members:

"""


__all__ = ['IRegMapRegister']

from abc import ABCMeta, abstractmethod, abstractproperty


class IRegMapRegister(object):
    """
    The interface for the dut.rm.<PERIPHERAL>.<REGISTER> component. This
    component will also contain a list of field name attributes in uppercase.
    """
    __metaclass__ = ABCMeta

    def _getio(self):
        """
        This is the io property getter to interact with the part's register,
        which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.io
        property to read the register value.

        :rtype: ``long``
        :return: The register value.
        """
        pass

    def _setio(self, value):
        """
        This is the io property setter to interact with the part's register,
        which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.io
        property to assign a value.

        :type  value: ``int`` or ``long``
        :param value: The value to assign to the register.
        """
        pass

    io = abstractproperty(_getio, _setio)

    # Query Methods

    @abstractmethod
    def registerAddressToNamesCb(self, addressToNamesCb):
        """
        Internal function used by the device module initialization.

        Args:
            addressToNamesCb: The top-level callable to decode register address.

        """
        pass

    @abstractmethod
    def getAliasedNames(self):
        """
        Get names of other registers at this register's address.

        Returns:
            List[str]: Register full names mapped to the same address.

        """
        pass

    @abstractmethod
    def isAliased(self):
        """
        Check for other registers at this register's address.

        Returns:
            Bool: True if there are other registers mapped to this address.

        """
        pass

    @abstractmethod
    def getFieldNames(self):
        """
        Return a list of all 'PERIPHERAL.REGISTER.FIELD' names in this register.

        :rtype: ``list`` of ``str``
        :return: A list of register field names
        """
        pass

    # I/O Access Methods

    @abstractmethod
    def isReadable(self):
        """
        Check if register is readable.

        :rtype: ``bool``
        :return: ``True`` if readable, else ``False``
        """
        pass

    @abstractmethod
    def isWriteable(self):
        """
        Check if register is writeable.

        :rtype: ``bool``
        :return: ``True`` if writeable, else ``False``
        """
        pass

    # Accessed Flag Methods

    @abstractmethod
    def clearAccessedFlag(self):
        """
        Clear the accessed flag for the register and all the fields.
        Note that the accessed flag is set whenever the register is written.
        """
        pass

    @abstractmethod
    def setAccessedFlag(self):
        """
        Set the accessed flag for the register.
        Note that the accessed flag is set whenever the register is written.
        """
        pass

    @abstractmethod
    def getAccessedFlag(self):
        """
        Return the accessed flag for the register.

        :rtype: ``bool``
        :return: The state of the register's accessed flag
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

    @abstractmethod
    def getDesc(self):
        """
        Return a string description of the register with address, name, accessed flag
        state, description, and access type.

        :rtype: ``str``
        :return: The description string
        """
        pass

    @abstractmethod
    def assignRegDefault(self):
        """
        Assign the register to default values from the CMSIS SVD XML.
        """
        pass

    # Dump - Public

    @abstractmethod
    def includeInDump(self):
        """
        Dump this register by default when a filter list is not supplied
        to the top-level device dump() method. Use this method to revert after
        a call to excludeFromDumpRegisterNames() or excludeFromDump().
        """
        pass

    @abstractmethod
    def excludeFromDump(self):
        """
        Skip this register by default when a filter list is not supplied
        to the top-level device dump() method. Use includeInDump() to reverse.
        """
        pass

    @abstractmethod
    def buildRegFilterList(self, outFH, filterList):
        """
        If register is readable, then store 'PERIPHERAL.REGISTER'
        name in both the output file and append to filterList.

        .. note:: This is an internal function used by the device container.
                  Do not call directly.

        :type  outFH: File Handle
        :param outFH: A file handle to dump the register names with formatting
        :type  filterList: ``list`` of ``str``
        :param filterList: The list to append 'PERIPHERAL.REGISTER' names
        """
        pass

    @abstractmethod
    def dump(self, outFH=None, valueDict=None):
        """
        Read register and store in valueDict. Default to stdout if no file
        handle is passed. The file handle is passed to internal functions
        to do the source dictionary output.  Providing Dumper callable
        enables skip reporting for failed reads.

        This dump is called for two cases:
            - From the register level in the course of performing a dump
              of one or more registers in a peripheral.
            - By the user in live console session to view formatted dump of
              all registers in console stdout.

        :type  outFH: None or File Handle
        :param outFH: A file handle to dump the value dictionary with formatting
        :type  valueDict: None or ``dict`` of ``str`` : ``long`` item
        :param valueDict: A dictionary to collect 'PERIPHERAL.REGISTER' name \
                          with value
        """
        pass

    @abstractmethod
    def dump_field(self, outFH, valueDict, field_name):
        """
        Read register field and store in valueDict. The file handle is passed to
        internal functions to do the source dictionary output. Providing Dumper
        callable in __init__() enables skip reporting for failed reads.

        .. note:: This is an internal function used by the device container.
                  Do not call directly.

        :type  outFH: File Handle
        :param outFH: A file handle to dump the value dictionary with formatting
        :type  valueDict: ``dict`` of ``str`` : ``long`` item
        :param valueDict: A dictionary to collect 'PERIPHERAL.REGISTER.FIELD' name \
                          with value
        :type  field_name: ``str``
        :param field_name: The FIELD name to dump. Only this field and its value
                           are captured.
        """