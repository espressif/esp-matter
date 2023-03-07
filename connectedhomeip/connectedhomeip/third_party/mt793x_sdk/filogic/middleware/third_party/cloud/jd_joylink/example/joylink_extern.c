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
#include "crc.h"
#include "joylink_porting_layer.h"
#include "netdb.h" 

extern void
joylink_dev_set_ver(short ver);

extern short
joylink_dev_get_ver();

int
joylink_get_wifi_attr(WIFICtrl_t *wi);

char * 
joylink_dev_get_client_list();

typedef struct __attr_{
    // mtk54101 modify : slim, decrease the name length from 128 to 20
    // char name[128];
    char name[20];
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
	Attr_t version;	
}WIFIManage_t;

WIFIManage_t _g_am, *_g_pam = &_g_am;

// mtk54101 modify: slim, RW->ZI. remove to init flow.
/*
WIFICtrl_t _g_wifi = {
//#ifdef __LINUX_UB2__
    .online_state = 1,
    .downspeed = 500,
    .upspeed = 600,
    ._24g.on = 0,
    ._24g.ssid = "tesssid",
    ._24g.encryption = "mixed-psk",
    ._24g.key = "tessskey",
    
    // mtk54101 modify : slim, delete _5g related
    // ._5g.on = 2,
    // ._5g.encryption = "mixed-psk",
    // ._5g.ssid = "5gtesssid",
    // ._5g.key = "5gtessskey",
//#endif
};
 */
WIFICtrl_t _g_wifi;

WIFICtrl_t *_g_pwifi = &_g_wifi;

E_JLRetCode_t
joylink_dev_set_attr_example(WIFICtrl_t *wi, JLPInfo_t *jlp);

int 
joylink_util_cut_ip_port(const char *ipport, char *out_ip, int *out_port);

E_JLRetCode_t
joylink_dev_is_net_ok()
{
    /**
     *FIXME:must to do
     */       
    int ret = E_RET_TRUE;
/*
    int test_socket = -1;
    int test_port = 80;
    char test_server[JL_MAX_SERVER_NAME_LEN] = "www.jd.com";   
    struct sockaddr_in saServer; 
    bzero(&saServer, sizeof(saServer)); 

    saServer.sin_family = AF_INET;

    saServer.sin_port = htons(test_port);

    struct hostent *host;
    if((host=gethostbyname(test_server)) == NULL) {
        log_error("gethostbyname error");
        return ;
    }    
    saServer.sin_addr = *((struct in_addr *)host->h_addr);

    test_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (test_socket < 0 ){
        log_error("socket() failed!\n");
        return E_RET_FAIL;
    }
    
    ret = connect(test_socket, 
            (struct sockaddr *)&saServer,
            sizeof(saServer));

    if(ret < 0){
        log_error("connect() server failed!\n");        
        ret = E_RET_FAIL;
    }else{        
        ret = E_RET_TRUE;
    }
    
    close(test_socket);
 */
    return ret;
}

