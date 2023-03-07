/***************************************************************************//**
 * @file
 * @brief CANopen Object Directory Management
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
 * @addtogroup CANOPEN_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_OBJ_H_
#define _CANOPEN_OBJ_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <canopen_cfg.h>

#include  <canopen/include/canopen_types.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_OBJ____R_         0x01                                // Read Only
#define  CANOPEN_OBJ_____W         0x02                                // Write Only
#define  CANOPEN_OBJ____RW         0x03                                // Read/Write
#define  CANOPEN_OBJ___P__         0x04                                // PDO Map
#define  CANOPEN_OBJ___PR_         0x05                                // Read Only, PDO Mappable
#define  CANOPEN_OBJ___P_W         0x06                                // Write Only, PDO Mappable
#define  CANOPEN_OBJ___PRW         0x07                                // Read/Write, PDO Mappable
#define  CANOPEN_OBJ__N___         0x40                                // Consider Node-Id
#define  CANOPEN_OBJ__N_R_         0x41                                // Read Only, + Node-Id
#define  CANOPEN_OBJ__N__W         0x42                                // Write Only, - Node-Id
#define  CANOPEN_OBJ__N_RW         0x43                                // Read/Write, +/- Node-Id
#define  CANOPEN_OBJ__NPR_         0x45                                // Read Only, PDO Mappable, + Node-Id
#define  CANOPEN_OBJ__NP_W         0x46                                // Write Only, PDO Mappable, - Node-Id
#define  CANOPEN_OBJ__NPRW         0x47                                // Read/Write, PDO Mappable, +/- Node-Id
#define  CANOPEN_OBJ_D____         0x80                                // Direct Access (Ptr = Value)
#define  CANOPEN_OBJ_D__R_         0x81                                // Read Only, Direct Access
#define  CANOPEN_OBJ_D___W         0x82                                // Write Only, Direct Access
#define  CANOPEN_OBJ_D__RW         0x83                                // Read/Write, Direct Access
#define  CANOPEN_OBJ_DN_R_         0xC1                                // Read Only, + Node-Id, Direct Access
#define  CANOPEN_OBJ_DN__W         0xC2                                // Write Only, - Node-Id, Direct Access
#define  CANOPEN_OBJ_DN_RW         0xC3                                // Read/Write, +/- Node-Id, Direct Access

#define  CANOPEN_OBJ_SGN           0x08                                // Signed number
#define  CANOPEN_OBJ_SZ1           0x00                                // Size 2^0 = 1 Byte
#define  CANOPEN_OBJ_SZ2           0x10                                // Size 2^1 = 2 Byte
#define  CANOPEN_OBJ_SZ4           0x20                                // Size 2^2 = 4 Byte
#define  CANOPEN_OBJ_SZ8           0x30                                // Size 2^3 = 8 Byte

#define  CANOPEN_OBJ_FLG_MSK       0xC7                                // Mask for flags
#define  CANOPEN_OBJ_SZ_MSK        0x30                                // Mask for fixed object size
#define  CANOPEN_OBJ_FMT_MSK       0x38                                // Mask for format

#define  CANOPEN_OBJ_UNSIGNED8     CANOPEN_OBJ_SZ1                     // CANopen Datatype: UNSIGNED8
#define  CANOPEN_OBJ_UNSIGNED16    CANOPEN_OBJ_SZ2                     // CANopen Datatype: UNSIGNED16
#define  CANOPEN_OBJ_UNSIGNED32    CANOPEN_OBJ_SZ4                     // CANopen Datatype: UNSIGNED32
#define  CANOPEN_OBJ_UNSIGNED64    CANOPEN_OBJ_SZ8                     // CANopen Datatype: UNSIGNED64
#define  CANOPEN_OBJ_SIGNED8       (CANOPEN_OBJ_SGN | CANOPEN_OBJ_SZ1) // CANopen Datatype: SIGNED8
#define  CANOPEN_OBJ_SIGNED16      (CANOPEN_OBJ_SGN | CANOPEN_OBJ_SZ2) // CANopen Datatype: SIGNED16
#define  CANOPEN_OBJ_SIGNED32      (CANOPEN_OBJ_SGN | CANOPEN_OBJ_SZ4) // CANopen Datatype: SIGNED32
#define  CANOPEN_OBJ_FLOAT         0                                   // CANopen Datatype: FLOAT
#define  CANOPEN_OBJ_DOMAIN        0                                   // CANopen Datatype: DOMAIN
#define  CANOPEN_OBJ_STRING        0                                   // CANopen Datatype: STRING

#define  CANOPEN_OBJ_BYTE          1                                   // Number of bytes for a byte
#define  CANOPEN_OBJ_WORD          2                                   // Number of bytes for a word
#define  CANOPEN_OBJ_LONG          4                                   // Number of bytes for a long

//                                                                 ---------- Type Control Function Code --------------
#define  CANOPEN_OBJ_CTRL_SET_OFFSET    1                              // Set read/write offset for next access

/********************************************************************************************************
 *                               COMMUNICATION PROFILE SPECIFIC OBJECTS
 *
 * Note(s) : More details about the specific objects of the Communication Profile can be found in
 *           section 7.5.2 of the specification 'CiA 301, CANopen application layer and communication
 *           profile, Version: 4.2.0'.
 *******************************************************************************************************/

