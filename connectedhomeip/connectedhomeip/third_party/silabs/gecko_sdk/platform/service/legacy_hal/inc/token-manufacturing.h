/***************************************************************************//**
 * @file
 * @brief Include correct token-manufacturing file based on device series
 *
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
#include "em_device.h"

#if defined(_SILICON_LABS_32B_SERIES_2)
  #include "token-manufacturing-series-2.h"
#else
  #include "token-manufacturing-series-1.h"
#endif
