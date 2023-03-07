/***************************************************************************//**
 * @file
 * @brief Common - RTOS Error Type & Utils
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

/****************************************************************************************************//**
 * @defgroup COMMON_RTOS_ERR RTOS_ERR API
 * @ingroup  COMMON
 * @brief      RTOS_ERR API
 *
 * @addtogroup COMMON_RTOS_ERR
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 * Note(s) : (1) This include guard must allow the file to be re-included if and only if
 *               RTOS_ERR_STR_MODULE has been defined. The RTOS_ERR_STR_MODULE MUST only be defined by the
 *               rtos_err_str.c file, in order to automatically create the table of error strings that
 *               can be used to print an error string instead of an error value.
 *
 *           (2) This list of error codes and strings associated is used by file rtos_err_str.c to fill
 *               a table containing string of error codes. The RTOS_ERR_STR_MODULE define indicates
 *               whether the current file is used 'normally' (to include the RTOS_ERR enum) or to generate
 *               the error strings.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Include guard, see Note #1.
#if !defined(_RTOS_ERR_H_) \
  || defined(RTOS_ERR_STR_MODULE)
#define  _RTOS_ERR_H_

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
#include  <cpu/include/cpu_port_sel.h>

#include  <common/include/lib_def.h>
#include  <common/include/toolchains.h>

#include  <common/include/rtos_path.h>
#include  <rtos_err_cfg.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
extern const CPU_CHAR *  const RTOS_ErrStrTbl[];
extern const CPU_CHAR *  const RTOS_ErrDescStrTbl[];
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               ERR STR MACROS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           RTOS_ERR_STR_GET()
 *
 * @brief    Obtains the error string or the error description string, respectively, from an error
 *           code. Will return a string saying strings are unavailable if RTOS_ERR_CFG_STR_EN is set
 *           to DEF_DISABLED.
 *
 * @param    err_code    Error code for which to obtain the string.
 *
 * @return   The required string (either the error or the description).
 *******************************************************************************************************/

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
    #define  RTOS_ERR_STR_GET(err_code)         RTOS_ErrStrTbl[(err_code)]
    #define  RTOS_ERR_DESC_STR_GET(err_code)    RTOS_ErrDescStrTbl[(err_code)]
#else
    #define  RTOS_ERR_STR_UNAVAIL               "Error string unavailable."
    #define  RTOS_ERR_STR_GET(err_code)         RTOS_ERR_STR_UNAVAIL
    #define  RTOS_ERR_DESC_STR_GET(err_code)    RTOS_ERR_STR_UNAVAIL
#endif
#endif

/********************************************************************************************************
 *                                               EXT ERR DEFINES
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           RTOS_ERR_CODE_GET()
 *
 * @brief    Obtains the error code from an error variable.
 *
 * @param    err_var     Error variable from which to obtain error code.
 *
 * @return   Error code in passed variable.
 *******************************************************************************************************/

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
#define  RTOS_ERR_CODE_GET(err_var)                     (err_var).Code
#endif

