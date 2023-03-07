#ifndef __PLATFORM_PORTING__
#define __PLATFORM_PORTING__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <tcp.h>
#include <inet.h>

#include <math.h>
#include <time.h>
#include <unistd.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if !defined(MBEDTLS_SHA256_ALT)
#include "mbedtls/sha256.h"
#else
#include "mbedtls/sha256_alt.h"
#endif

#if !defined(MBEDTLS_MD5_ALT)
#include "mbedtls/md5.h"
#else
#include "mbedtls/md5_alt.h"
#endif

#include "wifi_api.h"

typedef struct sockaddr_in alink_sockaddr_t;
typedef struct sockaddr alink_sockaddr;
typedef struct timeval alink_timeval_t;

typedef mbedtls_md5_context MD5_CTX;
typedef mbedtls_sha256_context SHA256_CTX;

typedef TaskHandle_t pthread_t;
typedef uint8_t pthread_attr_t;

typedef SemaphoreHandle_t pthread_mutex_t;
typedef int32_t pthread_mutexattr_t;

typedef SemaphoreHandle_t pthread_cond_t;
typedef int32_t pthread_condattr_t;

extern int global_alink_loglevel;

#define WSF_WORKER_THREAD_STACKSIZE 0x2500
#define ALINK_MAIN_THREAD_STACKSIZE 0x800
#define THREAD_FUNC_STACKSIZE 0x800
#define SEND_WORKER_STACKSIZE 0x800

//#define USE_LOOPBACK
#define IP_LOOPBACK (htonl(INADDR_LOOPBACK))

