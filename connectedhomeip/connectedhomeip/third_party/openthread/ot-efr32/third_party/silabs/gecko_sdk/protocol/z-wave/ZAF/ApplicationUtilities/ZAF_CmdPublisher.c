/**
 * @file
 *
 * Command Publisher module source file
 * @copyright 2018 Silicon Laboratories Inc.
 */

#include "ZAF_CmdPublisher.h"
#include "Assert.h"
#include <string.h>
#include <ZW_application_transport_interface.h>

//#define DEBUGPRINT
#include "DebugPrint.h"

/**
 * Subscriber can be subscribed to one or more of following types
 */
typedef enum {
  CP_SUBSCRIBER_TYPE_ALL = 0, //!< Subscriber gets all incoming frames
  CP_SUBSCRIBER_TYPE_CC,      //!< Subscriber gets all incoming frames for a given Command Class
  CP_SUBSCRIBER_TYPE_CMD,     //!< Subscriber gets all incoming frames for a given command
  CP_SUBSCRIBER_TYPE_UNDEFINED,
} e_ZAF_CP_SubscriberType_t;

/**
 * Subscriber struct
 */
typedef struct
{
  e_ZAF_CP_SubscriberType_t type;   /*! Indicating which validation to make to publish the command to the subscriber */
  uint16_t CmdClass;                /*! Command Classes can be up to 2 bytes*/
  uint8_t Cmd;                      /*! Command */
  void* pSubscriberContext;         /*! pContext is used for "object" like modules, it is a pointer to
                                        the main data structure of the subscriber. */
  zaf_cp_subscriberFunction_t pFunction; /*! pFunction will be called: pFunction(pSubscriberContext, package) */
} s_ZAF_CP_Subscriber_t;

/**
 * Context used to initialize Command Subscriber module
 */
typedef struct
{
  uint8_t cmdSubscriberListLength;        /*! length of the List of subscribers */
  s_ZAF_CP_Subscriber_t subscriberList[]; /*! List of subscribed modules*/
} s_ZAF_CP_Context_t;


static void InitEntry(s_ZAF_CP_Subscriber_t* subscriber);
static uint8_t SizeofSubscribersList(CP_Handle_t handle);
static bool validityCheck(s_ZAF_CP_Subscriber_t* subscriber);
static s_ZAF_CP_Subscriber_t * LookupSubscriber(CP_Handle_t cp, s_ZAF_CP_Subscriber_t* pSubscriber);
static bool ZAF_CP_Subscribe(CP_Handle_t handle,
                             void* pSubscriberContext,
                             e_ZAF_CP_SubscriberType_t type,
                             zaf_cp_subscriberFunction_t pFunction,
                             uint16_t CmdClass,
                             uint8_t Cmd);
static bool ZAF_CP_Unsubscribe(CP_Handle_t handle,
                               void* pSubscriberContext,
                               e_ZAF_CP_SubscriberType_t type,
                               zaf_cp_subscriberFunction_t pFunction,
                               uint16_t CmdClass,
                               uint8_t Cmd);


// Sanity checks to ensure the public CP_STORAGE_SIZE macro is aligned with the private struct definitions
STATIC_ASSERT(ZAF_CP_HEADER_SIZE == offsetof(s_ZAF_CP_Context_t, subscriberList),
              ASSERT_ZAF_CP_STORAGE_SIZE_macro_headersize_incorrect_1);
STATIC_ASSERT(ZAF_CP_ELEMENT_SIZE == sizeof(s_ZAF_CP_Subscriber_t),
              ASSERT_ZAF_CP_STORAGE_SIZE_macro_elementsize_incorrect_1);
STATIC_ASSERT(ZAF_CP_STORAGE_SIZE(1) == offsetof(s_ZAF_CP_Context_t, subscriberList[1]),
              ASSERT_ZAF_CP_STORAGE_SIZE_macro_calc_incorrect_1);
