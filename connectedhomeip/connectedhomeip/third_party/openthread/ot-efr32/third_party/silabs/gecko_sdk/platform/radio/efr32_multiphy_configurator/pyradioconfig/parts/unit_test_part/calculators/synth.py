import inspect

from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

"""
Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""
class Synth(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    """
    Populates a list of needed variables for this calculator
    """
    def buildVariables(self, modem_model):
        #self._addModelVariable(modem_model, 'FREQ_FREQ', int, ModelVariableFormat.DECIMAL, 'FREQ_FREQ')
        #self._addModelVariable(modem_model, 'CHCTRL_CHNO', int, ModelVariableFormat.DECIMAL, 'CHCTRL_CHNO')
        #self._addModelVariable(modem_model, 'CHSP_CHSP', int, ModelVariableFormat.DECIMAL, 'CHSP_CHSP')
        #self._addModelVariable(modem_model, 'CALOFFSET_CALOFFSET', int, ModelVariableFormat.DECIMAL, 'CALOFFSET_CALOFFSET')
        #self._addModelVariable(modem_model, 'IFFREQ_IFFREQ', int, ModelVariableFormat.DECIMAL, 'IFFREQ_IFFREQ')
        #self._addModelVariable(modem_model, 'IFFREQ_LOSIDE', int, ModelVariableFormat.DECIMAL, 'IFFREQ_LOSIDE')
        #self._addModelVariable(modem_model, 'synth_res', int, ModelVariableFormat.DECIMAL, 'synth_res')
        #self._addModelVariable(modem_model, 'rx_synth_freq', int, ModelVariableFormat.DECIMAL, 'rx_synth_freq')

        #self._addModelVariable(modem_model, 'tx_synth_freq', int, ModelVariableFormat.DECIMAL, 'tx_synth_freq')
        pass

    """
    This is an example of where we override the default function of ICalculator.getCalculationList()
    Note:  calc_newFunction3 is not returned in the list, this is why we overrode the default function that finds
           all functions that start with 'calc_' .
    """
    def getCalculationList(self):
        return [self.calc_rx_synth_freq_actual, self.calc_tx_synth_freq_actual, self.calc_someNewFunction, self.calc_newFunction2]

    def calc_someNewFunction(self, modem_model):
        LogMgr.Info("Called " + __name__ + "." + inspect.stack()[0][3] + "()")
        LogMgr.Info("I'm some cool calculator calc_newFunction")
        pass

    def calc_newFunction2(self, modem_model):
        LogMgr.Info("Called " + __name__ + "." + inspect.stack()[0][3] + "()")
        LogMgr.Info("I'm some cool calculator calc_newFunction2")
        pass

    def calc_newFunction3(self, modem_model):
        LogMgr.Info("Called " + __name__ + "." + inspect.stack()[0][3] + "()")
        LogMgr.Info("I'm some cool calculator calc_newFunction3")
        pass

    def calc_rx_synth_freq_actual(self, modem_model):
        LogMgr.Info("Called " + __name__ + "." + inspect.stack()[0][3] + "()")

        #chan0_freq = modem_model.vars.FREQ_FREQ.value_actual
        #chno = modem_model.vars.CHCTRL_CHNO.value_actual
        #chan_spacing = modem_model.vars.CHSP_CHSP.value_actual
        #cal_offset = modem_model.vars.CALOFFSET_CALOFFSET.value_actual
        #if_freq = modem_model.vars.IFFREQ_IFFREQ.value_actual
        #loside = modem_model.vars.IFFREQ_LOSIDE.value_actual
        #res = modem_model.vars.synth_res.value_actual

        #if loside:
        #    rx_synth_freq = chan0_freq + chno * chan_spacing + cal_offset - if_freq * res
        #else:
        #    rx_synth_freq = chan0_freq + chno * chan_spacing + cal_offset + if_freq * res

        #modem_model.vars.rx_synth_freq.value = int(round(1000))


    def calc_tx_synth_freq_actual(self, modem_model):
        LogMgr.Info("Called " + __name__ + "." + inspect.stack()[0][3] + "()")

        #chan0_freq = modem_model.vars.FREQ_FREQ.value_actual
        #chno = modem_model.vars.CHCTRL_CHNO.value_actual
        #chan_spacing = modem_model.vars.CHSP_CHSP.value_actual
        #cal_offset = modem_model.vars.CALOFFSET_CALOFFSET.value_actual

        #tx_synth_freq = 1.0 * chan0_freq + chno * chan_spacing + cal_offset

        #modem_model.vars.tx_synth_freq.valuel = int(2000)
        #modem_model.vars.modem_sync_bits.value = int(2001)

