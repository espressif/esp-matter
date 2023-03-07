#include "platform_porting.h"

#include "sockets.h"
#include "wifi_api.h"
#include "netif.h"
#include "dhcp.h"
#include "inet.h"
#include "fota.h"
#include "fota_config.h"
#include "hal_wdt.h"
#include "hal_flash.h"
#include "hal_rtc.h"
#include "hal_sys.h"

#include "alink_export.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "nvdm.h"
#include "task_def.h"

//#define ALINK_DEBUG
#ifdef ALINK_DEBUG
#define alink_debug     printf
#else
#define alink_debug(x, ...)
#endif

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) ||  \
    !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) || \
    !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||         \
    !defined(MBEDTLS_CERTS_C) || !defined(MBEDTLS_PEM_PARSE_C) || \
    !defined(MBEDTLS_CTR_DRBG_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)

int alink_ssl_force_close(void *ssl)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);

	return ALINK_ERR;
}

void *alink_ssl_connect(int fd, int cert_len, const char *cert, int *err)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return NULL;
}

int alink_ssl_pending(void *ssl)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}

int alink_ssl_send(void *ssl, const char *ptr, int length)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}

int alink_ssl_recv(void *ssl, void *buf, int length)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}

#else

#include "ssl.h"
#include "mbedtls/net.h"
#include "entropy.h"
#include "ctr_drbg.h"
#include <string.h>
#if defined(MBEDTLS_DEBUG_C)
#include "mbedtls/debug.h"
#endif
//#include "flash_map.h"

#if defined(MBEDTLS_DEBUG_C)
#define ALINK_MBEDTLS_DEBUG_LEVEL 0
#endif

typedef struct
{
	mbedtls_ssl_context ssl_ctx;	/* mbedtls ssl context */
	mbedtls_net_context net_ctx;	/* Fill in socket id */
	mbedtls_ssl_config ssl_conf;	/* SSL configuration */
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacert;
	mbedtls_x509_crt_profile profile;
	pthread_mutex_t ssl_mutex;
}alink_mbedtls_context_struct;

#define ALINK_ROOT_CERT1 \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\r\n" \
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\r\n" \
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\r\n" \
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\r\n" \
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\r\n" \
"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\r\n" \
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\r\n" \
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\r\n" \
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\r\n" \
"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\r\n" \
"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\r\n" \
"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\r\n" \
"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\r\n" \
"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\r\n" \
"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\r\n" \
"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\r\n" \
"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\r\n" \
"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\r\n" \
"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\r\n" \
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\r\n" \
"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\r\n" \
"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\r\n" \
"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\r\n" \
"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\r\n" \
"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\r\n" \
"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\r\n" \
"-----END CERTIFICATE-----\r\n";
const char alink_root_cert1[] = ALINK_ROOT_CERT1;


int alink_ssl_force_close(void *ssl)
{
	alink_mbedtls_context_struct *alink_ssl_ctx = (alink_mbedtls_context_struct *)ssl;

	printf("%s\r\n", __FUNCTION__);

	if (!ssl)
	{
        printf("close parameter error, ssl is null\n");
		return ALINK_ERR;
	}

	alink_pthread_mutex_lock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
	mbedtls_ssl_close_notify(&alink_ssl_ctx->ssl_ctx);

    mbedtls_x509_crt_free(&alink_ssl_ctx->cacert);
    mbedtls_ssl_free(&alink_ssl_ctx->ssl_ctx);
    mbedtls_ssl_config_free(&alink_ssl_ctx->ssl_conf);
    mbedtls_ctr_drbg_free(&alink_ssl_ctx->ctr_drbg);
    mbedtls_entropy_free(&alink_ssl_ctx->entropy);
	alink_pthread_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));

	alink_pthread_mutex_destroy(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
	alink_free_func(alink_ssl_ctx);
	alink_ssl_ctx = NULL;

	return ALINK_OK;
}

static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
	printf("mbedtls[%d]%s:%04d: %s\r\n", level,file, line, str);
}


