
# -*- coding: utf-8 -*-

from . static import Base_RM_Register
from . SUNOFDM_field import *


class RM_Register_SUNOFDM_CFG1(Base_RM_Register):
    def __init__(self, rmio, label):
        self.__dict__['zz_frozen'] = False
        super(RM_Register_SUNOFDM_CFG1, self).__init__(rmio, label,
            0xb500fc00, 0x000,
            'CFG1', 'SUNOFDM.CFG1', 'read-write',
            u"",
            0x00000000, 0xFFFF0FFF,
            0x00001000, 0x00002000,
            0x00003000)

        self.MODULATION = RM_Field_SUNOFDM_CFG1_MODULATION(self)
        self.zz_fdict['MODULATION'] = self.MODULATION
        self.OFDMOPTION = RM_Field_SUNOFDM_CFG1_OFDMOPTION(self)
        self.zz_fdict['OFDMOPTION'] = self.OFDMOPTION
        self.INTERLEAVING = RM_Field_SUNOFDM_CFG1_INTERLEAVING(self)
        self.zz_fdict['INTERLEAVING'] = self.INTERLEAVING
        self.MACFCSTYPE = RM_Field_SUNOFDM_CFG1_MACFCSTYPE(self)
        self.zz_fdict['MACFCSTYPE'] = self.MACFCSTYPE
        self.XTALFREQ = RM_Field_SUNOFDM_CFG1_XTALFREQ(self)
        self.zz_fdict['XTALFREQ'] = self.XTALFREQ
        self.__dict__['zz_frozen'] = True


