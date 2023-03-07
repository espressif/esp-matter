/***************************************************************************//**
 * @file
 * @brief CANopen Emergency (EMCY) Object Service
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

/****************************************************************************************************//**
 * @defgroup CANOPEN_COMM_OBJ CANopen Communication Object API
 * @ingroup CANOPEN
 * @brief CANopen Communication object API
 *
 * @addtogroup CANOPEN_COMM_OBJ
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_EMCY_H_
#define _CANOPEN_EMCY_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <canopen_cfg.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

#include  <canopen/include/canopen_types.h>
#include  <canopen/include/canopen_obj.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) The CANopen CiA-301 specification allows up to 65535 emergency error codes (cf. 'Table
 *               26: Emergency error codes'). The CANopen stack internally limits to 255 possible error
 *               codes which should satisfy a large number of CANopen devices designs.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_EMCY_MAX_ERR_CODE                  256u        // See Note #1.
#define  CANOPEN_EMCY_MAN_SPECIFIC_ERR_FIELD_LEN    5u          // Manufacturer-specific error field len in EMCY msg.

/********************************************************************************************************
 *                                           EMERGENCY CODE
 *
 * Note(s) : (1) The following defines holds the base values for the pre-defined standard error groups.
 *               The values can be used to simplify the definition of standard conform EMCY error codes.
 *******************************************************************************************************/

#define  CANOPEN_EMCY_CODE_NO_ERR                0x0000         // error reset or no error
#define  CANOPEN_EMCY_CODE_GEN_ERR               0x1000         // generic error
#define  CANOPEN_EMCY_CODE_CUR_ERR               0x2000         // current
#define  CANOPEN_EMCY_CODE_CUR_INPUT_ERR         0x2100         // current, device input side
#define  CANOPEN_EMCY_CODE_CUR_INTERN_ERR        0x2200         // current inside the device
#define  CANOPEN_EMCY_CODE_CUR_OUTPUT_ERR        0x2300         // current, device output side
#define  CANOPEN_EMCY_CODE_VOL_ERR               0x3000         // voltage
#define  CANOPEN_EMCY_CODE_VOL_INPUT_ERR         0x3100         // mains voltage
#define  CANOPEN_EMCY_CODE_VOL_INTERN_ERR        0x3200         // voltage inside the device
#define  CANOPEN_EMCY_CODE_VOL_OUTPUT_ERR        0x3300         // output voltage
#define  CANOPEN_EMCY_CODE_TEMP_ERR              0x4000         // temperature
#define  CANOPEN_EMCY_CODE_TEMP_AMBIENT_ERR      0x4100         // ambient temperature
#define  CANOPEN_EMCY_CODE_TEMP_DEVICE_ERR       0x4200         // device temperature
#define  CANOPEN_EMCY_CODE_HW_ERR                0x5000         // device hardware
#define  CANOPEN_EMCY_CODE_SW_ERR                0x6000         // device software
#define  CANOPEN_EMCY_CODE_SW_INTERNAL_ERR       0x6100         // internal software
#define  CANOPEN_EMCY_CODE_SW_USER_ERR           0x6200         // user software
#define  CANOPEN_EMCY_CODE_SW_DATASET_ERR        0x6300         // data set
#define  CANOPEN_EMCY_CODE_ADD_MODULES_ERR       0x7000         // additional modules
#define  CANOPEN_EMCY_CODE_MON_ERR               0x8000         // monitoring
#define  CANOPEN_EMCY_CODE_MON_COM_ERR           0x8100         // communication
#define  CANOPEN_EMCY_CODE_MON_COM_OVERRUN_ERR   0x8110         // CAN overrun (objects lost)
#define  CANOPEN_EMCY_CODE_MON_COM_PASSIVE_ERR   0x8120         // CAN in error passive mode
#define  CANOPEN_EMCY_CODE_MON_COM_HEARTBEAT_ERR 0x8130         // life guard error or heartbeat error
#define  CANOPEN_EMCY_CODE_MON_COM_RECOVER_ERR   0x8140         // recovered from bus off
#define  CANOPEN_EMCY_CODE_MON_COM_COLLISION_ERR 0x8150         // transmit COB-ID collision
#define  CANOPEN_EMCY_CODE_MON_PROT_ERR          0x8200         // protocol error
#define  CANOPEN_EMCY_CODE_MON_PROT_PDO_IGN_ERR  0x8210         // PDO not processed due to length error
#define  CANOPEN_EMCY_CODE_MON_PROT_PDO_LEN_ERR  0x8220         // PDO length exceeded
#define  CANOPEN_EMCY_CODE_EXT_ERR               0x9000         // external error
#define  CANOPEN_EMCY_CODE_ADD_FUNC_ERR          0xF000         // additional functions
#define  CANOPEN_EMCY_CODE_DEV_ERR               0xFF00         // device specific

