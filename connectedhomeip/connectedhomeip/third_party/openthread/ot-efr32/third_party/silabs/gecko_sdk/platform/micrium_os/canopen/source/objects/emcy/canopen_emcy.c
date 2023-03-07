/***************************************************************************//**
 * @file
 * @brief CANopen Emergency (Emcy) Object Service
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

#include  <canopen_cfg.h>

#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/objects/emcy/canopen_emcy_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define  CANOPEN_EMCY_ERROR_NOT_DETECTED         0
#define  CANOPEN_EMCY_ERROR_ALREADY_DETECTED     1

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_EmcyCheck(CANOPEN_EMCY *p_emcy,
                              RTOS_ERR     *p_err);

static CPU_INT16S CANopen_EmcyErrGet(CANOPEN_EMCY *p_emcy,
                                     CPU_INT08U   err_code_ix);

static CPU_INT16S CANopen_EmcyErrSet(CANOPEN_EMCY *p_emcy,
                                     CPU_INT08U   err_code_ix,
                                     CPU_INT08U   state);

static void CANopen_EmcySend(CANOPEN_EMCY     *p_emcy,
                             CPU_INT08U       err_code_ix,
                             CANOPEN_EMCY_USR *p_usr,
                             CPU_INT08U       state);

static void CANopen_EmcyUpdate(CANOPEN_EMCY     *p_emcy,
                               CPU_INT08U       err_code_ix,
                               CANOPEN_EMCY_USR *p_usr,
                               CPU_INT08U       state);

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
static void CANopen_EmcyHistInit(CANOPEN_EMCY *p_emcy,
                                 RTOS_ERR     *p_err);

static void CANopen_EmcyHistAdd(CANOPEN_EMCY     *p_emcy,
                                CPU_INT08U       err_code_ix,
                                CANOPEN_EMCY_USR *p_usr);

static void CANopen_EmcyHistRd(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               void                *p_buf,
                               CPU_INT32U          len,
                               RTOS_ERR            *p_err);

static void CANopen_EmcyHistWr(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               void                *p_buf,
                               CPU_INT32U          len,
                               RTOS_ERR            *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Object type emergency
 *
 * @note    (1) This object type is designed to provide the read and write feature
 *              of the emergency history.
 *******************************************************************************************************/

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
const CANOPEN_OBJ_TYPE CANopen_ObjTypeEmcy = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = CANopen_EmcyHistRd,
  .WrCb = CANopen_EmcyHistWr
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           CANopen_EmcySet()
 *
 * @brief    Sets an emergency message and sends an event to the Service task to trigger emergency
 *           processing.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_user          Vendor specific fields in EMCY history and/or EMCY message.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_EmcySet(CANOPEN_NODE_HANDLE node_handle,
                     CPU_INT08U          err_code_ix,
                     CANOPEN_EMCY_USR    *p_user,
                     RTOS_ERR            *p_err)
{
  CANOPEN_NODE  *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EVENT event;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  event.NodePtr = p_node;
  event.IntNbr = 0u;
  event.ErrCodeIx = err_code_ix;
  event.VendorErrCodePtr = p_user;
  event.EventType = CANOPEN_EVENT_ERR_SET;

  CANopen_SvcTaskWakeUp(&event);
}

/****************************************************************************************************//**
 *                                           CANopen_EmcyClr()
 *
 * @brief    Clears an emergency message and sends an event to the Service task to trigger emergency
 *           processing.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_EmcyClr(CANOPEN_NODE_HANDLE node_handle,
                     CPU_INT08U          err_code_ix,
                     RTOS_ERR            *p_err)
{
  CANOPEN_NODE  *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EVENT event;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  event.NodePtr = p_node;
  event.IntNbr = 0u;
  event.ErrCodeIx = err_code_ix;
  event.VendorErrCodePtr = DEF_NULL;
  event.EventType = CANOPEN_EVENT_ERR_CLR;

  CANopen_SvcTaskWakeUp(&event);
}

/****************************************************************************************************//**
 *                                         CANopen_EmcyGet()
 *
 * @brief    Returns the current EMCY error status.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NULL_PTR
 *
 * @return   Current EMCY error status.
 *               - 0, if the error is not detected.
 *               - 1, if the error was detected before.
 *               - < 0, if an error is detected inside this function
 *******************************************************************************************************/
