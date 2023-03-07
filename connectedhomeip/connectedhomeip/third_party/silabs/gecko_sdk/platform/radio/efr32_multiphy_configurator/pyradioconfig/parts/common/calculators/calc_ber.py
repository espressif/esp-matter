"""Core CALC_Ber Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum, ModelVariableEmptyValue, ModelVariableInvalidValueType


class CALC_Ber(ICalculator):

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

        # BER Mode Input variable
        self._addModelVariable(model, 'test_per',                   bool, ModelVariableFormat.ASCII, 'Enable to reconfigure for PER testing')
        self._addModelVariable(model, 'test_ber',                   bool, ModelVariableFormat.ASCII, 'Enable to reconfigure for BER testing')


        # Internal enables for different ber functions 
        self._addModelVariable(model, 'ber_force_fdm0',             bool, ModelVariableFormat.ASCII, 'Force fdm0 mode for ber testing')
        self._addModelVariable(model, 'ber_force_sync',             bool, ModelVariableFormat.ASCII, 'Force a specific sync word for ber testing')
        self._addModelVariable(model, 'ber_force_bitorder',         bool, ModelVariableFormat.ASCII, 'Force a specific bit ordering for ber testing')
        self._addModelVariable(model, 'ber_force_whitening',        bool, ModelVariableFormat.ASCII, 'Force a specific de-whitening configuration for ber testing')
        self._addModelVariable(model, 'ber_force_infinite_length',  bool, ModelVariableFormat.ASCII, 'Force infinite length mode for ber testing')
        self._addModelVariable(model, 'ber_force_freq_comp_off',    bool, ModelVariableFormat.ASCII, 'Disable frequency compensation during BER testing')
        #self._addModelVariable(model, '', bool, ModelVariableFormat.ASCII, '')
        


    def calc_ber_forces(self, model):

        if model.vars.test_ber.value == True:
            model.vars.ber_force_fdm0.value = True 
            model.vars.ber_force_sync.value = True 
            model.vars.ber_force_bitorder.value = True 
            model.vars.ber_force_whitening.value = True
            model.vars.ber_force_infinite_length.value = True
            model.vars.ber_force_freq_comp_off.value = True
            
        else:
            model.vars.ber_force_fdm0.value = False
            model.vars.ber_force_sync.value = False               
            model.vars.ber_force_bitorder.value = False
            model.vars.ber_force_whitening.value = False
            model.vars.ber_force_infinite_length.value = False
            model.vars.ber_force_freq_comp_off.value = False

