/* --------------------------------------------------
 * @brief: 
 *
 * @version: 1.0
 *
 * @date: 10/08/2015 09:28:27 AM
 *
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joylink.h"
#include "joylink_packets.h"
#include "joylink_extern.h"
#include "joylink_json.h"
#include "joylink_extern_json.h"
#include "joylink_ret_code.h"

typedef struct __attr_{
    char name[128];
    E_JL_DEV_ATTR_GET_CB get;
    E_JL_DEV_ATTR_SET_CB set;
}Attr_t;

typedef struct _attr_manage_{
    Attr_t wlan24g;
    Attr_t subdev;
    Attr_t wlanspeed;
    Attr_t uuid;
    Attr_t feedid;
    Attr_t accesskey;
    Attr_t localkey;
    Attr_t server_st;
	Attr_t macaddr;	
	Attr_t server_info;	
/*
 *ssid and pass_word is option for GW.
 */
	Attr_t ssid;	
	Attr_t pass_word;	

}WIFIManage_t;

WIFIManage_t _g_am, *_g_pam = &_g_am;

extern void
joylink_dev_set_ver(short ver);

extern short
joylink_dev_get_ver();

/**
 * brief:
 * Check dev net is ok.
 * @Param: st
 *
 * @Returns: 
 *  E_RET_TRUE
 *  E_RET_FAIL
 */
E_JLRetCode_t
joylink_dev_is_net_ok()
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_TRUE;
    return ret;
}

/**
 * brief:
 * When connecting server st changed,
 * this fun will be called.
 *
 * @Param: st
 * JL_SERVER_ST_INIT      (0)
 * JL_SERVER_ST_AUTH      (1)
 * JL_SERVER_ST_WORK      (2)
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_connect_st(int st)
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:
 * Save joylink protocol info in flash.
 *
 * @Param:jlp 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_attr_jlp(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return E_RET_ERROR;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;

    return ret;
}

/**
 * brief:
 * get joylink protocol info.
 *
 * @Param:jlp 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_get_jlp_info(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return E_RET_ERROR;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;

    return ret;
}

/**
 * brief:
 * Save dev attr info to flash.
 *
 * @Param:wi 
 * "wi" is only a example, replace with dev attr.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_attr(WIFICtrl_t *wi)
{
    if(NULL == wi){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;

    return ret;
}

/**
 * brief:
 * Get dev snap shot.
 *
 * @Param:out_snap
 *
 * @Returns: snap shot len.
 */
int
joylink_dev_get_snap_shot(char *out_snap, int32_t out_max)
{
    if(NULL == out_snap || out_max < 0){
        return 0;
    }
    /**
     *FIXME:must to do
     */
    int len = 0;

    return len;
}

/**
 * brief:
 * Get dev snap shot.
 *
 * @Param:out_snap
 *
 * @Returns: snap shot len.
 */
int
joylink_dev_get_json_snap_shot(char *out_snap, int32_t out_max, int code, char *feedid)
{
    /**
     *FIXME:must to do
     */
    sprintf(out_snap, "{\"code\":%d, \"feedid\":\"%s\"}", code, feedid);

    return strlen(out_snap);
}

/**
 * brief:
 * json ctrl.
 *
 * @Param:json_cmd
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t 
joylink_dev_lan_json_ctrl(const char *json_cmd)
{
    /**
     *FIXME:must to do
     */
    log_debug("json ctrl:%s", json_cmd);

    return E_RET_OK;
}

/**
 * brief:
 * script control.
 * @Param: 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t 
joylink_dev_script_ctrl(const char *cmd, JLContrl_t *ctr, int from_server)
{
    if(NULL == cmd|| NULL == ctr){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;
    int offset = 0;
    int time_tmp;
    memcpy(&time_tmp, cmd + offset, 4);
    offset +=4;
    memcpy(&ctr->biz_code, cmd + offset, 4);
    offset +=4;
    memcpy(&ctr->serial, cmd + offset, 4);
    offset +=4;

    if(ctr->biz_code == JL_BZCODE_GET_SNAPSHOT){
        /*
         *Nothing to do!
         */
        ret = 0;
    }else if(ctr->biz_code == JL_BZCODE_CTRL){
        /**
         *Must to do!
         */
        log_debug("script ctrl:%s", cmd+offset);
    }
    
    return ret;
}

/**
 * brief:
 * dev ota update
 * @Param: JLOtaOrder_t *otaOrder
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_ota(JLOtaOrder_t *otaOrder)
{
    if(NULL == otaOrder){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;
    log_debug("serial:%d | feedid:%s | productuuid:%s | version:%d | versionname:%s | crc32:%d | url:%s\n",
     otaOrder->serial, otaOrder->feedid, otaOrder->productuuid, otaOrder->version, 
     otaOrder->versionname, otaOrder->crc32, otaOrder->url);

    return ret;
}

/**
 * brief:
 * dev ota status upload
 * @Param: 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
void
joylink_dev_ota_status_upload()
{
    JLOtaUpload_t otaUpload;
    strcpy(otaUpload.feedid, _g_pdev->jlp.feedid);
    strcpy(otaUpload.productuuid, _g_pdev->jlp.uuid);

    /**
     *FIXME:must to do
     *status,status_desc, progress
     */
    joylink_server_ota_status_upload_req(&otaUpload);
}

/**
 * brief:1 start a softap with ap_ssid 
 *       2 option to do it.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_start_softap(char *ap_ssid)
{
    if(NULL == ap_ssid){
        return E_RET_ERROR;
    }
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:1 stop softap
 *       2 option to do it.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_stop_softap()
{
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:
 * attr set and get callback register.
 * @Param:
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
int 
joylink_dev_register_attr_cb(
        const char *name,
        E_JL_DEV_ATTR_TYPE type,
        E_JL_DEV_ATTR_GET_CB attr_get_cb,
        E_JL_DEV_ATTR_SET_CB attr_set_cb)
{
    if(NULL == name){
        return -1;
    }
    int ret = -1;
    log_debug("regster %s", name);
    if(!strcmp(name, JL_ATTR_WLAN24G)){
       _g_pam->wlan24g.get = attr_get_cb; 
       _g_pam->wlan24g.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_SUBDEVS)){
       _g_pam->subdev.get = attr_get_cb; 
       _g_pam->subdev.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_WAN_SPEED)){
       _g_pam->wlanspeed.get = attr_get_cb; 
       _g_pam->wlanspeed.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_UUID)){
       _g_pam->uuid.get = attr_get_cb; 
       _g_pam->uuid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_FEEDID)){
       _g_pam->feedid.get = attr_get_cb; 
       _g_pam->feedid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_ACCESSKEY)){
       _g_pam->accesskey.get = attr_get_cb; 
       _g_pam->accesskey.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_LOCALKEY)){
       _g_pam->localkey.get = attr_get_cb; 
       _g_pam->localkey.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_CONN_STATUS)){
       _g_pam->server_st.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_MACADDR)){
       _g_pam->macaddr.get = attr_get_cb; 
       ret = 0;
    }

    log_info("regster %s:ret:%d", name, ret);
    return ret;
}
