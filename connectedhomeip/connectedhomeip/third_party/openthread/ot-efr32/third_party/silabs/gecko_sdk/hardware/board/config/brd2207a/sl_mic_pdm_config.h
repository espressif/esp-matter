/***************************************************************************//**
 * @file
 * @brief MIC_PDM config
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

#ifndef SL_MIC_PDM_CONFIG_H
#define SL_MIC_PDM_CONFIG_H

// <<< Use Configuration Wizard in Context Menu
// <h> MIC PDM config
// <o> PDM down sampling rate <3-73>
// <i> Defines the ratio between PDM reference clock and the sampling frequency
#define SL_MIC_PDM_DSR           32
// </h> end MIC_PDM config
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <pdm signal=DAT0,CLK> SL_MIC_PDM
// $[PDM_SL_MIC_PDM]
#define SL_MIC_PDM_PERIPHERAL                    PDM

// PDM DAT0 on PB11
#define SL_MIC_PDM_DAT0_PORT                     gpioPortB
#define SL_MIC_PDM_DAT0_PIN                      11
#define SL_MIC_PDM_DAT0_LOC                      3

// PDM CLK on PB12
#define SL_MIC_PDM_CLK_PORT                      gpioPortB
#define SL_MIC_PDM_CLK_PIN                       12
#define SL_MIC_PDM_CLK_LOC                       3

// [PDM_SL_MIC_PDM]$
// <<< sl:end pin_tool >>>

#endif // SL_MIC_PDM_CONFIG_H
