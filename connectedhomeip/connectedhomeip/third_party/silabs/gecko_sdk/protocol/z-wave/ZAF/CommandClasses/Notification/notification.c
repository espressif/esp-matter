/**
 * @file
 * Command Class Notification helper module.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <string.h> // For memset
#include "SizeOf.h"
#include <notification.h>
#include <CC_Notification.h>
#include "config_app.h"
#include <ZAF_file_ids.h>
#include <Assert.h>

//#define DEBUGPRINT
#include "DebugPrint.h"
/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef struct _NOTIFICATION_
{
  AGI_PROFILE agiProfile;
  uint8_t ep;
  notification_type_t type;
  uint8_t* pSupportedEvents;
  uint8_t supportedEventsLen;
  uint8_t event;
  uint8_t* pEvPar;
  uint8_t evParLen;
  uint8_t stateless : 1;
  uint8_t trigged   : 1;
  uint8_t noUsed    : 6;
} NOTIFICATION;

typedef struct _MY_NOTIFICATION_
{
  uint8_t lastActionGrp;
  NOTIFICATION grp[MAX_NUM_OF_NOTIFICATION_GROUPS];
} MY_NOTIFICATION;



/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

MY_NOTIFICATION myNotification;

bool notificationBurglerUnknownEvent = false;
uint8_t notificationBurglerSequenceNbr = 0;

static zpal_nvm_handle_t pFileSystem;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static bool ValidateNotificationType(notification_type_t notificationType, uint8_t endpoint);

//Saves notificationStatus to persistent memory
static void SaveNotificationStatus(
    notification_type_t notificationType,
    NOTIFICATION_STATUS notificationStatus,
    uint8_t endpoint)
{
  zpal_status_t status;

  ASSERT(pFileSystem != 0);

  SNotificationData tSource;
  status = zpal_nvm_read(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &tSource, sizeof(SNotificationData));
  ASSERT(ZPAL_STATUS_OK == status);

  uint8_t tGroupNumber = GetGroupIndex(&notificationType, endpoint);

  if(0xFF != tGroupNumber)
  {
    //Only write to nvm if file will change
    if(tSource.AlarmStatus[tGroupNumber] != (uint8_t)notificationStatus)
    {
      tSource.AlarmStatus[tGroupNumber] = (uint8_t)notificationStatus;
      status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &tSource, sizeof(SNotificationData));
      ASSERT(ZPAL_STATUS_OK == status);
    }
  }
}

//Saves notificationStatus to persistent memory
static void SaveNotificationStatusForType(
    notification_type_t notificationType,
    NOTIFICATION_STATUS notificationStatus)
{
  zpal_status_t status;

  ASSERT(pFileSystem != 0);

  for (uint8_t i = 0; i < MAX_NUM_OF_NOTIFICATION_GROUPS; i++) {
    if(myNotification.grp[i].type == notificationType)
    {
      SNotificationData tSource;
      status = zpal_nvm_read(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &tSource, sizeof(SNotificationData));
      ASSERT(ZPAL_STATUS_OK == status);

      if(tSource.AlarmStatus[i] != (uint8_t)notificationStatus)
      {
        tSource.AlarmStatus[i] = (uint8_t)notificationStatus;
        status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &tSource, sizeof(SNotificationData));
        ASSERT(ZPAL_STATUS_OK == status);
      }
    }
  }
}

void InitNotification(zpal_nvm_handle_t pFS)
{
  ASSERT(pFS != NULL);

  uint8_t i = 0;
  notificationBurglerUnknownEvent = false;

  for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
  {
    myNotification.grp[i].agiProfile.profile_MS = 0;
    myNotification.grp[i].agiProfile.profile_LS = 0;
    myNotification.grp[i].ep = 0xff;
    myNotification.grp[i].pSupportedEvents = NULL;
    myNotification.grp[i].supportedEventsLen = 0;
    myNotification.grp[i].type = NOTIFICATION_TYPE_NONE;
    myNotification.grp[i].event = 0;
    myNotification.grp[i].stateless = 0;
    myNotification.grp[i].pEvPar = NULL;
    myNotification.grp[i].evParLen = 0;
    myNotification.grp[i].trigged = 0;
  }
  myNotification.lastActionGrp = 0;

  pFileSystem = pFS;

  size_t   dataLen;
  const zpal_status_t status = zpal_nvm_get_object_size(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &dataLen);

  if ((ZPAL_STATUS_OK != status) || (ZAF_FILE_SIZE_NOTIFICATIONDATA != dataLen))
  {
    //By default set: NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED
    DefaultNotificationStatus(NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED);
  }
}

/**
 * @brief In case of multidevice type (0xff) replace the endpoint to the endpoint of the last action group, if the lastActionGrp is not valid than return endpoint of group 0
 * @param notificationType Notification type.
 * @param pEndpoint Endpoint number to be updated
 * @return bool
 */