CPU_INT16S CANopen_EmcyGet(CANOPEN_NODE_HANDLE node_handle,
                           CPU_INT08U          err_code_ix,
                           RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EMCY *p_emcy;
  CPU_INT16S   cur = -1;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  p_emcy = &(p_node->Emcy);

  CANopen_EmcyCheck(p_emcy, p_err);                             // check parameter emcy to be valid
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if parameter emcy is bad
    return (cur);                                               // error is registered (if possible)
  }

  cur = CANopen_EmcyErrGet(p_emcy, err_code_ix);                // get current error detection marker

  return (cur);                                                 // return function result
}

/****************************************************************************************************//**
 *                                         CANopen_EmcyCnt()
 *
 * @brief    Returns the number of currently detected EMCY errors.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NULL_PTR
 *
 * @return   Number of EMCY errors.
 *******************************************************************************************************/
CPU_INT16S CANopen_EmcyCnt(CANOPEN_NODE_HANDLE node_handle,
                           RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EMCY *p_emcy;
  CPU_INT16S   result = -1;
  CPU_INT08U   n;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  p_emcy = &(p_node->Emcy);

  CANopen_EmcyCheck(p_emcy, p_err);                             // check parameter emcy to be valid
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if parameter emcy is bad
    return (result);                                            // error is registered (if possible)
  }

  result = 0;                                                   // reset error counter
  for (n = 0; n < CANOPEN_EMCY_REG_QTY; n++) {                  // loop through all error classes
    result += p_emcy->Cnt[n];                                   // add number of pending errors
  }

  return (result);                                              // return sum of pending errors
}

/****************************************************************************************************//**
 *                                        CANopen_EmcyReset()
 *
 * @brief    Clears all EMCY errors.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_INIT
 *                               - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_EmcyReset(CANOPEN_NODE_HANDLE node_handle,
                       RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EMCY *p_emcy;
  CPU_INT16S   change;
  CPU_INT08U   n;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  p_emcy = &(p_node->Emcy);

  CANopen_EmcyCheck(p_emcy, p_err);                             // check parameter p_emcy to be valid
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if parameter p_emcy is bad
    return;                                                     // error is registered (if possible)
  }

  for (n = 0; n < CANOPEN_EMCY_MAX_ERR_QTY; n++) {              // loop through all EMCY errors
    change = CANopen_EmcyErrSet(p_emcy, n, 0);                  // clear error detection marker
    if (change > 0) {                                           // see, if a error change is detected
      CANopen_EmcyUpdate(p_emcy, n, 0, 0);                      // update object directory entries
    }
  }
}

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                      CANopenEmcyHistReset()
 *
 * @brief    Clears the EMCY history in the object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *******************************************************************************************************/
void CANopen_EmcyHistReset(CANOPEN_NODE_HANDLE node_handle)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EMCY *p_emcy;
  CANOPEN_DICT *p_cod;                                          // Local: ptr to object directory
  CANOPEN_OBJ  *p_obj;                                          // Local: ptr to object entry
  CPU_INT32U   val32;                                           // Local: value for EMCY entry
  CPU_INT08U   sub;                                             // Local: loop cntr through subidx
  CPU_INT08U   val08;                                           // Local: value for EMCY number
  RTOS_ERR     local_err;

  RTOS_ASSERT_DBG(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), RTOS_ERR_NULL_PTR,; );

  p_emcy = &(p_node->Emcy);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  CANopen_EmcyCheck(p_emcy, &local_err);                        // check given parameter emcy
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if an error is detected
    return;                                                     // abort initialization function
  }

  p_cod = &p_node->Dict;                                        // get ptr to object directory

  val08 = 0;
  p_obj = CANopen_DictFind(p_cod,
                           CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, 0),
                           &local_err);                         // get object of EMCY number
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if EMCY history did not exist
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
    return;                                                     // abort reset function
  }

  CANopen_ObjDirectWr(p_obj, &val08, 1, &local_err);            // clear number of occurred EMCY
  CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                           p_node->CntErrPtr->Emcy.WrPreDefErrFailCtr);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  val32 = 0;
  for (sub = 1; sub <= p_emcy->Hist.TotLen; sub++) {            // loop through all EMCY storage entries
    p_obj = CANopen_DictFind(p_cod,
                             CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, sub),
                             &local_err);                       // get object of EMCY entry
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);

    CANopen_ObjDirectWr(p_obj, &val32, 4, &local_err);          // clear EMCY entry
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.WrPreDefErrFailCtr);
  }

  p_emcy->Hist.TotLen = sub - 1;                                // set number of available EMCY entries
  p_emcy->Hist.Offset = 0;                                      // set subidx for next entry
  p_emcy->Hist.HistQty = 0;                                     // indicate no EMCY entry in History
}
#endif // CANOPEN_EMCY_HIST_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                        INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_EmcyInit()
 *
 * @brief    Initializes the EMCY object and the internal error storage memory.
 *           Additionally, the links to the given user EMCY error codes to the given node are set.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    p_node          Pointer to the CANopen device node information structure.
 *
 * @param    p_root          Pointer to the start of the User EMCY table.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
