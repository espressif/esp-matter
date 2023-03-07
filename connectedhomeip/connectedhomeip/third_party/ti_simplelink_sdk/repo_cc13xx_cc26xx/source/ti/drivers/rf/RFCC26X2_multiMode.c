/*
* Copyright (c) 2015-2020, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* *  Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* *  Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* *  Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/drivers/Temperature.h>
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/utils/List.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_rfc_rat.h)
#include DeviceFamily_constructPath(inc/hw_rfc_dbell.h)
#include DeviceFamily_constructPath(inc/hw_fcfg1.h)
#include DeviceFamily_constructPath(driverlib/rfc.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/aon_ioc.h)
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/adi.h)
#include DeviceFamily_constructPath(driverlib/aon_rtc.h)
#include DeviceFamily_constructPath(driverlib/chipinfo.h)
#include DeviceFamily_constructPath(driverlib/osc.h)
#include DeviceFamily_constructPath(driverlib/ccfgread.h)

#if defined(__IAR_SYSTEMS_ICC__)
#pragma diag_remark=Pa082
#endif

#if defined(RF_SINGLEMODE)
#error "An incompatible symbol (RF_SINGLEMODE) is defined in the project. \
        To build with this driver, remove the RF_SINGLEMODE token definition."
#endif

/*-------------- Typedefs, structures & defines ---------------*/

/* Definition of internal state-machine events. */
typedef enum RF_FsmEvent_ {
    RF_FsmEventLastCommandDone  = (1UL << 1),   /* Indicates that a radio command is finished. */
    RF_FsmEventWakeup           = (1UL << 2),   /* Used to initiate the power up sequence of the RF core. */
    RF_FsmEventPowerDown        = (1UL << 3),   /* Used to initiate the power down sequence of the RF core. */
    RF_FsmEventInitChangePhy    = (1UL << 10),  /* Used to initiate the PHY change sequence. */
    RF_FsmEventFinishChangePhy  = (1UL << 11),  /* Used to finalize the PHY change sequence. */
    RF_FsmEventCpeInt           = (1UL << 14),  /* Generated during command execution. */
    RF_FsmEventPowerStep        = (1UL << 29),  /* Generated during the power up sequence of RF core. */
    RF_FsmEventRunScheduler     = (1UL << 30)   /* Used to invoke the scheduler again to check for conflicts. */
} RF_FsmEvent;

/* Definition of states of RF core. */
typedef enum RF_CoreStatus_ {
    RF_CoreStatusIdle            = 0,        /* The RF core is OFF. */
    RF_CoreStatusPoweringUp      = 1,        /* The RF core is being powered up. */
    RF_CoreStatusActive          = 2,        /* The RF core is ON. */
    RF_CoreStatusPoweringDown    = 3,        /* The RF core is being powered down. */
    RF_CoreStatusPhySwitching    = 4         /* The RF core is being reconfigured. */
} RF_CoreStatus;

/* Definition of internal power constraints. Note that the physical RAT channels in the RF core are
   not a one-to-one map to the constraint values here. */
typedef enum RF_PowerConstraintSrc_ {
    RF_PowerConstraintNone      = 0,
    RF_PowerConstraintRatCh0    = (1U << 0), /* Indicates that the Channel 0 of RAT timer is running. */
    RF_PowerConstraintRatCh1    = (1U << 1), /* Indicates that the Channel 1 of RAT timer is running. */
    RF_PowerConstraintRatCh2    = (1U << 2), /* Indicates that the Channel 2 of RAT timer is running. */
    RF_PowerConstraintCmdQ      = (1U << 3), /* Indicates that the RF core executing a radio command. */
    RF_PowerConstraintDisallow  = (1U << 7)  /* Disable automatic power management. */
} RF_PowerConstraintSrc;

/* Definition of internal Radio Timer (RAT) modes. */
typedef enum RF_RatMode_ {
    RF_RatModeUndefined         = 0,        /* Indicates that the RAT channel is not configured. */
    RF_RatModeCompare           = 1,        /* Indicates that the RAT channel is configured to compare mode. */
    RF_RatModeCapture           = 2         /* Indicates that the RAT channel is configured to capture mode. */
} RF_RatMode;

/* Definition of internal Radio Timer (RAT) states. */
typedef enum RF_RatStatus_ {
    RF_RatStatusIdle            = 0,        /* Indicates that the RAT channel is not used. */
    RF_RatStatusPending         = 1,        /* Indicates that the RAT channel is configured, but the RAT timer is not running (i.e. RF core is OFF). */
    RF_RatStatusRunning         = 2         /* Indicates that the RAT channel is configured, and the RAT timer is running. */
} RF_RatStatus;

/* Definition of internal status codes of command shceduling. */
typedef enum RF_ScheduleCmdStatus_ {
    RF_ScheduleCmdSuccess       = 0,        /* Schedule command success. */
    RF_ScheduleCmdAllocError    = 1,        /* Schedule command allocation error (such as queue is full). */
    RF_ScheduleCmdSchError      = 2         /* SChedule command scheduler error (timing or priority conflict). */
} RF_ScheduleCmdStatus;

/*-------------- Macros ---------------*/

#define ABS(x)     (((x) < 0)   ? -(x) : (x))
#define MAX(x,y)   (((x) > (y)) ?  (x) : (y))
#define MIN(x,y)   (((x) < (y)) ?  (x) : (y))
#define UDIFF(x,y) (((y) > (x)) ? ((y) - (x)) : ((~0) + (y) - (x) + (1)))
#define ADD(x,y)   ((x > ((~0) - (y))) ? (~0) : ((x) + (y)))

/*-------------- Defines ---------------*/

/* Max # of RF driver clients */
#define N_MAX_CLIENTS                          2
/* 8 RF_Cmds in pool */
#define N_CMD_POOL                             8
/* Modulus mask used for RF_CmdHandle calculations */
#define N_CMD_MODMASK                          0xFFF

/*-------------- Internal RF constants ---------------*/

#define RF_CMD0                                0x0607
#define RF_BOOT0                               0xE0000011
#define RF_BOOT1                               0x00000080
/* Accessible RF Core interrupts mask MSB 32 bits : RFHW int, LSB 32 bits : RF CPE int */
#define RF_INTERNAL_IFG_MASK                   0xFFFFFFDF60001000
#define RF_TERMINATION_EVENT_MASK              (RF_EventLastCmdDone | RF_EventLastFGCmdDone | RF_EventCmdAborted | RF_EventCmdStopped | RF_EventCmdCancelled)
#define RF_CMD_FG_CMD_FLAG                     (1 << 4)
#define RF_CMD_ALLOC_FLAG                      (1 << 7)
#define RF_CMD_TERMINATED                      (DONE_OK | ERROR_PAST_START)
#define RF_HW_INT_RAT_CH_MASK                  (RFC_DBELL_RFHWIFG_RATCH7 | RFC_DBELL_RFHWIFG_RATCH6 | RFC_DBELL_RFHWIFG_RATCH5)
#define RF_RAT_CH_CNT                          3
#define RF_HW_INT_CPE_MASK                     RFC_DBELL_RFHWIFG_MDMSOFT
#define RF_CPE0_INT_MASK                       0xFFFFFFFF
/* Default value for power up duration (in us) used before first power cycle */
#define RF_DEFAULT_POWER_UP_TIME               2500
/* Default minimum power up duration (in us) */
#define RF_DEFAULT_MIN_POWER_UP_TIME           300
/* Default power-up margin (in us) to account for wake-up sequence outside the RF power state machine */
#define RF_DEFAULT_POWER_UP_MARGIN             314
/* Default phy-switching margin (in us) to account for overhead of processing time on the system MCU. */
#define RF_DEFAULT_PHY_SWITCHING_MARGIN        314
/* Default power down duration in us */
#define RF_DEFAULT_POWER_DOWN_TIME             1000
#define RF_MAX_CHAIN_CMD_LEN                   8
/* RAT channel (0-4) are used by RF Core. Only 5,6,7 are available for application */
#define RF_RAT_CH_LOWEST                       5
#define RF_SEND_RAT_STOP_RATIO                 7
#define RF_RTC_CONV_TO_US_SHIFT                12
#define RF_SHIFT_4_BITS                        4
#define RF_SHIFT_8_BITS                        8
#define RF_SHIFT_16_BITS                       16
#define RF_SHIFT_32_BITS                       32
#define RF_RTC_TICK_INC                        (0x100000000LL/32768)
#define RF_SCALE_RTC_TO_4MHZ                   4000000
#define RF_NUM_RAT_TICKS_IN_1_US               4
/* (3/4)th of a full RAT cycle, in us */
#define RF_DISPATCH_MAX_TIME_US                (UINT32_MAX / RF_NUM_RAT_TICKS_IN_1_US * 3 / 4)
/* (1/4)th of a full RAT cycle, in us */
#define RF_DISPATCH_MAX_TIME_WRAPAROUND_US     (int32_t)(RF_DISPATCH_MAX_TIME_US - UINT32_MAX / RF_NUM_RAT_TICKS_IN_1_US)
#define RF_DISPATCH_INFINIT_TIME               (UINT32_MAX)
#define RF_XOSC_HF_SWITCH_CHECK_PERIOD_US      50
#define RF_DEFAULT_AVAILRATCH_VAL              0x7
#define RF_ABORT_FLUSH_ALL                     0x2
#define RF_CMDSTA_REG_VAL_MASK                 0xFF
#define RF_RAT_CAPTURE_REPEAT_MODE             0x10000000
#define RF_RAT_INTERRUPT_BASE_INDEX            0x01
#define RF_RAT_ERROR_BASE_INDEX                0x10
#define RF_RAT_COMPENSATION_TIME_US            25
#define RF_PHY_SWITCHING_MODE                  1
#define RF_PHY_BOOTUP_MODE                     0
#define RF_SCH_CMD_TIMING_INSERT               0x4
#define RF_REQ_ACCESS_MAX_DUR_US               1000000
/* Additional analog config time for setup command */
#define RF_ANALOG_CFG_TIME_US                  96
/* Update analog configuration in setup */
#define RF_SETUP_ANALOGCFG_UPDATE              0
/* Don't update analog configuration in setup */
#define RF_SETUP_ANALOGCFG_NOUPDATE            0x2D
#define RF_SCH_CMD_STARTTIME_NOW               0
#define RF_SCH_CMD_ENDTIME_IGNORE              0
#define RF_DEFAULT_PHY_SWITCHING_TIME          500
#define RF_RADIOFREECB_PREEMPT_FLAG            0x1
#define RF_RADIOFREECB_REQACCESS_FLAG          0x2
#define RF_RADIOFREECB_CMDREJECT_FLAG          0x4
#define RF_DEFAULT_RAT_RTC_ERR_TOL_IN_US       5
/* Approx for 1e6 / 500. XTAL drift is 500 ppm */
#define RF_DEFAULT_COMB_XTAL_DRIFT_BITS_SHIFT  11
/* Window (in us) to decide if wakeup was from RF power up clock */
#define RF_WAKEUP_DETECTION_WINDOW_IN_US       300
/* Ieee context mask and background value */
#define RF_IEEE_ID_MASK                        0xFC00
#define RF_IEEE_FG_CMD                         0x2C00
/* Defines for to mask High-PA overrides. */
#define RF_TX20_ENABLED                        0xFFFF
#define RF_TX20_PATYPE_ADDRESS                 0x21000345
#define RF_TX20_PATYPE_MASK                    0x04
#define RF_TX20_GAIN_ADDRESS                   0x2100034C
#define RF_TX20_GAIN_MASK                      0x003FFFFF
#define RF_TX20_PATTERN                        TX20_POWER_OVERRIDE(0)
#define RF_TXSTD_PATTERN                       TX_STD_POWER_OVERRIDE(0)
#define RF_TX_OVERRIDE_MASK                    0x000003FF
#define RF_TX_OVERRIDE_SHIFT                   10
#define RF_TX_OVERRIDE_INVALID_OFFSET          0xFF
/* Defines for update of the HPOSC override */
#define RF_HPOSC_OVERRIDE_PATTERN              HPOSC_OVERRIDE(0)
#define RF_HPOSC_OVERRIDE_MASK                 0xFFFF
/* Common defines for override handling*/
#define RF_OVERRIDE_SEARCH_DEPTH               80
/* Define for HPOSC temperature limits */
#define RF_TEMP_LIMIT_3_DEGREES_CELSIUS        0x3

/*-------------- Structures and definitions ---------------*/

/* FSM typedef. */
typedef void (*RF_FsmStateFxn)(RF_Object*, RF_FsmEvent const);

/* Rat channel configuration. */
typedef struct RF_RatChannel_s RF_RatChannel;

/* Rat channel configuration. */
struct RF_RatChannel_s {
  RF_Handle      pClient;        /* Pointer to client owning the channel. NULL means the channel is free. */
  RF_RatCallback pCb;            /* Callback pointer of the channel. */
  RF_RatMode     mode;           /* Mode of this RAT channel: RF_RatModeCompare, etc. */
  RF_RatHandle   handle;         /* Channel number: 0,1,2. */
  RF_RatStatus   status;         /* Status of the channel: RF_RatStatusIdle, RF_RatStatusPending, RF_RatStatusRunning */
  uint64_t       chCmd;          /* Generic storage for the command structure itself. */
  uint32_t       ioCmd;          /* Raw binary to be sent to the CM0 to set up the GPOs. This is optional. */
};

/* Rat module configuration. */
typedef struct RF_RatModule_s RF_RatModule;

/* Rat module configuration. */
struct RF_RatModule_s {
    RF_RatChannel     channel[RF_RAT_CH_CNT];  /* Container of channel configurations. */
    uint8_t           availableRatChannels;    /* Storage of available RAT channels read from the RF core. */
    uint8_t volatile  pendingInt;              /* Pending interrupt flags to be served. */
    uint8_t           numActiveChannels;       /* Counter of active channels. This is used to compensate the
                                                  overhead of programming the channels.*/
};

/* RF core configuration. */
typedef struct RF_CoreState_s RF_CoreState;

/* RF core configuration.  */
struct RF_CoreState_s
{
  RF_CoreStatus volatile status;
  RF_FsmStateFxn fxn;
  uint32_t       activeTimeUs;
  bool           init;
  bool           manualXoscHfSelect;
};

/* RAT synchronization. */
typedef union RF_RatSyncCmd_u RF_RatSyncCmd;

/* RAT synchronization. */
union RF_RatSyncCmd_u
{
    rfc_CMD_SYNC_START_RAT_t start;
    rfc_CMD_SYNC_STOP_RAT_t  stop;
};

/* Reconfigure the PA settings. */
typedef union RF_ConfigurePaCmd_u RF_ConfigurePaCmd;

/* Reconfigure the PA settings. */
union RF_ConfigurePaCmd_u {
    rfc_CMD_SET_TX_POWER_t   tuneTxPower;
    rfc_CMD_SET_TX20_POWER_t tuneTx20Power;
    rfc_CMD_CHANGE_PA_t      changePa;
};

/* Command queue. */
typedef struct RF_CmdQ_s RF_CmdQ;

/* Command queue. */
struct RF_CmdQ_s{
    List_List    pPend;               /* List of pending commands to be dispatched. */
    List_List    pDone;               /* List of executed commands to be served. */
    RF_Cmd*      volatile pCurrCmdBg; /* Currently running command. */
    RF_Cmd*      volatile pCurrCmdFg; /* Currently running foreground command. */
    RF_Cmd*      volatile pCurrCmdCb; /* Command which callback to be invoked. */
    RF_CmdHandle volatile nSeqPost;   /* Sequence # for previously posted command. */
    RF_CmdHandle volatile nSeqDone;   /* Sequence # for last done command. */
};

/* RF scheduler. */
typedef struct RF_Sch_s RF_Sch_t;

/* RF scheduler. */
struct RF_Sch_s {
    RF_Handle       clientHnd[N_MAX_CLIENTS];                   /* Client handle for each registered client. */
    RF_AccessParams accReq[N_MAX_CLIENTS];                      /* Input parameters from any RF_requestAccess API calls. */
    RF_Handle       clientHndRadioFreeCb;                       /* Client handle for the radio callback. */
    uint8_t         issueRadioFreeCbFlags;                      /* Indicate if driver needs to issue RF_EventRadioFree callback {0:pre-emption, 1:requestAccess running, 2: reject command}. */
    uint8_t         cmdInsertFlags;                             /* Indicate if the command was inserted based on timing information. */
};

/*-------------- RTOS objects ---------------*/

/* RF core software interrupts */
static SwiP_Struct RF_swiFsmObj;
static void RF_swiFsm(uintptr_t a, uintptr_t b);

/* RF core hardware interrupts */
static HwiP_Struct RF_hwiCpe0Obj;
static void RF_hwiCpe0Active(uintptr_t a);
static void RF_hwiCpe0PowerFsm(uintptr_t a);

/* RF core HW software interrupts */
static SwiP_Struct RF_swiHwObj;
static void RF_swiHw(uintptr_t a, uintptr_t b);

/* RF core HW hardware interrupts */
static HwiP_Struct RF_hwiHwObj;
static void RF_hwiHw(uintptr_t a);

/* Clock used for triggering power-up sequences */
static ClockP_Struct RF_clkPowerUpObj;
static void RF_clkPowerUp(uintptr_t a);

/* Common inactivity timeout clock callback */
static ClockP_Struct RF_clkInactivityObj;
static void RF_clkInactivityCallback(uintptr_t a);

/* Common request access timeout clock callback */
static void RF_clkReqAccess(uintptr_t a);


/*-------------- Static structures ---------------*/

/* Default RF parameters structure */
static const RF_Params RF_defaultParams = {
    .nInactivityTimeout          = SemaphoreP_WAIT_FOREVER,
    .nPowerUpDuration            = 0,
    .pPowerCb                    = NULL,
    .pErrCb                      = NULL,
    .nPowerUpDurationMargin      = RF_DEFAULT_POWER_UP_MARGIN,
    .nPhySwitchingDurationMargin = RF_DEFAULT_PHY_SWITCHING_MARGIN,
    .pClientEventCb              = NULL,
    .nClientEventMask            = 0,
    .nID                         = 0,
};

/*-------------- Global variables ---------------*/

/* RF_Cmd container pool. Containers with extra information about RF commands. */
static RF_Cmd                   RF_cmdPool[N_CMD_POOL];

/* Command queue top level structure. It contains pointers to the different queues. */
static RF_CmdQ                  RF_cmdQ;

/* Static object used to subscribe from early notification in the power driver */
static Power_NotifyObj          RF_wakeupNotifyObj;

/* Power constraints set by the RF driver */
static volatile uint8_t         RF_powerConstraint;

/* Pointer to current radio client (indicates also whether the radio is powered) */
static RF_Object*               RF_currClient;

/* Current state of the RF core. */
static RF_CoreState             RF_core;

/* Static container of a direct/immediate commands */
static RF_RatModule             RF_ratModule;

/* Commands used to synchronize the RTC and the RAT timer. */
static volatile RF_RatSyncCmd   RF_ratSyncCmd;

/* Top level structure of the shceduler unit. */
static RF_Sch_t                 RF_Sch;

/* Variables used for powerUpDuration, phySwitchingTime and RAT sync time calculation. */
static uint32_t                 RF_rtcTimestampA;     /* RTC timer value power-up and active time calculation. */
static uint32_t                 RF_rtcBeginSequence;  /* RTC timer value for switching time calculation. */
static uint32_t                 RF_errTolValInUs;     /* max allowed error between RAT/RTC drift to enable resync at power-down (in us). */

/* Counter of radio clients */
static uint8_t                  RF_numClients;

/* Current HPOSC frequency offset */
static int32_t                  RF_currentHposcFreqOffset;

/* Temperature notify object used by HPOSC device */
static Temperature_NotifyObj    RF_hposcRfCompNotifyObj;

/* Common system-level temperature based synth compensation  */
static void (*pfnUpdateHposcOverride)(uint32_t *pRegOverride) = NULL;
static int_fast16_t (*pfnTemperatureUnregisterNotify)(Temperature_NotifyObj *notifyObject) = NULL;

/*-------------- Externs ---------------*/

/* Hardware attribute structure populated in board file. */
extern const RFCC26XX_HWAttrsV2 RFCC26XX_hwAttrs;

/* Software policy set in the board file and implements the distributed scheduling algorithm. */
__attribute__((weak)) RFCC26XX_SchedulerPolicy RFCC26XX_schedulerPolicy = {
    .submitHook   = RF_defaultSubmitPolicy,
    .executeHook  = RF_defaultExecutionPolicy
};

/*-------------- Booleans ---------------*/

/* variable to indicate with the FLASH is disable during the power up */
static bool bDisableFlashInIdleConstraint;

/*-------------- State machine functions ---------------*/

/* FSM state functions */
static void             RF_fsmPowerUpState(RF_Object *pObj, RF_FsmEvent e);
static void             RF_fsmSetupState(RF_Object *pObj, RF_FsmEvent e);
static void             RF_fsmActiveState(RF_Object *pObj, RF_FsmEvent e);
static void             RF_fsmXOSCState(RF_Object *pObj, RF_FsmEvent e);

/*-------------- Helper functions ---------------*/

/* Command queue handling */
static RF_Cmd*          RF_queueEnd(RF_Handle h, List_List* pHead);

/* Command handling*/
static bool             RF_isClientOwner(RF_Handle h, RF_Cmd* pCmd);
static RF_Cmd*          RF_cmdAlloc(void);
static RF_Cmd*          RF_cmdGet(RF_Handle h, RF_CmdHandle ch, uint8_t mask);
static void             RF_cmdStoreEvents(RF_Cmd* pCmd, RF_EventMask events);
static bool             RF_cmdDispatchTime(uint32_t* dispatchTimeClockTicks, bool conflict, RF_Cmd** pAbsCmd);
static RF_Stat          RF_abortCmd(RF_Handle h, RF_CmdHandle ch, bool graceful, bool flush, bool preempt);
static bool             RF_checkCmdFsError(void);
static void             RF_cacheFsCmd(RF_Cmd* pCmd);
static uint32_t         RF_discardPendCmd(RF_Handle h, RF_Cmd* pCmd, bool bFlushAll, bool bPreempt);

/* Scheduler */
static void              RF_issueRadioFreeCb(uint8_t src);
static bool              RF_verifyGap(RF_Cmd* newCmd, RF_Cmd* prevCmd, RF_Cmd* nextCmd);
static RF_ScheduleStatus RF_howToSchedule(RF_Cmd* newCmd, RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue, RF_Cmd** pInsertLocation);

/* RAT module */
static RF_RatChannel*   RF_ratGetChannel(uint8_t ch);
static RF_RatChannel*   RF_ratAllocChannel(RF_RatHandle ratChannel);
static uint32_t         RF_ratGetChannelValue(RF_RatHandle ratHandle);
static uint32_t         RF_ratGetValue(void);
static void             RF_ratGenerateChCmd(RF_RatChannel* ratCh, void* ratConfig);
static void             RF_ratGenerateIoCmd(RF_RatChannel* ratCh, RF_RatConfigOutput* ioConfig);
static RF_RatHandle     RF_ratSetupChannel(RF_Handle ratClient, RF_RatMode ratMode, RF_RatCallback ratCallback, RF_RatHandle ratChannel, void* ratConfig, RF_RatConfigOutput* ioConfig);
static void             RF_ratRestartChannels(void);
static RF_Stat          RF_ratArmChannel(RF_RatChannel* ratCh);
static void             RF_ratFreeChannel(RF_RatChannel* ratCh);
static void             RF_ratSuspendChannels(void);
static bool             RF_ratReleaseChannels(void);
static bool             RF_ratDispatchTime(uint32_t* dispatchTimeClockTicks);
static bool             RF_ratIsRunning(void);

/* Time management */
static uint32_t         RF_calculateDeltaTimeUs(uint32_t absTime, uint32_t nTotalPowerUpDuration);
static bool             RF_calculateDispatchTime(uint32_t* dispatchTimeClockTicks);
static void             RF_dispatchNextEvent(void);
static void             RF_dispatchNextCmd(void);
static void             RF_restartClockTimeout(ClockP_Handle clock, uint32_t timeout);

/* Power management */
static void             RF_corePowerDown(void);
void                    RF_powerConstraintRelease(RF_PowerConstraintSrc src);
void                    RF_powerConstraintSet(RF_PowerConstraintSrc src);
RF_PowerConstraintSrc   RF_powerConstraintGet(RF_PowerConstraintSrc src);
static void             RF_setInactivityTimeout(void);

/* Others */
static void             RF_init(void);
static void             RF_defaultCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);
static RF_Stat          RF_runDirectImmediateCmd(RF_Handle h, uint32_t pCmd, uint32_t* rawStatus);
static RF_Stat          RF_executeDirectImmediateCmd(uint32_t pCmd, uint32_t* rawStatus);
static void             RF_invokeGlobalCallback(RF_GlobalEvent event, void* arg);
static void             RF_dbellSubmitCmdAsync(uint32_t rawCmd);
static void             RF_dbellSyncOnAck(void);
static bool             RF_isRadioSetup(RF_Op* pOp);
static void             RF_initRadioSetup(RF_Handle handle);
static void             RF_radioOpDoneCb(void);
static RF_Op*           RF_findEndOfChain(RF_Op* pOp);
static void             RF_applyRfCorePatch(bool mode);
static bool             RF_isStateTransitionAllowed(void);

/* PA management */
static RF_Stat          RF_updatePaConfiguration(RF_RadioSetup* radioSetup, RF_TxPowerTable_Value newValue, RF_ConfigurePaCmd* configurePaCmd);
static void             RF_extractPaConfiguration(RF_Handle handle);
static bool             RF_decodeOverridePointers(RF_RadioSetup* radioSetup, uint16_t** pTxPower, uint32_t** pRegOverride, uint32_t** pRegOverrideTxStd, uint32_t** pRegOverrideTx20);
static void             RF_attachOverrides(uint32_t* baseOverride, uint32_t* newOverride);
static void             RF_detachOverrides(uint32_t* baseOverride, uint32_t* newOverride);

/* HPOSC management */
static void             RF_updateHpOscOverride(uint32_t *pRegOverride);
static void             RF_hposcRfCompensateFxn(int16_t currentTemperature,
                                                int16_t thresholdTemperature,
                                                uintptr_t clientArg,
                                                Temperature_NotifyObj *notifyObject);

/*-------------- Command queue internal functions ---------------*/

/*
 *  Compares the client of a command.
 *
 *  Input:  h     - Client to check against.
 *          pCmd  - Command to check.
 *  Return: true  - If the client owns the command.
 *          false - Otherwise.
 */
