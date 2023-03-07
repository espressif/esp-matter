from pyradioconfig.parts.ocelot.calculators.calc_frame_coding import CALC_Frame_Coding_Ocelot

class Calc_Frame_Coding_Sol(CALC_Frame_Coding_Ocelot):

    def calc_blockwhitemode(self, model):

        # if soft modem FSK case set whitening mode to 0 else call super class method
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_FSK:
            self._reg_write(model.vars.FRC_FECCTRL_BLOCKWHITEMODE, 0)
        else:
            super().calc_blockwhitemode(model)

