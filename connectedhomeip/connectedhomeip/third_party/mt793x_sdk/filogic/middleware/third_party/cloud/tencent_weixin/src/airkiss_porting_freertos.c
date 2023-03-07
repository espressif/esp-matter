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

#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timer.h"

#include "netdb.h" 
#include "socket.h"

#include "airkiss_porting.h"

int airkiss_printfImp(const char *fmt, ...){
	va_list ap;
    static char bufPrint[512]={0};
    int cnt;
    memset(bufPrint, 0, sizeof(bufPrint));
	va_start(ap, fmt);
    cnt = vsnprintf(bufPrint, sizeof(bufPrint), fmt, ap);
	va_end(ap);
    if(cnt >= 0)
        printf("%s\r\n", bufPrint);
    else
        printf("airkiss_printfImp() error code:%d\r\n", cnt);
	return cnt;
}

int32_t parse_ipv4_ad(uint32_t * ip_address,   /* pointer to IP address returned */
        char *   stringin)
{
    int error = -1;
    char *   cp;
    int   dots  =  0; /* periods imbedded in input string */
    int   number;
    union
    {
        unsigned char   c[4];
        unsigned long   l;
    } retval;

    cp = stringin;
    while (*cp)
    {
        if (*cp > '9' || *cp < '.' || *cp == '/')
            return __LINE__;
        if (*cp == '.')dots++;
        cp++;
    }

    if ( dots < 1 || dots > 3 )
        return __LINE__;

    cp = stringin;
    if ((number = atoi(cp)) > 255)
        return __LINE__;

    retval.c[0] = (unsigned char)number;

    while (*cp != '.')cp++; /* find dot (end of number) */
    cp++;             /* point past dot */

    if (dots == 1 || dots == 2) retval.c[1] = 0;
    else
    {
        number = atoi(cp);
        while (*cp != '.')cp++; /* find dot (end of number) */
        cp++;             /* point past dot */
        if (number > 255) return(error);
        retval.c[1] = (unsigned char)number;
    }

    if (dots == 1) retval.c[2] = 0;
    else
    {
        number = atoi(cp);
        while (*cp != '.')cp++; /* find dot (end of number) */
        cp++;             /* point past dot */
        if (number > 255) return(error);
        retval.c[2] = (unsigned char)number;
    }

    if ((number = atoi(cp)) > 255)
        return __LINE__;
    retval.c[3] = (unsigned char)number;

    *ip_address = retval.l;
    printf("dest ip is:%d.%d.%d.%d\n", retval.c[0], retval.c[1], retval.c[2], retval.c[3]);
    return 0;
}

int airkiss_dns_gethost(char* url, uint32_t* ipaddr) { 
    char ip_addr[17] = "\0";
    struct hostent *host;
    
    if((host=gethostbyname(url)) == NULL) {
        printf("gethostbyname error");
        return AK_DNS_FAILED;
    }    
    
    strcpy(ipaddr, inet_ntoa(*((struct in_addr *)host->h_addr)));
    printf("airkiss_dns_gethost() url:%s, server_ip:%s\n", url, ipaddr);   

    return AK_DNS_SUCCESS;
}
int airkiss_dns_checkstate(uint32_t* ipaddr){
	return AK_DNS_SUCCESS;
}

int airkiss_tcp_connect(ak_socket sock, char* ipaddr, uint16_t port) {
	uint32_t addr;
	int ret;
    struct sockaddr_in sock_addr;
    
    printf("airkiss_tcp_connect() sockID:%d, ipaddr:%s, port:%d\n", sock, ipaddr, port);
    
	if (0 != parse_ipv4_ad(&addr, ipaddr)) {
		return AK_TCP_CONNECT_FAILED;
	}
	sock_addr.sin_addr.s_addr = addr;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_family = AF_INET;
	ret = connect(sock, (struct sockaddr *) &sock_addr, sizeof (struct sockaddr_in));
	if (ret < 0) {
		printf("Failed to connect socket %d.\n", sock);
		return AK_TCP_CONNECT_FAILED;
	}
	return AK_TCP_CONNECT_SUCCESS;
}


