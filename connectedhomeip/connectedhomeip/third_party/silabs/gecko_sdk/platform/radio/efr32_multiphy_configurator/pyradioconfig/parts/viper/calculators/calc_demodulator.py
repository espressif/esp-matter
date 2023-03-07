from pyradioconfig.parts.bobcat.calculators.calc_demodulator import Calc_Demodulator_Bobcat
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.parts.viper.calculators.calc_utilities import Calc_Utilities_Viper
from enum import Enum
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from math import *
from py_2_and_3_compatibility import *
from pyradioconfig.parts.viper.calculators.calc_shaping import Calc_Shaping_Viper
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
import numpy as np
import numpy.matlib
from scipy import signal as sp

class Calc_Demodulator_Viper(Calc_Demodulator_Bobcat):

    # override inhereited variable defining threshold for _channel_filter_clocks_valid
    chf_required_clks_per_sample = 3  # fixed in PGVIPER-248

    def calc_dec1_reg(self, model):
        #This function writes the register for dec1

        #Load model variables into local variables
        dec1_value = model.vars.dec1.value
        fefilt_selected = model.vars.fefilt_selected.value

        #Dec1 register is simply one less than the value
        dec1_reg = dec1_value - 1

        #Write the registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'CFG_DEC1', dec1_reg)

    def calc_dec0_actual(self,model):
        #This function calculates the actual dec0 based on the register value
        model.vars.dec0_actual.value = 4

    def calc_dec1_actual(self, model):
        #This function calculates the actual dec1 based on the register value

        #Load model variables into local variables
        dec1_reg = Calc_Utilities_Viper().get_fefilt_actual(model, 'CFG_DEC1')

        #Dec1 value is simply one more than the register setting
        dec1_value = dec1_reg + 1

        #Load local variables back into model variables
        model.vars.dec1_actual.value = dec1_value

    def calc_dec2_reg(self,model):
        #This function calculates the dec2 register value

        #Load model variables into local variables
        dec2_value = model.vars.dec2.value

        #The dec2 register is one less than the decimation value
        dec2_reg = dec2_value - 1

        #Write to register
        self._reg_write(model.vars.MODEM_CTRL5_DEC2, dec2_reg)

    def calc_src2_actual(self,model):
        #This function calculates the actual SRC2 ratio from the register value

        #Load model variables into local variables
        src2_en_reg = Calc_Utilities_Viper().get_fefilt_actual(model, 'SRC2_SRC2ENABLE')
        src2_reg = Calc_Utilities_Viper().get_fefilt_actual(model, 'SRC2_SRC2RATIO')

        if src2_en_reg:
            #The src2 ratio is simply 16384 divided by the register value
            src2_ratio_actual = 16384.0 / src2_reg
        else:
            src2_ratio_actual = 1.0

        #Load local variables back into model variables
        model.vars.src2_ratio_actual.value = src2_ratio_actual


    # Helper calculation for FW calulation of SRC2
    # These are phy specific calculations that is easier to do here
    # than to reverse calculate in firmware
    #
    # Used by rail_scripts -> rfhal_synth.c:SYNTH_CalcSrc2
    def calc_src2_denominator(self, model):
        # Load model variables into local variables
        osr = model.vars.oversampling_rate_actual.value
        datarate = model.vars.baudrate.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if (model.vars.adc_clock_mode.var_enum.HFXOMULT == adc_clock_mode):
            src2_calcDenominator = 0
        else:
            # This does not include the 8x downsampling polyphase filter after IFADC. Handled in RAIL code
            src2_calcDenominator = int(datarate * dec0 * dec1 * dec2 * osr)

        # Load local variables back into model variables
        model.vars.src2_calcDenominator.value = src2_calcDenominator

    def calc_rx_restart_reg(self, model):
        """
        Calculate collision restart control registers.

        Args:
            model:

        Returns:

        """

        self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTB4PREDET)
        self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMATAP)
        self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMALATCHSEL)
        self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMACOMPENSEL)
        self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTMATHRESHOLD)
        self._reg_do_not_care(model.vars.MODEM_RXRESTART_RXRESTARTUPONMARSSI)

    def calc_chfilt_reg(self,model):
        #This function calculates the channel filter registers

        #Load model variables into local variables
        bwsel = model.vars.bwsel.value
        fefilt_selected = model.vars.fefilt_selected.value
        chfgainreduction_reg = Calc_Utilities_Viper().get_fefilt_actual(model, 'CFG_CHFGAINREDUCTION')

        if (chfgainreduction_reg == 1):
            coeffs = [13,   31,   29,   -1,   -52,   -94,   -89,   -12,   136,   313,   458,   514]
        else :
            coeffs = self.return_coeffs(bwsel)

        csdcoeffs = [0,0,0,0,0,0,0,0,0,0,0,0]
        csdcoeffs_sign = [0,0,0,0,0,0,0,0,0,0,0,0]

