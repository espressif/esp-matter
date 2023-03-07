from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget
from os.path import join


class Target_Sim_Margay(ITarget):

    _targetName = "Sim"
    _description = "Supports the wired FPGA and other targets of sim PHYs"
    _store_config_output = True
    _cfg_location = join('target_sim','margay')
    _tag = "SIM"

    def target_calculate(self, model):

        #Always use fixed length in sim results
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.FIXED_LENGTH