#define xos_print(SW, MOD, LVL, FMT, ...) \
do {\
    if (SW) {\
        time_t t = time(NULL);\
        struct tm tm;\
        localtime_r(&t, &tm);\
        char buf[32] = { 0 };\
        strftime(buf, 32, "%a %b %d %H:%M:%S %Y", &tm);\
        printf("<%s/%s> %s [%lu][%s#%d]:" FMT "\r\n", MOD, LVL, buf, pthread_self(), __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }\
}while(0)

#define ALINK_THREAD_PTR(th) ((void *)th)
#define ALINK_THREAD_IS_VALID(th) ((th) > 0)
#define ALINK_THREAD_RESET(th) \
    do { \
        th = 0;\
    } while(0);


#define	u8	unsigned char
#define	u16	unsigned short
#define u32	unsigned int

/*********************** porting layer **************************/
#ifdef htons
#undef htons
#endif /* htons */
#ifdef htonl
#undef htonl
#endif /* htonl */
#ifdef ntohs
#undef ntohs
#endif /* ntohs */
#ifdef ntohl
#undef ntohl
#endif /* ntohl */

#define alink_htonl      	            htonl
#define alink_ntohl  	                ntohl
#define alink_ntohs      	            ntohs
#define alink_htons  	                htons
#ifdef select
#undef select
#endif /* select */
#ifdef connect
#undef connect
#endif /* connect */
#ifdef close
#undef close
#endif /* close */
#ifdef sendto
#undef sendto
#endif /* sendto */
#ifdef setsockopt
#undef setsockopt
#endif /* setsockopt */
#ifdef bind
#undef bind
#endif /* bind */
#ifdef recvfrom
#undef recvfrom
#endif /* recvfrom */
#ifdef getsockname
#undef getsockname
#endif /* getsockname */
#define alink_select                    select
#define alink_connect    	            connect
#define alink_socket_close              close
#define alink_sendto                    sendto
#define alink_setsockopt                setsockopt
#define alink_bind                      bind
#define alink_recvfrom                  recvfrom
#define alink_getsockname               getsockname

#ifdef inet_addr
#undef inet_addr
#endif /* inet_addr */
#define alink_ipaddr_addr               inet_addr
#define alink_pthread_join      	    pthread_join
#define alink_pthread_mutex_destroy  	pthread_mutex_destroy
#define alink_pthread_cond_init      	pthread_cond_init
#define alink_pthread_cond_wait     	pthread_cond_wait
//#define alink_pthread_cond_timedwait 	pthread_cond_timedwait
#define alink_pthread_cond_signal    	pthread_cond_signal
#define alink_pthread_cond_destroy  	pthread_cond_destroy
#define alink_ssl_connect               ssl_connect      
#define alink_ssl_force_close           ssl_force_close  
#define alink_ssl_pending               ssl_pending      
#define alink_ssl_recv                  ssl_recv         
#define alink_ssl_send                  ssl_send         
#define alink_create_thread             create_thread    
//#define alink_socket_close            SocketClose      
#define alink_resolve_ip                resolve_ip       
#define alink_wait_network_up           wait_network_up
#define alink_pthread_mutex_init        pthread_mutex_init     
#define alink_pthread_mutex_lock        pthread_mutex_lock     
#define alink_pthread_mutex_unlock      pthread_mutex_unlock  
/*********************** porting layer **************************/

#ifdef __cplusplus
extern "C" {
#endif

    // libalink.a used
	void *alink_ssl_connect(int fd, int cert_len, const char *cert, int *err);
	int alink_ssl_force_close(void *ssl);
	int alink_ssl_pending(void *ssl);
	int alink_ssl_recv(void *ssl, void *buf, int length);
	int alink_ssl_send(void *ssl, const char *ptr, int length);

    int alink_create_thread(pthread_t * thread, const char *thread_name, void *thread_func, void *arg, int stack_size);
    void alink_exit_thread(void *retval);
    int alink_pthread_join(pthread_t pthread, void **retval);
    int alink_is_current_thread(pthread_t * threadId);

    int alink_pthread_mutex_init (pthread_mutex_t *mutex, pthread_mutexattr_t *attr);
    int alink_pthread_mutex_lock (pthread_mutex_t *mutex);
    int alink_pthread_mutex_timedlock (pthread_mutex_t *mutex,int timeout);    
    int alink_pthread_mutex_unlock (pthread_mutex_t *mutex);
    int alink_pthread_mutex_destroy(pthread_mutex_t *mutex);

    int alink_pthread_cond_init (pthread_cond_t *cond, pthread_condattr_t *attr);
    int alink_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
    int alink_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,int timeout);
    int alink_pthread_cond_signal (pthread_cond_t *cond);
    int alink_pthread_cond_destroy(pthread_cond_t *cond);

    void alink_sleep(int millsec);
    int alink_get_free_heap_size(void);
    void alink_wait_network_up(void);

    int alink_create_socket(int domain, int type, int protocol);   
    int alink_connect(int s, const struct sockaddr *name, socklen_t namelen);
    int alink_send(int s, char *buf, int len, int flags);
    int alink_recv(int s, char *buf, int len, int flag);
    int alink_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);    
    int alink_socket_close(int fd);    
    int alink_sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
    int alink_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
    int alink_bind(int s, const struct sockaddr *name, socklen_t namelen);
    int alink_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
    int alink_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
    alink_sockaddr_t *alink_create_addr(alink_sockaddr_t * addr, in_addr_t ip, short port);
    int alink_valid_socket(int sock);
    int alink_resolve_ip(const char *addr, in_addr_t * ip);
    unsigned int alink_htonl(unsigned int n);
    unsigned int alink_ntohl(unsigned int n);
    unsigned short alink_htons(unsigned short n);
    unsigned short alink_ntohs(unsigned short n); 
    uint32_t alink_inet_addr(const char *ip_addr);
    unsigned int alink_ipaddr_addr(const char *cp);
    void* alink_calloc_func(size_t size, size_t num);
    void* alink_malloc_func(size_t sz);
    void alink_free_func(void *ptr);

    // internal used
    int alink_get_wifi_station_rssi(void);
    char * alink_get_device_mac();
    char * alink_get_device_chipid();
    char * alink_get_device_ip();
    char * alink_get_device_sn();

    int alink_fw_download(char * buffer, unsigned int len);
    int alink_fw_check(void);
    int alink_fw_upgrade(void);
    void alink_os_reboot(void);
    
    int alink_read_flash(char * buffer ,int len);
    int alink_write_flash(char * buffer ,int len);

    int alink_md5_init(MD5_CTX * ctx);
    int alink_md5_update(MD5_CTX * ctx, unsigned char *input, int ilen);
    int alink_md5_final(MD5_CTX * ctx, unsigned char output[16]);
    
    int alink_sha256_init(SHA256_CTX * ctx);
    int alink_sha256_update(SHA256_CTX * ctx, const void *data,unsigned long len);
    int alink_sha256_final(unsigned char *res, SHA256_CTX * ctx);

#ifdef __cplusplus
}
#endif
#endif				//end of __PLATFORM_PORTING__
