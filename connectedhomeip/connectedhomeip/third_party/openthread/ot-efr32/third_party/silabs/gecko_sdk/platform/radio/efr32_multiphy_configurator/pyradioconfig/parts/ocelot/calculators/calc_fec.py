from pyradioconfig.parts.lynx.calculators.calc_fec import CALC_FEC_Lynx
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_FEC_Ocelot(CALC_FEC_Lynx):

    def __init__(self):
        #Call init of parent class
        super().__init__()

    def buildVariables(self, model):
        #First build all of the variables from the parent class
        super().buildVariables(model)

        #Now add variables
        self._addModelVariable(model, 'mbus_postamble_length', int, ModelVariableFormat.DECIMAL, 'Mbus postamble legnth in sets of two alternating chips')

    def _build_fec_var(self, model):
        # This function adds the fec_en enumerated variable. It is a separate method to allow overwriting in future parts.

        var = self._addModelVariable(model, 'fec_en', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported FEC Configurations')
        member_data = [
            ['NONE', 0, 'No FEC'],
            ['FEC_154G_NRNSC_INTERLEAVING',  1, '15.4G FEC settings with NRNSC and interleaving'],
            ['FEC_154G_RSC_INTERLEAVING', 2, '15.4G FEC settings with RSC and interleaving'],
            ['FEC_154G_RSC_NO_INTERLEAVING', 3, '15.4G FEC settings with RSC and no interleaving'],
            ['FEC_K7_INTERLEAVING', 4, 'FEC settings with K=7 with interleaving']
        ]
        var.var_enum = CreateModelVariableEnum(
            'FECEnum',
            'List of supported FEC Configurations',
            member_data)

    def calc_fec(self, model):
        #Need override method here due to added FEC_K7_INTERLEAVING case

        model.vars.fec_en.value = model.vars.fec_en.var_enum.NONE  # Calculate a default value for Profiles where this is an advanced input

        # Always initialize FEC regs to these values
        self._calc_init(model)

        # IF FEC is enabled then write the 802.15.4g base values
        if model.vars.fec_en.value != model.vars.fec_en.var_enum.NONE:
            self._FEC_154G_Base(model)

            # : FEC_154G_Base uses K=4 by default. Override code to K=7 case.
            if model.vars.fec_en.value == model.vars.fec_en.var_enum.FEC_K7_INTERLEAVING:
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0x6D)
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0x4F)

    def calc_fec_enabled(self, model):
        model.vars.fec_enabled.value = int(model.vars.FRC_FECCTRL_CONVMODE.value != 0)

    def calc_postamble_regs(self, model):
        # This function calculate  registers to configure the postamble of PHYs with FEC enabled and
        # Mbus Mode T/Mode S/Mode R PHYs. All other PHYs are not affected

        # Write the Mbus postamble length as 0 by default (overridden by Mbus Profile Input)
        model.vars.mbus_postamble_length.value = 0
        mbus_symbol_encoding = model.vars.mbus_symbol_encoding.value
        profile = model.profile.name.lower()
        fec_enable = model.vars.fec_en.value
        postamble_length = model.vars.mbus_postamble_length.value

        if profile == 'mbus':
            if mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.Manchester:
                trailtxdataforce = 1
                trailtxdata = 0xff
                postambleen = 0
                trailtxreplen = 0
                # model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT.value_forced = 0  # shortest allowed postamble is 2 chip
                # Setting total number of chips, Due to Manchester encoding
                # the actual number of chips used is (FRC_TRAILTXDATACTRL_TRAILTXDATACNT + 1)*2
                if postamble_length == 1:
                    trailtxdatacnt = 0
                elif postamble_length == 2:
                    trailtxdatacnt = 1
                elif postamble_length == 3:
                    trailtxdatacnt = 2
                elif postamble_length == 4:
                    trailtxdatacnt = 3
                else:
                    trailtxdatacnt = 0
            elif mbus_symbol_encoding == model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6:
                trailtxdataforce = 1
                trailtxdata = 0
                postambleen = 1
                trailtxreplen = 0
                # Setting total number of chips, the actual number of chips used is FRC_TRAILTXDATACTRL_TRAILTXDATACNT + 1
                if postamble_length == 1:
                    trailtxdatacnt = 1
                elif postamble_length == 2:
                    trailtxdatacnt = 3
                elif postamble_length == 3:
                    trailtxdatacnt = 5
                elif postamble_length == 4:
                    trailtxdatacnt = 7
                else:
                    trailtxdatacnt = 1
            else:
                trailtxdataforce = 0
                trailtxdata = 0
                trailtxdatacnt = 0
                postambleen = 0
                trailtxreplen = 0
        elif fec_enable != model.vars.fec_en.var_enum.NONE:
            trailtxdata = 0x0B
            trailtxdatacnt = 0
            postambleen = 0
            trailtxdataforce = 0
            trailtxreplen = 0
        else:
            trailtxdataforce = 0
            trailtxdata = 0
            trailtxdatacnt = 0
            postambleen = 0
            trailtxreplen = 0

        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATA, trailtxdata)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE, trailtxdataforce)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_POSTAMBLEEN, postambleen)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT, trailtxdatacnt)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXREPLEN, trailtxreplen)

    def calc_feccrl_interleavemode_reg(self, model):
        # This function calculates the FRC_FECCTRL_INTERLEAVEMODE reg field
        # A unique version is needed here due to the added FEC_K7_INTERLEAVING mode

        # Read in model variables
        fec_en = model.vars.fec_en.value
        fec_enabled = model.vars.fec_enabled.value

        #For dynamic FEC case, RAIL will handle reading in this interleavemode and enabling RX buffering
        #(changing to interleavemode=2) as needed
        if fec_enabled:
            if (fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING) or \
                    (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING) or \
                    (fec_en == model.vars.fec_en.var_enum.FEC_K7_INTERLEAVING):
                #We need to turn on interleaving to TX properly
                interleavemode = 1
            else:
                interleavemode = 0
        else:
            interleavemode = 0

        # Load value into register
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEMODE, interleavemode)

    def calc_convdecodemode_reg(self, model):
        # This function calculates the CONVDECODEMODE (hard vs soft decision symbols)

        # Assign inputs to variables
        fec_en = model.vars.fec_en.value
        fec_enabled = model.vars.fec_enabled.value

        if fec_enabled or (fec_en != model.vars.fec_en.var_enum.NONE):
            convdecodemode = 1  # hard decision (default in most cases)
        else:
            convdecodemode = 0

        # Write the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVDECODEMODE, convdecodemode)