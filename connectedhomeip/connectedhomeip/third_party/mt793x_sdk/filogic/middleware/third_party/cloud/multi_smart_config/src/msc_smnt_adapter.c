#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "type_def.h"
#include "hal_aes.h"
#include "wifi_api.h"
#include "smt_conn.h"
#include "smt_core.h"
#include "semphr.h"
#include "syslog.h"

#include "msc_internal.h"

extern multi_smtcn_info_t saved_multi_smtcn_info;

static sub_proto_sm_t smnt_status = SUB_INIT;

static void _smnt_get_info(void)
{
    int ssid_len = WIFI_MAX_LENGTH_OF_SSID,
        pwd_len = WIFI_LENGTH_PASSPHRASE,
        pmk_len = WIFI_LENGTH_PMK;

    /*SSID*/
    if (elian_get(TYPE_ID_SSID, (char *)saved_multi_smtcn_info.ssid, &ssid_len) != ESUCCESS) {
        LOG_E(multiSmnt, "SSID got failed.\n");
        msc_write_flag(SMTCN_FLAG_FAIL);
        return;
    }
    saved_multi_smtcn_info.ssid_len = ssid_len;

    /*password*/
    if (elian_get(TYPE_ID_PWD, (char *)saved_multi_smtcn_info.pwd, &pwd_len) != ESUCCESS) {
        LOG_W(multiSmnt, "Passphase got failed.\n");
        pwd_len = 0;
    }
    saved_multi_smtcn_info.pwd_len = pwd_len;

    /*PMK */
    if (elian_get(TYPE_ID_PMK, (char *)saved_multi_smtcn_info.pmk, &pmk_len) != ESUCCESS) {
        LOG_W(multiSmnt, "PMK got failed.\n");
    }

#if (msc_debug_log == 1)
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
    uint8_t passwd[WIFI_LENGTH_PASSPHRASE + 1] = {0};

    memcpy(ssid, saved_multi_smtcn_info.ssid, saved_multi_smtcn_info.ssid_len);
    memcpy(passwd, saved_multi_smtcn_info.pwd, saved_multi_smtcn_info.pwd_len);

    LOG_I(multiSmnt, "ssid:%s/%d, passwd:%s/%d\n",
          ssid, saved_multi_smtcn_info.ssid_len,
          passwd, saved_multi_smtcn_info.pwd_len);
#endif

    msc_write_flag(SMTCN_FLAG_FIN);
}

static void _smnt_report_evt(enum eevent_id evt)
{
    switch (evt) {
        case EVT_ID_SYNCSUC:
            smnt_status = SUB_SYNC_SUCC;
            msc_stop_switch_channel();
            LOG_I(multiSmnt, "sync succeed.\n");
            break;

        case EVT_ID_INFOGET:
            LOG_I(multiSmnt, "smnt finished.\n");
            _smnt_get_info();
            sc_rst();
            smnt_status = SUB_FIN;
            break;

        case EVT_ID_TIMEOUT: // must be called by timer task
            smnt_status = SUB_INIT;
            LOG_W(multiSmnt, "lock channel timeout.\n");
            break;

        case EVT_ID_SYNFAIL:
        default :
            break;
    }
}

static void _smnt_start_timer(struct etimer *petimer)
{
    return;
}

static int _smnt_stop_timer(struct etimer *petimer)
{
    return 0;
}

static const struct efunc_table smnt_efunc_tbl = {
    .report_evt     = _smnt_report_evt,
    .start_timer    = _smnt_start_timer,
    .stop_timer     = _smnt_stop_timer,
    .aes128_decrypt = msc_aes_decrypt,
};

static int _smnt_sub_proto_init(const unsigned char *key, const unsigned char key_length)
{
    uint8_t addr[6] = {0};

    wifi_config_get_mac_address(WIFI_PORT_STA, addr);
    elian_init((char *)addr, &smnt_efunc_tbl, key);

    smnt_status = SUB_INIT;

    return 0;
}

static void _smnt_sub_proto_cleanup(void)
{
    return;
}

static int _smnt_sub_proto_rst_channel(void)
{
    if(smnt_status == SUB_SYNC_SUCC) {
        return -1;
    }

    sc_rst();
    smnt_status = SUB_INIT;

    return 0;
}

static sub_proto_sm_t _smnt_sub_proto_rcv(char *data, int len)
{
    elian_input(data, len);

    return smnt_status;
}

static void _smnt_sub_proto_rx_timeout(void)
{
    struct etimer *petimer;
    petimer = get_etimer();

    petimer->func((unsigned long) 0);
}

msc_sub_proto_ops smnt_msc_sub_proto = {
    .sub_proto_init         =   &_smnt_sub_proto_init,
    .sub_proto_cleanup      =   &_smnt_sub_proto_cleanup,
    .sub_proto_rst_channel  =   &_smnt_sub_proto_rst_channel,
    .sub_proto_rcv          =   &_smnt_sub_proto_rcv,
    .sub_proto_rx_timeout   =   &_smnt_sub_proto_rx_timeout,
};
