import math
import sys
if sys.version_info[0] < 3:
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

# try:
#     import __builtin__
# except ModuleNotFoundError:
#     import builtins as __builtin__

def py2round(x):
    if x >= 0.0:
        return math.floor(x + 0.5)
    else:
        return math.ceil(x - 0.5)