from pylib_multi_phy_model.protocol_reference_files.utils.iProtocol import *

class z_wave_IN_validation_testing(IProtocol):

    def get_model(self):
        mphy_config = multi_phy_configuration(part_family="nerio",
                                                 part_revision="A0",
                                                 desc="z_wave_IN_validation_testing",
                                                 rail_adapter_version="rail_api_2.x",
                                                 xsd_version=MultiPHYConfig.xsd_version(),
                                                 status_code=ModelDiffCodes.OK.value,
                                                 readable_name="Z-wave IN validation testing")

        base_channel_configuration = base_channel_configurationType(name="Z-Wave IN validation testing with 3 Channels", profile=None)
        phy_name = ""
        base_channel_configuration.phy = phyType(phy_name)

        profile_input_overrides = base_channel_configuration.phy.profile_input_overrides
        # profile_input_overrides.add_override(overrideType("bitrate", 50001))
        # profile_input_overrides.add_override(overrideType("agc_hysteresis", ModelDiffCodes.OK.value))
        # profile_input_overrides.add_override(overrideType("diff_encoding_mode", 'DISABLED'))
        # profile_input_overrides.add_override(overrideType("syncword_0", long(0x904E)))
        # profile_input_overrides.add_override(overrideType("syncword_1", long(0x7a0e)))

        channel_config_entry = channel_config_entryType(name="PHY_ZWave_100kbps_916MHz_IN_ch0",
                                            phy_name_override="PHY_ZWave_100kbps_916MHz",
                                            base_frequency=865.2e6,
                                            channel_spacing=0,
                                            physical_channel_offset=5,
                                            channel_number_start=0,
                                            channel_number_end=1,
                                            max_power=-2)

        # input_override = overrideType("bitrate", 200000)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("base_frequency_hz", 916.00e6)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("frame_length_type", 'FIXED_LENGTH')
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("fixed_length_size", 20)
        # channel_config_entry.profile_input_overrides.add_override(input_override)

        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        channel_config_entry = channel_config_entryType(name="PHY_ZWave_40kbps_908MHz_IN_ch1",
                                            phy_name_override="PHY_ZWave_40kbps_908MHz",
                                            base_frequency=865.2e6,
                                            channel_spacing=0,
                                            physical_channel_offset=3,
                                            channel_number_start=1,
                                            channel_number_end=2,
                                            max_power=-1)
        # input_override = overrideType("bitrate", 150000)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("base_frequency_hz", 908.40e6)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("frame_length_type", 'FIXED_LENGTH')
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("fixed_length_size", 20)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        channel_config_entry = channel_config_entryType(name="PHY_ZWave_9p6kbps_908MHz_IN_ch2",
                                            phy_name_override="PHY_ZWave_9p6kbps_908MHz",
                                            base_frequency=865.2e6,
                                            channel_spacing=0,
                                            physical_channel_offset=0,
                                            channel_number_start=2,
                                            channel_number_end=3,
                                            max_power=0)
        # input_override = overrideType("bitrate", 50002)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("base_frequency_hz", 908.42e6)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("frame_length_type", 'FIXED_LENGTH')
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        # input_override = overrideType("fixed_length_size", 20)
        # channel_config_entry.profile_input_overrides.add_override(input_override)
        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        mphy_config.base_channel_configurations.add_base_channel_configuration(base_channel_configuration)

        return mphy_config

