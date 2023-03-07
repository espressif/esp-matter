--Validates the selected device type is a coordinator or a router since alt MAC feature supports only on these device types.

local primary_device_type = slc.config("SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE").value
local correct_device_types = Set("SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER","SLI_ZIGBEE_NETWORK_DEVICE_TYPE_COORDINATOR_OR_ROUTER")

if correct_device_types[primary_device_type] == nil then 
    validation.error("Zigbee Pro Stack Alt MAC component supports being either coordinator or router device type only.",
    validation.target_for_defines({"SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE"}), "Select coordinator or router device type ", nil)
end