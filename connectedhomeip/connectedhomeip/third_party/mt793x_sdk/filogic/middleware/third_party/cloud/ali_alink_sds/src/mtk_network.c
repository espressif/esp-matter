#include <netdb.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "platform.h"
#include "lwip/sockets.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "alink_export.h"
#include "syslog.h"

#define SOMAXCONN 5
#ifdef ALINK_SDS_PRINTF
#define alink_log  printf
#else
#define alink_log(fmt, args...)     LOG_I(alink_sds, "[alink] "fmt, ##args)
#define alink_loge(fmt, args...)    LOG_E(alink_sds, "[alink] "fmt, ##args)
#endif

static int network_create_socket( pplatform_netaddr_t netaddr, int type, struct sockaddr_in *paddr, int *psock)
{
	struct hostent *hp;
	struct in_addr in;
	uint32_t ip;
	int opt_val = 1;

    assert(paddr && psock);

    if (NULL == netaddr->host)
    {
    	ip = htonl(INADDR_ANY);
    }
    else
    {
        /*
         * in some platform gethostbyname() will return bad result
         * if host is "255.255.255.255", so calling inet_aton first
         */
        if (inet_aton(netaddr->host, &in)) {
            ip = *(uint32_t *)&in;
        } else {
            hp = gethostbyname(netaddr->host);
            if (!hp) {
                alink_loge("can't resolute the host address");
                return -1;
            }
            ip = *(uint32_t *)(hp->h_addr);
		}
    }

    *psock = socket(AF_INET, type, 0);
    if (*psock < 0)
    {
        perror("socket");
    	return -1;
    }

    memset(paddr, 0, sizeof(struct sockaddr_in));

    if (0 != setsockopt(*psock, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)))
    {
        perror("setsockopt");
        close((int)*psock);
        return -1;
    }

    if (type == SOCK_DGRAM) {
        if (0 != setsockopt(*psock, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(opt_val)))
        {
            perror("setsockopt");
            close((int)*psock);
            return -1;
        }
    }

    paddr->sin_addr.s_addr = ip;
	paddr->sin_family = AF_INET;
	paddr->sin_port = htons( netaddr->port );

	return 0;
}


void *platform_udp_server_create(_IN_ uint16_t port)
{
	struct sockaddr_in addr;
	int server_socket;
	platform_netaddr_t netaddr = {NULL, port};

	if (0 != network_create_socket(&netaddr, SOCK_DGRAM, &addr, &server_socket))
	{
		return PLATFORM_INVALID_FD;
	}

    if (-1 == bind(server_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)))
    {
        platform_udp_close((void *)server_socket);
    	return PLATFORM_INVALID_FD;
    }

    return (void *)server_socket;
}




void *platform_udp_client_create(void)
{
	struct sockaddr_in addr;
	int sock;
	platform_netaddr_t netaddr = {NULL, 0};

	if (0 != network_create_socket(&netaddr, SOCK_DGRAM, &addr, &sock))
	{
		return PLATFORM_INVALID_FD;
	}

	return (void *)sock;
}



void *platform_udp_multicast_server_create(pplatform_netaddr_t netaddr, const char *ifname)
{
	int option = 1;
	struct sockaddr_in addr;
    int sock = 0;
    struct ip_mreq mreq;
    /* Note: ignore host right now */
    platform_netaddr_t netaddr_client = {NULL, netaddr->port};

    memset(&addr, 0, sizeof(addr));
    memset(&mreq, 0, sizeof(mreq));

    if (0 != network_create_socket(&netaddr_client, SOCK_DGRAM, &addr, &sock)) {
        return PLATFORM_INVALID_FD;
    }

    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0) {
        perror("setsockopt");
        goto err;
    }

    if (-1 == bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
        perror("bind");
        goto err;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(netaddr->host);
    if(NULL == ifname || strlen(ifname) == 0){
        mreq.imr_multiaddr.s_addr = inet_addr(netaddr->host);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);      //defaut router if ip
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq)) < 0) {
            alink_loge("setsockopt membership error, ip:default router, multicast addr:%s", netaddr->host);
            goto err;
        }
    }
    else{
        char ip_str[PLATFORM_IP_LEN] = {0};
        if (platform_wifi_get_ip(ip_str, ifname) < 0) {
            alink_loge("get %s interface ip address fail", ifname);
            goto err;
        }
        mreq.imr_multiaddr.s_addr = inet_addr(netaddr->host);
        mreq.imr_interface.s_addr = inet_addr(ip_str); //lan ip
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, sizeof(mreq)) < 0) {
            alink_loge("setsockopt membership error , ip:%s,  multicast addr:%s", ip_str, netaddr->host);
            goto err;
        }
    }

    return (void *)sock;