static bool UpdateEndpointForRoot(
    notification_type_t notificationType,
    uint8_t *pEndpoint) 
{
  if (0xFF == notificationType)
  {
    if (0xFF != myNotification.lastActionGrp)
    {
      *pEndpoint = myNotification.grp[myNotification.lastActionGrp].ep;
    }
    else
    {
      *pEndpoint = myNotification.grp[0].ep;
    }

    return true;
  }

  return false;
}

void DefaultNotificationStatus(NOTIFICATION_STATUS status)
{
  ASSERT(pFileSystem != 0);

  SNotificationData tDefaultAlarmStatus;
  if(0 != status)
  {
    memset(&tDefaultAlarmStatus, 0xFF, sizeof(tDefaultAlarmStatus));
  }
  else
  {
    memset(&tDefaultAlarmStatus, 0x00, sizeof(tDefaultAlarmStatus));
  }

  const zpal_status_t write_status = zpal_nvm_write(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &tDefaultAlarmStatus, sizeof(SNotificationData));
  ASSERT(ZPAL_STATUS_OK == write_status);
}


/*============================ AddNotification ===============================
** Function description
** This function...
**
** Side effects:
**
**-------------------------------------------------------------------------*/
bool AddNotification(
    AGI_PROFILE const * const pAgiProfile,
    notification_type_t notificationType,
    uint8_t * pSupportedEvents,
    uint8_t supportedEventsLen,
    bool stateless,
    uint8_t endpoint,
	NOTIFICATION_STATUS defaultNotificationStatus,
	bool fileAlreadySet
	)
{
  uint8_t i;
  /*Find free slot*/
  for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
  {
    if( 0 == myNotification.grp[i].type)
    {
      /*Event configuration*/
      myNotification.lastActionGrp = i;
      myNotification.grp[i].agiProfile.profile_MS = pAgiProfile->profile_MS;
      myNotification.grp[i].agiProfile.profile_LS = pAgiProfile->profile_LS;
      myNotification.grp[i].ep = endpoint;
      myNotification.grp[i].type = notificationType;
      myNotification.grp[i].pSupportedEvents = pSupportedEvents;
      myNotification.grp[i].supportedEventsLen = supportedEventsLen;
      myNotification.grp[i].stateless = (stateless == false) ? 0 : 1;

      /*Event state*/
      myNotification.grp[i].event = 0;
      myNotification.grp[i].pEvPar = NULL;
      myNotification.grp[i].evParLen = 0;
      myNotification.grp[i].trigged = 0;

      if(!fileAlreadySet)
      {
        //Saves notificationStatus to persistent memory
        SaveNotificationStatus(notificationType, defaultNotificationStatus, endpoint);
      }
      return true;
    }
  }
  return false;
}

/**
 * @brief Search the index of group.
 * @param notificationType Notification type.
 * @param endpoint Endpoint number
 * @return If the given endpoint is 0 (root) it return the firs groupnumber where the notification type is the same. Otherwise return the group number where the endpoint and type is matched.
 */
uint8_t GetGroupIndex(notification_type_t* pNotificationType, uint8_t endpoint)
{
  uint8_t i = 0;

  DPRINTF("\r\nGetGroupIndex %d", *pNotificationType);

  if(0xFF == *pNotificationType)
  {
    /*Check last action is ready*/
    if(0xff == myNotification.lastActionGrp)
    {
      /* no last action, take first*/
      myNotification.lastActionGrp = 0;
    }

    if(endpoint == 0)
    {
      *pNotificationType =  myNotification.grp[myNotification.lastActionGrp].type;
    }
    else
    {
      if(myNotification.grp[myNotification.lastActionGrp].ep == endpoint)
      {
        *pNotificationType =  myNotification.grp[myNotification.lastActionGrp].type;
      }
      else{
        /*find notification out from end-point*/
        for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
        {
          if(myNotification.grp[i].ep == endpoint)
          {
            *pNotificationType =  myNotification.grp[i].type;
          }
        }
      }
    }
  }

  for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
  {
    DPRINTF("%d %d ", myNotification.grp[i].type, *myNotification.grp[i].pSupportedEvents);
  
    if( (myNotification.grp[i].type == *pNotificationType) && (ENDPOINT_ROOT == endpoint) )
    {
      DPRINTF("ID %d", i);
      return i;
    }
    else
    {
       if((myNotification.grp[i].type == *pNotificationType) && (myNotification.grp[i].ep == endpoint))
       {
        DPRINTF("ID %d", i);
        return i;
       }

    }
  }
  return 0xff;
}

