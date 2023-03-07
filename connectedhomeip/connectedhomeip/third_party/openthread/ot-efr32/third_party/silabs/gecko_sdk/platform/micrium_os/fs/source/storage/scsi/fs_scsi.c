/***************************************************************************//**
 * @file
 * @brief File System - SCSI Media Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include <rtos_description.h>

#if (defined(RTOS_MODULE_FS_STORAGE_SCSI_AVAIL))

#if (!defined(RTOS_MODULE_FS_AVAIL))

#error SCSI module requires File System Storage module. Make sure it is part of your project and that \
  RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FS_SCSI_MODULE

#include  <fs/include/fs_scsi.h>
#include  <fs/source/shared/fs_utils_priv.h>
#include  <fs/source/storage/fs_blk_dev_priv.h>

#include  <fs_storage_cfg.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_utils.h>
#include  <common/include/rtos_path.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/lib/lib_str_priv.h>
#include  <common/source/logging/logging_priv.h>
#include  <common/source/platform_mgr/platform_mgr_priv.h>

#include  <em_core.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                                RTOS_CFG_MODULE_FS
#define  LOG_DFLT_CH                                    (FS, DRV, SCSI)

#define  SCSI_LU_RDY_RETRY_CNT_MAX                        20u
#define  SCSI_CMD_TIMEOUT_mS                            5000u
#define  SCSI_MAX_LU_NAME_LEN                             10u

/********************************************************************************************************
 *                                               SCSI DEFINES
 *
 * Note(s) : (1) See 'SCSI Primary Commands - 3 (SPC-3)', section D.3.1 for more details about the
 *               operation codes.
 *
 *           (2) See 'SCSI Primary Commands - 3 (SBC-3)', section 4.5.6 for more details about sense
 *               key and sense code definitions.
 *
 *           (3) See 'SCSI Primary Commands - 3 (SPC-3)', section D.2 for more details about the
 *               additional sense codes.
 *
 *           (3) See 'SCSI Primary Commands - 3 (SPC-3)', section D.6 for more details about the
 *               page codes.
 *******************************************************************************************************/

//                                                                 ------------------- SCSI OPCODES -------------------
//                                                                 See Note #1.
#define  SCSI_CMD_TEST_UNIT_READY                      0x00u
#define  SCSI_CMD_REWIND                               0x01u
#define  SCSI_CMD_REZERO_UNIT                          0x01u
#define  SCSI_CMD_REQUEST_SENSE                        0x03u
#define  SCSI_CMD_FORMAT_UNIT                          0x04u
#define  SCSI_CMD_FORMAT_MEDIUM                        0x04u
#define  SCSI_CMD_FORMAT                               0x04u
#define  SCSI_CMD_READ_BLOCK_LIMITS                    0x05u
#define  SCSI_CMD_REASSIGN_BLOCKS                      0x07u
#define  SCSI_CMD_INITIALIZE_ELEMENT_STATUS            0x07u
#define  SCSI_CMD_READ_06                              0x08u
#define  SCSI_CMD_RECEIVE                              0x08u
#define  SCSI_CMD_GET_MESSAGE_06                       0x08u
#define  SCSI_CMD_WRITE_06                             0x0Au
#define  SCSI_CMD_SEND_06                              0x0Au
#define  SCSI_CMD_SEND_MESSAGE_06                      0x0Au
#define  SCSI_CMD_PRINT                                0x0Au
#define  SCSI_CMD_SEEK_06                              0x0Bu
#define  SCSI_CMD_SET_CAPACITY                         0x0Bu
#define  SCSI_CMD_SLEW_AND_PRINT                       0x0Bu
#define  SCSI_CMD_READ_REVERSE_06                      0x0Fu

#define  SCSI_CMD_WRITE_FILEMARKS_06                   0x10u
#define  SCSI_CMD_SYNCHRONIZE_BUFFER                   0x10u
#define  SCSI_CMD_SPACE_06                             0x11u
#define  SCSI_CMD_INQUIRY                              0x12u
#define  SCSI_CMD_VERIFY_06                            0x13u
#define  SCSI_CMD_RECOVER_BUFFERED_DATA                0x14u
#define  SCSI_CMD_MODE_SELECT_06                       0x15u
#define  SCSI_CMD_RESERVE_06                           0x16u
#define  SCSI_CMD_RESERVE_ELEMENT_06                   0x16u
#define  SCSI_CMD_RELEASE_06                           0x17u
#define  SCSI_CMD_RELEASE_ELEMENT_06                   0x17u
#define  SCSI_CMD_COPY                                 0x18u
#define  SCSI_CMD_ERASE_06                             0x19u
#define  SCSI_CMD_MODE_SENSE_06                        0x1Au
#define  SCSI_CMD_START_STOP_UNIT                      0x1Bu
#define  SCSI_CMD_LOAD_UNLOAD                          0x1Bu
#define  SCSI_CMD_SCAN_06                              0x1Bu
#define  SCSI_CMD_STOP_PRINT                           0x1Bu
#define  SCSI_CMD_OPEN_CLOSE_IMPORT_EXPORT_ELEMENT     0x1Bu
#define  SCSI_CMD_RECEIVE_DIAGNOSTIC_RESULTS           0x1Cu
#define  SCSI_CMD_SEND_DIAGNOSTIC                      0x1Du
#define  SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL         0x1Eu

#define  SCSI_CMD_READ_FORMAT_CAPACITIES               0x23u
#define  SCSI_CMD_SET_WINDOW                           0x24u
#define  SCSI_CMD_READ_CAPACITY_10                     0x25u
#define  SCSI_CMD_READ_CAPACITY                        0x25u
#define  SCSI_CMD_READ_CARD_CAPACITY                   0x25u
#define  SCSI_CMD_GET_WINDOW                           0x25u
#define  SCSI_CMD_READ_10                              0x28u
#define  SCSI_CMD_GET_MESSAGE_10                       0x28u
#define  SCSI_CMD_READ_GENERATION                      0x29u
#define  SCSI_CMD_WRITE_10                             0x2Au
#define  SCSI_CMD_SEND_10                              0x2Au
#define  SCSI_CMD_SEND_MESSAGE_10                      0x2Au
#define  SCSI_CMD_SEEK_10                              0x2Bu
#define  SCSI_CMD_LOCATE_10                            0x2Bu
#define  SCSI_CMD_POSITION_TO_ELEMENT                  0x2Bu
#define  SCSI_CMD_ERASE_10                             0x2Cu
#define  SCSI_CMD_READ_UPDATED_BLOCK                   0x2Du
#define  SCSI_CMD_WRITE_AND_VERIFY_10                  0x2Eu
#define  SCSI_CMD_VERIFY_10                            0x2Fu

#define  SCSI_CMD_SEARCH_DATA_HIGH_10                  0x30u
#define  SCSI_CMD_SEARCH_DATA_EQUAL_10                 0x31u
#define  SCSI_CMD_OBJECT_POSITION                      0x31u
#define  SCSI_CMD_SEARCH_DATA_LOW_10                   0x32u
#define  SCSI_CMD_SET_LIMITS_10                        0x33u
#define  SCSI_CMD_PRE_FETCH_10                         0x34u
#define  SCSI_CMD_READ_POSITION                        0x34u
#define  SCSI_CMD_GET_DATA_BUFFER_STATUS               0x34u
#define  SCSI_CMD_SYNCHRONIZE_CACHE_10                 0x35u
#define  SCSI_CMD_LOCK_UNLOCK_CACHE_10                 0x36u
#define  SCSI_CMD_READ_DEFECT_DATA_10                  0x37u
#define  SCSI_CMD_INIT_ELEMENT_STATUS_WITH_RANGE       0x37u
#define  SCSI_CMD_MEDIUM_SCAN                          0x38u
#define  SCSI_CMD_COMPARE                              0x39u
#define  SCSI_CMD_COPY_AND_VERIFY                      0x3Au
#define  SCSI_CMD_WRITE_BUFFER                         0x3Bu
#define  SCSI_CMD_READ_BUFFER                          0x3Cu
#define  SCSI_CMD_UPDATE_BLOCK                         0x3Du
#define  SCSI_CMD_READ_LONG_10                         0x3Eu
#define  SCSI_CMD_WRITE_LONG_10                        0x3Fu

#define  SCSI_CMD_CHANGE_DEFINITION                    0x40u
#define  SCSI_CMD_WRITE_SAME_10                        0x41u
#define  SCSI_CMD_READ_SUBCHANNEL                      0x42u
#define  SCSI_CMD_READ_TOC_PMA_ATIP                    0x43u
#define  SCSI_CMD_REPORT_DENSITY_SUPPORT               0x44u
#define  SCSI_CMD_READ_HEADER                          0x44u
#define  SCSI_CMD_PLAY_AUDIO_10                        0x45u
#define  SCSI_CMD_GET_CONFIGURATION                    0x46u
#define  SCSI_CMD_PLAY_AUDIO_MSF                       0x47u
#define  SCSI_CMD_GET_EVENT_STATUS_NOTIFICATION        0x4Au
#define  SCSI_CMD_PAUSE_RESUME                         0x4Bu
#define  SCSI_CMD_LOG_SELECT                           0x4Cu
#define  SCSI_CMD_LOG_SENSE                            0x4Du
#define  SCSI_CMD_STOP_PLAY_SCAN                       0x4Eu

#define  SCSI_CMD_XDWRITE_10                           0x50u
#define  SCSI_CMD_XPWRITE_10                           0x51u
#define  SCSI_CMD_READ_DISC_INFORMATION                0x51u
#define  SCSI_CMD_XDREAD_10                            0x52u
#define  SCSI_CMD_READ_TRACK_INFORMATION               0x52u
#define  SCSI_CMD_RESERVE_TRACK                        0x53u
#define  SCSI_CMD_SEND_OPC_INFORMATION                 0x54u
#define  SCSI_CMD_MODE_SELECT_10                       0x55u
#define  SCSI_CMD_RESERVE_10                           0x56u
#define  SCSI_CMD_RESERVE_ELEMENT_10                   0x56u
#define  SCSI_CMD_RELEASE_10                           0x57u
#define  SCSI_CMD_RELEASE_ELEMENT_10                   0x57u
#define  SCSI_CMD_REPAIR_TRACK                         0x58u
#define  SCSI_CMD_MODE_SENSE_10                        0x5Au
#define  SCSI_CMD_CLOSE_TRACK_SESSION                  0x5Bu
#define  SCSI_CMD_READ_BUFFER_CAPACITY                 0x5Cu
#define  SCSI_CMD_SEND_CUE_SHEET                       0x5Du
#define  SCSI_CMD_PERSISTENT_RESERVE_IN                0x5Eu
#define  SCSI_CMD_PERSISTENT_RESERVE_OUT               0x5Fu

