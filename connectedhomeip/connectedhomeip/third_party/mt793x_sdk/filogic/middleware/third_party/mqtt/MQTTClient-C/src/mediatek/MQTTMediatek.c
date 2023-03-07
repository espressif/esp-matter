#include <stdint.h>
#include <stdio.h>

#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#if 0 // not 2523
#include "timer.h"
#else // 2523
#include "hal_gpt.h"
#endif

#if LWIP_SOCKET
#include "lwip/sockets.h"
#endif
#include "mbedtls/debug.h"
#define DEBUG_LEVEL 3
#define CLK32_TICK_TO_MS (32)

#include "MQTTMediatek.h"
log_create_module(MQTT_CLIENT, PRINT_LEVEL_INFO);

int mqtt_read(Network *n, unsigned char *buffer, int len, int timeout_ms);
int mqtt_write(Network *n, unsigned char *buffer, int len, int timeout_ms);
void mqtt_disconnect(Network *n);

static unsigned int mqtt_current_time_ms(void)
{
    unsigned int current_ms = 0;

#if 0  // not 2523
    // time in unit of 32k clock,need transfer to ms
    current_ms = get_current_count() / CLK32_TICK_TO_MS;
#else // 2523
    uint32_t count = 0;
    uint64_t count_temp = 0;
    hal_gpt_status_t ret_status;

    ret_status = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    if (HAL_GPT_STATUS_OK != ret_status) {
        printf("[%s:%d]get count error, ret_status = %d \n", __FUNCTION__, __LINE__, ret_status);
    }
    
    count_temp = (uint64_t)count * 1000;
    current_ms = (uint32_t)(count_temp / 32768);
    
#endif
    return current_ms;
}


char expired(Timer *timer)
{
    unsigned int cur_time = 0;
    cur_time = mqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) {
        MQTT_DBG("MQTT expired enter");
        return 1;
    } else {
        MQTT_DBG("MQTT not expired");
        return 0;
    }
}


void countdown_ms(Timer *timer, unsigned int timeout)
{
    timer->end_time = mqtt_current_time_ms() + timeout;
}


void countdown(Timer *timer, unsigned int timeout)
{
    timer->end_time = mqtt_current_time_ms() + (timeout * 1000);
}


unsigned int left_ms(Timer *timer)
{
    unsigned int cur_time = 0;
    cur_time = mqtt_current_time_ms();
    if (timer->end_time < cur_time || timer->end_time == cur_time) {
        return 0;
    } else {
        return timer->end_time - cur_time;
    }
}


void InitTimer(Timer *timer)
{
    timer->end_time = 0;
}


int mqtt_read(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    int rc = 0;
    int recvlen = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);


    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    ret = select(n->my_socket + 1, &fdset, NULL, NULL, &tv);
    MQTT_DBG("mqtt read timer=%d ms", timeout_ms);
    if (ret < 0) {
        MQTT_DBG("mqtt read(select) fail ret=%d", ret);
        return -1;
    } else if (ret == 0) {
        MQTT_DBG("mqtt read(select) timeout");
        return -2;
    } else if (ret == 1) {
        do {
            MQTT_DBG("mqtt read recv len = %d, recvlen = %d", len, recvlen);
            rc = recv(n->my_socket, buffer + recvlen, len - recvlen, 0);
            if (rc > 0) {
            recvlen += rc;
                MQTT_DBG("mqtt read ret=%d, rc = %d, recvlen = %d", ret, rc, recvlen);
            } else {
                MQTT_DBG("mqtt read fail: ret=%d, rc = %d, recvlen = %d", ret, rc, recvlen);
                if (n->on_disconnect_callback) {
                    n->on_disconnect_callback(n);
                }
                return -3;
            }
        } while (recvlen < len);
    }
    return recvlen;
}