/**
 * @brief Validates the given notificationType - endpoint pairs, and updates the endpoint in a special case.
 * @param notificationType Notification type.
 * @param endpoint Endpoint number
 * @return False if the given endpoint not suppert the given notification type, otherwise is true.
 */
bool FindNotificationEndpoint(
    notification_type_t notificationType,
    uint8_t * pEndpoint)
{
  DPRINTF("\r\nFindNotificationEndpoint %d EP %d\r\n", notificationType, *pEndpoint);
  bool valid = ValidateNotificationType(notificationType , *pEndpoint );
  if (valid)
  {
	  return true;
  }
  else if ( (false == valid) && (ENDPOINT_ROOT == *pEndpoint) && (UpdateEndpointForRoot(notificationType, pEndpoint)) )
  {
	  return true;
  }
  return false;
}

e_cmd_handler_return_code_t handleAppNotificationSet(
    notification_type_t notificationType,
    NOTIFICATION_STATUS_SET notificationStatus,
    uint8_t endpoint)
{
  UNUSED(endpoint);
  NOTIFICATION_STATUS newStatus = NOTIFICATION_STATUS_NO_PENDING_NOTIFICATION;

  if(NOTIFICATION_STATUS_SET_UNSOLICIT_DEACTIVATED == notificationStatus)
  {
	  newStatus = NOTIFICATION_STATUS_UNSOLICIT_DEACTIVATED;
  }
  else if(NOTIFICATION_STATUS_SET_UNSOLICIT_ACTIVATED == notificationStatus)
  {
	  newStatus = NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED;
  }
  
  if (endpoint == 0) {
      // Enable/disable notification status for all EndPoints in case of root node is addressed

      SaveNotificationStatusForType(
              notificationType,
              newStatus);
    } else {
      // Enable/disable notification status for the requested EndPoint only
      SaveNotificationStatus(
          notificationType,
          newStatus,
          endpoint
        );
    }

  return E_CMD_HANDLER_RETURN_CODE_HANDLED;
}

void handleCmdClassNotificationEventSupportedReport(
    notification_type_t notificationType,
    uint8_t * pNbrBitMask,
    uint8_t * pBitMaskArray,
    uint8_t endpoint)
{
  if( true == (ValidateNotificationType(notificationType, endpoint)) )
  {
    notification_type_t temp_notificationType = notificationType;
    uint8_t grpNo = GetGroupIndex(&temp_notificationType, endpoint );
    uint8_t i;

    if(temp_notificationType != notificationType)
    {
      grpNo = 0xff;
    }

    *pNbrBitMask = 0;
    if(0xff == grpNo)
    {
      return;
    }
    for( i = 0; i < myNotification.grp[grpNo].supportedEventsLen; i++)
    {
      if( *pNbrBitMask < myNotification.grp[grpNo].pSupportedEvents[i] )
      {
        *pNbrBitMask = myNotification.grp[grpNo].pSupportedEvents[i];
      }

      pBitMaskArray[myNotification.grp[grpNo].pSupportedEvents[i]/8] |= (uint8_t)(0x01<< (myNotification.grp[grpNo].pSupportedEvents[i]%8));
    }
    /*calc number of bitmask bytes*/
    *pNbrBitMask = (*pNbrBitMask / 8) + 1;
  }
  else
  {
    /*Only support Unkown event why bit maks is 0*/
    *pNbrBitMask = 0;
  }
}

NOTIFICATION_STATUS CmdClassNotificationGetNotificationStatus(
    notification_type_t notificationType,
    uint8_t endpoint)
{
  NOTIFICATION_STATUS status = NOTIFICATION_STATUS_UNSOLICIT_DEACTIVATED;
  uint8_t grp = GetGroupIndex( &notificationType, endpoint );

  if(0xff != grp)
  {
    ASSERT(pFileSystem != 0);

    SNotificationData tNotificationData;

    const zpal_status_t read_status = zpal_nvm_read(pFileSystem, ZAF_FILE_ID_NOTIFICATIONDATA, &tNotificationData, sizeof(SNotificationData));
    ASSERT(ZPAL_STATUS_OK == read_status);

    uint8_t AlarmStatus = tNotificationData.AlarmStatus[grp];

    if(AlarmStatus)
    {
      status = NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED;
    }
  }
  DPRINTF("\r\nCCStatus %d\r\n", status);

  return status;
}

