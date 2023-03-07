from pyradioconfig.parts.ocelot.profiles.profile_wisun_fan_1_0 import ProfileWisunFanOcelot
from pyradioconfig.parts.sol.profiles.sw_profile_outputs_common import sw_profile_outputs_common_sol


class ProfileWisunFanSol(ProfileWisunFanOcelot):

    def __init__(self):
        super().__init__()
        self._family = "sol"
        self._sw_profile_outputs_common = sw_profile_outputs_common_sol()

    def _fixed_wisun_crc(self, model):
        model.vars.fcs_type_802154.value_forced = model.vars.fcs_type_802154.var_enum.FOUR_BYTE
        model.vars.crc_invert.value_forced = True
        model.vars.crc_pad_input.value_forced = True
        model.vars.crc_seed.value_forced = 0xFFFFFFFF

    def build_hidden_profile_inputs(self, model, profile):

        super().build_hidden_profile_inputs(model, profile)

        self.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")
        self.make_hidden_input(profile, model.vars.afc_run_mode, 'Advanced', readable_name="afc_run_mode")
        self.make_hidden_input(profile, model.vars.trecs_enabled, 'Advanced', readable_name="trecs_enabled")

        # Hidden input for dual front-end filter support
        self.make_hidden_input(profile, model.vars.dual_fefilt, "Advanced",
                               readable_name="Dual front-end filter enable")

    def build_advanced_profile_inputs(self, model, profile):
        super().build_advanced_profile_inputs(model, profile)
        self.make_linked_io(profile, model.vars.fpll_band, 'crystal', readable_name="RF Frequency Planning Band")

    def _fixed_wisun_vars(self, model):

        # Use the fixed wisun vars from Ocelot
        super()._fixed_wisun_vars(model)

        # Add the ofdm_option variable
        model.vars.ofdm_option.value_forced = model.vars.ofdm_option.var_enum.OPT1