void *alink_ssl_connect(int fd, int cert_len, const char *cert, int *err)
{
	alink_mbedtls_context_struct *ssl = NULL;
	int authmode = MBEDTLS_SSL_VERIFY_NONE;
	const char *pers = "mbedtls_porting_to_alink";
	int value, ret = ALINK_OK;
	uint32_t flags;

	printf("%s\r\n", __FUNCTION__);
    alink_get_free_heap_size();

	if (fd < 0 || (!cert_len && cert) || (cert_len && !cert))
	{
	    printf("Parameter error\r\n");
		ret = fd < 0 ? ALINK_SOCKET_ERR : ALINK_ERR;
		goto exit;
	}

	ssl = (alink_mbedtls_context_struct *)alink_calloc_func(1, sizeof(alink_mbedtls_context_struct));
	if (!ssl)
	{
	    printf("No memory\r\n");
		ret = ALINK_ERR_NO_MEMORY;
		goto exit;
	}

	// printf("ssl:0x%x\r\n", ssl);
	printf("ssl:0x%p\r\n", ssl);

	alink_pthread_mutex_init(&(ssl->ssl_mutex), NULL);

	alink_pthread_mutex_lock(&(ssl->ssl_mutex));

	#if defined(MBEDTLS_DEBUG_C)
	mbedtls_debug_set_threshold(ALINK_MBEDTLS_DEBUG_LEVEL);
	#endif

	mbedtls_net_init(&ssl->net_ctx);

	ssl->net_ctx.fd = fd;

	if (cert_len && cert)
	{
		authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
	}

	mbedtls_ssl_init(&ssl->ssl_ctx);
	mbedtls_ssl_config_init(&ssl->ssl_conf);
    mbedtls_x509_crt_init(&ssl->cacert);
	mbedtls_ctr_drbg_init(&ssl->ctr_drbg);

	/*
	 * 0. Initialize the RNG and the session data
	 */
	mbedtls_entropy_init(&ssl->entropy);
	if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg,
							   mbedtls_entropy_func,
							   &ssl->entropy,
						       (const unsigned char*)pers,
							   strlen(pers))) != 0)
	{
	    printf("mbedtls_ctr_drbg_seed() failed, value:-0x%x\r\n", -value);

		ret = ALINK_ERR;
		goto exit;
	}

	/*
	* 1. Load the trusted CA
	*/
	#if 0
	if ((value = mbedtls_x509_crt_parse(&ssl->cacert,
									    (const unsigned char *)alink_root_cert1,
									     sizeof(alink_root_cert1))) < 0)
	{
	    printf("Failed to load alink root CA for iot.aliyun.com, value:-0x%x\r\n", -value);
		//ret = ALINK_ERR;
		//goto exit;
	}
	#endif

	printf("Cert:\r\n%s", cert);

	/* cert_len passed in is gotten from strlen not sizeof */
	if (cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
									    (const unsigned char *)cert,
									     cert_len + 1)) < 0))
	{
	    printf("mbedtls_x509_crt_parse() failed, value:-0x%x\r\n", -value);
		ret = ALINK_ERR;
		goto exit;
	}

	/*
	 * 2. Setup stuff
	 */
    if((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf,
				                   MBEDTLS_SSL_IS_CLIENT,
				                   MBEDTLS_SSL_TRANSPORT_STREAM,
				                   MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        printf("mbedtls_ssl_config_defaults() failed, value:-0x%x\r\n", -value);
        ret = ALINK_ERR;
		goto exit;
    }

	memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
	ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_MD5 );
	mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

	mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
	mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

	mbedtls_ssl_conf_rng(&ssl->ssl_conf,
						 mbedtls_ctr_drbg_random,
						 &ssl->ctr_drbg );

	mbedtls_ssl_conf_dbg(&ssl->ssl_conf, my_debug, NULL);

    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx,
						   &ssl->ssl_conf)) != 0)
    {
        printf("mbedtls_ssl_setup() failed, value:-0x%x\r\n", -value);

        ret = ALINK_ERR;
		goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx,
						&ssl->net_ctx,
						mbedtls_net_send,
						mbedtls_net_recv,
						NULL);

	ret = mbedtls_net_set_nonblock(&ssl->net_ctx);
	printf("Set socket to be non-blocking for mbedtls. ret:%d\r\n", ret);

    /*
    if(1)
    {
        int val = 1;
        int rv;
        rv = setsockopt(ssl->net_ctx.fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
        if (rv == -1) {
            printf("set tcp nodelay error!");
        }
    }
    */

	/*
	* 3. Handshake
	*/
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
			printf("mbedtls_ssl_handshake() failed, ret:-0x%x\r\n", -ret);

            ret = ALINK_ERR;
			goto exit;
        }
    }

	/*
	 * 4. Verify the server certificate
	 */

	/* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
        * handshake would not succeed if the peer's cert is bad.  Even if we used
        * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
    //ret = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx);

	if( ( flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0 )
	{
		char vrfy_buf[512];

		mbedtls_printf( "Cert varification failed\n" );

		mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

		mbedtls_printf( "%s\n", vrfy_buf );
	}
	else
		mbedtls_printf( "Cert varification ok\n" );

exit:
    printf("%s : ret=%d\n", __FUNCTION__, ret);

	if (err)
	{
		*err = ret;
	}

	if (ALINK_OK != ret && ssl)
	{
		mbedtls_x509_crt_free(&ssl->cacert);
		mbedtls_ssl_free(&ssl->ssl_ctx);
		mbedtls_ssl_config_free(&ssl->ssl_conf);
		mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
		mbedtls_entropy_free(&ssl->entropy);

		alink_pthread_mutex_unlock(&(ssl->ssl_mutex));
		alink_pthread_mutex_destroy(&(ssl->ssl_mutex));
		alink_free_func(ssl);
		ssl = NULL;

        alink_get_free_heap_size();
        return NULL;
	}

    alink_get_free_heap_size();

	alink_pthread_mutex_unlock(&(ssl->ssl_mutex));
	return (void *)ssl;
}

int alink_ssl_pending(void *ssl)
{
	printf("%s : ", __FUNCTION__);

	int avail_bytes = 0;

	if (!ssl)
	{
		return ALINK_ERR;
	}

	//alink_pthread_mutex_lock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
	avail_bytes = (int)mbedtls_ssl_get_bytes_avail(&(((alink_mbedtls_context_struct *)ssl)->ssl_ctx));
	//alink_pthread_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));

	printf("avail_bytes:%d\r\n", avail_bytes);

	return avail_bytes;
}

int alink_ssl_send(void *ssl, const char *ptr, int length)
{
	int ret = ALINK_ERR;

	printf("%s : **%s=%p** \n", __FUNCTION__, pcTaskGetTaskName(xTaskGetCurrentTaskHandle()), xTaskGetCurrentTaskHandle());

	if (!ssl || !ptr || !length)
	{
        printf("send parameter error, ssl %p, ptr %p, length %d \n", ssl, ptr, length);
		return ALINK_ERR;
	}

	do {
	alink_pthread_mutex_lock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
	ret = mbedtls_ssl_write(&((alink_mbedtls_context_struct *)ssl)->ssl_ctx,
							(const unsigned char *)ptr, (size_t)length);
	alink_pthread_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));

		if( ret == MBEDTLS_ERR_SSL_WANT_READ ||
			ret == MBEDTLS_ERR_SSL_WANT_WRITE )
	{
			continue;
		}

		break;

	} while (1);


	if (0 < ret)
	{
		printf("send_ret=%d\n", ret);
	}
	else
	{
		printf("send_ret=-0x%x\n", -ret);
	}

	return ret <= 0 ? ALINK_ERR : ret;

}

int alink_ssl_recv(void *ssl, void *buf, int length)
{
    fd_set rfds,exps;
	int sel_ret = ALINK_ERR, rcv_ret = ALINK_ERR;

    printf("%s : **%s=%p** \n", __FUNCTION__, pcTaskGetTaskName(xTaskGetCurrentTaskHandle()), xTaskGetCurrentTaskHandle());

	if (!ssl || !buf || !length)
	{
        printf("recv parameter error, ssl %p, ptr %p, length %d \n", ssl, buf, length);
		return ALINK_ERR;
	}

    FD_ZERO(&rfds);
    FD_ZERO(&exps);
    FD_SET(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd, &rfds);
    FD_SET(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd, &exps);

#if 0
    sel_ret = lwip_select(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd + 1, &rfds, NULL, &exps, NULL);
    if (sel_ret > 0) {
         if (FD_ISSET(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd, &rfds)) {
	alink_pthread_mutex_lock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
				        rcv_ret = mbedtls_ssl_read(&((alink_mbedtls_context_struct *)ssl)->ssl_ctx,
							(unsigned char *)buf, (size_t)length);
	alink_pthread_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
				  } else {
				  	printf("exception happen!");
				  	return ALINK_ERR;
				  }
     }
#endif

	do {
		sel_ret = lwip_select(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd + 1, &rfds, NULL, &exps, NULL);
		if (sel_ret > 0) {
			if (!FD_ISSET(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd, &rfds))
	{
				continue;
	}
			alink_pthread_mutex_lock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
			rcv_ret = mbedtls_ssl_read(&((alink_mbedtls_context_struct *)ssl)->ssl_ctx,
										//(unsigned char *)buf, (size_t)length - 1);
										(unsigned char *)buf, (size_t)length);
			alink_pthread_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));

			if( rcv_ret == MBEDTLS_ERR_SSL_WANT_READ ||
				rcv_ret == MBEDTLS_ERR_SSL_WANT_WRITE )
				continue;

			if( rcv_ret <= 0 )
			{
				#if 0
				switch( rcv_ret )
	{
	                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
	                    mbedtls_printf( " connection was closed gracefully\n" );
	                    break;

	                case 0:
	                case MBEDTLS_ERR_NET_CONN_RESET:
	                    mbedtls_printf( " connection was reset by peer\n" );
	                    break;

	                default:
	                    mbedtls_printf( " mbedtls_ssl_read returned -0x%x\n", -rcv_ret );
	                    break;
	}
				#endif

				break;
			}

			//((char *)buf)[rcv_ret] = '\0';
			break;
		} else {
			printf("exception happen!");
			return ALINK_ERR;
		}

	} while( 1 );

	if (0 < rcv_ret)
	{
		printf("recv_ret=%d\n", rcv_ret);
	}
	else
	{
		printf("recv_ret=-0x%x\n", -rcv_ret);
	}

	return rcv_ret <= 0 ? ALINK_ERR : rcv_ret;
}

#endif


int alink_create_thread(pthread_t * thread, const char *thread_name,
		  void *thread_func, void *arg, int stack_size)
{
	printf("%s : ", __FUNCTION__);

    BaseType_t ret;
    // UBaseType_t uxPriority = tskIDLE_PRIORITY+1;
    UBaseType_t uxPriority = TASK_PRIORITY_NORMAL;

    /*
       lwip priority = 3, nvran priority =2
       1) send_worker
       2) callback_thread
       3) alink_main_thread priority = 1
       4) wsf_worker_thread
       5) firmware_update_pthread priority 3(same as lwip)
     */
    // if(thread_name[0] == 'f') // firmware_update_pthread
        //uxPriority = tskIDLE_PRIORITY + 3;
    #if 0
    if(thread_name[0] == 's') // send_worker
        uxPriority = tskIDLE_PRIORITY + 2;
    if(thread_name[0] == 'c') // callback_thread
        uxPriority = tskIDLE_PRIORITY + 2;
    if(thread_name[0] == 'w') // wsf_worker_thread
        uxPriority = tskIDLE_PRIORITY + 2;
    #endif

    ret = xTaskCreate ((pdTASK_CODE) thread_func, thread_name, stack_size/sizeof(portSTACK_TYPE), arg, uxPriority, thread);
    if (pdTRUE != ret)
        printf("create thread fail\n");

    printf("threadName=%s, threadId=0x%lx, priority=%ld\n",thread_name,(uint32_t)(*thread), uxPriority);

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}

