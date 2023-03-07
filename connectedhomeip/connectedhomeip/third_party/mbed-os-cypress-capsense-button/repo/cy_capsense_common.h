/***************************************************************************//**
* \file cy_capsense_common.h
* \version 3.0
*
* \brief
* This file provides the common CapSense definitions.
*
********************************************************************************
* \copyright
* Copyright 2018-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_CAPSENSE_COMMON_H)
#define CY_CAPSENSE_COMMON_H

#include "cy_device_headers.h"
#include "cy_sysint.h"

#if (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2) || defined(CY_IP_M0S8MSCV3))

#if defined(__cplusplus)
extern "C" {
#endif


/*******************************************************************************
* Macros
*******************************************************************************/

#if (defined(CY_IP_M0S8CSDV2))

    #define CY_CAPSENSE_PLATFORM_DEVICE_PSOC4           (1)
    #define CY_CAPSENSE_PLATFORM_DEVICE_PSOC6           (0)

    #define CY_CAPSENSE_PLATFORM_BLOCK_CSDV2            (1)
    #define CY_CAPSENSE_PLATFORM_BLOCK_MSCV3            (0)

    #define CY_CAPSENSE_PSOC4_CSDV2                     (1)
    #define CY_CAPSENSE_PSOC6_CSDV2                     (0)
    #define CY_CAPSENSE_PSOC4_MSCV3                     (0)

#elif (defined(CY_IP_MXCSDV2))

    #define CY_CAPSENSE_PLATFORM_DEVICE_PSOC4           (0)
    #define CY_CAPSENSE_PLATFORM_DEVICE_PSOC6           (1)

    #define CY_CAPSENSE_PLATFORM_BLOCK_CSDV2            (1)
    #define CY_CAPSENSE_PLATFORM_BLOCK_MSCV3            (0)

    #define CY_CAPSENSE_PSOC4_CSDV2                     (0)
    #define CY_CAPSENSE_PSOC6_CSDV2                     (1)
    #define CY_CAPSENSE_PSOC4_MSCV3                     (0)

#elif (defined(CY_IP_M0S8MSCV3))

    #define CY_CAPSENSE_PLATFORM_DEVICE_PSOC4           (1)
    #define CY_CAPSENSE_PLATFORM_DEVICE_PSOC6           (0)

    #define CY_CAPSENSE_PLATFORM_BLOCK_CSDV2            (0)
    #define CY_CAPSENSE_PLATFORM_BLOCK_MSCV3            (1)

    #define CY_CAPSENSE_PSOC4_CSDV2                     (0)
    #define CY_CAPSENSE_PSOC6_CSDV2                     (0)
    #define CY_CAPSENSE_PSOC4_MSCV3                     (1)

#endif


/******************************************************************************/
/** \addtogroup group_capsense_macros_general *//** \{ */
/******************************************************************************/
/** Middleware major version */
#define CY_CAPSENSE_MW_VERSION_MAJOR                    (3)
/** Middleware minor version */
#define CY_CAPSENSE_MW_VERSION_MINOR                    (0)
/** Middleware version */
#define CY_CAPSENSE_MW_VERSION                          (300)

#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
    /** Defined supported CSD driver version */
    #define CY_CSD_DRV_VERSION_REQUIRED                 (100)

    #if !defined (CY_CSD_DRV_VERSION)
        /** If CSD driver version not defined by driver then define it here */
        #define CY_CSD_DRV_VERSION                      (100)
    #endif

    /* Check used driver version */
    #if (CY_CSD_DRV_VERSION_REQUIRED > CY_CSD_DRV_VERSION)
        #error Some of the features supported by the CapSense Middleware version CY_CAPSENSE_MW_VERSION require newer version of the PDL. Update the PDL in your project.
    #endif
#endif

#if (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3)
    /** Defined supported CSD driver version */
    #define CY_MSC_DRV_VERSION_REQUIRED                 (100)

    #if !defined (CY_MSC_DRV_VERSION)
        /** If CSD driver version not defined by driver then define it here */
        #define CY_MSC_DRV_VERSION                      (100)
    #endif

    /* Check used driver version */
    #if (CY_MSC_DRV_VERSION_REQUIRED > CY_MSC_DRV_VERSION)
        #error Some of the features supported by the CapSense Middleware version CY_CAPSENSE_MW_VERSION require newer version of the PDL. Update the PDL in your project.
    #endif