void CANopen_EmcyInit(CANOPEN_EMCY     *p_emcy,
                      CANOPEN_NODE     *p_node,
                      CANOPEN_EMCY_TBL *p_root,
                      RTOS_ERR         *p_err)
{
  CPU_INT16U  n;                                                // Local: loop counter
  CANOPEN_OBJ *obj;                                             // Local: pointer to object entry
  CPU_INT32U  size;

  RTOS_ASSERT_DBG_ERR_SET((p_root != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_emcy != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_emcy->RootUserTblPtr = p_root;                              // store appl. EMCY code table reference
  p_emcy->NodePtr = p_node;                                     // link parent node to EMCY structure

  for (n = 0; n < CANOPEN_EMCY_ERR_CODE_STORAGE_SIZE; n++) {    // clear detailed error storage memory
    p_emcy->Err[n] = 0;
  }
  for (n = 0; n < CANOPEN_EMCY_REG_QTY; n++) {                  // clear all register bit counters
    p_emcy->Cnt[n] = 0;
  }

  obj = CANopen_DictFind(&p_node->Dict,
                         CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_COB_ID_EMCY, 0),
                         p_err);                                // check existence of EMCY identifier
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    size = CANopen_ObjSizeGet(obj, CANOPEN_OBJ_LONG, p_err);    // check object size matches expectation
    if (size == 0) {                                            // see, if an error is detected
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY);
    }
  }

  obj = CANopen_DictFind(&p_node->Dict,
                         CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_ERR_REG, 0),
                         p_err);                                // check existence of error register
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    size = CANopen_ObjSizeGet(obj, CANOPEN_OBJ_BYTE, p_err);    // check object size matches expectation
    if (size == 0) {                                            // see, if an error is detected
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY);
    }
  }

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
  CANopen_EmcyHistInit(p_emcy, p_err);                          // initialize EMCY history object entries
#endif
}

/****************************************************************************************************//**
 *                                     CANopen_EmcySetInternal()
 *
 * @brief    Checks the current error state and updates the object directory.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_usr           Pointer to the manufacturer specific fields in EMCY history and/or EMCY message.
 *                           Can be set to a NULL pointer if no manufacturer specific fields are used.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The EMCY message is transmitted, if the error is detected for the first time.
 *******************************************************************************************************/
void CANopen_EmcySetInternal(CANOPEN_EMCY     *p_emcy,
                             CPU_INT08U       err_code_ix,
                             CANOPEN_EMCY_USR *p_usr,
                             RTOS_ERR         *p_err)
{
  CPU_INT16S change;                                            // Local: current error change indication

  CANopen_EmcyCheck(p_emcy, p_err);                             // check parameter emcy to be valid
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if parameter emcy is bad
    return;                                                     // error is registered (if possible)
  }

  change = CANopen_EmcyErrSet(p_emcy, err_code_ix, 1);          // set error detection marker
  if (change > 0) {                                             // see, if a error change is detected
    CANopen_EmcyUpdate(p_emcy, err_code_ix, p_usr, 1);          // update object directory entries
    CANopen_EmcySend(p_emcy, err_code_ix, p_usr, 1);            // send EMCY message
  }
}

/****************************************************************************************************//**
 *                                    CANopen_EmcyClrInternal()
 *
 * @brief    Checks the current error state and updates the object directory.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The EMCY message is transmitted, if the error was previously detected.
 *******************************************************************************************************/
