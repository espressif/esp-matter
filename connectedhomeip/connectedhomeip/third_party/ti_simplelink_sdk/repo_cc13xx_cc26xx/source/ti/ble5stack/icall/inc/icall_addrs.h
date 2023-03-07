/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @file  icall_addrs.h
 *  @brief 		Stack image address information specific to build
 */

#ifndef ICALLADDRS_H
#define ICALLADDRS_H

#include <icall.h>

#ifndef USE_DEFAULT_USER_CFG

#include "hal_types.h"
#include "ble_user_config.h"

#ifdef ICALL_JT
extern icall_userCfg_t user0Cfg;		//!< user config
#else /* !(ICALL_JT) */
extern bleUserCfg_t user0Cfg;			//!< user config
#endif /* ICALL_JT */

/**
 *  @addtogroup ICall_Constants
 *  @{
 */

#define USER0_CFG                      &user0Cfg		//!< user config

#else

#define USER0_CFG                      NULL				//!< user config

#endif // USE_DEFAULT_USER_CFG

/**
 * An entry function address of an external image.
 * Note that it is not necessary to define this macro
 * in this header file. The macro is internally used
 * to expand @ref ICALL_ADDR_MAPS and also used
 * for the sample DummyStack code.
 */
#ifndef STACK_LIBRARY
#ifndef ICALL_STACK0_ADDR
#define ICALL_STACK0_ADDR              0x00005000ul
#endif
#endif /* STACK_LIBRARY */

/** @} */ // end of ICall_Constants

/**
 * Initializer for an array of @ref ICall_RemoteTaskEntry.
 * Each element of the array corresponds to an entry function
 * of an external image.
 * The function address must be an odd address for CC2650
 * so that call will be made in Thumb mode
 */
#ifdef STACK_LIBRARY
extern void startup_entry( const ICall_RemoteTaskArg *arg0, void *arg1 );
//extern ICall_RemoteTaskEntry startup_entry;
#define ICALL_ADDR_MAPS \
{ \
  (ICall_RemoteTaskEntry) (startup_entry) \
}
#else  /* ! STACK_LIBRARY */
#define ICALL_ADDR_MAPS \
{ \
  (ICall_RemoteTaskEntry) (ICALL_STACK0_ADDR) \
}
#endif /* STACK_LIBRARY */

/**
 *  @addtogroup ICall_Constants
 *  @{
 */

/**
 * Initializer for an array of thread priorities.
 * Each element of the array corresponds to TI-RTOS specific
 * thread priority value given to a thread to be created
 * per the entry function defined in @ref ICALL_ADDR_MAPS
 * initializer, in the same sequence.
 */
#define ICALL_TASK_PRIORITIES { 5 }

/**
 * Initializer for an array of thread stack sizes.
 * Each element of the array corresponds to stack depth
 * allocated to a thread to be created per the entry function
 * defined in @ref ICALL_ADDR_MAPS initializer, in the same sequence.
 */
#define ICALL_TASK_STACK_SIZES { 4096 }

/**
 * Initializer for custom initialization parameters.
 * Each element of the array corresponds to initialization parameter
 * (a pointer) specific to the image to be passed to the entry function
 * defined in @ref ICALL_ADDR_MAPS initializer.
 */
#define ICALL_CUSTOM_INIT_PARAMS { USER0_CFG }

#endif /* ICALLADDRS_H */

/** @} */ // end of ICall_Constants
