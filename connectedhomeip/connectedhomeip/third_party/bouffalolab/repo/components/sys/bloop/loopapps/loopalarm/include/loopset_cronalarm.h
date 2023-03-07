#ifndef __LOOPSET_CRON_ALARM_H__
#define __LOOPSET_CRON_ALARM_H__

#define EVT_MAP_CRON_ALARM_TRIGGER     (1U << 0)

void loopapp_cronalarm_trigger(void);
int loopapp_cronalarm_hook_on_looprt(void);

#endif
