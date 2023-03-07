
__all__ = [ 'VariableAccess' ]

# Singleton object

class VariableAccess(object):
    class __OnlyOne:
        def __init__(self):
            self.name = None

        def __str__(self):
            return 'self' + self.name

    instance = None

    def __new__(cls): # __new__ always a classmethod
        if not VariableAccess.instance:
            VariableAccess.instance = VariableAccess.__OnlyOne()
        return VariableAccess.instance

    def __getattr__(self, name):
        return getattr(self.instance, name)

    def __setattr__(self, name):
        return setattr(self.instance, name)