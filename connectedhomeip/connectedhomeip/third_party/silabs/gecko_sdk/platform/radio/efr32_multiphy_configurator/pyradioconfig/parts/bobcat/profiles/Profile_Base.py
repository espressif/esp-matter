from pyradioconfig.parts.ocelot.profiles.Profile_Base import Profile_Base_Ocelot
from pyradioconfig.parts.common.profiles.bobcat_regs import build_modem_regs_bobcat
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.bobcat.profiles.sw_profile_outputs_common import sw_profile_outputs_common_bobcat

class Profile_Base_Bobcat(Profile_Base_Ocelot):

    def __init__(self):
        super().__init__()
        self._description = "Profile used for most PHYs"
        self._family = "bobcat"
        self._sw_profile_outputs_common = sw_profile_outputs_common_bobcat()

    def buildRegisterOutputs(self, model, profile):
        build_modem_regs_bobcat(model, profile, self._family)

    def build_advanced_profile_inputs(self, model, profile):
        IProfile.make_linked_io(profile, model.vars.fec_tx_enable, 'Channel_Coding', readable_name="Enable FEC")
        IProfile.make_linked_io(profile, model.vars.skip2ant, 'Advanced',
                                'Skip 2nd antenna check with phase demod antenna diversity')
        IProfile.make_linked_io(profile, model.vars.preamble_detection_length, "preamble",
                                'Preamble Detection Length', value_limit_min=0, value_limit_max=2097151)
        IProfile.make_linked_io(profile, model.vars.lo_injection_side, "Advanced",
                                readable_name="Injection side")
        # The origin of the value used for the channel bandwidth upper limit isn't obvious.  The decimators can be set to
        # get a bandwidth of up to 3.4 MHz (DEC0 = 3 and DEC1 = 1). The ADC bandwidth, however, is limited to about 2.5 MHz.
        # We should set the limt to to the ADC limit, since the overall bandwidth is limited by the ADC bandwidth, not the
        # limit by the decimators.   See https://jira.silabs.com/browse/MCUW_RADIO_CFG-554 for additional discussion on this.
        bandwidth_limit_decimators = 3400000
        bandwidth_limit_adc = 2530000
        IProfile.make_linked_io(profile, model.vars.bandwidth_hz, 'Advanced',
                                readable_name="Acquisition Channel Bandwidth", value_limit_min=100,
                                value_limit_max=bandwidth_limit_adc, units_multiplier=UnitsMultiplier.KILO)

        IProfile.make_linked_io(profile, model.vars.if_frequency_hz, 'Advanced', readable_name="IF Frequency",
                                value_limit_min=70000, value_limit_max=1900000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_linked_io(profile, model.vars.rssi_period, 'Advanced', readable_name="RSSI Update Period",
                                value_limit_min=1, value_limit_max=15)
        IProfile.make_linked_io(profile, model.vars.etsi_cat1_compatible, 'Advanced',
                                readable_name="ETSI Category 1 Compatibility")

        # Expose diversity registers as advanced inputs
        IProfile.make_linked_io(profile, model.vars.antdivmode, 'Advanced', readable_name="Antenna diversity mode")
        IProfile.make_linked_io(profile, model.vars.antdivrepeatdis, 'Advanced',
                                readable_name="Diversity Select-Best repeat")
        IProfile.make_linked_io(profile, model.vars.var_length_loc, 'Advanced',
                                readable_name="Byte position of dynamic length byte", value_limit_min=0,
                                value_limit_max=4096)
        IProfile.make_linked_io(profile, model.vars.firstframe_bitsperword, 'Advanced',
                                readable_name="Length of the First Word", value_limit_min=1, value_limit_max=8)

    def build_hidden_profile_inputs(self, model, profile):
        IProfile.make_hidden_input(profile, model.vars.src1_range_available_minimum, "modem",
                                   readable_name="SRC range minimum", value_limit_min=125, value_limit_max=155)
        IProfile.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec3, "modem",
                                   readable_name="1=Allow input decimation filter decimate by 3 in cost function",
                                   value_limit_min=0, value_limit_max=1)
        IProfile.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec8, "modem",
                                   readable_name="1=Allow input decimation filter decimate by 8 in cost function",
                                   value_limit_min=0, value_limit_max=1)
        IProfile.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")
        IProfile.make_hidden_input(profile, model.vars.adc_clock_mode, "modem",
                                   readable_name="ADC Clock Mode (XO vs VCO)")
        IProfile.make_hidden_input(profile, model.vars.adc_rate_mode, 'Advanced', readable_name="ADC Rate Mode")
        IProfile.make_hidden_input(profile, model.vars.bcr_demod_en, 'Advanced',
                                   readable_name="Force BCR demod calculation", value_limit_min=0, value_limit_max=1)
        IProfile.make_hidden_input(profile, model.vars.synth_settling_mode, 'modem',
                                   readable_name="Synth Settling Mode")

        IProfile.make_hidden_input(profile, model.vars.fast_detect_enable, 'Advanced',
                                   readable_name="Fast preamble detect enable")
        IProfile.make_hidden_input(profile, model.vars.aox_enable, 'modem', readable_name="Enable AoX")
        IProfile.make_hidden_input(profile, model.vars.antdiv_switch_delay_us, 'Advanced',
                                   readable_name='Antdiv Delay in us', value_limit_min=0.0, value_limit_max=1e6)
        IProfile.make_hidden_input(profile, model.vars.antdiv_switch_skip_us, 'Advanced',
                                   readable_name='Antdiv Skip in us', value_limit_min=0.0, value_limit_max=1e6)
        IProfile.make_hidden_input(profile, model.vars.antdiv_adprethresh_scale, 'Advanced',
                                   readable_name='Antdiv preamble threshold scaling', value_limit_min=0.0, value_limit_max=1.0)
        IProfile.make_hidden_input(profile, model.vars.antdiv_enable_parallel_correlation, 'Advanced',
                                   readable_name='Antdiv Enable Parallel Correlation')
        IProfile.make_hidden_input(profile, model.vars.antdiv_adpcsigampthr, 'Advanced',
                                   readable_name='Antdiv Signal Amplitude Threshold', value_limit_min=0, value_limit_max=491520)
        IProfile.make_hidden_input(profile, model.vars.timing_detection_threshold, 'Advanced',
                                   readable_name='Timing Detection Threshold', value_limit_min=0, value_limit_max=255)
        IProfile.make_hidden_input(profile, model.vars.timing_sample_threshold, 'Advanced',
                                   readable_name="Timing Samples Threshold", value_limit_min=0, value_limit_max=100)
        IProfile.make_hidden_input(profile, model.vars.freq_offset_hz, 'Advanced',
                                   readable_name="Frequency Offset Compensation (AFC) Limit", value_limit_min=0,
                                   value_limit_max=500000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_hidden_input(profile, model.vars.synth_rx_mode, 'Advanced', readable_name='Synthesizer RX Mode')
        IProfile.make_hidden_input(profile, model.vars.synth_tx_mode, 'Advanced', readable_name='Synthesizer TX Mode')
        IProfile.make_hidden_input(profile, model.vars.symbols_in_timing_window, 'Advanced',
                                   readable_name="Number of Symbols in Timing Window", value_limit_min=0,
                                   value_limit_max=60)
        IProfile.make_hidden_input(profile, model.vars.errors_in_timing_window, 'Advanced',
                                   readable_name="Number of Errors Allowed in a Timing Window", value_limit_min=0,
                                   value_limit_max=4)
        IProfile.make_hidden_input(profile, model.vars.number_of_timing_windows, 'Advanced',
                                   readable_name="Number of Timing Windows to Detect", value_limit_min=1,
                                   value_limit_max=16)
        IProfile.make_hidden_input(profile, model.vars.sqi_threshold, 'Advanced',
                                   readable_name="Signal Quality Indicator Threshold", value_limit_min=0,
                                   value_limit_max=255)
        IProfile.make_hidden_input(profile, model.vars.timing_resync_period, 'Advanced',
                                   readable_name="Timing Resync Period", value_limit_min=0, value_limit_max=15)
        IProfile.make_hidden_input(profile, model.vars.frequency_offset_period, 'Advanced',
                                   readable_name="Frequency Offset Period", value_limit_min=0, value_limit_max=7)
        IProfile.make_hidden_input(profile, model.vars.afc_period, 'Advanced',
                                   readable_name="Frequency Offset Compensation (AFC) Period", value_limit_min=0,
                                   value_limit_max=7)
        IProfile.make_hidden_input(profile, model.vars.agc_power_target, 'Advanced', readable_name="AGC Power Target",
                                   value_limit_min=-40, value_limit_max=8)
        IProfile.make_hidden_input(profile, model.vars.agc_hysteresis, 'Advanced', readable_name="AGC Hysteresis",
                                   value_limit_min=0, value_limit_max=8)
        IProfile.make_hidden_input(profile, model.vars.agc_settling_delay, 'Advanced',
                                   readable_name="AGC Settling Delay",
                                   value_limit_min=0, value_limit_max=63)
        IProfile.make_hidden_input(profile, model.vars.afc_step_scale, 'Advanced',
                                   readable_name="Frequency Offset Compensation (AFC) Step Scale", value_limit_min=0.0,
                                   value_limit_max=2.0, fractional_digits=2)
        IProfile.make_hidden_input(profile, model.vars.agc_period, 'Advanced', readable_name="AGC Period",
                                   value_limit_min=0, value_limit_max=7)
        IProfile.make_hidden_input(profile, model.vars.agc_speed, 'Advanced', readable_name="AGC Speed")
        IProfile.make_hidden_input(profile, model.vars.frequency_comp_mode, 'Advanced',
                                   readable_name="Frequency Compensation Mode")
        IProfile.make_hidden_input(profile, model.vars.dsa_enable, 'Advanced', readable_name="Enable DSA")
        IProfile.make_hidden_input(profile, model.vars.agc_scheme, 'Advanced', readable_name="AGC backoff scheme")
        # BCR demod in Ocelot allows for much larger OSR
        IProfile.make_hidden_input(profile, model.vars.target_osr, 'Advanced',
                                   readable_name="Target oversampling rate", value_limit_min=3,
                                   value_limit_max=127)
        IProfile.make_hidden_input(profile, model.vars.ook_slicer_level, 'Advanced', readable_name="OOK slicer level",
                                   value_limit_min=1, value_limit_max=10)
        IProfile.make_hidden_input(profile, model.vars.ircal_rxtx_path_common, 'Advanced',
                                   readable_name="Common RX/TX circuit")
        IProfile.make_hidden_input(profile, model.vars.ircal_power_level, 'Advanced',
                                   readable_name="IR cal power level (amplitude)", value_limit_min=0,
                                   value_limit_max=255)