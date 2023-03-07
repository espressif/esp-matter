"""
Peripheral
----------
All rm.<PERIPHERAL> components subclass :class:`~Base_RM_Peripheral`, which
implements this interface.

  .. autoclass:: IRegMapPeripheral
     :members:
     :show-inheritance:
     :inherited-members:

"""


__all__ = ['IRegMapPeripheral']

from abc import ABCMeta, abstractmethod

class IRegMapPeripheral(object):
    """
    The interface for the dut.rm.<PERIPHERAL> component. This component
    will also contain a list of register name attributes in uppercase.
    """
    __metaclass__ = ABCMeta

    # Query Methods

    @abstractmethod
    def getRegisterNames(self):
        """
        Return a list of all 'PERIPHERAL.REGISTER' names in this peripheral.

        :rtype: ``list`` of ``str``
        :return: A list of register names, sorted by alphabetical order.
        """
        pass

    # Accessed Flag Methods

    @abstractmethod
    def clearAccessedFlags(self):
        """
        Clear the accessed flag for all registers in this peripheral.
        Note that the accessed flag is set whenever the register is written.
        """
        pass

    @abstractmethod
    def setAccessedFlags(self):
        """
        Set the accessed flag for all registers in this peripheral.
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

    @abstractmethod
    def assignRegDefault(self):
        """
        Assign all registers in this peripheral to default values from the
        CMSIS SVD XML.
        """
        pass

    @abstractmethod
    def getAddressNameMap(self, addrNameDict):
        """
        Update passed dictionary with address key and register name string value
        for all registers in this peripheral.

        .. note:: This is an internal function used by the device container.
                  Do not call directly.

        :type  addrNameDict: ``dict`` of ``long`` : ``str`` item
        :param addrNameDict: The items of address key and 'PERIPHERAL.REGISTER'
                             value
        """
        pass

    @abstractmethod
    def getAddressNamesMap(self, addrNameDict):
        """
        Update passed dictionary with address key and register name list value
        for all registers in this peripheral.

        .. note:: This is an internal function used by the device container.
                  Do not call directly.

        :type  addrNameDict: ``dict`` of ``long`` : ``list`` item
        :param addrNameDict: The items of address key and 'PERIPHERAL.REGISTER'
                             value
        """
        pass

    @abstractmethod
    def registerAddressToNamesCb(self, addressToNamesCb):
        """
        Register top-level method to query the list of aliased names.

        Args:
            addressToNamesCb: Callable method or None.

        """
    pass

    @abstractmethod
    def getAliasedNames(self):
        """
        Get list of other peripheral names mapped to the same base address.

        Returns:
            List[str]: The peripherals names or empty list.

        """

    @abstractmethod
    def isAliased(self):
        """
        Check for other peripherals at this peripheral's base address.

        Returns:
            Bool: True if there are other peripherals mapped to this base address.

        """

    @abstractmethod
    def getExcludedDumpRegisterNames(self):
        """
        Get a list of register names which are currently excluded from an
        unfiltered dump operation, which occurs when the user does not
        supply a regFilterList parameter to the top-level device dump() method.

        .. note:: An excluded register can still be dumped if it is
            explicitly listed in the regFilterList. Only a readable
            register can be dumped.

        :rtype: ``list`` of ``str``
        :return: A list of 'PERIPHERAL.REGISTER' names, which are
            not dumped unless explicitly listed in a filter list.
        """
        pass

    @abstractmethod
    def includeInDump(self):
        """
        Calls the includeInDump() method for all registers in this peripheral,
        so they can be dumped by default when a filter list is not supplied
        to the top-level device dump() method. Use this method to revert after
        a call to excludeFromDumpRegisterNames() or excludeFromDump().
        """
        pass

    @abstractmethod
    def excludeFromDump(self):
        """
        Calls the excludeFromDump() method for all registers in this peripheral,
        so they can be excluded by default when a filter list is not supplied
        to the top-level device dump() method. Use this method to completely
        skip a peripheral during dump.
        """
        pass

    @abstractmethod
    def buildRegFilterList(self, outFH, filterList):
        """
        Query peripheral for all readable registers which are not excluded
        from the dump and store 'PERIPHERAL.REGISTER' names in both the
        output file and append to filterList.

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
        Read values of all registers and store in valueDict. Default to
        stdout if no file handle is passed. The file handle is passed to
        internal functions to do the source dictionary output.

        This dump is called for two cases:
            - From the top-level dut.rm device API, for the use case with no
              filtered register list.
            - By the user in live console session to view formatted dump of
              all registers in console stdout.

        Uses the dumpActive flag to determine if a register should
        be read and dumped to file. For a filtered register list, the top-level
        dump() method will directly call the register's dump() method.

        This method will sort the peripheral registers by address when adding
        to the valueDict, which should be an ordered dict.

        :type  outFH: None or File Handle
        :param outFH: A file handle to dump the value dictionary with formatting.
                     If None, then use stdout.
        :type  valueDict: None or ``dict`` of ``str`` : ``long`` item
        :param valueDict: A dictionary to collect 'PERIPHERAL.REGISTER' name
                          with value.
        """
        pass
