// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "pw_boot/boot.h"

#include <array>

#include "FreeRTOS.h"
#include "config/sf2_mss_hal_conf.h"
#include "m2sxxx.h"
#include "pw_boot_cortex_m/boot.h"
#include "pw_malloc/malloc.h"
#include "pw_preprocessor/compiler.h"
#include "pw_string/util.h"
#include "pw_sys_io_emcraft_sf2/init.h"
#include "pw_system/init.h"
#include "system_m2sxxx.h"
#include "task.h"

#include liberosoc_CONFIG_FILE

namespace {

std::array<StackType_t, configMINIMAL_STACK_SIZE> freertos_idle_stack;
StaticTask_t freertos_idle_tcb;

std::array<StackType_t, configTIMER_TASK_STACK_DEPTH> freertos_timer_stack;
StaticTask_t freertos_timer_tcb;

std::array<char, configMAX_TASK_NAME_LEN> temp_thread_name_buffer;

}  // namespace

extern "C" void Reset_Handler(void);

// Functions needed when configGENERATE_RUN_TIME_STATS is on.
extern "C" void configureTimerForRunTimeStats(void) {}
extern "C" unsigned long getRunTimeCounterValue(void) { return 10 /* FIXME */; }
// uwTick is an uint32_t incremented each Systick interrupt 1ms. uwTick is used
// to execute HAL_Delay function.

// Required for configCHECK_FOR_STACK_OVERFLOW.
extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char* pcTaskName) {
  pw::string::Copy(pcTaskName, temp_thread_name_buffer);
  PW_CRASH("Stack OVF for task %s", temp_thread_name_buffer.data());
}

// Required for configUSE_TIMERS.
extern "C" void vApplicationGetTimerTaskMemory(
    StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer,
    uint32_t* pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &freertos_idle_tcb;
  *ppxIdleTaskStackBuffer = freertos_idle_stack.data();
  *pulIdleTaskStackSize = freertos_idle_stack.size();
}

extern "C" void vApplicationGetIdleTaskMemory(
    StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer,
    uint32_t* pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &freertos_timer_tcb;
  *ppxIdleTaskStackBuffer = freertos_timer_stack.data();
  *pulIdleTaskStackSize = freertos_timer_stack.size();
}

