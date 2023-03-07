/***************************************************************************//**
 * @file
 * @brief CANopen Process Data Objects (Pdo) Service
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

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_CANOPEN_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>

#if ((CANOPEN_RPDO_MAX_QTY > 0) || (CANOPEN_TPDO_MAX_QTY > 0))

#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/nmt/canopen_nmt_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                            (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_CANOPEN

/********************************************************************************************************
 *                                      RPDO/TPDO COMMON DEFINES
 *
 * Note(s) : (1) More details can be found in 'Table 74: Standard objects' of specification CiA-301
 *               "CANopen application layer and communication profile, Version: 4.2.0, 21 February 2011"
 *******************************************************************************************************/

//                                                                 See Note #1.
#define  CANOPEN_xPDO_COMM_PARAM_IX_OFFSET      0x200u
#define  CANOPEN_xPDO_COMM_PARAM_IX_RANGE       0x1FFu

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
static void CANopen_TPdoMapNumWr(CANOPEN_NODE_HANDLE node_handle,
                                 CANOPEN_OBJ         *p_obj,
                                 void                *p_buf,
                                 CPU_INT32U          size,
                                 RTOS_ERR            *p_err);

static void CANopen_TPdoMapWr(CANOPEN_NODE_HANDLE node_handle,
                              CANOPEN_OBJ         *p_obj,
                              void                *p_buf,
                              CPU_INT32U          size,
                              RTOS_ERR            *p_err);
#endif

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
static void CANopen_TPdoComIdWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err);

static void CANopen_TPdoComTypeWr(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  void                *p_buf,
                                  CPU_INT32U          size,
                                  RTOS_ERR            *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   Object type PDO mapping number
 *
 * @note     (1) This type is responsible for the access to the number of PDO mapping entries. Due to
 *               the special handling of PDO mapping change accesses, only the write access needs to
 *               be handled via the type structure; the read operation can be performed directly.
 *******************************************************************************************************/

#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_MapNbrs = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_TPdoMapNumWr
};
#endif

/****************************************************************************************************//**
 *                                        Object type PDO mapping
 *
 * @note     (1) This type is responsible for the access to the PDO mapping entries. Due to the
 *               special handling of PDO mapping change accesses, only the write access needs to
 *               be handled via the type structure; the read operation can be performed directly.
 *******************************************************************************************************/

#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_Map = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_TPdoMapWr
};
#endif

/****************************************************************************************************//**
 *                                     Object type PDO identifier
 *
 * @note     (1) This type is responsible for the access to the PDO communication entries. Due to the
 *               special handling of PDO communication change accesses, only the write access needs to
 *               be handled via the type structure; the read operation can be performed directly.
 *******************************************************************************************************/

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_ID = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_TPdoComIdWr
};
#endif