E_JLRetCode_t
joylink_dev_set_connect_st(int st)
{
    /**
     *FIXME:must to do
     */
    char buff[64] = {0};
    int ret = -1;
    sprintf(buff, "{\"conn_status\":\"%d\"}", st);
    if(_g_pam->server_st.set){
        if(_g_pam->server_st.set(buff)){
            log_error("set error");
        }else{
            ret = 0;
        }
    }

    return ret;
}

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
    char buff[256];

    bzero(buff, sizeof(buff));
    if(strlen(jlp->feedid)){
        sprintf(buff, "{\"feedid\":\"%s\"}", jlp->feedid);
        log_debug("--set buff:%s", buff);
        if(_g_pam->feedid.set){
            if(_g_pam->feedid.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->accesskey)){
        sprintf(buff, "{\"accesskey\":\"%s\"}", jlp->accesskey);
        log_debug("--set buff:%s", buff);
        if(_g_pam->accesskey.set){
            if(_g_pam->accesskey.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->localkey)){
        sprintf(buff, "{\"localkey\":\"%s\"}", jlp->localkey);
        log_debug("--set buff:%s", buff);
        if(_g_pam->localkey.set){
            if(_g_pam->localkey.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->joylink_server)){
        sprintf(buff, "%s:%d", jlp->joylink_server, jlp->server_port);
        log_debug("--set buff:%s", buff);
        if(_g_pam->server_info.set){
            if(_g_pam->server_info.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    sprintf(buff, "{\"version\":%d}", jlp->version);
    log_debug("--set buff:%s", buff);
    if(_g_pam->version.set){
        if(_g_pam->version.set(buff)){
            log_error("set error");
            goto RET;
        }
    }

    ret = E_RET_OK;
RET:

    return ret;
}

E_JLRetCode_t
joylink_dev_get_jlp_info(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = -1;
    char buff[256];

    if(_g_pam->accesskey.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->accesskey.get(buff, sizeof(buff))){
            log_debug("-->accesskey:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get accesskey error");
        }
    }

    if(_g_pam->localkey.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->localkey.get(buff, sizeof(buff))){
            log_debug("-->localkey:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get localkey error");
        }
    }

    if(_g_pam->feedid.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->feedid.get(buff, sizeof(buff))){
            log_debug("-->feedid:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get feedid error");
        }
    }

    if(_g_pam->uuid.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->uuid.get(buff, sizeof(buff))){
            log_debug("-->uuid:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get uuid error");
        }
    }

    if(_g_pam->macaddr.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->macaddr.get(buff, sizeof(buff))){
            log_info("-->mac:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get mac error");
        }
    }

    if(_g_pam->server_info.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->server_info.get(buff, sizeof(buff))){
            log_info("-->server_info:%s\n", buff);
            joylink_util_cut_ip_port(buff, jlp->joylink_server, &jlp->server_port);
        }else{
            log_error("get mac error");
        }
    }

    if(_g_pam->version.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->version.get(buff, sizeof(buff))){
            log_info("-->version:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get mac error");
        }
    }

    return ret;
}

E_JLRetCode_t
joylink_dev_set_attr_example(WIFICtrl_t *wi, JLPInfo_t *jlp)
{
    if(NULL == wi && NULL == jlp){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = 0;
    // mtk54101: memory overflow
    // char buff[256];    
    // char *w24gt ="{\"enabled\": \"%d\",\"ssid\":\"%s\",\"type\": \"%d\",\"passphrase\":\"%s\"}";
    char buff[1024];    
    char *w24gt ="{\"enabled\": \"%d\",\"ssid\":\"%-32s\",\"type\": \"%d\",\"passphrase\":\"%-32s\"}";
    int type;

    if(NULL != wi){
        int on_off = wi->_24g.on? 0:1;
        if(strlen(wi->_24g.ssid)){
            if(!strcmp("none", wi->_24g.encryption)){
                type = 0;
            }else{
                type = 1;
            }
            sprintf(buff, w24gt, on_off, wi->_24g.ssid, type, wi->_24g.key);
            log_info("24g.on:%d", wi->_24g.on);
            log_info("set buff:%s", buff);
            if(_g_pam->wlan24g.set){
                if(_g_pam->wlan24g.set(buff)){
                    log_error("set error");
                    ret = -1;
                }
            }
        }
    }

    if(NULL != jlp){
        bzero(buff, sizeof(buff));
        if(strlen(jlp->feedid)){
            sprintf(buff, "{\"feedid\":\"%s\"}", jlp->feedid);
            log_debug("--set buff:%s", buff);
            if(_g_pam->feedid.set){
                if(_g_pam->feedid.set(buff)){
                    log_error("set error");
                    ret = -1;
                }
            }
        }

        bzero(buff, sizeof(buff));
        if(strlen(jlp->accesskey)){
            sprintf(buff, "{\"accesskey\":\"%s\"}", jlp->accesskey);
            log_debug("--set buff:%s", buff);
            if(_g_pam->accesskey.set){
                if(_g_pam->accesskey.set(buff)){
                    log_error("set error");
                    ret = -1;
                }
            }
        }

        bzero(buff, sizeof(buff));
        if(strlen(jlp->localkey)){
            sprintf(buff, "{\"localkey\":\"%s\"}", jlp->localkey);
            log_debug("--set buff:%s", buff);
            if(_g_pam->localkey.set){
                if(_g_pam->localkey.set(buff)){
                    log_error("set error");
                    ret = -1;
                }
            }
        }
    }

    return ret;
}

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
    joylink_get_wifi_attr(_g_pwifi);
    char *p = joylink_dev_get_client_list();
    char *packet_data =  packageGWInfo(
            "you are ok!",
            0,
            _g_pwifi,
            p);

    /*if(p != NULL && packet_data != NULL){*/
        len = strlen(packet_data);
        printf("------>%s:len:%d\n", packet_data, len);
        if(len < out_max){
            memcpy(out_snap, packet_data, len); 
        }else{
            len = 0;
        }
    /*}*/

    if(NULL !=  packet_data){
        joylink_free(packet_data);
    }
    if(NULL != p){
        joylink_free(p);
    }

    return len;
}