err:
    if(sock > 0){
        platform_udp_close((void *)sock);
        return PLATFORM_INVALID_FD;
    }
    return PLATFORM_INVALID_FD;
}



void platform_udp_close(void *handle)
{
	close((int)handle);
}



int platform_udp_sendto(
		_IN_ void *handle,
		_IN_ const char *buffer,
		_IN_ uint32_t length,
		_IN_ pplatform_netaddr_t netaddr)
{
	int ret;
	uint32_t ip;
    struct in_addr in;
	struct hostent *hp;
	struct sockaddr_in addr;

    /*
     * in some platform gethostbyname() will return bad result
     * if host is "255.255.255.255", so calling inet_aton first
     */
    if (inet_aton(netaddr->host, &in)) {
        ip = *(uint32_t *)&in;
    } else {
        hp = gethostbyname(netaddr->host);
        if (!hp) {
            alink_loge("can't resolute the host address");
            return -1;
        }
        ip = *(uint32_t *)(hp->h_addr);
    }

	addr.sin_addr.s_addr = ip;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(netaddr->port);

	ret = sendto((int)handle,
					   buffer,
					   length,
					   0,
					   (struct sockaddr *)&addr,
					   sizeof(struct sockaddr_in));

    if (ret < 0)
        perror("sendto");

	return (ret) > 0 ? ret : -1;
}


int platform_udp_recvfrom(
		_IN_ void *handle,
		_OUT_ char *buffer,
		_IN_ uint32_t length,
		_OUT_OPT_ pplatform_netaddr_t netaddr)
{
	int ret;
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);

	ret = recvfrom((int)handle, buffer, length, 0, (struct sockaddr *)&addr, &addr_len);
	if (ret > 0)
	{
		if (NULL != netaddr)
		{
            netaddr->port = ntohs(addr.sin_port);

            if (NULL != netaddr->host)
            {
                strcpy(netaddr->host, inet_ntoa(addr.sin_addr));
            }
		}

		return ret;
	}

	return -1;
}



void *platform_tcp_server_create(_IN_ uint16_t port)
{
	struct sockaddr_in addr;
	int server_socket;
	platform_netaddr_t netaddr = {NULL, port};

	if (0 != network_create_socket(&netaddr, SOCK_STREAM, &addr, &server_socket))
	{
		return PLATFORM_INVALID_FD;
	}

    if (-1 == bind(server_socket, (struct sockaddr *)&addr, sizeof(addr)))
    {
    	platform_tcp_close((void *)server_socket);
    	return PLATFORM_INVALID_FD;
    }

    if (0 != listen(server_socket, SOMAXCONN))
	{
    	platform_tcp_close((void *)server_socket);
		return PLATFORM_INVALID_FD;
	}

    return (void *)server_socket;
}




void *platform_tcp_server_accept(_IN_ void *server)
{
	struct sockaddr_in addr;
	int addr_length = sizeof(addr);
	int new_client;

	if ((new_client = accept((int)server,(struct sockaddr*)&addr, &addr_length)) <= 0)
	{
		return PLATFORM_INVALID_FD;
	}

	return (void *)new_client;
}



extern int g_ip_ready ;
void *platform_tcp_client_connect(_IN_ pplatform_netaddr_t netaddr)
{
	struct sockaddr_in addr;
	int sock;

    if(g_ip_ready == 0)
    {
        lwip_net_ready();
        g_ip_ready = 1;
    }

	if (0 != network_create_socket(netaddr, SOCK_STREAM, &addr, &sock))
	{
		return PLATFORM_INVALID_FD;
	}

	if (-1 == connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)))
	{
		platform_tcp_close((void *)sock);
		return PLATFORM_INVALID_FD;
	}

	return (void *)sock;
}



int platform_tcp_send(_IN_ void *handle, _IN_ const char *buffer, _IN_ uint32_t length)
{
	int bytes_sent;

	bytes_sent = send((int)handle, buffer, length, 0);
	return bytes_sent > 0 ? bytes_sent : -1;
}



int platform_tcp_recv(_IN_ void *handle, _OUT_ char *buffer, _IN_ uint32_t length)
{
	int bytes_received;

	bytes_received = recv((int)handle, buffer, length, 0);

	return bytes_received > 0 ? bytes_received : -1;
}




void platform_tcp_close(_IN_ void *handle)
{
	close((int)handle);
}



