/***************************************************************************//**
 * @brief Framework header file, responsible for including generated tokens
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories, Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of  Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software  is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app/framework/include/af-types.h"

#ifndef UC_BUILD
#ifdef GENERATED_TOKEN_HEADER

// If we have generated header, use it.
#include GENERATED_TOKEN_HEADER

#else

// We don't have generated header. Default is to have no tokens.

#define GENERATED_TOKEN_LOADER(endpoint) {}
#define GENERATED_TOKEN_SAVER {}

#endif // GENERATED_TOKEN_HEADER
#endif // UC_BUILD
