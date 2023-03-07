#ifndef __CRONALARMS_H__
#define __CRONALARMS_H__

#include "ccronexpr.h"

/**
 * @brief  create cron alarm
 * @param[in]  cronstring: cron alarm command string. handler: callback. 
 * @param      oneshot: if only run once, oneshot is 1, else, oneshot is 0
 * 
 * @return 
 *	- 0: success 
 *	- -1: failed 
 *
 * */
int cron_alarm_create(const char * cronstring, void *handler, int oneshot);

/**
 * @brief  cron alarm process. 
 *
 * */
void cron_alarm_service(void);

/**
 * @brief  cron alarm init
 *
 * */
void cronalarminit(void);

/**
 * @brief  for demo.c test use. change time to check whether cron alarm run ok.
 * @param  test_time: add seconds
 * 
 * @return 
 *	- seconds 
 *
 * */
uint32_t cronal_time(uint32_t test_time);

#endif 