static bool RF_isClientOwner(RF_Handle h, RF_Cmd* pCmd)
{
    if (pCmd && (pCmd->pClient == h))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*
 *  Search last entry in simple queue for particular client.
 *
 *  Input:  h    - Client handle.
 *          list - List to search within.
 *  Return: RF_Cmd if found any
 */
static RF_Cmd* RF_queueEnd(RF_Handle h, List_List* list)
{
    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Local variables */
    List_Elem* pTail = NULL;
    List_Elem* pHead = List_head(list);

    /* Start at the head of queue */
    while (pHead)
    {
        if (RF_isClientOwner(h, (RF_Cmd*)pHead))
        {
            pTail = pHead;
        }

        pHead = List_next(pHead);
    }

    /* Exit critical section */
    HwiP_restore(key);

    /* Return with the last entry belongs to the client */
    return((RF_Cmd*)pTail);
}

/*
 *  Allocate a command buffer from the command pool.
 *
 *  Input:  none
 *  Return: RF command
 */
static RF_Cmd* RF_cmdAlloc(void)
{
    uint32_t i;
    for (i = 0; i < N_CMD_POOL; i++)
    {
        /* Find the first available entry in the command pool */
        if (!(RF_cmdPool[i].flags & RF_CMD_ALLOC_FLAG))
        {
            return(&RF_cmdPool[i]);
        }
    }
    return(NULL);
}

/*
 *  Search command in the command pool.
 *
 *  Input:  h    - Handle to the client which the command should belong to.
 *          ch   - Handle to the command to search for.
 *          mask - Optional mask of flags to compare to.
 *  Return: RF command
 */
static RF_Cmd* RF_cmdGet(RF_Handle h, RF_CmdHandle ch, uint8_t mask)
{
    uint32_t i;
    for (i = 0; i < N_CMD_POOL; i++)
    {
        /* Find the allocated command pool entry corresponding to ch */
        if (RF_cmdPool[i].ch == ch)
        {
            if (RF_isClientOwner(h, &RF_cmdPool[i]))
            {
                /* If a mask is provided, check the flags too */
                if (!mask || (RF_cmdPool[i].flags & mask))
                {
                    return(&RF_cmdPool[i]);
                }
            }
        }
    }
    return(NULL);
}

/*
 *  Atomic storage of radio events happened during the execution of a command.
 *
 *  Input:  pCmd  - Command the events belogn to.
 *          events - The radio events to be store within the context of the command.
 *  Return: none
 */
static void RF_cmdStoreEvents(RF_Cmd* pCmd, RF_EventMask events)
{
    /* Enter critical section. */
    uint32_t key = HwiP_disable();

    /* Store the events within the context of the command. */
    if (pCmd)
    {
        /* The field rfifg store the events for the next callback.
           The field pastifg accumulates the events in case an
           RF_pendCmd() API call happens. */
        pCmd->rfifg   |= events;
        pCmd->pastifg |= events;
    }

    /* Exit critical section. */
    HwiP_restore(key);
}

/*
 *  Reconfigure and restart a particular clock object.
 *
 *  Input:  clockObj          - A pointer to a clock object.
 *          timeoutClockTicks - The timeout to be set in unit of clock ticks.
 *  Return: none
 */
static void RF_restartClockTimeout(ClockP_Handle clockHandle, uint32_t timeoutClockTicks)
{
    /* Ceil the value at minimum 1 clock tick. */
    timeoutClockTicks = MAX(timeoutClockTicks, 1);

    /* Reprogram the clock object. */
    ClockP_setTimeout(clockHandle, timeoutClockTicks);
    ClockP_start(clockHandle);
}

/*
 *  Calculate the delta time to an RF event including the overhead of powering up
 *  and down.
 *
 *  Input:  abstime               - The timestamp the event will need to happen.
 *          nTotalPowerUpDuration - The duration we need to compensate with.
 *  Return: deltaTime             - The time left until the RF core need to be trigged.
 */
static uint32_t RF_calculateDeltaTimeUs(uint32_t absTime, uint32_t nTotalPowerUpDuration)
{
    /* Local variables. */
    uint32_t deltaTimeUs;

    /* Read the timestamp to calculate difference from. */
    uint32_t currentTime = RF_getCurrentTime();

    /* Calculate the difference with the current timestamp. */
    deltaTimeUs  = UDIFF(currentTime, absTime);
    deltaTimeUs /= RF_NUM_RAT_TICKS_IN_1_US;

    /* Check if delta time is greater than (powerup duration + power down duration) for a
       power cycle, and is less than 3/4 of a RAT cycle (~17 minutes) */
    if ((deltaTimeUs > (int32_t)(nTotalPowerUpDuration + RF_DEFAULT_POWER_DOWN_TIME)) &&
        (deltaTimeUs <= RF_DISPATCH_MAX_TIME_US))
    {
        /* Dispatch command in the future */
        return(MAX((deltaTimeUs - nTotalPowerUpDuration), 1));
    }
    else
    {
        /* Dispatch immediately */
        return(0);
    }
}

/*
 *  Calculate the wakeup time of next command in the queue.
 *
 *  Input:  dispatchTimeClockTicks - Location where the calculated time is being stored.
 *          conflict               - true: compare to the first command which have TRIG_ABSTIME trigger type.
 *                                   false: compare to the first command in the queue
 *          pAbsCmd                - Pointer to the first command which has an absolute start time.
 *  Return: validTime              - Indicates if the returned time is valid.
 */
static bool RF_cmdDispatchTime(uint32_t* dispatchTimeClockTicks, bool conflict, RF_Cmd** pAbsCmd)
{
    /* By default, there is no command in the queue. */
    RF_Cmd* pCmd   = NULL;
    bool validTime = false;

    /* The input argument determines which command to use as a reference. This is to be able to
       reuse the calculation for both power management and conflict resolution. */
    if (conflict == true)
    {
        /* Start from the beginning of queue. */
        RF_Cmd* pTempCmd = (RF_Cmd*)List_head(&RF_cmdQ.pPend);

        /* Walk the queue and find the first command contains an absolute start time. */
        while (pTempCmd)
        {
            /* Finish the search upon the first match. This assumes that commands with
               absolute start times are in order. */
            if (pTempCmd->pOp->startTrigger.triggerType == TRIG_ABSTIME)
            {
                pCmd = pTempCmd;
                break;
            }
            else
            {
                /* Continue the search if no match. */
                pTempCmd = (RF_Cmd*)List_next((List_Elem*)pTempCmd);
            }
        }
    }
    else
    {
        /* The next command in the queue independently of its type determines the timing. */
        pCmd = (RF_Cmd*)List_head(&RF_cmdQ.pPend);
    }

    /* Only recognizes TRIG_ABSTIME triggers, everything else gets dispatched immediately. */
    if (pCmd)
    {
        /* If there is at least one pending command, we can calculate a legit dispatch time. */
        validTime = true;

        /* Return with the command which we calculate the remained time against. */
        if (pAbsCmd)
        {
          *pAbsCmd = pCmd;
        }

        /* If the start trigger is absolute, we can calculate the time difference. */
        if (pCmd->pOp->startTrigger.triggerType == TRIG_ABSTIME)
        {
            uint32_t nTotalDuration = 0;

            /* Calculate the sum of overhead it takes to start up and configure the RF core. */
            if (conflict == true)
            {
                nTotalDuration = pCmd->pClient->clientConfig.nPhySwitchingDuration
                                      + pCmd->pClient->clientConfig.nPhySwitchingDurationMargin;
            }
            else
            {
                /* If radio wakes up to execute an FS command use nPowerUpDurationFs */
                if ((pCmd->pOp->commandNo == CMD_FS) || (pCmd->pOp->commandNo == CMD_FS_OFF))
                {
                    nTotalDuration = pCmd->pClient->clientConfig.nPowerUpDurationFs
                                    + pCmd->pClient->clientConfig.nPowerUpDurationMargin
                                    + (RF_RAT_COMPENSATION_TIME_US * RF_ratModule.numActiveChannels);
                }
                else
                {
                    nTotalDuration = pCmd->pClient->clientConfig.nPowerUpDuration
                                    + pCmd->pClient->clientConfig.nPowerUpDurationMargin
                                    + (RF_RAT_COMPENSATION_TIME_US * RF_ratModule.numActiveChannels);
                }
            }

            /* Calculate the remained time until this absolute event. The calculation takes
               into account the minimum power cycle time. */
            *dispatchTimeClockTicks = RF_calculateDeltaTimeUs(pCmd->startTime, nTotalDuration);

            /* Scale the value to clock ticks*/
            *dispatchTimeClockTicks /= ClockP_getSystemTickPeriod();
        }
        else
        {
            /* Dispatch immediately. */
            *dispatchTimeClockTicks  = 0;
        }
    }
    else
    {
        /* This value will not be used. */
        *dispatchTimeClockTicks  = 0;
    }

    /* If the returned timestamp represents a valid dispatch time, return with true. */
    return(validTime);
}

/*
 *  Determines if the RAT timer is running (clock is not gated) or not.
 *  This is used to determine if any RAT related command can be execured.
 *
 *  Input:  none
 *  Return: PWMCLK_EN_RAT - RAT timer is running.
 *          0             - RAT timer is not running.
 */
static bool RF_ratIsRunning(void)
{
    /* Assume by default that the RAT is not available. */
    bool status = false;

    /* If the RF core power domain is ON, read the clock of the RAT. */
    if (HWREG(PRCM_BASE + PRCM_O_PDSTAT0) & PRCM_PDSTAT0_RFC_ON)
    {
        status = (bool)(HWREG(RFC_PWR_BASE + RFC_PWR_O_PWMCLKEN) & RFC_PWR_PWMCLKEN_RAT_M);
    }

    /* Return with the status of RAT. */
    return(status);
}

/*
 *  Allocate a RAT channel from the three slots available
 *  for the user.
 *
 *  Input:  ratChannel     - Pointer to a user provided RF_RatHandle.
 *  Return: RF_RatChannel* - Pointer to the allocated channel if success.
 *          NULL           - If failure.
 */
static RF_RatChannel* RF_ratAllocChannel(RF_RatHandle ratChannel)
{
    /* Walk the RAT channel indexes. */
    uint32_t i;
    for (i = 0; i < RF_RAT_CH_CNT; i++)
    {
        /* Calculate the bit representing this channel within the available channels. */
        uint32_t bitMask = (1 << i);

        /* Verify that no one is using this channel (from outside the scope of RF driver). */
        if (RF_ratModule.availableRatChannels & bitMask)
        {
            /* Mask the possible channels if a user handle is provided, otherwise find the
               the first available channel. */
            if ((ratChannel == RF_RatChannelAny) || (ratChannel == i))
            {
                /* Decode the fields of a channel. */
                RF_RatChannel* ratCh = RF_ratGetChannel(i);

                /* If an available channel is found. */
                if (ratCh && (ratCh->status == RF_RatStatusIdle))
                {
                    /* Mark the channel as occupied. */
                    RF_ratModule.availableRatChannels &= ~bitMask;

                    /* Put the channel into pending state. */
                    ratCh->status = RF_RatStatusPending;
                    ratCh->handle = i;

                    /* Increment the counter of active channels. This is used to compensate the
                       power up time with the overhead of programming these channels. */
                    RF_ratModule.numActiveChannels += 1;

                    /* Return with a pointer to the channel. */
                    return(ratCh);
                }
            }
        }
    }

    /* Return with an invalid channel pointer in case of error. */
    return(NULL);
}

/*
 *  Free a given RAT channel.
 *
 *  Input:  ratCh - Pointer to a RAT channel in RF_ratModule.
 *  Return: none
 */
static void RF_ratFreeChannel(RF_RatChannel* ratCh)
{
    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* If a valid pointer is provided */
    if (ratCh && ratCh->status)
    {
        /* Precalculate the contraint ID of this channel. */
        RF_PowerConstraintSrc powerConstraint = (RF_PowerConstraintSrc)(1 << ratCh->handle);

        /* If the RF core power domain is ON. */
        if (RF_ratIsRunning())
        {
            /* Calculate the channel index based on the handle. */
            uint32_t ratChIntFlag = (1 << (RFC_DBELL_RFHWIFG_RATCH5_BITN + ratCh->handle));

            /* Disable the RAT channel interrupt source. */
            RFCHwIntDisable(ratChIntFlag);
            RFCHwIntClear(ratChIntFlag);
        }

        /* Reset the status of the channel. */
        ratCh->status  = RF_RatStatusIdle;
        ratCh->mode    = RF_RatModeUndefined;
        ratCh->pClient = NULL;
        ratCh->pCb     = NULL;
        ratCh->chCmd   = 0;
        ratCh->ioCmd   = 0;

        /* Mark the channel as available. */
        RF_ratModule.availableRatChannels |= (1 << ratCh->handle);

        /* Decrement the counter of active channels. To avoid underflow, check its value first. */
        if (RF_ratModule.numActiveChannels)
        {
            RF_ratModule.numActiveChannels -= 1;
        }

        /* Notify the state machine that the RF core can be possibly powered down. */
        RF_powerConstraintRelease(powerConstraint);
    }

    /* Exit critical section */
    HwiP_restore(key);
}

/*
 *  Returns with a pointer to a RAT channel based on it's handle.
 *
 *  Input:  ch    - Channel handle.
 *  Return: ratCh - Pointer to a RAT channel in RF_ratModule.
 */
static RF_RatChannel* RF_ratGetChannel(uint8_t ch)
{
    /* Convert a valid index into a pointer of a RAT channel configuration. */
    if (ch < RF_RAT_CH_CNT)
    {
        return((RF_RatChannel*)&RF_ratModule.channel[ch]);
    }

    /* Return with NULL in case of invalid input argument. */
    return(NULL);
}

/*
 *  Suspend the running channels and potentially initiate a power down.
 *
 *  Input:  none
 *  Return: true  - All RAT channel is suspended.
 *          false - Otherwise.
 */
static bool RF_ratReleaseChannels(void)
{
    /* Only try to release the RAT channels if there is no other dependencies set. */
    if (!RF_powerConstraintGet(RF_PowerConstraintCmdQ) &&
       !RF_powerConstraintGet(RF_PowerConstraintDisallow))
    {
        /* Calculate if there is enough time to power down and up. */
        uint32_t dispatchTimeClockTicks;
        bool validTime = RF_calculateDispatchTime(&dispatchTimeClockTicks);

        /* If the next event is sufficiently far into the future. */
        if (!validTime || (validTime && dispatchTimeClockTicks))
        {
            /* Avoid powering down the radio if there are callbacks to be served. The
               SWI will try to access the RAT timer, which need to be powered. */
            if (RF_ratModule.pendingInt == 0U)
            {
                /* Suspend all RAT channels. */
                RF_ratSuspendChannels();

                /* RAT channels were suspended. */
                return(true);
            }
        }
    }

    /* RAT channels were not suspended. */
    return(false);
}

/*
 *  Calculate the timeout of closest RAT event.
 *
 *  Input:  dispatchTimeClockTicks - Location where the calculated time is being stored.
 *  Return: validTime              - Indicates if the returned time is valid.
 */
static bool RF_ratDispatchTime(uint32_t* dispatchTimeClockTicks)
{
    /* By default, there is no RAT running. */
    bool validTime = false;

    /* Initialize the return value. */
    *dispatchTimeClockTicks = RF_DISPATCH_INFINIT_TIME;

    /* Iterate through the RAT containers and calculate the remained time until
       the closest RAT event. */
    uint32_t i;
    for(i = 0; i < RF_RAT_CH_CNT; i++)
    {
        /* Use a local pointer to have easier access to member fields. */
        RF_RatChannel* ratCh = RF_ratGetChannel(i);

        /* If the channel is either in PENDING or RUNNING state, meaning it is in use. */
        if (ratCh && ratCh->status)
        {
            /* There is at least one active channel, we can calculate a legit timestamp. */
            validTime = true;

            /* If there is at least one channel in Capture mode, we need to power
               up immediately. */
            if (ratCh->mode == RF_RatModeCapture)
            {
                /* Use immediate timeout orcing the RF core to be powered up. */
                *dispatchTimeClockTicks = 0;

                /* No point to look to the other RAT channels.*/
                break;
            }
            else
            {
                /* Decode the compareTime field. */
                rfc_CMD_SET_RAT_CMP_t* cmd = (rfc_CMD_SET_RAT_CMP_t *)&ratCh->chCmd;
                uint32_t compareTime = cmd->compareTime;

                /* Calculate the remained time until this RAT event. The calculation takes
                   into account the minimum power cycle time. */
                uint32_t deltaTimeUs = RF_calculateDeltaTimeUs(compareTime,
                                                               RF_currClient->clientConfig.nPowerUpDuration +
                                                               RF_currClient->clientConfig.nPowerUpDurationMargin +
                                                               (RF_RAT_COMPENSATION_TIME_US * RF_ratModule.numActiveChannels));

                /* Scale the value to clock ticks. */
                uint32_t deltaTimeClockTicks = deltaTimeUs / ClockP_getSystemTickPeriod();

                /* If this is the closest RAT event, update the timer. */
                if (deltaTimeClockTicks < (*dispatchTimeClockTicks))
                {
                    *dispatchTimeClockTicks = deltaTimeClockTicks;
                }
            }
        }
    }

    /* Return with true if the dispatchTime represents a valid timestamp. */
    return(validTime);
}

/*
 *  Arms a given RAT channel. The mode of the channel will define which mode
 *  it is being configured to. The cmd variable contains the raw word to be
 *  sent to the RF core.
 *
 *  Input:  ratCh  - Pointer to a RAT channel.
 *  Return: status - Status code based on the response of RF core.
 *
 */
static RF_Stat RF_ratArmChannel(RF_RatChannel* ratCh)
{
    /* Local variable */
    RF_Stat status = RF_StatError;

    /* Only those channels can be programmed which are in pending state. */
    if (ratCh && (ratCh->status == RF_RatStatusPending))
    {
        /* Calculate the channel interrupt flag on the handle. */
        uint32_t ratChIntFlag = (1 << (RFC_DBELL_RFHWIFG_RATCH5_BITN + ratCh->handle));

        /* Clear and enable the interrupt source for that particular channel. */
        RFCHwIntClear(ratChIntFlag);
        RFCHwIntEnable(ratChIntFlag);

        /* Set the power constraint on this channel to keep the RF core ON. */
        RF_powerConstraintSet((RF_PowerConstraintSrc)(1 << ratCh->handle));

        /* Send the command to the RF core. */
        status = RF_executeDirectImmediateCmd((uint32_t)&ratCh->chCmd, NULL);

        /* If the channel configuration is succesfull. */
        if (status == RF_StatCmdDoneSuccess)
        {
             /* Send the IO command to the RF core if there is any configured. */
            if (ratCh->ioCmd)
            {
                status = RF_executeDirectImmediateCmd((uint32_t)ratCh->ioCmd, NULL);
            }

            /* If both the channel and io configuration is succesfull. */
            if (status == RF_StatCmdDoneSuccess)
            {
                ratCh->status = RF_RatStatusRunning;
            }
        }
    }

    /* Return with the status code. */
    return(status);
}

/*
 *  Restarts any RAT channels which are in pending state at the moment of
 *  invoking this method. This is used to automatically restore the rat module
 *  right after the RF core is powered up. This is essential for power management.
 *
 *  Input:  none
 *  Return: none
 *
 */
static void RF_ratRestartChannels(void)
{
    /* Iterate through the RAT containers and restore the channels
       which were in running state before we entered Standby mode. */
    uint32_t i;
    for(i = 0; i < RF_RAT_CH_CNT; i++)
    {
        /* Convert the index to a pointer. */
        RF_RatChannel* ratCh = RF_ratGetChannel(i);

        /* If the channel is in pending state, program it. */
        if (ratCh && (ratCh->status == RF_RatStatusPending))
        {
            /* Try to program the RAT channel. */
            RF_Stat status = RF_ratArmChannel(ratCh);

            /* Execute error handling if programming fails, i.e. due to past timestamp.
               This is done in SWI context. */
            if (status != RF_StatCmdDoneSuccess)
            {
                /* Mark the event as an error by setting also a shadow bit. */
                RF_ratModule.pendingInt |= ((RF_RAT_INTERRUPT_BASE_INDEX | RF_RAT_ERROR_BASE_INDEX) << ratCh->handle);

                /* Post the SWI handler to serve the callback. */
                SwiP_or(&RF_swiHwObj, 0);
            }
        }
    }
}

/*
 *  Suspends any RAT channel which are in RUNNING state.
 *  This is used to force all RAT channels into pending state allowing the power
 *  management to power off the RF core power domain and resynchronize the RAT channels
 *  on next power up.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_ratSuspendChannels(void)
{
    /* Iterate through the RAT containers and suspend the active channels. */
    uint32_t i;
    for(i = 0; i < RF_RAT_CH_CNT; i++)
    {
        /* Set a pointer to the channel. */
        RF_RatChannel* ratCh = RF_ratGetChannel(i);

        /* Only actively running channles can be suspended. */
        if (ratCh && ratCh->status)
        {
            /* Set the status to be suspended. */
            ratCh->status = RF_RatStatusPending;

            /* Clear the power constraint of this channel */
            RF_powerConstraintRelease((RF_PowerConstraintSrc)(1 << ratCh->handle));
        }
    }
}

/*
 *  Read the counter value from the RAT timer.
 *
 *  Input:  none
 *  Return: time - The value found in the RATCNT running register.
 */
static uint32_t RF_ratGetValue(void)
{
    return(HWREG(RFC_RAT_BASE + RFC_RAT_O_RATCNT));
}

/*
 *  Read the channel value from the RAT timer.
 *
 *  Input:  ratHandle - The handle to the channel.
 *  Return: timeout   - The value found in the RATCHxVAL register.
 */
static uint32_t RF_ratGetChannelValue(RF_RatHandle ratHandle)
{
    /* Read the channel value. */
    return(HWREG(RFC_RAT_BASE + RFC_RAT_O_RATCH5VAL + ratHandle * sizeof(uint32_t)));
}

/*
 *  Generate a command which can be used to configure a RAT channel into COMPARE mode.
 *
 *  Input:  ratCh         - Pointer to the channel.
 *          ratConfig     - Configuration structure holding the channel setup.
 *  Return: none
 */
static void RF_ratGenerateChCmd(RF_RatChannel* ratCh, void* ratConfig)
{
    /* Generate a command based on the mode. */
    if (ratCh->mode == RF_RatModeCompare)
    {
        /* Local pointer to cast the configuration to the proper type. */
        RF_RatConfigCompare* ratCompareConfig = (RF_RatConfigCompare*) ratConfig;

        /* Point a pointer to the generic command field within the channels context. */
        rfc_CMD_SET_RAT_CMP_t* pCmd = (rfc_CMD_SET_RAT_CMP_t*)&ratCh->chCmd;

        /* Populate the command structure properly. */
        pCmd->commandNo   = CMD_SET_RAT_CMP;                            /* Instruct the RF core to use COMPARE mode. */
        pCmd->ratCh       = RF_RAT_CH_LOWEST + ratCh->handle;           /* Encode the selected channel number. */
        pCmd->compareTime = ratCompareConfig->timeout;                  /* Select the compare timeout. */
    }
    else if (ratCh->mode == RF_RatModeCapture)
    {
        /* Local pointer to cast the configuration to the proper type. */
        RF_RatConfigCapture* ratCaptureConfig = (RF_RatConfigCapture*) ratConfig;

        /* Point a pointer to the generic command field within the channels context. */
        rfc_CMD_SET_RAT_CPT_t* pCmd = (rfc_CMD_SET_RAT_CPT_t*)&ratCh->chCmd;

        /* Calculate the direct command to be sent to the RF core.*/
        pCmd->commandNo        = CMD_SET_RAT_CPT;                       /* Instruct the RF core to use CAPTURE mode. */
        pCmd->config.ratCh     = RF_RAT_CH_LOWEST + ratCh->handle;      /* Encode the selected channel number. */
        pCmd->config.inputSrc  = ratCaptureConfig->source;              /* Select the source to be captured. */
        pCmd->config.inputMode = ratCaptureConfig->captureMode;         /* Select the mode of capture: raising, falling, etc*/
        pCmd->config.bRepeated = ratCaptureConfig->repeat;              /* Select if we should re-arm the channel after a capture event. */
    }
}

/*
 *  Generate a command which can be used to configure an IO for a particular RAT channel.
 *
 *  Input:  ratCh         - Pointer to the channel.
 *          ioConfig      - Configuration channel for the IO.
 *  Return: cmdToDoorbell - Return with the command structure. It is casted to uint32_t as it is
 *                          stored in a generic variable.
 */
static void RF_ratGenerateIoCmd(RF_RatChannel* ratCh, RF_RatConfigOutput* ioConfig)
{
    /* Local variable. */
    uint32_t cmdToDoorbell = 0;

    /* If there is an IO configuration. */
    if (ioConfig)
    {
        cmdToDoorbell |= ioConfig->select << 2;
        cmdToDoorbell |= ioConfig->mode   << 5;
        cmdToDoorbell |= (uint32_t)(RF_RAT_CH_LOWEST + ratCh->handle) << 8;

        cmdToDoorbell = (uint32_t)CMDR_DIR_CMD_2BYTE(CMD_SET_RAT_OUTPUT, cmdToDoorbell);
    }

    /* Return with the raw command to be sent to the doorbell. */
    ratCh->ioCmd = cmdToDoorbell;
}

/*
 *  Wrapper function to setup a RAT channel into the selected mode.
 *
 *  Input:  ratClient   - Handle previously returned by RF_open().
 *          ratMode     - Identifies the mode the channel is being set up: RF_RatModeCompare or RF_RatModeCapture.
 *          ratCallback - Callback function to be registered to the RAT channel.
 *          ratChannel  - Preferred channel to be allocated. If RF_RatChannelAny is provided, allocatethe first available channel.
 *          ratConfig   - Configuration structure holding the setup of the particulare channel.
 *          ioConfig    - Configuration strucutre of the assosiated GPO setup.
 *  Return: ratHandle   - RF_RatHandle to the allocated channel. If allocation fails, RF_ALLOC_ERROR is returned.
 */
static RF_RatHandle RF_ratSetupChannel(RF_Handle ratClient, RF_RatMode ratMode, RF_RatCallback ratCallback, RF_RatHandle ratChannel, void* ratConfig, RF_RatConfigOutput* ioConfig)
{
    /* Return with an error. Either we couldn't allocate any RAT
       channel, or the RAT module declined our configuration. */
    RF_RatHandle ratHandle = (RF_RatHandle)RF_ALLOC_ERROR;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Find and allocate a RAT channel (if any is available) */
    RF_RatChannel* ratCh = RF_ratAllocChannel(ratChannel);

    /* If we could allocate a RAT channel */
    if (ratCh)
    {
        /* Populate the container. Use the default "do nothing" callback
           if no user callback is provided and generate the command based
           on the mode of the channel. */
        ratCh->pClient = ratClient;
        ratCh->mode    = ratMode;
        ratCh->pCb     = (RF_RatCallback)RF_defaultCallback;
        RF_ratGenerateChCmd(ratCh, ratConfig);
        RF_ratGenerateIoCmd(ratCh, ioConfig);

        /* If there is a user callback provided, override the default callback. */
        if (ratCallback)
        {
            ratCh->pCb = ratCallback;
        }

        /* Decide which PHY should be used upon first start up. */
        if (RF_currClient == NULL)
        {
            RF_currClient = ratCh->pClient;
        }

        /* Calculate the RAT/RTC timestamp to be used to wake the RF core. */
        RF_dispatchNextEvent();

        /* Return with the handle upon success. */
        ratHandle = (RF_RatHandle)ratCh->handle;
    }

    /* Exit critical section. */
    HwiP_restore(key);

    /* Return with either an error OR a handle to a RAT channel. */
    return(ratHandle);
}

/*
 *  Poll the RFACKIFG and clear the flag afterwards. This is used during the power up sequence
 *  of the RF core where interlaying processing is implemented.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_dbellSyncOnAck(void)
{
    while (!HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG));
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
}

/*
 *  Submit a command to the doorbell without blocking command execution. This is used during the
 *  power up sequence where the system CPU can continue with processing data while the RF core
 *  executes the submitted command.
 *
 *  Input:  rawCmd - The raw command to be written to the doorbell. This can be a pointer or a
 *                   a direct/immediate command.
 *  Return: none
 */
static void RF_dbellSubmitCmdAsync(uint32_t rawCmd)
{
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFACKIFG) = 0;
    HWREG(RFC_DBELL_BASE + RFC_DBELL_O_CMDR)     = rawCmd;
}

/*
 *  Wake up notification callback from the power driver. If the callback is from RF wakeup
 *  set constraint to let RF Driver control the XOSC switching else do nothing in the
 *  callback.
 *
 *  Input:  eventType - The type of event when the notification is invoked
 *          eventArg  - Not used.
 *          clientArg - Not used.
 *  Return: Power_NOTIFYDONE
 */
