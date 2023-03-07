from pyradioconfig.parts.ocelot.calculators.calc_misc import CALC_Misc_ocelot

class Calc_Misc_Bobcat(CALC_Misc_ocelot):

    def calc_misc(self, model):

        #Call inherited calc_misc method
        super().calc_misc(model)

        #Add LongRange reg writes
        self._reg_write(model.vars.MODEM_LONGRANGE_LRBLE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRBLEDSA, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRCORRSCHWIN, 0xA)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRCORRTHD, 0x3E8)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRDEC, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRTIMCORRTHD, 0x0FA)

        self._reg_write(model.vars.MODEM_COCURRMODE_CONCURRENT, 0)

    def calc_clkmult_misc(self, model):
        # do not inherit ocelot's clkmult overrides
        # ocelot's dualbrx_adcpll != bobcat's dualbclk_mult
        pass

    def calc_aox_misc(self, model):
        # Always force these to zero so all AoX features are disabled.
        # RAIL will dynamically set these when an AoX  packet is detected.
        # Still write them in the RC so other PHYs are guarenteed to have AoX features disabled and we can
        # exclude ETSLOC, ANTSWTIMSTART, ANTSWTIMSTOP
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTSWENABLE, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_CFGANTPATTEN, 0)

    def calc_OOKSHPING_misc(self, model):
        pass

    def calc_adpc_regs(self, model):
        self._reg_write(model.vars.MODEM_ADPC10_ADBBSSAMPJUMP, 0)
        self._reg_write(model.vars.MODEM_ADPC10_ADBBSSCHANGEEN, 0)
        self._reg_write(model.vars.MODEM_ADPC10_ADBBSSCHGDNTHR, 0)
        self._reg_write(model.vars.MODEM_ADPC10_ADBBSSCHGUPTHR, 0)
        self._reg_write(model.vars.MODEM_ADPC2_ADPCPRETIMINGBAUDS, 0)
        self._reg_write(model.vars.MODEM_ADPC2_ADPCWNDCNTRST, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSAVGEN, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSAVGFREEZE, 1)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSAVGPER, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSAVGWAIT, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSSELWRDATA, 0)
        self._reg_write(model.vars.MODEM_ADPC8_ADPCANTSAMPOFFSET, 0)
        self._reg_write(model.vars.MODEM_ADPC8_ADPCANTSAMPSWITCHWAIT, 1)
        self._reg_write(model.vars.MODEM_ADPC8_ADPCOSR, 5)
        self._reg_write(model.vars.MODEM_ADPC9_ADBBSSAMPAVGLIM, 0)
        self._reg_write(model.vars.MODEM_ADPC9_ADBBSSAMPTHR, 0)
        self._reg_write(model.vars.MODEM_ADPC9_ADBBSSDNTHR, 0)
        self._reg_write(model.vars.MODEM_ADPC9_ADBBSSSYNCEN, 0)
        self._reg_write(model.vars.MODEM_ADPC9_ADBBSSUPTHR, 0)
        self._reg_write(model.vars.MODEM_ADPC2_ADENCORR32, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSEN, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSFILTLENGTH, 4)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSAMPMANT, 0)
        self._reg_write(model.vars.MODEM_ADPC3_ADBBSSAMPEXP, 5)
        self._reg_write(model.vars.MODEM_ADPC4_ADBBSSAMPLUT0, 31)
        self._reg_write(model.vars.MODEM_ADPC4_ADBBSSAMPLUT1, 31)
        self._reg_write(model.vars.MODEM_ADPC4_ADBBSSAMPLUT2, 31)
        self._reg_write(model.vars.MODEM_ADPC4_ADBBSSAMPLUT3, 31)
        self._reg_write(model.vars.MODEM_ADPC5_ADBBSSAMPLUT4, 31)
        self._reg_write(model.vars.MODEM_ADPC5_ADBBSSAMPLUT5, 31)
        self._reg_write(model.vars.MODEM_ADPC5_ADBBSSAMPLUT6, 31)
        self._reg_write(model.vars.MODEM_ADPC5_ADBBSSAMPLUT7, 27)
        self._reg_write(model.vars.MODEM_ADPC6_ADBBSSAMPLUT8, 24)
        self._reg_write(model.vars.MODEM_ADPC6_ADBBSSAMPLUT9, 21)
        self._reg_write(model.vars.MODEM_ADPC6_ADBBSSAMPLUT10, 19)
        self._reg_write(model.vars.MODEM_ADPC6_ADBBSSAMPLUT11, 17)
        self._reg_write(model.vars.MODEM_ADPC7_ADBBSSAMPLUT12, 16)
        self._reg_write(model.vars.MODEM_ADPC7_ADBBSSAMPLUT13, 14)
        self._reg_write(model.vars.MODEM_ADPC7_ADBBSSAMPLUT14, 13)
        self._reg_write(model.vars.MODEM_ADPC7_ADBBSSAMPLUT15, 12)

    def calc_adqual_regs(self, model):
        self._reg_write(model.vars.MODEM_ADQUAL8_ADBAAGCTHR, 0)
        self._reg_write(model.vars.MODEM_ADQUAL8_ADBAMODE, 0)
        self._reg_write(model.vars.MODEM_ADQUAL7_ADBARSSITHR, 1023)
        self._reg_write(model.vars.MODEM_ADQUAL7_ADBARSSIDIFF, 0)
        self._reg_write(model.vars.MODEM_ADQUAL6_ADBACORRTHR, 65535)
        self._reg_write(model.vars.MODEM_ADQUAL6_ADBACORRDIFF, 0)
        self._reg_write(model.vars.MODEM_ADQUAL5_ADDIRECTCORR, 65535)
        self._reg_write(model.vars.MODEM_ADQUAL4_ADAGCGRTHR, 63)
        self._reg_write(model.vars.MODEM_ADQUAL4_ADRSSIGRTHR, 512)
        self._reg_write(model.vars.MODEM_ADQUAL4_ADGRMODE, 0)
        self._reg_write(model.vars.MODEM_ADQUAL8_ADBACORRTHR2, 0xFFFF)

    def calc_adfsm_regs(self, model):
        self._reg_write(model.vars.MODEM_ADFSM0_ADSTATREAD, 0)
        self._reg_write(model.vars.MODEM_ADFSM0_ADSTAT1SEL, 0)
        self._reg_write(model.vars.MODEM_ADFSM0_ADSTAT2SEL, 0)
