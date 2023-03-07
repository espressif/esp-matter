/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gphal_Reset.c
 *
 *  The file gpHal.h contains startup code of the gphal (init, reset).
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_Statistics.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"          //Containing all uC dependent implementations
#include "gpHal_reg.h"
#include "gpHal_Calibration.h"
#include "gpAssert.h"

#include "gpLog.h"
#include "gpReset.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GPHAL_TEST_ADDRESS         GP_WB_RX_BT_OBSERVE_CO_0_ADDRESS
#define GPHAL_TEST_ADDRESS_VALUE   0x9876

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

// pan coordinator regmap field has different encoding: (1 = no pan coordinator, 2 = pan coordinator)
#define GP_HAL_PAN_COORDINATOR_TO_ADDRESSMAP_FIELD(panCoordinator)  (panCoordinator + 1)

#ifndef GP_HAL_BSP_INIT
//No generated GPIO initialization available - for external usage
#define GP_HAL_BSP_INIT()
#endif //GP_HAL_BSP_INIT

#if defined(GP_HAL_DIVERSITY_EXT_MODE_SIGNALS)
#if defined(GP_BSP_RF_DEBUG_TX_SUPPORTED) || defined(GP_BSP_RF_DEBUG_RX_SUPPORTED)
#error Cannot combine RF debug signals with external model signals
#endif //defined(GP_BSP_RF_DEBUG_TX_SUPPORTED) || defined(GP_BSP_RF_DEBUG_RX_SUPPORTED)
#endif //GP_HAL_DIVERSITY_EXT_MODE_SIGNALS

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


static UInt8 gpHal_ChipVersion;


/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

