from .pro2plusmodemcalc import Pro2PlusModemCalc
from .pro2_ocelotmodemcalcinputs import Pro2OcelotModemCalcInputs
from .pro2_ocelotdemodulatorcore import Pro2OcelotDemodulatorCore
from .pro2_ocelotmodulatorcore import Pro2OcelotModulatorCore
import math

class Pro2OcelotModemCalc(Pro2PlusModemCalc):

    def __init__(self, inputs=None):
        if inputs is None:
            inputs = Pro2OcelotModemCalcInputs()
        super(Pro2OcelotModemCalc, self).__init__(inputs)

    def _init_demodulator_core(self):
        self.demodulator = Pro2OcelotDemodulatorCore()

    def _init_modulator_core(self, ):
        self.modulator = Pro2OcelotModulatorCore()

    def _get_API_Max_Rb_Error_low_limit(self):
        return 0.02