#endif

/** Middleware ID */
#define CY_CAPSENSE_ID                                  (CY_PDL_DRV_ID(0x07uL))


/* Scanning status */
#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
/** The CapSense middleware is busy */
    #define CY_CAPSENSE_BUSY                            (0x80u)
#else
/** The CapSense middleware busy mask */
    #define CY_CAPSENSE_BUSY                            (0xFFu)
/** The MSCv3 channel busy mask */
    #define CY_CAPSENSE_BUSY_CH_MASK                    (0x01u)
/** All MSCv3 channels busy mask */
    #define CY_CAPSENSE_BUSY_ALL_CH_MASK                (0x0Fu)
/** The CapSense middleware is busy by auto-calibration */
    #define CY_CAPSENSE_BUSY_CALIBRATION                (0x10u)
/** The CapSense middleware is busy by auto-calibration verification */
    #define CY_CAPSENSE_BUSY_VERIFY_CALIBRATION         (0x20u)
#endif

/** The CapSense middleware is not busy */
#define CY_CAPSENSE_NOT_BUSY                            (0x00u)

/** Feature enabled */
#define CY_CAPSENSE_ENABLE                              (1u)
/** Feature disabled */
#define CY_CAPSENSE_DISABLE                             (0u)

/** Sensor active status mask */
#define CY_CAPSENSE_SNS_TOUCH_STATUS_MASK               (0x01u)
/** Proximity active status mask */
#define CY_CAPSENSE_SNS_PROX_STATUS_MASK                (0x02u)
/** Sensor overflow mask */
#define CY_CAPSENSE_SNS_OVERFLOW_MASK                   (0x04u)
/** Widget active status mask */
#define CY_CAPSENSE_WD_ACTIVE_MASK                      (0x01u)
/** Widget disable status mask */
#define CY_CAPSENSE_WD_DISABLE_MASK                     (0x02u)
/** Widget working status mask */
#define CY_CAPSENSE_WD_WORKING_MASK                     (0x04u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_settings *//** \{ */
/******************************************************************************/

/** Interrupt Driven scanning mode */
#define CY_CAPSENSE_SCAN_MODE_INT_DRIVEN                (0u)
/** DMA Driven scanning mode */
#define CY_CAPSENSE_SCAN_MODE_DMA_DRIVEN                (1u)

/** Sensor connection method through analog mux bus */
#define CY_CAPSENSE_AMUX_SENSOR_CONNECTION_METHOD       (0u)
/** Sensor connection method through control mux switches */
#define CY_CAPSENSE_CTRLMUX_SENSOR_CONNECTION_METHOD    (1u)

/** CIC filter mode */
#define CY_CAPSENSE_CIC_FILTER                          (0u)
/** CIC2 filter mode */
#define CY_CAPSENSE_CIC2_FILTER                         (1u)

/** Raw count counter mode saturate */
#define CY_CAPSENSE_COUNTER_MODE_SATURATE               (0u)
/** Raw count counter mode overflow */
#define CY_CAPSENSE_COUNTER_MODE_OVERFLOW               (1u)

/** Synchronization mode is disabled */
#define CY_CAPSENSE_SYNC_MODE_OFF                       (0u)
/** Synchronization is external */
#define CY_CAPSENSE_SYNC_EXTERNAL                       (1u)
/** Synchronization is internal */
#define CY_CAPSENSE_SYNC_INTERNAL                       (2u)

/** CSD sensing group */
#define CY_CAPSENSE_CSD_GROUP                           (1u)
/** CSX sensing group */
#define CY_CAPSENSE_CSX_GROUP                           (2u)

/** CSD sensing method */
#define CY_CAPSENSE_CSD_SENSING_METHOD                  (1u)
/** CSX Ratio Metric sensing method */
#define CY_CAPSENSE_CSX_RM_SENSING_METHOD               (2u)
/** CSD Ratio Metric sensing method */
#define CY_CAPSENSE_CSD_RM_SENSING_METHOD               (3u)
/** Sensing method undefined used at initialization */
#define CY_CAPSENSE_SENSING_METHOD_UNDEFINED            (255u)

