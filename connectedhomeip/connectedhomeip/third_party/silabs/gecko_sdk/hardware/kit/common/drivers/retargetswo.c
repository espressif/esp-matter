/***************************************************************************//**
 * @file
 * @brief helper functions for configuring SWO
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_cmu.h"
#include "bsp_trace.h"

/**
 * @brief
 *   Output a single character on SWO.
 *
 * @detail
 *   This function will output a character on SWO using ITM channel 0.
 */
int RETARGET_WriteChar(char c)
{
  return ITM_SendChar(c);
}

/**
 * @brief
 *   This function returns 0 always
 *
 * @details
 *   SWO is a one-way link, it is not possible to read data from SWO.
 */
int RETARGET_ReadChar(void)
{
  return 0;
}

/**
 * @brief
 *   Setup SWO for output
 */
void RETARGET_SwoInit(void)
{
  BSP_TraceProfilerSetup();
}
