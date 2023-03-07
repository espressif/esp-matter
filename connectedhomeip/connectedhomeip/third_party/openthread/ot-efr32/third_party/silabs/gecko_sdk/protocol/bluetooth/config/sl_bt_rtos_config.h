#ifndef SL_BT_RTOS_CONFIG_H
#define SL_BT_RTOS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Priority Configuration for Bluetooth RTOS Tasks

// <o SL_BT_RTOS_LINK_LAYER_TASK_PRIORITY> Bluetooth link layer task priority
// <i> Default: 52 (CMSIS-RTOS2 osPriorityRealtime4)
// <i> Define the priority of the Bluetooth link layer task. This must be a valid
// <i> priority value from CMSIS-RTOS2 osPriority_t definition. The link layer
// <i> task must have the highest priority in these three Bluetooth RTOS tasks.
#define SL_BT_RTOS_LINK_LAYER_TASK_PRIORITY     (52)

// <o SL_BT_RTOS_LINK_LAYER_TASK_STACK_SIZE> Bluetooth link layer task stack size in bytes
// <i> Default: 1000
// <i> Define the stack size of the Bluetooth link layer task. The value is in bytes
// <i> and will be word aligned when it is applied at the task creation.
#define SL_BT_RTOS_LINK_LAYER_TASK_STACK_SIZE   (1000)

// <o SL_BT_RTOS_HOST_STACK_TASK_PRIORITY> Bluetooth host stack task priority
// <i> Default: 51 (CMSIS-RTOS2 osPriorityRealtime3)
// <i> Define the priority of the Bluetooth host stack task. This must be a
// <i> valid priority value from CMSIS-RTOS2 osPriority_t definition.
#define SL_BT_RTOS_HOST_STACK_TASK_PRIORITY     (51)

// <o SL_BT_RTOS_HOST_STACK_TASK_STACK_SIZE> Bluetooth host stack task stack size in bytes
// <i> Default: 2000
// <i> Define the stack size of the Bluetooth host stack task. The value is in bytes
// <i> and will be word aligned when it is applied at the task creation.
#define SL_BT_RTOS_HOST_STACK_TASK_STACK_SIZE   (2000)

// <o SL_BT_RTOS_EVENT_HANDLER_TASK_PRIORITY> Bluetooth event handler task priority
// <i> Default: 50 (CMSIS-RTOS2 osPriorityRealtime2)
// <i> Define the priority of the Bluetooth event handler task. This must be a
// <i> valid priority value from CMSIS-RTOS2 osPriority_t definition. The event
// <i> handler task must have the lowest priority in these three Bluetooth RTOS tasks.
#define SL_BT_RTOS_EVENT_HANDLER_TASK_PRIORITY  (50)

// <o SL_BT_RTOS_EVENT_HANDLER_STACK_SIZE> Bluetooth event handler task stack size in bytes
// <i> Default: 1000
// <i> Define the stack size of the Bluetooth event handler task. The value is in bytes
// <i> and will be word aligned when it is applied at the task creation.
#define SL_BT_RTOS_EVENT_HANDLER_STACK_SIZE     (1000)

// </h> End Priority Configuration for Bluetooth RTOS Tasks

// <<< end of configuration section >>>

#endif // SL_BT_RTOS_CONFIG_H
