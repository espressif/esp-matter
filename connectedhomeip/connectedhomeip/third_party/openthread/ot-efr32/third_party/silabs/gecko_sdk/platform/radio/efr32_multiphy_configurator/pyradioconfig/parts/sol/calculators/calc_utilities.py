from pyradioconfig.parts.ocelot.calculators.calc_utilities import CALC_Utilities_Ocelot
from pycalcmodel.core.variable import ModelVariableFormat
from enum import Enum

class Calc_Utilities_Sol(CALC_Utilities_Ocelot):

    def buildVariables(self, model):
        #Build all variables from the inherited class
        super().buildVariables(model)

        #Now also build the fefilt_selected variable
        self._addModelVariable(model, 'fefilt_selected', str, ModelVariableFormat.ASCII)

    def calc_fefilt_selected(self, model):
        #This method calculates which FEFILT register set should be used based on demod

        #Read in model variables
        demod_select = model.vars.demod_select.value

        #Calculate fefilt_selected based on the demod
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            fefilt_selected = 'FEFILT1'
        else:
            fefilt_selected = 'FEFILT0'

        #Write the model variable
        model.vars.fefilt_selected.value = fefilt_selected

    def get_fefilt_actual(self, model, reg_name_str):
        #This method queries the value of a reg name string based on the FEFILT register set in use

        #Read in model variables
        fefilt_selected = model.vars.fefilt_selected.value

        #Get the register object
        reg_name_complete = fefilt_selected+'_'+reg_name_str
        reg = getattr(model.vars, reg_name_complete)

        #Return the register value
        return reg.value

    def get_fefilt_value_forced(self, model, reg_name_str):
        #This method queries the value of a reg name string based on the FEFILT register set in use

        #Read in model variables
        fefilt_selected = model.vars.fefilt_selected.value

        #Get the register object
        reg_name_complete = fefilt_selected+'_'+reg_name_str
        reg = getattr(model.vars, reg_name_complete)

        #Return the register value
        return reg.value_forced

    def write_fefilt_reg(self, model, reg_name_str, value):
        #This method writes an FEFILT register field based on the FEFILT register set in use

        #Read in model variables
        fefilt_selected = model.vars.fefilt_selected.value

        #Write the register field
        self._reg_write_by_name_concat(model, fefilt_selected, reg_name_str, value)