from pyradioconfig.parts.lynx.calculators.calc_misc import CALC_Misc_lynx
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_Misc_ocelot(CALC_Misc_lynx):

    # def calc_misc(self, model):
    #     """
    #     These aren't really calculating right now.  Just using defaults or forced values.
    #
    #     Args:
    #         model (ModelRoot) : Data model to read and write variables from
    #     """
    def calc_misc_Panther(self, model):
        pass

    def calc_misc_Lynx(self, model):
        pass

    def calc_misc_ifadcenhalfmode(self, model):
        pass

    def calc_misc_sylodivrloadcclksel(self, model):
        pass

    def calc_misc_Ocelot(self, model):

        part_family = model.part_family.lower()

        self._reg_write(model.vars.MODEM_CF_ADCBITORDERI, 0)
        self._reg_write(model.vars.MODEM_CF_ADCBITORDERQ, 0)

        # Digital Gain Control
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DEC0GAIN, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINDOUBLE, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINEN, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINHALF, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINSEL, 0)

        self._reg_write(model.vars.MODEM_CHFCTRL_FWSELCOEFF, 0)
        self._reg_write(model.vars.MODEM_CHFCTRL_FWSWCOEFFEN, 0)
        ## New registers
        # FIXME: how to calculate these?

        self._reg_write(model.vars.MODEM_CHFLATENCYCTRL_CHFLATENCY, 0)
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXMODE, 1)
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXFB, 1)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINIT, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINITVALI, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINITVALQ, 0)
        self._reg_write_default(model.vars.RAC_IFADCTRIM0_IFADCSIDETONEAMP)

        self._reg_write(model.vars.MODEM_CTRL5_BBSS, 0)

    def buildVariables(self, model):
        # : Build variables using base
        super().buildVariables(model)

        """ Coherent Demod Calculations """
        # : Noise floor of channel power accumulator used for coherent demod.
        self._addModelVariable(model, 'chpwraccu_noise', float, ModelVariableFormat.DECIMAL)

    def calc_misc(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        part_family = model.part_family.lower()

        # Legacy Demod Registers
        # FIXME: calculate these

        self._reg_write(model.vars.MODEM_CTRL2_BRDIVA, 0)
        self._reg_write(model.vars.MODEM_CTRL2_BRDIVB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DEVMULA, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DEVMULB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_RATESELMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL2_RXFRCDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL2_SQITHRESH, 0)
        self._reg_write(model.vars.MODEM_CTRL2_TXPINMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL4_ADCSATDENS, 0)
        self._reg_write(model.vars.MODEM_CTRL4_ADCSATLEVEL, 6)
        self._reg_write(model.vars.MODEM_CTRL4_OFFSETPHASESCALING, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PHASECLICKFILT, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTAVG, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTDEB, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTGAIN, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTRST, 0)
        self._reg_write(model.vars.MODEM_CTRL4_SOFTDSSSMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL5_DEMODRAWDATASEL2, 0)
        self._reg_write(model.vars.MODEM_CTRL5_DETDEL, 0)
        self._reg_write(model.vars.MODEM_CTRL5_POEPER, 0)
        self._reg_write(model.vars.MODEM_CTRL5_RESYNCLIMIT, 0)
        self._reg_write(model.vars.MODEM_CTRL6_CODINGB, 0)
        self._reg_write(model.vars.MODEM_CTRL6_CPLXCORREN, 0)
        self._reg_write(model.vars.MODEM_CTRL6_DEMODRESTARTALL, 0)
        self._reg_write(model.vars.MODEM_CTRL6_DSSS3SYMBOLSYNCEN, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PREBASES, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXRESTARTUPONRSSI, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXRESTARTUPONSHORTRSSI, 0)
        self._reg_write(model.vars.MODEM_CTRL6_TXDBPSKINV, 0)
        self._reg_write(model.vars.MODEM_CTRL6_TXDBPSKRAMPEN, 0)
        self._reg_write(model.vars.MODEM_ANARAMPCTRL_VMIDCTRL, 1)
        self._reg_write(model.vars.MODEM_ANARAMPCTRL_MUTEDLY, 0)
        self._reg_write(model.vars.MODEM_ETSTIM_ETSCOUNTEREN, 0)
        self._reg_write(model.vars.MODEM_ETSTIM_ETSTIMVAL, 0)

        self._reg_write(model.vars.MODEM_PRE_DSSSPRE, 0)
        self._reg_write(model.vars.MODEM_PRE_PRESYMB4FSK, 0)
        self._reg_write(model.vars.MODEM_PRE_SYNCSYMB4FSK, 0)
        self._reg_write(model.vars.MODEM_TIMING_FASTRESYNC, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQINVEN, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQSYNC, 0)
        self._reg_write(model.vars.MODEM_TIMING_TSAGCDEL, 0)

        # Long Range registers
        # FIXME: calculate these
        self._reg_write(model.vars.MODEM_LONGRANGE1_LOGICBASEDLRDEMODGATE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LOGICBASEDPUGATE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LRSPIKETHADD, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LRSS, 0)
        self._reg_write(model.vars.MODEM_LRFRC_CI500, 1)
        self._reg_write(model.vars.MODEM_LRFRC_FRCACKTIMETHD, 0)
        self._reg_write(model.vars.MODEM_LRFRC_LRCORRMODE, 1)

        # FIXME: figure out how these AGC registers need to be calculated
        self._reg_write(model.vars.AGC_RSSISTEPTHR_DEMODRESTARTPER, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_DEMODRESTARTTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_NEGSTEPTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_POSSTEPTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_STEPPER, 0)

        # Coherent Demod Registers
        #FIXME: Check with Yan/Per on how to calculate these
        self._reg_write(model.vars.MODEM_COH2_DSAPEAKCHPWRTH, 0)
        self._reg_write(model.vars.MODEM_COH3_COHDSADETDIS, 0)
        self._reg_write(model.vars.MODEM_COH3_DSAPEAKCHPWREN, 0)
        self._reg_write(model.vars.MODEM_COH3_LOGICBASEDCOHDEMODGATE, 0)
        self._reg_write(model.vars.MODEM_COH3_ONEPEAKQUALEN, 0)
        self._reg_write(model.vars.MODEM_COH3_PEAKCHKTIMOUT, 0)

        # Clock-gating register
        self._reg_write(model.vars.MODEM_AUTOCG_AUTOCGEN, 0) #We calculate MODEM_CGCLKSTOP_FORCEOFF in calculator instead
        self._reg_write(model.vars.FRC_AUTOCG_AUTOCGEN, 7)

        # Modem Registers with fixed value
        self._reg_write(model.vars.MODEM_CTRL0_DEMODRAWDATASEL, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DMASEL, 0)
        self._reg_write(model.vars.MODEM_CTRL3_PRSDINEN, 0)
        self._reg_write(model.vars.MODEM_CTRL4_CLKUNDIVREQ, 0)
        self._reg_write(model.vars.MODEM_CTRL3_RAMTESTEN, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_CLKWIDTH, 1)
        self._reg_write(model.vars.MODEM_DIRECTMODE_DMENABLE, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_SYNCASYNC, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_SYNCPREAM, 3)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPACLKAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPAPOWER, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPASELSLICE, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_MANPACLKAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_CTRL0_OOKASYNCPIN, 0)
        self._reg_write(model.vars.MODEM_CTRL0_DUALCORROPTDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL0_FRAMEDETDEL, 0)
        self._reg_write(model.vars.MODEM_CTRL1_SYNC1INV, 0)

        # FRC Registers with fixed value
        self._reg_write(model.vars.FRC_BOICTRL_BOIBITPOS, 0)
        self._reg_write(model.vars.FRC_BOICTRL_BOIEN, 0)
        self._reg_write(model.vars.FRC_BOICTRL_BOIFIELDLOC, 0)
        self._reg_write(model.vars.FRC_BOICTRL_BOIMATCHVAL, 0)
        self._reg_write(model.vars.FRC_CTRL_RATESELECT, 0)
        self._reg_write(model.vars.FRC_CTRL_WAITEOFEN, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_DFLBOIOFFSET, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLBITORDER, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLBITS, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLMINLENGTH, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLMODE, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLOFFSET, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLSHIFT, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_RXSUPRECEPMODE, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_STORESUP, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_SUPSHFFACTOR, 0)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TXSUPPLENOVERIDE, 0)
        self._reg_write(model.vars.FRC_WCNTCMP3_SUPPLENFIELDLOC, 0)
        self._reg_write(model.vars.FRC_WCNTCMP4_SUPPLENGTH, 0)

        # Added new reg-fields related to 15.4 subG OQPSK phys
        self._reg_write(model.vars.MODEM_COH3_COHDSACMPLX, 0)
        self._reg_write(model.vars.MODEM_SYNCPROPERTIES_STATICSYNCTHRESH, 0)

        # Added new reg-fields related to Internal Long Range
        self._reg_write(model.vars.MODEM_PRE_PREWNDERRORS, 0)
        self._reg_write(model.vars.MODEM_CTRL3_TIMINGBASESGAIN, 0)

        #AGC default settings
        self._reg_write(model.vars.AGC_CTRL0_CFLOOPNFADJ, 0)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD1_RFPKDHITHD0, 1)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD1_RFPKDHITHD1, 40)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD0_RFPKDLOWTHD0, 1)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD0_RFPKDLOWTHD1, 10)
        self._reg_write(model.vars.AGC_CTRL6_ENDUALRFPKD, 1)

        # reg-fields to modify sync detection reset behavior PGOCELOT-5282
        self._reg_write(model.vars.MODEM_FRMSCHTIME_PMRSTSYCNEN, 0)
        self._reg_write(model.vars.MODEM_FRMSCHTIME_DSARSTSYCNEN, 0)

    def calc_clkmult_misc(self, model):
        # RAC settings
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVN)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVP)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENREG3)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENBYPASS40MHZ)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTREG3ADJV)
        self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTDRVAMPSEL)

    def calc_aox_misc(self, model):
        # AoX Registers
        # Always force these to zero so all AoX features are disabled.
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTSWENABLE, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_CFGANTPATTEN, 0)

        # Configuration registers for AoX. These registers do not matter as long as the
        # enables (ANTSWENABLE, CFGANTPATTEN) are set to 0.
        # These do not need to be written by calculator as RAIL will override them
        # for AoX. Left in for ocelot to avoid regression risk post validation.
        self._reg_write(model.vars.MODEM_ETSCTRL_CAPTRIG, 0)
        self._reg_write(model.vars.MODEM_ETSCTRL_ETSLOC, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL1_TIMEPERIOD, 436906)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTCOUNT, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTDFLTSEL, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTSWTYPE, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_EXTDSTOPPULSECNT, 30)
        self._reg_write(model.vars.MODEM_ANTSWEND_ANTSWENDTIM, 0)
        self._reg_write(model.vars.MODEM_ANTSWSTART_ANTSWSTARTTIM, 0)
        self._reg_write(model.vars.MODEM_CFGANTPATT_CFGANTPATTVAL, 0)

    def calc_OOKSHPING_misc(self, model):
        self._reg_write(model.vars.MODEM_OOKSHAPING_OOKSHAPINGEN, 0)
        self._reg_write(model.vars.MODEM_OOKSHAPING_OOKSHAPINGLUTSIZE, 0)
        self._reg_write(model.vars.MODEM_OOKSHAPING_OOKSHAPINGSTEP, 0)
