/*
 * Copyright (c) 2017, Qorvo Inc
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
 */

/** @file "gpUpgrade.h"
 *
 *  Upgrade functionality
 *
 *  Declarations of the public functions and enumerations of gpUpgrade.
*/

#ifndef _GPUPGRADE_H_
#define _GPUPGRADE_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpUpgrade_Status_t */
//@{
#define gpUpgrade_StatusSuccess                                0x00
#define gpUpgrade_StatusFailedChecksumError                    0x01
#define gpUpgrade_StatusFailedVerify                           0x02
#define gpUpgrade_StatusFailedVersionError                     0x03
#define gpUpgrade_StatusFailedProgramError                     0x04
#define gpUpgrade_StatusFailedRescueInstalled                  0x05
#define gpUpgrade_StatusWriteError                             0x06
#define gpUpgrade_StatusInvalidAddress                         0x07
#define gpUpgrade_StatusOutOfRange                             0x08
#define gpUpgrade_StatusStoreImageFailed                       0x09
#define gpUpgrade_StatusLoadImageFailed                        0x0A
#define gpUpgrade_StatusParseFailed                            0x0B
#define gpUpgrade_StatusInvalidImage                           0x0C
#define gpUpgrade_StatusPreCheckFailed                         0x0D
#define gpUpgrade_StatusInvalidArgument                        0x0E
/** @typedef gpUpgrade_Status_t
    @brief General return status for all functions of this API

    @li @c gpUpgrade_StatusSuccess               The requested operation was completed successfully.
    @li @c gpUpgrade_StatusFailedChecksumError   The requested operation failed due to checksum error.
    @li @c gpUpgrade_StatusFailedVerify          The image verification failed.
    @li @c gpUpgrade_StatusFailedVersionError    Version is not newer then currently running.
    @li @c gpUpgrade_StatusFailedProgramError    The requested operation failed due to program error.
    @li @c gpUpgrade_StatusFailedRescueInstalled Failed rescue image installment.
    @li @c gpUpgrade_StatusWriteError            Failed to write to ota area.
    @li @c gpUpgrade_StatusInvalidAddress        Unexpected Address.
    @li @c gpUpgrade_StatusOutOfRange            Trying to access an address that is outside the range.
    @li @c gpUpgrade_StatusStoreImageFailed      Failure in storing the image.
    @li @c gpUpgrade_StatusLoadImageFailed       Failure in loading the image.
    @li @c gpUpgrade_StatusParseFailed           Failure in parsing the image.
    @li @c gpUpgrade_StatusInvalidImage          Invalid image.
    @li @c gpUpgrade_StatusPreCheckFaild         Upgrade image pre-wipe check failed.
    @li @c gpUpgrade_StatusInvalidArgument       An argument that was supplied is invalid.
*/
typedef UInt8                             gpUpgrade_Status_t;
//@}

#if !defined(GP_DIVERSITY_APP_LICENSE_BASED_BOOT)
/** @enum gpUpgrade_ImageId_t */
//@{
#define gpUpgrade_ImageIdRescueImage                           0x00
#define gpUpgrade_ImageIdPendingImage                          0x01
#define gpUpgrade_ImageIdActiveImage                           0x02
/** @typedef gpUpgrade_ImageId_t
    @brief Different image types

    @li @c gpUpgrade_ImageIdRescueImage  Golden image to recover (if available).
    @li @c gpUpgrade_ImageIdPendingImage Image pending for update.
    @li @c gpUpgrade_ImageIdActiveImage  Currently running image.
*/
typedef UInt32                            gpUpgrade_ImageId_t;
//@}
#endif

#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)
/** @enum gpUpgrade_SecureBoot_Status_t */
//@{
#define gpUpgrade_SecureBoot_StatusSuccess                     0x00
#define gpUpgrade_SecureBoot_StatusNoValidImage                0x01
#define gpUpgrade_SecureBoot_StatusFlashSwapStatusCorrupt      0x02
#define gpUpgrade_SecureBoot_StatusMaxCopyAttemptsExceeded     0x03
/** @typedef gpUpgrade_SecureBoot_Status_t
    @brief Secure Boot status codes

    @li @c gpUpgrade_SecureBoot_StatusSuccess                 Success.
    @li @c gpUpgrade_SecureBoot_StatusNoValidImage            No valid image found.
    @li @c gpUpgrade_SecureBoot_StatusFlashSwapStatusCorrupt  Flash swap status in flash is corrupt (not 0 or 1).
    @li @c gpUpgrade_SecureBoot_StatusMaxCopyAttemptsExceeded Maximum number of attempts to copy image from external to
                                                              internal flash exceeded.
*/
typedef UInt8                            gpUpgrade_SecureBoot_Status_t;
//@}
#endif

