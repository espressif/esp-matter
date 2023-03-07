from pyradioconfig.parts.common.calculators.calc_ber import CALC_Ber

class CALC_Ber_Ocelot(CALC_Ber):

    def calc_test_ber(self,model):
        #This function sets a default value for the test_ber input

        #Set default value
        model.vars.test_ber.value = False

    def calc_per_forces(self,model):
        #This function doesn't do anything anyways, so override with nothing
        pass