int mqtt_write(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    int rc = 0;
    int ret = -1;
    fd_set fdset;
    struct timeval tv;

    FD_ZERO(&fdset);
    FD_SET(n->my_socket, &fdset);


    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    MQTT_DBG("mqtt write timer=%d ms", timeout_ms);
    ret = select(n->my_socket + 1, NULL, &fdset, NULL, &tv);

    if (ret < 0) {
        MQTT_DBG("mqtt write fail");
        return -1;
    } else if (ret == 0) {
        MQTT_DBG("mqtt write timeout");
        return -2;
    } else if (ret == 1) {
        rc = write(n->my_socket, buffer, len);
    }
    return rc;

}


void mqtt_disconnect(Network *n)
{
    close(n->my_socket);
}


void NewNetwork(Network *n)
{
    memset(n, 0, sizeof(Network));
    n->my_socket = -1;
    n->mqttread = mqtt_read;
    n->mqttwrite = mqtt_write;
    n->disconnect = mqtt_disconnect;
}

//tcp client
int ConnectNetwork(Network *n, char *addr,  char *port)
{
    int type = SOCK_STREAM;
    struct sockaddr_in address;
    int rc = -1;
    sa_family_t family = AF_INET;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

    if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0) {
        struct addrinfo *res = result;

        /* prefer ip4 addresses */
        while (res) {
            if (res->ai_family == AF_INET) {
                result = res;
                break;
            }
            res = res->ai_next;
        }

        if (result->ai_family == AF_INET) {
            address.sin_port = htons(atoi(port));
            address.sin_family = family = AF_INET;
            address.sin_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr;
#if MT76XX_MQTT_DEBUG
            int a = 0, b = 0, c = 0, d = 0;
            unsigned int temp = 0;

            temp = address.sin_addr.s_addr;
            a = temp & 0x000000FF;
            b = (temp >> 8) & 0x000000FF;
            c = (temp >> 16) & 0x000000FF;
            d = (temp >> 24) & 0x000000FF;
            MQTT_DBG("mqtt connect find addrinfo:%d.%d.%d.%d", a, b, c, d);
#endif
        } else {
            rc = -1;
        }
        freeaddrinfo(result);
    }
#if 0
    int ret;
    struct sockaddr_in addr_s;

    memset(&addr_s, 0, sizeof(addr_s));
    addr_s.sin_len = sizeof(addr_s);
    addr_s.sin_family = AF_INET;
    addr_s.sin_port = PP_HTONS(atoi(port));
    addr_s.sin_addr.s_addr = inet_addr(addr);

    /* create client socket */
    n->my_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (n->my_socket < 0) {
        MQTT_DBG("mqtt client create fail\n");
        return -1;
    }
#endif
    /* create client socket */
    if (rc == 0) {
        int opval = 1;
        n->my_socket = socket(family, type, 0);
        if (n->my_socket < 0) {
            MQTT_DBG("mqtt socket create fail");
            return -1;
        }
        /* connect remote servers*/
        rc = connect(n->my_socket, (struct sockaddr *)&address, sizeof(address));

        if (rc < 0) {
            close(n->my_socket);
            MQTT_DBG("mqtt socket connect fail:rc=%d,socket = %d", rc, n->my_socket);
            return -2;
        }
        
        setsockopt(n->my_socket ,IPPROTO_TCP, TCP_NODELAY, &opval, sizeof(opval));
    }

    return rc;
}

#if 0
typedef struct {
    mbedtls_x509_crt cacertl;
    const char *trusted_ca_crt;
    mbedtls_x509_crl *ca_crl;
    const char *peer_cn;
    int (*f_confirm)(int);
} verify_source_t;

typedef struct {
    int authmode;
    int (*f_random)(void *p_rng, unsigned char *output, size_t output_len);
    void (*f_debug)(void *ctx, int level, const char *file, int line, const char *str);
    int (*f_recv)(void *ctx, unsigned char *buf, size_t len);
    int (*f_send)(void *ctx, const unsigned char *buf, size_t len);
} common_settings_t;
ssl_config->common_settings.authmode = MBEDTLS_SSL_VERIFY_OPTIONAL;
ssl_config->common_settings.f_random = ssl_random;
ssl_config->common_settings.f_debug  = my_debug;
ssl_config->common_settings.f_recv   = mbedtls_net_recv;
ssl_config->common_settings.f_send   = mbedtls_net_send;