bool CmdClassNotificationGetNotificationEvent(
    notification_type_t * pNotificationType,
    uint8_t * pNotificationEvent,
    uint8_t * pEventPar,
    uint8_t * pEvNbrs,
    uint8_t endpoint)
{
  uint8_t i = 0;
  uint8_t grpNo = GetGroupIndex(pNotificationType, endpoint );
  *pEventPar = 0;
  *pEvNbrs = 0;
  if(0xff == grpNo)
  {
    return false;
  }

  DPRINTF("GetNotificationEvent %d %d ", *pNotificationType, *pNotificationEvent);
  /*check valid type*/
  if( true == (ValidateNotificationType(*pNotificationType, endpoint)) )
  {
    DPRINTF("%d", myNotification.grp[grpNo].event);

    /*check valid event*/
    DPRINT(" valid event ");
    for(i = 0; i < myNotification.grp[grpNo].supportedEventsLen; i++)
    {
      DPRINTF("%d", myNotification.grp[grpNo].pSupportedEvents[i]);


      if((*pNotificationEvent == myNotification.grp[grpNo].pSupportedEvents[i]) && (0x00 != *pNotificationEvent))
      {
        /* Found correct supported event*/
        if(*pNotificationEvent == myNotification.grp[grpNo].event)
        {
          /* Event in queue*/
          *pEvNbrs = myNotification.grp[grpNo].evParLen;
          for(i = 0; i < myNotification.grp[grpNo].evParLen; i++)
          {
            pEventPar[i] = myNotification.grp[grpNo].pEvPar[i];
          }
          return true;
        }
        else
        {
          /*No event in queue*/
          *pNotificationEvent = 0x00;
          *pNotificationEvent = 0;
          *pEventPar = 0;
          return true;
        }
      }
    }

    if( *pNotificationEvent == 0x00)
    {
      /*Check for any event in queue*/
      DPRINT("!");
      DPRINTF("0x%x", myNotification.lastActionGrp);
      if( 0xff != myNotification.lastActionGrp)
      {
        uint8_t i = 0;
        *pNotificationEvent = myNotification.grp[grpNo].event;
        *pEvNbrs = myNotification.grp[grpNo].evParLen;
        for(i = 0; i < myNotification.grp[grpNo].evParLen; i++)
        {
          pEventPar[i] = myNotification.grp[grpNo].pEvPar[i];
        }
        //myNotification.lastActionGrp = 0xff;// empty last action

        return true;
      }
      else
      {
        DPRINT("%");
        /*No event in queue*/
        *pNotificationEvent = 0x00;
        return true;
      }
    }

    /* Event is not supported!*/
    DPRINT("-");
    *pNotificationEvent = 0xFE;
    *pEventPar = 0;
    return true;
  }
  return false;
}

void NotificationEventTrigger(
    AGI_PROFILE const * const pAgiProfile,
	notification_type_t type,
    uint8_t notificationEvent,
    uint8_t * pEvPar,
    uint8_t evParLen,
    uint8_t sourceEndpoint)
{
  uint8_t i;
  for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
  {
    if( myNotification.grp[i].agiProfile.profile_MS == pAgiProfile->profile_MS &&
        myNotification.grp[i].agiProfile.profile_LS  == pAgiProfile->profile_LS &&
        myNotification.grp[i].type == type &&
        myNotification.grp[i].ep == sourceEndpoint
      )
    {
      DPRINTF("\r\nNTrig %d %d\r\n", myNotification.grp[i].type, myNotification.grp[i].ep);

      //myNotification.lastNotificationType = myNotification.grp[i].type;
      myNotification.lastActionGrp = i;
      myNotification.grp[i].event = notificationEvent;
      myNotification.grp[i].pEvPar = pEvPar;
      myNotification.grp[i].evParLen = evParLen;
      myNotification.grp[i].trigged = 1;
      i = MAX_NUM_OF_NOTIFICATION_GROUPS;
    }
  }
}