static uint8_t RF_wakeupNotification(uint8_t eventType, uint32_t *eventArg, uint32_t *clientArg)
{
    /* Check if the callback is for wakeup from standby and if power up clock is running */
    if ((eventType == PowerCC26XX_AWAKE_STANDBY) && (ClockP_isActive(&RF_clkPowerUpObj)))
    {
        /* Calculate time (in us) until next trigger (assume next trigger is max ~70 min away) */
        uint32_t timeInUsUntilNextTrig = ClockP_getSystemTickPeriod() * ClockP_getTimeout(&RF_clkPowerUpObj);

        /* Check if the next trig time is close enough to the actual power up */
        if (timeInUsUntilNextTrig < RF_WAKEUP_DETECTION_WINDOW_IN_US)
        {
            /* Stop power up clock */
            ClockP_stop(&RF_clkPowerUpObj);

            /* Setup RF Driver to do the XOSC_HF switching */
            Power_setConstraint(PowerCC26XX_SWITCH_XOSC_HF_MANUALLY);

            /* Set variable to indicate RF Driver will do the XOSC_HF switching */
            RF_core.manualXoscHfSelect = true;

            /* Start the RF Core power up */
            SwiP_or(&RF_swiFsmObj, RF_FsmEventWakeup);
        }
    }

    return(Power_NOTIFYDONE);
}

/*-------------- Scheduler internal functions --------------------------------*/

/*
 *  Issue RF_EventRadioFree callback to the client. The callback is issued -
 *  1. After pre-emption is complete
 *  2. Dedicated request access period expires or released
 *  3. command reject because of other high priority command running
 *
 *  Input:  src - Flag indicating the source of callback request.
 *  Return: none
 */
static void RF_issueRadioFreeCb(uint8_t src)
{
    /* Enter critical section*/
    uint32_t key = HwiP_disable();

    /* Clear the reason why the callback is being invoked */
    RF_Sch.issueRadioFreeCbFlags &= ~src;

    /* Local variable */
    bool isReqAccessActive = false;

    /* If any of the clients has active request access, indicate it */
    if (RF_Sch.clientHnd[0])
    {
        isReqAccessActive |= ClockP_isActive(&RF_Sch.clientHnd[0]->state.clkReqAccess);
    }
    if (RF_Sch.clientHnd[1])
    {
        isReqAccessActive |= ClockP_isActive(&RF_Sch.clientHnd[1]->state.clkReqAccess);
    }

    /* If we cleared all the potential sources and there is no request access*/
    if ((RF_Sch.issueRadioFreeCbFlags == 0) && !isReqAccessActive)
    {
        /* If a valid client handle is provided through the global pointer */
        if (RF_Sch.clientHndRadioFreeCb && (RF_Sch.clientHndRadioFreeCb->clientConfig.nClientEventMask & RF_ClientEventRadioFree))
        {
            /* Get a pointer to the client event callback */
            RF_ClientCallback pClientEventCb = (RF_ClientCallback)RF_Sch.clientHndRadioFreeCb->clientConfig.pClientEventCb;

            /* Exit critical section */
            HwiP_restore(key);

            /* Invoek the client event callback */
            pClientEventCb(RF_Sch.clientHndRadioFreeCb, RF_ClientEventRadioFree, NULL);

            /* Clear the client pointer in any case */
            RF_Sch.clientHndRadioFreeCb = NULL;
        }
        else
        {
            /* Exit critical section */
            HwiP_restore(key);
        }
    }
    else
    {
        /* Exit critical section */
        HwiP_restore(key);
    }
}

/*
 *  Decode how much time it will take to switch protocol/phy configuration.
 *
 *  Input:  prevCmd       - The command switching from.
 *          nextCmd       - The command switching to.
 *  Return: switchingTime - The time it takes to switch the PHY.
 */
static int32_t RF_getSwitchingTimeInUs(RF_Cmd* prevCmd, RF_Cmd* nextCmd)
{
    int32_t switchingTime = 0;

    /*  If otherCmd and newCmd are from different client then there is a switching time
        related to moving between the two commands. */
    if (prevCmd->pClient != nextCmd->pClient)
    {
        switchingTime = nextCmd->pClient->clientConfig.nPhySwitchingDuration;
    }

    /* Return the switching time related to moving between the two clients. */
    return(switchingTime);
}

/*
 *  Check if new request can inserted between the previous and next command in the
 *  current queue.
 *
 *  Input:  newCmd  - RF_Cmd pointer for the new command request
 *          prevCmd - RF_Cmd pointer for the previous cmd in the queue
 *          nextCmd - RF_Cmd pointer for the next cmd in the queue
 *  Return: true    - If command can be inserted in the queue else
 *          false   - Otherwise.
 */
static bool RF_verifyGap(RF_Cmd* newCmd, RF_Cmd* prevCmd, RF_Cmd* nextCmd)
{
    /* Initialize local variables. */
    bool    insertNewCmdAfterPrev  = prevCmd ? false : true;
    bool    insertNewCmdBeforeNext = nextCmd ? false : true;
    int32_t deltaInUs              = 0;

    /*  Step 1 - The newCmd must have an endTime in order to be placed anywhere
                 else than the end. Or if there are no commands behind. */
    if ((newCmd) && (insertNewCmdBeforeNext || (newCmd->endType != RF_EndNotSpecified)))
    {
        /* If there is a prevCmd and it have an endTime, we could potentially
           put the new command behind it. */
        if ((prevCmd) && (prevCmd->endType != RF_EndNotSpecified))
        {
            /* Take the start time of the command located later in the timeline. */
            deltaInUs = (int32_t)RF_convertRatTicksToUs(newCmd->startTime);

            /* Substract the time earlier in the timeline. The result is the gap in between. */
            deltaInUs -= (int32_t)RF_convertRatTicksToUs(prevCmd->endTime);

            /* Substract the switching time needed to move between prevCmd and newCmd. */
            deltaInUs -= RF_getSwitchingTimeInUs(prevCmd, newCmd);

            /* Handle timer overflow with the assumption that the difference between the startTime
               and endTime is less than ~8 min. */
            if ((deltaInUs < ((int32_t)RF_DISPATCH_MAX_TIME_WRAPAROUND_US)) || (deltaInUs > 0))
            {
                /* Allow insertion if startTime has wrapped around or no wrap around and we can insert the command */
                insertNewCmdAfterPrev = true;
            }
        }

        /* If there is a nextCmd, and it has an aboslute startTime, we could potentially put the new command in front of it.
           If we already have evaluated that we can't be behind the prevCmd, there is no need to evalue this. */
        if ((insertNewCmdAfterPrev) && (nextCmd) && (nextCmd->pOp->startTrigger.triggerType == TRIG_ABSTIME))
        {
            /* Take the start time of the command located later in the timeline. */
            deltaInUs = (int32_t)RF_convertRatTicksToUs(nextCmd->startTime);

            /* Substract the time earlier in the timeline. The result is the gap in between. */
            deltaInUs -= (int32_t)RF_convertRatTicksToUs(newCmd->endTime);

            /* Substract the switching time needed to move between newCmd and nextCmd. */
            deltaInUs -= RF_getSwitchingTimeInUs(newCmd, nextCmd);

            /* Handle timer overflow with the assumption that the difference between the startTime
               and endTime is less than ~8 min. */
            if ((deltaInUs < ((int32_t)RF_DISPATCH_MAX_TIME_WRAPAROUND_US)) || (deltaInUs > 0))
            {
                /* Allow insertion if startTime has wrapped around or no wrap around and we can insert the command. */
                insertNewCmdBeforeNext = true;
            }
        }
    }

    /* Return with true if the command can be inserted into the queue (both before or after criteria met). */
    return(insertNewCmdBeforeNext & insertNewCmdAfterPrev);
}

/*
 *  Check what scheduling strategy that can be used to schedule the requesting command.
 *
 *  Input: newCmd      - Points to the newly submitted radio command,
 *         pCmdBg      - Points to the active background command (if any).
 *         pCmdFg      - Points to the active foreground command (if any).
 *         pPendQueue  - Points to the queue holding the commands to be executed.
 *         pDoneQueue  - Points to the queue holding the commands which has been exeuted.
 *         pInsertLocation - Reference to command which the newCmd shall be inserted behind.
 *
 *  Return: RF_ScheduleStatus - Returning status containing the scheduling decision.
 */
static RF_ScheduleStatus RF_howToSchedule(RF_Cmd* newCmd, RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue, RF_Cmd** pInsertLocation)
{
    /* By default, reject any new request. */
    volatile RF_ScheduleStatus status = RF_ScheduleStatusError;

    /* Typecast the arguments to RF commands. */
    RF_Cmd* pHead = (RF_Cmd*)List_head(pPendQueue);

    /* Load list head as the start point of the iterator. */
    RF_Cmd* it = pHead;

    /* Step 1 - Check if new command can be inserted based on the timing information
                at the top of the pending queue. */
    if (RF_verifyGap(newCmd, pCmdBg, pHead))
    {
        /* Indicate that the command was put on the top of the queue.ss */
        status = RF_ScheduleStatusTop;
    }

    /* Step 2 - Check if new command can be inserted based on the timing information
                in the middle/end of the pending queue. This require the new command
                to have an ABSOLUTE startTrigger type. */
    if ((status == RF_ScheduleStatusError) && (newCmd->pOp->startTrigger.triggerType == TRIG_ABSTIME))
    {
        /* Walk the queue.*/
        while (it)
        {
            /* Check if we can insert the new command in between. */
            if (RF_verifyGap(newCmd, it, (RF_Cmd*)List_next((List_Elem*)it)))
            {
                /* Set the return value that the new command should be
                   inserted in between it and it->pNext. */
                status = RF_ScheduleStatusMiddle;
                break;
            }
            else
            {
                it = (RF_Cmd*)List_next((List_Elem*)it);
            }
        }
    }

    /*  Step 3 - If step 1) or 2) fails, reject or append the command to the end of the queue
                 based on the allowDelay argument of RF_scheduleCmd() API call. */
    if ((status == RF_ScheduleStatusError) && (newCmd->allowDelay))
    {
        status = RF_ScheduleStatusTail;
    }

    /* Set pInsertLocation to mark where to insert the new command. */
    *pInsertLocation = it;

    /* Return with the scheduling method. */
    return(status);
}

/**
 *  Sorts and adds command to the RF driver internal command queue.
 *
 *  Input:  pCmdNew              - Pointer to the command to be submitted.
 *          pCmdBg               - Running background command.
 *          pCmdFg               - Running foreground command.
 *          pPendQueue           - Pointer to the head structure of pend queue.
 *          pDoneQueue           - Pointer to the head structure of done queue.
 *  Return: RF_ScheduleStatus    - Identifies the success or failure of enquing.
 */
RF_ScheduleStatus RF_defaultSubmitPolicy(RF_Cmd* pCmdNew, RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue)
{
    /* Local pointer to a command which will be used if insertition
       is selected as a method. */
    RF_Cmd* pInsertLocation = NULL;

    /* Check the method how the new command should be scheduled. */
    RF_ScheduleStatus status = RF_howToSchedule(pCmdNew, pCmdBg, pCmdFg, pPendQueue, pDoneQueue, &pInsertLocation);

    /* Step 1 - If the new command is placed to the top of the pend queue. */
    if (status == RF_ScheduleStatusTop)
    {
        /* Insert command at the beginning of the queue */
        List_putHead(pPendQueue, (List_Elem*)pCmdNew);
    }

    /* Step 2 - If the new command is inserted behind a particular command. */
    if (status == RF_ScheduleStatusMiddle)
    {
        /* Insert command between pInsertLocation and pInsertLocation->pNext. */
        if (List_next((List_Elem*)pInsertLocation))
        {
            /* Insert command before pInsertLocation->next. */
            List_insert(pPendQueue, (List_Elem*)pCmdNew, List_next((List_Elem*)pInsertLocation));
        }
        else
        {
            /* Append command to the end of the queue (if next does not exist). */
            List_put(pPendQueue, (List_Elem*)pCmdNew);
        }
    }

    /* Step 3 - Append command to the end of the queue. */
    if (status == RF_ScheduleStatusTail)
    {
        List_put(pPendQueue, (List_Elem*)pCmdNew);
    }

    /* Return command with the method we used to schedule the command.
       Might be RF_ScheduleStatusError if none of the above rules applied. */
    return(status);
}

/**
 *  Sorts and adds command to the RF driver internal command queue.
 *
 *  Input:  pCmdBg               - Running background command.
 *          pCmdFg               - Running foreground command.
 *          pPendQueue           - Pointer to the head structure of pend queue.
 *          pDoneQueue           - Pointer to the head structure of done queue.
 *          bConflict            - Whether the incoming command conflicts with ongoing activity.
 *          conflictCmd          - Pointer to the command that conflicts with ongoing.
 *  Return: RF_ScheduleStatus    - Identifies the success or failure of enquing.
 */
RF_ExecuteAction RF_defaultExecutionPolicy(RF_Cmd* pCmdBg, RF_Cmd* pCmdFg, List_List* pPendQueue, List_List* pDoneQueue, bool bConflict, RF_Cmd* conflictCmd)
{
    return(RF_ExecuteActionNone);
}

/*
 *  Execute RF power down sequence.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_corePowerDown(void)
{
    /* Local variables to calculate active time in current window. */
    uint32_t deltaTimeInUs = 0;

    /* Disable all CPE and HW interrupts as we are about to power down the core.
       Clearing is not important as content will be lost anyway. */
    RFCCpeIntDisable(~0);
    RFCHwIntDisable(~0);

    /* Remap HWI to the startup function (preparing for next wake up) */
    HwiP_setFunc(&RF_hwiCpe0Obj, RF_hwiCpe0PowerFsm, (uintptr_t)NULL);

    /* Take wake up timestamp and the current timestamp */
    uint32_t rtcTimestampB = (uint32_t) AONRTCCurrent64BitValueGet();

    /* Find the radio core active delta time since the last power up. */
    deltaTimeInUs   = UDIFF(RF_rtcTimestampA, rtcTimestampB);
    deltaTimeInUs >>= RF_RTC_CONV_TO_US_SHIFT;

    /* Accumulate the delta time with the previous active time windows. Avoid overflow. */
    RF_core.activeTimeUs = ADD(RF_core.activeTimeUs, deltaTimeInUs);

    /* Decide whether to send the CMD_SYNC_STOP_RAT command. If this is first power down (.init) or active time (activeTimeInUs)
       is longer than the time that can cause maximum allowed error between RAT and RTC clocks. Yielding will automatically fulfill
       the latter. */
    if (!(RF_core.init)    ||
         (RF_core.activeTimeUs  > (RF_errTolValInUs << RF_DEFAULT_COMB_XTAL_DRIFT_BITS_SHIFT)))
    {
        /* Stop and synchronize the RAT if it is running */
        if (RF_ratIsRunning())
        {
            /* Setup RAT_SYNC command to follow powerdown. */
            RF_ratSyncCmd.stop.commandNo                 = CMD_SYNC_STOP_RAT;
            RF_ratSyncCmd.stop.status                    = IDLE;
            RF_ratSyncCmd.stop.condition.rule            = COND_NEVER;
            RF_ratSyncCmd.stop.startTrigger.triggerType  = TRIG_NOW;
            RF_ratSyncCmd.stop.pNextOp                   = NULL;

            /* Send RAT Stop command and synchronously wait until it run. */
            RF_dbellSubmitCmdAsync((uint32_t)&RF_ratSyncCmd.stop);
            while (!(RF_ratSyncCmd.stop.status & RF_CMD_TERMINATED));
        }

        /* The RF core is now initialized and RAT is synchronized. */
        RF_core.init         = true;
        RF_core.activeTimeUs = 0;
    }

    /* Turn off Synth */
    RFCSynthPowerDown();

    /* Turn off the RF core by gating its clock. This is a quick way to have it shut off. */
    RFCClockDisable();
}

/*-------------- Power constraints internal functions ------------------------*/

/*
 * Set RF power constraints.
 *
 * Input:  src - RF_PowerConstraintSrc (Source: Queue or RAT)
 * Return: none
 */
void RF_powerConstraintSet(RF_PowerConstraintSrc src)
{
    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Set constraint based on source */
    RF_powerConstraint |= src;

    /* Exit critical section */
    HwiP_restore(key);
}

/*
 * Release RF power constraints.
 *
 * Input:  src - RF_PowerConstraintSrc (Source: Queue or RAT)
 * Return: none
 */
void RF_powerConstraintRelease(RF_PowerConstraintSrc src)
{
    /* Enter critical section. */
    uint32_t key = HwiP_disable();

    /* Release this constraint. */
    RF_powerConstraint &= ~src;

    /* Check if all constraints are clear. */
    if (!(RF_powerConstraint & RF_PowerConstraintCmdQ))
    {
        /* Initiate power down if the above criterion is met.
           The RAT timer though might will prevent us to proceed. */
        SwiP_or(&RF_swiFsmObj, RF_FsmEventPowerDown);
    }

    /* Exit critical section. */
    HwiP_restore(key);
}

/*
 * Get RF power constraints.
 *
 * Input:  src - Mask of constraints we requesting
 * Return: Bitwise-OR of the power constraints set and the input argument
 */
RF_PowerConstraintSrc RF_powerConstraintGet(RF_PowerConstraintSrc src)
{
    /* Set constraint based on source */
    return((RF_PowerConstraintSrc)(RF_powerConstraint & (uint8_t)src));
}

/*
 *  It calculates and returns the closest RF event in time if any.
 *
 *  Calling context: Hwi, Swi
 *
 *  Input:   dispatchTime - pointer to a container where the calculated time can be returned
 *  Return:  ticks - If command is far away in future.
 *           0     - If command is too close and should be scheduled now.
 */
static bool RF_calculateDispatchTime(uint32_t* dispatchTimeClockTicks)
{
    /* Local variables. */
    uint32_t deltaTimeCmdClockTicks;
    uint32_t deltaTimeRatClockTicks;

    /* Initialize return value. */
    *dispatchTimeClockTicks = 0;

    /* Calculate the timestamp of the next command in the command queue. */
    bool validCmdTime = RF_cmdDispatchTime(&deltaTimeCmdClockTicks, false, NULL);

    /* If any of the RAT timers expire before the command should be dispatched,
       reprogram the power up clock to the RAT event instead. */
    bool validRatTime = RF_ratDispatchTime(&deltaTimeRatClockTicks);

    if (validCmdTime && validRatTime)
    {
        /* Determine if command execution or RAT event comes first. */
        *dispatchTimeClockTicks = MIN(deltaTimeCmdClockTicks, deltaTimeRatClockTicks);
    }
    else if (validCmdTime)
    {
        /* Command queue determines the next event. */
        *dispatchTimeClockTicks = deltaTimeCmdClockTicks;
    }
    else if (validRatTime)
    {
        /* RAT timer determines the next event. */
        *dispatchTimeClockTicks = deltaTimeRatClockTicks;
    }

    /* If any of them valid, return with true indicating a valid dispatch time. */
    return(validCmdTime || validRatTime);
}

/*
 *  Dispatch the closest event generated either by a command or the RAT timer.
 *  If the RF core is powered, it triggs the HWI to execute the dispatcher.
 *  If the RF core is not powered, it decides if it should be powered ON immediately, or
 *  the execution can be deferred to a later timestamp. In the latter case, the RTC is used to keep
 *  track of proper timing.
 *
 *  Input:  none
 *  Return: status - Status of the command execution.
 *
 */
static void RF_dispatchNextEvent(void)
{
    if (RF_core.status == RF_CoreStatusActive)
    {
        /* Kick the HWI to dispatch the next pending event. */
        HwiP_post(INT_RFC_CPE_0);
    }
    else if ((RF_core.status == RF_CoreStatusPoweringUp) ||
             (RF_core.status == RF_CoreStatusPhySwitching))
    {
        /* Do nothing. We will dispatch the next event at the end
           of power-up/phy-switching sequence naturally. */
    }
    else
    {
        /* Enter critical section. */
        uint32_t key = HwiP_disable();

        /* Calculate dispatch time. */
        uint32_t dispatchTimeClockTicks;
        bool validTime = RF_calculateDispatchTime(&dispatchTimeClockTicks);

        if (validTime)
        {
            /* Decide whether the command should be dispatched. */
            if (dispatchTimeClockTicks)
            {
                /* Dispatch command in the future. */
                RF_restartClockTimeout(&RF_clkPowerUpObj, dispatchTimeClockTicks);
            }
            else
            {
                /* Dispatch the event immediately. Clock is not needed anymore. */
                ClockP_stop(&RF_clkPowerUpObj);

                /* Initiate powering up the RF core. */
                SwiP_or(&RF_swiFsmObj, RF_FsmEventWakeup);
            }
        }
        else
        {
            /* There is no event to be dispatched. */
            ClockP_stop(&RF_clkPowerUpObj);
        }

        /* Exit critical section. */
        HwiP_restore(key);
    }
}

/*
 *  Update the cached FS command within the client's context.
 *
 *  Calling context: Hwi, Swi
 *
 *  Input:  pCmd - Pointer to radio operation command.
 *  Return: none
 */
static void RF_cacheFsCmd(RF_Cmd* pCmd)
{
    /* Upper limit of the number of operations in a chain */
    uint8_t nCmdChainMax = RF_MAX_CHAIN_CMD_LEN;

    /* Traverse the chain */
    RF_Op* pOp = pCmd->pOp;
    while (pOp && nCmdChainMax)
    {
        /* If the operation is a CMD_FS or CMD_FS_OFF */
        if ((pOp->commandNo == CMD_FS) || (pOp->commandNo == CMD_FS_OFF))
        {
            /* Create a copy of the first CMD_FS command (or CMD_FS_OFF) for later power up */
            memcpy(&pCmd->pClient->state.mode_state.cmdFs, pOp, sizeof(pCmd->pClient->state.mode_state.cmdFs));
            break;
        }

        /* Step the chain */
        pOp = pOp->pNextOp;

        /* Avoid infinit loop (in case of closed loops) */
        --nCmdChainMax;
    }
}

/*
 *  Find the last radio operation within a chain.
 *
 *  Calling context: Task, Hwi, Swi
 *
 *  Input:  pOp    - Pointer to the first radio operation.
 *  Return: RF_Op* - Pointer to the last radio operation.
 */
static RF_Op* RF_findEndOfChain(RF_Op* pOp)
{
    /* Upper limit of the number of operations in a chain. */
    uint8_t nCmdChainMax = RF_MAX_CHAIN_CMD_LEN;

    /* Traverse the chain. */
    while (pOp->pNextOp && nCmdChainMax)
    {
        /* Step the chain. */
        pOp = pOp->pNextOp;

        /* Avoid infinit loop (in case of closed loops). */
        --nCmdChainMax;
    }

    /* Return with the last radio operation. */
    return(pOp);
}

/*
 *  Verify if the given command is a setup command.
 *
 *  Calling context: Hwi, Swi
 *
 *  Input:  pOp   - Pointer to radio operation.
 *  Return: true  - The given command is a setup command.
 *          false - The given command is not a setup command.
 */
static bool RF_isRadioSetup(RF_Op* pOp)
{
    /* Verify the command ID against the known setup commands. */
    switch(pOp->commandNo)
    {
        case (CMD_RADIO_SETUP):
        case (CMD_BLE5_RADIO_SETUP):
        case (CMD_PROP_RADIO_SETUP):
        case (CMD_PROP_RADIO_DIV_SETUP):
              /* The given radio operation is indead a setup command. */
              return(true);
        default:
              /* Do nothing. */
              return(false);
    }
}

/*
 *  Ensure that the setup command is properly initialized.
 *
 *  Input:  handle - Pointer to the client.
 *  Return: None
 */
static void RF_initRadioSetup(RF_Handle handle)
{
    /* Local variables. */
    uint16_t* pTxPower             = NULL;
    uint32_t* pRegOverride         = NULL;
    uint32_t* pRegOverrideTxStd    = NULL;
    uint32_t* pRegOverrideTx20     = NULL;
    bool      tx20FeatureAvailable = false;
    bool      update               = handle->clientConfig.bUpdateSetup;

    /* Decode the setup command. */
    RF_RadioSetup* radioSetup = handle->clientConfig.pRadioSetup;
    radioSetup->common.status = IDLE;

    /* Adjust the setup command if needed. */
    switch (radioSetup->common.commandNo)
    {
        case (CMD_RADIO_SETUP):
        case (CMD_BLE5_RADIO_SETUP):
             /* Configure that the frequency synthetizer should be powered up */
             radioSetup->common.config.bNoFsPowerUp = 0;

             /* If requested, also change the analog configuration during power up */
             if (update)
             {
                 radioSetup->common.config.analogCfgMode = RF_SETUP_ANALOGCFG_UPDATE;
             }
             else
             {
                 radioSetup->common.config.analogCfgMode = RF_SETUP_ANALOGCFG_NOUPDATE;
             }
             break;

        case (CMD_PROP_RADIO_SETUP):
        case (CMD_PROP_RADIO_DIV_SETUP):
             /* Configure that the frequency synthetizer should be powered ON */
             radioSetup->prop.config.bNoFsPowerUp = 0;

             /* If requested, also change the analog configuration during power up */
             if (update)
             {
                 radioSetup->prop.config.analogCfgMode = RF_SETUP_ANALOGCFG_UPDATE;
             }
             else
             {
                 radioSetup->prop.config.analogCfgMode = RF_SETUP_ANALOGCFG_NOUPDATE;
             }
             break;
        default:
             break;
    }

    /* Clear the update request flag as it was handled by now. */
    handle->clientConfig.bUpdateSetup = false;

    /* Decode if High Gain PA is available. */
    tx20FeatureAvailable = RF_decodeOverridePointers(radioSetup, &pTxPower, &pRegOverride, &pRegOverrideTxStd, &pRegOverrideTx20);

    /* Ensure that overrides are in sync with the selected PA. */
    if (tx20FeatureAvailable && (*pTxPower == RF_TX20_ENABLED))
    {
        /* Attach the High Gain overrides. It does nothing if the extra overrides are NULL. */
        RF_attachOverrides(pRegOverride, pRegOverrideTx20);
    }
    else
    {
        /* Detach the High Gain overrides. It does nothing if it is not present. */
        RF_detachOverrides(pRegOverride, pRegOverrideTx20);
    }

    /* Perform Software Temperature Controlled Crystal Oscillator compensation (SW TCXO), if enabled */
    if(pfnUpdateHposcOverride != NULL)
    {
        pfnUpdateHposcOverride(pRegOverride);
    }
}

