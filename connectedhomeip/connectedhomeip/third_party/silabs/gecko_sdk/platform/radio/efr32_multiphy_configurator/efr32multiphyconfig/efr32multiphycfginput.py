from common import InputInterface
from .efr32multiphycategory import Efr32MultiPHYCategory

__all__ = ["Efr32MultiPHYCfgInput"]

class Efr32MultiPHYCfgInput(InputInterface):
    """
    Input is a high level container that holds categories.
    Do not get this confused with the actual input kay/value pairs.
    This is just a container for other objects.
    """

    def __init__(self):
        catagories = dict()
        catagories['inputs'] = {'some_input': 0}  # attribute name and default value
        catagories['outputs'] = {'some_input': 0}

        # Add all categories as attributes
        for catagory_name, catagory_attributes in catagories.items():
            setattr(self, str(catagory_name), Efr32MultiPHYCategory(catagory_name, catagory_attributes))



