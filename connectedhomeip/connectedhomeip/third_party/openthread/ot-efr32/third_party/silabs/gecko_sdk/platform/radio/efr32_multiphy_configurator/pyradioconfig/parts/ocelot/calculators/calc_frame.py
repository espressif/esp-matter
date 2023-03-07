from pyradioconfig.parts.panther.calculators.calc_frame import CALC_Frame_panther

# This file inherits from Panther instead of Lynx because Ocelot does not need
# the workaround for PGLYNX-2670
class CALC_Frame_ocelot(CALC_Frame_panther):

    def calc_possible_future_inputs(self, model):
        #Copied from Commmon but removed excludesubframewcnt_en variables as those will be calculated in another method

        model.vars.header_include_crc.value = False
        model.vars.header_addtrailtxdata_en.value = False
        model.vars.payload_addtrailtxdata_en.value = False

        return

    def calc_excludesubframewcnt_en(self, model):
        # This function calculates the excludesubframewcnt_en variables for header and payload
        # based on whether we are in BER test mode or not

        # Read in model variables
        ber_force_infinite_length = model.vars.ber_force_infinite_length.value

        #Calculate based on BER test mode
        if ber_force_infinite_length:
            header_excludesubframewcnt_en = True
            payload_excludesubframewcnt_en = True
        else:
            header_excludesubframewcnt_en = False
            payload_excludesubframewcnt_en = False

        #Write to model variables
        model.vars.header_excludesubframewcnt_en.value = header_excludesubframewcnt_en
        model.vars.payload_excludesubframewcnt_en.value = payload_excludesubframewcnt_en

    def calc_ctrl_lpmodedis_reg(self, model):
        # This method calculates the LPMODEDIS field

        # Calculate the field
        lpmodedis = 0

        # Write the register
        self._reg_write(model.vars.FRC_CTRL_LPMODEDIS, lpmodedis)