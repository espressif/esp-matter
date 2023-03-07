from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget
from os.path import join

class Target_FPGA_Ocelot(ITarget):

    _targetName = "FPGA"
    _description = "Supports the OTA FPGA"
    _store_config_output = False
    _cfg_location = join('target_fpga','ocelot')
    _tag = "FPGA"

    def target_calculate(self, model):

        #FPGA can only run in full rate mode
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.FULLRATE
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        #Use Zero-IF
        model.vars.if_frequency_hz.value_forced = 0

        #Disable IQ swap
        model.vars.MODEM_MIXCTRL_DIGIQSWAPEN.value_forced = 0