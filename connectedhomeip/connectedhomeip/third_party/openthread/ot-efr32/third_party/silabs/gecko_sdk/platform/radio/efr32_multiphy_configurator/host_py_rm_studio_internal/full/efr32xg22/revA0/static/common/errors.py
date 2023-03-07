
"""
Exceptions
^^^^^^^^^^
This package can raise a number of exceptions from the top-level device class.

  .. autoexception:: RegMapAccessError

  .. autoexception:: RegMapAddressError

  .. autoexception:: RegMapNameError

  .. autoexception:: RegMapValueError

  .. autoexception:: RegMapLoggingError

"""

__all__ = [
    'RegMapAccessError',
    'RegMapAddressError',
    'RegMapNameError',
    'RegMapValueError',
    'RegMapLoggingError'
]


class RegMapAccessError(Exception):
    """
    Indicates the register map access is invalid. For example, reading a write-only
    register or writing a read-only register.
    """
    pass


class RegMapAddressError(Exception):
    """
    Indicates an invalid address in the register map.
    """
    pass


class RegMapNameError(Exception):
    """
    Indicates an invalid peripheral register or register field name.
    """
    pass


class RegMapValueError(Exception):
    """
    Indicates a invalid range or type error for a register or field value.
    """
    pass


class RegMapLoggingError(Exception):
    """
    Indicates a invalid sequence, option, or file name.
    """
    pass
