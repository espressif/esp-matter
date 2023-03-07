from pyradioconfig.parts.bobcat.calculators.calc_diversity import Calc_Diversity_Bobcat

class calc_diversity_viper(Calc_Diversity_Bobcat):
    
    def calc_diversity_default_configs(self, model):
        model.vars.antdiv_enable_parallel_correlation.value = False
    
    def calc_diversity_adpcen(self, model):
        pass
    
    def calc_diveristy_adpc_windowsize(self, model):
        pass
    
    def calc_diversity_adpc_windowcnt(self, model):
        pass

    def calc_diversity_adpc_timingbauds(self, model):
        pass
    
    def calc_diversity_adpc_corroffsetchip(self, model):
        pass

    def calc_diversity_adpc_corrsamples(self, model):
        pass

    def calc_diversity_adpc_buffersize(self, model):
        pass

    def calc_diversity_adctrl1(self, model):
        pass

    def calc_diversity_adctrl2(self, model):
        pass

    def calc_adpcsigampthr_reg(self, model):
        pass

    def calc_adprethresh_scale(self, model):
        pass

    def calc_diversity_adprethresh(self, model):
        pass
