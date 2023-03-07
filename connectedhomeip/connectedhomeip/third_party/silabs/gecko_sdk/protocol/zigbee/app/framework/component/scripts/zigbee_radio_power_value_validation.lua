-- This validation script checks following:
-- 1. The radio transmit power (in dBm) at which the node will operate from the valid list of options

local radio_powers_efr32 = {
    20,
    19,
    18,
    17,
    16,
    15,
    14,
    13,
    12,
    11,
    10,
    9,
    8,
    7,
    6,
    5,
    4,
    3,
    2,
    1,
    0,
    -1,
    -2,
    -3,
    -4,
    -5,
    -6,
    -7,
    -8,
    -10,
    -11,
    -12,
    -14,
    -15,
    -16,
    -26
}
local zigbee_zll_commissioning_common_enabled = slc.is_selected("zigbee_zll_commissioning_common")
local zigbee_network_steering_enabled = slc.is_selected("zigbee_network_steering")
local zigbee_network_find_enabled = slc.is_selected("zigbee_network_find")
local zigbee_network_find_sub_ghz_enabled = slc.is_selected("zigbee_network_find_sub_ghz")
local zigbee_network_creator_enabled = slc.is_selected("zigbee_network_creator")

local function has_value(radio_powers, val)
    for index, value in ipairs(radio_powers) do
        if value == val then
            return true
        end
    end
    return false
end

local function validate_value(macro, radio_power_val)
    if radio_power_val ~= nil and (not has_value(radio_powers_efr32, tonumber(radio_power_val.value))) then
        validation.error(
            "Radio transmit power not in supported range of [-26, -16...-14, -12...-10, -8...20]",
            validation.target_for_defines({macro}),
            nil,
            nil
        )
    end
end

if zigbee_zll_commissioning_common_enabled then
    local radio_power_val = slc.config("EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_RADIO_TX_POWER")
    validate_value("EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_RADIO_TX_POWER", radio_power_val)
end
if zigbee_network_steering_enabled then
    local radio_power_val = slc.config("EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_POWER")
    validate_value("EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_POWER", radio_power_val)
end
if zigbee_network_find_enabled then
    local radio_power_val = slc.config("EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_POWER")
    validate_value("EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_POWER", radio_power_val)
end
if zigbee_network_find_sub_ghz_enabled then
    local radio_power_val = slc.config("EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_RADIO_TX_POWER")
    validate_value("EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_RADIO_TX_POWER", radio_power_val)
end
if zigbee_network_creator_enabled then
    local radio_power_val = slc.config("EMBER_AF_PLUGIN_NETWORK_CREATOR_RADIO_POWER")
    validate_value("EMBER_AF_PLUGIN_NETWORK_CREATOR_RADIO_POWER", radio_power_val)
end