int
joylink_dev_get_json_snap_shot(char *out_snap, int32_t out_max, int code, char *feedid)
{
    /**
     *FIXME:must to do
     */
    sprintf(out_snap, "{\"code\":%d, \"feedid\":\"%s\"}", code, feedid);

    return strlen(out_snap);
}

E_JLRetCode_t 
joylink_dev_lan_json_ctrl(const char *json_cmd)
{
    /**
     *FIXME:must to do
     */
    log_debug("json ctrl:%s", json_cmd);

    return E_RET_OK;
}

E_JLRetCode_t 
joylink_dev_script_ctrl(const char *recPainText, JLContrl_t *ctr, int from_server)
{
    if(NULL == recPainText || NULL == ctr){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = -1;
    ctr->biz_code = (int)(*((int *)(recPainText + 4)));
    ctr->serial = (int)(*((int *)(recPainText +8)));

    WIFICtrl_t wctr;
    bzero(&wctr, sizeof(wctr));
    wctr._24g.on = -1;   
    time_t tt = time(NULL);
    log_info("bcode:%d:server:%d:time:%ld", ctr->biz_code, from_server,(long)tt);

    if(ctr->biz_code == JL_BZCODE_GET_SNAPSHOT){
        /*
         *Nothing to do!
         */
        ret = 0;
    }else if(ctr->biz_code == JL_BZCODE_CTRL){
        joylink_dev_parse_wifi_ctrl(&wctr, recPainText + 12);
        if(strlen(wctr._24g.encryption)){
            if(!strcmp(wctr._24g.encryption, "none")){
                strcpy(_g_pwifi->_24g.key, "");
            }else if(!strcmp(wctr._24g.encryption, "mixed-psk")){
                strcpy(_g_pwifi->_24g.key, wctr._24g.key);
            }

            strcpy(_g_pwifi->_24g.encryption, wctr._24g.encryption);
        }
        if(strlen(wctr._24g.ssid)){
            if(strcmp(_g_pwifi->_24g.ssid, wctr._24g.ssid)){
                strcpy(_g_pwifi->_24g.ssid, wctr._24g.ssid); 
            }
        }
        if(wctr._24g.on != -1 ){
            _g_pwifi->_24g.on = wctr._24g.on;
        }
       
        if(!joylink_dev_set_attr_example(_g_pwifi, NULL)){
            log_info("set atrr ok");
        }else{
            log_error("set attr error");
        }
        ret = 0;
    }else{
        log_error("unKown biz_code:%d", ctr->biz_code);
    }
    
    return ret;
}

E_JLRetCode_t
joylink_dev_ota(JLOtaOrder_t *otaOrder)
{
    if(NULL == otaOrder){
        return -1;
    }
    int ret = E_RET_OK;

    log_debug("serial:%d | feedid:%s | productuuid:%s | version:%d | versionname:%s | crc32:%d | url:%s\n",
     otaOrder->serial, otaOrder->feedid, otaOrder->productuuid, otaOrder->version, 
     otaOrder->versionname, otaOrder->crc32, otaOrder->url);

    strcpy(_g_fota_ctx.download_url, (const char*)otaOrder->url);
    _g_fota_ctx.crc32 = otaOrder->crc32;

    joylink_fota_download_package();
    
    return ret;

}

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
    while (_g_fota_ctx.upgrade_status == FOTA_STATUS_DOWNLOAD) {
        otaUpload.status = _g_fota_ctx.upgrade_status;
        otaUpload.progress = _g_fota_ctx.progress;
        strcpy(otaUpload.status_desc, joylink_fota_get_status_desc(_g_fota_ctx.error_code));
        joylink_server_ota_status_upload_req(&otaUpload);
        vTaskDelay(2000);
    }

    otaUpload.status = _g_fota_ctx.upgrade_status;
    otaUpload.progress = _g_fota_ctx.progress;
    strcpy(otaUpload.status_desc, joylink_fota_get_status_desc(FOTA_ERROR_CODE_CRC32_ERROR));
    joylink_server_ota_status_upload_req(&otaUpload);

}

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
    }else if(!strcmp(name, JL_ATTR_VERSION)){
       _g_pam->version.get = attr_get_cb; 
       _g_pam->version.set = attr_set_cb; 
       ret = 0;
    } else if(!strcmp(name, JL_ATTR_SERVER_INFO)){
       _g_pam->server_info.get = attr_get_cb; 
       _g_pam->server_info.set = attr_set_cb; 
       ret = 0;
    }

    log_info("regster %s:ret:%d", name, ret);
    return ret;
}