void CANopen_EmcyClrInternal(CANOPEN_EMCY *p_emcy,
                             CPU_INT08U   err_code_ix,
                             RTOS_ERR     *p_err)
{
  CPU_INT16S change;                                            // Local: current error change indication

  CANopen_EmcyCheck(p_emcy, p_err);                             // check parameter emcy to be valid
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if parameter emcy is bad
    return;                                                     // error is registered (if possible)
  }

  change = CANopen_EmcyErrSet(p_emcy, err_code_ix, 0);          // clear error detection marker
  if (change > 0) {                                             // see, if a error change is detected
    CANopen_EmcyUpdate(p_emcy, err_code_ix, DEF_NULL, 0);       // update object directory entries
    CANopen_EmcySend(p_emcy, err_code_ix, DEF_NULL, 0);         // send EMCY message
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                      CANopen_EmcyCheck()
 *
 * @brief    Centralizes the parameter checking of the EMCY structure.
 *
 * @param    p_emcy  Pointer to the EMCY object.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void CANopen_EmcyCheck(CANOPEN_EMCY *p_emcy,
                              RTOS_ERR     *p_err)
{
  CANOPEN_NODE *p_node;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_emcy != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_node = p_emcy->NodePtr;                                     // get ptr to parent node
  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (p_emcy->RootUserTblPtr == DEF_NULL) {                     // see, if root pointer is invalid
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_INIT);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }
}

/****************************************************************************************************//**
 *                                      CANopen_EmcyErrGet()
 *
 * @brief    Gets the error state of the given error identifier out of the internal error storage memory.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @return   0 if the error is not present.
 *           1 if the error is already detected.
 *
 * @note     (1) The parameter p_emcy shall be checked by the calling function.
 *******************************************************************************************************/
