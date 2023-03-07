from .pro2plusmodemcalcinputs import Pro2PlusModemCalcInputs

__all__ = ["Pro2OcelotModemCalcInputs"]

class Pro2OcelotModemCalcInputs(Pro2PlusModemCalcInputs):
    API_dsm_ratio = 0.0
    API_Fs_rx_CH = 0.0
    API_fb_frequency_resolution = 0.0
    API_modulation_index = 0.0

    def __init__(self, input_data=None):
        super(Pro2OcelotModemCalcInputs, self).__init__(input_data)

    def _init_from_input_array(self, input_array):
        super(Pro2OcelotModemCalcInputs,self)._init_from_input_array(input_array[:-4])
        self.API_dsm_ratio = input_array[-4]
        self.API_Fs_rx_CH = input_array[-3]
        self.API_fb_frequency_resolution = input_array[-2]
        self.API_modulation_index = input_array[-1]

    def get_defaults(self):
        defaults = super(Pro2OcelotModemCalcInputs,self).get_defaults().copy()
        defaults["API_dsm_ratio"] = 0
        defaults["API_Fs_rx_CH"] = 0
        defaults["API_fb_frequency_resolution"] = 0
        defaults["API_modulation_index"] = 0
        return defaults
