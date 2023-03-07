from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.common.profiles.jumbo_regs import *
from pyradioconfig.parts.common.profiles.profile_modem import buildModemInfoOutputs, buildRailOutputs
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs, build_ircal_sw_vars

class Profile_Long_Range_Jumbo(IProfile):

    def __init__(self):
        self._profileName = "Long Range"
        self._readable_name = "Long Range Profile"
        self._category = ""
        self._description = "Profile used for Long Range PHYs"
        self._default = False
        self._activation_logic = ""
        self._family = "jumbo"

    def buildProfileModel(self, model):

        # Build profile object and append it to the model
        profile = self._makeProfile(model)

        # Build inputs
        self.build_required_profile_inputs(model, profile)
        self.build_optional_profile_inputs(model, profile)
        self.build_advanced_profile_inputs(model, profile)
        self.build_hidden_profile_inputs(model, profile)
        self.build_deprecated_profile_inputs(model, profile)

        # Build outputs
        self.build_register_profile_outputs(model, profile)
        self.build_variable_profile_outputs(model, profile)
        self.build_info_profile_outputs(model, profile)

    def build_required_profile_inputs(self, model, profile):

        IProfile.make_required_input(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=358000000,
                                     value_limit_max=956000000, units_multiplier=UnitsMultiplier.MEGA)

        IProfile.make_required_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0, value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)

        IProfile.make_required_input(profile, model.vars.longrange_mode, "long_range", readable_name="Long Range Mode")

    def build_optional_profile_inputs(self, model, profile):
        # No optional inputs for this profile
        pass

    def build_advanced_profile_inputs(self, model, profile):
        IProfile.make_linked_io(profile, model.vars.fec_en, 'Channel_Coding', readable_name="FEC Algorithm")

    def build_hidden_profile_inputs(self, model, profile):
        #No hidden inputs for this profile
        pass

    def build_deprecated_profile_inputs(self, model, profile):
        #No deprecated inputs for this profile
        pass

    def build_register_profile_outputs(self, model, profile):
        family = self._family
        build_modem_regs_jumbo(model, profile, family)
        buildFrameOutputs(model, profile, family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)
        pass

    def build_variable_profile_outputs(self, model, profile):
        buildRailOutputs(model, profile)
        build_ircal_sw_vars(model, profile)
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.RAIL_CONFIG,
                                       readable_name='IR cal power level (amplitude)'))

    def build_info_profile_outputs(self, model, profile):
        buildModemInfoOutputs(model, profile)

    def profile_calculate(self, model):
        self._fixed_longrange_vars(model)
        self._lookup_from_longrange_mode(model)
        self._disable_dynamic_BBSS(model)

    def _fixed_longrange_vars(self, model):
        # Calculate all of the fixed values (forces) unique to this profile

        # "base" definitions of the non-FEC PHY. allows usage in multiple PHYs

        # -----------------------------------------------------------------------------------------------
        # LONG RANGE PHYs BASE, CORE CONFIGURATION
        # -----------------------------------------------------------------------------------------------
        # -----------------------------------------------------------------------------------------------
        # Note: Based on Lijun's profile found in the third testrun's excel sheet in https://jira.silabs.com/browse/MCUW_RADIO_CFG-717,
        # but slightly modofied to get back the reference rail_config.c that was given by Lijun to BP
        # office for testing (remeasured and validated sensitivity first by Tibor P. @ BP office).
        # -----------------------------------------------------------------------------------------------

        # Frame inputs
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.LSB_FIRST
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        model.vars.payload_white_en.value_forced = False
        model.vars.payload_crc_en.value_forced = True
        model.vars.header_en.value_forced = True
        model.vars.header_size.value_forced = 1
        model.vars.header_calc_crc.value_forced = False
        model.vars.header_white_en.value_forced = False
        model.vars.var_length_numbits.value_forced = 8
        model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        model.vars.var_length_shift.value_forced = 0
        model.vars.var_length_minlength.value_forced = 5
        model.vars.var_length_maxlength.value_forced = 0x7F
        model.vars.var_length_includecrc.value_forced = True
        model.vars.crc_poly.value_forced = model.vars.crc_poly.var_enum.CCITT_16
        model.vars.crc_seed.value_forced = 0x00000000
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.LSB_FIRST
        model.vars.crc_bit_endian.value_forced = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        model.vars.crc_byte_endian.value_forced = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        model.vars.crc_pad_input.value_forced = False
        model.vars.crc_invert.value_forced = False

        # Assorted app parameters
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0
        model.vars.preamble_pattern.value_forced = 0
        model.vars.preamble_pattern_len.value_forced = 4
        model.vars.rx_xtal_error_ppm.value_forced = 0
        model.vars.symbol_encoding.value_forced = model.vars.symbol_encoding.var_enum.DSSS
        model.vars.timing_sample_threshold.value_forced = 0
        model.vars.tx_xtal_error_ppm.value_forced = 0
        model.vars.xtal_frequency_hz.value_forced = 38400000
        model.vars.agc_power_target.value_forced = -11
        model.vars.agc_scheme.value_forced = model.vars.agc_scheme.var_enum.SCHEME_3
        model.vars.agc_settling_delay.value_forced = 40
        model.vars.baudrate_tol_ppm.value_forced = 0
        model.vars.dsss_chipping_code.value_forced = 0xC8DD7892
        model.vars.dsss_len.value_forced = 32
        model.vars.dsss_spreading_factor.value_forced = 8
        model.vars.frequency_comp_mode.value_forced = model.vars.frequency_comp_mode.var_enum.DISABLED
        model.vars.modulation_type.value_forced = model.vars.modulation_type.var_enum.OQPSK
        model.vars.number_of_timing_windows.value_forced = 7
        model.vars.preamble_length.value_forced = 40
        model.vars.rssi_period.value_forced = 8
        model.vars.shaping_filter.value_forced = model.vars.shaping_filter.var_enum.Custom_OQPSK
        model.vars.shaping_filter_param.value_forced = 0.3
        model.vars.symbols_in_timing_window.value_forced = 12
        model.vars.syncword_length.value_forced = 12
        model.vars.syncword_0.value_forced = 0xe5
        model.vars.syncword_1.value_forced = 0x0
        model.vars.target_osr.value_forced = 5

        # Variables that are optional inputs in the Base profile and need to be explicitly set
        model.vars.test_ber.value_forced = False
        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Default
        model.vars.syncword_tx_skip.value_forced = False
        model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.NONE
        model.vars.white_seed.value_forced = 0
        model.vars.white_output_bit.value_forced = 0
        model.vars.asynchronous_rx_enable.value_forced = False
        model.vars.fixed_length_size.value_forced = 1
        model.vars.frame_type_0_filter.value_forced = True
        model.vars.frame_type_0_length.value_forced = 0
        model.vars.frame_type_0_valid.value_forced = False
        model.vars.frame_type_1_filter.value_forced = True
        model.vars.frame_type_1_length.value_forced = 0
        model.vars.frame_type_1_valid.value_forced = False
        model.vars.frame_type_2_filter.value_forced = True
        model.vars.frame_type_2_length.value_forced = 0
        model.vars.frame_type_2_valid.value_forced = False
        model.vars.frame_type_3_filter.value_forced = True
        model.vars.frame_type_3_length.value_forced = 0
        model.vars.frame_type_3_valid.value_forced = False
        model.vars.frame_type_4_filter.value_forced = True
        model.vars.frame_type_4_length.value_forced = 0
        model.vars.frame_type_4_valid.value_forced = False
        model.vars.frame_type_5_filter.value_forced = True
        model.vars.frame_type_5_length.value_forced = 0
        model.vars.frame_type_5_valid.value_forced = False
        model.vars.frame_type_6_filter.value_forced = True
        model.vars.frame_type_6_length.value_forced = 0
        model.vars.frame_type_6_valid.value_forced = False
        model.vars.frame_type_7_filter.value_forced = True
        model.vars.frame_type_7_length.value_forced = 0
        model.vars.frame_type_7_valid.value_forced = False
        model.vars.frame_type_bits.value_forced = 3
        model.vars.frame_type_loc.value_forced = 0
        model.vars.frame_type_lsbit.value_forced = 0
        model.vars.var_length_adjust.value_forced = 0
        model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.LSB_FIRST

        # Output registers
        model.vars.AGC_CTRL2_ADCRSTSTARTUP.value_forced = 0
        model.vars.AGC_CTRL2_FASTLOOPDEL.value_forced = 5
        model.vars.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.value_forced = 4
        model.vars.AGC_GAINSTEPLIM_SLOWDECAYCNT.value_forced = 0
        model.vars.AGC_LOOPDEL_IFPGADEL.value_forced = 7
        model.vars.AGC_LOOPDEL_LNASLICESDEL.value_forced = 7
        model.vars.AGC_LOOPDEL_PKDWAIT.value_forced = 15
        model.vars.AGC_RSSISTEPTHR_DEMODRESTARTPER.value_forced = 5
        model.vars.AGC_RSSISTEPTHR_DEMODRESTARTTHR.value_forced = 0xab
        model.vars.AGC_RSSISTEPTHR_POSSTEPTHR.value_forced = 3
        model.vars.MODEM_AFC_AFCRXCLR.value_forced = 1
        model.vars.MODEM_AFC_AFCSCALEM.value_forced = 3
        model.vars.MODEM_AFCADJLIM_AFCADJLIM.value_forced = 2750
        model.vars.MODEM_CTRL1_PHASEDEMOD.value_forced = 3
        model.vars.MODEM_CTRL2_DATAFILTER.value_forced = 7
        model.vars.MODEM_CTRL3_TSAMPDEL.value_forced = 2
        model.vars.MODEM_CTRL5_FOEPREAVG.value_forced = 7
        model.vars.MODEM_CTRL5_LINCORR.value_forced = 1
        model.vars.MODEM_CTRL5_POEPER.value_forced = 1
        model.vars.MODEM_CTRL6_ARW.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT0.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT1.value_forced = 1
        model.vars.MODEM_CTRL6_PSTIMABORT2.value_forced = 1
        model.vars.MODEM_CTRL6_RXBRCALCDIS.value_forced = 1
        model.vars.MODEM_CTRL6_TDREW.value_forced = 64
        model.vars.MODEM_INTAFC_FOEPREAVG0.value_forced = 1
        model.vars.MODEM_INTAFC_FOEPREAVG1.value_forced = 3
        model.vars.MODEM_INTAFC_FOEPREAVG2.value_forced = 5
        model.vars.MODEM_INTAFC_FOEPREAVG3.value_forced = 5
        model.vars.MODEM_TIMING_OFFSUBDEN.value_forced = 5
        model.vars.MODEM_TIMING_OFFSUBNUM.value_forced = 12
        model.vars.MODEM_TIMING_TIMSEQSYNC.value_forced = 1
        model.vars.MODEM_PRE_TXBASES.value_forced = 10
        model.vars.MODEM_CGCLKSTOP_FORCEOFF.value_forced = 0x1E00
        model.vars.MODEM_CTRL0_DUALCORROPTDIS.value_forced = 0

    def _lookup_from_longrange_mode(self, model):
        #This function calculates some variables/registers based on the longrange mode

        #Read the mode from the profile inputs (not yet written to model vars)
        longrange_mode = model.profile.inputs.longrange_mode.var_value

        if longrange_mode == model.vars.longrange_mode.var_enum.LR_1p2k:
            model.vars.bitrate.value_forced = 1200
            model.vars.deviation.value_forced = 2400

            if model.profile.inputs.base_frequency_hz.var_value > 600000000:
                model.vars.timing_detection_threshold.value_forced = 98
                model.vars.MODEM_CTRL5_BBSS.value_forced = 1
            else:
                model.vars.timing_detection_threshold.value_forced = 64
                model.vars.MODEM_CTRL5_BBSS.value_forced = 2

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_2p4k:
            model.vars.bitrate.value_forced = 2400
            model.vars.deviation.value_forced = 4800
            model.vars.timing_detection_threshold.value_forced = 180
            model.vars.MODEM_CTRL5_BBSS.value_forced = 2

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_4p8k:
            model.vars.bitrate.value_forced = 4800
            model.vars.deviation.value_forced = 9600

            if model.profile.inputs.base_frequency_hz.var_value > 600000000:
                model.vars.timing_detection_threshold.value_forced = 184
                model.vars.MODEM_CTRL5_BBSS.value_forced = 2
            else:
                model.vars.timing_detection_threshold.value_forced = 138
                model.vars.MODEM_CTRL5_BBSS.value_forced = 3

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_9p6k:
            model.vars.bitrate.value_forced = 9600
            model.vars.deviation.value_forced = 19200

            if model.profile.inputs.base_frequency_hz.var_value > 600000000:
                model.vars.timing_detection_threshold.value_forced = 148
            else:
                model.vars.timing_detection_threshold.value_forced = 146

            model.vars.MODEM_CTRL5_BBSS.value_forced = 3

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_19p2k:
            model.vars.bitrate.value_forced = 19200
            model.vars.deviation.value_forced = 38400

            if model.profile.inputs.base_frequency_hz.var_value > 600000000:
                model.vars.timing_detection_threshold.value_forced = 176
                model.vars.MODEM_CTRL5_BBSS.value_forced = 3
            else:
                model.vars.timing_detection_threshold.value_forced = 120
                model.vars.MODEM_CTRL5_BBSS.value_forced = 4

        elif longrange_mode == model.vars.longrange_mode.var_enum.LR_80k:
            model.vars.bitrate.value_forced = 80000
            model.vars.deviation.value_forced = 160000
            model.vars.timing_detection_threshold.value_forced = 72
            model.vars.MODEM_CTRL5_BBSS.value_forced = 5

    def _disable_dynamic_BBSS(self, model):
        #Need only for Jumbo and Nerio
        model.vars.SEQ_MISC_DYNAMIC_BBSS_SW_EN.value_forced = 0
        pass
