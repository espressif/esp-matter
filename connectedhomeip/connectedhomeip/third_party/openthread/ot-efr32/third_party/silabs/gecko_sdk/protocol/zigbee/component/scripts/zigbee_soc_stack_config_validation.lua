--[[ This script validates following as part of the multi-network and multi-PAN configurations.
    a) To use a multi-network or multi-PAN feature, secondary network config option must be enabled.
    b) For multi-network application:
      - Make sure multi-network component is enabled.
      - Make sure one of the device types has to be a sleepy end device.
    c) For multi-PAN application:
      - Make sure multi-PAN component is enabled,
      - Make sure both device types are selected for coordinator role.
    d) To use multi-network the primary and secondary security should be one of the SE/HA ones or Z3
    e) Zigbee Light Link component shall not be included for multi-network or multi-PAN scenarios --]]

local device_type_primary_val = slc.config("SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE").value
local device_type_secondary_val = slc.config("SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE").value
local primary_network_security_val = slc.config("SLI_ZIGBEE_PRIMARY_NETWORK_SECURITY_TYPE").value
local secondary_network_security_val = slc.config("SLI_ZIGBEE_SECONDARY_NETWORK_SECURITY_TYPE").value
local secondary_network_enabled = slc.config("SLI_ZIGBEE_SECONDARY_NETWORK_ENABLED").value == "1"
local coordinator_device_type_val = "SLI_ZIGBEE_NETWORK_DEVICE_TYPE_COORDINATOR_OR_ROUTER"
local rx_on_device_list_val = Set("SLI_ZIGBEE_NETWORK_DEVICE_TYPE_COORDINATOR_OR_ROUTER", "SLI_ZIGBEE_NETWORK_DEVICE_TYPE_ROUTER", "SLI_ZIGBEE_NETWORK_DEVICE_TYPE_END_DEVICE")
local supported_multi_network_security_types = Set("SLI_ZIGBEE_NETWORK_SECURITY_TYPE_3_0","SLI_ZIGBEE_NETWORK_SECURITY_TYPE_HA","SLI_ZIGBEE_NETWORK_SECURITY_TYPE_HA_1_2","SLI_ZIGBEE_NETWORK_SECURITY_TYPE_SE_TEST","SLI_ZIGBEE_NETWORK_SECURITY_TYPE_SE_FULL")

if secondary_network_enabled then
  if not slc.is_selected("zigbee_multi_network") then
    validation.error("Application needs multi-network component when the secondary network is enabled.",
                     validation.target_for_project(),
                     "Select the multi-network component if the application is intended for multi-network/multi-PAN feature.",
                     nil)
  else
    -- Check for coordinator device type for both networks if multi-PAN component is selected.
    if slc.is_selected("zigbee_multi_pan") then
      if device_type_primary_val ~= coordinator_device_type_val or device_type_secondary_val ~= coordinator_device_type_val then
        validation.error("Multi-PAN application is mandated for being coordinator on both networks. ",
                         validation.target_for_defines({"SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE"}, {"SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE"}),
                         "If the application is intended for multi-PAN feature then both device types must be coordinator.",
                         nil)
      end
    -- Check for Multi PAN component if both device types are coordinator.
    elseif device_type_primary_val == coordinator_device_type_val and device_type_secondary_val == coordinator_device_type_val then
        if not slc.is_selected("zigbee_multi_pan") then
          validation.error("Application needs multi-PAN component with the given primary and secondary network device types.",
                           validation.target_for_project(),
                           "Select the multi-PAN component if the application is intended for multi-PAN feature",
                           nil)
        end
    else
      -- Check for one of the device types to be a Rx-Off/sleepy end device if only multi-network component is enabled.
      if  rx_on_device_list_val[device_type_primary_val] ~= nil and rx_on_device_list_val[device_type_secondary_val] ~= nil then
        validation.error("Multi-network application needs to have one of the device types to be a sleepy end device",
                         validation.target_for_defines({"SLI_ZIGBEE_PRIMARY_NETWORK_DEVICE_TYPE"}, {"SLI_ZIGBEE_SECONDARY_NETWORK_DEVICE_TYPE"}),
                         "If the application is intended for multi-network feature then one of the device types has to be a sleepy end device.",
                         nil)
      end
    end
  end
  --  Validate primary and secondary network security configuration when only multi-network in enabled
  if slc.is_selected("zigbee_multi_network") and not slc.is_selected("zigbee_multi_pan") then
    if supported_multi_network_security_types[primary_network_security_val] == nil then
      validation.error("Unsupported primary network security type",
                       validation.target_for_defines({"SLI_ZIGBEE_PRIMARY_NETWORK_SECURITY_TYPE"}),
                       "If the application is intended to run multi-network, then the security type of the primary network must be one of Zigbee 3.0, Home Automation, or Smart Energy.",
                       nil)
    elseif supported_multi_network_security_types[secondary_network_security_val] ==  nil then
      validation.error("Unsupported secondary network security type",
                       validation.target_for_defines({"SLI_ZIGBEE_SECONDARY_NETWORK_SECURITY_TYPE"}),
                       "If the application is intended to run multi-network, then the security type of the secondary network must be one of Zigbee 3.0, Home Automation, or Smart Energy.",
                       nil)
    end
  end
  -- Validate that the zll component is not installed with multi networks
  if slc.is_selected("zigbee_zll") then
      validation.error("Zigbee Light Link component is not supported",
                       validation.target_for_project(),
                       "If the application is intended for multi-network or multi-PAN then unselect Zigbee Light Link component",
                       nil)
  end
else
  if slc.is_selected("zigbee_multi_network") then
    validation.error("Multi-network component is not needed for single network application",
                      validation.target_for_project(),
                      "Addition of multi-network component will cause increase in flash size for no reason.",
                      nil)
  end
end
