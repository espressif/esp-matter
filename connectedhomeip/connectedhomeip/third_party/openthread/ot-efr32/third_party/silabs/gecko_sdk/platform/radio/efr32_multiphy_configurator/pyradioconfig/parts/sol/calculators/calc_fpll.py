from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.parts.ocelot.calculators.calc_fpll import calc_fpll_ocelot
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget
from enum import Enum

class calc_fpll_sol(calc_fpll_ocelot):

    def buildVariables(self, model):

        #Build variables from Ocelot
        super().buildVariables(model)

        #Add calculator model variables
        self._addModelVariable(model, 'fpll_divx', int, ModelVariableFormat.DECIMAL, desc='RFFPLL X divider')
        self._addModelVariable(model, 'fpll_divy', int, ModelVariableFormat.DECIMAL, desc='RFFPLL Y divider')
        self._addModelVariable(model, 'fpll_divn', int, ModelVariableFormat.DECIMAL, desc='RFFPLL N divider')
        self._addModelVariable(model, 'fpll_div_array', int, ModelVariableFormat.DECIMAL, desc='RFFPLL divider array [divx,divy,divn]', is_array=True)
        self._addModelVariable(model, 'fpll_divx_freq', float, ModelVariableFormat.DECIMAL, units='Hz', desc='RFFPLL frequency after DIVX')
        self._addModelVariable(model, 'fpll_divy_freq', float, ModelVariableFormat.DECIMAL, units='Hz', desc='RFFPLL frequency after DIVY')
        self._addModelVariable(model, 'dac_freq_actual', float, ModelVariableFormat.DECIMAL, units='Hz', desc='DAC frequency')
        self._addModelVariable(model, 'fpll_band', Enum , ModelVariableFormat.DECIMAL, desc='Used to optimize the modem and system clock rates based on the RF band selected. The same selection must be made for all PHYs in a configuration.')
        model.vars.fpll_band.var_enum = CreateModelVariableEnum(
            enum_name='FpllBandEnum',
            enum_desc='RF Frequency Planning Band Selection',
            member_data=[
                ['BAND_928', 0, '928 to 960 MHz'],
                ['BAND_9xx', 1, '902 to 928 MHz'],
                ['BAND_896', 2, '896 to 901 MHz'],
                ['BAND_863', 3, '863 to 870 MHz'],
                ['BAND_780', 4, '779 to 787 MHz'],
                ['BAND_470', 5, '470 to 510 MHz'],
                ['BAND_450', 6, 'Below 470 MHz'],
            ])


    def calc_fpll_band(self, model):

        rf_freq = model.vars.base_frequency_hz.value
        fpll_band_enum = model.vars.fpll_band.var_enum

        if (rf_freq < 470e6):
            fpll_band = fpll_band_enum.BAND_450
        elif (rf_freq < 600e6):
            fpll_band = fpll_band_enum.BAND_470
        elif (rf_freq < 800e6):
            fpll_band = fpll_band_enum.BAND_780
        elif (rf_freq < 880e6 ):
            fpll_band = fpll_band_enum.BAND_863
        elif (rf_freq < 902e6):
            fpll_band = fpll_band_enum.BAND_896
        elif (rf_freq < 928e6 ):
            fpll_band = fpll_band_enum.BAND_9xx
        else:
            fpll_band = fpll_band_enum.BAND_928

        # Write the model var
        model.vars.fpll_band.value = fpll_band

    def calc_fpll_dividers(self, model):

        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        fpll_band = model.vars.fpll_band.value
        fpll_band_enum = model.vars.fpll_band.var_enum

        if xtal_frequency_hz == 38e6:
            if fpll_band == fpll_band_enum.BAND_450:
                divx = 7
                divn = 118
                divy = 23
            elif fpll_band == fpll_band_enum.BAND_470:
                divx = 6
                divn = 98
                divy = 19
            elif fpll_band == fpll_band_enum.BAND_780:
                divx = 7
                divn = 118
                divy = 23
            elif fpll_band == fpll_band_enum.BAND_863:
                divx = 6
                divn = 101
                divy = 20
            elif fpll_band == fpll_band_enum.BAND_896:
                divx = 7
                divn = 118
                divy = 23
            elif fpll_band == fpll_band_enum.BAND_9xx:
                divx = 6
                divn = 103
                divy = 20
            else: #BAND_928
                divx = 7
                divn = 118
                divy = 23
        elif xtal_frequency_hz == 38.4e6:
            if fpll_band == fpll_band_enum.BAND_450:
                divx = 6
                divn = 100
                divy = 20
            elif fpll_band == fpll_band_enum.BAND_470:
                divx = 6
                divn = 97
                divy = 19
            elif fpll_band == fpll_band_enum.BAND_780:
                divx = 6
                divn = 100
                divy = 20
            elif fpll_band == fpll_band_enum.BAND_863:
                divx = 6
                divn = 100
                divy = 20
            elif fpll_band == fpll_band_enum.BAND_896:
                divx = 6
                divn = 100
                divy = 20
            elif fpll_band == fpll_band_enum.BAND_9xx:
                divx = 5
                divn = 85
                divy = 17
            else: #BAND_928
                divx = 7
                divn = 117
                divy = 23
        elif xtal_frequency_hz == 39e6:
            if fpll_band == fpll_band_enum.BAND_450:
                divx = 7
                divn = 115
                divy = 23
            elif fpll_band == fpll_band_enum.BAND_470:
                divx = 7
                divn = 111
                divy = 22
            elif fpll_band == fpll_band_enum.BAND_780:
                divx = 7
                divn = 115
                divy = 23
            elif fpll_band == fpll_band_enum.BAND_863:
                divx = 6
                divn = 98
                divy = 20
            elif fpll_band == fpll_band_enum.BAND_896:
                divx = 7
                divn = 115
                divy = 23
            elif fpll_band == fpll_band_enum.BAND_9xx:
                divx = 6
                divn = 100
                divy = 20
            else: #BAND_928
                divx = 6
                divn = 99
                divy = 20
        elif xtal_frequency_hz == 40e6:
            if fpll_band == fpll_band_enum.BAND_450:
                divx = 5
                divn = 80
                divy = 16
            elif fpll_band == fpll_band_enum.BAND_470:
                divx = 6
                divn = 93
                divy = 19
            elif fpll_band == fpll_band_enum.BAND_780:
                divx = 5
                divn = 80
                divy = 16
            elif fpll_band == fpll_band_enum.BAND_863:
                divx = 5
                divn = 80
                divy = 16
            elif fpll_band == fpll_band_enum.BAND_896:
                divx = 5
                divn = 80
                divy = 16
            elif fpll_band == fpll_band_enum.BAND_9xx:
                divx = 5
                divn = 81
                divy = 17
            else: #BAND_928
                divx = 5
                divn = 80
                divy = 16
        else:
            LogMgr.Warning("Unsupported xtal frequency, assuming modem clock equal to HFXO unless explicitly set")
            divx = 5
            divn = 80
            divy = 16

        # Write the model vars
        model.vars.fpll_divx.value = divx
        model.vars.fpll_divy.value = divy
        model.vars.fpll_divn.value = divn

    def calc_fpll_div_array(self, model):

        fpll_divx = model.vars.fpll_divx.value
        fpll_divy = model.vars.fpll_divy.value
        fpll_divn = model.vars.fpll_divn.value

        model.vars.fpll_div_array.value = [fpll_divx, fpll_divy, fpll_divn]

    def calc_fpll_output_freq(self, model):

        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        fpll_divn = model.vars.fpll_divn.value
        fpll_divx = model.vars.fpll_divx.value
        fpll_divy = model.vars.fpll_divy.value
        divr = 2 #Assumption

        # Calculate the VCO frequency given PLL settings
        fvco = xtal_frequency_hz * fpll_divn / divr  # Pull range 1.6 - 2.25GHz

        # Calculate the output rates
        fpll_divx_freq = fvco / fpll_divx
        fpll_divy_freq = fvco / fpll_divy

        #Write the model vars
        model.vars.fpll_divx_freq.value = fpll_divx_freq
        model.vars.fpll_divy_freq.value = fpll_divy_freq

    def calc_modem_frequency(self, model):

        fpll_divx_freq = model.vars.fpll_divx_freq.value
        divxmodemsel = 2  # Assumption

        modem_frequency_hz = fpll_divx_freq / divxmodemsel / 4.0

        model.vars.modem_frequency_hz.value = modem_frequency_hz

    def calc_adc_freq_actual(self, model):

        #Read in model variables
        fpll_divx_freq = model.vars.fpll_divx_freq.value
        divxadcsel = model.vars.RFFPLL0_RFFPLLCTRL1_DIVXADCSEL.value + 1
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        rx_ifadc_en_xo_bypass = model.vars.RAC_IFADCTRIM1_IFADCENXOBYPASS.value
        fadc_target = model.vars.adc_target_freq.value
        fsynth = model.vars.rx_synth_freq_actual.value
        adc_vco_div_actual = model.vars.adc_vco_div_actual.value
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value

        # Calculate ADC rate based on xtal PLL settings
        fadc = fpll_divx_freq / divxadcsel         # ADC clock frequency

        # If using XO bypass, then calculate ADC rate based on xtal only
        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            if 1 == rx_ifadc_en_xo_bypass:
                adc_freq_actual = xtal_frequency_hz
            else:
                adc_freq_actual = int(fadc)
        else:
            adc_freq_actual = int(fsynth / adc_vco_div_actual)

        # Compute the final ADC frequency percent error
        ferror = 100 * (fadc_target - adc_freq_actual) / float(fadc_target)

        # Load local variables back into model variables
        model.vars.adc_freq_actual.value = adc_freq_actual
        model.vars.adc_freq_error.value = ferror

        return

    def calc_dac_freq_actual(self, model):

        # Read in model variables
        fpll_divx_freq = model.vars.fpll_divx_freq.value
        divxdacsel = 8

        # Calculate the DAC frequency given PLL settings
        fdac = fpll_divx_freq / divxdacsel  # DAC clock frequency

        # Write model variables
        model.vars.dac_freq_actual.value = fdac

    def calc_fpll_sim_regs(self, model):

        fpll_divx = model.vars.fpll_divx.value
        fpll_divy = model.vars.fpll_divy.value
        fpll_divn = model.vars.fpll_divn.value

        #Only write these in the case of Sim target
        if model.target == ITarget.SIM_str:
            self._reg_write(model.vars.RFFPLL0_RFFPLLCTRL1_DIVX, fpll_divx)
            self._reg_write(model.vars.RFFPLL0_RFFPLLCTRL1_DIVY, fpll_divy)
            self._reg_write(model.vars.RFFPLL0_RFFPLLCTRL1_DIVN, fpll_divn)
            self._reg_write(model.vars.RFFPLL0_RFFPLLCTRL1_DIVXMODEMSEL, 1) #Corresponds to divider of 2
            self._reg_write(model.vars.RFFPLL0_RFFPLLCTRL1_DIVXDACSEL, 7)  # Corresponds to divider of 8