//                                                                 ------------- STANDARD OBJECT INDEXES --------------
#define  CANOPEN_COMM_PROFILE_OBJ_IX_DEVICE_TYPE                     0x1000
#define  CANOPEN_COMM_PROFILE_OBJ_IX_ERR_REG                         0x1001
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MANUFACTURER_STATUS_REG         0x1002
#define  CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR                     0x1003
#define  CANOPEN_COMM_PROFILE_OBJ_IX_COB_ID_SYNC_MSG                 0x1005
#define  CANOPEN_COMM_PROFILE_OBJ_IX_COMM_CYCLE_PERIOD               0x1006
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SYNCH_WINDOW_LEN                0x1007
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MANUFACTURER_DEV_NAME           0x1008
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MANUFACTURER_HW_VERSION         0x1009
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MANUFACTURER_SW_VERSION         0x100A
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MANUFACTURER_GUARD_TIME         0x100C
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MANUFACTURER_LIFETIME_FACTOR    0x100D
#define  CANOPEN_COMM_PROFILE_OBJ_IX_STORE_PARAM                     0x1010
#define  CANOPEN_COMM_PROFILE_OBJ_IX_RESTORE_DFLT_PARAM              0x1011
#define  CANOPEN_COMM_PROFILE_OBJ_IX_COB_ID_TS_OBJ                   0x1012
#define  CANOPEN_COMM_PROFILE_OBJ_IX_HIGH_RES_TS                     0x1013
#define  CANOPEN_COMM_PROFILE_OBJ_IX_COB_ID_EMCY                     0x1014
#define  CANOPEN_COMM_PROFILE_OBJ_IX_INHIBIT_TIME_EMCY               0x1015
#define  CANOPEN_COMM_PROFILE_OBJ_IX_CONS_HB_TIME                    0x1016
#define  CANOPEN_COMM_PROFILE_OBJ_IX_PROD_HB_TIME                    0x1017
#define  CANOPEN_COMM_PROFILE_OBJ_IX_ID_OBJ                          0x1018
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SYNC_CNT_OVERFLOW_VAL           0x1019
#define  CANOPEN_COMM_PROFILE_OBJ_IX_VERIFY_CFG                      0x1020
#define  CANOPEN_COMM_PROFILE_OBJ_IX_STORE_EDS                       0x1021
#define  CANOPEN_COMM_PROFILE_OBJ_IX_STORE_FMT                       0x1022
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OS_CMD                          0x1023
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OS_CMD_MODE                     0x1024
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OS_DBG_IF                       0x1025
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OS_PROMPT                       0x1026
#define  CANOPEN_COMM_PROFILE_OBJ_IX_MODULE_LIST                     0x1027
#define  CANOPEN_COMM_PROFILE_OBJ_IX_EMCY_CONS_OBJ                   0x1028
#define  CANOPEN_COMM_PROFILE_OBJ_IX_ERR_BEHAVIOR_OBJ                0x1029
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SDO_SERVER_PARAM_START          0x1200
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SDO_SERVER_PARAM_END            0x127F
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SDO_SERVER_PARAM_RANGE          0x7F
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SDO_CLIENT_PARAM_START          0x1280
#define  CANOPEN_COMM_PROFILE_OBJ_IX_SDO_CLIENT_PARAM_END            0x12FF
#define  CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_START           0x1400
#define  CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_END             0x15FF
#define  CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_START        0x1600
#define  CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_END          0x17FF
#define  CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START           0x1800
#define  CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_END             0x19FF
#define  CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_START        0x1A00
#define  CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_END          0x1BFF
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OBJ_SCANNER_LIST_START          0x1FA0
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OBJ_SCANNER_LIST_END            0x1FCF
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OBJ_DISPATCHING_LIST_START      0x1FD0
#define  CANOPEN_COMM_PROFILE_OBJ_IX_OBJ_DISPATCHING_LIST_END        0x1FFF