#if defined(GP_COMP_EXTSTORAGE)
/** @enum gpUpgrade_FlashLoadSource_t */
//@{
#define gpUpgrade_FlashLoadSourceExternal                      0x00
#define gpUpgrade_FlashLoadSourceInternal                      0x01
/** @typedef gpUpgrade_FlashLoadSource_t
    @brief gpUpgrade source to load flash content

    @li @c gpUpgrade_FlashLoadSourceExternal                  Load from external flash
    @li @c gpUpgrade_FlashLoadSourceInternal                  Load from internal flash
*/
typedef UInt8                            gpUpgrade_FlashLoadSource_t;
//@}
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_UPGRADE_MAGIC_WORD */
/** @brief Magic word set on reset to bootloader. */
#define GP_UPGRADE_MAGIC_WORD                        0x524F4752
/** @macro GP_UPGRADE_FLAG_PENDING */
#define GP_UPGRADE_FLAG_PENDING                      0x00000001
/** @macro GP_UPGRADE_FLAG_UPGRADE_HANDLED */
#define GP_UPGRADE_FLAG_UPGRADE_HANDLED              0x00000002
/** @macro GP_UPGRADE_FLAG_JUMPTABLE_UPGRADE */
#define GP_UPGRADE_FLAG_JUMPTABLE_UPGRADE            0x00000004
#if defined(GP_DIVERSITY_GPHAL_K8E)
//Check linkerscript!
#if defined(GP_DIVERSITY_GPHAL_K8E)
/** @brief  Jumptables is always at offset 0x800 of the flash half (512KB) */
#if !defined(GP_HAL_EXPECTED_CHIP_EMULATED)
#define GP_UPGRADE_APP_JUMP_TABLE_ADDR(_addr)        (((_addr) & 0xfff80000) | 0x800)
#else
#define GP_UPGRADE_APP_JUMP_TABLE_ADDR(_addr)        (((_addr) & 0xfffc0000) | 0x800)
#endif
#else
/** @brief  Jumptables is always at offset 0x800 of the flash half (256KB) */
#define GP_UPGRADE_APP_JUMP_TABLE_ADDR(_addr)        (((_addr) & 0xfffc0000) | 0x800)
#endif
/** @macro GP_UPGRADE_APP_JUMP_TABLE_SIZE */
/** @brief  0x1000 is jumptable size (also defined in linkerscript)
                  MAKE SURE THIS DEFINE MATCHES THE LINKERSCRIPT OF THE APPLICATION
             */
#define GP_UPGRADE_APP_JUMP_TABLE_SIZE               0x1000
#endif //defined(GP_DIVERSITY_GPHAL_K8C) || defined(GP_DIVERSITY_GPHAL_K8D) || defined(GP_DIVERSITY_GPHAL_K8E)

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#ifndef GP_DIVERSITY_BOOTLOADER_BUILD
/** @pointer to function gpUpgrade_cbEraseComplete_t
 *  @brief Application API: Pointer to callback for erase complete.
*/
typedef void (*gpUpgrade_cbEraseComplete_t) (void);
#endif

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
#ifndef GP_DIVERSITY_BOOTLOADER_BUILD

/** @brief Application API: initialise the OTA area and notify the application if an upgrade was handled.
*
*   Should be invoked by the application during its initialisation.
*/
void gpUpgrade_Init(void);


/** @brief Application API: erase the OTA area (blocking).
*/
void gpUpgrade_EraseOtaArea(void);

/** @brief Application API: start non-blocking erase of the OTA area.
*
*   @param cb                        Pointer to erase complete callback
*/
void gpUpgrade_StartEraseOtaArea(gpUpgrade_cbEraseComplete_t cb);

/** @brief Application API: get total size of the OTA upgrade area.
*
*   @return size Number of bytes in the OTA upgrade area.
*/
UInt32 gpUpgrade_GetOtaAreaSize(void);

/** @brief Application API: get start address of the OTA upgrade area.
*
*   @return address
*/
UInt32 gpUpgrade_GetOtaAreaStartAddress(void);

/** @brief Application API: get start address of the Jumptable OTA upgrade area.
*
*   @return address
*/
UInt32 gpUpgrade_GetJumptableOtaAreaStartAddress(void);

/** @brief Application API: get total size of the Jumptable OTA upgrade area.
*
*   @return size Number of bytes in the JT OTA upgrade area.
*/
UInt32 gpUpgrade_GetJumptableOtaAreaSize(void);