/*
 *  Submit the pending command to the RF Core.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_dispatchNextCmd(void)
{
    bool doDispatchNow   = false;
    RF_Cmd* pConflictCmd = NULL;
    RF_Cmd* pNextCmd     = (RF_Cmd*)List_head(&RF_cmdQ.pPend);
    bool validTime = false;
    uint32_t dispatchTimeClockTicks = 0;

    /* Decide whether to schedule the next command or not. */
    if (pNextCmd)
    {
       /* Either no commands are active, and we will execute */
       if (NULL == RF_cmdQ.pCurrCmdFg && NULL == RF_cmdQ.pCurrCmdBg)
       {
           doDispatchNow = true;
       }
       /* Or the current client wants to execute in the foreground on top of a background command */
       else if (RF_cmdQ.pCurrCmdBg
                && (RF_cmdQ.pCurrCmdBg->pClient == pNextCmd->pClient)
                && (pNextCmd->flags & RF_CMD_FG_CMD_FLAG)
                && (NULL == RF_cmdQ.pCurrCmdFg))
       {
            /* Try to execute the foreground command. */
            doDispatchNow = true;

            /* Be sure that the background command is started within the RF core.
                This is to avoid race condition. */
            while ((RF_cmdQ.pCurrCmdBg->pOp->status == IDLE) ||
                    (RF_cmdQ.pCurrCmdBg->pOp->status == PENDING));
        }
        /* Or there is conflict. */
        else
        {
            /* By default let current command finish. */
            doDispatchNow = false;
        }
    }
    else
    {
        /* There is nothing to do, serve the last callbacks. */
        SwiP_or(&RF_swiFsmObj, RF_FsmEventLastCommandDone);
    }

    /*
     * Calculate the timestamp of the next command in the command queue.
     * `conflict` parameter (here opposite of doDistpatch) implicitly used to determine margin needed to execute incoming command.
     */
    if (pNextCmd && (validTime = RF_cmdDispatchTime(&dispatchTimeClockTicks, !doDispatchNow, &pConflictCmd)))
    {
        /* If there is a conflict, but there is time left  */
        if (!doDispatchNow && dispatchTimeClockTicks > 0)
        {
            /* The conflict is in the future, and might resolve naturally.
               Revisit the issue again before the execution should start. */
            RF_restartClockTimeout(&RF_clkPowerUpObj, dispatchTimeClockTicks);

            /* Set pNext to NULL to avoid calling the hook until we revisit. */
            pNextCmd = NULL;
        }
    }

    /* Invoke the registered hook to get a final decision if there is still one to be made */
    if(pNextCmd && RFCC26XX_schedulerPolicy.executeHook && validTime && (0 == dispatchTimeClockTicks))
    {
        /* Invoke the conflit hook to determine what action we shall take. */
        RF_ExecuteAction action =
            RFCC26XX_schedulerPolicy.executeHook(RF_cmdQ.pCurrCmdBg,
                                                 RF_cmdQ.pCurrCmdFg,
                                                 &RF_cmdQ.pPend,
                                                 &RF_cmdQ.pDone,
                                                 !doDispatchNow,
                                                 pConflictCmd);

        switch (action)
        {
            case RF_ExecuteActionAbortOngoing:
            {
                RF_abortCmd(RF_cmdQ.pCurrCmdBg->pClient, RF_cmdQ.pCurrCmdBg->ch, false, true, true);
                /* Do not dispatch now, wait for abort to complete, then reschedule */
                doDispatchNow = false;
            }
            break;

            case RF_ExecuteActionRejectIncoming:
            {
                RF_Cmd *abortCmd = pConflictCmd ? pConflictCmd : pNextCmd;
                RF_abortCmd(abortCmd->pClient, abortCmd->ch, false, false, true);
                /* Do not dispatch now, wait for abort to complete, then reschedule */
                doDispatchNow = false;
            }
            break;

            case RF_ExecuteActionNone:
            default:
            {
                /*
                 * Ignore command if conflict, and pick it up after current finishes.
                 * If no conflict, dispatch.
                 */
                ;
            }
            break;
        }
    }

    /* We need to evaluate and handle the next command. Check pointer for static analysis. */
    if (doDispatchNow && pNextCmd)
    {
        if (pNextCmd->pClient != RF_currClient)
        {
            /* We need to change radio client, signal to FSM. */
            SwiP_or(&RF_swiFsmObj, RF_FsmEventInitChangePhy);
        }
        else
        {
            /* Dispatch command in the future */
            if (validTime && dispatchTimeClockTicks && !RF_cmdQ.pCurrCmdBg && !RF_cmdQ.pCurrCmdFg)
            {
                /* Command sufficiently far into future that it shouldn't be dispatched yet
                Release RF power constraint and potentially power down radio */
                RF_powerConstraintRelease(RF_PowerConstraintCmdQ);
            }
            else
            {
                /* Set power constraint on the command queue, since there is now a running command. */
                RF_powerConstraintSet(RF_PowerConstraintCmdQ);

                /* Move the command from the pending queue to the current command. */
                if (pNextCmd->flags & RF_CMD_FG_CMD_FLAG)
                {
                    RF_cmdQ.pCurrCmdFg = (RF_Cmd*)List_get(&RF_cmdQ.pPend);
                }
                else
                {
                    RF_cmdQ.pCurrCmdBg = (RF_Cmd*)List_get(&RF_cmdQ.pPend);
                }

                /* Clear and enable the requested interrupt sources of the command. */
                RFCCpeIntClear((uint32_t) (pNextCmd->bmEvent));
                RFCCpeIntEnable((uint32_t)(pNextCmd->bmEvent));
                RFCHwIntClear((uint32_t)  (pNextCmd->bmEvent >> RF_SHIFT_32_BITS));
                RFCHwIntEnable((uint32_t) (pNextCmd->bmEvent >> RF_SHIFT_32_BITS));

                /* Decode the radio operation itself. */
                RF_Op* pOp = (RF_Op*)pNextCmd->pOp;

                /* Invoke global callback to indicate start of command chain */
                RF_invokeGlobalCallback(RF_GlobalEventCmdStart, (void*)pNextCmd);

                /* Send the radio operation to the RF core. */
                RFCDoorbellSendTo((uint32_t)pOp);

                /* If the command is a new setup command, notify the board file. */
                if (RF_isRadioSetup(pOp))
                {
                    /* Invoke the global callback if the setup command changed. This is needed to
                       adjust the front-end configuration according to the new PHY. */
                    RF_invokeGlobalCallback(RF_GlobalEventRadioSetup, (void*)pOp);
                }

                /* Check the pending queue for any foreground command (IEEE 15.4 mode) */
                SwiP_or(&RF_swiFsmObj, RF_FsmEventRunScheduler);
            }
        }
    }
}

/*
 *  Check if there was an error with the synth while running CMD_FS
 *  error callback is not issued in this function.
 *
 *  Input:  none
 *  Return: true  - If there was an error.
 *          false - If there was no error.
 */
static bool RF_checkCmdFsError(void)
{
    /* Take the handle of the current client */
    RF_Handle pObj = RF_currClient;

    /* Find the FS command stored in the context of the client */
    RF_Op *tmp1 = (RF_Op*)&pObj->clientConfig.pRadioSetup->prop;
    while (tmp1->pNextOp && tmp1->pNextOp != (RF_Op*)&pObj->state.mode_state.cmdFs)
    {
      tmp1 = tmp1->pNextOp;
    }

    /* Evaluate if the FS command succeeded */
    if ((tmp1->condition.rule == COND_ALWAYS) &&
        (pObj->state.mode_state.cmdFs.status == ERROR_SYNTH_PROG))
    {
       /* CMD_FS completed with error so return true */
       return(true);
    }
    else
    {
       /* There is no synth error so return false */
       return(false);
    }
}

/*
 *  RF HW ISR when radio is active.
 *
 *  Input:  a - Not used.
 *  Return: none
 */
static void RF_hwiHw(uintptr_t a)
{
    /* Prepare a direct command */
    RF_Cmd* pCmd = RF_cmdQ.pCurrCmdBg;

    /* Read and clear the interrupt flags */
    uint32_t rfchwifg = RFCHwIntGetAndClear(RF_HW_INT_CPE_MASK | RF_HW_INT_RAT_CH_MASK);
    uint32_t rfchwien = HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) & RF_HW_INT_CPE_MASK;
    uint32_t rathwien = HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFHWIEN) & RF_HW_INT_RAT_CH_MASK;

    if (rfchwifg & rfchwien)
    {
        /* Post SWI_FSM if MODEM_SOFT event occured and the interrupt was enabled */
        if (pCmd)
        {
            /* Store the command which callback need to be served */
            RF_cmdQ.pCurrCmdCb = pCmd;

            /* Decode the event numeber. */
            RF_EventMask events = ((RF_EventMask)(rfchwifg & rfchwien) << RF_SHIFT_32_BITS);

            /* Store the events within the context of the command for the callback. */
            RF_cmdStoreEvents(pCmd, events);

            /* Trig the state machine to handle this event */
            SwiP_or(&RF_swiFsmObj, RF_FsmEventCpeInt);
        }
    }

    /* Post the SWI_HW if any RAT channel event occured */
    if (rfchwifg & rathwien)
    {
        /* Store the channel which cause the interrupt */
        RF_ratModule.pendingInt |= (rfchwifg & rathwien) >> RFC_DBELL_RFHWIFG_RATCH5_BITN;

        /* Post the swi to handle its callback */
        SwiP_or(&RF_swiHwObj, 0);
    }
}

/*
 *  Software interrupt handler which servers Radio Timer (RAT) related events.
 *
 *  Input:  a    - Generic argument. Not used.
 *          b    - Generic argument. Not used.
 *  Return: none
 */
static void RF_swiHw(uintptr_t a, uintptr_t b)
{
    /* Local variable */
    bool error = false;

    /* If the interrupt was trigged due to one of the RAT channels. */
    if (RF_ratModule.pendingInt)
    {
        /* Process lower channel first and allow multiple interrupt flags to be processed sequentially. */
        uint32_t i;
        for(i = 0; i < RF_RAT_CH_CNT; i++)
        {
            if (RF_ratModule.pendingInt & (RF_RAT_INTERRUPT_BASE_INDEX << i))
            {
                /* If there is also a bit indicating that the interrupt is due to an error. */
                if (RF_ratModule.pendingInt & (RF_RAT_ERROR_BASE_INDEX << i))
                {
                    error = true;
                }

                /* Enter critical section. */
                uint32_t key= HwiP_disable();

                /* Atomic read-modify-write instruction of the interrupt flags.
                   Knowing that this is the only place when such a flag can be cleared, it is safe to only guard this
                   operation. Additional flags (which have been raised in the meantime) will be reserved and served in the
                   next iteration. */
                RF_ratModule.pendingInt &= ~((RF_RAT_INTERRUPT_BASE_INDEX | RF_RAT_ERROR_BASE_INDEX) << i);

                /* Exit critical section. */
                HwiP_restore(key);

                /* Convert the channel index to a pointer of rat configuration. */
                RF_RatChannel* ratCh = RF_ratGetChannel(i);

                /* Serve the interrupt if it is from an active channel. This is to avoid decoding function
                   pointers from invalid containers due to fantom interrupts. */
                if (ratCh && ratCh->status)
                {
                    /* Read the channel counter from the RAT timer. In capture mode this is the captured value,
                       in compare mode this is the compare timestamp.*/
                    uint32_t compareCaptureValue = RF_ratGetChannelValue(ratCh->handle);

                    /* Temporarily store the callback handler and the channel offset.
                       This is necessary in order to be able to free and reallocate the
                       same channel within the context of the callback itself. */
                    RF_Handle      ratClient   = (RF_Handle)      ratCh->pClient;
                    RF_RatHandle   ratHandle   = (RF_CmdHandle)   ratCh->handle;
                    RF_RatCallback ratCallback = (RF_RatCallback) ratCh->pCb;

                    /* Only free the channel if it is NOT in repeated capture mode, or an error occured. */
                    if (error || !(ratCh->mode == RF_RatModeCapture) || !(ratCh->chCmd & RF_RAT_CAPTURE_REPEAT_MODE))
                    {
                        /* Free RAT channel. If this is the last channel, it might delay with 1 LF edge to
                           calculate the next wake up event. */
                        RF_ratFreeChannel(ratCh);
                    }

                    /* Serve the user callback with Error or Compare/Capture Event. */
                    if (error)
                    {
                        ratCallback(ratClient, ratHandle, RF_EventError, 0);
                    }
                    else
                    {
                        ratCallback(ratClient, ratHandle, RF_EventRatCh, compareCaptureValue);
                    }
                }

                /* Only serve one channel at a time. */
                break;
            }
        }
    }

    /* Repost the SWI again if multiple interrupt flags are still set. */
    if (RF_ratModule.pendingInt)
    {
        SwiP_or(&RF_swiHwObj, 0);
    }
}

/*
 *  RF CPE0 ISR when radio is active. Assume that all IRQs relevant to command
 *  dispatcher are mapped here. Furthermore, assume that there is no need for
 *  critical sections here (i.e. that this ISR has higher priority than
 *  any HWI calling a RF API function or that HWIs can't call the RF API).
 *
 *  Input:  a - Not used.
 *  Return: none
 */
static void RF_hwiCpe0Active(uintptr_t a)
{
    /* Local variables. */
    RF_Cmd* volatile* ppActiveCmd  = NULL;
    RF_Cmd* volatile* activeCmd[2] = {&RF_cmdQ.pCurrCmdBg, &RF_cmdQ.pCurrCmdFg};
    uint32_t          rfcpeifgMask = 0;
    uint32_t          rfcpeifg     = 0;
    uint32_t          nextEvent    = 0;

    /* Handle PA switching. */
    if (RFCCpeIntGetAndClear(RF_EventPaChanged))
    {
        /* The PA was changed during a chain of radio operation. We need to extract the current configuration
           and propagate it back to the setup command. This is to reserve the change after power cycle. */
        RF_extractPaConfiguration(RF_currClient);

        /* Invoke the board file to reconfigure the external front-end configuration. */
        RF_invokeGlobalCallback(RF_GlobalEventRadioSetup, (void*) RF_currClient->clientConfig.pRadioSetup);
    }

    /* Iterate through the possible active commands. */
    uint32_t i;
    for(i = 0; i < sizeof(activeCmd)/sizeof(uint32_t); i++)
    {
        /* Decode the active command. */
        ppActiveCmd = activeCmd[i];

        /* If there was a command running (handles both foreground and background context). */
        if (*ppActiveCmd)
        {
            /* Decode the events the active command subscribed to. */
            rfcpeifgMask = (*ppActiveCmd)->bmEvent;

            /* Read the interrupt flags which belong to the active command (including the mandatory termination events). */
            rfcpeifg = RFCCpeIntGetAndClear(rfcpeifgMask);

            /* Save the events happened and to be passed to the callback. */
            RF_cmdStoreEvents((*ppActiveCmd), rfcpeifg);

            /* Look for termination events. */
            if (rfcpeifg & (RFC_DBELL_RFCPEIFG_LAST_FG_COMMAND_DONE_M | RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M))
            {
                /* Disable interrupt sources which were subsribed by the command. Since the LAST_CMD_DONE is
                   is shared with the state machine, it cannot be disabled. */
                RFCCpeIntDisable((uint32_t)((*ppActiveCmd)->bmEvent & ~(RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M | RFC_DBELL_RFCPEIEN_IRQ14_M)));
                RFCHwIntDisable((uint32_t) ((*ppActiveCmd)->bmEvent >> RF_SHIFT_32_BITS));

                /* Invoke global callback to indicate end of command chain */
                RF_invokeGlobalCallback(RF_GlobalEventCmdStop, (void*)(*ppActiveCmd));

                /* Move active command to done queue. */
                List_put(&RF_cmdQ.pDone, (List_Elem*)(*ppActiveCmd));

                /* Retire the command, it is not running anymore. */
                (*ppActiveCmd) = NULL;

                /* We will invoke the callback and deallocate the command. */
                nextEvent |= RF_FsmEventLastCommandDone;
            }
            else if (rfcpeifg)
            {
                /* The interrupt is just an ordinary event without termination. */
                RF_cmdQ.pCurrCmdCb = (*ppActiveCmd);

                /* We will just invoke the callback. */
                nextEvent |= RF_FsmEventCpeInt;
            }
        }
    }

    /* Post SWI to handle registered callbacks if there is any. */
    if (nextEvent)
    {
        SwiP_or(&RF_swiFsmObj, nextEvent);
    }

    /* Restart pending rat channels. */
    RF_ratRestartChannels();

    /* Dispatch the next pending command if exists. */
    RF_dispatchNextCmd();
}

/*
 *  Temperature limit notification function.
 */
static void RF_hposcRfCompensateFxn(int16_t currentTemperature,
                                    int16_t thresholdTemperature,
                                    uintptr_t clientArg,
                                    Temperature_NotifyObj *NotifyObj)
{
    int32_t relFreqOffset;
    int16_t relFreqOffsetConverted;
    int_fast16_t status;

    /* Check if HPOSC frequency offset has changed */
    relFreqOffset = OSC_HPOSCRelativeFrequencyOffsetGet(currentTemperature);
    if (relFreqOffset != RF_currentHposcFreqOffset)
    {
        /* Frequency offset has changed. Compensation is required */
        RF_currentHposcFreqOffset = relFreqOffset;
        relFreqOffsetConverted = OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert(relFreqOffset);

        /* Check if radio is powered */
        if ((RF_core.status == RF_CoreStatusActive) || (RF_core.status == RF_CoreStatusPhySwitching))
        {
            /* Radio is powered. Check if any actively running command */
            if (RF_cmdQ.pCurrCmdBg || RF_cmdQ.pCurrCmdFg)
            {
                /* Command is running. Abort command and assure that both RF_EventCmdAborted and RF_EventCmdPreemptrf events are set */
                RF_abortCmd(RF_cmdQ.pCurrCmdBg->pClient, RF_cmdQ.pCurrCmdBg->ch, false, false, true);
            }

            /* Update RFCore with the HPOSC frequency offset */
            RF_runDirectImmediateCmd(RF_currClient, CMDR_DIR_CMD_2BYTE(CMD_UPDATE_HPOSC_FREQ, relFreqOffsetConverted), NULL);
        }
    }

    /* Register the notification again with updated thresholds */
    status = Temperature_registerNotifyRange(NotifyObj,
                                             currentTemperature + RF_TEMP_LIMIT_3_DEGREES_CELSIUS,
                                             currentTemperature - RF_TEMP_LIMIT_3_DEGREES_CELSIUS,
                                             RF_hposcRfCompensateFxn,
                                             (uintptr_t)NULL);

    if (status != Temperature_STATUS_SUCCESS) {
        /* Invoke global callback to indicate unsuccessful registration of temperature notification */
        RF_invokeGlobalCallback(RF_GlobalEventTempNotifyFail, NULL);
    }
}

/*
 *  Clock callback due to inactivity timeout.
 *
 *  Input:  pObj - Not used.
 *  Return: none
 */
static void RF_clkInactivityCallback(uintptr_t a)
{
    /* If there are no pending commands in the queue */
    if (RF_cmdQ.nSeqPost == RF_cmdQ.nSeqDone)
    {
        /* Release the constraint on the command queue and if nothing prevents, power down the radio */
        RF_powerConstraintRelease(RF_PowerConstraintCmdQ);
    }
}

/*
 *  Clock callback due to request access timeout.
 *
 *  Input:  a - Not used.
 *  Return: none
 */
static void RF_clkReqAccess(uintptr_t a)
{
    RF_issueRadioFreeCb(RF_RADIOFREECB_REQACCESS_FLAG   |
                        RF_RADIOFREECB_PREEMPT_FLAG     |
                        RF_RADIOFREECB_CMDREJECT_FLAG);
}

/*
 *  Callback used to post semaphore for runCmd() and pendCmd().
 *
 *  Input:  h    - Handle to the client.
 *          ch   - Handle to the command which callback to be invoked.
 *          e    - Events causing the function call.
 *  Return: none
 */
static void RF_syncCb(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
    /* Local variables */
    RF_Cmd* pCmd;

    /* If there is a user callback provided. */
    if (h->state.pCbSync)
    {
        /* Invoke the user callback with the events fired. */
        ((RF_Callback)h->state.pCbSync)(h, ch, e);
    }

    /* Mask the possible causes of releasing the semaphore */
    RF_EventMask maskedEvents = (e & h->state.eventSync);

    /* Release the semaphore on any of the reasons: last command done,
       subscribed event happened, last FG command is done in IEEE mode */
    if (maskedEvents)
    {
        /* Find the command. We do it here within the SWI context. */
        pCmd = RF_cmdGet(h, ch, RF_CMD_ALLOC_FLAG);

        /* Store the events in the context of the client */
        h->state.unpendCause = maskedEvents;

        /* Find the command. We do it here within the SWI context. */
        if (pCmd)
        {
            /* Clear the handled past events so it is possible to pend again */
            pCmd->pastifg &= ~h->state.unpendCause;

            /* Exhange the callback function: use the user callback from this point */
            pCmd->pCb = (RF_Callback)h->state.pCbSync;
        }

        /* Clear temporary storage of user callback (it was restored and served at this point) */
        h->state.pCbSync = NULL;

        /* Post the semaphore to release the RF_pendCmd() */
        SemaphoreP_post(&h->state.semSync);
    }
}

/*
 *  Invoke the global callback registered through the RFCC26XX_hwAttrs.
 *
 *  Input:  e    - Events causing the function call.
 *  Return: none
 */
static void RF_invokeGlobalCallback(RF_GlobalEvent event, void* arg)
{
    /* Decode the global callback and it's mask from the board file. */
    RF_GlobalCallback  callback  = RFCC26XX_hwAttrs.globalCallback;
    RF_GlobalEventMask eventMask = RFCC26XX_hwAttrs.globalEventMask;

    /* If the board has subscribed to this event, invoke the callback. */
    if (callback && (eventMask & event))
    {
        callback(RF_currClient, event, arg);
    }
}

/*
 *  Default callback function.
 *
 *  Input:  h    - Handle to the client.
 *          ch   - Handle to the command which callback to be invoked.
 *          e    - Events causing the function call.
 *  Return: none
 */
static void RF_defaultCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e)
{
  /* Do nothing */;
}

/*-------------- RF powerup/powerdown FSM functions ---------------*/

/*
 *  The SWI handler for FSM events.
 *
 *  Input:  a0 - Not used.
 *          a1 - Not used.
 *  Return: none
 */
static void RF_swiFsm(uintptr_t a0, uintptr_t a1)
{
    RF_core.fxn(RF_currClient, (RF_FsmEvent)SwiP_getTrigger());
}

/*
 *  Clock callback called upon powerup.
 *
 *  Input:  a - Not used.
 *  Return: none
 */
static void RF_clkPowerUp(uintptr_t a)
{
    if (RF_core.fxn == RF_fsmActiveState)
    {
        /* Dispatch the next RF core event. */
        RF_dispatchNextEvent();
    }
    else
    {
        /* Trigger FSM SWI to start the wake up sequence of the radio.
           This is important when we poll the XOSC_HF. */
        SwiP_or(&RF_swiFsmObj, RF_FsmEventWakeup);
    }
}

/*
 *  RF CPE0 ISR during FSM powerup/powerdown.
 *
 *  Input:  a0 - Not used.
 *  Return: none
 */
static void RF_hwiCpe0PowerFsm(uintptr_t a0)
{
    /* Read all IRQ flags in doorbell and then clear them */
    uint32_t rfcpeifg = RFCCpeIntGetAndClear(RF_CPE0_INT_MASK);

    /* If the radio is active */
    if (RF_core.fxn == RF_fsmActiveState)
    {
        /* Change HWI handler to the correct one */
        HwiP_setFunc(&RF_hwiCpe0Obj, RF_hwiCpe0Active, (uintptr_t)NULL);

        /* Mark radio and client as being active */
        RF_core.status = RF_CoreStatusActive;

        /* No synth error */
        if (!RF_checkCmdFsError())
        {
            /* Restart pending rat channels. */
            RF_ratRestartChannels();

            /* Dispatch the next command */
            RF_dispatchNextCmd();
        }
    }

    /* Handle special events as boot, etc */
    if (rfcpeifg & (RFC_DBELL_RFCPEIFG_BOOT_DONE_M | RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M))
    {
        SwiP_or(&RF_swiFsmObj, RF_FsmEventPowerStep);
    }
}

/*
 *  RF CPE0 ISR during Change PHY switching.
 *
 *  Input:  a0 - Not used.
 *  Return: none
 */
static void RF_hwiCpe0ChangePhy(uintptr_t a0)
{
    /* Clear all IRQ flags in doorbell and then clear them */
    uint32_t rfcpeifg = RFCCpeIntGetAndClear(RF_CPE0_INT_MASK);

    if (rfcpeifg & IRQ_LAST_COMMAND_DONE)
    {
        /* Proceed to the second phase of the phy switching process */
        SwiP_or(&RF_swiFsmObj, RF_FsmEventFinishChangePhy);
    }
}

/*-------------- Power management state functions ---------------*/
/*
 *  Handles RF Core patching for CPE, MCE, RFE (if required) in setup state during power-up.
 *
 *  Input:  mode   - RF_PHY_BOOTUP_MODE:    First boot of the RF core.
 *                 - RF_PHY_SWITCHING_MODE: Switching between two phys.
 *  Return: none
 */
static void RF_applyRfCorePatch(bool mode)
{
    /* Local reference to the patches. */
    void (*cpePatchFxn)(void) = RF_currClient->clientConfig.pRfMode->cpePatchFxn;
    void (*mcePatchFxn)(void) = RF_currClient->clientConfig.pRfMode->mcePatchFxn;
    void (*rfePatchFxn)(void) = RF_currClient->clientConfig.pRfMode->rfePatchFxn;

    if (mode == RF_PHY_SWITCHING_MODE)
    {
        /* If patches are provided, enable RFE and MCE clocks. */
        if ((mcePatchFxn != NULL) || (rfePatchFxn != NULL))
        {
            RF_dbellSubmitCmdAsync((uint32_t)CMDR_DIR_CMD_2BYTE(RF_CMD0, RFC_PWR_PWMCLKEN_MDMRAM | RFC_PWR_PWMCLKEN_RFERAM));
        }

        /* Clear the previous patch. */
        if (cpePatchFxn != NULL)
        {
            RFCCpePatchReset();
        }
    }

    /* Load the patches if relevant for this phy. */
    if (cpePatchFxn != NULL)
    {
        if (mode == RF_PHY_BOOTUP_MODE)
        {
            cpePatchFxn();
        }
    }

    if ((mcePatchFxn != NULL) || (rfePatchFxn != NULL))
    {
        /* Wait for clocks to be turned ON */
        RF_dbellSyncOnAck();

        /* Patch MCE if relevant */
        if (mcePatchFxn != NULL)
        {
            mcePatchFxn();
        }

        /* Patch RFE if relevant */
        if (rfePatchFxn != NULL)
        {
            rfePatchFxn();
        }

        /* Turn off additional clocks */
        RFCDoorbellSendTo(CMDR_DIR_CMD_2BYTE(RF_CMD0, 0));
    }
}

/*
 *  Arms the inactivity timer and hence postpones the decision whether
 *  power management shall take place or not.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_setInactivityTimeout(void)
{
    /* Local variables to be used to find the correct timeout value. */
    uint32_t inactivityTimeUsA = 0;
    uint32_t inactivityTimeUsB = 0;
    RF_Handle handleA = RF_Sch.clientHnd[0];
    RF_Handle handleB = RF_Sch.clientHnd[1];

    /* Issue radio free callback after pre-emption if required */
    uint8_t tmp = RF_RADIOFREECB_PREEMPT_FLAG | RF_RADIOFREECB_CMDREJECT_FLAG;

    /* If the radio was yielded, add the flag */
    if (RF_currClient->state.bYielded)
    {
        tmp |= RF_RADIOFREECB_REQACCESS_FLAG;
    }

    /* Call the radio free callback */
    RF_issueRadioFreeCb(tmp);

    if (handleA)
    {
        if (handleA->state.bYielded == false)
        {
            inactivityTimeUsA = handleA->clientConfig.nInactivityTimeout;
        }
        handleA->state.bYielded = false;
    }

    if (handleB)
    {
        if (handleB->state.bYielded == false)
        {
            inactivityTimeUsB = handleB->clientConfig.nInactivityTimeout;
        }
        handleB->state.bYielded = false;
    }

    /* Set the inactivity time to the max between the two clients */
    uint32_t inactivityTimeUs = MAX(inactivityTimeUsA, inactivityTimeUsB);

    /* If immediate power down is reuqested  */
    if (inactivityTimeUs == SemaphoreP_NO_WAIT)
    {
        /* We can powerdown immediately */
        RF_clkInactivityCallback((uintptr_t)NULL);
    }
    else if (inactivityTimeUs != SemaphoreP_WAIT_FOREVER)
    {
        /* Reprogram and start inactivity timer */
        RF_restartClockTimeout(&RF_clkInactivityObj, inactivityTimeUs/ClockP_getSystemTickPeriod());
    }
}


