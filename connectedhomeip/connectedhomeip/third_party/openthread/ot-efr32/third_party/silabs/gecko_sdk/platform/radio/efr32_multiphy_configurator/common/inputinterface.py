'''
Created on Feb 7, 2014

@author: sesuskic
'''

__all__ = ["InputInterface"]

class InputInterface(object):
    def __init__(self):
        raise RuntimeError("Not allowed to crate instance of this class")
    
    def get_categories(self):
        return self.__dict__.keys()
    
    def get_category(self, name):
        return getattr(self, name)
    
    def get_options(self, category):
        return self.__dict__[category].get_options()
    
    def get_option(self, category, option):
        return self.get_options(category)[option]