#csdcoeffs_bit_widths = [6,8,8,8,8,9,9,9,10,10,11,12] CSD coefficients bit width is smaller due to it's unsigned
        bit_widths    = [7,8,8,9,9,9,9,10,10,11,12,12]

        # replace negative numbers with 2s complement
        for i in range(12):
            csdcoeffs[i], csdcoeffs_sign[i] = self.return_csd(coeffs[i], bit_widths[i])

        # Write registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00_SET0CSDCOEFF0', csdcoeffs[0])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00_SET0CSDCOEFF1', csdcoeffs[1])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00_SET0CSDCOEFF2', csdcoeffs[2])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00_SET0CSDCOEFF3', csdcoeffs[3])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01_SET0CSDCOEFF4', csdcoeffs[4])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01_SET0CSDCOEFF5', csdcoeffs[5])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01_SET0CSDCOEFF6', csdcoeffs[6])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE02_SET0CSDCOEFF7', csdcoeffs[7])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE02_SET0CSDCOEFF8', csdcoeffs[8])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE02_SET0CSDCOEFF9', csdcoeffs[9])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE03_SET0CSDCOEFF10', csdcoeffs[10])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE03_SET0CSDCOEFF11', csdcoeffs[11])
        
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF0S', csdcoeffs_sign[0])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF1S', csdcoeffs_sign[1])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF2S', csdcoeffs_sign[2])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF3S', csdcoeffs_sign[3])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF4S', csdcoeffs_sign[4])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF5S', csdcoeffs_sign[5])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE00S_SET0CSDCOEFF6S', csdcoeffs_sign[6])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01S_SET0CSDCOEFF7S', csdcoeffs_sign[7])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01S_SET0CSDCOEFF8S', csdcoeffs_sign[8])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01S_SET0CSDCOEFF9S', csdcoeffs_sign[9])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01S_SET0CSDCOEFF10S', csdcoeffs_sign[10])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE01S_SET0CSDCOEFF11S', csdcoeffs_sign[11])
              
        # Load model variables into local variables
        bwsel = model.vars.lock_bwsel.value
        coeffs = self.return_coeffs(bwsel)

        # replace negative numbers with 2s complement
        for i in range(12):
            csdcoeffs[i], csdcoeffs_sign[i] = self.return_csd(coeffs[i], bit_widths[i])

        # TODO: calculate the second set separately
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10_SET1CSDCOEFF0', csdcoeffs[0])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10_SET1CSDCOEFF1', csdcoeffs[1])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10_SET1CSDCOEFF2', csdcoeffs[2])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10_SET1CSDCOEFF3', csdcoeffs[3])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11_SET1CSDCOEFF4', csdcoeffs[4])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11_SET1CSDCOEFF5', csdcoeffs[5])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11_SET1CSDCOEFF6', csdcoeffs[6])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE12_SET1CSDCOEFF7', csdcoeffs[7])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE12_SET1CSDCOEFF8', csdcoeffs[8])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE12_SET1CSDCOEFF9', csdcoeffs[9])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE13_SET1CSDCOEFF10', csdcoeffs[10])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE13_SET1CSDCOEFF11', csdcoeffs[11])
        
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF0S', csdcoeffs_sign[0])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF1S', csdcoeffs_sign[1])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF2S', csdcoeffs_sign[2])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF3S', csdcoeffs_sign[3])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF4S', csdcoeffs_sign[4])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF5S', csdcoeffs_sign[5])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE10S_SET1CSDCOEFF6S', csdcoeffs_sign[6])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11S_SET1CSDCOEFF7S', csdcoeffs_sign[7])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11S_SET1CSDCOEFF8S', csdcoeffs_sign[8])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11S_SET1CSDCOEFF9S', csdcoeffs_sign[9])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11S_SET1CSDCOEFF10S', csdcoeffs_sign[10])
        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCSDCOE11S_SET1CSDCOEFF11S', csdcoeffs_sign[11])                       
                
    def return_coeffs(self, bwsel):

        # this table is generated with srw_model/models/channel_filters/gen_channel_filter_coeffs.m
        if bwsel < 0.155:
            coeffs = [-16,   -51,   -86,   -101,   -82,   -17,   97,   248,   414,   565,   671,   709]
        elif bwsel < 0.165:
            coeffs = [-6,   -38,   -84,   -115,   -111,   -54,   60,   223,   409,   582,   704,   749]
        elif bwsel < 0.175:
            coeffs = [1,   -22,   -74,   -121,   -134,   -89,   23,   196,   402,   599,   740,   792]
        elif bwsel < 0.185:
            coeffs = [22,   5,   -53,   -110,   -139,   -113,   -12,   164,   388,   609,   772,   831]
        elif bwsel < 0.195:
            coeffs = [25,   21,   -32,   -98,   -145,   -138,   -48,   131,   372,   619,   804,   872]
        elif bwsel < 0.205:
            coeffs = [24,   32,   -10,   -82,   -147,   -162,   -86,   95,   354,   627,   836,   914]
        elif bwsel < 0.215:
            coeffs = [19,   37,   8,   -65,   -147,   -183,   -123,   56,   332,   634,   868,   956]
        elif bwsel < 0.225:
            coeffs = [21,   47,   31,   -38,   -130,   -189,   -153,   18,   305,   632,   893,   992]
        elif bwsel < 0.235:
            coeffs = [23,   56,   54,   -7,   -107,   -190,   -180,   -23,   273,   627,   915,   1026]
        elif bwsel < 0.245:
            coeffs = [24,   67,   83,   36,   -67,   -171,   -190,   -51,   251,   632,   951,   1075]
        elif bwsel < 0.255:
            coeffs = [14,   61,   97,   66,   -36,   -159,   -205,   -84,   223,   631,   981,   1118]
        elif bwsel < 0.265:
            coeffs = [6,   48,   100,   91,   -5,   -143,   -218,   -119,   190,   625,   1007,   1159]
        elif bwsel < 0.275:
            coeffs = [-2,   34,   99,   113,   29,   -121,   -226,   -154,   154,   616,   1033,   1201]
        elif bwsel < 0.285:
            coeffs = [-9,   17,   88,   126,   62,   -93,   -226,   -185,   117,   604,   1058,   1243]
        elif bwsel < 0.295:
            coeffs = [-15,   -1,   72,   132,   92,   -61,   -221,   -212,   79,   589,   1081,   1284]
        elif bwsel < 0.305:
            coeffs = [-21,   -19,   50,   130,   118,   -26,   -208,   -235,   42,   573,   1102,   1324]
        elif bwsel < 0.315:
            coeffs = [-26,   -38,   23,   117,   137,   11,   -188,   -252,   4,   555,   1125,   1367]
        elif bwsel < 0.325:
            coeffs = [-30,   -57,   -10,   95,   148,   47,   -162,   -262,   -28,   540,   1150,   1413]
        elif bwsel < 0.335:
            coeffs = [-33,   -76,   -50,   55,   135,   61,   -151,   -283,   -73,   509,   1160,   1444]
        elif bwsel < 0.345:
            coeffs = [-23,   -78,   -75,   30,   137,   92,   -125,   -293,   -111,   485,   1178,   1485]
        elif bwsel < 0.355:
            coeffs = [-13,   -70,   -94,   -1,   130,   120,   -93,   -297,   -150,   457,   1195,   1528]
        elif bwsel < 0.365:
            coeffs = [-4,   -59,   -107,   -32,   116,   143,   -59,   -296,   -186,   429,   1211,   1570]
        elif bwsel < 0.375:
            coeffs = [6,   -43,   -110,   -61,   95,   160,   -24,   -290,   -220,   398,   1225,   1611]
        elif bwsel < 0.385:
            coeffs = [15,   -24,   -108,   -88,   68,   170,   11,   -279,   -251,   366,   1238,   1652]
        elif bwsel < 0.395:
            coeffs = [23,   -2,   -96,   -109,   37,   171,   44,   -264,   -281,   333,   1249,   1692]
        elif bwsel < 0.405:
            coeffs = [31,   22,   -76,   -124,   4,   165,   73,   -247,   -308,   298,   1258,   1731]
        elif bwsel < 0.415:
            coeffs = [39,   51,   -42,   -124,   -28,   153,   98,   -231,   -339,   255,   1260,   1765]
        elif bwsel < 0.425:
            coeffs = [40,   78,   1,   -106,   -42,   152,   138,   -195,   -352,   225,   1275,   1814]
        elif bwsel < 0.435:
            coeffs = [30,   86,   29,   -101,   -70,   135,   164,   -166,   -371,   188,   1283,   1854]
        elif bwsel < 0.445:
            coeffs = [20,   86,   58,   -87,   -97,   112,   188,   -133,   -387,   148,   1289,   1897]
        elif bwsel < 0.455:
            coeffs = [9,   80,   82,   -66,   -117,   84,   205,   -99,   -399,   108,   1293,   1939]
        elif bwsel < 0.465:
            coeffs = [-2,   68,   101,   -39,   -131,   54,   217,   -63,   -407,   69,   1296,   1980]
        elif bwsel < 0.475:
            coeffs = [-14,   49,   111,   -9,   -135,   22,   223,   -27,   -411,   29,   1297,   2020]
        else:
            coeffs = [-26,   25,   114,   23,   -131,   -8,   224,   10,   -411,   -10,   1297,   2060]

        # Confirm Sum of the Magnitudes is in spec to not overflow the
        # filter accumulator
        try:
            assert sum([abs(i) for i in coeffs]) < 2**16
        except AssertionError:
            raise CalculationException('ERROR: Channel Filter Coefficients Sum of Magnitudes >= 2^16')

        return coeffs

    def calc_digmixfreq_reg(self,model):
        #This function calculates the digital mixer register
        fefilt_selected = model.vars.fefilt_selected.value
        digmixfreq = model.vars.digmixfreq.value

        # Write register
        self._reg_write_by_name_concat(model, fefilt_selected, 'DIGMIXCTRL_DIGMIXFREQ', digmixfreq)

    def calc_digmixfreq_actual(self,model):
        #This function calculates the actual digital mixer frequency based on the register

        #Load model variables into local variables
        digmixfreq_reg = Calc_Utilities_Viper().get_fefilt_actual(model, 'DIGMIXCTRL_DIGMIXFREQ')
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value

        #Calculate the actual mixer frequency
        digmixfreq_actual = int(digmixfreq_reg * model.vars.digmix_res_actual.value)

        #Load local variables back into model variables
        model.vars.digmixfreq_actual.value = digmixfreq_actual

    def calc_dec1gain_reg(self, model):
        """set DEC1GAIN register based on calculated value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = model.vars.dec1gain.value
        fefilt_selected = model.vars.fefilt_selected.value

        if val == 12:
            reg = 2
        elif val == 6:
            reg = 1
        else:
            reg = 0

        self._reg_write_by_name_concat(model, fefilt_selected, 'GAINCTRL_DEC1GAIN', reg)

    def calc_chflatency_actual(self, model):        
        model.vars.chflatency_actual.value = 0

    def calc_dccomp_misc_reg(self, model):

        #Read in model vars
        fefilt_selected = model.vars.fefilt_selected.value

        # always enable both DC offset estimation and compensation blocks
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCCOMPEN', 1)
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCESTIEN', 1)

        # don't reset at every packet
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCRSTEN', 0)

        # always enable gear shifting option
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCGAINGEAREN', 1)

        # when AGC gain change happens set the gear to fastest
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCGAINGEAR', 7)

        # final gear setting after settling
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCCOMPGEAR', 3)

        # limit max DC to 1V
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCLIMIT', 0)

        # don't freeze state of DC comp filters
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCCOMPFREEZE', 0)

        # time between gear shifts - set to fixed value for now
        self._reg_write_by_name_concat(model, fefilt_selected, 'DCCOMP_DCGAINGEARSMPS', 40)

    def gen_frequency_signal(self, x, sf, cf, sfosr, model):
        # get parameters
        deviation = model.vars.deviation.value
        baudrate = model.vars.baudrate.value
        demodosr = round(model.vars.oversampling_rate_actual.value)
        src2 = Calc_Utilities_Viper().get_fefilt_actual(model, 'SRC2_SRC2RATIO')
        datafilter = model.vars.MODEM_CTRL2_DATAFILTER.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remodoutsel = model.vars.MODEM_PHDMODCTRL_REMODOUTSEL.value
        demod_select = model.vars.demod_select.value
        dec2 = model.vars.dec2_actual.value

        remodpath = True if remoden or demod_select == model.vars.demod_select.var_enum.BCR else False

        if demod_select == model.vars.demod_select.var_enum.BCR:
            rawndec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value #Moved inside BCR statement to allow inheritance
            dec2 = 2 ** rawndec

        # scale shaping filter to desired amplitude OSR = 8
        sf = sf / np.sum(sf) * sfosr

        # pulse shape OSR = 8
        y = sp.lfilter(sf, 1, x)

        # apply deviation OSR = 8
        z = y * deviation

        # integrate to get phase after scaling by sampling rate at TX OSR = 8
        t = np.cumsum(z / (baudrate * sfosr))

        # modulate at baseband OSR = 8
        u = np.exp(1j * 2 * pi * t)

        # resample at channel filter rate (e.g. sfosr -> osr) OSR = chflt_osr * src2
        # FIXME: handle other remod paths here if we end up using them
        if remodpath:
            osr = demodosr * dec2
        else:
            osr = demodosr

        u2 = sp.resample_poly(u,osr*src2, sfosr*16384)

        # channel filter OSR = chflt_osr * src2
        v = sp.lfilter(cf, 1, u2)

        # src2 - resample to target OSR rate OSR = target_osr * dec2
        v2 = sp.resample_poly(v, 16384, src2)

        # CORDIC OSR = target_osr * dec2
        a = np.unwrap(np.angle(v2))

        # downsample by dec2 to get to target_osr if remod enabled
        if remodpath: #and remodoutsel == 1:
            # differentiate phase to frequency OSR = target_osr * dec2
            f1 = a[1:] - a[0:-1]
            #f = sp.resample_poly(f1, 1, dec2)

            # when downsampling pick the best phase that results in max eye opening as we are going to feed the samples
            # from here to the datafilter. Low value samples will bring the average soft decision to a lower value.
            best_min = 0
            for phase in range(dec2):
                f2 = sp.resample_poly(f1[round(len(f1)/4)+phase:], 1, dec2)
                min_val = min(abs(f2[3:-3]))
                if min_val >= best_min:
                    best_min = min_val
                    f = f2
        else:
            # differentiate phase to frequency OSR = target_osr * dec2
            f = a[osr:] - a[0:-osr]

        # optional decimation and filtering for remod paths
        if demod_select == model.vars.demod_select.var_enum.BCR:
            rawgain = model.vars.MODEM_BCRDEMODOOK_RAWGAIN.value #Moved inside BCR statement to allow inheritance
            rawfltsel = model.vars.MODEM_BCRDEMODCTRL_RAWFLTSEL.value
            ma1 = self.get_ma1_filter(rawgain)
            g1 = sp.lfilter(ma1, 1, f)
            ma2 = self.get_ma2_filter(rawfltsel)
            g = sp.lfilter(ma2, 1, g1)
        elif remoden and (remodoutsel == 0 or remodoutsel == 1):
            df = self.get_data_filter(datafilter)
            g = sp.lfilter(df, 1, f)
        else:
            g = f

        # return frequency signal
        return g

    def calc_dec2_actual(self,model):
        #This function calculates the actual dec2 ratio from the register value

        #Load model variables into local variables
        dec2_reg = model.vars.MODEM_CTRL5_DEC2.value

        #The actual dec2 value is the dec2 register plus one
        dec2_actual = dec2_reg + 1

        #Load local variables back into model variables
        model.vars.dec2_actual.value = dec2_actual

    def calc_src2_reg(self,model):
        #This function calculates the src2 register writes

        # Load model variables into local variables
        src2_value = model.vars.src2_ratio.value
        min_src2 = model.vars.min_src2.value  # min value for SRC2
        max_src2 = model.vars.max_src2.value  # max value for SRC2
        fefilt_selected = model.vars.fefilt_selected.value
        src2ratio_value_forced = Calc_Utilities_Viper().get_fefilt_value_forced(model, 'SRC2_SRC2RATIO')

        if src2ratio_value_forced != None:
            src2ratio_value = Calc_Utilities_Viper().get_fefilt_actual(model, 'SRC2_SRC2RATIO')
            src2_forced =  src2ratio_value / self.SRC2DENUM
            if (src2_forced > max_src2):
                max_src2 = src2_forced
            if (src2_forced < min_src2):
                min_src2 = src2_forced

        if (src2_value) >= min_src2 and (src2_value <= max_src2):
            src2_reg = int(round(16384/src2_value))
        else:
            raise CalculationException('WARNING: src2 value out of range in calc_src2_reg()')

        if (src2_reg != 16384):
            src2_en = 1
        else:
            src2_en = 0

        #Write to registers
        self._reg_write_by_name_concat(model, fefilt_selected, 'SRC2_SRC2RATIO', src2_reg)
        self._reg_write_by_name_concat(model, fefilt_selected, 'SRC2_SRC2ENABLE', src2_en)

    def return_ksi2_ksi3_calc(self, model, ksi1):
        # get parameters
        lock_bwsel = model.vars.lock_bwsel.value # use the lock bw
        bwsel = model.vars.bwsel.value  # use the lock bw
        osr = int(round(model.vars.oversampling_rate_actual.value))

        # calculate only if needed - ksi1 would be already calculated if that is the case
        if (ksi1 == 0):
            best_ksi2 = 0
            best_ksi3 = 0
            best_ksi3wb = 0
        else:
            # get shaping filter and it oversampling rate with respect to baudrate
            sf = Calc_Shaping_Viper().get_shaping_filter(model)/1.0
            sfosr = 8 # shaping filter coeffs are sampled at 8x

            # get channel filter and expend the symmetric part
            cfh = np.asarray(self.return_coeffs(lock_bwsel))
            cf = np.block([cfh, cfh[-2::-1]])/1.0
            cfh = np.asarray(self.return_coeffs(bwsel))
            cfwb = np.block([cfh, cfh[-2::-1]])/1.0

            # base sequences for +1 and -1
            a = np.array([ 1.0, 0, 0, 0, 0, 0, 0, 0])
            b = np.array([-1.0, 0, 0, 0, 0, 0, 0, 0])

            # generate frequency signal for periodic 1 1 1 0 0 0 sequence for ksi1
            x1 = np.matlib.repmat(np.append(np.matlib.repmat(a, 1, 3),np.matlib.repmat(b, 1, 3)), 1, 4)
            f1 = self.gen_frequency_signal( x1[0], sf, cf, sfosr, model)

            # generate frequency signal for periodic 1 1 0 0 1 1 sequence for ksi2
            x2 = np.matlib.repmat(np.append(np.matlib.repmat(a, 1, 2), np.matlib.repmat(b, 1, 2)), 1, 6)
            f2 = self.gen_frequency_signal( x2[0], sf, cf, sfosr, model)

            # generate frequency signal for periodic 1 0 1 0 1 0 sequence for ksi3
            x3 = np.matlib.repmat(np.append(np.matlib.repmat(a, 1, 1), np.matlib.repmat(b, 1, 1)), 1, 12)
            f3 = self.gen_frequency_signal( x3[0], sf, cf, sfosr, model)

            # generate frequency signal for periodic 1 0 1 0 1 0 sequence for ksi3 but with aqcusition channel filter
            f3wb = self.gen_frequency_signal( x3[0], sf, cfwb, sfosr, model)

            # find scaling needed to get f1 to the desired ksi1 value and apply it to f2 and f3
            ind = osr - 1
            scaler = ksi1 / np.max(np.abs(f1[ind + 8 * osr - 1: - 2 * osr: osr]))
            f2 = scaler * f2
            f3 = scaler * f3
            f3wb = scaler * f3wb

            # from matplotlib import pyplot as plt
            # plt.plot(f1*scaler,'x-')
            # plt.show()
            # plt.plot(f2,'x-')
            # plt.plot(f3,'x-')
            # plt.plot(f3wb,'x-')

            # search for best phase to sample to get ksi3 value.
            # best phase is the phase that gives largest eye opening
            best_ksi3 = 0
            for ph in range(osr):
                ksi3 = np.max(np.round(np.abs(f3[ - 6 * osr + ph: - 2 * osr: osr])))
                if ksi3 > best_ksi3:
                    best_ksi3 = ksi3

            best_ksi3wb = 0
            for ph in range(osr):
                ksi3wb = np.max(np.round(np.abs(f3wb[ - 6 * osr + ph: - 2 * osr: osr])))
                if ksi3wb > best_ksi3wb:
                    best_ksi3wb = ksi3wb

            # ksi2 is tricky depending if we sampled perfectly (symmetric around a
            # pulse we should see the same value for 1 1 0 and 0 1 1 sequence but
            # most of the time we cannot sample perfectly since can go as low as 4x
            # oversampling for Viterbi PHYs. In this case we have 2 ksi values which we
            # average to get the ksi2 value
            best_cost = 1e9
            for ph in range(osr):
                x = np.round(np.abs(f2[- 6 * osr + ph: - 2 * osr: osr]))
                cost = np.sum(np.abs(x - np.mean(x)))
                if cost < best_cost:
                    best_cost = cost
                    best_ksi2 = np.round(np.mean(x))

        # ensure that ksi1 >= ksi2 >= ksi3
        # this code should only be needed in the extreme case when ksi1 = ksi2 = ksi3 and
        # small variation can cause one to be larger than the other
        best_ksi2 = ksi1 if best_ksi2 > ksi1 else best_ksi2
        best_ksi3 = best_ksi2 if best_ksi3 > best_ksi2 else best_ksi3
        best_ksi3wb = best_ksi2 if best_ksi3wb > best_ksi2 else best_ksi3wb

        return best_ksi2, best_ksi3, best_ksi3wb

    def return_dec0_list(self,if_frequency_hz,adc_freq):
        return [4]

    def return_csd(self, coeff, bitwidth):
        csdcoeffsign = 0
        pointer = 0
        if coeff < 0:
            coeff = coeff + 2**bitwidth
            csdcoeffsign = csdcoeffsign + 2**(bitwidth//2)
        csdcoeff = coeff
        if coeff > 3:            
            while coeff > 2 : 
                if coeff%4 == 3:
                    csdcoeff = csdcoeff + 2**(pointer+1)
                    csdcoeffsign = csdcoeffsign + 2**(pointer//2)
                coeff = csdcoeff//2**(pointer+1)
                pointer += 1
        if csdcoeff > 2**bitwidth:
            csdcoeff = csdcoeff - 2**bitwidth
            csdcoeffsign = csdcoeffsign - 2 ** (bitwidth // 2)
        return csdcoeff, csdcoeffsign

    def calc_dec1gain_actual(self, model):
        #given register settings return actual DEC1GAIN used

        reg = Calc_Utilities_Viper().get_fefilt_actual(model, 'GAINCTRL_DEC1GAIN')

        if reg == 0:
            val = 0
        elif reg == 1:
            val = 6
        else:
            val = 12

        model.vars.dec1gain_actual.value = val

    def calc_rssi_dig_adjust_db(self, model):
        #These variables are passed to RAIL so that RSSI corrections can be made to more accurately measure power

        #Read in model vars
        dec1_actual = model.vars.dec1_actual.value
        dec1gain_actual = model.vars.dec1gain_actual.value

        #Calculate gains

        dec1_gain_linear = (dec1_actual**4) * (2**(-1*math.floor(4*math.log2(dec1_actual)-4)))
        dec1_gain_db = 20*math.log10(dec1_gain_linear/16) + dec1gain_actual #Normalize so that dec1=0 gives gain=16

        rssi_dig_adjust_db = dec1_gain_db

        #Write the vars
        model.vars.rssi_dig_adjust_db.value = rssi_dig_adjust_db
