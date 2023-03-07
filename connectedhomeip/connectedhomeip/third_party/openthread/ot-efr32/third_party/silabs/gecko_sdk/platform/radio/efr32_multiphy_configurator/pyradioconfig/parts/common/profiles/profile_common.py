from pycalcmodel.core.output import ModelOutput, ModelOutputType
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile

from py_2_and_3_compatibility import *

"""
Generic template to build a Model Output based on a hardware register
"""
def _buildModelOutputStringFromRegisterField(hw_string, category):
    return "ModelOutput(model.vars.{0}, '{1}', ModelOutputType.SVD_REG_FIELD, readable_name = '{0}')".format(hw_string, category)

"""
Builds the inputs and outputs of the CRC block
"""
def buildCrcInputs(model, profile):
    IProfile.make_optional_input(profile, model.vars.crc_poly,        'crc', readable_name="CRC Polynomial",        default=model.vars.crc_poly.var_enum.CRC_16 )
    IProfile.make_optional_input(profile, model.vars.crc_seed,        'crc', readable_name="CRC Seed",              default=long(0),   value_limit_min=long(0), value_limit_max=long(0xFFFFFFFF))
    IProfile.make_optional_input(profile, model.vars.crc_byte_endian, 'crc', readable_name="CRC Byte Endian",       default=model.vars.crc_byte_endian.var_enum.MSB_FIRST)
    IProfile.make_optional_input(profile, model.vars.crc_bit_endian,  'crc', readable_name="CRC Output Bit Endian", default=model.vars.crc_bit_endian.var_enum.MSB_FIRST)
    IProfile.make_optional_input(profile, model.vars.crc_pad_input,   'crc', readable_name="CRC Input Padding",     default=False)
    IProfile.make_optional_input(profile, model.vars.crc_input_order, 'crc', readable_name="CRC Input Bit Endian",  default=model.vars.crc_input_order.var_enum.LSB_FIRST)
    IProfile.make_optional_input(profile, model.vars.crc_invert,      'crc', readable_name="CRC Invert",            default=False)

def buildCrcOutputs(model, profile, family):
    # These are named differently in 90nm parts vs Panther
    if family in ["dumbo", "jumbo", "nerio", "nixi"]:
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_PADCRCINPUT', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_BITSPERWORD', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_BITREVERSE', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_BYTEREVERSE', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_INPUTBITORDER', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_CRCWIDTH', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_CTRL_OUTPUTINV', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_INIT_INIT', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('CRC_POLY_POLY', 'crc')))
    else:
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_PADCRCINPUT', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_BITSPERWORD', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_BITREVERSE', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_BYTEREVERSE', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_INPUTBITORDER', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_CRCWIDTH', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_CTRL_OUTPUTINV', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_INIT_INIT', 'crc')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('RFCRC_POLY_POLY', 'crc')))

"""
Builds the inputs and outputs of the Whitening block
"""
def buildWhiteInputs(model, profile):
    IProfile.make_optional_input(profile, model.vars.white_poly, 'whitening', default=model.vars.white_poly.var_enum.NONE, readable_name="Whitening Polynomial")
    IProfile.make_optional_input(profile, model.vars.white_seed, 'whitening', default=0x0000FFFF, readable_name="Whitening Seed", value_limit_min=0, value_limit_max=0xFFFF)
    IProfile.make_optional_input(profile, model.vars.white_output_bit, 'whitening', default=0, readable_name="Whitening Output Bit", value_limit_min=0, value_limit_max=0x0F)

def buildWhiteOutputs(model, profile):
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WHITECTRL_SHROUTPUTSEL', 'whitening')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WHITECTRL_XORFEEDBACK', 'whitening')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WHITECTRL_FEEDBACKSEL', 'whitening')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WHITEPOLY_POLY', 'whitening')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WHITEINIT_WHITEINIT', 'whitening')))

