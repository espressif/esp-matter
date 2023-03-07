from pyradioconfig.parts.nixi.calculators.calc_misc import CALC_Misc_nixi

class CALC_Misc_panther(CALC_Misc_nixi):

    # def calc_misc(self, model):
    #     """
    #     These aren't really calculating right now.  Just using defaults or forced values.
    #
    #     Args:
    #         model (ModelRoot) : Data model to read and write variables from
    #     """

    def calc_misc_Panther(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.
        Specifically registers that are not handled in inherited CALC_Misc_nixi

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # To address unit test warnings: pyradioconfig/unit_tests/test_generate_header_files.py
        # Field AGC.EN.EN does not have a valid value
        # etc...
        # Fields are being set to the reset value as found in the Register Model SVD at the time of this code being written.
        # There is a risk of the values below becoming stale (no longer equal to the SVD source) should the SVD change in the future.
        #
        # MCUW_RADIO_CFG-734 Panther: PHY/MAC Design requests more registers added (all MODEM, AGC, etc.) with re-writes of reset values

        self._reg_write(model.vars.MODEM_CTRL0_OOKASYNCPIN, 0)
        self._reg_write(model.vars.MODEM_CTRL0_DETDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL0_DEMODRAWDATASEL, 0)
        self._reg_write(model.vars.MODEM_CTRL1_TXSYNC, 0)
        self._reg_write(model.vars.MODEM_CTRL2_SQITHRESH, 0)
        self._reg_write(model.vars.MODEM_CTRL2_RXFRCDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL2_TXPINMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DEVMULA, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DEVMULB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_RATESELMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DMASEL, 0)
        self._reg_write(model.vars.MODEM_CTRL3_PRSDINEN, 0)
        self._reg_write(model.vars.MODEM_CTRL3_RAMTESTEN, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PHASECLICKFILT, 0)
        self._reg_write(model.vars.MODEM_CTRL4_OFFSETPHASESCALING, 0)
        self._reg_write(model.vars.MODEM_CTRL4_CLKUNDIVREQ, 0)
        self._reg_write(model.vars.MODEM_CTRL5_DEMODRAWDATASEL2, 0)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_CTRL5_XINGDEBUG, 0)
        self._reg_write(model.vars.MODEM_CTRL6_CODINGB, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXRESTARTUPONRSSI, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXRESTARTUPONSHORTRSSI, 0)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_CF_RESYNCRESETTIMING, 0)
            self._reg_write(model.vars.MODEM_CF_RESYNCBYPASS, 0)
        self._reg_write(model.vars.MODEM_CF_ADCBITORDERI, 0)
        self._reg_write(model.vars.MODEM_CF_ADCBITORDERQ, 0)
        self._reg_write(model.vars.MODEM_PRE_PRESYMB4FSK, 0)
        self._reg_write(model.vars.MODEM_PRE_SYNCSYMB4FSK, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQINVEN, 0)
        self._reg_write(model.vars.MODEM_DCCOMP_DCESTIEN, 1) # Explicitly enable DC Offset Estimation using tracking filter (non-default value) https://jira.silabs.com/browse/MCUW_RADIO_CFG-747
        self._reg_write(model.vars.MODEM_DCCOMP_DCCOMPEN, 1) # Explicitly enable DC Offset compensation using tracking filter (non-default value) https://jira.silabs.com/browse/MCUW_RADIO_CFG-747
        self._reg_write(model.vars.MODEM_DCCOMP_DCRSTEN, 0)
        self._reg_write(model.vars.MODEM_DCCOMP_DCCOMPFREEZE, 0)
        self._reg_write(model.vars.MODEM_DCCOMP_DCCOMPGEAR, 3)
        self._reg_write(model.vars.MODEM_DCCOMP_DCLIMIT, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINITVALI, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINITVALQ, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINIT, 0)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG4, 0)
            self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG5, 0)
            self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG6, 0)
            self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG7, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_TRANRSTDSA, 0)
        self._reg_write(model.vars.MODEM_AUTOCG_AUTOCGEN, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_DMENABLE, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_SYNCASYNC, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_SYNCPREAM, 3)
        self._reg_write(model.vars.MODEM_DIRECTMODE_CLKWIDTH, 1)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_LONGRANGE_LRCORRTHDDYNEN, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LOGICBASEDPUGATE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LOGICBASEDLRDEMODGATE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRTH11, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSH12, 0)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICPRETHRESH, 0)
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICPRETHRESHSEL, 0)
        self._reg_write(model.vars.MODEM_DSATHD3_SPIKETHDLO, 100)
        self._reg_write(model.vars.MODEM_DSATHD3_UNMODTHDLO, 4)
        self._reg_write(model.vars.MODEM_DSATHD3_FDEVMINTHDLO, 12)
        self._reg_write(model.vars.MODEM_DSATHD3_FDEVMAXTHDLO, 120)
        self._reg_write(model.vars.MODEM_DSATHD4_POWABSTHDLO, 5000)
        self._reg_write(model.vars.MODEM_DSATHD4_ARRTOLERTHD0LO, 2)
        self._reg_write(model.vars.MODEM_DSATHD4_ARRTOLERTHD1LO, 4)
        self._reg_write(model.vars.MODEM_DSATHD4_SWTHD, 0)
        self._reg_write(model.vars.MODEM_VTBLETIMING_VTBLETIMINGSEL, 0)
        self._reg_write(model.vars.MODEM_VTBLETIMING_TIMINGDELAY, 0)
        self._reg_write(model.vars.MODEM_VTBLETIMING_FLENOFF, 0)
        # Panther-specific. Not in Ocelot.
        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            self._reg_write(model.vars.MODEM_MIXCTRL_ANAMIXMODE, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSA_BLEIQDSAEN, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSA_BLEIQDSATH, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSA_BLEIQDSAIIRCOEFPWR, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSA_BLEIQDSADIFFTH1, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_FREQSCALEIQDSA, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_CHPWRFIRAVGEN, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_CHPWRFIRAVGVAL, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_CORRIIRAVGMULFACT, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_BLEIQDSAADDRBIAS, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_BLEIQDSATHCOMB, 0)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_MAXCORRCNTIQDSA, 7)
            self._reg_write(model.vars.MODEM_BLEIQDSAEXT1_IIRRST, 0)
        self._reg_write(model.vars.MODEM_SYNCPROPERTIES_SYNCCORRCLR, 0)
        self._reg_write(model.vars.MODEM_SYNCPROPERTIES_SYNCSECPEAKTH, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINEN, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINSEL, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINDOUBLE, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINHALF, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DEC0GAIN, 0)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_PADEBUG_MANPAAMPCTRL, 0)
            self._reg_write(model.vars.MODEM_PADEBUG_ENMANPAAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_MANPACLKAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPACLKAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPAPOWER, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPASELSLICE, 0)
        # Panther-specific. Not in Lynx.
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.MODEM_PADEBUG_ENMANPATRIMPASLICE0DBM, 0)

        # Default (reset value) for FRC_CTRL_RATESELECT, this gets overriden for BLE LR 500 kbps
        self._reg_write(model.vars.FRC_CTRL_RATESELECT, 0)

        # Panther-specific. Not in Lynx. Renamed to IFADCTRIM0
        if model.part_family.lower() in ["panther"]:
            self._reg_write(model.vars.RAC_IFADCTRIM_IFADCOTAST1CURRENT, 0) # Reduce RFVDD current (non-default value) https://jira.silabs.com/browse/MCUW_RADIO_CFG-840
            self._reg_write(model.vars.RAC_IFADCTRIM_IFADCOTAST2CURRENT, 0) # Reduce RFVDD current (non-default value) https://jira.silabs.com/browse/MCUW_RADIO_CFG-840
        self._reg_write(model.vars.RAC_LNAMIXTRIM0_LNAMIXLOWCUR, 1) # Reduce RFVDD current (non-default value) https://jira.silabs.com/browse/MCUW_RADIO_CFG-840

    def calc_dynamic_bbss_sw_en(self, model):
        # Removed because these registers no longer appear in the Profile Outputs
        pass

    def calc_dynamic_slicer_sw_en(self, model):
        # We will not use dynamic slicing with series 2
        model.vars.dynamic_slicer_enabled.value = False

    def calc_dynamic_slicer_values(self, model):
        # Removed as we will not use dynamic slicing with series 2
        pass