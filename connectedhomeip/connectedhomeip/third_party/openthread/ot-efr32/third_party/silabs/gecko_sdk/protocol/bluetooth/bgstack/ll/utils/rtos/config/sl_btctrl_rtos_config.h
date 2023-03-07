#ifndef SL_BTCTRL_RTOS_CONFIG_H
#define SL_BTCTRL_RTOS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Priority Configuration for Bluetooth RTOS Tasks

// <o SL_BTCTRL_RTOS_LINK_LAYER_TASK_PRIORITY> Bluetooth link layer task priority
// <i> Default: 52 (CMSIS-RTOS2 osPriorityRealtime4)
// <i> Define the priority of the Bluetooth link layer task. This must be a valid
// <i> priority value from CMSIS-RTOS2 osPriority_t definition.
#define SL_BTCTRL_RTOS_LINK_LAYER_TASK_PRIORITY     (52)

// <o SL_BTCTRL_RTOS_LINK_LAYER_TASK_STACK_SIZE> Bluetooth link layer task stack size in bytes
// <i> Default: 1000
// <i> Define the stack size of the Bluetooth link layer task. The value is in bytes
// <i> and will be word aligned when it is applied at the task creation.
#define SL_BTCTRL_RTOS_LINK_LAYER_TASK_STACK_SIZE   (1000)

// </h> End Priority Configuration for Bluetooth RTOS Tasks

// <<< end of configuration section >>>

#endif // SL_BT_RTOS_CONFIG_H
