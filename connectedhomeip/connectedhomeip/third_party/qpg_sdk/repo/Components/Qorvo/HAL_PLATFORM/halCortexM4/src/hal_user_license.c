/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Default logging handlers for ARM-based devices.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "hal_user_license.h"

/*****************************************************************************
 *                    Default licenses
 *****************************************************************************/


/* Minimal user license */
#if defined(GP_DIVERSITY_CUSTOM_LOADED_USER_LICENSE)
#if !defined(GP_DIVERSITY_LOADED_USER_LICENSE)
#error error
#endif
#endif


/***********************/
/* Native User License */
/***********************/



#if defined(GP_DIVERSITY_CUSTOM_NATIVE_USER_LICENSE)
/* Application will declare it's own custom native user license */
extern const userlicense_t FLASH_PROGMEM native_user_license LINKER_SECTION(".native_user_license");
#elif !defined(GP_DIVERSITY_LOADED_USER_LICENSE)
/* default minimal definition of native user license */
const userlicense_t FLASH_PROGMEM native_user_license LINKER_SECTION(".native_user_license") =
{
      .vpp = (UInt32)&_native_user_license_vpp,
      .programLoadedMagicWord  = (UInt32) USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD,
#if defined(GP_COMP_VERSION)
      .swVersion = {{GP_VERSIONINFO_GLOBAL_VERSION},0x00,0,0,((0x00FFFFFFUL & GP_CHANGELIST)<<8)}
#endif

};

#endif



/***********************/
/* Loaded User License */
/***********************/
#if defined(GP_DIVERSITY_LOADED_USER_LICENSE) || defined(GP_DIVERSITY_CUSTOM_LOADED_USER_LICENSE)
#if !defined(GP_DIVERSITY_CUSTOM_LOADED_USER_LICENSE)
/* default minimal definition of loaded license */
const loaded_userlicense_t FLASH_PROGMEM loaded_user_license LINKER_SECTION(".loaded_user_license") =
{
      .vpp = (UInt32)&_loaded_user_license_vpp,
      .programLoadedMagicWord  = (UInt32) USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD,

#if defined(GP_COMP_VERSION)
      .swVersion = {{GP_VERSIONINFO_GLOBAL_VERSION},0x00,0,0,((0x00FFFFFFUL & GP_CHANGELIST)<<8)}
#endif
};
#else
/* Application will declare it's own custom loaded user license */
extern const loaded_userlicense_t FLASH_PROGMEM loaded_user_license LINKER_SECTION(".loaded_user_license");
#endif
#endif

/*************************/
/* Extended User License */
/*************************/
#if defined(GP_DIVERSITY_EXTENDED_USER_LICENSE) || defined(GP_DIVERSITY_CUSTOM_EXTENDED_USER_LICENSE)
#if !defined(GP_DIVERSITY_CUSTOM_EXTENDED_USER_LICENSE)
/* default definition of extended user license */
const extended_userlicense_t FLASH_PROGMEM extended_user_license LINKER_SECTION(".extended_user_license") =
{
    .QorvoExtendedLoadedUserLicenseMW = (UInt32) USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD
};
#else
/* Application will declare it's own custom extended user license */
extern const extended_userlicense_t FLASH_PROGMEM extended_user_license LINKER_SECTION(".extended_user_license");
#endif
#endif

/*************************/
/* Extended Native User License */
/*************************/
#if defined(GP_DIVERSITY_EXTENDED_NATIVE_USER_LICENSE)
#if !defined(GP_DIVERSITY_CUSTOM_EXTENDED_NATIVE_USER_LICENSE)
const extended_nativeuserlicense_t FLASH_PROGMEM extended_nativeuser_license LINKER_SECTION(".extended_native_user_license") =
{
    .QorvoExtendedNativeUserLicenseMW = (UInt32) NATIVE_USER_LICENSE_EXTENDED_MW,
};
#else
/* Application will declare it's own custom extended native user license */
extern const extended_nativeuserlicense_t FLASH_PROGMEM extended_nativeuser_license LINKER_SECTION(".extended_native_user_license");
#endif
#endif

/*****************************************************************************
 *                    Functions
 *****************************************************************************/
#if defined(GP_DIVERSITY_LOADED_USER_LICENSE)
const loaded_userlicense_t *hal_get_loaded_user_license(void)
{
    return &loaded_user_license;
}
#else
const userlicense_t *hal_get_user_license(void)
{
    return &native_user_license;
}
#endif

