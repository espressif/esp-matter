from pyradioconfig.parts.ocelot.calculators.calc_demodulator import CALC_Demodulator_ocelot
from math import *


class CALC_Demodulator_Margay(CALC_Demodulator_ocelot):

    # override inhereited variable defining threshold for _channel_filter_clocks_valid
    chf_required_clks_per_sample = 3  # fixed in PGMARGAY-1557

    def _check_trecs_required_clk_cycles(self, adc_freq, baudrate, osr, dec0, dec1, xtal_frequency_hz, relaxsrc2,
                                         model):
        # Returns True if the filter chain configuration meets the requirement for trecs
        # minimum clock cycles between samples. Returns False if the configuration is invalid
        #
        # PGMARGAY-106 added configurable clk spacing for interpolated samples. Compute minimum sample clk spacing.
        # (same as sol) but since margay inherits from ocelot, copy here.

        # calculate the src_ratio as this function is called in the process of evaluating
        # osr, dec0, dec1, so the src_ratio_actual cannot be calculated
        dec1_freq = adc_freq / (8 * dec0 * dec1)
        src_freq = baudrate * osr
        src_ratio = src_freq / dec1_freq
        TRECS_REQUIRED_CLKS_PER_SAMPLE = 3

        if src_ratio > 1:
            # sample clk is not equi-spaced, interpolated samples have best-case spacing of 1/2 relative to src disabled
            # while using UPGAPS clk delay adjustment in the SRW CMU
            cycles_per_sample = floor(xtal_frequency_hz / src_freq / 2)
        else:
            cycles_per_sample = floor(xtal_frequency_hz / src_freq)

        if cycles_per_sample < TRECS_REQUIRED_CLKS_PER_SAMPLE:
            return False
        else:
            return True