""" CALC_Demodulator_jumbo Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be rturned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
import itertools

from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from pyradioconfig.parts.common.calculators.calc_demodulator import CALC_Demodulator
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities
from pyradioconfig.parts.common.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp
from pycalcmodel.core.variable import ModelVariableFormat

from py_2_and_3_compatibility import *

class CALC_Demodulator_jumbo(CALC_Demodulator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def calc_init_advanced(self, model):
        mod_format = model.vars.modulation_type.value
        baudrate_tol = model.vars.baudrate_tol_ppm.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value
        symbols_in_timing_window = model.vars.symbols_in_timing_window.value

        # init advanced inputs to defaults
        model.vars.src_disable.value = model.vars.src_disable.var_enum.ENABLED
        model.vars.viterbi_enable.value = False
        model.vars.dsa_enable.value = False

        # Change the target OSR based on the desired tolerance (lower OSR is better for tolerance but degrades sensitivity)
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            if baudrate_tol < 10000:
                target_osr = 7
            else:
                target_osr = 5
        # Use a smaller target OSR when more baudrate tolerance is needed or when using FDM0 (to reduce RAM throughput)
        elif in_2fsk_opt_scope:
            if baudrate_tol >= 10000 or symbols_in_timing_window==0:
                target_osr = 5
            else:
                target_osr = 7
        else:
            target_osr = 7

        model.vars.target_osr.value = target_osr

    def calc_src_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        osr = model.vars.oversampling_rate_actual.value

        src1 = model.vars.src1.value
        src2 = model.vars.src2.value
        src1_enable = 1
        src2_enable = 1

        if src1 == pow(2, (model.vars.src1_bit_width.value - 1) ):
            src1_enable = 0

        if src2 == pow(2, (model.vars.src2_bit_width.value - 1) ):
            src2_enable = 0

        if int(osr) == osr:
            intosr = 1
        else:
            intosr = 0

        self._reg_write(model.vars.MODEM_SRCCHF_SRCRATIO1, src1)
        self._reg_write(model.vars.MODEM_SRCCHF_SRCRATIO2, src2)
        self._reg_write(model.vars.MODEM_SRCCHF_SRCENABLE1, src1_enable)
        self._reg_write(model.vars.MODEM_SRCCHF_SRCENABLE2, src2_enable)
        self._reg_write(model.vars.MODEM_SRCCHF_INTOSR, intosr)


    def calc_bwsel_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        bwsel = model.vars.bwsel.value

        if bwsel == 0.263:
            reg = 0
        elif bwsel == 0.219:
            reg = 1
        else:
            reg = 2

        self._reg_write(model.vars. MODEM_SRCCHF_BWSEL,  reg)


#FIXME: in FDM0 mode make sure osr * bitrate < 10M
    def calc_d0_d1_d2_cfosr_rxbr_value(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # read required config parameters
        baudrate = model.vars.baudrate.value
        fxo = model.vars.fxo_or_fdec8.value
        bw_desired = model.vars.bandwidth_hz.value * 1.0
        brcalen = model.vars.brcalen.value
        tol = model.vars.baudrate_tol_ppm.value
        baudrate_offset = model.vars.rx_baudrate_offset_hz.value
        timingwindow = model.vars.timing_window_actual.value
        target_osr = model.vars.target_osr.value
        mod_format = model.vars.modulation_type.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value
        src1_bit_width = model.vars.src1_bit_width.value
        src2_bit_width = model.vars.src2_bit_width.value
        MODEM_VITERBIDEMOD_VTDEMODEN = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        bandwidth_hz_value_forced = model.vars.bandwidth_hz.value_forced
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value

        # Series 2: optional input to disallow DEC0 enums of DEC3/DEC8 (any width)
        flag_allow_dec0_dec_by_3 = model.vars.input_decimation_filter_allow_dec3.value
        flag_allow_dec0_dec_by_8 = model.vars.input_decimation_filter_allow_dec8.value

        if model.vars.src_disable.value == model.vars.src_disable.var_enum.DISABLED:
            src_disable = True
        else:
            src_disable = False

        debug = 0

        if debug:
            f = open('debug.csv','w')
            f.write(    'cost, bw_error, range_error, rate_error, dec0, dec1, dec2, bwsel, src1, src2, bw, baudrate_real, osr, eliminate0, eliminate1, eliminate2, eliminate3, eliminate4\n')


        # baudrate calibration works only in one direction so we set the
        # baudrate to the smallest value we want to support and let the calibration
        # hardware track it

        # 3 options in how we choose rxbr and src values:
        # 1) if baudrate calibration is enable (in which case SRC should be disabled as well)
        #    we fix rxbrden to 31 and src1 to pow(2, (model.vars.src1_bit_width.value - 1) ) and let rxbrnum range from 0 to 31
        # 2) if SRC is disabled we fix src1 to pow(2, (model.vars.src1_bit_width.value - 1) ) and let both rxbrnum and rxbrden range from 0 to 31
        # 3) in all other cases we limit rxbr numbers that are multiples of 1/2 so that osr is integer
        #    and give src1 the full range supported.

        src1_bit_width_pow = pow(2, (src1_bit_width - 1))
        src2_bit_width_pow = pow(2, (src2_bit_width - 1))

        if brcalen == 1:
            baudrate -= baudrate * tol / 1000000.0
            rxbrint_list = [2, 3, 4]
            rxbrnum_list = xrange(0, 32, 1)
            rxbrden_list = [31]
            src1_range = [src1_bit_width_pow]
        elif src_disable:
            rxbrint_list = [1, 2, 3, 4, 5, 6, 7]
            rxbrnum_list = xrange(0, 32, 1)
            rxbrden_list = xrange(1, 32, 1)
            src1_range = [src1_bit_width_pow]
        else:
            rxbrint_list = [1, 2, 3, 4, 5, 6, 7]
            rxbrnum_list = [1, 0]
            rxbrden_list = [2]
            src1_range = xrange(127, 156, 1)
            # Note that if model.vars.src1_bit_width.value is ever not 8, the src1_range needs to change. For now it is left hard-coded to be centered around 128.

            # Calculated per part family:
            # EFR32 90nm allows 127 - 156
            # 40nm Panther allows 125 - 156
            src1_range = model.vars.src1_range_available

        # limit and apply baudrate offset to 1%
        offset_limit = 0.01 * baudrate

        if baudrate_offset > offset_limit:
            baudrate_offset = offset_limit
        elif baudrate_offset < -offset_limit:
            baudrate_offset = - offset_limit

        baudrate += baudrate_offset

        # want to minimize this so start with big number
        best_cost = 99e99
        best_fc = 0

        # loop over all possible (and permitted by optional input for DEC0) DEC0, DEC1 values

        dec0_choices = []

        if flag_allow_dec0_dec_by_8 == 1:
            dec0_choices += [8]
        # 4 is always allowed
        dec0_choices += [4]
        if flag_allow_dec0_dec_by_3 == 1:
            dec0_choices += [3]
        flag_allow_dec0_dec_by_8 = model.vars.input_decimation_filter_allow_dec8.value

        if model.part_family.lower() in ["dumbo", "jumbo", "nerio", "nixi"]:
            dumbo_jumbo_nerio_nixi = True
        else:
            dumbo_jumbo_nerio_nixi = False

        for dec0 in dec0_choices:

            # Calculated per part family:
            # EFR32 90nm allows [0.263, 0.196]
            # 40nm Panther restricted to [0.263] only
            bwsel_list =  model.vars.ch_filt_bw_available

            # 0.219 setting is going away in Nerio so we should not expose that to the customer
            for bwsel in bwsel_list: #[0.263, 0.219, 0.196]:

                # limit SRC1 rate change to +/-18%
                # lower limit is 0.82 * pow(2, (model.vars.src1_bit_width.value - 1) ) = 104.96 = 105
                # upper limit is 1.18 * pow(2, (model.vars.src1_bit_width.value - 1) ) = 151.04 = 151
                for src1 in src1_range:

                    src1ratio = (src1_bit_width_pow * 1.0) / src1

                    dec1 = py2round((fxo * bwsel * src1ratio) / (dec0 * bw_desired ))

                    if dec1 < 1:
                        dec1 = 1

                    if dec1 > 11500:
                        dec1 = 11500

                    eliminate0 = 0

                    #only for EFR 90nm, NOT Panther
                    if dumbo_jumbo_nerio_nixi:
                        if dec1 == 1 and not src_disable and src1 != src1_bit_width_pow:
                            if debug:
                                eliminate0 = 1
                            else:
                                continue

                    # given dec0 and dec1 calculate bandwidth we actually get
                    bw = (fxo * bwsel * src1ratio) / (dec0 * dec1 )

                    # unless we are at the largest BW possible skip the rest if bw is less
                    # than 95% of carson bw or 82% of carson bw for 2.4GHz band
                    # TODO: tune these constants - dont'forget to factor into min_dec1, max_dec1
                    if baudrate >= 1e6:
                        carson_scale = 0.82
                    else:
                        carson_scale = 0.95

                    #bw_desired *= carson_scale

                    # we technically could do the following checks in the cost function below but
                    # that would mean calculating the cost function for thousands of
                    # combinations that we know will not work so to save computation
                    # we check this here.

                    # if bw less than a scaled version of the carson bw throw away this combination
                    # unless we are using the widest possible bw setting we have on the chip
                    # dec0 = 4 actually results in wider bw than dec0 = 3 due to the wider bw
                    # decimation filter it has.
                    eliminate1 = 0
                    if bw < bw_desired*carson_scale and not (dec1 == 1 and (dec0 == 3 or dec0 == 4)):
                        if debug:
                            eliminate1 = 1
                        else:
                            continue

                    # Always evaluate different OSR for OOK to prevent corner cases where no acceptable dec2 can be found
                    # We want to keep the OSR between 5 and 10 ideally, but will explore up to 15
                    if mod_format==model.vars.modulation_type.var_enum.OOK:
                        dec2_start = int(math.floor(fxo * src1ratio * 0.85 / (15.0 * dec0 * dec1 * baudrate))) #Assume worst-case 0.85 for SRC2
                        dec2_end = int(math.ceil(fxo * src1ratio / (5.0 * dec0 * dec1 * baudrate)))
                        rxbrmax = 8
                    elif in_2fsk_opt_scope:
                        dec2_start = int(math.floor(fxo * src1ratio * 0.85 / (9.0 * dec0 * dec1 * baudrate))) #Assume worst-case 0.85 for SRC2
                        dec2_end = int(math.ceil(fxo * src1ratio / (5.0 * dec0 * dec1 * baudrate)))
                        rxbrmax = 8
                    # for extreme bandwidth PHYs where bandwidth is much larger
                    # than the carson bandwidth we want to try large OSR values
                    # the limit for the else part is br = bw / 0.263*OSR*dec2 = bw/0.263*5*64 = bw/84
                    else:
                        if (bw_desired > 84 * baudrate):
                            dec2_start = int(math.floor(fxo / (15.0 * dec0 * dec1 * baudrate)))
                            dec2_end = int(math.ceil(fxo / (5.0 * dec0 * dec1 * baudrate)))
                            rxbrmax = 8
                        else:
                            # for normal PHYs we stick with OSR of 5
                            # we have two possible values for DEC2 given DEC0 and DEC1 so we loop
                            # over those values below
                            dec2_start = int(math.floor(fxo / (5.0 * dec0 * dec1 * baudrate)))
                            dec2_end = dec2_start + 1
                            rxbrmax = 4

                    # 0 is not a valid value for DEC2 so only try 1 if we end up with 0
                    # Also only use dec2=1 for series 2 parts where the Viterbi demod is enabled
                    if (not dumbo_jumbo_nerio_nixi) and (MODEM_VITERBIDEMOD_VTDEMODEN == 1):
                        dec2_start = 1
                        dec2_end = 1
                    elif dec2_start == 0:
                        dec2_start = 1

                    # max value for DEC2 is 63
                    if dec2_end > 63:
                        dec2_end = 63

                    #print(dec0, dec1, dec2_start, dec2_end)

                    for dec2 in xrange(dec2_start, dec2_end + 1):

                        #for rxbrint, rxbrnum in zip(rxbrint_list, rxbrnum_list):
                        for rxbrint, rxbrnum, rxbrden in list(itertools.product(rxbrint_list, rxbrnum_list, rxbrden_list)):

                            rxbrden = float(rxbrden)
                            rxbr = (rxbrint + rxbrnum / rxbrden)

                            eliminate2 = 0
                            if rxbr < 1.5 or rxbr > rxbrmax or rxbrnum >= rxbrden:
                                if debug:
                                    eliminate2 = 1
                                else:
                                    continue

                            if brcalen or src_disable:
                                src2 = src2_bit_width_pow
                            else:
                                src2 = py2round((fxo * (src2_bit_width_pow * 1.0 ) * src1ratio) / (dec0 * dec1 * dec2 * baudrate * 2 * rxbr))

                            eliminate3 = 0

                            #If using the viterbi demod on Panther or later, allow for a wider range of src2 values
                            if (not dumbo_jumbo_nerio_nixi) and (MODEM_VITERBIDEMOD_VTDEMODEN == 1):
                                if (src2 < (src2_bit_width_pow * 0.6)  or src2 > ( src2_bit_width_pow / 0.6) ):
                                    if debug:
                                        eliminate3 = 1
                                    else:
                                        continue
                            else:
                                # The 82% scaling factor was calculated such that the limits calculate precisely to 838 and 1249 for EFR32 90nm products. This is to prevent rounding effects from changing legacy .cfg results.
                                if (src2 < (src2_bit_width_pow * 0.818359375)  or src2 > (src2_bit_width_pow / 0.819855885) ):
                                    if debug:
                                        eliminate3 = 1
                                    else:
                                        continue

                            src2ratio = (src2_bit_width_pow * 1.0) / src2

                            # calculate real baudrate we get with these setting
                            baudrate_real = fxo * src1ratio * src2ratio / (dec0 * dec1 * dec2 * 2 * rxbr)

                            # calculate oversampling rate for given combination
                            osr = 2 * rxbr

                            eliminate4 = 0
                            if timingwindow * osr > 512:
                                #print("WARNING: Eliminating case due to memory restrictions")
                                if debug:
                                    eliminate4 = 1
                                else:
                                    continue

                            # calculate cost function based on over-sampling-rate, bw, and rx bit rate
                            cost, bw_error, range_error, rate_error, src_error = self.return_cost(model, osr, bw,
                                bw_desired, baudrate_real, baudrate, brcalen, target_osr, src1ratio, dec0, mod_format, bandwidth_hz_value_forced, in_2fsk_opt_scope)

                            if debug:
                                printlist = [cost, bw_error, range_error, rate_error, src_error, dec0, dec1, dec2,
                                    bwsel, src1, src2, bw, baudrate_real, osr,  eliminate0, eliminate1, eliminate2, eliminate3, eliminate4]

                                for item in printlist:
                                    f.write('%s,' % item)
                                f.write('\n')

                            # record necessary info if we found a better combination than we had
                            # prefer larger decimation in dec0 if the cost is the same
                            if cost < best_cost or (cost == best_cost and dec0 > dec1):
                                best_cost = cost
                                best_dec0 = dec0
                                best_dec1 = dec1
                                best_dec2 = dec2
                                best_rxbrnum = rxbrnum
                                best_rxbrden = rxbrden
                                best_rxbrint = rxbrint
                                best_bwsel = bwsel
                                best_src1 = src1
                                best_src2 = src2
                                best_bw_error = bw_error
                                best_range_error = range_error
                                best_rate_error = rate_error
                                best_src_error = src_error
                                best_cost_total = cost

        # store best combination in variables to be written to registers in other functions
        model.vars.dec0.value = int(best_dec0)
        model.vars.dec1.value = int(best_dec1)
        model.vars.dec2.value = int(best_dec2)
        model.vars.rxbrint.value = int(best_rxbrint)
        model.vars.rxbrnum.value = int(best_rxbrnum)
        model.vars.rxbrden.value = int(best_rxbrden)
        model.vars.bwsel.value = best_bwsel
        model.vars.src1.value = int(best_src1)
        model.vars.src2.value = int(best_src2)
        model.vars.cost_bandwidth.value = best_bw_error
        model.vars.cost_range.value = best_range_error
        model.vars.cost_rate.value = best_rate_error
        model.vars.cost_src.value = best_src_error
        model.vars.cost_total.value = best_cost_total

        if debug:
            f.close()

    # TODO: add deviation error to this cost function
    # TODO: if timing_window * osr  > 256 we might fail: find where we break down in terms of high OSR
    # TODO: make constant programmable
    def return_cost(self, model, osr, bw, bw_desired, baudrate_real, baudrate_hz, brcalen, target_osr, src1ratio, dec0, mod_format, bandwidth_hz_value_forced, in_2fsk_opt_scope):
        """
        Cost function used to find optimal settigs for DEC0, DEC1, DEC2, RXBRNUM, RXBRDEN

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # mod_format = model.vars.modulation_type.value
        # in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value

        if not in_2fsk_opt_scope:

            ###BANDWIDTH ERROR###
            bw_error = 100.0 * abs(bw - bw_desired) / bw_desired

            ###OVERSAMPLING ERROR###
            if target_osr == 7:
                range_error = abs(osr - target_osr)
            else:
                range_error = 10*abs(osr - target_osr)

            #TODO: This is a bug, but holding off on changes since this would impact many PHYs
            if osr < 5 or osr > 7:
                range_error*100

            #For OOK, strongly discourage OSR smaller than 5 or greater than 10
            if mod_format == model.vars.modulation_type.var_enum.OOK:
                if osr < 5 or osr >10:
                    range_error += 1.0e9

            ###BAUDRATE ERROR###
            # if the baudrate is not exact penalize this setting by 1e9
            rate_error = 100.0 * abs(baudrate_real - baudrate_hz) / baudrate_real

            # if baudrate calibration is enabled allow up to 1% error if not allow
            # 0.1% error before penalizing due to excessive baudrate offset
            if brcalen == 1:
                rate_error_limit = 10.0
            else:
                rate_error_limit = 0.4

            if rate_error > rate_error_limit:
                rate_error += 1.0e9
            elif rate_error > 0:
                rate_error *= 100.0

            ###SRC ERROR###
            src_error = 0.0

            # if bandwidth was forced choose setting with smallest bw_error and
            #  look at other metrics only if there are several with the same bw_error
            if bandwidth_hz_value_forced == None:
                    cost = bw_error + range_error + rate_error
            else:
                cost = 10*bw_error + range_error + rate_error#

        else: #2FSK optimization

            penalty_error = 1e9 #Added to extreme cases where we don't want to use the parameters unless absolutely necessary

            ###BANDWIDTH ERROR###
            bw_error_unweighted = 100.0 * abs(bw - bw_desired) / bw_desired #Equal to the percent BW error vs desired

            ###OVERSAMPLING ERROR###
            range_error_unweighted = abs(osr - target_osr) #Equal to the delta between the OSR and target OSR
            range_limit_low = 5
            range_limit_high = 7
            range_out_of_bounds = osr < range_limit_low or osr > range_limit_high
            if range_out_of_bounds:
                range_error_unweighted += penalty_error

            ###BAUDRATE ERROR###
            rate_error_unweighted = 100.0 * abs(baudrate_real - baudrate_hz) / baudrate_real #Equal to the percent baudrate error vs desired

            ###SRC ERROR###
            src_error_unweighted = 100 * abs(1-src1ratio) #Equal to the percent adjustment from src1

            ###DEC0_ERROR###
            if dec0 == 8:
                dec0_error_unweighted = 1 #Penalize when DEC0 is 8 due to poor flatness and large sidelobes
            else:
                dec0_error_unweighted = 0

            ###WEIGHTED AVERAGE COST###
            bw_error_weight = 1 #(1% BW error is minor, 5% just starts to matter, 20% is major)
            range_error_weight = 5 #An OSR delta of 1 is equivalent to a 5% bandwidth error
            rate_error_weight = 40 #A 0.5% baudrate error is very significant, and so this is eqivalent to a 20% bandwidth error
            src_error_weight = 0.25 #A +/- 20% ratio for SRC1 is not a showstopper, and so this is equivalent to 5% bandwidth error
            dec0_error_weight = 5 #When dec0 is 8, penalize equivalent to 5% bandwidth error

            bw_error = bw_error_unweighted * bw_error_weight
            range_error = range_error_unweighted * range_error_weight
            rate_error = rate_error_unweighted * rate_error_weight
            src_error = src_error_unweighted * src_error_weight
            dec0_error = dec0_error_unweighted * dec0_error_weight

            cost = bw_error + range_error + rate_error + src_error + dec0_error

        return cost, bw_error, range_error, rate_error, src_error


    def calc_rxbr_reg(self, model):
        """
        write RXBR register values

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.MODEM_RXBR_RXBRINT, int(model.vars.rxbrint.value))
        self._reg_write(model.vars.MODEM_RXBR_RXBRNUM, int(model.vars.rxbrnum.value))
        self._reg_write(model.vars.MODEM_RXBR_RXBRDEN, int(model.vars.rxbrden.value))


    def calc_rx_baud_rate_actual(self, model):
        """
        calculate actual RX baud rate from register settings
        Equation (5.17) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        fxo = model.vars.fxo_or_fdec8.value 
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        rxbrint = model.vars.rxbrint_actual.value
        rxbrden = model.vars.rxbrden_actual.value*1.0
        rxbrnum = model.vars.rxbrnum_actual.value
        src1ratio = model.vars.src1_ratio_actual.value
        src2ratio = model.vars.src2_ratio_actual.value

        rx_baud_rate = (fxo * src1ratio * src2ratio) / (dec0 * dec1 * dec2 * 2 * (rxbrint + rxbrnum / rxbrden))

        model.vars.rx_baud_rate_actual.value = rx_baud_rate


    def calc_src_actual(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        src1 = model.vars.MODEM_SRCCHF_SRCRATIO1.value
        src2 = model.vars.MODEM_SRCCHF_SRCRATIO2.value
        src1_enable = model.vars.MODEM_SRCCHF_SRCENABLE1.value
        src2_enable = model.vars.MODEM_SRCCHF_SRCENABLE2.value

        if src1_enable:
            model.vars.src1_ratio_actual.value = ( pow(2, (model.vars.src1_bit_width.value - 1) ) * 1.0 ) / src1
        else:
            model.vars.src1_ratio_actual.value = 1.0

        if src2_enable:
            model.vars.src2_ratio_actual.value = ( pow(2, (model.vars.src2_bit_width.value - 1) ) * 1.0 ) / src2
        else:
            model.vars.src2_ratio_actual.value = 1.0



    def calc_bwsel_actual(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        bwsel = model.vars.MODEM_SRCCHF_BWSEL.value

        # 0 and 1 both mean wideband and 2 or 3 mean narrowband
        if bwsel == 0 or bwsel == 1:
            value = 0.263
        else:
            value = 0.196

        model.vars.bwsel_actual.value = value

    def calc_bandwidth_actual(self, model):
        """
        calculate actual BW from register settings
        Equation (5.15) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.fxo_or_fdec8.value 
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        bwsel = model.vars.bwsel_actual.value
        src1ratio = model.vars.src1_ratio_actual.value

        bandwidth = py2round((fxo * bwsel * src1ratio ) / (dec0 * dec1))

        model.vars.bandwidth_actual.value = int(bandwidth)


    def dcalc_print_results(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        bw_actual = model.vars.bandwidth_actual.value
        br_actual = model.vars.rx_baud_rate_actual.value
        baudrate = model.vars.baudrate.value
        bw_desired = model.vars.bandwidth_hz.value
        baudrate_offset = model.vars.rx_baudrate_offset_hz.value

        LogMgr.Info(baudrate, baudrate_offset, br_actual, bw_desired, bw_actual)

    def calc_rx_freq_dev_actual(self, model):
        """
        given register settings calculate actual frequency deviation
        the PHY nominally expects in the receive path.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.fxo_or_fdec8.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        gain = model.vars.freq_gain_actual.value
        mod_format = model.vars.modulation_type.value
        src1ratio = model.vars.src1_ratio_actual.value
        src2ratio = model.vars.src2_ratio_actual.value

        # frequency deviation only used for 2-FSK and 4-FSK modulation
        deviation = 0.0

        if gain > 0:
            if mod_format == model.vars.modulation_type.var_enum.FSK2:
                deviation = fxo * src1ratio * src2ratio / (4.0 * gain * dec0 * dec1 * dec2)
            elif mod_format == model.vars.modulation_type.var_enum.FSK4:
                deviation = fxo * src1ratio * src2ratio / (8.0 * gain * dec0 * dec1 * dec2)

        model.vars.rx_deviation_actual.value = deviation

    def calc_freq_gain_value(self, model):
        """
        calculate desired frequency gain
        Using Equation (5.22) and (5.23) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.fxo_or_fdec8.value
        freq_offset = model.vars.freq_offset_hz.value * 1.0
        freq_dev_hz = model.vars.deviation.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        mod_format = model.vars.modulation_type.value
        src1ratio = model.vars.src1_ratio_actual.value
        src2ratio = model.vars.src2_ratio_actual.value

        # if supported frequency offset is larger than the deviation we should use
        # that to calculate frequency gain to avoid saturation
        if freq_dev_hz != 0 and freq_offset >= freq_dev_hz:
            scale = freq_offset / freq_dev_hz
            freq_dev_hz = freq_offset
        else:
            scale = 1.0

        if freq_dev_hz > 0:
            if mod_format == model.vars.modulation_type.var_enum.FSK2:
                target_freq_gain = fxo * src1ratio * src2ratio / (4.0 * freq_dev_hz * dec0 * dec1 * dec2)
            elif mod_format == model.vars.modulation_type.var_enum.FSK4:
                target_freq_gain = fxo * src1ratio * src2ratio / (8.0 * freq_dev_hz * dec0 * dec1 * dec2)
            else:
                target_freq_gain = 0.0
        else:
            target_freq_gain = 0.0

        if target_freq_gain > 28: # max freq gain m=7 e=0 m*2^(2-e)=28
            target_freq_gain = 28.0

        model.vars.freq_gain.value = target_freq_gain
        model.vars.freq_gain_scale.value = scale


    def calc_datafilter_reg(self, model):
        """set register for datafilter size
        see DATAFILTER register entry in EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modformat = model.vars.modulation_type.value
        osr = model.vars.oversampling_rate_actual.value

        # disable datafilter by default
        datafilter = 0

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
           modformat == model.vars.modulation_type.var_enum.FSK4 or \
           modformat == model.vars.modulation_type.var_enum.OOK or \
           modformat == model.vars.modulation_type.var_enum.ASK or \
           modformat == model.vars.modulation_type.var_enum.BPSK:

            if osr >= 8.5:
                datafilter = 7
            elif osr >= 7.5:
                datafilter = 6
            elif osr >= 6.5:
                datafilter = 5
            elif osr >= 5.5:
                datafilter = 4
            elif osr >= 4.5:
                datafilter = 3
            elif osr >= 3.5:
                datafilter = 2
            elif osr >= 2.5:
                datafilter = 1

        self._reg_write(model.vars.MODEM_CTRL2_DATAFILTER,  datafilter)

    def calc_digmixfreq_reg(self, model):

        if_analog_hz = model.vars.if_frequency_hz_actual.value
        fxo = model.vars.fxo_or_fdec8.value
        dec0 = model.vars.dec0_actual.value

        digmixfreq = int(if_analog_hz * dec0 * pow(2, 20) / fxo)

        if digmixfreq > pow(2, 20) - 1:
            digmixfreq = pow(2, 20) - 1

        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXFREQ,  digmixfreq)

    def calc_digmixmode_reg(self, model):

        # always use DIGMIXFREQ mode over legacy CFOSR mode
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXMODE,  1)
        # set to CFOSR register to 0 (divider value 7) by default
        model.vars.cfosr.value = 7

    def calc_if_frequency_actual(self, model):
        return

    def calc_if_center_digital_hz_actual(self, model):
        """calculate the actual IF frequency (IF frequency)
        Equation (5.14) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        fxo = model.vars.fxo_or_fdec8.value 
        dec0 = model.vars.dec0_actual.value
        cfosr = model.vars.cfosr_actual.value
        digmixfreq = model.vars.MODEM_DIGMIXCTRL_DIGMIXFREQ.value
        digmixmode = model.vars.MODEM_DIGMIXCTRL_DIGMIXMODE.value

        if digmixmode:
            if_frequency = fxo * digmixfreq / (dec0 * pow(2, 20))
        else:
            if_frequency = fxo / (dec0 * cfosr)

        model.vars.if_center_digital_hz_actual.value = int(if_frequency)


    def calc_resyncbaudtrans_reg(self, model):

        brcalen = model.vars.brcalen.value
        mod_type = model.vars.modulation_type.value

        #For OOK, never use timing windows without baud transitions to resync
        if mod_type == model.vars.modulation_type.var_enum.OOK:
            self._reg_write(model.vars.MODEM_CTRL5_RESYNCBAUDTRANS, 1)
        else:
            # if baudrate calibration is on don't use timing windows without baud transitions
            # in estimating timing
            if brcalen:
                self._reg_write(model.vars.MODEM_CTRL5_RESYNCBAUDTRANS, 1)
            else:
                self._reg_write(model.vars.MODEM_CTRL5_RESYNCBAUDTRANS, 0)


    def calc_brcalmode_reg(self, model):

        brcalen = model.vars.brcalen.value

        if brcalen:
            # Intentional, not a bug.
            # Based on feedback from Zoltan, we'll always set this to mode 0 instead of 2
            # Keeping the structure just in case we change it again.
            self._reg_write(model.vars.MODEM_CTRL5_BRCALMODE, 0)
        else:
            self._reg_write(model.vars.MODEM_CTRL5_BRCALMODE, 0)


    def calc_detdel_reg(self, model):

        brcalen = model.vars.brcalen.value

        if brcalen:
            self._reg_write(model.vars.MODEM_CTRL5_DETDEL, 1)
        else:
            self._reg_write(model.vars.MODEM_CTRL5_DETDEL, 0)

