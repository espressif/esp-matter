/***************************************************************************//**
 * @file
 * @brief CANopen Counter Management
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _CANOPEN_CTR_PRIV_H_
#define  _CANOPEN_CTR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <canopen_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  CANOPEN_DBG_CTR_ERR_EN
#error  "CANOPEN_DBG_CTR_ERR_EN not #define'd in 'canopen_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"

#elif  ((CANOPEN_DBG_CTR_ERR_EN != DEF_DISABLED) \
  && (CANOPEN_DBG_CTR_ERR_EN != DEF_ENABLED))
#error  "CANOPEN_DBG_CTR_ERR_EN  illegally #define'd in 'canopen_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      CANOPEN COUNTER DATA TYPE
 *******************************************************************************************************/

typedef CPU_INT32U CANOPEN_CTR;                                 ///< Defines max nbr of errs to cnt.

/********************************************************************************************************
 *                                       CANOPEN ERROR DATA TYPES
 *******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
 *                               NETWORK MANAGEMENT (NMT) ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct canopen_ctr_nmt_errs {
  CANOPEN_CTR CmdCheckFailCtr;                                  ///< Nbr of failed cmd checks.
  CANOPEN_CTR HbConsCheckFailCtr;                               ///< Nbr of failed Hearbeat consumer msg checks.
  CANOPEN_CTR HbConsActFailCtr;                                 ///< Nbr of failed Hearbeat consumer activation.

  CANOPEN_CTR CmdResetCommProcessFailCtr;                       ///< Nbr of failed RESET COMM cmd processing.
  CANOPEN_CTR CmdResetNodeProcessFailCtr;                       ///< Nbr of failed RESET NODE cmd processing.
  CANOPEN_CTR CmdEnterPreopProcessFailCtr;                      ///< Nbr of failed ENTER PRE-OP cmd processing.
  CANOPEN_CTR CmdStartProcessFailCtr;                           ///< Nbr of failed START cmd processing.
  CANOPEN_CTR CmdStopProcessFailCtr;                            ///< Nbr of failed STOP cmd processing.

  CANOPEN_CTR InitFailCtr;                                      ///< Nbr of failed NMT service init.
  CANOPEN_CTR TransitionToInitStateFailCtr;                     ///< Nbr of failed transitions to INIT state.
  CANOPEN_CTR BootUpMsgFailCtr;                                 ///< Nbr of failed boot up message transmissions.
  CANOPEN_CTR IF_ResetFailCtr;                                  ///< Nbr of failed CAN interface reset.
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
  CANOPEN_CTR EmcyResetFailCtr;                                 ///< Nbr of failed emergency service reset.
#endif
  CANOPEN_CTR SdoInitFailCtr;                                   ///< Nbr of failed SDO service init.
#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
  CANOPEN_CTR ParamLoadFailCtr;                                 ///< Nbr of failed parameters loads.
#endif
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  CANOPEN_CTR SyncInitFailCtr;                                  ///< Nbr of failed SYNC service init.
#endif
} CANOPEN_CTR_NMT_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                              SERVICE DATA OBJECTS (SDO) ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct canopen_ctr_sdo_errs {
  CANOPEN_CTR RespPreparationFailCtr;                           ///< Nbr of failed SDO response preparations.
  CANOPEN_CTR RespTxFailCtr;                                    ///< Nbr of failed SDO response transmissions.
  CANOPEN_CTR ObjRdFailCtr;                                     ///< Nbr of failed object read.
  CANOPEN_CTR ObjWrFailCtr;                                     ///< Nbr of failed object write.
  CANOPEN_CTR EnFailCtr;                                        ///< Nbr of failed SDO enable.
  CANOPEN_CTR DictFindFailCtr;                                  ///< Nbr of failed dictionary find.
} CANOPEN_CTR_SDO_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                   SYNCHRONIZATION ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct canopen_ctr_sync_errs {
  CANOPEN_CTR TpdoTxFailCtr;                                    ///< Nbr of failed TPDO transfers.
  CANOPEN_CTR RpdoObjDictionaryWrFailCtr;                       ///< Nbr of failed RPDO data written to obj dictionary.
} CANOPEN_CTR_SYNC_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                        EMERGENCY ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct canopen_ctr_emcy_errs {
  CANOPEN_CTR TxFailCtr;                                        ///< Nbr of failed transmit.
  CANOPEN_CTR RdErrRegFailCtr;                                  ///< Nbr of failed read error register.
  CANOPEN_CTR WrErrRegFailCtr;                                  ///< Nbr of failed write error register.
  CANOPEN_CTR RdPreDefErrorFailCtr;                             ///< Nbr of failed read pre-defined error register.
  CANOPEN_CTR WrPreDefErrFailCtr;                               ///< Nbr of failed write pre-defined error register.
  CANOPEN_CTR FindPreDefErrFailCtr;                             ///< Nbr of failed find pre-defined error register.
  CANOPEN_CTR RdIdFailCtr;                                      ///< Nbr of failed read identifier.
} CANOPEN_CTR_EMCY_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                  PROCESS DATA EXCHANGE ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct canopen_ctr_pdo_errs {
  CANOPEN_CTR RdIdFailCtr;                                      ///< Nbr of failed read PDO identification.
  CANOPEN_CTR RdMapFailCtr;                                     ///< Nbr of failed read PDO mapping.
  CANOPEN_CTR TPdoRstFailCtr;                                   ///< Nbr of failed TPDO reset.
  CANOPEN_CTR RPdoRstFailCtr;                                   ///< Nbr of failed RPDO reset.
  CANOPEN_CTR RPdoWrFailCtr;                                    ///< Nbr of failed RPDO write.
  CANOPEN_CTR TPdoTxFailCtr;                                    ///< Nbr of failed TPDO transmit.
  CANOPEN_CTR PdoTmrCreateFailCtr;                              ///< Nbr of failed PDO timer create.
} CANOPEN_CTR_PDO_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct canopen_ctr_errs {
  CANOPEN_CTR_SDO_ERRS  Sdo;
  CANOPEN_CTR_NMT_ERRS  Nmt;
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  CANOPEN_CTR_SYNC_ERRS Sync;
#endif
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
  CANOPEN_CTR_EMCY_ERRS Emcy;
#endif
#if ((CANOPEN_RPDO_MAX_QTY > 0) \
  || (CANOPEN_TPDO_MAX_QTY > 0))
  CANOPEN_CTR_PDO_ERRS Pdo;
#endif
} CANOPEN_CTR_ERRS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CANOPEN COUNTER MACRO'S
 *******************************************************************************************************/

#if (CANOPEN_DBG_CTR_ERR_EN == DEF_ENABLED)
#define  CANOPEN_CTR_ERR_INC(err_ctr) \
  {                                   \
    (err_ctr)++;                      \
  }

#define  CANOPEN_CTR_ERR_INC_COND(cond, err_ctr) \
  {                                              \
    if ((cond)) {                                \
      (err_ctr)++;                               \
    }                                            \
  }

#else
#define  CANOPEN_CTR_ERR_INC(err_ctr)
#define  CANOPEN_CTR_ERR_INC_COND(cond, err_ctr)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_CTR_PRIV_H_