extern UInt8 gpHal_awakeCounter;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void gpHal_InitRadioSettings(void);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void gpHalReset_InitMacFilter(void)
{
    // init macfilt stuff, not really at it's place in fll lib....
    //Skipping false values - register starts out as 0
    GP_WB_WRITE_MACFILT_EXT_ADDR_TABLE_SIZE(GP_HAL_MAC_NUMBER_OF_RX_SRCIDS); // 3 entries

    //Init channel indices fixed - srcId 0/1/2 use index 0/1/2
    GP_WB_WRITE_MACFILT_CHANNEL_IDX(BM(0));
    GP_WB_WRITE_MACFILT_CHANNEL_IDX_ALT_A(BM(1));
    GP_WB_WRITE_MACFILT_CHANNEL_IDX_ALT_B(BM(2));

    GP_WB_WRITE_MACFILT_ACCEPT_FT_BCN(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FT_DATA(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FT_ACK(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FT_CMD(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_4(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_5(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_6(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FT_RSV_7(true);

    GP_WB_WRITE_MACFILT_ACCEPT_FV_2003(true);
    GP_WB_WRITE_MACFILT_ACCEPT_FV_2006(true);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_2010(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_3(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_4(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_5(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_6(false);
    //GP_WB_WRITE_MACFILT_ACCEPT_FV_RSV_7(false);

    //GP_WB_WRITE_MACFILT_ACCEPT_FT_BCN_SRC_ADDR_MODE_00(false);
    GP_WB_WRITE_MACFILT_ACCEPT_BCAST_DADDR(true);
    GP_WB_WRITE_MACFILT_ACCEPT_BCAST_PAN_ID(true);
    GP_WB_WRITE_MACFILT_SRC_PAN_ID_BEACON_CHECK_ON(true);
    GP_WB_WRITE_MACFILT_SRC_PAN_ID_DATA_COMMAND_CHECK_ON(true);
    GP_WB_WRITE_MACFILT_DST_PAN_ID_CHECK_ON(true);
    GP_WB_WRITE_MACFILT_DST_ADDR_CHECK_ON(true);
    //GP_WB_WRITE_MACFILT_CMD_TYPE_CHECK_ON(false);

    GP_WB_WRITE_MACFILT_FT_BCN_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_DATA_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_CMD_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_RSV_4_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_MP_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_FRA_TO_QUEUE(true);
    GP_WB_WRITE_MACFILT_FT_EXT_TO_QUEUE(true);

    GP_WB_WRITE_MACFILT_BROADCAST_MASK(0xFFFF);
    GP_WB_WRITE_MACFILT_SHORT_ADDRESS(0xFFFF); //Previous Kx default
    GP_WB_WRITE_MACFILT_PAN_COORDINATOR(GP_HAL_PAN_COORDINATOR_TO_ADDRESSMAP_FIELD(false));

    GP_WB_WRITE_RIB_ACK_TIMEOUT(GP_HAL_IMMACK_TIMEOUT);
    GP_WB_WRITE_RIB_ACK_TIMEOUT_ON_START(GP_HAL_ACK_TIMEOUT_ON_START);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  INIT / RESET / INT handling
//-------------------------------------------------------------------------------------------------------

Bool gpHal_DidGPReset(void)
{
    if(GP_WB_READ_STANDBY_UNMASKED_PORD_INTERRUPT())
    {
        GP_WB_STANDBY_CLR_PORD_INTERRUPT();
        return true;
    }
    return false;
}

Bool gpHal_CheckMsi(void)
{
    return (GPHAL_TEST_ADDRESS_VALUE == GP_WB_READ_U16(GPHAL_TEST_ADDRESS));
}

UInt8 gpHal_IsRadioAccessible(void)
{
    return gpHal_awakeCounter;
}

void gpHal_Init(Bool timedMAC)
{
    NOT_USED(timedMAC);

    //Version checking
    gpHal_InitVersionInfo();


    if(GP_BSP_32KHZ_CRYSTAL_AVAILABLE())
    {
        // Enable to work with 32kHz Xtal sleep mode
        GP_WB_WRITE_PMUD_CLK_XT32K_PUP_FORCE(1);
    }

    gpHal_InitCalibration();
#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    gpHal_FllInit();
#endif

    // Set any radio related settings
    gpHalReset_InitMacFilter();

    // Initialize RNG functionality
    gpHal_InitRandom();

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    gpHal_PbmInit();
    gpHal_IpcInit();
#endif // defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

    gpHal_InitRadioSettings();

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    // Trigger (initial) calibration routine on gpMicro
    gpHal_IpcTriggerCommand(BLE_MGR_FLL_CAL_NRT, 0, NULL);
#endif // defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)

#if defined(GP_COMP_GPHAL_MAC) || defined(GP_COMP_GPHAL_BLE)
    gpHal_InitPhy();
#endif

#ifdef GP_COMP_GPHAL_MAC
    gpHal_InitScan();
    gpHal_InitMAC();
#endif //GP_COMP_GPHAL_MAC

#ifdef GP_DIVERSITY_GPHAL_INDIRECT_TRANSMISSION
    gpHal_InitDataPending();
#endif //GP_DIVERSITY_GPHAL_INDIRECT_TRANSMISSION

#ifdef GP_COMP_GPHAL_BLE
    gpHal_InitBle();
#endif //GP_COMP_GPHAL_BLE

    gpHal_InitSleep();

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT_WKUP
#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
    // configure sleep/wakeup
    {
        gpHal_ExternalEventDescriptor_t externalEventDescriptor;

        externalEventDescriptor.type = gpHal_EventTypeDummy;
        gpHal_EnableExternalEventCallbackInterrupt(true);
        gpHal_ScheduleExternalEvent(&externalEventDescriptor);
    }
#endif
#endif // GP_COMP_GPHAL_ES_EXT_EVENT_WKUP



    gpHal_StatisticsCountersClear();
}

void gpHal_AdvancedInit(void)
{
#ifdef GP_COMP_GPHAL_ES
    // Start taking benchmark measurements for calibrating the sleep clock
    gpHal_StartInitialBenchmarkMeasurements();
#endif //ifdef GP_COMP_GPHAL_ES

#if defined(GP_DIVERSITY_GPHAL_TRIM_XTAL_32M)
    gpHal_Xtal32MHzTrimAlgorithmInit();
#else
#if defined(GP_DIVERSITY_ENABLE_125_DEGREE_CELSIUS)
#error "Support of 125C in applications requires XTAL trimming! Please also define GP_DIVERSITY_GPHAL_TRIM_XTAL_32M."
#endif //defined(GP_DIVERSITY_ENABLE_125_DEGREE_CELSIUS)
#endif //defined(GP_DIVERSITY_GPHAL_TRIM_XTAL_32M)
#ifndef GP_DIVERSITY_GPHAL_DISABLE_TRIM_VDD_RAM_TUNE
    gpHal_VddRamTuneTrimAlgoInit();
#endif

    gpHal_GoToSleepWhenIdle(true);
}

/*****************************************************************************
 *                    Kx Specific
 *****************************************************************************/

UInt16 gpHal_GetHWVersionId(void)
{
#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
/*
 *                   |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * STANDBY_VERSION_0 |chipid(1:0)|             chipVersion           |
 * STANDBY_VERSION_1 | ext |     yieldid     | EMU |   chipid(4:2)   |
*/
#else
/*
 *                   |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * STANDBY_VERSION_0 |                  chipVersion                  |
 * STANDBY_VERSION_1 |       Metalfix        | EMU | colibri version |
*/
#endif
    return GP_WB_READ_STANDBY_VERSION();
}

void gpHal_InitVersionInfo(void)
{
#ifndef GP_DIVERSITY_GPHAL_K8E
#error "Following HW version id is specific for K8E"
#endif

    //emulation         assert if build is not compatible with device
    //chipid            assert if build is not compatible with device
    //chipidext         assert if build is not compatible with device
    //chipversion       read from device; assert when < GPHAL_MIN_CHIPVERSION; ignored GP_HAL_EXPECTED_CHIP_VERSION
    //yieldid           no impact on SW
    //romblversion      read from device; ignored GP_HAL_EXPECTED_ROM_BL_VERSION
    //rtflashversion    read from device; assert when < GPHAL_MIN_RTSYSTEMVERSION; in case of flash assert when != GP_DIVERSITY_RT_SYSTEM_IN_FLASH_VERSION

    UInt16 HWversion = gpHal_GetHWVersionId();

#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
    //>>> CHIP EMULATION (bit 11)
    GP_ASSERT_SYSTEM( false == BIT_TST(HWversion,11) );
#else
    //>>> CHIP EMULATION (bit 11)
    GP_ASSERT_SYSTEM( true == BIT_TST(HWversion,11) );
#endif

#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
    //>>> !EMULATED CHIP ID (bit 6..10)
    GP_ASSERT_SYSTEM(GPHAL_CHIP_ID_K8E == BF(HWversion,0x07C0,6));
    //>>> !EMULATED CHIP ID EXT (bit 15)
    GP_ASSERT_SYSTEM( false == BIT_TST(HWversion,15) );
    //>>> !EMULATED CHIP ID (bit 14..12)
    /* Bits 14 to 12 are -on purpose- not checked. */
    /* These bits keep track of the yield improvement */
    /* updates for which _no_ SW changes are required */
#else
    //>>> EMULATED CHIP ID (bit 0..7)
    GP_ASSERT_SYSTEM(0xF == BF(HWversion,0x00FF,0));
#endif

#ifndef GP_HAL_EXPECTED_CHIP_EMULATED
    {
        //>>> !EMULATED CHIP VERSION (bit 0..5)
        gpHal_ChipVersion = BF(HWversion,0x003F,0);
        switch (gpHal_ChipVersion) //convert HW encoding in SW encoding
        {
            case 1: //MF0
            case 2: //MF1 // No changes relevant to SW.
            case 3: //MF2 // No changes relevant to SW.
            case 4: //MF3 // No changes relevant to SW.
                gpHal_ChipVersion = 0;
                break;
            default:
                gpHal_ChipVersion = 0xFF;
                break;
        }
    }
#else //!GP_HAL_EXPECTED_CHIP_EMULATED
    {

        //>>> EMULATED CHIP VERSION (bit 12..15)
        gpHal_ChipVersion = BF(HWversion,0xF000,12);
        switch (gpHal_ChipVersion) //convert HW encoding in SW encoding
        {
            case 0: //MF0
                break;
            default:
                gpHal_ChipVersion = 0xFF;
                break;
        }
    }
#endif //!GP_HAL_EXPECTED_CHIP_EMULATED
    if (gpHal_ChipVersion == 0xFF)
    {
        gpHal_ChipVersion = GP_WB_READ_NVR_UNKNOWN_CHIP_VERSION_HANDLING();

        if (gpHal_ChipVersion == 0) //chipversion is not written
        {
            //treat as last known chipversion (all checks should use <= 2, instead of == 2)
            gpHal_ChipVersion = 0xFF;
        }
    }
    GP_ASSERT_SYSTEM(gpHal_ChipVersion >= GPHAL_MIN_CHIPVERSION);

#ifdef GP_HAL_DIVERSITY_INCLUDE_IPC
    gpHal_RtInitVersionInfo();
#endif
}

UInt8 gpHal_GetChipVersion(void)
{
    return gpHal_ChipVersion;
}



void gpHal_InitRadioSettings(void)
{
    rap_sys_hal_init();
}
