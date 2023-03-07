from pyradioconfig.parts.panther.calculators.calc_dsa import CALC_DSA_panther

class CALC_DSA_lynx(CALC_DSA_panther):

    def calc_dsathd2_vals(self, model):

        dsa_enabled = model.vars.MODEM_DSACTRL_DSAMODE.value

        if dsa_enabled == 0:
            #DSA is not enabled, so we don't care about DSATHD2 values
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_POWABSTHDLOG)
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_JUMPDETEN)
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_FDADJTHD)
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_PMDETPASSTHD)
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_FREQESTTHD)
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_INTERFERDET)
            self._reg_do_not_care(model.vars.MODEM_DSATHD2_PMDETFORCE)


