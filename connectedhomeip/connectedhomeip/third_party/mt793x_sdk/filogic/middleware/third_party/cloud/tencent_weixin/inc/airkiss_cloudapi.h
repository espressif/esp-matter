/*
 * airkiss_cloudapi.h
 *
 *  Created on: Dec 28, 2015
 *      Author: zorrowu
 */

#ifndef AIRKISS_CLOUDAPI_H_
#define AIRKISS_CLOUDAPI_H_

#include "airkiss_types.h"

typedef enum
{
	EVENT_VALUE_LOGIN 	= 1,
	EVENT_VALUE_LOGOUT 	= 2
}EventValue;

typedef void (*airkiss_onresponse_fn) (uint32_t hashcode, uint32_t errcode, uint32_t funcid, const uint8_t* body, uint32_t bodylen);
typedef void (*airkiss_onnitify_fn) (uint32_t funcid, const uint8_t* body, uint32_t bodylen);
typedef void (*airkiss_onsdkevent_fn) (EventValue event_value);


typedef struct
{
	airkiss_onresponse_fn m_respcb;
	airkiss_onnitify_fn m_notifycb;
	airkiss_onsdkevent_fn m_eventcb;
} airkiss_callbacks_t;



const char* airkiss_cloud_version();
int32_t airkiss_cloud_init(uint8_t *devlicence, uint32_t licencelen, ak_mutex_t* ak_task_mt, ak_mutex_t* ak_mem_mt, void* heap, uint32_t heaplen);
void airkiss_regist_callbacks(airkiss_callbacks_t* _callbacks);
uint32_t airkiss_cloud_loop();
uint32_t airkiss_cloud_sendmessage(uint32_t funcid, uint8_t *body, uint32_t bodylen);
uint32_t airkiss_cloud_checkupdate(uint8_t *body, uint32_t bodylen);
const uint8_t* airkiss_get_venderid();
const uint8_t* airkiss_get_deviceid();

#endif /* AIRKISS_CLOUDAPI_H_ */