STATIC_ASSERT(ZAF_CP_STORAGE_SIZE(15) == offsetof(s_ZAF_CP_Context_t, subscriberList[15]),
              ASSERT_ZAF_CP_STORAGE_SIZE_macro_calc_incorrect_15);

CP_Handle_t ZAF_CP_Init(void *pStorage, uint8_t numSubscribers)
{
  DPRINTF("%s\n", __func__);
  s_ZAF_CP_Context_t *pCtx = (s_ZAF_CP_Context_t *) pStorage;
  pCtx->cmdSubscriberListLength = numSubscribers;

  for (int i = 0; i < numSubscribers; i++)
  {
    InitEntry(&pCtx->subscriberList[i]);
  }
  return pCtx;
}

bool ZAF_CP_SubscribeToAll(CP_Handle_t handle,
                           void* pSubscriberContext,
                           zaf_cp_subscriberFunction_t pFunction)
{
  return ZAF_CP_Subscribe(handle, pSubscriberContext, CP_SUBSCRIBER_TYPE_ALL, pFunction, 0x00, 0x00);
}

bool ZAF_CP_SubscribeToCC(CP_Handle_t handle,
                          void* pSubscriberContext,
                          zaf_cp_subscriberFunction_t pFunction,
                          uint16_t CmdClass)
{
  return ZAF_CP_Subscribe(handle, pSubscriberContext, CP_SUBSCRIBER_TYPE_CC, pFunction, CmdClass, 0x00);
}

bool ZAF_CP_SubscribeToCmd(CP_Handle_t handle,
                           void* pSubscriberContext,
                           zaf_cp_subscriberFunction_t pFunction,
                           uint16_t CmdClass,
                           uint8_t Cmd)
{
  return ZAF_CP_Subscribe(handle, pSubscriberContext, CP_SUBSCRIBER_TYPE_CMD, pFunction, CmdClass, Cmd);
}


bool ZAF_CP_UnsubscribeToAll(CP_Handle_t handle,
                             void* pSubscriberContext,
                             zaf_cp_subscriberFunction_t pFunction)
{
  return ZAF_CP_Unsubscribe(handle, pSubscriberContext, CP_SUBSCRIBER_TYPE_ALL, pFunction, 0x00, 0x00);
}

bool ZAF_CP_UnsubscribeToCC(CP_Handle_t handle,
                            void* pSubscriberContext,
                            zaf_cp_subscriberFunction_t pFunction,
                            uint16_t CmdClass)
{
  return ZAF_CP_Unsubscribe(handle, pSubscriberContext, CP_SUBSCRIBER_TYPE_CC, pFunction, CmdClass, 0x00);
}

bool ZAF_CP_UnsubscribeToCmd(CP_Handle_t handle,
                             void* pSubscriberContext,
                             zaf_cp_subscriberFunction_t pFunction,
                             uint16_t CmdClass,
                             uint8_t Cmd)
{
  return ZAF_CP_Unsubscribe(handle, pSubscriberContext, CP_SUBSCRIBER_TYPE_CMD, pFunction, CmdClass, Cmd);
}


