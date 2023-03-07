/**
 * @file
 * Contains types and defines needed for security.
 * @copyright 2018 Silicon Laboratories Inc.
 */
#ifndef ZW_SECURITY_API_H_
#define ZW_SECURITY_API_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_typedefs.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
/* The security key a frame was received with or should be sent with.
 *
 * Special values:
*/
typedef enum SECURITY_KEY
{
  SECURITY_KEY_NONE = 0x00,              //!< SECURITY_KEY_NONE
  SECURITY_KEY_S2_UNAUTHENTICATED = 0x01,//!< SECURITY_KEY_S2_UNAUTHENTICATED
  SECURITY_KEY_S2_AUTHENTICATED = 0x02,  //!< SECURITY_KEY_S2_AUTHENTICATED
  SECURITY_KEY_S2_ACCESS = 0x03,         //!< SECURITY_KEY_S2_ACCESS
  SECURITY_KEY_S0 = 0x04,                //!< SECURITY_KEY_S0
} security_key_t;


/**
 * Bitmask for security keys.
 */
#define SECURITY_KEY_S2_UNAUTHENTICATED_BIT 0x01
#define SECURITY_KEY_S2_AUTHENTICATED_BIT 0x02
#define SECURITY_KEY_S2_ACCESS_BIT 0x04
#define SECURITY_KEY_S0_BIT 0x80

#define SECURITY_KEY_S2_MASK (SECURITY_KEY_S2_UNAUTHENTICATED_BIT \
                              | SECURITY_KEY_S2_AUTHENTICATED_BIT \
                              | SECURITY_KEY_S2_ACCESS_BIT)
#define SECURITY_KEY_NONE_MASK 0x00


/**
 * Security S2 Public DSK Key length
 */
#define SECURITY_KEY_S2_PUBLIC_DSK_LENGTH     16


/**
 * Security S2 Public CSA DSK Key length
 */
#define SECURITY_KEY_S2_PUBLIC_CSA_DSK_LENGTH 4

typedef struct _S_SECURITY_S2_INCLUSION_CSA_PUBLIC_DSK_
{
  uint8_t aCSA_DSK[SECURITY_KEY_S2_PUBLIC_CSA_DSK_LENGTH];
} s_SecurityS2InclusionCSAPublicDSK_t;


/**
 * Definitions for Application bound Security events
 * Delivered from protocol to Application through the 
 * SReceiveSecurityEvent notification being passed to application.
  */
typedef enum _E_APPLICATION_SECURITY_EVENT_
{
  E_APPLICATION_SECURITY_EVENT_S2_INCLUSION_REQUEST_DSK_CSA
} e_application_security_event_t;


/**
 *
 *
 */
typedef struct _S_APPLICATION_SECURITY_EVENT_DATA_
{
  e_application_security_event_t event;
  uint8_t eventDataLength;
  uint8_t *eventData;
} s_application_security_event_data_t;


#endif /* ZW_SECURITY_API_H_ */