int platform_select(void *read_fds[PLATFORM_SOCKET_MAXNUMS],
		void *write_fds[PLATFORM_SOCKET_MAXNUMS],
		int timeout_ms)
{
	int i, ret = -1;
	struct timeval timeout_value;
	struct timeval *ptimeval = &timeout_value;
	fd_set r_set, w_set;

	if (PLATFORM_WAIT_INFINITE == timeout_ms)
		ptimeval = NULL;
	else {
		ptimeval->tv_sec = timeout_ms / 1000;
		ptimeval->tv_usec = (timeout_ms % 1000) * 1000;
	}

    FD_ZERO(&r_set);
    FD_ZERO(&w_set);

    if (read_fds) {
        for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
            if (PLATFORM_INVALID_FD != read_fds[i])
                FD_SET((int)read_fds[i], &r_set);
        }
    }

    if (write_fds) {
        for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
            if ( PLATFORM_INVALID_FD != write_fds[i] )
                FD_SET((int)write_fds[i], &w_set);
        }
    }

	ret = select(FD_SETSIZE, &r_set, &w_set, NULL, ptimeval);

	if (ret > 0) {
		if (read_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
				if (PLATFORM_INVALID_FD != read_fds[i]
                        && !FD_ISSET((int)read_fds[i], &r_set))
					read_fds[i] = PLATFORM_INVALID_FD;
			}
		}

		if (write_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i) {
				if (PLATFORM_INVALID_FD != write_fds[i]
                        && !FD_ISSET((int)write_fds[i], &w_set))
					write_fds[i] = PLATFORM_INVALID_FD;
			}
		}
	} else {/* clear all fd */
		if (read_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i)
					read_fds[i] = PLATFORM_INVALID_FD;
		}

		if (write_fds) {
			for (i = 0; i < PLATFORM_SOCKET_MAXNUMS; ++i)
					write_fds[i] = PLATFORM_INVALID_FD;
		}
    }

	return (ret >= 0) ? ret : -1;
}

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


#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include <string.h>
#if defined(MBEDTLS_DEBUG_C)
#include "mbedtls/debug.h"
#endif

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


int platform_ssl_close(void *ssl)
{
	alink_mbedtls_context_struct *alink_ssl_ctx = (alink_mbedtls_context_struct *)ssl;

	alink_loge("ssl_close");

	if (!ssl)
	{
        alink_loge("close parameter error, ssl is null");
		return ALINK_ERR;
	}

	platform_mutex_lock(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);
	mbedtls_ssl_close_notify(&alink_ssl_ctx->ssl_ctx);

    mbedtls_x509_crt_free(&alink_ssl_ctx->cacert);
    mbedtls_ssl_free(&alink_ssl_ctx->ssl_ctx);
    mbedtls_ssl_config_free(&alink_ssl_ctx->ssl_conf);
    mbedtls_ctr_drbg_free(&alink_ssl_ctx->ctr_drbg);
    mbedtls_entropy_free(&alink_ssl_ctx->entropy);
	platform_mutex_unlock(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);

	platform_mutex_destroy(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);
	platform_free(alink_ssl_ctx);
	alink_ssl_ctx = NULL;

	return ALINK_OK;
}

static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
	alink_log("mbedtls[%d]%s:%04d: %s", level,file, line, str);
}

//void *platform_ssl_connect(int fd, int cert_len, const char *cert, int *err)
void *platform_ssl_connect(_IN_ void *tcp_fd, _IN_ const char *cert, _IN_ int cert_len)
{
	alink_mbedtls_context_struct *ssl = NULL;
	int authmode = MBEDTLS_SSL_VERIFY_NONE;
	const char *pers = "mbedtls_porting_to_alink";
	int value, ret = ALINK_OK;
	uint32_t flags;
	int fd  = (int)tcp_fd;

	if (fd < 0 || (!cert_len && cert) || (cert_len && !cert))
	{
	    alink_loge("Parameter error");
		ret = fd < 0 ? ALINK_SOCKET_ERR : ALINK_ERR;
		goto exit;
	}

	ssl = (alink_mbedtls_context_struct *)platform_calloc_func(1, sizeof(alink_mbedtls_context_struct));
	if (!ssl)
	{
	    alink_loge("No memory");
		ret = ALINK_ERR_NO_MEMORY;
		goto exit;
	}

	// printf("ssl:0x%x\r\n", ssl);
	alink_log("ssl:0x%p", ssl);

	ssl->ssl_mutex = platform_mutex_init();

	platform_mutex_lock(ssl->ssl_mutex);

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
	    alink_loge("mbedtls_ctr_drbg_seed() failed, value:-0x%x", -value);

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

	//printf("Cert:\r\n%s", cert);

	/* cert_len passed in is gotten from strlen not sizeof */
	if (cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
									    (const unsigned char *)cert,
									     cert_len + 1)) < 0))
	{
	    alink_loge("mbedtls_x509_crt_parse() failed, value:-0x%x", -value);
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
        alink_loge("mbedtls_ssl_config_defaults() failed, value:-0x%x", -value);
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
        alink_loge("mbedtls_ssl_setup() failed, value:-0x%x", -value);

        ret = ALINK_ERR;
		goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx,
						&ssl->net_ctx,
						mbedtls_net_send,
						mbedtls_net_recv,
						NULL);

	ret = mbedtls_net_set_nonblock(&ssl->net_ctx);
	alink_log("Set socket to be non-blocking for mbedtls. ret:%d", ret);

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
			alink_loge("mbedtls_ssl_handshake() failed, ret:-0x%x", -ret);

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
    alink_log("ret=%d\n", ret);
