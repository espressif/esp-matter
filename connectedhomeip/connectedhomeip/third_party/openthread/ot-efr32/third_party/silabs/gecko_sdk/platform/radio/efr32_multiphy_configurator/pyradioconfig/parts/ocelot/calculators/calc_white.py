from pyradioconfig.parts.common.calculators.calc_white import CALC_Whitening

class CALC_Whitening_Ocelot(CALC_Whitening):
    def calc_white_settings(self, model):
        super().calc_white_settings(model)

        # PGOCELOT-5467 Fix whitening for TXTRAIL
        self._reg_write(model.vars.FRC_CTRL_SKIPTXTRAILDATAWHITEN, 0x0)
        self._reg_write(model.vars.FRC_CTRL_SKIPRXSUPSTATEWHITEN, 0x0)