static CPU_INT16S CANopen_EmcyErrGet(CANOPEN_EMCY *p_emcy,
                                     CPU_INT08U   err_code_ix)
{
  CPU_INT08U result;                                            // Local: function result
  CPU_INT08U byte;                                              // Local: calculated byte in error memory
  CPU_INT08U mask;                                              // Local: calculated bit mask in the byte

  if (err_code_ix >= CANOPEN_EMCY_MAX_ERR_QTY) {                // see, if error identifier is invalid
    err_code_ix = CANOPEN_EMCY_MAX_ERR_QTY - 1;                 // limit to last entry in user table
  }
  byte = err_code_ix >> 3;                                      // calculate byte, holding the given error
  mask = (CPU_INT08U)1 << (err_code_ix & 0x7);                  // calculate mask, representing the error
  if ((p_emcy->Err[byte] & mask) == 0) {                        // see, if this error is currently not set
    result = CANOPEN_EMCY_ERROR_NOT_DETECTED;                   // set result to 'error not detected'
  } else {                                                      // otherwise, this error is currently set
    result = CANOPEN_EMCY_ERROR_ALREADY_DETECTED;               // set result to 'error present'
  }
  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                      CANopen_EmcyErrSet()
 *
 * @brief    Sets the error state of the given error identifier into the internal error storage memory.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error identifier in User EMCY table.
 *
 * @param    state           Error state.
 *                               Equal to 0          No Error
 *                               Different from 0    Error
 *
 * @return   0 if the error state is unchanged.
 *           1 otherwise.
 *
 * @note     (1) The parameter p_emcy shall be checked by the calling function.
 *******************************************************************************************************/
static CPU_INT16S CANopen_EmcyErrSet(CANOPEN_EMCY *p_emcy,
                                     CPU_INT08U   err_code_ix,
                                     CPU_INT08U   state)
{
  CPU_INT08U result;                                            // Local: function result
  CPU_INT08U byte;                                              // Local: calculated byte in error memory
  CPU_INT08U mask;                                              // Local: calculated bit mask in the byte

  if (err_code_ix >= CANOPEN_EMCY_MAX_ERR_QTY) {                // see, if error identifier is invalid
    err_code_ix = CANOPEN_EMCY_MAX_ERR_QTY - 1;                 // limit to last entry in user table
  }
  byte = err_code_ix >> 3;                                      // calculate byte, holding the given error
  mask = (CPU_INT08U)1 << (err_code_ix & 0x7);                  // calculate mask, representing the error
  if ((p_emcy->Err[byte] & mask) == 0) {                        // see, if this error is currently not set
    if (state != 0) {                                           // yes: see, if new error state is set
      p_emcy->Err[byte] |= mask;                                // yes: mark error detected in memory
      result = 1;                                               // indicate error state change
    } else {                                                    // otherwise: both states indicates not set
      result = 0;                                               // indicate same error state as before
    }
  } else {                                                      // otherwise: error is currently set
    if (state == 0) {                                           // yes: see, if new error state is cleared
      p_emcy->Err[byte] &= ~mask;                               // yes: clear detected error in memory
      result = 1;                                               // indicate error state change
    } else {                                                    // otherwise: both states indicates set
      result = 0;                                               // indicate same error state as before
    }
  }
  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                    CANopen_EmcySend()
 *
 * @brief    Transmits the EMCY message to the configured CAN bus.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_usr           Pointer to the manufacturer specific fields in EMCY message and/or EMCY
 *                           history.
 *
 * @param    state           Error state.
 *                               - Equal to 0          No Error
 *                               - Different from 0    Error
 *
 * @note     (1) The parameter p_emcy shall be checked by the calling function.
 *******************************************************************************************************/
static void CANopen_EmcySend(CANOPEN_EMCY     *p_emcy,
                             CPU_INT08U       err_code_ix,
                             CANOPEN_EMCY_USR *p_usr,
                             CPU_INT08U       state)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_IF_FRM      frm;                                      // Local: CAN frame for transmission
  CANOPEN_NODE        *p_node;                                  // Local: pointer to node informations
  CANOPEN_EMCY_TBL    *p_data;                                  // Local: pointer to user error field table
  CPU_INT08U          n;                                        // Local: loop through user error field
  RTOS_ERR            local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_node = p_emcy->NodePtr;                                           // get node pointer
  if ((p_node->Nmt.CurStateObjAllowed & CANOPEN_EMCY_ALLOWED) == 0) { // see, if EMCY messages are not allowed
    return;                                                           // exit function without error
  }
  if (err_code_ix >= CANOPEN_EMCY_MAX_ERR_QTY) {                      // see, if error identifier is invalid
    err_code_ix = CANOPEN_EMCY_MAX_ERR_QTY - 1;                       // limit to last entry in user table
  }

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  p_data = &p_emcy->RootUserTblPtr[err_code_ix];                // get entry of addressed user error field

  //                                                               read EMCY identifier (existence checked)
  CANopen_DictLongRd(handle,
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_COB_ID_EMCY, 0),
                     &frm.MsgId,
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if an error is detected
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_emcy->NodePtr->CntErrPtr->Emcy.RdIdFailCtr);
    return;                                                     // set bus write error
  }

  frm.DLC = 8;                                                  // fixed DLC (8 bytes)
  if (state == 1) {                                             // see, if new error is detected
    frm.Data[0] = (CPU_INT08U)(p_data->Code);                   // set pre-defined error code in message
    frm.Data[1] = (CPU_INT08U)(p_data->Code >> 8);
  } else {
    frm.Data[0] = (CPU_INT08U)0;                                // set error code 'no error' in message
    frm.Data[1] = (CPU_INT08U)0;
  }

  //                                                               set error register (existence checked)
  CANopen_DictByteRd(handle,
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_ERR_REG, 0),
                     &frm.Data[2],
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if an error is detected
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_emcy->NodePtr->CntErrPtr->Emcy.RdErrRegFailCtr);
  }
  //                                                               loop through manufacturer specific field
  for (n = 0; n < CANOPEN_EMCY_MAN_SPECIFIC_ERR_FIELD_LEN; n++) {
    frm.Data[3 + n] = 0;                                        // clear user error codes in message
  }
#if (CANOPEN_EMCY_EMCY_MAN_EN == DEF_ENABLED)
  if (p_usr != DEF_NULL) {                                      // see, if additional information are given
                                                                // loop through manufacturer specific field
    for (n = 0; n < CANOPEN_EMCY_MAN_SPECIFIC_ERR_FIELD_LEN; n++) {
      frm.Data[3 + n] = p_usr->Emcy[n];                         // copy user error codes in message
    }
  }
#else
  PP_UNUSED_PARAM(p_usr);                                       // unused, prevent compiler warning
#endif

  CANopen_IfWr(&p_emcy->NodePtr->If, &frm, &local_err);         // transmit EMCY message
                                                                // possible error registered in node
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if an error is detected
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_emcy->NodePtr->CntErrPtr->Emcy.TxFailCtr);
    return;                                                     // set bus write error
  }
}

