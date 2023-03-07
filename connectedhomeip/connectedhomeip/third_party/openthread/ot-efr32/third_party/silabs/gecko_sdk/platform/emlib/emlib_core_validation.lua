
local interrupt_disabled_timing = slc.config("SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING")
if interrupt_disabled_timing.value == "1" and not slc.is_provided("cycle_counter") then
  validation.error(
    "Interrupt disabled timing requires cycle_counter",
    validation.target_for_defines({"SL_EMLIB_CORE_ENABLE_INTERRUPT_DISABLED_TIMING"}),
    nil,
    nil)
end
