'''
Created on Feb 14, 2014

@author: sesuskic
'''

import ast
import types

__all__ = ["CategoryInterface"]

class CategoryInterface(object):
    def __init__(self, radioApi = None):
        self.enabled = True
    
    def get_options(self):
        return self.__dict__
    
    # set_option() returns the 'coerced' value. 'Coercion' can be multi-faceted since the parameter 'value'
    # could be of completely type from that of what's behind the option. Any derived class that implements this
    # method needs to have this in mind.
    def set_option(self, option, value):
        
#             return
        # getattr() will throw AttributeError if the option does 
        # not exist
        opt = getattr(self, option)
        opt_type = type(opt)
        if type(opt) is type(value):
            newValue = value
        else:
            if type(opt) is bool and value in [True, "TRUE", "True","true", "YES", "Yes", "yes", "ON", "On", "on", "1", 1]:
                newValue = True
            elif type(opt) is bool and value in [False, "FALSE", "False","false", "NO", "No", "no", "OFF", "Off", "off", "0", 0]:
                newValue = False
            elif type(opt) in [int, float] and value in [True, "TRUE", "True","true", "YES", "Yes", "yes", "ON", "On", "on", "1", 1]:
                if type(opt) is float:
                    newValue = 1.0
                else:
                    newValue = 1    
            elif type(opt) in [int, float] and value in [False, "FALSE", "False","false", "NO", "No", "no", "OFF", "Off", "off", "0", 0]:
                if type(opt) is float:
                    newValue = 0.0
                else:
                    newValue = 0  
            else:
                try:
                    newValue = opt_type(eval(format(value)))
                except (NameError, SyntaxError, TypeError, ValueError) as e:
                    try:
                        newValue = opt_type(ast.literal_eval(value))
                    except (NameError, SyntaxError, TypeError, ValueError) as e:
                        try:
                            newValue = opt_type(ast.literal_eval("{}".format(value)))
                        except (NameError, SyntaxError, TypeError, ValueError) as e:
                            try:
                                newValue = opt_type(ast.literal_eval("\"{}\"".format(value)))
                            except (NameError, SyntaxError, TypeError, ValueError) as e:
                                raise RuntimeError("Failed to set option {} to \"{}\"".format(option, value))
        setattr(self, option, newValue)
        return newValue
        
    def validate(self):
        pass

    def reset(self):
        pass
    
    def get_configuration(self, chipConfigurator):
        return []
        