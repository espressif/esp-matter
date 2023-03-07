/***************************************************************************//**
 * @file
 * @brief Kernal Objects Configuration File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                                MODULE
 *********************************************************************************************************
 *********************************************************************************************************
 */

#ifndef SILABS_OS_CFG_APP_H
#define SILABS_OS_CFG_APP_H

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                     KERNEL OBJECTS CONFIGURATION
 *********************************************************************************************************
 *********************************************************************************************************
 */

#define  OS_CFG_ISR_STK_SIZE                256
#define  OS_CFG_MSG_POOL_SIZE               50
#define  OS_CFG_IDLE_TASK_STK_SIZE          128
#define  OS_CFG_STAT_TASK_STK_SIZE          256
#define  OS_CFG_TICK_TASK_STK_SIZE          128
#define  OS_CFG_TMR_TASK_STK_SIZE           128
#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY    10u
#define  OS_CFG_STAT_TASK_PRIO              3u
#define  OS_CFG_STAT_TASK_RATE_HZ           10u
#define  OS_CFG_TICK_RATE_HZ                1000u
#define  OS_CFG_TICK_TASK_PRIO              1u
#define  OS_CFG_TMR_TASK_PRIO               2u
#define  OS_CFG_TMR_TASK_RATE_HZ            10u

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                              MODULE END
 *********************************************************************************************************
 *********************************************************************************************************
 */

#endif
