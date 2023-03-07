from pyradioconfig.parts.panther.calculators.calc_synth import CALC_Synth_panther
from pycalcmodel.core.variable import ModelVariableFormat
import math

class CALC_Synth_lynx(CALC_Synth_panther):
    ## Retime Table Generation

    fvcomin = 4450e6
    fvcomax = 5950e6

    def buildVariables(self, model):

        #Add all of the variables from the Panther synth calculator
        super().buildVariables(model)

        # for HFXO/HFRCO retiming
        self._addModelVariable(model, 'lut_table_index', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_freq', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_freq_upper', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_valid', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_smuxdiv', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_limitl', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_limith', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'lut_dpll_freq_hz', int, ModelVariableFormat.DECIMAL, is_array=True)

    def retime_print(self, *arg):
        hide = True

        if (hide == False):
            print(arg)

    # Calculate the retiming smartmux divider setting
    def retime_calc_smuxdiv(self, freq):
        global SMUX_MAX_FREQ
        SMUX_MAX_FREQ = 625  # Define the maximum frequency in MHz that the smart mux can run

        # Calculate LO frequency divider
        if (freq <= SMUX_MAX_FREQ):
            # less than 625 MHz
            smuxdiv = 1
        elif (freq <= (2 * SMUX_MAX_FREQ)):
            # less than 1250 MHz
            smuxdiv = 2
        else:
            smuxdiv = 4
        return (smuxdiv)

    # Calculate the valid lowest synthesizer frequency
    # This will ensure we honor the catchup duration limit
    def retime_calc_min_synth_freq(self, sysclk_freq, smuxdiv, num_clks):
        global NUM_LAG_CATCHUP_CLKS
        global MAX_CATCHUP_DURATION

        NUM_LAG_CATCHUP_CLKS = 4.0  # max number of smart mux clock periods to catch up
        MAX_CATCHUP_DURATION = 10.0  # us

        # Dur = CATCHUP_DURATION =catchup_period * (Time_Lag / delta_period)
        # lag_time = NUM_LAG_CATCHUP_CLKS * lo_period;
        # L = NUM_LAG_CATCHUP_CLKS  = 4 ....this is approximately 4 lo clocks of delay that we need to catch up
        # cp = catchup_period = (smuxdiv  * num_clks) / freq
        # delta_period = sysclk_period - catchup_period;  ....the incremental time that we catch up per clock

        # Dur = cp * lagtime / delta
        # Dur = cp * (cp * L/N) / (sp - cp)
        # Dur = (L/N)*cp^2 / (sp - cp)
        # (L/N)*cp^2 + Dur * cp - Dur * sp = 0

        # cp = (-Dur +/- sqrt(Dur^2 + (4 * L * Dur * sp / N ) )/ (2 * (L/N) )
        # 1/cp = cf = (2 * (L/N) )  /    (-Dur +/- sqrt(Dur^2 + (4 * L * Dur * sp / N) )
        # freq = cf * (smuxdiv * N)
        # freq = (2 * smuxdiv * L )  / (-Dur +/- sqrt(Dur^2 + (4 * L * Dur * sp / N) )

        # =(2*L*smux)/((-1*Dur)+SQRT((Dur)^2 + 4*L*Dur*sp/num_clks))
        tmp = (float)(4.0 * NUM_LAG_CATCHUP_CLKS * MAX_CATCHUP_DURATION / sysclk_freq)
        tmp1 = (float)(num_clks)
        tmp = (float)(tmp / tmp1)
        tmp = math.sqrt((MAX_CATCHUP_DURATION * MAX_CATCHUP_DURATION) + tmp)
        tmp = ((-1.0 * MAX_CATCHUP_DURATION) + tmp)
        freq = ((2.0 * smuxdiv * NUM_LAG_CATCHUP_CLKS) / tmp)

        return (freq)

    # Generate one retiming entry for the table, given the multiple
    def return_retime_gen_entry(self, sysclk_freq, valid_freq, multiple, model, table_idx, start_table_idx):

        synth_freq_low = valid_freq * multiple
        smuxdiv = self.retime_calc_smuxdiv(synth_freq_low)
        next_multiple = multiple + smuxdiv

        synth_freq_high = valid_freq * next_multiple
        smuxdiv_high = self.retime_calc_smuxdiv(synth_freq_high)

        if ((smuxdiv_high > smuxdiv) and (not (multiple % smuxdiv_high))):
            next_multiple = multiple + smuxdiv_high
            synth_freq_high = valid_freq * next_multiple

        num_clks = round(multiple / smuxdiv) + 1
        limith = math.floor(num_clks / 2) #round(num_clks / 2); C tend to round down, while python tend to round up odd number
        limitl = num_clks - limith

        synth_freq_min_catchup = self.retime_calc_min_synth_freq(sysclk_freq, smuxdiv, num_clks)

        # If low end frequency is less than allowable catchup limit frequency
        # then add an invalid entry region (from low to min_catchup)
        if (synth_freq_min_catchup > synth_freq_low):
            model.vars.lut_freq.value.append(int(synth_freq_low))
            if (synth_freq_min_catchup > (smuxdiv * SMUX_MAX_FREQ)):
                model.vars.lut_freq_upper.value.append(int(smuxdiv * SMUX_MAX_FREQ))
            else:
                model.vars.lut_freq_upper.value.append(int(round(synth_freq_min_catchup)))
            model.vars.lut_valid.value.append(0)
            model.vars.lut_smuxdiv.value.append(0)
            model.vars.lut_limitl.value.append(0)
            model.vars.lut_limith.value.append(0)
            model.vars.lut_table_index.value[table_idx] += 1
            model.vars.lut_freq.value.append(model.vars.lut_freq_upper.value[start_table_idx + model.vars.lut_table_index.value[table_idx] - 1])
        else:
            model.vars.lut_freq.value.append(int(synth_freq_low))

        # If smuxdiv the same, add the final valid entry
        # from either low or catchup limit (see above) to final frequency
        if (smuxdiv_high == smuxdiv):
            model.vars.lut_freq_upper.value.append(int(synth_freq_high))
            model.vars.lut_valid.value.append(1)
            model.vars.lut_smuxdiv.value.append(smuxdiv_high)
            model.vars.lut_limitl.value.append(int(limitl))
            model.vars.lut_limith.value.append(int(limith))
            model.vars.lut_table_index.value[table_idx] += 1
        else:
            if (synth_freq_min_catchup < (smuxdiv * SMUX_MAX_FREQ)):
                model.vars.lut_freq_upper.value.append(int(smuxdiv * SMUX_MAX_FREQ))
                model.vars.lut_valid.value.append(1)
                model.vars.lut_smuxdiv.value.append(smuxdiv)
                model.vars.lut_limitl.value.append(int(limitl))
                model.vars.lut_limith.value.append(int(limith))
                model.vars.lut_table_index.value[table_idx] += 1

            # If smuxdiv changes within a multiple, then recalculate the low limits
            num_clks = round(next_multiple / smuxdiv_high)
            limith = math.floor(num_clks / 2) #round(num_clks / 2); C tend to round down, while python tend to round up odd number
            limitl = num_clks - limith

            synth_freq_low = smuxdiv * SMUX_MAX_FREQ
            synth_freq_min_catchup = self.retime_calc_min_synth_freq(sysclk_freq, smuxdiv_high, num_clks)

            if (synth_freq_min_catchup > synth_freq_low):
                if (model.vars.lut_valid.value[start_table_idx - 1] == 0):
                    model.vars.lut_freq_upper.value[start_table_idx - 1] = int(synth_freq_min_catchup)
                    model.vars.lut_freq.value.append(int(synth_freq_min_catchup))
                else:
                    model.vars.lut_freq.value.append(int(synth_freq_low))
                    model.vars.lut_freq_upper.value.append(int(synth_freq_min_catchup))
                    model.vars.lut_valid.value.append(0)
                    model.vars.lut_smuxdiv.value.append(0)
                    model.vars.lut_limitl.value.append(0)
                    model.vars.lut_limith.value.append(0)
                    model.vars.lut_table_index.value[table_idx] += 1

                    model.vars.lut_freq.value.append(int(synth_freq_min_catchup))
            else:
                model.vars.lut_freq.value.append(int(synth_freq_low))

            model.vars.lut_freq_upper.value.append(int(synth_freq_high))
            model.vars.lut_valid.value.append(1)
            model.vars.lut_smuxdiv.value.append(smuxdiv_high)
            model.vars.lut_limitl.value.append(int(limitl))
            model.vars.lut_limith.value.append(int(limith))
            model.vars.lut_table_index.value[table_idx] += 1

        return (next_multiple)

    # Calculate the catchup duration
    def retime_calc_catchup_duration(self, sysclk_freq, freq, smuxdiv, num_clks):
        float_freq = (float)(freq / 1.0)
        min_sys_period = (float)(num_clks * (smuxdiv / float_freq))
        lag_time = (float)(smuxdiv / float_freq) * NUM_LAG_CATCHUP_CLKS
        delta = (float)((1.0 / sysclk_freq) - min_sys_period)
        self.retime_print("lag_time: %f, delta: %f" % (lag_time, delta))
        if (delta == 0):
            delta = (2.2250738585072014e-308)
        catchup_clocks = (float)(lag_time / delta)  # should probably round up..oh well
        catchup_duration = catchup_clocks * min_sys_period
        return (catchup_duration)

    # Used to verify retime table
    def retime_check_table(self, count, sysclk_freq, synth_low_freq, synth_high_freq, valid_freq, model, table_idx_start):
        # /Make sure table is properly built
        start_checking = 0
        valid = 1
        sticky_valid = 1

        if (valid_freq <= sysclk_freq):
            self.retime_print(
                "ERROR: valid frequency %6.1f is less than system frequency %6.1f" % (valid_freq, sysclk_freq))
            return 0

        if (not count):
            self.retime_print("invalid: no table generated")
            return 0

        if (model.vars.lut_freq.value[table_idx_start] > synth_low_freq):
            sticky_valid = 0
            self.retime_print("ERROR : first entry frequency > lower limit %6.1f %6.1f" % (
            model.vars.lut_freq.value[table_idx_start], synth_low_freq))
            return 0

        if (model.vars.lut_freq_upper.value[table_idx_start + count - 1] < synth_high_freq):
            sticky_valid = 0
            self.retime_print("ERROR : last entry frequency < upper limit %6.1f %6.1f" % (
            model.vars.lut_freq_upper.value[table_idx_start + count - 1], synth_high_freq))
            return 0

        # Check each entry
        for k in range(count):
            valid = 1
            # Checks against previous entries
            num_clks = model.vars.lut_limitl.value[table_idx_start + k] + model.vars.lut_limith.value[table_idx_start + k]

            #print("k: %d" % (k))
            if ((k > 0) and (start_checking == 1)):
                # Check LimitL vs LimitH
                if (model.vars.lut_limitl.value[table_idx_start + k] < model.vars.lut_limith.value[table_idx_start + k]):
                    valid= 0
                    self.retime_print("ERROR: LimitL: {} is less than LimitH: {}".format(model.vars.lut_limitl.value[table_idx_start + k], model.vars.lut_limith.value[table_idx_start + k]))
                if model.vars.lut_limitl.value[table_idx_start + k] > 7:
                    valid= 0
                    self.retime_print("Warning: LimitL: {}, LimitH: {}  is bigger than range".format(model.vars.lut_limitl.value[table_idx_start + k], model.vars.lut_limith.value[table_idx_start + k]))

                # Check frequency sorted
                if (model.vars.lut_freq.value[table_idx_start + k] <= model.vars.lut_freq.value[table_idx_start + k - 1]):
                    valid = 0
                    self.retime_print("ERROR: Frequency not sorted: {}".format(table_idx_start + k))

                # Check that there is no consecutive invalids (except for first invalid)
                if ((not model.vars.lut_valid.value[table_idx_start + k]) and (not model.vars.lut_valid.value[table_idx_start + k - 1])):
                    valid = 0
                    self.retime_print("ERROR: Consecutive invalids: {}".format(table_idx_start + k))

                # Check that range is continuous across all entries
                if (model.vars.lut_freq.value[table_idx_start + k] != model.vars.lut_freq_upper.value[table_idx_start + k - 1]):
                    valid = 0
                    self.retime_print("ERROR: Range not contiguous: {}".format(table_idx_start + k))

                # Check entry frequency is increasing in the table
                if (model.vars.lut_freq.value[table_idx_start + k] <= model.vars.lut_freq.value[table_idx_start + k - 1]):
                    valid = 0
                    self.retime_print("ERROR: Not increasing in frequency: %f %f" % (
                    model.vars.lut_freq.value[table_idx_start + k], model.vars.lut_freq.value[table_idx_start + k - 1]))

                # Check entry number of clocks (limitl+limith) is incrementing in the table
                if (model.vars.lut_smuxdiv.value[table_idx_start + k] == prev_smuxdiv):
                    #                    print("prev_smuxdiv: %d" % (prev_smuxdiv))
                    if (num_clks != (prev_num_clks + 1)):
                        valid = 0
                        self.retime_print("ERROR: Num_clks not incrementing: %d %d" % (num_clks, prev_num_clks + 1))

            # Check num_clks increments between multiples
            if (model.vars.lut_valid.value[table_idx_start + k]):
                start_checking = 1
                prev_num_clks = num_clks
                prev_smuxdiv = model.vars.lut_smuxdiv.value[table_idx_start + k]
                #                print("k: %d, prev_smuxdiv: %d" % (k, prev_smuxdiv))

                if (model.vars.lut_freq_upper.value[table_idx_start + k] / num_clks / model.vars.lut_smuxdiv.value[table_idx_start + k] > valid_freq):
                    valid = 0
                    self.retime_print("ERROR: exceed max frequency %6.1f %6.1f: lut_freq[k]/num_clks" %
                                      (model.vars.lut_freq_upper.value[table_idx_start + k] / num_clks / model.vars.lut_smuxdiv.value[table_idx_start + k],
                                       valid_freq))

                catchup_duration = self.retime_calc_catchup_duration(sysclk_freq, model.vars.lut_freq.value[table_idx_start + k],
                                                                     model.vars.lut_smuxdiv.value[table_idx_start + k],
                                                                     model.vars.lut_limitl.value[table_idx_start + k] +
                                                                     model.vars.lut_limith.value[table_idx_start + k])

                if ((catchup_duration > MAX_CATCHUP_DURATION * 1.001) or (catchup_duration < 0)):
                    valid = 0
                    self.retime_print("ERROR: Catchup time failure: %6.1f exceeds %6.1f" %
                                      (catchup_duration, MAX_CATCHUP_DURATION))

        return sticky_valid

    def retime_main(self, model, dpll_frequency_hz, table_idx, start_table_idx):

        sysclk_freq = dpll_frequency_hz / 1000000
        lo_low_freq = math.floor(self.fvcomin / 2 / model.vars.lodiv_actual.value / 1000000)
        lo_high_freq = math.ceil(self.fvcomax / 2 / model.vars.lodiv_actual.value / 1000000)
        self.retime_print(
            "sysclk_freq: %f, lo_low_freq: %f, lo_high_freq: %f" % (sysclk_freq, lo_low_freq, lo_high_freq))

        model.vars.lut_dpll_freq_hz.value.append(int(sysclk_freq*1000000))
        if (sysclk_freq <= 40):
            # Max frequency in MHz @ 1.0V
            valid_freq = 46.0
        else:
            valid_freq = 89.4  # Max frequency in MHz @ 1.1V

        # Get starting multiple
        smuxdiv  = self.retime_calc_smuxdiv(lo_low_freq)
        multiple = math.floor(lo_low_freq/smuxdiv/valid_freq)
        multiple = multiple * smuxdiv

        # Generate Table
        while ((valid_freq * multiple) < lo_high_freq):
            (multiple) = self.return_retime_gen_entry(sysclk_freq, valid_freq, multiple, model, table_idx, start_table_idx)

        status = self.retime_check_table(model.vars.lut_table_index.value[table_idx], sysclk_freq, lo_low_freq, lo_high_freq,
                                         valid_freq, model, start_table_idx)

        if (not status):
            self.retime_print("!!!! ERRORs Found !!!!!! in calc_hfxo_retiming_table")

        self.retime_print(
            "Sysclk Freq = %4.1f. Internal Max Freq = %4.1f, Lower Synth Limit= %6.1f, Upper Synth Limit = %6.1f, Table count = %d" % (
            model.vars.lut_dpll_freq_hz.value[table_idx]/1000000.0, valid_freq, lo_low_freq, lo_high_freq, model.vars.lut_table_index.value[table_idx]))
        self.retime_print("Entry Synth Range  Valid SMUXDIV LIMITL LIMITH sysclk range Catchup(us)")
        for i in range(model.vars.lut_table_index.value[table_idx]):
            if (model.vars.lut_valid.value[start_table_idx + i]):
                min_freq = model.vars.lut_freq.value[start_table_idx + i] / (
                            (model.vars.lut_limitl.value[start_table_idx + i] + model.vars.lut_limith.value[start_table_idx + i]) * 1.0) / \
                           model.vars.lut_smuxdiv.value[start_table_idx + i]
                max_freq = model.vars.lut_freq_upper.value[start_table_idx + i] / (
                            (model.vars.lut_limitl.value[start_table_idx + i] + model.vars.lut_limith.value[start_table_idx + i]) * 1.0) / \
                           model.vars.lut_smuxdiv.value[start_table_idx + i]
            else:
                max_freq = 0
                min_freq = 0

            self.retime_print("%5d %6.1f %6.1f %5d %6d %6d %6d %6.1f %6.1f %7.3f" %
                              (start_table_idx + i, model.vars.lut_freq.value[start_table_idx + i], model.vars.lut_freq_upper.value[start_table_idx + i],
                               model.vars.lut_valid.value[start_table_idx + i], model.vars.lut_smuxdiv.value[start_table_idx + i],
                               model.vars.lut_limitl.value[start_table_idx + i], model.vars.lut_limith.value[start_table_idx + i], min_freq,
                               max_freq, self.retime_calc_catchup_duration(sysclk_freq, model.vars.lut_freq.value[start_table_idx + i],
                                                                           model.vars.lut_smuxdiv.value[start_table_idx + i], (
                                                                                       model.vars.lut_limitl.value[start_table_idx + i] +
                                                                                       model.vars.lut_limith.value[
                                                                                          start_table_idx +  i]))))

    def calc_hfxo_retiming_table(self, model):
        # Initialize Table
        model.vars.lut_table_index.value = [0]
        model.vars.lut_freq.value = []
        model.vars.lut_freq_upper.value = []
        model.vars.lut_valid.value = []
        model.vars.lut_smuxdiv.value = []
        model.vars.lut_limitl.value = []
        model.vars.lut_limith.value = []
        model.vars.lut_dpll_freq_hz.value = []

        #setting for 1x HFXO frequency
        self.retime_print("calculating 1x HFXO freq")
        self.retime_main(model, model.vars.xtal_frequency_hz.value, 0, 0)

        #setting for 2x HFXO frequency
        self.retime_print("calculating 2x HFXO freq")
        model.vars.lut_table_index.value.append(0)
        self.retime_main(model, model.vars.xtal_frequency_hz.value * 2, 1, model.vars.lut_table_index.value[0])