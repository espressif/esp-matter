from pylib_multi_phy_model.protocol_reference_files.utils.iProtocol import *

from py_2_and_3_compatibility import *

class MultiPhyExample2(IProtocol):

    def get_model(self):
        mphy_config = multi_phy_configuration(part_family="jumbo",
                                                 part_revision="A0",
                                                 desc="test_register_diff",
                                                 rail_adapter_version="rail_api_2.x",
                                                 xsd_version=MultiPHYConfig.xsd_version(),
                                                 status_code=ModelDiffCodes.OK.value,
                                                 readable_name="multi-phy example v2")

        base_channel_configuration = base_channel_configurationType(name="802.15.4g with 3 Channels", profile=None)
        phy_name = "PHY_IEEE802154g_MRFSK_OM1_16bitpre"
        base_channel_configuration.phy = phyType(phy_name)

        profile_input_overrides = base_channel_configuration.phy.profile_input_overrides
        profile_input_overrides.add_override(overrideType("bitrate", 50001))
        profile_input_overrides.add_override(overrideType("agc_hysteresis", ModelDiffCodes.OK.value))
        profile_input_overrides.add_override(overrideType("diff_encoding_mode", 'DISABLED'))
        profile_input_overrides.add_override(overrideType("syncword_0", long(0x904E)))
        profile_input_overrides.add_override(overrideType("syncword_1", long(0x7a0e)))

        channel_config_entry = channel_config_entryType(name="my_channel_config_0",
                                            base_frequency=2405000000,
                                            channel_spacing=78900,
                                            physical_channel_offset=0,
                                            channel_number_start=0,
                                            channel_number_end=2,
                                            max_power=0)
        input_override = overrideType("bitrate", 50002)
        channel_config_entry.profile_input_overrides.add_override(input_override)
        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        channel_config_entry = channel_config_entryType(name="my_channel_config_1",
                                            base_frequency=2405000000,
                                            channel_spacing=78900,
                                            physical_channel_offset=3,
                                            channel_number_start=3,
                                            channel_number_end=4,
                                            max_power=-1)
        input_override = overrideType("bitrate", 150000)
        channel_config_entry.profile_input_overrides.add_override(input_override)
        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        channel_config_entry = channel_config_entryType(name="my_channel_config_2",
                                            base_frequency=2405000000,
                                            channel_spacing=78900,
                                            physical_channel_offset=5,
                                            channel_number_start=5,
                                            channel_number_end=5,
                                            max_power=-2)
        input_override = overrideType("bitrate", 200000)
        channel_config_entry.profile_input_overrides.add_override(input_override)
        base_channel_configuration.channel_config_entries.add_channel_config_entry(channel_config_entry)

        mphy_config.base_channel_configurations.add_base_channel_configuration(base_channel_configuration)

        return mphy_config

