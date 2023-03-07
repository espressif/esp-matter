/*************************************

Copyright (c) 2015-2050, JD Smart All rights reserved. 

*************************************/
#ifndef _JOYLINK_SMNT_H_
#define _JOYLINK_SMNT_H_

#include "joylink_smnt_adp.h"

//#define printf_high printf

#define IS_FULL_LOG							//is to open the full log function

/*
typedef struct _joylinkResult
{
	unsigned char type;	                        // 0:NotReady, 1:ControlPacketOK, 2:BroadcastOK, 3:MulticastOK
	unsigned char ssid[33];
	unsigned char pass[33];
	unsigned char bssid[6];
}joylinkResult_t;
*/
typedef enum 
{
	TYPE_JOY_OTHER = 0,
	TYPE_JOY_SMNT  = 1
}joylink_smnt_type_t;

typedef enum _joylink_smnt_status
{
	SMART_CH_INIT 		=		0x1,
	SMART_CH_LOCKING 	=	0x2,
	SMART_CH_LOCKED 	=	0x4,
	SMART_FINISH 		= 0x8
}joylink_smnt_status_t;

typedef struct _joylink_smnt_info
{
	joylink_smnt_type_t			joy_smnt_type;				//smnt type TYPE_JOY_OTHER-other type, TYPE_JOY_SMNT - joylink's smnt
	joylink_smnt_status_t 		joy_smnt_state;				//smnt state,
	uint8 						joy_smnt_bssid[6];			//bssid
	uint8 						joy_smnt_channel_fix;		//the current channel
}joylink_smnt_info_t;


typedef struct
{
	uint8 jd_ssid_len;
	uint8 jd_password_len;
	uint8 jd_ssid[33];
	uint8 jd_password[33];
//	uint8 jd_bssid[6];
}joylink_smnt_result_t;

typedef void (*joylink_smnt_result_callback_t)(joylink_smnt_result_t result_info);
typedef int (*printf_fuc_t)(char *fmt, ...);

typedef struct
{
	uint8	key[16];
	joylink_smnt_result_callback_t	smnt_result_cb;
	printf_fuc_t					printf_func;
} joylink_smnt_init_param_t;

extern void adp_changeCh(int i);
void joylink_smnt_init(joylink_smnt_init_param_t init_para);                                  
joylink_smnt_info_t	joylink_get_smnt_info(void);								//查看当前的joylink 一键配置状态
void joylink_cfg_DataAction(PHEADER_802_11_SMNT pHeader, int length);                // 局域网收到数据包
int joylink_smnt_50msTimer(unsigned char is_change_channel);
#endif
