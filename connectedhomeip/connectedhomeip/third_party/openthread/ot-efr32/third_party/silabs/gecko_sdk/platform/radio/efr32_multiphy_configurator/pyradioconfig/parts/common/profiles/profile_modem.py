from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pycalcmodel.core.output import ModelOutput, ModelOutputType
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile


def buildModemAdvancedInputs(model, profile, family):
    IProfile.make_linked_io(profile, model.vars.timing_detection_threshold        , 'Advanced', readable_name='Timing Detection Threshold',                   value_limit_min=0,      value_limit_max=255)
    IProfile.make_linked_io(profile, model.vars.timing_sample_threshold           , 'Advanced', readable_name="Timing Samples Threshold",                     value_limit_min=0,      value_limit_max=100)
    IProfile.make_linked_io(profile, model.vars.freq_offset_hz                    , 'Advanced', readable_name="Frequency Offset Compensation (AFC) Limit",                          value_limit_min=0,      value_limit_max=500000, units_multiplier=UnitsMultiplier.KILO)

    # The origin of the value used for the channel bandwidth upper limit isn't obvious.  The decimators can be set to
    # get a bandwidth of up to 3.4 MHz (DEC0 = 3 and DEC1 = 1). The ADC bandwidth, however, is limited to about 2.5 MHz.
    # We should set the limt to to the ADC limit, since the overall bandwidth is limited by the ADC bandwidth, not the
    # limit by the decimators.   See https://jira.silabs.com/browse/MCUW_RADIO_CFG-554 for additional discussion on this.
    bandwidth_limit_decimators = 3400000
    bandwidth_limit_adc        = 2530000
    IProfile.make_linked_io(profile, model.vars.bandwidth_hz                      , 'Advanced', readable_name="Acquisition Channel Bandwidth",                 value_limit_min=100,    value_limit_max=bandwidth_limit_adc, units_multiplier=UnitsMultiplier.KILO)

    IProfile.make_linked_io(profile, model.vars.if_frequency_hz                   , 'Advanced', readable_name="IF Frequency",                                 value_limit_min=70000, value_limit_max=1900000, units_multiplier=UnitsMultiplier.KILO)
    IProfile.make_linked_io(profile, model.vars.pll_bandwidth_tx                  , 'Advanced', readable_name="PLL Bandwidth in TX mode")
    IProfile.make_linked_io(profile, model.vars.pll_bandwidth_rx                  , 'Advanced', readable_name="PLL Bandwidth in RX mode")
    IProfile.make_hidden_input(profile, model.vars.pll_bandwidth_miracle_mode     , 'Advanced', readable_name="PLL Miracle Mode")
    IProfile.make_linked_io(profile, model.vars.symbols_in_timing_window          , 'Advanced', readable_name="Number of Symbols in Timing Window",           value_limit_min=0,      value_limit_max=60)
    IProfile.make_linked_io(profile, model.vars.errors_in_timing_window           , 'Advanced', readable_name="Number of Errors Allowed in a Timing Window",  value_limit_min=0,      value_limit_max=4)
    IProfile.make_linked_io(profile, model.vars.number_of_timing_windows          , 'Advanced', readable_name="Number of Timing Windows to Detect",           value_limit_min=1,      value_limit_max=16)
    IProfile.make_hidden_input(profile, model.vars.sqi_threshold                     , 'Advanced', readable_name="Signal Quality Indicator Threshold",           value_limit_min=0,      value_limit_max=255)
    IProfile.make_linked_io(profile, model.vars.timing_resync_period              , 'Advanced', readable_name="Timing Resync Period",                         value_limit_min=0,      value_limit_max=15)
    IProfile.make_linked_io(profile, model.vars.frequency_offset_period           , 'Advanced', readable_name="Frequency Offset Period",                      value_limit_min=0,      value_limit_max=7)
    IProfile.make_linked_io(profile, model.vars.afc_period                        , 'Advanced', readable_name="Frequency Offset Compensation (AFC) Period",                            value_limit_min=0,      value_limit_max=7)
    IProfile.make_linked_io(profile, model.vars.agc_power_target                  , 'Advanced', readable_name="AGC Power Target",                             value_limit_min=-40,    value_limit_max=8)
    IProfile.make_linked_io(profile, model.vars.rssi_period                       , 'Advanced', readable_name="RSSI Update Period",                           value_limit_min=1,      value_limit_max=15)
    IProfile.make_linked_io(profile, model.vars.agc_hysteresis                    , 'Advanced', readable_name="AGC Hysteresis",                               value_limit_min=0,      value_limit_max=8)
    IProfile.make_linked_io(profile, model.vars.agc_settling_delay                , 'Advanced', readable_name="AGC Settling Delay",                           value_limit_min=0,      value_limit_max=63)
    IProfile.make_linked_io(profile, model.vars.afc_step_scale                    , 'Advanced', readable_name="Frequency Offset Compensation (AFC) Step Scale",                               value_limit_min=0.0,    value_limit_max=2.0, fractional_digits=2)
    IProfile.make_linked_io(profile, model.vars.agc_period                        , 'Advanced', readable_name="AGC Period",                                   value_limit_min=0,      value_limit_max=7)
    IProfile.make_hidden_input(profile, model.vars.rx_bitrate_offset_hz              , 'Advanced', readable_name="RX Baudrate offset",                           value_limit_min=0,      value_limit_max=200000)
    IProfile.make_linked_io(profile, model.vars.agc_speed                         , 'Advanced', readable_name="AGC Speed")
    IProfile.make_linked_io(profile, model.vars.frequency_comp_mode               , 'Advanced', readable_name="Frequency Compensation Mode")
    if family != "dumbo":
        IProfile.make_linked_io(profile, model.vars.src_disable                       , 'Advanced', readable_name="SRC Operation")
        IProfile.make_hidden_input(profile, model.vars.viterbi_enable                 , "Advanced", readable_name="Enable Viterbi")
        IProfile.make_hidden_input(profile, model.vars.dsa_enable                     , 'Advanced', readable_name="Enable DSA")
        IProfile.make_linked_io(profile, model.vars.agc_scheme                        , 'Advanced', readable_name="AGC backoff scheme")
        IProfile.make_linked_io(profile, model.vars.etsi_cat1_compatible              , 'Advanced', readable_name="ETSI Category 1 Compatibility")
        IProfile.make_linked_io(profile, model.vars.target_osr, 'Advanced',readable_name="Target oversampling rate", value_limit_min=3, value_limit_max=8)

    if family == "dumbo":
        IProfile.make_linked_io(profile, model.vars.ook_slicer_level                  , 'Advanced', readable_name="OOK slicer level",
                                value_limit_min=1,     value_limit_max=10)
    else:
        IProfile.make_hidden_input(profile, model.vars.ook_slicer_level, 'Advanced', readable_name="OOK slicer level",
                                value_limit_min=1, value_limit_max=10)

    IProfile.make_hidden_input(profile, model.vars.in_2fsk_opt_scope, 'Advanced', readable_name="Include in 2FSK optimization scope")

    if family != "dumbo":
        # Expose diversity registers as advanced inputs
        IProfile.make_linked_io(profile, model.vars.antdivmode, 'Advanced', readable_name="Antenna diversity mode")
        IProfile.make_linked_io(profile, model.vars.antdivrepeatdis, 'Advanced', readable_name="Diversity Select-Best repeat")

    IProfile.make_linked_io(profile, model.vars.ircal_rxtx_path_common, 'Advanced', readable_name="Common RX/TX circuit")
    IProfile.make_linked_io(profile, model.vars.ircal_power_level, 'Advanced', readable_name="IR cal power level (amplitude)",                               value_limit_min=0,     value_limit_max=255)

    IProfile.make_linked_io(profile, model.vars.var_length_loc, 'Advanced', readable_name="Byte position of dynamic length byte",                               value_limit_min=0,     value_limit_max=4096)

    IProfile.make_linked_io(profile, model.vars.firstframe_bitsperword, 'Advanced', readable_name="Length of the First Word",                               value_limit_min=1,     value_limit_max=8)