/****************************************************************************************************//**
 *                                      CANopen_EmcyUpdate()
 *
 * @brief    Updates the EMCY object directory entries with the given error code change.
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_usr           Pointer to the manufacturer specific fields in EMCY message and/or EMCY
 *                           history.
 *
 * @param    state           Error state.
 *                               - Equal to 0          No Error
 *                               - Different from 0    Error
 *
 * @note     (1) The parameter p_emcy shall be checked by the calling function.
 *******************************************************************************************************/
static void CANopen_EmcyUpdate(CANOPEN_EMCY     *p_emcy,
                               CPU_INT08U       err_code_ix,
                               CANOPEN_EMCY_USR *p_usr,
                               CPU_INT08U       state)
{
  CANOPEN_NODE_HANDLE handle;
  CPU_INT08U          regbit;                                   // Local: bit in error register
  CPU_INT08U          regmask;                                  // Local: bit mask for error register
  CPU_INT08U          reg;                                      // Local: error register value
  RTOS_ERR            local_err;

  CANOPEN_NODE_HANDLE_SET(handle, p_emcy->NodePtr);
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

#if (CANOPEN_EMCY_HIST_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_usr);                                       // unused, prevent compiler warning
#endif

  if (err_code_ix >= CANOPEN_EMCY_MAX_ERR_QTY) {                // see, if error identifier is invalid
    err_code_ix = CANOPEN_EMCY_MAX_ERR_QTY - 1;                 // limit to last entry in user table
  }
  regbit = p_emcy->RootUserTblPtr[err_code_ix].Reg;             // get corresponding bit in error register
  regmask = (CPU_INT08U)(1u << regbit);                         // calculate register mask

  //                                                               get error register (existence checked)
  CANopen_DictByteRd(handle, CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_ERR_REG, 0), &reg, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if an error is detected
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_emcy->NodePtr->CntErrPtr->Emcy.RdErrRegFailCtr);
    return;
  }

  if (state != 0) {                                             // -------------------- SET ERROR ---------------------
    if ((reg & regmask) == 0) {                                 // see, if this error class is not set
      reg |= regmask;                                           // yes: set error class in error register
      reg |= 0x01;                                              // generic error is set for any error
    }
#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
    CANopen_EmcyHistAdd(p_emcy, err_code_ix, p_usr);            // add this EMCY to EMCY history
#endif
    p_emcy->Cnt[regbit]++;                                      // count occurance of this error class
  } else {                                                      // ------------------- CLEAR ERROR --------------------
    p_emcy->Cnt[regbit]--;                                      // decrement number of occurance
    if (p_emcy->Cnt[regbit] == 0) {                             // see, if error was the last in this class
      reg &= ~regmask;                                          // yes: clear error class in error register
#if (CANOPEN_EMCY_REG_CLASS_EN == DEF_ENABLED)
      if ((reg & ~0x01) != 0) {                                 // see, if not all error classes are cleared
        reg |= 0x01;                                            // ensure that generic error stays till end
      } else {                                                  // otherwise: all error classes are cleared
        if (p_emcy->Cnt[0] == 0) {                              // see, if no generic error is pending
          reg &= ~0x01;                                         // yes: clear generic error register bit
        }
      }
#endif
    }
  }
  //                                                               set error register (existence checked)
  CANopen_DictByteWr(handle, CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_ERR_REG, 0), reg, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // see, if an error is detected
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_emcy->NodePtr->CntErrPtr->Emcy.WrErrRegFailCtr);
  }
}

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                     CANopen_EmcyHistInit()
 *
 * @brief    Initializes the EMCY history management. Additionally, the object
 *           entries, mandatory for EMCY history are checked for existence.
 *
 * @param    p_emcy  Pointer to the EMCY object.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void CANopen_EmcyHistInit(CANOPEN_EMCY *p_emcy,
                                 RTOS_ERR     *p_err)
{
  CANOPEN_NODE *p_node;                                         // Local: ptr to parent node
  CANOPEN_DICT *p_cod;                                          // Local: ptr to object directory
  CANOPEN_OBJ  *p_obj;                                          // Local: ptr to object entry
  CPU_INT08U   sub_ix;                                          // Local: loop cntr through subidx

  p_emcy->Hist.TotLen = 0;                                      // indicate no EMCY entry available
  p_emcy->Hist.HistQty = 0;                                     // indicate no EMCY entry in History
  p_emcy->Hist.Offset = 0;                                      // indicate no EMCY entry writable

  p_node = p_emcy->NodePtr;                                     // get ptr to parent node
  p_cod = &p_node->Dict;                                        // get ptr to object dictionary

  p_obj = CANopen_DictFind(p_cod,
                           CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, 0),
                           p_err);                              // try to find index for EMCY History
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if EMCY history did not exist
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
    return;
  }

  p_obj = CANopen_DictFind(p_cod,
                           CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, 1),
                           p_err);                              // try to find one entry for EMCY History
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if EMCY entry did not exist
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
    return;
  }

  sub_ix = 2;                                                   // start max entry search with 2nd entry
  while (p_obj != DEF_NULL) {                                   // repeat until EMCY entry is not existing
    p_obj = CANopen_DictFind(p_cod,
                             CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, sub_ix),
                             p_err);                            // find EMCY entry in object directory
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                               p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
      break;
    }
    if (p_obj != DEF_NULL) {                                    // see, if EMCY entry is found
      sub_ix++;                                                 // switch to next EMCY entry
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_emcy->Hist.TotLen = sub_ix - 1;                             // set number of available EMCY entries
  p_emcy->Hist.Offset = 0;                                      // set subidx for next entry
}
#endif

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                      CANopen_EmcyHistAdd()
 *
 * @brief    Appends a new EMCY entry into the history. To avoid moving the object entries with each new
 *           entry, the following algorithm is implemented:
 *               - This function will use the EMCY Entries as ring buffer. Each Add-function call
 *                 will use the next higher sub-index for storage of the EMCY History informations.
 *               - The read/write access functions will remap the different sub-index entry values
 *                 to the required order (latest EMCY add must appear at sub-index 1).
 *
 * @param    p_emcy          Pointer to the EMCY object.
 *
 * @param    err_code_ix     EMCY error index in User EMCY table.
 *
 * @param    p_usr           Pointer to the manufacturer specific fields in EMCY history and/or EMCY
 *                           message.
 *
 * @note     (1) The parameter p_emcy shall be checked by the calling function.
 *******************************************************************************************************/