/****************************************************************************************************//**
 *                                               RTOS_ERR_SET()
 *
 * @brief    Sets a given error variable to an error value. Also sets the extended error, if enabled.
 *
 * @param    err_var     Error variable to set to 'err'.
 *
 * @param    err_code    Error value to which to set 'err_var'.
 *******************************************************************************************************/

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
#if (RTOS_ERR_CFG_EXT_EN == DEF_ENABLED)
  #if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
    #ifdef PP_C_STD_VERSION_C99_PRESENT                         // Only present if C99 enabled.
      #define  RTOS_ERR_SET(err_var, err_code)            (err_var).Code = (err_code); \
  (err_var).CodeText = RTOS_ERR_STR_GET((err_code));                                   \
  (err_var).DescText = RTOS_ERR_DESC_STR_GET((err_code));                              \
  (err_var).FileName = __FILE__;                                                       \
  (err_var).LineNbr = __LINE__;                                                        \
  (err_var).FnctName = &__func__[0]

      #define  RTOS_ERR_INIT_CODE(err_code)               { .Code = (err_code),                            \
                                                            .CodeText = RTOS_ERR_STR_GET((err_code)),      \
                                                            .DescText = RTOS_ERR_DESC_STR_GET((err_code)), \
                                                            .FileName = __FILE__,                          \
                                                            .LineNbr = __LINE__,                           \
                                                            .FnctName = &__func__[0] }
    #else
      #define  RTOS_ERR_SET(err_var, err_code)            (err_var).Code = (err_code); \
  (err_var).CodeText = RTOS_ERR_STR_GET((err_code));                                   \
  (err_var).DescText = RTOS_ERR_DESC_STR_GET((err_code));                              \
  (err_var).FileName = __FILE__;                                                       \
  (err_var).LineNbr = __LINE__

      #define  RTOS_ERR_INIT_CODE(err_code)               { .Code = (err_code),                            \
                                                            .CodeText = RTOS_ERR_STR_GET((err_code)),      \
                                                            .DescText = RTOS_ERR_DESC_STR_GET((err_code)), \
                                                            .FileName = __FILE__,                          \
                                                            .LineNbr = __LINE__ }
    #endif
  #else
    #ifdef PP_C_STD_VERSION_C99_PRESENT                         // Only present if C99 enabled.
      #define  RTOS_ERR_SET(err_var, err_code)            (err_var).Code = (err_code); \
  (err_var).FileName = __FILE__;                                                       \
  (err_var).LineNbr = __LINE__;                                                        \
  (err_var).FnctName = &__func__[0]

      #define  RTOS_ERR_INIT_CODE(err_code)               { .Code = (err_code),   \
                                                            .FileName = __FILE__, \
                                                            .LineNbr = __LINE__,  \
                                                            .FnctName = &__func__[0] }
    #else
      #define  RTOS_ERR_SET(err_var, err_code)            (err_var).Code = (err_code); \
  (err_var).FileName = __FILE__;                                                       \
  (err_var).LineNbr = __LINE__

      #define  RTOS_ERR_INIT_CODE(err_code)               { .Code = (err_code),   \
                                                            .FileName = __FILE__, \
                                                            .LineNbr = __LINE__ }
    #endif
  #endif
#else
  #define  RTOS_ERR_SET(err_var, err_code)                    (err_var).Code = (err_code)

  #define  RTOS_ERR_INIT_CODE(err_code)                       { .Code = (err_code) }
#endif
#endif

/****************************************************************************************************//**
 *                                     RTOS_ERR_CONTEXT_REFRESH()
 *
 * @brief    Refreshes extended error information with current context.
 *
 * @param    err_var     Error variable to refresh.
 *******************************************************************************************************/

#if (RTOS_ERR_CFG_EXT_EN == DEF_ENABLED)
    #ifdef PP_C_STD_VERSION_C99_PRESENT                         // Only present if C99 enabled.
        #define  RTOS_ERR_CONTEXT_REFRESH(err_var)          (err_var).FileName = __FILE__; \
  (err_var).LineNbr = __LINE__;                                                            \
  (err_var).FnctName = &__func__[0]
    #else
        #define  RTOS_ERR_CONTEXT_REFRESH(err_var)          (err_var).FileName = __FILE__; \
  (err_var).LineNbr = __LINE__
    #endif
#else
    #define  RTOS_ERR_CONTEXT_REFRESH(err_var)
#endif

/****************************************************************************************************//**
 *                                               RTOS_ERR_COPY()
 *
 * @brief    Copies every field of an error variable to another one.
 *
 * @param    err_dst     Destination error variable.
 *
 * @param    err_src     Source error variable.
 *
 * @note     (1) This wrapper is present if other fields require particular manipulations to copy,
 *               even if a simple assignation could have been enough for the moment.
 *******************************************************************************************************/

