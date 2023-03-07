/***************************************************************************/ /**
 * @file
 * @brief Implementation of the Cipher-based Message Authentication Code (CMAC)
 * specified in the IETF memo "The AES-CMAC Algorithm".
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/**
 * @defgroup aes-cmac AES-CMAC
 * @ingroup component
 * @brief API and Callbacks for the AES-CMAC Component
 *
 * Silicon Labs implementation of AES-CMAC. See AN875 for more information.
 *
 */

/**
 * @addtogroup aes-cmac
 * @{
 */

// Returns the most significant bit of a bit string.
#define MSB(x) (x[0] >> 7)

#define initToConstZero(x) \
  do                       \
  {                        \
    MEMSET(x, 0x00, 16);   \
  } while (0)

#define initToConstRb(x) \
  do                     \
  {                      \
    MEMSET(x, 0x00, 15); \
    x[15] = 0x87;        \
  } while (0)

/**
 * @name API
 * @{
 */

/** @brief Compute the Cipher-based Message Authentication Code (CMAC)
 * corresponding to the passed message and the passed key.
 *
 * @param[in] key           The key to be used to produce the cipher-based
 * authentication code.
 *
 * @param[in] message       A pointer to the message for which a cipher-based
 * authentication code should be computed.
 *
 * @param[in] messageLength The length in bytes of the message to be authenticated.
 *
 * @param[out] out          A pointer to a 16-byte area memory in which the resulting
 * cipher-based authentication code will be stored.
 */
void emberAfPluginAesMacAuthenticate(const uint8_t *key,
                                     const uint8_t *message,
                                     uint8_t messageLength,
                                     uint8_t *out);

/** @} */ // end of name APIs
/** @} */ // end of group aes-cmac