/** Total number of modes */
#define CY_CAPSENSE_SENSING_METHOD_NUMBER               (3u)

/** Sensing mode undefined */
#define CY_CAPSENSE_REG_MODE_UNDEFINED                  (255u)
/** CSD sensing mode */
#define CY_CAPSENSE_REG_MODE_CSD                        (0u)
/** CSX Ratio Metric sensing mode */
#define CY_CAPSENSE_REG_MODE_CSX_RM                     (1u)
/** CSD Ratio Metric sensing mode */
#define CY_CAPSENSE_REG_MODE_CSD_RM                     (2u)
/** CSD sensing mode with CapDAC dithering enabled */
#define CY_CAPSENSE_REG_MODE_CSD_DITHERING              (3u)
/** CSX Ratio Metric sensing mode with CapDAC dithering enabled */
#define CY_CAPSENSE_REG_MODE_CSX_RM_DITHERING           (4u)
/** CSD Ratio Metric sensing mode with CapDAC dithering enabled */
#define CY_CAPSENSE_REG_MODE_CSD_RM_DITHERING           (5u)

/** Number of base sensing modes */
#define CY_CAPSENSE_REG_MODE_BASE_NUMBER                (3u)

/** Total number of modes */
#define CY_CAPSENSE_REG_MODE_NUMBER                     (6u)

/** Control mux switch state defined as CSX Rx electrode */
#define CY_CAPSENSE_CTRLMUX_STATE_RX                    (0u)
/** Control mux switch state defined as CSX Tx electrode */
#define CY_CAPSENSE_CTRLMUX_STATE_TX                    (1u)
/** Control mux switch state is ground */
#define CY_CAPSENSE_CTRLMUX_STATE_GND                   (2u)
/** Control mux switch state defined as CSD sensor */
#define CY_CAPSENSE_CTRLMUX_STATE_SNS                   (3u)
/** Control mux switch state is High-Z */
#define CY_CAPSENSE_CTRLMUX_STATE_HIGH_Z                (4u)
/** Control mux switch state defined as connection to shield signal */
#define CY_CAPSENSE_CTRLMUX_STATE_SHIELD                (5u)
/** Control mux switch state defined as CSX Negative Tx electrode */
#define CY_CAPSENSE_CTRLMUX_STATE_TX_NEGATIVE           (6u)
/** Number of supported pin states for smaller sensor configuration with 5 registers */
#define CY_CAPSENSE_PIN_STATE_FUNC_LIMIT                (4u)
/** Number of total possible pin states */
#define CY_CAPSENSE_PIN_STATE_NUMBER                    (7u)


/* SmartSense modes */
/** Manual tunning mode */
#define CY_CAPSENSE_CSD_SS_DIS                          (0x00uL)
/** Hardware auto-tune mask */
#define CY_CAPSENSE_CSD_SS_HW_EN                        (0x01uL)
/** Threshold auto-tune mask */
#define CY_CAPSENSE_CSD_SS_TH_EN                        (0x02uL)
/** Full auto-tune is enabled */
#define CY_CAPSENSE_CSD_SS_HWTH_EN                      (CY_CAPSENSE_CSD_SS_HW_EN | CY_CAPSENSE_CSD_SS_TH_EN)

/* MFS macros */
/** Number of multi-frequency scan channels */
#define CY_CAPSENSE_FREQ_CHANNELS_NUM                   (3u)
/** Multi-frequency channel 0 constant */
#define CY_CAPSENSE_MFS_CH0_INDEX                       (0u)
/** Multi-frequency channel 1 constant */
#define CY_CAPSENSE_MFS_CH1_INDEX                       (1u)
/** Multi-frequency channel 2 constant */
#define CY_CAPSENSE_MFS_CH2_INDEX                       (2u)

/* Inactive sensor connection options */
/** Inactive sensor connection to ground */
#define CY_CAPSENSE_SNS_CONNECTION_GROUND               (1u)
/** Inactive sensor connection to High-Z */
#define CY_CAPSENSE_SNS_CONNECTION_HIGHZ                (2u)
/** Inactive sensor connection to shield */
#define CY_CAPSENSE_SNS_CONNECTION_SHIELD               (3u)

