"""Core AGC Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math

from enum import Enum

from pycalcmodel.py2_and_3_compatibility import py2round
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat
from pyradioconfig.parts.common.utils.tinynumpy import tinynumpy

class CALC_Shaping(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Insert variables used by this block here

        var = self._addModelVariable(model, 'shaping_filter', Enum, ModelVariableFormat.DECIMAL, desc='Defines the shaping filter to be used in the TX side.')
        member_data = [
            ['NONE',  0, 'No shaping filter is applied'],
            ['Gaussian',  1, 'Gaussian shaping filter - BT is defined at entry field Shaping Filter Parameter'],
            ['Raised_Cosine',  2, 'Raised Cosine shaping filter - R is defined at entry field Shaping Filter Parameter'],
            ['Root_Raised_Cosine',  3, 'Filter for 802.15.4 250 kbps DSSS OQPSK PHY'],
            ['Custom_OQPSK',  4, 'Filter for 802.15.4 250 kbps DSSS OQPSK PHY'],
            ['Custom_PSK', 5, 'Legacy 3rd party MSK filter']
        ]
        var.var_enum = CreateModelVariableEnum(
            'ShapingFilterEnum',
            'Defines the shaping filter to be used in the TX side.',
            member_data)



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
        c_hack = tinynumpy.round_(85 * f_hack)
        # keep only first half of 17 coeffs - also skip convolution artifacts
        #coeff = c_hack[4:13]
        coeff = []
        for i in range(4,14):
            coeff.append(c_hack[i])

        # c8 is not used in gaussian pulse shapes
        coeff[8] = 0
        # return coeffs
        return coeff

    @staticmethod
    def gaussian(M, std, sym=True):
        r"""Return a Gaussian window.

        Parameters
        ----------
        M : int
            Number of points in the output window. If zero or less, an empty
            array is returned.
        std : float
            The standard deviation, sigma.
        sym : bool, optional
            When True (default), generates a symmetric window, for use in filter
            design.
            When False, generates a periodic window, for use in spectral analysis.

        Returns
        -------
        w : ndarray
            The window, with the maximum value normalized to 1 (though the value 1
            does not appear if `M` is even and `sym` is True).

        Notes
        -----
        The Gaussian window is defined as

        .. math::  w(n) = e^{ -\frac{1}{2}\left(\frac{n}{\sigma}\right)^2 }
        """
        if M < 1:
            return tinynumpy.array([])
        if M == 1:
            return tinynumpy.ones(1, 'd')
        odd = M % 2
        if not sym and not odd:
            M = M + 1
        n = tinynumpy.arange(0, M) - (M - 1.0) / 2.0
        sig2 = 2 * std * std
        w = tinynumpy.exp(-n ** int(2) * (1/sig2))
        if not sym and not odd:
            w = w[:-1]
        return w

    def raised_cosine_filter(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # for raised cosine pulse shapes pulse_shape_parameter holds roll off factor value
        bt = model.vars.shaping_filter_param.value
        # create empty coefficient array
        #h = tinynumpy.zeros((17,), dtype=float)
        h = [0.0] * 17
        # for each coeff to be calculated
        for x in tinynumpy.arange(0, 17):
            # get time index
            x = int(x)
            t = (x - 8) / 8.0
            # handle special cases and calculate the coeffs
            if t == 0.0:
                h[x] = 127
            elif bt != 0 and t == 1/(2*bt):
                h[x] = py2round(127*(math.pi/4)*(math.sin(math.pi*t)/(math.pi*t)))
            elif bt != 0 and t == -1/(2*bt):
                h[x] = py2round(127*(math.pi/4)*(math.sin(math.pi*t)/(math.pi*t)))
            else:
                h[x] = py2round(127*(math.sin(math.pi*t)/(math.pi*t))*(math.cos(math.pi*bt*t)/(1-(((2*bt*t))*((2*bt*t))))))
        # keep only first half of coeffs
        coeff = h[0:9]

        return coeff

    def root_raised_cosine_filter(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # for raised cosine pulse shapes pulse_shape_parameter holds roll off factor value
        bt = model.vars.shaping_filter_param.value
        pi = math.pi
        # create empty coefficient array
        #h = tinynumpy.zeros((17,), dtype=float)
        h = [0.0] * 17
        # for each coeff to be calculated
        for x in tinynumpy.arange(0, 17):
            # get time index
            x = int(x)
            t = (x - 8) / 8.0
            # handle special cases and calculate the coeffs
            if t == 0.0:
                h[x] = (1-bt)+4*bt/pi
            elif bt != 0 and t == 1/(4*bt):
                h[x] = bt/math.sqrt(2) * ((1+2/pi)*math.sin(pi/(4*bt))+(1-2/pi)*math.cos(pi/(4*bt)))
            elif bt != 0 and t == -1/(4*bt):
                h[x] = bt/math.sqrt(2) * ((1+2/pi)*math.sin(pi/(4*bt))+(1-2/pi)*math.cos(pi/(4*bt)))
            else:
                h[x] = (math.sin(pi*t*(1-bt)) + 4*bt*t*math.cos(pi*t*(1+bt)))/( pi*t*(1-(4*bt*t)*(4*bt*t)) )

        # scale so that the peak tap is 127
        peak = max(h)
        for x in tinynumpy.arange(0, 17):
            x = int(x)
            h[x] = py2round(h[x] / peak * 127 - 0.5)
            if h[x] < 0:
                # Guner Arslan added a comment - 3 days ago Looks like we have couple issues in the
                # root_raised_cosine_filter(self, model) function: the shaping filter coefficients (
                # MODEM.SHAPINGx.COEFFy) are unsigned registers but root raised cosine filters can have negative
                # coefficients at the edges. So technically the HW does not support generic root raised cosine filters.
                #
                # We have a number of PHYs that use root raised cosine but all of them use BT = 0 in which case all coeffs end up greater or equal to zero.
                #
                # For BT > 0 The negative coeffs will be very small at the edges I think we should be able to get away
                # with setting them to zero. So let's just set all negative coeffs to zero before writing to the
                # registers.
                #
                # There is also a rounding function missing:  line 191 should read something like: h[x] = py2round(h[x] /
                # peak * 127)
                #
                # In Lynx we converted some of the COEFFx registers to signed to be able to support filters like the
                # root raised cosine.

                h[x] = 0

        # keep only needed half of symmetric coeffs
        coeff = h[0:9]

        return coeff

    # TODO: add shaping filters for BT = 0.1 to 1.0 in 0.1 steps for now
    #       or alternatively we can implement filter design in Python
    def calc_shaping_reg(self, model):
        """
        given shaping filter input parameter set shaping filter coeffs and type

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        shaping_filter_option = model.vars.shaping_filter.value
        c = tinynumpy.array([0, 0, 0, 0, 0, 0, 0, 0, 0])

        if shaping_filter_option.value == model.vars.shaping_filter.var_enum.NONE.value:
            shaping = 0
    
        elif shaping_filter_option.value == model.vars.shaping_filter.var_enum.Gaussian.value:
            c = self.gaussian_shaping_filter(model)
            shaping = 2
    
        elif shaping_filter_option.value == model.vars.shaping_filter.var_enum.Custom_OQPSK.value:
            # Not sure what filter this is
            c[0] = c[1] = 1
            c[2] = 16
            c[3] = 48
            c[4] = 80
            c[5] = 112
            c[6] = c[7] = 127
            c[8] = 0
            shaping = 2

        elif shaping_filter_option.value == model.vars.shaping_filter.var_enum.Raised_Cosine.value:
            c = self.raised_cosine_filter(model)
            shaping = 1

        elif shaping_filter_option.value == model.vars.shaping_filter.var_enum.Root_Raised_Cosine.value:
            c = self.root_raised_cosine_filter(model)
            shaping = 1

        # filter derived from Imagotag legacy PHY transmit capture
        elif shaping_filter_option.value == model.vars.shaping_filter.var_enum.Custom_PSK.value:
            c[0] = 51
            c[1] = 117
            c[2] = 96
            c[3] = 53
            c[4] = 20
            c[5] = 2
            c[6] = 0
            c[7] = 0
            c[8] = 0    # not used
            shaping = 3
        else:
            raise CalculationException("ERROR: Unrecognized shaping filter option")

        self._reg_write(model.vars.MODEM_SHAPING0_COEFF0,  int(c[0]))
        self._reg_write(model.vars.MODEM_SHAPING0_COEFF1,  int(c[1]))
        self._reg_write(model.vars.MODEM_SHAPING0_COEFF2,  int(c[2]))
        self._reg_write(model.vars.MODEM_SHAPING0_COEFF3,  int(c[3]))
        self._reg_write(model.vars.MODEM_SHAPING1_COEFF4,  int(c[4]))
        self._reg_write(model.vars.MODEM_SHAPING1_COEFF5,  int(c[5]))
        self._reg_write(model.vars.MODEM_SHAPING1_COEFF6,  int(c[6]))
        self._reg_write(model.vars.MODEM_SHAPING1_COEFF7,  int(c[7]))
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF8,  int(c[8]))
        self._reg_write(model.vars.MODEM_CTRL0_SHAPING,  shaping)


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
    
        if shaping_filter_mode == 0:
            shaping_filter_gain = 127
    
        elif shaping_filter_mode == 1:
            shaping_filter_gain = max(c0+c8+c0, c1+c7, c2+c6, c3+c5, c4+c4)
    
        elif shaping_filter_mode == 2:
            shaping_filter_gain = max(c0+c7, c1+c6, c2+c5, c3+c4)
    
        else:
            shaping_filter_gain = max(c0, c1, c2, c3, c4, c5, c6, c7)
    
        model.vars.shaping_filter_gain_actual.value = int(shaping_filter_gain)




