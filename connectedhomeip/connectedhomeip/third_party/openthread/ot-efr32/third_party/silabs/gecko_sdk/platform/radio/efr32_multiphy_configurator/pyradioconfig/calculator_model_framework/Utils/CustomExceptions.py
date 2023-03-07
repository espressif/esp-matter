
__all__ = ['UnknownOPNTypeException', 'InvalidOptionOverride' ]

"""
Holds all custom exceptions used in this package
"""

class UnknownOPNTypeException(Exception):
    pass

class InvalidOptionOverride(Exception):
    pass

class CalculationException(Exception):
    pass

class UnknownProfileException(Exception):
    pass