void ZAF_CP_CommandPublish(CP_Handle_t handle, void* pRxPackage)
{
  s_ZAF_CP_Context_t *pContext = (s_ZAF_CP_Context_t *) handle;

  for (int i = 0; i < pContext->cmdSubscriberListLength; i++)
  {
    SZwaveReceivePackage* pZwRxPackage = (SZwaveReceivePackage*) pRxPackage;
//    DPRINTF("Entry %d type: %d, CmdClass: %#02x Cmd: %#02x\n", i, pContext->subscriberList[i].type,
//            pContext->subscriberList[i].CmdClass, pContext->subscriberList[i].Cmd);
    switch (pContext->subscriberList[i].type)
    {
      case CP_SUBSCRIBER_TYPE_ALL:
        pContext->subscriberList[i].pFunction(pContext->subscriberList[i].pSubscriberContext, pZwRxPackage);
        break;
      case CP_SUBSCRIBER_TYPE_CC:
        if (pZwRxPackage->uReceiveParams.Rx.Payload.rxBuffer.ZW_Common.cmdClass == pContext->subscriberList[i].CmdClass)
        {
          pContext->subscriberList[i].pFunction(pContext->subscriberList[i].pSubscriberContext, pZwRxPackage);
        }
        break;
      case CP_SUBSCRIBER_TYPE_CMD:
        if (pZwRxPackage->uReceiveParams.Rx.Payload.rxBuffer.ZW_Common.cmdClass == pContext->subscriberList[i].CmdClass
            && pZwRxPackage->uReceiveParams.Rx.Payload.rxBuffer.ZW_Common.cmd == pContext->subscriberList[i].Cmd)
        {
          pContext->subscriberList[i].pFunction(pContext->subscriberList[i].pSubscriberContext, pZwRxPackage);
        }
        break;
      case CP_SUBSCRIBER_TYPE_UNDEFINED:
        // Empty slot, move on
        break;
      default:
        DPRINTF("Unknown subscriber type %d\n", pContext->subscriberList[i].type);
        break;
    }
  }
}

/**
 * Add new subscriber to the list
 *
 * @param handle Handle to initialized array. New subscriber should be added to this array
 * @param pSubscriberContext Context of the subscriber to be added
 * @param type Subscription type
 * @param pFunction Callback function of the subscriber to be added
 * @param CmdClass If applicable, Cmd class of the subscriber to be added
 * @param Cmd If applicable, Specific command of the subscriber to be added
 *
 * @return true if subscriber has been added, false otherwise
 */
static bool ZAF_CP_Subscribe(CP_Handle_t handle,
                             void* pSubscriberContext,
                             e_ZAF_CP_SubscriberType_t type,
                             zaf_cp_subscriberFunction_t pFunction,
                             uint16_t CmdClass,
                             uint8_t Cmd)
{
  // If all initialized entries are already occupied, return.
  if (SizeofSubscribersList(handle) == ((s_ZAF_CP_Context_t *) handle)->cmdSubscriberListLength)
  {
    DPRINT("Failed adding new subscriber: Maximum number of subscribers reached\n");
    return false;
  }

  s_ZAF_CP_Subscriber_t subscriber = {
                                       .type = type,
                                       .Cmd = Cmd,
                                       .CmdClass = CmdClass,
                                       .pFunction = pFunction,
                                       .pSubscriberContext = pSubscriberContext
  };

  //Check that subscriber has valid data
  if (!validityCheck(&subscriber))
  {
    DPRINT("Failed adding new subscriber: Incorrect input data\n");
    return false;
  }

  // Check if subscriber is already added
  if (NULL != LookupSubscriber(handle, &subscriber))
  {
    DPRINT("Failed to add new subscriber: already subscribed\n");
    return false;
  }

  s_ZAF_CP_Context_t *pCtx = (s_ZAF_CP_Context_t *) handle;

  for (int i = 0; i < pCtx->cmdSubscriberListLength; i++)
  {
    // Look through the array until uninitialized member is found
    if (pCtx->subscriberList[i].type == CP_SUBSCRIBER_TYPE_UNDEFINED)
    {
      // Empty one found
      pCtx->subscriberList[i].type = type;
      pCtx->subscriberList[i].pFunction = pFunction;
      pCtx->subscriberList[i].pSubscriberContext = pSubscriberContext;
      if (CmdClass)
      {
        pCtx->subscriberList[i].CmdClass = CmdClass;
        if (Cmd)
        {
          pCtx->subscriberList[i].Cmd = Cmd;
        }
      }
      DPRINTF("Added subscriber[%d]: type: %d, CmdClass: %#02x Cmd: %#02x\n", i, pCtx->subscriberList[i].type,
              pCtx->subscriberList[i].CmdClass, pCtx->subscriberList[i].Cmd);
      return true;
    }
  }

  DPRINT("Failed to add new subscriber\n");
  return false;
}

