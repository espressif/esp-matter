"""
This defines the Whitening variables and calculation

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pyradioconfig.parts.common.calculators.calc_frame_detect import CALC_Frame_Detect  # for the flip_bits routine
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum, ModelVariableEmptyValue, ModelVariableInvalidValueType
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

from py_2_and_3_compatibility import *

class CALC_Whitening(ICalculator):

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
        #-------- Frame Coding--------
        var = self._addModelVariable(model, 'frame_coding', Enum, ModelVariableFormat.DECIMAL, 'List of supported frame coding methods')
        member_data = [
            ['NONE',        0, 'No Frame Coding'],
            ['UART_NO_VAL', 1, 'UART Frame Coding without start/stop bit validation'],
            ['UART_VAL',    2, 'UART Frame Coding with start/stop bit validation'],
            ['MBUS_3OF6',   3, 'Mbus 3 of 6 coding']
        ]
        var.var_enum = CreateModelVariableEnum(
            'FrameCodingEnum',
            'List of supported Frame Coding Methods',
            member_data)

        """
        Internal variables
        """
        self._addModelVariable(model, 'frame_coding_array', int, ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model, 'frame_coding_message_bits', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'frame_coding_coded_bits', int, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'frame_coding_array_width', int, ModelVariableFormat.DECIMAL,
                               'Bytes required for coding table entries.  0=frame coding disabled, 8=one byte, 16=two bytes')
        self._addModelVariable(model, 'frame_coding_fshroutputsel_val', int, ModelVariableFormat.HEX)
        self._addModelVariable(model, 'frame_coding_poly_val', int, ModelVariableFormat.HEX)

        """
        #Outputs
        """
        self._addModelRegister(model, 'FRC.FECCTRL.BLOCKWHITEMODE', int, ModelVariableFormat.HEX )

        self._addModelVariable(model, 'frame_coding_array_packed', long, ModelVariableFormat.HEX, is_array=True)


    def _calc_init(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """


    def _frame_coding_none(self):
        return None


    def _frame_coding_UART(self):
        """_frame_coding_UART
        Function creates block coding array for UART frame coding

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        content = []
        for received in range(1024):
            if (received & 0x200) and (not (received & 0x001)):
                content.append((received >> 1) & 0xFF)
            else:
                content.append(0x8000 + ((received >> 1) & 0xFF))

        # Transmit table
        for i in range(256):
            content.append(0x200 + (i << 1))
        return content


    #
    # This implements the 3 of 6 encoding and decoding tables as defined in
    # \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\MBUS\En13757-4-2013_wMBUS_standard.pdf
    #
    # See table 10 on page 16 in section 6.4.2.1 for a table describing the details.
    #
    # This creates a table that first stores the 6-bit entries, followed by the 4-bit entries.
    #
    def _frame_coding_3of6(self):
        # List the lookup data with 4-bit sequence as keys, 6-bit sequence as values.
        coding_table = {
            0b0000: 0b010110,
            0b0001: 0b001101,
            0b0010: 0b001110,
            0b0011: 0b001011,

            0b0100: 0b011100,
            0b0101: 0b011001,
            0b0110: 0b011010,
            0b0111: 0b010011,

            0b1000: 0b101100,
            0b1001: 0b100101,
            0b1010: 0b100110,
            0b1011: 0b100011,

            0b1100: 0b110100,
            0b1101: 0b110001,
            0b1110: 0b110010,
            0b1111: 0b101001
        }

        message_bits = 4  # Later this could be pulled from the definition of the coding scheme
        coded_bits = 6    # Later this could be pulled from the definition of the coding scheme
        array_width = 8
        coding_table_is_msb_first = True  # Pull this from the frame format variable

        # Notice that everything below this line could be refactored into common code.  The only
        # thing specific to a given coding scheme is the table above and parameters above

        error_value = 1 << (array_width-1)

        # Calculate the tx table from the coded table.  It's just flipping the endianness of
        # the bits if the table was initially expressed in msb first format

        tx_map_table = dict()
        for message_value in coding_table.keys():
            coded_value = coding_table[message_value]
            # if the coding_table was entered msb first for sanity reasons,
            # flip the input and output values to match the hardware which is always lsb first
            if coding_table_is_msb_first:
                message_value = CALC_Frame_Detect.flip_bits(message_value, message_bits)
                coded_value = CALC_Frame_Detect.flip_bits(coded_value, coded_bits)

            tx_map_table[message_value] = coded_value


        # Create a receive data map from the transmit table.
        # Just flip the keys and values from the transmit table.
        rx_map_table = dict()
        # Now add the valid values from the tx map table to the rx_map_table
        for message_value in tx_map_table.keys():
            coded_value = tx_map_table[message_value]
            rx_map_table[coded_value] = message_value

        # Now create a list where we list all 64 receive values first, followed by the 16 transmit values.
        content = []

        # Receive table
        rx_table_size = 1 << coded_bits
        for rx_data in range(rx_table_size):
            if rx_data in rx_map_table.keys():
                content.append(int(rx_map_table[rx_data]))
            else:
                content.append(error_value)

        # Transmit table
        tx_table_size = 1 << message_bits
        for tx_data in range(tx_table_size):
            content.append(int(tx_map_table[tx_data]))

        return content


    def calc_frame_coding(self, model):
        """calc_frame_coding
        For coding schemes that use the block coding hardware,\n
        this is a dictionary lookup of each of the supported frame coding methods\n
        Each dictionary entry is a tuple which maps to the following entries:\n
          (starting bit size, coded bit size, bit width in table, coding array generation function)\n

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        FRAME_CODING_LOOKUP = {
            model.vars.frame_coding.var_enum.NONE.value: (0, 0, 0, self._frame_coding_none),
            model.vars.frame_coding.var_enum.UART_NO_VAL.value: (0, 0, 0, self._frame_coding_none),
            model.vars.frame_coding.var_enum.UART_VAL.value: (8, 10, 16, self._frame_coding_UART),
            model.vars.frame_coding.var_enum.MBUS_3OF6.value: (4, 6, 8, self._frame_coding_3of6)
        }

        # Need to use block coding in efr
        frameCodingParams = FRAME_CODING_LOOKUP[model.vars.frame_coding.value]
        model.vars.frame_coding_message_bits.value = frameCodingParams[0]
        model.vars.frame_coding_coded_bits.value = frameCodingParams[1]
        model.vars.frame_coding_array_width.value = frameCodingParams[2]
        model.vars.frame_coding_array.value = frameCodingParams[3]()
        pass

    def calc_uartmode(self, model):
        if model.vars.frame_coding.value == model.vars.frame_coding.var_enum.UART_NO_VAL.value:
            self._reg_write(model.vars.FRC_CTRL_UARTMODE, 1)
        else:
            self._reg_write(model.vars.FRC_CTRL_UARTMODE, 0)


    # The values that get written to the whitening registers will be calculated here.  We won't actually write
    # them to the registers here though.  These registers are shared with the whitening registers.  Set the variables
    # here, and write the registers over in the whitening code if whitening is disabled.
    def calc_frame_coding_reg_values(self, model):
        if model.vars.frame_coding_array_width.value > 0:
            model.vars.frame_coding_fshroutputsel_val.value = model.vars.frame_coding_message_bits.value - 1
            model.vars.frame_coding_poly_val.value = 1 << (model.vars.frame_coding_coded_bits.value - 1)
        else:
            model.vars.frame_coding_fshroutputsel_val.value = 0
            model.vars.frame_coding_poly_val.value = 0

    @staticmethod
    def pack_list(input_list, width):
        packed_list = list()
        total_bits = 0
        word_being_built = long(0)

        for list_item in input_list:
            #word_being_built = (word_being_built << width) | list_item                # Use this line for Big Endian
            word_being_built = (word_being_built >> width) | (list_item << (32-width)) # Use this line for Little Endian
            total_bits += width
            if total_bits % 32 == 0:
                packed_list.append(word_being_built)
                word_being_built = long(0)
        return packed_list

    def calc_frame_coding_array_packed(self, model):

            #Only perform this calculation when dealing with a frame_coding setting that produces a frame_coding_array
            #that is not None
            frame_coding = model.vars.frame_coding.value
            if frame_coding != model.vars.frame_coding.var_enum.NONE and \
                    frame_coding != model.vars.frame_coding.var_enum.UART_NO_VAL:

                coding_array = model.vars.frame_coding_array.value
                width = model.vars.frame_coding_array_width.value
                if width == 0:
                    model.vars.frame_coding_array_packed.value = None
                elif width == 8:
                    if (len(coding_array) % 4) != 0:
                        raise CalculationException("Frame coding array not word aligned!")
                    model.vars.frame_coding_array_packed.value = self.pack_list(coding_array, width)
                elif width == 16:
                    if (len(coding_array) % 2) != 0:
                        raise CalculationException("Frame coding array not word aligned!")
                    model.vars.frame_coding_array_packed.value = self.pack_list(coding_array, width)
                else:
                    raise CalculationException("Unexpected frame coding array width of %s!" % width)

    def calc_blockwhitemode(self, model):
        # This method calculates the FRC_FECCTRL_BLOCKWHITEMODE field

        #The LFSR used for whitening is also used for block coding forward-error-correction.
        # This means that it is not possible to perform both whitening and block coding on the same frame.

        #Read in model variables
        ber_force_whitening = model.vars.ber_force_whitening.value
        payload_white_en = model.vars.payload_white_en.value
        header_white_en = model.vars.header_white_en.value
        frame_coding_array_width = model.vars.frame_coding_array_width.value
        white_poly = model.vars.white_poly.value
        fec_enabled = model.vars.fec_enabled.value

        if ber_force_whitening:
            # Force whitening when desired for BER testing
            blockwhitemode = 1
        elif frame_coding_array_width > 0:
            # Using block coding
            blockwhitemode = 7
        elif payload_white_en or header_white_en:
            # Whitening is turned on for either header or payload
            if fec_enabled:
                if payload_white_en and header_white_en:
                    # Using FEC with whitening of header and payload
                    # Should work with interleaving enabled, need to verify operation with interleaving disabled
                    blockwhitemode = 3
                elif payload_white_en:
                    #Using FEC with whitening of only payload (skip whitening of first 16*interleavewidth bits)
                    blockwhitemode = 4
            elif 'byte' in white_poly.name.lower():
                # Using whitening polynomial enum corresponding to bytewhite
                blockwhitemode = 2
            else:
                # Standard whitening
                blockwhitemode = 1
        elif white_poly != model.vars.white_poly.var_enum.NONE:
            #If a whitening polynomial is present, then enable standard whitening
            #This still allows disabling whitening via the SKIPWHITE field in each FCD
            blockwhitemode = 1
        else:
            # Disable whitening
            blockwhitemode = 0

        #Write the register
        self._reg_write(model.vars.FRC_FECCTRL_BLOCKWHITEMODE,blockwhitemode)