ssl_config->verify_source.trusted_ca_crt = NULL;
ssl_config->verify_source.peer_cn = NULL;
ssl_config->verify_source.ca_crl = NULL;
ssl_config->verify_source.f_confirm = real_confirm;

typedef struct {
    mbedtls_ssl_config conf;
    //common_settings_t common_settings;
    //verify_source_t verify_source;
} ssl_custom_conf_t;
#endif
u32_t mqtt_avRandom(void)
{
    return (((u32_t)rand() << 16) + rand());
}

static int mqtt_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t   rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)mqtt_avRandom() ;
        rngoffset++;
        rnglen--;
    }
    return 0;
}



static void mqtt_ssl_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    printf("%s\n", str);
}

int mqtt_real_confirm(int verify_result)
{
#define VERIFY_ITEM(Result, Item, ErrMsg) \
    do { \
        if (((Result) & (Item)) != 0) { \
            printf(ErrMsg); \
        } \
    } while (0)

    MQTT_DBG("certificate verification result: 0x%02x", verify_result);
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_EXPIRED, "! fail ! server certificate has expired");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_REVOKED, "! fail ! server certificate has been revoked");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_CN_MISMATCH, "! fail ! CN mismatch");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_NOT_TRUSTED, "! fail ! self-signed or not signed by a trusted CA");
    return 0;
}

static int ssl_parse_crt(mbedtls_x509_crt *crt)
{
    char buf[1024];
    mbedtls_x509_crt *local_crt = crt;
    int i = 0;
    while (local_crt) {
        printf("# %d\r\n", i);
        mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", local_crt);
        {
            char str[512];
            const char *start, *cur;
            start = buf;
            for (cur = buf; *cur != '\0'; cur++) {
                if (*cur == '\n') {
                    size_t len = cur - start + 1;
                    if (len > 511) {
                        len = 511;
                    }
                    memcpy(str, start, len);
                    str[len] = '\0';
                    start = cur + 1;
                    printf("%s", str);
                }
            }
        }
        printf("crt content:%d!\r\n", strlen(buf));
        local_crt = local_crt->next;
        i++;
    }
    return i;
}

int mqtt_ssl_client_init(mbedtls_ssl_context *ssl,
                         mbedtls_net_context *tcp_fd,
                         mbedtls_ssl_config *conf,
                         mbedtls_x509_crt *crt509_ca, const char *ca_crt, size_t ca_len,
                         mbedtls_x509_crt *crt509_cli, const char *cli_crt, size_t cli_len,
                         mbedtls_pk_context *pk_cli, const char *cli_key, size_t key_len,  const char *cli_pwd, size_t pwd_len
                        )
{
    int ret = -1;
    //verify_source_t *verify_source = &custom_config->verify_source;

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init( tcp_fd );
    mbedtls_ssl_init( ssl );
    mbedtls_ssl_config_init( conf );
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    MQTT_DBG( "  . Loading the CA root certificate ..." );
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            MQTT_DBG(" failed ! x509parse_crt returned -0x%04x", -ret);
            return ret;
        }
    }
    ssl_parse_crt(crt509_ca);
    MQTT_DBG( " ok (%d skipped)", ret );


    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init( pk_cli );
