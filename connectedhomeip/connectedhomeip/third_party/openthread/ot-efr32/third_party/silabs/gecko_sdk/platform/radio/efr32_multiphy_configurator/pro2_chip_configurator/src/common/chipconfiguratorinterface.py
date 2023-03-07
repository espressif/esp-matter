'''
Created on Feb 18, 2014

@author: sesuskic
'''

__all__ = ["ChipConfiguratorInterface"]

from copy import copy

class ChipConfiguratorInterface(object):

    def __init__(self):
        self.inputs = None
        self.categories = {}
        self.options = {}

    def _sync_categories_and_options(self):
        for category in self.inputs.get_categories():
            self._sync_category(category)
            
    def _sync_category(self, category):
            options = {}
            for key, value in self.inputs.get_options(category).items():
                self.options[category+"."+key] = value
                options[category+"."+key] = value
            self.categories[category] = copy(options)
        
    def configure(self, **kwargs):
        raise RuntimeError("Must implement in derived class")

    def instance(self, chipId):
        return self
    
    def get_categories(self):
        return self.categories.keys()
    
    def get_category(self, name):
        return self.inputs.get_category(name)
    
    def get_options(self, category = None):
        if category in [None, "none", "Null", "null", ""]:
            return self.options
        return self.categories[category]
    
    def set_option(self, option, value):
        [cat, opt] = option.split(".")
        real_value = self.inputs.get_category(cat).set_option(opt, value)
        if real_value is None:
            return
        if isinstance(real_value, dict):
            self._sync_category(cat)
        else:
            self.categories[cat][option] = real_value
            self.options[option] = real_value
        
    def get_option(self, option):
        return self.options[option]
    
    def set_profile(self, name):
        pass
    
    def get_profile_ids(self):
        return []
    
    def get_profile_data(self, profile):
        return {}
    
    def add_profile(self, profile, data):
        pass
