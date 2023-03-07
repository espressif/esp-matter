from pyradioconfig.calculator_model_framework.interfaces.iprofile import ModelOutput, ModelOutputType

class wisun_profile_outputs_common_sol(object):

    def build_wisun_mode_switch_outputs(self, model, profile):
        profile.outputs.append(ModelOutput(model.vars.wisun_phy_mode_id, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='WiSUN PhyModeID'))
        profile.outputs.append(ModelOutput(model.vars.wisun_mode_switch_phr, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='WiSUN Mode Switch PHR'))
        profile.outputs.append(ModelOutput(model.vars.bch_lut_data, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='BCH lookup table for error correction'))
        profile.outputs.append(ModelOutput(model.vars.ofdm_symbol_rate, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='Number of symbols sent in 1 second'))