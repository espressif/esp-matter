/***************************************************************************//**
 * @file    iot_adc_drv_inc.h
 * @brief   Incremental ADC driver header file
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

#ifndef _IOT_ADC_DRV_INC_H_
#define _IOT_ADC_DRV_INC_H_

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

#include "em_core.h"
#include "em_device.h"

#include "sl_status.h"

/*******************************************************************************
 *                             START OF MODULE
 ******************************************************************************/

#if (_SILICON_LABS_32B_SERIES == 2)

/*******************************************************************************
 *                            SERIES 2 INCLUDES
 ******************************************************************************/

#include "em_iadc.h"

/*******************************************************************************
 *                               PROTOTYPES
 ******************************************************************************/

/* driver control */
sl_status_t iot_adc_drv_inc_driver_init(void *pvHndl);
sl_status_t iot_adc_drv_inc_driver_deinit(void *pvHndl);

/* driver configuration */
sl_status_t iot_adc_drv_inc_config_set(void *pvHndl,
                                          uint32_t ulAq,
                                          uint8_t ucRs);
sl_status_t iot_adc_drv_inc_config_get(void *pvHndl,
                                          uint32_t *pulAq,
                                          uint8_t *pucRs);

/* h/w control */
sl_status_t iot_adc_drv_inc_hw_enable(void *pvHndl);
sl_status_t iot_adc_drv_inc_hw_disable(void *pvHndl);

/* bus control */
sl_status_t iot_adc_drv_inc_bus_alloc(void *pvHndl, uint8_t ucChannel);
sl_status_t iot_adc_drv_inc_bus_dealloc(void *pvHndl, uint8_t ucChannel);

/* single transfers */
sl_status_t iot_adc_drv_inc_single_init(void *pvHndl, uint8_t ucCh);
sl_status_t iot_adc_drv_inc_single_start(void *pvHndl);
sl_status_t iot_adc_drv_inc_single_read(void *pvHndl, uint16_t *pusSample);

/* multi transfers */
sl_status_t iot_adc_drv_inc_multi_init(void *pvHndl, uint8_t ucCh);
sl_status_t iot_adc_drv_inc_multi_start(void *pvHndl);
sl_status_t iot_adc_drv_inc_multi_read(void *pvHndl, uint16_t *pusSample);

/* irq transfers */
sl_status_t iot_adc_drv_inc_irq_enable(void *pvHndl);
sl_status_t iot_adc_drv_inc_irq_disable(void *pvHndl);

/*******************************************************************************
 *                            CHANNEL MAPPING
 ******************************************************************************/

typedef struct iot_adc_drv_inc_map {
  IADC_PosInput_t         posInput;
  IADC_NegInput_t         negInput;
} iot_adc_drv_inc_map_t[256];

/*******************************************************************************
 *                             SERIES 2 MACROS
 ******************************************************************************/

#define iadcPosInputInvalid    ((IADC_PosInput_t) -1)
#define iadcNegInputInvalid    ((IADC_NegInput_t) -1)

#define IOT_ADC_DRV_INC_INVALID                                 \
    /* 0x00 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x01 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x02 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x03 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x04 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x05 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x06 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x07 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x08 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x09 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0A */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0B */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0C */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0D */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0E */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0F */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \

#define IOT_ADC_DRV_INC_SPECIAL_0                               \
    /* 0x00 */ {iadcPosInputAvdd       , iadcNegInputInvalid    }, \
    /* 0x01 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x02 */ {iadcPosInputDvdd       , iadcNegInputInvalid    }, \
    /* 0x03 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x04 */ {iadcPosInputDecouple   , iadcNegInputInvalid    }, \
    /* 0x05 */ {iadcPosInputVddio      , iadcNegInputInvalid    }, \
    /* 0x06 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x07 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x08 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x09 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0A */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0B */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0C */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0D */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0E */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0F */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \

#define IOT_ADC_DRV_INC_SPECIAL_1                               \
    /* 0x00 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x01 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x02 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x03 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x04 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x05 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x06 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x07 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x08 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x09 */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0A */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0B */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0C */ {iadcPosInputInvalid    , iadcNegInputInvalid    }, \
    /* 0x0D */ {iadcPosInputGnd        , iadcNegInputGnd        }, \
    /* 0x0E */ {iadcPosInputVssaux     , iadcNegInputInvalid    }, \
    /* 0x0F */ {iadcPosInputVss        , iadcNegInputInvalid    }, \