int alink_is_current_thread(pthread_t * threadId)
{
    printf("%s : threadId=0x%lx, currentId=0x%lx", __FUNCTION__,(uint32_t)(*threadId),(uint32_t)xTaskGetCurrentTaskHandle());
    return (*threadId == xTaskGetCurrentTaskHandle());
}

void alink_exit_thread(void *retval)
{
    printf("%s : threadId=0x%lx\n",__FUNCTION__,(uint32_t)xTaskGetCurrentTaskHandle());
	vTaskDelete(NULL);
}

int alink_pthread_join(pthread_t pthread, void **retval)
{
	printf("%s\r\n", __FUNCTION__);

    return 0;
}

int alink_pthread_mutex_init (pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
	printf("%s : ", __FUNCTION__);

    BaseType_t ret;

    vSemaphoreCreateBinary (*mutex);
    ret = (*mutex != NULL)? ALINK_OK : ALINK_ERR;
    if (ALINK_ERR == ret)
        printf("create mutex fail\n");

    printf("mutexHandle=0x%lx\n", (uint32_t)(*mutex));

    return ret;
}
int alink_pthread_mutex_lock (pthread_mutex_t *mutex)
{
    // printf("%s : mutexHandle=0x%lx\n", __FUNCTION__, (uint32_t)(*mutex));

    BaseType_t ret;

    ret = xSemaphoreTake (*mutex, portMAX_DELAY);
    if (pdTRUE != ret)
        printf("mutex lock error : mutexHandle=0x%lx\n", (uint32_t)(*mutex));

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}
int alink_pthread_mutex_timedlock (pthread_mutex_t *mutex,int timeout)
{
    // printf("%s : mutexHandle=0x%lx, timeout=%d\n",__FUNCTION__,(uint32_t)(*mutex),timeout);

    BaseType_t ret;

    ret = xSemaphoreTake (*mutex, (timeout * 1000 / portTICK_PERIOD_MS));

    if (pdTRUE != ret)
        printf("mutex timelock timeout : mutexHandle=0x%lx\n", (uint32_t)(*mutex));

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}
int alink_pthread_mutex_unlock (pthread_mutex_t *mutex)
{
    // printf("%s : mutexHandle=0x%lx\n",__FUNCTION__,(uint32_t)(*mutex));

    BaseType_t ret;

    ret = xSemaphoreGive(*mutex);
    if (pdTRUE != ret)
        printf("mutex unlock fail : mutexHandle=0x%lx\n", (uint32_t)(*mutex));

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}
int alink_pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    printf("%s : mutexHandle=0x%lx\n",__FUNCTION__,(uint32_t)(*mutex));

    vSemaphoreDelete( *mutex );

    return ALINK_OK;
}

