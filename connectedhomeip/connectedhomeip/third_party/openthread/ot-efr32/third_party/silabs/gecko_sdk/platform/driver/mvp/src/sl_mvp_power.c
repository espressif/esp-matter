#include "sl_mvp_power.h"
#include "sl_mvp_config.h"
#include "em_emu.h"
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#endif
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

// Software retained MVP registers.
static uint32_t mvp_reg_cfg = MVP_CFG_PERF0CNTSEL_RUN | MVP_CFG_PERF1CNTSEL_STALL | MVP_CFG_PERFCNTEN;

void sli_mvp_power_up(void)
{
  CMU->CLKEN1_SET = CMU_CLKEN1_MVP;
  MVP->EN_SET = MVP_EN_EN;
  MVP->IEN_SET = MVP_IEN_PERFCNT0 | MVP_IEN_PERFCNT1 | MVP_IEN_PROGDONE;
  MVP->CFG = mvp_reg_cfg;
}

void sli_mvp_power_down(void)
{
  if (((CMU->CLKEN1 & CMU_CLKEN1_MVP) != 0)
      && ((MVP->EN & MVP_EN_EN) != 0)) {
    mvp_reg_cfg = MVP->CFG;
    MVP->EN_CLR = MVP_EN_EN;
    while (MVP->EN & MVP_EN_DISABLING)
      ;
    CMU->CLKEN1_CLR = CMU_CLKEN1_MVP;
  }
}

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
// Power manager callback when entering EM2/EM3 or waking up from EM2/EM3.
static void sli_mvp_on_em_transition(sl_power_manager_em_t from, sl_power_manager_em_t to)
{
  (void) from;

  if ((to == SL_POWER_MANAGER_EM2) || (to == SL_POWER_MANAGER_EM3)) {
    sli_mvp_power_down();
  } else if ((to == SL_POWER_MANAGER_EM1) || (to == SL_POWER_MANAGER_EM0)) {
    sli_mvp_power_up();
  }
}

static sl_power_manager_em_transition_event_handle_t mvp_event_handle;
static const sl_power_manager_em_transition_event_info_t mvp_event_info = {
  .event_mask = (SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2
                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2
                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3
                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3),
  .on_event = sli_mvp_on_em_transition,
};

static void sli_mvp_add_transition_callback(void)
{
  sl_power_manager_subscribe_em_transition_event(&mvp_event_handle, &mvp_event_info);
}

static void sli_mvp_remove_transition_callback(void)
{
  sl_power_manager_unsubscribe_em_transition_event(&mvp_event_handle);
}
#endif

#if SL_MVP_POWER_MODE == 0 || SL_MVP_POWER_MODE == 1

#if SL_MVP_POWER_MODE == 1 && defined(SL_CATALOG_KERNEL_PRESENT)
  #error "SL_MVP_POWER_MODE=1 cannot be used together with an RTOS"
#endif

static void mvp_wait(void)
{
  while ((MVP->EN & MVP_EN_EN) == 0) {
    // If MVP is disabled then the status register will report "IDLE"
    // even if the MVP is not ready for any new program. This can happen
    // if sli_mvp_power_up() is not called after an EM2 wakeup.
    EFM_ASSERT(false);
  }

  // wait for complete
#if SL_MVP_POWER_MODE == 0
  while (!(MVP->STATUS & MVP_STATUS_IDLE)) {
  }
#elif SL_MVP_POWER_MODE == 1
  CORE_DECLARE_IRQ_STATE;

  // wait loop which allows other system interrupts to be handled
  while (!(MVP->STATUS & MVP_STATUS_IDLE)) {
    CORE_ENTER_CRITICAL();
    if (!(MVP->STATUS & MVP_STATUS_IDLE)) {
      EMU_EnterEM1();
    }
    CORE_EXIT_CRITICAL();
  }
#endif
}

void sli_mvp_power_init(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sli_mvp_add_transition_callback();
#endif
  sli_mvp_power_up();
}

void sli_mvp_power_deinit(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sli_mvp_remove_transition_callback();
#endif
  sli_mvp_power_down();
}

void sli_mvp_power_program_prepare(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && SL_MVP_POWER_MODE == 1
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif
  mvp_wait();
}

void sli_mvp_power_program_wait(void)
{
  mvp_wait();
}

void sli_mvp_power_program_complete(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && SL_MVP_POWER_MODE == 1
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif
}

#elif SL_MVP_POWER_MODE == 2

#if !defined(SL_CATALOG_KERNEL_PRESENT)
  #error "SL_MVP_POWER_MODE=2 requires an RTOS"
#endif

// Low power mode when an RTOS is used
#define SL_MVP_EVENT_FLAG_IDLE   0x1
static osEventFlagsId_t mvp_event_id;
static bool initialized = false;

void sli_mvp_power_init(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sli_mvp_add_transition_callback();
#endif
  sli_mvp_power_up();
  mvp_event_id = osEventFlagsNew(NULL);
  EFM_ASSERT(mvp_event_id != NULL);
}

void sli_mvp_power_deinit(void)
{
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sli_mvp_remove_transition_callback();
#endif
  sli_mvp_power_down();
  osStatus_t status = osEventFlagsDelete(mvp_event_id);
  EFM_ASSERT(status == osOK);
  initialized = false;
}

void sli_mvp_power_program_prepare(void)
{
  EFM_ASSERT((MVP->EN & MVP_EN_EN) != 0);

  if (!initialized) {
    // We must wait for os to be running before setting initial flag state
    uint32_t ret = osEventFlagsSet(mvp_event_id, SL_MVP_EVENT_FLAG_IDLE);
    EFM_ASSERT((int)ret >= 0);
    initialized = true;
  }
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif
  uint32_t ret = osEventFlagsWait(mvp_event_id, SL_MVP_EVENT_FLAG_IDLE, osFlagsWaitAny, osWaitForever);
  EFM_ASSERT((int)ret >= 0);
}

void sli_mvp_power_program_wait(void)
{
  uint32_t ret = osEventFlagsWait(mvp_event_id, SL_MVP_EVENT_FLAG_IDLE, osFlagsNoClear, osWaitForever);
  EFM_ASSERT((int)ret >= 0);
}

void sli_mvp_power_program_complete(void)
{
  uint32_t ret = osEventFlagsSet(mvp_event_id, SL_MVP_EVENT_FLAG_IDLE);
  EFM_ASSERT((int)ret >= 0);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif
}

#else
#error "Unknown SL_MVP_POWER_MODE value"
#endif
