/******************************************************************************

 @file  rom_flash_jt.h

 @brief This file contains a define for every ROM-to-ROM or ROM-to-Flash
        function call that is located in the flash Jump Table. The flash
        Jump Table can be used to patch functions from ROM to flash. Data
        can be mapped this way as well.

        Table Demarcation
        // <START TABLE - DO NOT REMOVE!>
        // <END TABLE - DO NOT REMOVE!>

        Meta-Commands
        Used to insert text to output file rom_init.c (see examples below).

        // <<INSERT:[String]>>
        Used to insert a string "[String]"

        // <<BEGIN_COUNT>>
        Used to start a counter of the number of subsequent lines.

        // <<END_COUNT>>
        Used to stop counting lines.

        // <<INSERT_LOOP:[String]>>
        Used to insert "[String]" count number of times.

        // <<INSERT_DATA:[String]>>
        Used to insert "[String]" before data.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef ROM_FLASH_JT_H
#define ROM_FLASH_JT_H

#if defined( ROM_BUILD )

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include <ti/drivers/rf/RF.h>
#include "rf_api.h"
#include "osal_cbtimer.h"
#include "ll_config.h"
//
#include "rom_jt.h"

/*******************************************************************************
 * EXTERNS
 */

// Common ROM's RAM table for pointers to ICall functions and flash jump tables.
// Note: This linker imported symbol is treated as a variable by the compiler.
// 0: iCall Dispatch Function Pointer
// 1: iCall Enter Critical Section Function Pointer
// 2: iCall Leave Critical Section Function Pointer
// 3: ROM Flash Jump Table Pointer
extern uint32 RAM_BASE_ADDR[];

/*******************************************************************************
 * CONSTANTS
 */

// ROM's RAM table offset to ROM Flash Jump Table pointer.
#define ROM_RAM_TABLE_ROM_JT                            3

// Defines used for the flash jump table routines that are not part of build.
// Note: Any change to this table must accompany a change to ROM_Flash_JT[]!
#define ROM_BLE_JT_LOCATION                             (&RAM_BASE_ADDR[ROM_RAM_TABLE_ROM_JT])
#define ROM_BLE_JT_BASE                                 (*((uint32 **)ROM_BLE_JT_LOCATION))
#define ROM_BLE_JT_OFFSET(index)                        (*(ROM_BLE_JT_BASE+(index)))

/*******************************************************************************
 * ROM Jump Table
 */

//
// ROM-to-RAM Data
//
// <START TABLE - DO NOT REMOVE!>
// <<INSERT_DATA:&>>
#define hciTaskID                                       (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(0))
// <<INSERT_DATA:&>>
#define hciL2capTaskID                                  (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(1))
// <<INSERT_DATA:&>>
#define hciGapTaskID                                    (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(2))
// <<INSERT_DATA:&>>
#define hciSmpTaskID                                    (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(3))
// <<INSERT_DATA:&>>
#define hciTestTaskID                                   (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(4))
// <<INSERT_DATA:&>>
#define llConfigTable                                   (*(llCfgTable_t *)                                                                                                                             ROM_BLE_JT_OFFSET(5))
// <<INSERT_DATA:&>>
#define llTaskList                                      (*(taskList_t *)                                                                                                                               ROM_BLE_JT_OFFSET(6))
// <<INSERT_DATA:&>>
#define sizeInfo                                        (*(sizeInfo_t *)                                                                                                                               ROM_BLE_JT_OFFSET(7))
// <<INSERT_DATA:&>>
#define wlSize                                          (*(wlSize_t  *)                                                                                                                                ROM_BLE_JT_OFFSET(8))
// <<INSERT_DATA:&>>
#define rlSize                                          (*(rlSize_t *)                                                                                                                                 ROM_BLE_JT_OFFSET(9))
// <<INSERT:#ifdef HOST_CONFIG>>
// <<BEGIN_COUNT>>
// <<INSERT_DATA:&>>
#define gapAppTaskID                                    (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(10))
// <<INSERT_DATA:&>>
#define gapEndAppTaskID                                 (*(uint8 *)                                                                                                                                    ROM_BLE_JT_OFFSET(11))
#define primaryServiceUUID                              ((const uint8 *)                                                                                                                               ROM_BLE_JT_OFFSET(12))
#define secondaryServiceUUID                            ((const uint8 *)                                                                                                                               ROM_BLE_JT_OFFSET(13))
#define serviceChangedUUID                              ((const uint8 *)                                                                                                                               ROM_BLE_JT_OFFSET(14))
#define characterUUID                                   ((const uint8 *)                                                                                                                               ROM_BLE_JT_OFFSET(15))
#define includeUUID                                     ((const uint8 *)                                                                                                                               ROM_BLE_JT_OFFSET(16))
// <<END_COUNT>>
// <<INSERT:#else // !HOST_CONFIG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // HOST_CONFIG>>
//
// ROM-to-ROM or ROM-to-Flash Functions
//
#define MAP_osal_list_empty                             ((bool                      (*) (osal_list_list *))                                                                                            ROM_BLE_JT_OFFSET(17))
#define MAP_osal_list_put                               ((void                      (*) (osal_list_list *, osal_list_elem *))                                                                          ROM_BLE_JT_OFFSET(18))
#define MAP_osal_list_head                              ((osal_list_elem *          (*) (osal_list_list *))                                                                                            ROM_BLE_JT_OFFSET(19))
#define MAP_osal_list_remove                            ((void                      (*) (osal_list_list *, osal_list_elem *))                                                                          ROM_BLE_JT_OFFSET(20))
#define MAP_osal_mem_alloc                              ((void *                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(21))
#define MAP_osal_mem_free                               ((void                      (*) (void *))                                                                                                      ROM_BLE_JT_OFFSET(22))
#define MAP_osal_bm_free                                ((void                      (*) (void *))                                                                                                      ROM_BLE_JT_OFFSET(23))
#define MAP_osal_memcpy                                 ((void *                    (*) (void *, const void *, uint32))                                                                                ROM_BLE_JT_OFFSET(24))
#define MAP_osal_revmemcpy                              ((void *                    (*) (void *, const void *, unsigned int))                                                                          ROM_BLE_JT_OFFSET(25))
#define MAP_osal_memset                                 ((void *                    (*) (void *, uint8, int))                                                                                          ROM_BLE_JT_OFFSET(26))
#define MAP_osal_msg_allocate                           ((uint8 *                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(27))
#define MAP_osal_msg_deallocate                         ((uint8                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(28))
#define MAP_osal_msg_send                               ((uint8                     (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(29))
#define MAP_osal_msg_receive                            ((uint8 *                   (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(30))
#define MAP_osal_set_event                              ((uint8                     (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(31))
#define MAP_osal_bm_alloc                               ((void *                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(32))
#define MAP_osal_bm_adjust_header                       ((void *                    (*) (void *, int16))                                                                                               ROM_BLE_JT_OFFSET(33))
#define MAP_osal_buffer_uint32                          ((uint8 *                   (*) (uint8 *, uint32))                                                                                             ROM_BLE_JT_OFFSET(34))
#define MAP_osal_build_uint32                           ((uint32                    (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(35))
#define MAP_osal_isbufset                               ((uint8                     (*) (uint8 *, uint8, uint8))                                                                                       ROM_BLE_JT_OFFSET(36))
#define MAP_osal_start_timerEx                          ((uint8                     (*) (uint8, uint16, uint32))                                                                                       ROM_BLE_JT_OFFSET(37))
#define MAP_osal_stop_timerEx                           ((uint8                     (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(38))
#define MAP_osal_start_reload_timer                     ((uint8                     (*) (uint8, uint16, uint32))                                                                                       ROM_BLE_JT_OFFSET(39))
#define MAP_osal_clear_event                            ((uint8                     (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(40))
#define MAP_osal_memdup                                 ((void *                    (*) (const void *, unsigned int))                                                                                  ROM_BLE_JT_OFFSET(41))
#define MAP_osal_memcmp                                 ((uint8                     (*) (const void *, const void *, uint32))                                                                          ROM_BLE_JT_OFFSET(42))
#define MAP_osal_rand                                   ((uint16                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(43))
#define MAP_osal_CbTimerStart                           ((uint8                     (*) (pfnCbTimer_t, uint8 *, uint32, uint8 *))                                                                      ROM_BLE_JT_OFFSET(44))
#define MAP_osal_CbTimerStop                            ((uint8                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(45))
#define MAP_osal_CbTimerUpdate                          ((uint8                     (*) (uint8, uint32))                                                                                               ROM_BLE_JT_OFFSET(46))
#define MAP_osal_mem_allocLimited                       ((void *                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(47))
#define MAP_osal_get_timeoutEx                          ((uint32                    (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(48))
#define halAssertHandler                                ((void                      (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(49))
#define halAssertHandlerExt                             ((bStatus_t                 (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(50))
//
// Controller
//
// HCI Bluetooth
#define MAP_HCI_bm_alloc                                 ((void *                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(51))
#define MAP_HCI_CommandCompleteEvent                     ((void                     (*) (uint16, uint8, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(52))
#define MAP_HCI_CommandStatusEvent                       ((void                     (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(53))
#define MAP_HCI_DataBufferOverflowEvent                  ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(54))
#define MAP_HCI_DisconnectCmd                            ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(55))
#define MAP_HCI_GAPTaskRegister                          ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(56))
#define MAP_HCI_HardwareErrorEvent                       ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(57))
#define MAP_HCI_HostBufferSizeCmd                        ((uint8                    (*) (uint16, uint8, uint16, uint16))                                                                               ROM_BLE_JT_OFFSET(58))
#define MAP_HCI_HostNumCompletedPktCmd                   ((uint8                    (*) (uint8, uint16 *, uint16 *))                                                                                   ROM_BLE_JT_OFFSET(59))
#define MAP_HCI_L2CAPTaskRegister                        ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(60))
#define MAP_HCI_NumOfCompletedPacketsEvent               ((uint8                    (*) (uint8, uint16 *, uint16 *))                                                                                   ROM_BLE_JT_OFFSET(61))
#define MAP_HCI_ReadBDADDRCmd                            ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(62))
#define MAP_HCI_ResetCmd                                 ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(63))
#define MAP_HCI_ReverseBytes                             ((void                     (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(64))
#define MAP_HCI_SendCommandCompleteEvent                 ((void                     (*) (uint8, uint16, uint8, uint8 *))                                                                               ROM_BLE_JT_OFFSET(65))
#define MAP_HCI_SendCommandStatusEvent                   ((void                     (*) (uint8, uint16, uint16))                                                                                       ROM_BLE_JT_OFFSET(66))
#define MAP_HCI_SendControllerToHostEvent                ((void                     (*) (uint8, uint8, uint8 *))                                                                                       ROM_BLE_JT_OFFSET(67))
#define MAP_HCI_SendDataPkt                              ((uint8                    (*) (uint16, uint8, uint16, uint8 *))                                                                              ROM_BLE_JT_OFFSET(68))
#define MAP_HCI_SetControllerToHostFlowCtrlCmd           ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(69))
#define MAP_HCI_SMPTaskRegister                          ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(70))
#define MAP_HCI_ValidConnTimeParams                      ((uint8                    (*) (uint16, uint16, uint16, uint16))                                                                              ROM_BLE_JT_OFFSET(71))
#define MAP_HCI_VendorSpecifcCommandCompleteEvent        ((void                     (*) (uint16, uint8, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(72))
// HCI Bluetooth Internal
#define MAP_hciInitEventMasks                            ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(73))
// HCI BLE
#define MAP_HCI_LE_ConnUpdateCmd                         ((uint8                    (*) (uint16, uint16, uint16, uint16, uint16, uint16, uint16))                                                      ROM_BLE_JT_OFFSET(74))
#define MAP_HCI_LE_CreateConnCancelCmd                   ((hciStatus_t              (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(75))

