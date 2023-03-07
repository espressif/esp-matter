/**
 * @file
 * @brief Handler for Command Class User Code.
 *
 * @details The purpose of the User Code Command Class is to supply a enabled
 * Door Lock Device with a command class to manage user codes.
 *
 * User Identifier (8 bits).
 * -------------------------
 * The User Identifier used to recognise the user identity. The User Identifier
 * values MUST be a sequence starting from 1. This field can be ignored in case
 * the node only supports one User Code. Setting the User Identifier to 0 will
 * address all User Identifiers available in the device.
 *
 * USER_CODE1, USER_CODEn.
 * -----------------------
 * These fields contain the user code. Minimum code length is 4 and maximum 10
 * ASCII digits. The number of data fields transmitted can be determined from
 * the length field returned by the ApplicationCommandHandler. The user code
 * fields MUST be initialize to 0x00 0x00 0x00 0x00 (4 bytes) when User ID
 * Status is equal to 0x00.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _COMMANDCLASSUSERCODE_H_
#define _COMMANDCLASSUSERCODE_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <CC_Common.h>
#include <agi.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * User ID Status.
 * ---------------
 * The User ID Status field indicates the state of the User Identifier. All
 * other values not mentioned in below list are reserved for future
 * implementation.
 * Hex | Description
 * ----|---------------------------
 *  00 | Available (not set)
 *  01 | Occupied
 *  02 | Reserved by administrator
 *  FE | Status not available
 */
typedef enum
{
  USER_ID_AVAILBLE = 0x00, /**< Available (not set)*/
  USER_ID_OCCUPIED = 0x01, /**< Occupied*/
  USER_ID_RESERVED = 0x02, /**< Reserved by administrator*/
  USER_ID_NO_STATUS = 0xFE /**<	Status not available*/
}
user_id_status_t;

typedef user_id_status_t USER_ID_STATUS;

/**
 * Minimum length of a user code as defined in SDS12652.
 */
#define USERCODE_MIN_LEN 4

/**
 * Maximum length of a user code as defined in SDS12652.
 */
#define USERCODE_MAX_LEN 10

// Used by application file system.
typedef struct SUserCode
{
  USER_ID_STATUS user_id_status;
  uint8_t userCode[USERCODE_MAX_LEN];
  uint8_t userCodeLen;
} SUserCode;

#define ZAF_FILE_SIZE_USERCODE            (sizeof(SUserCode) * USER_ID_MAX)


/**
 * Struct used to pass operational data to TSE module
 */
typedef struct s_CC_userCode_data_t_
{
  RECEIVE_OPTIONS_TYPE_EX rxOptions; /**< rxOptions */
  uint8_t userIdentifier;
} s_CC_userCode_data_t;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief The User Code Set Command used to set a User Code in the device.
 * @param[in] identifier User Identifier.
 * @param[in] id user Id status.
 * @param[in] pUserCode pointer to UserCode data.
 * @param[in] len UserCode data.
 * @param[in] endpoint is the destination endpoint
 * @return command handler return code
 */
extern e_cmd_handler_return_code_t
CC_UserCode_Set_handler(
  uint8_t identifier,
  USER_ID_STATUS id,
  uint8_t* pUserCode,
  uint8_t len,
  uint8_t endpoint );

/**
 * @brief The User Code Get ID.
 * @param[in] identifier User Identifier.
 * @param[out] pId pointer to return Id.
 * @param[in] endpoint is the destination endpoint
 * @return status valid boolean.
 */
extern bool CC_UserCode_getId_handler(
  uint8_t identifier,
  USER_ID_STATUS* pId,
  uint8_t endpoint );


/**
 * @brief The User Code Report Command can be used by e.g. a door lock device to send a
 * report either unsolicited or requested by the User Code Get Command.
 * @param[in] identifier User Identifier.
 * @param[out] pUserCode pointer to UserCode data.
 * @param[out] pLen length UserCode data.
 * @param[in] endpoint is the destination endpoint
 * @return status valid boolean.
 */
extern bool CC_UserCode_Report_handler(
  uint8_t identifier,
  uint8_t* pUserCode,
  size_t  *pLen,
  uint8_t endpoint );


/**
 * @brief The Users Number Report Command used to report the maximum number of USER CODES
 * the given node supports. The Users Number Report Command can be send requested
 * by the Users Number Get Command.
 * @param[in] endpoint is the destination endpoint
 * @return maximum number of USER CODES.
 */
extern uint8_t CC_UserCode_UsersNumberReport_handler( uint8_t endpoint );

/**
 * @brief Send a Command Class User code support report
 * @param[in] pProfile pointer to AGI profile
 * @param[in] sourceEndpoint source endpoint
 * @param[in] userIdentifier user identifier
 * @param[in] userIdStatus user Id status
 * @param[in] pUserCode user code
 * @param[in] userCodeLen length of user code
 * @param[out] pCallback callback function returning status destination node receive job.
 * @return status on the job.
 */
JOB_STATUS
CC_UserCode_SupportReport(
  AGI_PROFILE* pProfile,
  uint8_t sourceEndpoint,
  uint8_t userIdentifier,
  uint8_t userIdStatus,
  uint8_t* pUserCode,
  uint8_t userCodeLen,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult));

/**
 * Send report when change happen via lifeLine.
 *
 * Callback used by TSE module. Refer to @ref ZAF_TSE.h for more details.
 *
 * @param txOptions txOptions
 * @param pData Command payload for the report
 */
void CC_UserCode_report_stx(TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions, void* pData);

#endif /* _COMMANDCLASSUSERCODE_H_ */