int alink_pthread_cond_init (pthread_cond_t *cond, pthread_condattr_t *attr)
{
	printf("%s : ", __FUNCTION__);

    BaseType_t ret;

    *cond = xSemaphoreCreateBinary ();
    ret = (*cond != NULL)? ALINK_OK : ALINK_ERR;
    if (ALINK_ERR == ret)
        printf("condition init fail\n");

    printf("condHandle=0x%lx\n", (uint32_t)(*cond));

    return ret;
}

int alink_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    alink_debug("%s : condHandle=0x%lx, mutex=0x%lx\n",__FUNCTION__,(uint32_t)(*cond), (uint32_t)(*mutex));

    BaseType_t ret;

    alink_pthread_mutex_unlock(mutex);
    ret = xSemaphoreTake(*cond, portMAX_DELAY);
    if (pdTRUE != ret)
        alink_debug("condition wait error : condHandle=0x%lx\n", (uint32_t)(*cond));
    alink_pthread_mutex_lock(mutex);

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}
int alink_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,int timeout)
{
    alink_debug("%s : condHandle=0x%lx, mutex=0x%lx, timeout=%d\n", __FUNCTION__,(uint32_t)(*cond), (uint32_t)(*mutex), timeout);

    BaseType_t ret;

    alink_pthread_mutex_unlock(mutex);

    ret = xSemaphoreTake(*cond, (timeout * 1000 / portTICK_PERIOD_MS));
    if (pdTRUE != ret)
        alink_debug("condition timedwait timeout : condHandle=0x%lx\n", (uint32_t)(*cond));
    alink_pthread_mutex_lock(mutex);

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;

}

int alink_pthread_cond_signal (pthread_cond_t *cond)
{
    alink_debug("%s : condHandle=0x%lx\n",__FUNCTION__,(uint32_t)(*cond));

    BaseType_t ret;

    ret = xSemaphoreGive(*cond);
    if (pdTRUE != ret)
        alink_debug("condition signal fail : condHandle=0x%lx\n", (uint32_t)(*cond));

    return (pdTRUE == ret)? ALINK_OK : ALINK_ERR;
}

int alink_pthread_cond_destroy(pthread_cond_t *cond)
{
    alink_debug("%s : condHandle=0x%lx\n",__FUNCTION__,(uint32_t)(*cond));

    vSemaphoreDelete( *cond );

    return ALINK_OK;
}

