/***************************************************************************//**
 * @file    iot_adc_drv_sar.h
 * @brief   Successive-Approximation Register ADC driver header file
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_ADC_DRV_SAR_H_
#define _IOT_ADC_DRV_SAR_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "em_core.h"
#include "em_device.h"

#include "sl_status.h"

/*******************************************************************************
 *                             START OF MODULE
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES == 0) || (_SILICON_LABS_32B_SERIES == 1)

/*******************************************************************************
 *                          SERIES 0/1 INCLUDES
 ******************************************************************************/

#include "em_adc.h"

/*******************************************************************************
 *                               PROTOTYPES
 ******************************************************************************/

/* driver control */
sl_status_t iot_adc_drv_sar_driver_init(void *pvHndl);
sl_status_t iot_adc_drv_sar_driver_deinit(void *pvHndl);

/* driver configuration */
sl_status_t iot_adc_drv_sar_config_set(void *pvHndl,
                                          uint32_t ulAq,
                                          uint8_t ucRs);
sl_status_t iot_adc_drv_sar_config_get(void *pvHndl,
                                          uint32_t *pulAq,
                                          uint8_t *pucRs);

/* h/w control */
sl_status_t iot_adc_drv_sar_hw_enable(void *pvHndl);
sl_status_t iot_adc_drv_sar_hw_disable(void *pvHndl);

/* bus control */
sl_status_t iot_adc_drv_sar_bus_alloc(void *pvHndl, uint8_t ucChannel);
sl_status_t iot_adc_drv_sar_bus_dealloc(void *pvHndl, uint8_t ucChannel);

/* single transfers */
sl_status_t iot_adc_drv_sar_single_init(void *pvHndl, uint8_t ucCh);
sl_status_t iot_adc_drv_sar_single_start(void *pvHndl);
sl_status_t iot_adc_drv_sar_single_read(void *pvHndl, uint16_t *pusSample);

/* multi transfers */
sl_status_t iot_adc_drv_sar_multi_init(void *pvHndl, uint8_t ucCh);
sl_status_t iot_adc_drv_sar_multi_start(void *pvHndl);
sl_status_t iot_adc_drv_sar_multi_read(void *pvHndl, uint16_t *pusSample);

/* irq transfers */
sl_status_t iot_adc_drv_sar_irq_enable(void *pvHndl);
sl_status_t iot_adc_drv_sar_irq_disable(void *pvHndl);

/*******************************************************************************
 *                            CHANNEL MAPPING
 ******************************************************************************/

typedef struct iot_adc_drv_sar_map {
#if (_SILICON_LABS_32B_SERIES == 0)
  ADC_SingleInput_TypeDef singleInput;
#elif (_SILICON_LABS_32B_SERIES == 1)
  ADC_PosSel_TypeDef      posSel;
  ADC_NegSel_TypeDef      negSel;
#endif
} iot_adc_drv_sar_map_t[256][2];

/*******************************************************************************
 *                            SERIES 0 MAPPINGS
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES == 0)

#define adcSingleInputInvalid    ((ADC_SingleInput_TypeDef) -1)

#define IOT_ADC_DRV_SAR_INVALID           \
    { /* 0x00 */ adcSingleInputInvalid    }, \
    { /* 0x01 */ adcSingleInputInvalid    }, \
    { /* 0x02 */ adcSingleInputInvalid    }, \
    { /* 0x03 */ adcSingleInputInvalid    }, \
    { /* 0x04 */ adcSingleInputInvalid    }, \
    { /* 0x05 */ adcSingleInputInvalid    }, \
    { /* 0x06 */ adcSingleInputInvalid    }, \
    { /* 0x07 */ adcSingleInputInvalid    }, \
    { /* 0x08 */ adcSingleInputInvalid    }, \
    { /* 0x09 */ adcSingleInputInvalid    }, \
    { /* 0x0A */ adcSingleInputInvalid    }, \
    { /* 0x0B */ adcSingleInputInvalid    }, \
    { /* 0x0C */ adcSingleInputInvalid    }, \
    { /* 0x0D */ adcSingleInputInvalid    }, \
    { /* 0x0E */ adcSingleInputInvalid    }, \
    { /* 0x0F */ adcSingleInputInvalid    }, \

#define IOT_ADC_DRV_SAR_SPECIAL_0         \
    { /* 0x00 */ adcSingleInputVDD        }, \
    { /* 0x01 */ adcSingleInputInvalid    }, \
    { /* 0x02 */ adcSingleInputVDDDiv3    }, \
    { /* 0x03 */ adcSingleInputInvalid    }, \
    { /* 0x04 */ adcSingleInputInvalid    }, \
    { /* 0x05 */ adcSingleInputInvalid    }, \
    { /* 0x06 */ adcSingleInputInvalid    }, \
    { /* 0x07 */ adcSingleInputInvalid    }, \
    { /* 0x08 */ adcSingleInputInvalid    }, \
    { /* 0x09 */ adcSingleInputInvalid    }, \
    { /* 0x0A */ adcSingleInputInvalid    }, \
    { /* 0x0B */ adcSingleInputInvalid    }, \
    { /* 0x0C */ adcSingleInputInvalid    }, \
    { /* 0x0D */ adcSingleInputInvalid    }, \
    { /* 0x0E */ adcSingleInputInvalid    }, \
    { /* 0x0F */ adcSingleInputInvalid    }, \

#define IOT_ADC_DRV_SAR_SPECIAL_1         \
    { /* 0x00 */ adcSingleInputInvalid    }, \
    { /* 0x01 */ adcSingleInputInvalid    }, \
    { /* 0x02 */ adcSingleInputInvalid    }, \
    { /* 0x03 */ adcSingleInputTemp       }, \
    { /* 0x04 */ adcSingleInputDACOut0    }, \
    { /* 0x05 */ adcSingleInputInvalid    }, \
    { /* 0x06 */ adcSingleInputInvalid    }, \
    { /* 0x07 */ adcSingleInputInvalid    }, \
    { /* 0x08 */ adcSingleInputDACOut1    }, \
    { /* 0x09 */ adcSingleInputInvalid    }, \
    { /* 0x0A */ adcSingleInputInvalid    }, \
    { /* 0x0B */ adcSingleInputATEST      }, \
    { /* 0x0C */ adcSingleInputVrefDiv2   }, \
    { /* 0x0D */ adcSingleInputVSS        }, \
    { /* 0x0E */ adcSingleInputInvalid    }, \
    { /* 0x0F */ adcSingleInputVSS        }, \

