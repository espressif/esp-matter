
#ifndef BLE_MGR_H
#define BLE_MGR_H

/*****************************************************************************
 * GENERAL
 *****************************************************************************/

/*! Macro to convert an address to the GPMicro processor's address space.
 * @param addr : input address, may be compressed or uncompressed address.
 * @return corresponding address in the GPMicro address space
 */
#define TO_GPM_ADDR(addr) ( ((addr>=GP_MM_WISHB_START)          && (addr<(GP_MM_WISHB_START+0x2000)))           ? (0x0000+(addr-GP_MM_WISHB_START)) : \
                            ((addr>=GP_MM_RAM_PBM_OPTS_START)   && (addr<(GP_MM_RAM_PBM_OPTS_START+0x2000)))    ? (0x2000+(addr-GP_MM_RAM_PBM_OPTS_START)) : \
                            ((addr>=GP_MM_RAM_PBM_0_DATA_START) && (addr<(GP_MM_RAM_PBM_0_DATA_START+0x4000)))  ? (0x4000+(addr-GP_MM_RAM_PBM_0_DATA_START)) : \
                            ((addr>=GP_MM_RAM_LINEAR_START)     && (addr<(GP_MM_RAM_LINEAR_START+0x4000)))      ? (0x8000+(addr-GP_MM_RAM_LINEAR_START)) : \
                                                                                                                  (0x0000) )

/*****************************************************************************
 * BLE COMMAND interface
 *****************************************************************************/

/*! Address of the command type register
 *  \n\n
 *  The BLE COMMAND interface is an Inter-Processor Interface between the BLE_MGR
 *  and an other processor. The interface allows other processors to trigger actions
 *  on the Real-Time processor. The sequence for issuing a command is as follows:
 *      - Write the command type to #BLE_MGR_CMD_REG_ADDRESS. For available commands, see
 *        the BLE_MGR_CMD defines.
 *      - Write command arguments to #BLE_MGR_CMD_ARGS_START_ADDRESS. The contents of the
 *        arguments is dependent of the command type.
 *      - Trigger the command interrupt to the RT processor by writing IPC_SET_X2GPM_CMD_INTERRUPT
 *      - The IPC_UNMASKED_GPM2X_CMD_PROCESSED_INTERRUPT is asserted when the RT processor has
 *        finished running the command.
 *      - The status of the ran command is found @ #BLE_MGR_CMD_RESULT_REG_ADDRESS. If the contents of
 *        this register is equal to the BLE_MGR_CMD type number, it indicates success. If otherwise,
 *        an error has occurred during the command execution.
 *
 *  \n\n
 *  \note
 *      Only 1 command can be run at a time. The NRT software must wait for completion before initiating a
 *      new command.
 */
#define BLE_MGR_CMD_REG_ADDRESS         (GP_MM_RAM_GPMICRO_RAM_END - 0x20 + 0)
#define BLE_MGR_CMD_RESULT_REG_ADDRESS  (GP_MM_RAM_GPMICRO_RAM_END - 0x20 + 2)
#define BLE_MGR_CMD_ARGS_START_ADDRESS  (GP_MM_RAM_GPMICRO_RAM_END - 0x20 + 4)

/*! BLE_MGR_CMD to initialize the BLE manager:
 *  -   enables the processing of BLE event interrupts
 *  -   initializes the internal state 
 *  -   connects the necessary hardware interrupts to the RT processor
 * @param None
 */
#define BLE_MGR_CMD_INIT_BLE_MGR 1

/*! BLE_MGR_CMD to disable the BLE manager:
 *  -   disables the processing of BLE event interrupts
 *  -   disconnects the hardware interrupts from the RT processor
 * @param None
 */
#define BLE_MGR_CMD_DISABLE_BLE_MGR 2

/*! BLE_MGR_CMD to start a BLE event.
 *  @param ble_mgr_start_event_args_t structure that describes the event to be started
 */
#define BLE_MGR_CMD_START_EVENT 3

// it is very important that this structure is aligned on 4 bytes!!
typedef PACKED_PRE struct ble_mgr_start_event_args_t
{
    UInt8    event_nr;      //!< Number of the ES HW event to be used for this BLE event
    UInt8    event_type;    //!< Type of the BLE event. Same format as EVENT_TYPE
    UInt16   info_ptr;      //!< Pointer to the info structure for that BLE event
                            /*!< \note Value of this pointer must be in the GPMicro address space.
                             *  Use #TO_GPM_ADDR macro to convert.
                             */
    UInt32   schedule_time; //!< First execution time to be scheduled for the BLE event. in microseconds
} PACKED_POST ble_mgr_start_event_args_t;