#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
/* Shield electrode delay macros */
/** No shield signal delay */
#define CY_CAPSENSE_SH_DELAY_0NS                        (0u)
/** Shield signal delay > 5 ns */
#define CY_CAPSENSE_SH_DELAY_5NS                        (1u)
/** Shield signal delay > 10 ns */
#define CY_CAPSENSE_SH_DELAY_10NS                       (2u)
/** Shield signal delay > 20 ns */
#define CY_CAPSENSE_SH_DELAY_20NS                       (3u)

/* Idac sensing configuration */
/** Idac sourcing */
#define CY_CAPSENSE_IDAC_SOURCING                       (0u)
/** Idac sinking */
#define CY_CAPSENSE_IDAC_SINKING                        (1u)

/* Shield tank capacitor pre-charge source options */
/** Shield tank capacitor pre-charge from Vref */
#define CY_CAPSENSE_CSH_PRECHARGE_VREF                  (0u)
/** Shield tank capacitor pre-charge from IO buffer */
#define CY_CAPSENSE_CSH_PRECHARGE_IO_BUF                (1u)
#endif

/* Sense clock selection options */
/** Auto-mode of clock source selection mask */
#define CY_CAPSENSE_CLK_SOURCE_AUTO_MASK                (0x80u)
/** Clock source direct */
#define CY_CAPSENSE_CLK_SOURCE_DIRECT                   (0x00u)
/** Clock source SSC6 */
#define CY_CAPSENSE_CLK_SOURCE_SSC6                     (0x01u)
/** Clock source SSC7 */
#define CY_CAPSENSE_CLK_SOURCE_SSC7                     (0x02u)
/** Clock source SSC9 */
#define CY_CAPSENSE_CLK_SOURCE_SSC9                     (0x03u)
/** Clock source SSC10 */
#define CY_CAPSENSE_CLK_SOURCE_SSC10                     (0x04u)
/** Clock source PRS8 */
#define CY_CAPSENSE_CLK_SOURCE_PRS8                     (0x05u)
/** Clock source PRS12 */
#define CY_CAPSENSE_CLK_SOURCE_PRS12                    (0x06u)

/* Sense clock selection options for MSCv3 */
/** Clock source PRS */
#define CY_CAPSENSE_CLK_SOURCE_PRS                      (0x02u)
/** Clock source SSC */
#define CY_CAPSENSE_CLK_SOURCE_SSC                      (0x01u)

#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
/* Initialization switch resistance options */
/** Low switch resistance at initialization */
#define CY_CAPSENSE_INIT_SW_RES_LOW                     (0x00000000uL)
/** Medium switch resistance at initialization */
#define CY_CAPSENSE_INIT_SW_RES_MEDIUM                  (0x00000001uL)
/** High switch resistance at initialization */
#define CY_CAPSENSE_INIT_SW_RES_HIGH                    (0x00000002uL)

/* Scanning switch resistance options */
/** Low switch resistance at scanning */
#define CY_CAPSENSE_SCAN_SW_RES_LOW                     (0x00000000uL)
/** Medium switch resistance at scanning */
#define CY_CAPSENSE_SCAN_SW_RES_MEDIUM                  (0x00000001uL)
/** High switch resistance at scanning */
#define CY_CAPSENSE_SCAN_SW_RES_HIGH                    (0x00000002uL)

/* CSD shield switch resistance options */
/** Low shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_LOW                   (0x00000000uL)
/** Medium shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_MEDIUM                (0x00000001uL)
/** High shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_HIGH                  (0x00000002uL)
/** Low-EMI shield switch resistance */
#define CY_CAPSENSE_SHIELD_SW_RES_LOW_EMI               (0x00000003uL)
#endif

/* CSD shield mode options */
/** Shield disabled */
#define CY_CAPSENSE_SHIELD_DISABLED                     (0u)
/** Active shield mode */
#define CY_CAPSENSE_SHIELD_ACTIVE                       (1u)
/** Passive shield mode */
#define CY_CAPSENSE_SHIELD_PASSIVE                      (2u)

