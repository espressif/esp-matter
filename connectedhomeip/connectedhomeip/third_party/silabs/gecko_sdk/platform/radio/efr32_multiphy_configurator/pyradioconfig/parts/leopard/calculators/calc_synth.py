from pyradioconfig.parts.lynx.calculators.calc_synth import CALC_Synth_lynx


class calc_synth_leopard(CALC_Synth_lynx):

    def calc_adc_clockmode_reg(self,model):
        #This function handles writes to the registers impacting ADC clock mode
        adc_clock_mode = model.vars.adc_clock_mode.value

        if( model.vars.adc_clock_mode.var_enum.HFXOMULT == adc_clock_mode ):
            self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCCLKSEL, 1)
        else:
            self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCCLKSEL, 0)

    def calc_sylodivrloadcclk_reg(self, model):
        adc_rate_mode = model.vars.adc_rate_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_SYTRIM1_SYLODIVRLOADCCLKSEL, reg)

    def calc_ifadcenhalfmode_reg(self, model):

        adc_rate_mode = model.vars.adc_rate_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE, reg)

    def calc_adc_clockmode_actual(self, model):
        #This function calculates the actual value of the adc clock mode based on the register value used
        ifadc_clk_sel = model.vars.RAC_IFADCTRIM0_IFADCCLKSEL.value

        if( 1 == ifadc_clk_sel ):
            model.vars.adc_clock_mode_actual.value = model.vars.adc_clock_mode.var_enum.HFXOMULT
        else:
            model.vars.adc_clock_mode_actual.value = model.vars.adc_clock_mode.var_enum.VCODIV

    def calc_adc_rate_mode_actual(self, model):
        #This function calculates the actual value of the adc rate mode based on the reg value used
        ifadcenhalfmode = model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value

        if ifadcenhalfmode == 1:
            adc_rate_mode_actual = model.vars.adc_rate_mode.var_enum.HALFRATE
        else:
            adc_rate_mode_actual = model.vars.adc_rate_mode.var_enum.FULLRATE

        #Write the variable
        model.vars.adc_rate_mode_actual.value = adc_rate_mode_actual

    def calc_adc_clock_config(self, model):
        # This function calculates both the ADC mode (e.g. fullrate, halfrate, etc) as well as the ADC clock divider path

        # By default always use VCODIV FULLRATE

        adc_rate_mode = model.vars.adc_rate_mode.var_enum.FULLRATE
        adc_clock_mode = model.vars.adc_clock_mode.var_enum.VCODIV

        # Load local variables back into model variables
        model.vars.adc_clock_mode.value = adc_clock_mode
        model.vars.adc_rate_mode.value = adc_rate_mode