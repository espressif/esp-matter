#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "smt_conn.h"
#include "airkiss.h"
#include "semphr.h"
#include "syslog.h"

#include "msc_internal.h"

extern multi_smtcn_info_t   saved_multi_smtcn_info;


static airkiss_context_t *msc_ak_context = NULL;
static sub_proto_sm_t ak_status = SUB_INIT;

static airkiss_config_t msc_ak_conf = {
    .memset = memset,
    .memcpy = memcpy,
    .memcmp = memcmp,
    .printf = NULL,
};

static int _ak_sub_proto_init(const unsigned char *key, const unsigned char key_length)
{
    int ret = 0;

    msc_ak_context = (airkiss_context_t *) pvPortMalloc(sizeof(airkiss_context_t));
    if(msc_ak_context == NULL) {
        LOG_E(multiSmnt, "Airkiss pvPortMalloc ak_context failed!\n");
        return -1;
    }

    ret = airkiss_init(msc_ak_context, &msc_ak_conf);
    if(ret < 0) {
        LOG_E(multiSmnt, "Airkiss init failed!\n");
        return -1;
    }

#if AIRKISS_ENABLE_CRYPT
    const char *key = "1234567890123456";
    airkiss_set_key(msc_ak_context, key, strlen(key));
#endif

    ak_status = SUB_INIT;
    return ret;
}

static void _ak_sub_proto_cleanup(void)
{
    if(msc_ak_context != NULL) {
        vPortFree(msc_ak_context);
        msc_ak_context = NULL;
    }
}

static int _ak_sub_proto_rst_channel(void)
{
    airkiss_init(msc_ak_context, &msc_ak_conf);
    airkiss_change_channel(msc_ak_context);
    ak_status = SUB_INIT;

    return 0;
}

static int _ak_get_info(void)
{
    airkiss_result_t result;

    if(airkiss_get_result(msc_ak_context, &result) < 0) {
        LOG_E(multiSmnt, "Airkiss get result failed.\n");
        msc_write_flag(SMTCN_FLAG_FAIL);
        return -1;
    }

    /*SSID*/
    saved_multi_smtcn_info.ssid_len = result.ssid_length;
    if(saved_multi_smtcn_info.ssid_len > WIFI_MAX_LENGTH_OF_SSID) {
        saved_multi_smtcn_info.ssid_len = WIFI_MAX_LENGTH_OF_SSID;
    }
    memcpy(saved_multi_smtcn_info.ssid, result.ssid, saved_multi_smtcn_info.ssid_len);

    /*password*/
    saved_multi_smtcn_info.pwd_len = result.pwd_length;
    if(saved_multi_smtcn_info.pwd_len > WIFI_LENGTH_PASSPHRASE) {
        saved_multi_smtcn_info.pwd_len = WIFI_LENGTH_PASSPHRASE;
    }
    memcpy(saved_multi_smtcn_info.pwd, result.pwd, saved_multi_smtcn_info.pwd_len);

#if (msc_debug_log == 1)
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
    uint8_t passwd[WIFI_LENGTH_PASSPHRASE + 1] = {0};

    memcpy(ssid, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);
    memcpy(passwd, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);

    LOG_I(multiSmnt, "ssid:%s/%d, passwd:%s/%d\n",
          ssid, saved_multi_smtcn_info.ssid_len,
          passwd, saved_multi_smtcn_info.pwd_len );
#endif

    msc_write_flag(SMTCN_FLAG_FIN);
    return 0;
}

static sub_proto_sm_t _ak_sub_proto_rcv(char *data, int len)
{
    int ret;
    ret = airkiss_recv(msc_ak_context, data, len);

    switch(ak_status) {
        case SUB_INIT:
            if(ret == AIRKISS_STATUS_CHANNEL_LOCKED) {
                LOG_I(multiSmnt, "Airkiss channel locked\n");
                ak_status = SUB_SYNC_SUCC;
                msc_stop_switch_channel();
            }
            break;

        case SUB_SYNC_SUCC:
            if(ret == AIRKISS_STATUS_COMPLETE) {
                LOG_I(multiSmnt, "Airkiss finished\n");
                _ak_get_info();
                ak_status = SUB_FIN;
            }
            break;

        default:
            break;
    }

    return ak_status;
}

static void _ak_sub_proto_rx_timeout(void)
{
    ak_status = SUB_INIT;
    LOG_I(multiSmnt, " Airkiss lock channel timeout.\n");
}

msc_sub_proto_ops ak_msc_sub_proto = {
    .sub_proto_init         =   &_ak_sub_proto_init,
    .sub_proto_cleanup      =   &_ak_sub_proto_cleanup,
    .sub_proto_rst_channel  =   &_ak_sub_proto_rst_channel,
    .sub_proto_rcv          =   &_ak_sub_proto_rcv,
    .sub_proto_rx_timeout   =   &_ak_sub_proto_rx_timeout,
};

