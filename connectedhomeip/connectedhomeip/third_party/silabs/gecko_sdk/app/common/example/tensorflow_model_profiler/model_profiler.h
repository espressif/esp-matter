/***************************************************************************//**
 * @file model_profiler.h
 * @brief TFLM model profiler header.
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
#ifndef MODEL_PROFILER_H
#define MODEL_PROFILER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool model_profiler_init(void);
void model_profiler_process_action(void);

#ifdef __cplusplus
}
#endif

#endif  // MODEL_PROFILER_H