/********************************************************************************************************
 *                                     OBJECT TYPE DEFINES
 *******************************************************************************************************/

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
//                                                                 Object Type EMCY History
#define  CANOPEN_OBJ_TYPE_EMCY        ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeEmcy)
#endif

#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
#define  CANOPEN_OBJ_TYPE_STR         ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeStr)
#endif

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
#define  CANOPEN_OBJ_TYPE_DOMAIN      ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeDomain)
#endif

#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) \
  || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
//                                                                 Object Type PDO Map Number
#define  CANOPEN_OBJ_TYPE_PDO_NBRS    ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypePDO_MapNbrs)
//                                                                 Object Type PDO Mapping
#define  CANOPEN_OBJ_TYPE_PDO_MAP     ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypePDO_Map)
#endif

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) \
  || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
//                                                                 Object Type Dynamic PDO Identifier
#define  CANOPEN_OBJ_TYPE_PDO_ID      ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypePDO_ID)
//                                                                 Object Type Dynamic PDO Transm. Type
#define  CANOPEN_OBJ_TYPE_PDO_TYPE    ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypePDO_Type)
#endif

#if (CANOPEN_SDO_DYN_ID_EN == DEF_ENABLED)
//                                                                 Object Type Dynamic SDO Identifier
#define CANOPEN_OBJ_TYPE_SDO_ID       ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeSdoId)
#endif
#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
//                                                                 Object Type Parameter
#define  CANOPEN_OBJ_TYPE_PARAM       ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeParam)
#endif

#if (CANOPEN_TPDO_MAX_QTY > 0)
//                                                                 Object Type Asynchronous TPDO Object
#define  CANOPEN_OBJ_TYPE_TPDO_ASYNC  ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeTpdoAsync)
//                                                                 Object Type Synchronous TPDO Object
#define  CANOPEN_OBJ_TYPE_TPDO_SYNC   ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeTpdoSync)
//                                                                 Object Type TPDO Event Timer
#define  CANOPEN_OBJ_TYPE_TPDO_EVENT  ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeTpdoEvent)
//                                                                 Object Type TPDO Invalid sub-index
#define  CANOPEN_OBJ_TYPE_TPDO_INVALID ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeTpdoInvalid)
#endif
//                                                                 Object Type Heartbeat Consumer
#define  CANOPEN_OBJ_TYPE_HB_CONS     ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeHbCons)
//                                                                 Object Type Heartbeat Producer
#define  CANOPEN_OBJ_TYPE_HB_PROD     ((CANOPEN_OBJ_TYPE *)&CANopen_ObjTypeHbProd)

/********************************************************************************************************
 *                                     OBJECT DICTIONARY END MARKER
 *
 * Note(s) : (1) This define may be used in object directory definitions. It marks the first unused
 *               object entry.
 *******************************************************************************************************/

#define CANOPEN_OBJ_DICT_ENDMARK   { (CPU_INT32U)0, (CANOPEN_OBJ_TYPE *)0, (CPU_INT32U)0 }

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct canopen_obj_type CANOPEN_OBJ_TYPE;

/****************************************************************************************************//**
 *                                            Object entry
 *
 * @note     (1) This structure holds all data, needed for managing a single object entry.
 *
 * @note     (2) The key field is encoded as follows: 16-bit index + 8-bit sub-index + 8-bit flags.
 *               The flags are the following:
 *               - bit 0: 1 = read access allowed
 *               - bit 1: 1 = write access allowed
 *               - bit 2: 1 = PDO mappable
 *               - bit 3: 1 = signed value
 *               - bit 4: |- valid bytes in 2^n; used if
 *               - bit 5: |  Type=0, or Type->Size ptr=0
 *               - bit 6: 1 = +/- node-id on read/write
 *               - bit 7: 1 = direct access, 0 = access via specialized functions
 *
 * @note     (3) This pointer is optional. If the pointer is null, the object value is accessed directly
 *               via the field 'Data'. If the pointer is defined, the object value is accessed by some
 *               specialized functions referenced by 'TypePtr'.
 *******************************************************************************************************/

