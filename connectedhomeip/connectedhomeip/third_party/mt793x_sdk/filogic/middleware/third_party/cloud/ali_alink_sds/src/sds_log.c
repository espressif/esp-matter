/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "task_def.h"
#include "sds_log.h"

#ifdef USE_SDS_LOG_TASK

#define  MAX_SDS_QUEUE_LEN  8
#define MAX_SDS_CONTENT_LEN  254

#define SDS_LOG_TASK_NAME  "sdslog"
#define SDS_LOG_STACK_SIZE 512


typedef struct
{
    bool  occupied;
    char  message[MAX_SDS_CONTENT_LEN];
}message_node_t;

typedef struct
{
    int index;
    message_node_t list[MAX_SDS_QUEUE_LEN];
}message_pool_t;


static xQueueHandle sds_log_queue = NULL;
static xTaskHandle sds_log_task_handle = NULL;
static bool sds_log_task_ready = FALSE;
static message_pool_t sds_pool;
static bool sds_init_done = FALSE;
static void __sds_pool_init(void)
{
    int i = 0;
    for(;i < MAX_SDS_QUEUE_LEN; ++i)
    {
        sds_pool.list[i].occupied = FALSE;
    }
    sds_pool.index = 0;
}

static message_node_t * __sds_pool_allocate_node(void)
{
    int i = 0;
    #if 0
    for(; i < MAX_SDS_QUEUE_LEN; ++i)
    {
        if(!sds_pool.list[i].occupied)
        {
            sds_pool.list[i].occupied = TRUE;
            return &sds_pool.list[i];
        }
    }
    return NULL;
    #endif
    #if 1
    int tmp_index = sds_pool.index;
    
    for(;;)
    {
       if(!sds_pool.list[tmp_index].occupied)
       {
           sds_pool.index ++;
           if(sds_pool.index == MAX_SDS_QUEUE_LEN)
                sds_pool.index = 0;
           sds_pool.list[tmp_index].occupied = TRUE;
           return &sds_pool.list[tmp_index];
       }
       sds_pool.index ++;
       if(sds_pool.index == MAX_SDS_QUEUE_LEN)
            sds_pool.index = 0;
       if(tmp_index == sds_pool.index)
       {
          return NULL;
       }
    }
    #endif
}

static void __sds_pool_release_node( message_node_t * node)
{
    node->occupied = FALSE;
}

static void __sds_log_task_entry(void *args)
{
    sds_log_task_ready = TRUE;
    for(;;)
    {
        message_node_t * message_node = NULL;
        if (xQueueReceive(sds_log_queue, (void *)&message_node, portMAX_DELAY) == pdPASS) 
        {
            printf(message_node->message);
            __sds_pool_release_node(message_node);
        }
    }
}
int sds_log_init(void)
{
    if(sds_init_done)
        return;
    __sds_pool_init();
    
    sds_log_queue = xQueueCreate(MAX_SDS_QUEUE_LEN, sizeof(message_node_t *));
    if (NULL == sds_log_queue) 
    {
        printf("sds_log_init: create log queue failed.\r\n");
        return -1;
    }

    
    if (pdPASS != xTaskCreate(__sds_log_task_entry,
                              SDS_LOG_TASK_NAME,
                              SDS_LOG_STACK_SIZE,
                              NULL,
                              TASK_PRIORITY_SYSLOG,
                              &sds_log_task_handle)) 
    {
        printf("sds_log_init: create sds_log task failed.\r\n");
        return -1;
    }
    return 0;
}


void sds_log_print(const char *message,va_list list)
{
    if(sds_log_task_ready)
    {
        message_node_t * message_node = NULL;
        message_node =  __sds_pool_allocate_node();

        if(NULL != message_node)
        {
            vsnprintf(message_node->message, MAX_SDS_CONTENT_LEN, message, list);
            if (xQueueSendToBack(sds_log_queue, (void *)&message_node, ( TickType_t ) 0) != pdPASS) 
            {
                printf("sds queue is full\n");
            }
        }
    }
}

void sds_log_dump()
{
    int i = 0;

    for(;i < MAX_SDS_QUEUE_LEN; ++i)
    {
        printf(" number %d  occupied  %d\n",i ,sds_pool.list[i].occupied);
    }

}
#endif
