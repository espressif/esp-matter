from pyradioconfig.parts.ocelot.calculators.calc_agc import CALC_AGC_ocelot

class Calc_AGC_Sol(CALC_AGC_ocelot):

    def calc_agc_misc(self, model):
        super().calc_agc_misc(model)

        #Enable peak detector freeze
        self._reg_write(model.vars.AGC_CTRL4_FRZPKDEN, 1)

    def calc_settlingindctrl_reg(self, model):

        #For now, always disable
        settlingindctrl_en = 0
        delayperiod = 0

        #Write the registers
        self._reg_write(model.vars.AGC_SETTLINGINDCTRL_EN, settlingindctrl_en)
        self._reg_write(model.vars.AGC_SETTLINGINDCTRL_POSTHD, 1)
        self._reg_write(model.vars.AGC_SETTLINGINDCTRL_NEGTHD, 1)
        self._reg_write(model.vars.AGC_SETTLINGINDPER_SETTLEDPERIOD, 200) # fast loop
        self._reg_write(model.vars.AGC_SETTLINGINDPER_DELAYPERIOD, delayperiod) # fast loop

    def calc_lnamixrfatt_reg(self, model):

        #Load in model variables
        rf_band = model.vars.rf_band.value

        #Calculate the LNAMIXRFATT values based on band
        if rf_band == model.vars.rf_band.var_enum.BAND_915:
            lnamixrfatt = [0, 25, 58, 101, 150, 218, 448, 698, 982, 1456, 1970, 2538, 3322, 4350, 5842, 7514,
                           9882, 12664, 16383, 14042, 14800, 16383, 16383]
            pnindexmax = 19
        elif rf_band == model.vars.rf_band.var_enum.BAND_868:
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
