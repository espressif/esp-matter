from pyradioconfig.parts.ocelot.calculators.calc_demodulator import CALC_Demodulator_ocelot
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pycalcmodel.core.variable import ModelVariableFormat
from math import floor, log, ceil
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

class Calc_Demodulator_Bobcat(CALC_Demodulator_ocelot):

    def buildVariables(self, model):
        super().buildVariables(model)

        self._addModelVariable(model, 'freq_dev_max', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'freq_dev_min', int, ModelVariableFormat.DECIMAL)

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

    def calc_trecs_enabled(self, model):
        demod_select = model.vars.demod_select.value
        concurrent_ble = model.vars.MODEM_COCURRMODE_CONCURRENT.value
        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:
            trecs_enabled = True
        elif concurrent_ble:
            trecs_enabled = True
        else:
            trecs_enabled = False

        model.vars.trecs_enabled.value = trecs_enabled

    def calc_baudrate_actual(self, model):
        # This function calculates the actual baudrate based on register settings

        # Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2ratio_actual = model.vars.src2_ratio_actual.value
        subfrac_actual = model.vars.subfrac_actual.value
        rxbrfrac_actual = model.vars.rxbrfrac_actual.value

        if subfrac_actual > 0:
            frac = subfrac_actual
        else:
            frac = rxbrfrac_actual

        # Calculate actual baudrate once the ADC, decimator, SRC, and rxbr settings are kown
        baudrate_actual = (adc_freq * src2ratio_actual) / (dec0_actual * dec1_actual * dec2_actual * 8 * 2 * frac)

        # Load local variables back into model variables
        model.vars.rx_baud_rate_actual.value = baudrate_actual

    def calc_osr_actual(self,model):
        #This function calculates the actual OSR based on the ADC rate and decimator/SRC values

        #Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        baudrate_actual = model.vars.rx_baud_rate_actual.value
        src2_actual = model.vars.src2_ratio_actual.value

        osr_actual = adc_freq_actual * src2_actual / (dec0_actual * dec1_actual * 8 * dec2_actual * baudrate_actual)

        #Load local variables back into model variables
        model.vars.oversampling_rate_actual.value = osr_actual


    def get_limits(self, demod_select, withremod, relaxsrc2, model):

        #Load model variables into local variables
        bandwidth = model.vars.bandwidth_hz.value #from calc_target_bandwidth

        baudrate = model.vars.baudrate.value #We don't know the actual bandrate yet
        modtype = model.vars.modulation_type.value
        mi = model.vars.modulation_index.value
        min_chfilt_osr = None
        max_chfilt_osr = None
        osr_list = None

        # Define constraints for osr, src2, dec2
        if demod_select == model.vars.demod_select.var_enum.BCR:
            # FIXME:  osr_list and resulting target osr are really chfilt_osr, pro2 calculator defines target_osr
            #       This doesn't cause an error but is confusing.
            osr_est = int(ceil(2 * float(bandwidth) / baudrate))

            min_osr = 8
            max_osr = 127
            min_chfilt_osr = 8
            if (modtype == model.vars.modulation_type.var_enum.OOK) or \
                    (modtype == model.vars.modulation_type.var_enum.ASK):
                max_chfilt_osr = 16256  #127*max_bcr_dec = 127*128
                osr_list = range(12, max_chfilt_osr)
            else:
                max_chfilt_osr = 127
                osr_list = [osr_est]

            min_src2 = 1.0
            max_src2 = 1.0
            min_dec2 = 1
            max_dec2 = 1
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
        elif demod_select == model.vars.demod_select.var_enum.LEGACY:
            if (modtype == model.vars.modulation_type.var_enum.FSK2 or \
                  modtype == model.vars.modulation_type.var_enum.FSK4 or \
                  modtype == model.vars.modulation_type.var_enum.MSK) and (mi<1):
                # >=7 is better for sensitivity and frequency offset
                # cost (sens degrade) increases with decreasing osr 6,5,4
                osr_list = [7, 8, 9, 6, 5, 4]
                min_osr = 4
            else:
                osr_list = [5, 7, 6, 4, 8, 9]
                min_osr = 4
            max_osr = 9
            min_src2 = 0.8
            max_src2 = 1.65 if relaxsrc2 else 1.2
            min_dec2 = 1
            max_dec2 = 64
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
        elif demod_select == model.vars.demod_select.var_enum.COHERENT:
            osr_list = [5]
            min_osr = 5
            max_osr = 5
            min_src2 = 0.8
            min_dec2 = 1
            max_dec2 = 1
            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4
            max_src2 = 1.65 if relaxsrc2 else 1.2
        elif demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            min_bwsel = 0.2
            target_bwsel = 0.4
            max_bwsel = 0.4

            if relaxsrc2 == True:
                min_src2 = 0.55
                max_src2 = 1.3
                min_bwsel = 0.15
            else:
                min_src2 = 0.8
                max_src2 = 1.0

            if withremod == True:
                min_dec2 = 1
                max_dec2 = 64
                min_osr = 4
                max_osr = 32
                osr_list = [4, 5, 6, 7]
            elif mi > 2.5: #FIXME: arbitrary threshold here - for zwave 9.6kbps with mi=2.1 we prefer not to use int/diff path but at some point we will have to
                min_dec2 = 1
                max_dec2 = 64
                min_osr = 4
                max_osr = 7
                osr_list = [4, 5, 6, 7]
            else:
                # Standard TRECs, no DEC2 or remod path
                min_dec2 = 1
                max_dec2 = 1
                min_osr = 4
                max_osr = 7
                osr_list = [4, 5, 6, 7]
        elif demod_select == model.vars.demod_select.var_enum.LONGRANGE:
            min_dec2 = 1
            max_dec2 = 1
            min_osr = 4
            max_osr = 4
            osr_list = [4]
            min_src2 = 0.8
            max_src2 = 1.2
            min_bwsel = 0.2
            target_bwsel = 0.3
            max_bwsel = 0.3
        else:
            raise CalculationException('ERROR: invalid demod_select in return_osr_dec0_dec1()')

        # save to use in other functions
        model.vars.min_bwsel.value = min_bwsel  # min value for normalized channel filter bandwidth
        model.vars.max_bwsel.value = max_bwsel  # max value for normalized channel filter bandwidth
        model.vars.min_src2.value = min_src2  # min value for SRC2
        model.vars.max_src2.value = max_src2  # max value for SRC2
        model.vars.max_dec2.value = max_dec2
        model.vars.min_dec2.value = min_dec2

        return min_bwsel, max_bwsel, min_chfilt_osr, max_chfilt_osr, min_src2, max_src2, min_dec2, max_dec2, min_osr, max_osr, target_bwsel, osr_list




    def calc_rxbr(self,model):
        #This function calculates the receive baudrate settings
        # based on actual dec0,dec1,dec2,src2, and desired baudrate
        # then baudrate_actual will be calculated from rxbrfrac_actual

        #Load model variables into local variables
        target_osr = model.vars.target_osr.value #We don't know the actual OSR yet, because that has to be based on the final baudrate
        targetmax_osr = model.vars.targetmax_osr.value
        targetmin_osr = model.vars.targetmin_osr.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2_actual = model.vars.src2_ratio_actual.value
        baudrate = model.vars.baudrate.value

        denlist = range(2, 31)
        error_limit = 0.5

        # not using target_osr, because in some cases (e.g. BCR with fractional OSR)
        # the OSR w.r.t desired baudrate and acutal decimators varies from the target
        # allowing 0.01% variation from targetmin_osr and targetmax_osr for range check
        # because this osr calculation uses src2_actual, which has some small quantization noise
        osr = float(adc_freq_actual * src2_actual) / float(dec0_actual * dec1_actual * 8 * dec2_actual * baudrate)
        osr_limit_min = targetmin_osr * (1 - 0.0001)
        osr_limit_max = targetmax_osr * (1 + 0.0001)

        if (osr >= osr_limit_min) and (osr <= osr_limit_max):
            #search for best fraction
            rxbrint = int(floor(osr/2))
            frac = (osr/2) - float(rxbrint)
            numlist = range(0,31)
            min_error = 100
            for den in denlist:
                for num in numlist:
                    frac_error = abs(float(num)/float(den) - frac)
                    if (frac_error<min_error):
                        min_error = frac_error
                        best_den = den
                        best_num = num
            # calculate error in percent of baudrate, and require < 0.5% error
            # matlab simulation sweeping osr with 0.01% step size, showed the max osr relative error = 0.4%
            # using num=0:31, den=2:31
            error_percent = 100 * abs( 2 * ( rxbrint + float(best_num) / float(best_den) ) - osr ) / osr
            if error_percent < error_limit:
                rxbrnum = best_num
                rxbrden = best_den
                if (rxbrnum==rxbrden):
                    rxbrden=2
                    rxbrnum=0
                    rxbrint=rxbrint+1
                elif rxbrnum>rxbrden:
                    raise CalculationException('ERROR: num > den in calc_rxbr()')

            else:
                #print("adc_freq = %f" % adc_freq_actual)
                #print("baudrate = %f" % baudrate)
                #print("target_osr = %f" % target_osr)
                #print("adjust_osr = %f" % osr)
                #print("rxbrint = %d" % rxbrint)
                #print("best_num = %d" % best_num)
                #print("best_den = %d" % best_den)
                #print(model.vars.demod_select.value)
                raise CalculationException('ERROR: baudrate error > 0.5% in calc_rxbr()')
        else:
            #print("adc_freq = %f" % adc_freq_actual)
            #print("baudrate = %f" % baudrate)
            #print("target_osr = %f" % target_osr)
            #print("adjust_osr = %f" % osr)
            #print("targetmin_osr = %f" % targetmin_osr)
            #print("targetmax_osr = %f" % targetmax_osr)
            #print(str(model.vars.demod_select.value).split(".")[-1])
            raise CalculationException('ERROR: OSR out of range in calc_rxbr()')

        #Load local variables back into model variables
        model.vars.rxbrint.value = rxbrint
        model.vars.rxbrnum.value = rxbrnum
        model.vars.rxbrden.value = rxbrden

    def calc_freq_dev_min(self, model):
        # Reading variables from model variables
        fdeverror = model.vars.deviation_tol_ppm.value
        deviation = model.vars.deviation.value
        freq_dev_min = int(deviation - (fdeverror * deviation) / 1000000)
        model.vars.freq_dev_min.value = freq_dev_min

    def calc_freq_dev_max(self, model):
        # Reading variables from model variables
        fdeverror = model.vars.deviation_tol_ppm.value
        deviation = model.vars.deviation.value
        freq_dev_max = int(deviation + (fdeverror * deviation) / 1000000)
        model.vars.freq_dev_max.value = freq_dev_max

    def calc_interpolation_gain_actual(self, model):
        #This function calculates the actual interpolation gain

        #Load model variables into local variables
        txbrnum = model.vars.MODEM_TXBR_TXBRNUM.value
        modformat = model.vars.modulation_type.value

        if txbrnum < 256:
            interpolation_gain = txbrnum / 1.0
        elif modformat == model.vars.modulation_type.var_enum.BPSK or \
             modformat == model.vars.modulation_type.var_enum.DBPSK:
            interpolation_gain = 16 * txbrnum * 2 ** (3-floor(log(txbrnum, 2)))
        elif txbrnum < 512:
            interpolation_gain = txbrnum / 2.0
        elif txbrnum < 1024:
            interpolation_gain = txbrnum / 4.0
        elif txbrnum < 2048:
            interpolation_gain = txbrnum / 8.0
        elif txbrnum < 4096:
            interpolation_gain = txbrnum / 16.0
        elif txbrnum < 8192:
            interpolation_gain = txbrnum / 32.0
        elif txbrnum < 16384:
            interpolation_gain = txbrnum / 64.0
        else:
            interpolation_gain = txbrnum / 128.0

        # following the rtl
        interpolation_gain = floor(interpolation_gain)

        # calculate phase interpolation gain for OQPSK cases
        if modformat == model.vars.modulation_type.var_enum.OQPSK:
            interpolation_gain = 2 ** (ceil(log(interpolation_gain, 2)))

        #Load local variables back into model variables
        model.vars.interpolation_gain_actual.value = float(interpolation_gain)

    def calc_lock_bandwidth(self, model):
        # Load model variables into local variables
        bw_acq = model.vars.bandwidth_hz.value
        aox_enable = True if model.vars.aox_enable.value == model.vars.aox_enable.var_enum.ENABLED else False

        if not aox_enable:
            super().calc_lock_bandwidth(model)
        else:
            # Widen channel filter bandwidth for faster settling time during CTE, controlled by CHFSWSEL
            # See PGBOBCATVALTEST-807. 2xBW came from system study as default. Silicon may need further tweaks
            # to limit step response overshoot, depending on the channel filter
            lock_bandwidth_hz = 2 * bw_acq
            model.vars.lock_bandwidth_hz.value = int(lock_bandwidth_hz)

    def calc_rssi_rf_adjust_db(self, model):
        model.vars.rssi_rf_adjust_db.value = -8.0