#if 0
	if (err)
	{
		*err = ret;
	}
#endif
	if (ALINK_OK != ret && ssl)
	{
		mbedtls_x509_crt_free(&ssl->cacert);
		mbedtls_ssl_free(&ssl->ssl_ctx);
		mbedtls_ssl_config_free(&ssl->ssl_conf);
		mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
		mbedtls_entropy_free(&ssl->entropy);

		platform_mutex_unlock(ssl->ssl_mutex);
		platform_mutex_destroy(ssl->ssl_mutex);
		platform_free(ssl);
		ssl = NULL;

        return NULL;
	}

	platform_mutex_unlock(ssl->ssl_mutex);
	return (void *)ssl;
}

int platform_ssl_pending(void *ssl)
{
	alink_log("ssl_pending");

	int avail_bytes = 0;

	if (!ssl)
	{
		return ALINK_ERR;
	}

	//alink_pthread_mutex_lock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
	avail_bytes = (int)mbedtls_ssl_get_bytes_avail(&(((alink_mbedtls_context_struct *)ssl)->ssl_ctx));
	//alink_pthread_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
	alink_log("avail_bytes:%d", avail_bytes);

	return avail_bytes;
}

int platform_ssl_send(void *ssl, const char *ptr, int length)
{
	int ret = ALINK_ERR;

	alink_log("**%s=%p** ", pcTaskGetTaskName(xTaskGetCurrentTaskHandle()), xTaskGetCurrentTaskHandle());

	if (!ssl || !ptr || !length)
	{
        alink_log("send parameter error, ssl %p, ptr %p, length %d ", ssl, ptr, length);
		return ALINK_ERR;
	}

	do {
	platform_mutex_lock(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);
	ret = mbedtls_ssl_write(&((alink_mbedtls_context_struct *)ssl)->ssl_ctx,
							(const unsigned char *)ptr, (size_t)length);
	platform_mutex_unlock(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);

		if( ret == MBEDTLS_ERR_SSL_WANT_READ ||
			ret == MBEDTLS_ERR_SSL_WANT_WRITE )
	{
			continue;
		}

		break;

	} while (1);


	if (0 < ret)
	{
		alink_log("send_ret=%d", ret);
	}
	else
	{
		alink_loge("send_ret=-0x%x", -ret);
	}

	return ret <= 0 ? ALINK_ERR : ret;
}

//int platform_ssl_recv(void *ssl, void *buf, int length)
int platform_ssl_recv(_IN_ void *ssl, _IN_ char *buf, _IN_ int length)
{
    fd_set rfds,exps;
	int sel_ret = ALINK_ERR, rcv_ret = ALINK_ERR;

    alink_log("**%s=%p** ", pcTaskGetTaskName(xTaskGetCurrentTaskHandle()), xTaskGetCurrentTaskHandle());

	if (!ssl || !buf || !length)
	{
        alink_log("recv parameter error, ssl %p, ptr %p, length %d ", ssl, buf, length);
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
	platform_mutex_unlock(&(((alink_mbedtls_context_struct*)ssl)->ssl_mutex));
				  } else {
				  	printf("exception happen!");
				  	return ALINK_ERR;
				  }
     }
#endif

	do {
        int i;
		sel_ret = lwip_select(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd + 1, &rfds, NULL, &exps, NULL);
		if (sel_ret > 0) {
			if (!FD_ISSET(((alink_mbedtls_context_struct*)ssl)->net_ctx.fd, &rfds))
	{
				continue;
	}
			platform_mutex_lock(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);
			rcv_ret = mbedtls_ssl_read(&((alink_mbedtls_context_struct *)ssl)->ssl_ctx,
										//(unsigned char *)buf, (size_t)length - 1);
										(unsigned char *)buf, (size_t)length);
        #if 0
            printf("----recv_ret=%d, length=%d----\r\n", rcv_ret, length);
            printf("buf: ");
            for(i = 0; i < rcv_ret; i++)
                printf("%02X", buf[i]);
            printf("\r\n");
        #endif
			platform_mutex_unlock(((alink_mbedtls_context_struct*)ssl)->ssl_mutex);

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
			alink_loge("exception happen!");
			return ALINK_ERR;
		}

	} while( 1 );

	if (0 < rcv_ret)
	{
		alink_log("recv_ret=%d", rcv_ret);
	}
	else
	{
		alink_loge("recv_ret=-0x%x", -rcv_ret);
	}

	return rcv_ret <= 0 ? ALINK_ERR : rcv_ret;
}

#endif