"""
Builds the inputs and outputs of the FEC block
"""
def buildFecInputs(model, profile):

    IProfile.make_optional_input(profile, model.vars.fec_en, 'Channel_Coding', default=model.vars.fec_en.var_enum.NONE, readable_name="FEC Algorithm")

def buildFecOutputs(model, profile):
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVMODE', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVDECODEMODE', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVTRACEBACKDISABLE', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVINV', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_INTERLEAVEMODE', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_INTERLEAVEFIRSTINDEX', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_INTERLEAVEWIDTH', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVBUSLOCK', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVSUBFRAMETERMINATE', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_SINGLEBLOCK', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_FORCE2FSK', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_CONVHARDERROR', 'fec')))

    # NOTE: We are purposely removing FRC_CONVRAMADDR_CONVRAMADDR from the profile outputs because it needs to be
    # written by the SW to a buffer allocated in a specific RAM region, no point simply writing it to 0x00000000
    # profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CONVRAMADDR_CONVRAMADDR', 'fec')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_TRAILTXDATACTRL_TRAILTXDATA', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_TRAILTXDATACTRL_TRAILTXDATACNT', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE', 'fec')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CONVGENERATOR_GENERATOR0', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CONVGENERATOR_GENERATOR1', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CONVGENERATOR_RECURSIVE', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CONVGENERATOR_NONSYSTEMATIC', 'fec')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_PUNCTCTRL_PUNCT0', 'fec')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_PUNCTCTRL_PUNCT1', 'fec')))