#define  SCSI_CMD_EXTENDED_CDB                         0x7Eu
#define  SCSI_CMD_VARIABLE_LENGTH_CDB                  0x7Fu

#define  SCSI_CMD_XDWRITE_EXTENDED_16                  0x80u
#define  SCSI_CMD_WRITE_FILEMARKS_16                   0x80u
#define  SCSI_CMD_REBUILD_16                           0x81u
#define  SCSI_CMD_READ_REVERSE_16                      0x81u
#define  SCSI_CMD_REGENERATE_16                        0x82u
#define  SCSI_CMD_EXTENDED_COPY                        0x83u
#define  SCSI_CMD_RECEIVE_COPY_RESULTS                 0x84u
#define  SCSI_CMD_ATA_COMMAND_PASS_THROUGH_16          0x85u
#define  SCSI_CMD_ACCESS_CONTROL_IN                    0x86u
#define  SCSI_CMD_ACCESS_CONTROL_OUT                   0x87u
#define  SCSI_CMD_READ_16                              0x88u
#define  SCSI_CMD_WRITE_16                             0x8Au
#define  SCSI_CMD_ORWRITE                              0x8Bu
#define  SCSI_CMD_READ_ATTRIBUTE                       0x8Cu
#define  SCSI_CMD_WRITE_ATTRIBUTE                      0x8Du
#define  SCSI_CMD_WRITE_AND_VERIFY_16                  0x8Eu
#define  SCSI_CMD_VERIFY_16                            0x8Fu

#define  SCSI_CMD_PREFETCH_16                          0x90u
#define  SCSI_CMD_SYNCHRONIZE_CACHE_16                 0x91u
#define  SCSI_CMD_SPACE_16                             0x91u
#define  SCSI_CMD_LOCK_UNLOCK_CACHE_16                 0x92u
#define  SCSI_CMD_LOCATE_16                            0x92u
#define  SCSI_CMD_WRITE_SAME_16                        0x93u
#define  SCSI_CMD_ERASE_16                             0x93u
#define  SCSI_CMD_SERVICE_ACTION_IN_16                 0x9Eu
#define  SCSI_CMD_READ_CAPACITY_16                     0x9Eu
#define  SCSI_CMD_SERVICE_ACTION_OUT_16                0x9Fu

#define  SCSI_CMD_REPORT_LUNS                          0xA0u
#define  SCSI_CMD_BLANK                                0xA1u
#define  SCSI_CMD_ATA_COMMAND_PASS_THROUGH_12          0xA1u
#define  SCSI_CMD_SECURITY_PROTOCOL_IN                 0xA2u
#define  SCSI_CMD_MAINTENANCE_IN                       0xA3u
#define  SCSI_CMD_SEND_KEY                             0xA3u
#define  SCSI_CMD_MAINTENANCE_OUT                      0xA4u
#define  SCSI_CMD_REPORT_KEY                           0xA4u
#define  SCSI_CMD_MOVE_MEDIUM                          0xA5u
#define  SCSI_CMD_PLAY_AUDIO_12                        0xA5u
#define  SCSI_CMD_EXCHANGE_MEDIUM                      0xA6u
#define  SCSI_CMD_LOAD_UNLOAD_CDVD                     0xA6u
#define  SCSI_CMD_MOVE_MEDIUM_ATTACHED                 0xA7u
#define  SCSI_CMD_SET_READ_AHEAD                       0xA7u
#define  SCSI_CMD_READ_12                              0xA8u
#define  SCSI_CMD_GET_MESSAGE_12                       0xA8u
#define  SCSI_CMD_SERVICE_ACTION_OUT_12                0xA9u
#define  SCSI_CMD_WRITE_12                             0xAAu
#define  SCSI_CMD_SEND_MESSAGE_12                      0xAAu
#define  SCSI_CMD_SERVICE_ACTION_IN_12                 0xABu
#define  SCSI_CMD_ERASE_12                             0xACu
#define  SCSI_CMD_GET_PERFORMANCE                      0xACu
#define  SCSI_CMD_READ_DVD_STRUCTURE                   0xADu
#define  SCSI_CMD_WRITE_AND_VERIFY_12                  0xAEu
#define  SCSI_CMD_VERIFY_12                            0xAFu

#define  SCSI_CMD_SEARCH_DATA_HIGH_12                  0xB0u
#define  SCSI_CMD_SEARCH_DATA_EQUAL_12                 0xB1u
#define  SCSI_CMD_SEARCH_DATA_LOW_12                   0xB2u
#define  SCSI_CMD_SET_LIMITS_12                        0xB3u
#define  SCSI_CMD_READ_ELEMENT_STATUS_ATTACHED         0xB4u
#define  SCSI_CMD_SECURITY_PROTOCOL_OUT                0xB5u
#define  SCSI_CMD_REQUEST_VOLUME_ELEMENT_ADDRESS       0xB5u
#define  SCSI_CMD_SEND_VOLUME_TAG                      0xB6u
#define  SCSI_CMD_SET_STREAMING                        0xB6u
#define  SCSI_CMD_READ_DEFECT_DATA_12                  0xB7u
#define  SCSI_CMD_READ_ELEMENT_STATUS                  0xB8u
#define  SCSI_CMD_READ_CD_MSF                          0xB9u
#define  SCSI_CMD_REDUNDANCY_GROUP_IN                  0xBAu
#define  SCSI_CMD_SCAN                                 0xBAu
#define  SCSI_CMD_REDUNDANCY_GROUP_OUT                 0xBBu
#define  SCSI_CMD_SET_CD_SPEED                         0xBBu
#define  SCSI_CMD_SPARE_IN                             0xBCu
#define  SCSI_CMD_SPARE_OUT                            0xBDu
#define  SCSI_CMD_MECHANISM_STATUS                     0xBDu
#define  SCSI_CMD_VOLUME_SET_IN                        0xBEu
#define  SCSI_CMD_READ_CD                              0xBEu
#define  SCSI_CMD_VOLUME_SET_OUT                       0xBFu
#define  SCSI_CMD_SEND_DVD_STRUCTURE                   0xBFu

//                                                                 ---------------- SCSI STATUS CODES -----------------
#define  SCSI_STATUS_GOOD                              0x00u
#define  SCSI_STATUS_CHECK_CONDITION                   0x02u
#define  SCSI_STATUS_CONDITION_MET                     0x04u
#define  SCSI_STATUS_BUSY                              0x08u
#define  SCSI_STATUS_RESERVATION_CONFLICT              0x18u
#define  SCSI_STATUS_TASK_SET_FULL                     0x28u
#define  SCSI_STATUS_ACA_ACTIVE                        0x30u
#define  SCSI_STATUS_TASK_ABORTED                      0x40u

//                                                                 ----------------- SCSI SENSE KEYS ------------------
//                                                                 See Note #4.
#define  SCSI_SENSE_KEY_NO_SENSE                       0x00u
#define  SCSI_SENSE_KEY_RECOVERED_ERROR                0x01u
#define  SCSI_SENSE_KEY_NOT_RDY                        0x02u
#define  SCSI_SENSE_KEY_MEDIUM_ERROR                   0x03u
#define  SCSI_SENSE_KEY_HARDWARE_ERROR                 0x04u
#define  SCSI_SENSE_KEY_ILLEGAL_REQUEST                0x05u
#define  SCSI_SENSE_KEY_UNIT_ATTENTION                 0x06u
#define  SCSI_SENSE_KEY_DATA_PROTECT                   0x07u
#define  SCSI_SENSE_KEY_BLANK_CHECK                    0x08u
#define  SCSI_SENSE_KEY_VENDOR_SPECIFIC                0x09u
#define  SCSI_SENSE_KEY_COPY_ABORTED                   0x0Au
#define  SCSI_SENSE_KEY_ABORTED_COMMAND                0x0Bu
#define  SCSI_SENSE_KEY_VOLUME_OVERFLOW                0x0Du
#define  SCSI_SENSE_KEY_MISCOMPARE                     0x0Eu

//                                                                 ----------- SCSI ADDITIONAL SENSE CODES ------------
//                                                                 See Note #3.
#define  SCSI_ASC_NO_ADDITIONAL_SENSE_INFO             0x00u
#define  SCSI_ASC_NO_INDEX_SECTOR_SIGNAL               0x01u
#define  SCSI_ASC_NO_SEEK_COMPLETE                     0x02u
#define  SCSI_ASC_PERIPHERAL_DEV_WR_FAULT              0x03u
#define  SCSI_ASC_LOG_UNIT_NOT_RDY                     0x04u
#define  SCSI_ASC_LOG_UNIT_NOT_RESPOND_TO_SELECTION    0x05u
#define  SCSI_ASC_NO_REFERENCE_POSITION_FOUND          0x06u
#define  SCSI_ASC_MULTIPLE_PERIPHERAL_DEVS_SELECTED    0x07u
#define  SCSI_ASC_LOG_UNIT_COMMUNICATION_FAIL          0x08u
#define  SCSI_ASC_TRACK_FOLLOWING_ERR                  0x09u
#define  SCSI_ASC_ERR_LOG_OVERFLOW                     0x0Au
#define  SCSI_ASC_WARNING                              0x0Bu
#define  SCSI_ASC_WR_ERR                               0x0Cu
#define  SCSI_ASC_ERR_DETECTED_BY_THIRD_PARTY          0x0Du
#define  SCSI_ASC_INVALID_INFO_UNIT                    0x0Eu

#define  SCSI_ASC_ID_CRC_OR_ECC_ERR                    0x10u
#define  SCSI_ASC_UNRECOVERED_RD_ERR                   0x11u
#define  SCSI_ASC_ADDR_MARK_NOT_FOUND_FOR_ID           0x12u
#define  SCSI_ASC_ADDR_MARK_NOT_FOUND_FOR_DATA         0x13u
#define  SCSI_ASC_RECORDED_ENTITY_NOT_FOUND            0x14u
#define  SCSI_ASC_RANDOM_POSITIONING_ERR               0x15u
#define  SCSI_ASC_DATA_SYNCHRONIZATION_MARK_ERR        0x16u
#define  SCSI_ASC_RECOVERED_DATA_NO_ERR_CORRECT        0x17u
#define  SCSI_ASC_RECOVERED_DATA_ERR_CORRECT           0x18u
#define  SCSI_ASC_DEFECT_LIST_ERR                      0x19u
#define  SCSI_ASC_PARAMETER_LIST_LENGTH_ERR            0x1Au
#define  SCSI_ASC_SYNCHRONOUS_DATA_TRANSFER_ERR        0x1Bu
#define  SCSI_ASC_DEFECT_LIST_NOT_FOUND                0x1Cu
#define  SCSI_ASC_MISCOMPARE_DURING_VERIFY_OP          0x1Du
#define  SCSI_ASC_RECOVERED_ID_WITH_ECC_CORRECTION     0x1Eu
#define  SCSI_ASC_PARTIAL_DEFECT_LIST_TRANSFER         0x1Fu

