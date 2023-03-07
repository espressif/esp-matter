from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

class calc_fpll_ocelot(ICalculator):

    def buildVariables(self, model):

        # Add calculator model variables
        self._addModelVariable(model, 'modem_frequency_hz', float, ModelVariableFormat.DECIMAL, units='Hz', desc='MODEM clock frequency (may differ from xtal rate if FPLL is configured')

    def calc_modem_frequency(self, model):

        #Read in model variables
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value

        #For Ocelot, the hard demod rate is simply the xtal frequency
        modem_frequency_hz = float(xtal_frequency_hz)

        #Write the model variable
        model.vars.modem_frequency_hz.value = modem_frequency_hz