/*! BLE_MGR_CMD to stop an ongoing event, and disable it.
 * If the event was not ongoing (currently executed), it will only be disabled.
 * @param event_nr (UInt8) : the ES event number of the event to be stopped.
 */
#define BLE_MGR_CMD_STOP_EVENT 4

/*! BLE_MGR_CMD to wake up an event in case it went to sleep due to slave latency.
 * Sleeping events can only happen on connection slave events with slave latency 
 * and no_wakeup feature enabled.
 * @param event_nr (UInt8) : the ES event number of the event to be woken up.
 */
#define BLE_MGR_CMD_WAKEUP_EVENT 5

/*! BLE_MGR_CMD to start the BLE direct test mode. This can only be done when there are no BLE
 * event scheduled or active.
 * @param info_ptr (UInt16) : pointer in the GPMicro address space to the ble_test_info structure
 * \note This command can return unsuccessful when the RT processor was unable to start the
 * direct test mode.
 */
#define BLE_MGR_START_DIRECT_TEST_MODE 6

/*! BLE_MGR_CMD to stop the BLE direct test mode
 * @param None
 */
#define BLE_MGR_STOP_DIRECT_TEST_MODE 7

/*! BLE_MGR_CMD to halt the GPMicro from the moment the RT system is IDLE.
 * so halts the processor in a clean way so internal state is not corrupted.
 * This command is completed immediately, but the RT processor is only stopped when the 
 * STANDBY_RESET_GPMICRO asserted. 
 * BLE_MGR_SHUTDOWN_DURING_CLEANUP has impact on the behavior of this command.
 * @param None
 */
#define BLE_MGR_HALT_GPMICRO 8

/*!
 * calibrate the channel independant FLL parameters (DTC, coarse, fine_gain) 
 * @param None
 */
#define BLE_MGR_FLL_CAL_NRT 9

/*! BLE_MGR_CMD to schedule a SubEvDsc. RT will add the de SED into the time-sorted PDL
 * @param sed_idx (UInt8): index of the SED to be scheduled.
 */
#define BLE_MGR_SCHEDULE_SED 10

/*! BLE_MGR_CMD to release a SubEvDsc claimed by RT
 * @param sed_idx (UInt8): index of the SED to be released. should be an index claimed
 * by RT. (ie sed_idx is in BLE_MGR_SUBEV_DSC_ENTRY_RT_MASK )
 */
#define BLE_MGR_RELEASE_RT_SED 11

/*! BLE_MGR_CMD to enable/disable the closed loop tx power measurements in RT
 * @param enable (Bool): Enable or disable the tx power measurements
 */
#define BLE_MGR_ENABLE_TX_POWER_MEASUREMENTS  12

/*! BLE_MGR_CMD that does nothing, handy if we just want to wake up the RT processor to service the main loop once
 * @param None
 */
#define BLE_MGR_CMD_DUMMY 99

/*****************************************************************************
 * UNEXPECTED CONDITION interface 
 *****************************************************************************/

/*! Address of the unexpected condition register.
 * \n\n
 * The unexpected condition interface is an unidirectional interface from the GPMicro
 * to NRT Software. It is used to signal unexpected conditions in the RT software.
 * The GPMicro asserts the IPC_UNMASKED_GPM2X_UNEXPECTED_COND_INTERUPT when such a situation
 * occurs. The type of condition is at #BLE_MGR_UNEXPECTED_COND_REG_ADDRESS, and the 
 * #BLE_MGR_UNEXPECTED_COND_ARG0_REG_ADDRESS register contains additional info. (which is
 * dependent on the type of condition)
*  \note
 *      This is an one hit interface. As long as the IPC_UNMASKED_GPM2X_UNEXPECTED_COND_INTERUPT is asserted,
 *      the RT processor will not generate new messages. The NRT software is responsible for clearing
 *      the interrupt.
 */
#define BLE_MGR_UNEXPECTED_COND_REG_ADDRESS         (GP_MM_RAM_GPMICRO_RAM_START)
#define BLE_MGR_UNEXPECTED_COND_ARG0_REG_ADDRESS    (GP_MM_RAM_GPMICRO_RAM_START+1)

