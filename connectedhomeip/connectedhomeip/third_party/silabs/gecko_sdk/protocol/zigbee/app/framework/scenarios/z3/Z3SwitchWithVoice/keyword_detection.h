/***************************************************************************//**
 * @file keyword_detection.h
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef KEYWORD_DETECTION_H
#define KEYWORD_DETECTION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void keyword_detection_init(void);

/***************************************************************************//**
 * Keyword detection task
 ******************************************************************************/
void keyword_detection_process_action(void);

// Implemented in Z3SwitchVoice_CallBacks.c
void keyword_detected(uint8_t found_command_index);

#ifdef __cplusplus
}
#endif

#endif // KEYWORD_DETECTION_H
