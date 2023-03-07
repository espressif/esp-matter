
"""
Enum
----
All rm.<PERIPHERAL>.<REGISTER>.<FIELD>.enum components subclass
:class:`~Base_RM_Enum`, which implements this interface.

  .. autoclass:: IRegMapEnumInt
     :members:
     :show-inheritance:

  .. autoclass:: IRegMapEnum
     :members:
     :show-inheritance:
     :inherited-members:

"""

__all__ = ['IRegMapEnum', 'IRegMapEnumInt']


from abc import ABCMeta, abstractmethod
try:
    unicode = unicode
except NameError:
    # 'unicode' is undefined, must be Python 3
    str = str
    unicode = str
    bytes = bytes
    basestring = (str, bytes)
    long = int
else:
    # 'unicode' exists, must be Python 2
    str = str
    unicode = unicode
    bytes = str
    basestring = basestring
    long = long


class IRegMapEnumInt(long):
    """
    The interface for handling the enums as a base int object with
    additional write attributes. This enables writing the enum value
    directly to the field.
    """

    @abstractmethod
    def write(self):
        """
        Write the enum value to the register field

        Returns:
            None
        """


class IRegMapEnum(object):
    """
    The interface for the dut.rm.<PERIPHERAL>.<REGISTER>.<FIELD>.enum
    component. This component will also contain enum name attributes in
    uppercase. The dut.rm.<PERIPHERAL>.<REGISTER>.<FIELD>.enum will
    contain an both value and description dictionaries. The __repr__()
    method will return name, value, and description details.
    """
    __metaclass__ = ABCMeta

    @abstractmethod
    def getDescByValue(self, value):
        """
        Get the enum description string based on the passed field value.

        :type  value: ``int`` or ``long``
        :param value: The field value
        :rtype: ``str``
        :return: The enum description
        """
        pass

    @abstractmethod
    def getNameByValue(self, value):
        """
        Get the enum name string based on the passed field value.

        :type  value: ``int`` or ``long``
        :param value: The field value
        :rtype: ``str``
        :return: The enum name
        """
        pass
