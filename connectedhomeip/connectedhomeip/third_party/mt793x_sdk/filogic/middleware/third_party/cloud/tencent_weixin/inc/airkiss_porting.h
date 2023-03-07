/*
 * airkiss_porting.h
 *
 *  Created on: Dec 16, 2015
 *      Author: zorrowu
 */

#ifndef AIRKISS_PORTING_H_
#define AIRKISS_PORTING_H_
#include "airkiss_types.h"
#include <string.h>

typedef enum {
	AK_TCP_CONNECT_SUCCESS      =  0,     //succeed
	AK_TCP_CONNECT_FAILED 		= -1,     //failed
	AK_TCP_CONNECT_WAITING    	= 2,      //waiting
} airkiss_tcp_state;

typedef enum {
	AK_DNS_SUCCESS      =  0,     //succeed
	AK_DNS_FAILED 		= -1,     //failed
	AK_DNS_WAITING    	= 2,      //waiting
} airkiss_dns_state;

#define AIRKISS_ENABLE_LOG

#ifdef AIRKISS_ENABLE_LOG
int airkiss_printfImp(const char *fmt, ...);
#endif

#define AIRKISS_SUPPORT_MULTITHREAD

#ifdef AIRKISS_SUPPORT_MULTITHREAD
int airkiss_mutex_create(ak_mutex_t *mutex_ptr);
int airkiss_mutex_lock(ak_mutex_t *mutex_ptr);
int airkiss_mutex_unlock(ak_mutex_t *mutex_ptr);
#endif

int airkiss_dns_gethost(char* url, uint32_t* ipaddr);
int airkiss_dns_checkstate(uint32_t* ipaddr);

ak_socket airkiss_tcp_socket_create();
int airkiss_tcp_connect(ak_socket sock, char* ipaddr, uint16_t port);
int airkiss_tcp_checkstate(ak_socket sock);
int airkiss_tcp_send(ak_socket socket, char*buf, uint32_t len);
void airkiss_tcp_disconnect(ak_socket socket);
int airkiss_tcp_recv(ak_socket socket, char *buf, uint32_t size, uint32_t timeout);


uint32_t airkiss_gettime_ms();


//void* airkiss_malloc(uint32_t size);
//void airkiss_free(void *p);
//void *airkiss_calloc(uint32_t n, uint32_t size);


#endif /* AIRKISS_PORTING_H_ */
