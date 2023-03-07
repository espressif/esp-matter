local cfg_dbg_en = slc.config("OS_CFG_DBG_EN")

if cfg_dbg_en.value == "0" then
  validation.error(
    "Systemview requires the kernel Debug feature (OS_CFG_DBG_EN) to be enabled.",
    validation.target_for_defines({"OS_CFG_DBG_EN"}),
    nil,
    nil)
end