#define  RTOS_ERR_COPY(err_dst, err_src)                (err_dst) = (err_src)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               ERR CODES
 *
 * @note     (1) User should NEVER rely on the order/numerical value of ANY of these enum items. Their
 *               order within the enum may change and other enum item may be added anywhere, impacting
 *               the numerical values of other enum items.
 *
 * @note     (2) A function MUST return the most accurate error it can. For example, if a NULL pointer is
 *               passed to a function requiring a non-NULL pointer, it should return the RTOS_ERR_NULL_PTR
 *               error and NOT RTOS_ERR_INVALID_ARG. A function should only return RTOS_ERR_INVALID_ARG
 *               if no other error code can better describe the error that occurred.
 *******************************************************************************************************/

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
#define  RTOS_ERR_DECLARE(err, str)  err,
typedef enum rtos_err_code {
#endif

RTOS_ERR_DECLARE (RTOS_ERR_NONE, "No error.")

RTOS_ERR_DECLARE (RTOS_ERR_FAIL, "Generic failure for operation.")
RTOS_ERR_DECLARE (RTOS_ERR_NOT_READY, "Module is not ready for requested operation.")

RTOS_ERR_DECLARE (RTOS_ERR_ASSERT_DBG_FAIL, "Debug assertion failed.")
RTOS_ERR_DECLARE (RTOS_ERR_ASSERT_CRITICAL_FAIL, "Critical assertion failed.")
RTOS_ERR_DECLARE (RTOS_ERR_ASSERT_ERR_PTR_NULL, "Pointer to error variable to return error code is null.")

//                                                                 ------------------ FEATURE SUPPORT -----------------
RTOS_ERR_DECLARE (RTOS_ERR_NOT_AVAIL, "Feature not avail (due to cfg val(s)).")
RTOS_ERR_DECLARE (RTOS_ERR_NOT_SUPPORTED, "Feature not supported.")

//                                                                 ------------------- INVALID ARGS -------------------
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_ARG, "Invalid argument or consequence of invalid argument.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_CFG, "Invalid configuration provided.")
RTOS_ERR_DECLARE (RTOS_ERR_NULL_PTR, "Invalid null pointer received as argument.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_HANDLE, "Invalid handle passed to function.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_TYPE, "Invalid type for operation.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_IX, "Invalid index.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_CREDENTIALS, "Credentials used are invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_NOT_FOUND, "Requested item could not be found.")

//                                                                 ------------- INIT/CREATION/ALLOCATION -------------
RTOS_ERR_DECLARE (RTOS_ERR_ALLOC, "Generic allocation error.")
RTOS_ERR_DECLARE (RTOS_ERR_NO_MORE_RSRC, "Resource not available to perform the operation.")
RTOS_ERR_DECLARE (RTOS_ERR_INIT, "Initialization failed.")
RTOS_ERR_DECLARE (RTOS_ERR_NOT_INIT, "Module has not been initialized.")
RTOS_ERR_DECLARE (RTOS_ERR_ALREADY_INIT, "Module has already been initialized.")
RTOS_ERR_DECLARE (RTOS_ERR_ALREADY_EXISTS, "Item already exists.")
RTOS_ERR_DECLARE (RTOS_ERR_SEG_OVF, "Mem seg would overflow.")
RTOS_ERR_DECLARE (RTOS_ERR_POOL_FULL, "Mem pool full;  i.e. all mem blks avail in mem pool.")
RTOS_ERR_DECLARE (RTOS_ERR_POOL_EMPTY, "Mem pool empty; i.e. NO  mem blks avail in mem pool.")
RTOS_ERR_DECLARE (RTOS_ERR_POOL_UNLIMITED, "Mem pool is unlimited, cannot obtain rem nbr of blks.")
RTOS_ERR_DECLARE (RTOS_ERR_BLK_ALLOC_CALLBACK, "Block alloc callback failed.")

//                                                                 ----------------- CONTEXT INVALID ------------------
RTOS_ERR_DECLARE (RTOS_ERR_OWNERSHIP, "Ownership error.")
RTOS_ERR_DECLARE (RTOS_ERR_PERMISSION, "Operation not allowed.")
RTOS_ERR_DECLARE (RTOS_ERR_WOULD_BLOCK, "Non-blocking operation would block.")
RTOS_ERR_DECLARE (RTOS_ERR_WOULD_OVF, "Item would overflow.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_STATE, "Cannot execute requested operation while in current state.")
RTOS_ERR_DECLARE (RTOS_ERR_ISR, "Illegal call from ISR.")

//                                                                 ----------------- SYNCHRONIZATION ------------------
RTOS_ERR_DECLARE (RTOS_ERR_ABORT, "Operation aborted.")
RTOS_ERR_DECLARE (RTOS_ERR_TIMEOUT, "Operation timed out.")
RTOS_ERR_DECLARE (RTOS_ERR_IS_OWNER, "Already/still owning resource.")
RTOS_ERR_DECLARE (RTOS_ERR_NONE_WAITING, "No task waiting/pending for that action/event.")

//                                                                 ------------------ COMMUNICATION -------------------
RTOS_ERR_DECLARE (RTOS_ERR_IO, "Generic I/O err.")
RTOS_ERR_DECLARE (RTOS_ERR_IO_FATAL, "Generic I/O fatal err.")
RTOS_ERR_DECLARE (RTOS_ERR_IO_TIMEOUT, "Generic I/O timeout err.")
RTOS_ERR_DECLARE (RTOS_ERR_TX, "Generic Tx err.")
RTOS_ERR_DECLARE (RTOS_ERR_RX, "Generic Rx err.")
RTOS_ERR_DECLARE (RTOS_ERR_RX_OVERRUN, "Overrun error, in Rx.")
RTOS_ERR_DECLARE (RTOS_ERR_OBJ_RD, "Failed to read on/via given object.")
RTOS_ERR_DECLARE (RTOS_ERR_OBJ_WR, "Failed to write on/via given object.")

//                                                                 ------------------- OS-SPECIFIC --------------------
RTOS_ERR_DECLARE (RTOS_ERR_OS_SCHED_LOCKED, "Operation cannot be done when the scheduler is locked.")
RTOS_ERR_DECLARE (RTOS_ERR_OS_TASK_WAITING, "Operation not allowed when tasks are waiting/pending on OS object.")
RTOS_ERR_DECLARE (RTOS_ERR_OS_TASK_SUSPENDED, "Task is still suspended.")
RTOS_ERR_DECLARE (RTOS_ERR_OS_ILLEGAL_RUN_TIME, "Operation not allowed after call to OSStart.")
RTOS_ERR_DECLARE (RTOS_ERR_OS_OBJ_DEL, "Object has been deleted during pend.")
RTOS_ERR_DECLARE (RTOS_ERR_OS, "Generic OS err.")

//                                                                 ------------------ SHELL-SPECIFIC ------------------
RTOS_ERR_DECLARE (RTOS_ERR_SHELL_CMD_EXEC, "Error when shell executed command.")
RTOS_ERR_DECLARE (RTOS_ERR_CMD_EMPTY, "Shell command is empty.")

RTOS_ERR_DECLARE (RTOS_ERR_TIME_INVALID, "Time is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_SIZE_INVALID, "Size is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_NAME_INVALID, "Name contains illegal characters.")

RTOS_ERR_DECLARE (RTOS_ERR_ECC_CORR, "Correctable ECC error.")
RTOS_ERR_DECLARE (RTOS_ERR_ECC_CRITICAL_CORR, "Critical correctable ECC error.")
RTOS_ERR_DECLARE (RTOS_ERR_ECC_UNCORR, "Uncorrectable ECC error.")

//                                                                 ------------------- FS-SPECIFIC --------------------
RTOS_ERR_DECLARE (RTOS_ERR_ENTRY_PARENT_NOT_DIR, "Entry parent must be a directory.")
RTOS_ERR_DECLARE (RTOS_ERR_ENTRY_ROOT_DIR, "Operation not allowed on root directory.")
RTOS_ERR_DECLARE (RTOS_ERR_ENTRY_MAX_DEPTH_EXCEEDED, "Maximum directory tree depth exceeded.")
RTOS_ERR_DECLARE (RTOS_ERR_ENTRY_OPENED, "Operation not allowed on opened entry.")
RTOS_ERR_DECLARE (RTOS_ERR_ENTRY_CLOSED, "Operation not allowed on closed entry.")

RTOS_ERR_DECLARE (RTOS_ERR_FILE_ACCESS_MODE_INVALID, "File access mode is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_FILE_ERR_STATE, "Operation cannot be performed on a file in error state.")

RTOS_ERR_DECLARE (RTOS_ERR_DIR_FULL, "Directory is full.")
RTOS_ERR_DECLARE (RTOS_ERR_DIR_NOT_EMPTY, "Directory containes files or sub-directories.")

RTOS_ERR_DECLARE (RTOS_ERR_WRK_DIR_CLOSED, "Working directory is closed.")

RTOS_ERR_DECLARE (RTOS_ERR_VOL_FMT_INVALID, "Volume format is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_VOL_CORRUPTED, "Volume metadata is corrupted.")
RTOS_ERR_DECLARE (RTOS_ERR_VOL_OPENED, "Operation not allowed on opened volume.")
RTOS_ERR_DECLARE (RTOS_ERR_VOL_CLOSED, "Operation not allowed on closed volume.")
RTOS_ERR_DECLARE (RTOS_ERR_VOL_RD_ONLY, "Volume is read-only.")
RTOS_ERR_DECLARE (RTOS_ERR_VOL_FULL, "Volume is full.")

RTOS_ERR_DECLARE (RTOS_ERR_PARTITION_INVALID, "Partition is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_PARTITION_MAX_EXCEEDED, "Maximum partition count exceeded.")

RTOS_ERR_DECLARE (RTOS_ERR_BLK_DEV_FMT_INCOMPATIBLE, "Block device low-level format is incompatible with user configuration.")
RTOS_ERR_DECLARE (RTOS_ERR_BLK_DEV_FMT_INVALID, "Block device low-level format is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_BLK_DEV_CORRUPTED, "Block device metadata is corrupted.")
RTOS_ERR_DECLARE (RTOS_ERR_BLK_DEV_OPENED, "Block device is opened.")
RTOS_ERR_DECLARE (RTOS_ERR_BLK_DEV_CLOSED, "Block device is closed.")

//                                                                 ----------------- USB ERROR CODES ------------------
RTOS_ERR_DECLARE (RTOS_ERR_DEV_ALLOC, "Device allocation failed.")
RTOS_ERR_DECLARE (RTOS_ERR_CONFIG_ALLOC, "Configuration allocation failed.")
RTOS_ERR_DECLARE (RTOS_ERR_IF_ALLOC, "Interface allocation failed.")
RTOS_ERR_DECLARE (RTOS_ERR_IF_ALT_ALLOC, "Interface allocation failed.")
RTOS_ERR_DECLARE (RTOS_ERR_IF_GRP_ALLOC, "Interface group allocation failed.")
RTOS_ERR_DECLARE (RTOS_ERR_FNCT_ALLOC, "No more class function in this config.")
RTOS_ERR_DECLARE (RTOS_ERR_EP_ALLOC, "USB core unable to allocate endpoint.")
RTOS_ERR_DECLARE (RTOS_ERR_EP_NONE_AVAIL, "Physical endpoint/pipe NOT available.")
RTOS_ERR_DECLARE (RTOS_ERR_URB_ALLOC, "USB core unable to allocate URB.")
RTOS_ERR_DECLARE (RTOS_ERR_DRV_EP_ALLOC, "USB driver/controller unable to allocate endpoint.")
RTOS_ERR_DECLARE (RTOS_ERR_DRV_URB_ALLOC, "USB driver/controller unable to allocate URB.")

RTOS_ERR_DECLARE (RTOS_ERR_CLASS_INSTANCE_ALLOC, "Unable to allocate class instance.")
RTOS_ERR_DECLARE (RTOS_ERR_SUBCLASS_INSTANCE_ALLOC, "Unable to allocate subclass instance.")
RTOS_ERR_DECLARE (RTOS_ERR_AOAP_FNCT_ALLOC, "Unable to allocate AOAP class function instance.")
RTOS_ERR_DECLARE (RTOS_ERR_CDC_FNCT_ALLOC, "Unable to allocate CDC class function instance.")
RTOS_ERR_DECLARE (RTOS_ERR_CDC_ACM_FNCT_ALLOC, "Unable to allocate CDC-ACM subclass function instance.")
RTOS_ERR_DECLARE (RTOS_ERR_USB2SER_FNCT_ALLOC, "Unable to allocate USB2Ser class function instance.")
RTOS_ERR_DECLARE (RTOS_ERR_HID_FNCT_ALLOC, "Unable to allocate HID class function instance.")
RTOS_ERR_DECLARE (RTOS_ERR_MSC_FNCT_ALLOC, "Unable to allocate MSC class function instance.")

RTOS_ERR_DECLARE (RTOS_ERR_INVALID_DEV_STATE, "Invalid device state.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_CLASS_STATE, "Invalid class state.")
RTOS_ERR_DECLARE (RTOS_ERR_INVALID_EP_STATE, "Invalid endpoint state.")
RTOS_ERR_DECLARE (RTOS_ERR_DEV_HANDLE_HAS_CHANGED, "Handle has changed since providing it to caller.")

RTOS_ERR_DECLARE (RTOS_ERR_INVALID_DESC, "Desciptor content is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_OVERFLOW_DESC, "Desciptor's size is bigger than buffer size.")

RTOS_ERR_DECLARE (RTOS_ERR_INVALID_DEV_SPD, "Invalid device speed.")
RTOS_ERR_DECLARE (RTOS_ERR_DEV_CONN_DECLINED, "Connection for device was declined by app.")
RTOS_ERR_DECLARE (RTOS_ERR_DEV_SUSPEND, "Operation failed because device is suspended.")

RTOS_ERR_DECLARE (RTOS_ERR_EP_INVALID, "Invalid endpoint (address, type or direction).")
RTOS_ERR_DECLARE (RTOS_ERR_EP_BW_NOT_AVAIL, "Not enough bandwidth available to open endpoint.")
RTOS_ERR_DECLARE (RTOS_ERR_EP_QUEUING, "Unable to queue transfer on endpoint.")
RTOS_ERR_DECLARE (RTOS_ERR_EP_STALL, "Endpoint was/is stalled.")

RTOS_ERR_DECLARE (RTOS_ERR_CLASS_DRV_NOT_FOUND, "No class driver found.")
RTOS_ERR_DECLARE (RTOS_ERR_SUBCLASS_DRV_NOT_FOUND, "No subclass driver found.")

RTOS_ERR_DECLARE (RTOS_ERR_USB2SER_FLOW_CTRL_EN, "Operation not allowed when flow control enabled.")

//                                                                 ------------------ NET-SPECIFIC --------------------
RTOS_ERR_DECLARE (RTOS_ERR_NET_INVALID_ADDR_SRC, "Address source not found to send data.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_IF_LINK_DOWN, "The interface link is down.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_RETRY_MAX, "The maximum number of retry was reached.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_ADDR_UNRESOLVED, "The stack was unable to resolved the IP address.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_ICMP_ECHO_REPLY_DATA_CMP, "Data received in echo reply is not the same as the data sent.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_NEXT_HOP, "The stack was unable to found a valid next hop to send the packet to.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_INVALID_CONN, "The socket connection is invalid.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_CONN_CLOSE_RX, "The stack received a connection closed from other half.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_CONN_CLOSED_FAULT, "The connection was closed abruptly by other half.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_OP_IN_PROGRESS, "The current operation is still in progress.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_SOCK_CLOSED, "The operation cannot be done because the socket is closed.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_STR_ADDR_INVALID, "The string address is in an invalid format.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_PHY_TIMEOUT_AUTO_NEG, "The PHY auto-negotiation timed out.")
RTOS_ERR_DECLARE (RTOS_ERR_NET_SSL, "TLS/SSL Error.")
//                                                                 MQTT-Specific.
RTOS_ERR_DECLARE (RTOS_ERR_MQTT_MSG_FAIL, "Message failed to complete correctly.")
RTOS_ERR_DECLARE (RTOS_ERR_MQTTc_QoS_LEVEL_NOT_GRANTED, "Broker failed to grant QoS level requested.")

//                                                                 ------------------- IO-SPECIFIC --------------------
RTOS_ERR_DECLARE (RTOS_ERR_FNCT_DRV_NOT_FOUND, "No class driver found.")

//                                                                 --------------- CAN/CANOPEN-SPECIFIC ---------------
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_PARAM_STORE, "Failed to store CANopen parameter.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_PARAM_RESTORE, "Failed to restore CANopen parameter.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_xPDO_COM_INVALID_CFG, "Configuration error in RPDO/TPDO communication.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_xPDO_MAP_INVALID_CFG, "Configuration error in RPDO/TPDO mapping.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_INVALID_ENTRY, "Entry is corrupted/not existing.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_LSS_LOAD, "Error during loading LSS configuration")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_LSS_NO_REQ, "No LSS request.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_LSS_IGNORED, "LSS request ignored.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_GEN_ERR, "General error.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_PARAM_INCOMP, "General parameter incompatibility in the device.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_PDO_INVALID, "Object doesn't exist in directory.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_SDO_ERR_SUB_IX, "Sub-index does not exist.")
RTOS_ERR_DECLARE (RTOS_ERR_CANOPEN_SDO_ERR_RANGE, "Value range of parameter exceeded .")

#ifndef  RTOS_ERR_STR_MODULE
} RTOS_ERR_CODE;
#endif

/****************************************************************************************************//**
 *                                               EXTENDED ERR
 * @brief   RTOS err structure
 *******************************************************************************************************/

#ifndef  RTOS_ERR_STR_MODULE                                    // If file is used to generate err strings. See Note #2.
typedef struct rtos_err {
  RTOS_ERR_CODE Code;                                           ///< Err code enum val.

#if (RTOS_ERR_CFG_EXT_EN == DEF_ENABLED)
#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
  CPU_CHAR const *CodeText;                                     ///< Err code in string fmt.
  CPU_CHAR const *DescText;                                     ///< Err desc string.
#endif
  CPU_CHAR       *FileName;                                     ///< File name where error occurred.
  CPU_INT32U     LineNbr;                                       ///< Line nbr  where error occurred.

  //                                                               Only used if C99 enabled.
  const CPU_CHAR *FnctName;                                     ///< Fnct name where err occurred.
#endif
} RTOS_ERR;

#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos err code module include.
