from pyradioconfig.parts.ocelot.calculators.calc_modulator import CALC_Modulator_Ocelot

class Calc_Modulator_Bobcat(CALC_Modulator_Ocelot):

    # Class variable to store the max PA value (needs to be correct for full power output)
    max_pa_value = 200