/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef TENSORFLOW_LITE_MICRO_MICROSPEECH_H
#define TENSORFLOW_LITE_MICRO_MICROSPEECH_H

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void tflite_micro_micro_speech_init(void);

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void tflite_micro_micro_speech_process_action(void);

#endif // TENSORFLOW_MICRO_SPEECH_H