extern "C" void pw_boot_PreStaticMemoryInit() {
#if SF2_MSS_NO_BOOTLOADER
  SystemInit();
  // Initialize DDR
  // inclusive-language: disable
  MDDR->core.ddrc.DYN_SOFT_RESET_CR = 0x0000;
  MDDR->core.ddrc.DYN_REFRESH_1_CR = 0x27de;
  MDDR->core.ddrc.DYN_REFRESH_2_CR = 0x030f;
  MDDR->core.ddrc.DYN_POWERDOWN_CR = 0x0002;
  MDDR->core.ddrc.DYN_DEBUG_CR = 0x0000;
  MDDR->core.ddrc.MODE_CR = 0x00C1;
  MDDR->core.ddrc.ADDR_MAP_BANK_CR = 0x099f;
  MDDR->core.ddrc.ECC_DATA_MASK_CR = 0x0000;
  MDDR->core.ddrc.ADDR_MAP_COL_1_CR = 0x3333;
  MDDR->core.ddrc.ADDR_MAP_COL_2_CR = 0xffff;
  MDDR->core.ddrc.ADDR_MAP_ROW_1_CR = 0x7777;
  MDDR->core.ddrc.ADDR_MAP_ROW_2_CR = 0x0fff;
  MDDR->core.ddrc.INIT_1_CR = 0x0001;
  MDDR->core.ddrc.CKE_RSTN_CYCLES_CR[0] = 0x4242;
  MDDR->core.ddrc.CKE_RSTN_CYCLES_CR[1] = 0x0008;
  MDDR->core.ddrc.INIT_MR_CR = 0x0033;
  MDDR->core.ddrc.INIT_EMR_CR = 0x0020;
  MDDR->core.ddrc.INIT_EMR2_CR = 0x0000;
  MDDR->core.ddrc.INIT_EMR3_CR = 0x0000;
  MDDR->core.ddrc.DRAM_BANK_TIMING_PARAM_CR = 0x00c0;
  MDDR->core.ddrc.DRAM_RD_WR_LATENCY_CR = 0x0023;
  MDDR->core.ddrc.DRAM_RD_WR_PRE_CR = 0x0235;
  MDDR->core.ddrc.DRAM_MR_TIMING_PARAM_CR = 0x0064;
  MDDR->core.ddrc.DRAM_RAS_TIMING_CR = 0x0108;
  MDDR->core.ddrc.DRAM_RD_WR_TRNARND_TIME_CR = 0x0178;
  MDDR->core.ddrc.DRAM_T_PD_CR = 0x0033;
  MDDR->core.ddrc.DRAM_BANK_ACT_TIMING_CR = 0x1947;
  MDDR->core.ddrc.ODT_PARAM_1_CR = 0x0010;
  MDDR->core.ddrc.ODT_PARAM_2_CR = 0x0000;
  MDDR->core.ddrc.ADDR_MAP_COL_3_CR = 0x3300;
  MDDR->core.ddrc.MODE_REG_RD_WR_CR = 0x0000;
  MDDR->core.ddrc.MODE_REG_DATA_CR = 0x0000;
  MDDR->core.ddrc.PWR_SAVE_1_CR = 0x0514;
  MDDR->core.ddrc.PWR_SAVE_2_CR = 0x0000;
  MDDR->core.ddrc.ZQ_LONG_TIME_CR = 0x0200;
  MDDR->core.ddrc.ZQ_SHORT_TIME_CR = 0x0040;
  MDDR->core.ddrc.ZQ_SHORT_INT_REFRESH_MARGIN_CR[0] = 0x0012;
  MDDR->core.ddrc.ZQ_SHORT_INT_REFRESH_MARGIN_CR[1] = 0x0002;
  MDDR->core.ddrc.PERF_PARAM_1_CR = 0x4000;
  MDDR->core.ddrc.HPR_QUEUE_PARAM_CR[0] = 0x80f8;
  MDDR->core.ddrc.HPR_QUEUE_PARAM_CR[1] = 0x0007;
  MDDR->core.ddrc.LPR_QUEUE_PARAM_CR[0] = 0x80f8;
  MDDR->core.ddrc.LPR_QUEUE_PARAM_CR[1] = 0x0007;
  MDDR->core.ddrc.WR_QUEUE_PARAM_CR = 0x0200;
  MDDR->core.ddrc.PERF_PARAM_2_CR = 0x0001;
  MDDR->core.ddrc.PERF_PARAM_3_CR = 0x0000;
  MDDR->core.ddrc.DFI_RDDATA_EN_CR = 0x0003;
  MDDR->core.ddrc.DFI_MIN_CTRLUPD_TIMING_CR = 0x0003;
  MDDR->core.ddrc.DFI_MAX_CTRLUPD_TIMING_CR = 0x0040;
  MDDR->core.ddrc.DFI_WR_LVL_CONTROL_CR[0] = 0x0000;
  MDDR->core.ddrc.DFI_WR_LVL_CONTROL_CR[1] = 0x0000;
  MDDR->core.ddrc.DFI_RD_LVL_CONTROL_CR[0] = 0x0000;
  MDDR->core.ddrc.DFI_RD_LVL_CONTROL_CR[1] = 0x0000;
  MDDR->core.ddrc.DFI_CTRLUPD_TIME_INTERVAL_CR = 0x0309;
  MDDR->core.ddrc.AXI_FABRIC_PRI_ID_CR = 0x0000;
  MDDR->core.ddrc.ECC_INT_CLR_REG = 0x0000;

  MDDR->core.phy.LOOPBACK_TEST_CR = 0x0000;
  MDDR->core.phy.CTRL_SLAVE_RATIO_CR = 0x0080;
  MDDR->core.phy.DATA_SLICE_IN_USE_CR = 0x0003;
  MDDR->core.phy.DQ_OFFSET_CR[0] = 0x00000000;
  MDDR->core.phy.DQ_OFFSET_CR[2] = 0x0000;
  MDDR->core.phy.DLL_LOCK_DIFF_CR = 0x000B;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[0] = 0x0040;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[1] = 0x0401;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[2] = 0x4010;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[3] = 0x0000;
  MDDR->core.phy.LOCAL_ODT_CR = 0x0001;
  MDDR->core.phy.RD_DQS_SLAVE_RATIO_CR[0] = 0x0040;
  MDDR->core.phy.RD_DQS_SLAVE_RATIO_CR[1] = 0x0401;
  MDDR->core.phy.RD_DQS_SLAVE_RATIO_CR[2] = 0x4010;
  MDDR->core.phy.WR_DATA_SLAVE_RATIO_CR[0] = 0x0040;
  MDDR->core.phy.WR_DATA_SLAVE_RATIO_CR[1] = 0x0401;
  MDDR->core.phy.WR_DATA_SLAVE_RATIO_CR[2] = 0x4010;
  MDDR->core.phy.WR_RD_RL_CR = 0x0021;
  MDDR->core.phy.RDC_WE_TO_RE_DELAY_CR = 0x0003;
  MDDR->core.phy.USE_FIXED_RE_CR = 0x0001;
  MDDR->core.phy.USE_RANK0_DELAYS_CR = 0x0001;
  MDDR->core.phy.CONFIG_CR = 0x0009;
  MDDR->core.phy.DYN_RESET_CR = 0x01;
  MDDR->core.ddrc.DYN_SOFT_RESET_CR = 0x01;
  // inclusive-language: enable
  // Wait for config
  while ((MDDR->core.ddrc.DDRC_SR) == 0x0000) {
  }
#endif
}

extern "C" void pw_boot_PreStaticConstructorInit() {
  // TODO(skeys) add "#if no_bootLoader" and the functions needed for init.

#if PW_MALLOC_ACTIVE
  pw_MallocInit(&pw_boot_heap_low_addr, &pw_boot_heap_high_addr);
#endif  // PW_MALLOC_ACTIVE
  pw_sys_io_Init();
}

// TODO(amontanez): pw_boot_PreMainInit() should get renamed to
// pw_boot_FinalizeBoot or similar when main() is removed.
extern "C" void pw_boot_PreMainInit() {
  pw::system::Init();
  vTaskStartScheduler();
  PW_UNREACHABLE;
}

extern "C" void sf2_SocInit() {
#if SF2_MSS_NO_BOOTLOADER
  Reset_Handler();
#endif
  pw_boot_Entry();
}

// This `main()` stub prevents another main function from being linked since
// this target deliberately doesn't run `main()`.
extern "C" int main() {}

extern "C" PW_NO_RETURN void pw_boot_PostMain() {
  // In case main() returns, just sit here until the device is reset.
  while (true) {
  }
  PW_UNREACHABLE;
}
