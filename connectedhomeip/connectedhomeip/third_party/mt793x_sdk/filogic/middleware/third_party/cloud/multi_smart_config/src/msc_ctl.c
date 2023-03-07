#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
#include "timers.h"
#include "syslog.h"

#include "msc_internal.h"
#include "msc_api.h"
#include "wifi_scan.h"
#include "wifi_api.h"
/******************************************************************************
 * macro , typedef
 ******************************************************************************/

#define locked_channel_timems   (5 * 1000)     /* 5s */

#define MAX_SMNT_PROTO_NUM 10


/******************************************************************************
 * static variable definition
 ******************************************************************************/
static msc_proto_adapter_t *msc_ctl_list = NULL;

static msc_sm_t msc_sm = MSC_PROB_PROTO;

static msc_proto_adapter_t *proto_locked = NULL;

static TimerHandle_t msc_rst_timer = NULL;


/******************************************************************************
 * function prototype
 ******************************************************************************/
static int _msc_ctl_init(const unsigned char *key, const unsigned char key_length);
static void _msc_ctl_cleanup(void);
static int _msc_ctl_rst(void);
static int _msc_ctl_rx_handler(char *data, int len);
static int _msc_ctl_unregister_all_proto(void);


/******************************************************************************
 * extern variable definition , declaration
 ******************************************************************************/
extern msc_sub_proto_ops smnt_msc_sub_proto;
extern msc_sub_proto_ops ak_msc_sub_proto;
extern msc_sub_proto_ops bsmtcn_sub_proto;

const multi_smtcn_proto_ops multi_smart_config_ops = {
    .init               =   &_msc_ctl_init,
    .cleanup            =   &_msc_ctl_cleanup,
    .switch_channel_rst =   &_msc_ctl_rst,
    .rx_handler         =   &_msc_ctl_rx_handler,
};


/******************************************************************************
 * static function definition
 ******************************************************************************/
static msc_proto_adapter_t *_msc_ctl_request_proto_adapter(msc_sub_proto_ops *proto)
{
    msc_proto_adapter_t *prev_adapter, *new_adapter = NULL;

    if((new_adapter = pvPortMalloc(sizeof(msc_proto_adapter_t))) == NULL) {
        LOG_E(multiSmnt, "alloc new proto adapter failed");
        return NULL;
    }
    new_adapter->proto_ops = proto;
    new_adapter->next = NULL;

    taskENTER_CRITICAL();
    prev_adapter = msc_ctl_list;
    while(prev_adapter && prev_adapter->next) {
        if(prev_adapter->proto_ops == proto) {
            taskEXIT_CRITICAL();
            LOG_E(multiSmnt, "This protocol been exists, 0x%x", proto);
            vPortFree(new_adapter);
            return NULL;
        }
        prev_adapter = prev_adapter->next;
    }

    if(prev_adapter == NULL) {
        msc_ctl_list = new_adapter;
    } else {
        prev_adapter->next = new_adapter;
    }
    taskEXIT_CRITICAL();

    return new_adapter;
}

static void _msc_ctl_lock_timeout(TimerHandle_t tmr)
{
    LOG_W(multiSmnt, "lock channel timeout.\n");

    proto_locked->proto_ops->sub_proto_rx_timeout();
    msc_sm = MSC_PROB_PROTO;
    proto_locked = NULL;
    msc_continue_switch_channel();
}

static void _msc_ctl_cleanup(void)
{
    msc_proto_adapter_t *iter;

    if(msc_rst_timer != NULL) {
        xTimerDelete(msc_rst_timer, 0);
        msc_rst_timer = NULL;
    }

    //no protection. There is not any concurrency until now.
    for(iter = msc_ctl_list; iter; iter = iter->next) {
        iter->proto_ops->sub_proto_cleanup();
    }

    _msc_ctl_unregister_all_proto();
}

static int _msc_ctl_init(const unsigned char *key, const unsigned char key_length)
{
    msc_proto_adapter_t *iter;

    msc_rst_timer = xTimerCreate("msc_rst_timer",
                                 (locked_channel_timems / portTICK_PERIOD_MS), /*the period being used.*/
                                 pdFALSE,
                                 NULL,
                                 _msc_ctl_lock_timeout);
    if(msc_rst_timer == NULL) {
        LOG_E(multiSmnt, "msc_rst_timer create failed.");
        return -1;
    }

    //no protection. There is not any concurrency until now.
    for(iter = msc_ctl_list; iter; iter = iter->next)
        if(iter->proto_ops->sub_proto_init(NULL, 0) < 0) {
            goto fail;
        }

    msc_sm = MSC_PROB_PROTO;
    return 0;

fail:
    _msc_ctl_cleanup();
    return -1;
}

