
"""
Field
-----
All rm.<PERIPHERAL>.<REGISTER>.<FIELD> components subclass :class:`~Base_RM_Field`,
which implements this interface.

  .. autoclass:: IRegMapField
     :members:
     :show-inheritance:
     :inherited-members:

"""


__all__ = ['IRegMapField']

from abc import ABCMeta, abstractmethod, abstractproperty


class IRegMapField(object):
    """
    The interface for the dut.rm.<PERIPHERAL>.<REGISTER>.<FIELD> component. This
    component will also contain an enum attribute.
    """
    __metaclass__ = ABCMeta

    def _getio(self):
        """
        This is the io property getter to interact with the part's register
        field, which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.<FIELD>.io
        property to read the register field value.

        :rtype: ``long``
        :return: The field value.
        """
        pass

    def _setio(self, value):
        """
        This is the io property setter to interact with the part's register
        field, which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.<FIELD>.io
        property to assign a value.

        :type  value: ``int`` or ``long``
        :param value: The value to assign to the register field.
        """
        pass

    io = abstractproperty(_getio, _setio)

    def set(self, mask):
        """
        This is the bit-wise set method to interact with the part's register
        field, which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.<FIELD>.set()
        method call to set all the bits in the field or set certain specific bits by
        providing the mask value.

        Notes: This method uses the register set alias offsets for Series 2 devices with
        aliased register addresses. For other devices, it uses read-modify-write as a
        default strategy.

        Args:
            mask: The bit-wise set mask for the register field
        """

    def clr(self, mask):
        """
        This is the bit-wise clear method to interact with the part's register
        field, which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.<FIELD>.clr()
        method call to clear all the bits in the field or clear certain specific bits by
        providing the mask value.

        Notes: This method uses the register clear alias offsets for Series 2 devices with
        aliased register addresses. For other devices, it uses read-modify-write as a
        default strategy.

        Args:
            mask: The bit-wise clear mask for the register field
        """

    def tgl(self, mask):
        """
        This is the bit-wise toggle method to interact with the part's register
        field, which may be a live instance via J-Link or a simulated dictionary
        for offline use case. Use via the dut.<PERIPHERAL>.<REGISTER>.<FIELD>.tgl()
        method call to toggle all the bits in the field or toggle certain specific bits by
        providing the mask value.

        Notes: This method uses the register toggle alias offsets for Series 2 devices with
        aliased register addresses. For other devices, it uses read-modify-write as a
        default strategy.

        Args:
            mask: The bit-wise toggle mask for the register field
        """

    # Query Methods

    def getEnumNames(self):
        """
        Return a list of 'PERIPHERAL.REGISTER.FIELD.enum.ENUM' names for this field.

        :rtype: ``list`` of ``str``
        :return: The enum names for this field. Empty list if field is not enumerated.
        """
        pass

    # I/O Access Methods

    @abstractmethod
    def isReadable(self):
        """
        Check if field is readable.

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
        Clear the accessed flag for the field. Note that the accessed flag
        is set whenever the register is written. Any field write will set
        the accessed flag for the parent register as well.
        """
        pass

    @abstractmethod
    def setAccessedFlag(self):
        """
        Set the accessed flag for the field. Note that the accessed flag
        is set whenever the register is written. Any field write will set
        the accessed flag for the parent register as well.
        """
        pass

    @abstractmethod
    def getAccessedFlag(self):
        """
        Return the accessed flag for the field.

        :rtype: ``bool``
        :return: The state of the field's accessed flag
        """
        pass

    @abstractmethod
    def getSliceStr(self):
        """
        Return a string "{msb}:{lsb}" for the field's position within the parent
        register.

        :rtype: ``str``
        :return: The field slice string of format "msb:lsb" within the register
        """
        pass

    # Dump - Public

    @abstractmethod
    def includeInDump(self):
        """
        Dump the parent register by default when a filter list is not supplied
        to the top-level device dump() method.

        .. note:: The filter granularity stops at the register level. This
            function exists for the direct use case, where the user performs
            a dut.rm.PERIPHERAL.REGISTER.FIELD.includeInDump() call.
        """
        pass

    @abstractmethod
    def excludeFromDump(self):
        """
        Skip the parent register by default when a filter list is not supplied
        to the top-level device dump() method. Use includeInDump() to reverse.

        .. note:: The filter granularity stops at the register level. This
            function exists for the direct use case, where the user performs
            a dut.rm.PERIPHERAL.REGISTER.FIELD.excludeFrom() call.
        """
        pass

    # Dump - Private Helper

    @abstractmethod
    def dump(self, regValue, fieldCommentList):
        """
        Read register and store in valueDict. The file handle is passed to
        internal functions to do the source dictionary output.

        .. note:: This is an internal function used by the device container.
                  Do not call directly.

        :type  regValue: ``long``
        :param regValue: The register value to process into field review
        :type  fieldCommentList: ``list`` of ``str``
        :param fieldCommentList: A list to collect the comment string \
                                for all fields in the register
        """
        pass
