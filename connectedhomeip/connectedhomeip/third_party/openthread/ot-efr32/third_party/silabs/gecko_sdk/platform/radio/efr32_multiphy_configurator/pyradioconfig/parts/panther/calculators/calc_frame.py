from enum import Enum
from pyradioconfig.parts.common.calculators.calc_frame import CALC_Frame as CALC_Frame_common
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException


class CALC_Frame_panther(CALC_Frame_common):
    #inherit from common


    # Need to redefine for Panther as registers moved in Panther to FRC_FCDn_* to FRC_FCDn_FCD_*
    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        """
        #Inputs
        """
        # -------- General Frame Configurations --------
        # BIT_ORDER
        var = self._addModelVariable(model, 'frame_bitendian', Enum, ModelVariableFormat.DECIMAL,
                                     'Define how the payload bits are transmitted over the air')
        member_data = [
            ['LSB_FIRST', 0, 'Least significant bit is transmitted first over the air'],
            ['MSB_FIRST', 1, 'Most significant bit is transmitted first over the air'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'bitEndian',
            'Define how the payload bits are transmitted over the air',
            member_data)

        #FIRSTFRAME_BITSPERWORD
        self._addModelVariable(model, 'firstframe_bitsperword', int, ModelVariableFormat.DECIMAL, units='bits', desc='On reception, create the first received bytes from less than 8 bits. This can be used to "bitshift" the frame. Upper bits are padded with 0 in the downloaded frame.')


        # UART_MODE
        # self._addModelVariable(model, 'uart_coding', bool, ModelVariableFormat.ASCII, 'Set to true to enable uart coding of the frame.')

        # FRAME_LENGTH
        var = self._addModelVariable(model, 'frame_length_type', Enum, ModelVariableFormat.DECIMAL,
                                     'Possible Length Configurations')
        member_data = [
            ['FIXED_LENGTH', 0, 'The frame length is fixed and never changes'],
            ['VARIABLE_LENGTH', 1,
             'The frame length is determined by an explicit length field within the packet. Requires header to be enabled.'],
            ['FRAME_TYPE', 2,
             'The packet length is determined from an encoded set of bit that implicitly determines the length'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FrameLengthEnum',
            'List of supported frame length configurations',
            member_data)

        # -------- Payload Configurations --------
        # PAYLOAD_WHITE_EN
        self._addModelVariable(model, 'payload_white_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to whiten the payload')
        # PAYLOAD_CRC_EN
        self._addModelVariable(model, 'payload_crc_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to check/transmit crc after the payload')
        # ACCEPT_CRC_ERRORS
        self._addModelVariable(model, 'accept_crc_errors', bool, ModelVariableFormat.ASCII,
                               'Set to true if you want to accept invalid crcs')

        # Jumbo and Nerio specific, for now we have it here to avoid adding a new different class
        # PAYLOAD_ADDTRAILTXDATA_EN
        self._addModelVariable(model, 'payload_addtrailtxdata_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to add Trail TX data at the end of the frame')

        # Nerio specific, for now we have it here to avoid adding a new different class
        # PAYLOAD_EXCLUDESUBFRAMEWCNT_EN
        self._addModelVariable(model, 'payload_excludesubframewcnt_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to exclude words in the subframe from the Word Counter (WCNT), useful in Dynamic Frame Length (DFL) mode')

        # -------- Header Configurations --------
        # HEADER_ENABLE
        self._addModelVariable(model, 'header_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to enable a distinct header from the payload.')
        # HEADER_SIZE
        self._addModelVariable(model, 'header_size', int, ModelVariableFormat.DECIMAL, units='bytes',
                               desc='Define the number of bytes that make up the header. Include the variable length byte(s).')
        # HEADER_CALC_CRC
        self._addModelVariable(model, 'header_calc_crc', bool, ModelVariableFormat.ASCII,
                               'Set to true to include the header bytes in the payload CRC.')
        # HEADER_INCLUDE_CRC
        self._addModelVariable(model, 'header_include_crc', bool, ModelVariableFormat.ASCII,
                               'Set to true to check/transmit crc specifically for the header')
        # HEADER_WHITE_EN
        self._addModelVariable(model, 'header_white_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to enable whitening over the header')

        # Jumbo and Nerio specific, for now we have it here to avoid adding a new different class
        # HEADER_ADDTRAILTXDATA_EN
        self._addModelVariable(model, 'header_addtrailtxdata_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to add Trail TX data at the end of the frame')

        # Nerio specific, for now we have it here to avoid adding a new different class
        # HEADER_EXCLUDESUBFRAMEWCNT_EN
        self._addModelVariable(model, 'header_excludesubframewcnt_en', bool, ModelVariableFormat.ASCII,
                               'Set to true to exclude words in the subframe from the Word Counter (WCNT), useful in Dynamic Frame Length (DFL) mode')

        # -------- Fixed Length Configurations --------
        # FIXED_LENGTH_SIZE
        self._addModelVariable(model, 'fixed_length_size', int, ModelVariableFormat.DECIMAL, units='bytes',
                               desc='Define the number of bytes in the payload. This does not include the length of the header if used. Header + Payload must be less than 4096 bytes.')

        # -------- Variable Length Configurations --------
        # VARIABLE_LENGTH_NUMBITS
        self._addModelVariable(model, 'var_length_numbits', int, ModelVariableFormat.DECIMAL, units='bits',
                               desc='Define the size of the variable length field in bits.')
        # VARIABLE_LENGTH_BYTEENDIAN
        var = self._addModelVariable(model, 'var_length_byteendian', Enum, ModelVariableFormat.DECIMAL,
                                     'Define the byte endianness of the variable length field')
        member_data = [
            ['LSB_FIRST', 0,
             'The least significant byte of the variable length field is transmitted over the air first.'],
            ['MSB_FIRST', 1,
             'The most significant byte of the variable length field is transmitted over the air first.'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'VarLengthByteEndian',
            'Define how the variable length byte(s) are transmitted over the air',
            member_data)
        # VARIABLE_LENGTH_BITENDIAN
        var = self._addModelVariable(model, 'var_length_bitendian', Enum, ModelVariableFormat.DECIMAL,
                                     'Define the bit endianness of the variable length field')
        member_data = [
            ['LSB_FIRST', 0, 'The variable length field is transmitted least signficant bit first.'],
            ['MSB_FIRST', 1, 'The variable length field is transmitted most significant bit first.'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'VarLengthBitEndian',
            'Define how the variable length bits are transmitted over the air',
            member_data)
        # VARIABLE_LENGTH_SHIFT
        self._addModelVariable(model, 'var_length_shift', int, ModelVariableFormat.DECIMAL,
                               'Define the location of the least significant bit of the variable length field.')
        # VARIABLE_LENGTH_MINLENGTH
        self._addModelVariable(model, 'var_length_minlength', int, ModelVariableFormat.DECIMAL, units='bytes',
                               desc='Define the minimum value of the variable length field.')
        # VARIABLE_LENGTH_MAXLENGTH
        self._addModelVariable(model, 'var_length_maxlength', int, ModelVariableFormat.DECIMAL, units='bytes',
                               desc='Define the maximum value of the variable length field. Cannot exceed the variable length size.')
        # VARIABLE_LENGTH_INCLUDECRC
        self._addModelVariable(model, 'var_length_includecrc', bool, ModelVariableFormat.ASCII,
                               'Set to true if the crc bytes are included in the variable length')
        # VARIABLE_LENGTH_OFFSET
        self._addModelVariable(model, 'var_length_adjust', int, ModelVariableFormat.DECIMAL,
                               'Value to add to the variable length extracted from the packet when calculating the total payload length to receive.  A positive number here indicates the payload will be larger than the length value extracted from the variable length bits.')

        # -------- Frame Type Configurations --------
        # FRAME_TYPE
        self._addModelVariable(model, 'frame_type_loc', int, ModelVariableFormat.DECIMAL,
                               'Define the zero-based start location in the frame that holds the frame type encoding.')
        self._addModelVariable(model, 'frame_type_mask', int, ModelVariableFormat.HEX,
                               'Define the bitmask to extract the frame type in the byte.')
        self._addModelVariable(model, 'frame_type_bits', int, ModelVariableFormat.DECIMAL,
                               desc='Define the number of bits of the frame type field.', units='bits')
        self._addModelVariable(model, 'frame_type_lsbit', int, ModelVariableFormat.DECIMAL,
                               "Define the bit location of the frame type's least significant bit.")
        self._addModelVariable(model, 'frame_type_lengths', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', is_array=True, units='bytes')
        self._addModelVariable(model, 'frame_type_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.', is_array=True)
        self._addModelVariable(model, 'frame_type_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.', is_array=True)

        # @bug https://jira.silabs.com/browse/MCUW_RADIO_CFG-37
        # This is a temporary measure to not use is_array
        self._addModelVariable(model, 'frame_type_0_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_1_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_2_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_3_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_4_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_5_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_6_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')
        self._addModelVariable(model, 'frame_type_7_length', int, ModelVariableFormat.DECIMAL,
                               desc='Define the frame length of each frame type.', units='bytes')

        self._addModelVariable(model, 'frame_type_0_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_1_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_2_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_3_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_4_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_5_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_6_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')
        self._addModelVariable(model, 'frame_type_7_valid', bool, ModelVariableFormat.ASCII,
                               desc='Define the valid frame types.')

        self._addModelVariable(model, 'frame_type_0_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_1_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_2_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_3_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_4_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_5_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_6_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')
        self._addModelVariable(model, 'frame_type_7_filter', bool, ModelVariableFormat.ASCII,
                               desc='Define the frame types that should have address filtering applied.')

        """
        Internal variables
        """
        # VARIABLE_LENGTH_LOCATION
        self._addModelVariable(model, 'var_length_loc', int, ModelVariableFormat.DECIMAL,
                               'Define the zero-based start location in the header that holds the first byte of the variable length field.')
        # VARIABLE_LENGTH_NUMBYTES
        self._addModelVariable(model, 'var_length_numbytes', int, ModelVariableFormat.DECIMAL, units='bytes',
                               desc='Define the size of the variable length field in bytes.')

        # Internal HEADER_SIZE
        self._addModelVariable(model, 'header_size_internal', int, ModelVariableFormat.DECIMAL, units='bytes',
                               desc='Internal representation of header size. 0 for no header.')

        """
        #Outputs
        """
        self._addModelRegister(model, 'FRC.CTRL.BITSPERWORD', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.CTRL.RXFCDMODE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.CTRL.TXFCDMODE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.CTRL.BITORDER', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.CTRL.UARTMODE', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'FRC.WCNTCMP0.FRAMELENGTH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.WCNTCMP1.LENGTHFIELDLOC', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'FRC.DFLCTRL.DFLINCLUDECRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.DFLCTRL.MINLENGTH', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.DFLCTRL.DFLBITS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.DFLCTRL.DFLOFFSET', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.DFLCTRL.DFLSHIFT', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.DFLCTRL.DFLBITORDER', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.DFLCTRL.DFLMODE', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'FRC.MAXLENGTH.MAXLENGTH', int, ModelVariableFormat.HEX)

        # Moved in Panther to FRC_FCDn_* to FRC_FCDn_FCD_*

        # self._addModelRegister(model, 'FRC.FCD0.EXCLUDESUBFRAMEWCNT', int, ModelVariableFormat.HEX)
        # Duplicated elsewhere...
        # self._addModelRegister(model, 'FRC.FCD0.ADDTRAILTXDATA', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD0.SKIPWHITE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD0.SKIPCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD0.CALCCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD0.INCLUDECRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD0.BUFFER', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD0.WORDS', int, ModelVariableFormat.HEX)

        # self._addModelRegister(model, 'FRC.FCD1.EXCLUDESUBFRAMEWCNT', int, ModelVariableFormat.HEX)
        # self._addModelRegister(model, 'FRC.FCD1.ADDTRAILTXDATA', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.SKIPWHITE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.SKIPCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.CALCCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.INCLUDECRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.BUFFER', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD1.WORDS', int, ModelVariableFormat.HEX)

        # self._addModelRegister(model, 'FRC.FCD2.EXCLUDESUBFRAMEWCNT', int, ModelVariableFormat.HEX)
        # self._addModelRegister(model, 'FRC.FCD2.ADDTRAILTXDATA', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.SKIPWHITE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.SKIPCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.CALCCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.INCLUDECRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.BUFFER', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD2.WORDS', int, ModelVariableFormat.HEX)

        # self._addModelRegister(model, 'FRC.FCD3.EXCLUDESUBFRAMEWCNT', int, ModelVariableFormat.HEX)
        # self._addModelRegister(model, 'FRC.FCD3.ADDTRAILTXDATA', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.SKIPWHITE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.SKIPCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.CALCCRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.INCLUDECRC', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.BUFFER', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FRC.FCD3.WORDS', int, ModelVariableFormat.HEX)

    def _configure_fcd(self, model, fcdindex, excludesubframewcnt = 0, addtrailtxdata = 0, skipwhite = 0,
                                              skipcrc = 0, calccrc = 0, includecrc = 0, buf = 0, words = 0):

        """_configure_fcd

        Args:
            model (ModelRoot) : Data model to read and write variables from
            fcdindex (unknown) : unknown
            excludesubframewcnt (unknown) : unknown
            addtrailtxdata (unknown) : unknown
            skipwhite (unknown) : unknown
            skipcrc (unknown) : unknown
            calccrc (unknown) : unknown
            includecrc (unknown) : unknown
            buf (unknown) : unknown
            words (unknown) : unknown
        """

        # Moved in Panther to FRC_FCDn_* to FRC_FCDn_FCD_*

        self._reg_write(eval("model.vars.FRC_FCD{}_EXCLUDESUBFRAMEWCNT".format(fcdindex)), excludesubframewcnt)
        self._reg_write(eval("model.vars.FRC_FCD{}_ADDTRAILTXDATA".format(fcdindex)), addtrailtxdata)

        if model.vars.ber_force_whitening.value == True:
            self._reg_write(eval("model.vars.FRC_FCD{}_SKIPWHITE".format(fcdindex)), 0)
        else:
            self._reg_write(eval("model.vars.FRC_FCD{}_SKIPWHITE".format(fcdindex)), skipwhite)

        self._reg_write(eval("model.vars.FRC_FCD{}_SKIPCRC".format(fcdindex)), skipcrc)
        self._reg_write(eval("model.vars.FRC_FCD{}_CALCCRC".format(fcdindex)), calccrc)
        self._reg_write(eval("model.vars.FRC_FCD{}_INCLUDECRC".format(fcdindex)), includecrc)
        self._reg_write(eval("model.vars.FRC_FCD{}_BUFFER".format(fcdindex)), buf)
        self._reg_write(eval("model.vars.FRC_FCD{}_WORDS".format(fcdindex)), words)
        return

    def _calc_frame_length_defaults(self, model):
        """_calc_frame_length_defaults

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.FRC_WCNTCMP0_FRAMELENGTH, 0)
        self._reg_write(model.vars.FRC_WCNTCMP1_LENGTHFIELDLOC, 0)

        self._reg_write(model.vars.FRC_DFLCTRL_DFLINCLUDECRC, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_MINLENGTH, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_DFLBITS, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_DFLOFFSET, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_DFLSHIFT, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_DFLBITORDER, 0)
        if (model.vars.ber_force_infinite_length.value == True):
            # Infinite length
            self._reg_write(model.vars.FRC_DFLCTRL_DFLMODE, 5)
        else:
            self._reg_write(model.vars.FRC_DFLCTRL_DFLMODE, 0)

        # for Panther, do not write this to 0 -- PHYs override it
        self._reg_write(model.vars.FRC_MAXLENGTH_MAXLENGTH, 0)


    def calc_frame_length(self, model):
        """calc_frame_length

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        if (model.vars.frame_length_type.value == model.vars.frame_length_type.var_enum.FIXED_LENGTH):
            if (model.vars.header_en.value == True):
                self._calc_frame_length_defaults(model)
                self._fixed_length_with_header(model)
            else:
                self._calc_frame_length_defaults(model)
                self._fixed_length_no_header(model)

        elif (model.vars.frame_length_type.value == model.vars.frame_length_type.var_enum.VARIABLE_LENGTH and model.vars.header_en.value == True):
            #Variable Length requires headers
            self._configure_header(model)

            #Configure rest of payload options
            self._configure_payload_with_header(model)

            #Variable Length
            self._configure_variable_length(model)

            #Use FCD0/2 for first subframe then FCD1/3 is used for all following subframes
            self._reg_write(model.vars.FRC_CTRL_TXFCDMODE, 2)
            self._reg_write(model.vars.FRC_CTRL_RXFCDMODE, 2)
        elif (model.vars.frame_length_type.value == model.vars.frame_length_type.var_enum.FRAME_TYPE):
            self._calc_frame_length_defaults(model)
            self._configure_fcd_for_frame_type(model)

            #Frame Type
            self._configure_frame_type(model)
            pass
        return