int alink_valid_socket(int sock)
{
    alink_debug("%s : sock %d\n", __FUNCTION__, sock);
    return (sock >= 0);
}

int alink_create_socket(int domain, int type, int protocol)
{
    alink_debug("%s : ", __FUNCTION__);

    int sock = lwip_socket(domain, type, protocol);
    alink_debug("sock %d\n", sock);
    return sock;
}

int alink_socket_close(int fd)
{
    alink_debug("%s : ", __FUNCTION__);

    int ret = lwip_close(fd);
    alink_debug("sock: %d, ret: %d \n", fd, ret);

    return ret;
}

int alink_resolve_ip(const char *addr, in_addr_t * ip)
{
    alink_debug("%s : addr: %s, ", __FUNCTION__, addr);

    struct hostent *hp;
    if ((hp = gethostbyname(addr)) == NULL) {
        printf("gethostbyname fail\n");
        return ALINK_SOCKET_ERR;
    }
    *ip = *((in_addr_t *) (hp->h_addr));
    alink_debug("ip: %s\n", inet_ntoa(*ip));

    return ALINK_OK;
}

alink_sockaddr_t *alink_create_addr(alink_sockaddr_t * addr, in_addr_t ip,
                                   short port)
{
    alink_debug("%s : ", __FUNCTION__);

    memset(addr, 0, sizeof(alink_sockaddr_t));
    addr->sin_family = AF_INET;

    addr->sin_addr.s_addr = ip;
    addr->sin_port = alink_htons(port);
    alink_debug("ip: %s, port: %d\n", inet_ntoa(addr->sin_addr.s_addr), port);

    return addr;
}

uint32_t alink_inet_addr(const char *ip_addr)
{
    // return inet_addr(ip_addr);
    return alink_ipaddr_addr(ip_addr);
}
char *alink_inet_ntoa(uint32_t ip_addr, char *ip_str, unsigned int buff_size)
{
    snprintf(ip_str, buff_size, "%s", inet_ntoa(*(struct in_addr *)&(ip_addr)));
    return ip_str;
}
char *alink_get_addr_string(alink_sockaddr_t *addr, char *ip_port_buff, unsigned int buff_size)
{
    snprintf(ip_port_buff, buff_size, "%s:%d",
        inet_ntoa((addr->sin_addr)), alink_ntohs(addr->sin_port));
    return ip_port_buff;
}
void alink_create_mreq_opt(struct ip_mreq *mreq, unsigned int group_addr, unsigned int interface_addr)
{
    mreq->imr_multiaddr.s_addr = group_addr;
    mreq->imr_interface.s_addr = interface_addr;
}

/*------------------------------------------------------------------------------*/
/* return sec */
unsigned int alink_get_os_time_sec(void)
{
    unsigned int result = time(NULL);

    alink_debug("%s : %d\n", __FUNCTION__, result);

    return result;
}

#if 0
/* return sec */
unsigned int alink_get_os_time_sec()
{
    alink_debug("%s : ", __FUNCTION__);

    /* calculate days from 2000.1.1 */
    hal_rtc_status_t status;
    hal_rtc_time_t rtc_time;
    static const int past_days[12] =
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    unsigned int year, result;
    if (HAL_RTC_STATUS_OK == hal_rtc_init()) {
        do {
	        status = hal_rtc_get_time(&rtc_time);
        } while(HAL_RTC_STATUS_OK != status);
        // printf("init pass\n");
        year = rtc_time.rtc_year + 2000; /* base year from RTC module */
        alink_debug("current time: %d-%d-%d %d:%d:%d\n",
                year,
                rtc_time.rtc_mon,
                rtc_time.rtc_day,
                rtc_time.rtc_hour,
                rtc_time.rtc_min,
                rtc_time.rtc_sec);
        /* calculate days start */
        result = (year - 2000) * 365 + past_days[(rtc_time.rtc_mon-1) % 12];
        result += (year - 1996) / 4;
        result -= (year - 1996) / 100;
        result += (year - 1996) / 400;
        /* minus one day if this is leap year, but the date is before Feb. 29th */
		if (((year % 4) == 0 && (year % 100) != 0 || (year % 400) == 0) &&
            (rtc_time.rtc_mon % 12) < 2)
            result--;

        result += rtc_time.rtc_day;
		/* calculate days end */

        result *= 24;
        result += rtc_time.rtc_hour;
        result *= 60;
        result += rtc_time.rtc_min;
        result *= 60;
        result += rtc_time.rtc_sec;

        return result;
    }
    else {
        printf("fail\n");
        return -1;
    }
}

uint32_t previous_count = 0;
uint32_t next_count = 0;
uint32_t gpt_sec = 0;
unsigned int alink_get_os_time_sec()
{
    alink_debug("%s : ", __FUNCTION__);

    uint32_t gpt_count;
    hal_gpt_status_t status;
    uint32_t duration_count;
    status = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_count);
    if (status == HAL_GPT_STATUS_OK) {
        // printf("init pass\n");
        previous_count = next_count;
        next_count = gpt_count;
        if (next_count > previous_count) {
            duration_count = next_count - previous_count;
        } else {
            duration_count = (0xffffffff - (previous_count - next_count)) + 1;
        }
        alink_debug("duration_count: %d\n", duration_count);
        /* change duration_count to sec */
        duration_count = (uint32_t)(duration_count / 32768);
        gpt_sec = gpt_sec + duration_count;
        alink_debug("gpt_sec: %d\n", gpt_sec);
        return gpt_sec;
    }
    else {
        printf("fail\n");
        return -1;
    }
}
#endif