static int _msc_ctl_rst(void)
{
    msc_proto_adapter_t *iter;

    //no protection. There is not any concurrency until now.
    for(iter = msc_ctl_list; iter; iter = iter->next) {
        iter->proto_ops->sub_proto_rst_channel();
    }

    return 0;
}

static char g_ap_mac[6] = {0};

static bool get_ap_mac_by_sync_packet(char * data,int len)
{
    if((data[0] & 0x0C) == 0x08)
    {
        char * src_addr;
        char * des_addr;
        char * ap_mac;
        int  to_ds = 0;
        
        if ((data[1] & 3) == 2)
        {
            /* from DS. */
            src_addr = data + 16;  
            des_addr =  data + 4;
            ap_mac = data + 10;
        } 
        else if ((data[1] & 3) == 1)
        {
            /* To DS. */
            src_addr = data + 10;
            des_addr = data + 16;
            ap_mac = data + 4;
            to_ds = 1;
        }

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_PRINT(mac) mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]
#define IS_MULTICAST_ADDR(Addr) (((((Addr[0]) & 0x01) == 0x01) && ((Addr[0]) != 0xff)) || (Addr[0] == 0x33 && Addr[1] == 0x33))


        LOG_I(multiSmnt," src addr " MAC_FMT" ap mac "MAC_FMT"\n",MAC_PRINT(src_addr),MAC_PRINT(ap_mac));
        
        memcpy(g_ap_mac,ap_mac,6);
        return TRUE;
    }
    return FALSE;

}

static int get_ch_info(char * pbuf,int len,uint8_t * channel)
{
    size_t left = len;
    PFRAME_802_11 pFrame;
    uint8_t *Ptr = NULL;
    PEID_STRUCT pEid;
    int Length = 0;
    unsigned char *ptr_eid = NULL;

    pFrame = (PFRAME_802_11)pbuf;

    Ptr = pFrame->Octet;
    Length += 24;
    
    Ptr += 8;
    Length += 8;

    /* get beacon interval from payload and advance the pointer*/
    Ptr += 2;
    Length += 2;

    /* get capability info from payload and advance the pointer*/
    Ptr += 2;
    Length += 2;

    pEid = (PEID_STRUCT) Ptr;
    while ((Length + 2 + pEid->Len) <= len)  
    {
        ptr_eid = (unsigned char*)pEid;
        switch(pEid->Eid)
        {
            case IE_DS_PARM:
               if(pEid->Len == 1)
               {
                   * channel = *pEid->Octet;
                   return 0;
               }
           default:
                break;
                
        }
		Length = Length + 2 + pEid->Len;  /* Eid[1] + Len[1]+ content[Len]*/
		pEid = (PEID_STRUCT)((unsigned char*)pEid + 2 + pEid->Len);    
    }

    return -2;
}


static int get_ap_ch(char * data,int len,uint8_t *channel)
{
    if((data[0] & 0x0C) == 0)
    {
        if((data[0] & 0xF0) == 0x80)
        {
            char * ap_mac = data + 10;

            if(memcmp(g_ap_mac,ap_mac, 6 ) == 0)
            {
                return get_ch_info(data, len,  channel);
                
            }
            return -1;
        }
        else
        {
            return -1;
        }
    }
    else
        return -1;
}

#undef BIT
#define BIT(n) (0x0001 << (n))