#define  SCSI_ASC_INVALID_CMD_OP_CODE                  0x20u
#define  SCSI_ASC_LOG_BLOCK_ADDR_OUT_OF_RANGE          0x21u
#define  SCSI_ASC_ILLEGAL_FUNCTION                     0x22u
#define  SCSI_ASC_INVALID_FIELD_IN_CDB                 0x24u
#define  SCSI_ASC_LOG_UNIT_NOT_SUPPORTED               0x25u
#define  SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST      0x26u
#define  SCSI_ASC_WR_PROTECTED                         0x27u
#define  SCSI_ASC_NOT_RDY_TO_RDY_CHANGE                0x28u
#define  SCSI_ASC_POWER_ON_OR_BUS_DEV_RESET            0x29u
#define  SCSI_ASC_PARAMETERS_CHANGED                   0x2Au
#define  SCSI_ASC_CANNOT_COPY_CANNOT_DISCONNECT        0x2Bu
#define  SCSI_ASC_CMD_SEQUENCE_ERR                     0x2Cu
#define  SCSI_ASC_OVERWR_ERR_ON_UPDATE_IN_PLACE        0x2Du
#define  SCSI_ASC_INSUFFICIENT_TIME_FOR_OP             0x2Eu
#define  SCSI_ASC_CMDS_CLEARED_BY_ANOTHER_INIT         0x2Fu

#define  SCSI_ASC_INCOMPATIBLE_MEDIUM_INSTALLED        0x30u
#define  SCSI_ASC_MEDIUM_FORMAT_CORRUPTED              0x31u
#define  SCSI_ASC_NO_DEFECT_SPARE_LOCATION_AVAIL       0x32u
#define  SCSI_ASC_TAPE_LENGTH_ERR                      0x33u
#define  SCSI_ASC_ENCLOSURE_FAIL                       0x34u
#define  SCSI_ASC_ENCLOSURE_SERVICES_FAIL              0x35u
#define  SCSI_ASC_RIBBON_INK_OR_TONER_FAIL             0x36u
#define  SCSI_ASC_ROUNDED_PARAMETER                    0x37u
#define  SCSI_ASC_EVENT_STATUS_NOTIFICATION            0x38u
#define  SCSI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED      0x39u
#define  SCSI_ASC_MEDIUM_NOT_PRESENT                   0x3Au
#define  SCSI_ASC_SEQUENTIAL_POSITIONING_ERR           0x3Bu
#define  SCSI_ASC_INVALID_BITS_IN_IDENTIFY_MSG         0x3Du
#define  SCSI_ASC_LOG_UNIT_HAS_NOT_SELF_CFG_YET        0x3Eu
#define  SCSI_ASC_TARGET_OP_CONDITIONS_HAVE_CHANGED    0x3Fu

#define  SCSI_ASC_RAM_FAIL                             0x40u
#define  SCSI_ASC_DATA_PATH_FAIL                       0x41u
#define  SCSI_ASC_POWER_ON_SELF_TEST_FAIL              0x42u
#define  SCSI_ASC_MSG_ERR                              0x43u
#define  SCSI_ASC_INTERNAL_TARGET_FAIL                 0x44u
#define  SCSI_ASC_SELECT_OR_RESELECT_FAIL              0x45u
#define  SCSI_ASC_UNSUCCESSFUL_SOFT_RESET              0x46u
#define  SCSI_ASC_SCSI_PARITY_ERR                      0x47u
#define  SCSI_ASC_INIT_DETECTED_ERR_MSG_RECEIVED       0x48u
#define  SCSI_ASC_INVALID_MSG_ERR                      0x49u
#define  SCSI_ASC_CMD_PHASE_ERR                        0x4Au
#define  SCSI_ASC_DATA_PHASE_ERR                       0x4Bu
#define  SCSI_ASC_LOG_UNIT_FAILED_SELF_CFG             0x4Cu
#define  SCSI_ASC_OVERLAPPED_CMDS_ATTEMPTED            0x4Eu

#define  SCSI_ASC_WR_APPEND_ERR                        0x50u
#define  SCSI_ASC_ERASE_FAIL                           0x51u
#define  SCSI_ASC_CARTRIDGE_FAULT                      0x52u
#define  SCSI_ASC_MEDIA_LOAD_OR_EJECT_FAILED           0x53u
#define  SCSI_ASC_SCSI_TO_HOST_SYSTEM_IF_FAIL          0x54u
#define  SCSI_ASC_SYSTEM_RESOURCE_FAIL                 0x55u
#define  SCSI_ASC_UNABLE_TO_RECOVER_TOC                0x57u
#define  SCSI_ASC_GENERATION_DOES_NOT_EXIST            0x58u
#define  SCSI_ASC_UPDATED_BLOCK_RD                     0x59u
#define  SCSI_ASC_OP_REQUEST_OR_STATE_CHANGE_INPUT     0x5Au
#define  SCSI_ASC_LOG_EXCEPT                           0x5Bu
#define  SCSI_ASC_RPL_STATUS_CHANGE                    0x5Cu
#define  SCSI_ASC_FAIL_PREDICTION_TH_EXCEEDED          0x5Du
#define  SCSI_ASC_LOW_POWER_CONDITION_ON               0x5Eu

#define  SCSI_ASC_LAMP_FAIL                            0x60u
#define  SCSI_ASC_VIDEO_ACQUISITION_ERR                0x61u
#define  SCSI_ASC_SCAN_HEAD_POSITIONING_ERR            0x62u
#define  SCSI_ASC_END_OF_USER_AREA_ENCOUNTERED         0x63u
#define  SCSI_ASC_ILLEGAL_MODE_FOR_THIS_TRACK          0x64u
#define  SCSI_ASC_VOLTAGE_FAULT                        0x65u
#define  SCSI_ASC_AUTO_DOCUMENT_FEEDER_COVER_UP        0x66u
#define  SCSI_ASC_CONFIGURATION_FAIL                   0x67u
#define  SCSI_ASC_LOG_UNIT_NOT_CONFIGURED              0x68u
#define  SCSI_ASC_DATA_LOSS_ON_LOG_UNIT                0x69u
#define  SCSI_ASC_INFORMATIONAL_REFER_TO_LOG           0x6Au
#define  SCSI_ASC_STATE_CHANGE_HAS_OCCURRED            0x6Bu
#define  SCSI_ASC_REBUILD_FAIL_OCCURRED                0x6Cu
#define  SCSI_ASC_RECALCULATE_FAIL_OCCURRED            0x6Du
#define  SCSI_ASC_CMD_TO_LOG_UNIT_FAILED               0x6Eu
#define  SCSI_ASC_COPY_PROTECTION_KEY_EXCHANGE_FAIL    0x6Fu
#define  SCSI_ASC_DECOMPRESSION_EXCEPT_LONG_ALGO_ID    0x71u
#define  SCSI_ASC_SESSION_FIXATION_ERR                 0x72u
#define  SCSI_ASC_CD_CONTROL_ERR                       0x73u
#define  SCSI_ASC_SECURITY_ERR                         0x74u

//                                                                 --------------- SCSI PAGE PARAMETERS ---------------
//                                                                 See Note #4.
#define SCSI_PAGE_CODE_READ_WRITE_ERROR_RECOVERY       0x01u
#define SCSI_PAGE_CODE_FORMAT_DEVICE                   0x03u
#define SCSI_PAGE_CODE_FLEXIBLE_DISK                   0x05u
#define SCSI_PAGE_CODE_INFORMATIONAL_EXCEPTIONS        0x1Cu
#define SCSI_PAGE_CODE_ALL                             0x3Fu

#define SCSI_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS      0x0Au
#define SCSI_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY     0x0Au
#define SCSI_PAGE_LENGTH_FLEXIBLE_DISK                 0x1Eu
#define SCSI_PAGE_LENGTH_FORMAT_DEVICE                 0x16u

/********************************************************************************************************
 *                                           SCSI COMMAND LENGTH
 *******************************************************************************************************/

#define  SCSI_CMD_INQUIRY_LEN                            6u
#define  SCSI_CMD_TEST_UNIT_RDY_LEN                      6u
#define  SCSI_CMD_RD_CAPACITY_LEN                       10u
#define  SCSI_CMD_REQ_SENSE_LEN                          6u
#define  SCSI_CMD_READ_10_LEN                           10u
#define  SCSI_CMD_WRITE_10_LEN                          10u
#define  SCSI_CMD_SYNCHRONIZE_CACHE_10_LEN              10u

/********************************************************************************************************
 *                                   SCSI COMMAND PARAMETER DATA LENGTH
 *******************************************************************************************************/

#define  SCSI_CMD_REQ_SENSE_RESP_DATA_LEN              18u
#define  SCSI_CMD_INQUIRY_RESP_DATA_LEN                36u
#define  SCSI_CMD_READ_CAPACITY_RESP_DATA_LEN           8u

/********************************************************************************************************
 *                                       SCSI COMMAND REQUEST SENSE
 *******************************************************************************************************/

#define  SCSI_CMD_REQ_SENSE_BIT_DESC                    DEF_BIT_07
#define  SCSI_CMD_REQ_SENSE_RESP_DATA_ERR_CUR           0x70u
#define  SCSI_CMD_REQ_SENSE_RESP_DATA_ERR_DEFERRED      0x71u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           SCSI TRANSPORT API
 *******************************************************************************************************/
//                                                                 *INDENT-OFF*
typedef struct fs_scsi_transport_api {
  void (*Init)(RTOS_ERR *p_err);

  void *(*RespBufGet)(CPU_INT16U required_buf_len,
                      void       *p_arg,
                      RTOS_ERR   *p_err);

  void (*RespBufFree)(void     *p_resp_buf,
                      void     *p_arg,
                      RTOS_ERR *p_err);

  CPU_INT32U (*Rx)(CPU_INT08U  lun,
                   void        *p_cmd_blk,
                   CPU_INT08U  cmd_blk_len,
                   void        *p_buf,
                   CPU_INT32U  buf_len,
                   CPU_INT32U  timeout,
                   void        *p_arg,
                   CPU_BOOLEAN *p_cmd_status,
                   RTOS_ERR    *p_err);

  CPU_INT32U (*Tx)(CPU_INT08U  lun,
                   void        *p_cmd_blk,
                   CPU_INT08U  cmd_blk_len,
                   void        *p_buf,
                   CPU_INT32U  buf_len,
                   CPU_INT32U  timeout,
                   void        *p_arg,
                   CPU_BOOLEAN *p_cmd_status,
                   RTOS_ERR    *p_err);
} FS_SCSI_TRANSPORT_API;
//                                                                 *INDENT-ON*
/********************************************************************************************************
 *                                       SCSI INTERNAL DATA STRUCTURE
 *
 * Note(s) : (1) The logical unit (LU) active list allows to retrieve the LU control block while
 *               processing the LU disconnection event.
 *
 *           (2) The logical unit (LU) free list allows to reuse a previously allocated LU control block
 *               while processing the LU connection event.
 *******************************************************************************************************/