#define IOT_ADC_DRV_INC_PORT_A                                  \
    /* PA0  */ {iadcPosInputPortAPin0  , iadcNegInputPortAPin0  }, \
    /* PA1  */ {iadcPosInputPortAPin1  , iadcNegInputPortAPin1  }, \
    /* PA2  */ {iadcPosInputPortAPin2  , iadcNegInputPortAPin2  }, \
    /* PA3  */ {iadcPosInputPortAPin3  , iadcNegInputPortAPin3  }, \
    /* PA4  */ {iadcPosInputPortAPin4  , iadcNegInputPortAPin4  }, \
    /* PA5  */ {iadcPosInputPortAPin5  , iadcNegInputPortAPin5  }, \
    /* PA6  */ {iadcPosInputPortAPin6  , iadcNegInputPortAPin6  }, \
    /* PA7  */ {iadcPosInputPortAPin7  , iadcNegInputPortAPin7  }, \
    /* PA8  */ {iadcPosInputPortAPin8  , iadcNegInputPortAPin8  }, \
    /* PA9  */ {iadcPosInputPortAPin9  , iadcNegInputPortAPin9  }, \
    /* PA10 */ {iadcPosInputPortAPin10 , iadcNegInputPortAPin10 }, \
    /* PA11 */ {iadcPosInputPortAPin11 , iadcNegInputPortAPin11 }, \
    /* PA12 */ {iadcPosInputPortAPin12 , iadcNegInputPortAPin12 }, \
    /* PA13 */ {iadcPosInputPortAPin13 , iadcNegInputPortAPin13 }, \
    /* PA14 */ {iadcPosInputPortAPin14 , iadcNegInputPortAPin14 }, \
    /* PA15 */ {iadcPosInputPortAPin15 , iadcNegInputPortAPin15 }, \

#define IOT_ADC_DRV_INC_PORT_B                                  \
    /* PB0  */ {iadcPosInputPortBPin0  , iadcNegInputPortBPin0  }, \
    /* PB1  */ {iadcPosInputPortBPin1  , iadcNegInputPortBPin1  }, \
    /* PB2  */ {iadcPosInputPortBPin2  , iadcNegInputPortBPin2  }, \
    /* PB3  */ {iadcPosInputPortBPin3  , iadcNegInputPortBPin3  }, \
    /* PB4  */ {iadcPosInputPortBPin4  , iadcNegInputPortBPin4  }, \
    /* PB5  */ {iadcPosInputPortBPin5  , iadcNegInputPortBPin5  }, \
    /* PB6  */ {iadcPosInputPortBPin6  , iadcNegInputPortBPin6  }, \
    /* PB7  */ {iadcPosInputPortBPin7  , iadcNegInputPortBPin7  }, \
    /* PB8  */ {iadcPosInputPortBPin8  , iadcNegInputPortBPin8  }, \
    /* PB9  */ {iadcPosInputPortBPin9  , iadcNegInputPortBPin9  }, \
    /* PB10 */ {iadcPosInputPortBPin10 , iadcNegInputPortBPin10 }, \
    /* PB11 */ {iadcPosInputPortBPin11 , iadcNegInputPortBPin11 }, \
    /* PB12 */ {iadcPosInputPortBPin12 , iadcNegInputPortBPin12 }, \
    /* PB13 */ {iadcPosInputPortBPin13 , iadcNegInputPortBPin13 }, \
    /* PB14 */ {iadcPosInputPortBPin14 , iadcNegInputPortBPin14 }, \
    /* PB15 */ {iadcPosInputPortBPin15 , iadcNegInputPortBPin15 }, \

#define IOT_ADC_DRV_INC_PORT_C                                  \
    /* PC0  */ {iadcPosInputPortCPin0  , iadcNegInputPortCPin0  }, \
    /* PC1  */ {iadcPosInputPortCPin1  , iadcNegInputPortCPin1  }, \
    /* PC2  */ {iadcPosInputPortCPin2  , iadcNegInputPortCPin2  }, \
    /* PC3  */ {iadcPosInputPortCPin3  , iadcNegInputPortCPin3  }, \
    /* PC4  */ {iadcPosInputPortCPin4  , iadcNegInputPortCPin4  }, \
    /* PC5  */ {iadcPosInputPortCPin5  , iadcNegInputPortCPin5  }, \
    /* PC6  */ {iadcPosInputPortCPin6  , iadcNegInputPortCPin6  }, \
    /* PC7  */ {iadcPosInputPortCPin7  , iadcNegInputPortCPin7  }, \
    /* PC8  */ {iadcPosInputPortCPin8  , iadcNegInputPortCPin8  }, \
    /* PC9  */ {iadcPosInputPortCPin9  , iadcNegInputPortCPin9  }, \
    /* PC10 */ {iadcPosInputPortCPin10 , iadcNegInputPortCPin10 }, \
    /* PC11 */ {iadcPosInputPortCPin11 , iadcNegInputPortCPin11 }, \
    /* PC12 */ {iadcPosInputPortCPin12 , iadcNegInputPortCPin12 }, \
    /* PC13 */ {iadcPosInputPortCPin13 , iadcNegInputPortCPin13 }, \
    /* PC14 */ {iadcPosInputPortCPin14 , iadcNegInputPortCPin14 }, \
    /* PC15 */ {iadcPosInputPortCPin15 , iadcNegInputPortCPin15 }, \

