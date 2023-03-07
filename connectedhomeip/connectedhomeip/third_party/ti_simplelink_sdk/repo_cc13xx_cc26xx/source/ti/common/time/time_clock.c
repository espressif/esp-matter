/******************************************************************************

 @file  time_clock.c

 @brief Time clock display and timekeeping.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */

#include <string.h>
#include "comdef.h"
#include "cui.h"
#include "utc_clock.h"
#include "time_clock.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
// extern Display_Handle dispHandle;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Month string
static const char timeMonthStr[12][3] =
{
  {'J', 'a', 'n'},
  {'F', 'e', 'b'},
  {'M', 'a', 'r'},
  {'A', 'p', 'r'},
  {'M', 'a', 'y'},
  {'J', 'u', 'n'},
  {'J', 'u', 'l'},
  {'A', 'u', 'g'},
  {'S', 'e', 'p'},
  {'O', 'c', 't'},
  {'N', 'o', 'v'},
  {'D', 'e', 'c'}
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static char *num2Str(char *pStr, uint8_t num);
static char *year2Str(char *pStr, uint16_t year);

/*********************************************************************
 * @fn      Time_clockInit()
 *
 * @brief   Initialize the Time clock.
 *
 * @return  none
 */
void Time_clockInit(void)
{
  // Start the UTC clock.
  UTC_init();
}

/*********************************************************************
 * @fn      Time_clockDisplay()
 *
 * @brief   Write the clock time to the display.
 *
 * @param       cuiHandle - Client handle that owns the status line
 *              cuiLine - unsigned integer of the line that you are updating.
 *
 * @return  none
 */
void Time_clockDisplay(CUI_clientHandle_t cuiHandle, uint32_t cuiLine)
{
  char lineFormat[MAX_STATUS_LINE_VALUE_LEN];
  char *p = lineFormat;
  UTCTimeStruct time;

  memset(lineFormat, 0x00, MAX_STATUS_LINE_VALUE_LEN);

  // Get time structure from UTC.
  UTC_convertUTCTime(&time, UTC_getClock());

  // Display is in the format:
  // HH:MM MmmDD YYYY

  p = num2Str(p, time.hour);
  *p++ = ':';
  p = num2Str(p, time.minutes);
  *p++ = ' ';

  *p++ = timeMonthStr[time.month][0];
  *p++ = timeMonthStr[time.month][1];
  *p++ = timeMonthStr[time.month][2];

  p = num2Str(p, time.day + 1);
  *p++ = ' ';

  p = year2Str(p, time.year);

  CUI_statusLinePrintf(cuiHandle, cuiLine, lineFormat);
}

/*********************************************************************
 * @fn      Time_clockSet()
 *
 * @brief   Set the clock.
 *
 * @param   pData - Pointer to a Date Time characteristic structure
 *
 * @return  none
 */
void Time_clockSet(uint8_t *pData)
{
  UTCTimeStruct time;

  // Parse time service structure to UTC time structure.
  time.year = BUILD_UINT16(pData[0], pData[1]);
  if (time.year == 0)
  {
    time.year = 2000;
  }
  pData += 2;
  time.month = *pData++;
  if (time.month > 0)
  {
   // time.month--;
  }
  time.day = *pData++;
  if (time.day > 0)
  {
  //  time.day--;
  }
  time.hour = *pData++;
  time.minutes = *pData++;
  time.seconds = *pData;

  // Update UTC time.
  UTC_setClock(UTC_convertUTCSecs(&time));
}

/*********************************************************************
 * @fn      num2Str()
 *
 * @brief   Convert unsigned int 0-99 to decimal digit string.
 *
 * @return  pointer to string
 */
static char *num2Str(char *pStr, uint8_t num)
{
  *pStr++ = (num / 10) + '0';
  *pStr++ = (num % 10) + '0';

  return pStr;
}

/*********************************************************************
 * @fn      num2Str()
 *
 * @brief   Convert a year [9999-0000] to decimal digit string.
 *          Note: this assumes the device's longevity will not surpass
 *          year 9999.
 *
 * @return  pointer to string
 */
static char *year2Str(char *pStr, uint16_t year)
{
  //thousands
  *pStr++ = ((year / 1000) % 10) + '0';
  //hundreds
  *pStr++ = ((year / 100) % 10) + '0';
  //tens
  *pStr++ = ((year / 10) % 10) + '0';
  //units
  *pStr++ = (year % 10) + '0';

  return pStr;
}


/*********************************************************************
*********************************************************************/
