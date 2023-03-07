-- simple_timer_freertos_static validation for configUSE_TIMERS and configSUPPORT_STATIC_ALLOCATION
local timer_enable = autonumber_common.autonumber(slc.config('configUSE_TIMERS').value)
local enable_static = autonumber_common.autonumber(slc.config('configSUPPORT_STATIC_ALLOCATION').value)

if timer_enable ~= nil and timer_enable == 0 then
        validation.error("Kernel configUSE_TIMERS config needs to be enabled",
        validation.target_for_defines({"configUSE_TIMERS"}),
        nil,
        nil)
    end

if enable_static ~= nil and enable_static == 0 then
        validation.error("Kernel configSUPPORT_STATIC_ALLOCATION config needs to be enabled",
        validation.target_for_defines({"configSUPPORT_STATIC_ALLOCATION"}),
        nil,
        nil)
    end


