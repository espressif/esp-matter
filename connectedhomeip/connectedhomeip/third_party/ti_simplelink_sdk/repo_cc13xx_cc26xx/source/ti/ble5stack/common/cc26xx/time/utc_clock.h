/******************************************************************************

 @file  utc_clock.h

 @brief UTC Clock types and functions prototypes.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
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

#ifndef UTC_CLOCK_H
#define UTC_CLOCK_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC
typedef uint32_t UTCTime;

// UTC time structs broken down until standard components.
typedef struct
{
  uint8_t seconds;  // 0-59
  uint8_t minutes;  // 0-59
  uint8_t hour;     // 0-23
  uint8_t day;      // 0-30
  uint8_t month;    // 0-11
  uint16_t year;    // 2000+
} UTCTimeStruct;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * @fn      UTC_init
 *
 * @brief   Initialize the UTC clock module.  Sets up and starts the
 *          clock instance.
 *
 * @param   None.
 *
 * @return  None.
 */
extern void UTC_init(void);

/*
 * Set the new time.  This will only set the seconds portion
 * of time and doesn't change the factional second counter.
 *     newTime - number of seconds since 0 hrs, 0 minutes,
 *               0 seconds, on the 1st of January 2000 UTC
 */
extern void UTC_setClock( UTCTime newTime );

/*
 * Gets the current time.  This will only return the seconds
 * portion of time and doesn't include the factional second counter.
 *     returns: number of seconds since 0 hrs, 0 minutes,
 *              0 seconds, on the 1st of January 2000 UTC
 */
extern UTCTime UTC_getClock( void );

/*
 * Converts UTCTime to UTCTimeStruct
 *
 * secTime - number of seconds since 0 hrs, 0 minutes,
 *          0 seconds, on the 1st of January 2000 UTC
 * tm - pointer to breakdown struct
 */
extern void UTC_convertUTCTime( UTCTimeStruct *tm, UTCTime secTime );

/*
 * Converts UTCTimeStruct to UTCTime (seconds since 00:00:00 01/01/2000)
 *
 * tm - pointer to UTC time struct
 */
extern UTCTime UTC_convertUTCSecs( UTCTimeStruct *tm );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* UTC_CLOCK_H */