/*
 *  Handle callback to client for RF_EventLastCmdDone and issue radio free callback if required.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_radioOpDoneCb(void)
{
    /* Serve the first entry in the done queue */
    RF_Cmd* pCmd = (RF_Cmd*)List_head(&RF_cmdQ.pDone);

    /* Radio command done */
    if (pCmd)
    {
        /* Update implicit radio state (chained FS command if any) */
        RF_cacheFsCmd(pCmd);

        /* Read and clear the events */
        RF_EventMask events = pCmd->rfifg;
        pCmd->rfifg  = 0;

        /* Issue callback, free container and dequeue */
        if (pCmd->pCb)
        {
            /* If any of the cancel events are set, mask out the other events. */
            RF_EventMask abortMask = (RF_EventCmdCancelled
                                      | RF_EventCmdAborted
                                      | RF_EventCmdStopped
                                      | RF_EventCmdPreempted);

            /* Mask out the other events if any of the above is set. */
            if (events & abortMask)
            {
                RF_EventMask nonTerminatingEvents = events & ~(abortMask | RF_EventCmdDone | RF_EventLastCmdDone |
                                                               RF_EventLastFGCmdDone | RF_EventFGCmdDone);
                if (nonTerminatingEvents)
                {
                    /* Invoke the user callback with any pending non-terminating events, since bare abort will follow */
                    pCmd->pCb(pCmd->pClient, pCmd->ch, nonTerminatingEvents);
                }

                /* Mask out the other events if any of the above is set. */
                events &= abortMask;
            }

            /* Invoke the user callback */
            pCmd->pCb(pCmd->pClient, pCmd->ch, events);
        }

        /* Enter critical section */
        uint32_t key = HwiP_disable();

        /* Update num of radio command done */
        RF_cmdQ.nSeqDone = (RF_cmdQ.nSeqDone+1) & N_CMD_MODMASK;

        /* Commmand completed reset command flags  */
        pCmd->flags = 0;

        /* Invalidate the command handle. This is to avoid having duplicate
           handles in the pool. */
        pCmd->ch = RF_SCHEDULE_CMD_ERROR;

        /* Command completed, free command queue container */
        List_get(&RF_cmdQ.pDone);

        /* Exit critical section */
        HwiP_restore(key);

        /* Check if there are any more pending commands */
        if (RF_cmdQ.nSeqDone == RF_cmdQ.nSeqPost)
        {
            RF_setInactivityTimeout();
        }
    }
}

/*
 *  Verify if reconiguring or powering down the radio is allowed.
 *
 *  Input:  none
 *  Return: none
 */
static bool RF_isStateTransitionAllowed(void)
{
    /* Local variable. */
    bool status = false;

    /* If we are not performing RF core state changes. */
    if (RF_core.status == RF_CoreStatusActive)
    {
        if(RF_cmdQ.pCurrCmdBg == NULL &&
           RF_cmdQ.pCurrCmdFg == NULL)
        {
            status = true;
        }
    }

    /* Return with the decision. */
    return(status);
}

/*
 *  Measure radio powerup time. If it is lesser than the previous run, update
 *  the powerup duration value so that it can be used during next wake up.
 *
 *  Input:  *powerupDuration    Pointer to the powerup duration that needs updating
 *  Return: none
 */
static void RF_updatePowerupDuration(uint32_t *powerupDuration)
{
    uint32_t rtcValTmp1;
    uint32_t rtcValTmp2;
    uint32_t prevPowerUpDuration;
    /* Temporary storage to be able to compare the new value to the old measurement */
    prevPowerUpDuration = *powerupDuration;

    /* Take wake up timestamp and the current timestamp */
    rtcValTmp1  = (uint32_t) RF_rtcTimestampA;
    rtcValTmp2  = (uint32_t) AONRTCCurrent64BitValueGet();

    /* Calculate the difference of the timestamps and convert it to us units */
    *powerupDuration = UDIFF(rtcValTmp1, rtcValTmp2);
    *powerupDuration >>= RF_RTC_CONV_TO_US_SHIFT;

    /* Low pass filter on power up durations less than in the previous cycle */
    if (prevPowerUpDuration > *powerupDuration)
    {
        /* Expect that the values are small and the calculation can be done in 32 bits */
        *powerupDuration = (prevPowerUpDuration + *powerupDuration)/2;
    }

    /* Power up duration should be within certain upper and lower bounds */
    if (*powerupDuration > RF_DEFAULT_POWER_UP_TIME)
    {
        *powerupDuration = RF_DEFAULT_POWER_UP_TIME;
    }
    else if (*powerupDuration < RF_DEFAULT_MIN_POWER_UP_TIME)
    {
        *powerupDuration = RF_DEFAULT_MIN_POWER_UP_TIME;
    }
}

/*
 *  RF state machine function during power up state.
 *
 *  Input:  pObj - Pointer to RF object.
 *          e    - State machine event.
 *  Return: none
 */
static void RF_fsmPowerUpState(RF_Object *pObj, RF_FsmEvent e)
{
    /* Note: pObj is NULL in this state */
    if (e & RF_FsmEventLastCommandDone)
    {
        /* Invoke the user provided callback function */
        RF_radioOpDoneCb();

        /* Retrig the SWI if there are more commands in the done queue. */
        if (List_head(&RF_cmdQ.pDone))
        {
            /* Trigger self if there are more commands in callback queue */
            SwiP_or(&RF_swiFsmObj, (e | RF_FsmEventLastCommandDone));
        }
        else
        {
            /* We've handled this event now */
            e &= ~RF_FsmEventLastCommandDone;

            /* Schedule the next event based on the state of the command queue
               and the RAT module. */
            RF_dispatchNextEvent();
        }
    }
    else if (e & RF_FsmEventWakeup)
    {
        /* Notify the power driver that FLASH is needed in IDLE */
        bDisableFlashInIdleConstraint = true;
        Power_setConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);

        /* Store the current RTC tick for nPowerUpDuration calculation */
        RF_rtcTimestampA = AONRTCCurrent64BitValueGet();

        /* Set current client from first command in command queue */
        RF_Cmd* pNextCmd = (RF_Cmd*)List_head(&RF_cmdQ.pPend);
        if (pNextCmd)
        {
            RF_Object* pNextClient = pNextCmd->pClient;

            /* If the next command belongs to another client, initiate PHY switching */
            if ((RF_currClient) && (RF_currClient != pNextClient))
            {
                /* Invoke the client switch callback if it was provided */
                if (pNextClient->clientConfig.nClientEventMask & RF_ClientEventSwitchClientEntered)
                {
                    RF_ClientCallback pClientEventCb = (RF_ClientCallback)pNextClient->clientConfig.pClientEventCb;
                    pClientEventCb(pNextClient, RF_ClientEventSwitchClientEntered, NULL);
                }

                /* Due to client switching, update the analogCfg field of setup command. */
                pNextClient->clientConfig.bUpdateSetup = true;
            }

            /* Set the current client to be the next client */
            RF_currClient = pNextClient;
        }

        /* Set the RF mode in the PRCM register (RF_open already verified that it is valid) */
        HWREG(PRCM_BASE + PRCM_O_RFCMODESEL) = RF_currClient->clientConfig.pRfMode->rfMode;

        /* Notiy the power driver that Standby is not allowed and RF core need to be powered */
        Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);
        Power_setDependency(PowerCC26XX_DOMAIN_RFCORE);

        /* Indicate that the power-up sequence is being started */
        RF_core.status = RF_CoreStatusPoweringUp;

        /* If the configuration on board level requires to set the dependency every time. */
        if (RFCC26XX_hwAttrs.xoscHfAlwaysNeeded == false)
        {
            Power_setDependency(PowerCC26XX_XOSC_HF);
        }

        /* If there are RFE and MCE patches, turn on their clocks */
        if ((RF_currClient->clientConfig.pRfMode->mcePatchFxn != NULL) ||
            (RF_currClient->clientConfig.pRfMode->rfePatchFxn != NULL))
        {
            RF_dbellSubmitCmdAsync((uint32_t)CMDR_DIR_CMD_2BYTE(RF_CMD0, RFC_PWR_PWMCLKEN_MDMRAM | RFC_PWR_PWMCLKEN_RFERAM));
        }

        /* Turn on the clock to the RF core. Registers can be accessed afterwards. */
        RFCClockEnable();

        /* Reconfigure the CPE interrupt lines to a start up value on a controlled way. */
        RFCCpeIntDisable(RF_CPE0_INT_MASK);
        RFCCpe0IntSelect(RF_CPE0_INT_MASK);

        /* Enable some of the interrupt sources. */
        RFCCpeIntEnable(RFC_DBELL_RFCPEIEN_BOOT_DONE_M
                        | RFC_DBELL_RFCPEIEN_LAST_COMMAND_DONE_M
                        | RFC_DBELL_RFCPEIEN_IRQ14_M);

        /* Set the next state. */
        RF_core.fxn = RF_fsmSetupState;

        /* Enable interrupts: continue when boot is done */
        HwiP_enableInterrupt(INT_RFC_HW_COMB);
        HwiP_enableInterrupt(INT_RFC_CPE_0);
    }
}

/*
 *  RF state machine function during setup state.
 *
 *  Input:  pObj - Pointer to RF object.
 *          e    - State machine event.
 *  Return: none
 */
static void RF_fsmSetupState(RF_Object *pObj, RF_FsmEvent e)
{
    if (e & RF_FsmEventPowerStep)
    {
        /* Apply RF Core patches (if required) */
        RF_applyRfCorePatch(RF_PHY_BOOTUP_MODE);

        /* Initialize system bus request */
        RF_dbellSubmitCmdAsync((uint32_t)CMDR_DIR_CMD_1BYTE(CMD_BUS_REQUEST, 1));

        /* Configure the RAT_SYNC command which will follow SETUP command */
        RF_ratSyncCmd.start.commandNo                = CMD_SYNC_START_RAT;
        RF_ratSyncCmd.start.status                   = IDLE;
        RF_ratSyncCmd.start.startTrigger.triggerType = TRIG_NOW;
        RF_ratSyncCmd.start.pNextOp                  = NULL;
        RF_ratSyncCmd.start.condition.rule           = COND_NEVER;

        /* Init the content of setup command. */
        RF_initRadioSetup(pObj);

        /* Configure the SETUP command. */
        RF_RadioSetup* pRadioSetup = pObj->clientConfig.pRadioSetup;

        /* Search for specific commands in the command chain. */
        RF_Op* tmp = (RF_Op*)&pRadioSetup->prop;
        while ((tmp->pNextOp) && (tmp->pNextOp->commandNo != CMD_SYNC_START_RAT) &&
               (tmp->pNextOp->commandNo != CMD_FS) &&
               (tmp->pNextOp->commandNo != CMD_FS_OFF))
        {
            /* Trace to the end of chain */
            tmp = tmp->pNextOp;
        }

        /* Add the CMD_RAT_SYNC to the end of chain */
        tmp->pNextOp        = (RF_Op*)&RF_ratSyncCmd.start;
        tmp->condition.rule = COND_ALWAYS;

        /* Setup FS command to follow SETUP command */
        RF_Cmd* pCmdFirstPend = (RF_Cmd*)List_head(&RF_cmdQ.pPend);
        if (pCmdFirstPend && ((pCmdFirstPend->pOp->commandNo == CMD_FS) || (pCmdFirstPend->pOp->commandNo == CMD_FS_OFF)))
        {
            /* Do Nothing */
        }
        else
        {
            if (pObj->state.mode_state.cmdFs.commandNo)
            {
                /* Chain in the implicit FS command */
                rfc_CMD_FS_t* pOpFs                = &pObj->state.mode_state.cmdFs;
                pOpFs->status                      = IDLE;
                pOpFs->pNextOp                     = NULL;
                pOpFs->startTrigger.triggerType    = TRIG_NOW;
                pOpFs->condition.rule              = COND_NEVER;
                RF_ratSyncCmd.start.pNextOp        = (RF_Op*)pOpFs;
                RF_ratSyncCmd.start.condition.rule = COND_ALWAYS;
            }
        }

#if defined(DeviceFamily_PARENT) && (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1)
        /* Trim directly the radio register values based on the ID of setup command. */
        rfTrim_t rfTrim;
        RFCRfTrimRead((rfc_radioOp_t *)pRadioSetup, &rfTrim);
        RFCRfTrimSet(&rfTrim);
#endif

        /* Make sure system bus request is done by now */
        RF_dbellSyncOnAck();

        /* Set the next state. */
        RF_core.fxn = RF_fsmActiveState;

        /* Run the XOSC_HF switching if the pre-notify function setup the power
           constraint PowerCC26XX_SWITCH_XOSC_HF_MANUALLY */
        if (RF_core.manualXoscHfSelect)
        {
            /* Wait until the XOSC_HF is stable */
            while (!PowerCC26XX_isStableXOSC_HF());

            /* Invoke the XOSC_HF switching */
            PowerCC26XX_switchXOSC_HF();
        }
        else if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_XOSC_HF)
        {
            /* If the XOSC_HF is not ready yet, only execute the first hal of the chain*/
            tmp->condition.rule = COND_NEVER;

            /* Next state: RF_fsmXOSCState (polling XOSC_HF)*/
            RF_core.fxn = RF_fsmXOSCState;
        }

        /* Send the setup chain to the RF core */
        RF_dbellSubmitCmdAsync((uint32_t)pRadioSetup);

        /* Invoke the global callback. */
        RF_invokeGlobalCallback(RF_GlobalEventRadioSetup, (void*)pRadioSetup);
    }
}

/*
 *  RF state machine function during XOSC state.
 *
 *  Input:  pObj - Pointer to RF object.
 *          e    - State machine event.
 *  Return: none
 */
static void RF_fsmXOSCState(RF_Object *pObj, RF_FsmEvent e)
{
    if ((e & RF_FsmEventPowerStep) || (e & RF_FsmEventWakeup))
    {
        /* If XOSC_HF is now ready */
        if (OSCClockSourceGet(OSC_SRC_CLK_HF) == OSC_XOSC_HF)
        {
            /* Next state: RF_fsmActiveState */
            RF_core.fxn = RF_fsmActiveState;

            /* Continue with the CMD_RAT_SYNC and the rest of the chain. */
            RF_dbellSubmitCmdAsync((uint32_t)&RF_ratSyncCmd.start);
        }
        else
        {
            /* Clock source not yet switched to XOSC_HF: schedule new polling */
            RF_restartClockTimeout(&RF_clkPowerUpObj, RF_XOSC_HF_SWITCH_CHECK_PERIOD_US/ClockP_getSystemTickPeriod());
        }
    }
}

/*
 *  RF state machine function during active state.
 *
 *  Input:  pObj - Pointer to RF object.
 *          e    - State machine event.
 *  Return: none
 */
static void RF_fsmActiveState(RF_Object *pObj, RF_FsmEvent e)
{
    volatile RF_Cmd* pCmd;
    RF_EventMask events;
    bool transitionAllowed;
    uint32_t key;

    if (e & RF_FsmEventCpeInt)
    {
        /* Enter critical section */
        key = HwiP_disable();

        /* Dereference the command which requested the callback*/
        pCmd = (RF_Cmd*)RF_cmdQ.pCurrCmdCb;

        /* If this is due to other event than LastCmdDone */
        if (pCmd && !(pCmd->rfifg & RF_TERMINATION_EVENT_MASK))
        {
            /* Temporarily store the reason of callback */
            events = pCmd->rfifg;

            /* Clear the events which are handled here */
            pCmd->rfifg &= (~events);

            /* Exit critical section */
            HwiP_restore(key);

            /* Invoke the user callback if it is provided */
            if (pCmd->pCb && events)
            {
                pCmd->pCb(pCmd->pClient, pCmd->ch, events);
            }
        }
        else
        {
            /* Exit critical section */
            HwiP_restore(key);
        }

        /* We've handled this event now */
        e &= ~RF_FsmEventCpeInt;
    }
    /* Coming from powerup states */
    else if (e & RF_FsmEventPowerStep)
    {
        /* RF core boot process is now finished */
        HWREG(PRCM_BASE + PRCM_O_RFCBITS) |= RF_BOOT1;

        /* Release the constraint on the FLASH in IDLE */
        if (bDisableFlashInIdleConstraint)
        {
            Power_releaseConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);
            bDisableFlashInIdleConstraint = false;
        }

        /* Enter critical section */
        key = HwiP_disable();

        /* Init last LF clock source to default boot source */
        static uint32_t lfClockSourceLast = OSC_RCOSC_HF;

        uint32_t lfClockSource = OSCClockSourceGet(OSC_SRC_CLK_LF);

        /* Update power up duration if the LF clock is derived from the same source as the last check.
            Reset the calculation if the LF clock source has changed */
        if ((lfClockSource == lfClockSourceLast)
             && pObj->clientConfig.bMeasurePowerUpDuration
             && RF_rtcTimestampA)
        {
            /* Dereference the active background command */
            pCmd = (RF_Cmd*)RF_cmdQ.pCurrCmdBg;

            /* If CMD_FS is the first command executed after powerup, a cached FS cmd is not executed,
               thus the powerupduration is expected to be shorter and is stored in nPowerUpDurationFs */
            if (pCmd && ((pCmd->pOp->commandNo == CMD_FS) || (pCmd->pOp->commandNo == CMD_FS_OFF)))
            {
                RF_updatePowerupDuration(&(pObj->clientConfig.nPowerUpDurationFs));
            }
            /* Any other command executed on powerup would mean an implicit FS command is executed
               by the RF driver, so update nPowerUpduration. This duration includes the time required to
               execute the implicit FS cmd previously cached by the RF Driver */
            else
            {
                RF_updatePowerupDuration(&(pObj->clientConfig.nPowerUpDuration));
            }
        }
        else
        {
            /* LF Clock source changed, reset power up duration measurement */
            pObj->clientConfig.nPowerUpDuration = RF_DEFAULT_POWER_UP_TIME;
            pObj->clientConfig.nPowerUpDurationFs = RF_DEFAULT_POWER_UP_TIME;
            lfClockSourceLast = lfClockSource;
        }

        /* Exit critical section */
        HwiP_restore(key);

        /* Check the status of the CMD_FS, if it was sent (chained) to the setup command.
           If it failed, return an error callback to the client.
           The client can either resend the CMD_FS or ignore the error as per Errata on PG2.1 */
        if (RF_checkCmdFsError())
        {
            /* Invoke the error callback: deault is do nothing */
            RF_Callback pErrCb = (RF_Callback)pObj->clientConfig.pErrCb;
            pErrCb(pObj, RF_ERROR_CMDFS_SYNTH_PROG, RF_EventError);

            /* Check if there is pending command */
            if (List_head(&RF_cmdQ.pPend))
            {
                /* Make sure the next pending command gets dispatched by issuing CPE0 IRQ */
                RF_dispatchNextEvent();
            }
            else
            {
                /* No pending command */
                e |= RF_FsmEventLastCommandDone;
            }
        }

        /* Issue power up callback: the RF core is active */
        RF_Callback pPowerCb = (RF_Callback)pObj->clientConfig.pPowerCb;
        pPowerCb(pObj, 0, RF_EventPowerUp);

        /* We've handled this event now */
        e &= ~RF_FsmEventPowerStep;
    }
    else if (e & RF_FsmEventLastCommandDone)
    {
        /* Issue radio operation done callback */
        RF_radioOpDoneCb();

        /* Take the next command in the done queue if any left */
        if (List_empty(&RF_cmdQ.pDone))
        {
            /* We've handled this event now */
            e &= ~RF_FsmEventLastCommandDone;
        }
    }
    else if (e & RF_FsmEventInitChangePhy)
    {
        /* Enter critical section */
        key = HwiP_disable();

        /* We only continue with phy switching if the RF core is still available.
           This check is important since the queues might have changed in the meantime
           of servicing the SWI. */
        transitionAllowed = RF_isStateTransitionAllowed();

        /* Take the next command from the pend queue */
        RF_Cmd* pNextCmd = (RF_Cmd*)List_head(&RF_cmdQ.pPend);

        if ((transitionAllowed == true) && (pNextCmd != NULL))
        {
            /* Indicate that we are changing phy on the RF core. */
            RF_core.status = RF_CoreStatusPhySwitching;

            /* Change HWI handler while switching the phy */
            HwiP_setFunc(&RF_hwiCpe0Obj, RF_hwiCpe0ChangePhy, (uintptr_t)NULL);

            /* Exit critical section */
            HwiP_restore(key);

            /* Stop inactivity clock of the current client if running */
            ClockP_stop(&RF_clkInactivityObj);

            /* Store the timestamp or measurement of the switching time */
            RF_rtcBeginSequence = AONRTCCurrent64BitValueGet();

            /* Notify the power driver that FLASH is needed in IDLE */
            bDisableFlashInIdleConstraint = true;
            Power_setConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);

            /* Switch the current client to the commands client */
            RF_currClient = pNextCmd->pClient;

            /* Do client switch callback if provided */
            if (RF_currClient->clientConfig.nClientEventMask & RF_ClientEventSwitchClientEntered)
            {
                RF_ClientCallback pClientEventCb = (RF_ClientCallback)RF_currClient->clientConfig.pClientEventCb;
                pClientEventCb(RF_currClient, RF_ClientEventSwitchClientEntered, NULL);
            }

            /* Apply the new RF Core patch */
            RF_applyRfCorePatch(RF_PHY_SWITCHING_MODE);

            /* Ensure that the analog domain is updated. */
            RF_currClient->clientConfig.bUpdateSetup = true;

            /* Ensure that the overrides are correct. */
            RF_initRadioSetup(RF_currClient);

            /* Configure the SETUP command */
            RF_RadioSetup* pRadioSetup = RF_currClient->clientConfig.pRadioSetup;

            /* Walk the chain and search or specific commands */
            RF_Op* tmp = (RF_Op*)&pRadioSetup->prop;
            while ((tmp->pNextOp) && (tmp->pNextOp->commandNo != CMD_SYNC_START_RAT) &&
                   (tmp->pNextOp->commandNo != CMD_FS) &&
                   (tmp->pNextOp->commandNo != CMD_FS_OFF))
            {
                tmp = tmp->pNextOp;
            }

            /* Clear any of the found specific command */
            tmp->pNextOp        = NULL;
            tmp->condition.rule = COND_NEVER;

            /* Setup FS command to follow SETUP command */
            RF_Op* pOpFirstPend = pNextCmd->pOp;
            if ((pOpFirstPend->commandNo == CMD_FS) || (pOpFirstPend->commandNo == CMD_FS_OFF))
            {
                /* First command is FS command so no need to chain an implicit FS command -> reset nRtc2 */
                RF_rtcBeginSequence = 0;
            }
            else
            {
                if (RF_currClient->state.mode_state.cmdFs.commandNo)
                {
                    /* Chain in the implicit FS command */
                    rfc_CMD_FS_t* pOpFs              = &RF_currClient->state.mode_state.cmdFs;
                    pOpFs->status                    = IDLE;
                    pOpFs->pNextOp                   = NULL;
                    pOpFs->startTrigger.triggerType  = TRIG_NOW;
                    pOpFs->condition.rule            = COND_NEVER;
                    tmp->pNextOp                     = (RF_Op*)pOpFs;
                    tmp->condition.rule              = COND_ALWAYS;
                }
            }

#if defined(DeviceFamily_PARENT) && (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1)
            /* Trim directly the radio register values based on the ID of setup command. */
            rfTrim_t rfTrim;
            RFCRfTrimRead((rfc_radioOp_t *)pRadioSetup, &rfTrim);
            RFCRfTrimSet(&rfTrim);
#endif

            /* Send the command chain */
            RF_dbellSubmitCmdAsync((uint32_t)pRadioSetup);

            /* Invoke the global callback. */
            RF_invokeGlobalCallback(RF_GlobalEventRadioSetup, (void*)pRadioSetup);
        }
        else
        {
            /* Exit critical section */
            HwiP_restore(key);
        }

        /* We've handled this event now */
        e &= ~RF_FsmEventInitChangePhy;
    }
    else if (e & RF_FsmEventFinishChangePhy)
    {
        /* Release the constraint on the FLASH in IDLE */
        if (bDisableFlashInIdleConstraint)
        {
            Power_releaseConstraint(PowerCC26XX_NEED_FLASH_IN_IDLE);
            bDisableFlashInIdleConstraint = false;
        }

        /* Check the status of the CMD_FS, if it was sent (chained) to the setup command.
           If it failed, invoke the error callback of the client.
           The client can either resend the CMD_FS or ignore the error. */
        if (RF_checkCmdFsError())
        {
            RF_Callback pErrCb = (RF_Callback)RF_currClient->clientConfig.pErrCb;
            pErrCb(RF_currClient, RF_ERROR_CMDFS_SYNTH_PROG, RF_EventError);
        }

        /* Only compute PHY switching time if RF_rtcBeginSequence is not zero (was initialized) */
        if (RF_rtcBeginSequence)
        {
            /* Record the timestamp for switching time measurement. */
            uint32_t RF_rtcEndSequence = (uint32_t) AONRTCCurrent64BitValueGet();

            /* Calculate how long it took to reconfigure the radio to a new phy. */
            RF_currClient->clientConfig.nPhySwitchingDuration = UDIFF(RF_rtcBeginSequence, RF_rtcEndSequence);
            RF_currClient->clientConfig.nPhySwitchingDuration >>= RF_RTC_CONV_TO_US_SHIFT;

            /* Reset RF_rtcBeginSequence value at the end of phy switching sequence. */
            RF_rtcBeginSequence = 0;
        }

        /* Change HWI handler */
        HwiP_setFunc(&RF_hwiCpe0Obj, RF_hwiCpe0Active, (uintptr_t)NULL);

        /* Mark radio and client as being active */
        RF_core.status = RF_CoreStatusActive;

        /* Serve the callbacks if the queue was rearranged while PHY switching was performed. */
        if (List_head(&RF_cmdQ.pDone))
        {
            SwiP_or(&RF_swiFsmObj, RF_FsmEventLastCommandDone);
        }

        /* Run the scheduler again. */
        RF_dispatchNextEvent();

        /* We have handled this event now */
        e &= ~RF_FsmEventFinishChangePhy;
    }
    else if (e & RF_FsmEventPowerDown)
    {
        /* Enter critical section. */
        key = HwiP_disable();

        /* Verify if the decision has not been reverted in the meantime. */
        transitionAllowed = RF_isStateTransitionAllowed();

        /* If possible, put the running RAT channels into pending state allowing to
           power down the RF core. */
        if (transitionAllowed)
        {
            transitionAllowed = RF_ratReleaseChannels();
        }

        /* If there is nothing prevent us to power down, proceed. */
        if (transitionAllowed)
        {
            /* Indicate that the RF core is being powered down from now */
            RF_core.status = RF_CoreStatusPoweringDown;

            /* Stop inactivity timer. */
            ClockP_stop(&RF_clkInactivityObj);

            /* Exit ritical setion. */
            HwiP_restore(key);

            /* Execute power down sequence of the RF core */
            RF_corePowerDown();

            /* Invoke the global callback. At this point the clock of RF core is OFF, but the
               power domain is still powered (hence the doorbell signals are still active.
               We do the callback here to save some power. */
            RF_invokeGlobalCallback(RF_GlobalEventRadioPowerDown, NULL);

            /* Notify the power driver that Standby mode is allowed and the RF core can be powered down. */
            Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
            Power_releaseDependency(PowerCC26XX_DOMAIN_RFCORE);

            /* Closing all handles */
            if (!RF_numClients)
            {
                /* Release the semaphore to be sure no one is pending on it */
                SemaphoreP_post(&RF_currClient->state.semSync);
            }

            /* If there is no specific client request or the XOSC, release the dependency */
            if (RFCC26XX_hwAttrs.xoscHfAlwaysNeeded == false)
            {
                Power_releaseDependency(PowerCC26XX_XOSC_HF);
            }

            /* Release constraint of switching XOSC_HF from the RF driver itself */
            if (RF_core.manualXoscHfSelect)
            {
                RF_core.manualXoscHfSelect = false;
                Power_releaseConstraint(PowerCC26XX_SWITCH_XOSC_HF_MANUALLY);
            }

            /* Next state: RF_fsmPowerUpState */
            RF_core.fxn = RF_fsmPowerUpState;

            /* Indicate that the RF core is now powered down */
            RF_core.status = RF_CoreStatusIdle;

            /* Issue radio available callback if RF_yield was called with no
            pending commands in the queue */
            uint8_t tmp = RF_RADIOFREECB_REQACCESS_FLAG;
            if (RF_cmdQ.nSeqDone == RF_cmdQ.nSeqPost)
            {
                tmp |= RF_RADIOFREECB_PREEMPT_FLAG | RF_RADIOFREECB_CMDREJECT_FLAG;
            }
            RF_issueRadioFreeCb(tmp);
        }
        else
        {
            /* Exit ritical setion. */
            HwiP_restore(key);
        }

        /* Reschedule the next event based on the state of the command queue
           and the RAT module. We do it here as future commands need to work even if
           power management is disabled manually. */
        RF_dispatchNextEvent();

        /* We've handled this event now */
        e &= ~RF_FsmEventPowerDown;
    }
    else if (e & RF_FsmEventRunScheduler)
    {
        /* Run the scheduler again. */
        RF_dispatchNextEvent();

        /* We've handled this event now */
        e &= ~RF_FsmEventRunScheduler;
    }

    /* Call self again if there are outstanding events to be processed */
    if (e)
    {
        /* Trig the SWI with the remained/unhandled events */
        SwiP_or(&RF_swiFsmObj, e);
    }
}