void alink_sleep(int millsec)
{
    // printf("%s : millsec:%d\n", __FUNCTION__, millsec);

    TickType_t xDelay;

    xDelay = millsec * portTICK_PERIOD_MS;
    vTaskDelay(xDelay);

}

int is_alink_network_up = 0;
void alink_wait_network_up()
{
    uint8_t link_status = 0;
    struct netif *sta_if = NULL;

    printf("%s : start\n", __FUNCTION__);

    //check wifi link
    while(!link_status){
        wifi_connection_get_link_status(&link_status);
        if(!link_status){
            /*wait until wifi connect*/
            alink_debug("Waiting for STA link up...\n");
            alink_sleep(1000);
        }
    }

    //check netif up
    while(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
        sta_if = netif_find("st2");
        if(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
            /*wait until netif up*/
            alink_debug("Waiting for netif up...\n");
            alink_sleep(1000);
        }
    }

    while (!(sta_if->dhcp && (sta_if->dhcp->state == DHCP_STATE_BOUND || sta_if->dhcp->state == DHCP_STATE_RENEWING))) {
        /*wait until got ip address*/
        alink_debug("Waiting for got ip address...\n");
        alink_sleep(1000);
    }

    is_alink_network_up = 1;

    printf("%s : end\n", __FUNCTION__);
}

int alink_send(int s, char *buf, int len, int flags)
{
    alink_debug("%s : start", __FUNCTION__);

    int ret = send(s, buf, len, flags);
    alink_debug("%s : end, sock: %d, len: %d, flags: 0x%dx, ret: %d\n", __FUNCTION__, s, len, flags, ret);

    return ret;
}

int alink_recv(int s, char *buf, int len, int flag)
{
    alink_debug("%s : start", __FUNCTION__);

    int ret = recv(s, buf, len, flag);

    alink_debug("%s : end, sock: %d, len: %d, flags: 0x%dx, ret: %d\n", __FUNCTION__, s, len, flag, ret);
    return ret;
}

unsigned int alink_htonl(unsigned int n)
{
    unsigned int ret = lwip_htonl(n);

    return ret;
}

unsigned int alink_ntohl(unsigned int n)
{
    unsigned int ret = lwip_ntohl(n);

    return ret;
}

unsigned short alink_htons(unsigned short n)
{
    unsigned int ret = lwip_htons(n);

    return ret;
}

unsigned short alink_ntohs(unsigned short n)
{
    unsigned int ret = lwip_ntohs(n);

    return ret;
}

#if 0
int ALINK_FD_SET(int sd, fd_set *sockSet)
{
    printf("%s : sd:%d, sockSet:%p, ", __FUNCTION__, sd, sockSet);

    if(sd < 0 )
        printf("sd error\n");
        //return 0;

    int ret = FD_SET(sd, sockSet);

    printf("ret = %s\n", ret);

    return ret;

}

void ALINK_FD_ZERO(fd_set *sockSet)
{
    printf("%s\n", __FUNCTION__);

    FD_ZERO(sockSet);
}

int ALINK_FD_ISSET(int sd, fd_set *sockSet)
{
    printf("%s : sd:%d, sockSet:%p, ", __FUNCTION__, sd, sockSet);

    if(sd < 0 )
        printf("sd error\n");
        //return 0;

    int ret = FD_ISSET(sd, sockSet);

    printf("ret = %s\n", ret);

    return ret;
}
#endif

void* alink_calloc_func(size_t size, size_t num)
{
    void *buf = NULL;
    buf = pvPortCalloc(num, size);

    if(NULL == buf)
        printf("alink_calloc_func() Error! size:%d, num:%d", size, num);

    /*
        uint32_t  free_size;
        free_size = xPortGetFreeHeapSize();
        printf("alink_calloc_func() free_size:%d, buf:%p, sz:%d, num:%d", free_size, buf, size, num);
     */

    return buf;
}

void* alink_malloc_func(size_t sz)
{
    void *buf = NULL;
    buf = pvPortMalloc(sz);

    if(NULL == buf)
        printf("alink_malloc_func() Error! size:%d", sz);

    /*
        uint32_t  free_size;
        free_size = xPortGetFreeHeapSize();
        printf("alink_malloc_func() free_size:%d, buf:%p, sz:%d", free_size, buf, sz);
    */


    return buf;
}

void alink_free_func(void *ptr)
{
    /*
        uint32_t  free_size;
        free_size = xPortGetFreeHeapSize();
        printf("alink_free_func() free_size:%d, buf:%p", free_size, ptr);
    */

    if(ptr)
        vPortFree(ptr);
}

int alink_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

	int ret = lwip_select(maxfdp1, readset, writeset, exceptset, timeout);

	alink_debug("%s : end, ret:%d\n", __FUNCTION__, ret);
    return ret;
}

int alink_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

	int ret = lwip_connect(s, name, namelen);

	alink_debug("%s : end, soid:%d, ret:%d\n", __FUNCTION__, s, ret);
    return ret;
}

