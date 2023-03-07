local config_peripheral = slc.config("SL_SLEEPTIMER_PERIPHERAL")

if config_peripheral.value == "SL_SLEEPTIMER_PERIPHERAL_RTCC" and not slc.is_provided("device_has_rtcc") then
    validation.error(
    "RTCC peripheral is not available on the selected target",
    validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
    nil,
    nil)
elseif config_peripheral.value == "SL_SLEEPTIMER_PERIPHERAL_RTC" and not slc.is_provided("device_has_rtc") then
    validation.error(
    "RTC peripheral is not available on the selected target",
    validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
    nil,
    nil)
elseif config_peripheral.value == "SL_SLEEPTIMER_PERIPHERAL_PRORTC" and not slc.is_provided("device_has_prortc") then
    validation.error(
    "PRORTC peripheral is not available on the selected target",
    validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
    nil,
    nil)
elseif config_peripheral.value == "SL_SLEEPTIMER_PERIPHERAL_SYSRTC" and not slc.is_provided("device_has_sysrtc") then
    validation.error(
    "SYSRTC peripheral is not available on the selected target",
    validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
    nil,
    nil)
elseif config_peripheral.value == "SL_SLEEPTIMER_PERIPHERAL_BURTC" then
    if not slc.is_provided("device_has_burtc") then
        validation.error(
        "BURTC peripheral is not available on the selected target",
        validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
        nil,
        nil)
    elseif slc.is_provided("device_series_0") then
        validation.error(
        "sleeptimer doesn't support BURTC for Series 0 devices",
        validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
        nil,
        nil)
    end
end