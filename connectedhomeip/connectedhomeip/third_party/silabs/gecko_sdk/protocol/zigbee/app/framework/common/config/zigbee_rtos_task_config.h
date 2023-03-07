/***************************************************************************//**
 * @brief Zigbee Application Framework common component configuration header.
 *\n*******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee stack RTOS task configuration
// <o SL_ZIGBEE_OS_STACK_TASK_SIZE> Zigbee stack task stack size in bytes <250-5000>
// <i> Default: 1400
// <i> Defines the stack size of the Zibgee RTOS stack task. The value is in bytes
// <i> and will be word aligned when it is applied at the task creation.
#define SL_ZIGBEE_OS_STACK_TASK_SIZE 1400

// <o SL_ZIGBEE_OS_STACK_TASK_PRIORITY> Zigbee Task RTOS priority <41-49>
// <i> Default: 49
// <i> Defines the priority of the Zigbee RTOS stack task - CAUTION: Change not recommended!
#define SL_ZIGBEE_OS_STACK_TASK_PRIORITY 49

// </h>

// <<< end of configuration section >>>
