#include "sl_event_handler.h"

#include "em_chip.h"
#include "sl_device_init_nvic.h"
#include "sl_board_init.h"
#include "sl_device_init_dcdc.h"
#include "sl_device_init_hfxo.h"
#include "sl_device_init_lfxo.h"
#include "sl_device_init_clocks.h"
#include "sl_device_init_emu.h"
#include "pa_conversions_efr32.h"
#include "sl_rail_util_pti.h"
#include "sl_board_control.h"
#include "sl_bt_rtos_adaptation.h"
#include "sl_sleeptimer.h"
#include "gpiointerrupt.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#if defined(CONFIG_ENABLE_UART)
#include "sl_uartdrv_instances.h"
#include "psa/crypto.h"
#include "cmsis_os2.h"
#endif // CONFIG_ENABLE_UART

#ifdef SL_WIFI
#include "sl_spidrv_instances.h"
#endif

#include "sl_bluetooth.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif
#if defined(SL_CATALOG_SENSOR_RHT_PRESENT)
#include "sl_i2cspm_instances.h"
#endif

void sl_platform_init(void)
{
    CHIP_Init();
    sl_device_init_nvic();
    sl_board_preinit();
    sl_device_init_dcdc();
    sl_device_init_hfxo();
    sl_device_init_lfxo();
    sl_device_init_clocks();
    sl_device_init_emu();
    sl_board_init();
    osKernelInitialize();
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    sl_power_manager_init();
#endif
}

void sl_kernel_start(void)
{
    osKernelStart();
}

void sl_driver_init(void)
{
    GPIOINT_Init();
#ifdef SL_WIFI
    sl_spidrv_init_instances();
#endif
    sl_simple_button_init_instances();
#if defined(SL_CATALOG_SENSOR_RHT_PRESENT)
    sl_i2cspm_init_instances();
#endif
    sl_simple_led_init_instances();
#if defined(CONFIG_ENABLE_UART)
    sl_uartdrv_init_instances();
#endif // CONFIG_ENABLE_UART
}

void sl_service_init(void)
{
  sl_board_configure_vcom();
  sl_sleeptimer_init();
}

void sl_stack_init(void)
{
  sl_rail_util_pa_init();
  sl_rail_util_pti_init();
  sl_bt_rtos_init();
}

void sl_internal_app_init(void) {}

void sl_platform_process_action(void) {}

void sl_service_process_action(void) {}

void sl_stack_process_action(void) {}

void sl_internal_app_process_action(void) {}