/*-------------- Initialization & helper functions ---------------*/

/*
 *  Initialize RF driver.
 *
 *  Input:  none
 *  Return: none
 */
static void RF_init(void)
{
    union {
        HwiP_Params hp;
        SwiP_Params sp;
    } params;

    /* Power init */
    Power_init();

    /* Enable output RTC clock for Radio Timer Synchronization */
    HWREG(AON_RTC_BASE + AON_RTC_O_CTL) |= AON_RTC_CTL_RTC_UPD_EN_M;

    /* Set the automatic bus request */
    HWREG(PRCM_BASE + PRCM_O_RFCBITS) = RF_BOOT0;

    /* Initialize SWI used by the RF driver. */
    SwiP_Params_init(&params.sp);
    params.sp.priority = RFCC26XX_hwAttrs.swiPriority;
    SwiP_construct(&RF_swiFsmObj, RF_swiFsm, &params.sp);
    SwiP_construct(&RF_swiHwObj,  RF_swiHw,  &params.sp);

    /* Initialize HWI used by the RF driver. */
    HwiP_Params_init(&params.hp);
    params.hp.priority = RFCC26XX_hwAttrs.hwiPriority;
    HwiP_construct(&RF_hwiCpe0Obj, INT_RFC_CPE_0,   RF_hwiCpe0PowerFsm, &params.hp);
    HwiP_construct(&RF_hwiHwObj,   INT_RFC_HW_COMB, RF_hwiHw,           &params.hp);

    /* Initialize clock object used as power-up trigger */
    ClockP_construct(&RF_clkPowerUpObj, &RF_clkPowerUp, 0, NULL);
    ClockP_construct(&RF_clkInactivityObj, &RF_clkInactivityCallback, 0, NULL);

    /* If TCXO is selected in CCFG, the RF Driver must not be allowed to control
       the XOSC switching by subscribing to wakeup notification from the Power driver. */
    if (CCFGRead_XOSC_FREQ() != CCFGREAD_XOSC_FREQ_TCXO)
    {
        /* Subscribe to wakeup notification from the Power driver */
        Power_registerNotify(&RF_wakeupNotifyObj,                     /* Object to register */
                             PowerCC26XX_AWAKE_STANDBY,               /* Event the notification to be invoked upon */
                             (Power_NotifyFxn) RF_wakeupNotification, /* Function to be invoked */
                             (uintptr_t) NULL);                       /* Parameters */
    }

    /* Set the XOSC_HF dependency if the HW attributes say so. This will ensure
       that the XOSC_HF is turned on by the power driver as soon as possible when
       coming out of standby. */
    if (RFCC26XX_hwAttrs.xoscHfAlwaysNeeded == true)
    {
        Power_setDependency(PowerCC26XX_XOSC_HF);
    }

    /* Initialized the queues. */
    List_clearList(&RF_cmdQ.pDone);
    List_clearList(&RF_cmdQ.pPend);

    /* Initialize global variables */
    RF_core.status                    = RF_CoreStatusIdle;
    RF_core.init                      = false;
    RF_core.activeTimeUs              = 0;
    RF_core.manualXoscHfSelect        = false;
    RF_ratModule.availableRatChannels = RF_DEFAULT_AVAILRATCH_VAL;
    RF_rtcTimestampA                  = 0;
    RF_rtcBeginSequence               = 0;
    RF_errTolValInUs                  = RF_DEFAULT_RAT_RTC_ERR_TOL_IN_US;
    RF_powerConstraint                = 0;

    /* Set FSM state to power up */
    RF_core.fxn = RF_fsmPowerUpState;

    /* Invoke the board file init function. */
    RF_invokeGlobalCallback(RF_GlobalEventInit, NULL);
}

/*
 *  Trace through the pending queue and flush the command(s).
 *
 *  Input:  h         - Handle to the client calling this function.
 *          pCmd      - Pointer to the command where the cancelling should start with.
 *          bFlushAll - Decides weather one or more commands should be aborted.
 *  Return: Number of commands was terminated.
 */
static uint32_t RF_discardPendCmd(RF_Handle h, RF_Cmd* pCmd, bool bFlushAll, bool bPreempt)
{
    /* Local variables, start from the head of queue. */
    uint32_t numDiscardedCmd = 0;
    RF_Cmd*  pElem           = (RF_Cmd*)List_head(&RF_cmdQ.pPend);

    /* Find the first command to be cancelled. */
    while (pElem && (pElem != pCmd))
    {
        pElem = (RF_Cmd*)List_next((List_Elem*)pElem);
    }

    /* If we found the command to be cancelled. */
    while (pElem)
    {
        /* Temporarly store the next element, since we will need
           to continue from there. */
        RF_Cmd* pNextElem = (RF_Cmd*)List_next((List_Elem*)pElem);

        if (RF_isClientOwner(h, pElem))
        {
            /* Mark the command that it was cancelled. */
            RF_cmdStoreEvents(pElem, RF_EventCmdCancelled);

            if (bPreempt)
            {
                /* Mark the command as being preempted. */
                RF_cmdStoreEvents(pElem, RF_EventCmdPreempted);

                /* Subscribe the client for RadioFree callback. */
                RF_Sch.clientHndRadioFreeCb   = pCmd->pClient;
                RF_Sch.issueRadioFreeCbFlags |= RF_RADIOFREECB_PREEMPT_FLAG;
            }

            /* Remove the command from the pend queue and place it to
               the done queue. */
            List_remove(&RF_cmdQ.pPend, (List_Elem*)pElem);
            List_put(&RF_cmdQ.pDone, (List_Elem*)pElem);

            /* Increment the counter of cancelled commands. */
            numDiscardedCmd += 1;
        }

        /* Break the loop if only single cancel was requested.
           Step the queue otherwise. */
        if (bFlushAll)
        {
            pElem = pNextElem;
        }
        else
        {
            break;
        }
    }

    /* Return with the number of cancelled commands. */
    return(numDiscardedCmd);
}

/*
 *  Process cancel commands. It is used by RF_cancelCmd, RF_flushCmd API.
 *
 *  Input:  h        - Handle to the client calling this function.
 *          ch       - Handle to the command where the cancelling should start with.
 *          graceful - true:  stop the command
 *                     false: abort the command
 *          flush    - true:  flush all commands of this client
 *                     false: only cancel the given command
 *          preempt  - mark the command as the reason of aborting is preemption
 *  Return: status
 */
static RF_Stat RF_abortCmd(RF_Handle h, RF_CmdHandle ch, bool graceful, bool flush, bool preempt)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Initialize local variables */
    RF_Cmd* pCmd       = NULL;
    RF_Stat status     = RF_StatInvalidParamsError;
    RF_EventMask event = graceful ? RF_EventCmdStopped : RF_EventCmdAborted;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Handle FLUSH_ALL request */
    if (ch == RF_CMDHANDLE_FLUSH_ALL)
    {
        /* Start to cancel the commands from the actively running onces if it belongs to this client. */
        if (RF_isClientOwner(h, RF_cmdQ.pCurrCmdBg))
        {
            pCmd = RF_cmdQ.pCurrCmdBg;
        }
        else if (RF_isClientOwner(h, RF_cmdQ.pCurrCmdFg))
        {
            pCmd = RF_cmdQ.pCurrCmdFg;
        }
        else
        {
            /* Start to walk the pending queue from its head. */
            pCmd = (RF_Cmd*)List_head(&RF_cmdQ.pPend);
        }
    }
    else
    {
        /* Search for the command in the command pool based on its handle. The command can
           locate on any of the queues at this point. */
        pCmd = RF_cmdGet(h, ch, 0x00);
    }

    /* If command handle is valid, proceed to cancel. */
    if (pCmd)
    {
        /* If the command is still allocated. */
        if (pCmd->flags & RF_CMD_ALLOC_FLAG)
        {
            /* If the command we want to cancel is actively running. */
            if ((pCmd == RF_cmdQ.pCurrCmdBg) || (pCmd == RF_cmdQ.pCurrCmdFg))
            {
                /* Flag that the command has been aborted. In IEEE 15.4 mode, this means
                   aborting both the background and foreground commands. */
                RF_cmdStoreEvents(RF_cmdQ.pCurrCmdBg, event);
                RF_cmdStoreEvents(RF_cmdQ.pCurrCmdFg, event);

                /* Decode what method to use to terminate the ongoing radio operation. */
                uint32_t directCmd = (graceful) ? CMDR_DIR_CMD(CMD_STOP) : CMDR_DIR_CMD(CMD_ABORT);

                /* Send the abort/stop command through the doorbell to the RF core. */
                RFCDoorbellSendTo(directCmd);

                if (preempt)
                {
                    /* Mark the command as being preempted. */
                    RF_cmdStoreEvents(RF_cmdQ.pCurrCmdBg, RF_EventCmdPreempted);
                    RF_cmdStoreEvents(RF_cmdQ.pCurrCmdFg, RF_EventCmdPreempted);

                    /* Subscribe the client for RadioFree callback. */
                    RF_Sch.clientHndRadioFreeCb   = pCmd->pClient;
                    RF_Sch.issueRadioFreeCbFlags |= RF_RADIOFREECB_PREEMPT_FLAG;
                }

                /* Remove all commands from the pend queue belong to this client. Only do it
                   if it was explicitely requested through the flush argument. */
                if (flush)
                {
                    RF_discardPendCmd(h, (RF_Cmd*)List_head(&RF_cmdQ.pPend), flush, preempt);
                }

                /* Return with success as we cancelled at least the currently running command. */
                status = RF_StatSuccess;
            }
            else
            {
                /* Remove one/all commands from the pend queue based on the flush argument.
                   If at least one command is cancelled the operation was succesful. Otherwise,
                   either the pend queue is empty or pCmd have terminated earlier */
                if (RF_discardPendCmd(h, pCmd, flush, preempt))
                {
                    /* Kick the state machine to handle the done queue and re-execute the scheduler.
                       This is not necessary when the RF is currently performing a power-up. */
                    if ((RF_core.status != RF_CoreStatusPoweringUp) &&
                        (RF_core.status != RF_CoreStatusPhySwitching))
                    {
                        SwiP_or(&RF_swiFsmObj, (RF_FsmEventLastCommandDone | RF_FsmEventRunScheduler));
                    }

                    /* At least one command was cancelled. */
                    status = RF_StatSuccess;
                }
                else
                {
                    /* The command is not running and is not in the pend queue. It is located on the
                       done queue, hence return RF_StatCmdEnded. */
                    status = RF_StatCmdEnded;
                }
            }
        }
        else
        {   /* If command is still in the pool but it is not allocated anymore, i.e. it was already served. */
            status = RF_StatCmdEnded;
        }
    }

    /* Exit critical section */
    HwiP_restore(key);

    /* Return with the result:
     - RF_StatSuccess if at least one command was cancelled.
     - RF_StatCmdEnded, when the command already finished (in the Done Q, but not deallocated yet).
     - RF_StatInvalidParamsError otherwise.  */
    return(status);
}

/*
 *  Execute a direct or immediate command in the RF Core if possible.
 *
 *  Input:  pCmd                       - Pointer to the command which shall be sent to the RF core.
 *          rawStatus                  - Return address of the raw status byte read from the CMDSTA register.
 *  Return: The return value interprets and converts the result of command execution to and RF_Stat value.
 *          RF_StatCmdDoneSuccess       - If the command was sent and accepted by the RF core.
 *          RF_StatCmdDoneError         - Command was rejected by the RF core.
 *          RF_StatRadioInactiveError   - The RF core is OFF.
 */
static RF_Stat RF_executeDirectImmediateCmd(uint32_t pCmd, uint32_t* rawStatus)
{
    /* If the RF core is ON, we can send the command */
    if (RF_core.status == RF_CoreStatusActive)
    {
        /* Submit the command to the doorbell */
        uint32_t localStatus = RFCDoorbellSendTo(pCmd);

        /* Pass the rawStatus to the callee if possible. */
        if (rawStatus)
        {
            *rawStatus = localStatus;
        }

        /* Check the return value of the RF core through the CMDSTA register within the doorbell */
        if ((localStatus & RF_CMDSTA_REG_VAL_MASK) == CMDSTA_Done)
        {
            /* The command was accepted */
            return(RF_StatCmdDoneSuccess);
        }
        else
        {
            /* The command was rejected */
            return(RF_StatCmdDoneError);
        }
    }
    else
    {
        /* The RF core is not capable of receiving the command */
        return(RF_StatRadioInactiveError);
    }
}

/*
 *  Send a direct or immediate command to the RF core. The command is rejected
 *  if the RF core is configured to a different PHY (client).
 *
 *  Input:  h                           - Handle to the client calling this function.
 *          pCmd                        - Pointer to the command which shall be sent to the RF core.
 *          rawStatus                   - Return address of raw status byte read from CMDSTA register.
 *  Return: RF_StatCmdDoneSuccess       - If the command was sent and accepted by the RF core.
 *          RF_StatCmdDoneError         - Command was rejected by the RF core.
 *          RF_StatInvalidParamsError   - Client do not have the right to send commands now.
 *          RF_StatRadioInactiveError   - The RF core is OFF.
 */
static RF_Stat RF_runDirectImmediateCmd(RF_Handle h, uint32_t pCmd, uint32_t* rawStatus)
{
    /* Local variable. */
    RF_Stat status;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Only the current client is allowed to send direct commands */
    if (h != RF_currClient)
    {
        /* Return with an error code it is a different client */
        status = RF_StatInvalidParamsError;
    }
    else
    {
        /* Execute the direct or immediate command. */
        status = RF_executeDirectImmediateCmd(pCmd, rawStatus);
    }

    /* Exit critical section. */
    HwiP_restore(key);

    /* Return with the status information about the success of command execution. */
    return(status);
}

/*
 *  Helper function to find the first override representing a High PA value (CC13x2P devices).
 *
 *  Input:  pOverride                     - Pointer to an override list to be searched.
 *          overridePattern               - Pattern o override to search for.
 *          currentValue                  - Reference where the current value can be returned.
 *  Return: paOffset                      - Offset of the High PA override.
 *          RF_TX_OVERRIDE_INVALID_OFFSET - No override was found in the list.
 */
static uint8_t RF_getPAOverrideOffsetAndValue(uint32_t* pOverride, uint32_t overridePattern, uint32_t* currentValue)
{
    /* Search for the particular override. */
    uint8_t paOffset = RFCOverrideSearch(pOverride, overridePattern, RF_TX_OVERRIDE_MASK, RF_OVERRIDE_SEARCH_DEPTH);

    /* If the override was found. */
    if (currentValue)
    {
        *currentValue = pOverride[paOffset] >> RF_TX_OVERRIDE_SHIFT;
    }

    /* Return with an invalid value. */
    return(paOffset);
}

/*
 *  Helper function to find and replace the first override representing a High PA value.
 *
 *  Input:  pOverride                     - Pointer to an override list to be searched.
 *          overridePattern               - Mask of override type to searh for.
 *          newValue                      - The new raw value the PA to be set to.
 *  Return: paOffset                      - Offset of the High PA override.
 *          RF_TX_OVERRIDE_INVALID_OFFSET - No override was found in the list. Hence nothing to replace.
 */
static uint8_t RF_searchAndReplacePAOverride(uint32_t* pOverride, uint32_t overridePattern, uint32_t newValue)
{
    /* Search for the particular override. */
    uint8_t paOffset = RF_getPAOverrideOffsetAndValue(pOverride, overridePattern, NULL);

    /* If the override was found. */
    if (paOffset != RF_TX_OVERRIDE_INVALID_OFFSET)
    {
        if (overridePattern == RF_TX20_PATTERN)
        {
            /* Replace the high PA gain with the new value. */
            pOverride[paOffset] = TX20_POWER_OVERRIDE(newValue);
        }
        else
        {
            /* Replace the default PA gain with the new value. */
            pOverride[paOffset] = TX_STD_POWER_OVERRIDE(newValue);
        }
    }

    /* Return with the offset of the PA override. */
    return(paOffset);
}

/*
 *  Appends the PA specific override list to the end of given overrides.
 *
 *  Input:  baseOverride - Override list to append the applicable segment to.
 *          newOverride  - Override segment to be appended.
 *  Return: none
 */
static void RF_attachOverrides(uint32_t* baseOverride, uint32_t* newOverride)
{
    if (newOverride != NULL)
    {
        /* Search for the attached override list. */
        uint32_t maskOverride = NEW_OVERRIDE_SEGMENT(newOverride);

        /* Search for the end of the base override list. We also look for new segment vectors. */
        while ((*baseOverride != END_OVERRIDE) && (*baseOverride != maskOverride))
        {
            baseOverride++;
        }

        /* Append the second override list. */
        *baseOverride = maskOverride;
    }
}

/*
 *  Terminate the override list at the first match of a jump to the given newOverride.
 *  The function assumes that there are no other jump vectors before.
 *
 *  Input:  baseOverride - Override list to append the applicable segment to.
 *          newOverride  - Override segment to be appended.
 *  Return: none
 */
static void RF_detachOverrides(uint32_t* baseOverride, uint32_t* newOverride)
{
    if (newOverride != NULL)
    {
        /* Search for the attached override list. */
        uint32_t maskOverride = NEW_OVERRIDE_SEGMENT(newOverride);

        /* Search for the end of the base override list. We also look for new segment vectors. */
        while ((*baseOverride != END_OVERRIDE) && (*baseOverride != maskOverride))
        {
            baseOverride++;
        }

        /* Append the second override list if exists. */
        *baseOverride = END_OVERRIDE;
    }
}

/*
 *  Decode all the override pointers according to the type of the setup command.
 *
 *  Input:  radioSetup           - Pointer to the setup command to be evaluated.
 *  Return: tx20FeatureAvailable - true if the High Gain PA is available.
 *          pTxPower             - Pointer to the txPower field of setup command.
 *          pRegOverride         - Pointer to the base override list.
 *          pRegOverrideTxStd    - Pointer to the Default PA override list.
 *          pRegOverrideTx20     - Pointer to the High PA override list.
 */
static bool RF_decodeOverridePointers(RF_RadioSetup* radioSetup, uint16_t** pTxPower, uint32_t** pRegOverride, uint32_t** pRegOverrideTxStd, uint32_t** pRegOverrideTx20)
{
    /* Read FCFG user ID register for device identification */
    uint32_t fcfg1UserId = ChipInfo_GetUserId();

    /* Decode if High Gain PA is even available. Bit FCFG1:USER_ID.PA tells us PA availability. */
    bool tx20FeatureAvailable = (( fcfg1UserId & FCFG1_USER_ID_PA_M ) >> FCFG1_USER_ID_PA_S );

    /* Only decode the offset of those fields which exist on this device. */
    if (tx20FeatureAvailable)
    {
        /* Local variables. */
        uint8_t loDivider;
        uint8_t frontEndMode;
        uint8_t index;

        /* Decode the offset of txPower field and all the override pointers
           available on the CC1352P/CC2652P devices. */
        switch (radioSetup->commandId.commandNo)
        {
            case (CMD_RADIO_SETUP):
                *pTxPower          = &radioSetup->common_pa.txPower;
                *pRegOverride      = radioSetup->common_pa.pRegOverride;
                *pRegOverrideTxStd = radioSetup->common_pa.pRegOverrideTxStd;
                *pRegOverrideTx20  = radioSetup->common_pa.pRegOverrideTx20;

                /* Input to recalculation of overrides. */
                loDivider          = radioSetup->common_pa.loDivider;
                frontEndMode       = radioSetup->common_pa.config.frontEndMode;

                break;
            case (CMD_BLE5_RADIO_SETUP):
                *pTxPower          = &radioSetup->ble5_pa.txPower;
                *pRegOverride      = radioSetup->ble5_pa.pRegOverrideCommon;
                *pRegOverrideTxStd = radioSetup->ble5_pa.pRegOverrideTxStd;
                *pRegOverrideTx20  = radioSetup->ble5_pa.pRegOverrideTx20;

                /* Input to recalculation of overrides. */
                loDivider          = radioSetup->ble5_pa.loDivider;
                frontEndMode       = radioSetup->ble5_pa.config.frontEndMode;

                break;
            case (CMD_PROP_RADIO_SETUP):
                *pTxPower          = &radioSetup->prop_pa.txPower;
                *pRegOverride      = radioSetup->prop_pa.pRegOverride;
                *pRegOverrideTxStd = radioSetup->prop_pa.pRegOverrideTxStd;
                *pRegOverrideTx20  = radioSetup->prop_pa.pRegOverrideTx20;

                /* Input to recalculation of overrides. */
                loDivider          = 0;
                frontEndMode       = radioSetup->prop_pa.config.frontEndMode;
                break;
            default:
                *pTxPower          = &radioSetup->prop_div_pa.txPower;
                *pRegOverride      = radioSetup->prop_div_pa.pRegOverride;
                *pRegOverrideTxStd = radioSetup->prop_div_pa.pRegOverrideTxStd;
                *pRegOverrideTx20  = radioSetup->prop_div_pa.pRegOverrideTx20;

                /* Input to recalculation of overrides. */
                loDivider          = radioSetup->prop_div_pa.loDivider;
                frontEndMode       = radioSetup->prop_div_pa.config.frontEndMode;
                break;
        }

        /* Modify the divider and front-end specific override. This is to keep the override
           list and the setup command in sync, even if the setup command was changed runtime
           due to the changing stack configuration. */
        if (*pRegOverrideTxStd)
        {
            index = RFCOverrideSearch(*pRegOverrideTxStd, RFC_FE_OVERRIDE_ADDRESS, RFC_FE_OVERRIDE_MASK, RFC_MAX_SEARCH_DEPTH);

            if (index < RFC_MAX_SEARCH_DEPTH)
            {
                (*pRegOverrideTxStd)[index] = RFCAnaDivTxOverride(loDivider, frontEndMode);
            }
        }

        if (*pRegOverrideTx20)
        {
            index = RFCOverrideSearch(*pRegOverrideTx20, RFC_FE_OVERRIDE_ADDRESS, RFC_FE_OVERRIDE_MASK, RFC_MAX_SEARCH_DEPTH);

            if (index < RFC_MAX_SEARCH_DEPTH)
            {
                (*pRegOverrideTx20)[index] = RFCAnaDivTxOverride(loDivider, RFC_FE_MODE_ESCAPE_VALUE);
            }
        }
    }
    else
    {
        /* Decode the offset of txPower field and the only relevant override pointer
           available on all other devices. */
        switch (radioSetup->commandId.commandNo)
        {
            case (CMD_RADIO_SETUP):
                *pTxPower     = &radioSetup->common.txPower;
                *pRegOverride = radioSetup->common.pRegOverride;
                break;
            case (CMD_BLE5_RADIO_SETUP):
                *pTxPower     = &radioSetup->ble5.txPower;
                *pRegOverride = radioSetup->ble5.pRegOverrideCommon;
                break;
            case (CMD_PROP_RADIO_SETUP):
                *pTxPower     = &radioSetup->prop.txPower;
                *pRegOverride = radioSetup->prop.pRegOverride;
                break;
            default:
                *pTxPower     = &radioSetup->prop_div.txPower;
                *pRegOverride = radioSetup->prop_div.pRegOverride;
                break;
        }

        /* Force the value of non-existing pointers to be NULL. */
        *pRegOverrideTxStd = NULL;
        *pRegOverrideTx20  = NULL;
    }

    /* Return if the High Gain PA feature is available or not. */
    return (tx20FeatureAvailable);
}

/*
 *  In case the PA configuration changes during the execution of a chain, this function
 *  propagates the change back to the setup command. This is to reserve the change even
 *  after a power cycle
 *
 *  Input:  handle - Radio handle the change should be stored within
 *  Return: none
 */
static void RF_extractPaConfiguration(RF_Handle handle)
{
    /* Local variable to store the return value of function call. It is not used here. */
    RF_ConfigurePaCmd configurePaCmd;

    /* Retrieve the PA configuration from the RF core itself. */
    RF_TxPowerTable_Value value;
    value.rawValue = RFCGetPaGain();
    value.paType   = (RF_TxPowerTable_PAType) RFCGetPaType();

    /* Update the setup command with the new settings. The change is now permanent
       and will be kept even if the RF core is powered off. */
    RF_updatePaConfiguration(handle->clientConfig.pRadioSetup, value, &configurePaCmd);
}

/*
 *  Helper function to find the HPOSC_OVERRIDE in provided override list and modify the HPOSC frequency offset.
 *
 *  Input:  pRegOverride            - Pointer to override list.
 *  Return: None
 */
static void RF_updateHpOscOverride(uint32_t *pRegOverride)
{
    /* Local variables. */
    int32_t tempDegC;
    int32_t relFreqOffset;
    int16_t relFreqOffsetConverted;

    /* Find override for HPOSC frequency offset. */
    if (pRegOverride)
    {
        uint8_t index;
        index = RFCOverrideSearch(pRegOverride, RF_HPOSC_OVERRIDE_PATTERN, RF_HPOSC_OVERRIDE_MASK, RF_OVERRIDE_SEARCH_DEPTH);

        if (index < RF_OVERRIDE_SEARCH_DEPTH)
        {
            /* Get temperature dependent HPOSC frequency offset */
            tempDegC = Temperature_getTemperature();
            relFreqOffset = OSC_HPOSCRelativeFrequencyOffsetGet(tempDegC);
            RF_currentHposcFreqOffset = relFreqOffset;
            relFreqOffsetConverted = OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert(relFreqOffset);

            /* Update override with the HPOSC frequency offset */
            pRegOverride[index] = HPOSC_OVERRIDE(relFreqOffsetConverted);
        }
    }
}