typedef struct canopen_obj {
  //                                                               Bitmap with information about object (see Note #2).
  CPU_INT32U       Key;
  //                                                               Pointer to object access specialized functions.
  //                                                               See Note #3.
  CANOPEN_OBJ_TYPE *TypePtr;
  //                                                               Address of value/data structure
  //                                                               or data value for direct access.
  CPU_INT32U       Data;
} CANOPEN_OBJ;

typedef CPU_INT32U (*CANOPEN_OBJ_SIZE_FNCT)(CANOPEN_OBJ *p_obj,
                                            CPU_INT32U  width,
                                            RTOS_ERR    *p_err);

typedef void (*CANOPEN_OBJ_CTRL_FNCT)(CANOPEN_NODE_HANDLE node_handle,
                                      CANOPEN_OBJ         *p_obj,
                                      CPU_INT16U          fnct,
                                      CPU_INT32U          param,
                                      RTOS_ERR            *p_err);

typedef void (*CANOPEN_OBJ_RD_FNCT)  (CANOPEN_NODE_HANDLE node_handle,
                                      CANOPEN_OBJ         *p_obj,
                                      void                *p_buf,
                                      CPU_INT32U          len,
                                      RTOS_ERR            *p_err);

typedef void (*CANOPEN_OBJ_WR_FNCT)  (CANOPEN_NODE_HANDLE node_handle,
                                      CANOPEN_OBJ         *p_obj,
                                      void                *p_buf,
                                      CPU_INT32U          size,
                                      RTOS_ERR            *p_err);

/****************************************************************************************************//**
 *                                             CANopen object type
 *
 * @note     This structure holds all data, needed for managing a special object entry type.
 *******************************************************************************************************/

struct canopen_obj_type {
  CANOPEN_OBJ_SIZE_FNCT SizeCb;                                 ///< Get size of type function.
  CANOPEN_OBJ_CTRL_FNCT CtrlCb;                                 ///< Special type control function.
  CANOPEN_OBJ_RD_FNCT   RdCb;                                   ///< Read function.
  CANOPEN_OBJ_WR_FNCT   WrCb;                                   ///< Write function.
};

/****************************************************************************************************//**
 *                           CANopen domain and string management structure
 *
 * @note     This structure holds all data, which are needed for the domain and string objects management
 *           within the object directory.
 *******************************************************************************************************/

#if ((CANOPEN_OBJ_STRING_EN == DEF_ENABLED) \
  || (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED))
typedef struct canopen_domain_str {
  CPU_INT08U *DataMemStartPtr;                                  ///< Pointer to Domain/String memory region.
  CPU_INT32U DataMemSize;                                       ///< Domain/String memory region size, in bytes.
  CPU_INT32U DataMemOffset;                                     ///< Offset within Domain/String memory region.
} CANOPEN_DOMAIN_STR;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeEmcy;              // Link to EMCY Object Type Structure
#endif
#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeStr;
#endif
#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeDomain;
#endif
#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) \
  || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_MapNbrs;       // Link to PDO Map Numbers Type Structure
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_Map;           // Link to PDO Mapping Type Structure
#endif

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) \
  || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_ID;            // Link to PDO Identifier Type Structure
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_Type;          // Link to PDO Transmission Type Structure
#endif
#if (CANOPEN_SDO_DYN_ID_EN == DEF_ENABLED)
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeSdoId;             // Link to SDO-Id Object Type Structure
#endif
#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeParam;             // Link to Parameter Object Type Structure
#endif
#if (CANOPEN_TPDO_MAX_QTY > 0)
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoAsync;         // Link to Parameter Object Type Structure
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoSync;          // Link to Parameter Object Type Structure
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoEvent;         // Link to Parameter Object Type Structure
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeTpdoInvalid;       // Link to Parameter Object Type Structure
#endif

extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeHbCons;            // Link to Heartbeat Consumer Object Type
extern const CANOPEN_OBJ_TYPE CANopen_ObjTypeHbProd;            // Link to Heartbeat Producer Object Type

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             CANOPEN_KEY()
 *
 * @brief    This macro helps to build the CANopen object member variable 'key'.
 *
 * @param    idx     CANopen object index [0x0000..0xFFFF]
 *
 * @param    sub     CANopen object subindex [0x00..0xFF]
 *
 * @param    flags   The additional object property flags
 *
 * @note     The resulting variable is used to hold the unique address of the object entry with
 *           index:subindex. Furthermore the lower 8 bits of the key are used to describe the way of
 *           accessing this object entry.
 *******************************************************************************************************/

