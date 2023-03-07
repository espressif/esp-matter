from pyradioconfig.parts.common.profiles.Profile_Base import *



class Profile_Base_Dumbo(Profile_Base):

    """
    Init internal variables
    """
    def __init__(self):
        super(self.__class__, self).__init__()

        self._family = "dumbo"

