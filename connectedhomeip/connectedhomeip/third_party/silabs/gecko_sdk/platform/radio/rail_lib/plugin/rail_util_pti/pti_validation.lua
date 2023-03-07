--[[
    This script validates that the correct PTI pins are defined based on the PTI mode.
--]]

if slc.config("SL_RAIL_UTIL_PTI_MODE") ~= "RAIL_PTI_MODE_DISABLED" then
    local pti_mode = slc.config("SL_RAIL_UTIL_PTI_MODE").value
    local pti_dout = slc.config("SL_RAIL_UTIL_PTI_DOUT_PORT")
    local pti_dframe = slc.config("SL_RAIL_UTIL_PTI_DFRAME_PORT")
    local pti_dclk = slc.config("SL_RAIL_UTIL_PTI_DCLK_PORT")

    if pti_dout == nil then
        validation.error("DOUT is not defined",
                         validation.target_for_defines({"SL_RAIL_UTIL_PTI_DOUT_PORT",
                                                        "SL_RAIL_UTIL_PTI_DOUT_PIN"}),
                         "DOUT is required when PTI enabled, please select pin for DOUT",
                         nil)
    end

    if pti_mode == "RAIL_PTI_MODE_UART" then
        if pti_dframe == nil then
            validation.error("DFRAME is not defined",
                             validation.target_for_defines({"SL_RAIL_UTIL_PTI_DFRAME_PORT",
                                                            "SL_RAIL_UTIL_PTI_DFRAME_PIN"}),
                             "DFRAME is required when PTI is in UART mode, please select pin for DFRAME",
                             nil)
        end
        if pti_dclk_port ~= nil then
            validation.error("DCLK is defined",
                             validation.target_for_defines({"SL_RAIL_UTIL_PTI_DCLK_PORT",
                                                            "SL_RAIL_UTIL_PTI_DCLK_PIN"}),
                             "DCLK is not used when PTI is in UART mode, please deselect DCLK pin",
                             nil)
        end

    elseif pti_mode == "RAIL_PTI_MODE_SPI" then
        if pti_dframe == nil then
            validation.error("DFRAME is not defined",
                             validation.target_for_defines({"SL_RAIL_UTIL_PTI_DFRAME_PORT",
                                                            "SL_RAIL_UTIL_PTI_DFRAME_PIN"}),
                             "DFRAME is required when PTI is in SPI mode, please select pin for DFRAME",
                             nil)
        end
        if pti_dclk_port == nil then
            validation.error("DCLK is not defined",
                             validation.target_for_defines({"SL_RAIL_UTIL_PTI_DCLK_PORT",
                                                            "SL_RAIL_UTIL_PTI_DCLK_PIN"}),
                             "DCLK is required when PTI is in SPI mode, please select a pin for DCLK",
                             nil)
        end

    elseif pti_mode == "RAIL_PTI_MODE_UART_ONEWIRE" then
        if pti_dframe ~= nil then
            validation.error("DFRAME is defined",
                             validation.target_for_defines({"SL_RAIL_UTIL_PTI_DFRAME_PORT",
                                                            "SL_RAIL_UTIL_PTI_DFRAME_PIN"}),
                             "DFRAME is not used when PTI is in UART_ONEWIRE mode, please deselect DFRAME pin",
                             nil)
        end
        if pti_dclk_port ~= nil then
            validation.error("DCLK is defined",
                             validation.target_for_defines({"SL_RAIL_UTIL_PTI_DCLK_PORT",
                                                            "SL_RAIL_UTIL_PTI_DCLK_PIN"}),
                             "DCLK is not used when PTI is in UART mode, please deselect DCLK pin",
                             nil)
        end
    end
end