-- This validation script checks following:
-- The Packet Handoff component must be selected in order to enable recording of RSSI and LQI.
local packet_handoff_enabled = slc.is_selected("zigbee_packet_handoff")
local record_lqi_rssi_val = slc.config("EMBER_AF_PLUGIN_STACK_DIAGNOSTICS_RECORD_LQI_RSSI")
if ((record_lqi_rssi_val ~= nil) and (record_lqi_rssi_val.value == "1") and (not packet_handoff_enabled)) then
    validation.error(
        "The Stack Diagnostics component requires the Packet Handoff component to record LQI and RSSI values. " ..
        "Please enable the Packet Handoff component or unselect the 'Record LQI and RSSI for each neighbor' " ..
        "option in the Stack Diagnostics component.",
        validation.target_for_defines({"EMBER_AF_PLUGIN_STACK_DIAGNOSTICS_RECORD_LQI_RSSI"}),
        nil,
        nil
    )
end
