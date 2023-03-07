--[[
    This script validates the value of EMBER_SUPPORTED_NETWORKS defined.
    Multi-network is not supported with ZLL networks so the value of 
    EMBER_SUPPORTED_NETWORKS must be 1.
--]]
local zll_commissioning_common = slc.is_selected("zigbee_zll_commissioning_common")
local zll_commissioning_client = slc.is_selected("zigbee_zll_commissioning_client")
local zll_commissioning_server = slc.is_selected("zigbee_zll_commissioning_server")
local zigbee_multi_network = slc.is_selected("zigbee_multi_network")

if zll_commissioning_common == true or zll_commissioning_common == true or zll_commissioning_common == true then
    if zigbee_multi_network == true then
        validation.error(
            "ZLL is not supported with multiple networks. Either disable the multi-network component or disable ZLL components.",
            validation.target_for_defines({"EMBER_SUPPORTED_NETWORKS"}),
            nil,
            nil
        )
    end
end
