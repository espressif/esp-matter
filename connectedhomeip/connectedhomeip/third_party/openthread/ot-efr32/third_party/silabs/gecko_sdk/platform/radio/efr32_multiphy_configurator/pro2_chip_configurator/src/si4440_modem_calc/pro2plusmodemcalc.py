'''
Created on Mar 20, 2013

@author: sesuskic
'''

from .pro2modemcalc import Pro2ModemCalc
from .pro2plusmodemcalcinputs import Pro2PlusModemCalcInputs
from .pro2plusdemodulatorcore import Pro2PlusDemodulatorCore
from .pro2modulatorcore import Pro2PlusModulatorCore
from .pro2plusapilist import Pro2PlusApiList
from .pro2registers import Pro2PlusRegisters
from .decode_api import get_df
from .trueround import trueround

__all__ = ["Pro2PlusModemCalc", "Pro2PlusLegacyModemCalc"]

class Pro2PlusModemCalc(Pro2ModemCalc):
    
    def _get_API_Max_Rb_Error_low_limit(self):
        return 0.05

    def _get_API_Max_Rb_Error_high_limit(self):
        return 0.2
    
    def _validate_API_pm_pattern(self):
        return

    def _dsa_rx_hopping(self):
        ###################### DSA RX hopping for super low data rate
        #  if(self.inputs.API_dsa_mode ==3):
        #  calculate DSA RX hopping step size using RX bandwidth
        self.BW_mod =self.inputs.API_Rsymb+2*get_df(self.inputs.API_modulation_type,self.inputs.API_Fdev) # modulation BW (Hz)

        self.tx_rx_err = self.inputs.API_fc*self.inputs.API_crystal_tol*2/10**6

        self.max_freq_error = 2*self.tx_rx_err # Hz (evaled from API_RXBW-modBW in inputBW==1)
        if(self.inputs.API_dsa_mode ==3.0):
            # FH step size in Hz, it is overwritten by  BW_mod/2
            self.inputs.API_fhst = trueround(1.0*self.BW_mod/2)
            self.inputs.API_fhch = trueround(self.max_freq_error/self.inputs.API_fhst)
            self.inputs.API_fc = trueround(self.inputs.API_fc - self.tx_rx_err)
            self.inputs.API_inputBW =0
            self.inputs.API_afc_en = 2.0
            self.inputs.API_crystal_tol = 0.0
        #return

    def _validate_API_Manchester(self):
        if self.inputs.API_Manchester == 0:   # no Manch crossing entired packet
            self.inputs.API_Manchester = 0
            self.demodulator.fields.pk_pm_manch = 0
            self.demodulator.fields.pk_sync_manch = 0
            self.demodulator.fields.pk_field_manch = 0 
        elif self.inputs.API_Manchester == 1:   # Manch crossing entired packet
            self.inputs.API_Manchester = 1
            self.demodulator.fields.pk_pm_manch = 1
            self.demodulator.fields.pk_sync_manch = 1
            self.demodulator.fields.pk_field_manch = 1
        elif self.inputs.API_Manchester == 2:  # Manch crossing entired packet except PM
            self.inputs.API_Manchester = 0
            self.demodulator.fields.pk_pm_manch = 0
            self.demodulator.fields.pk_sync_manch = 1
            self.demodulator.fields.pk_field_manch = 1
        elif self.inputs.API_Manchester == 3:   # Manch crossing entired packet except PM & Sync-word
            self.inputs.API_Manchester = 0
            self.demodulator.fields.pk_pm_manch = 0
            self.demodulator.fields.pk_sync_manch = 0
            self.demodulator.fields.pk_field_manch = 1
        elif self.inputs.API_Manchester == 4:  # Manch crossing entired packet except Sync-word
            self.inputs.API_Manchester = 0
            self.demodulator.fields.pk_pm_manch = 1
            self.demodulator.fields.pk_sync_manch = 0
            self.demodulator.fields.pk_field_manch = 1
        else:   # no Manch crossing entired packet
            self.inputs.API_Manchester = 0
            self.demodulator.fields.pk_pm_manch = 0
            self.demodulator.fields.pk_sync_manch = 0
            self.demodulator.fields.pk_field_manch = 0
           
    def _validate_API_afc_en(self):
        return

    def _init_modulator_core(self, ):
        self.modulator = Pro2PlusModulatorCore()
        
    def _init_demodulator_core(self):
        self.demodulator = Pro2PlusDemodulatorCore()
        
    def _init_api_list(self):
        self.api_list = Pro2PlusApiList()

    def _create_registers(self):
        self.registers = Pro2PlusRegisters(self)

    # jira-1651: ifpkd)_th  <-0xB5 for ETSI==1 868M or MBUS-Ng/GRDF;  <-0xF9 for ETSI=2 169MHz
    # add ESTI=3 for Mbus-Ng??
    def _validate_etsi(self):
        if self.inputs.API_ETSI in [0, 1, 2, 3]:
            if self.inputs.API_ETSI == 1:
                ## remove frequency restriction from calculator for ETSI mode applicability SI4440-1748
                #if self.inputs.API_fc > 878e6 or self.inputs.API_fc < 858e6:
                #    raise RuntimeError("Invalid value of API_ETSI: {}, for given frequency {}".format(self.inputs.API_ETSI, self.inputs.API_fc))
                self.demodulator.fields.ifpkd_th = 181    # 0xB5
            if self.inputs.API_ETSI == 2:
                ## remove frequency restriction from calculator for ETSI mode applicability SI4440-1748
                #if self.inputs.API_fc > 179e6 or self.inputs.API_fc < 159e6:
                #    raise RuntimeError("Invalid value of API_ETSI: {}, for given frequency {}".format(self.inputs.API_ETSI, self.inputs.API_fc))
                self.demodulator.fields.ifpkd_th = 249    # 0xF9
            if self.inputs.API_ETSI == 3:
                self.demodulator.fields.ifpkd_th = 181    # 0xB5 for Mbus-Ng or GRDF
            pass
        else:
            raise RuntimeError("Invalid value of API_ETSI: {}! Valid values are 0, 1, 2, or 3".format(self.inputs.API_ETSI))
                                     
    def __init__(self, inputs = None):
        if inputs is None:
            inputs = Pro2PlusModemCalcInputs()
        super(Pro2PlusModemCalc, self).__init__(inputs)
        self.revc0_c1 = False
        
    def initialize(self):
        super(Pro2PlusModemCalc, self).initialize()
        self._validate_etsi()
        self._dsa_rx_hopping()

class Pro2PlusLegacyModemCalc(Pro2PlusModemCalc):
    
    def __init__(self, inputs = None):
        super(Pro2PlusLegacyModemCalc, self).__init__(inputs)
        self.revc0_c1 = True
