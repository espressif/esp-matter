#ifndef _MSC_API_H_
#define _MSC_API_H_

#include "wifi_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SUPPORT_MTK_SMNT = 1,
	SUPPORT_AIRKISS  = 2,
	SUPPORT_JOYLINK  = 4,
	SUPPORT_AWS      = 8
}smnt_type_e;


#define    MSC_FAIL              -1
#define    MSC_OK                 0
#define    MSC_SMNT_TYPE_INCORRECT -2 
#define    MSC_INVALID_ARG       -4



#define SIZE_CID 			4
#define SIZE_PUID			6
#define SIZE_MAC			6
#define SIZE_SMNT_KEY		16

/*used by joylink*/
typedef struct
{
	uint8_t cid[SIZE_CID];
	uint8_t puid[SIZE_PUID];
	uint8_t smnt_key[SIZE_SMNT_KEY];
}joylink_info_t;

/*used by airkiss and mtk_smnt*/
typedef struct
{
    uint8_t * key;
    uint8_t   key_len;
}am_info_t;

typedef struct
{
    joylink_info_t joylink_device_info;
    am_info_t      am_device_info;
}msc_device_info_t;

typedef enum
{
    CLOUD_SMART_PLATFORM_JOYLINK = 0x01,
    CLOUD_SMART_PLATFORM_ALINK = 0x02,
    CLOUD_SMART_PLATFORM_AIRKISS = 0x04,
    CLOUD_SMART_PLATFORM_ALL = 0xFF    
}cloud_platform_e;

typedef enum {
    WIFI_SMART_CONNECTION_EVENT_CHANNEL_LOCKED,  /**< Locked channel event. */
    WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED,  /**< The Smart Connection has finished. */
} smart_connection_event_t ;

typedef struct 
{
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
    uint8_t ssid_len;
    uint8_t pwd[WIFI_LENGTH_PASSPHRASE];
    uint8_t pwd_len;
    uint8_t *tlv_data;
    uint8_t tlv_data_len;
}smt_connt_result_t;

/**
 * @brief    this callback function is registered by #wifi_multi_smart_connection_start, when #wifi_multi_smart_connection_event_t event happens
 *           this function will be called
 *
 * @param    [IN] event  please refer to #smart_connection_event_t
 * @param    [IN] data
 *                  when event == WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED, the type of data is smt_connt_result_t* and the data contains the ssid ,pwd infomation
 *                  when event == WIFI_SMART_CONNECTION_EVENT_CHANNEL_LOCKED, data == NULL
 *			 
 */
typedef void (* smart_connection_callback_t) (smart_connection_event_t event, void *data);


/**
 * @function msc_config_set_cloud_platform
 *
 * @brief    set the cloud platform you will use to the flash
 *
 * @param    [IN] mode  please refer to #cloud_platform_e
 *
 * @return  success 0, fail -1    
 *			 
 */
extern int msc_config_set_cloud_platform(cloud_platform_e mode);


/**
 * @function msc_config_get_cloud_platform
 *
 * @brief    get the cloud platform you will use from the flash
 *
 * @param    [OUT] mode    
 * @return  success 0, fail -1    
 *			 
 */
extern int msc_config_get_cloud_platform(cloud_platform_e *mode);


/**
 * @function msc_config_set_smnt_type
 *
 * @brief    set the smart connection type you will use to the flash
 *
 * @param    [IN] mode     please refer to #smnt_type_e
 *                         currently you'better set mode as below values:
 *                                          SUPPORT_MTK_SMNT                       only start mtk smart connection
 *                                          SUPPORT_AIRKISS                        only start airkiss
 *                                          SUPPORT_MTK_SMNT | SUPPORT_AIRKISS     start both mtk smart connection and airkiss
 *                                          SUPPORT_JOYLINK                        only start joylink
 *                                          SUPPORT_AWS                            only start aws
 * @return  success 0, fail -1    
 *			 
 */
extern int msc_config_set_smnt_type(smnt_type_e mode);


/**
 * @function msc_config_get_smnt_type
 *
 * @brief   get the smart connection type you will use from the flash 
 *
 * @param    [OUT] mode  
 * @return  success 0, fail -1    
 *			 
 */
extern int msc_config_get_smnt_type(smnt_type_e *mode);


/**
 * @brief    set whether auto start the smart connection after boot up
 *
 * @param    [IN] flag
 *              0 means disable autostart,  1 means enable autostart
 *
 * @return	 0 means success,-1 means fail
 */
extern int msc_config_set_smnt_autostart(int flag);


/**
 * @brief    get whether auto start the smart connection after boot up
 *
 * @param    [OUT] flag
 *              0 means disable autostart,  1 means enable autostart
 *
 * @return	 0 means success,-1 means fail
 */
extern int msc_config_get_smnt_autostart(int *flag);

/**
 * @function wifi_multi_smart_connection_start
 *
 * @brief    This function will start the smart connection task , user can choose one or multi type.
 *           When get the correct information from the app, the task will be terminated, or user can stop the 
 *           task by calling #wifi_multi_smart_connection_stop 
 *
 * @param    [IN] t_device_info      fill the information needed by the smart connection
 *                                          if e_type == SUPPORT_JOYLINK,please fill t_device_info.joylink_device_info
 *                                          if e_type == SUPPORT_MTK_SMNT , SUPPORT_AIRKISS , SUPPORT_AIRKISS | SUPPORT_MTK_SMNT, please fill t_device_info.am_device_info
 * @param    [IN] e_type             please refer to #smnt_type_e, you can choose one or multi type. 
 *                                   currently you can set e_type as below values:
 *                                          SUPPORT_MTK_SMNT                       only start mtk smart connection
 *                                          SUPPORT_AIRKISS                        only start airkiss
 *                                          SUPPORT_MTK_SMNT | SUPPORT_AIRKISS     start both mtk smart connection and airkiss
 *                                          SUPPORT_JOYLINK                        only start joylink
 *                                          SUPPORT_AWS                            only start aws
 *                                          if you set the e_type value none of them, you will get the MSC_SMNT_TYPE_INCORRECT error
 *
 * @param    [IN] pf_callback 
 *                     if pf_callback != NULL , please refer to #smart_connection_callback_t
                       if pf_callback == NULL , the default behavior is connect to the ap
 * @return    MSC_FAIL 
 *		  	  MSC_OK      
 *            MSC_SMNT_TYPE_INCORRECT
 *			  MSC_INVALID_ARG     		 
 */
extern int32_t wifi_multi_smart_connection_start (msc_device_info_t t_device_info,smnt_type_e e_type,smart_connection_callback_t pf_callback);

/**
 * @function wifi_multi_smart_connection_stop
 *
 * @brief    stop smart connection task
 */
extern void wifi_multi_smart_connection_stop(void);
#ifdef __cplusplus
}
#endif


#endif