typedef struct fs_scsi {
  MEM_DYN_POOL LU_Pool;                                         // Dyn mem pool of LU struct.
  MEM_DYN_POOL PmItemPool;
  SLIST_MEMBER *LU_ActiveListHeadPtr;                           // Ptr to head of active LU list (see Note #1).

#if (FS_STORAGE_CFG_CTR_STAT_EN == DEF_ENABLED)
  CPU_INT16U   StatConnCtr;
  CPU_INT16U   StatDisconnCtr;
  CPU_INT16U   StatOpenCtr;
  CPU_INT16U   StatCloseCtr;
#endif
} FS_SCSI;

/********************************************************************************************************
 *                                       LOGICAL UNIT DATA TYPE
 *******************************************************************************************************/

typedef struct fs_lu {
  FS_MEDIA        Media;
  FS_BLK_DEV      BlkDev;
  FS_SCSI_LU_INFO Info;
} FS_LU;

typedef struct fs_scsi_lu {
  FS_LU                 LU;
  CPU_INT08U            LU_Nbr;                                 // Logical unit nbr provided by Transport layer.
  CPU_INT08U            DevID;                                  // Unique dev ID assigned by Transport layer.
  CPU_BOOLEAN           Conn;                                   // Flag indicating if LU is conn.
  CPU_BOOLEAN           SyncCache;                              // Flag indicating if LU supports SCSI SYNC CACHE cmd.
  FS_SCSI_TRANSPORT_API *TransportApiPtr;                       // Ptr to Transport API used by SCSI.
  void                  *TranportArgPtr;                        // Ptr to arg used by Transport layer.
  SLIST_MEMBER          ListMember;                             // Ptr to nxt LU.
} FS_SCSI_LU;

/********************************************************************************************************
 *                                           SENSE DATA DATA TYPE
 *******************************************************************************************************/

typedef struct fs_scsi_sense_data {
  CPU_INT08U SenseKey;
  CPU_INT08U AddidionalSenseCode;
  CPU_INT08U AddidionalSenseCodeQual;
} FS_SCSI_SENSE_DATA;

typedef struct fs_scsi_pm_item {
  FS_MEDIA_PM_ITEM      MediaPmItem;
  CPU_INT08U            LU_Nbr;
  CPU_INT08U            DevID;
  FS_SCSI_TRANSPORT_API *TransportApiPtr;
  void                  *TranportArgPtr;
  CPU_CHAR              Name[SCSI_MAX_LU_NAME_LEN + 1u];
} FS_SCSI_PM_ITEM;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if LOG_VRB_IS_EN()
static const CPU_CHAR *FS_SCSI_InquiryDevTypeStrPtrTbl[] = {
  "Direct access block device\0",
  "Sequential-access device\0",
  "Printer device\0",
  "Processor device\0",
  "Write-once device\0",
  "CD-DVD device\0",
  "Scanner device\0",
  "Optical memory device\0",
  "Medium changer device\0",
  "Communications device\0",
  "-\0",
  "-\0",
  "Storage array controller device\0",
  "Enclosure services device\0",
  "Simplified direct-access device\0",
  "Optical card reader/writer device\0",
  "Bridge Controller Commands\0",
  "Object-based Storage device\0",
  "Automation-Drive Interface\0"
};

static const CPU_CHAR *FS_SCSI_ReqSense_SenseKey_StrPtrTbl[] = {
  "No sense\0",
  "Recovered error\0",
  "Not ready\0",
  "Medium error\0",
  "Hardware error\0",
  "Illegal request\0",
  "Unit attention\0",
  "Data protect\0",
  "Blank check\0",
  "Vendor specific\0",
  "Copy aborted\0",
  "Aborted command\0",
  "-\0",
  "Volume overflow\0",
  "Miscompare\0"
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const FS_SCSI_HANDLE FS_SCSI_NullHandle = { { 0 } };

static FS_SCSI *FS_SCSI_Ptr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ----------------- BLK DEV IF FNCTS -----------------

static FS_MEDIA *FS_SCSI_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                  RTOS_ERR               *p_err);

static void FS_SCSI_MediaRem(FS_MEDIA *p_media,
                             RTOS_ERR *p_err);

static FS_BLK_DEV *FS_SCSI_BlkDevAdd(FS_MEDIA *p_media,
                                     RTOS_ERR *p_err);

static void FS_SCSI_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                              RTOS_ERR   *p_err);

static void FS_SCSI_OpenInternal(FS_BLK_DEV *p_blk_dev,
                                 RTOS_ERR   *p_err);

static void FS_SCSI_CloseInternal(FS_BLK_DEV *p_blk_dev,
                                  RTOS_ERR   *p_err);

