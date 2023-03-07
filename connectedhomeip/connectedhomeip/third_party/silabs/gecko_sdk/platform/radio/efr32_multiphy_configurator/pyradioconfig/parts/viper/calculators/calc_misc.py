from pyradioconfig.parts.bobcat.calculators.calc_misc import Calc_Misc_Bobcat

class calc_misc_viper(Calc_Misc_Bobcat):
    def calc_misc_Bobcat(self, model):
        pass

    def calc_misc_Viper(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.
       fefilt_selected = model.vars.fefilt_selected.value
        Args:
             model (ModelRoot) : Data model to read and write variables from
        """
        fefilt_selected = model.vars.fefilt_selected.value

        self._reg_write_by_name_concat(model, fefilt_selected, 'CFG_CHFGAINREDUCTION', 0)
        # FIXME: This was not calculated on Ocelot
        #self._reg_write(model.vars.FEFILT0_SRCCHF_SRCDECEN2, 0)                
        
        ## New registers
        # FIXME: how to calculate these?
        self._reg_write_by_name_concat(model, fefilt_selected, 'CFG_CHFCOEFFFWSWSEL', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'CFG_CHFCOEFFFWSWEN', 0)
        
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGMIXCTRL_DIGMIXFBENABLE', 1)

        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMPFILTINIT_DCCOMPINIT', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMPFILTINIT_DCCOMPINITVALI', 0)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMPFILTINIT_DCCOMPINITVALQ', 0)

        self._reg_write(model.vars.MODEM_TXCORR_TXDGAIN6DB, 0)
        self._reg_write(model.vars.MODEM_TXCORR_TXDGAIN, 2)
        self._reg_write(model.vars.MODEM_TXCORR_TXGAINIMB, 0)
        self._reg_write(model.vars.MODEM_TXCORR_TXPHSIMB, 0)
        self._reg_write(model.vars.MODEM_TXCORR_TXFREQCORR, 0)

        self._reg_write(model.vars.MODEM_TXMISC_FORCECLKEN, 0)
        self._reg_write(model.vars.MODEM_TXMISC_TXIQIMBEN, 1)
        self._reg_write(model.vars.MODEM_TXMISC_TXINTPEN, 1)
        self._reg_write(model.vars.MODEM_TXMISC_TXDSEN, 1)
        self._reg_write(model.vars.MODEM_TXMISC_TXIQSWAP, 0)
        self._reg_write(model.vars.MODEM_TXMISC_TXDACFORMAT, 1)
        self._reg_write(model.vars.MODEM_TXMISC_TXDACFORCE, 0)
        self._reg_write(model.vars.MODEM_TXMISC_TXDCI, 0)
        self._reg_write(model.vars.MODEM_TXMISC_TXDCQ, 0)
  #      self._reg_write(model.vars.MODEM_TXMISC_BR2M, 1)
