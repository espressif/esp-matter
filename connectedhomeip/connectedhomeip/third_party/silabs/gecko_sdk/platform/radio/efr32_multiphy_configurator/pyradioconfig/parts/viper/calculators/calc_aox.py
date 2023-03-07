from pyradioconfig.parts.bobcat.calculators.calc_aox import Calc_AoX_Bobcat

class calc_aox_viper(Calc_AoX_Bobcat):
    def calc_aox_misc(self, model):
        aox_enable = True if model.vars.aox_enable.value == model.vars.aox_enable.var_enum.ENABLED else False
        fefilt_selected = model.vars.fefilt_selected.value

        if aox_enable:
            chfcoeffswsel = 2 # CHFCOEFFSWSEL_RAWCAP
            disafcsupp = 1 # disable AFC during the CTE
            chfswtrig = 1 # clk cycles to trigger after ets_set_mux, must be non-zero
        else:
            chfcoeffswsel = 0
            disafcsupp = 0
            chfswtrig = 0
            
        self._reg_write_by_name_concat(model, fefilt_selected, 'CFG_CHFCOEFFSWSEL', chfcoeffswsel)
        self._reg_write(model.vars.MODEM_CHFSWCTRL_CHFSWTIME, chfswtrig)
        self._reg_write(model.vars.MODEM_AFC_DISAFCCTE, disafcsupp)



