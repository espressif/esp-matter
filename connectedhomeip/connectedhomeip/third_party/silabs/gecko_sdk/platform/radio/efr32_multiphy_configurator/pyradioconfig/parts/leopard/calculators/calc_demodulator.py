from pyradioconfig.parts.lynx.calculators.calc_demodulator import CALC_Demodulator_lynx
from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum


class calc_demodulator_leopard(CALC_Demodulator_lynx):

    def buildVariables(self, model):

        super().buildVariables(model)

        self._addModelVariable(model, 'adc_rate_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_rate_mode.var_enum = CreateModelVariableEnum(
            enum_name='AdcRateModeEnum',
            enum_desc='ADC Clock Rate Mode',
            member_data=[
                ['FULLRATE', 0, 'Full rate mode'],
                ['HALFRATE', 1, 'Half rate mode'],
                ['EIGHTHRATE', 2, 'Eighth rate mode']
            ])

        self._addModelVariable(model, 'adc_clock_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_clock_mode.var_enum = CreateModelVariableEnum(
            enum_name='AdcClockModeEnum',
            enum_desc='Defines how the ADC clock is derived',
            member_data=[
                ['HFXOMULT', 0, 'Multiply HFXO for ADC Clock'],
                ['VCODIV', 1, 'Divide VCO for ADC Clock'],
            ])

        self._addModelActual(model, 'adc_clock_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_clock_mode_actual.var_enum = model.vars.adc_clock_mode.var_enum

        self._addModelActual(model, 'adc_rate_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.adc_rate_mode_actual.var_enum = model.vars.adc_rate_mode.var_enum

    # Helper calculation for FW calulation of SRC2
    # These are phy specific calculations that is easier to do here
    # than to reverse calculate in firmware
    #
    # Used by rail_scripts -> rfhal_synth.c:SYNTH_CalcSrc2
    def calc_src2_denominator(self, model):
        # Load model variables into local variables
        osr = model.vars.oversampling_rate_actual.value
        datarate = model.vars.baudrate.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if (model.vars.adc_clock_mode.var_enum.HFXOMULT == adc_clock_mode):
            src2_calcDenominator = 0
        else:
            # This does not include the 8x downsampling polyphase filter after IFADC. Handled in RAIL code
            src2_calcDenominator = int(datarate * dec0 * dec1 * dec2 * osr)

        # Load local variables back into model variables
        model.vars.src2_calcDenominator.value = src2_calcDenominator

    def calc_fxo_or_fdec8(self, model):

        #Read in model vars
        # We can not use rx_synth_freq_actual in these calculations due to circular dependency
        flag_using_half_rate_mode = (model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value == 1)
        adc_clock_mode = model.vars.adc_clock_mode.value
        xtal_frequency = model.vars.xtal_frequency.value

        if adc_clock_mode == model.vars.adc_clock_mode.var_enum.VCODIV:
            if flag_using_half_rate_mode:
                fxo_or_fdec8 = model.vars.rx_synth_freq_actual.value / 128.0
            else:
                fxo_or_fdec8 = model.vars.rx_synth_freq_actual.value / 64.0
        else:
            fxo_or_fdec8 = xtal_frequency * 1.0

        #Write the variable
        model.vars.fxo_or_fdec8.value = fxo_or_fdec8