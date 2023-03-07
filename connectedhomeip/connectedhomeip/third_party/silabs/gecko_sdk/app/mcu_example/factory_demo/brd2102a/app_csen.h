/***************************************************************************//**
 * @file
 * @brief Helper functions for capacitive touch using CSEN
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef APP_CSEN_H
#define APP_CSEN_H

#define NUM_OF_PAD 2
#define PAD_THRS 1500
#define PAD_LEVEL_THRS { PAD_THRS, PAD_THRS, PAD_THRS, PAD_THRS }
#define APP_CSEN_NOISE_MARGIN 500

typedef struct {
  int32_t sliderPos;
  int32_t sliderPrevPos;
  int32_t sliderStartPos;
  int32_t sliderTravel;
  uint32_t eventStart;
  uint32_t eventDuration;
  uint32_t touchForce;
  bool eventActive;
} CSEN_Event_t;

#define CSEN_EVENT_DEFAULT \
  {                        \
    -1,                    \
    -1,                    \
    -1,                    \
    0,                     \
    0,                     \
    0,                     \
    0,                     \
    false,                 \
  }

// Function prototypes
void setupCSEN(void);

int32_t csenCalcPos(void);

void csenCheckScannedData(void);

CSEN_Event_t csenGetEvent(void);

#endif /* APP_CSEN_H */
