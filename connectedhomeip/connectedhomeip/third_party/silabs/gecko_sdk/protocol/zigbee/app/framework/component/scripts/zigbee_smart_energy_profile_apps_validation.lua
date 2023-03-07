--[[ This validates the basic components needed by any single network Smart Energy Profile Application.
     This validation is invoked via zigbee_device_config.slcc which validates SoC and Host apps
     but not NCP app since there is no way of knowing security profile.
    It validates following
    a) If an application configured with Smart Energy security type then
        a) Make sure either zigbee_cbke_163k1 or zigbee_cbke_283k1 provides present in the project.
        b) Make sure zigbee_key_establishment component is present.
        c) EMBER_KEY_TABLE_SIZE is set to non zero.
    b) If the zigbee_key_establishment component is present in the application then validate
      the security type to be Smart Energy Test or Full. --]]

local primary_network_security_val = slc.config("SLI_ZIGBEE_PRIMARY_NETWORK_SECURITY_TYPE").value
local smart_energy_security_types = Set("SLI_ZIGBEE_NETWORK_SECURITY_TYPE_SE_TEST","SLI_ZIGBEE_NETWORK_SECURITY_TYPE_SE_FULL")
local secondary_network_enabled = slc.config("SLI_ZIGBEE_SECONDARY_NETWORK_ENABLED").value == "1"

if not secondary_network_enabled then
  if smart_energy_security_types[primary_network_security_val] ~= nil then
    -- Make sure either zigbee_cbke_163k1 or zigbee_cbke_283k1 provides present in the project.
    if not slc.is_provided("zigbee_cbke_163k1") and not slc.is_provided("zigbee_cbke_283k1") then
      validation.error("Smart Energy profile application needs CBKE component.",
                        validation.target_for_project(),
                        "Include either 'CBKE 163k1' or 'CBKE 283k1' component",
                        nil)
    end
    -- Make sure zigbee_key_establishment component is present.
    if not slc.is_selected("zigbee_key_establishment") then
      validation.error("Smart Energy profile application needs 'Key Establishment' component.",
                        validation.target_for_project(),
                        nil,
                        nil)
    end
    -- EMBER_KEY_TABLE_SIZE is set to non zero.
    local key_table_size_val = slc.config("EMBER_KEY_TABLE_SIZE").value
    if key_table_size_val == "0" then
      validation.error("Smart Energy profile application needs key table to store keys.",
                        validation.target_for_defines({"EMBER_KEY_TABLE_SIZE"}),
                        "Recommended key table size value is 12",
                        nil)
    end
  end

  if slc.is_selected("zigbee_key_establishment") then
    -- Make sure SE security type is selected since key establishment is present oin the project.
    if smart_energy_security_types[primary_network_security_val] == nil then
      validation.error("Select the security type to be Smart Energy Test or Full from 'Zigbee Device Config' component",
                      validation.target_for_defines({"SLI_ZIGBEE_PRIMARY_NETWORK_SECURITY_TYPE"}),
                      "Key establishment component is used for Smart Energy (SE) profile application only. If the " ..
                      "application is not intended for SE then remove 'Key Establishment' component or fix the security type",
                      nil)
    end
  end
end