JOB_STATUS UnsolicitedNotificationAction(
  AGI_PROFILE const * const pProfile,
  uint8_t sourceEndpoint,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult))
{
  if (myNotification.lastActionGrp >= MAX_NUM_OF_NOTIFICATION_GROUPS)
  {
    return JOB_STATUS_BUSY;
  }

  if ((myNotification.grp[myNotification.lastActionGrp].agiProfile.profile_MS == pProfile->profile_MS) &&
      (myNotification.grp[myNotification.lastActionGrp].agiProfile.profile_LS == pProfile->profile_LS) &&
      (myNotification.grp[myNotification.lastActionGrp].trigged  == 1) &&
      (NOTIFICATION_STATUS_UNSOLICIT_ACTIVATED ==
      CmdClassNotificationGetNotificationStatus(
          myNotification.grp[myNotification.lastActionGrp].type,
          myNotification.grp[myNotification.lastActionGrp].ep))
      )
  {
    DPRINTF("\r\nUnAction %d %d\r\n", myNotification.lastActionGrp, myNotification.grp[myNotification.lastActionGrp].trigged);
    DPRINTF("%d\r\n", sourceEndpoint);
    DPRINT("a");
    return CmdClassNotificationReport( pProfile,
                                   sourceEndpoint,
                                   myNotification.grp[myNotification.lastActionGrp].type,
                                   myNotification.grp[myNotification.lastActionGrp].event,
                                   pCallback);
  }
  myNotification.grp[myNotification.lastActionGrp].trigged = 0;
  DPRINT("\r\nb");
  return JOB_STATUS_BUSY;
}

void ClearLastNotificationAction(AGI_PROFILE const * const pAgiProfile, uint8_t sourceEndpoint)
{
  if (myNotification.lastActionGrp < MAX_NUM_OF_NOTIFICATION_GROUPS)
  {
    if( myNotification.grp[myNotification.lastActionGrp].agiProfile.profile_MS == pAgiProfile->profile_MS &&
        myNotification.grp[myNotification.lastActionGrp].agiProfile.profile_LS  == pAgiProfile->profile_LS &&
        myNotification.grp[myNotification.lastActionGrp].ep == sourceEndpoint)
    {
      myNotification.grp[myNotification.lastActionGrp].event = 0;
      myNotification.grp[myNotification.lastActionGrp].pEvPar = NULL;
      myNotification.grp[myNotification.lastActionGrp].evParLen = 0;
      myNotification.grp[myNotification.lastActionGrp].trigged = 0;
    }
  }
  myNotification.lastActionGrp = 0xff;
}

/*
  Set a bit in the notifcation type bitmask array that correspond to notifcation type.
*/
static void
SetNotificationBit(uint8_t index,
                   uint8_t *pNbrBitMask,
                   uint8_t *pBitMaskArray,
                   uint8_t bBitMaskLen)
{
  /*Don't write to the bitmask array if the index is out of bound or */
  /*the notification type is invalid*/
  if ((myNotification.grp[index].type != NOTIFICATION_TYPE_NONE) &&
      (bBitMaskLen > (myNotification.grp[index].type / 8)))
  {
    /* Find max number of bit masks*/
    if (*pNbrBitMask < ((myNotification.grp[index].type / 8) + 1))
    {
      *pNbrBitMask = (uint8_t)((myNotification.grp[index].type / 8) + 1);
    }
    /* Add Bit in bit-mask byte (myNotification.grp[i].type / 8)*/
    *(pBitMaskArray + (myNotification.grp[index].type / 8)) |= (uint8_t)(1 << ((myNotification.grp[index].type) % 8));
  }
}

void handleCmdClassNotificationSupportedReport(
    uint8_t * pNbrBitMask,
    uint8_t * pBitMaskArray,
    uint8_t   bBitMaskLen,
    uint8_t endpoint)
{
  *pNbrBitMask = 0;
  for(uint8_t i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++) {
    if((0 == endpoint) ||                          /* find all notification types for device*/
       (myNotification.grp[i].ep == endpoint) ) {  /* find all notification types for endpoint*/
      SetNotificationBit(i, pNbrBitMask, pBitMaskArray, bBitMaskLen);
    }
  }
}

/**
 * @brief Validates whether a given notification type is set for a given endpoint.
 * @param notificationType Notification type.
 * @param endpoint Endpoint number
 * @return true if notification type is set for endpoint, false otherwise.
 */
static bool ValidateNotificationType(notification_type_t notificationType, uint8_t endpoint)
{
  uint8_t i = 0;

  if( 0xFF == notificationType)
  {
    for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
    {
      if(myNotification.grp[i].ep == endpoint)
      {
        return true;
      }
    }
    return false;
  }

  for(i = 0; i< MAX_NUM_OF_NOTIFICATION_GROUPS; i++)
  {
    if(myNotification.grp[i].type == notificationType)
    {
      return true;
    }
  }
  return false;
}