#define IOT_ADC_DRV_SAR_PORT_D            \
    { /* PD0  */ adcSingleInputCh0        }, \
    { /* PD1  */ adcSingleInputCh1        }, \
    { /* PD2  */ adcSingleInputCh2        }, \
    { /* PD3  */ adcSingleInputCh3        }, \
    { /* PD4  */ adcSingleInputCh4        }, \
    { /* PD5  */ adcSingleInputCh5        }, \
    { /* PD6  */ adcSingleInputCh6        }, \
    { /* PD7  */ adcSingleInputCh7        }, \
    { /* PD8  */ adcSingleInputInvalid    }, \
    { /* PD9  */ adcSingleInputInvalid    }, \
    { /* PD10 */ adcSingleInputInvalid    }, \
    { /* PD11 */ adcSingleInputInvalid    }, \
    { /* PD12 */ adcSingleInputInvalid    }, \
    { /* PD13 */ adcSingleInputInvalid    }, \
    { /* PD14 */ adcSingleInputInvalid    }, \
    { /* PD15 */ adcSingleInputInvalid    }, \

#endif // (_SILICON_LABS_32B_SERIES == 0)

/*******************************************************************************
 *                            SERIES 1 MAPPINGS
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES == 1)

//                                   {SERIES 1}
//             PC9 - ADC0 Port 2X Channel 9 (Expansion Header Pin 10)
//             ------------------------------------------------------
// Board:  Silicon Labs EFM32PG1   Starter Kit (SLSTK3401A) + EFM32PG1B200F256GM48
// Board:  Silicon Labs EFM32PG12  Starter Kit (SLSTK3402A) + EFM32PG12B500F1024GL125
//
// Board:  Silicon Labs EFR32BG1P  Wir.  Board (BRD4100A)   + EFR32BG1P232F256GM48
// Board:  Silicon Labs EFR32BG12P Wir.  Board (BRD4103A)   + EFR32BG12P332F1024GL125
// Board:  Silicon Labs EFR32FG1P  Wir.  Board (BRD4250A)   + EFR32FG1P133F256GM48
// Board:  Silicon Labs EFR32FG12P Wir.  Board (BRD4253A)   + EFR32FG12P433F1024GL125
// Board:  Silicon Labs EFR32MG1P  Wir.  Board (BRD4151A)   + EFR32MG1P232F256GM48
//
// Board:  Silicon Labs EFR32BG13  Radio Board (SLWRB4104A) + EFR32BG13P632F512GM48
// Board:  Silicon Labs EFR32FG13  Radio Board (SLWRB4256A) + EFR32FG13P233F512GM48
// Board:  Silicon Labs EFR32MG12  Radio Board (SLWRB4161A) + EFR32MG12P432F1024GL125
// Board:  Silicon Labs EFR32MG13  Radio Board (SLWRB4159A) + EFR32MG13P632F512GM48
//
//
//                                {SERIES 1 xG14}
//             PA3 - ADC0 Port 4X Channel 11 (Expansion Header Pin 5)
//             -------------------------------------------------------
// Board:  Silicon Labs EFR32BG14 Radio Board (SLWRB4105A) + EFR32BG14P732F256GM48
// Board:  Silicon Labs EFR32FG14 Radio Board (SLWRB4257A) + EFR32FG14P233F256GM48
// Board:  Silicon Labs EFR32MG14 Radio Board (SLWRB4169A) + EFR32MG14P733F256GM48
//
//
//                                {SERIES 1 GG11}
//             PE11- ADC0 Port 4X Channel 11 (Expansion Header Pin 6)
//             ------------------------------------------------------
// Board:  Silicon Labs EFM32TG11 Starter Kit (SLSTK3301A) + EFM32TG11B520F128GM80
//
//                                {SERIES 1 TG11}
//             PD2 - ADC0 Port 0X Channel 2  (Expansion Header Pin 7)
//             ------------------------------------------------------
// Board:  Silicon Labs EFM32GG11 Starter Kit (SLSTK3701A) + EFM32GG11B820F2048GL192
//
//

//
// BUSAX=PA0|PA2|PA4|PA6|PA8|PA10|PA12|PA14|PB0|PB2|PB4|PB6|PB8|PB10|PB12|PB14
// BUSBX=PA1|PA3|PA5|PA7|PA9|PA11|PA13|PA15|PB1|PB3|PB5|PB7|PB9|PB11|PB13|PB15
// BUSCX=PE0|PE2|PE4|PE6|PE8|PE10|PE12|PE14|PF0|PF2|PF4|PF6|PF8|PF10|PF12|PF14
// BUSDX=PE1|PE3|PE5|PE7|PE9|PE11|PE13|PE15|PF1|PF3|PF5|PF7|PF9|PF11|PF13|PF15
//
// BUSAY=PA1|PA3|PA5|PA7|PA9|PA11|PA13|PA15|PB1|PB3|PB5|PB7|PB9|PB11|PB13|PB15
// BUSBY=PA0|PA2|PA4|PA6|PA8|PA10|PA12|PA14|PB0|PB2|PB4|PB6|PB8|PB10|PB12|PB14
// BUSCY=PE1|PE3|PE5|PE7|PE9|PE11|PE13|PE15|PF1|PF3|PF5|PF7|PF9|PF11|PF13|PF15
// BUSDY=PE0|PE2|PE4|PE6|PE8|PE10|PE12|PE14|PF0|PF2|PF4|PF6|PF8|PF10|PF12|PF14
//
// ADC0X=PD0|PD1|PD2|PD3|PD4|PD5|PD6|PD7
// ADC0Y=PD0|PD1|PD2|PD3|PD4|PD5|PD6|PD7
// ADC1X=PH0|PH1|PH2|PH3|PH4|PH5|PH6|PH7
// ADC1Y=PH0|PH1|PH2|PH3|PH4|PH5|PH6|PH7
//
//
//                      ADC0X  BUSAX  BUSBX  BUSCX  BUSDX    ?      ?
// +------+               |      |      |      |      |      |      |
// |      |<- POS = MUX(BUS0X, BUS1X, BUS2X, BUS3X, BUS4X, NEXT0, NEXT2)
// |      |<- NEG = MUX(BUS0Y, BUS1Y, BUS2Y, BUS3Y, BUS4Y, NEXT1, NEXT3)
// |      |               |      |      |      |      |      |      |
// | ADC0 |             ADC0Y  BUSAY  BUSBY  BUSCY  BUSDY    ?      ?
// |      |
// |      |-> EXTP
// |      |-> EXTN
// +------+
//
//                      ADC1X  BUSAX  BUSBX  BUSCX  BUSDX    ?      ?
// +------+               |      |      |      |      |      |      |
// |      |<- POS = MUX(BUS0X, BUS1X, BUS2X, BUS3X, BUS4X, NEXT0, NEXT2)
// |      |<- NEG = MUX(BUS0Y, BUS1Y, BUS2Y, BUS3Y, BUS4Y, NEXT1, NEXT3)
// |      |               |      |      |      |      |      |      |
// | ADC1 |             ADC1Y  BUSAY  BUSBY  BUSCY  BUSDY    ?      ?
// |      |
// |      |-> EXTP
// |      |-> EXTN
// +------+
//

                /*******************************************************************/
                /*                             Common                              */
                /*******************************************************************/

