from pyradioconfig.parts.bobcat.calculators.calc_agc import Calc_AGC_Bobcat

class Calc_AGC_Viper(Calc_AGC_Bobcat):
    def calc_rssiperiod_val(self, model):        
        rssi_period_val = 3
        model.vars.rssi_period.value = rssi_period_val

    def calc_antdiv_gainmode_reg(self, model):
        pass

    def calc_antdiv_debouncecntthd(self, model):
        pass
    pass
