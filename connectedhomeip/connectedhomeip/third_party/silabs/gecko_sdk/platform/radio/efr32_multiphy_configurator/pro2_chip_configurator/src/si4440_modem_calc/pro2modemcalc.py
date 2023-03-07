'''
Created on Mar 1, 2013

@author: sesuskic
'''
import math
from .pro2modemcalcinputs import Pro2ModemCalcInputs
from .pro2apilist import Pro2ApiList
from .pro2modulatorcore import Pro2ModulatorCore
from .pro2demodulatorcore import Pro2DemodulatorCore
from .modemcalcdata import ModemCalcData
from .pro2registers import Pro2Registers
from .calc_log import CalcLog

__all__ = ["Pro2ModemCalc"]

class Pro2ModemCalc(object):
    '''
    classdocs
    '''
    def _get_API_Max_Rb_Error_low_limit(self):
        return 0.01
    
    def _get_API_Max_Rb_Error_high_limit(self):
        return 0.1
    
    def _validate_API_pm_pattern(self):
        if ((self.inputs.API_pm_pattern<0) or (self.inputs.API_pm_pattern>31)) :
            self.inputs.API_pm_pattern = min(max(self.inputs.API_pm_pattern, 0), 31)
            self.warning2log += '# Valid range of API_pm_pattern is [0, 31].  Assign API_pm_pattern to {:d} \n'.format(self.inputs.API_pm_pattern)
#             print(('# Valid range of API_pm_pattern is [0, 31].  Assign API_pm_pattern to {:d} \n'.format(self.inputs.API_pm_pattern)))
    
    def _validate_API_Manchester(self):
        if (self.inputs.API_Manchester):   # force to 0/1
            self.inputs.API_Manchester = 1
        

    def _validate_API_afc_en(self):
        if (self.inputs.API_afc_en): # force to 0/1
            self.inputs.API_afc_en = 1

    def _validate(self):
        #API argument range checking
        FXO = [25e6,  32e6]    # Xtal freq range [25.5e6  34e6] => [25,34] on July 5, 2012
        if (self.inputs.API_freq_xo < FXO[0] ) or (self.inputs.API_freq_xo>FXO[1] ) :
            self.warning2log += '# Crystal oscillator freq is out of range! [{:3.2f} {:3.2f}] MHz'.format(FXO[0]/1e6,FXO[1]/1e6)
#             print(('# Crystal oscillator freq is out of range! [{:3.2f} {:3.2f}] MHz'.format(FXO[0]/1e6,FXO[1]/1e6)))
    
        self.inputs.API_crystal_tol = abs(self.inputs.API_crystal_tol)
        if (self.inputs.API_crystal_tol>1e4) :
            self.calc_log.add_to_log('# API_crystal_tol is too big! Max allowed value is 10000', True)
    
        self.inputs.API_if_mode = int(math.floor(self.inputs.API_if_mode)) # force to integer 0/1/2
    
        if (self.inputs.API_High_perf_Ch_Fil) :   # force to 0/1
            self.inputs.API_High_perf_Ch_Fil = 1
    
        # 10/19/2011: change OSRtune from [0.1, 25] into step up or down OSR, i.e., 
        # 0: normal, +1: step up OSR (ndec-1);  -2: step down OSR (e.g. remove
        # dec-3)
        self.inputs.API_OSRtune = int(math.floor(self.inputs.API_OSRtune)) # force to integer
        
        if (self.inputs.API_Ch_Fil_Bw_AFC) :    # force to 0/1
            self.inputs.API_Ch_Fil_Bw_AFC = 1
    
        if (self.inputs.API_ant_div) :          # force to 0/1
            self.inputs.API_ant_div = 1
    
        self._validate_API_pm_pattern()
        
        # row 2
        self.inputs.API_modulation_type = int(self.inputs.API_modulation_type) # force to integer
        if ((self.inputs.API_modulation_type<0) or (self.inputs.API_modulation_type>5)):
            self.inputs.API_modulation_type = min(max(self.inputs.API_modulation_type, 0), 5)
            self.warning2log += '# Valid range of API_modulation_type is [0, 5].  Assign API_modulation_type to {:d} \n'.format(self.inputs.API_modulation_type)
#             print(('# Valid range of API_modulation_type is [0, 5].  Assign API_modulation_type to {:d} \n'.format(self.inputs.API_modulation_type)))
        
        # API_Rsymb/Fdev coupled to mod-type
        # ookBW
        if ((self.inputs.API_modulation_type==1) and (self.inputs.API_RXBW<=2*self.inputs.API_Rsymb) ): # check ook_BW in OOK mode
            self.calc_log.add_to_log('# OOK_BW={:3.2f}kHz is too small for data rate Rsymb={:3.2f}ksps'.format(self.inputs.API_RXBW/1e3, self.inputs.API_Rsymb/1e3), True)
        
        self._validate_API_Manchester()
    
        self._validate_API_afc_en()

        self.inputs.API_Max_Rb_Error = abs(self.inputs.API_Max_Rb_Error)
        if (self.inputs.API_Max_Rb_Error<=self._get_API_Max_Rb_Error_low_limit()) :                       
            self.API_Rb_error = 0
        elif (self.inputs.API_Max_Rb_Error > self._get_API_Max_Rb_Error_low_limit() and self.inputs.API_Max_Rb_Error <=self._get_API_Max_Rb_Error_high_limit()) : 
            self.API_Rb_error = 1
        # 9/11/2012: remove option for 20% DataRate Error. PRO2 currently can NOT track 20%
        # elseif (API_Max_Rb_Error > 0.1  && API_Max_Rb_Error <=0.2)
        #     API_Rb_error = 2;
        else : # data rate error >0.2
            self.API_Rb_error = 1
    
        self.inputs.API_Chip_Version = int(math.floor(self.inputs.API_Chip_Version))
        if ( (self.inputs.API_Chip_Version<0) or (self.inputs.API_Chip_Version>3) ): # limit to {0,1,2,3}, 07/26/2011
            self.warning2log += '# API_Chip_Version must be 0,1,2, or 3.  Assigned to 2\n'
