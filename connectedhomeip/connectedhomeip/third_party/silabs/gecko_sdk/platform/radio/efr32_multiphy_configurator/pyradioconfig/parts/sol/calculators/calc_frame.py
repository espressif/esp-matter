from pyradioconfig.parts.ocelot.calculators.calc_frame import CALC_Frame_ocelot

class Calc_Frame_Sol(CALC_Frame_ocelot):

    def calc_ctrl_lpmodedis_reg(self, model):
        #This method calculates the LPMODEDIS field

        #Read in model vars
        demod_select = model.vars.demod_select.value

        # Calculate the field
        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            lpmodedis = 1
        else:
            lpmodedis = 0

        # Write the register
        self._reg_write(model.vars.FRC_CTRL_LPMODEDIS, lpmodedis)

    def _configure_variable_length(self, model):
        demod_select = model.vars.demod_select.value

        # Call old calculation first
        super(Calc_Frame_Sol, self)._configure_variable_length(model)

        if demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD:
            # Set FRC_DFLCTRL_DFLMODE to DISABLE mode for Soft Modem modulations
            # Preserving all other settings as they are still used by packet generator
            self._reg_write(model.vars.FRC_DFLCTRL_DFLMODE, 0)

    def _fixed_length_no_header(self, model):
        """_fixed_length_no_header

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # call function as defined in super class
        super()._fixed_length_no_header(model)

        # for soft modem FSK overwrite FCD0 settings
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_FSK:
            fcdDict = {
                "excludesubframewcnt": int(model.vars.payload_excludesubframewcnt_en.value == True),
                "addtrailtxdata": int(model.vars.payload_addtrailtxdata_en.value == True),
                "skipwhite": int(model.vars.payload_white_en.value == True),
                "skipcrc": 0,
                "calccrc": int(model.vars.payload_crc_en.value == False),
                "includecrc": int(model.vars.payload_crc_en.value == False),
                "words": 0xFF,
            }
            self._configure_fcd(model, fcdindex="0", buf=0, **fcdDict)

        return

    def calc_frame_length(self, model):
        """calc_frame_length

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        # handle special case for soft modem FSK otherwise call super class method
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_FSK:
            self._calc_frame_length_defaults(model)
            self._fixed_length_no_header(model)
        else:
            super().calc_frame_length(model)

        return

    def calc_sunfsk_frame_dont_cares(self, model):
        # This method sets all of the FCD1 and FCD3 fields to do not care for softmodem SUN FSK PHYs
        softmodem_modulation_type = model.vars.softmodem_modulation_type.value
        if softmodem_modulation_type == model.vars.softmodem_modulation_type.var_enum.SUN_FSK:
            for model_var in model.vars:
                if ("FRC_FCD1" in model_var.name) or ("FRC_FCD3" in model_var.name):
                        model_var.value_do_not_care = True