static int _msc_ctl_rx_handler(char *data, int len)
{
    int ret = 0;

    switch(msc_sm) {
            msc_proto_adapter_t *iter;

        case MSC_PROB_PROTO:
            //no protection. There is not any concurrency until now.
            for(iter = msc_ctl_list; iter; iter = iter->next) {
                ret = iter->proto_ops->sub_proto_rcv(data, len);
                if(ret == SUB_SYNC_SUCC) {
                    if(get_ap_mac_by_sync_packet(data,len)) {
                        uint32_t rx_filter;
                        msc_sm = MSC_GET_AP_CH;
                        wifi_config_get_rx_filter(&rx_filter);
                        rx_filter &= ~BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);
                        wifi_config_set_rx_filter(rx_filter);
                    }
                    else {
                        LOG_I(multiSmnt, "sync packet not data frame\n");
                        msc_sm = MSC_RCV_INFO;
                        xTimerStart(msc_rst_timer, 0);
                    }
                    proto_locked = iter;
                    break;
                }
            }
            break;
        case MSC_GET_AP_CH:
            {
                uint32_t rx_filter;
                int ret = 0;
                uint8_t channel = 0;
                
                ret = get_ap_ch(data,len,&channel);

                if(ret == -1)
                    break;
                if(ret == 0)
                {
                    uint8_t org_channel;
                    wifi_config_get_channel(0,&org_channel);
                    if(org_channel != channel)
                    {
                        LOG_I(multiSmnt,"org channel  %d , ap channel is %d\n",org_channel,channel);
                        wifi_config_set_channel(0,channel);
                    }
                }
                wifi_config_get_rx_filter(&rx_filter);
                rx_filter |= BIT(WIFI_RX_FILTER_DROP_BSSID_BCN);
                wifi_config_set_rx_filter(rx_filter);
                xTimerStart(msc_rst_timer, 0);
                msc_sm = MSC_RCV_INFO;
            }
            break;
            
        case MSC_RCV_INFO:
            if(proto_locked && (proto_locked->proto_ops->sub_proto_rcv(data, len) == SUB_FIN)) {
                xTimerStop(msc_rst_timer, 0);
                msc_sm = MSC_SUCC;
                proto_locked = NULL;

            }
            break;

        case MSC_TIMEOUT:
            break;

        case MSC_SUCC:
        default:
            break;
    }

    return 0;
}

static int _msc_ctl_check_smnt_type(smnt_type_e ctl_flg)
{
    smnt_type_e e_supported_type = 0;

    e_supported_type = 0;

#if defined(MTK_SMTCN_V4_ENABLE) ||  defined(MTK_SMTCN_V5_ENABLE)
    e_supported_type |= SUPPORT_MTK_SMNT;
#endif

#ifdef MTK_AIRKISS_ENABLE
    e_supported_type |= SUPPORT_AIRKISS;
#endif

    LOG_I(multiSmnt, "supported type is %d\n", e_supported_type);
    LOG_I(multiSmnt, "ctl type is %d\n", ctl_flg);

    if(ctl_flg == 0) {
        LOG_E(multiSmnt, "should choose a smnt to config\n");
        return MSC_SMNT_TYPE_INCORRECT;
    }

    if( (ctl_flg & SUPPORT_MTK_SMNT) &&
            !(e_supported_type & SUPPORT_MTK_SMNT) ) {
        LOG_E(multiSmnt, "not support mtk smnt\n");
        return MSC_SMNT_TYPE_INCORRECT;
    }

    if( (ctl_flg & SUPPORT_AIRKISS) &&
            !(e_supported_type & SUPPORT_AIRKISS) ) {
        LOG_E(multiSmnt, "not support airkiss\n");
        return MSC_SMNT_TYPE_INCORRECT;
    }

    return MSC_OK;
}

static int _msc_ctl_unregister_all_proto(void)
{
    msc_proto_adapter_t *adapter = NULL;

    taskENTER_CRITICAL();
    while((adapter = msc_ctl_list) != NULL) {
        msc_ctl_list = msc_ctl_list->next;
        vPortFree(adapter);
    }
    taskEXIT_CRITICAL();

    return 0;
}


/******************************************************************************
 * extern function definition
 ******************************************************************************/
int msc_ctl_register_multi_proto(smnt_type_e  ctl_flg)
{
    int ret = MSC_OK;
    ret = _msc_ctl_check_smnt_type(ctl_flg);

    if(ret != MSC_OK) {
        return ret;
    }

#ifdef MTK_SMTCN_V4_ENABLE
    if(ctl_flg & SUPPORT_MTK_SMNT) {
        _msc_ctl_request_proto_adapter(&smnt_msc_sub_proto);
    }
#endif

#ifdef MTK_SMTCN_V5_ENABLE
    if(ctl_flg & SUPPORT_MTK_SMNT) {
        _msc_ctl_request_proto_adapter(&bsmtcn_sub_proto);
    }
#endif

#ifdef MTK_AIRKISS_ENABLE
    if(ctl_flg & SUPPORT_AIRKISS) {
        _msc_ctl_request_proto_adapter(&ak_msc_sub_proto);
    }
#endif

    return 0;
}

