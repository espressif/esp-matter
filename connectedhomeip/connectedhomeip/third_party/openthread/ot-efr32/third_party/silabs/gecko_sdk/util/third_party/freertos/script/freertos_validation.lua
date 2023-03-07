if not slc.is_selected("device_init_nvic") and not slc.is_provided("cortexm0plus") then
    validation.warning("Interrupts priorities are not automatically configured since device_init_nvic is not selected",
                       nil,
                       "To prevent corruption caused by concurrent calls to the FreeRTOS API, interrupts priorities need to be configured according to the FreeRTOS configMAX_SYSCALL_INTERRUPT_PRIORITY setting in FreeRTOSConfig.h",
                       nil)
end
