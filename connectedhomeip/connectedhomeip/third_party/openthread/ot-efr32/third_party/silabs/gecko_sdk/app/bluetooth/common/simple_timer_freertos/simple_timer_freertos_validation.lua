-- simple_timer_freertos validation for configUSE_TIMERS
local cfg_value = autonumber_common.autonumber(slc.config("configUSE_TIMERS").value)
if cfg_value ~= nil and cfg_value == 0 then
        validation.error("Kernel configUSE_TIMERS config needs to be enabled",
        validation.target_for_defines({"configUSE_TIMERS"}),
        nil,
        nil)
end

