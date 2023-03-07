import math
from pyradioconfig.parts.panther.calculators.calc_agc import CALC_AGC_panther
from py_2_and_3_compatibility import *
from scipy import interpolate

#This file contains calculations related to the configuring the AGC

class CALC_AGC_ocelot(CALC_AGC_panther):

    def calc_agc_misc(self, model):
        self._reg_write(model.vars.AGC_AGCPERIOD0_SETTLETIMEIF,6)
        self._reg_write(model.vars.AGC_AGCPERIOD0_SETTLETIMERF,14)
        self._reg_write(model.vars.AGC_CTRL0_AGCRST,0)
        self._reg_write(model.vars.AGC_CTRL0_DISCFLOOPADJ,1)
        self._reg_write(model.vars.AGC_CTRL0_DISRESETCHPWR,0)
        self._reg_write(model.vars.AGC_CTRL0_DSADISCFLOOP,0)
        self._reg_write(model.vars.AGC_CTRL0_DISPNDWNCOMP,0)
        self._reg_write(model.vars.AGC_CTRL0_DISPNGAINUP,0)
        self._reg_write(model.vars.AGC_CTRL0_ENRSSIRESET,0)
        self._reg_write(model.vars.AGC_CTRL1_PWRPERIOD, 1)
        self._reg_write(model.vars.AGC_CTRL2_DISRFPKD,0)
        self._reg_write(model.vars.AGC_CTRL2_DMASEL,0)
        self._reg_write(model.vars.AGC_CTRL2_PRSDEBUGEN,0)
        self._reg_write(model.vars.AGC_CTRL2_REHICNTTHD,7)
        self._reg_write(model.vars.AGC_CTRL2_RELBYCHPWR,3)
        self._reg_write(model.vars.AGC_CTRL2_RELOTHD,4)
        self._reg_write(model.vars.AGC_CTRL2_RELTARGETPWR,236)
        self._reg_write(model.vars.AGC_CTRL2_SAFEMODE,0)
        self._reg_write(model.vars.AGC_CTRL2_SAFEMODETHD,3)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEB,1)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEBPRD,40)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEBRST,10)
        self._reg_write(model.vars.AGC_CTRL3_IFPKDDEBTHD,1)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEB,1)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEBPRD,40)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEBRST,10)
        self._reg_write(model.vars.AGC_CTRL3_RFPKDDEBTHD,1)
        self._reg_write(model.vars.AGC_CTRL4_FRZPKDEN,0)
        self._reg_write(model.vars.AGC_CTRL4_PERIODRFPKD,4000)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDCNTEN,1)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDPRDGEAR,4)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDSEL,1)
        self._reg_write(model.vars.AGC_CTRL4_RFPKDSYNCSEL,1)
        self._reg_write(model.vars.AGC_CTRL5_PNUPDISTHD,48)
        self._reg_write(model.vars.AGC_CTRL5_PNUPRELTHD,4)
        self._reg_write(model.vars.AGC_CTRL5_SEQPNUPALLOW,0)
        self._reg_write(model.vars.AGC_CTRL5_SEQRFPKDEN,0)
        self._reg_write(model.vars.AGC_GAINRANGE_GAININCSTEP,1)
        self._reg_write(model.vars.AGC_GAINRANGE_HIPWRTHD,3)
        self._reg_write(model.vars.AGC_GAINRANGE_LATCHEDHISTEP,0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_MAXPWRVAR,0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_TRANRSTAGC,0)
        self._reg_write(model.vars.AGC_LBT_CCARSSIPERIOD,0)
        self._reg_write(model.vars.AGC_LBT_ENCCAGAINREDUCED,0)
        self._reg_write(model.vars.AGC_LBT_ENCCARSSIMAX,0)
        self._reg_write(model.vars.AGC_LBT_ENCCARSSIPERIOD,0)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE1,61)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE2,46)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE3,36)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE4,28)
        self._reg_write(model.vars.AGC_LNAMIXCODE0_LNAMIXSLICE5,21)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE6,17)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE7,12)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE8,10)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE9,6)
        self._reg_write(model.vars.AGC_LNAMIXCODE1_LNAMIXSLICE10,5)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINEN,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINIFPGA,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNA,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINPN,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANIFHILATRST,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANIFLOLATRST,0)
        self._reg_write(model.vars.AGC_MANGAIN_MANRFLATRST,0)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN1,0)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN2,1)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN3,2)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN4,3)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN5,4)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN6,5)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN7,6)
        self._reg_write(model.vars.AGC_PGACODE0_PGAGAIN8,7)
        self._reg_write(model.vars.AGC_PGACODE1_PGAGAIN9,8)
        self._reg_write(model.vars.AGC_PGACODE1_PGAGAIN10,9)
        self._reg_write(model.vars.AGC_PGACODE1_PGAGAIN11,10)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_RSSIFAST,0)
        self._reg_write(model.vars.RAC_PGACTRL_PGAENLATCHI,1)
        self._reg_write(model.vars.RAC_PGACTRL_PGAENLATCHQ,1)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_CFLOOPSTEPMAX, 0)
        self._reg_write(model.vars.AGC_GAINSTEPLIM0_HYST, 0)

        # numbers from Arup's email dated 7/2/19 to Xun

    def calc_lnamixenrfpkdlothresh_reg(self, model):
        self._reg_write(model.vars.RAC_RX_LNAMIXENRFPKDLOTHRESH, 1)

    def calc_lnamixrfatt_reg(self, model):

        #Load in model variables
        rf_band = model.vars.rf_band.value

        #Calculate the LNAMIXRFATT values based on band
        if rf_band == model.vars.rf_band.var_enum.BAND_915 or rf_band == model.vars.rf_band.var_enum.BAND_868:
            lnamixrfatt = [0, 63, 141, 238, 502, 940, 1269, 1942, 2526, 3484, 4547, 6035, 7678, 9973, 12989, 16383,
                           5630, 7160, 9180, 11700, 14800, 16383, 16383]
            pnindexmax = 16
        elif rf_band == model.vars.rf_band.var_enum.BAND_490:
            lnamixrfatt = [0, 20, 45, 78, 125, 175, 246, 477, 726, 1012, 1471, 1983, 2703, 3485, 4543, 5835,
                           7582, 9707, 12672, 16359, 16383, 16383, 16383]
            pnindexmax = 20
        elif rf_band == model.vars.rf_band.var_enum.BAND_434:
            lnamixrfatt = [0, 15, 44, 77, 117, 172, 239, 468, 725, 998, 1469, 1983, 2551, 3491, 4525, 5849,
                           7623, 9910, 12750, 16383, 14800, 16383, 16383]
            pnindexmax = 20
        elif rf_band == model.vars.rf_band.var_enum.BAND_315:
            lnamixrfatt = [0, 12, 29, 52, 78, 111, 156, 207, 420, 502, 750, 1187, 1502, 2012, 2720, 3521,
                           4557, 5870, 7623, 9711, 12719, 16383, 16383]
            pnindexmax = 22
        elif rf_band == model.vars.rf_band.var_enum.BAND_169:
            lnamixrfatt = [0, 13, 36, 63, 99, 141, 199, 412, 502, 757, 1190, 1507, 2029, 2743, 3541, 4601,
                           6055, 7811, 9966, 12964, 16383, 16383, 16383]
            pnindexmax = 21

        else:
            lnamixrfatt = [0, 20, 48, 76, 116, 164, 228, 436, 668, 924, 1210, 1530, 2030, 2720, 3480, 4350,
                           5630, 7160, 9180, 11700, 14800, 16383, 16383]
            pnindexmax = 17

        #Write registers
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT1,lnamixrfatt[0])
        self._reg_write(model.vars.AGC_PNRFATT0_LNAMIXRFATT2, lnamixrfatt[1])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT3, lnamixrfatt[2])
        self._reg_write(model.vars.AGC_PNRFATT1_LNAMIXRFATT4, lnamixrfatt[3])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT5, lnamixrfatt[4])
        self._reg_write(model.vars.AGC_PNRFATT2_LNAMIXRFATT6, lnamixrfatt[5])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT7, lnamixrfatt[6])
        self._reg_write(model.vars.AGC_PNRFATT3_LNAMIXRFATT8, lnamixrfatt[7])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT9, lnamixrfatt[8])
        self._reg_write(model.vars.AGC_PNRFATT4_LNAMIXRFATT10, lnamixrfatt[9])
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT11, lnamixrfatt[10])
        self._reg_write(model.vars.AGC_PNRFATT5_LNAMIXRFATT12, lnamixrfatt[11])
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT13, lnamixrfatt[12])
        self._reg_write(model.vars.AGC_PNRFATT6_LNAMIXRFATT14, lnamixrfatt[13])
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT15, lnamixrfatt[14])
        self._reg_write(model.vars.AGC_PNRFATT7_LNAMIXRFATT16, lnamixrfatt[15])
        self._reg_write(model.vars.AGC_PNRFATT8_LNAMIXRFATT17, lnamixrfatt[16])
        self._reg_write(model.vars.AGC_PNRFATT8_LNAMIXRFATT18, lnamixrfatt[17])
        self._reg_write(model.vars.AGC_PNRFATT9_LNAMIXRFATT19, lnamixrfatt[18])
        self._reg_write(model.vars.AGC_PNRFATT9_LNAMIXRFATT20, lnamixrfatt[19])
        self._reg_write(model.vars.AGC_PNRFATT10_LNAMIXRFATT21, lnamixrfatt[20])
        self._reg_write(model.vars.AGC_PNRFATT10_LNAMIXRFATT22, lnamixrfatt[21])
        self._reg_write(model.vars.AGC_PNRFATT11_LNAMIXRFATT23, lnamixrfatt[22])
        self._reg_write(model.vars.AGC_GAINSTEPLIM1_PNINDEXMAX, pnindexmax)

    def calc_agc_series2(self, model):
        #We need to override this function because the variable fxo_or_fdec8 will go away (replaced by ADC rate calculations)

        mod_format = model.vars.modulation_type.value
        modem_frequency_hz = model.vars.modem_frequency_hz.value
        f_if = model.vars.if_frequency_hz_actual.value

        # period over which we count how many times we tripped the HI threshold - xtal PLL freq because AGC runs at this clock
        if f_if > 0:
            periodhi = int(py2round(modem_frequency_hz / (2 * f_if)))
        else:
            periodhi = 14 # for zero-IF used on FPGA tests fix periodhi to 14

        # Function of PERIODHI and attack vs decay ratio needed. We currently use 3x
        # The scaler 3 could be an input that tunes attack vs decay ratio.
        baudrate = model.vars.baudrate.value

        if (mod_format == model.vars.modulation_type.var_enum.OOK):
            periodlow = int(py2round((modem_frequency_hz/(baudrate * 0.9))))
        else:
            periodlow = 3 * periodhi

        self._reg_write(model.vars.AGC_AGCPERIOD0_PERIODHI, int(round(periodhi)))
        self._reg_write(model.vars.AGC_AGCPERIOD1_PERIODLOW, int(round(periodlow)))

        # % There are many possible ways to handle the table but the simplest would be:
        # % this is based on sine wave tripping N times at different gain settings
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN0, 0)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN1, 1)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN2, 2)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN3, 3)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN4, 4)
        self._reg_write(model.vars.AGC_STEPDWN_STEPDWN5, 5)

        hicntregion0 = int(py2round(0.55 * periodhi))
        hicntregion1 = int(py2round(0.75 * periodhi))
        hicntregion2 = int(py2round(0.85 * periodhi))
        hicntregion3 = int(py2round(0.90 * periodhi))
        hicntregion4 = int(py2round(0.93 * periodhi))

        if (hicntregion0 > 255):
            print("  WARNING: AGC_HICNTREGION_HICNTREGION 0 calculated beyond range: hicntregion0 {}, hicntregion1 {}, AGC_AGCPERIOD0_PERIODHI {}. Saturating value to 255 !".format(hicntregion0, hicntregion1, periodhi))
            hicntregion0 = 255

        if (hicntregion1 > 255):
            print("  WARNING: AGC_HICNTREGION_HICNTREGION 1 calculated beyond range: hicntregion0 {}, hicntregion1 {}, AGC_AGCPERIOD0_PERIODHI {}. Saturating value to 255 !".format(hicntregion0, hicntregion1, periodhi))
            hicntregion1 = 255

        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION0, hicntregion0)
        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION1, hicntregion1)
        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION2, hicntregion2)
        self._reg_write(model.vars.AGC_HICNTREGION0_HICNTREGION3, hicntregion3)
        self._reg_write(model.vars.AGC_HICNTREGION1_HICNTREGION4, hicntregion4)

        # % safe way of setting this.
        self._reg_write(model.vars.AGC_AGCPERIOD0_MAXHICNTTHD, hicntregion4)


    def calc_fastloopdel_reg(self, model):
        #This register does not exist in Ocelot
        pass

    def calc_ifpgadel_reg(self, model):
        #This register does not exist in Ocelot
        pass

    def calc_lnaslicesdel_reg(self, model):
        # This register does not exist in Ocelot
        pass

    def calc_pngainstep_reg(self, model):
        etsi_cat1_compatability = model.vars.etsi_cat1_compatible.value

        # PN gain step size should be 1 for ETSI Cat 1 case and 2 in all other cases
        if (etsi_cat1_compatability != model.vars.etsi_cat1_compatible.var_enum.Normal):
            reg = 1
        else:
            reg = 2

        self._reg_write(model.vars.AGC_GAINRANGE_PNGAINSTEP, reg)

    def calc_agc_settling_delay(self, model):
        pass

    def calc_agcperiod_actual(self, model):

        #Read in the actual AGC period register (different name for Ocelot)
        agc_period_reg = model.vars.AGC_CTRL1_PWRPERIOD.value

        #Calculate the actual period value based on the reg
        val = 2 ** (agc_period_reg * 1.0)

        model.vars.agcperiod_actual.value = val

    def calc_cfloopdel_reg(self, model):
        """calculate AGC settling delay which is basically the group delay of decimation and
        channel filters through the datapath plus processing delays

        calculations are in channel filter clock cycles to directly program into CFLOOPDEL

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        agc_delay = model.vars.agc_settling_delay.value

        cfloopdel = agc_delay

        if cfloopdel > 127:
            cfloopdel = 127

        self._reg_write(model.vars.AGC_GAINSTEPLIM0_CFLOOPDEL, int(math.ceil(cfloopdel)))

    def calc_agc_cfloopdel_actual(self, model):
        reg = model.vars.AGC_GAINSTEPLIM0_CFLOOPDEL.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        src2_actual = model.vars.src2_ratio_actual.value

        fsrc2 = adc_freq_actual / 8.0 / dec0 / dec1 * src2_actual
        model.vars.cfloopdel_us_actual.value = reg / fsrc2 * 1e6

        return

    def calc_agc_sub_reg(self, model):
        demod_sel = model.vars.demod_select.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        baudrate = model.vars.baudrate.value
        src2_actual = model.vars.src2_ratio_actual.value

        osr = adc_freq_actual * src2_actual / (dec0_actual * dec1_actual * 8 * dec2_actual * baudrate)
        # when BCR demod is selected we the OSR can be as high as 127
        # this does not fit into the RXBR register which is used in generating a baudrate clock
        # we switch to using SUB registers instead of RXBR for this purpose when BCR is selected
        if (demod_sel == model.vars.demod_select.var_enum.BCR):
            rawndec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value #Checking inside of the BCR condition to allow inheritance for Bobcat
            subperiod = 1
            dec = pow(2, rawndec)
            osr_bcr = osr / dec

            # write half of sampling rate into SUB registers
            subfrac = osr_bcr / 2

            subint = math.floor(subfrac)

            frac = subfrac - subint

            best_error = 999
            for den in range(1,256):
                num = round(frac * den)
                error = abs(frac - num/den)
                if error < best_error:
                    subden = den
                    subnum = num
                    best_error = error

                if best_error < 1e-3:
                    break
        elif (demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI or
              demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER) and osr >= 8:
            subint = model.vars.rxbrint.value
            subnum = model.vars.rxbrnum.value
            subden = model.vars.rxbrden.value
            subperiod = 1

        else:
            subden = 0
            subint = 0
            subnum = 0
            subperiod = 0

        self._reg_write(model.vars.AGC_CTRL7_SUBDEN,subden)
        self._reg_write(model.vars.AGC_CTRL7_SUBINT,subint)
        self._reg_write(model.vars.AGC_CTRL7_SUBNUM,subnum)
        self._reg_write(model.vars.AGC_CTRL7_SUBPERIOD,subperiod)

        return

    def calc_subfrac_actual(self, model):

        # Load model variables into local variables
        subint_actual = model.vars.AGC_CTRL7_SUBINT.value
        subnum_actual = model.vars.AGC_CTRL7_SUBNUM.value
        subden_actual = model.vars.AGC_CTRL7_SUBDEN.value
        subperiod = model.vars.AGC_CTRL7_SUBPERIOD.value

        # Calculate the sub fraction
        if subperiod and subden_actual > 0:
            subfrac_actual = float(subint_actual + float(subnum_actual) / subden_actual)
        else:
            subfrac_actual = 0.0

        # Load local variables back into model variables
        model.vars.subfrac_actual.value = subfrac_actual


    def calc_agc_mode_reg(self, model):

        demod_sel = model.vars.demod_select.value
        preamsch = model.vars.MODEM_TRECPMDET_PREAMSCH.value

        # we like AGC to freeze once timing is detected (MODE=1) to avoid AGC changes while receiving data
        # the only exception to this is the case where we are directly searching for the sync word when using Viterbi
        # in this case we need to set the MODE=2 to freeze the AGC when frame is detected
        if (demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER):
            if preamsch:
                mode = 1
            else:
                mode = 2
        else:
            mode = 1

        self._reg_write(model.vars.AGC_CTRL0_MODE, mode)

        return

    def calc_agc_decision_matrix_reg(self, model):
        dualrfpkddec_val = 240296
        self._reg_write(model.vars.AGC_CTRL6_DUALRFPKDDEC, dualrfpkddec_val)

    #Removing legacy AGC calculations

    def calc_agc_scheme(self, model):
        pass

    def calc_agc_index_min_atten_reg(self, model):
        pass

    def calc_agc_index_min_degen_reg(self, model):
        pass

    def calc_agc_index_min_pga_reg(self, model):
        pass

    def calc_agc_index_min_slices_reg(self, model):
        pass

    def calc_agcperiod_reg(self, model):
        pass

    def calc_agc_reg(self, model):
        pass

    def calc_agc_cfloopstepmax_reg(self, model):
        pass

    def calc_hyst_reg(self, model):
        pass

    def calc_agc_baudrate_calculation_mode(self, model):
        demod_select = model.vars.demod_select.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : disable RX baudrate calculation used by AGC and instead assume OSR = 2 * RXBRFRAC
            self._reg_write(model.vars.MODEM_CTRL6_RXBRCALCDIS, 1)
        else:
            self._reg_write(model.vars.MODEM_CTRL6_RXBRCALCDIS, 0)

    def calc_dagc_channel_power_accumulator_reg(self, model):
        demod_select = model.vars.demod_select.value
        modtype = model.vars.modulation_type.value
        preamble_pattern_len = model.vars.preamble_pattern_len.value
        sens_calculated = model.vars.sensitivity.value
        target_osr = model.vars.target_osr.value
        agc_period_actual = model.vars.agcperiod_actual.value
        baudrate = model.vars.baudrate.value
        bitrate = model.vars.bitrate.value
        mod_format = model.vars.modulation_type.value

        if hasattr(model.profiles, 'Long_Range'):
            is_long_range = model.profile == model.profiles.Long_Range
        else:
            is_long_range = False

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : accumulate channel power for preamble pattern duration
            # : actual avgwin period is 2^(avgwin+2)*2^pwrperiod_s*OSR
            # : agc_period_actual = 2^pwrperiod
            # : actual avgwin time is 2^(avgwin+2)*agc_period_actual/baudrate
            preamble_time_us = preamble_pattern_len * (1 / bitrate) * 1e6
            target_avgwin_time_us = 4.0 * preamble_time_us
            target_avgwin_period = (target_avgwin_time_us / 1e6) * baudrate
            target_avgwin = round(math.log2(target_avgwin_period / agc_period_actual) - 2)
            if target_avgwin < 0:
                target_avgwin = 0
            avgwin = int(target_avgwin)
            chpwraccudel = 0
        else:
            avgwin = 0
            chpwraccudel = 0

        if demod_select == model.vars.demod_select.var_enum.COHERENT and \
                        mod_format == model.vars.modulation_type.var_enum.OQPSK:
            # : 0 - Channel power is locked when timing is detected
            # : 1 - Channel power is locked when DSA is detected
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRLOCK, 0)
            # : Set to enable automatic restart of channel power - needed to make sure channel power is not dependent on
            # : power of received frames
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRRESTART, 1)
        else:
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRLOCK, 0)
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRRESTART, 0)

        if demod_select == model.vars.demod_select.var_enum.COHERENT and is_long_range:
            self._reg_write(model.vars.MODEM_LONGRANGE1_AVGWIN, avgwin)
            self._reg_write(model.vars.MODEM_LONGRANGE1_CHPWRACCUDEL, chpwraccudel)
        else:
            self._reg_write(model.vars.MODEM_LONGRANGE1_AVGWIN, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE1_CHPWRACCUDEL, 0)

    def calc_dagc_dynamic_bbss_reg(self, model):
        demod_select = model.vars.demod_select.value
        sens_calculated = model.vars.sensitivity.value
        bitrate = model.vars.bitrate.value
        mod_format = model.vars.modulation_type.value

        if hasattr(model.profiles, 'Long_Range'):
            is_long_range = model.profile == model.profiles.Long_Range
        else:
            is_long_range = False

        """ Model calculation range based on DUT power [dBm] """
        min_dut_power = -140 # : This is arbitrarily low power that should be below sensitivity
        max_dut_power = 10 # : This is maximum supported receive power of DUT
        if sens_calculated < min_dut_power:
            min_dut_power = round(sens_calculated) - 10
        dut_power_list = list(range(min_dut_power,max_dut_power,+1))

        """ Model of Q Sample vs. Dut Power """
        estimated_adc_noise_dBm = -162.8694 #  This is measured value
        model_max_Q_list = []
        for dut_power in  dut_power_list:
            # : Model ADC resolution
            adc_enob = (dut_power - estimated_adc_noise_dBm - 1.76) / 6.02

            # : Model analog frontend noise
            minimum_detectable_signal_dBm = -173.9 + 10*math.log10(bitrate) + 4.0
            if bitrate <= 2.8e3:
                analog_noise_bitwidth = 0.1938 * minimum_detectable_signal_dBm + 33.296
            elif bitrate < 67e3: # : TODO change to ADC mode.
                analog_noise_bitwidth = 0.1565 * minimum_detectable_signal_dBm + 27.928
            else:
                analog_noise_bitwidth = 0.1599 * minimum_detectable_signal_dBm + 28.119

            # : Determine signal + noise level
            signal_max_q = math.pow(2,adc_enob) + math.pow(2,analog_noise_bitwidth)
            signal_max_q_bitwidth = math.log2(signal_max_q)
            if signal_max_q_bitwidth > 17:
                signal_max_q_bitwidth = 17

            model_max_Q_list.append(signal_max_q_bitwidth)
                
        # : Calculate bbss shift based on the Q sample model
        bbss_model = []
        for model_max_Q in model_max_Q_list:
            bbss_at_max_Q_calc = round(model_max_Q - 5.5)
            bbss_model.append(bbss_at_max_Q_calc)

        """ Calculate chpwraccumux model """
        model_chpwraccumux_list = []
        for dut_power in dut_power_list:
            calc_chpwraccumux = dut_power + 139.0

            # : Based on measurement, chpwraccumux does not go below 14 due to noise
            if calc_chpwraccumux < 14:
                calc_chpwraccumux = 14
            # : By design, maximum chpwraccumux is 80 due signal power after AGC
            elif calc_chpwraccumux > 80:
                calc_chpwraccumux = 80

            model_chpwraccumux_list.append(calc_chpwraccumux)
        
        """  """
        # : Calculate chpwraccumux at sensitivity. This can be used to calculate noise level
        chpwraccumux_interp_func = interpolate.interp1d(dut_power_list, model_chpwraccumux_list)
        chpwr_accumux_noise = float(chpwraccumux_interp_func(sens_calculated))

        """ """
        bbss_interp_func = interpolate.interp1d(dut_power_list, bbss_model)
        starting_BBSS = math.ceil(bbss_interp_func(sens_calculated))

        """ Calculate BBSS transitions """
        bbss_transition = []
        bbss_transition_threshold = []
        for dut_power_index in range(len(dut_power_list)):
            bbss_val = bbss_model[dut_power_index]
            if bbss_val > starting_BBSS:
                if bbss_model[dut_power_index-1] < bbss_val:
                    bbss_transition.append(int(bbss_val))
                    bbss_transition_threshold.append(int(round(model_chpwraccumux_list[dut_power_index])))

        # : Add two bbss transitions below sensitivity
        bbss_transition.insert(0, starting_BBSS)
        bbss_transition.insert(0, starting_BBSS - 1)
        bbss_transition_threshold.insert(0, int(round(chpwr_accumux_noise-6)))

        # : fill up remaining registers
        max_bbss_shift = 12
        while len(bbss_transition_threshold) < 11:
            bbss_transition_threshold.append(80)
        while len(bbss_transition) < 12:
            bbss_transition.append(max_bbss_shift)

        """ Set calculated value """
        model.vars.chpwraccu_noise.value = chpwr_accumux_noise

        """ Set registers related to dynamic bbss mode"""
        if demod_select == model.vars.demod_select.var_enum.COHERENT and \
                        mod_format == model.vars.modulation_type.var_enum.OQPSK:
            # : Enable dynamic BBSS adjustment
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICBBSSEN, 1)

            # : BBSS hysteresis
            self._reg_write(model.vars.MODEM_LONGRANGE1_HYSVAL, 3)
        else:
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICBBSSEN, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE1_HYSVAL, 0)

        """ Set registers related to dynamic thresholds """
        if demod_select == model.vars.demod_select.var_enum.COHERENT and is_long_range:
            # : BBSS thresholds
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH1, bbss_transition_threshold[0])
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH2, bbss_transition_threshold[1])
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH3, bbss_transition_threshold[2])
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH4, bbss_transition_threshold[3])
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH5, bbss_transition_threshold[4])
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH6, bbss_transition_threshold[5])
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH7, bbss_transition_threshold[6])
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH8, bbss_transition_threshold[7])
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRTH9, bbss_transition_threshold[8])
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRTH10, bbss_transition_threshold[9])
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRTH11, bbss_transition_threshold[10])

            # : BBSS Shifts
            self._reg_limit_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH1, bbss_transition[0], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH2, bbss_transition[1], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH3, bbss_transition[2], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH4, bbss_transition[3], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH5, bbss_transition[4], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH6, bbss_transition[5], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH7, bbss_transition[6], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH8, bbss_transition[7], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH9, bbss_transition[8], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH10, bbss_transition[9], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH11, bbss_transition[10], max_bbss_shift)
            self._reg_limit_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSH12, bbss_transition[11], max_bbss_shift)
        else:
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH1, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH2, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH3, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE2_LRCHPWRTH4, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH5, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH6, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH7, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE3_LRCHPWRTH8, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRTH9, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRTH10, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRTH11, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH1, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH2, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH3, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE4_LRCHPWRSH4, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH5, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH6, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH7, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH8, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH9, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH10, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE5_LRCHPWRSH11, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSH12, 0)

    def calc_antdiv_gainmode_reg(self, model):
        #Antenna diversity has its own AGC gain restore mode that determines how to set the gain index based on previous packet
        antdivmode = model.vars.antdivmode.value

        if antdivmode == model.vars.antdivmode.var_enum.DISABLE or \
                antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            #The value of this field does matter even when antdiv is disabled
            gainmode = 0
        else:
            gainmode = 1  # : Always enable gain restore mode - clear stored gain from last packet when RX is turned off.

        self._reg_write(model.vars.AGC_ANTDIV_GAINMODE, gainmode)

    def calc_antdiv_debouncecntthd(self, model):
        """
        Number of clock cycles to wait after antenna switching before setting AGC gains.

        Args:
            model:

        Returns:

        """
        antdivmode = model.vars.antdivmode.value

        debouncecntthd = 40 # : Recommended value by He Gou

        if antdivmode == model.vars.antdivmode.var_enum.DISABLE or \
                antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            self._reg_do_not_care(model.vars.AGC_ANTDIV_DEBOUNCECNTTHD)
        else:
            self._reg_write(model.vars.AGC_ANTDIV_DEBOUNCECNTTHD, debouncecntthd)


    def calc_gain_schedule_regs(self, model):
        lnaindexborder = 7
        pgaindexborder = 8

        self._reg_write(model.vars.AGC_GAINRANGE_LNAINDEXBORDER, lnaindexborder)
        self._reg_write(model.vars.AGC_GAINRANGE_PGAINDEXBORDER, pgaindexborder)

    def calc_rssiperiod_reg(self, model):

        period = model.vars.rssi_period.value

        if period > 15:
            period = 15

        if period < 0:
            period = 0

        self._reg_write(model.vars.AGC_CTRL1_RSSIPERIOD, period)