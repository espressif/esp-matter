#include <FreeRTOS.h>
#include <task.h>
#include <utils_debug.h>
#include <utils_log.h>
#include <utils_list.h>
#include <cronalarms.h>
#include <sntp.h>
#include <string.h>
#include <stdio.h>
#include <blog.h>

utils_dlist_t cronalarm_queue;
typedef struct cronevent
{
    utils_dlist_t dlist_item;
    cron_expr expr;
    void (*handler)(void);
    uint32_t nexttrigger;
    int enabled;  
    int oneshot; 
}cronevent_t;

static uint32_t test_count = 0;//for test use
uint32_t cronal_time(uint32_t test_s)
{
    uint32_t seconds = 0, frags = 0;
   
    sntp_get_time(&seconds, &frags);
    test_count = test_count + test_s;
    seconds = seconds + test_count;

    return seconds;
}

static void cronal_free(cronevent_t *pstcron)
{
    utils_dlist_del(&(pstcron->dlist_item));
    vPortFree(pstcron);
    return;
}

static void update_next_trigger(cronevent_t *pstcron)
{
    uint32_t timenow;

    if (pstcron->enabled == 1) {
        timenow = cronal_time(0);
        if (pstcron->handler != NULL && pstcron->nexttrigger <= timenow) {
            pstcron->nexttrigger = (uint32_t)cron_next(&(pstcron->expr), (time_t)timenow);
        }
    }

    return;
}

static void cronal_enable(cronevent_t *pstcron)
{
    if (cronal_time(0) == 0) {
        pstcron->enabled = 0;
    } else {
        pstcron->enabled = 1;
        update_next_trigger(pstcron);
    }
       
    return;
}

void cronalarminit(void)
{
    INIT_UTILS_DLIST_HEAD(&cronalarm_queue);
    return;
}

void cron_alarm_service(void)
{
    cronevent_t *node;
    void (*handler)(void) = NULL;

    if(cronal_time(0) == 0) {
        blog_info("time not start \r\n");
        return;
    } else {
    }

    utils_dlist_for_each_entry(&cronalarm_queue, node, cronevent_t, dlist_item) {
       if(node->enabled == 1 && (cronal_time(0) >= node->nexttrigger)) {
            handler = node->handler;
            if (node->oneshot == 1) {
                cronal_free(node); 
            } else {
                update_next_trigger(node);
            }
            if (handler != NULL) {
                handler();

            }
        } else if (node->enabled == 0) {
            cronal_enable(node);    
        } else {
        } 
 
    }
    
    return;
}

int cron_alarm_create(const char * cronstring, void *handler, int oneshot)
{   
    cronevent_t *pstcron;
    const char *err;

    if (handler == NULL) {
        blog_error("NULL pointer return \r\n");
        return -1;
    }

    pstcron = pvPortMalloc(sizeof(cronevent_t));
    if (pstcron == NULL) {
        blog_error("malloc failed , return \r\n");
        return -1;
    }
    
    memset(pstcron, 0, sizeof(cronevent_t));
    cron_parse_expr(cronstring, &(pstcron->expr), &err);
    if (err) {
        memset(&(pstcron->expr), 0, sizeof(pstcron->expr));
        return -1;
    }

    pstcron->handler = (void (*)(void))handler;
    pstcron->oneshot = oneshot;
    cronal_enable(pstcron); 
    utils_dlist_add(&(pstcron->dlist_item), &cronalarm_queue);
    
    return 0;
}
