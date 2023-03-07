from enum import IntEnum


class ModelDiffCodes(IntEnum):
    """
    Warnings are positive numbers
    Errors are negative numbers
    """
    OK = 0
    UNKNOWN_ERROR = -1
    UNKNOWN_WARNING = 1