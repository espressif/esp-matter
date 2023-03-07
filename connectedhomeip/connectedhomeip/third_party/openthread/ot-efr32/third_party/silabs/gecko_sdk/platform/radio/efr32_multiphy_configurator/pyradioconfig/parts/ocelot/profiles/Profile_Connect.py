from pyradioconfig.parts.ocelot.profiles.Profile_Base import Profile_Base_Ocelot
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.ocelot.profiles.frame_profile_inputs_common import frame_profile_inputs_common_ocelot
from pyradioconfig.parts.ocelot.profiles.sw_profile_outputs_common import sw_profile_outputs_common_ocelot

class Profile_Connect_Ocelot(Profile_Base_Ocelot):

    def __init__(self):
        self._profileName = "Connect"
        self._readable_name = "Connect Profile"
        self._category = ""
        self._description = "Profile used for Connect phys"
        self._default = False
        self._activation_logic = ""
        self._family = "ocelot"
        self._frame_profile_inputs_common = frame_profile_inputs_common_ocelot()
        self._sw_profile_outputs_common = sw_profile_outputs_common_ocelot()

    """
    Builds inputs, forced, outputs into modem model
    """
    def buildProfileModel(self, model):

        # Start with base profile
        profile = super(Profile_Connect_Ocelot, self).buildProfileModel(model)

        # Start with a profile that has lots of knobs, and remove most of them.
        # Remove those that are not in a certain category
        for input in profile.inputs:
            # Force things in these categories
            if (input.category.startswith('frame_')) \
                or (input.category == 'crc') \
                or (input.category == 'whitening'):

                    # Don't force these specific inputs in the categories above
                    if (input._var._name == "crc_byte_endian") \
                        or (input._var._name == "crc_bit_endian") \
                        or (input._var._name == "white_poly"):
                            continue

                    self._removeVariableFromInputs(profile, input._var, input.default)

        #Hidden inputs to allow for fixed frame length testing
        IProfile.make_hidden_input(profile, model.vars.frame_length_type, 'frame_general',
                                   readable_name="Frame Length Algorithm")
        IProfile.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length',
                                   readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)

        return profile

    def profile_calculate(self, model):

        # frame_general
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.LSB_FIRST
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        model.vars.header_en.value_forced = True
        model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE

        # frame_payload
        model.vars.payload_white_en.value_forced = False
        model.vars.payload_crc_en.value_forced = True

        # frame_header
        model.vars.header_size.value_forced = 1
        model.vars.header_calc_crc.value_forced = False
        model.vars.header_white_en.value_forced = False

        # frame_fixed_length
        model.vars.fixed_length_size.value_forced = 16

        # frame_var_length
        model.vars.var_length_numbits.value_forced = 7
        model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        model.vars.var_length_shift.value_forced = 0
        model.vars.var_length_minlength.value_forced = 5
        model.vars.var_length_maxlength.value_forced = 127
        model.vars.var_length_includecrc.value_forced = True
        model.vars.var_length_adjust.value_forced = 0

        # frame_type_length
        model.vars.frame_type_loc.value_forced = 0
        model.vars.frame_type_bits.value_forced = 3
        model.vars.frame_type_lsbit.value_forced = 0
        model.vars.frame_type_0_length.value_forced = 0
        model.vars.frame_type_1_length.value_forced = 0
        model.vars.frame_type_2_length.value_forced = 0
        model.vars.frame_type_3_length.value_forced = 0
        model.vars.frame_type_4_length.value_forced = 0
        model.vars.frame_type_5_length.value_forced = 0
        model.vars.frame_type_6_length.value_forced = 0
        model.vars.frame_type_7_length.value_forced = 0
        model.vars.frame_type_0_valid.value_forced = False
        model.vars.frame_type_1_valid.value_forced = False
        model.vars.frame_type_2_valid.value_forced = False
        model.vars.frame_type_3_valid.value_forced = False
        model.vars.frame_type_4_valid.value_forced = False
        model.vars.frame_type_5_valid.value_forced = False
        model.vars.frame_type_6_valid.value_forced = False
        model.vars.frame_type_7_valid.value_forced = False

        # crc
        model.vars.crc_poly.value_forced = model.vars.crc_poly.var_enum.CCITT_16
        model.vars.crc_seed.value_forced = 0x00000000
        model.vars.crc_pad_input.value_forced = False
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.LSB_FIRST
        model.vars.crc_invert.value_forced = False

        # whitening
        model.vars.white_seed.value_forced = 0
        model.vars.white_output_bit.value_forced = 0





