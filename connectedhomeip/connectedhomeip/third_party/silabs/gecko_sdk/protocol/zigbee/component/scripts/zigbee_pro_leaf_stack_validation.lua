-- This validation script checks following:
-- 1. The selected primary device type is not a coordinator or a router since leaf stack library supports only rx-on/rx-off end devices.
-- 2. In case of multi network configurations, validate both device types are either end device(s) or sleepy end device(s). */
 
local device_type_primary_val = slc.config("SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE").value
local device_type_secondary_val = slc.config("SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE").value
local secondary_network_enabled = slc.config("SLI_ZIGBEE_SECONDARY_NETWORK_ENABLED").value == "1"

-- rfd stands for reduced function device such as end device and sleepy end device.
local rfd_device_list_val = Set("SLI_ZIGBEE_NETWORK_DEVICE_TYPE_END_DEVICE", "SLI_ZIGBEE_NETWORK_DEVICE_TYPE_SLEEPY_END_DEVICE")
 
if rfd_device_list_val[device_type_primary_val] == nil and not secondary_network_enabled then
    validation.error("Zigbee Pro Leaf Stack component supports only end device and sleepy end device types as primary device type.",
        validation.target_for_defines({"SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE"}),
        "Select Zigbee Pro Stack component for coordinator or router device types.",
        nil)
elseif rfd_device_list_val[device_type_primary_val] == nil or rfd_device_list_val[device_type_secondary_val] == nil then
    validation.warning("Zigbee Pro Leaf Stack component supports only end device and sleepy end device types as primary and secondary device types.",
        validation.target_for_defines({"SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE", "SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE"}),
        "Select Zigbee Pro Stack component if one of the device types to be a coordinator or router.",
        nil)
end