static void CANopen_EmcyHistAdd(CANOPEN_EMCY     *p_emcy,
                                CPU_INT08U       err_code_ix,
                                CANOPEN_EMCY_USR *p_usr)
{
  CANOPEN_NODE *p_node;                                         // Local: ptr to parent node
  CANOPEN_DICT *p_cod;                                          // Local: ptr to object directory
  CANOPEN_OBJ  *p_obj;                                          // Local: ptr to object entry
  CPU_INT32U   val = 0;                                         // Local: EMCY history value
  CPU_INT08U   sub;                                             // Local: loop cntr through subidx
  RTOS_ERR     local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

#if (CANOPEN_EMCY_HIST_MAN_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(p_usr);                                       // unused; prevent compiler warning
#endif
  if (p_emcy->Hist.TotLen == 0) {                               // see, if EMCY history is not available
    return;                                                     // abort add function
  }

  p_node = p_emcy->NodePtr;                                     // get ptr to parent node
  p_cod = &p_node->Dict;                                        // get ptr to object dictionary

  p_emcy->Hist.Offset++;                                        // move last write subidx to next entry
  if (p_emcy->Hist.Offset > p_emcy->Hist.TotLen) {              // see, if last entry is passed
    p_emcy->Hist.Offset = 1;                                    // switch back to first entry
  }
  sub = p_emcy->Hist.Offset;                                    // get next write subidx
  val = (CPU_INT32U)p_emcy->RootUserTblPtr[err_code_ix].Code;   // set emcy code
#if (CANOPEN_EMCY_HIST_MAN_EN == DEF_ENABLED)
  if (p_usr != DEF_NULL) {                                      // see, if user infos are given
    val |= (((CPU_INT32U)p_usr->Hist) << 16);                   // set manufacturer specific information field
  }
#endif
  p_obj = CANopen_DictFind(p_cod,
                           CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, sub),
                           &local_err);                         // get object of ring buffer EMCY entry
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
    return;
  }

  CANopen_ObjDirectWr(p_obj, &val, 4, &local_err);              // write emergency in ring buffer entry
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.WrPreDefErrFailCtr);
    return;
  }

  p_emcy->Hist.HistQty++;                                       // increment number of entries in history
  if (p_emcy->Hist.HistQty > p_emcy->Hist.TotLen) {             // see, if max. number is passed
    p_emcy->Hist.HistQty = p_emcy->Hist.TotLen;                 // limit to max. number of entries
  } else {
    p_obj = CANopen_DictFind(p_cod,
                             CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, 0),
                             &local_err);                       // get object of EMCY numbers
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                               p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
      return;
    }
    //                                                             write number of emergency in entry
    CANopen_ObjDirectWr(p_obj, &(p_emcy->Hist.HistQty), 1, &local_err);
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                             p_node->CntErrPtr->Emcy.WrPreDefErrFailCtr);
  }
}
#endif

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                      CANopen_EmcyHistRd()
 *
 * @brief    Corrects read access within the EMCY History object entry. (index 0x1003: Pre-defined error
 *           field).
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to EMCY history object entry.
 *
 * @param    p_buf           Pointer to buffer memory.
 *
 * @param    len             Length of buffer memory.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) When accessing the EMCY history entries, the sub-index is remapped to the correct
 *               value, because the ring buffer must be transferred into a LIFO list.
 *******************************************************************************************************/
