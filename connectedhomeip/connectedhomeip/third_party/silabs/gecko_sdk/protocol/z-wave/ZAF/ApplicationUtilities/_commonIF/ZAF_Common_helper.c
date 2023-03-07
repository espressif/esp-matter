/**
 * @file
 * @copyright 2018 Silicon Laboratories Inc.
 */

#include "ZAF_Common_helper.h"

#include "ZW_TransportSecProtocol.h"
#include <ZW_TransportMulticast.h>
#include <CC_Common.h>
#include "ZAF_TSE.h"
#include <ZAF_CmdPublisher.h>
#include "ZAF_CC_Invoker.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/*
 * Max number of subscribers. Those are known so far:
 * ApplicationCommandHandler + Security Discovery + Multicast = 3
 */
#define CP_MAX_SUBSCRIBERS  3

static bool invoke_init(CC_handler_map_latest_t const * const p_cc_entry, zaf_cc_context_t context)
{
  UNUSED(context);
  if (NULL != p_cc_entry->init)
  {
    p_cc_entry->init();
  }
  return false;
}

void ZAF_Init(TaskHandle_t pAppTaskHandle,
              SApplicationHandles* pAppHandle,
              const SProtocolConfig_t * pAppProtocolConfig,
              void (*updateStayAwakePeriodFunc)(void)
              )
{
  DPRINT("* ZAF_Init *\r\n");
  ZAF_setApplicationData(pAppTaskHandle, pAppHandle, pAppProtocolConfig);

  // in case of FLiRS, which is currently only DoorLock, do some additional initialization
  if (isFLiRS(ZAF_getAppNodeInfo()))
  {
    DPRINT("Initialize power lock for FLiRS \r\n");
    zpal_pm_handle_t power_lock = zpal_pm_register(ZPAL_PM_TYPE_USE_RADIO);
    ZAF_setPowerLock(power_lock);
  }

  ZW_TransportMulticast_Init(NULL);
  ZW_TransportEndpoint_Init(NULL, updateStayAwakePeriodFunc);


  //Initializing TSE timers
  ZAF_TSE_Init();

  static ZAF_CP_STORAGE(content, CP_MAX_SUBSCRIBERS);
  ZAF_SetCPHandle(ZAF_CP_Init((void*) &content, CP_MAX_SUBSCRIBERS));
  if (false == ZAF_CP_SubscribeToAll(ZAF_getCPHandle(), NULL, (zaf_cp_subscriberFunction_t)ApplicationCommandHandler))
  {
    DPRINT("Subscription to Command Publisher failed!\r\n");
    ASSERT(false);
  }

  // Initialize command classes that have registered an init function.
  // Don't pass a context because invoke_init() doesn't require it.
  ZAF_CC_foreach(invoke_init, NULL);

  // more common function calls can be added here
}

static bool invoke_reset(CC_handler_map_latest_t const * const p_cc_entry, zaf_cc_context_t context)
{
  UNUSED(context);
  if (NULL != p_cc_entry->reset)
  {
    p_cc_entry->reset();
  }
  return false;
}

void ZAF_Reset(void)
{
  // Don't pass a context because invoke_reset() doesn't require it.
  ZAF_CC_foreach(invoke_reset, NULL);
}

void ZAF_FLiRS_StayAwake()
{
  DPRINT("\r\nZAF_FLiRS_StayAwake\r\n");
  zpal_pm_stay_awake(ZAF_getPowerLock(), 2000);
}