#             print('API_Chip_Version must be 0,1,2, or 3.  Assigned to 2\n')
            self.inputs.API_Chip_Version = 2
        
        self.inputs.API_TC = int(math.floor(self.inputs.API_TC))   # force to integer in [0,31]; in (G)FSK, <=29
        self.crystal_tol = self.inputs.API_crystal_tol
        
        # force the following block to binary: 0/1
        if (self.inputs.API_inputBW):
            self.inputs.API_inputBW = 1
    
        if (self.inputs.API_hi_pfm_div_mode):
            self.inputs.API_hi_pfm_div_mode = 1
    
        if (self.inputs.API_BER_mode):
            self.inputs.API_BER_mode = 1
    
        if (self.inputs.API_raw_demod):
            self.inputs.API_raw_demod = 1
    
        # ideally this checking is done by WDS
        self.inputs.API_dsource = int(math.floor((self.inputs.API_dsource)))
        if ((self.inputs.API_dsource<0) or (self.inputs.API_dsource>31)):
            self.inputs.API_dsource = min(max(self.inputs.API_dsource, 0), 31)
            text = '# Valid range of API_dsource is [0, 31]. Assign API_dsource to {:d} \n'.format(self.inputs.API_dsource) 
            self.warning2log += text
#             print(text);
        
        return

    def _init_modulator_core(self, ):
        self.modulator = Pro2ModulatorCore()
    def _init_demodulator_core(self, ):
        self.demodulator = Pro2DemodulatorCore()
        

    def _init_api_list(self):
        self.api_list = Pro2ApiList()
    
    def _create_registers(self):
        self.registers = Pro2Registers(self)
        

    def __init__(self, inputs = None):
        '''
        Constructor
        '''
        if inputs is None:
            inputs = Pro2ModemCalcInputs()
        self.inputs = inputs
        self.original_inputs = self.inputs
    
    def initialize(self):
        self.API_Rb_error = 0 # default to some value, but it gets set in validate()
        self.w_size = 32
        self.if_freq              = 0.0
        self.if_freq_iqcal        = 0.0   # calculate this in op-mode: jira-1055
        self.IQ_CAL_setting       = 0
        self.IQ_CAL_Fc            = 0.0
        if not hasattr(self, "calc_log"):
            self.calc_log = CalcLog()
        self.warning2log = ''

        self.svnRev = 8810
        
        self._init_modulator_core()
        self._init_demodulator_core()
        self._init_api_list()
        self._validate()
        return
    
    def _modem_calc_calculate(self) :
        #  modulator register calculator
        self.modulator.calculate(self)
        #  demodulator register calculator
        self.demodulator.calculate(self)
        self.api_list.populate(self)
        self._create_registers()
        self._data = ModemCalcData()
        self._data.populate(self)
    
    def calculate_iqcal(self):
        self.warning2log = ''   # init with empty string
        
        self.inputs.API_High_perf_Ch_Fil = 1  # (0/1), if '0', chfil tap size drops from 27 to 15
        self.inputs.API_Ch_Fil_Bw_AFC    = 0  # (0/1) dis/enable filter switch after AFC
        self.inputs.API_ant_div          = 0  # (0/1) dis/enable RX antenna div
        self.inputs.API_pm_pattern       = 0  # 0-"1010";1-"xx" repeat;2-random data;3-constant 1
        
        self.inputs.API_modulation_type  = 3  # modulation type is 0=carrier/1=OOK/2=2FSK/3*=2GFSK/4=4FSK/5=4GFSK
        
        self.inputs.API_Rsymb            = 4e3  # symbol rate (sps). This is post Manchester encoding if Manchester=1.
        self.inputs.API_Fdev             = 1e3  # in Hz, fsk modulation deviation (inner)

        # print('Fc= {0:.2f}MHz, if_freq = {0:.2f}'.format(self.inputs.API_fc/1e6, self.if_freq) )
        # print('if_freq = {0:.2f} Hz'.format(self.if_freq) )
        # print('if_freq_iqcal = {0:.2f} Hz'.format(self.if_freq_iqcal) )
        self.inputs.API_fc = self.IQ_CAL_Fc - 2*self.if_freq_iqcal       # use freq computed from operation mode
        #API_fhst             = modem_test_plan_in(3,3);  # FH step size in Hz
        #API_inputBW   = modem_test_plan_in(3,4);  # (0/1)
        self.inputs.API_BER_mode         = 1  # (0/1) BER mode enable 
        self.inputs.API_raw_demod        = 0  # (0/1) o/p raw bits w/o BCR
        #API_dsource          = modem_test_plan_in(3,7)  # (0=pk/1=dir/2=async dir/3=mcu) mode
        #API_hi_pfm_div_mode  = modem_test_plan_in(3,8)  # (0/1)
        
        self.inputs.API_OSRtune = 0    # 8-bit, 0.1/bit,range is from 0 to 255
        self.inputs.API_crystal_tol = 0
        self.inputs.API_inputBW = 0
        self.IQ_CAL_setting = 1
    
        self._modem_calc_calculate()
        
    def calculate(self):
        self.initialize()
        self._modem_calc_calculate()
    
    def get_api_list(self, radio_api=None):
        return self.api_list.get_api_list(radio_api)
    
    def get_data(self):
        return self._data.get()
        
        