//=========== example ==================
int
joylink_get_wifi_attr(WIFICtrl_t *wi)
{
    if(NULL == wi){
        return -1;
    }
    int ret = -1;
    char buff[256];

    if(_g_pam->wlan24g.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->wlan24g.get(buff, sizeof(buff))){
            log_debug("-->wlan24g:%s\n", buff);
            jl_parse_24g(wi, buff);
        }else{
            log_error("get wlan24g error");
        }
    }else{
        log_error("nofoud wlan24g cb");
    }

    if(_g_pam->wlanspeed.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->wlanspeed.get(buff, sizeof(buff))){
            log_debug("get wlanspeed:%s", buff);
            jl_parse_wan_speed(wi, buff);
        }else{
            log_error("get wlanspeed error");
        }
    }else{
        log_error("nofoud wlan24g cb");
    }

    return ret;
}

char *
joylink_dev_get_client_list()
{
    char buff[1024];
    int count = 0;
    char *p = NULL;

    SubDev_t sdev[10];
    bzero(sdev, sizeof(sdev));

    if(_g_pam->subdev.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->subdev.get(buff, sizeof(buff))){
            log_debug("-->devs:%s\n", buff);
            count = joylink_parse_client_list(sdev, 10, buff);
            
            p = joylink_package_client_list_(sdev, count);
        }else{
            log_error("get subdev error");
        }
    }

    return p;
}

int
joylink_test_crc32()
{
    FILE* pFile;
    long lSize;
    char* buffer;
    size_t result;

    pFile = fopen("/home/yn/workspace/1448518667891_H5.zip" , "r");
    if (pFile == NULL){
        log_error("File error\n");
        return -1;;
    }

    fseek(pFile , 0 , SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    buffer = (char*) joylink_malloc(sizeof(char) * lSize);
    if (buffer == NULL){
        log_error("Memory error\n");
        return -1;
    }

    result = fread(buffer, 1, lSize, pFile);
    if (result != lSize){
        log_error("Reading error\n");
        return -1;
    }

    make_crc32_table();
    uint32_t crc = {0};
    crc = make_crc(crc, (unsigned char*)buffer, (uint32_t)lSize);

    log_debug("lSize:%ld\ncrc:%d\nsizeof_crc:%d\n", lSize, crc, sizeof(crc));
    
    fclose(pFile);
    joylink_free(buffer);
    return 0;
}
