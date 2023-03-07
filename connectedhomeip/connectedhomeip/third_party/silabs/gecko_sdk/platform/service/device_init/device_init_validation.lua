if slc.is_selected("device_init_lfrco") then
  local config_peripheral = slc.config("SL_DEVICE_INIT_LFRCO_PRECISION")

  if slc.is_provided("device_init_lfrco") and not slc.is_provided("device_init_hfxo") then
      if config_peripheral.value == "cmuPrecisionHigh" then
          validation.warning(
          "PRORTC peripheral is not available on the selected target",
          validation.target_for_defines({"SL_SLEEPTIMER_PERIPHERAL"}),
          nil,
          nil)
      end        
  end
end

if slc.is_selected("device_init_hfxo") and slc.is_provided("device_series_1") then
  local config_hfxo_autostart = slc.config("SL_DEVICE_INIT_HFXO_AUTOSTART")
  local config_hfxo_autoselect = slc.config("SL_DEVICE_INIT_HFXO_AUTOSELECT")

  if config_hfxo_autostart.value == "true" then
    validation.warning(
    "SL_DEVICE_INIT_HFXO_AUTOSTART configuration is now deprecated.",
    validation.target_for_defines({"SL_DEVICE_INIT_HFXO_AUTOSTART"}), 
    "To use the HFXO autostart feature, please call CMU_HFXOAutostartEnable() API directly in your application after clocks initialization. Note that this feature is incompatible with Power Manager and can only be used in applications that do not use Power Manager or a radio protocol stack.",
    nil)
  end

  if config_hfxo_autoselect.value == "true" then
    validation.warning(
    "SL_DEVICE_INIT_HFXO_AUTOSELECT configuration is now deprecated.",
    validation.target_for_defines({"SL_DEVICE_INIT_HFXO_AUTOSELECT"}), 
    "To use the HFXO autoselect feature, please call CMU_HFXOAutostartEnable() API directly in your application after clocks initialization. Note that this feature is incompatible with Power Manager and can only be used in applications that do not use Power Manager or a radio protocol stack.",
    nil)
  end
end