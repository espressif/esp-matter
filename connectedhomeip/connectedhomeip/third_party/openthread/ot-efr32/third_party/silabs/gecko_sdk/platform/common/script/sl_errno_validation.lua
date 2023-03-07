-- FreeRTOS
if slc.is_selected("freertos") then
  -- retrieve config that we need to check
  local define_freertos_errno = slc.config("configUSE_POSIX_ERRNO")  
  if define_freertos_errno == nil or define_freertos_errno.value ~= "1" then
    validation.error("Configuration configUSE_POSIX_ERRNO in FreeRTOS must be enable for errno support.",
          validation.target_for_defines({"configUSE_POSIX_ERRNO"}),
          "Set the define in FreeRTOSConfig.h.",
          nil)
  end
end

-- MicriumOS
if slc.is_selected("micriumos_kernel") then
  -- retrieve config that we need to check
  local define_micriumos_errno = slc.config("OS_CFG_ERRNO_EN")  
  if define_micriumos_errno == nil or define_micriumos_errno.value ~= "1" then
    validation.error("Configuration OS_CFG_ERRNO_EN in MicriumOS must be enable for errno support.",
          validation.target_for_defines({"OS_CFG_ERRNO_EN"}),
          "Set the define in os_cfg.h",
          nil)
  end
end