/* Vref source options */
/** Vref source is taken from SRSS */
#define CY_CAPSENSE_VREF_SRSS                           (0x00000000uL)
/** Vref source is taken from PASS */
#define CY_CAPSENSE_VREF_PASS                           (0x00000001uL)

/* Iref source options */
/** Iref source is taken from SRSS */
#define CY_CAPSENSE_IREF_SRSS                           (0x00000000uL)
/** Iref source is taken from PASS */
#define CY_CAPSENSE_IREF_PASS                           (0x00000001uL)

/** Voltage limit to switch to low-voltage configuration */
#define CY_CAPSENSE_LOW_VOLTAGE_LIMIT                   (2000u)

/* Position Filter Configuration */
/** Mask of all filters enabling */
#define CY_CAPSENSE_POSITION_FILTERS_MASK               (0x000000FFu)
/** Median position filter enable mask */
#define CY_CAPSENSE_POSITION_MED_MASK                   (0x00000001u)
/** IIR position filter enable mask */
#define CY_CAPSENSE_POSITION_IIR_MASK                   (0x00000002u)
/** Adaptive IIR position filter enable mask */
#define CY_CAPSENSE_POSITION_AIIR_MASK                  (0x00000004u)
/** Average position filter enable mask */
#define CY_CAPSENSE_POSITION_AVG_MASK                   (0x00000008u)
/** Jitter position filter enable mask */
#define CY_CAPSENSE_POSITION_JIT_MASK                   (0x00000010u)
/** Mask of position filters history size */
#define CY_CAPSENSE_POSITION_FILTERS_SIZE_MASK          (0x0000FF00u)
/** Offset of position filters history size */
#define CY_CAPSENSE_POSITION_FILTERS_SIZE_OFFSET        (8u)
/** Mask of IIR coefficient of position filter */
#define CY_CAPSENSE_POSITION_IIR_COEFF_MASK             (0x00FF0000u)
/** Offset of IIR coefficient of position filter */
#define CY_CAPSENSE_POSITION_IIR_COEFF_OFFSET           (16u)

/* Raw count filter macros */
/** Offset of raw count filter history size */
#define CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_OFFSET   (0u)
/** Offset of raw count median filter enable mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_EN_OFFSET          (4u)
/** Offset of raw count median filter mode mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_MODE_OFFSET        (5u)
/** Offset of raw count IIR filter enable mask */
#define CY_CAPSENSE_RC_FILTER_IIR_EN_OFFSET             (7u)
/** Offset of raw count IIR filter mode mask */
#define CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET           (8u)
/** Offset of raw count average filter enable mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_EN_OFFSET         (10u)
/** Offset of raw count average filter mode mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_OFFSET       (11u)

/** Mask of raw count filter history size */
#define CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_MASK     ((uint16_t)((uint16_t)0x000Fu << CY_CAPSENSE_RC_FILTER_SNS_HISTORY_SIZE_OFFSET))
/** Median raw count filter enable mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_EN_MASK            ((uint16_t)((uint16_t)0x0001u << CY_CAPSENSE_RC_FILTER_MEDIAN_EN_OFFSET))
/** Median raw count filter mode mask */
#define CY_CAPSENSE_RC_FILTER_MEDIAN_MODE_MASK          ((uint16_t)((uint16_t)0x0003u << CY_CAPSENSE_RC_FILTER_MEDIAN_MODE_OFFSET))
/** IIR raw count filter enable mask */
#define CY_CAPSENSE_RC_FILTER_IIR_EN_MASK               ((uint16_t)((uint16_t)0x0001u << CY_CAPSENSE_RC_FILTER_IIR_EN_OFFSET))
/** IIR raw count filter mode mask */
#define CY_CAPSENSE_RC_FILTER_IIR_MODE_MASK             ((uint16_t)((uint16_t)0x0003u << CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET))
/** Average raw count filter enable mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_EN_MASK           ((uint16_t)((uint16_t)0x0001u << CY_CAPSENSE_RC_FILTER_AVERAGE_EN_OFFSET))
/** Average raw count filter mode mask */
#define CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_MASK         ((uint16_t)((uint16_t)0x0003u << CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_OFFSET))
/** All raw count filters enable mask */
#define CY_CAPSENSE_RC_FILTER_ALL_EN_MASK               (CY_CAPSENSE_RC_FILTER_MEDIAN_EN_MASK |\
                                                         CY_CAPSENSE_RC_FILTER_IIR_EN_MASK |\
                                                         CY_CAPSENSE_RC_FILTER_AVERAGE_EN_OFFSET)
