from pyradioconfig.parts.jumbo.calculators.calc_shaping import CALC_Shaping_jumbo

class CALC_Shaping_nixi(CALC_Shaping_jumbo):

    def calc_shaping_misc(self, model):

        self._reg_write(model.vars.MODEM_SHAPING2_COEFF9, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF10, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF11, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF12, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF13, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF14, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF15, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF16, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF17, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF18, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF19, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF22, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF23, 0)