/****************************************************************************************************//**
 *                                  Object type PDO transmission type
 *
 * @note     (1) This type is responsible for the access to the PDO communication entries. Due to the
 *               special handling of PDO communication change accesses, only the write access needs to
 *               be handled via the type structure; the read operation can be performed directly.
 *******************************************************************************************************/

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
const CANOPEN_OBJ_TYPE CANopen_ObjTypePDO_Type = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_TPdoComTypeWr
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
/****************************************************************************************************//**
 *                                        CANopen_TPdoMapNumWr()
 *
 * @brief    Allows the write access to the number of PDO mappings (sub-index 0 of RPDO/TPDO mapping
 *           parameter) within the communication profile.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to PDO number of mappings object.
 *
 * @param    p_buf           Pointer to data value to write.
 *
 * @param    size            Object size in byte.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoMapNumWr(CANOPEN_NODE_HANDLE node_handle,
                                 CANOPEN_OBJ         *p_obj,
                                 void                *p_buf,
                                 CPU_INT32U          size,
                                 RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_INT32U   id;                                              // Local: current identifier
  CPU_INT32U   mapentry;                                        // Local: mapping entry
  CPU_INT16U   pmapidx;                                         // Local: object mapping entry index
  CPU_INT16U   pcomidx;                                         // Local: object communication entry index
  CPU_INT08U   mapbytes;                                        // Local: number of mapped bytes
  CPU_INT08U   mapnum;                                          // Local: new number of maps to be written
  CPU_INT08U   i;                                               // Local: counter

  PP_UNUSED_PARAM(size);

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((CANOPEN_OBJ_GET_SUBIX(p_obj->Key) == 0), *p_err, RTOS_ERR_INVALID_IX,; );

  mapnum = (CPU_INT08U)(*(CPU_INT32U *)p_buf);                  // get new number of mapping

  pmapidx = CANOPEN_OBJ_GET_IX(p_obj->Key);                     // get index of object
                                                                // see, if index is in RPDO communication
  if ((pmapidx >= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_START)
      && (pmapidx <= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_END)) {
    if (mapnum > CANOPEN_RPDO_MAX_MAP_QTY) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
    //                                                             otherwise, see, if in TPDO communication
  } else if ((pmapidx >= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_START)
             && (pmapidx <= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_END)) {
    if (mapnum > CANOPEN_TPDO_MAX_MAP_QTY) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  } else {                                                      // otherwise, not in any PDO communication
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  pcomidx = pmapidx - CANOPEN_xPDO_COMM_PARAM_IX_OFFSET;                // calculate corresponding PDO com. parameter
  CANopen_DictLongRd(node_handle, CANOPEN_DEV(pcomidx, 1), &id, p_err); // get current identifier value
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RdIdFailCtr);
    return;
  }
  if ((id & CANOPEN_TPDO_COMM_COB_ID_VALID) == 0) {             // see, if current id indicates PDO valid
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_MAP_INVALID_CFG);
  } else {                                                      // otherwise: current PDO is invalid
                                                                // check for number of mapped bytes
    mapbytes = 0;                                               // clear byte counter

    for (i = 1; i <= mapnum; i++) {                             // loop through entries to be mapped
      CANopen_DictLongRd(node_handle,                           // get mapping entry
                         CANOPEN_DEV(pmapidx, i),
                         &mapentry,
                         p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {         // if mapping parameter not readable
        return;
      }

      mapbytes += CANOPEN_OBJ_GET_SIZE(mapentry);               // get size and add
    }

    if (mapbytes > 8) {                                         // if more than 8 bytes to be mapped
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;                                                   // indicate error to caller; cannot be mapped
    }

    CANopen_ObjDirectWr(p_obj,
                        &mapnum,                                // set new mapping value
                        CANOPEN_OBJ_LONG,
                        p_err);
  }
}
#endif

#if ((CANOPEN_RPDO_DYN_MAP_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_MAP_EN == DEF_ENABLED))
/****************************************************************************************************//**
 *                                          CANopen_TPdoMapWr()
 *
 * @brief    Allows the write access to the PDO mapping (sub-index 1 to 64 of RPDO/TPDO mapping parameter)
 *           within the communication profile.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to PDO mapping object.
 *
 * @param    p_buf           Pointer to data value to write.
 *
 * @param    size            Object size in byte.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoMapWr(CANOPEN_NODE_HANDLE node_handle,
                              CANOPEN_OBJ         *p_obj,
                              void                *p_buf,
                              CPU_INT32U          size,
                              RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_OBJ  *objm;                                           // Local: ptr to map object entry
  CPU_INT32U   map;                                             // Local: new type to be written
  CPU_INT32U   id;                                              // Local: current identifier
  CPU_INT32U   maps;                                            // Local: buffer for supported features
  CPU_INT16U   pmapidx;                                         // Local: object mapping entry index
  CPU_INT16U   pcomidx;                                         // Local: object communication entry index
  CPU_INT08U   mapn;                                            // Local: number of mapping entries

  PP_UNUSED_PARAM(size);                                        // prevent compiler warning

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((CANOPEN_OBJ_GET_SUBIX(p_obj->Key) >= 1), *p_err, RTOS_ERR_INVALID_IX,; );

  pmapidx = CANOPEN_OBJ_GET_IX(p_obj->Key);                     // get index of object
                                                                // see, if index is in RPDO mapping
  if ((pmapidx >= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_START)
      && (pmapidx <= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_END)) {
    //                                                             otherwise, see, if in TPDO mapping
  } else if ((pmapidx >= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_START)
             && (pmapidx <= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_END)) {
  } else {                                                      // otherwise, not in any PDO mapping
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }

  pcomidx = pmapidx - CANOPEN_xPDO_COMM_PARAM_IX_OFFSET;        // calculate corresponding PDO com. parameter
  map = *(CPU_INT32U *)p_buf;                                   // get new mapping
                                                                // get current identifier value
  CANopen_DictLongRd(node_handle, CANOPEN_DEV(pcomidx, 1), &id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RdIdFailCtr);
    return;
  }

  if ((id & CANOPEN_TPDO_COMM_COB_ID_VALID) == 0) {             // see, if current id indicates PDO valid
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
  } else {                                                      // otherwise: current PDO is invalid
    CANopen_DictByteRd(node_handle,                             // get current number of mapping entries
                       CANOPEN_DEV(pmapidx, 0),
                       &mapn,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RdMapFailCtr);
      return;
    }

    if (mapn != 0) {                                            // mapping not deactivated
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      return;
    }

    objm = CANopen_DictFind(&(p_node->Dict), map, p_err);       // try to find the entry to be mapped
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // entry not found
      return;
    }

    maps = CANOPEN_OBJ_IS_PDOMAP(objm->Key);                    // check if mappable
    if (maps == 0) {                                            // not mappable
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      return;
    }
    //                                                             see, if index is in RPDO mapping
    if ((pmapidx >= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_START)
        && (pmapidx <= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_END)) {
      maps = CANOPEN_OBJ_IS_WR(objm->Key);                      // check if writeable
      if (maps == 0) {                                          // not writeable
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
        return;
      }
      //                                                           otherwise, see, if in TPDO mapping
    } else if ((pmapidx >= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_START)
               && (pmapidx <= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_MAPPING_PARAM_END)) {
      maps = CANOPEN_OBJ_IS_RD(objm->Key);                      // check if readable
      if (maps == 0) {                                          // not readable
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
        return;
      }
    }
    //                                                             set new mapping value
    CANopen_ObjDirectWr(p_obj, &map, CANOPEN_OBJ_LONG, p_err);
  }
}
#endif

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
/****************************************************************************************************//**
 *                                         CANopen_TPdoComIdWr()
 *
 * @brief    Allows the write access to the PDO identifier (COB-ID of RPDO/TPDO communication parameter)
 *           within the communication profile.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to PDO identifier object.
 *
 * @param    p_buf           Pointer to data value to write.
 *
 * @param    size            Object size in byte.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoComIdWr(CANOPEN_NODE_HANDLE node_handle,
                                CANOPEN_OBJ         *p_obj,
                                void                *p_buf,
                                CPU_INT32U          size,
                                RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_NMT  *nmt;                                            // Local: pointer to NMT management
#if (CANOPEN_TPDO_MAX_QTY > 0)
  CANOPEN_TPDO *p_tpdo = DEF_NULL;                              // Local: pointer to transmit PDO object
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
  CANOPEN_RPDO *p_rpdo = DEF_NULL;                              // Local: pointer to receive PDO object
#endif
  CPU_INT32U new_id;                                            // Local: new identifier to be written
  CPU_INT32U old_id;                                            // Local: old identifier to be over-written
  CPU_INT16U pcomidx;                                           // Local: object entry index
  CPU_INT16U num;                                               // Local: PDO number

  PP_UNUSED_PARAM(size);                                        // prevent compiler warning

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((CANOPEN_OBJ_GET_SUBIX(p_obj->Key) == 1), *p_err, RTOS_ERR_INVALID_IX,; );

  new_id = *(CPU_INT32U *)p_buf;                                // get new identifier
  if (((new_id & CANOPEN_TPDO_COMM_COB_ID_FRM) != 0)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;
  }

  nmt = &p_node->Nmt;                                           // get pointer to NMT management
  pcomidx = CANOPEN_OBJ_GET_IX(p_obj->Key);                     // get index of object
                                                                // see, if index is in RPDO communication
  if ((pcomidx >= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_START)
      && (pcomidx <= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_END)) {
#if (CANOPEN_RPDO_MAX_QTY > 0)
    p_rpdo = p_node->RpdoTbl;                                   // get pointer to receive PDO object
    num = pcomidx & CANOPEN_xPDO_COMM_PARAM_IX_RANGE;           // get PDO number
#else
    //                                                             indicate error to caller
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_COM_INVALID_CFG);
    return;
#endif
    //                                                             otherwise, see, if in TPDO communication
  } else if ((pcomidx >= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START)
             && (pcomidx <= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_END)) {
#if (CANOPEN_TPDO_MAX_QTY > 0)
    if ((new_id & CANOPEN_TPDO_COMM_COB_ID_RTR) == 0) {         // see, if new identifier allows RTR
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;                                                   // yes: abort write access (not supported)
    }

    p_tpdo = p_node->TpdoTbl;                                   // get pointer to transmit PDO object
    num = pcomidx & CANOPEN_xPDO_COMM_PARAM_IX_RANGE;           // get PDO number
#else
    //                                                             indicate error to caller
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_COM_INVALID_CFG);
    return;
#endif
  } else {                                                      // otherwise, not in any PDO communication
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }
  //                                                               get old identifier value
  CANopen_ObjDirectRd(p_obj, &old_id, CANOPEN_OBJ_LONG, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RdIdFailCtr);
    return;
  }

  if ((old_id & CANOPEN_TPDO_COMM_COB_ID_VALID) == 0) {         // see, if current id indicates PDO valid
    if ((new_id & CANOPEN_TPDO_COMM_COB_ID_VALID) != 0) {       // see, if new id indicates invalid PDO
      CANopen_ObjDirectWr(p_obj,
                          &new_id,                              // set new identifier value
                          CANOPEN_OBJ_LONG,
                          p_err);
      if (nmt->State == CANOPEN_OPERATIONAL) {                  // see, if device is in OPERATIONAL state
#if (CANOPEN_TPDO_MAX_QTY > 0)
        if ((p_tpdo != DEF_NULL)
            && (num <= CANOPEN_TPDO_MAX_QTY)) {                 // see, if an TPDO should be rescanned
          CANopen_TPdoReset(p_tpdo, num, p_err);                // rescan TPDO from object directory
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.TPdoRstFailCtr);
            return;
          }
        }
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
        if ((p_rpdo != DEF_NULL)
            && (num <= CANOPEN_RPDO_MAX_QTY)) {                 // see, if an RPDO should be rescanned
          CANopen_RPdoReset(p_rpdo, num, p_err);                // rescan RPDO from object directory
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RPdoRstFailCtr);
            return;
          }
        }
#endif
      }
    } else {                                                    // otherwise: new id is valid PDO id
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);             // indicate error; access not allowed
    }
  } else {                                                      // otherwise: current PDO is invalid
    if ((new_id & CANOPEN_TPDO_COMM_COB_ID_VALID) != 0) {       // see, if new id indicates invalid PDO
      CANopen_ObjDirectWr(p_obj,
                          &new_id,                              // set new identifier value
                          CANOPEN_OBJ_LONG,
                          p_err);
    } else {                                                    // otherwise: new id is valid PDO id
      CANopen_ObjDirectWr(p_obj,
                          &new_id,                              // set new identifier value
                          CANOPEN_OBJ_LONG,
                          p_err);
      if (nmt->State == CANOPEN_OPERATIONAL) {                  // see, if device is in OPERATIONAL state
#if (CANOPEN_TPDO_MAX_QTY > 0)
        if (p_tpdo != DEF_NULL) {                               // see, if an TPDO should be rescanned
          CANopen_TPdoReset(p_tpdo, num, p_err);                // rescan TPDO from object directory
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.TPdoRstFailCtr);
            return;
          }
        }                                                       // otherwise an RPDO should be rescanned
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
        if (p_rpdo != DEF_NULL) {                               // see, if an RPDO should be rescanned
          CANopen_RPdoReset(p_rpdo, num, p_err);                // rescan RPDO from object directory
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RPdoRstFailCtr);
            return;
          }
        }
#endif
      }
    }
  }
}
#endif

#if ((CANOPEN_RPDO_DYN_COM_EN == DEF_ENABLED) || (CANOPEN_TPDO_DYN_COM_EN == DEF_ENABLED))
/****************************************************************************************************//**
 *                                        CANopen_TPdoComTypeWr()
 *
 * @brief    Allows the write access to the PDO transmission type (sub-index 2 of RPDO/TPDO communication
 *           parameter) within the communication profile.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to PDO transmission type object.
 *
 * @param    p_buf           Pointer to data value to write.
 *
 * @param    size            Object size in byte
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_TPdoComTypeWr(CANOPEN_NODE_HANDLE node_handle,
                                  CANOPEN_OBJ         *p_obj,
                                  void                *p_buf,
                                  CPU_INT32U          size,
                                  RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_INT08U   type;                                            // Local: new type to be written
  CPU_INT32U   id;                                              // Local: current identifier
  CPU_INT16U   pcomidx;                                         // Local: object entry index

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((size == CANOPEN_OBJ_LONG), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET((CANOPEN_OBJ_GET_SUBIX(p_obj->Key) == 2), *p_err, RTOS_ERR_INVALID_IX,; );

  type = *(CPU_INT08U *)p_buf;                                     // get new type
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  if (type < CANOPEN_TPDO_COMM_TYPE_ASYNC_EVENT_VENDOR_SPECIFIC) { // see, if new type is invalid
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
    return;                                                        // yes: abort write access (not supported)
  }
#endif

  pcomidx = CANOPEN_OBJ_GET_IX(p_obj->Key);                        // get index of object
                                                                   // see, if index is in RPDO communication
  if ((pcomidx >= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_START)
      && (pcomidx <= CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_END)) {
    //                                                             otherwise, see, if in TPDO communication
  } else if ((pcomidx >= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_START)
             && (pcomidx <= CANOPEN_COMM_PROFILE_OBJ_IX_TPDO_COMM_PARAM_END)) {
  } else {                                                      // otherwise, not in any PDO communication
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_IX);
    return;
  }
  //                                                               get current identifier value
  CANopen_DictLongRd(node_handle, CANOPEN_DEV(pcomidx, 1), &id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Pdo.RdIdFailCtr);
    return;
  }
  if ((id & CANOPEN_TPDO_COMM_COB_ID_VALID) == 0) {             // see, if current id indicates PDO valid
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
  } else {                                                      // otherwise: current PDO is invalid
                                                                // set new type value
    CANopen_ObjDirectWr(p_obj, &type, CANOPEN_OBJ_LONG, p_err);
  }
}
#endif

#endif // CANOPEN_RPDO_MAX_QTY || CANOPEN_TPDO_MAX_QTY

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
