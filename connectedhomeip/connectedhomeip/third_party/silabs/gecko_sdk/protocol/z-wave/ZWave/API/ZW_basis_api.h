/**
 * @file
 * Z-Wave common controller/slave application interface.
 *
 * Z Wave <=> Application general software interface.
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ZW_BASIS_API_H_
#define _ZW_BASIS_API_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_typedefs.h>
#include <stdint.h>

/* Z-Wave includes */
#include <ZW_transport_api.h>
#include <ZW_classcmd.h>

/**
 * \defgroup COMMON Z-Wave Common API
 * This section describes interface functions that are implemented within all Z Wave nodes.
 * The first subsection defines functions that must be implemented within the application
 * modules, while the second subsection defines the functions that are implemented within
 * the Z Wave basis library.
 * Functions that do not complete the requested action before returning to the application
 *  (e.g. ZW_SEND_DATA) have a callback function pointer as one of the entry parameters.
 *  Unless explicitly specified this function pointer can be set to NULL (no action to take on completion).
 *
 *
 * \defgroup BASIS Z-Wave Basis API
 * This section defines functions that are implemented in all Z Wave nodes.
 */


/* APPLICATION_STATUS one of these are returned from ApplicationInit */
/* Protocol type */
typedef enum _APPLICATION_STATUS_
{
  APPLICATION_RUNNING,
  APPLICATION_POWER_DOWN,
  APPLICATION_TEST
} ZW_APPLICATION_STATUS;

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#define NUM_CHANNELS               3
#define NUM_CHANNELS_LR            4 /* 100kbps, 40kbps, 9.6kbps and US_LR1/US_LR2 */
#define NUM_CHANNELS_LR_END_DEVICE 2 /* US_LR1 and US_LR2 */

/* Node type structure used in ApplicationNodeInformation */
typedef struct _APPL_NODE_TYPE_
{
  uint8_t generic;
  uint8_t specific;
} APPL_NODE_TYPE;

typedef struct _NODE_TYPE_
{
  uint8_t basic;                 /* Basic Device Type, is it a Controller, Controller_Static, */
                              /* Slave or a Slave_Routing Device Type */
  uint8_t generic;               /* Generic Device Type */
  uint8_t specific;              /* Specific Device Type */
} NODE_TYPE;


/* Node info stored within the non-volatile memory */
/* This are the first (protocol part) payload bytes from the Node Infomation frame */
typedef struct _NODEINFO_
{
  uint8_t        capability;     /* Network capabilities */
  uint8_t        security;       /* Network security */
  uint8_t        reserved;
  NODE_TYPE   nodeType;       /* Basic, Generic and Specific Device types - Basic is generated... */
} NODEINFO;


typedef struct
{
  signed char rssi_dBm[NUM_CHANNELS_LR];
} RSSI_LEVELS;


/* ZW_NetworkLearnModeStart mode parameter definitions */
typedef enum _E_NETWORK_LEARN_MODE_ACTION_
{
  E_NETWORK_LEARN_MODE_DISABLE =  0,      /* Disable learn process */
  E_NETWORK_LEARN_MODE_INCLUSION  = 1,    /* Enable the learn process to do an inclusion */
  E_NETWORK_LEARN_MODE_EXCLUSION  = 2,    /* Enable the learn process to do an exclusion */
  E_NETWORK_LEARN_MODE_EXCLUSION_NWE = 3, /* Enable the learn process to do an network wide exclusion */
  E_NETWORK_LEARN_MODE_INCLUSION_SMARTSTART = 4,/* Enable the learn process to do an SMARTSTART inclusion */
  E_NETWORK_LEARN_MODE_COUNT
} E_NETWORK_LEARN_MODE_ACTION;


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/**
* Description of reset reason.
*
* Enum is defined in ZW API as its not HW platform specific.
* !!IMPORTANT: For backwards compability the enum values must comply with
* the values from ZW_WAKEUP below.
*/
typedef enum EResetReason_t
{
  ERESETREASON_PIN         = 0,   /**< Reset triggered by reset pin */
  ERESETREASON_DEEP_SLEEP_WUT     = 1,   /**< Reset triggered by wake up by timer from deep sleep state  */
//Legacy: ZW_WAKEUP_SENSOR = 2,   /**< Wake up by a Wakeup Beam */
  ERESETREASON_WATCHDOG    = 3,   /**< Reset triggered by watchdog */
  ERESETREASON_DEEP_SLEEP_EXT_INT = 4,   /**< Reset triggered by external interrupt event in deep sleep state */
  ERESETREASON_POWER_ON    = 5,   /**< Reset triggered by power on */
//Legacy: ZW_WAKEUP_USB_SUSPEND= 6  /**< Wake up by USB suspend */
  ERESETREASON_SOFTWARE    = 7,   /**< Reset triggered by software */
//Legacy: ZW_WAKEUP_EMRG_WD_RESET= 8,  /**< Wake up by emergency watchdog reset */
  ERESETREASON_BROWNOUT    = 9,   /**< Reset triggered by brownout circuit */
  ERESETREASON_OTHER       = 0xFF /**< Reset triggered by none of the above */
} EResetReason_t;

