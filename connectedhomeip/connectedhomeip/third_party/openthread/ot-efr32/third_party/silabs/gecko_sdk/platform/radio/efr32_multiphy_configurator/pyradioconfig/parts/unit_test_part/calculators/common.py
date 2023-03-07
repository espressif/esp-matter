import inspect
import traceback

from enum import Enum

from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum, ModelVariableEmptyValue, ModelVariableInvalidValueType

from py_2_and_3_compatibility import *

"""
This is common calculators or variables get defined.
"""
"""
Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""
class Common(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 2
        self._minor = 0
        self._patch = 0

    """
    Populates a list of needed variables for this calculator
    """
    def buildVariables(self, modem_model):
        #Add a chip identifier
        self._addModelVariable(modem_model, 'family',                           str,   ModelVariableFormat.ASCII)

        # modem_sync_bits
        self._addModelVariable(modem_model, 'dummy_modem_sync_bits', int, ModelVariableFormat.DECIMAL, 'The modem control sync bits.')

        # modindex
        self._addModelVariable(modem_model, 'dummy_modindex', float, ModelVariableFormat.FLOAT, 'The mod index.')

        # dummy complex
        self._addModelVariable(modem_model, 'dummy_complex', complex, ModelVariableFormat.COMPLEX, 'An unit_test_part complex variable.')

        # dummy long
        self._addModelVariable(modem_model, 'dummy_long', long, ModelVariableFormat.HEX, 'An unit_test_part long variable.')

        # dummy bool
        self._addModelVariable(modem_model, 'dummy_bool', bool, ModelVariableFormat.ASCII, 'An unit_test_part bool variable.')

        # dummy string
        self._addModelVariable(modem_model, 'dummy_str', str, ModelVariableFormat.ASCII, 'An unit_test_part string variable.')

        # modem CF oversampling enum
        var = self._addModelVariable(modem_model, 'dummy_modem_cf_oversampling', Enum, ModelVariableFormat.DECIMAL, 'The center frequency oversampling')

        member_data = [
            ['CF7' , 0, ''],
            ['CF8',  1, ''],
            ['CF12', 2, ''],
            ['CF16', 3, ''],
            ['CF32', 4, ''],
            ['CF0',  5, ''],
        ]
        var.var_enum = CreateModelVariableEnum(
            'ModemCenterFreqOverSample',
            'An example description of the overall enum.',
            member_data)

    def calc_doubleAllOutputs(self, modem_model):
        LogMgr.Debug("Called " + __name__ + "." + inspect.stack()[0][3] + "()")
        for var in modem_model.vars:
            try:
                if not var.value is None and (isinstance(var.value, int) or isinstance(var.value, float)):
                    if var.name not in ['lo_injection_side', 'fec_en']:
                        var.value = var.value * 2
            except ModelVariableEmptyValue:
                pass
            except ModelVariableInvalidValueType:
                pass
            # Raising exceptions we definitely can't handle
            except (SystemExit, SystemError, KeyboardInterrupt, MemoryError):
                raise
            except BaseException:
                traceback.print_exc()
        return

    def calc_someOtherCalcFunction(self, modem_model):
        LogMgr.Debug("Called " + __name__ + "." + inspect.stack()[0][3] + "()")
        return
