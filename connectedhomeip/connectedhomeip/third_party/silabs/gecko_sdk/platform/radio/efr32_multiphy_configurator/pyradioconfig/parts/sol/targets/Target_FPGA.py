from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget
from os.path import join

class Target_FPGA_Sol(ITarget):

    _targetName = "FPGA"
    _description = "Supports the OTA FPGA"
    _store_config_output = False
    _cfg_location = join('target_fpga','sol')
    _tag = "FPGA"

    def target_calculate(self, model):

        #FPGA can only run in eighthrate, halfrate, fullrate
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        #Use Zero-IF
        model.vars.if_frequency_hz.value_forced = 0

        #Disable IQ swap
        model.vars.FEFILT0_DIGMIXCTRL_DIGIQSWAPEN.value_forced = 0
        model.vars.FEFILT1_DIGMIXCTRL_DIGIQSWAPEN.value_forced = 0

        #38.4 MHz XO Frequency
        model.vars.xtal_frequency_hz.value_forced = int(38.4e6)

        #Use a FPLL at same rate as HFXO
        model.vars.fpll_divx.value_forced = 5
        model.vars.fpll_divy.value_forced = 16
        model.vars.fpll_divn.value_forced = 80

        #Disable AFC (right now the OTA FPGA incorrectly adjusts the synth even when we are using DIGMIXFB)
        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        model.vars.afc_run_mode.value_forced = model.vars.afc_run_mode.var_enum.CONTINUOUS
