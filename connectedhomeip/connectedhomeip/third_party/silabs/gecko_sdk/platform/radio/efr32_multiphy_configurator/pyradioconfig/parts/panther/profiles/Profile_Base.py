from pyradioconfig.parts.common.profiles.Profile_Base import *



class Profile_Base_Panther(Profile_Base):

    """
    Init internal variables
    """
    def __init__(self):
        super(self.__class__, self).__init__()
        self._description = "Profile used for most phy's on EFR32xG21 parts"
        self._family = "panther"

