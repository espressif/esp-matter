'''
Created on Apr 2, 2013

@author: sesuskic
'''
from .pro2modemcalcinputs import Pro2ModemCalcInputs

__all__ = ["Pro2PlusModemCalcInputs"]

class Pro2PlusModemCalcInputs(Pro2ModemCalcInputs):
    
    API_PM_patt = 0.0
    API_patt_Len = 0.0
    API_pm_len = 0.0
    API_PMallow_Err = 0.0
    API_Sync1_Word = 0.0
    API_Sync_Len = 0.0
    API_Sync_err  = 0.0
    API_S4FSK_en = 0.0
     
    API_Pay_Len = 0.0
    API_CRC_en = 0.0
    API_3out6_en = 0.0
    API_PostPM_Len = 0.0
    API_Sync2_Word = 0.0
    API_Sync2_en = 0.0
    API_STPM = 0.0
    API_rxPH_SkipPM = 0.0
    TestMode       = 0.0
    pk_len         = 0.0 
    ch_model       = 0.0
    interf_freq    = 0.0
    interf_pw      = 0.0 
    rf_power       = 0.0
    Fdev_error     = 0.0

    def _init_from_input_array(self, input_array):
        super(Pro2PlusModemCalcInputs, self)._init_from_input_array(input_array)
        if len(input_array) == 24:
            self.API_dsa_mode = 0.0
            self.Fdev_error     = 0.0
            self.API_ETSI       = 0.0
            self.API_fhch       = 0
            self.API_pm_len     = 0            
        elif len(input_array) == 27:
            self.API_dsa_mode = input_array[24]
            self.Fdev_error     = input_array[25]
            self.API_ETSI       = input_array[26]
            self.API_fhch       = 0
            self.API_pm_len     = 0            
        elif len(input_array) == 29:
            self.API_dsa_mode = input_array[24]
            self.Fdev_error     = input_array[25]
            self.API_ETSI       = input_array[26]
            self.API_fhch       = input_array[27]
            self.API_pm_len     = input_array[28]
        elif len(input_array) in [48, 49]:
            self.API_PM_patt = input_array[24]
            self.API_patt_Len = input_array[25]
            self.API_pm_len = input_array[26]
            self.API_PMallow_Err = input_array[27]
            self.API_Sync1_Word = input_array[28]
            self.API_Sync_Len = input_array[29]
            self.API_Sync_err  = input_array[30]
            self.API_S4FSK_en = input_array[31]
            
            self.API_Pay_Len = input_array[32]
            self.API_CRC_en = input_array[33]
            self.API_3out6_en = input_array[34]
            self.API_PostPM_Len = input_array[35]
            self.API_Sync2_Word = input_array[36]
            self.API_Sync2_en = input_array[37]
            self.API_STPM = input_array[38]
            self.API_rxPH_SkipPM = input_array[39]
            self.TestMode       = input_array[40]
            self.pk_len         = input_array[41] #for BER Sim 
            self.ch_model       = input_array[42]
            self.interf_freq    = input_array[43] # how many Hz away from IF
            self.interf_pw      = input_array[44] # how many dB above desired signal 
            self.rf_power       = input_array[45] # desired signal power
            
            self.API_dsa_mode = input_array[46]
            self.Fdev_error     = input_array[47] # the reange to allow Fdev to very
            try:
                self.API_ETSI       = input_array[48]
                self.API_fhch       = input_array[49]
            except IndexError:
                self.API_ETSI = 0.0
                self.API_fhch = 0
        else:
            raise RuntimeError("Invalid number of inputs")    

    def __init__(self, input_data=None):
        super(Pro2PlusModemCalcInputs, self).__init__(input_data)

    def get_defaults(self):
        # Need a copy of the parent defaults, and then append a few more items.
        defaults = super(Pro2PlusModemCalcInputs, self).get_defaults().copy()
        defaults["Fdev_error"] = 0.0
        defaults["API_dsa_mode"] = 0
        defaults["API_ETSI"] = 0.0
        defaults["API_fhch"] = 10
        defaults["API_pm_len"] = 16
        return defaults
    