/** @brief Application API: initialize CRC so a writing operation can start.
*/
void gpUpgrade_StartWrite(void);

/** @brief Application API: write chunk of data to external or internal storage device and update CRC.
*
*   @param address                   Address location of where to write the data.
*   @param length                    Number of bytes to write.
*   @param dataChunk                 Pointer to the data to write.
*   @return result
*/
gpUpgrade_Status_t gpUpgrade_WriteChunk(UInt32 address, UInt16 length, UInt8* dataChunk);

/** @brief Application API: read chunk of data from external or internal storage device.
*
*   @param address                   Address location of data to read.
*   @param length                    Number of bytes to read.
*   @param dataChunk                 Pointer to the buffer to store read data.
*   @return result
*/
gpUpgrade_Status_t gpUpgrade_ReadChunk(UInt32 address, UInt16 length, UInt8* dataChunk);

/** @brief Application API: get current value of CRC.
*
*   @return crcValue
*/
UInt32 gpUpgrade_GetCrc(void);

/** @brief Application API: set the value of CRC.
*
*   @param crcValue
*/
void gpUpgrade_SetCrc(UInt32 crcValue);

/** @brief Application API: set the new pending image information for the User Mode Bootloader.
*
*   This writes the properties of the new update image to the software upgrade table in flash. The User Mode Bootloader
*   inspects this information to determine whether to carry out an update, and if so where to find the update image.
*
*   @param swVer                     Software version of the new image.
*   @param hwVer                     Hardware version of the new image.
*   @param startAddr                 Image base address.
*   @param imgSz                     Image size (in bytes).
*   @return status                   gpUpgrade_StatusSuccess if the pending image set is successful,
*                                    gpUpgrade_StatusFailedVersionError if the pending image set failed
*                                    (version incompatibility)
*/
gpUpgrade_Status_t gpUpgrade_SetPendingImage(UInt32 swVer, UInt32 hwVer, UInt32 startAddr, UInt32 imgSz);

/** @brief Application API: trigger a hardware reset that will jump to the bootloader (to execute update).
*/
void gpUpgrade_Reset(void);

#else //GP_DIVERSITY_BOOTLOADER_BUILD

#ifndef GP_DIVERSITY_APP_LICENSE_BASED_BOOT

/** @brief Bootloader API (not license based boot): install a pending image.
*
*   @return status
*/
gpUpgrade_Status_t gpUpgrade_InstallPendingImage(void);

/** @brief Bootloader API (not license based boot): *NOT IMPLEMENTED YET*, install a rescue image (if available).
*
*   @return status
*/
gpUpgrade_Status_t gpUpgrade_InstallRescueImage(void);

/** @brief Bootloader API (not license based boot): return whether an image is pending for installation.
*
*   @return pending                  true if a new image is pending, false otherwise.
*/
Bool gpUpgrade_IsImagePending(void);

/** @brief Bootloader API (not license based boot): set the status of the upgrade, reported to user application.
*
*   @param status                    Status of upgrade to set for the application.
*/
void gpUpgrade_SetUpgradeHandled(gpUpgrade_Status_t status);

/** @brief Bootloader API (not license based boot): clear the pending image flag.
*/
void gpUpgrade_ClrPendingImage(void);

/** @brief Bootloader API (not license based boot): mark the pending image as active image.
*/
void gpUpgrade_SetPendingToActive(void);


#else /* GP_DIVERSITY_APP_LICENSE_BASED_BOOT */
#if defined(GP_APP_DIVERSITY_SECURE_BOOTLOADER)

/** @brief Bootloader API (license based boot, secure boot): lock out write access to boot loader flash region and
*          Native User License.
*/
void gpUpgrade_SecureBoot_LockBootloader(void);

/** @brief Bootloader API (license based boot, secure boot): callback invoked when the bootloader encounters a fatal
*          error.
*
*   @param status                   Secure Boot failure status.
*/
void gpUpgrade_SecureBoot_cbFatalError(gpUpgrade_SecureBoot_Status_t status);

/** @brief Bootloader API (license based boot, flash remapping, secure boot): select freshest application image and swap
*          flash areas to activate it.
*/
void gpUpgrade_SecureBoot_selectActiveApplication(void);


#else
/** @brief Bootloader API (license based boot, no secure boot): select freshest application image and swap flash areas
*                     if needed.
*/
void gpUpgrade_selectActiveApplication(void);
#endif
#endif /* GP_DIVERSITY_APP_LICENSE_BASED_BOOT */

#endif //GP_DIVERSITY_BOOTLOADER_BUILD

//Indications

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPUPGRADE_H_