int alink_sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

	int ret = lwip_sendto(s, data, size, flags, to, tolen);

	alink_debug("%s : end, soid:%d, ret:%d, size:%d, flags:%d\n", __FUNCTION__, s, ret, size, flags);
    return ret;
}

int alink_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

    // mtk workaround
    if (optname == IP_ADD_MEMBERSHIP)
        level = IPPROTO_IP;
    else if (optname == SO_REUSEADDR)
        level = SOL_SOCKET;

	int ret = lwip_setsockopt(s, level, optname, optval, optlen);

	alink_debug("%s : end, soid:%d, ret:%d, level:%d, optname:%d\n", __FUNCTION__, s, ret, level, optname);
    return ret;
}

int alink_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

    int ret = lwip_bind(s, name, namelen);

    alink_debug("%s : end, soid:%d, ret:%d\n", __FUNCTION__, s, ret);
    return ret;
}

int alink_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

    int ret = lwip_recvfrom(s, mem, len, flags, from, fromlen);

    alink_debug("%s : end, soid:%d, ret:%d, len:%d, flags:%d\n", __FUNCTION__, s, ret, len, flags);
    return ret;
}

int alink_getsockname(int s, struct sockaddr *name, socklen_t *namelen)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

    int ret = lwip_getsockname(s, name, namelen);

    alink_debug("%s : end, soid:%d, ret:%d\n", __FUNCTION__, s, ret);
    return ret;
}

unsigned int alink_ipaddr_addr(const char *cp)
{
    alink_debug("%s : start\r\n", __FUNCTION__);

    int ret = ipaddr_addr(cp);

    alink_debug("%s : end, ret:%d, cp:%s\n", __FUNCTION__, ret, cp);
    return ret;
}

int alink_get_free_heap_size()
{
    int size = xPortGetFreeHeapSize();

    printf("%s : size:%d\n", __FUNCTION__, size);

    return size;
}

char * alink_get_device_sn()
{
    printf("%s\n", __FUNCTION__);

    return NULL;
}

char * alink_get_device_mac()
{
    char mac_addr[WIFI_MAC_ADDRESS_LENGTH]={0};
    static char address[STR_MAC_LEN] = {0};
    printf("%s : ", __FUNCTION__);

    int32_t ret;
    ret = wifi_config_get_mac_address(WIFI_PORT_STA, (uint8_t *)mac_addr);
    if(ret == 0)
        snprintf(address, STR_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
    printf("ret:%d, address:%s \n", (int)ret, address);
    return address;
}

char * alink_get_device_chipid()
{
    printf("%s\n", __FUNCTION__);

    return NULL;
}
char * alink_get_device_ip()
{
    alink_debug("%s : ", __FUNCTION__);

    struct netif *sta_if = NULL;
    static char address[16] = {0};
    uint32_t addr;

    sta_if = netif_find("st2");

    if (sta_if) {
        addr = ip4_addr_get_u32(netif_ip4_addr(sta_if));
        alink_inet_ntoa(addr, address, 16);
        alink_debug("address: %s\n", address);
    } else {
        printf("netif_find fail\n");
    }

    return address;
}

#if 0
int alink_wifi_connect(char * ssid,char *password)
{
    // while wifi connected
    return 0;
}
#endif

int alink_read_flash(char * buffer ,int len)
{
    printf("%s : ", __FUNCTION__);
    // unsigned int flashAddr;
    // int status;

    // status = hal_flash_read(CM4_FLASH_ALINK_ADDR, buffer, len);

    nvdm_status_t status;
    status= nvdm_read_data_item(
        "Alink",
        "Config",
        (uint8_t *)buffer,
        (uint32_t *)&len);

    int i;
    printf("ret:%d, buff(len:%d):", status, len);
    for (i=0; i<len; i++)
        printf("%c", buffer[i]);
    printf("\r\n");

    return status;
}
int alink_write_flash(char * buffer ,int len)
{
    printf("%s : ", __FUNCTION__);
    // unsigned int flashAddr;
    // int status;

    // hal_flash_erase(CM4_FLASH_ALINK_ADDR, HAL_FLASH_BLOCK_4K);
    // status = hal_flash_write(CM4_FLASH_ALINK_ADDR, buffer, len);

    nvdm_status_t status;
    status = nvdm_write_data_item(
        "Alink",
        "Config",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)buffer,
        (uint32_t)len);

    int i;
    printf("ret:%d, buff(len:%d):", status, len);
    for (i=0; i<len; i++)
        printf("%c", buffer[i]);
    printf("\r\n");

    return status;
}

static int32_t s_isInited = 0;

int alink_fw_download(char * buffer, unsigned int len)
{
    fota_status_t ret = FOTA_STATUS_OK;
    // init flash layout info
    printf("%s, buffer_len:%d\r\n", __FUNCTION__, len);

    if(!s_isInited)
    {
        if (FOTA_STATUS_OK == fota_init(&fota_flash_default_config))
        {
            s_isInited = 1;
        }
    }

    ret = fota_write(FOTA_PARITION_TMP, (const uint8_t *)buffer, (uint32_t)len);

    if (FOTA_STATUS_OK == ret) {
        return 0;
    }
    else {
        printf("%s : write flash fail, err = %d\n", __FUNCTION__, ret);
        return -1;
    }
}

int alink_fw_check(void)
{
    printf("%s\r\n", __FUNCTION__);

    // firmware check will be performed in bootloader, maybe it's not necessary after downloading.
    return 0;
}

