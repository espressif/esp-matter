from pyradioconfig.parts.ocelot.calculators.calc_freq_offset_comp import CALC_Freq_Offset_Comp_ocelot
from py_2_and_3_compatibility import *

class Calc_Freq_Offset_Comp_Bobcat(CALC_Freq_Offset_Comp_ocelot):

    def calc_offsub_reg(self, model):

        #calculate OFFSUBDEN and OFFSUBNUM register for more accurate frequency offset estimation
        #described in Section 5.7.7.1 of EFR32 Reference Manual (internal.pdf)

        # TODO: implement case when AFC is enabled
        rxbrfrac = 2*model.vars.rxbrfrac_actual.value
        timing_window = model.vars.timing_window_actual.value
        demod_sel = model.vars.demod_select.value

        # : Variables used in case of antenna diversity enabled
        enable_parallel_correlation = model.vars.antdiv_enable_parallel_correlation.value
        oversampling_rate_actual = model.vars.oversampling_rate_actual.value

        # : Calculate based on parallel window size if antenna diversity parallel correlator is enabled
        if enable_parallel_correlation:
            adpcwndsizechip = model.vars.antdiv_adpcwndsize.value
            exp = adpcwndsizechip * oversampling_rate_actual / 128.0
            diff_min = 9999.0
            num = 1
            num_max = 15
            best_num = 1
            best_den = 1
            while num <= num_max:
                den = 1
                while den < num:
                    res = 1.0 * num / den
                    if res <= exp:
                        diff = exp - res
                        if diff < diff_min:
                            diff_min = diff
                            best_num = num
                            best_den = den
                    den += 1
                num += 1

        elif(demod_sel==model.vars.demod_select.var_enum.LEGACY):
            timing_samples = timing_window * rxbrfrac
            closest_power_of_two = 2.0 ** (math.floor(math.log(timing_samples, 2)))

            if timing_samples == closest_power_of_two:
                best_den = 0
                best_num = 0
            else:
                error_min = 1e9

                # find best den, num pair that gets us closest to 2**N
                for den in xrange(1, 16):
                    for num in xrange(1, 16):

                        error = abs(closest_power_of_two * num / den - timing_samples)

                        if error < error_min:
                            error_min = error
                            best_den = den
                            best_num = num
        else:
            best_den = 0
            best_num = 0

        self._reg_write(model.vars.MODEM_TIMING_OFFSUBDEN, best_den)
        self._reg_write(model.vars.MODEM_TIMING_OFFSUBNUM, best_num)
