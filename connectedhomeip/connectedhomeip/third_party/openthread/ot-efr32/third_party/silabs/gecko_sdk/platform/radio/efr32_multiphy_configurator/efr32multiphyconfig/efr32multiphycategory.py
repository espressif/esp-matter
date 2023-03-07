
from common import CategoryInterface

__all__ = ["Efr32MultiPHYCategory"]

class Efr32MultiPHYCategory(CategoryInterface):
    """
    Imports and lists all the input categories defined for this library.
    This class manages the actual key/value pairs
    """
    
    NO_CATEGORY_STRING = "NO_CATEGORY"
    #OUTPUT_CATEGORY_STRING = "OUTPUT_OVERRIDE"

    def __init__(self, catagory_name, catagory_attributes):
        super(Efr32MultiPHYCategory, self).__init__()

        self._name = catagory_name
        self._options = dict()

        for attr_name, attr_default in catagory_attributes.items():
            self._options[attr_name] = attr_default
            setattr(self, str(attr_name), attr_default)