static void CANopen_EmcyHistRd(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               void                *p_buf,
                               CPU_INT32U          len,
                               RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_EMCY *p_emcy;                                         // Local: ptr to EMCY structure
  CPU_INT08U   sub;                                             // Local: addressed subidx
  CPU_INT08U   map;                                             // Local: mapped subidx

  p_emcy = &p_node->Emcy;                                       // get ptr to EMCY management
  sub = CANOPEN_OBJ_GET_SUBIX(p_obj->Key);                      // get addressed subindex

  if (sub == 0) {                                               // see, if special case: subidx 0
    CANopen_ObjDirectRd(p_obj, p_buf, len, p_err);              // read number of entries directly
  } else {                                                      // otherwise: read within history
    if (sub <= p_emcy->Hist.HistQty) {                          // see, if subidx is in range
      if (sub <= p_emcy->Hist.Offset) {                         // yes: see if mapped subidx is lower
        map = p_emcy->Hist.Offset - (sub - 1);
      } else {                                                  // otherwise: mapped subidx is higher
        map = (p_emcy->Hist.TotLen - (sub - 1))
              + p_emcy->Hist.Offset;
      }

      p_obj = CANopen_DictFind(&p_node->Dict,
                               CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_PRE_DEF_ERR, map),
                               p_err);                          // get object of mapped EMCY entry
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                                 p_node->CntErrPtr->Emcy.FindPreDefErrFailCtr);
        return;
      }

      CANopen_ObjDirectRd(p_obj, p_buf, len, p_err);            // read number of mapped entry
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE,
                               p_node->CntErrPtr->Emcy.RdErrRegFailCtr);
    }
  }
}
#endif

#if (CANOPEN_EMCY_HIST_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                      CANopen_EmcyHistWr()
 *
 * @brief    Corrects write access within the EMCY History object entry (index 0x1003: Pre-defined error
 *           field).
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to EMCY history object entry.
 *
 * @param    p_buf           Pointer to buffer memory.
 *
 * @param    len             Length of buffer memory.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) The access is only allowed in sub-index 0. the only writable value is 0. The result
 *               should be a complete reset of the EMCY history.
 *******************************************************************************************************/
static void CANopen_EmcyHistWr(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               void                *p_buf,
                               CPU_INT32U          len,
                               RTOS_ERR            *p_err)
{
  CPU_INT08U val = 0;                                           // Local: EMCY history value
  CPU_INT08U sub;                                               // Local: addressed subidx

  PP_UNUSED_PARAM(len);
  sub = CANOPEN_OBJ_GET_SUBIX(p_obj->Key);                      // get addressed subindex

  if (sub == 0) {                                               // see, if special case: subidx 0
    val = (CPU_INT08U)(*(CPU_INT32U *)p_buf);                   // get write value
    if (val == 0) {                                             // see, if write value is 0
      CANopen_EmcyHistReset(node_handle);                       // reset complete EMCY history
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_SDO_ERR_RANGE);
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_SDO_ERR_RANGE);
  }
}
#endif

#endif // CANOPEN_EMCY_MAX_ERR_QTY

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
