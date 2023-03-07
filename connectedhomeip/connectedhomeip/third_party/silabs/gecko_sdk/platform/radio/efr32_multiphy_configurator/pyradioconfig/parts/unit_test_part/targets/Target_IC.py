from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget


class Target_IC_UnitTest(ITarget):

    _targetName = ITarget.IC_str
    _description = ""
    _store_config_output = False
    _cfg_location = "unit_test_part"
    _tag = ITarget.IC_str

    def target_calculate(self, model):
        pass
