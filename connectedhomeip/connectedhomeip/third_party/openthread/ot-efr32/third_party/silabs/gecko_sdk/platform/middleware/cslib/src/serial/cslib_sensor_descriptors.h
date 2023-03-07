/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef CSLIB_SENSOR_DESCRIPTORS_H
#define CSLIB_SENSOR_DESCRIPTORS_H

#include "cslib_hwconfig.h"

void outputsensorDescriptors(void);

#define HAS_SENSOR_DESCRIPTORS

#if (DEF_NUM_SENSORS == 2)
#define SENSOR_DESCRIPTOR_LIST "B0", "B1"
#elif (DEF_NUM_SENSORS == 3)
#define SENSOR_DESCRIPTOR_LIST "B0", "B1", "B2"
#elif (DEF_NUM_SENSORS == 4)
#define SENSOR_DESCRIPTOR_LIST "B0", "B1", "B2", "B3"
#else
#error "Unsupported CSLIB sensor count."
#endif

#endif // CSLIB_SENSOR_DESCRIPTORS_H