/*
 *  Helper function to find and modify the PA selection and gain of the provided setup command.
 *
 *  Input:  radioSetup            - Setup command belong to the client.
 *          newValue              - The new value the PA to be set to.
 *          configurePaCmd        - The immediate command to be used to apply the changes if the RF core is active.
 *  Return: RF_StatSuccess        - The setup command was reconfigured.
 *          Otherwise             - An error occured.
 */
static RF_Stat RF_updatePaConfiguration(RF_RadioSetup* radioSetup, RF_TxPowerTable_Value newValue, RF_ConfigurePaCmd* configurePaCmd)
{
    /* Set the default return value to indicate success. */
    RF_Stat status = RF_StatSuccess;

    /* Local variables. */
    uint16_t* pTxPower          = NULL;
    uint32_t* pRegOverride      = NULL;
    uint32_t* pRegOverrideTxStd = NULL;
    uint32_t* pRegOverrideTx20  = NULL;

    /* Decode if High Gain PA is available. */
    bool tx20FeatureAvailable = RF_decodeOverridePointers(radioSetup, &pTxPower, &pRegOverride, &pRegOverrideTxStd, &pRegOverrideTx20);

    /* The new value requires the deault PA. */
    if (newValue.paType == RF_TxPowerTable_DefaultPA)
    {
        /* On CC1352P/CC2652P devices with the correct override lists. */
        if (tx20FeatureAvailable && pRegOverrideTxStd && pRegOverrideTx20)
        {
            /* Store the new value in the setup command. */
            *pTxPower = (uint16_t) newValue.rawValue;

            /* Ensure that the gain within the overrides are also updated. */
            RF_searchAndReplacePAOverride(pRegOverrideTxStd, RF_TXSTD_PATTERN, newValue.rawValue);

            /* Detach the High Gain overrides. It does nothing if the overrides are not attached. */
            RF_detachOverrides(pRegOverride, pRegOverrideTx20);

            /* Return with the immediate command in the argument. */
            configurePaCmd->changePa.commandNo    = CMD_CHANGE_PA;
            configurePaCmd->changePa.pRegOverride = pRegOverrideTxStd;
        }
        else if (tx20FeatureAvailable)
        {
            /* Limited backward compatibility on CC1352P/CC2652P devices without the
               proper override lists. Only gain tuning on the Default PA is available. */
            if (*pTxPower != RF_TX20_ENABLED)
            {
                /* Store the new value in the setup command. */
                *pTxPower = (uint16_t) newValue.rawValue;

                /* Use the dedicated command to tune the gain */
                configurePaCmd->tuneTxPower.commandNo = CMD_SET_TX_POWER;
                configurePaCmd->tuneTxPower.txPower   = newValue.rawValue;
            }
            else
            {
                /* PA swithing is not allowed due to the missing overrides. */
                status = RF_StatInvalidParamsError;
            }
        }
        else
        {
            /* On any other devices, just accept the new gain. */
            *pTxPower = (uint16_t) newValue.rawValue;

            /* Use the dedicated command to tune the gain. */
            configurePaCmd->tuneTxPower.commandNo = CMD_SET_TX_POWER;
            configurePaCmd->tuneTxPower.txPower   = newValue.rawValue;
        }
    }
    else
    {
        /* On CC1352P/CC2652P devices with the correct override lists. */
        if (tx20FeatureAvailable && pRegOverrideTxStd && pRegOverrideTx20)
        {
            /* If the High Gain PA is available store the escape value in the setup
               command and update the overrides. */
            *pTxPower = (uint16_t) RF_TX20_ENABLED;

            /* Change the gain to the new value. */
            RF_searchAndReplacePAOverride(pRegOverrideTx20, RF_TX20_PATTERN, newValue.rawValue);

            /* Attach the High Gain overrides. */
            RF_attachOverrides(pRegOverride, pRegOverrideTx20);

            /* Return with the command argument to be used. */
            configurePaCmd->changePa.commandNo    = CMD_CHANGE_PA;
            configurePaCmd->changePa.pRegOverride = pRegOverrideTx20;
        }
        else if (tx20FeatureAvailable)
        {
            /* Limited backward compatibility on CC1352P/CC2652P devices without the
               proper override lists. Only gain tuning on the High PA is available
               if the gain override is present within the base override list.*/
            if (RF_searchAndReplacePAOverride(pRegOverride, RF_TX20_PATTERN, newValue.rawValue) == RF_TX_OVERRIDE_INVALID_OFFSET)
            {
                /* Cannot use the high gain PA without a proper override list
                   that contains at least a placeholder gain entry. */
                status = RF_StatInvalidParamsError;
            }
            else
            {
                /* If updating the override list with the gain value was succesful,
                   set the escape value in the setup command. */
                *pTxPower = (uint16_t) RF_TX20_ENABLED;

                /* Use the dedicated command to tune the gain. */
                configurePaCmd->tuneTx20Power.commandNo = CMD_SET_TX20_POWER;
                configurePaCmd->tuneTx20Power.tx20Power = newValue.rawValue;
            }
        }
        else
        {
            /* Do not accept any high gain PA values on devices which do not support it. */
            status = RF_StatInvalidParamsError;
        }
    }

    /* Return with the status. */
    return(status);
}

/*-------------- API functions ---------------*/
/*
 * ======== RF_open ========
 * Open an RF handle
 */
RF_Handle RF_open(RF_Object *pObj, RF_Mode* pRfMode, RF_RadioSetup* pRadioSetup, RF_Params *params)
{
    /* Local variables. */
    uint8_t index;
    uint16_t* pTxPower          = NULL;
    uint32_t* pRegOverride      = NULL;
    uint32_t* pRegOverrideTxStd = NULL;
    uint32_t* pRegOverrideTx20  = NULL;

    /* Assert */
    DebugP_assert(pObj != NULL);

    /* Read available RF modes from the PRCM register */
    uint32_t availableRfModes = HWREG(PRCM_BASE + PRCM_O_RFCMODEHWOPT);

    /* Check for illegal PHY mode in CC2672 device */
    #if defined(DeviceFamily_PARENT) && (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
    /* Check for chip ID */
    if ((ChipInfo_GetChipType() == CHIP_TYPE_CC2672P3) ||
        (ChipInfo_GetChipType() == CHIP_TYPE_CC2672R3))
    {
        /* Check if Sub-1G frequency band is used */
        if (pRadioSetup->commandId.commandNo == CMD_PROP_RADIO_DIV_SETUP)
        {
            if (pRadioSetup->prop_div.loDivider > 2)
                /* If the data rate is NOT 100kbps or 500 kbps return NULL handle
                   as this is an unsupported setting on this device */
                if (! ((pRadioSetup->prop_div.symbolRate.preScale == 0x0F &&
                        pRadioSetup->prop_div.symbolRate.rateWord == 0x10000) ||
                       (pRadioSetup->prop_div.symbolRate.preScale == 0x0F &&
                        pRadioSetup->prop_div.symbolRate.rateWord == 0x50000)))
                {
                    return(NULL);
                }
        }
        else if (pRadioSetup->commandId.commandNo == CMD_RADIO_SETUP)
        {
            if (pRadioSetup->common.loDivider > 2)
                return(NULL);
        }
        else if (pRadioSetup->commandId.commandNo == CMD_BLE5_RADIO_SETUP)
        {
            if (pRadioSetup->ble5.loDivider > 2)
                return(NULL);
        }
    }
    #endif

    /* Verify that the provided configuration is supported by this device.
       Reject any request which is not compliant. */
    if (pRfMode && pRadioSetup && (availableRfModes & (1 << pRfMode->rfMode)))
    {
        /* Trim the override list; The implementation of RFCOverrideUpdate is device specific */
        RFCOverrideUpdate((RF_Op*)pRadioSetup, NULL);

        /* Register the setup command to the client */
        pObj->clientConfig.pRadioSetup = pRadioSetup;

        /* Register the mode to the client */
        pObj->clientConfig.pRfMode = pRfMode;
    }
    else
    {
        /* Return with null if the device do not support the requested configuration */
        return(NULL);
    }

    /* Verify that the HPOSC_OVERRIDE exists in the override list if SW TCXO is enabled */
    if (pfnUpdateHposcOverride)
    {
        /* Get pointer to override list */
        RF_decodeOverridePointers(pRadioSetup, &pTxPower, &pRegOverride, &pRegOverrideTxStd, &pRegOverrideTx20);

        /* Search override list for HPOSC_OVERRIDE */
        index = RFCOverrideSearch(pRegOverride, RF_HPOSC_OVERRIDE_PATTERN, RF_HPOSC_OVERRIDE_MASK, RF_OVERRIDE_SEARCH_DEPTH);

        /* Return NULL if HPOSC_OVERRIDE is not found on the override list */
        if (index == 0xFF)
        {
            return(NULL);
        }
    }

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Check whether RF driver is accepting more clients */
    if (RF_numClients < N_MAX_CLIENTS)
    {
        /* Initialize shared objects on first client opening */
        if (RF_numClients == 0) RF_init();

        /* Save the new RF_Handle */
        RF_Sch.clientHnd[RF_numClients++] = pObj;

        /* Exit critical section */
        HwiP_restore(key);

        /* Populate default RF parameters if not provided */
        RF_Params rfParams;
        if (params == NULL)
        {
            RF_Params_init(&rfParams);
            params = &rfParams;
        }

        /* Initialize RF_Object configuration */
        pObj->clientConfig.nInactivityTimeout      = params->nInactivityTimeout;
        pObj->clientConfig.nPhySwitchingDuration   = RF_DEFAULT_PHY_SWITCHING_TIME;
        pObj->clientConfig.nClientEventMask        = params->nClientEventMask;
        pObj->clientConfig.nPowerUpDurationMargin  = params->nPowerUpDurationMargin;
        pObj->clientConfig.bUpdateSetup            = true;
        pObj->clientConfig.nID                     = params->nID;

        /* Decide if automatic adjustment should be used. */
        if (params->nPowerUpDuration)
        {
            pObj->clientConfig.nPowerUpDuration        = params->nPowerUpDuration;
            /* Use same value of user provided power up duration even if the first cmd
               is an Fs command */
            pObj->clientConfig.nPowerUpDurationFs      = params->nPowerUpDuration;
            pObj->clientConfig.bMeasurePowerUpDuration = false;
        }
        else
        {
            pObj->clientConfig.nPowerUpDuration        = RF_DEFAULT_POWER_UP_TIME;
            pObj->clientConfig.nPowerUpDurationFs      = RF_DEFAULT_POWER_UP_TIME;
            pObj->clientConfig.bMeasurePowerUpDuration = true;
        }

        /* Set all the callbacks to the default (do nothing) callback */
        pObj->clientConfig.pErrCb                 = (void*) RF_defaultCallback;
        pObj->clientConfig.pClientEventCb         = (void*) RF_defaultCallback;
        pObj->clientConfig.pPowerCb               = (void*) RF_defaultCallback;

        /* If a user specified callback is provided, overwrite the default */
        if (params->pErrCb)
        {
            pObj->clientConfig.pErrCb = (void *)params->pErrCb;
        }
        if (params->pClientEventCb)
        {
            pObj->clientConfig.pClientEventCb = (void *)params->pClientEventCb;
        }
        if (params->pPowerCb)
        {
            pObj->clientConfig.pPowerCb = (void *)params->pPowerCb;
        }

        /* Initialize client state & variables to zero */
        memset((void*)&pObj->state, 0, sizeof(pObj->state));

        /* Initialize client specific semaphore object */
        SemaphoreP_constructBinary(&pObj->state.semSync, 0);

        /* Initialize client specific clock objects */
        ClockP_construct(&pObj->state.clkReqAccess, RF_clkReqAccess, 0, NULL);

        /* Return with the RF handle. */
        return(pObj);
    }
    else
    {
        /* Exit critical section */
        HwiP_restore(key);

        /* Return with null if no more clients are accepted */
        return(NULL);
    }
}

/*
 * ======== RF_close ========
 * Close an RF handle
 */
void RF_close(RF_Handle h)
{
    DebugP_assert(h != NULL);

    /* If there is at least one active client */
    if (RF_numClients)
    {
        /* Wait for all issued commands to finish before freeing the resources */
        if (RF_cmdQ.nSeqPost != RF_cmdQ.nSeqDone)
        {
            /* There are commands which not even dispatched yet. */
            RF_Cmd* pCmd = RF_queueEnd(h, &RF_cmdQ.pPend);

            /* There is no pending commmand, determine if there are items on the
               other queues. */
            if (!pCmd)
            {
                /* If the client is executing a command running. */
                if (RF_isClientOwner(h, RF_cmdQ.pCurrCmdBg))
                {
                    /* The currentlty running command is the last. */
                    pCmd = RF_cmdQ.pCurrCmdBg;
                }
                else
                {
                    /* All commands has been dispatched, some just need to be served. This also
                       can return with NULL if nothing to be done. */
                    pCmd = RF_queueEnd(h, &RF_cmdQ.pDone);
                }
            }

            /* Pend until the running command terminates */
            if (pCmd)
            {
                RF_pendCmd(h, pCmd->ch, RF_TERMINATION_EVENT_MASK);
            }
        }

        /* Enter critical section */
        uint32_t key = HwiP_disable();

        /* Clear the RF_sch client handle */
        if (h == RF_Sch.clientHnd[0])
        {
            RF_Sch.clientHnd[0] = NULL;
        }
        else
        {
            RF_Sch.clientHnd[1] = NULL;
        }

        /* Check whether this is the last client */
        if (--RF_numClients == 0)
        {
            /* If this is the last client, set it to be the active client */
            RF_currClient = h;

            if (RF_core.status == RF_CoreStatusActive)
            {
                /* Release the constraint on the RF resources */
                RF_powerConstraintRelease(RF_PowerConstraintCmdQ);

                /* Exit critical section */
                HwiP_restore(key);

                /* Wait until the radio is powered down (outside critical section) */
                SemaphoreP_pend(&h->state.semSync, SemaphoreP_WAIT_FOREVER);

                /* Enter critical section */
                key = HwiP_disable();
            }

            /* Unregister shared RTOS objects initalized during RF_init by the first client */
            SwiP_destruct(&RF_swiFsmObj);
            HwiP_destruct(&RF_hwiCpe0Obj);
            SwiP_destruct(&RF_swiHwObj);
            HwiP_destruct(&RF_hwiHwObj);
            ClockP_destruct(&RF_clkPowerUpObj);
            ClockP_destruct(&RF_clkInactivityObj);

            /* Unregister temperature notify object if SW TCXO functionality is enabled. */
            if(pfnTemperatureUnregisterNotify != NULL)
            {
                pfnTemperatureUnregisterNotify(&RF_hposcRfCompNotifyObj);
            }

            /* Unregister the wakeup notify callback */
            Power_unregisterNotify(&RF_wakeupNotifyObj);

            /* Release XOSC_HF dependency if it was set on board level. */
            if (RFCC26XX_hwAttrs.xoscHfAlwaysNeeded == true)
            {
                Power_releaseDependency(PowerCC26XX_XOSC_HF);
            }
        }

        /* If we're the current RF client, stop being it */
        if (RF_currClient == h)
        {
            RF_currClient = NULL;
        }

        /* Exit critical section */
        HwiP_restore(key);

        /* Unregister client specific RTOS objects (these are not shared between clients) */
        SemaphoreP_destruct(&h->state.semSync);
        ClockP_destruct(&h->state.clkReqAccess);
    }
}

/*
 * ======== RF_getCurrentTime ========
 * Get current time in RAT ticks
 */
uint32_t RF_getCurrentTime(void)
{
    /* Local variable */
    uint64_t nCurrentTime = 0;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* If radio is active, read the RAT */
    if ((RF_core.status == RF_CoreStatusActive) || (RF_core.status == RF_CoreStatusPhySwitching))
    {
        /* Read the RAT timer through register access */
        nCurrentTime = RF_ratGetValue();

        /* Exit critical section */
        HwiP_restore(key);
    }
    else
    {
        /* Exit critical section */
        HwiP_restore(key);

        /* The radio is inactive, read the RTC instead */
        nCurrentTime  = AONRTCCurrent64BitValueGet();

        /* Conservatively assume that we are just about to increment the RTC
           Scale with the 4 MHz that the RAT is running
           Add the RAT offset for RTC==0 */
        nCurrentTime += RF_RTC_TICK_INC;
        nCurrentTime *= RF_SCALE_RTC_TO_4MHZ;
        nCurrentTime += ((uint64_t)RF_ratSyncCmd.start.rat0) << RF_SHIFT_32_BITS;
        nCurrentTime >>= RF_SHIFT_32_BITS;
    }

    /* Return with the current value */
    return((uint32_t) nCurrentTime);
}

/*
 * ======== RF_postCmd ========
 * Post radio command
 */
RF_CmdHandle RF_postCmd(RF_Handle h, RF_Op* pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent)
{
    /* Assert */
    DebugP_assert(h   != NULL);
    DebugP_assert(pOp != NULL);

    /* Local pointer to a radio commands */
    RF_CmdHandle cmdHandle = (RF_CmdHandle)RF_ALLOC_ERROR;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Try to allocate container */
    RF_Cmd* pCmd = RF_cmdAlloc();

    /* If allocation succeeded */
    if (pCmd)
    {
        /* Stop inactivity clock if running */
        ClockP_stop(&RF_clkInactivityObj);

        /* Increment the sequence number and mask the value */
        RF_cmdQ.nSeqPost = (RF_cmdQ.nSeqPost + 1) & N_CMD_MODMASK;

        /* Populate container with reset values */
        pCmd->pOp          = pOp;
        pCmd->ePri         = ePri;
        pCmd->pCb          = pCb;
        pCmd->ch           = RF_cmdQ.nSeqPost;
        pCmd->pClient      = h;
        pCmd->bmEvent      = (bmEvent | RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M) & ~RF_INTERNAL_IFG_MASK;
        pCmd->pastifg      = 0;
        pCmd->flags        = RF_CMD_ALLOC_FLAG;
        pCmd->endTime      = 0;
        pCmd->endType      = RF_EndNotSpecified;
        pCmd->startTime    = 0;
        pCmd->startType    = RF_StartNotSpecified;
        pCmd->allowDelay   = RF_AllowDelayAny;
        pCmd->duration     = 0;
        pCmd->activityInfo = 0;
        pCmd->coexPriority = RF_PriorityCoexDefault;
        pCmd->coexRequest  = RF_RequestCoexDefault;

        /* Update start time if absolute start time present in radio operation. */
        if (pOp->startTrigger.triggerType == TRIG_ABSTIME)
        {
            pCmd->startType = RF_StartAbs;
            pCmd->startTime = pOp->startTime;
        }

        /* Cancel ongoing yielding */
        h->state.bYielded = false;

        /* Submit to pending command to the queue. */
        List_put(&RF_cmdQ.pPend, (List_Elem*)pCmd);

        /* Trigger dispatcher if the timings need to be reconsidered. */
        if (List_head(&RF_cmdQ.pPend) == (List_Elem*)pCmd)
        {
            RF_dispatchNextEvent();
        }

        /* Return with the command handle as success */
        cmdHandle = pCmd->ch;
    }

    /* Exit critical section */
    HwiP_restore(key);

    /* Return with an error code */
    return(cmdHandle);
}

/*
 *  ==================== RF_ScheduleCmdParams_init ============================
 *  Initialize the parameter structure to be used with RF_scheduleCmd().
 */
void RF_ScheduleCmdParams_init(RF_ScheduleCmdParams *pSchParams)
{
    /* Assert */
    DebugP_assert(pSchParams != NULL);

    /* Set the configuration to use the default values. */
    pSchParams->startTime    = 0;
    pSchParams->startType    = RF_StartNotSpecified;
    pSchParams->allowDelay   = RF_AllowDelayAny;
    pSchParams->endTime      = 0;
    pSchParams->endType      = RF_EndNotSpecified;
    pSchParams->duration     = 0;
    pSchParams->activityInfo = 0;
    pSchParams->coexPriority = RF_PriorityCoexDefault;
    pSchParams->coexRequest  = RF_RequestCoexDefault;
}

/*
 *  ==================== RF_scheduleCmd ============================
 *  Process request to schedule new command from a particular client
 */
RF_CmdHandle RF_scheduleCmd(RF_Handle h, RF_Op* pOp, RF_ScheduleCmdParams *pSchParams, RF_Callback pCb, RF_EventMask bmEvent)
{
    /* Local variable declaration. */
    RF_Cmd*           pCmd;
    RF_Handle         h2;
    RF_ScheduleStatus status;

    /* Assert. */
    DebugP_assert(h   != NULL);
    DebugP_assert(pOp != NULL);

    /* Local pointer to a radio commands. */
    RF_CmdHandle cmdHandle = (RF_CmdHandle)RF_ALLOC_ERROR;

    /* Enter critical section. */
    uint32_t key = HwiP_disable();

    /* Assign h2 to client that is not issuing the new command.
       The client h is issuing the new command. */
    if (h == RF_Sch.clientHnd[0])
    {
        h2 = RF_Sch.clientHnd[1];
    }
    else
    {
        h2 = RF_Sch.clientHnd[0];
    }

    /* If client h2 already has, reject any new commands from h. */
    if (h2 && (ClockP_isActive(&h2->state.clkReqAccess)))
    {
        /* Set the status value to schedule_error if we could not allocate space. */
        cmdHandle = (RF_CmdHandle) RF_ScheduleStatusError;

        /* Store the reason and the handle why the callback is being invoked. */
        RF_Sch.issueRadioFreeCbFlags |= RF_RADIOFREECB_CMDREJECT_FLAG;
        RF_Sch.clientHndRadioFreeCb   = h;
    }
    else
    {
        /* Check if command queue has free entries and allocate RF_Op* container
           if command queue is full reject the command. */
        pCmd = RF_cmdAlloc();

        /* If allocation was successful. */
        if (pCmd)
        {
            /* Stop inactivity clock if running. */
            ClockP_stop(&RF_clkInactivityObj);

            /* Increment the sequence number and mask the value. */
            RF_cmdQ.nSeqPost = (RF_cmdQ.nSeqPost + 1) & N_CMD_MODMASK;

            /* Cache meta-data. */
            pCmd->pOp          = pOp;
            pCmd->ePri         = RF_PriorityNormal;
            pCmd->pCb          = pCb;
            pCmd->ch           = RF_cmdQ.nSeqPost;
            pCmd->pClient      = h;
            pCmd->bmEvent      = bmEvent & ~RF_INTERNAL_IFG_MASK;
            pCmd->flags        = 0;
            pCmd->pastifg      = 0;
            pCmd->endTime      = pSchParams->endTime;
            pCmd->endType      = pSchParams->endType;
            pCmd->startTime    = pSchParams->startTime;
            pCmd->startType    = pSchParams->startType;
            pCmd->allowDelay   = pSchParams->allowDelay;
            pCmd->duration     = pSchParams->duration;
            pCmd->activityInfo = pSchParams->activityInfo;
            pCmd->coexPriority = pSchParams->coexPriority;
            pCmd->coexRequest  = pSchParams->coexRequest;

            /* Update the default endTime based on the scheduling parameters. */
            if (pSchParams->endType == RF_EndNotSpecified)
            {
                if (pSchParams->endTime != 0)
                {
                    pCmd->endType = RF_EndAbs;
                }
            }

            /* Update the default startTime based on the command parameters. */
            if (pSchParams->startType == RF_StartNotSpecified)
            {
                if (pOp->startTrigger.triggerType == TRIG_ABSTIME)
                {
                    pCmd->startType = RF_StartAbs;
                    pCmd->startTime = pOp->startTime;
                }
            }

            /* Find the last radio operation within the chain. */
            RF_Op* pEndOfChain = RF_findEndOfChain(pOp);

            /* Mark the context of the command based on it's ID and subscribe it
               to the expected termination event. */
            if ((pEndOfChain->commandNo & RF_IEEE_ID_MASK) == RF_IEEE_FG_CMD)
            {
                pCmd->flags   |= RF_CMD_FG_CMD_FLAG;
                pCmd->bmEvent |= RFC_DBELL_RFCPEIFG_LAST_FG_COMMAND_DONE_M;
            }
            else
            {
                pCmd->bmEvent |= RFC_DBELL_RFCPEIFG_LAST_COMMAND_DONE_M;
            }

            /* Cancel the radio free callback if new command is from the same client. */
            if ((RF_Sch.clientHndRadioFreeCb == h) &&
                (RF_Sch.issueRadioFreeCbFlags & RF_RADIOFREECB_PREEMPT_FLAG))
            {
                RF_Sch.issueRadioFreeCbFlags &= ~RF_RADIOFREECB_PREEMPT_FLAG;
            }

            /* Invoke the submit policy which shall identify where exactly the new command is being
               inserted based on the application level prioritization table. */
            if (RFCC26XX_schedulerPolicy.submitHook == NULL)
            {
                status = RF_ScheduleStatusError;
            }
            else
            {
                /* Execute the scheduling logic and queue management. */
                status = RFCC26XX_schedulerPolicy.submitHook(pCmd,
                                                             RF_cmdQ.pCurrCmdBg,
                                                             RF_cmdQ.pCurrCmdFg,
                                                             &RF_cmdQ.pPend,
                                                             &RF_cmdQ.pDone);

                /* In case of rescheduling (re-entering the same command), the assigned handle will
                   not match and the counter need to be corrected. */
                if ((status != RF_ScheduleStatusError) && (RF_cmdQ.nSeqPost != pCmd->ch))
                {
                    /* Decrement the sequence number and mask the value. */
                    RF_cmdQ.nSeqPost = (RF_cmdQ.nSeqPost - 1) & N_CMD_MODMASK;
                }
            }

            /* Command was rejected. Either there was no slot available, or the timing did not fit. */
            if ((status == RF_ALLOC_ERROR) || (status == RF_ScheduleStatusError))
            {
                /* Decrement the sequence number and mask the value. */
                RF_cmdQ.nSeqPost = (RF_cmdQ.nSeqPost - 1) & N_CMD_MODMASK;

                /* Store the reason and the handle why the callback is being invoked. */
                RF_Sch.issueRadioFreeCbFlags |= RF_RADIOFREECB_CMDREJECT_FLAG;
                RF_Sch.clientHndRadioFreeCb   = h;

                /* Ensure that the error code reflects the reason of rejection. */
                cmdHandle = (RF_CmdHandle) status;
            }
            else
            {
                /* Command was inserted. Return with the valid handle. */
                cmdHandle = pCmd->ch;

                /* Mark the command as being allocated. */
                pCmd->flags |= RF_CMD_ALLOC_FLAG;

                /* Cancel previous yielding. */
                h->state.bYielded = false;

                /* Trigger dispatcher if the timings need to be reconsidered. */
                if ((List_head(&RF_cmdQ.pPend) == (List_Elem*)pCmd) ||
                    (pCmd->pOp->startTrigger.triggerType == TRIG_ABSTIME))
                {
                    RF_dispatchNextEvent();
                }
            }
        }
    }

    /* Exit critical section. */
    HwiP_restore(key);

    /* Return with the command handle. */
    return(cmdHandle);
}

/*
 * ======== RF_pendCmd ========
 * Pend on radio command
 */
