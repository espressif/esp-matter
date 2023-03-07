"""Core AGC Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math

from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat
from pyradioconfig.parts.common.utils.tinynumpy import tinynumpy
from pyradioconfig.parts.common.calculators.calc_shaping import CALC_Shaping

class CALC_Shaping_jumbo(CALC_Shaping):

    def gaussian_shaping_filter(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # for gaussian pulse shapes pulse_shape_parameter holds BT value
        bt = model.vars.shaping_filter_param.value
        # map BT value to standard deviation
        std = 1.05 / bt

        # generate gaussian pulse shape
        w = self.gaussian(17, std)
        # scale for unit DC gain
        w = tinynumpy.divide(w, w.sum())
        # convolve with square wave of oversampling rate width which is 8 for the shaping filter
        f_hack = tinynumpy.convolve(w, tinynumpy.ones((1, 8)).flatten())
        # scale and quantize coefficients
        c_hack = tinynumpy.round_((84.5 * f_hack))
        # keep only first half of 17 coeffs - also skip convolution artifacts
        #coeff = c_hack[4:13]
        coeff = []
        for i in range(4, 14):
            coeff.append(c_hack[i])

        # c8 is not used in gaussian pulse shapes
        coeff[8] = 0.0
        # return coeffs
        return coeff

    def calc_shaping_filter_gain_actual(self, model):
        """
        given shaping filter coefficients and mode calculate shaping filter gain
        Equations from Table 5.26 in EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        shaping_filter_mode = model.vars.MODEM_CTRL0_SHAPING.value
        c0 = model.vars.MODEM_SHAPING0_COEFF0.value
        c1 = model.vars.MODEM_SHAPING0_COEFF1.value
        c2 = model.vars.MODEM_SHAPING0_COEFF2.value
        c3 = model.vars.MODEM_SHAPING0_COEFF3.value
        c4 = model.vars.MODEM_SHAPING1_COEFF4.value
        c5 = model.vars.MODEM_SHAPING1_COEFF5.value
        c6 = model.vars.MODEM_SHAPING1_COEFF6.value
        c7 = model.vars.MODEM_SHAPING1_COEFF7.value
        c8 = model.vars.MODEM_SHAPING2_COEFF8.value
        c9 = model.vars.MODEM_SHAPING2_COEFF9.value
        c10 = model.vars.MODEM_SHAPING2_COEFF10.value
        c11 = model.vars.MODEM_SHAPING2_COEFF11.value
        c12 = model.vars.MODEM_SHAPING3_COEFF12.value
        c13 = model.vars.MODEM_SHAPING3_COEFF13.value
        c14 = model.vars.MODEM_SHAPING3_COEFF14.value
        c15 = model.vars.MODEM_SHAPING3_COEFF15.value
        c16 = model.vars.MODEM_SHAPING4_COEFF16.value
        c17 = model.vars.MODEM_SHAPING4_COEFF17.value
        c18 = model.vars.MODEM_SHAPING4_COEFF18.value
        c19 = model.vars.MODEM_SHAPING4_COEFF19.value
        c20 = model.vars.MODEM_SHAPING4_COEFF20.value
        c21 = model.vars.MODEM_SHAPING4_COEFF21.value
        c22 = model.vars.MODEM_SHAPING5_COEFF22.value
        c23 = model.vars.MODEM_SHAPING5_COEFF23.value
        c24 = model.vars.MODEM_SHAPING5_COEFF24.value
        c25 = model.vars.MODEM_SHAPING5_COEFF25.value
        c26 = model.vars.MODEM_SHAPING5_COEFF26.value
        c27 = model.vars.MODEM_SHAPING5_COEFF27.value
        c28 = model.vars.MODEM_SHAPING5_COEFF28.value
        c29 = model.vars.MODEM_SHAPING5_COEFF29.value
        c30 = model.vars.MODEM_SHAPING6_COEFF30.value
        c31 = model.vars.MODEM_SHAPING6_COEFF31.value
        c32 = model.vars.MODEM_SHAPING6_COEFF32.value
        c33 = model.vars.MODEM_SHAPING6_COEFF33.value
        c34 = model.vars.MODEM_SHAPING6_COEFF34.value
        c35 = model.vars.MODEM_SHAPING6_COEFF35.value
        c36 = model.vars.MODEM_SHAPING6_COEFF36.value
        c37 = model.vars.MODEM_SHAPING6_COEFF37.value
        c38 = model.vars.MODEM_SHAPING6_COEFF38.value
        c39 = model.vars.MODEM_SHAPING6_COEFF39.value


        if shaping_filter_mode == 0:
            shaping_filter_gain = 127

        elif shaping_filter_mode == 1:
            shaping_filter_gain = max(c0+c8+c0, c1+c7, c2+c6, c3+c5, c4+c4)

        elif shaping_filter_mode == 2:
            shaping_filter_gain = max(c0+c7, c1+c6, c2+c5, c3+c4)

        else:
            shaping_filter_gain = max(c0+c8 +c16+c24+c32,
                                      c1+c9 +c17+c25+c33,
                                      c2+c10+c18+c26+c34,
                                      c3+c11+c19+c27+c35,
                                      c4+c12+c20+c28+c36,
                                      c5+c13+c21+c29+c37,
                                      c6+c14+c22+c30+c38,
                                      c7+c15+c23+c31+c39)

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
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF20, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF21, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF22, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF23, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF24, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF25, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF26, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF27, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF28, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF29, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF30, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF31, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF32, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF33, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF34, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF35, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF36, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF37, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF38, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF39, 0)




