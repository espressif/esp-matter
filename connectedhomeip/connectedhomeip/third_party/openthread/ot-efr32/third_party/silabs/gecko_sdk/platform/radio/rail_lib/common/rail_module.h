/***************************************************************************//**
 * @file
 * @brief Header file for module functions. This should not be released.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef _RAIL_MODULE_H_
#define _RAIL_MODULE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Call to unlock RAIL on modules by providing RAIL_MODULE_UNLOCK_KEY or
 * RAIL_MODULE_UNLOCK_KEY_MGM240L for MGM240L022VNF and MGM240L022RNF module.
 * This is a no-op when not using RAIL on a module.
 *
 * @param[in] secretKey The key provided to unlock RAIL for modules.
 */
void RAIL_UnlockModule(uint32_t unlockKey);

/** "Secret" Key used to unlock RAIL for use on modules. Chosen randomly */
#define RAIL_MODULE_UNLOCK_KEY 0xF312C717

/** "Secret" key used to unlock RAIL for use on MGM240L022VNF and MGM240L022RNF module. Chosen randomly. */
#define RAIL_MODULE_UNLOCK_KEY_MGM240L 0xA391B647

/** For Backwards compatibility keep the previous naming of secret key used for MGM240L022VNF */
#define RAIL_MODULE_UNLOCK_KEY_MGM240L022VNF RAIL_MODULE_UNLOCK_KEY_MGM240L

#ifdef __cplusplus
}
#endif

#endif // __RAIL_MODULE_H_
