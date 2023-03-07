from pyradioconfig.parts.ocelot.calculators.calc_agc import CALC_AGC_ocelot

class Calc_AGC_Bobcat(CALC_AGC_ocelot):

    def calc_agc_misc(self, model):
        super().calc_agc_misc(model)

        #Enable peak detector freeze
        self._reg_write(model.vars.AGC_CTRL4_FRZPKDEN, 1)

        # MCUW_RADIO_CFG-1856: Set PNUPDISTHD to 0 When Using RFPKD to Prevent AGC chattering at after PN exhaustion
        # only acceptable to set to 0 when dual RFPKD THD can release the disgainup
        self._reg_write(model.vars.AGC_CTRL5_PNUPDISTHD, 0)

    def calc_lnamixrfatt_reg(self, model):

        #Values from Sanjeev Suresh

        lnamixrfatt = [0, 1, 2, 4, 6, 8, 11, 15, 18, 24, 31, 33, 47, 63, 110, 127]
        pnindexmax = 16 # Set to 16 to allow for max pn attenuator to handle wifi blockers

        #Write registers
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT1, lnamixrfatt[0])
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT2, lnamixrfatt[1])
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT3, lnamixrfatt[2])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT4, lnamixrfatt[3])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT5, lnamixrfatt[4])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT6, lnamixrfatt[5])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT7, lnamixrfatt[6])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT8, lnamixrfatt[7])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT9, lnamixrfatt[8])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT10, lnamixrfatt[9])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT11, lnamixrfatt[10])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT12, lnamixrfatt[11])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT13, lnamixrfatt[12])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT14, lnamixrfatt[13])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT15, lnamixrfatt[14])
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT16, lnamixrfatt[15])
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT17, 0)
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT18, 0)
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT19, 0)
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT20, 0)
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT21, 0)
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT22, 0)
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT23, 0)
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT24, 0)

        self._reg_write(model.vars.AGC_GAINSTEPLIM1_PNINDEXMAX, pnindexmax)

    def calc_agc_mode_reg(self, model):

        demod_sel = model.vars.demod_select.value
        preamsch = model.vars.MODEM_TRECPMDET_PREAMSCH.value

        # we like AGC to freeze once timing is detected (MODE=1) to avoid AGC changes while receiving data
        # the only exception to this is the case where we are directly searching for the sync word when using Viterbi
        # Or using BLE Longrange demod path,
        # in this case we need to set the MODE=2 to freeze the AGC when frame is detected
        if (demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER):
            if preamsch:
                mode = 1
            else:
                mode = 2
        elif demod_sel == model.vars.demod_select.var_enum.LONGRANGE:
            mode = 2
        else:
            mode = 1

        self._reg_write(model.vars.AGC_CTRL0_MODE, mode)

    def calc_lnamixenrfpkdlothresh_reg(self, model):
        pass

    def calc_gain_schedule_regs(self, model):
        lnaindexborder = 1 # pga, pn, lna backoff for IMD performance
        pgaindexborder = 5 # based on optimization from sweeping pgaindexborder and selecting for blocking/IMD performance

        self._reg_write(model.vars.AGC_GAINRANGE_LNAINDEXBORDER, lnaindexborder)
        self._reg_write(model.vars.AGC_GAINRANGE_PGAINDEXBORDER, pgaindexborder)

    def calc_rssiperiod_val(self, model):
        """calculate RSSIPERIOD as fixed number 3 for now

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        adpc_en = model.vars.MODEM_ADPC1_ADPCEN.value
        antdiv_enable_dual_window = model.vars.antdiv_enable_dual_window

        if adpc_en:
            if antdiv_enable_dual_window:
                rssi_period_val = 4
            else:
                rssi_period_val = 5
        else:
            rssi_period_val = 3

        model.vars.rssi_period.value = rssi_period_val

    def calc_antdiv_debouncecntthd(self, model):
        """
        Number of clock cycles to wait after antenna switching before setting AGC gains.

        Args:
            model:

        Returns:

        """
        antdivmode = int(model.vars.antdivmode.value)

        debouncecntthd = 40 # : Recommended value by He Gou

        # : If antenna diversity mode is 0 (disabled) or 1 (antenna 1 fixed), then debouncecntthd is not used
        if antdivmode == 0 or \
                antdivmode == 1:
            self._reg_do_not_care(model.vars.AGC_ANTDIV_DEBOUNCECNTTHD)
        else:
            self._reg_write(model.vars.AGC_ANTDIV_DEBOUNCECNTTHD, debouncecntthd)