from pyradioconfig.parts.ocelot.calculators.calc_fec import CALC_FEC_Ocelot

class Calc_FEC_Sol(CALC_FEC_Ocelot):

    def calc_postamble_regs(self, model):
        demod_select = model.vars.demod_select.value
        fcs_type_802154 = model.vars.fcs_type_802154.value

        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            trailtxdata = 0
            trailtxdataforce = 1
            postambleen = 0
            trailtxdatacnt = 7

            if fcs_type_802154 == model.vars.fcs_type_802154.var_enum.TWO_BYTE:
                fcs_len_byte = 2
            else:
                fcs_len_byte = 4

            trailtxreplen = fcs_len_byte - 1

            self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATA, trailtxdata)
            self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE, trailtxdataforce)
            self._reg_write(model.vars.FRC_TRAILTXDATACTRL_POSTAMBLEEN, postambleen)
            self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT, trailtxdatacnt)
            self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXREPLEN, trailtxreplen)

        else:
            super(Calc_FEC_Sol, self).calc_postamble_regs(model)


    def calc_fecctrl_convmode_reg(self, model):
        #This function calculates the CONVMODE field

        #Read in model variables
        demod_select = model.vars.demod_select.value

        #Always disable for softmodem
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            self._reg_write(model.vars.FRC_FECCTRL_CONVMODE, 0)
        else:
            super().calc_fecctrl_convmode_reg(model)

    def calc_feccrl_interleavemode_reg(self, model):
        # always disable INTERLEAVEMODE for soft modem as interleaving is handled in the soft modem
        demod_select = model.vars.demod_select.value
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEMODE, 0)
        else:
            super().calc_feccrl_interleavemode_reg(model)

    def calc_fec_enabled(self, model):
        # For the soft modem FEC PHYs we like to have CONVMODE = 0 (so that no coding is done on the data as
        # this is handled by the soft modem) but fec_enabled = 1 because this determines the bitrate_gross which
        # factors in the FEC encoding.
        # beware of the circular logic: cannot key of demod_select model var here which would be ideal because
        # demod_select is a function of the baudrate which is a function fec_enabled

        #Read in model vars
        fec_tx_enable = model.vars.fec_tx_enable.value
        fec_en = model.vars.fec_en.value

        #FEC is enabled when FEC TX is enabled and the FEC configuration is set
        fec_enabled = (fec_tx_enable == model.vars.fec_tx_enable.var_enum.ENABLED) and (fec_en != model.vars.fec_en.var_enum.NONE)

        #Write the model var
        model.vars.fec_enabled.value = int(fec_enabled)