#define IOT_ADC_DRV_INC_PORT_D                                  \
    /* PD0  */ {iadcPosInputPortDPin0  , iadcNegInputPortDPin0  }, \
    /* PD1  */ {iadcPosInputPortDPin1  , iadcNegInputPortDPin1  }, \
    /* PD2  */ {iadcPosInputPortDPin2  , iadcNegInputPortDPin2  }, \
    /* PD3  */ {iadcPosInputPortDPin3  , iadcNegInputPortDPin3  }, \
    /* PD4  */ {iadcPosInputPortDPin4  , iadcNegInputPortDPin4  }, \
    /* PD5  */ {iadcPosInputPortDPin5  , iadcNegInputPortDPin5  }, \
    /* PD6  */ {iadcPosInputPortDPin6  , iadcNegInputPortDPin6  }, \
    /* PD7  */ {iadcPosInputPortDPin7  , iadcNegInputPortDPin7  }, \
    /* PD8  */ {iadcPosInputPortDPin8  , iadcNegInputPortDPin8  }, \
    /* PD9  */ {iadcPosInputPortDPin9  , iadcNegInputPortDPin9  }, \
    /* PD10 */ {iadcPosInputPortDPin10 , iadcNegInputPortDPin10 }, \
    /* PD11 */ {iadcPosInputPortDPin11 , iadcNegInputPortDPin11 }, \
    /* PD12 */ {iadcPosInputPortDPin12 , iadcNegInputPortDPin12 }, \
    /* PD13 */ {iadcPosInputPortDPin13 , iadcNegInputPortDPin13 }, \
    /* PD14 */ {iadcPosInputPortDPin14 , iadcNegInputPortDPin14 }, \
    /* PD15 */ {iadcPosInputPortDPin15 , iadcNegInputPortDPin15 }, \

/*******************************************************************************
 *                            ALL BOARDS
 ******************************************************************************/

#if !defined(IOT_ADC_DRV_INC_SPECIAL_0)
#define IOT_ADC_DRV_INC_SPECIAL_0         IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_SPECIAL_1)
#define IOT_ADC_DRV_INC_SPECIAL_1         IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_RESERVED_0)
#define IOT_ADC_DRV_INC_RESERVED_0        IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_RESERVED_1)
#define IOT_ADC_DRV_INC_RESERVED_1        IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_A)
#define IOT_ADC_DRV_INC_PORT_A            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_B)
#define IOT_ADC_DRV_INC_PORT_B            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_C)
#define IOT_ADC_DRV_INC_PORT_C            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_D)
#define IOT_ADC_DRV_INC_PORT_D            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_E)
#define IOT_ADC_DRV_INC_PORT_E            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_F)
#define IOT_ADC_DRV_INC_PORT_F            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_G)
#define IOT_ADC_DRV_INC_PORT_G            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_H)
#define IOT_ADC_DRV_INC_PORT_H            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_I)
#define IOT_ADC_DRV_INC_PORT_I            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_J)
#define IOT_ADC_DRV_INC_PORT_J            IOT_ADC_DRV_INC_INVALID
#endif

#if !defined(IOT_ADC_DRV_INC_PORT_K)
#define IOT_ADC_DRV_INC_PORT_K            IOT_ADC_DRV_INC_INVALID
#endif

#define IOT_ADC_DRV_INC_MAPPING                      \
  {                                                     \
    /* 0x0X */ IOT_ADC_DRV_INC_INVALID               \
    /* 0x1X */ IOT_ADC_DRV_INC_PORT_I                \
    /* 0x2X */ IOT_ADC_DRV_INC_SPECIAL_0             \
    /* 0x3X */ IOT_ADC_DRV_INC_SPECIAL_1             \
    /* 0x4X */ IOT_ADC_DRV_INC_RESERVED_0            \
    /* 0x5X */ IOT_ADC_DRV_INC_RESERVED_1            \
    /* 0x6X */ IOT_ADC_DRV_INC_PORT_G                \
    /* 0x7X */ IOT_ADC_DRV_INC_PORT_J                \
    /* 0x8X */ IOT_ADC_DRV_INC_PORT_H                \
    /* 0x9X */ IOT_ADC_DRV_INC_PORT_K                \
    /* 0xAX */ IOT_ADC_DRV_INC_PORT_A                \
    /* 0xBX */ IOT_ADC_DRV_INC_PORT_B                \
    /* 0xCX */ IOT_ADC_DRV_INC_PORT_C                \
    /* 0xDX */ IOT_ADC_DRV_INC_PORT_D                \
    /* 0xEX */ IOT_ADC_DRV_INC_PORT_E                \
    /* 0xFX */ IOT_ADC_DRV_INC_PORT_F                \
  }

/*******************************************************************************
*                             END OF MODULE
******************************************************************************/

#endif

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_ADC_DRV_INC_H_ */