/********************************************************************************************************
 *                                       EMERGENCY REGISTER BITS
 *
 * Note(s) : (1) The following defines holds the bit-number within the error register object (1001h) in
 *               the object directory. These values can be used to specify a standard conform EMCY
 *               error code relation table.
 *******************************************************************************************************/

#define  CANOPEN_EMCY_REG_GENERAL       0                       // general error (includes all classes)

#if (CANOPEN_EMCY_REG_CLASS_EN == DEF_ENABLED)
#define  CANOPEN_EMCY_REG_CURRENT       1                       // error class: current
#define  CANOPEN_EMCY_REG_VOLTAGE       2                       // error class: voltage
#define  CANOPEN_EMCY_REG_TEMP          3                       // error class: temperature
#define  CANOPEN_EMCY_REG_COM           4                       // error class: communication
#define  CANOPEN_EMCY_REG_PROFILE       5                       // error class: profile specific error
#define  CANOPEN_EMCY_REG_MANUFACTURER  7                       // error class: manufacturer specific
#define  CANOPEN_EMCY_REG_QTY           8                       // number of supported error classes
#else
#define  CANOPEN_EMCY_REG_QTY           1                       // number of supported error classes
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   CANopen emergency user structure
 *
 * @note    (1) This structure holds the optional manufacturer specific fields for the EMCY message
 *               and for the EMCY history.
 *
 * @note    (2) To reduce memory consumption, the configuration values of CANOPEN_EMCY_HIST_MAN_EN
 *              and CANOPEN_EMCY_HIST_MAN_EN may remove some bytes. If no manufacturer specific
 *              field is enabled, the structure holds an unused dummy byte.
 *******************************************************************************************************/

typedef struct canopen_emcy_usr {
#if (CANOPEN_EMCY_HIST_MAN_EN == DEF_ENABLED)
  CPU_INT16U Hist;                                              ///< Manufacturer specific field in History.
#endif
#if (CANOPEN_EMCY_EMCY_MAN_EN == DEF_ENABLED)
  CPU_INT08U Emcy[CANOPEN_EMCY_MAN_SPECIFIC_ERR_FIELD_LEN];     ///< Manufacturer specific field in EMCY message.
#endif
#if ((CANOPEN_EMCY_HIST_MAN_EN == DEF_DISABLED) && (CO_EMCY_EMCY_MAN_EN == DEF_DISABLED))
  CPU_INT08U Dummy;                                             ///< Manufacturer specific field is unused.
#endif
} CANOPEN_EMCY_USR;

/*
 ********************************************************************************************************
 *                                 CANopen emergency history structure
 *******************************************************************************************************/

typedef struct canopen_emcy_hist {
  CPU_INT08U TotLen;                                            ///< Total length of EMCY history.
  CPU_INT08U HistQty;                                           ///< Number of EMCY in history.
  CPU_INT08U Offset;                                            ///< Sub-index-Offset to newest EMCY entry.
} CANOPEN_EMCY_HIST;

/****************************************************************************************************//**
 *                                 CANopen emergency table structure
 *
 * @note    (1)  Some pre-defined standard error code values are listed in the section EMERGENCY CODE
 *******************************************************************************************************/

typedef struct canopen_emcy_tbl {
  CPU_INT08U Reg;                                               ///< Bit number (0..7) in error register.
  CPU_INT16U Code;                                              ///< Error code (See Note #1).
} CANOPEN_EMCY_TBL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
void CANopen_EmcySet(CANOPEN_NODE_HANDLE node_handle,
                     CPU_INT08U          err_code_ix,
                     CANOPEN_EMCY_USR    *p_user,
                     RTOS_ERR            *p_err);

void CANopen_EmcyClr(CANOPEN_NODE_HANDLE node_handle,
                     CPU_INT08U          err_code_ix,
                     RTOS_ERR            *p_err);

CPU_INT16S CANopen_EmcyGet(CANOPEN_NODE_HANDLE node_handle,
                           CPU_INT08U          err_code_ix,
                           RTOS_ERR            *p_err);

CPU_INT16S CANopen_EmcyCnt(CANOPEN_NODE_HANDLE node_handle,
                           RTOS_ERR            *p_err);

void CANopen_EmcyReset(CANOPEN_NODE_HANDLE node_handle,
                       RTOS_ERR            *p_err);
#endif

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
void CANopen_EmcyHistReset(CANOPEN_NODE_HANDLE node_handle);
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_EMCY_H_
