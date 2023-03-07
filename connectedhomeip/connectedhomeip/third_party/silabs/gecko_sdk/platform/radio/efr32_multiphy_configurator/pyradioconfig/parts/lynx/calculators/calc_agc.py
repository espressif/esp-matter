from pyradioconfig.parts.common.calculators.calc_agc import CALC_AGC
from pyradioconfig.parts.lynx.phys.PHY_internal_base import Phy_Internal_Base_Lynx
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_AGC_lynx(CALC_AGC):

    def buildVariables(self, model):

        #Build all of the variables from inherited CALC_AGC
        super().buildVariables(model)

        #Add variables
        self._addModelActual(model, 'cfloopdel_us', float, ModelVariableFormat.DECIMAL)

    def calc_agc_fastloop_defaults(self, model):

        #We want to set the AGC defaults on Lynx to a set of static values
        #These static values are stored in the PHY file PHY_internal_base.py
        #because we might need to apply the override function where PHYs are inherited

        #Use the values from PHY_internal_base so that we don't have to store the register values in two places

        phy_internal_base = Phy_Internal_Base_Lynx()
        register_dict = phy_internal_base.modified_panther_style_agc_dict
        for register in register_dict:
            register_object = getattr(model.vars,register)
            register_object.value = register_dict[register]

    def calc_agc_cfloopdel_actual(self, model):
        reg = model.vars.AGC_GAINSTEPLIM_CFLOOPDEL.value
        fxo = model.vars.fxo_or_fdec8.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        src1_actual = model.vars.src1_ratio_actual.value
        src2_actual = model.vars.src2_ratio_actual.value

        fsrc2 = fxo / dec0 / dec1 * src1_actual * src2_actual
        model.vars.cfloopdel_us_actual.value = reg / fsrc2 * 1e6

        return

    #WE need to inherit from CALC_AGC otherwise the global functions will run and we will be writing to registers from two places (not allowed)
    #The method overrides below disable the CALC_AGC functions

    def calc_agc_reg(self, model):
        pass

    def calc_agc_misc(self, model):
        pass

    def calc_agc_faststepup_reg(self, model):
        pass

    def calc_agc_cfloopstepmax_reg(self, model):
        pass

    def calc_agc_mode_reg(self, model):
        pass

    def calc_agc_speed_val(self, model):
        pass

    def calc_pwrtarget_val(self, model):
        pass

    def calc_pwrtarget_reg(self, model):
        pass

    def calc_rssiperiod_val(self, model):
        pass

    def calc_rssiperiod_reg(self, model):
        pass

    def calc_hyst_val(self, model):
        pass

    def calc_hyst_reg(self, model):
        pass

    def calc_agcperiod_value(self, model):
        pass

    def calc_agcperiod_reg(self, model):
        pass

    def calc_agcperiod_actual(self, model):
        pass

    def calc_agc_settling_delay(self, model):
        pass

    def calc_cfloopdel_reg(self, model):
        pass

    def calc_agc_clock_cycle(self, model):
        pass

    def calc_agc_delays(self, model):
        pass

    def calc_faststepdown_reg(self, model):
        pass

    def calc_pkdwait_reg(self, model):
        pass

    def calc_fastloopdel_reg(self, model):
        pass

    def calc_lnaslicesdel_reg(self, model):
        pass

    def calc_ifpgadel_reg(self, model):
        pass