#define  CANOPEN_KEY(idx, sub, flags) \
  ((CPU_INT32U)((idx) & 0xFFFF) << 16 \
    | (CPU_INT32U)((sub) & 0xFF) << 8 \
    | (CPU_INT32U)((flags) & 0xFF))

/****************************************************************************************************//**
 *                                             CANOPEN_DEV()
 *
 * @brief    This macro helps to build the CANopen object member device identifier.
 *
 * @param    idx     CANopen object index [0x0000..0xFFFF]
 *
 * @param    sub     CANopen object subindex [0x00..0xFF]
 *
 * @note     This identifier is used to search the unique address of the object entry (index:subindex).
 *******************************************************************************************************/

#define  CANOPEN_DEV(idx, sub)        \
  ((CPU_INT32U)((idx) & 0xFFFF) << 16 \
    | (CPU_INT32U)((sub) & 0xFF) << 8)

/****************************************************************************************************//**
 *                                     CANOPEN_OBJ_MAPPING_LINK()
 *
 * @brief    This macro helps to build the CANopen object entry for a PDO mapping.
 *
 * @param    idx     CANopen object index [0x0000..0xFFFF]
 *
 * @param    sub     CANopen object subindex [0x00..0xFF]
 *
 * @param    bit     Length of mapped signal in bits [8,16 or 32]
 *
 * @note     This entry is used to specify the linked signal within a PDO.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_MAPPING_LINK(idx, sub, bit) \
  ( (CPU_INT32U)((idx) & 0xFFFF) << 16           \
    | (CPU_INT32U)((sub) & 0xFF) << 8            \
    | (CPU_INT32U)((bit) & 0xFF))

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_GET_DEV()
 *
 * @brief    This macro helps to extract the index and subindex out of the CANopen object entry
 *           member 'key'.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_GET_DEV(key) \
  (CPU_INT32U)(((key) & 0xFFFFFF00))

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_GET_SUBIX()
 *
 * @brief    This macro helps to extract the subindex out of the CANopen object entry member 'key'.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_GET_SUBIX(key) \
  (CPU_INT08U)(((key) & 0x0000FF00) >> 8)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_GET_IX()
 *
 * @brief    This macro helps to extract the index out of the CANopen object entry member 'key'.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_GET_IX(key) \
  (CPU_INT16U)((key) >> 16)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_GET_SIZE()
 *
 * @brief    This macro helps to extract the object entry size in bytes out of the CANopen object
 *           entry member 'key'.
 *
 * @param    key     CANopen object member variable 'key'.
 *
 * @note     If this result is 0, the size must be calculated with the function Size(), referenced in the
 *           linked type structure.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_GET_SIZE(key) \
  (CPU_INT32U)(1L << (((key) & CANOPEN_OBJ_SZ_MSK) >> 4))

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_IS_PDOMAP()
 *
 * @brief    This macro helps to determine, if the object entry is PDO mappable.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_IS_PDOMAP(key) \
  (CPU_INT32U)(key & CANOPEN_OBJ___P__)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_IS_NODEID()
 *
 * @brief    This macro helps to determine, if the object entry value depends on the node-ID.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_IS_NODEID(key) \
  (CPU_INT32U)(key & CANOPEN_OBJ__N___)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_IS_DIRECT()
 *
 * @brief    This macro helps to determine, if the object entry value is a direct value.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_IS_DIRECT(key) \
  (CPU_INT32U)(key & CANOPEN_OBJ_D____)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_IS_RD()
 *
 * @brief    This macro helps to determine, if the object entry value is readable.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_IS_RD(key) \
  (CPU_INT32U)(key & CANOPEN_OBJ____R_)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_IS_WR()
 *
 * @brief    This macro helps to determine, if the object entry value is writeable.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_IS_WR(key) \
  (CPU_INT32U)(key & CANOPEN_OBJ_____W)

/****************************************************************************************************//**
 *                                        CANOPEN_OBJ_IS_RD_ONLY()
 *
 * @brief    This macro helps to determine, if the object entry value is read-only.
 *
 * @param    key     CANopen object member variable 'key'.
 *******************************************************************************************************/

#define  CANOPEN_OBJ_IS_RD_ONLY(key) \
  (CPU_INT32U)((key & CANOPEN_OBJ____RW) == CANOPEN_OBJ____R_)

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_OBJ_H_
