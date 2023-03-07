/**
 * @file
 * Offers Power Management commands for Silabs targets only.
 * @attention Must be linked for Silabs build targets only.
 * @copyright 2022 Silicon Laboratories Inc.
 */
#include <stdint.h>
#include "cmd_handlers.h"
#include "ZW_SerialAPI.h"
#include "serialappl.h"
#include "zpal_power_manager.h"
#include "SwTimer.h"

extern zpal_pm_handle_t radio_power_lock;
extern zpal_pm_handle_t io_power_lock;
extern SSwTimer mWakeupTimer;

ZW_ADD_CMD(FUNC_ID_PM_STAY_AWAKE)
{
  /* HOST->ZW: PowerLock Type, timeout of stay awake, timeout of wakeup */
  /*           Power locks type 0 for radio and 1 for peripheral*/
  uint32_t timeout = (uint32_t)(frame->payload[1]<<24);
  timeout |= (uint32_t)(frame->payload[2]<<16);
  timeout |= (uint32_t)(frame->payload[3]<<8);
  timeout |= (uint32_t)(frame->payload[4]);

  uint32_t timeoutwakeup = (uint32_t)(frame->payload[5]<<24);
  timeoutwakeup |= (uint32_t)(frame->payload[6]<<16);
  timeoutwakeup |= (uint32_t)(frame->payload[7]<<8);
  timeoutwakeup |= (uint32_t)(frame->payload[8]);

  if (0 == frame->payload[0])
  {
    zpal_pm_stay_awake(radio_power_lock, timeout);
  }
  else if (1 == frame->payload[0])
  {
    zpal_pm_stay_awake(io_power_lock, timeout);
  }

  if (timeout && timeoutwakeup)
  {
    AppTimerDeepSleepPersistentStart(&mWakeupTimer, timeoutwakeup);
  }
  set_state_and_notify(stateIdle);
}


ZW_ADD_CMD(FUNC_ID_PM_CANCEL)
{
  /* HOST->ZW: PowerLock Type*/
  /*Power locks type 0 for radio and 1 for peripheral*/
  if (0 == frame->payload[0])
  {
    zpal_pm_cancel(radio_power_lock);
  }
  else if (1 == frame->payload[0])
  {
    zpal_pm_cancel(io_power_lock);
  }
  set_state_and_notify(stateIdle);  
}
