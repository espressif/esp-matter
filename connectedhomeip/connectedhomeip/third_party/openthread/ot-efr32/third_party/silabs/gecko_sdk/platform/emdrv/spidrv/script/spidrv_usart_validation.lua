local spidrv = slc.component("spidrv_usart")

for k, v in pairs(spidrv.instances) do 
    local instance = string.upper(k)
    local spidrv_config_prefix = "SL_SPIDRV_USART_" .. instance 
    local str_cs_port = spidrv_config_prefix .. "_CS_PORT"
    local str_cs_control = spidrv_config_prefix .. "_CS_CONTROL"
    local config_control = slc.config(str_cs_control)
    local config_cs = slc.config(str_cs_port)
    if (config_control.value == "spidrvCsControlAuto") and config_cs == nil then
        local msg = instance .. " : SPIDRV is configured to control CS, but no CS pin is selected"
        validation.error(msg,
                        validation.target_for_defines({str_cs_port}),
                        "CS must be controlled by the application, or a CS pin must be configured",
                        nil)
    end
end