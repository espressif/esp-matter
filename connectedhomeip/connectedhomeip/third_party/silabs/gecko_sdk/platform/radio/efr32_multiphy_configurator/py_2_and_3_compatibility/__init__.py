import math
import sys

if sys.version_info[0] < 3:
    # Assume Python 2
    class FileExistsError(Exception):
        pass

    pass
else:
    # hack for backward compatibility
    long = int
    xrange = range
    basestring = str
    sys.maxint = 9223372036854775807
    import builtins as __builtin__
    import imp
    reload = imp.reload
    unicode = str

# try:
#     import __builtin__
# except ModuleNotFoundError:
#     import builtins as __builtin__

def py2round(x):
    if x >= 0.0:
        return math.floor(x + 0.5)
    else:
        return math.ceil(x - 0.5)

def py2floatround(number, places=0):
    '''
    py2floatround(number, places)

    example:

        >>> py2floatround(2.55, 1) == 2.6
        True

    uses standard functions with no import to give "normal" behavior to
    rounding so that trueround(2.5) == 3, trueround(3.5) == 4,
    trueround(4.5) == 5, etc. Use with caution, however. This still has
    the same problem with floating point math. The return object will
    be type int if places=0 or a float if places=>1.

    number is the floating point number needed rounding

    places is the number of decimal places to round to with '0' as the
        default which will actually return our interger. Otherwise, a
        floating point will be returned to the given decimal place.
    '''
    place = 10**(places)
    rounded = (int(number*place + 0.5if number>=0 else -0.5))/place
    if rounded == int(rounded):
        rounded = int(rounded)
    return rounded
