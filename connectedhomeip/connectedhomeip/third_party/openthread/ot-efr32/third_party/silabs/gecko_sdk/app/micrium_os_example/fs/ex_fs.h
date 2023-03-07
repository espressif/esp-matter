/***************************************************************************//**
 * @file
 * @brief File System Example
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _EX_FS_H_
#define  _EX_FS_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) You may define EX_CFG_FS_ACTIVE_MEDIA_NAME before the '#ifndef' to overwrite the default
 *               value determined according to the media AVAIL defined in rtos_description.h. This
 *               #define specifies the media name you want to use. For instance:
 *
 *               #define     EX_CFG_FS_ACTIVE_MEDIA_NAME     "nand0"
 *
 *               The media name must have the following format: "<media-name><number>" such as:
 *
 *               "nand0" for NAND
 *               "nor0"  for NOR
 *               "ram0"  for RAM Disk
 *               "sd0"   for SD Card or SPI
 *
 *               The media name could be any name in fact. But the FS examples requires that the media
 *               name defined here matches the one defined in the BSP when the media is registered. Most
 *               of the time the media name used in the BSP will be "<media-name>0".
 *
 *               (a) For SCSI devices, you do not need to specify for instance "scsi0". SCSI devices
 *                   are used only by the Media Polling example and do not require a media name defined
 *                   here. The media name for SCSI device is automatically managed by the Media Polling
 *                   example.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------- LOCAL CONFIGURATION ----------------
//                                                                 TODO See Note #1.
//                                                                 Default media according to media AVAIL.
#ifndef  EX_CFG_FS_ACTIVE_MEDIA_NAME
#if  (defined(RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL) && defined(EX_FS_RAMDISK_INIT_AVAIL))
#define  EX_CFG_FS_ACTIVE_MEDIA_NAME            "ram0"
#elif (defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL))
#define  EX_CFG_FS_ACTIVE_MEDIA_NAME            "nand0"
#elif (defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL))
#define  EX_CFG_FS_ACTIVE_MEDIA_NAME            "nor0"
#elif (defined(RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL) || defined(RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL))
#define  EX_CFG_FS_ACTIVE_MEDIA_NAME            "sd0"
#else
#define  EX_CFG_FS_ACTIVE_MEDIA_NAME            "None"
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void Ex_FS_Init(void);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                        CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