int airkiss_tcp_checkstate(ak_socket sock){
	return AK_TCP_CONNECT_SUCCESS;
}

int airkiss_tcp_send(ak_socket socket, char*buf, uint32_t len) {
    int send_len;
    
    printf("airkiss_tcp_send() sockID:%d, buf(len:%d):%s\n", socket, len, buf);    
    send_len = send(socket, buf, len, 0);    
    printf("airkiss_tcp_send() send_len:%d\n", send_len);
    
	return send_len;
}

void airkiss_tcp_disconnect(ak_socket socket) {
    
    printf("airkiss_tcp_disconnect() sockID:%d\n", socket);
    
	close(socket);
}

int airkiss_tcp_recv(ak_socket socket, char *buf, uint32_t size, uint32_t timeout) {
	int recvBytes;
	fd_set sockSet;
    int32_t fdAct = 0;
	struct timeval tmo;
	FD_ZERO(&sockSet);
	FD_SET(socket, &sockSet);
	tmo.tv_sec = timeout/1000;
	timeout = timeout%1000;
	tmo.tv_usec = timeout*1000;
	fdAct = select(socket + 1, &sockSet, NULL, NULL, &tmo);
	if (0 != fdAct) {
		printf("fdAct is not 0:%d\r\n", fdAct);
		if (FD_ISSET(socket, &sockSet)) {            
            printf("airkiss_tcp_recv() sockID:%d, buf_size:%s\n", socket, size);    
			recvBytes = recv(socket, buf, size, 0);           
            printf("airkiss_tcp_recv() buf(recv_size:%d):%s\n", recvBytes, buf);    
			if (recvBytes < 0) {
				return -1;
			} else {
				return recvBytes;
			}
		}
	}
	return 0;
}

ak_socket airkiss_tcp_socket_create() {
	ak_socket sock;
	sock = socket (AF_INET, SOCK_STREAM, 0);
    
    printf("airkiss_tcp_socket_create() sockID:%d\n", sock);

	if (sock < 0) {
		return -1;
	}
	return sock;
}

uint32_t airkiss_gettime_ms() {
	return get_current_count();
}

struct ak_mutex_t{
	SemaphoreHandle_t m_mutex;
};

ak_mutex_t m_task_mutex;
ak_mutex_t m_malloc_mutex;

//return 0 if success
int airkiss_mutex_create(ak_mutex_t *mutex_ptr) {
    if (NULL == mutex_ptr)
        assert(0);

    // Semaphore cannot be used before a call to xSemaphoreCreateMutex().
    // This is a macro so pass the variable in directly.
    mutex_ptr->m_mutex = xSemaphoreCreateMutex();   
    printf("airkiss_mutex_create() m_mutex:%p\n",  mutex_ptr->m_mutex);
    if (mutex_ptr->m_mutex != NULL) {
        // The semaphore was created successfully.
        // The semaphore can now be used.
        return 0;
    }
    else 
        return -1;
}

//return 0 if success
int airkiss_mutex_lock(ak_mutex_t *mutex_ptr) {
    if (NULL == mutex_ptr)
        assert(0);

    if (NULL == mutex_ptr->m_mutex)
        assert(0);

    printf("airkiss_mutex_lock() m_mutex:%p\n",  mutex_ptr->m_mutex);

    if (pdTRUE == xSemaphoreTake(mutex_ptr->m_mutex, portMAX_DELAY))
        return 0;
    else
        return -1;    
}

//return 0 if success
int airkiss_mutex_unlock(ak_mutex_t *mutex_ptr) {
    if (NULL == mutex_ptr)
        assert(0);

    if (NULL == mutex_ptr->m_mutex)
        assert(0);

    printf("airkiss_mutex_unlock() m_mutex:%p\n",  mutex_ptr->m_mutex);

    if (pdTRUE == xSemaphoreGive(mutex_ptr->m_mutex))
        return 0;
    else
        return -1;   
}