/** Raw count IIR filter mode standard */
#define CY_CAPSENSE_IIR_FILTER_STANDARD                 ((uint16_t)((uint16_t)1u << CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET))
/** Raw count IIR filter mode performance */
#define CY_CAPSENSE_IIR_FILTER_PERFORMANCE              ((uint16_t)((uint16_t)2u << CY_CAPSENSE_RC_FILTER_IIR_MODE_OFFSET))
/** Raw count average filter mode */
#define CY_CAPSENSE_AVERAGE_FILTER_LEN_4                ((uint16_t)((uint16_t)2u << CY_CAPSENSE_RC_FILTER_AVERAGE_MODE_OFFSET))
/** \} */

/* Slot configuration */
/** Shield only slot */
#define CY_CAPSENSE_SLOT_SHIELD_ONLY                    (0xFFFDu)
/** TX only slot */
#define CY_CAPSENSE_SLOT_TX_ONLY                        (0xFFFEu)
/** Empty slot */
#define CY_CAPSENSE_SLOT_EMPTY                          (0xFFFFu)

/* Centroid configuration */
/** Supported touch count mask */
#define CY_CAPSENSE_CENTROID_NUMBER_MASK                (0x0003u)
/** Diplex enable mask */
#define CY_CAPSENSE_DIPLEXING_MASK                      (0x0004u)
/** Ballistic multiplier enable mask */
#define CY_CAPSENSE_CENTROID_BALLISTIC_MASK             (0x0008u)
/** 3x3 centroid enable mask */
#define CY_CAPSENSE_CENTROID_3X3_MASK                   (0x0020u)
/** 5x5 centroid enable mask */
#define CY_CAPSENSE_CENTROID_5X5_MASK                   (0x0040u)
/** Edge correction enable mask */
#define CY_CAPSENSE_EDGE_CORRECTION_MASK                (0x0080u)
/**
* Centroid calculation method mask
* * Direct provides correct calculation but worse linearity on edges
*   and requires edge correction [MaxPosition / (numSns)]
* * Approximate provides better linearity on edges that leads to
*   worse accuracy in general [MaxPosition / (numSns - 1u)]
*/
#define CY_CAPSENSE_CALC_METHOD_MASK                    (0x0100u)