///////////////////////////////////////////////////////////////////////////
//  Legacy ZW_WAKEUP kept for reference.
///////////////////////////////////////////////////////////////////////////
//  typedef enum _ZW_WAKEUP{
//      /** Wake up after Hardware Reset (RESET_N pin activation) */
//    ZW_WAKEUP_RESET = 0,
//      /* Wake up by the WUT timer */
//    ZW_WAKEUP_WUT = 1,
//      /* Wake up by a Wakeup Beam */
//    ZW_WAKEUP_SENSOR = 2,
//      /* Reset because of a watchdog timeout */
//    ZW_WAKEUP_WATCHDOG = 3,
//      /* Wake up by External input x */
//    ZW_WAKEUP_EXT_INT = 4,
//      /* Reset by Power On Reset circuit */
//    ZW_WAKEUP_POR =5,
//      /* Wake up by USB suspend */
//    ZW_WAKEUP_USB_SUSPEND = 6,
//      /* Wake up by SW reset */
//    ZW_WAKEUP_SW_RESET = 7,
//      /* Wake up by emergency watchdog reset */
//    ZW_WAKEUP_EMRG_WD_RESET = 8,
//      /* Unknown Wakeup reason */
//    ZW_WAKEUP_UNKNOWN = 0xFF
//  } ZW_WAKEUP;
////////////////////////////////////////////////////////////////////////


/*============================================================================
**    Defines for ApplicationNodeInformation field deviceOptionMask
**--------------------------------------------------------------------------*/
#define APPLICATION_NODEINFO_NOT_LISTENING            0x00
#define APPLICATION_NODEINFO_LISTENING                0x01
#define APPLICATION_FREQ_LISTENING_MODE_1000ms        0x10
#define APPLICATION_FREQ_LISTENING_MODE_250ms         0x20

/**
 *
 */
typedef enum _E_SYSTEM_TYPE_
{
  /**
   *
   */
  E_SYSTEM_TYPE_NON_LISTENING = APPLICATION_NODEINFO_NOT_LISTENING,     /* 0x00 */
  /**
   *
   */
  E_SYSTEM_TYPE_LISTENING = APPLICATION_NODEINFO_LISTENING,     /* 0x01 */
  /**
   *
   */
  E_SYSTEM_TYPE_FLIRS = 0x02,                                       /* 0x02 */
} E_SYSTEM_TYPE;

#define SYSTEM_TYPE_LISTENING_MASK          E_SYSTEM_TYPE_LISTENING
#define SYSTEM_TYPE_FLIRS_MASK              E_SYSTEM_TYPE_FLIRS


/*==============================   ApplicationInit   ======================*/
/**
 * \ingroup COMMON
 * \ref ApplicationInit is used to initialize the application.
 * The Z Wave hardware initialization function set all application IO pins to input mode.
 * The \ref ApplicationInit function MUST be called by the Z Wave main function during system startup.
 * At this point of time the Z Wave timer system is not started so waiting on hardware to get ready SHOULD be done by MCU busy loops.
 *
 * \return \ref APPLICATION_RUNNING Application initialized, startup can continue normally
 * \return \ref APPLICATION_TEST Application requested test mode to be enabled
 * \return \ref APPLICATION_POWER_DOWN Application requested power down of the system.
 * \param[in] eResetReason
 *
 * Serial API (Not supported)
 *
 */
extern  ZW_APPLICATION_STATUS
ApplicationInit(
  EResetReason_t eResetReason
);

/*======================   sl_zwave_protocol_startup   ====================*/
/**
 * \ingroup COMMON
 * Initialize the Z-Wave protocol stack.
 * 
 */
void sl_zwave_protocol_startup(void);

/*======================   sl_zwave_platform_startup   ====================*/
/**
 * \ingroup COMMON
 * Initialize the Z-Wave protocol stack.
 * 
 */
void sl_zwave_platform_startup(void);

/**
 * Gets Z-Wave Protocol Build number
 * @return Z-Wave Protocol Build number
 */
uint16_t ZW_GetProtocolBuildNumber();

#endif /* _ZW_BASIS_API_H_ */