int alink_fw_upgrade(void)
{
    fota_ret_t fota_ret = FOTA_TRIGGER_FAIL;
    // hal_wdt_status_t wdt_ret;
    printf("%s : ", __FUNCTION__);

    fota_ret = fota_trigger_update();
    if (FOTA_TRIGGER_SUCCESS == fota_ret) {
        // reboot device
        // hal_wdt_config_t wdt_init;
        // wdt_init.mode = HAL_WDT_MODE_RESET;
        // wdt_init.seconds = 4;

        // wdt_ret = hal_wdt_init(&wdt_init);
        // TODO:
        printf("reboot Bye Bye Bye!!!!\n");
        // wdt rest
        // wdt_ret = hal_wdt_software_reset();

        //reboot api
        //hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
        hal_wdt_software_reset();
        return 0;
    } else {
        printf("%s : fail to trigger update, err = %d\n", __FUNCTION__, fota_ret);
        return -1;
    }
}

void alink_os_reboot(void)
{
    printf("%s \n", __FUNCTION__);
    printf("reboot Bye Bye Bye!!!!\n");

    //reboot api
    //hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
    hal_wdt_software_reset();
}

int alink_get_wifi_station_rssi(void)
{
    printf("%s : ", __FUNCTION__);

    int8_t rssi;
    if(wifi_connection_get_rssi(&rssi) < 0)
        rssi = 0;

    printf("rssi:%d \n", rssi);
    return rssi;
}

#if 0
int ssl_close(void *ssl)
{
    return 0;
}
#endif

/*u32 convert_time(const char *str)
{
    return 0;
}*/

#if defined(MBEDTLS_MD5_C)
int alink_md5_init(MD5_CTX * ctx)
{
    alink_debug("%s\n", __FUNCTION__);

	if (!ctx)
	{
		return ALINK_ERR;
	}

	mbedtls_md5_init((mbedtls_md5_context *)ctx);
	mbedtls_md5_starts((mbedtls_md5_context *)ctx);

    return ALINK_OK;
}

int alink_md5_update(MD5_CTX * ctx, unsigned char *input, int ilen)
{
    alink_debug("%s\n", __FUNCTION__);

	if (!ctx || !input || !ilen)
	{
		return ALINK_ERR;
	}

	mbedtls_md5_update((mbedtls_md5_context *)ctx, input, ilen);

    return ALINK_OK;
}

int alink_md5_final(MD5_CTX * ctx, unsigned char output[16])
{
    alink_debug("%s\n", __FUNCTION__);

    if (!ctx || !output)
	{
		return ALINK_ERR;
	}

	mbedtls_md5_finish((mbedtls_md5_context *)ctx, output);
	mbedtls_md5_free((mbedtls_md5_context *)ctx);

    return ALINK_OK;
}

#else

int alink_md5_init(MD5_CTX * ctx)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
    return ALINK_ERR;
}

int alink_md5_update(MD5_CTX * ctx, unsigned char *input, int ilen)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}

int alink_md5_final(MD5_CTX * ctx, unsigned char output[16])
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
    return ALINK_ERR;
}
#endif /* MBEDTLS_MD5_C */

#if defined(MBEDTLS_SHA256_C)
int alink_sha256_init(SHA256_CTX * ctx)
{
    alink_debug("%s\n", __FUNCTION__);

	if (!ctx)
	{
		return ALINK_ERR;
	}

	mbedtls_sha256_init((mbedtls_sha256_context *)ctx);
	mbedtls_sha256_starts((mbedtls_sha256_context *)ctx, 0);

	return ALINK_OK;
}

int alink_sha256_update(SHA256_CTX * ctx, const void *data,unsigned long len)
{
    alink_debug("%s\n", __FUNCTION__);

	if (!ctx || !data || !len)
	{
		return ALINK_ERR;
	}

	mbedtls_sha256_update((mbedtls_sha256_context *)ctx, data, len);

	return ALINK_OK;
}

int alink_sha256_final(unsigned char *res, SHA256_CTX * ctx)
{
    alink_debug("%s\n", __FUNCTION__);

	if (!ctx || !res)
	{
		return ALINK_ERR;
	}

	mbedtls_sha256_finish((mbedtls_sha256_context *)ctx, res);
	mbedtls_sha256_free((mbedtls_sha256_context *)ctx);

	return ALINK_OK;
}

#else

int alink_sha256_init(SHA256_CTX * ctx)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}

int alink_sha256_update(SHA256_CTX * ctx, const void *data,unsigned long len)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}

int alink_sha256_final(unsigned char *res, SHA256_CTX * ctx)
{
	printf("%s : macro not enable\r\n", __FUNCTION__);
	return ALINK_ERR;
}
#endif /* MBEDTLS_SHA256_C */

int alink_get_vender_sdk_version(char * buffer,unsigned int len)
{
    alink_debug("%s: len%d, ", __FUNCTION__, len);

    char *sdk_version = "SDK_V1.2.2_[public]";

    memset(buffer, 0, len);
    if(strlen(sdk_version) >= len)
        memcpy(buffer, sdk_version, len-1);
    else
        memcpy(buffer, sdk_version, strlen(sdk_version));

    alink_debug("buffer: %s\n", buffer);

    return 0;
}
// #endif