static void FS_SCSI_Rd(FS_BLK_DEV *p_blk_dev,
                       void       *p_dest,
                       FS_LB_NBR  start,
                       FS_LB_QTY  cnt,
                       RTOS_ERR   *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SCSI_Wr(FS_BLK_DEV *p_blk_dev,
                       void       *p_src,
                       FS_LB_NBR  start,
                       FS_LB_QTY  cnt,
                       RTOS_ERR   *p_err);

static void FS_SCSI_Sync(FS_BLK_DEV *p_blk_dev,
                         RTOS_ERR   *p_err);

static void FS_SCSI_Trim(FS_BLK_DEV *p_blk_dev,
                         FS_LB_NBR  lb_nbr,
                         RTOS_ERR   *p_err);
#endif

static void FS_SCSI_Query(FS_BLK_DEV      *p_blk_dev,
                          FS_BLK_DEV_INFO *p_info,
                          RTOS_ERR        *p_err);

static CPU_SIZE_T FS_SCSI_AlignReqGet(FS_MEDIA *p_media,
                                      RTOS_ERR *p_err);

static CPU_BOOLEAN FS_SCSI_IsConn(FS_MEDIA *p_media);

//                                                                 ------------------ SCSI CMD FNCTS ------------------
static void FS_SCSI_CmdInquiry(FS_SCSI_LU *p_lu,
                               RTOS_ERR   *p_err);

static CPU_BOOLEAN FS_SCSI_CmdTestUnitRdy(FS_SCSI_LU *p_lu,
                                          RTOS_ERR   *p_err);

static void FS_SCSI_CmdRdCapacity(FS_SCSI_LU *p_lu,
                                  RTOS_ERR   *p_err);

static void FS_SCSI_CmdReqSense(FS_SCSI_LU         *p_lu,
                                FS_SCSI_SENSE_DATA *p_sense_data,
                                RTOS_ERR           *p_err);

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SCSI_CmdSynchCache(FS_SCSI_LU *p_lu,
                                  RTOS_ERR   *p_err);
#endif

#if LOG_VRB_IS_EN()
static void FS_SCSI_SenseDataPrint(FS_SCSI_LU         *p_lu,
                                   FS_SCSI_SENSE_DATA *p_sense_data);
#endif

/********************************************************************************************************
 *                                           INTERFACE STRUCTURE
 *******************************************************************************************************/

static const FS_MEDIA_API FS_SCSI_MediaApi = {
  .Add = FS_SCSI_MediaAdd,
  .Rem = FS_SCSI_MediaRem,
  .AlignReqGet = FS_SCSI_AlignReqGet,
  .IsConn = FS_SCSI_IsConn
};

const FS_BLK_DEV_API FS_SCSI_BlkDevApi = {
  .Add = FS_SCSI_BlkDevAdd,
  .Rem = FS_SCSI_BlkDevRem,
  .Open = FS_SCSI_OpenInternal,
  .Close = FS_SCSI_CloseInternal,
  .Rd = FS_SCSI_Rd,
#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
  .Wr = FS_SCSI_Wr,
  .Sync = FS_SCSI_Sync,
  .Trim = FS_SCSI_Trim,
#endif
  .Query = FS_SCSI_Query,
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               FS_SCSI_Init()
 *
 * @brief    Initialize the driver.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *******************************************************************************************************/
void FS_SCSI_Init(RTOS_ERR *p_err)
{
  //                                                               Chk if SCSI already init.
  RTOS_ASSERT_DBG(FS_SCSI_Ptr == DEF_NULL, RTOS_ERR_ALREADY_INIT,; );

  //                                                               Alloc SCSI data.
  FS_SCSI_Ptr = (FS_SCSI *)Mem_SegAlloc("FS - SCSI Data",
                                        FSMedia_InitCfgPtr->MemSegPtr,
                                        sizeof(FS_SCSI),
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_Clr((void *)FS_SCSI_Ptr, sizeof(FS_SCSI));

  //                                                               Create LU objects pool.
  Mem_DynPoolCreate("FS - SCSI LU objects",
                    &FS_SCSI_Ptr->LU_Pool,
                    FSMedia_InitCfgPtr->MemSegPtr,
                    sizeof(FS_SCSI_LU),
                    sizeof(CPU_ALIGN),
                    FSMedia_InitCfgPtr->MaxSCSILuCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 0u : FSMedia_InitCfgPtr->MaxSCSILuCnt,
                    FSMedia_InitCfgPtr->MaxSCSILuCnt,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Mem_DynPoolCreate("FS - SCSI PM items",
                    &FS_SCSI_Ptr->PmItemPool,
                    FSMedia_InitCfgPtr->MemSegPtr,
                    sizeof(FS_SCSI_PM_ITEM),
                    sizeof(CPU_ALIGN),
                    FSMedia_InitCfgPtr->MaxSCSILuCnt == LIB_MEM_BLK_QTY_UNLIMITED ? 0u : FSMedia_InitCfgPtr->MaxSCSILuCnt,
                    FSMedia_InitCfgPtr->MaxSCSILuCnt,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SList_Init(&FS_SCSI_Ptr->LU_ActiveListHeadPtr);
}

/****************************************************************************************************//**
 *                                               FS_SCSI_Open()
 *
 * @brief    Open a SCSI device.
 *
 * @param    media_handle    Handle to a media.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_IO
 *
 * @return   Handle to a SCSI device.
 *******************************************************************************************************/
FS_SCSI_HANDLE FS_SCSI_Open(FS_MEDIA_HANDLE media_handle,
                            RTOS_ERR        *p_err)
{
  FS_SCSI_LU     *p_lu;
  FS_SCSI_HANDLE scsi_handle = FS_SCSI_NullHandle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, scsi_handle);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  //                                                               Check that Storage layer is initialized.
  CORE_ENTER_ATOMIC();
  if (!FSBlkDev_Data.IsInit) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
    CORE_EXIT_ATOMIC();
    return (scsi_handle);
  }
  CORE_EXIT_ATOMIC();

  //                                                               Send READ CAPACITY to LU to get its characteristics.
  p_lu = (FS_SCSI_LU *)media_handle.MediaPtr;
  FS_SCSI_CmdRdCapacity(p_lu, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (scsi_handle);
  }

  scsi_handle.MediaHandle = media_handle;

  return (scsi_handle);
}

/****************************************************************************************************//**
 *                                               FS_SCSI_Close()
 *
 * @brief    Close a SCSI device.
 *
 * @param    scsi_handle     Handle to a SCSI device.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void FS_SCSI_Close(FS_SCSI_HANDLE scsi_handle,
                   RTOS_ERR       *p_err)
{
  PP_UNUSED_PARAM(scsi_handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           FS_SCSI_LU_InfoGet()
 *
 * @brief    Get SCSI logical unit information.
 *
 * @param    scsi_handle     Handle to a SCSI device.
 *
 * @param    p_lu_info       Pointer to logical unit information structure to be populated.
 *
 * @param    p_err           Pointer to variable that will receive the return error code(s) from this
 *                           function:
 *                               - RTOS_ERR_NONE
 *******************************************************************************************************/
void FS_SCSI_LU_InfoGet(FS_SCSI_HANDLE  scsi_handle,
                        FS_SCSI_LU_INFO *p_lu_info,
                        RTOS_ERR        *p_err)
{
  FS_MEDIA   *p_media;
  FS_SCSI_LU *p_lu;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  RTOS_ASSERT_DBG_ERR_SET(p_lu_info != DEF_NULL, *p_err, RTOS_ERR_NULL_PTR,; )

  FS_MEDIA_WITH_NO_IO(scsi_handle.MediaHandle, p_err) {
    p_media = scsi_handle.MediaHandle.MediaPtr;

    FS_MEDIA_LOCK_WITH(p_media) {
      p_lu = (FS_SCSI_LU *)p_media;
      *p_lu_info = p_lu->LU.Info;
    }
  }
}

/****************************************************************************************************//**
 *                                               FS_SCSI_LU_Conn()
 *
 * @brief    Callback called by Transport layer to notify about a logical unit connection.
 *
 * @param    lun                 Logical unit number belonging to a given device.
 *
 * @param    dev_id              Unique device ID assigned by Transport layer.
 *
 * @param    p_transport_api     Pointer to Transport layer API.
 *
 * @param    p_transport_arg     Pointer to Transport layer argument.
 *******************************************************************************************************/
void FS_SCSI_LU_Conn(CPU_INT08U lun,
                     CPU_INT16U dev_id,
                     void       *p_transport_api,
                     void       *p_transport_arg)
{
  FS_SCSI_PM_ITEM *p_scsi_pm_item;
  FS_SCSI_LU      *p_lu;
  CPU_SIZE_T      char_cnt;
  RTOS_ERR        err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  RTOS_ASSERT_CRITICAL((FS_SCSI_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  WITH_SCOPE_BEGIN(&err) {
    BREAK_ON_ERR(p_scsi_pm_item = (FS_SCSI_PM_ITEM *)Mem_DynPoolBlkGet(&FS_SCSI_Ptr->PmItemPool, &err));
    ON_BREAK {
      RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
      Mem_DynPoolBlkFree(&FS_SCSI_Ptr->PmItemPool, (void *)p_scsi_pm_item, &err_tmp);
      break;
    } WITH {
      p_scsi_pm_item->LU_Nbr = lun;
      p_scsi_pm_item->DevID = dev_id;
      p_scsi_pm_item->TranportArgPtr = p_transport_arg;
      p_scsi_pm_item->TransportApiPtr = (FS_SCSI_TRANSPORT_API *)p_transport_api;

      char_cnt = Str_Snprintf(p_scsi_pm_item->Name,
                              SCSI_MAX_LU_NAME_LEN + 1u,
                              "scsi%02u%02u",
                              FS_UTIL_MODULO_PWR2(dev_id, 6u),
                              FS_UTIL_MODULO_PWR2(lun, 6u));

      ASSERT_BREAK(char_cnt <= SCSI_MAX_LU_NAME_LEN + 1u, RTOS_ERR_WOULD_OVF);

      p_scsi_pm_item->MediaPmItem.PmItem.StrID = p_scsi_pm_item->Name;
      p_scsi_pm_item->MediaPmItem.PmItem.Type = PLATFORM_MGR_ITEM_TYPE_HW_INFO_FS_SCSI;
      p_scsi_pm_item->MediaPmItem.MediaApiPtr = &FS_SCSI_MediaApi;
      BREAK_ON_ERR(PlatformMgrItemAdd(&p_scsi_pm_item->MediaPmItem.PmItem, &err));

      ON_BREAK {
        RTOS_ERR err_tmp = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);
        PlatformMgrItemRem(&p_scsi_pm_item->MediaPmItem.PmItem, &err_tmp);
        break;
      } WITH {
        BREAK_ON_ERR(p_lu = (FS_SCSI_LU *)FSMedia_Add(&p_scsi_pm_item->MediaPmItem, &err));

#if (FS_STORAGE_CFG_MEDIA_POLL_TASK_EN == DEF_DISABLED)
        ON_BREAK {
          FSMedia_Rem(&p_lu->LU.Media);
          break;
        } WITH {
          CPU_INT08U  retry = SCSI_LU_RDY_RETRY_CNT_MAX;
          CPU_BOOLEAN is_conn;
          //                                                       ----------------- CHECK IF LU RDY ------------------
          BREAK_ON_ERR(while) (retry > 0u) {
            is_conn = FS_SCSI_IsConn(&p_lu->LU.Media);
            if (is_conn) {
              break;                                            // LU rdy, quit the loop.
            }

            KAL_Dly(50u);                                       // Wait 50 ms before nxt TEST UNIT RDY attempt.
            retry--;
          }
          if (!is_conn) {                                       // LU NOT rdy, app won't be notified.
            FSMedia_Rem(&p_lu->LU.Media);
          } else {
            FSMedia_Conn(&p_lu->LU.Media);                      // Notify app that LU is rdy for comm.
          }
        }
#else
        PP_UNUSED_PARAM(p_lu);
#endif
      }
    }
  } WITH_SCOPE_END

  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Connecting Dev #", (u)dev_id, "/LU #", (u)lun,
             " failed w/ err = ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/****************************************************************************************************//**
 *                                           FS_SCSI_LU_Disconn()
 *
 * @brief    Callback called by Transport layer to notify about a logical unit disconnection.
 *
 * @param    lun                 Logical unit number belonging to a given device.
 *
 * @param    dev_id              Unique device ID assigned by Transport layer.
 *
 * @param    p_transport_arg     Pointer to Transport layer argument.
 *
 * @note     (1) When a target composed of multiple logical units (LU) disconnects, it is normal that
 *               no LU is found in the list containing all connected LUs. After a multi-LU target
 *               connects, a LU is inserted in this list only if this one responds with success to
 *               the command TEST UNIT READY. Each LU not ready (TEST UNIT READY failed) won't be
 *               inserted in the list.
 *******************************************************************************************************/
void FS_SCSI_LU_Disconn(CPU_INT08U lun,
                        CPU_INT16U dev_id,
                        void       *p_transport_arg)
{
  FS_SCSI_LU  *p_lu;
  CPU_BOOLEAN lu_found;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_CRITICAL((FS_SCSI_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  PP_UNUSED_PARAM(p_transport_arg);
  FS_BLK_DEV_CTR_STAT_INC(FS_SCSI_Ptr->StatDisconnCtr);

  LOG_VRB((">> Dev #", (u)dev_id, "/LU #", (u)lun, " disconnected!"));

  //                                                               Get LU struct associated to LU nbr.
  lu_found = DEF_NO;
  SLIST_FOR_EACH_ENTRY(FS_SCSI_Ptr->LU_ActiveListHeadPtr, p_lu, FS_SCSI_LU, ListMember) {
    if ((p_lu->LU_Nbr == lun)
        && (p_lu->DevID == dev_id)) {
      lu_found = DEF_YES;
      break;
    }
  }

  if (!lu_found) {                                              // See Note #1.
    return;
  }

  CORE_ENTER_ATOMIC();
  p_lu->Conn = DEF_NO;                                          // LU marked as NOT conn prevents further SCSI comm.
  p_lu->SyncCache = DEF_NO;
  CORE_EXIT_ATOMIC();

  FSMedia_Disconn(&p_lu->LU.Media);
  FSMedia_Rem(&p_lu->LU.Media);
}

/****************************************************************************************************//**
 *                                       FS_SCSI_MaxRespBufLenGet()
 *
 * @brief    Callback called by Transport layer to get the maximum response buffer length needed
 *           among all SCSI commands generated internally by the SCSI layer.
 *
 * @return   Maximum response buffer length.
 *******************************************************************************************************/
CPU_INT16U FS_SCSI_MaxRespBufLenGet(void)
{
  RTOS_ASSERT_CRITICAL((FS_SCSI_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT, 0u);
  return (SCSI_CMD_INQUIRY_RESP_DATA_LEN);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DRIVER INTERFACE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SCSI_MediaAdd()
 *
 * @brief    Add a SCSI logical unit media instance.
 *
 * @param    p_pm_item   Pointer to a media platform manager item.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to the added media.
 *******************************************************************************************************/
static FS_MEDIA *FS_SCSI_MediaAdd(const FS_MEDIA_PM_ITEM *p_pm_item,
                                  RTOS_ERR               *p_err)
{
  FS_SCSI_LU      *p_lu;
  FS_SCSI_PM_ITEM *p_scsi_pm_item = (FS_SCSI_PM_ITEM *)p_pm_item;

  PP_UNUSED_PARAM(p_err);

  p_lu = (FS_SCSI_LU *)Mem_DynPoolBlkGet(&FS_SCSI_Ptr->LU_Pool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error getting a new LU struct for Dev #", (u)p_scsi_pm_item->DevID,
             "/LU #", (u)p_scsi_pm_item->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return (DEF_NULL);
  }

  Mem_Clr((void *)p_lu, sizeof(FS_SCSI_LU));                    // Clr LU struct.

  //                                                               Save some LU info.
  p_lu->LU_Nbr = p_scsi_pm_item->LU_Nbr;
  p_lu->DevID = p_scsi_pm_item->DevID;
  p_lu->TranportArgPtr = p_scsi_pm_item->TranportArgPtr;
  p_lu->TransportApiPtr = p_scsi_pm_item->TransportApiPtr;
  p_lu->Conn = DEF_YES;
  p_lu->SyncCache = DEF_YES;                                    // By dflt, mark LU as supporting SYNC CACHE.

  FS_SCSI_CmdInquiry(p_lu, p_err);                              // Send INQUIRY cmd to dev.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR err;
    LOG_ERR(("Error sending INQUIRY cmd for Dev #", (u)p_scsi_pm_item->DevID,
             "/LU #", (u)p_scsi_pm_item->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));

    Mem_DynPoolBlkFree(&FS_SCSI_Ptr->LU_Pool, (void *)p_lu, &err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE,
                         RTOS_ERR_ASSERT_CRITICAL_FAIL, DEF_NULL);
    return (DEF_NULL);
  }
  //                                                               Add LU to active list.
  SList_PushBack(&FS_SCSI_Ptr->LU_ActiveListHeadPtr, &p_lu->ListMember);

  LOG_VRB((">> Dev #", (u)p_scsi_pm_item->DevID, "/LU #", (u)p_scsi_pm_item->LU_Nbr, " connected!"));

  return (&p_lu->LU.Media);
}

/****************************************************************************************************//**
 *                                               FS_SCSI_Rem()
 *
 * @brief    Remove a SCSI logical unit media instance.
 *
 * @param    p_media     Pointer to a SCSI logical unit instance.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SCSI_MediaRem(FS_MEDIA *p_media,
                             RTOS_ERR *p_err)
{
  FS_SCSI_LU *p_lu = (FS_SCSI_LU *)p_media;
  RTOS_ERR   err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);

  PP_UNUSED_PARAM(p_err);

  SList_Rem(&FS_SCSI_Ptr->LU_ActiveListHeadPtr, &p_lu->ListMember);

  Mem_DynPoolBlkFree(&FS_SCSI_Ptr->LU_Pool, (void *)p_lu, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE,
                       RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  PlatformMgrItemRem(&p_media->PmItemPtr->PmItem, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE,
                       RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  Mem_DynPoolBlkFree(&FS_SCSI_Ptr->PmItemPool, (void *)p_media->PmItemPtr, &err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE,
                       RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_SCSI_BlkDevAdd()
 *
 * @brief    Add a SCSI block device.
 *
 * @param    p_media     Pointer to a SCSI media instance.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Pointer to block device.
 *******************************************************************************************************/
static FS_BLK_DEV *FS_SCSI_BlkDevAdd(FS_MEDIA *p_media,
                                     RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  return (&((FS_LU *)p_media)->BlkDev);
}

/****************************************************************************************************//**
 *                                           FS_SCSI_BlkDevRem()
 *
 * @brief    Remove a SCSI block device.
 *
 * @param    p_blk_dev   Pointer to a SCSI block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SCSI_BlkDevRem(FS_BLK_DEV *p_blk_dev,
                              RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);
}

/****************************************************************************************************//**
 *                                           FS_SCSI_OpenInternal()
 *
 * @brief    Open a SCSI block device.
 *
 * @param    p_blk_dev   Pointer to SCSI block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SCSI_OpenInternal(FS_BLK_DEV *p_blk_dev,
                                 RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);

  FS_BLK_DEV_CTR_STAT_INC(FS_SCSI_Ptr->StatOpenCtr);
}

/****************************************************************************************************//**
 *                                           FS_SCSI_CloseInternal()
 *
 * @brief    Close a SCSI block device.
 *
 * @param    p_blk_dev   Pointer to SCSI block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
static void FS_SCSI_CloseInternal(FS_BLK_DEV *p_blk_dev,
                                  RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(p_err);

  FS_BLK_DEV_CTR_STAT_INC(FS_SCSI_Ptr->StatCloseCtr);
}

/****************************************************************************************************//**
 *                                               FS_SCSI_Rd()
 *
 * @brief    Read from a block device & store data in buffer.
 *
 * @param    p_blk_dev   Pointer to logical unit block device instance.
 *
 * @param    p_dest      Pointer to destination buffer.
 *
 * @param    start       Start sector to read from.
 *
 * @param    cnt         Number of sectors to read.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) See 'SCSI Block Commands - 3 (SBC-3)', section 5.8 for more details about SCSI
 *               "READ(10)" command.
 *
 * @note     (2) The application may communicate with the logical unit (LU) while this one is not
 *               ready anymore. A LU is not ready anymore for 2 main reasons:
 *              - (a) LU has been physically removed from the SCSI host.
 *              - (b) LU is a removable media belonging to a target device. It has been physically
 *                     removed from the target device but this one is still connected to the SCSI host.
 *               In all cases, a new communication attempt must not take place and the device
 *               IO error is reported.
 *
 * @note     (3) Even if the SCSI READ command has reported a failed command, if the sense data
 *               indicates NO SENSE or RECOVERED ERROR, the command is considered as successful.
 *               See 'SCSI Primary Commands - 3 (SBC-3)', section 4.5.6 for more details about sense
 *               keys description.
 *******************************************************************************************************/
static void FS_SCSI_Rd(FS_BLK_DEV *p_blk_dev,
                       void       *p_dest,
                       FS_LB_NBR  start,
                       FS_LB_QTY  cnt,
                       RTOS_ERR   *p_err)
{
  FS_SCSI_LU  *p_lu = (FS_SCSI_LU *)p_blk_dev->MediaPtr;
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_READ_10_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;
  CPU_INT32U  rx_buf_len = cnt * p_lu->LU.Info.SecDfltSize;

  //                                                               Ensure LU is really rdy for comm (see Note #2).
  if (!p_lu->Conn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_READ_10;
  MEM_VAL_COPY_SET_INT32U_BIG(&cmd_blk_buf[2u], &start);        // Logical Block Address (LBA).
  MEM_VAL_COPY_SET_INT16U_BIG(&cmd_blk_buf[7u], &cnt);          // Transfer length (number of logical blocks).

  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Rx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf[0u],
                            SCSI_CMD_READ_10_LEN,
                            p_dest,
                            rx_buf_len,
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error sending SCSI READ(10) cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                          // Translate Transport layer err in IO err.
    return;
  }

  if (cmd_status == DEF_FAIL) {                                 // Send REQ SENSE to have more details about failed cmd.
    FS_SCSI_SENSE_DATA sense_data;

    FS_SCSI_CmdReqSense(p_lu, &sense_data, p_err);
    if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
        || ((sense_data.SenseKey != SCSI_SENSE_KEY_NO_SENSE)
            && (sense_data.SenseKey != SCSI_SENSE_KEY_RECOVERED_ERROR))) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                        // See Note #3.
    }
  }
}

/****************************************************************************************************//**
 *                                               FS_SCSI_Wr()
 *
 * @brief    Write data to a block device from a buffer.
 *
 * @param    p_blk_dev   Pointer to logical unit block device instance.
 *
 * @param    p_src       Pointer to source buffer.
 *
 * @param    start       Start sector to write to.
 *
 * @param    cnt         Number of sectors to write.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) See 'SCSI Block Commands - 3 (SBC-3)', section 5.27 for more details about SCSI
 *               "WRITE(10)" command.
 *
 * @note     (2) The application may communicate with the logical unit (LU) while this one is not
 *               ready anymore. A LU is not ready anymore for 2 main reasons:
 *               - (a) LU has been physically removed from the SCSI host.
 *               - (b) LU is a removable media belonging to a target device. It has been physically
 *                     removed from the target device but this one is still connected to the SCSI host.
 *               In all cases, a new communication attempt must not take place and the device
 *               IO error is reported.
 *
 * @note     (3) See Note #3 of function FS_SCSI_Rd().
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SCSI_Wr(FS_BLK_DEV *p_blk_dev,
                       void       *p_src,
                       FS_LB_NBR  start,
                       FS_LB_QTY  cnt,
                       RTOS_ERR   *p_err)
{
  FS_SCSI_LU  *p_lu = (FS_SCSI_LU *)p_blk_dev->MediaPtr;
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_WRITE_10_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;
  CPU_INT32U  tx_buf_len = cnt * p_lu->LU.Info.SecDfltSize;

  //                                                               Ensure LU is really rdy for comm (see Note #2).
  if (!p_lu->Conn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_WRITE_10;
  MEM_VAL_COPY_SET_INT32U_BIG(&cmd_blk_buf[2u], &start);        // Logical Block Address (LBA).
  MEM_VAL_COPY_SET_INT16U_BIG(&cmd_blk_buf[7u], &cnt);          // Transfer length (number of logical blocks).

  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Tx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf[0u],
                            SCSI_CMD_WRITE_10_LEN,
                            p_src,
                            tx_buf_len,
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error sending SCSI WRITE(10) cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                          // Translate Transport layer err in IO err.
    return;
  }

  if (cmd_status == DEF_FAIL) {                                 // Send REQ SENSE to have more details about failed cmd.
    FS_SCSI_SENSE_DATA sense_data;

    FS_SCSI_CmdReqSense(p_lu, &sense_data, p_err);
    if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
        || ((sense_data.SenseKey != SCSI_SENSE_KEY_NO_SENSE)
            && (sense_data.SenseKey != SCSI_SENSE_KEY_RECOVERED_ERROR))) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                        // See Note #3.
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                               FS_SCSI_Sync()
 *
 * @brief    Sync SCSI logical unit.
 *
 * @param    p_blk_dev   Pointer to SCSI block device.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SCSI_Sync(FS_BLK_DEV *p_blk_dev,
                         RTOS_ERR   *p_err)
{
  FS_SCSI_LU *p_lu = (FS_SCSI_LU *)p_blk_dev->MediaPtr;

  if (!p_lu->Conn) {                                            // Ensure LU is really rdy for comm.
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  if (!p_lu->SyncCache) {                                       // Ensure LU support SCSI SYNC CACHE cmd.
    return;
  }

  FS_SCSI_CmdSynchCache(p_lu, p_err);
}
#endif

/****************************************************************************************************//**
 *                                               FS_SCSI_Trim()
 *
 * @brief    Trim SCSI logical unit.
 *
 * @param    p_blk_dev   Pointer to SCSI block device.
 *
 * @param    lb_nbr      Logical block number.
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SCSI_Trim(FS_BLK_DEV *p_blk_dev,
                         FS_LB_NBR  lb_nbr,
                         RTOS_ERR   *p_err)
{
  PP_UNUSED_PARAM(p_blk_dev);
  PP_UNUSED_PARAM(lb_nbr);
  PP_UNUSED_PARAM(p_err);
}
#endif

/****************************************************************************************************//**
 *                                               FS_SCSI_Query()
 *
 * @brief    Get information about a device.
 *
 * @param    p_blk_dev   Pointer to SCSI block device.
 *
 * @param    p_info      Pointer to structure that will receive device information.
 *
 * @param    p_err       Error pointer.
 *
 * @note     (1) The application may communicate with the logical unit (LU) while this one is not
 *               ready anymore. A LU is not ready anymore for 2 main reasons:
 *               - (a) LU has been physically removed from the SCSI host.
 *               - (b) LU is a removable media belonging to a target device. It has been physically
 *                     removed from the target device but this one is still connected to the SCSI host.
 *               In all cases, a new communication attempt must not take place and the device
 *               IO error is reported.
 *******************************************************************************************************/
static void FS_SCSI_Query(FS_BLK_DEV      *p_blk_dev,
                          FS_BLK_DEV_INFO *p_info,
                          RTOS_ERR        *p_err)
{
  FS_SCSI_LU *p_lu = (FS_SCSI_LU *)p_blk_dev->MediaPtr;

  //                                                               Ensure LU is really rdy for comm (see Note #1).
  if (!p_lu->Conn) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
  //                                                               Send READ CAPACITY to LU to get its characteristics.
  FS_SCSI_CmdRdCapacity(p_lu, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_info->LbSizeLog2 = FSUtil_Log2(p_lu->LU.Info.SecDfltSize);
  p_info->LbCnt = p_lu->LU.Info.SecCnt;
  p_info->Fixed = p_lu->LU.Info.Removable;
}

/****************************************************************************************************//**
 *                                           FS_SCSI_AlignReqGet()
 *
 * @brief    Get buffer alignment requirement of SCSI.
 *
 * @param    p_media     Pointer to media.
 *
 * @param    p_err       Error pointer.
 *
 * @return   Buffer alignment in bytes.
 *******************************************************************************************************/
static CPU_SIZE_T FS_SCSI_AlignReqGet(FS_MEDIA *p_media,
                                      RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_media);
  PP_UNUSED_PARAM(p_err);

  return (sizeof(CPU_ALIGN));
}

/****************************************************************************************************//**
 *                                               FS_SCSI_IsConn()
 *
 * @brief    Gives the status of device presence.
 *
 * @param    p_media     Pointer to media.
 *
 * @return   DEF_YES, if device is present.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) This function can be called by the Media Poll task and the application. If the
 *               logical unit has been physically removed from the SCSI host (flag 'Conn'), no error
 *               is reported and the media state is reported as not connected. This is a normal
 *               situation and thus the error cannot be reported as an IO error. IsConn() is
 *               actively used by the Media Poll task that relies on the proper media state
 *               to recognize a real media connection or disconnection.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_SCSI_IsConn(FS_MEDIA *p_media)
{
  FS_SCSI_LU  *p_lu = (FS_SCSI_LU *)p_media;
  CPU_BOOLEAN lu_rdy;
  RTOS_ERR    err;

  RTOS_ERR_SET(err, RTOS_ERR_NONE);

  if (!p_lu->Conn) {                                            // Ensure LU is still present (see Note #1).
    return (DEF_NO);
  }
  //                                                               Send TEST UNIT READY cmd to dev.
  lu_rdy = FS_SCSI_CmdTestUnitRdy(p_lu, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (DEF_NO);
  }

  return (lu_rdy);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           FS_SCSI_CmdInquiry()
 *
 * @brief    Send command INQUIRY to the logical unit.
 *
 * @param    p_lu    Pointer to logical unit.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) See 'SCSI Primary Commands - 3 (SBC-3)', section 6.4 for more details about SCSI
 *               "INQUIRY" command.
 *
 * @note     (2) If the INQUIRY command has failed, there is no need to send REQUEST SENSE to have
 *               more details about the failed command. Indeed, INQUIRY is send to the logical unit
 *               from the Transport layer context and this command answers to all logical units even
 *               if there are not present. So analyzing the media presence state with REQUEST SENSE
 *               is not relevant in  this case.
 *******************************************************************************************************/
static void FS_SCSI_CmdInquiry(FS_SCSI_LU *p_lu,
                               RTOS_ERR   *p_err)
{
  CPU_INT08U  *p_resp_buf;
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_INQUIRY_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;
  RTOS_ERR    local_err;

  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_INQUIRY;
  cmd_blk_buf[4u] = SCSI_CMD_INQUIRY_RESP_DATA_LEN;
  //                                                               ------------------- GET RESP BUF -------------------
  p_resp_buf = (CPU_INT08U *)p_lu->TransportApiPtr->RespBufGet(SCSI_CMD_INQUIRY_RESP_DATA_LEN,
                                                               p_lu->TranportArgPtr,
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error getting a resp buf for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Rx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf[0u],
                            SCSI_CMD_INQUIRY_LEN,
                            (void *) p_resp_buf,
                            SCSI_CMD_INQUIRY_RESP_DATA_LEN,
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
      || (cmd_status != DEF_OK)) {                              // See Note #2.
    LOG_ERR(("Error sending SCSI INQUIRY cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                          // Translate Transport layer err in IO err.
    goto end_free;
  }
  //                                                               ----------------- PROCESS RESP BUF -----------------
  //                                                               Store relevant param data content of this cmd.
  p_lu->LU.Info.Removable = p_resp_buf[1u] >> 7u;

  Mem_Copy((void *) p_lu->LU.Info.VendorID_StrTbl,
           (void *)&p_resp_buf[8u],
           FS_SCSI_CMD_INQUIRY_VID_FIELD_LEN);

  Mem_Copy((void *) p_lu->LU.Info.ProductID_StrTbl,
           (void *)&p_resp_buf[16u],
           FS_SCSI_CMD_INQUIRY_PID_FIELD_LEN);

  Mem_Copy((void *) p_lu->LU.Info.ProdRevLevelStrTbl,
           (void *)&p_resp_buf[32u],
           FS_SCSI_CMD_INQUIRY_PROD_REV_LEVEL_FIELD_LEN);

#if LOG_VRB_IS_EN()
  {
    CPU_INT08U dev_type = p_resp_buf[0u] & 0x1Fu;

    LOG_VRB(("Device type     : ", (s)FS_SCSI_InquiryDevTypeStrPtrTbl[dev_type]));
  }
#endif
  LOG_VRB(("Vendor ID       : ", (s) & p_lu->LU.Info.VendorID_StrTbl[0u]));
  LOG_VRB(("Product ID      : ", (s) & p_lu->LU.Info.ProductID_StrTbl[0u]));
  LOG_VRB(("Product revision: ", (s) & p_lu->LU.Info.ProdRevLevelStrTbl[0u]));

end_free:
  //                                                               Rel resp buf.
  p_lu->TransportApiPtr->RespBufFree((void *)p_resp_buf,
                                     p_lu->TranportArgPtr,
                                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_SCSI_CmdTestUnitRdy()
 *
 * @brief    Send command TEST UNIT READY to the logical unit.
 *
 * @param    p_lu    Pointer to logical unit.
 *
 * @param    p_err   Error pointer.
 *
 * @return   DEF_YES, if logical unit is ready.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) See 'SCSI Primary Commands - 3 (SBC-3)', section 6.33 for more details about SCSI
 *               "TEST UNIT READY" command.
 *
 * @note     (2) When TEST UNIT READY is sent and the target device reports a failed command, the
 *               SCSI command REQUEST SENSE is sent to have more details about the error. If the
 *               error detail is a sense data indicating NOT READY or UNIT ATTENTION, it is
 *               considered as a no error situation as it indicates that the medium is not present.
 *               In that case, a new TEST UNIT READY should be sent to the target device. Other sense
 *               data will provoke a IO error and another error recovery procedure should be used by
 *               upper layers.
 *               See 'SCSI Primary Commands - 3 (SBC-3)', section 4.5.6 for more details about sense
 *               key description.
 *******************************************************************************************************/
static CPU_BOOLEAN FS_SCSI_CmdTestUnitRdy(FS_SCSI_LU *p_lu,
                                          RTOS_ERR   *p_err)
{
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_TEST_UNIT_RDY_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;
  CPU_BOOLEAN unit_rdy = DEF_NO;

  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_TEST_UNIT_READY;

  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Tx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf,
                            SCSI_CMD_TEST_UNIT_RDY_LEN,
                            DEF_NULL,
                            0u,                                 // No data phase.
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error sending SCSI TEST_UNIT_RDY cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return (DEF_NO);
  }

  if (cmd_status == DEF_FAIL) {                                 // Confirm that logical unit is not rdy.
    FS_SCSI_SENSE_DATA sense_data;

    FS_SCSI_CmdReqSense(p_lu, &sense_data, p_err);
    if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
        || ((sense_data.SenseKey != SCSI_SENSE_KEY_NOT_RDY)
            && (sense_data.SenseKey != SCSI_SENSE_KEY_UNIT_ATTENTION))) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                        // See Note #2.
    }
  } else {
    unit_rdy = DEF_YES;
  }

  return (unit_rdy);
}

/********************************************************************************************************
 *                                           FS_SCSI_CmdRdCapacity()
 *
 * Description : Send command READ CAPACITY to the logical unit.
 *
 * Argument(s) : p_lu    Pointer to logical unit.
 *
 *               p_err   Error pointer.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) See 'SCSI Block Commands - 3 (SBC-3)', section 5.12 for more details about SCSI
 *                   "READ CAPAPITY (10)" command.
 *
 *               (2) See Note #3 of function FS_SCSI_Rd().
 ******************************************************************************************\**************/
static void FS_SCSI_CmdRdCapacity(FS_SCSI_LU *p_lu,
                                  RTOS_ERR   *p_err)
{
  CPU_INT08U  *p_resp_buf;
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_RD_CAPACITY_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;
  RTOS_ERR    local_err;

  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_READ_CAPACITY_10;
  //                                                               ------------------- GET RESP BUF -------------------
  p_resp_buf = (CPU_INT08U *)p_lu->TransportApiPtr->RespBufGet(SCSI_CMD_READ_CAPACITY_RESP_DATA_LEN,
                                                               p_lu->TranportArgPtr,
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error getting a resp buf for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }
  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Rx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf,
                            SCSI_CMD_RD_CAPACITY_LEN,
                            (void *) p_resp_buf,
                            SCSI_CMD_READ_CAPACITY_RESP_DATA_LEN,
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error sending SCSI RD_CAPACITY cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                          // Translate Transport layer err in IO err.
    goto end_free;
  }
  if (cmd_status == DEF_FAIL) {
    goto end_free;
  }
  //                                                               ----------------- PROCESS RESP BUF -----------------
  //                                                               Store relevant param data content of this cmd.
  MEM_VAL_COPY_GET_INT32U_BIG(&p_lu->LU.Info.SecCnt, &p_resp_buf[0u]);
  p_lu->LU.Info.SecCnt += 1u;                                   // LBA is zero-based.
  MEM_VAL_COPY_GET_INT32U_BIG(&p_lu->LU.Info.SecDfltSize, &p_resp_buf[4u]);

  LOG_VRB(("Default sector size: ", (u)p_lu->LU.Info.SecDfltSize));
  LOG_VRB(("Number of sectors  : ", (u)p_lu->LU.Info.SecCnt));

end_free:
  //                                                               Rel resp buf.
  p_lu->TransportApiPtr->RespBufFree((void *)p_resp_buf,
                                     p_lu->TranportArgPtr,
                                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  if (cmd_status == DEF_FAIL) {                                 // Send REQ SENSE to have more details about failed cmd.
    FS_SCSI_SENSE_DATA sense_data;

    FS_SCSI_CmdReqSense(p_lu, &sense_data, p_err);
    if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
        || ((sense_data.SenseKey != SCSI_SENSE_KEY_NO_SENSE)
            && (sense_data.SenseKey != SCSI_SENSE_KEY_RECOVERED_ERROR))) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                        // See Note #2.
    }
  }
}

/****************************************************************************************************//**
 *                                           FS_SCSI_CmdReqSense()
 *
 * @brief    Send command REQUEST SENSE to the logical unit.
 *
 * @param    p_lu    Pointer to logical unit.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) See 'SCSI Primary Commands - 3 (SBC-3)', section 6.27 for more details about SCSI
 *               "REQUEST SENSE" command.
 *
 * @note     (2) See 'SCSI Primary Commands - 3 (SBC-3)', section 4.5.3 for more details about the
 *               fixed format sense data of SCSI "REQUEST SENSE" command.
 *******************************************************************************************************/
static void FS_SCSI_CmdReqSense(FS_SCSI_LU         *p_lu,
                                FS_SCSI_SENSE_DATA *p_sense_data,
                                RTOS_ERR           *p_err)
{
  CPU_INT08U  *p_resp_buf;
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_REQ_SENSE_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;
  CPU_INT08U  resp_code;
  RTOS_ERR    local_err;

  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_REQUEST_SENSE;
  //                                                               Fixed format sense data returned by dev.
  DEF_BIT_CLR(cmd_blk_buf[0u], (CPU_INT08U)SCSI_CMD_REQ_SENSE_BIT_DESC);
  cmd_blk_buf[4u] = SCSI_CMD_REQ_SENSE_RESP_DATA_LEN;
  //                                                               ------------------- GET RESP BUF -------------------
  p_resp_buf = (CPU_INT08U *)p_lu->TransportApiPtr->RespBufGet(SCSI_CMD_REQ_SENSE_RESP_DATA_LEN,
                                                               p_lu->TranportArgPtr,
                                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error getting a resp buf for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Rx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf[0u],
                            SCSI_CMD_REQ_SENSE_LEN,
                            (void *)p_resp_buf,
                            SCSI_CMD_REQ_SENSE_RESP_DATA_LEN,
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
      || (cmd_status == DEF_FAIL)) {
    LOG_ERR(("Error sending SCSI REQUEST SENSE cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                          // Translate Transport layer err in IO err.
    goto end_free;
  }
  //                                                               ----------------- PROCESS RESP BUF -----------------
  //                                                               Get relevant sense data (see Note #2).
  resp_code = p_resp_buf[0u] & 0x7F;
  if ((resp_code != SCSI_CMD_REQ_SENSE_RESP_DATA_ERR_CUR)
      && (resp_code != SCSI_CMD_REQ_SENSE_RESP_DATA_ERR_DEFERRED)) {
    LOG_ERR(("Invalid sense data. Should be a fixed format for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr));
    goto end_free;
  }

  p_sense_data->SenseKey = p_resp_buf[2u] & 0x0Fu;
  p_sense_data->AddidionalSenseCode = p_resp_buf[12u];
  p_sense_data->AddidionalSenseCodeQual = p_resp_buf[13u];

#if LOG_VRB_IS_EN()
  FS_SCSI_SenseDataPrint(p_lu, p_sense_data);
#endif

end_free:
  //                                                               Rel resp buf.
  p_lu->TransportApiPtr->RespBufFree((void *)p_resp_buf,
                                     p_lu->TranportArgPtr,
                                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                           FS_SCSI_CmdSynchCache()
 *
 * @brief    Send command SYNCHRONIZE CACHE to the logical unit.
 *
 * @param    p_lu    Pointer to logical unit.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) See 'SCSI Block Commands - 3 (SBC-3)', section 5.20 for more details about SCSI
 *               "SYNCHRONIZE CACHE (10)" command.
 *
 * @note     (2) SYNCHRONIZE CACHE support is determined by the first command SYNCHRONIZE CACHE
 *               sent to the target device. If this one accepts the command, a flag is set indicating
 *               the support and subsequent synchronize cache will occur. If the command fails on
 *               first attempt, the SCSI REQUEST SENSE command is sent to confirm the support or not.
 *               The flag is cleared and any subsequent synchronize cache will be skipped.
 *
 * @note     (3) Even if the SCSI READ command has reported a failed command, if the sense data
 *               indicates NO SENSE or RECOVERED ERROR, the command is considered as successful.
 *               Hence the SCSI SYNCHRONIZE CACHE command is supported.
 *
 *               See 'SCSI Primary Commands - 3 (SBC-3)', section 4.5.6 for more details about sense
 *               keys description.
 *******************************************************************************************************/

#if (FS_STORAGE_CFG_RD_ONLY_EN == DEF_DISABLED)
static void FS_SCSI_CmdSynchCache(FS_SCSI_LU *p_lu,
                                  RTOS_ERR   *p_err)
{
  CPU_INT08U  cmd_blk_buf[SCSI_CMD_SYNCHRONIZE_CACHE_10_LEN] = { 0u };
  CPU_BOOLEAN cmd_status;

  //                                                               ----------------- PREPARE CMD BLK ------------------
  cmd_blk_buf[0u] = SCSI_CMD_SYNCHRONIZE_CACHE_10;
  //                                                               Other important fields set to 0:
  //                                                               SYNC_NV = 0: sync cache to medium.
  //                                                               NUMBER OF LOGICAL BLOCKS = 0: all blks sync'ed.

  //                                                               ----------- SEND CMD TO TRANSPORT LAYER ------------
  p_lu->TransportApiPtr->Tx(p_lu->LU_Nbr,
                            (void *)&cmd_blk_buf,
                            SCSI_CMD_SYNCHRONIZE_CACHE_10_LEN,
                            DEF_NULL,
                            0u,                                 // No data phase.
                            SCSI_CMD_TIMEOUT_mS,
                            p_lu->TranportArgPtr,
                            &cmd_status,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Error sending SCSI SYNCHRONIZE_CACHE cmd to transport layer for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr, " w/ err = ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                          // Translate Transport layer err in IO err.
    return;
  }

  if (cmd_status == DEF_FAIL) {                                 // Send REQ SENSE to have more details about failed cmd.
    FS_SCSI_SENSE_DATA sense_data;

    FS_SCSI_CmdReqSense(p_lu, &sense_data, p_err);              // See Note #2.
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    } else if ((sense_data.SenseKey == SCSI_SENSE_KEY_NO_SENSE)
               && (sense_data.SenseKey == SCSI_SENSE_KEY_RECOVERED_ERROR)) {
      p_lu->SyncCache = DEF_YES;                                // See Note #3.
    } else {
      p_lu->SyncCache = DEF_NO;
    }
  } else {
    p_lu->SyncCache = DEF_YES;
  }
}
#endif

/****************************************************************************************************//**
 *                                       FS_SCSI_SenseDataAnalyze()
 *
 * @brief    Analyze the sense data obtained with the command REQUEST SENSE and print some
 *           information.
 *
 * @param    p_lu            Pointer to logical unit.
 *
 * @param    p_sense_data    Pointer to sense data.
 *
 * @note     (1) See 'SCSI Primary Commands - 3 (SBC-3)', section 4.5.6 for more details about sense
 *               key and sense code definitions.
 *******************************************************************************************************/

#if LOG_VRB_IS_EN()
static void FS_SCSI_SenseDataPrint(FS_SCSI_LU         *p_lu,
                                   FS_SCSI_SENSE_DATA *p_sense_data)
{
  if ((p_sense_data->SenseKey == SCSI_SENSE_KEY_NOT_RDY)
      || (p_sense_data->SenseKey == SCSI_SENSE_KEY_UNIT_ATTENTION)) {
    switch (p_sense_data->AddidionalSenseCode) {
      case SCSI_ASC_NOT_RDY_TO_RDY_CHANGE:
        LOG_DBG(("Medium going into ready state for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr));
        break;

      case SCSI_ASC_MEDIUM_NOT_PRESENT:
        LOG_DBG(("Medium NOT present for Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr));
        break;

      default:                                                  // See Note #1.
        LOG_DBG(("Medium state unknown for Dev #", (u)p_lu->DevID,
                 "/LU #", (u)p_lu->LU_Nbr,
                 ". Check sense key (0x", (X)p_sense_data->SenseKey,
                 "), ASC (0x", (X)p_sense_data->AddidionalSenseCode,
                 ") and ASCQ (0x", (X)p_sense_data->AddidionalSenseCodeQual, ") values in SCSI spec for more details."));
        if (p_sense_data->SenseKey < (SCSI_SENSE_KEY_MISCOMPARE + 1u)) {
          LOG_VRB(("Sense key description: ", (s)FS_SCSI_ReqSense_SenseKey_StrPtrTbl[p_sense_data->SenseKey]));
        }
        break;
    }
  } else if (p_sense_data->SenseKey == SCSI_SENSE_KEY_ILLEGAL_REQUEST) {
    LOG_DBG(("Illegal request sent to Dev #", (u)p_lu->DevID, "/LU #", (u)p_lu->LU_Nbr));
  } else {
    LOG_DBG(("Medium state unknown for Dev #", (u)p_lu->DevID,
             "/LU #", (u)p_lu->LU_Nbr,
             ". Check sense key (", (X)p_sense_data->SenseKey,
             "), ASC (", (X)p_sense_data->AddidionalSenseCode,
             ") and ASCQ (", (X)p_sense_data->AddidionalSenseCodeQual, ") values in SCSI spec for more details."));
    if (p_sense_data->SenseKey < (SCSI_SENSE_KEY_MISCOMPARE + 1u)) {
      LOG_VRB(("Sense key description: ", (s)FS_SCSI_ReqSense_SenseKey_StrPtrTbl[p_sense_data->SenseKey]));
    }
  }

  PP_UNUSED_PARAM(p_lu);
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_STORAGE_SCSI_AVAIL