#endif
    if ( cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
        MQTT_DBG("start prepare client cert .\n");
        ret = mbedtls_x509_crt_parse( crt509_cli, (const unsigned char *) cli_crt, cli_len );
#else
        {
            ret = 1;
            MQTT_DBG("MBEDTLS_CERTS_C not defined.");
        }
#endif
        if ( ret != 0 ) {
            MQTT_DBG( " failed!  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret );
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        MQTT_DBG("start mbedtls_pk_parse_key[%s]", cli_pwd);
        ret = mbedtls_pk_parse_key( pk_cli,
                                    (const unsigned char *) cli_key, key_len,
                                    (const unsigned char *) cli_pwd, pwd_len);
#else
        {
            ret = 1;
            MQTT_DBG("MBEDTLS_CERTS_C not defined.");
        }
#endif

        if ( ret != 0 ) {
            MQTT_DBG( " failed\n  !  mbedtls_pk_parse_key returned -0x%x\n\n", -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}


int mqtt_ssl_read_all(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    size_t readLen = 0;
    // struct timeval tv;
    int ret = -1;
    //tv.tv_sec = timeout_ms / 1000;
    //tv.tv_usec = (timeout_ms % 1000) * 1000;
    // ret = mt7687_tcp_select(n->my_socket, &tv, 1, 0);
    //MQTT_DBG("%s, mt7687_tcp_select return:%d", __func__, ret);

    MQTT_DBG("mqtt_ssl_read_all len=%d", len);
    //if(ret<=0) return -1;
    MQTT_DBG("mqtt ssl read all timer=%d ms", timeout_ms);
    mbedtls_ssl_conf_read_timeout(&(n->conf), timeout_ms);
    while ((int)readLen < len) {
        //int ret = mt7687_tcp_select(n->my_socket, &tv, 1, 0);
        //if (ret < 0) {
        //MQTT_DBG("%s, mt7687_tcp_select return:%d", __func__, ret);
        //break;
        //} else {
        ret = mbedtls_ssl_read(&(n->ssl), (unsigned char *)(buffer + readLen), (len - readLen));
        MQTT_DBG("%s, mbedtls_ssl_read return:%d", __func__, ret);
        if (ret > 0) {
            readLen += ret;
        } else if (ret == 0) {
            MQTT_DBG("mqtt ssl read timeout");
            return -2; //eof
        } else {
            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {//read already complete(if call mbedtls_ssl_read again, it will return 0(eof))
                MQTT_DBG("MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY");
                return -2;
            }
            return -1; //Connnection error
            //}
        }
    }
    MQTT_DBG("mqtt_ssl_read_all readlen=%d", readLen);
    return readLen;
}

int mqtt_ssl_write_all(Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    size_t writtenLen = 0;
    //struct timeval tv;
    int ret = -1;
    //tv.tv_sec = timeout_ms / 1000;;
    //tv.tv_usec = (timeout_ms % 1000) * 1000;

    //ret = mt7687_tcp_select(n->my_socket, &tv, 0, 1);
    //MQTT_DBG("%s, mt7687_tcp_select return:%d", __func__, ret);

    MQTT_DBG("mqtt_ssl_write_all len=%d", len);

    //if(ret<=0) return -1;

    MQTT_DBG("mqtt ssl read write timer=%d ms", timeout_ms);
    while ((int)writtenLen < len) {
        //int ret = mt7687_tcp_select(n->my_socket, &timeout, 0, 1);
        //if (ret < 0){
        //break;
        // } else
        //{
        ret = mbedtls_ssl_write(&(n->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            MQTT_DBG("mqtt ssl write timeout");
            return writtenLen;
        } else {
            MQTT_DBG("mqtt ssl write fail");
            return -1; //Connnection error
        }
        //}
    }
    MQTT_DBG("mqtt ssl write len=%d", writtenLen);
    return writtenLen;
}

void mqtt_ssl_disconnect(Network *n)
{
    mbedtls_ssl_close_notify(&(n->ssl));
    mbedtls_net_free(&(n->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free( &(n->cacertl));
    if ((n->pkey).pk_info != NULL) {
        MQTT_DBG("mqtt need free client crt&key");
        mbedtls_x509_crt_free( &(n->clicert));
        mbedtls_pk_free( &(n->pkey) );
    }
#endif
    mbedtls_ssl_free( &(n->ssl));
    mbedtls_ssl_config_free(&(n->conf));
    MQTT_DBG( " mqtt_ssl_disconnect\n" );
}


int TLSConnectNetwork(Network *n, const char *addr, const char *port,
                      const char *ca_crt, size_t ca_crt_len,
                      const char *client_crt,	size_t client_crt_len,
                      const char *client_key,	size_t client_key_len,
                      const char *client_pwd, size_t client_pwd_len)
{
    int ret = -1;
    /*
     * 0. Init
     */
    if (0 != (ret = mqtt_ssl_client_init(&(n->ssl), &(n->fd), &(n->conf),
                                         &(n->cacertl), ca_crt, ca_crt_len,
                                         &(n->clicert), client_crt, client_crt_len,
                                         &(n->pkey), client_key, client_key_len, client_pwd, client_pwd_len))) {
        MQTT_DBG( " failed ! ssl_client_init returned -0x%04x", -ret );
        return ret;
    }


    /*
     * 1. Start the connection
     */
    MQTT_DBG("  . Connecting to tcp/%s/%s...", addr, port);
    if (0 != (ret = mbedtls_net_connect(&(n->fd), addr, port, MBEDTLS_NET_PROTO_TCP))) {
        MQTT_DBG(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
    MQTT_DBG( " ok\n" );

    /*
     * 2. Setup stuff
     */
    MQTT_DBG( "  . Setting up the SSL/TLS structure..." );
    if ( ( ret = mbedtls_ssl_config_defaults( &(n->conf),
                 MBEDTLS_SSL_IS_CLIENT,
                 MBEDTLS_SSL_TRANSPORT_STREAM,
                 MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 ) {
        MQTT_DBG( " failed! mbedtls_ssl_config_defaults returned %d", ret );
        return ret;
    }
    MQTT_DBG( " ok" );

    /* OPTIONAL is not optimal for security,
         * but makes interop easier in this simplified example */
    if (ca_crt != NULL) {
        mbedtls_ssl_conf_authmode( &(n->conf), MBEDTLS_SSL_VERIFY_OPTIONAL );
    } else {
        mbedtls_ssl_conf_authmode( &(n->conf), MBEDTLS_SSL_VERIFY_NONE);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain( &(n->conf), &(n->cacertl), NULL);

    if ( ( ret = mbedtls_ssl_conf_own_cert( &(n->conf), &(n->clicert), &(n->pkey) ) ) != 0 ) {
        MQTT_DBG( " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
        return ret;
    }
#endif
    mbedtls_ssl_conf_rng( &(n->conf), mqtt_ssl_random, NULL );
    mbedtls_ssl_conf_dbg( &(n->conf), mqtt_ssl_debug, NULL );


    if ( ( ret = mbedtls_ssl_setup(&(n->ssl), &(n->conf)) ) != 0 ) {
        MQTT_DBG( " failed! mbedtls_ssl_setup returned %d", ret );
        return ret;
    }
    mbedtls_ssl_set_hostname(&(n->ssl), addr);
    mbedtls_ssl_set_bio( &(n->ssl), &(n->fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    /*
      * 4. Handshake
      */
    MQTT_DBG("  . Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(&(n->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            MQTT_DBG( " failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    MQTT_DBG( " ok" );
    /*
     * 5. Verify the server certificate
     */
    MQTT_DBG("  . Verifying peer X.509 certificate..");
    if (0 != (ret = mqtt_real_confirm(mbedtls_ssl_get_verify_result(&(n->ssl))))) {
        MQTT_DBG(" failed  ! verify result not confirmed.");
        return ret;
    }
    n->my_socket = (int)((n->fd).fd);
    MQTT_DBG("my_socket=%d", n->my_socket);
    n->mqttread = mqtt_ssl_read_all;
    n->mqttwrite = mqtt_ssl_write_all;
    n->disconnect = mqtt_ssl_disconnect;

    return 0;
}
