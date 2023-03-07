from pyradioconfig.parts.ocelot.calculators.calc_misc import CALC_Misc_ocelot

class Calc_Misc_Sol(CALC_Misc_ocelot):

    def calc_misc_Ocelot(self, model):
        pass

    def calc_misc_Sol(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.
       fefilt_selected = model.vars.fefilt_selected.value
        Args:
             model (ModelRoot) : Data model to read and write variables from
        """
        fefilt_selected = model.vars.fefilt_selected.value

        # FIXME: This was not calculated on Ocelot
        #self._reg_write(model.vars.FEFILT0_CF_CFOSR, 0)
        
        self._reg_write_by_name_concat(model, fefilt_selected, 'CF_ADCBITORDERI', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'CF_ADCBITORDERQ', 0)
        
        # FIXME: This was not calculated on Ocelot
        #self._reg_write(model.vars.FEFILT0_SRCCHF_SRCDECEN2, 0)                
        
        # Digital Gain Control
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGIGAINCTRL_DEC0GAIN', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGIGAINCTRL_DIGIGAINEN', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGIGAINCTRL_DIGIGAIN', 0)

        ## New registers
        # FIXME: how to calculate these?
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCTRL_FWSELCOEFF', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCTRL_FWSWCOEFFEN', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCTRL_CHFLATENCY', 0)


        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGMIXCTRL_DIGMIXFBENABLE', 1)


        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMPFILTINIT_DCCOMPINIT', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMPFILTINIT_DCCOMPINITVALI', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMPFILTINIT_DCCOMPINITVALQ', 0)

        self._reg_write(model.vars.MODEM_PHDMODANTDIV_ANTDECRSTBYP, 1)
        self._reg_write(model.vars.MODEM_PHDMODANTDIV_RECHKCORREN,  0)
        self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIPTHDSEL,   1)

        self._reg_write(model.vars.RAC_PATRIM6_TXTRIMBBREGFB, 8)

        # For now, set to max amplitude of 3
        # See findings in https://jira.silabs.com/browse/MCUW_RADIO_CFG-1800
        self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCSIDETONEAMP, 3)
