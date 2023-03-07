"""
This defines fec configurations and calculations
This file also houses calculations for fields that are affected by multiiple blocks:
  calc_blockwhitemode(): calc_whiten, calc_fec

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
import math
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum, ModelVariableEmptyValue, ModelVariableInvalidValueType

class CALC_FEC(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        """
        #Inputs
        """

        #Call the function to build the fec_en variable
        self._build_fec_var(model)

        var = self._addModelVariable(model, 'fec_tx_enable', Enum, ModelVariableFormat.DECIMAL, 'FEC enable')
        member_data = [
            ['DISABLED', 0, 'FEC Disabled'],
            ['ENABLED', 1, 'FEC Enabled'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FECTxEnableEnum',
            'FEC TX Enable/Disable Selection',
            member_data)

        var = self._addModelVariable(model, 'dynamic_fec_enable', Enum, ModelVariableFormat.DECIMAL,
                                     'Enable dynamic FEC based on syncword')
        member_data = [
            ['DISABLED', 0, 'Dynamic FEC Disabled'],
            ['ENABLED', 1, 'Dynamic FEC Enabled'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'DynamicFecEnum',
            'Dynamic FEC Enable/Disable Selection',
            member_data)

        """
        #Outputs
        """
        self._addModelRegister(model, 'FRC.FECCTRL.CONVMODE', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.CONVDECODEMODE', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.CONVTRACEBACKDISABLE', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.CONVINV', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.INTERLEAVEMODE', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.INTERLEAVEFIRSTINDEX', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.INTERLEAVEWIDTH', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.CONVBUSLOCK', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.CONVSUBFRAMETERMINATE', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.SINGLEBLOCK', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.FORCE2FSK', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.FECCTRL.CONVHARDERROR', int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'FRC.TRAILTXDATACTRL.TRAILTXDATA', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.TRAILTXDATACTRL.TRAILTXDATACNT', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.TRAILTXDATACTRL.TRAILTXDATAFORCE', int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'FRC.CONVGENERATOR.GENERATOR0', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.CONVGENERATOR.GENERATOR1', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.CONVGENERATOR.RECURSIVE', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.CONVGENERATOR.NONSYSTEMATIC', int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'FRC.PUNCTCTRL.PUNCT0', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FRC.PUNCTCTRL.PUNCT1', int, ModelVariableFormat.HEX )

        # Output software variables for RAIL to consume
        self._addModelVariable(model, 'frc_conv_decoder_buffer_size', int, ModelVariableFormat.DECIMAL, units='bytes', desc='Size (in bytes) of the buffer necessary for the Convolutional Decoder')
        self._addModelVariable(model, 'fec_enabled', int, ModelVariableFormat.DECIMAL, 'FEC enabled flag')

    def _build_fec_var(self, model):
        # This function adds the fec_en enumerated variable. It is a separate method to allow overwriting in future parts.

        var = self._addModelVariable(model, 'fec_en', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported FEC Configurations')
        member_data = [
            ['NONE', 0, 'No FEC'],
            ['FEC_154G', 1, '(legacy not recommended) 15.4G FEC settings'],
            ['FEC_154G_K7', 2, '(legacy not recommended) 15.4G FEC settings with K=7'],
            ['FEC_154G_NRNSC_INTERLEAVING', 3, '15.4G FEC settings with NRNSC and interleaving'],
            ['FEC_154G_RSC_INTERLEAVING', 4, '15.4G FEC settings with RSC and interleaving'],
            ['FEC_154G_RSC_NO_INTERLEAVING', 5, '15.4G FEC settings with RSC and no interleaving'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FECEnum',
            'List of supported FEC Configurations',
            member_data)

    def _calc_init(self, model):
        # Need to override this method in order to instead set CONVMODE elsewhere

        self._reg_write(model.vars.FRC_FECCTRL_CONVTRACEBACKDISABLE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEFIRSTINDEX, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVBUSLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVSUBFRAMETERMINATE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_SINGLEBLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_FORCE2FSK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVHARDERROR, 0)

        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0)
        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0)
        self._reg_write(model.vars.FRC_CONVGENERATOR_NONSYSTEMATIC, 0)

        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT0, 0)
        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT1, 0)

    def _FEC_154G_Base(self, model):

        self._reg_write(model.vars.FRC_FECCTRL_CONVTRACEBACKDISABLE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEFIRSTINDEX, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVBUSLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVSUBFRAMETERMINATE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_SINGLEBLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_FORCE2FSK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVHARDERROR, 0)

        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0x0F)
        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0x0D)
        self._reg_write(model.vars.FRC_CONVGENERATOR_NONSYSTEMATIC, 0)

        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT0, 1)
        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT1, 1)

    def calc_fec(self, model):
        """calc_fec

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.fec_en.value = model.vars.fec_en.var_enum.NONE #Calculate a default value for Profiles where this is an advanced input

        #Always initialize FEC regs to these values
        self._calc_init(model)

        # IF FEC is enabled then write the 802.15.4g base values
        if model.vars.fec_en.value != model.vars.fec_en.var_enum.NONE:
            self._FEC_154G_Base(model)

            if model.vars.fec_en.value == model.vars.fec_en.var_enum.FEC_154G:
                # Settings are only a minor departure from _FEC_154G_Base since the legacy fec_en modes were incorrect
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0x0D)
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0x0E)

            elif (model.vars.fec_en.value == model.vars.fec_en.var_enum.FEC_154G_K7):
                # Settings are only a minor departure from _FEC_154G_Base since the legacy fec_en modes were incorrect
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0x6D)
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0x4F)

        model.vars.fec_enabled.value = int(model.vars.FRC_FECCTRL_CONVMODE.value != 0)

    def calc_feccrl_interleavemode_reg(self, model):
        # This function calculates the FRC_FECCTRL_INTERLEAVEMODE reg field

        # Read in model variables
        fec_en = model.vars.fec_en.value
        fec_enabled = model.vars.fec_enabled.value

        # For dynamic FEC case, RAIL will handle reading in this interleavemode and enabling RX buffering
        # (changing to interleavemode=2) as needed
        if fec_enabled:
            if (fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING) or \
                    (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING) or \
                    (fec_en == model.vars.fec_en.var_enum.FEC_154G) or \
                    (fec_en == model.vars.fec_en.var_enum.FEC_154G_K7):
                # We need to turn on interleaving to TX properly
                interleavemode = 1
            else:
                interleavemode = 0
        else:
            interleavemode = 0

        # Load value into register
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEMODE, interleavemode)

    def calc_fecctrl_convinv_reg(self, model):
        # This function calculates the FRC_FECCTRL_CONVINV reg field

        # Read in model variables
        fec_en = model.vars.fec_en.value

        # Calculate the output bits to invert based on the fec_en mode
        if fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING:
            convinv = 3  # Invert both bits
        else:
            convinv = 0

        # Write the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVINV, convinv)

    def calc_convgenerator_recursive_reg(self, model):
        # This function calculates the FRC_CONVGENERATOR_RECURSIVE reg field

        # Read in model variables
        fec_en = model.vars.fec_en.value

        if (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING) or (
                fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_NO_INTERLEAVING):
            recursive = 1
        else:
            recursive = 0

        # Load value into register
        self._reg_write(model.vars.FRC_CONVGENERATOR_RECURSIVE, recursive)

    def calc_fecctrl_interleavewidth_reg(self, model):
        #This function calculates the FRC_FECCTRL_INTERLEAVEWIDTH reg field

        #Read in model variables
        modulation_type = model.vars.modulation_type.value
        fec_en = model.vars.fec_en.value

        if fec_en != model.vars.fec_en.var_enum.NONE:
            if modulation_type == model.vars.modulation_type.var_enum.FSK2:
                interleavewidth = 1
            elif modulation_type == model.vars.modulation_type.var_enum.FSK4:
                interleavewidth = 0
            else:
                interleavewidth = 0
        else:
            interleavewidth = 0

        #Load value into register
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEWIDTH, interleavewidth)

    def calc_convdecodemode_reg(self, model):
        # This function calculates the CONVDECODEMODE (hard vs soft decision symbols)

        # Assign inputs to variables
        fec_enabled = model.vars.fec_enabled.value
        dsss_bits_per_symbol = model.vars.dsss_bits_per_symbol.value

        if fec_enabled and dsss_bits_per_symbol > 2:
            convdecodemode = 1  # hard decision
        else:
            convdecodemode = 0  # soft decision

        # Write the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVDECODEMODE, convdecodemode)

    def calc_fecctrl_convmode_reg(self, model):
        #This function calculates the CONVMODE field

        #Read in model variables
        fec_tx_enable = (model.vars.fec_tx_enable.value == model.vars.fec_tx_enable.var_enum.ENABLED)

        #Calculate the register
        if fec_tx_enable:
            convmode = 1
        else:
            convmode = 0

        #Set the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVMODE, convmode)

    def calc_fec_tx_enable(self, model):
        # This function enables or disables FEC in TX
        # It is used so that we can configure FEC type with fec_en but still disable on TX if desired

        # Read in model variables
        fec_en = model.vars.fec_en.value

        # By default enable FEC TX whenever a customer selects a fec_en setting that is not NONE
        if fec_en != model.vars.fec_en.var_enum.NONE:
            fec_tx_enable = model.vars.fec_tx_enable.var_enum.ENABLED
        else:
            fec_tx_enable = model.vars.fec_tx_enable.var_enum.DISABLED

        # Write the variable
        model.vars.fec_tx_enable.value = fec_tx_enable

    def calc_postamble_regs(self, model):
        # This function calculate  registers to configure the postamble of PHYs with FEC enabled

        fec_en = model.vars.fec_en.value

        if (fec_en == model.vars.fec_en.var_enum.NONE) or (fec_en == model.vars.fec_en.var_enum.FEC_154G) or \
                (fec_en == model.vars.fec_en.var_enum.FEC_154G_K7):
            trailtxdata = 0
        else:
            trailtxdata = 0x0B

        #Always set these to 0 for now
        trailtxdatacnt = 0
        trailtxdataforce = 0

        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATA, trailtxdata)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE, trailtxdataforce)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT, trailtxdatacnt)

    def calc_convolutional_decoder_buffer_size(self, model):
        """
        calc_convolutional_decoder_buffer_size

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # This whole calculation only applies if convolutional Encoder/Decoder is enabled
        if model.vars.FRC_FECCTRL_CONVMODE != 0:

            # From Reference Manual, section 5.8.16.4 Convolutional decoder,
            # 'Convolutional decoding RAM buffer size' table
            # Constraint length : RAM size (bytes)
            convDecodRamBufSize = {0:0,
                                   1:0,
                                   2:16,
                                   3:32,
                                   4:64,
                                   5:128,
                                   6:384,
                                   7:768}

            # Get the value set in FRC_CONVGENERATOR_GENERATOR0/1
            generator0 = model.vars.FRC_CONVGENERATOR_GENERATOR0.value
            generator1 = model.vars.FRC_CONVGENERATOR_GENERATOR1.value

            # Make sure the values are >= 1
            generator0 = 1 if generator0 < 1 else generator0
            generator1 = 1 if generator1 < 1 else generator1

            # Get the MSB set in the generator0/generator1 variables
            # We use ceil to round up any fractional value
            generator0_constraintLength = int(math.ceil(math.log(generator0, 2)))
            generator1_constraintLength = int(math.ceil(math.log(generator1, 2)))

            # Get the larger of the two MSBs obtained in the previous step and use that
            # as the key to lookup the required buffer size in convDecodRamBufSize dict
            constraintLength = max(generator0_constraintLength, generator1_constraintLength)
            model.vars.frc_conv_decoder_buffer_size.value = convDecodRamBufSize[constraintLength]
