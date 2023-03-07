#ifndef _JOYLINK_API_H_
#define _JOYLINK_API_H_

#include "joylink_innet_ctl.h"
#include "msc_api.h"

typedef struct{
	uint8_t cid[SIZE_CID];
	uint8_t puid[SIZE_PUID];
	uint8_t smnt_key[SIZE_SMNT_KEY];
}DEVICE_INFO_T;


/*
 *  @description : Stop the joylink smart connection task
 */
extern void joylink_smnt_stop();


/*
 *  @function     : joylink_smnt_start
 *  @description  : This function will start the joylink smart connection task 
 *                  When get the correct information from the app, the task will be terminated, or user can stop the 
 *                  task by calling #joylink_smnt_stop function
 *  @para         : (in)pt_info
 *  @ret          : 0 success
 *				   -1 fail
 */
extern int joylink_smnt_start(const DEVICE_INFO_T * pt_info,smart_connection_callback_t pf_callback);

#endif