"""
Builds the inputs and outputs of the general frame settings
"""
def buildFrameInputs(model, profile, family):
    MIN_FRAME_LENGTH = 1
    MAX_FRAME_LENGTH = 0x7fffffff
    MIN_HEADER_LENGTH = 1
    MAX_HEADER_LENGTH = 254
    MAX_FRAME_TYPE_LENGTH = 0xffff
    #Inputs
    IProfile.make_optional_input(profile, model.vars.frame_bitendian, 'frame_general', default=model.vars.frame_bitendian.var_enum.LSB_FIRST, readable_name="Frame Bit Endian")
    IProfile.make_optional_input(profile, model.vars.frame_length_type, 'frame_general', default=model.vars.frame_length_type.var_enum.FIXED_LENGTH, readable_name="Frame Length Algorithm")
    IProfile.make_optional_input(profile, model.vars.header_en, 'frame_general', default=False, readable_name="Header Enable")
    IProfile.make_optional_input(profile, model.vars.frame_coding, 'frame_general', default=model.vars.frame_coding.var_enum.NONE, readable_name="Frame Coding Method")
    # IProfile.make_required_input(profile, model.vars.accept_crc_errors, 'frame', default=False)

    # -- Payload --
    IProfile.make_optional_input(profile, model.vars.payload_white_en, 'frame_payload', default=False, readable_name="Payload Whitening Enable")
    IProfile.make_optional_input(profile, model.vars.payload_crc_en, 'frame_payload', default=True, readable_name="Insert/Check CRC after payload")
    if family != 'dumbo':
        IProfile.make_hidden_input(profile, model.vars.payload_addtrailtxdata_en, 'frame_payload', readable_name="Add Trail TX Data to subframe")
    if family == 'nerio' or family == 'nixi':           # TODO Is this correct?  Do we include this in Nixi?
        IProfile.make_hidden_input(profile, model.vars.payload_excludesubframewcnt_en, 'frame_payload', readable_name="Exclude words in subframe from Word Couunter")

    # -- Header --
    IProfile.make_optional_input(profile, model.vars.header_size, 'frame_header', default=1, readable_name="Header Size", value_limit_min=MIN_HEADER_LENGTH, value_limit_max=MAX_HEADER_LENGTH)
    IProfile.make_optional_input(profile, model.vars.header_calc_crc, 'frame_header', default=False, readable_name="CRC Header")
    # IProfile.make_required_input(profile, model.vars.header_include_crc, 'frame_header', readable_name="Insert/Check CRC after header", default=False, default_visibility=ModelInputDefaultVisibilityType.HIDDEN))
    IProfile.make_optional_input(profile, model.vars.header_white_en, 'frame_header', default=False, readable_name="Whiten Header")
    if family != 'dumbo':
        IProfile.make_hidden_input(profile, model.vars.header_addtrailtxdata_en, 'frame_header',       readable_name="Add Trail TX Data to subframe")
    if family not in ["dumbo", "jumbo"]:
        # TODO Is this correct?  Do we include this in Nixi?
        IProfile.make_hidden_input(profile, model.vars.header_excludesubframewcnt_en, 'frame_payload', readable_name="Exclude words in subframe from Word Couunter")

    # -- Fixed Length --
    # RAIL doesn't support packets longer than 255 (appended info inclusive) so restrict this.
    # I do realize that header+frame_fixed_length can possibly exceed this.
    IProfile.make_optional_input(profile, model.vars.fixed_length_size, 'frame_fixed_length', default=1, readable_name="Fixed Payload Size", value_limit_min=MIN_FRAME_LENGTH, value_limit_max=MAX_FRAME_LENGTH)

    # -- Variable Length --
    IProfile.make_optional_input(profile, model.vars.var_length_numbits, 'frame_var_length', default=8, readable_name="Variable Length Bit Size", value_limit_min=1, value_limit_max=12)
    IProfile.make_optional_input(profile, model.vars.var_length_bitendian, 'frame_var_length', default=model.vars.var_length_bitendian.var_enum.LSB_FIRST, readable_name="Variable Length Bit Endian")
    IProfile.make_optional_input(profile, model.vars.var_length_byteendian, 'frame_var_length', default=model.vars.var_length_byteendian.var_enum.LSB_FIRST, readable_name="Variable Length Byte Endian")
    IProfile.make_optional_input(profile, model.vars.var_length_shift, 'frame_var_length', default=0, readable_name="Variable Length Bit Location", value_limit_min=0, value_limit_max=7)
    IProfile.make_optional_input(profile, model.vars.var_length_minlength, 'frame_var_length', default=0, readable_name="Minimum Length", value_limit_min=0, value_limit_max=4095)
    IProfile.make_optional_input(profile, model.vars.var_length_maxlength, 'frame_var_length', default=255, readable_name="Maximum Length", value_limit_min=0, value_limit_max=4095)
    IProfile.make_optional_input(profile, model.vars.var_length_includecrc, 'frame_var_length', default=False, readable_name="Length Includes CRC Bytes")
    IProfile.make_optional_input(profile, model.vars.var_length_adjust, 'frame_var_length', default=0, readable_name="Variable Frame Length Adjust", value_limit_min=-4096, value_limit_max=4095)

    # -- Frame Type --
    IProfile.make_optional_input(profile, model.vars.frame_type_loc, 'frame_type_length', default=0, readable_name="Frame Type Location", value_limit_min=0, value_limit_max=255)
    IProfile.make_optional_input(profile, model.vars.frame_type_bits, 'frame_type_length', default=3, readable_name="Number of Frame Type Bits", value_limit_min=1, value_limit_max=3)
    IProfile.make_optional_input(profile, model.vars.frame_type_lsbit, 'frame_type_length', default=0, readable_name="Frame Type Bit 0 Location", value_limit_min=0, value_limit_max=0x7)
    IProfile.make_optional_input(profile, model.vars.frame_type_0_length, 'frame_type_length', default=16, readable_name="Frame Type 0 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_1_length, 'frame_type_length', default=16, readable_name="Frame Type 1 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_2_length, 'frame_type_length', default=16, readable_name="Frame Type 2 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_3_length, 'frame_type_length', default=16, readable_name="Frame Type 3 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_4_length, 'frame_type_length', default=16, readable_name="Frame Type 4 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_5_length, 'frame_type_length', default=16, readable_name="Frame Type 5 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_6_length, 'frame_type_length', default=16, readable_name="Frame Type 6 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_7_length, 'frame_type_length', default=16, readable_name="Frame Type 7 Length", value_limit_min=0, value_limit_max=MAX_FRAME_TYPE_LENGTH)
    IProfile.make_optional_input(profile, model.vars.frame_type_0_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 0")
    IProfile.make_optional_input(profile, model.vars.frame_type_1_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 1")
    IProfile.make_optional_input(profile, model.vars.frame_type_2_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 2")
    IProfile.make_optional_input(profile, model.vars.frame_type_3_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 3")
    IProfile.make_optional_input(profile, model.vars.frame_type_4_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 4")
    IProfile.make_optional_input(profile, model.vars.frame_type_5_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 5")
    IProfile.make_optional_input(profile, model.vars.frame_type_6_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 6")
    IProfile.make_optional_input(profile, model.vars.frame_type_7_valid, 'frame_type_length', default=True, readable_name="Accept Frame Type 7")
    IProfile.make_optional_input(profile, model.vars.frame_type_0_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 0")
    IProfile.make_optional_input(profile, model.vars.frame_type_1_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 1")
    IProfile.make_optional_input(profile, model.vars.frame_type_2_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 2")
    IProfile.make_optional_input(profile, model.vars.frame_type_3_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 3")
    IProfile.make_optional_input(profile, model.vars.frame_type_4_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 4")
    IProfile.make_optional_input(profile, model.vars.frame_type_5_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 5")
    IProfile.make_optional_input(profile, model.vars.frame_type_6_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 6")
    IProfile.make_optional_input(profile, model.vars.frame_type_7_filter, 'frame_type_length', default=False, readable_name="Apply Address Filter for Frame Type 7")

def buildFrameOutputs(model, profile, family):
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FECCTRL_BLOCKWHITEMODE', 'frame')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CTRL_BITSPERWORD', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CTRL_RXFCDMODE', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CTRL_TXFCDMODE', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CTRL_BITORDER', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_CTRL_UARTMODE', 'frame')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WCNTCMP0_FRAMELENGTH', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_WCNTCMP1_LENGTHFIELDLOC', 'frame')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_DFLINCLUDECRC', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_MINLENGTH', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_DFLBITS', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_DFLOFFSET', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_DFLSHIFT', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_DFLBITORDER', 'frame')))
    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_DFLCTRL_DFLMODE', 'frame')))

    profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_MAXLENGTH_MAXLENGTH', 'frame')))

    #These are named differently in 90nm parts vs Panther
    if family in ["dumbo", "jumbo", "nerio", "nixi"]:

        if family != 'dumbo':
            profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_WORDS', 'frame')))

        if family != 'dumbo':
            profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_WORDS', 'frame')))

        if family != 'dumbo':
            profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_WORDS', 'frame')))

        if family != 'dumbo':
            profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_WORDS', 'frame')))

        if family in ["nerio"]:
            profile.outputs.append(ModelOutput(model.vars.FRC_FCD0_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD0.EXCLUDESUBFRAMEWCNT'))
            profile.outputs.append(ModelOutput(model.vars.FRC_FCD1_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD1.EXCLUDESUBFRAMEWCNT'))
            profile.outputs.append(ModelOutput(model.vars.FRC_FCD2_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD2.EXCLUDESUBFRAMEWCNT'))
            profile.outputs.append(ModelOutput(model.vars.FRC_FCD3_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD3.EXCLUDESUBFRAMEWCNT'))
            profile.outputs.append(ModelOutput(model.vars.FRC_CTRL_RATESELECT,         '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.CTRL.RATESELECT'))

    # These are named differently in 90nm parts vs Panther
    if family not in ["dumbo", "jumbo", "nerio", "nixi"]:
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD0_WORDS', 'frame')))


        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD1_WORDS', 'frame')))


        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD2_WORDS', 'frame')))


        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_ADDTRAILTXDATA', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_SKIPWHITE', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_SKIPCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_CALCCRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_INCLUDECRC', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_BUFFER', 'frame')))
        profile.outputs.append(eval(_buildModelOutputStringFromRegisterField('FRC_FCD3_WORDS', 'frame')))

        profile.outputs.append(ModelOutput(model.vars.FRC_FCD0_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD0.EXCLUDESUBFRAMEWCNT'))
        profile.outputs.append(ModelOutput(model.vars.FRC_FCD1_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD1.EXCLUDESUBFRAMEWCNT'))
        profile.outputs.append(ModelOutput(model.vars.FRC_FCD2_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD2.EXCLUDESUBFRAMEWCNT'))
        profile.outputs.append(ModelOutput(model.vars.FRC_FCD3_EXCLUDESUBFRAMEWCNT, '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.FCD3.EXCLUDESUBFRAMEWCNT'))
        profile.outputs.append(ModelOutput(model.vars.FRC_CTRL_RATESELECT,         '', ModelOutputType.SVD_REG_FIELD, readable_name='FRC.CTRL.RATESELECT'))

    # Output software variables
    profile.outputs.append(ModelOutput(model.vars.frame_coding_array_packed, '', ModelOutputType.SW_VAR, readable_name='Packed Frame Coding Array'))



def buildLongRangeOutputs(model, profile):
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE_LRCORRTHD, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRCORRTHD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE_LRTIMCORRTHD, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRTIMCORRTHD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE_LRCORRSCHWIN, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRTIMCORRTHD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE_LRBLE, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRCORRSCHWIN'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LRFRC_CI500, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRBLE'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LRFRC_FRCACKTIMETHD, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LRFRC.CI500'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE_LRDEC, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LRFRC.FRCACKTIMETHD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE_LRBLEDSA, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRDEC'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE1_LRSS, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE.LRBLEDSA'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE1.LRSS'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE1.LRTIMEOUTTHD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE6_LRSPIKETHD, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE6.LRCHPWRSPIKETH'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE1_LRSPIKETHADD, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE6.LRSPIKETHD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE1_CHPWRACCUDEL, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE1.LRSPIKETHADD'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE1_HYSVAL, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE1.CHPWRACCUDEL'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE1_AVGWIN, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE1.HYSVAL'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE2_LRCHPWRTH1, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE1.AVGWIN'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE2_LRCHPWRTH2, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE2.LRCHPWRTH1'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE2_LRCHPWRTH3, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE2.LRCHPWRTH2'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE2_LRCHPWRTH4, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE2.LRCHPWRTH3'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE3_LRCHPWRTH5, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE2.LRCHPWRTH4'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE3_LRCHPWRTH6, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE3.LRCHPWRTH5'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE3_LRCHPWRTH7, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE3.LRCHPWRTH6'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE3_LRCHPWRTH8, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE3.LRCHPWRTH7'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE4_LRCHPWRTH9, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE3.LRCHPWRTH8'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE4_LRCHPWRTH10, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE4.LRCHPWRTH9'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE4_LRCHPWRSH1, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE4.LRCHPWRTH10'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE4_LRCHPWRSH2, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE4.LRCHPWRSH1'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE4_LRCHPWRSH3, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE4.LRCHPWRSH2'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE4_LRCHPWRSH4, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE4.LRCHPWRSH3'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH5, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE4.LRCHPWRSH4'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH6, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE5.LRCHPWRSH5'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH7, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE5.LRCHPWRSH6'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH8, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE5.LRCHPWRSH7'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH9, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE5.LRCHPWRSH8'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH10, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE5.LRCHPWRSH9'))
    profile.outputs.append(ModelOutput(model.vars.MODEM_LONGRANGE5_LRCHPWRSH11, '', ModelOutputType.SVD_REG_FIELD, readable_name='MODEM.LONGRANGE5.LRCHPWRSH1'))

def build_ircal_sw_vars(model, profile):
    # Output Software Variables
    profile.outputs.append(ModelOutput(model.vars.ircal_auxndiv, '', ModelOutputType.SW_VAR, readable_name='IRCAL auxndiv'))
    profile.outputs.append(ModelOutput(model.vars.ircal_auxlodiv, '', ModelOutputType.SW_VAR, readable_name='IRCAL auxlodiv'))
    profile.outputs.append(ModelOutput(model.vars.ircal_rampval, '', ModelOutputType.SW_VAR, readable_name='IRCAL rampval'))
    profile.outputs.append(ModelOutput(model.vars.ircal_rxamppll, '', ModelOutputType.SW_VAR, readable_name='IRCAL rxamppll'))
    profile.outputs.append(ModelOutput(model.vars.ircal_rxamppa, '', ModelOutputType.SW_VAR, readable_name='IRCAL rxamppa'))
    profile.outputs.append(ModelOutput(model.vars.ircal_manufconfigvalid, '', ModelOutputType.SW_VAR, readable_name='IRCAL manufconfigvalid'))
    profile.outputs.append(ModelOutput(model.vars.ircal_pllconfigvalid, '', ModelOutputType.SW_VAR, readable_name='IRCAL pllconfigvalid'))
    profile.outputs.append(ModelOutput(model.vars.ircal_paconfigvalid, '', ModelOutputType.SW_VAR, readable_name='IRCAL paconfigvalid'))
    profile.outputs.append(ModelOutput(model.vars.ircal_useswrssiaveraging, '', ModelOutputType.SW_VAR, readable_name='IRCAL useswrssiaveraging'))
    profile.outputs.append(ModelOutput(model.vars.ircal_numrssitoavg, '', ModelOutputType.SW_VAR, readable_name='IRCAL numrssitoavg'))
    profile.outputs.append(ModelOutput(model.vars.ircal_throwawaybeforerssi, '', ModelOutputType.SW_VAR, readable_name='IRCAL throwawaybeforerssi'))
    profile.outputs.append(ModelOutput(model.vars.ircal_delayusbeforerssi, '', ModelOutputType.SW_VAR, readable_name='IRCAL delayusbeforerssi'))
    profile.outputs.append(ModelOutput(model.vars.ircal_delayusbetweenswrssi, '', ModelOutputType.SW_VAR, readable_name='IRCAL delayusbetweenswrssi'))
    profile.outputs.append(ModelOutput(model.vars.ircal_bestconfig, '', ModelOutputType.SW_VAR, readable_name='IRCAL bestconfig'))

    # All but one (agcrssiperiod) of these were created for backwards compatibility with RAIL 1.x - remove in RAIL 2.x
    profile.outputs.append(ModelOutput(model.vars.ircal_agcrssiperiod, '', ModelOutputType.SW_VAR, readable_name='IRCAL agcrssiperiod'))
    profile.outputs.append(ModelOutput(model.vars.ircal_useswrssiaveraging2, '', ModelOutputType.SW_VAR, readable_name='IRCAL useswrssiaveraging new'))
    profile.outputs.append(ModelOutput(model.vars.ircal_numrssitoavg2, '', ModelOutputType.SW_VAR, readable_name='IRCAL numrssitoavg new'))
    profile.outputs.append(ModelOutput(model.vars.ircal_throwawaybeforerssi2, '', ModelOutputType.SW_VAR, readable_name='IRCAL throwawaybeforerssi new'))
    profile.outputs.append(ModelOutput(model.vars.ircal_delayusbeforerssi2, '', ModelOutputType.SW_VAR, readable_name='IRCAL delayusbeforerssi new'))
    profile.outputs.append(ModelOutput(model.vars.ircal_delayusbetweenswrssi2, '', ModelOutputType.SW_VAR, readable_name='IRCAL delayusbetweenswrssi new'))

