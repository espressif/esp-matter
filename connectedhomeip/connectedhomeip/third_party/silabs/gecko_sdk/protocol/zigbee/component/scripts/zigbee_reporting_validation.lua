--[[
    This script validates the expandable table restrictions in reporting table component
    1. Checks if the expanded reporting tables are enabled.
    2. If enabled it is only supported in host mode or NVM3 is enabled 
--]]
local expanded_table_enabled_object = slc.config("EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE")
local host_enabled = slc.is_selected("zigbee_ezsp")
local nvm3_enabled = slc.is_provided("nvm3")

if expanded_table_enabled_object ~= nil and expanded_table_enabled_object.value == "1" then
        if host_enabled == false and nvm3_enabled == false then
            validation.error(
                "Expanded Table limited to Host or EFR32 (with NVM3) configurations",
                validation.target_for_defines({"EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE"}),
                nil,
                nil
            )
        end
end