/******************************************************************************/
/** \addtogroup group_capsense_macros_touch *//** \{ */
/******************************************************************************/
/** No touch detected */
#define CY_CAPSENSE_POSITION_NONE                       (0x00u)
/** One finger touch detected */
#define CY_CAPSENSE_POSITION_ONE                        (0x01u)
/** Two finger touches detected */
#define CY_CAPSENSE_POSITION_TWO                        (0x02u)
/** Three finger touches detected */
#define CY_CAPSENSE_POSITION_THREE                      (0x03u)
/** Multiple touches detected that exceed number of supported touches */
#define CY_CAPSENSE_POSITION_MULTIPLE                   (0xFFu)
/** Maximum possible centroid number */
#define CY_CAPSENSE_MAX_CENTROIDS                       (3u)
/** Maximum number of peaks in CSX Touchpad */
#define CY_CAPSENSE_CSX_TOUCHPAD_MAX_PEAKS              (5u)
/** Maximum age of a touch */
#define CY_CAPSENSE_CSX_TOUCHPAD_MAX_AGE                (0xFFu)
/** Undefined ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_UNDEFINED           (0x80u)
/** Non-assigned ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_ABSENT              (0x40u)
/** Touch below threshold ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_ON_FAIL             (0x20u)
/** Minimum ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_MIN                 (0u)
/** Maximum ID */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_MAX                 (7u)
/** ID mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_ID_MASK                (0x00FFu)
/** Touch power mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_Z_MASK                 (0x00FFu)
/** Age mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_AGE_MASK               (0xFF00u)
/** Debounce mask */
#define CY_CAPSENSE_CSX_TOUCHPAD_DEBOUNCE_MASK          (0xFF00u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_process *//** \{ */
/******************************************************************************/
/** Applies all enabled filters in the default order to the raw counts */
#define CY_CAPSENSE_PROCESS_FILTER                      (0x01u)
/** Updates baselines using current raw count values for the widget/sensor */
#define CY_CAPSENSE_PROCESS_BASELINE                    (0x02u)
/** Calculates differences for the widget/sensor */
#define CY_CAPSENSE_PROCESS_DIFFCOUNTS                  (0x04u)
/** Runs the noise envelope filter to measure noise magnitude for the widget/sensor */
#define CY_CAPSENSE_PROCESS_CALC_NOISE                  (0x08u)
/** Updates widget thresholds based on raw counts noise magnitude */
#define CY_CAPSENSE_PROCESS_THRESHOLDS                  (0x10u)
/** Runs the widget-specific processing algorithms and updates it status */
#define CY_CAPSENSE_PROCESS_STATUS                      (0x20u)
/** Definition that combines all possible processing tasks */
#define CY_CAPSENSE_PROCESS_ALL                         (CY_CAPSENSE_PROCESS_FILTER | \
                                                         CY_CAPSENSE_PROCESS_BASELINE | \
                                                         CY_CAPSENSE_PROCESS_DIFFCOUNTS | \
                                                         CY_CAPSENSE_PROCESS_CALC_NOISE | \
                                                         CY_CAPSENSE_PROCESS_THRESHOLDS | \
                                                         CY_CAPSENSE_PROCESS_STATUS)
/** Multi-phase TX min order */
#define CY_CAPSENSE_MPTX_MIN_ORDER                      (4u)
/** Multi-phase TX max order */
#define CY_CAPSENSE_MPTX_MAX_ORDER                      (32u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_pin *//** \{ */
/******************************************************************************/
/** Connection of pin to ground */
#define CY_CAPSENSE_GROUND                              (0u)
/** Connection of pin to High-Z */
#define CY_CAPSENSE_HIGHZ                               (1u)
/** Configuring of pin as a shield */
#define CY_CAPSENSE_SHIELD                              (2u)
/** Configuring of pin as a CSD sensor */
#define CY_CAPSENSE_SENSOR                              (3u)
/** Configuring of pin as a Tx */
#define CY_CAPSENSE_TX_PIN                              (4u)
/** Configuring of pin as a Rx */
#define CY_CAPSENSE_RX_PIN                              (5u)
/** Pin is not connected to scanning bus */
#define CY_CAPSENSE_SNS_DISCONNECTED                    (0u)
/** Pin is connected to scanning bus */
#define CY_CAPSENSE_SNS_CONNECTED                       (1u)
/** \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros_miscellaneous *//** \{ */
/******************************************************************************/
/** Defines the communication command complete bit mask */
#define CY_CAPSENSE_TU_CMD_COMPLETE_BIT                 (0x8000u)
/** Defines the status if restart was not performed in Cy_CapSense_RunTuner() */
#define CY_CAPSENSE_STATUS_RESTART_NONE                 (0x00u)
/** Defines the status if restart was done in Cy_CapSense_RunTuner() */
#define CY_CAPSENSE_STATUS_RESTART_DONE                 (0x01u)
#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
/** Number of supported idac gains */
#define CY_CAPSENSE_IDAC_GAIN_NUMBER                    (6u)
#endif
/** 100% value */
#define CY_CAPSENSE_PERCENTAGE_100                      (100u)
/* Scope of scanning macros */
/** Sensor scanning scope is undefined */
#define CY_CAPSENSE_SCAN_SCOPE_UND                      (0u)
/** Sensor scanning scope is a single sensor */
#define CY_CAPSENSE_SCAN_SCOPE_SGL_SNS                  (1u)
/** Sensor scanning scope is all sensors in a queue */
#define CY_CAPSENSE_SCAN_SCOPE_ALL_SNS                  (2u)
/** Widget scanning scope is a single widget */
#define CY_CAPSENSE_SCAN_SCOPE_SGL_WD                   (0u)
/** Widget scanning scope is all widgets */
#define CY_CAPSENSE_SCAN_SCOPE_ALL_WD                   (1u)