#define MAP_HCI_LE_LtkReqNegReplyCmd                     ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(76))
#define MAP_HCI_LE_LtkReqReplyCmd                        ((uint8                    (*) (uint16, uint8 *))                                                                                             ROM_BLE_JT_OFFSET(77))
#define MAP_HCI_LE_ReadBufSizeCmd                        ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(78))
#define MAP_HCI_LE_RemoteConnParamReqReplyCmd            ((uint8                    (*) (uint16, uint16, uint16, uint16, uint16, uint16, uint16))                                                      ROM_BLE_JT_OFFSET(79))
#define MAP_HCI_LE_RemoteConnParamReqNegReplyCmd         ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(80))
#define MAP_HCI_LE_SetAdvDataCmd                         ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(81))
#define MAP_HCI_LE_SetAdvEnableCmd                       ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(82))
#define MAP_HCI_LE_SetAdvParamCmd                        ((uint8                    (*) (uint16, uint16, uint8, uint8, uint8, uint8  *, uint8, uint8))                                                 ROM_BLE_JT_OFFSET(83))
#define MAP_HCI_LE_SetRandomAddressCmd                   ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(84))
#define MAP_HCI_LE_SetScanRspDataCmd                     ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(85))
#define MAP_HCI_LE_GenerateDHKeyCmd                      ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(86))
#define MAP_HCI_LE_ReadLocalP256PublicKeyCmd             ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(87))
#define MAP_HCI_LE_SetAddressResolutionEnableCmd         ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(88))
#define MAP_HCI_LE_SetPrivacyModeCmd                     ((hciStatus_t              (*) (uint8,uint8 *,uint8))                                                                                         ROM_BLE_JT_OFFSET(89))
#define MAP_HCI_LE_SetResolvablePrivateAddressTimeoutCmd ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(90))
#define MAP_HCI_LE_StartEncyptCmd                       ((hciStatus_t               (*) (uint16,uint8 *,uint8 *,uint8 *))                                                                              ROM_BLE_JT_OFFSET(91))
// HCI Vendor Specific
// LE Bluetooth
#define MAP_LE_AE_SetData                               ((llStatus_t                (*) (aeSetDataCmd_t *, uint8))                                                                                     ROM_BLE_JT_OFFSET(92))
#define MAP_LE_ClearAdvSets                             ((llStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(93))
#define MAP_LE_ExtCreateConn                            ((llStatus_t                (*) (aeCreateConnCmd_t *))                                                                                         ROM_BLE_JT_OFFSET(94))
#define MAP_LE_ReadNumSupportedAdvSets                  ((uint8                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(95))
#define MAP_LE_ReadRfPathCompCmd                        ((uint8                     (*) (int16 *, int16 *))                                                                                            ROM_BLE_JT_OFFSET(96))
#define MAP_LE_ReadTxPowerCmd                           ((uint8                     (*) (int8 *, int8 *))                                                                                              ROM_BLE_JT_OFFSET(97))
#define MAP_LE_RemoveAdvSet                             ((llStatus_t                (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(98))
#define MAP_LE_SetExtAdvData                            ((llStatus_t                (*) (aeSetDataCmd_t *))                                                                                            ROM_BLE_JT_OFFSET(99))
#define MAP_LE_SetExtAdvEnable                          ((llStatus_t                (*) (aeEnableCmd_t *))                                                                                             ROM_BLE_JT_OFFSET(100))
#define MAP_LE_SetExtAdvParams                          ((llStatus_t                (*) (aeSetParamCmd_t *, aeSetParamRtn_t *))                                                                        ROM_BLE_JT_OFFSET(101))
#define MAP_LE_SetExtScanEnable                         ((llStatus_t                (*) (aeEnableScanCmd_t *))                                                                                         ROM_BLE_JT_OFFSET(102))
#define MAP_LE_SetExtScanParams                         ((llStatus_t                (*) (aeSetScanParamCmd_t *))                                                                                       ROM_BLE_JT_OFFSET(103))
#define MAP_LE_SetExtScanRspData                        ((llStatus_t                (*) (aeSetDataCmd_t *))                                                                                            ROM_BLE_JT_OFFSET(104))
#define MAP_LE_WriteRfPathCompCmd                       ((llStatus_t                (*) (int8, int8))                                                                                                  ROM_BLE_JT_OFFSET(105))
// LL Bluetooth
#define MAP_LL_AE_RegCBack                              ((llStatus_t                (*) (uint8, void *))                                                                                               ROM_BLE_JT_OFFSET(106))
#define MAP_LL_AddWhiteListDevice                        ((uint8                    (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(107))
#define MAP_LL_AuthPayloadTimeoutExpiredCback            ((void                     (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(108))
#define MAP_LL_ChanMapUpdate                             ((uint8                    (*) (uint8 * , uint16))                                                                                            ROM_BLE_JT_OFFSET(109))
#define MAP_LL_ClearWhiteList                            ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(110))
#define MAP_LL_ConnActive                                ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(111))
#define MAP_LL_ConnParamUpdateCback                      ((void                     (*) (uint8, uint16, uint16, uint16, uint16))                                                                       ROM_BLE_JT_OFFSET(112))
#define MAP_LL_ConnUpdate                                ((uint8                    (*) (uint16, uint16, uint16, uint16, uint16, uint16, uint16))                                                      ROM_BLE_JT_OFFSET(113))
#define MAP_LL_CountAdvSets                             ((uint8                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(114))
// <<INSERT:#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))>>
// <<BEGIN_COUNT>>
#define MAP_LL_CreateConn                                ((uint8                    (*) (uint16, uint16, uint8, uint8, uint8 *, uint8, uint16, uint16, uint16, uint16, uint16, uint16))                ROM_BLE_JT_OFFSET(115))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_LL_CreateConnCancel                          ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(116))
#define MAP_LL_CtrlToHostFlowControl                     ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(117))
#define MAP_LL_DirectTestEnd                             ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(118))
#define MAP_LL_DirectTestEndDoneCback                    ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(119))
#define MAP_LL_DirectTestRxTest                          ((uint8                    (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(120))
#define MAP_LL_DirectTestTxTest                          ((uint8                    (*) (uint8, uint8, uint8, uint8))                                                                                  ROM_BLE_JT_OFFSET(121))
#define MAP_LL_DisableAdvSets                            ((void                     (*) (void))                                                                                                       ROM_BLE_JT_OFFSET(122))
#define MAP_LL_Disconnect                                ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(123))
#define MAP_LL_DisconnectCback                           ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(124))
#define MAP_LL_EncChangeCback                            ((void                     (*) (uint16, uint8, uint8))                                                                                        ROM_BLE_JT_OFFSET(125))
#define MAP_LL_EncKeyRefreshCback                        ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(126))
#define MAP_LL_EncLtkNegReply                            ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(127))
#define MAP_LL_EncLtkReply                               ((uint8                    (*) (uint16, uint8 *))                                                                                             ROM_BLE_JT_OFFSET(128))
#define MAP_LL_EncLtkReqCback                            ((void                     (*) (uint16, uint8 *, uint8 *))                                                                                    ROM_BLE_JT_OFFSET(129))
#define MAP_LL_Encrypt                                   ((uint8                    (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(130))
#define MAP_LL_EnhancedRxTest                           ((llStatus_t                (*) (uint8,uint8,uint8))                                                                                           ROM_BLE_JT_OFFSET(131))
#define MAP_LL_EnhancedTxTest                           ((llStatus_t                (*) (uint8,uint8,uint8,uint8))                                                                                     ROM_BLE_JT_OFFSET(132))
#define MAP_LL_GetAdvSet                                ((advSet_t     *            (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(133))
#define MAP_LL_GetNumActiveConns                         ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(134))
#define MAP_LL_PhyUpdateCompleteEventCback              ((void                      (*) (llStatus_t,uint16,uint8,uint8))                                                                               ROM_BLE_JT_OFFSET(135))
#define MAP_LL_RX_bm_alloc                               ((void *                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(136))
#define MAP_LL_Rand                                      ((uint8                    (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(137))
#define MAP_LL_RandCback                                 ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(138))
#define MAP_LL_ReadAdvChanTxPower                        ((uint8                    (*) (int8 *))                                                                                                      ROM_BLE_JT_OFFSET(139))
#define MAP_LL_ReadAuthPayloadTimeout                    ((uint8                    (*) (uint16, uint16 *))                                                                                            ROM_BLE_JT_OFFSET(140))
#define MAP_LL_ReadBDADDR                                ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(141))
#define MAP_LL_ReadChanMap                               ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(142))
#define MAP_LL_ReadLocalSupportedFeatures                ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(143))
#define MAP_LL_ReadLocalVersionInfo                      ((uint8                    (*) (uint8 *, uint16 *, uint16 *))                                                                                 ROM_BLE_JT_OFFSET(144))
#define MAP_LL_ReadPhy                                  ((llStatus_t                (*) (uint16,uint8 *,uint8 *))                                                                                      ROM_BLE_JT_OFFSET(145))
#define MAP_LL_ReadRemoteUsedFeatures                    ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(146))
#define MAP_LL_ReadRemoteUsedFeaturesCompleteCback       ((uint8                    (*) (uint8, uint16, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(147))
#define MAP_LL_ReadRemoteVersionInfo                     ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(148))
#define MAP_LL_ReadRemoteVersionInfoCback                ((void                     (*) (uint8, uint16, uint8, uint16, uint16))                                                                        ROM_BLE_JT_OFFSET(149))
#define MAP_LL_ReadRssi                                  ((uint8                    (*) (uint16, int8 *))                                                                                              ROM_BLE_JT_OFFSET(150))
#define MAP_LL_ReadSupportedStates                       ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(151))
#define MAP_LL_ReadTxPowerLevel                          ((uint8                    (*) (uint8, uint8, int8 *))                                                                                        ROM_BLE_JT_OFFSET(152))
#define MAP_LL_ReadWlSize                                ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(153))
#define MAP_LL_RemoteConnParamReqReply                   ((uint8                    (*) (uint16, uint16, uint16, uint16, uint16, uint16, uint16))                                                      ROM_BLE_JT_OFFSET(154))
#define MAP_LL_RemoteConnParamReqNegReply                ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(155))
#define MAP_LL_RemoteConnParamReqCback                   ((void                     (*) (uint16, uint16, uint16, uint16, uint16))                                                                      ROM_BLE_JT_OFFSET(156))
#define MAP_LL_RemoveWhiteListDevice                     ((uint8                    (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(157))
#define MAP_LL_Reset                                     ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(158))
// <<INSERT:#if (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
#define MAP_LL_RxDataCompleteCback                       ((void                     (*) (uint16, uint8 *, uint8, uint8, int8))                                                                         ROM_BLE_JT_OFFSET(159))
// <<INSERT:#else // !(ADV_CONN_CFG | INIT_CFG)>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif // (ADV_CONN_CFG | INIT_CFG)>>
// <<INSERT:#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))>>
// <<BEGIN_COUNT>>
#define MAP_LL_SetAdvControl                             ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(160))
#define MAP_LL_SetAdvData                                ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(161))
#define MAP_LL_SetAdvParam                               ((uint8                    (*) (uint16, uint16, uint8, uint8, uint8, uint8 *, uint8, uint8))                                                  ROM_BLE_JT_OFFSET(162))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_LL_SetDefaultPhy                            ((llStatus_t                (*) (uint8,uint8,uint8))                                                                                           ROM_BLE_JT_OFFSET(163))
#define MAP_LL_SetPhy                                   ((llStatus_t                (*) (uint16,uint8,uint8,uint8,uint16))                                                                             ROM_BLE_JT_OFFSET(164))
#define MAP_LL_SetPrivacyMode                           ((llStatus_t                (*) (uint8,uint8 *,uint8))                                                                                         ROM_BLE_JT_OFFSET(165))
#define MAP_LL_SetRandomAddress                          ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(166))
// <<INSERT:#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))>>
// <<BEGIN_COUNT>>
#define MAP_LL_SetScanControl                            ((uint8                    (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(167))
#define MAP_LL_SetScanParam                              ((uint8                    (*) (uint8, uint16, uint16, uint8, uint8))                                                                         ROM_BLE_JT_OFFSET(168))
#define MAP_LL_SetScanRspData                            ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(169))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_LL_StartEncrypt                              ((uint8                    (*) (uint16, uint8 *, uint8 *, uint8 *))                                                                           ROM_BLE_JT_OFFSET(170))
#define MAP_LL_TX_bm_alloc                               ((void *                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(171))
#define MAP_LL_TxData                                    ((uint8                    (*) (uint16, uint8 *, uint8, uint8))                                                                               ROM_BLE_JT_OFFSET(172))
#define MAP_LL_WriteAuthPayloadTimeout                   ((uint8                    (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(173))
// LL Encryption
#define MAP_LL_ENC_AES128_Decrypt                        ((void                     (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(174))
#define MAP_LL_ENC_AES128_Encrypt                        ((void                     (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(175))
#define MAP_LL_ENC_DecryptMsg                            ((void                     (*) (uint8 *, uint8, uint8, uint8 *, uint8 *))                                                                     ROM_BLE_JT_OFFSET(176))
#define MAP_LL_ENC_Encrypt                               ((void                     (*) (llConnState_t *, uint8, uint8, uint8 *))                                                                      ROM_BLE_JT_OFFSET(177))
#define MAP_LL_ENC_EncryptMsg                            ((void                     (*) (uint8 *, uint8, uint8, uint8 *))                                                                              ROM_BLE_JT_OFFSET(178))
#define MAP_LL_ENC_GenDeviceIV                           ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(179))
#define MAP_LL_ENC_GenDeviceSKD                          ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(180))
#define MAP_LL_ENC_GenerateNonce                         ((void                     (*) (uint32, uint8, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(181))
#define MAP_LL_ENC_GeneratePseudoRandNum                 ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(182))
#define MAP_LL_ENC_GenerateTrueRandNum                   ((uint8                    (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(183))
#define MAP_LL_ENC_LoadKey                               ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(184))
#define MAP_LL_ENC_ReverseBytes                          ((void                     (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(185))
// LL Vendor Specific
#define MAP_LL_EXT_BuildRevision                         ((uint8                    (*) (uint8, uint16, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(186))
#define MAP_LL_EXT_ClkDivOnHalt                          ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(187))
#define MAP_LL_EXT_DeclareNvUsage                        ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(188))
#define MAP_LL_EXT_Decrypt                               ((uint8                    (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(189))
#define MAP_LL_EXT_DisconnectImmed                       ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(190))
// <<INSERT:#if !defined(CTRL_V50_CONFIG)>>
// <<BEGIN_COUNT>>
#define MAP_LL_EXT_EndModemTest                          ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(191))
// <<END_COUNT>>
// <<INSERT:#else // !CTRL_V50_CONFIG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // CTRL_V50_CONFIG>>
#define MAP_LL_EXT_EnhancedModemHopTestTx                ((llStatus_t               (*) (uint8,uint8,uint8))                                                                                           ROM_BLE_JT_OFFSET(192))
#define MAP_LL_EXT_EnhancedModemTestRx                   ((llStatus_t               (*) (uint8,uint8))                                                                                                 ROM_BLE_JT_OFFSET(193))
#define MAP_LL_EXT_EnhancedModemTestTx                   ((llStatus_t               (*) (uint8,uint8,uint8))                                                                                           ROM_BLE_JT_OFFSET(194))
#define MAP_LL_EXT_ExtendRfRange                         ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(195))
#define MAP_LL_EXT_GetConnInfo                           ((uint8                    (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(196))
#define MAP_LL_EXT_HaltDuringRf                          ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(197))
#define MAP_LL_EXT_MapPmIoPort                           ((uint8                    (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(198))
// <<INSERT:#if !defined(CTRL_V50_CONFIG)>>
// <<BEGIN_COUNT>>
#define MAP_LL_EXT_ModemHopTestTx                        ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(199))
#define MAP_LL_EXT_ModemTestRx                           ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(200))
#define MAP_LL_EXT_ModemTestTx                           ((uint8                    (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(201))
// <<END_COUNT>>
// <<INSERT:#else // !CTRL_V50_CONFIG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // CTRL_V50_CONFIG>>
#define MAP_LL_EXT_NumComplPktsLimit                     ((uint8                    (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(202))
#define MAP_LL_EXT_OnePacketPerEvent                     ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(203))
#define MAP_LL_EXT_OverlappedProcessing                  ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(204))
#define MAP_LL_EXT_PERbyChan                             ((uint8                    (*) (uint16, perByChan_t *))                                                                                       ROM_BLE_JT_OFFSET(205))
#define MAP_LL_EXT_PacketErrorRate                       ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(206))
#define MAP_LL_EXT_PacketErrorRateCback                  ((void                     (*) (uint16, uint16, uint16, uint16))                                                                              ROM_BLE_JT_OFFSET(207))
#define MAP_LL_EXT_ReadRandomAddress                     ((llStatus_t               (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(208))
#define MAP_LL_EXT_ResetSystem                           ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(209))
#define MAP_LL_EXT_SaveFreqTune                          ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(210))
#define MAP_LL_EXT_SetBDADDR                             ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(211))
#define MAP_LL_EXT_SetDtmTxPktCnt                        ((llStatus_t               (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(212))
#define MAP_LL_EXT_SetFastTxResponseTime                 ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(213))
#define MAP_LL_EXT_SetFreqTune                           ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(214))
#define MAP_LL_EXT_SetLocalSupportedFeatures             ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(215))
#define MAP_LL_EXT_SetMaxDtmTxPower                      ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(216))
#define MAP_LL_EXT_SetRxGain                             ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(217))
#define MAP_LL_EXT_SetSCA                                ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(218))
#define MAP_LL_EXT_SetSlaveLatencyOverride               ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(219))
#define MAP_LL_EXT_SetTxPower                            ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(220))
// LL White List
#define MAP_WL_AddEntry                                  ((uint8                    (*) (wlTable_t *, uint8 *, uint8, uint8))                                                                          ROM_BLE_JT_OFFSET(221))
#define MAP_WL_Clear                                     ((void                     (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(222))
#define MAP_WL_ClearEntry                                ((void                     (*) (wlEntry_t *))                                                                                                 ROM_BLE_JT_OFFSET(223))
#define MAP_WL_ClearIgnoreList                           ((uint8                    (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(224))
#define MAP_WL_FindEntry                                 ((uint8                    (*) (wlTable_t *, uint8 *, uint8))                                                                                 ROM_BLE_JT_OFFSET(225))
#define MAP_WL_GetNumFreeEntries                         ((uint8                    (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(226))
#define MAP_WL_GetSize                                   ((uint8                    (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(227))
#define MAP_WL_Init                                      ((void                     (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(228))
#define MAP_WL_RemoveEntry                               ((uint8                    (*) (wlTable_t *, uint8 *, uint8))                                                                                 ROM_BLE_JT_OFFSET(229))
#define MAP_WL_SetWlIgnore                               ((llStatus_t               (*) (wlTable_t *, uint8 *, uint8))                                                                                 ROM_BLE_JT_OFFSET(230))
// LL RF
#define MAP_rfCallback                                   ((void                     (*) (RF_Handle, RF_CmdHandle, RF_EventMask))                                                                       ROM_BLE_JT_OFFSET(231))
// LL RFHAL
#define MAP_RFHAL_InitDataQueue                          ((void                     (*) (dataEntryQ_t *))                                                                                              ROM_BLE_JT_OFFSET(232))
#define MAP_RFHAL_FreeNextTxDataEntry                    ((void                     (*) (dataEntryQ_t *))                                                                                              ROM_BLE_JT_OFFSET(233))
#define MAP_RFHAL_GetNextDataEntry                       ((dataEntry_t *            (*) (dataEntryQ_t *))                                                                                              ROM_BLE_JT_OFFSET(234))
#define MAP_RFHAL_GetTempDataEntry                       ((dataEntry_t *            (*) (dataEntryQ_t *))                                                                                              ROM_BLE_JT_OFFSET(235))
#define MAP_RFHAL_NextDataEntryDone                      ((void                     (*) (dataEntryQ_t *))                                                                                              ROM_BLE_JT_OFFSET(236))
// V4.2 Extended Data Length
#define MAP_LL_DataLengthChangeEventCback                ((uint8                    (*) (uint16, uint16, uint16, uint16, uint16))                                                                      ROM_BLE_JT_OFFSET(237))
#define MAP_LL_ReadDefaultDataLen                        ((uint8                    (*) (uint16 *, uint16 *))                                                                                          ROM_BLE_JT_OFFSET(238))
#define MAP_LL_ReadMaxDataLen                            ((uint8                    (*) (uint16 *, uint16 *, uint16 *, uint16 *))                                                                      ROM_BLE_JT_OFFSET(239))
#define MAP_LL_SetDataLen                                ((uint8                    (*) (uint16, uint16, uint16))                                                                                      ROM_BLE_JT_OFFSET(240))
#define MAP_LL_WriteDefaultDataLen                       ((uint8                    (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(241))
#define MAP_LL_EXT_SetMaxDataLen                         ((uint8                    (*) (uint16, uint16, uint16, uint16))                                                                              ROM_BLE_JT_OFFSET(242))
#define MAP_llCheckCBack                                 ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(243))
#define MAP_llCheckRxBuffers                             ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(244))
#define MAP_llCreateRxBuffer                             ((uint8                    (*) (llConnState_t *, dataEntry_t *))                                                                              ROM_BLE_JT_OFFSET(245))
#define MAP_llOctets2Time                                ((uint16                   (*) (uint8, uint16, uint8, uint8))                                                                                 ROM_BLE_JT_OFFSET(246))
#define MAP_llReplaceRxBuffers                           ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(247))
#define MAP_llRegisterConnEvtCallback                    ((void                     (*) (llConnEvtCB_t, uint16))                                                                                       ROM_BLE_JT_OFFSET(248))
#define MAP_llSetupLenCtrlPkt                            ((uint8                    (*) (llConnState_t *, uint8))                                                                                      ROM_BLE_JT_OFFSET(249))
#define MAP_llTime2Octets                                ((uint16                   (*) (uint8, uint8, uint16, uint8))                                                                                 ROM_BLE_JT_OFFSET(250))
#define MAP_llTxPwrPoutLU                                ((uint8                    (*) (int8))                                                                                                        ROM_BLE_JT_OFFSET(251))
// V4.2 Secure Connections
#define MAP_LL_GenerateDHKeyCmd                          ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(252))
#define MAP_LL_GenerateDHKeyCompleteEventCback           ((void                     (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(253))
#define MAP_LL_ReadLocalP256PublicKeyCmd                 ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(254))
#define MAP_LL_ReadLocalP256PublicKeyCompleteEventCback  ((void                     (*) (uint8, uint8 *, uint8 *))                                                                                     ROM_BLE_JT_OFFSET(255))
// <<BEGIN EXTERN
#define MAP_ll_eccInit                                   ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(256))
#define MAP_ll_GenerateDHKey                             ((uint8                    (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(257))
#define MAP_ll_ReadLocalP256PublicKey                    ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(258))
// END EXTERN>>
// V4.2 Privacy 1.2
#define MAP_LL_AddDeviceToResolvingList                  ((uint8                    (*) (uint8, uint8 *, uint8 *, uint8 *))                                                                            ROM_BLE_JT_OFFSET(259))
#define MAP_LL_RemoveDeviceFromResolvingList             ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(260))
#define MAP_LL_ClearResolvingList                        ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(261))
#define MAP_LL_ReadResolvingListSize                     ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(262))
#define MAP_LL_ReadPeerResolvableAddress                 ((uint8                    (*) (uint8, uint8 *, uint8 *))                                                                                     ROM_BLE_JT_OFFSET(263))
#define MAP_LL_ReadLocalResolvableAddress                ((uint8                    (*) (uint8, uint8 *, uint8 *))                                                                                     ROM_BLE_JT_OFFSET(264))
#define MAP_LL_SetAddressResolutionEnable                ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(265))
#define MAP_LL_SetResolvablePrivateAddressTimeout        ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(266))
//
#define MAP_LL_PRIV_AddExtWLEntry                        ((uint8                    (*) (wlTable_t *, uint8 *, uint8, uint8))                                                                          ROM_BLE_JT_OFFSET(267))
#define MAP_LL_PRIV_Ah                                   ((uint32                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(268))
#define MAP_LL_PRIV_CheckRLPeerId                        ((void                     (*) (rlEntry_t *, wlTable_t *))                                                                                    ROM_BLE_JT_OFFSET(269))
#define MAP_LL_PRIV_CheckRLPeerIdEntry                   ((void                     (*) (rlEntry_t *, wlTable_t *))                                                                                    ROM_BLE_JT_OFFSET(270))
#define MAP_LL_PRIV_ClearAllPrivIgn                      ((void                     (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(271))
#define MAP_LL_PRIV_ClearExtWL                           ((void                     (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(272))
#define MAP_LL_PRIV_FindExtWLEntry                       ((uint8                    (*) (wlTable_t *, uint8 *, uint8))                                                                                 ROM_BLE_JT_OFFSET(273))
#define MAP_LL_PRIV_FindPeerInRL                         ((uint8                    (*) (rlEntry_t *, uint8, uint8 *))                                                                                 ROM_BLE_JT_OFFSET(274))
#define MAP_LL_PRIV_GenerateRPA                          ((void                     (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(275))
#define MAP_LL_PRIV_IsIDA                                ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(276))
#define MAP_LL_PRIV_IsRPA                                ((uint8                    (*) (uint8, uint8 *))                                                                                              ROM_BLE_JT_OFFSET(277))
#define MAP_LL_PRIV_IsZeroIRK                            ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(278))
#define MAP_LL_PRIV_IsResolvable                         ((uint8                    (*) (uint8 *, rlEntry_t *))                                                                                        ROM_BLE_JT_OFFSET(279))
#define MAP_LL_PRIV_ResolveRPA                           ((uint8                    (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(280))
#define MAP_LL_PRIV_SetupPrivacy                         ((void                     (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(281))
#define MAP_LL_PRIV_SetWLSize                            ((void                     (*) (wlTable_t *, uint8))                                                                                          ROM_BLE_JT_OFFSET(282))
#define MAP_LL_PRIV_TeardownPrivacy                      ((void                     (*) (wlTable_t *))                                                                                                 ROM_BLE_JT_OFFSET(283))
// LL Internal
#define MAP_llActiveTask                                 ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(284))
#define MAP_llAllocTask                                  ((taskInfo_t *             (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(285))
#define MAP_llAddTxDataEntry                             ((void                     (*) (dataEntryQ_t *, dataEntry_t *))                                                                               ROM_BLE_JT_OFFSET(286))
#define MAP_llAlignToNextEvent                           ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(287))
#define MAP_llAllocConnId                                ((llConnState_t *          (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(288))
// <<INSERT:#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )>>
#define MAP_llAllocRfMem                                 ((void                     (*) (advSet_t *))                                                                                                  ROM_BLE_JT_OFFSET(289))
// <<INSERT:#else // !(ADV_NCONN_CFG | ADV_CONN_CFG)>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif // (ADV_NCONN_CFG | ADV_CONN_CFG)>>
#define MAP_llAtLeastTwoChans                            ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(290))
#define MAP_llCalcScaFactor                              ((uint16                   (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(291))
#define MAP_llCBTimer_AptoExpiredCback                   ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(292))
#define MAP_llCheckForLstoDuringSL                       ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(293))
#define MAP_llCheckWhiteListUsage                        ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(294))
#define MAP_llClearRatCompare                            ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(295))
#define MAP_llConnCleanup                                ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(296))
#define MAP_llConnExists                                 ((uint8                    (*) (uint8, uint8 *, uint8))                                                                                       ROM_BLE_JT_OFFSET(297))
#define MAP_llConnTerminate                              ((void                     (*) (llConnState_t *, uint8))                                                                                      ROM_BLE_JT_OFFSET(298))
#define MAP_llConvertCtrlProcTimeoutToEvent              ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(299))
#define MAP_llConvertLstoToEvent                         ((void                     (*) (llConnState_t *, connParam_t *))                                                                              ROM_BLE_JT_OFFSET(300))
#define MAP_llDataGetConnPtr                             ((llConnState_t *          (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(301))
#define MAP_llDequeueCtrlPkt                             ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(302))
// <<INSERT:#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )>>
// <<BEGIN_COUNT>>
#define MAP_llExtAdvSchedSetup                           ((void                     (*) (taskInfo_t *))                                                                                               ROM_BLE_JT_OFFSET(303))
// <<END_COUNT>>
// <<INSERT:#else // !(ADV_NCONN_CFG | ADV_CONN_CFG) >>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // (ADV_NCONN_CFG | ADV_CONN_CFG) >>
#define MAP_llEndExtAdvTask                              ((void                     (*) (advSet_t *))                                                                                                  ROM_BLE_JT_OFFSET(304))
#define MAP_llEndExtInitTask                             ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(305))
#define MAP_llEndExtScanTask                             ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(306))
#define MAP_llEnqueueCtrlPkt                             ((void                     (*) (llConnState_t *, uint8))                                                                                      ROM_BLE_JT_OFFSET(307))
#define MAP_llEqAlreadyValidAddr                         ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(308))
#define MAP_llEqSynchWord                                ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(309))
#define MAP_llEqualBytes                                 ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(310))
#define MAP_llEventDelta                                 ((uint16                   (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(311))
#define MAP_llEventInRange                               ((uint8                    (*) (uint16, uint16, uint16))                                                                                      ROM_BLE_JT_OFFSET(312))
#define MAP_llExtAdvCBack                                ((void                     (*) (uint8, void *))                                                                                               ROM_BLE_JT_OFFSET(313))
#define MAP_llExtInit_PostProcess                        ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(314))
// <<INSERT:#if ( CTRL_CONFIG & INIT_CFG )>>
// <<BEGIN_COUNT>>
#define MAP_llExtInitSchedSetup                          ((void                     (*) (taskInfo_t *))                                                                                                ROM_BLE_JT_OFFSET(315))
// <<END_COUNT>>
// <<INSERT:#else // !INIT_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // INIT_CFG>>
#define MAP_llExtScan_PostProcess                        ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(316))
// <<INSERT:#if ( CTRL_CONFIG & SCAN_CFG )>>
// <<BEGIN_COUNT>>
#define MAP_llExtScanSchedSetup                          ((void                     (*) (taskInfo_t *))                                                                                                ROM_BLE_JT_OFFSET(317))
// <<END_COUNT>>
// <<INSERT:#else // !SCAN_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // SCAN_CFG>>
#define MAP_llFindNextAdvSet                             ((ble5OpCmd_t  *           (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(318))
#define MAP_llFindNextSecTask                            ((taskInfo_t *             (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(319))
#define MAP_llFindStartType                              ((uint8                    (*) (taskInfo_t *, taskInfo_t *))                                                                                  ROM_BLE_JT_OFFSET(320))
#define MAP_llFragmentPDU                                ((uint8                    (*) (llConnState_t *, uint8 *, uint16))                                                                            ROM_BLE_JT_OFFSET(321))
#define MAP_llFreeTask                                   ((void                     (*) (taskInfo_t **))                                                                                               ROM_BLE_JT_OFFSET(322))
#define MAP_llGenerateCRC                                ((uint32                   (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(323))
#define MAP_llGetActiveTasks                             ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(324))
#define MAP_llGetCurrentTask                             ((taskInfo_t *             (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(325))
#define MAP_llGetExtHdrLen                               ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(326))
// <<INSERT:#if (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
#define MAP_llGetNextConn                                ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(327))
// <<INSERT:#else // !(ADV_CONN_CFG | INIT_CFG)>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif // (ADV_CONN_CFG | INIT_CFG)>>
#define MAP_llGetNumTasks                                ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(328))
#define MAP_llGetSlowestPhy                              ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(329))
#define MAP_llGetTaskState                               ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(330))
// <<INSERT:#if ( CTRL_CONFIG & INIT_CFG )>>
// <<BEGIN_COUNT>>
#define MAP_llGenerateValidAccessAddr                    ((uint32                   (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(331))
// <<END_COUNT>>
// <<INSERT:#else // !INIT_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // INIT_CFG>>
#define MAP_llGetCurrentTime                             ((uint32                   (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(332))
#define MAP_llGetNextDataChan                            ((uint8                    (*) (llConnState_t *, uint16))                                                                                     ROM_BLE_JT_OFFSET(333))
#define MAP_llGetNextDataChanAlgo1                       ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(334))
#define MAP_llGetNextDataChanAlgo2                       ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(335))
#define MAP_llGetTxPower                                 ((int8                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(336))
// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & PHY_LR_CFG)>>
// <<BEGIN_COUNT>>
#define MAP_llGtElevenTransitionsInLsh                   ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(337))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_llGtSixConsecZerosOrOnes                     ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(338))
#define MAP_llGtTwentyFourTransitions                    ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(339))
#define MAP_llHaltRadio                                  ((void                     (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(340))
#define MAP_llHardwareError                              ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(341))
#define MAP_llInitFeatureSet                             ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(342))
// <<INSERT:#if (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))>>
#define MAP_llLinkSchedSetup                             ((void                     (*) (taskInfo_t *))                                                                                                ROM_BLE_JT_OFFSET(343))
// <<INSERT:#else // !(ADV_CONN_CFG | INIT_CFG)>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif // (ADV_CONN_CFG | INIT_CFG)>>
// <<INSERT:#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & PHY_LR_CFG)>>
// <<BEGIN_COUNT>>
#define MAP_llLtThreeOnesInLsb                           ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(344))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_llLtTwoChangesInLastSixBits                  ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(345))
#define MAP_llMemCopySrc                                 ((uint8 *                  (*) (uint8 *, uint8 *, uint8))                                                                                     ROM_BLE_JT_OFFSET(346))
#define MAP_llMoveTempTxDataEntries                      ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(347))
#define MAP_llNextChanIndex                              ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(348))
#define MAP_llOneBitSynchWordDiffer                      ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(349))
#define MAP_llPendingUpdateParam                         ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(350))
#define MAP_llProcessChanMap                             ((void                     (*) (llConnState_t *, uint8 *))                                                                                    ROM_BLE_JT_OFFSET(351))
#define MAP_llProcessMasterControlProcedures             ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(352))
#define MAP_llProcessTxData                              ((void                     (*) (llConnState_t *, uint8))                                                                                      ROM_BLE_JT_OFFSET(353))
#define MAP_llProcessSlaveControlProcedures              ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(354))
#define MAP_llReleaseConnId                              ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(355))
#define MAP_llReplaceCtrlPkt                             ((void                     (*) (llConnState_t *, uint8, uint8))                                                                               ROM_BLE_JT_OFFSET(356))
#define MAP_llReverseBits                                ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(357))
#define MAP_llRfInit                                     ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(358))
#define MAP_llRfSetup                                    ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(359))
#define MAP_llRfStartFS                                  ((void                     (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(360))
#define MAP_llScheduler                                  ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(361))
#define MAP_llScheduleTask                               ((void                     (*) (taskInfo_t *))                                                                                                ROM_BLE_JT_OFFSET(362))
#define MAP_llSendAdvSetEndEvent                         ((void                     (*) (advSet_t *))                                                                                                  ROM_BLE_JT_OFFSET(363))
#define MAP_llSendAdvSetTermEvent                        ((void                     (*) (advSet_t *, uint8, uint8))                                                                                    ROM_BLE_JT_OFFSET(364))
#define MAP_llSendReject                                 ((void                     (*) (llConnState_t *, uint8, uint8))                                                                               ROM_BLE_JT_OFFSET(365))
#define MAP_llSetCodedMaxTxTime                          ((uint16                   (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(366))
#define MAP_llSetNextDataChan                            ((void                     (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(367))
#define MAP_llSetTxPower                                 ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(368))
// <<INSERT:#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))>>
// <<BEGIN_COUNT>>
#define MAP_llSetupAdv                                   ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(369))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_llSetupAdvDataEntryQueue                     ((dataEntryQ_t *           (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(370))
#define MAP_llSetupConn                                  ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(371))
#define MAP_llSetupConnParamReq                          ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(372))
#define MAP_llSetupConnParamRsp                          ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(373))
#define MAP_llSetupConnRxDataEntryQueue                  ((dataEntryQ_t *           (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(374))
#define MAP_llSetupEncReq                                ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(375))
#define MAP_llSetupEncRsp                                ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(376))
// <<INSERT:#if ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )>>
#define MAP_llSetupExtAdv                                ((llStatus_t               (*) (advSet_t *))                                                                                                  ROM_BLE_JT_OFFSET(377))
// <<INSERT:#else // !( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif // ( CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG) )>>
#define MAP_llSetupExtAdvLegacy                          ((llStatus_t               (*) (advSet_t *))                                                                                                  ROM_BLE_JT_OFFSET(378))
#define MAP_llSetupExtData                               ((void                     (*) (advSet_t *))                                                                                                  ROM_BLE_JT_OFFSET(379))
#define MAP_llSetupExtHdr                                ((void                     (*) (advSet_t *, uint8, uint16))                                                                                   ROM_BLE_JT_OFFSET(380))
#define MAP_llSetupExtInit                               ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(381))
#define MAP_llSetupExtScan                               ((llStatus_t               (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(382))
#define MAP_llSetupFeatureSetReq                         ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(383))
#define MAP_llSetupFeatureSetRsp                         ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(384))
// <<INSERT:#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))>>
// <<BEGIN_COUNT>>
#define MAP_llSetupInit                                  ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(385))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_llSetupInitDataEntryQueue                    ((dataEntryQ_t *           (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(386))
#define MAP_llSetupNextMasterEvent                       ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(387))
#define MAP_llSetupNextSlaveEvent                        ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(388))
#define MAP_llSetupPauseEncReq                           ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(389))
#define MAP_llSetupPauseEncRsp                           ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(390))
#define MAP_llSetupPhyCtrlPkt                            ((uint8                    (*) (llConnState_t *, uint8))                                                                                      ROM_BLE_JT_OFFSET(391))
#define MAP_llSetupPingReq                               ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(392))
#define MAP_llSetupPingRsp                               ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(393))
#define MAP_llSetupRatCompare                            ((void                     (*) (taskInfo_t *))                                                                                               ROM_BLE_JT_OFFSET(394))
#define MAP_llSetupRejectInd                             ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(395))
#define MAP_llSetupRejectIndExt                          ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(396))
// <<INSERT:#if !(defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & AE_CFG))>>
// <<BEGIN_COUNT>>
#define MAP_llSetupScan                                  ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(397))
// <<END_COUNT>>
// <<INSERT:#else // AE_CFG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // !AE_CFG>>
#define MAP_llSetupScanDataEntryQueue                    ((dataEntryQ_t *           (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(398))
#define MAP_llSetupStartEncReq                           ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(399))
#define MAP_llSetupStartEncRsp                           ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(400))
#define MAP_llSetupTermInd                               ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(401))
#define MAP_llSetupUnknownRsp                            ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(402))
#define MAP_llSetupUpdateChanReq                         ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(403))
#define MAP_llSetupUpdateParamReq                        ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(404))
#define MAP_llSetupVersionIndReq                         ((uint8                    (*) (llConnState_t *))                                                                                             ROM_BLE_JT_OFFSET(405))
#define MAP_llStartDurationTimer                         ((llStatus_t               (*) (uint16, uint32))                                                                                              ROM_BLE_JT_OFFSET(406))
#define MAP_llTimeCompare                                ((uint8                    (*) (uint32, uint32))                                                                                              ROM_BLE_JT_OFFSET(407))
#define MAP_llTimeDelta                                  ((uint32                   (*) (uint32, uint32))                                                                                              ROM_BLE_JT_OFFSET(408))
#define MAP_llValidAccessAddr                            ((uint8                    (*) (uint32))                                                                                                      ROM_BLE_JT_OFFSET(409))
#define MAP_llValidateConnParams                         ((uint8                    (*) (llConnState_t *, uint16, uint16, uint16, uint16, uint16, uint8, uint16, uint16 *))                            ROM_BLE_JT_OFFSET(410))
#define MAP_llVerifyCodedConnInterval                    ((uint8                    (*) (llConnState_t *, uint16))                                                                                     ROM_BLE_JT_OFFSET(411))
#define MAP_llVerifyConnParamReqParams                   ((uint8                    (*) (uint16, uint16, uint16, uint8, uint16, uint16 *))                                                             ROM_BLE_JT_OFFSET(412))
#define MAP_llWriteTxData                                ((uint8                    (*) (llConnState_t *, uint8 *, uint8, uint8, uint8))                                                               ROM_BLE_JT_OFFSET(413))
//
// Host
//
// <<INSERT:#ifdef HOST_CONFIG>>
// <<BEGIN_COUNT>>
// LinkDB
#define MAP_linkDB_Add                                   ((uint8                    (*) (uint8, uint16, uint8, uint8, uint8 *, uint8 *, uint8, uint16, uint16, uint16, uint16))                        ROM_BLE_JT_OFFSET(414))
#define MAP_linkDB_Authen                                ((uint8                    (*) (uint16, uint8, uint8))                                                                                        ROM_BLE_JT_OFFSET(415))
#define MAP_linkDB_Find                                  ((linkDBItem_t *           (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(416))
#define MAP_linkDB_Init                                  ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(417))
#define MAP_linkDB_MTU                                   ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(418))
#define MAP_linkDB_Register                              ((uint8                    (*) (pfnLinkDBCB_t))                                                                                               ROM_BLE_JT_OFFSET(419))
#define MAP_linkDB_Remove                                ((uint8                    (*) (linkDBItem_t *))                                                                                              ROM_BLE_JT_OFFSET(420))
#define MAP_linkDB_Role                                  ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(421))
#define MAP_linkDB_reportStatusChange                    ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(422))
#define MAP_linkDB_UpdateMTU                             ((uint8                    (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(423))
#define MAP_linkDB_Update                                ((uint8                    (*) (uint16, uint8, uint8))                                                                                        ROM_BLE_JT_OFFSET(424))
#define MAP_linkDB_updateConnParam                       ((uint8                    (*) (uint16, uint16, uint16, uint16))                                                                              ROM_BLE_JT_OFFSET(425))
#define MAP_linkDB_NumActive                             ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(426))
#define MAP_linkDB_NumConns                              ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(427))
#define MAP_linkDB_State                                 ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(428))
// L2CAP
#define MAP_L2CAP_bm_alloc                               ((void *                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(429))
#define MAP_L2CAP_BuildCmdReject                         ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(430))
#define MAP_L2CAP_BuildConnectReq                        ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(431))
#define MAP_L2CAP_BuildParamUpdateRsp                    ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(432))
#define MAP_L2CAP_CmdReject                              ((bStatus_t                (*) (uint16, uint8, l2capCmdReject_t *))                                                                           ROM_BLE_JT_OFFSET(433))
#define MAP_L2CAP_ConnParamUpdateReq                     ((bStatus_t                (*) (uint16, l2capParamUpdateReq_t *, uint8))                                                                      ROM_BLE_JT_OFFSET(434))
#define MAP_L2CAP_ConnParamUpdateRsp                     ((bStatus_t                (*) (uint16, uint8, l2capParamUpdateRsp_t *))                                                                      ROM_BLE_JT_OFFSET(435))
#define MAP_L2CAP_DisconnectReq                          ((bStatus_t                (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(436))
#define MAP_L2CAP_GetMTU                                 ((uint16                   (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(437))
#define MAP_L2CAP_HostNumCompletedPkts                   ((void                     (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(438))
#define MAP_L2CAP_ParseConnectReq                        ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(439))
#define MAP_L2CAP_ParseFlowCtrlCredit                    ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(440))
#define MAP_L2CAP_ParseParamUpdateReq                    ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(441))
#define MAP_L2CAP_RegisterApp                            ((bStatus_t                (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(442))
#define MAP_L2CAP_SendData                               ((bStatus_t                (*) (uint16, l2capPacket_t *))                                                                                     ROM_BLE_JT_OFFSET(443))
// <<BEGIN EXTERN
#define MAP_L2CAP_SendDataPkt                            ((bStatus_t                (*) (uint16, uint16, uint8 *))                                                                                     ROM_BLE_JT_OFFSET(444))
// END EXTERN>>
#define MAP_L2CAP_SetBufSize                             ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(445))
#define MAP_L2CAP_SetControllerToHostFlowCtrl            ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(446))
// <<BEGIN EXTERN
#define MAP_l2capAllocChannel                            ((l2capChannel_t *         (*) (l2capPsm_t *))                                                                                                ROM_BLE_JT_OFFSET(447))
#define MAP_l2capAllocConnChannel                        ((bStatus_t                (*) (uint16, uint8, l2capPsm_t *, l2capChannel_t **))                                                              ROM_BLE_JT_OFFSET(448))
#define MAP_l2capBuildInfoReq                            ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(449))
#define MAP_l2capBuildParamUpdateReq                     ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(450))
#define MAP_l2capBuildSignalHdr                          ((void                     (*) (l2capSignalHdr_t *, uint8 *))                                                                                 ROM_BLE_JT_OFFSET(451))
#define MAP_l2capDisconnectAllChannels                   ((void                     (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(452))
#define MAP_l2capEncapSendData                           ((bStatus_t                (*) (uint16, l2capPacket_t *))                                                                                     ROM_BLE_JT_OFFSET(453))
#define MAP_l2capFindLocalId                             ((l2capChannel_t *         (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(454))
#define MAP_l2capFreeChannel                             ((void                     (*) (l2capChannel_t *))                                                                                            ROM_BLE_JT_OFFSET(455))
#define MAP_l2capFreePendingPkt                          ((void                     (*) (uint16, uint16, uint8))                                                                                       ROM_BLE_JT_OFFSET(456))
#define MAP_l2capHandleTimerCB                           ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(457))
#define MAP_l2capHandleRxError                           ((void                     (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(458))
#define MAP_l2capNotifyData                              ((bStatus_t                (*) (uint8, uint16, l2capPacket_t *))                                                                              ROM_BLE_JT_OFFSET(459))
#define MAP_l2capNotifyEvent                             ((void                     (*) (l2capChannel_t *, uint8))                                                                                     ROM_BLE_JT_OFFSET(460))
#define MAP_l2capNotifySignal                            ((void                     (*) (uint8, uint16, uint8, uint8, uint8, l2capSignalCmd_t *))                                                      ROM_BLE_JT_OFFSET(461))
#define MAP_l2capParseCmdReject                          ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(462))
#define MAP_l2capParsePacket                             ((uint8                    (*) (l2capPacket_t *, hciDataEvent_t *))                                                                           ROM_BLE_JT_OFFSET(463))
#define MAP_l2capParseParamUpdateRsp                     ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(464))
#define MAP_l2capParseSignalHdr                          ((void                     (*) (l2capSignalHdr_t *, uint8 *))                                                                                 ROM_BLE_JT_OFFSET(465))
#define MAP_l2capProcessOSALMsg                          ((void                     (*) (osal_event_hdr_t *))                                                                                          ROM_BLE_JT_OFFSET(466))
#define MAP_l2capProcessReq                              ((bStatus_t                (*) (uint16, l2capSignalHdr_t *, uint8 *))                                                                         ROM_BLE_JT_OFFSET(467))
#define MAP_l2capProcessRsp                              ((bStatus_t                (*) (uint16, l2capSignalHdr_t *, uint8 *))                                                                         ROM_BLE_JT_OFFSET(468))
#define MAP_l2capProcessRxData                           ((void                     (*) (hciDataEvent_t *))                                                                                            ROM_BLE_JT_OFFSET(469))
#define MAP_l2capProcessSignal                           ((void                     (*) (uint16, l2capPacket_t *))                                                                                     ROM_BLE_JT_OFFSET(470))
#define MAP_l2capSendCmd                                 ((bStatus_t                (*) (uint16, uint8, uint8, uint8 *, pfnL2CAPBuildCmd_t))                                                           ROM_BLE_JT_OFFSET(471))
#define MAP_l2capSendFCPkt                               ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(472))
#define MAP_l2capSendPkt                                 ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(473))
#define MAP_l2capSendReq                                 ((bStatus_t                (*) (uint16, uint8, uint8 *, pfnL2CAPBuildCmd_t, uint8, uint8, l2capPsm_t *))                                      ROM_BLE_JT_OFFSET(474))
#define MAP_l2capStartTimer                              ((void                     (*) (l2capChannel_t *, uint16))                                                                                    ROM_BLE_JT_OFFSET(475))
#define MAP_l2capStopTimer                               ((void                     (*) (l2capChannel_t *))                                                                                            ROM_BLE_JT_OFFSET(476))
#define MAP_l2capStoreFCPkt                              ((bStatus_t                (*) (uint16, l2capPacket_t *))                                                                                     ROM_BLE_JT_OFFSET(477))
#define MAP_l2capFindPsm                                 ((l2capPsm_t *             (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(478))
#define MAP_l2capAllocPsm                                ((bStatus_t                (*) (l2capPsm_t *))                                                                                                ROM_BLE_JT_OFFSET(479))
#define MAP_l2capNumActiveChannnels                      ((uint8                    (*) (l2capPsm_t *))                                                                                                ROM_BLE_JT_OFFSET(480))
#define MAP_l2capFindLocalCID                            ((l2capChannel_t *         (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(481))
#define MAP_l2capGetCoChannelInfo                        ((void                     (*) (l2capCoChannel_t *, l2capCoCInfo_t *))                                                                        ROM_BLE_JT_OFFSET(482))
#define MAP_l2capFindRemoteId                            ((l2capChannel_t *         (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(483))
#define MAP_l2capSendConnectRsp                          ((bStatus_t                (*) (uint16, uint8,uint16, l2capChannel_t *))                                                                      ROM_BLE_JT_OFFSET(484))
#define MAP_l2capBuildDisconnectReq                      ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(485))
#define MAP_l2capFlowCtrlCredit                          ((bStatus_t                (*) (uint16, uint16, uint16))                                                                                      ROM_BLE_JT_OFFSET(486))
#define MAP_l2capReassembleSegment                       ((uint8                    (*) (uint16, l2capPacket_t *))                                                                                     ROM_BLE_JT_OFFSET(487))
#define MAP_l2capParseConnectRsp                         ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(488))
#define MAP_l2capNotifyChannelEstEvt                     ((void                     (*) (l2capChannel_t *, uint8, uint16))                                                                             ROM_BLE_JT_OFFSET(489))
#define MAP_l2capParseDisconnectRsp                      ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(490))
#define MAP_l2capNotifyChannelTermEvt                    ((void                     (*) (l2capChannel_t *, uint8, uint16))                                                                             ROM_BLE_JT_OFFSET(491))
#define MAP_l2capProcessConnectReq                       ((void                     (*) (uint16, uint8, l2capConnectReq_t *))                                                                          ROM_BLE_JT_OFFSET(492))
#define MAP_l2capParseDisconnectReq                      ((bStatus_t                (*) (l2capSignalCmd_t *, uint8 *, uint16))                                                                         ROM_BLE_JT_OFFSET(493))
#define MAP_l2capBuildDisconnectRsp                      ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(494))
#define MAP_l2capFindRemoteCID                           ((l2capChannel_t *         (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(495))
#define MAP_l2capDisconnectChannel                       ((void                     (*) (l2capChannel_t *, uint16))                                                                                    ROM_BLE_JT_OFFSET(496))
#define MAP_l2capSendNextSegment                         ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(497))
#define MAP_l2capFindNextSegment                         ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(498))
#define MAP_l2capSendSegment                             ((uint8                    (*) (l2capChannel_t *))                                                                                            ROM_BLE_JT_OFFSET(499))
#define MAP_l2capFreeTxSDU                               ((void                     (*) (l2capChannel_t *, uint8))                                                                                     ROM_BLE_JT_OFFSET(500))
#define MAP_l2capNotifyCreditEvt                         ((void                     (*) (l2capChannel_t *, uint8))                                                                                     ROM_BLE_JT_OFFSET(501))
#define MAP_l2capBuildConnectRsp                         ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(502))
#define MAP_l2capNotifySendSduDoneEvt                    ((void                     (*) (l2capChannel_t *, uint8))                                                                                     ROM_BLE_JT_OFFSET(503))
#define MAP_l2capBuildFlowCtrlCredit                     ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(504))
// END EXTERN>>
// ATT
#define MAP_ATT_BuildErrorRsp                            ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(505))
#define MAP_ATT_BuildExchangeMTURsp                      ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(506))
#define MAP_ATT_BuildFindByTypeValueRsp                  ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(507))
#define MAP_ATT_BuildFindInfoRsp                         ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(508))
#define MAP_ATT_BuildHandleValueInd                      ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(509))
#define MAP_ATT_BuildPrepareWriteRsp                     ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(510))
#define MAP_ATT_BuildReadBlobRsp                         ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(511))
#define MAP_ATT_BuildReadByGrpTypeRsp                    ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(512))
#define MAP_ATT_BuildReadByTypeRsp                       ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(513))
#define MAP_ATT_BuildReadMultiRsp                        ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(514))
#define MAP_ATT_BuildReadRsp                             ((uint16                   (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(515))
#define MAP_ATT_CompareUUID                              ((uint8                    (*) (const uint8 *, uint16, const uint8 *, uint16))                                                                ROM_BLE_JT_OFFSET(516))
#define MAP_ATT_ConvertUUIDto128                         ((uint8                    (*) (const uint8 *, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(517))
#define MAP_ATT_ErrorRsp                                 ((bStatus_t                (*) (uint16, attErrorRsp_t *))                                                                                     ROM_BLE_JT_OFFSET(518))
#define MAP_ATT_ExchangeMTUReq                           ((bStatus_t                (*) (uint16, attExchangeMTUReq_t *))                                                                               ROM_BLE_JT_OFFSET(519))
#define MAP_ATT_ExchangeMTURsp                           ((bStatus_t                (*) (uint16, attExchangeMTURsp_t *))                                                                               ROM_BLE_JT_OFFSET(520))
#define MAP_ATT_ExecuteWriteReq                          ((bStatus_t                (*) (uint16, attExecuteWriteReq_t *))                                                                              ROM_BLE_JT_OFFSET(521))
#define MAP_ATT_ExecuteWriteRsp                          ((bStatus_t                (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(522))
#define MAP_ATT_FindByTypeValueReq                       ((bStatus_t                (*) (uint16, attFindByTypeValueReq_t *))                                                                           ROM_BLE_JT_OFFSET(523))
#define MAP_ATT_FindByTypeValueRsp                       ((bStatus_t                (*) (uint16, attFindByTypeValueRsp_t *))                                                                           ROM_BLE_JT_OFFSET(524))
#define MAP_ATT_FindInfoReq                              ((bStatus_t                (*) (uint16, attFindInfoReq_t *))                                                                                  ROM_BLE_JT_OFFSET(525))
#define MAP_ATT_FindInfoRsp                              ((bStatus_t                (*) (uint16, attFindInfoRsp_t *))                                                                                  ROM_BLE_JT_OFFSET(526))
#define MAP_ATT_GetMTU                                   ((uint16                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(527))
#define MAP_ATT_HandleValueInd                           ((bStatus_t                (*) (uint16, attHandleValueInd_t *))                                                                               ROM_BLE_JT_OFFSET(528))
#define MAP_ATT_HandleValueNoti                          ((bStatus_t                (*) (uint16, attHandleValueNoti_t *))                                                                              ROM_BLE_JT_OFFSET(529))
#define MAP_ATT_ParseErrorRsp                            ((bStatus_t                (*) (uint8 *, uint16, attMsg_t *))                                                                                 ROM_BLE_JT_OFFSET(530))
#define MAP_ATT_ParseExchangeMTUReq                      ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(531))
#define MAP_ATT_ParseExecuteWriteReq                     ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(532))
#define MAP_ATT_ParseFindInfoReq                         ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(533))
#define MAP_ATT_ParseFindByTypeValueReq                  ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(534))
#define MAP_ATT_ParseHandleValueInd                      ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(535))
#define MAP_ATT_ParsePacket                              ((uint8                    (*) (l2capDataEvent_t *, attPacket_t *))                                                                           ROM_BLE_JT_OFFSET(536))
#define MAP_ATT_ParsePrepareWriteReq                     ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(537))
#define MAP_ATT_ParseReadBlobReq                         ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(538))
#define MAP_ATT_ParseReadByTypeReq                       ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(539))
#define MAP_ATT_ParseReadMultiReq                        ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(540))
#define MAP_ATT_ParseReadReq                             ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(541))
#define MAP_ATT_ParseWriteReq                            ((bStatus_t                (*) (uint8, uint8, uint8 *, uint16, attMsg_t *))                                                                   ROM_BLE_JT_OFFSET(542))
#define MAP_ATT_PrepareWriteReq                          ((bStatus_t                (*) (uint16, attPrepareWriteReq_t *))                                                                              ROM_BLE_JT_OFFSET(543))
#define MAP_ATT_PrepareWriteRsp                          ((bStatus_t                (*) (uint16, attPrepareWriteRsp_t *))                                                                              ROM_BLE_JT_OFFSET(544))
#define MAP_ATT_ReadBlobReq                              ((bStatus_t                (*) (uint16, attReadBlobReq_t *))                                                                                  ROM_BLE_JT_OFFSET(545))
#define MAP_ATT_ReadBlobRsp                              ((bStatus_t                (*) (uint16, attReadBlobRsp_t *))                                                                                  ROM_BLE_JT_OFFSET(546))
#define MAP_ATT_ReadByGrpTypeReq                         ((bStatus_t                (*) (uint16, attReadByGrpTypeReq_t *))                                                                             ROM_BLE_JT_OFFSET(547))
#define MAP_ATT_ReadByGrpTypeRsp                         ((bStatus_t                (*) (uint16, attReadByGrpTypeRsp_t *))                                                                             ROM_BLE_JT_OFFSET(548))
#define MAP_ATT_ReadByTypeReq                            ((bStatus_t                (*) (uint16, attReadByTypeReq_t *))                                                                                ROM_BLE_JT_OFFSET(549))
#define MAP_ATT_ReadByTypeRsp                            ((bStatus_t                (*) (uint16, attReadByTypeRsp_t *))                                                                                ROM_BLE_JT_OFFSET(550))
#define MAP_ATT_ReadMultiReq                             ((bStatus_t                (*) (uint16, attReadMultiReq_t *))                                                                                 ROM_BLE_JT_OFFSET(551))
#define MAP_ATT_ReadMultiRsp                             ((bStatus_t                (*) (uint16, attReadMultiRsp_t *))                                                                                 ROM_BLE_JT_OFFSET(552))
#define MAP_ATT_ReadReq                                  ((bStatus_t                (*) (uint16, attReadReq_t *))                                                                                      ROM_BLE_JT_OFFSET(553))
#define MAP_ATT_ReadRsp                                  ((bStatus_t                (*) (uint16, attReadRsp_t *))                                                                                      ROM_BLE_JT_OFFSET(554))
#define MAP_ATT_RegisterServer                           ((void                     (*) (attNotifyTxCB_t))                                                                                             ROM_BLE_JT_OFFSET(555))
#define MAP_ATT_RegisterClient                           ((void                     (*) (attNotifyTxCB_t))                                                                                             ROM_BLE_JT_OFFSET(556))
#define MAP_ATT_UpdateMTU                                ((uint8                    (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(557))
#define MAP_ATT_WriteReq                                 ((bStatus_t                (*) (uint16, attWriteReq_t *))                                                                                     ROM_BLE_JT_OFFSET(558))
#define MAP_ATT_WriteRsp                                 ((bStatus_t                (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(559))
// <<BEGIN EXTERN
#define MAP_attSendMsg                                   ((bStatus_t                (*) (uint16, attBuildMsg_t, uint8, uint8 *, uint8 *))                                                              ROM_BLE_JT_OFFSET(560))
#define MAP_attSendRspMsg                                ((bStatus_t                (*) (uint16, attBuildMsg_t, uint8, uint8 *, uint8 *))                                                              ROM_BLE_JT_OFFSET(561))
// END EXTERN>>
// GATT
#define MAP_GATT_AppCompletedMsg                         ((void                     (*) (gattMsgEvent_t *))                                                                                            ROM_BLE_JT_OFFSET(562))
#define MAP_GATT_bm_alloc                                ((void *                   (*) (uint16, uint8, uint16, uint16 *))                                                                             ROM_BLE_JT_OFFSET(563))
#define MAP_GATT_bm_free                                 ((void                     (*) (gattMsg_t *, uint8))                                                                                          ROM_BLE_JT_OFFSET(564))
#define MAP_GATT_FindHandle                              ((gattAttribute_t *        (*) (uint16, uint16 *))                                                                                            ROM_BLE_JT_OFFSET(565))
#define MAP_GATT_FindHandleUUID                          ((gattAttribute_t *        (*) (uint16, uint16, const uint8 *, uint16, uint16 *))                                                             ROM_BLE_JT_OFFSET(566))
#define MAP_GATT_FindNextAttr                            ((gattAttribute_t *        (*) (gattAttribute_t *, uint16, uint16, uint16 *))                                                                 ROM_BLE_JT_OFFSET(567))
#define MAP_GATT_InitServer                              ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(568))
#define MAP_GATT_Indication                              ((bStatus_t                (*) (uint16, attHandleValueInd_t *, uint8, uint8))                                                                 ROM_BLE_JT_OFFSET(569))
#define MAP_GATT_NotifyEvent                             ((bStatus_t                (*) (uint16, uint8, uint8, gattMsg_t *))                                                                           ROM_BLE_JT_OFFSET(570))
#define MAP_GATT_ServiceEncKeySize                       ((uint8                    (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(571))
#define MAP_GATT_ServiceNumAttrs                         ((uint16                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(572))
#define MAP_GATT_UpdateMTU                               ((void                     (*) (uint16, uint16))                                                                                              ROM_BLE_JT_OFFSET(573))
#define MAP_GATT_VerifyReadPermissions                   ((bStatus_t                (*) (uint16, gattAttribute_t *, uint16))                                                                                       ROM_BLE_JT_OFFSET(574))
#define MAP_GATT_VerifyWritePermissions                  ((bStatus_t                (*) (uint16, uint8, uint16, attWriteReq_t *))                                                                      ROM_BLE_JT_OFFSET(575))
// <<BEGIN EXTERN
#define MAP_gattClientHandleConnStatusCB                 ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(576))
#define MAP_gattClientHandleTimerCB                      ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(577))
#define MAP_gattClientNotifyTxCB                         ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(578))
#define MAP_gattClientProcessMsgCB                       ((bStatus_t                (*) (uint16, attPacket_t *,uint8 *))                                                                               ROM_BLE_JT_OFFSET(579))
#define MAP_gattFindClientInfo                           ((gattClientInfo_t *       (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(580))
#define MAP_gattFindServerInfo                           ((gattServerInfo_t *       (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(581))
#define MAP_gattFindService                              ((gattService_t *          (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(582))
#define MAP_gattGetPayload                               ((uint8 *                  (*) (gattMsg_t *, uint8))                                                                                          ROM_BLE_JT_OFFSET(583))
#define MAP_gattGetServerStatus                          ((bStatus_t                (*) (uint16, gattServerInfo_t **))                                                                                 ROM_BLE_JT_OFFSET(584))
#define MAP_gattNotifyEvent                              ((bStatus_t                (*) (uint8, uint16, uint8, uint8, gattMsg_t *))                                                                    ROM_BLE_JT_OFFSET(585))
#define MAP_gattParseReq                                 ((gattParseReq_t           (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(586))
#define MAP_gattProcessExchangeMTUReq                    ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(587))
#define MAP_gattProcessExecuteWriteReq                   ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(588))
#define MAP_gattProcessFindByTypeValueReq                ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(589))
#define MAP_gattProcessFindInfoReq                       ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(590))
#define MAP_gattProcessReadByGrpTypeReq                  ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(591))
#define MAP_gattProcessReadByTypeReq                     ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(592))
#define MAP_gattProcessReadReq                           ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(593))
#define MAP_gattProcessReq                               ((gattProcessReq_t         (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(594))
#define MAP_gattProcessRxData                            ((void                     (*) (l2capDataEvent_t *))                                                                                          ROM_BLE_JT_OFFSET(595))
#define MAP_gattProcessOSALMsg                           ((void                     (*) (osal_event_hdr_t *))                                                                                          ROM_BLE_JT_OFFSET(596))
#define MAP_gattProcessWriteReq                          ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(597))
#define MAP_gattProcessReadMultiReq                      ((bStatus_t                (*) (uint16, attMsg_t *))                                                                                          ROM_BLE_JT_OFFSET(598))
#define MAP_gattRegisterClient                           ((void                     (*) (gattProcessMsg_t))                                                                                            ROM_BLE_JT_OFFSET(599))
#define MAP_gattRegisterServer                           ((void                     (*) (gattProcessMsg_t))                                                                                            ROM_BLE_JT_OFFSET(600))
#define MAP_gattResetServerInfo                          ((void                     (*) (gattServerInfo_t *))                                                                                          ROM_BLE_JT_OFFSET(601))
#define MAP_gattSendFlowCtrlEvt                          ((void                     (*) (uint16, uint8, uint8))                                                                                        ROM_BLE_JT_OFFSET(602))
#define MAP_gattServerHandleConnStatusCB                 ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(603))
#define MAP_gattServerHandleTimerCB                      ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(604))
#define MAP_gattServerNotifyTxCB                         ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(605))
#define MAP_gattServerProcessMsgCB                       ((bStatus_t                (*) (uint16, attPacket_t *, uint8 *))                                                                              ROM_BLE_JT_OFFSET(606))
#define MAP_gattServerStartTimer                         ((void                     (*) (uint8 *, uint16, uint8 *))                                                                                    ROM_BLE_JT_OFFSET(607))
#define MAP_gattServiceLastHandle                        ((uint16                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(608))
#define MAP_gattStartTimer                               ((void                     (*) (pfnCbTimer_t, uint8 *, uint16, uint8 *))                                                                      ROM_BLE_JT_OFFSET(609))
#define MAP_gattStopTimer                                ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(610))
#define MAP_gattStoreServerInfo                          ((void                     (*) (gattServerInfo_t *, uint8))                                                                                   ROM_BLE_JT_OFFSET(611))
#define MAP_gattClientStartTimer                         ((void                     (*) (uint8 *, uint16, uint8 *))                                                                                    ROM_BLE_JT_OFFSET(612))
#define MAP_gattProcessMultiReqs                         ((uint8                    (*) (uint16, gattClientInfo_t *,uint8, gattMsg_t *))                                                               ROM_BLE_JT_OFFSET(613))
#define MAP_gattResetClientInfo                          ((void                     (*) (gattClientInfo_t *))                                                                                          ROM_BLE_JT_OFFSET(614))
#define MAP_gattProcessFindInfo                          ((uint8                    (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(615))
#define MAP_gattProcessFindByTypeValue                   ((uint8                    (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(616))
#define MAP_gattProcessReadByType                        ((uint8                    (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(617))
#define MAP_gattProcessReadLong                          ((uint8                    (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(618))
#define MAP_gattProcessReadByGrpType                     ((uint8                    (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(619))
#define MAP_gattProcessReliableWrites                    ((bStatus_t                (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(620))
#define MAP_gattProcessWriteLong                         ((bStatus_t                (*) (gattClientInfo_t *,uint8, gattMsg_t *))                                                                       ROM_BLE_JT_OFFSET(621))
#define MAP_gattWrite                                    ((bStatus_t                (*) (uint16, attWriteReq_t *, uint8))                                                                              ROM_BLE_JT_OFFSET(622))
#define MAP_gattWriteLong                                ((bStatus_t                (*) (uint16, attPrepareWriteReq_t *, uint8))                                                                       ROM_BLE_JT_OFFSET(623))
#define MAP_gattPrepareWriteReq                          ((bStatus_t                (*) (uint16, uint16,uint16, uint16, uint8 *))                                                                      ROM_BLE_JT_OFFSET(624))
#define MAP_gattStoreClientInfo                          ((void                     (*) (gattClientInfo_t *, gattMsg_t *,uint8, gattParseRsp_t, uint8))                                                ROM_BLE_JT_OFFSET(625))
#define MAP_gattReadByGrpType                            ((bStatus_t                (*) (uint16, attReadByGrpTypeReq_t *, uint8))                                                                      ROM_BLE_JT_OFFSET(626))
#define MAP_gattFindByTypeValue                          ((bStatus_t                (*) (uint16, gattFindByTypeValueReq_t *))                                                                          ROM_BLE_JT_OFFSET(627))
#define MAP_gattReadByType                               ((bStatus_t                (*) (uint16, attReadByTypeReq_t *,uint8, uint8))                                                                   ROM_BLE_JT_OFFSET(628))
#define MAP_gattFindInfo                                 ((bStatus_t                (*) (uint16, attFindInfoReq_t *, uint8))                                                                           ROM_BLE_JT_OFFSET(629))
#define MAP_gattRead                                     ((bStatus_t                (*) (uint16, attReadReq_t *, uint8))                                                                               ROM_BLE_JT_OFFSET(630))
#define MAP_gattReadLong                                 ((bStatus_t                (*) (uint16, attReadBlobReq_t *, uint8))                                                                           ROM_BLE_JT_OFFSET(631))
#define MAP_gattGetClientStatus                          ((bStatus_t                (*) (uint16, gattClientInfo_t **))                                                                                 ROM_BLE_JT_OFFSET(632))
// END EXTERN>>
// GATT Server App
#define MAP_gattServApp_buildReadByTypeRsp               ((bStatus_t                (*) (uint16, uint8 *, uint16, uint16))                                                                             ROM_BLE_JT_OFFSET(633))
// <<INSERT:#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )>>
#define MAP_gattServApp_ClearPrepareWriteQ               ((void                     (*) (prepareWrites_t *))                                                                                           ROM_BLE_JT_OFFSET(634))
#define MAP_gattServApp_EnqueuePrepareWriteReq           ((bStatus_t                (*) (uint16, attPrepareWriteReq_t *))                                                                              ROM_BLE_JT_OFFSET(635))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
#define MAP_gattServApp_EnqueueReTx                     ((bStatus_t                 (*) (uint16, uint8, gattMsg_t *))                                                                                  ROM_BLE_JT_OFFSET(636))
#define MAP_gattServApp_FindAuthorizeAttrCB              ((pfnGATTAuthorizeAttrCB_t (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(637))
#define MAP_gattServApp_FindPrepareWriteQ                ((prepareWrites_t *        (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(638))
// <<BEGIN EXTERN
#define MAP_gattServApp_FindServiceCBs                   ((gattServiceCBs_t *       (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(639))
// END EXTERN>>
#define MAP_gattServApp_IsWriteLong                      ((uint8                    (*) (attExecuteWriteReq_t *, prepareWrites_t *))                                                                   ROM_BLE_JT_OFFSET(640))
// <<BEGIN EXTERN
// <<INSERT:#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )>>
#define MAP_gattServApp_ProcessExchangeMTUReq            ((bStatus_t                (*) (gattMsgEvent_t *))                                                                                            ROM_BLE_JT_OFFSET(641))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
#define MAP_gattServApp_ProcessExecuteWriteReq           ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *))                                                                                  ROM_BLE_JT_OFFSET(642))
#define MAP_gattServApp_ProcessFindByTypeValueReq        ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *))                                                                                  ROM_BLE_JT_OFFSET(643))
#define MAP_gattServApp_ProcessPrepareWriteReq           ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *, uint8 *))                                                                         ROM_BLE_JT_OFFSET(644))
// END EXTERN>>
// <<INSERT:#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )>>
#define MAP_GATTServApp_ReadAttr                         ((uint8                    (*) (uint16, gattAttribute_t *, uint16, uint8 *, uint16 *, uint16, uint16, uint8))                                 ROM_BLE_JT_OFFSET(645))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
// <<BEGIN EXTERN
#define MAP_gattServApp_ProcessReadBlobReq               ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *))                                                                                  ROM_BLE_JT_OFFSET(646))
#define MAP_gattServApp_ProcessReadByTypeReq             ((bStatus_t                (*) (gattMsgEvent_t *, bStatus_t, uint16 *))                                                                       ROM_BLE_JT_OFFSET(647))
#define MAP_gattServApp_ProcessReadByGrpTypeReq          ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *))                                                                                  ROM_BLE_JT_OFFSET(648))
#define MAP_gattServApp_ProcessReadMultiReq              ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *))                                                                                  ROM_BLE_JT_OFFSET(649))
#define MAP_gattServApp_ProcessReadReq                   ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *))                                                                                  ROM_BLE_JT_OFFSET(650))
// END EXTERN>>
// <<INSERT:#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )>>
#define MAP_gattServApp_ProcessReliableWrites            ((bStatus_t                (*) (gattMsgEvent_t *,  prepareWrites_t *, uint16 *))                                                              ROM_BLE_JT_OFFSET(651))
#define MAP_GATTServApp_WriteAttr                        ((uint8                    (*) (uint16, uint16, uint8 *, uint16, uint16, uint8))                                                              ROM_BLE_JT_OFFSET(652))
#define MAP_gattServApp_ProcessWriteLong                 ((bStatus_t                (*) (gattMsgEvent_t *, prepareWrites_t *, uint16 *))                                                               ROM_BLE_JT_OFFSET(653))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
// <<BEGIN EXTERN
#define MAP_gattServApp_ProcessWriteReq                  ((bStatus_t                (*) (gattMsgEvent_t *, uint16 *, uint8 *))                                                                         ROM_BLE_JT_OFFSET(654))
// END EXTERN>>
// GAP API
#define MAP_gap_CentConnRegister                         ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(655))
#define MAP_GAP_DeviceInit                               ((bStatus_t                (*) (uint8, uint8, GAP_Addr_Modes_t, uint8 *))                                                                     ROM_BLE_JT_OFFSET(656))
#define MAP_GAP_GetDevAddress                            ((uint8 *                  (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(657))
#define MAP_GAP_GetIRK                                   ((uint8 *                  (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(658))
#define MAP_GAP_GetParamValue                            ((uint16                   (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(659))
#define MAP_GapConfig_SetParameter                       ((status_t                 (*) (Gap_configParamIds_t, void *))                                                                                ROM_BLE_JT_OFFSET(660))
#define MAP_gapConnEvtNoticeCB                           ((void                     (*) (Gap_ConnEventRpt_t *))                                                                                        ROM_BLE_JT_OFFSET(661))
#define MAP_GAP_NumActiveConnections                     ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(662))
#define MAP_gap_ParamsInit                               ((bStatus_t                (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(663))
#define MAP_GAP_PasscodeUpdate                           ((bStatus_t                (*) (uint32, uint16))                                                                                              ROM_BLE_JT_OFFSET(664))
#define MAP_gap_PeriConnRegister                         ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(665))
#define MAP_gap_PrivacyInit                              ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(666))
#define MAP_gap_SecParamsInit                            ((void                     (*) (uint8 *, uint32 *))                                                                                           ROM_BLE_JT_OFFSET(667))
#define MAP_GAP_UpdateLinkParamReqReply                  ((bStatus_t                (*) (gapUpdateLinkParamReqReply_t *))                                                                              ROM_BLE_JT_OFFSET(668))
#define MAP_GAP_UpdateResolvingList                      ((void                     (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(669))
// GAP Task
#define MAP_gapProcessBLEEvents                          ((uint8                    (*) (osal_event_hdr_t *))                                                                                          ROM_BLE_JT_OFFSET(670))
#define MAP_gapProcessCommandStatusEvt                   ((uint8                    (*) (hciEvt_CommandStatus_t *))                                                                                    ROM_BLE_JT_OFFSET(671))
#define MAP_gapProcessConnEvt                            ((uint8                    (*) (uint16, uint16, hciEvt_CommandStatus_t *))                                                                    ROM_BLE_JT_OFFSET(672))
#define MAP_gapProcessHCICmdCompleteEvt                  ((uint8                    (*) (hciEvt_CmdComplete_t *))                                                                                      ROM_BLE_JT_OFFSET(673))
#define MAP_gapProcessOSALMsg                            ((uint8                    (*) (osal_event_hdr_t *))                                                                                          ROM_BLE_JT_OFFSET(674))
// GAP Config Mgr
#define MAP_gapGetDevAddressMode                         ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(675))
#define MAP_gapGetSignCounter                            ((uint32                   (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(676))
#define MAP_gapGetState                                  ((gapLLParamsStates_t      (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(677))
#define MAP_gapGetSRK                                    ((uint8 *                  (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(678))
#define MAP_gapHost2CtrlOwnAddrType                      ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(679))
#define MAP_gapIncSignCounter                            ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(680))
#define MAP_gapReadBufSizeCmdStatus                      ((uint8                    (*) (uint8 *))                                                                                                     ROM_BLE_JT_OFFSET(681))
#define MAP_gapSendDeviceInitDoneEvent                   ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(682))
// GAP Link Mgr
#define MAP_gapCentProcessConnEvt                        ((uint8                    (*) (uint16, hciEvt_CommandStatus_t *))                                                                            ROM_BLE_JT_OFFSET(683))
#define MAP_gapCentProcessConnUpdateCompleteEvt          ((void                     (*) (hciEvt_BLEConnUpdateComplete_t *))                                                                            ROM_BLE_JT_OFFSET(684))
#define MAP_gapCentProcessSignalEvt                      ((uint8                    (*) (l2capSignalEvent_t *))                                                                                        ROM_BLE_JT_OFFSET(685))
#define MAP_disconnectNext                               ((bStatus_t                (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(686))
#define MAP_gapFreeAuthLink                              ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(687))
#define MAP_gapPairingCompleteCB                         ((void                     (*) (uint8, uint8, uint16, uint8, smSecurityInfo_t *, smSecurityInfo_t *, smIdentityInfo_t *, smSigningInfo_t  *)) ROM_BLE_JT_OFFSET(688))
#define MAP_gapPasskeyNeededCB                           ((void                     (*) (uint16, uint8, uint32))                                                                                       ROM_BLE_JT_OFFSET(689))
#define MAP_gapProcessConnectionCompleteEvt              ((void                     (*) (uint8, hciEvt_BLEConnComplete_u *))                                                                           ROM_BLE_JT_OFFSET(690))
#define MAP_gapProcessDisconnectCompleteEvt              ((void                     (*) (hciEvt_DisconnComplete_t *))                                                                                  ROM_BLE_JT_OFFSET(691))
#define MAP_gapProcessRemoteConnParamReqEvt              ((void                     (*) (hciEvt_BLERemoteConnParamReq_t *, uint8))                                                                     ROM_BLE_JT_OFFSET(692))
#define MAP_gapRegisterCentralConn                       ((void                     (*) (gapCentralConnCBs_t *))                                                                                       ROM_BLE_JT_OFFSET(693))
#define MAP_gapRegisterPeripheralConn                    ((void                     (*) (gapPeripheralConnCBs_t *))                                                                                    ROM_BLE_JT_OFFSET(694))
#define MAP_gapSendBondCompleteEvent                     ((void                     (*) (uint8, uint16))                                                                                               ROM_BLE_JT_OFFSET(695))
#define MAP_gapSendLinkUpdateEvent                       ((void                     (*) (uint8, uint16, uint16, uint16, uint16))                                                                       ROM_BLE_JT_OFFSET(696))
#define MAP_gapSendPairingReqEvent                       ((void                     (*) (uint8, uint16, uint8, uint8, uint8, uint8, keyDist_t))                                                        ROM_BLE_JT_OFFSET(697))
#define MAP_gapSendSignUpdateEvent                       ((void                     (*) (uint8, uint8, uint8 *, uint32))                                                                               ROM_BLE_JT_OFFSET(698))
#define MAP_gapSendSlaveSecurityReqEvent                 ((void                     (*) (uint8, uint16, uint8 *, uint8))                                                                               ROM_BLE_JT_OFFSET(699))
#define MAP_gapUpdateConnSignCounter                     ((void                     (*) (uint16, uint32))                                                                                              ROM_BLE_JT_OFFSET(700))
#define MAP_sendAuthEvent                                ((void                     (*) (uint8, uint16, uint8, smSecurityInfo_t *))                                                                    ROM_BLE_JT_OFFSET(701))
#define MAP_sendEstLinkEvent                             ((void                     (*) (uint8, uint8, uint8, uint8 *, uint16, uint8, uint16, uint16, uint16, uint16))                                 ROM_BLE_JT_OFFSET(702))
#define MAP_sendTerminateEvent                           ((void                     (*) (uint8, uint8, uint16, uint8))                                                                                 ROM_BLE_JT_OFFSET(703))
// GAP Dev Mgr
#define MAP_gapClrState                                  ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(704))
#define MAP_gapFindADType                                ((uint8 *                  (*) (uint8, uint8 *, uint16, uint8 *))                                                                             ROM_BLE_JT_OFFSET(705))
#define MAP_gapIsAdvertising                             ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(706))
#define MAP_gapIsInitiating                              ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(707))
#define MAP_gapIsScanning                                ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(708))
#define MAP_gapSetState                                  ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(709))
#define MAP_gapValidADType                               ((uint8                    (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(710))
// GAP Peripheral Link Mgr
#define MAP_gapL2capConnParamUpdateReq                   ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(711))
#define MAP_gapPeriProcessConnUpdateCmdStatus            ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(712))
#define MAP_gapPeriProcessConnUpdateCompleteEvt          ((void                     (*) (hciEvt_BLEConnUpdateComplete_t *))                                                                            ROM_BLE_JT_OFFSET(713))
#define MAP_gapPeriProcessSignalEvt                      ((uint8                    (*) (l2capSignalEvent_t *))                                                                                        ROM_BLE_JT_OFFSET(714))
#define MAP_gapPeriProcessConnEvt                        ((uint8                    (*) (uint16, hciEvt_CommandStatus_t *))                                                                            ROM_BLE_JT_OFFSET(715))
// SM API
// <<INSERT:#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )>>
#define MAP_SM_dhKeyCB                                   ((void                          (*) (hciEvt_BLEGenDHKeyComplete_t *))                                                                         ROM_BLE_JT_OFFSET(716))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
#define MAP_SM_GenerateAuthenSig                         ((bStatus_t                (*) (uint8 *, uint16, uint8 *, uint16))                                                                            ROM_BLE_JT_OFFSET(717))
// <<INSERT:#if ( ( HOST_CONFIG & CENTRAL_CFG ) || ( HOST_CONFIG & PERIPHERAL_CFG) )>>
#define MAP_SM_p256KeyCB                                 ((void                          (*) (hciEvt_BLEReadP256PublicKeyComplete_t *, uint8 *))                                                       ROM_BLE_JT_OFFSET(718))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
#define MAP_SM_PasskeyUpdate                             ((bStatus_t                (*) (uint8 *, uint16))                                                                                             ROM_BLE_JT_OFFSET(719))
#define MAP_SM_StartPairing                              ((bStatus_t                (*) (uint8, uint8, uint16, smLinkSecurityReq_t *))                                                                 ROM_BLE_JT_OFFSET(720))
#define MAP_SM_VerifyAuthenSig                           ((bStatus_t                (*) (uint16, uint8, uint8 *, uint16, uint8 *))                                                                     ROM_BLE_JT_OFFSET(721))
#define MAP_SM_ResponderInit                             ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(722))
#define MAP_SM_InitiatorInit                             ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(723))
#define MAP_SM_StartEncryption                           ((bStatus_t                (*) (uint16, uint8 *, uint16, uint8 *, uint8))                                                                     ROM_BLE_JT_OFFSET(724))
// SM Task
// <<BEGIN EXTERN
#define MAP_smProcessHCIBLEEventCode                     ((uint8                    (*) (hciEvt_CmdComplete_t *))                                                                                      ROM_BLE_JT_OFFSET(725))
#define MAP_smProcessHCIBLEMetaEventCode                 ((uint8                    (*) (hciEvt_CmdComplete_t *))                                                                                      ROM_BLE_JT_OFFSET(726))
#define MAP_smProcessOSALMsg                             ((uint8                    (*) (osal_event_hdr_t *))                                                                                          ROM_BLE_JT_OFFSET(727))
// SM Manager
#define MAP_generate_subkey                              ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(728))
#define MAP_leftshift_onebit                             ((void                     (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(729))
#define MAP_padding                                      ((void                     (*) (uint8 *, uint8 *, uint8))                                                                                     ROM_BLE_JT_OFFSET(730))
#define MAP_smAuthReqToUint8                             ((uint8                    (*) (authReq_t *))                                                                                                 ROM_BLE_JT_OFFSET(731))
#define MAP_smEncrypt                                    ((bStatus_t                (*) (sm_Encrypt_t *))                                                                                              ROM_BLE_JT_OFFSET(732))
#define MAP_smEncryptLocal                               ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(733))
#define MAP_smGenerateRandBuf                            ((void                     (*) (uint8 *, uint8))                                                                                              ROM_BLE_JT_OFFSET(734))
#define MAP_smStartRspTimer                              ((void                     (*) (uint16))                                                                                                      ROM_BLE_JT_OFFSET(735))
#define MAP_smUint8ToAuthReq                             ((void                     (*) (authReq_t *, uint8))                                                                                          ROM_BLE_JT_OFFSET(736))
#define MAP_sm_c1new                                     ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *, uint8 *, uint8, uint8 *, uint8, uint8 *, uint8 *))                                 ROM_BLE_JT_OFFSET(737))
#define MAP_sm_CMAC                                      ((bStatus_t                (*) (uint8 *, uint8 *, uint16, uint8 *, uint8))                                                                    ROM_BLE_JT_OFFSET(738))
#define MAP_sm_f4                                        ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *, uint8, uint8 *))                                                                   ROM_BLE_JT_OFFSET(739))
#define MAP_sm_f5                                        ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *, uint8 *, uint8 *, uint8 *))                                                        ROM_BLE_JT_OFFSET(740))
#define MAP_sm_f6                                        ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *, uint8 *, uint8 *, uint8 *, uint8 *, uint8 *))                                      ROM_BLE_JT_OFFSET(741))
#define MAP_sm_g2                                        ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *, uint8 *, uint32 *))                                                                ROM_BLE_JT_OFFSET(742))
#define MAP_sm_s1                                        ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *, uint8 *))                                                                          ROM_BLE_JT_OFFSET(743))
#define MAP_sm_xor                                       ((void                     (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(744))
#define MAP_xor_128                                      ((void                     (*) (uint8 *, CONST uint8 *, uint8 *))                                                                             ROM_BLE_JT_OFFSET(745))
// SM Pairing Manager
#define MAP_smDetermineIOCaps                            ((bStatus_t                (*) (uint8, uint8))                                                                                                ROM_BLE_JT_OFFSET(746))
#define MAP_smDetermineKeySize                           ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(747))
#define MAP_smEndPairing                                 ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(748))
#define MAP_smFreePairingParams                          ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(749))
#define MAP_smF5Wrapper                                  ((uint8                    (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(750))
#define MAP_smGenerateAddrInput                          ((bStatus_t                (*) (uint8 *, uint8 *))                                                                                            ROM_BLE_JT_OFFSET(751))
#define MAP_smGenerateConfirm                            ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(752))
#define MAP_smGenerateDHKeyCheck                         ((uint8                    (*) (uint8 *, uint8 *, uint8 *, uint8))                                                                            ROM_BLE_JT_OFFSET(753))
#define MAP_smGeneratePairingReqRsp                      ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(754))
#define MAP_smGenerateRandMsg                            ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(755))
#define MAP_smGetECCKeys                                 ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(756))
#define MAP_smIncrementEccKeyRecycleCount                ((void                     (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(757))
#define MAP_smLinkCheck                                  ((void                     (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(758))
#define MAP_smNextPairingState                           ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(759))
#define MAP_smOobSCAuthentication                        ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(760))
#define MAP_smPairingSendEncInfo                         ((void                     (*) (uint16, uint8 *))                                                                                             ROM_BLE_JT_OFFSET(761))
#define MAP_smPairingSendIdentityAddrInfo                ((void                     (*) (uint16, uint8, uint8 *))                                                                                      ROM_BLE_JT_OFFSET(762))
#define MAP_smPairingSendIdentityInfo                    ((void                     (*) (uint16, uint8 *))                                                                                             ROM_BLE_JT_OFFSET(763))
#define MAP_smPairingSendMasterID                        ((void                     (*) (uint16, uint16, uint8 *))                                                                                     ROM_BLE_JT_OFFSET(764))
#define MAP_smPairingSendSigningInfo                     ((void                     (*) (uint16, uint8 *))                                                                                             ROM_BLE_JT_OFFSET(765))
#define MAP_smProcessDataMsg                             ((void                     (*) (l2capDataEvent_t *))                                                                                          ROM_BLE_JT_OFFSET(766))
#define MAP_smProcessEncryptChange                       ((uint8                    (*) (uint16, uint8))                                                                                               ROM_BLE_JT_OFFSET(767))
#define MAP_smProcessPairingReq                          ((void                     (*) (linkDBItem_t *, gapPairingReq_t *))                                                                           ROM_BLE_JT_OFFSET(768))
#define MAP_smRegisterResponder                          ((void                     (*) (smResponderCBs_t *))                                                                                          ROM_BLE_JT_OFFSET(769))
#define MAP_smSavePairInfo                               ((bStatus_t                (*) (smpPairingReq_t *))                                                                                           ROM_BLE_JT_OFFSET(770))
#define MAP_smSaveRemotePublicKeys                       ((void                     (*) (smpPairingPublicKey_t *))                                                                                     ROM_BLE_JT_OFFSET(771))
#define MAP_smSendDHKeyCheck                             ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(772))
#define MAP_smSendFailAndEnd                             ((bStatus_t                (*) (uint16, smpPairingFailed_t *))                                                                                ROM_BLE_JT_OFFSET(773))
#define MAP_smSendFailureEvt                             ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(774))
#define MAP_smSetPairingReqRsp                           ((void                     (*) (smpPairingReq_t *))                                                                                           ROM_BLE_JT_OFFSET(775))
#define MAP_smSendPublicKeys                             ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(776))
#define MAP_smStartEncryption                            ((bStatus_t                (*) (uint16, uint8 *, uint16, uint8 *, uint8))                                                                     ROM_BLE_JT_OFFSET(777))
#define MAP_smTimedOut                                   ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(778))
#define MAP_sm_allocateSCParameters                      ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(779))
#define MAP_sm_computeDHKey                              ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(780))
#define MAP_sm_c1                                        ((bStatus_t                (*) (uint8 *, uint8 *, uint8 *))                                                                                   ROM_BLE_JT_OFFSET(781))
#define MAP_smpProcessIncoming                           ((uint8                    (*) (uint16, uint8, smpMsgs_t *))                                                                                  ROM_BLE_JT_OFFSET(782))
// SM Response Pairing Manager
#define MAP_smFinishPublicKeyExchange                    ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(783))
#define MAP_smResponderAuthStageTwo                      ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(784))
#define MAP_smpResponderProcessEncryptionInformation     ((uint8                    (*) (smpEncInfo_t *))                                                                                              ROM_BLE_JT_OFFSET(785))
#define MAP_smpResponderProcessIdentityAddrInfo          ((uint8                    (*) (smpIdentityAddrInfo_t *))                                                                                     ROM_BLE_JT_OFFSET(786))
#define MAP_smpResponderProcessIdentityInfo              ((uint8                    (*) (smpIdentityInfo_t *))                                                                                         ROM_BLE_JT_OFFSET(787))
#define MAP_smpResponderProcessMasterID                  ((uint8                    (*) (smpMasterID_t *))                                                                                             ROM_BLE_JT_OFFSET(788))
#define MAP_smpResponderProcessPairingConfirm            ((uint8                    (*) (smpPairingConfirm_t *))                                                                                       ROM_BLE_JT_OFFSET(789))
#define MAP_smpResponderProcessPairingDHKeyCheck         ((uint8                    (*) (smpPairingDHKeyCheck_t *))                                                                                    ROM_BLE_JT_OFFSET(790))
#define MAP_smpResponderProcessPairingPublicKey          ((uint8                    (*) (smpPairingPublicKey_t *))                                                                                     ROM_BLE_JT_OFFSET(791))
#define MAP_smpResponderProcessPairingRandom             ((uint8                    (*) (smpPairingRandom_t *))                                                                                        ROM_BLE_JT_OFFSET(792))
#define MAP_smpResponderProcessPairingReq                ((uint8                    (*) (smpPairingReq_t *))                                                                                           ROM_BLE_JT_OFFSET(793))
#define MAP_smpResponderProcessSigningInfo               ((uint8                    (*) (smpSigningInfo_t *))                                                                                          ROM_BLE_JT_OFFSET(794))
// SM Protocol
#define MAP_smpBuildEncInfo                              ((bStatus_t                (*) (smpEncInfo_t *, uint8 *))                                                                                     ROM_BLE_JT_OFFSET(795))
#define MAP_smpBuildIdentityAddrInfo                     ((bStatus_t                (*) (smpIdentityAddrInfo_t *, uint8 *))                                                                            ROM_BLE_JT_OFFSET(796))
#define MAP_smpBuildIdentityInfo                         ((bStatus_t                (*) (smpIdentityInfo_t *, uint8 *))                                                                                ROM_BLE_JT_OFFSET(797))
#define MAP_smpBuildMasterID                             ((bStatus_t                (*) (smpMasterID_t *, uint8 *))                                                                                    ROM_BLE_JT_OFFSET(798))
#define MAP_smpBuildPairingConfirm                       ((bStatus_t                (*) (smpPairingConfirm_t *, uint8 *))                                                                              ROM_BLE_JT_OFFSET(799))
#define MAP_smpBuildPairingDHKeyCheck                    ((bStatus_t                (*) (smpPairingDHKeyCheck_t *, uint8 *))                                                                           ROM_BLE_JT_OFFSET(800))
#define MAP_smpBuildPairingFailed                        ((bStatus_t                (*) (smpPairingFailed_t *, uint8 *))                                                                               ROM_BLE_JT_OFFSET(801))
#define MAP_smpBuildPairingPublicKey                     ((bStatus_t                (*) (smpPairingPublicKey_t *, uint8 *))                                                                            ROM_BLE_JT_OFFSET(802))
#define MAP_smpBuildPairingRandom                        ((bStatus_t                (*) (smpPairingRandom_t *, uint8 *))                                                                               ROM_BLE_JT_OFFSET(803))
#define MAP_smpBuildPairingReq                           ((bStatus_t                (*) (smpPairingReq_t *, uint8 *))                                                                                  ROM_BLE_JT_OFFSET(804))
#define MAP_smpBuildPairingReqRsp                        ((bStatus_t                (*) (uint8, smpPairingReq_t *, uint8 *))                                                                           ROM_BLE_JT_OFFSET(805))
#define MAP_smpBuildPairingRsp                           ((bStatus_t                (*) (smpPairingRsp_t *, uint8 *))                                                                                  ROM_BLE_JT_OFFSET(806))
#define MAP_smpBuildSecurityReq                          ((bStatus_t                (*) (smpSecurityReq_t *, uint8 *))                                                                                 ROM_BLE_JT_OFFSET(807))
#define MAP_smpBuildSigningInfo                          ((bStatus_t                (*) (smpSigningInfo_t *, uint8 *))                                                                                 ROM_BLE_JT_OFFSET(808))
#define MAP_smpParseEncInfo                              ((bStatus_t                (*) (uint8 *, smpEncInfo_t *))                                                                                     ROM_BLE_JT_OFFSET(809))
#define MAP_smpParseIdentityAddrInfo                     ((bStatus_t                (*) (uint8 *, smpIdentityAddrInfo_t *))                                                                            ROM_BLE_JT_OFFSET(810))
#define MAP_smpParseIdentityInfo                         ((bStatus_t                (*) (uint8 *, smpIdentityInfo_t *))                                                                                ROM_BLE_JT_OFFSET(811))
#define MAP_smpParseKeypressNoti                         ((bStatus_t                (*) (uint8 *, smpKeyPressNoti_t *))                                                                                ROM_BLE_JT_OFFSET(812))
#define MAP_smpParseMasterID                             ((bStatus_t                (*) (uint8 *, smpMasterID_t *))                                                                                    ROM_BLE_JT_OFFSET(813))
#define MAP_smpParsePairingConfirm                       ((bStatus_t                (*) (uint8 *, smpPairingConfirm_t *))                                                                              ROM_BLE_JT_OFFSET(814))
#define MAP_smpParsePairingDHKeyCheck                    ((bStatus_t                (*) (uint8 *, smpPairingDHKeyCheck_t *))                                                                           ROM_BLE_JT_OFFSET(815))
#define MAP_smpParsePairingFailed                        ((bStatus_t                (*) (uint8 *, smpPairingFailed_t *))                                                                               ROM_BLE_JT_OFFSET(816))
#define MAP_smpParsePairingPublicKey                     ((bStatus_t                (*) (uint8 *, smpPairingPublicKey_t *))                                                                            ROM_BLE_JT_OFFSET(817))
#define MAP_smStopRspTimer                               ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(818))
#define MAP_smpParsePairingRandom                        ((bStatus_t                (*) (uint8 *, smpPairingRandom_t *))                                                                               ROM_BLE_JT_OFFSET(819))
#define MAP_smpParsePairingReq                           ((bStatus_t                (*) (uint8 *, smpPairingReq_t *))                                                                                  ROM_BLE_JT_OFFSET(820))
#define MAP_smpParseSecurityReq                          ((bStatus_t                (*) (uint8 *, smpSecurityReq_t *))                                                                                 ROM_BLE_JT_OFFSET(821))
#define MAP_smpParseSigningInfo                          ((bStatus_t                (*) (uint8 *, smpSigningInfo_t *))                                                                                 ROM_BLE_JT_OFFSET(822))
#define MAP_smSendSMMsg                                  ((bStatus_t                (*) (uint16 connHandle, uint8, smpMsgs_t *, pfnSMBuildCmd_t))                                                      ROM_BLE_JT_OFFSET(823))
#define MAP_smpInitiatorProcessPairingRsp                ((uint8                    (*) (smpPairingRsp_t *))                                                                                           ROM_BLE_JT_OFFSET(824))
#define MAP_smpInitiatorProcessPairingPubKey             ((uint8                    (*) (smpPairingPublicKey_t *))                                                                                     ROM_BLE_JT_OFFSET(825))
#define MAP_smpInitiatorProcessPairingDHKeyCheck         ((uint8                    (*) (smpPairingDHKeyCheck_t *))                                                                                    ROM_BLE_JT_OFFSET(826))
#define MAP_smpInitiatorProcessPairingConfirm            ((uint8                    (*) (smpPairingConfirm_t *))                                                                                       ROM_BLE_JT_OFFSET(827))
// <<INSERT:#if ( HOST_CONFIG & CENTRAL_CFG )>>
#define MAP_smpInitiatorProcessPairingRandom             ((uint8                    (*) (smpPairingRandom_t *, uint16))                                                                                ROM_BLE_JT_OFFSET(828))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
#define MAP_smpInitiatorProcessEncryptionInformation     ((uint8                    (*) (smpEncInfo_t *))                                                                                              ROM_BLE_JT_OFFSET(829))
#define MAP_smpInitiatorProcessMasterID                  ((uint8                    (*) (smpMasterID_t *))                                                                                             ROM_BLE_JT_OFFSET(830))
#define MAP_smpInitiatorProcessIdentityInfo              ((uint8                    (*) (smpIdentityInfo_t *))                                                                                         ROM_BLE_JT_OFFSET(831))
#define MAP_smpInitiatorProcessIdentityAddrInfo          ((uint8                    (*) (smpIdentityAddrInfo_t *))                                                                                     ROM_BLE_JT_OFFSET(832))
#define MAP_smpInitiatorProcessSigningInfo               ((uint8                    (*) (smpSigningInfo_t *))                                                                                          ROM_BLE_JT_OFFSET(833))
#define MAP_smInitiatorAuthStageTwo                      ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(834))
#define MAP_setupInitiatorKeys                           ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(835))
#define MAP_smInitiatorSendNextKeyInfo                   ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(836))
#define MAP_smpResponderProcessIncoming                  ((uint8                    (*) (linkDBItem_t *, uint8, smpMsgs_t *))                                                                          ROM_BLE_JT_OFFSET(837))
#define MAP_smResponderSendNextKeyInfo                   ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(838))
#define MAP_smpResponderSendPairRspEvent                 ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(839))
// <<INSERT:#if ( HOST_CONFIG & PERIPHERAL_CFG )>>
#define MAP_smResponderProcessLTKReq                     ((uint8                    (*) (uint16, uint8 *, uint16))                                                                                     ROM_BLE_JT_OFFSET(840))
// <<INSERT:#else //>>
// <<INSERT:  (uint32)ROM_Spinlock,>>
// <<INSERT:#endif>>
#define MAP_smRegisterInitiator                          ((void                     (*) (smInitiatorCBs_t *))                                                                                          ROM_BLE_JT_OFFSET(841))
#define MAP_smEncLTK                                     ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(842))
#define MAP_smpInitiatorProcessIncoming                  ((uint8                    (*) (linkDBItem_t *, uint8, smpMsgs_t *))                                                                          ROM_BLE_JT_OFFSET(843))
// END EXTERN>>
// GAP SCAN
#define MAP_gapScan_init                                 ((status_t                 (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(844))
// <<BEGIN EXTERN
#define MAP_gapScan_filterDiscMode                       ((uint8_t                  (*) (uint8_t*, uint16))                                                                                            ROM_BLE_JT_OFFSET(845))
#define MAP_gapScan_discardAdvRptSession                 ((void                     (*) (GapScan_AdvRptSession_t*))                                                                                    ROM_BLE_JT_OFFSET(846))
#define MAP_gapScan_sendSessionEndEvt                    ((void                     (*) (GapScan_AdvRptSession_t*,uint8_t))                                                                            ROM_BLE_JT_OFFSET(847))
// END EXTERN>>
#define MAP_GapScan_discardAdvReportList                 ((status_t                 (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(848))
// <<BEGIN EXTERN
#define MAP_gapScan_defragAdvRpt                         ((status_t                 (*) (GapScan_AdvRptSession_t*,uint8_t))                                                                            ROM_BLE_JT_OFFSET(849))
#define MAP_gapScan_saveRptAndNotify                     ((void                     (*) (GapScan_Evt_AdvRpt_t*))                                                                                       ROM_BLE_JT_OFFSET(850))
#define MAP_gapScan_processAdvRptCb                      ((void                     (*) (uint8_t, void*))                                                                                              ROM_BLE_JT_OFFSET(851))
#define MAP_gapScan_processStartEndCb                    ((void                     (*) (uint8_t, void*))                                                                                              ROM_BLE_JT_OFFSET(852))
#define MAP_gapScan_processErrorCb                       ((void                     (*) (uint8_t, void*))                                                                                              ROM_BLE_JT_OFFSET(853))
#define MAP_gapScan_processSessionEndEvt                 ((void                     (*) (GapScan_AdvRptSession_t*,uint8_t))                                                                            ROM_BLE_JT_OFFSET(854))
// END EXTERN>>
// GAP INIT
#define MAP_GapInit_cancelConnect                        ((uint8                    (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(855))
// <<BEGIN EXTERN
#define MAP_gapInit_connect_internal                     ((status_t                 (*) (uint8_t,uint8_t, uint8_t*,uint8_t, uint16_t))                                                                 ROM_BLE_JT_OFFSET(856))
#define MAP_gapInit_sendConnCancelledEvt                 ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(857))
#define MAP_gapInit_initiatingEnd                        ((void                     (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(858))
// END EXTERN>>
// GAP ADVERTISMENT
#define MAP_gapAdv_init                                  ((bStatus_t                (*) (void))                                                                                                        ROM_BLE_JT_OFFSET(859))
#define MAP_gapAdv_searchForBufferUse                    ((bStatus_t                (*) (uint8 *, advSet_t *))                                                                                         ROM_BLE_JT_OFFSET(860))
#define MAP_GapAdv_disable                               ((bStatus_t                (*) (uint8))                                                                                                       ROM_BLE_JT_OFFSET(861))
#define MAP_gapAdv_processRemoveSetEvt                   ((void                     (*) (uint8, aeSetParamCmd_t *))                                                                                    ROM_BLE_JT_OFFSET(862))
// <<END_COUNT>>
// <<INSERT:#else // HOST_CONFIG>>
// <<INSERT_LOOP:(uint32)ROM_Spinlock,>>
// <<INSERT:#endif // HOST_CONFIG>>
// <END TABLE - DO NOT REMOVE!>
#endif // ROM_BUILD
#endif /* ROM_FLASH_JT_H */
