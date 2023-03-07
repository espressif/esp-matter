from pyradioconfig.calculator_model_framework.interfaces.iprofile import ModelOutput, ModelOutputType
from pyradioconfig.parts.bobcat.profiles.sw_profile_outputs_common import sw_profile_outputs_common_bobcat

class sw_profile_outputs_common_viper(sw_profile_outputs_common_bobcat):

    def build_info_outputs(self, model, profile):
        profile.outputs.append(ModelOutput(model.vars.preamble_string, '', ModelOutputType.INFO,
                                           readable_name="Preamble Binary Pattern"))
        profile.outputs.append(ModelOutput(model.vars.syncword_string, '', ModelOutputType.INFO,
                                           readable_name="Sync Word Binary Pattern"))
        profile.outputs.append(ModelOutput(model.vars.bandwidth_actual, '', ModelOutputType.INFO,
                                           readable_name='Actual Bandwidth'))
        profile.outputs.append(ModelOutput(model.vars.baudrate, '', ModelOutputType.INFO,
                                           readable_name='Desired baudrate'))
        profile.outputs.append(ModelOutput(model.vars.sample_freq_actual, '', ModelOutputType.INFO,
                                           readable_name='Actual sample frequency'))
        profile.outputs.append(ModelOutput(model.vars.frc_conv_decoder_buffer_size, '', ModelOutputType.SW_VAR,
                                           readable_name='Convolutional Decoder Buffer Size'))
        profile.outputs.append(ModelOutput(model.vars.fec_enabled, '', ModelOutputType.INFO,
                                           readable_name='FEC enabled flag'))