/** \} */


#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
/******************************************************************************/
/** \addtogroup group_capsense_macros_bist *//** \{ */
/******************************************************************************/
/** The mask for a widget CRC test */
#define CY_CAPSENSE_BIST_CRC_WDGT                       (0x0001uL)
/** The mask for a baseline integrity test */
#define CY_CAPSENSE_BIST_BSLN_INTEGRITY                 (0x0002uL)
/** The mask for a rawcount integrity test */
#define CY_CAPSENSE_BIST_RAW_INTEGRITY                  (0x0004uL)
/** The mask for a pin integrity test */
#define CY_CAPSENSE_BIST_SNS_INTEGRITY                  (0x0008uL)
/** The mask for a sensor capacitance measurement test */
#define CY_CAPSENSE_BIST_SNS_CAP                        (0x0010uL)
/** The mask for a shield capacitance measurement test */
#define CY_CAPSENSE_BIST_SHIELD_CAP                     (0x0020uL)
/** The mask for an external capacitor capacitance measurement test */
#define CY_CAPSENSE_BIST_EXTERNAL_CAP                   (0x0040uL)
/** The mask for a VDDA measurement test */
#define CY_CAPSENSE_BIST_VDDA                           (0x0080uL)

/** \} */
#endif


/******************************************************************************/
/** \addtogroup group_capsense_macros_general *//** \{ */
/******************************************************************************/
/** Return status \ref cy_capsense_status_t of CapSense operation: Successfully */
#define CY_CAPSENSE_STATUS_SUCCESS                      (0x00u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Bad input parameter */
#define CY_CAPSENSE_STATUS_BAD_PARAM                    (0x01u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Received wrong data */
#define CY_CAPSENSE_STATUS_BAD_DATA                     (0x02u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Timeout */
#define CY_CAPSENSE_STATUS_TIMEOUT                      (0x04u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Invalid state */
#define CY_CAPSENSE_STATUS_INVALID_STATE                (0x08u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Bad configuration */
#define CY_CAPSENSE_STATUS_BAD_CONFIG                   (0x10u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Overflow */
#define CY_CAPSENSE_STATUS_CONFIG_OVERFLOW              (0x20u)
/** Return status \ref cy_capsense_status_t of CapSense operation: HW is busy */
#define CY_CAPSENSE_STATUS_HW_BUSY                      (0x40u)
/** Return status \ref cy_capsense_status_t of CapSense operation: HW is locked */
#define CY_CAPSENSE_STATUS_HW_LOCKED                    (0x80u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Unable to perform calibration */
#define CY_CAPSENSE_STATUS_CALIBRATION_FAIL             (0x100u)
/** Return status \ref cy_capsense_status_t of CapSense operation: Unknown */
#define CY_CAPSENSE_STATUS_UNKNOWN                      (0x80000000u)

/** \} */

#define CY_CAPSENSE_BYTE_IN_32_BIT                      (4u)
#define CY_CAPSENSE_CONVERSION_MEGA                     (1000000u)

/*
* These defines are obsolete and kept for backward compatibility only.
* They will be removed in the future versions.
*/
#define CY_CAPSENSE_MDW_VERSION_MAJOR                   (CY_CAPSENSE_MW_VERSION_MAJOR)
#define CY_CAPSENSE_MDW_VERSION_MINOR                   (CY_CAPSENSE_MW_VERSION_MINOR)
#define CY_CAPSENSE_MDW_ID                              (CY_CAPSENSE_ID)
#define CY_CAPSENSE_SW_STS_BUSY                         (CY_CAPSENSE_BUSY)

#if defined(__cplusplus)
}
#endif

#endif /* (defined(CY_IP_MXCSDV2) || defined(CY_IP_M0S8CSDV2) || defined(CY_IP_M0S8MSCV3)) */

#endif /* CY_CAPSENSE_COMMON_H */


/* [] END OF FILE */
