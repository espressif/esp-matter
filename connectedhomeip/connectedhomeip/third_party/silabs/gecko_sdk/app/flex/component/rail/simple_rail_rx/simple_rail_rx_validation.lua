-- simple_rail_rx validation script for RAIL auto state transition and RAIL_ConfigChannel call
local flex_channel_config = slc.config("SL_RAIL_UTIL_INIT_PROTOCOLS_INST0_ENABLE").value

if flex_channel_config ~= nil then
    if tonumber(flex_channel_config) ~= 1 then
        validation.error(
            "SL_RAIL_UTIL_INIT_PROTOCOLS_INST0_ENABLE config shall be set to 1, current value: " .. tostring(flex_channel_config),
            validation.target_for_defines({"SL_RAIL_UTIL_INIT_PROTOCOLS_INST0_ENABLE"}),
            "RAIL_ConfigChannels() needs to be called with a generated RAIL config struct",
            nil)
    end
end