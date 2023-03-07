"""
 This file contains common input settings that might be shared across PHYs
"""

from py_2_and_3_compatibility import *

# Common Frame Inputs for IEEE 802.15.4
def PHY_COMMON_FRAME_154(phy, model):
    #Packet Inputs
    phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
    phy.profile_inputs.payload_white_en.value = False
    phy.profile_inputs.payload_crc_en.value = True

    #Variable length includes header
    phy.profile_inputs.header_en.value = True
    phy.profile_inputs.header_size.value = 1
    phy.profile_inputs.header_calc_crc.value = False
    phy.profile_inputs.header_white_en.value = False

    phy.profile_inputs.var_length_numbits.value = 8
    phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.var_length_shift.value = 0
    phy.profile_inputs.var_length_minlength.value = 5
    phy.profile_inputs.var_length_maxlength.value = 0x7F
    phy.profile_inputs.var_length_includecrc.value = True

    #CRC Inputs
    phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CCITT_16
    phy.profile_inputs.crc_seed.value = long(0x00000000)
    phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.LSB_FIRST
    phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
    phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
    phy.profile_inputs.crc_pad_input.value = False
    phy.profile_inputs.crc_invert.value = False

# Common Frame Configuration for Bluetooth Low Energy
def PHY_COMMON_FRAME_BLE(phy, model):

    # Settings applicable for Bluetooth Direct Test Mode (DTM)

    # Syncword-related inputs
    # From BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part F
    # 4.1.2 Preamble and Synchronization Word
    phy.profile_inputs.syncword_0.value = long(0x94826E8E)
    phy.profile_inputs.syncword_1.value = long(0x0)
    phy.profile_inputs.syncword_length.value = 32

    # For the PREAMBLE, preamble_pattern = 0b10 (decimal 2) translates into
    # MODEM_PRE_BASE = 0x1 because of bit-flipping
    phy.profile_inputs.preamble_pattern.value = 0b10 if phy.profile_inputs.syncword_0.value & (1<<31) != 0  else 0b01
    phy.profile_inputs.preamble_pattern_len.value = 2
    phy.profile_inputs.preamble_length.value = 8

    #Packet Inputs
    phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
    phy.profile_inputs.payload_white_en.value = False
    phy.profile_inputs.payload_crc_en.value = True

    #Variable length includes header
    phy.profile_inputs.header_en.value = True
    phy.profile_inputs.header_size.value = 2
    phy.profile_inputs.header_calc_crc.value = True
    phy.profile_inputs.header_white_en.value = False

    # NOTE: Currently the 'header_include_crc' variable is not part of the
    # profile inputs, so we can't set it here, instead we need to force the
    # output for both FCDs (TX/RX).
    phy.profile_outputs.FRC_FCD0_INCLUDECRC.override = 1
    phy.profile_outputs.FRC_FCD2_INCLUDECRC.override = 1

    # NOTE: Because BLE uses Dynamic Frame Length, we need to set the FCD_WORDS
    # field to 0xFF, but since that field is currently populated by header_size
    # input (wrong concept being applied), we also need to force the output for
    # both FCDs (TX/RX).
    phy.profile_outputs.FRC_FCD0_WORDS.override = 0xFF
    phy.profile_outputs.FRC_FCD2_WORDS.override = 0xFF

    phy.profile_inputs.var_length_numbits.value = 8
    phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.var_length_shift.value = 0
    phy.profile_inputs.var_length_minlength.value = 0
    phy.profile_inputs.var_length_maxlength.value = 255
    phy.profile_inputs.var_length_includecrc.value = False

    #CRC Inputs
    phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.BLE_24
    phy.profile_inputs.crc_seed.value = long(0x00555555) # Per BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part F. 4.1.3 CRC
    phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.LSB_FIRST
    phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
    phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
    phy.profile_inputs.crc_pad_input.value = False
    phy.profile_inputs.crc_invert.value = False

def PHY_COMMON_FRAME_IOHOME (phy, model):
    #Packet Inputs
    phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.frame_coding.value = model.vars.frame_coding.var_enum.UART_NO_VAL
    phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
    phy.profile_inputs.payload_white_en.value = False
    phy.profile_inputs.payload_crc_en.value = True

    #Variable length includes header
    phy.profile_inputs.header_en.value = True
    phy.profile_inputs.header_size.value = 1
    phy.profile_inputs.header_calc_crc.value = True
    phy.profile_inputs.header_white_en.value = False

    phy.profile_inputs.var_length_numbits.value = 5
    phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.var_length_shift.value = 0
    phy.profile_inputs.var_length_minlength.value = 0
    phy.profile_inputs.var_length_maxlength.value = 29
    phy.profile_inputs.var_length_includecrc.value = False

    #CRC Inputs
    phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CRC_16
    phy.profile_inputs.crc_seed.value = long(0x00000000)
    phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.MSB_FIRST
    phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
    phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
    phy.profile_inputs.crc_pad_input.value = False
    phy.profile_inputs.crc_invert.value = False

def PHY_COMMON_FRAME_INTERNAL(phy, model):
    # Frame Configuration
    #Frame Inputs
    phy.profile_inputs.fixed_length_size.value = 16
    phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.FIXED_LENGTH
    phy.profile_inputs.header_en.value = False
    phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST
    phy.profile_inputs.payload_crc_en.value = True
    phy.profile_inputs.payload_white_en.value = False

    #CRC Inputs
    phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
    phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.LSB_FIRST
    phy.profile_inputs.crc_seed.value = long(0x00000000)
    phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.LSB_FIRST
    phy.profile_inputs.crc_invert.value = False
    phy.profile_inputs.crc_pad_input.value = False
    phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CRC_16
