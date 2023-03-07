local config_tick = slc.config("OS_CFG_TICK_EN")
if config_tick.value == "1" and not slc.is_provided("sleeptimer") then
  validation.error(
  "Sleeptimer is needed if OS_CFG_TICK_EN is equal to 1",
  validation.target_for_defines({"OS_CFG_TICK_EN"}),
  nil,
  nil)
end