static bool ZAF_CP_Unsubscribe(CP_Handle_t handle,
                               void* pSubscriberContext,
                               e_ZAF_CP_SubscriberType_t type,
                               zaf_cp_subscriberFunction_t pFunction,
                               uint16_t CmdClass,
                               uint8_t Cmd)
{
  s_ZAF_CP_Context_t *pCtx = (s_ZAF_CP_Context_t *)handle;
  for (uint8_t i = 0; i < pCtx->cmdSubscriberListLength; i++)
  {
    if (pCtx->subscriberList[i].type == type
        && pCtx->subscriberList[i].CmdClass == CmdClass
        && pCtx->subscriberList[i].Cmd == Cmd
        && pCtx->subscriberList[i].pFunction == pFunction
        && pCtx->subscriberList[i].pSubscriberContext == pSubscriberContext
        )
    {
      InitEntry(&pCtx->subscriberList[i]);
      return true;
    }
  }
  DPRINT("Failed to Unsubscribe\n");
  return false;
}

/**
 * Private lookup function - used by publisher and during subscription.
 * Finds subscriber in the subscribers list
 *
 * @param handle Handle to allocated memory
 * @param pSubscriber Subscriber to be found
 * @return Pointer to subscriber, or null, if not found
 */
static s_ZAF_CP_Subscriber_t * LookupSubscriber(CP_Handle_t handle, s_ZAF_CP_Subscriber_t* pSubscriber)
{
  s_ZAF_CP_Context_t *pCtx = handle;
  for (uint8_t i = 0; i < pCtx->cmdSubscriberListLength; i++)
  {
    if (pCtx->subscriberList[i].type == pSubscriber->type
        && pCtx->subscriberList[i].CmdClass == pSubscriber->CmdClass
        && pCtx->subscriberList[i].Cmd == pSubscriber->Cmd
        && pCtx->subscriberList[i].pFunction == pSubscriber->pFunction
        && pCtx->subscriberList[i].pSubscriberContext == pSubscriber->pSubscriberContext
        )
    {
      return &pCtx->subscriberList[i];
    }
  }
  return NULL;
}

/**
 * Initialize new subscriber
 *
 * @param subscriber Set this subscriber to default values
 */
static void InitEntry(s_ZAF_CP_Subscriber_t* subscriber)
{
  subscriber->type = CP_SUBSCRIBER_TYPE_UNDEFINED;
  subscriber->CmdClass = 0x00;
  subscriber->Cmd = 0x00;
  subscriber->pFunction = NULL;
  subscriber->pSubscriberContext = NULL;
}

/**
 * Calculates total amount of items in subscribers list
 *
 * @param handle Pointer to the list of subscribers
 * @return number of added subscribers
 */
static uint8_t SizeofSubscribersList(CP_Handle_t handle)
{
  uint8_t items = 0;
  s_ZAF_CP_Context_t *pCtx = handle;
  for (uint8_t i = 0; i < pCtx->cmdSubscriberListLength; i++)
  {
    if (pCtx->subscriberList[i].type != CP_SUBSCRIBER_TYPE_UNDEFINED)
    {
      items++;
    }
  }
  return items;
}

/**
 * Check if subscribers data is valid. Used before new subscriber is added
 *
 * @param subscriber Subscriber structure to check
 * @return true if subscriber is valid, false if conflict is found
 */
static bool validityCheck(s_ZAF_CP_Subscriber_t* subscriber)
{
//  DPRINTF("%s type: %d, CC:%#02x, Cmd: %#02x, function: %p\n",
//          __func__, subscriber->type, subscriber->CmdClass, subscriber->Cmd, subscriber->pFunction);
  if ((NULL == subscriber->pFunction) ||
      (CP_SUBSCRIBER_TYPE_CC == subscriber->type && !subscriber->CmdClass) ||
      (CP_SUBSCRIBER_TYPE_CMD == subscriber->type && (!subscriber->Cmd || !subscriber->CmdClass)))
  {
    return false;
  }
  return true;
}
