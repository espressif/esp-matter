'''
Created on Mar 29, 2013

@author: sesuskic
'''
from collections import OrderedDict
from pro2_chip_configurator.src.common import CategoryInterface

__all__ = ["Pro2ModemCalcInputs"]


class DefaultInputs(object):
    default_inputs = OrderedDict({})
    default_inputs["API_freq_xo"] =  30000000.0
    default_inputs["API_crystal_tol"] =  30.0
    default_inputs["API_if_mode"] =  2.0
    default_inputs["API_High_perf_Ch_Fil"] =  1.0
    default_inputs["API_OSRtune"] =  0.0
    default_inputs["API_Ch_Fil_Bw_AFC"] =  0.0
    default_inputs["API_ant_div"] =  0.0
    default_inputs["API_pm_pattern"] =  10.0
    default_inputs["API_modulation_type"] =  2.0
    default_inputs["API_Rsymb"] = 10000.0
    default_inputs["API_Fdev"] =  20000.0
    default_inputs["API_RXBW"] =  200000.0
    default_inputs["API_Manchester"] =  0.0
    default_inputs["API_afc_en"] =  1.0
    default_inputs["API_Max_Rb_Error"] =  0.0 
    default_inputs["API_Chip_Version"] =  2.0
    default_inputs["API_fc"] =  430e6
    default_inputs["API_TC"] =  28.0
    default_inputs["API_fhst"] =  250000.0
    default_inputs["API_inputBW"] =  0.0
    default_inputs["API_BER_mode"] =  0.0
    default_inputs["API_raw_demod"] =  0.0
    default_inputs["API_dsource"] =  0.0
    default_inputs["API_hi_pfm_div_mode"] =  0.0
        
    def get(self):
        return self.default_inputs
    
class Pro2ModemCalcInputs(CategoryInterface):

    def get_defaults(self):
        return DefaultInputs().get()

    def _init_from_input_array(self, input_array):
        ## read in API params
        try:
            self.API_freq_xo          = input_array[0]  # crystal frequency, unit is Hz  ([25.6, 33] MHz) => [25,32]MHz data sheet
            self.API_crystal_tol      = input_array[1]  # Crystal tolerance, unit is ppm ( [0,100] )
            self.API_if_mode          = input_array[2]  # IF mode (0/1/2/3)
            self.API_High_perf_Ch_Fil = input_array[3]  # (0/1), if '0', chfil tap size drops from 27 to 15
            self.API_OSRtune          = input_array[4]  # tuning parameter for OSR,[0.5, 10] => 10/19/2011 change to step up or down
            self.API_Ch_Fil_Bw_AFC    = input_array[5]  # (0/1) dis/enable filter switch after AFC
            self.API_ant_div          = input_array[6]  # (0/1) dis/enable RX antenna div
            self.API_pm_pattern       = input_array[7]  # 0-"1010";1-"xx" repeat;2-random data;3-constant 1
            
            self.API_modulation_type  = input_array[8]  # modulation type is 0=carrier/1=OOK/2=2FSK/3*=2GFSK/4=4FSK/5=4GFSK
            self.API_Rsymb            = input_array[9]  # symbol rate (sps). This is post Manchester encoding if Manchester=1. 
            self.API_Fdev             = input_array[10]  # in Hz, fsk modulation deviation (inner dev)
            self.API_RXBW             = input_array[11]  # RX chfil BW in Hz
            self.API_Manchester       = input_array[12]  # (0/1) enable API_Manchesterer
            self.API_afc_en           = input_array[13]  # (0/1) enable AFC correction feedback to frac-N PLL
            self.API_Max_Rb_Error     = input_array[14]  # data rate error 
            self.API_Chip_Version     = input_array[15]  # (0/1) Chip version: 0 for normal; 1 for narrowband continuous RF coverage
            
            self.API_fc               = input_array[16]  # in Hz, RF carrier frequency
            self.API_TC               = input_array[17]  # PA ramp time
            self.API_fhst             = input_array[18]  # FH step size in Hz
            # updated 4/3/2012:  in (G)FSK if API_inputBW=1 then use RXBW w/o ppm calculation
            self.API_inputBW          = input_array[19]  # (0/1) to indicate whether use input RXBW or ppm 
            self.API_BER_mode         = input_array[20]  # (0/1) BER mode enable 
            self.API_raw_demod        = input_array[21]  # (0/1) o/p raw bits w/o BCR
        
            # updated 03/23/2012: WDS will combine input 3 fields to form 5 MSBs of
            # MODEM_MOD_TYPE dsource = MOD_SOURCE + TX_DIRECT_MODE_GPIO*2**2 + TX_DIRECT_MODE_TYPE*2**4
            self.API_dsource          = input_array[22]  # (0=pk/1=dir/2=async dir/3=mcu) mode; updated 3/23/2012 to include direct-mode inputs
            self.API_hi_pfm_div_mode  = input_array[23]   # (0/1)
        except:
            raise RuntimeError('Invalid format')
        
    def __init__(self, input_data=None):
        super(Pro2ModemCalcInputs, self).__init__()
        # First, need to initialize the class with right attributes.
        self._init_from_input_array([0.0]*len(self.get_defaults()))
        if (input_data == None):
            self.set_inputs_from_dict( self.get_defaults())
        elif (isinstance(input_data, list)):
            self._init_from_input_array(input_data)
        elif(isinstance(input_data, dict)):
            self.set_inputs_from_dict(input_data)
        else:
            raise RuntimeError('Invalid input data. Input data must be list or dictionary')
    
    def set_inputs_from_dict(self, input_dict):
        for k, v in input_dict.items():
            self.set_option(k, v)

    def reset(self):
        self.set_inputs_from_dict(self.get_defaults())