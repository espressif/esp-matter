

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/platform/debug_uart.h>
#include <openthread/platform/logging.h>
#include <openthread_port.h>



void otSysProcessDrivers(otInstance *aInstance) 
{
    ot_system_event_t sevent = OT_SYSTEM_EVENT_NONE;

    OT_GET_NOTIFY(sevent);
    ot_alarmTask(sevent);
    ot_uartTask(sevent);
    ot_radioTask(sevent);
}