RF_EventMask RF_pendCmd(RF_Handle h, RF_CmdHandle ch, RF_EventMask bmEvent)
{
    /* Assert */
    DebugP_assert(h  != NULL);

    /* If the command handle is invalid (i.e. RF_ALLOC_ERROR) */
    if (ch < 0)
    {
      /* Return with zero means the command was rejected earlier */
      return(0);
    }

    /* Enter critical section */
    uint32_t key = SwiP_disable();

    /* Find the command based on its handle in the command pool */
    RF_Cmd* pCmd = RF_cmdGet(h, ch, RF_CMD_ALLOC_FLAG);

    /* If the command was already disposed */
    if (!pCmd || !(pCmd->flags & RF_CMD_ALLOC_FLAG))
    {
        /* Exit critical section */
        SwiP_restore(key);

        /* Return with last command done event */
        return(RF_EventLastCmdDone);
    }

    /* Expand the pend mask to accept RF_EventLastCmdDone and RF_EventLastFGCmdDone events even if it is not given explicitely */
    bmEvent = (bmEvent | RF_TERMINATION_EVENT_MASK);

    /* If the command is being executed, but the event we pending on has already happend (i.e. in a chain),
       return the past events */
    if (pCmd->pastifg & bmEvent)
    {
        /* Exit critical section */
        SwiP_restore(key);

        /* Store the cause of returning */
        h->state.unpendCause = pCmd->pastifg & bmEvent;

        /* Clear the handled past events so it is possible to pend again */
        pCmd->pastifg &= ~h->state.unpendCause;

        /* Return with the events */
        return(h->state.unpendCause);
    }

    /* Command has still not finished, override user callback with one that calls the user callback then posts to semaphore */
    if (pCmd->pCb != RF_syncCb)
    {
        /* Temporarily store the callback function */
        h->state.pCbSync = (void*)pCmd->pCb;

        /* Exhange the callback function: this will invoke the user callback and post to the semaphore if needed */
        pCmd->pCb = RF_syncCb;
    }

    /* Store the event subscriptions in the clients context. This can only be one of the already enabled
       interrupt sources by RF_postCmd (including RF_EventLastCmdDone) */
    h->state.eventSync = bmEvent;

    /* Exit critical section */
    SwiP_restore(key);

    /* Wait for semaphore */
    SemaphoreP_pend(&h->state.semSync, SemaphoreP_WAIT_FOREVER);

    /* Return the events that resulted in releasing the RF_pend() call */
    return(h->state.unpendCause);
}

/*
 *  ======== RF_runCmd ========
 *  Run to completion a posted command
 */
RF_EventMask RF_runCmd(RF_Handle h, RF_Op* pOp, RF_Priority ePri, RF_Callback pCb, RF_EventMask bmEvent)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Post the requested command */
    RF_CmdHandle ch = RF_postCmd(h, pOp, ePri, pCb, bmEvent);

    /* If the command was accepted, pend until one of the special events occur */
    return(RF_pendCmd(h, ch, RF_TERMINATION_EVENT_MASK));
}

/*
 *  ======== RF_runScheduleCmd ========
 *  Run to completion a scheduled command
 */
RF_EventMask RF_runScheduleCmd(RF_Handle h, RF_Op* pOp, RF_ScheduleCmdParams *pSchParams, RF_Callback pCb, RF_EventMask bmEvent)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Post the requested command */
    RF_CmdHandle ch = RF_scheduleCmd(h, pOp, pSchParams, pCb, bmEvent);

    /* If the command was accepted, pend until one of the special events occur */
    return(RF_pendCmd(h, ch, RF_TERMINATION_EVENT_MASK));
}

/*
 *  ======== RF_yieldCmd ========
 *  Release client access
 */
void RF_yield(RF_Handle h)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Request the synchronization of RTC and RAT at next power down. This is trigged
       by ceiling the active time to the maximum value. */
    RF_core.activeTimeUs = UINT32_MAX;

    /* Stop ongoing request access and issue callback if the radio is off */
    ClockP_stop((&h->state.clkReqAccess));

    /* If all commands are done */
    if (RF_cmdQ.nSeqDone == RF_cmdQ.nSeqPost)
    {
        if ((RF_core.status != RF_CoreStatusActive) && RF_Sch.issueRadioFreeCbFlags)
        {
            /* Exit critical section. */
            HwiP_restore(key);

            /* Invoke the radio free callback provided by the user. */
            RF_issueRadioFreeCb(RF_RADIOFREECB_REQACCESS_FLAG |
                                RF_RADIOFREECB_PREEMPT_FLAG   |
                                RF_RADIOFREECB_CMDREJECT_FLAG);

            /* Enter critical section. */
            key = HwiP_disable();
        }
    }

    /* If the radioFreeCb did not post new commands. */
    if (RF_cmdQ.nSeqDone == RF_cmdQ.nSeqPost)
    {
        /* All commands are done. Stop inactivity timer. */
        ClockP_stop(&RF_clkInactivityObj);

        /* Potentially power down the RF core. */
        RF_powerConstraintRelease(RF_PowerConstraintCmdQ);
    }
    else
    {
        /* There are still client commands that haven't finished.
           Set flag to indicate immediate powerdown when last command is done. */
        h->state.bYielded = true;
    }

    /* Exit critical section */
    HwiP_restore(key);
}

/*
 *  ======== RF_cancelCmd ========
 *  Cancel single radio command
 */
RF_Stat RF_cancelCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode)
{
    /* Assert  */
    DebugP_assert(h != NULL);

    /* Decode what method to be used for terminating the commands. */
    bool graceful = (bool)(mode & RF_ABORT_GRACEFULLY);
    bool flush    = (bool)(mode & RF_ABORT_FLUSH_ALL);
    bool preempt  = (bool)(mode & RF_ABORT_PREEMPTION);

    /* Invoke the aborting process with the input arguments on a single command */
    return(RF_abortCmd(h, ch, graceful, flush, preempt));
}

/*
 *  ======== RF_flushCmd ========
 *  Cancel multiple radio commands from a client
 */
RF_Stat RF_flushCmd(RF_Handle h, RF_CmdHandle ch, uint8_t mode)
{
    /* Assert  */
    DebugP_assert(h != NULL);

    /* Decode what method to be used for terminating the commands. */
    bool graceful = (bool)(mode & RF_ABORT_GRACEFULLY);
    bool flush    = true;
    bool preempt  = (bool)(mode & RF_ABORT_PREEMPTION);

    /* Abort multiple radio commands implicitly */
    return(RF_abortCmd(h, ch, graceful, flush, preempt));
}

/*
 *  ======== RF_Params_init ========
 *  Initialize the RF_params to default value
 */
void RF_Params_init(RF_Params *params)
{
    /* Assert */
    DebugP_assert(params != NULL);

    /* Assign default values for RF_params */
    *params = RF_defaultParams;
}

/*
 *  ======== RF_runImmediateCmd ========
 *  Run immediate command
 */
RF_Stat RF_runImmediateCmd(RF_Handle h, uint32_t* pCmd)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Submit the command to the RF core */
    return(RF_runDirectImmediateCmd(h, (uint32_t)pCmd, NULL));
}

/*
 *  ======== RF_runDirectCmd ========
 *  Run direct command
 */
RF_Stat RF_runDirectCmd(RF_Handle h, uint32_t cmd)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Submit the command to the RF core */
    return(RF_runDirectImmediateCmd(h, cmd, NULL));
}

/*
 *  ======== RF_getRssi ========
 *  Get RSSI value
 */
int8_t RF_getRssi(RF_Handle h)
{
    /* Assert  */
    DebugP_assert(h != NULL);

    /* Local variable. */
    uint32_t rawRssi;

    /* Read the RSSI value if possible. */
    RF_Stat status = RF_runDirectImmediateCmd(h, CMDR_DIR_CMD(CMD_GET_RSSI), &rawRssi);

    /* Decode the RSSI value if possible. */
    if (status == RF_StatCmdDoneSuccess)
    {
        return((int8_t)((rawRssi >> RF_SHIFT_16_BITS) & RF_CMDSTA_REG_VAL_MASK));
    }
    else
    {
        return((int8_t)RF_GET_RSSI_ERROR_VAL);
    }
}

/*
 *  ======== RF_getInfo ========
 *  Get RF driver info
 */
RF_Stat RF_getInfo(RF_Handle h, RF_InfoType type, RF_InfoVal *pValue)
{
    /* Prepare the default status value */
    RF_Stat status = RF_StatSuccess;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Serve the different flavor of requests */
    switch (type)
    {
        case RF_GET_CURR_CMD:
            /* Get the handle of the currently running command. It can be conerted
               to a pointer through the RF_getCmdOp() API. */
            if (RF_cmdQ.pCurrCmdBg)
            {
                pValue->ch = RF_cmdQ.pCurrCmdBg->ch;
            }
            else
            {
                status = RF_StatError;
            }
            break;

        case RF_GET_AVAIL_RAT_CH:
            /* Get available user channels within the RAT timer.
               These channels can be allocated and used by the application. */
            pValue->availRatCh = RF_ratModule.availableRatChannels;
            break;

        case RF_GET_RADIO_STATE:
            /* Get current radio state */
            pValue->bRadioState = (RF_core.status == RF_CoreStatusActive) ? true : false;
            break;

        case RF_GET_CLIENT_LIST:
            /* Copy the client pointer list ([0] -> client 1, [1] -> client 2) */
            pValue->pClientList[0] = RF_Sch.clientHnd[0];
            pValue->pClientList[1] = RF_Sch.clientHnd[1];
            break;

        case RF_GET_CLIENT_SWITCHING_TIME:
            /* Copy the phy switching times to the RF_InfoVal structure */
            pValue->phySwitchingTimeInUs[0] = RF_Sch.clientHnd[0] ? RF_Sch.clientHnd[0]->clientConfig.nPhySwitchingDuration : 0;
            pValue->phySwitchingTimeInUs[1] = RF_Sch.clientHnd[1] ? RF_Sch.clientHnd[1]->clientConfig.nPhySwitchingDuration : 0;
            break;

        default:
            status = RF_StatInvalidParamsError;
            break;
    }

    /* Exit critical section */
    HwiP_restore(key);

    /* Return with a status code */
    return(status);
}

/*
 *  ======== RF_getCmdOp ========
 *  Get RF command
 */
RF_Op* RF_getCmdOp(RF_Handle h, RF_CmdHandle ch)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Find the command in the command pool based on its handle */
    RF_Cmd* pCmd = RF_cmdGet(h, ch, RF_CMD_ALLOC_FLAG);

    /* If the command is found */
    if (pCmd)
    {
        /* Return with the first operation in the command */
        return(pCmd->pOp);
    }
    else
    {
        /* Return with null in case of error */
        return(NULL);
    }
}

/*
 *  ======== RF_RatConfigCompare_init ========
 *  Initialize RAT compare configuration
 */
void RF_RatConfigCompare_init(RF_RatConfigCompare* channelConfig)
{
    /* Assert */
    DebugP_assert(channelConfig != NULL);

    /* Set the values to default. */
    memset((void*)channelConfig, 0, sizeof(RF_RatConfigCompare));

    /* Set the default allocation method to use any channel. */
    channelConfig->channel = RF_RatChannelAny;
}

/*
 *  ======== RF_RatConfigCapture_init ========
 *  Initialize RAT capture configuration
 */
void RF_RatConfigCapture_init(RF_RatConfigCapture* channelConfig)
{
    /* Assert */
    DebugP_assert(channelConfig != NULL);

    /* Set the values to default. */
    memset((void*)channelConfig, 0, sizeof(RF_RatConfigCapture));

    /* Set the default allocation method to use any channel. */
    channelConfig->channel = RF_RatChannelAny;
}

/*
 *  ======== RF_RatConfigOutput_init ========
 *  Initialize RAT IO configuration
 */
void RF_RatConfigOutput_init(RF_RatConfigOutput* ioConfig)
{
    /* Assert */
    DebugP_assert(ioConfig != NULL);

    /* Set the values to default. */
    memset((void*)ioConfig, 0, sizeof(RF_RatConfigOutput));
}

/*
 *  ======== RF_ratCompare ========
 *  Set RAT compare
 */
RF_RatHandle RF_ratCompare(RF_Handle rfHandle, RF_RatConfigCompare* channelConfig, RF_RatConfigOutput* ioConfig)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Configure the RAT channel into COMPARE mode. */
    return(RF_ratSetupChannel(rfHandle, RF_RatModeCompare, channelConfig->callback, channelConfig->channel, (void*) channelConfig, ioConfig));
}

/*
 *  ======== RF_ratCapture ========
 *  Set RAT capture
 */
RF_RatHandle RF_ratCapture(RF_Handle rfHandle, RF_RatConfigCapture* channelConfig, RF_RatConfigOutput* ioConfig)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Configure the RAT channel into CAPTURE mode. */
    return(RF_ratSetupChannel(rfHandle, RF_RatModeCapture, channelConfig->callback, channelConfig->channel, (void*) channelConfig, ioConfig));
}

/*
 *  ======== RF_ratDisableChannel ========
 *  Disable RAT channel
 */
RF_Stat RF_ratDisableChannel(RF_Handle h, RF_RatHandle ratHandle)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Default return value */
    RF_Stat status = RF_StatError;

    /* Enter critical section. */
    uint32_t key = HwiP_disable();

    /* Find the pointer to the RAT channel configuration. */
    RF_RatChannel* ratCh = RF_ratGetChannel(ratHandle);

    /* If the provided handler is valid. */
    if (ratCh && ratCh->status && (ratCh->pClient == h))
    {
        /* If the RF core is active, abort the RAT event. */
        if (RF_core.status == RF_CoreStatusActive)
        {
            /* Calculate the configuration field of command (the channel we disable). */
            uint16_t config = (uint16_t)(RF_RAT_CH_LOWEST + ratCh->handle) << RF_SHIFT_8_BITS;

            /* Disable the channel within the RF core.
               It has been checked that RAT channel to be disabled is owned by the input handle h.
               Call the function that executes the disabling with RF_currClient as input argument in
               instead of h in order to force the function to accept the disable request. This is
               required in the case where the client that will disable a RAT channel is not the same
               client as currently controlling the radio.
            */
            status = RF_runDirectImmediateCmd(RF_currClient, ((uint32_t)CMDR_DIR_CMD_2BYTE(CMD_DISABLE_RAT_CH, config)), NULL);

            /* Free the container for further use. We do it after the direct command to be sure it is not powered down.
               This will implicitely schedule the next event and run the power management accordingly. */
            RF_ratFreeChannel(ratCh);
        }
        else
        {
            /* Set status to be successful. */
            status = RF_StatCmdDoneSuccess;

            /* Free the container for further use. If possible, power down the radio. */
            RF_ratFreeChannel(ratCh);

            /* Recalculate the next wakeup event if the radio was off. */
            RF_dispatchNextEvent();
        }
    }

    /* Exit critical section. */
    HwiP_restore(key);

    /* Return with the status code */
    return(status);
}

/*
 *  ======== RF_control ========
 *  RF control
 */
RF_Stat RF_control(RF_Handle h, int8_t ctrl, void *args)
{
    /* Assert */
    DebugP_assert(h != NULL);

    /* Prepare the return value for worst case scenario */
    RF_Stat status = RF_StatSuccess;

    /* Enter critical section */
    uint32_t key = HwiP_disable();

    /* Serve the different requests */
    switch (ctrl)
    {
        case RF_CTRL_SET_INACTIVITY_TIMEOUT:
            /* Update the inactivity timeout of the client.
               This can be used if the value given at RF_open
               need to be updated */
            h->clientConfig.nInactivityTimeout = *(uint32_t *)args;
            break;

        case RF_CTRL_UPDATE_SETUP_CMD:
            /* Enable a special boot process which can be controlled
               through the config field of the radio setup command.
               This will influence only the next power up sequence
               and will be reset automatically afterwards. The special
               power up process will require longer power up time, hence
               the nPowerUpDuration need to be increased */
            h->clientConfig.bUpdateSetup      = true;
            h->clientConfig.nPowerUpDuration += RF_ANALOG_CFG_TIME_US;
            h->clientConfig.nPowerUpDurationFs += RF_ANALOG_CFG_TIME_US;
            break;

        case RF_CTRL_SET_POWERUP_DURATION_MARGIN:
            /* Configure the margin which is added to the measured
               nPowerUpDuration. This can ensure that the commands
               are executed on time, depending on the load of the
               cpu */
            h->clientConfig.nPowerUpDurationMargin = *(uint32_t *)args;
            break;

        case RF_CTRL_SET_PHYSWITCHING_DURATION_MARGIN:
            /* Configure the margin which is added to the measured
               nPowerUpDuration. This can ensure that the commands
               are executed on time, depending on the load of the
               cpu */
            h->clientConfig.nPhySwitchingDurationMargin = *(uint32_t *)args;
            break;

        case RF_CTRL_SET_RAT_RTC_ERR_TOL_VAL:
            /* Configure the tolerance value which is used to determine
               the period when the RAT need to be syncronized to the RTC
               due to the frequency offset */
            RF_errTolValInUs = *(uint32_t*)args;
            break;

        case RF_CTRL_SET_POWER_MGMT:
            /* The RF drivers power management can be enabled/disabled by
               directly setting the power constraints from the application.
               It is important that the order of actions align. */
            if (*(uint32_t*)args == 0)
            {
                RF_powerConstraintSet(RF_PowerConstraintDisallow);
            }
            else if (*(uint32_t*)args == 1)
            {
                RF_powerConstraintRelease(RF_PowerConstraintDisallow);
            }
            else
            {
                status = RF_StatInvalidParamsError;
            }
            break;

        case RF_CTRL_SET_HWI_PRIORITY:
            /* Changing priorities during run-time has constraints.
               To not mess up with the RF driver, we require the RF
               driver to be inactive. */
            if (RF_core.status || (List_head(&RF_cmdQ.pPend)))
            {
                status = RF_StatBusyError;
            }
            else
            {
                HwiP_setPriority(INT_RFC_CPE_0,   *(uint32_t *)args);
                HwiP_setPriority(INT_RFC_HW_COMB, *(uint32_t *)args);
            }
            break;

        case RF_CTRL_SET_SWI_PRIORITY:
            /* Changing priorities during run-time has constraints.
               To not mess up with the RF driver, we require the RF
               driver to be inactive. */
            if (RF_core.status || (List_head(&RF_cmdQ.pPend)))
            {
                status = RF_StatBusyError;
            }
            else
            {
                SwiP_setPriority(&RF_swiFsmObj, *(uint32_t *)args);
                SwiP_setPriority(&RF_swiHwObj,  *(uint32_t *)args);
            }
            break;

        case RF_CTRL_SET_AVAILABLE_RAT_CHANNELS_MASK:
            /* Mask the available RAT channels manually. This can be used when
               a particular RAT channel is used through oridnary radio operations
               instead of the dedicated RAT APIs. */
            RF_ratModule.availableRatChannels = *(uint8_t *)args;
            break;

        case RF_CTRL_COEX_CONTROL:
            /* Pass this request on to the dynamically generated board file
               event handler */
            RF_invokeGlobalCallback(RF_GlobalEventCoexControl, args);
            break;

        default:
            /* Request can not be served */
            status = RF_StatInvalidParamsError;
            break;
    }

    /* Exit critical section */
    HwiP_restore(key);

    /* Return with the status code */
    return(status);
}

/*
 *  ======== RF_requestAccess ========
 *  RF request access
 */
RF_Stat RF_requestAccess(RF_Handle h, RF_AccessParams *pParams)
{
    /* Assert. */
    DebugP_assert(h       != NULL);
    DebugP_assert(pParams != NULL);

    /* By default, the status is set to busy. */
    RF_Stat status = RF_StatBusyError;

    /* Convert the requested duration to us. */
    uint32_t durationInUs = RF_convertRatTicksToUs(pParams->duration);

    /* Check if the requested period is within the acceptable range. */
    if (durationInUs > RF_REQ_ACCESS_MAX_DUR_US)
    {
        /* Reject the request if not. */
        status = RF_StatInvalidParamsError;
    }

    /* Enter critical section. */
    uint32_t key = HwiP_disable();

    /* Determine the ID of the requesting client. */
    uint8_t clientIdx = 0;
    if (h == RF_Sch.clientHnd[1])
    {
        clientIdx = 1;
    }

    /* Get handle to the other client. */
    RF_Handle h2 = RF_Sch.clientHnd[clientIdx ^ 0x1];

    /* Check if the radio is free and if request can be served.
       If possible update the RF_Sch structure and start the timer (RTC)
       for the request access duration, else, return RF_StatBusyError. */
    if (!(h && ClockP_isActive(&h->state.clkReqAccess)) &&
        !(h2 && ClockP_isActive(&h2->state.clkReqAccess)))
    {
        /* Update the scheduler. */
        RF_Sch.accReq[clientIdx].duration = pParams->duration;
        RF_Sch.accReq[clientIdx].priority = pParams->priority;

        /* Start timeout of the request. */
        RF_restartClockTimeout(&h->state.clkReqAccess, durationInUs/ClockP_getSystemTickPeriod());

        /* Set status to success after the access was granted. */
        status = RF_StatSuccess;
    }
    else
    {
        /* In case the request can not be served, prepare for a notification
           callback when the radio becomes available. */
        RF_Sch.issueRadioFreeCbFlags |= RF_RADIOFREECB_REQACCESS_FLAG;
        RF_Sch.clientHndRadioFreeCb   = h;
    }

    /* Exit critical section. */
    HwiP_restore(key);

    /* Return the status. */
    return(status);
}

/*
 *  ======== RF_setTxPower ========
 *  Set the TX power of the client
 */
RF_Stat RF_setTxPower(RF_Handle handle, RF_TxPowerTable_Value value)
{
    /* Local variable stores the return value. */
    RF_Stat status;

    /* Placeholder of the command to be used to update the PA configuration within the RF core immediately. */
    RF_ConfigurePaCmd configurePaCmd;

    /* Update the setup command to make the changes permanent. */
    status = RF_updatePaConfiguration(handle->clientConfig.pRadioSetup, value, &configurePaCmd);

    /* If we managed to decode and cache the changes in the setup command. */
    if (status == RF_StatSuccess)
    {
        /* Execute the necessary command to apply the changes. It only takes effect if the RF core
           is active and we configure the current client. The IO configuration can be re-evaluated when
           the RF core issues the PA_CHANGED interrupt. */
        RF_runDirectImmediateCmd(handle, (uint32_t)&configurePaCmd, NULL);
    }

    /* Return with the status. */
    return(status);
}

/*
 *  ======== RF_getTxPower ========
 *  Get the current TX power value
 */
RF_TxPowerTable_Value RF_getTxPower(RF_Handle handle)
{
    /* Default return value. */
    RF_TxPowerTable_Value value = { .rawValue = RF_TxPowerTable_INVALID_VALUE,
                                    .paType   = RF_TxPowerTable_DefaultPA};

    /* Local variables. */
    uint16_t* pTxPower          = NULL;
    uint32_t* pRegOverride      = NULL;
    uint32_t* pRegOverrideTxStd = NULL;
    uint32_t* pRegOverrideTx20  = NULL;

    /* Decode if High Gain PA is available. */
    bool tx20FeatureAvailable = RF_decodeOverridePointers(handle->clientConfig.pRadioSetup, &pTxPower, &pRegOverride, &pRegOverrideTxStd, &pRegOverrideTx20);

    /* Continue the search for the poper value if the High PA is used. */
    if (*pTxPower == RF_TX20_ENABLED)
    {
        /* Local variable. */
        uint32_t rawValue;

        /* Returning the High Gain PA gain is only possible if the P device is in use. */
        if (tx20FeatureAvailable && pRegOverrideTxStd && pRegOverrideTx20)
        {
            if (RF_getPAOverrideOffsetAndValue(pRegOverrideTx20, RF_TX20_PATTERN, &rawValue) != RF_TX_OVERRIDE_INVALID_OFFSET)
            {
                /* Return the value found in the gain related list. */
                value.rawValue = rawValue;
                value.paType   = RF_TxPowerTable_HighPA;
            }
        }
        else if (tx20FeatureAvailable)
        {
            if (RF_getPAOverrideOffsetAndValue(pRegOverride, RF_TX20_PATTERN, &rawValue) != RF_TX_OVERRIDE_INVALID_OFFSET)
            {
                /* As a backup option, parse the common list too. This is or backward compatibility
                   and new software shall not rely on this feature. */
                value.rawValue = rawValue;
                value.paType   = RF_TxPowerTable_HighPA;
            }
        }
    }
    else
    {
        /* The value in the .txPower field represents the output power.*/
        value.rawValue = *pTxPower;
    }

    /* Return with the decoded value. */
    return(value);
}

/*
 *  ======== RF_TxPowerTable_findPowerLevel ========
 *  Retrieves a power level in dBm for a given power configuration value.
 */
int8_t RF_TxPowerTable_findPowerLevel(RF_TxPowerTable_Entry table[], RF_TxPowerTable_Value value)
{
    /* Iterate through the power table. We do not verify against nullptr. */
    uint32_t i;
    for (i=0; (table[i].power != RF_TxPowerTable_INVALID_DBM) &&
              (table[i].value.rawValue != RF_TxPowerTable_INVALID_VALUE); i++)
    {
        if (((uint32_t)table[i].value.paType   == (uint32_t)value.paType) &&
           ((uint32_t)table[i].value.rawValue == (uint32_t)value.rawValue))
        {
            /* Break the loop on the first entry which satisfies
               the lower-or-equal criterion toward the input argument. */
            break;
        }
    }

    /* Return with the power level in dBm or with the
       termination value RF_TxPowerTable_INVALID_DBM. */
    return(table[i].power);
}

/*
 *  ======== RF_TxPowerTable_findValue ========
 * Retrieves a power configuration value for a given power level in dBm.
 */
RF_TxPowerTable_Value RF_TxPowerTable_findValue(RF_TxPowerTable_Entry table[], int8_t powerLevel)
{
    /* Local variable stores an invalid value. */
    RF_TxPowerTable_Value invalidValue =  { .rawValue = RF_TxPowerTable_INVALID_VALUE,
                                            .paType   = RF_TxPowerTable_DefaultPA };

    /* Handle special input argument. */
    if (powerLevel == RF_TxPowerTable_MIN_DBM)
    {
        return(table[0].value);
    }
    else
    {
        /* Iterate through the power table. We do not verify against nullptr. */
        uint32_t i;
        for (i=0; ((int8_t)table[i].power != (int8_t)RF_TxPowerTable_INVALID_DBM) &&
                  ((uint32_t)table[i].value.rawValue != (uint32_t)RF_TxPowerTable_INVALID_VALUE); i++)
        {
            if (table[i].power > powerLevel)
            {
                /* Break the loop on the first entry which satisfies
                   the lower-or-equal criterion toward the input argument. */
                break;
            }
        }

        if (i == 0)
        {
            /* If the first entry is already larger, then the requested
               power level is invalid. */
            return(invalidValue);
        }
        else
        {
            /* Return with a valid RF_TxPowerTable_Value or with the
               maximum value in the table. */
            return(table[i-1].value);
        }
    }
}

/*
 *  ======== RF_enableHPOSCTemperatureCompensation ========
 * Initializes the temperature compensation monitoring (SW TCXO)
 * This function enables RF synthesizer temperature compensation
 * It is intended for use on the SIP or BAW devices where compensation
 * coefficients are available inside the chip.
 *
 * The name of the function is a misnomer, as it does not only apply to
 * HPOSC (BAW) configuration, but will generically enable SW TCXO.
 */
RF_Stat RF_enableHPOSCTemperatureCompensation(void)
{
    int_fast16_t status;

    Temperature_init();

    int16_t currentTemperature = Temperature_getTemperature();

    status = Temperature_registerNotifyRange(&RF_hposcRfCompNotifyObj,
                                                currentTemperature + RF_TEMP_LIMIT_3_DEGREES_CELSIUS,
                                                currentTemperature - RF_TEMP_LIMIT_3_DEGREES_CELSIUS,
                                                RF_hposcRfCompensateFxn,
                                                (uintptr_t)NULL);

    pfnUpdateHposcOverride = &RF_updateHpOscOverride;
    pfnTemperatureUnregisterNotify = &Temperature_unregisterNotify;

    if (status != Temperature_STATUS_SUCCESS)
    {
        return(RF_StatInvalidParamsError);
    }
    return(RF_StatSuccess);
}