def buildModemInfoOutputs(model, profile):
    profile.outputs.append(ModelOutput(model.vars.preamble_string,  '', ModelOutputType.INFO, readable_name="Preamble Binary Pattern"))
    profile.outputs.append(ModelOutput(model.vars.syncword_string,  '', ModelOutputType.INFO, readable_name="Sync Word Binary Pattern"))
    profile.outputs.append(ModelOutput(model.vars.bandwidth_actual, '', ModelOutputType.INFO, readable_name='Actual Bandwidth'))
    profile.outputs.append(ModelOutput(model.vars.baudrate,         '', ModelOutputType.INFO, readable_name='Desired baudrate'))
    profile.outputs.append(ModelOutput(model.vars.sample_freq_actual, '', ModelOutputType.INFO, readable_name='Actual sample frequency'))
    profile.outputs.append(ModelOutput(model.vars.frc_conv_decoder_buffer_size, '', ModelOutputType.SW_VAR, readable_name='Convolutional Decoder Buffer Size'))
    profile.outputs.append(ModelOutput(model.vars.fec_enabled, '', ModelOutputType.INFO, readable_name='FEC enabled flag'))


def buildRailOutputs(model, profile):
    profile.outputs.append(ModelOutput(model.vars.frequency_offset_factor, '', ModelOutputType.RAIL_CONFIG, readable_name='Frequency Offset Factor'))
    profile.outputs.append(ModelOutput(model.vars.frequency_offset_factor_fxp, '', ModelOutputType.RAIL_CONFIG, readable_name='Frequency Offset Factor FXP'))
    profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_enabled, '', ModelOutputType.RAIL_CONFIG, readable_name='Dynamic Slicer Feature Enabled'))
    profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_threshold_values, '', ModelOutputType.RAIL_CONFIG, readable_name='Dynamic Slicer Threshold Values'))
    profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_level_values, '', ModelOutputType.RAIL_CONFIG, readable_name='Dynamic Slicer Level Values'))
    profile.outputs.append(ModelOutput(model.vars.src1_calcDenominator, '', ModelOutputType.RAIL_CONFIG, readable_name='SRC1 Helper Calculation'))
    profile.outputs.append(ModelOutput(model.vars.src2_calcDenominator, '', ModelOutputType.RAIL_CONFIG, readable_name='SRC2 Helper Calculation'))
    profile.outputs.append(ModelOutput(model.vars.tx_baud_rate_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='TX Baud Rate'))
    profile.outputs.append(ModelOutput(model.vars.baud_per_symbol_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='Number of baud to transmit 1 symbol'))
    profile.outputs.append(ModelOutput(model.vars.bits_per_symbol_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='Number of bits contained in 1 symbol'))
    profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_order_num, '', ModelOutputType.RAIL_CONFIG, readable_name='For receive scanning PHYs: order of PHY in scanning sequence'))
    profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_mode, '', ModelOutputType.RAIL_CONFIG, readable_name='For receive scanning PHYs: event to trigger a hop to next PHY'))
    profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_delay_usec, '', ModelOutputType.RAIL_CONFIG,readable_name='For receive scanning PHYs: delay in microseconds to look for RX on a particular PHY'))
    profile.outputs.append(ModelOutput(model.vars.div_antdivmode, '', ModelOutputType.RAIL_CONFIG, readable_name='Antenna diversity mode'))
    profile.outputs.append(ModelOutput(model.vars.div_antdivrepeatdis, '', ModelOutputType.RAIL_CONFIG, readable_name='Disable repeated measurement of first antenna when Select-Best algorithm is used'))
    profile.outputs.append(ModelOutput(model.vars.stack_info, '', ModelOutputType.RAIL_CONFIG, readable_name='Stack information containing protocol and PHY IDs'))
    profile.outputs.append(ModelOutput(model.vars.rx_sync_delay_ns, '', ModelOutputType.RAIL_CONFIG, readable_name='Time needed from start of preamble on-air to sync detect'))
    profile.outputs.append(ModelOutput(model.vars.rx_eof_delay_ns, '', ModelOutputType.RAIL_CONFIG, readable_name='Time from end of frame on-air to RX EOF timestamp'))
    profile.outputs.append(ModelOutput(model.vars.tx_eof_delay_ns, '', ModelOutputType.RAIL_CONFIG, readable_name='Time from end of frame on-air to TX EOF timestamp'))
    if model.part_family.lower() not in ["dumbo", "jumbo", "nerio", "nixi", "unit_test_part"]:
        profile.outputs.append(ModelOutput(model.vars.rssi_adjust_db, '', ModelOutputType.RAIL_CONFIG, readable_name='RSSI compensation value calculated from decimation and digital gains'))