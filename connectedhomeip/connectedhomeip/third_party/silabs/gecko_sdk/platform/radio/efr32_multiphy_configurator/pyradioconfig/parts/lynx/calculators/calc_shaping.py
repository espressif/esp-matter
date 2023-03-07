from pyradioconfig.parts.panther.calculators.calc_shaping import CALC_Shaping_panther

class CALC_Shaping_lynx(CALC_Shaping_panther):

    def calc_shaping_filter_gain_actual(self, model):
        #The Lynx shaping filter registers have changed in the register map, so need to override this function

        shaping_filter_mode = model.vars.MODEM_CTRL0_SHAPING.value
        c0 = model.vars.MODEM_SHAPING0_COEFF0.value #SHAPING0
        c1 = model.vars.MODEM_SHAPING0_COEFF1.value
        c2 = model.vars.MODEM_SHAPING0_COEFF2.value
        c3 = model.vars.MODEM_SHAPING0_COEFF3.value
        c4 = model.vars.MODEM_SHAPING1_COEFF4.value #SHAPING1
        c5 = model.vars.MODEM_SHAPING1_COEFF5.value
        c6 = model.vars.MODEM_SHAPING1_COEFF6.value
        c7 = model.vars.MODEM_SHAPING1_COEFF7.value
        c8 = model.vars.MODEM_SHAPING2_COEFF8.value #SHAPING2
        c9 = model.vars.MODEM_SHAPING2_COEFF9.value
        c10 = model.vars.MODEM_SHAPING2_COEFF10.value
        c11 = model.vars.MODEM_SHAPING2_COEFF11.value
        c12 = model.vars.MODEM_SHAPING3_COEFF12.value #SHAPING3
        c13 = model.vars.MODEM_SHAPING3_COEFF13.value
        c14 = model.vars.MODEM_SHAPING3_COEFF14.value
        c15 = model.vars.MODEM_SHAPING3_COEFF15.value
        c16 = model.vars.MODEM_SHAPING4_COEFF16.value #SHAPING4
        c17 = model.vars.MODEM_SHAPING4_COEFF17.value
        c18 = model.vars.MODEM_SHAPING4_COEFF18.value
        c19 = model.vars.MODEM_SHAPING4_COEFF19.value
        c20 = model.vars.MODEM_SHAPING5_COEFF20.value #SHAPING5
        c21 = model.vars.MODEM_SHAPING5_COEFF21.value
        c22 = model.vars.MODEM_SHAPING5_COEFF22.value
        c23 = model.vars.MODEM_SHAPING5_COEFF23.value
        c24 = model.vars.MODEM_SHAPING6_COEFF24.value #SHAPING6
        c25 = model.vars.MODEM_SHAPING6_COEFF25.value
        c26 = model.vars.MODEM_SHAPING6_COEFF26.value
        c27 = model.vars.MODEM_SHAPING6_COEFF27.value
        c28 = model.vars.MODEM_SHAPING7_COEFF28.value #SHAPING7
        c29 = model.vars.MODEM_SHAPING7_COEFF29.value
        c30 = model.vars.MODEM_SHAPING7_COEFF30.value
        c31 = model.vars.MODEM_SHAPING7_COEFF31.value
        c32 = model.vars.MODEM_SHAPING8_COEFF32.value #SHAPING8
        c33 = model.vars.MODEM_SHAPING8_COEFF33.value
        c34 = model.vars.MODEM_SHAPING8_COEFF34.value
        c35 = model.vars.MODEM_SHAPING8_COEFF35.value
        c36 = model.vars.MODEM_SHAPING9_COEFF36.value #SHAPING9
        c37 = model.vars.MODEM_SHAPING9_COEFF37.value
        c38 = model.vars.MODEM_SHAPING9_COEFF38.value
        c39 = model.vars.MODEM_SHAPING9_COEFF39.value
        c40 = model.vars.MODEM_SHAPING10_COEFF40.value #SHAPING10
        c41 = model.vars.MODEM_SHAPING10_COEFF41.value
        c42 = model.vars.MODEM_SHAPING10_COEFF42.value
        c43 = model.vars.MODEM_SHAPING10_COEFF43.value
        c44 = model.vars.MODEM_SHAPING11_COEFF44.value #SHAPING11
        c45 = model.vars.MODEM_SHAPING11_COEFF45.value
        c46 = model.vars.MODEM_SHAPING11_COEFF46.value
        c47 = model.vars.MODEM_SHAPING11_COEFF47.value

        if shaping_filter_mode == 0:
            shaping_filter_gain = 127

        elif shaping_filter_mode == 1:
            shaping_filter_gain = max(c0+c8+c0, c1+c7, c2+c6, c3+c5, c4+c4)

        elif shaping_filter_mode == 2:
            shaping_filter_gain = max(c0+c7, c1+c6, c2+c5, c3+c4)

        else:
            shaping_filter_gain = max(c0+c8 +c16+c24+c32+c40,
                                      c1+c9 +c17+c25+c33+c41,
                                      c2+c10+c18+c26+c34+c42,
                                      c3+c11+c19+c27+c35+c43,
                                      c4+c12+c20+c28+c36+c44,
                                      c5+c13+c21+c29+c37+c45,
                                      c6+c14+c22+c30+c38+c46,
                                      c7+c15+c23+c31+c39+c47)

        model.vars.shaping_filter_gain_actual.value = int(shaping_filter_gain)

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
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF20, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF21, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF22, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF23, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF24, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF25, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF26, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF27, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF28, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF29, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF30, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF31, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF32, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF33, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF34, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF35, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF36, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF37, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF38, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF39, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF40, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF41, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF42, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF43, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF44, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF45, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF46, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF47, 0)