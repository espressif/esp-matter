from pyradioconfig.calculator_model_framework.interfaces.iprofile import ModelOutput, ModelOutputType
from pyradioconfig.parts.ocelot.profiles.sw_profile_outputs_common import sw_profile_outputs_common_ocelot

class sw_profile_outputs_common_sol(sw_profile_outputs_common_ocelot):

    def build_ircal_outputs(self, model, profile):
        # Output Software Variables
        profile.outputs.append(ModelOutput(model.vars.ircal_auxndiv, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL auxndiv'))
        profile.outputs.append(ModelOutput(model.vars.ircal_auxlodiv, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL auxlodiv'))
        profile.outputs.append(ModelOutput(model.vars.ircal_rampval, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL rampval'))
        profile.outputs.append(ModelOutput(model.vars.ircal_rxamppll, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL rxamppll'))
        profile.outputs.append(ModelOutput(model.vars.ircal_rxamppa, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL rxamppa'))
        profile.outputs.append(ModelOutput(model.vars.ircal_manufconfigvalid, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL manufconfigvalid'))
        profile.outputs.append(ModelOutput(model.vars.ircal_pllconfigvalid, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL pllconfigvalid'))
        profile.outputs.append(ModelOutput(model.vars.ircal_paconfigvalid, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL paconfigvalid'))
        profile.outputs.append(ModelOutput(model.vars.ircal_useswrssiaveraging, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL useswrssiaveraging'))
        profile.outputs.append(ModelOutput(model.vars.ircal_numrssitoavg, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL numrssitoavg'))
        profile.outputs.append(ModelOutput(model.vars.ircal_throwawaybeforerssi, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL throwawaybeforerssi'))
        profile.outputs.append(ModelOutput(model.vars.ircal_delayusbeforerssi, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL delayusbeforerssi'))
        profile.outputs.append(ModelOutput(model.vars.ircal_delayusbetweenswrssi, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL delayusbetweenswrssi'))
        profile.outputs.append(ModelOutput(model.vars.ircal_bestconfig, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL bestconfig'))

        # All but one (agcrssiperiod) of these were created for backwards compatibility with RAIL 1.x - remove in RAIL 2.x
        profile.outputs.append(ModelOutput(model.vars.ircal_agcrssiperiod, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL agcrssiperiod'))
        profile.outputs.append(ModelOutput(model.vars.ircal_useswrssiaveraging2, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL useswrssiaveraging new'))
        profile.outputs.append(ModelOutput(model.vars.ircal_numrssitoavg2, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL numrssitoavg new'))
        profile.outputs.append(ModelOutput(model.vars.ircal_throwawaybeforerssi2, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL throwawaybeforerssi new'))
        profile.outputs.append(ModelOutput(model.vars.ircal_delayusbeforerssi2, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL delayusbeforerssi new'))
        profile.outputs.append(ModelOutput(model.vars.ircal_delayusbetweenswrssi2, '', ModelOutputType.SW_VAR,
                                           readable_name='IRCAL delayusbetweenswrssi new'))
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='IR cal power level (amplitude)'))

    def build_rail_outputs(self, model, profile):
        profile.outputs.append(ModelOutput(model.vars.frequency_offset_factor, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Frequency Offset Factor'))
        profile.outputs.append(ModelOutput(model.vars.frequency_offset_factor_fxp, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Frequency Offset Factor FXP'))
        profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_enabled, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Dynamic Slicer Feature Enabled'))
        profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_threshold_values, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Dynamic Slicer Threshold Values'))
        profile.outputs.append(ModelOutput(model.vars.dynamic_slicer_level_values, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Dynamic Slicer Level Values'))
        profile.outputs.append(ModelOutput(model.vars.src1_calcDenominator, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='SRC1 Helper Calculation'))
        profile.outputs.append(ModelOutput(model.vars.src2_calcDenominator, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='SRC2 Helper Calculation'))
        profile.outputs.append(
            ModelOutput(model.vars.tx_baud_rate_actual, '', ModelOutputType.RAIL_CONFIG, readable_name='TX Baud Rate'))
        profile.outputs.append(ModelOutput(model.vars.baud_per_symbol_actual, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Number of baud to transmit 1 symbol'))
        profile.outputs.append(ModelOutput(model.vars.bits_per_symbol_actual, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Number of bits contained in 1 symbol'))
        profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_order_num, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='For receive scanning PHYs: order of PHY in scanning sequence'))
        profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_mode, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='For receive scanning PHYs: event to trigger a hop to next PHY'))
        profile.outputs.append(ModelOutput(model.vars.rx_ch_hopping_delay_usec, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='For receive scanning PHYs: delay in microseconds to look for RX on a particular PHY'))
        profile.outputs.append(ModelOutput(model.vars.ppnd_0, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='DCDC Pulse Period for first quarter of synth region'))
        profile.outputs.append(ModelOutput(model.vars.ppnd_1, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='DCDC Pulse Period for second quarter of synth region'))
        profile.outputs.append(ModelOutput(model.vars.ppnd_2, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='DCDC Pulse Period for third quarter of synth region'))
        profile.outputs.append(ModelOutput(model.vars.ppnd_3, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='DCDC Pulse Period for forth quarter of synth region'))
        profile.outputs.append(ModelOutput(model.vars.psm_max_sleep_us, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Maximum time to sleep in PSM (us)'))
        profile.outputs.append(ModelOutput(model.vars.am_low_ramplev, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='LOW ramp level for amplitude modulation'))
        profile.outputs.append(ModelOutput(model.vars.div_antdivmode, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Antenna diversity mode'))
        profile.outputs.append(ModelOutput(model.vars.div_antdivrepeatdis, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Disable repeated measurement of first antenna when Select-Best algorithm is used'))
        profile.outputs.append(ModelOutput(model.vars.rssi_adjust_db, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='RSSI compensation value calculated from decimation and digital gains'))
        profile.outputs.append(ModelOutput(model.vars.stack_info, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Stack information containing protocol and PHY IDs'))
        profile.outputs.append(ModelOutput(model.vars.rx_sync_delay_ns, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Time needed from start of preamble on-air to sync detect'))
        profile.outputs.append(ModelOutput(model.vars.rx_eof_delay_ns, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Time from end of frame on-air to RX EOF timestamp'))
        profile.outputs.append(ModelOutput(model.vars.tx_eof_delay_ns, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Time from end of frame on-air to TX EOF timestamp'))
        profile.outputs.append(ModelOutput(model.vars.fpll_div_array, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='RFFPLL divider array [divx,divy,divn]'))
        profile.outputs.append(ModelOutput(model.vars.fpll_divx_freq, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='RFFPLL frequency after DIVX'))
        profile.outputs.append(ModelOutput(model.vars.fpll_divy_freq, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='RFFPLL frequency after DIVY'))
        profile.outputs.append(ModelOutput(model.vars.softmodem_txircal_params, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='TX IRCal parameters [kt, int2ratio, int2gain]'))
        profile.outputs.append(ModelOutput(model.vars.softmodem_txircal_freq, '', ModelOutputType.RAIL_CONFIG,
                                                   readable_name='TX IRCal tone freq'))
