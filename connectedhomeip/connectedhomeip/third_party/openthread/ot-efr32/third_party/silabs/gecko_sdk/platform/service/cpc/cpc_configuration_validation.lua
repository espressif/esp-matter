local config_dbg_core = slc.config("SL_CPC_DEBUG_SYSTEM_VIEW_LOG_CORE_EVENT")
local config_dbg_endpoint = slc.config("SL_CPC_DEBUG_SYSTEM_VIEW_LOG_ENDPOINT_EVENT")
if config_dbg_core.value == "1" and not slc.is_provided("segger_systemview") then
  validation.error(
  "Segger System View is required when SL_CPC_DEBUG_SYSTEM_VIEW_LOG_CORE_EVENT is enabled",
  validation.target_for_defines({"SL_CPC_DEBUG_SYSTEM_VIEW_LOG_CORE_EVENT"}),
  nil,
  nil)
elseif config_dbg_endpoint.value == "1" and not slc.is_provided("segger_systemview") then
  validation.error(
  "Segger System View is required when SL_CPC_DEBUG_SYSTEM_VIEW_LOG_ENDPOINT_EVENT is enabled",
  validation.target_for_defines({"SL_CPC_DEBUG_SYSTEM_VIEW_LOG_ENDPOINT_EVENT"}),
  nil,
  nil)
end