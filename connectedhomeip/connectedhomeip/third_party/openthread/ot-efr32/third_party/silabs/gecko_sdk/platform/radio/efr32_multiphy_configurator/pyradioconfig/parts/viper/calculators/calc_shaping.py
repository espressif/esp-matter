from pyradioconfig.parts.bobcat.calculators.calc_shaping import Calc_Shaping_Bobcat
import numpy as np

class Calc_Shaping_Viper(Calc_Shaping_Bobcat):
    # In mode 1 and 2, I think, the calculator only generates filters with up to 8 coefficients anyway so no need to
    # update the gain calculations for those cases. If, however, we change shaping filter coefficient calculation we
    # might need to update gain calculations as well. -- Guner

    def calc_shaping_filter_gain_actual(self, model):
        # The Ocelot shaping filter registers have changed in the register map, so need to override this function

        shaping_filter_mode = model.vars.MODEM_CTRL0_SHAPING.value
        c0 = model.vars.MODEM_SHAPING0_COEFF0.value  # SHAPING0
        c1 = model.vars.MODEM_SHAPING0_COEFF1.value
        c2 = model.vars.MODEM_SHAPING0_COEFF2.value
        c3 = model.vars.MODEM_SHAPING0_COEFF3.value
        c4 = model.vars.MODEM_SHAPING1_COEFF4.value  # SHAPING1
        c5 = model.vars.MODEM_SHAPING1_COEFF5.value
        c6 = model.vars.MODEM_SHAPING1_COEFF6.value
        c7 = model.vars.MODEM_SHAPING1_COEFF7.value
        c8 = model.vars.MODEM_SHAPING2_COEFF8.value  # SHAPING2
        c9 = model.vars.MODEM_SHAPING2_COEFF9.value
        c10 = model.vars.MODEM_SHAPING2_COEFF10.value
        c11 = model.vars.MODEM_SHAPING2_COEFF11.value
        c12 = model.vars.MODEM_SHAPING3_COEFF12.value  # SHAPING3
        c13 = model.vars.MODEM_SHAPING3_COEFF13.value
        c14 = model.vars.MODEM_SHAPING3_COEFF14.value
        c15 = model.vars.MODEM_SHAPING3_COEFF15.value

        if shaping_filter_mode == 0:
            shaping_filter_gain = 127.0 / 128.0

        elif shaping_filter_mode == 1:
            shaping_filter_gain = ((c0 + c8 + c0) + (c1 + c7) + (c2 + c6) + (c3 + c5) + (c4 + c4))/5.0/128.0

        else:
            shaping_filter_gain = ((c0 + c7) + (c1 + c6) + (c2 + c5) + (c3 + c4))/4.0/128.0


        model.vars.shaping_filter_gain_iqmod_actual.value = shaping_filter_gain

    def get_shaping_filter(self, model):
        # Construct shaping filter from register settings

        shaping_filter_mode = model.vars.MODEM_CTRL0_SHAPING.value
        c0 = model.vars.MODEM_SHAPING0_COEFF0.value  # SHAPING0
        c1 = model.vars.MODEM_SHAPING0_COEFF1.value
        c2 = model.vars.MODEM_SHAPING0_COEFF2.value
        c3 = model.vars.MODEM_SHAPING0_COEFF3.value
        c4 = model.vars.MODEM_SHAPING1_COEFF4.value  # SHAPING1
        c5 = model.vars.MODEM_SHAPING1_COEFF5.value
        c6 = model.vars.MODEM_SHAPING1_COEFF6.value
        c7 = model.vars.MODEM_SHAPING1_COEFF7.value
        c8 = model.vars.MODEM_SHAPING2_COEFF8.value  # SHAPING2
        c9 = model.vars.MODEM_SHAPING2_COEFF9.value
        c10 = model.vars.MODEM_SHAPING2_COEFF10.value
        c11 = model.vars.MODEM_SHAPING2_COEFF11.value
        c12 = model.vars.MODEM_SHAPING3_COEFF12.value  # SHAPING3
        c13 = model.vars.MODEM_SHAPING3_COEFF13.value
        c14 = model.vars.MODEM_SHAPING3_COEFF14.value
        c15 = model.vars.MODEM_SHAPING3_COEFF15.value

        if shaping_filter_mode == 0:
            sp = np.array([127, 127, 127, 127, 127, 127, 127, 127])

        elif shaping_filter_mode == 1:
            sp = np.array([c0, c1, c2, c3, c4, c5, c6, c7, c8, c7, c6, c5, c4, c3, c2, c1, c0])

        elif shaping_filter_mode == 2:
            sp = np.array([c0, c1, c2, c3, c4, c5, c6, c7, c7, c6, c5, c4, c3, c2, c1, c0])

        else:
            sp = np.array([ c0,  c1,  c2,  c3,  c4,  c5,  c6,  c7,  c8,  c9, c10, c11, c12, c13, c14, c15])

        return sp

    def calc_shaping_misc(self, model):

        self._reg_write(model.vars.MODEM_SHAPING2_COEFF9, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF10, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF11, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF12, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF13, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF14, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF15, 0)