#define adcPosSelInvalid    ((ADC_PosSel_TypeDef) -1)
#define adcNegSelInvalid    ((ADC_NegSel_TypeDef) -1)

#define IOT_ADC_DRV_SAR_INVALID                                                                             \
    { /* 0x00 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x01 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x02 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x03 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x04 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x05 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x06 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x07 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x08 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x09 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0A */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0B */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0C */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0D */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0E */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0F */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \

#define IOT_ADC_DRV_SAR_SPECIAL_0                                                                           \
    { /* 0x00 */ {adcPosSelAVDD       , adcNegSelInvalid    }, {adcPosSelAVDD       , adcNegSelInvalid    } }, \
    { /* 0x01 */ {adcPosSelBUVDD      , adcNegSelInvalid    }, {adcPosSelBUVDD      , adcNegSelInvalid    } }, \
    { /* 0x02 */ {adcPosSelDVDD       , adcNegSelInvalid    }, {adcPosSelDVDD       , adcNegSelInvalid    } }, \
    { /* 0x03 */ {adcPosSelPAVDD      , adcNegSelInvalid    }, {adcPosSelPAVDD      , adcNegSelInvalid    } }, \
    { /* 0x04 */ {adcPosSelDECOUPLE   , adcNegSelInvalid    }, {adcPosSelDECOUPLE   , adcNegSelInvalid    } }, \
    { /* 0x05 */ {adcPosSelIOVDD      , adcNegSelInvalid    }, {adcPosSelIOVDD      , adcNegSelInvalid    } }, \
    { /* 0x06 */ {adcPosSelIOVDD1     , adcNegSelInvalid    }, {adcPosSelIOVDD1     , adcNegSelInvalid    } }, \
    { /* 0x07 */ {adcPosSelVSP        , adcNegSelInvalid    }, {adcPosSelVSP        , adcNegSelInvalid    } }, \
    { /* 0x08 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x09 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0A */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0B */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0C */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0D */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0E */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0F */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \

#define IOT_ADC_DRV_SAR_SPECIAL_1                                                                           \
    { /* 0x00 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x01 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x02 */ {adcPosSelOPA2       , adcNegSelInvalid    }, {adcPosSelOPA2       , adcNegSelInvalid    } }, \
    { /* 0x03 */ {adcPosSelTEMP       , adcNegSelInvalid    }, {adcPosSelTEMP       , adcNegSelInvalid    } }, \
    { /* 0x04 */ {adcPosSelDAC0OUT0   , adcNegSelInvalid    }, {adcPosSelDAC0OUT0   , adcNegSelInvalid    } }, \
    { /* 0x05 */ {adcPosSelR5VOUT     , adcNegSelInvalid    }, {adcPosSelR5VOUT     , adcNegSelInvalid    } }, \
    { /* 0x06 */ {adcPosSelSP1        , adcNegSelInvalid    }, {adcPosSelSP1        , adcNegSelInvalid    } }, \
    { /* 0x07 */ {adcPosSelSP2        , adcNegSelInvalid    }, {adcPosSelSP2        , adcNegSelInvalid    } }, \
    { /* 0x08 */ {adcPosSelDAC0OUT1   , adcNegSelInvalid    }, {adcPosSelDAC0OUT1   , adcNegSelInvalid    } }, \
    { /* 0x09 */ {adcPosSelSUBLSB     , adcNegSelInvalid    }, {adcPosSelSUBLSB     , adcNegSelInvalid    } }, \
    { /* 0x0A */ {adcPosSelOPA3       , adcNegSelInvalid    }, {adcPosSelOPA3       , adcNegSelInvalid    } }, \
    { /* 0x0B */ {adcPosSelInvalid    , adcNegSelTESTN      }, {adcPosSelInvalid    , adcNegSelTESTN      } }, \
    { /* 0x0C */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0D */ {adcPosSelVSS        , adcNegSelVSS        }, {adcPosSelVSS        , adcNegSelVSS        } }, \
    { /* 0x0E */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* 0x0F */ {adcPosSelVSS        , adcNegSelVSS        }, {adcPosSelVSS        , adcNegSelVSS        } }, \

                /*******************************************************************/
                /*                            EFM32GG11                            */
                /*******************************************************************/

#ifdef _EFM32_GIANT_FAMILY

#define IOT_ADC_DRV_SAR_PORT_A                                                                              \
    { /* PA0  */ {adcPosSelAPORT1XCH0 , adcNegSelAPORT2YCH0 }, {adcPosSelAPORT1XCH0 , adcNegSelAPORT2YCH0 } }, \
    { /* PA1  */ {adcPosSelAPORT2XCH1 , adcNegSelAPORT1YCH1 }, {adcPosSelAPORT2XCH1 , adcNegSelAPORT1YCH1 } }, \
    { /* PA2  */ {adcPosSelAPORT1XCH2 , adcNegSelAPORT2YCH2 }, {adcPosSelAPORT1XCH2 , adcNegSelAPORT2YCH2 } }, \
    { /* PA3  */ {adcPosSelAPORT2XCH3 , adcNegSelAPORT1YCH3 }, {adcPosSelAPORT2XCH3 , adcNegSelAPORT1YCH3 } }, \
    { /* PA4  */ {adcPosSelAPORT1XCH4 , adcNegSelAPORT2YCH4 }, {adcPosSelAPORT1XCH4 , adcNegSelAPORT2YCH4 } }, \
    { /* PA5  */ {adcPosSelAPORT2XCH5 , adcNegSelAPORT1YCH5 }, {adcPosSelAPORT2XCH5 , adcNegSelAPORT1YCH5 } }, \
    { /* PA6  */ {adcPosSelAPORT1XCH6 , adcNegSelAPORT2YCH6 }, {adcPosSelAPORT1XCH6 , adcNegSelAPORT2YCH6 } }, \
    { /* PA7  */ {adcPosSelAPORT2XCH7 , adcNegSelAPORT1YCH7 }, {adcPosSelAPORT2XCH7 , adcNegSelAPORT1YCH7 } }, \
    { /* PA8  */ {adcPosSelAPORT1XCH8 , adcNegSelAPORT2YCH8 }, {adcPosSelAPORT1XCH8 , adcNegSelAPORT2YCH8 } }, \
    { /* PA9  */ {adcPosSelAPORT2XCH9 , adcNegSelAPORT1YCH9 }, {adcPosSelAPORT2XCH9 , adcNegSelAPORT1YCH9 } }, \
    { /* PA10 */ {adcPosSelAPORT1XCH10, adcNegSelAPORT2YCH10}, {adcPosSelAPORT1XCH10, adcNegSelAPORT2YCH10} }, \
    { /* PA11 */ {adcPosSelAPORT2XCH11, adcNegSelAPORT1YCH11}, {adcPosSelAPORT2XCH11, adcNegSelAPORT1YCH11} }, \
    { /* PA12 */ {adcPosSelAPORT1XCH12, adcNegSelAPORT2YCH12}, {adcPosSelAPORT1XCH12, adcNegSelAPORT2YCH12} }, \
    { /* PA13 */ {adcPosSelAPORT2XCH13, adcNegSelAPORT1YCH13}, {adcPosSelAPORT2XCH13, adcNegSelAPORT1YCH13} }, \
    { /* PA14 */ {adcPosSelAPORT1XCH14, adcNegSelAPORT2YCH14}, {adcPosSelAPORT1XCH14, adcNegSelAPORT2YCH14} }, \
    { /* PA15 */ {adcPosSelAPORT2XCH15, adcNegSelAPORT1YCH15}, {adcPosSelAPORT2XCH15, adcNegSelAPORT1YCH15} }, \

#define IOT_ADC_DRV_SAR_PORT_B                                                                              \
    { /* PB0  */ {adcPosSelAPORT1XCH16, adcNegSelAPORT2YCH16}, {adcPosSelAPORT1XCH16, adcNegSelAPORT2YCH16} }, \
    { /* PB1  */ {adcPosSelAPORT2XCH17, adcNegSelAPORT1YCH17}, {adcPosSelAPORT2XCH17, adcNegSelAPORT1YCH17} }, \
    { /* PB2  */ {adcPosSelAPORT1XCH18, adcNegSelAPORT2YCH18}, {adcPosSelAPORT1XCH18, adcNegSelAPORT2YCH18} }, \
    { /* PB3  */ {adcPosSelAPORT2XCH19, adcNegSelAPORT1YCH19}, {adcPosSelAPORT2XCH19, adcNegSelAPORT1YCH19} }, \
    { /* PB4  */ {adcPosSelAPORT1XCH20, adcNegSelAPORT2YCH20}, {adcPosSelAPORT1XCH20, adcNegSelAPORT2YCH20} }, \
    { /* PB5  */ {adcPosSelAPORT2XCH21, adcNegSelAPORT1YCH21}, {adcPosSelAPORT2XCH21, adcNegSelAPORT1YCH21} }, \
    { /* PB6  */ {adcPosSelAPORT1XCH22, adcNegSelAPORT2YCH22}, {adcPosSelAPORT1XCH22, adcNegSelAPORT2YCH22} }, \
    { /* PB7  */ {adcPosSelAPORT2XCH23, adcNegSelAPORT1YCH23}, {adcPosSelAPORT2XCH23, adcNegSelAPORT1YCH23} }, \
    { /* PB8  */ {adcPosSelAPORT1XCH24, adcNegSelAPORT2YCH24}, {adcPosSelAPORT1XCH24, adcNegSelAPORT2YCH24} }, \
    { /* PB9  */ {adcPosSelAPORT2XCH25, adcNegSelAPORT1YCH25}, {adcPosSelAPORT2XCH25, adcNegSelAPORT1YCH25} }, \
    { /* PB10 */ {adcPosSelAPORT1XCH26, adcNegSelAPORT2YCH26}, {adcPosSelAPORT1XCH26, adcNegSelAPORT2YCH26} }, \
    { /* PB11 */ {adcPosSelAPORT2XCH27, adcNegSelAPORT1YCH27}, {adcPosSelAPORT2XCH27, adcNegSelAPORT1YCH27} }, \
    { /* PB12 */ {adcPosSelAPORT1XCH28, adcNegSelAPORT2YCH28}, {adcPosSelAPORT1XCH28, adcNegSelAPORT2YCH28} }, \
    { /* PB13 */ {adcPosSelAPORT2XCH29, adcNegSelAPORT1YCH29}, {adcPosSelAPORT2XCH29, adcNegSelAPORT1YCH29} }, \
    { /* PB14 */ {adcPosSelAPORT1XCH30, adcNegSelAPORT2YCH30}, {adcPosSelAPORT1XCH30, adcNegSelAPORT2YCH30} }, \
    { /* PB15 */ {adcPosSelAPORT2XCH31, adcNegSelAPORT1YCH31}, {adcPosSelAPORT2XCH31, adcNegSelAPORT1YCH31} }, \

#define IOT_ADC_DRV_SAR_PORT_D                                                                              \
    { /* PD0  */ {adcPosSelAPORT0XCH0 , adcNegSelAPORT0YCH0 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD1  */ {adcPosSelAPORT0XCH1 , adcNegSelAPORT0YCH1 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD2  */ {adcPosSelAPORT0XCH2 , adcNegSelAPORT0YCH2 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD3  */ {adcPosSelAPORT0XCH3 , adcNegSelAPORT0YCH3 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD4  */ {adcPosSelAPORT0XCH4 , adcNegSelAPORT0YCH4 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD5  */ {adcPosSelAPORT0XCH5 , adcNegSelAPORT0YCH5 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD6  */ {adcPosSelAPORT0XCH6 , adcNegSelAPORT0YCH6 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD7  */ {adcPosSelAPORT0XCH7 , adcNegSelAPORT0YCH7 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD8  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD9  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD10 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD11 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD12 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD13 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD14 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD15 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \

#define IOT_ADC_DRV_SAR_PORT_E                                                                              \
    { /* PE0  */ {adcPosSelAPORT3XCH0 , adcNegSelAPORT4YCH0 }, {adcPosSelAPORT3XCH0 , adcNegSelAPORT4YCH0 } }, \
    { /* PE1  */ {adcPosSelAPORT4XCH1 , adcNegSelAPORT3YCH1 }, {adcPosSelAPORT4XCH1 , adcNegSelAPORT3YCH1 } }, \
    { /* PE2  */ {adcPosSelAPORT3XCH2 , adcNegSelAPORT4YCH2 }, {adcPosSelAPORT3XCH2 , adcNegSelAPORT4YCH2 } }, \
    { /* PE3  */ {adcPosSelAPORT4XCH3 , adcNegSelAPORT3YCH3 }, {adcPosSelAPORT4XCH3 , adcNegSelAPORT3YCH3 } }, \
    { /* PE4  */ {adcPosSelAPORT3XCH4 , adcNegSelAPORT4YCH4 }, {adcPosSelAPORT3XCH4 , adcNegSelAPORT4YCH4 } }, \
    { /* PE5  */ {adcPosSelAPORT4XCH5 , adcNegSelAPORT3YCH5 }, {adcPosSelAPORT4XCH5 , adcNegSelAPORT3YCH5 } }, \
    { /* PE6  */ {adcPosSelAPORT3XCH6 , adcNegSelAPORT4YCH6 }, {adcPosSelAPORT3XCH6 , adcNegSelAPORT4YCH6 } }, \
    { /* PE7  */ {adcPosSelAPORT4XCH7 , adcNegSelAPORT3YCH7 }, {adcPosSelAPORT4XCH7 , adcNegSelAPORT3YCH7 } }, \
    { /* PE8  */ {adcPosSelAPORT3XCH8 , adcNegSelAPORT4YCH8 }, {adcPosSelAPORT3XCH8 , adcNegSelAPORT4YCH8 } }, \
    { /* PE9  */ {adcPosSelAPORT4XCH9 , adcNegSelAPORT3YCH9 }, {adcPosSelAPORT4XCH9 , adcNegSelAPORT3YCH9 } }, \
    { /* PE10 */ {adcPosSelAPORT3XCH10, adcNegSelAPORT4YCH10}, {adcPosSelAPORT3XCH10, adcNegSelAPORT4YCH10} }, \
    { /* PE11 */ {adcPosSelAPORT4XCH11, adcNegSelAPORT3YCH11}, {adcPosSelAPORT4XCH11, adcNegSelAPORT3YCH11} }, \
    { /* PE12 */ {adcPosSelAPORT3XCH12, adcNegSelAPORT4YCH12}, {adcPosSelAPORT3XCH12, adcNegSelAPORT4YCH12} }, \
    { /* PE13 */ {adcPosSelAPORT4XCH13, adcNegSelAPORT3YCH13}, {adcPosSelAPORT4XCH13, adcNegSelAPORT3YCH13} }, \
    { /* PE14 */ {adcPosSelAPORT3XCH14, adcNegSelAPORT4YCH14}, {adcPosSelAPORT3XCH14, adcNegSelAPORT4YCH14} }, \
    { /* PE15 */ {adcPosSelAPORT4XCH15, adcNegSelAPORT3YCH15}, {adcPosSelAPORT4XCH15, adcNegSelAPORT3YCH15} }, \

#define IOT_ADC_DRV_SAR_PORT_F                                                                              \
    { /* PF0  */ {adcPosSelAPORT3XCH16, adcNegSelAPORT4YCH16}, {adcPosSelAPORT3XCH16, adcNegSelAPORT4YCH16} }, \
    { /* PF1  */ {adcPosSelAPORT4XCH17, adcNegSelAPORT3YCH17}, {adcPosSelAPORT4XCH17, adcNegSelAPORT3YCH17} }, \
    { /* PF2  */ {adcPosSelAPORT3XCH18, adcNegSelAPORT4YCH18}, {adcPosSelAPORT3XCH18, adcNegSelAPORT4YCH18} }, \
    { /* PF3  */ {adcPosSelAPORT4XCH19, adcNegSelAPORT3YCH19}, {adcPosSelAPORT4XCH19, adcNegSelAPORT3YCH19} }, \
    { /* PF4  */ {adcPosSelAPORT3XCH20, adcNegSelAPORT4YCH20}, {adcPosSelAPORT3XCH20, adcNegSelAPORT4YCH20} }, \
    { /* PF5  */ {adcPosSelAPORT4XCH21, adcNegSelAPORT3YCH21}, {adcPosSelAPORT4XCH21, adcNegSelAPORT3YCH21} }, \
    { /* PF6  */ {adcPosSelAPORT3XCH22, adcNegSelAPORT4YCH22}, {adcPosSelAPORT3XCH22, adcNegSelAPORT4YCH22} }, \
    { /* PF7  */ {adcPosSelAPORT4XCH23, adcNegSelAPORT3YCH23}, {adcPosSelAPORT4XCH23, adcNegSelAPORT3YCH23} }, \
    { /* PF8  */ {adcPosSelAPORT3XCH24, adcNegSelAPORT4YCH24}, {adcPosSelAPORT3XCH24, adcNegSelAPORT4YCH24} }, \
    { /* PF9  */ {adcPosSelAPORT4XCH25, adcNegSelAPORT3YCH25}, {adcPosSelAPORT4XCH25, adcNegSelAPORT3YCH25} }, \
    { /* PF10 */ {adcPosSelAPORT3XCH26, adcNegSelAPORT4YCH26}, {adcPosSelAPORT3XCH26, adcNegSelAPORT4YCH26} }, \
    { /* PF11 */ {adcPosSelAPORT4XCH27, adcNegSelAPORT3YCH27}, {adcPosSelAPORT4XCH27, adcNegSelAPORT3YCH27} }, \
    { /* PF12 */ {adcPosSelAPORT3XCH28, adcNegSelAPORT4YCH28}, {adcPosSelAPORT3XCH28, adcNegSelAPORT4YCH28} }, \
    { /* PF13 */ {adcPosSelAPORT4XCH29, adcNegSelAPORT3YCH29}, {adcPosSelAPORT4XCH29, adcNegSelAPORT3YCH29} }, \
    { /* PF14 */ {adcPosSelAPORT3XCH30, adcNegSelAPORT4YCH30}, {adcPosSelAPORT3XCH30, adcNegSelAPORT4YCH30} }, \
    { /* PF15 */ {adcPosSelAPORT4XCH31, adcNegSelAPORT3YCH31}, {adcPosSelAPORT4XCH31, adcNegSelAPORT3YCH31} }, \

#define IOT_ADC_DRV_SAR_PORT_H                                                                              \
    { /* PH0  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH0 , adcNegSelAPORT0YCH0 } }, \
    { /* PH1  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH1 , adcNegSelAPORT0YCH1 } }, \
    { /* PH2  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH2 , adcNegSelAPORT0YCH2 } }, \
    { /* PH3  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH3 , adcNegSelAPORT0YCH3 } }, \
    { /* PH4  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH4 , adcNegSelAPORT0YCH4 } }, \
    { /* PH5  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH5 , adcNegSelAPORT0YCH5 } }, \
    { /* PH6  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH6 , adcNegSelAPORT0YCH6 } }, \
    { /* PH7  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelAPORT0XCH7 , adcNegSelAPORT0YCH7 } }, \
    { /* PH8  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH9  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH10 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH11 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH12 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH13 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH14 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PH15 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \

                /*******************************************************************/
                /*                      ALL OTHER SERIES 1                         */
                /*******************************************************************/

#else

#define IOT_ADC_DRV_SAR_PORT_A                                                                              \
    { /* PA0  */ {adcPosSelAPORT3XCH8 , adcNegSelAPORT4YCH8 }, {adcPosSelAPORT3XCH8 , adcNegSelAPORT4YCH8 } }, \
    { /* PA1  */ {adcPosSelAPORT4XCH9 , adcNegSelAPORT3YCH9 }, {adcPosSelAPORT4XCH9 , adcNegSelAPORT3YCH9 } }, \
    { /* PA2  */ {adcPosSelAPORT3XCH10, adcNegSelAPORT4YCH10}, {adcPosSelAPORT3XCH10, adcNegSelAPORT4YCH10} }, \
    { /* PA3  */ {adcPosSelAPORT4XCH11, adcNegSelAPORT3YCH11}, {adcPosSelAPORT4XCH11, adcNegSelAPORT3YCH11} }, \
    { /* PA4  */ {adcPosSelAPORT3XCH12, adcNegSelAPORT4YCH12}, {adcPosSelAPORT3XCH12, adcNegSelAPORT4YCH12} }, \
    { /* PA5  */ {adcPosSelAPORT4XCH13, adcNegSelAPORT3YCH13}, {adcPosSelAPORT4XCH13, adcNegSelAPORT3YCH13} }, \
    { /* PA6  */ {adcPosSelAPORT3XCH14, adcNegSelAPORT4YCH14}, {adcPosSelAPORT3XCH14, adcNegSelAPORT4YCH14} }, \
    { /* PA7  */ {adcPosSelAPORT4XCH15, adcNegSelAPORT3YCH15}, {adcPosSelAPORT4XCH15, adcNegSelAPORT3YCH15} }, \
    { /* PA8  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA9  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA10 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA11 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA12 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA13 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA14 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PA15 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \

#define IOT_ADC_DRV_SAR_PORT_B                                                                              \
    { /* PB0  */ {adcPosSelAPORT3XCH16, adcNegSelAPORT4YCH16}, {adcPosSelAPORT3XCH16, adcNegSelAPORT4YCH16} }, \
    { /* PB1  */ {adcPosSelAPORT4XCH17, adcNegSelAPORT3YCH17}, {adcPosSelAPORT4XCH17, adcNegSelAPORT3YCH17} }, \
    { /* PB2  */ {adcPosSelAPORT3XCH18, adcNegSelAPORT4YCH18}, {adcPosSelAPORT3XCH18, adcNegSelAPORT4YCH18} }, \
    { /* PB3  */ {adcPosSelAPORT4XCH19, adcNegSelAPORT3YCH19}, {adcPosSelAPORT4XCH19, adcNegSelAPORT3YCH19} }, \
    { /* PB4  */ {adcPosSelAPORT3XCH20, adcNegSelAPORT4YCH20}, {adcPosSelAPORT3XCH20, adcNegSelAPORT4YCH20} }, \
    { /* PB5  */ {adcPosSelAPORT4XCH21, adcNegSelAPORT3YCH21}, {adcPosSelAPORT4XCH21, adcNegSelAPORT3YCH21} }, \
    { /* PB6  */ {adcPosSelAPORT3XCH22, adcNegSelAPORT4YCH22}, {adcPosSelAPORT3XCH22, adcNegSelAPORT4YCH22} }, \
    { /* PB7  */ {adcPosSelAPORT4XCH23, adcNegSelAPORT3YCH23}, {adcPosSelAPORT4XCH23, adcNegSelAPORT3YCH23} }, \
    { /* PB8  */ {adcPosSelAPORT3XCH24, adcNegSelAPORT4YCH24}, {adcPosSelAPORT3XCH24, adcNegSelAPORT4YCH24} }, \
    { /* PB9  */ {adcPosSelAPORT4XCH25, adcNegSelAPORT3YCH25}, {adcPosSelAPORT4XCH25, adcNegSelAPORT3YCH25} }, \
    { /* PB10 */ {adcPosSelAPORT3XCH26, adcNegSelAPORT4YCH26}, {adcPosSelAPORT3XCH26, adcNegSelAPORT4YCH26} }, \
    { /* PB11 */ {adcPosSelAPORT4XCH27, adcNegSelAPORT3YCH27}, {adcPosSelAPORT4XCH27, adcNegSelAPORT3YCH27} }, \
    { /* PB12 */ {adcPosSelAPORT3XCH28, adcNegSelAPORT4YCH28}, {adcPosSelAPORT3XCH28, adcNegSelAPORT4YCH28} }, \
    { /* PB13 */ {adcPosSelAPORT4XCH29, adcNegSelAPORT3YCH29}, {adcPosSelAPORT4XCH29, adcNegSelAPORT3YCH29} }, \
    { /* PB14 */ {adcPosSelAPORT3XCH30, adcNegSelAPORT4YCH30}, {adcPosSelAPORT3XCH30, adcNegSelAPORT4YCH30} }, \
    { /* PB15 */ {adcPosSelAPORT4XCH31, adcNegSelAPORT3YCH31}, {adcPosSelAPORT4XCH31, adcNegSelAPORT3YCH31} }, \

#define IOT_ADC_DRV_SAR_PORT_C                                                                              \
    { /* PC0  */ {adcPosSelAPORT1XCH0 , adcNegSelAPORT2YCH0 }, {adcPosSelAPORT1XCH0 , adcNegSelAPORT2YCH0 } }, \
    { /* PC1  */ {adcPosSelAPORT2XCH1 , adcNegSelAPORT1YCH1 }, {adcPosSelAPORT2XCH1 , adcNegSelAPORT1YCH1 } }, \
    { /* PC2  */ {adcPosSelAPORT1XCH2 , adcNegSelAPORT2YCH2 }, {adcPosSelAPORT1XCH2 , adcNegSelAPORT2YCH2 } }, \
    { /* PC3  */ {adcPosSelAPORT2XCH3 , adcNegSelAPORT1YCH3 }, {adcPosSelAPORT2XCH3 , adcNegSelAPORT1YCH3 } }, \
    { /* PC4  */ {adcPosSelAPORT1XCH4 , adcNegSelAPORT2YCH4 }, {adcPosSelAPORT1XCH4 , adcNegSelAPORT2YCH4 } }, \
    { /* PC5  */ {adcPosSelAPORT2XCH5 , adcNegSelAPORT1YCH5 }, {adcPosSelAPORT2XCH5 , adcNegSelAPORT1YCH5 } }, \
    { /* PC6  */ {adcPosSelAPORT1XCH6 , adcNegSelAPORT2YCH6 }, {adcPosSelAPORT1XCH6 , adcNegSelAPORT2YCH6 } }, \
    { /* PC7  */ {adcPosSelAPORT2XCH7 , adcNegSelAPORT1YCH7 }, {adcPosSelAPORT2XCH7 , adcNegSelAPORT1YCH7 } }, \
    { /* PC8  */ {adcPosSelAPORT1XCH8 , adcNegSelAPORT2YCH8 }, {adcPosSelAPORT1XCH8 , adcNegSelAPORT2YCH8 } }, \
    { /* PC9  */ {adcPosSelAPORT2XCH9 , adcNegSelAPORT1YCH9 }, {adcPosSelAPORT2XCH9 , adcNegSelAPORT1YCH9 } }, \
    { /* PC10 */ {adcPosSelAPORT1XCH10, adcNegSelAPORT2YCH10}, {adcPosSelAPORT1XCH10, adcNegSelAPORT2YCH10} }, \
    { /* PC11 */ {adcPosSelAPORT2XCH11, adcNegSelAPORT1YCH11}, {adcPosSelAPORT2XCH11, adcNegSelAPORT1YCH11} }, \
    { /* PC12 */ {adcPosSelAPORT1XCH12, adcNegSelAPORT2YCH12}, {adcPosSelAPORT1XCH12, adcNegSelAPORT2YCH12} }, \
    { /* PC13 */ {adcPosSelAPORT2XCH13, adcNegSelAPORT1YCH13}, {adcPosSelAPORT2XCH13, adcNegSelAPORT1YCH13} }, \
    { /* PC14 */ {adcPosSelAPORT1XCH14, adcNegSelAPORT2YCH14}, {adcPosSelAPORT1XCH14, adcNegSelAPORT2YCH14} }, \
    { /* PC15 */ {adcPosSelAPORT2XCH15, adcNegSelAPORT1YCH15}, {adcPosSelAPORT2XCH15, adcNegSelAPORT1YCH15} }, \

#define IOT_ADC_DRV_SAR_PORT_D                                                                              \
    { /* PD0  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD1  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD2  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD3  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD4  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD5  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD6  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD7  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PD8  */ {adcPosSelAPORT3XCH0 , adcNegSelAPORT4YCH0 }, {adcPosSelAPORT3XCH0 , adcNegSelAPORT4YCH0 } }, \
    { /* PD9  */ {adcPosSelAPORT4XCH1 , adcNegSelAPORT3YCH1 }, {adcPosSelAPORT4XCH1 , adcNegSelAPORT3YCH1 } }, \
    { /* PD10 */ {adcPosSelAPORT3XCH2 , adcNegSelAPORT4YCH2 }, {adcPosSelAPORT3XCH2 , adcNegSelAPORT4YCH2 } }, \
    { /* PD11 */ {adcPosSelAPORT4XCH3 , adcNegSelAPORT3YCH3 }, {adcPosSelAPORT4XCH3 , adcNegSelAPORT3YCH3 } }, \
    { /* PD12 */ {adcPosSelAPORT3XCH4 , adcNegSelAPORT4YCH4 }, {adcPosSelAPORT3XCH4 , adcNegSelAPORT4YCH4 } }, \
    { /* PD13 */ {adcPosSelAPORT4XCH5 , adcNegSelAPORT3YCH5 }, {adcPosSelAPORT4XCH5 , adcNegSelAPORT3YCH5 } }, \
    { /* PD14 */ {adcPosSelAPORT3XCH6 , adcNegSelAPORT4YCH6 }, {adcPosSelAPORT3XCH6 , adcNegSelAPORT4YCH6 } }, \
    { /* PD15 */ {adcPosSelAPORT4XCH7 , adcNegSelAPORT3YCH7 }, {adcPosSelAPORT4XCH7 , adcNegSelAPORT3YCH7 } }, \

#define IOT_ADC_DRV_SAR_PORT_F                                                                              \
    { /* PF0  */ {adcPosSelAPORT1XCH16, adcNegSelAPORT2YCH16}, {adcPosSelAPORT1XCH16, adcNegSelAPORT2YCH16} }, \
    { /* PF1  */ {adcPosSelAPORT2XCH17, adcNegSelAPORT1YCH17}, {adcPosSelAPORT2XCH17, adcNegSelAPORT1YCH17} }, \
    { /* PF2  */ {adcPosSelAPORT1XCH18, adcNegSelAPORT2YCH18}, {adcPosSelAPORT1XCH18, adcNegSelAPORT2YCH18} }, \
    { /* PF3  */ {adcPosSelAPORT2XCH19, adcNegSelAPORT1YCH19}, {adcPosSelAPORT2XCH19, adcNegSelAPORT1YCH19} }, \
    { /* PF4  */ {adcPosSelAPORT1XCH20, adcNegSelAPORT2YCH20}, {adcPosSelAPORT1XCH20, adcNegSelAPORT2YCH20} }, \
    { /* PF5  */ {adcPosSelAPORT2XCH21, adcNegSelAPORT1YCH21}, {adcPosSelAPORT2XCH21, adcNegSelAPORT1YCH21} }, \
    { /* PF6  */ {adcPosSelAPORT1XCH22, adcNegSelAPORT2YCH22}, {adcPosSelAPORT1XCH22, adcNegSelAPORT2YCH22} }, \
    { /* PF7  */ {adcPosSelAPORT2XCH23, adcNegSelAPORT1YCH23}, {adcPosSelAPORT2XCH23, adcNegSelAPORT1YCH23} }, \
    { /* PF8  */ {adcPosSelAPORT1XCH24, adcNegSelAPORT2YCH24}, {adcPosSelAPORT1XCH24, adcNegSelAPORT2YCH24} }, \
    { /* PF9  */ {adcPosSelAPORT2XCH25, adcNegSelAPORT1YCH25}, {adcPosSelAPORT2XCH25, adcNegSelAPORT1YCH25} }, \
    { /* PF10 */ {adcPosSelAPORT1XCH26, adcNegSelAPORT2YCH26}, {adcPosSelAPORT1XCH26, adcNegSelAPORT2YCH26} }, \
    { /* PF11 */ {adcPosSelAPORT2XCH27, adcNegSelAPORT1YCH27}, {adcPosSelAPORT2XCH27, adcNegSelAPORT1YCH27} }, \
    { /* PF12 */ {adcPosSelAPORT1XCH28, adcNegSelAPORT2YCH28}, {adcPosSelAPORT1XCH28, adcNegSelAPORT2YCH28} }, \
    { /* PF13 */ {adcPosSelAPORT2XCH29, adcNegSelAPORT1YCH29}, {adcPosSelAPORT2XCH29, adcNegSelAPORT1YCH29} }, \
    { /* PF14 */ {adcPosSelAPORT1XCH30, adcNegSelAPORT2YCH30}, {adcPosSelAPORT1XCH30, adcNegSelAPORT2YCH30} }, \
    { /* PF15 */ {adcPosSelAPORT2XCH31, adcNegSelAPORT1YCH31}, {adcPosSelAPORT2XCH31, adcNegSelAPORT1YCH31} }, \

#define IOT_ADC_DRV_SAR_PORT_I                                                                              \
    { /* PI0  */ {adcPosSelAPORT0XCH0 , adcNegSelAPORT0YCH0 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI1  */ {adcPosSelAPORT0XCH1 , adcNegSelAPORT0YCH1 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI2  */ {adcPosSelAPORT0XCH2 , adcNegSelAPORT0YCH2 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI3  */ {adcPosSelAPORT0XCH3 , adcNegSelAPORT0YCH3 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI4  */ {adcPosSelAPORT0XCH4 , adcNegSelAPORT0YCH4 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI5  */ {adcPosSelAPORT0XCH5 , adcNegSelAPORT0YCH5 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI6  */ {adcPosSelAPORT0XCH6 , adcNegSelAPORT0YCH6 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI7  */ {adcPosSelAPORT0XCH7 , adcNegSelAPORT0YCH7 }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI8  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI9  */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI10 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI11 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI12 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI13 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI14 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \
    { /* PI15 */ {adcPosSelInvalid    , adcNegSelInvalid    }, {adcPosSelInvalid    , adcNegSelInvalid    } }, \

#endif // <FAMILY>

#endif // (_SILICON_LABS_32B_SERIES == 1)

/*******************************************************************************
 *                            ALL BOARDS
 ******************************************************************************/

#if !defined(IOT_ADC_DRV_SAR_SPECIAL_0)
#define IOT_ADC_DRV_SAR_SPECIAL_0         IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_SPECIAL_1)
#define IOT_ADC_DRV_SAR_SPECIAL_1         IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_RESERVED_0)
#define IOT_ADC_DRV_SAR_RESERVED_0        IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_RESERVED_1)
#define IOT_ADC_DRV_SAR_RESERVED_1        IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_A)
#define IOT_ADC_DRV_SAR_PORT_A            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_B)
#define IOT_ADC_DRV_SAR_PORT_B            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_C)
#define IOT_ADC_DRV_SAR_PORT_C            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_D)
#define IOT_ADC_DRV_SAR_PORT_D            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_E)
#define IOT_ADC_DRV_SAR_PORT_E            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_F)
#define IOT_ADC_DRV_SAR_PORT_F            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_G)
#define IOT_ADC_DRV_SAR_PORT_G            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_H)
#define IOT_ADC_DRV_SAR_PORT_H            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_I)
#define IOT_ADC_DRV_SAR_PORT_I            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_J)
#define IOT_ADC_DRV_SAR_PORT_J            IOT_ADC_DRV_SAR_INVALID
#endif

#if !defined(IOT_ADC_DRV_SAR_PORT_K)
#define IOT_ADC_DRV_SAR_PORT_K            IOT_ADC_DRV_SAR_INVALID
#endif

#define IOT_ADC_DRV_SAR_MAPPING                      \
  {                                                     \
    /* 0x0X */ IOT_ADC_DRV_SAR_INVALID               \
    /* 0x1X */ IOT_ADC_DRV_SAR_PORT_I                \
    /* 0x2X */ IOT_ADC_DRV_SAR_SPECIAL_0             \
    /* 0x3X */ IOT_ADC_DRV_SAR_SPECIAL_1             \
    /* 0x4X */ IOT_ADC_DRV_SAR_RESERVED_0            \
    /* 0x5X */ IOT_ADC_DRV_SAR_RESERVED_1            \
    /* 0x6X */ IOT_ADC_DRV_SAR_PORT_G                \
    /* 0x7X */ IOT_ADC_DRV_SAR_PORT_J                \
    /* 0x8X */ IOT_ADC_DRV_SAR_PORT_H                \
    /* 0x9X */ IOT_ADC_DRV_SAR_PORT_K                \
    /* 0xAX */ IOT_ADC_DRV_SAR_PORT_A                \
    /* 0xBX */ IOT_ADC_DRV_SAR_PORT_B                \
    /* 0xCX */ IOT_ADC_DRV_SAR_PORT_C                \
    /* 0xDX */ IOT_ADC_DRV_SAR_PORT_D                \
    /* 0xEX */ IOT_ADC_DRV_SAR_PORT_E                \
    /* 0xFX */ IOT_ADC_DRV_SAR_PORT_F                \
  }

/*******************************************************************************
*                             END OF MODULE
******************************************************************************/

#endif

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_ADC_DRV_SAR_H_ */
