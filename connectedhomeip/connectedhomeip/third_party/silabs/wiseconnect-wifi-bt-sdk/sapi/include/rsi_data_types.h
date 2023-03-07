/*******************************************************************************
* @file  rsi_data_types.h
* @brief 
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

#ifndef RSI_DATA_TYPES_H
#define RSI_DATA_TYPES_H
#include <stdint.h>
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef NULL
#define NULL 0
#endif

//! Enable feature
#define RSI_ENABLE 1
//! Disable feature
#define RSI_DISABLE 0

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
#if 0
#ifndef int8_t
typedef signed char  	int8_t;
#endif

#ifndef uint8_t
typedef unsigned char  	uint8_t;
#endif

#ifndef int16_t
typedef short int  int16_t;
#endif

#ifndef uint16_t
typedef unsigned short int  uint16_t;
#endif

#ifndef int32_t
typedef int  int32_t;
#endif

#ifndef uint32_t
typedef unsigned int  uint32_t;
#endif

#ifndef int64_t
typedef signed long long int 	int64_t;
#endif

#ifndef uint64_t
typedef unsigned long long int 	uint64_t;
#endif
#endif

#ifndef RSI_M4_INTERFACE
#define STATIC static
#define INLINE inline
#endif
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#endif