/*! BLE_MGR_UNEXPECTED_COND type to indicate a PARBLE_PACKET_DONE interrupt is received
 * while the RT system was not in an active BLE state.
 * @param rx_pbm_nr (UInt8) : number of the BLE PBM that was received in the non-active state.
 * The NRT software is responsible for releasing this PBM and handling this error.
 */
#define BLE_MGR_UNEXPECTED_COND_PACKET_DONE 1

/*! BLE_MGR_UNEXPECTED_COND type to indicate a BLE RIB_RX_WD_DONE interrupt is received
 * while the RT system was not in an active BLE state.
 * @param None
 */
#define BLE_MGR_UNEXPECTED_COND_RX_WD_DONE  2


/*****************************************************************************
 * STATUS interface
 *****************************************************************************/

/*! Address of the status type register address
 *  \n\n
 *  The STATUS interface is a general information output channel from RT processor.
 *  It is used to send out internal state/debug information.
 *  When there is a info message the IPC_UNMASKED_GPM2X_COMM_INTERRUPT is asserted.
 *  Upon detection of this interrupt, the NRT software can read out the type and information message
 *  from #BLE_MGR_STAT_TYPE_REG_ADDRESS and #BLE_MGR_STAT_INFO_REG_ADDRESS.
 *  The structure of the information message depends on the BLE_MGR_STAT_TYPE.
 *  \note
 *      This is an one hit interface. As long as the IPC_UNMASKED_GPM2X_STAT_INTERRUPT is asserted,
 *      the RT processor will not generate new messages. The NRT software is responsible for clearing
 *      the interrupt.
 */
#define BLE_MGR_STAT_TYPE_REG_ADDRESS    (GP_MM_RAM_GPMICRO_RAM_START+0x4)
//! Start address of the status info message 
#define BLE_MGR_STAT_INFO_REG_ADDRESS    (GP_MM_RAM_GPMICRO_RAM_START+0x4+2)

/*! BLE_MGR_STAT_TYPE to indicate an ES event is executed too late by the ES hardware
 * @return ble_mgr_stat_es_trigger_too_late_t info record @ #BLE_MGR_STAT_INFO_REG_ADDRESS
 */
#define BLE_MGR_STAT_TYPE_ES_TRIGGER_TOO_LATE 1

#define ES_TRIGGER_TYPE_CLEANUP 0
#define ES_TRIGGER_TYPE_EVENT   1
typedef PACKED_PRE struct ble_mgr_stat_es_trigger_too_late_t
{
    UInt8   trigger_type;   //!< Which trigger type was executed too late. one of the ES_TRIGGER_TYPE_*
    UInt8   event_type;     //!< Which event type was executed too late. Same format as EVENT_TYPE
    UInt16  t_too_late;     //!< amount of time in microseconds the trigger was too late.
                            /*!< \warning Accuracy of this value is not that good. 
                             *      Because this value is calculated by sampling the symbol counter at ES interrupt entry, 
                             *      and this might be blocked by a higher priority interrupt (which may take up to 200 us).
                             */
} PACKED_POST ble_mgr_stat_es_trigger_too_late_t;

/*****************************************************************************
 * DESENSE FIX
 *****************************************************************************/

#define BLE_MGR_DSFIX_CH_LIST_SPACING       14
#define BLE_MGR_DSFIX_PARAM_LIST_SPACING    6
#define BLE_MGR_DSFIX_MAX_NR_PARAMS         4

/*****************************************************************************
 * Closed loop
 *****************************************************************************/

// Can be used to disable tx power update and measurements
#define BLE_MGR_PA_POWER_TABLE_INDEX_INVALID    0xFF

// Power limitation structure. Can be used to hard-limit the tx power on a certain channel
typedef PACKED_PRE struct
{
    UInt8 fllChannel;
    UInt8 paPowerTableIndex;
} PACKED_POST BleMgr_TxPowerLimitation_t;

// Tx power measurement done structure. Will be used to communicate measurement results from RT to NRT
typedef PACKED_PRE struct
{
    UInt8 paPowerTableIndex;
    UInt8 usedTxPowerSetting;
    UInt16 detectedVoltage;
} PACKED_POST BleMgr_TxPowerMeasurementDone_t;

#endif //BLE_MGR_H
