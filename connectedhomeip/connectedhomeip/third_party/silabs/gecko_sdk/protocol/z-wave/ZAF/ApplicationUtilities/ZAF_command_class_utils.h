/**
 * @file
 * @brief Validation of command class agains application NIF's
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */


#ifndef _ZAF_COMMAND_CLASS_UTILS_H_
#define _ZAF_COMMAND_CLASS_UTILS_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_security_api.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/


/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * Returns whether a given command is supported based on command class lists and different rules.
 *
 * @param[in] eKey The security key that the frame was received with.
 * @param[in] commandClass The CC of the frame.
 * @param[in] command The command of the frame.
 * @param[in] pSecurelist Pointer to the secure CC list.
 * @param[in] securelistLen The length of the secure CC list.
 * @param[in] pNonSecurelist Pointer to the non-secure CC list.
 * @param[in] nonSecurelistLen Length of the non-secure CC list.
 * @return Returns true if the command is supported and false otherwise.
 */
bool
CmdClassSupported(security_key_t eKey,
                  uint8_t commandClass,
                  uint8_t command,
                  uint8_t* pSecurelist,
                  uint8_t securelistLen,
                  uint8_t* pNonSecurelist,
                  uint8_t nonSecurelistLen);

#endif /* _ZAF_COMMAND_CLASS_UTILS_H_ */
