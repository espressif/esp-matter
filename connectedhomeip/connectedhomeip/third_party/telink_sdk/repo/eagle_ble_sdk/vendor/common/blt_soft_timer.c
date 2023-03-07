/********************************************************************************************************
 * @file	blt_soft_timer.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#include "stack/ble/ble.h"
#include "tl_common.h"
#include "blt_soft_timer.h"



#include "vendor/common/user_config.h"

#if (BLT_SOFTWARE_TIMER_ENABLE)




_attribute_data_retention_	blt_soft_timer_t	blt_timer;


/**
 * @brief		This function is used to Sort the timers according
 * 				to the time of the timed task, so as to trigger the
 * 				timers in turn
 * @param[in]	none
 * @return      none
 */
int  blt_soft_timer_sort(void)
{
	if(blt_timer.currentNum < 1 || blt_timer.currentNum > MAX_TIMER_NUM){
		return 0;
	}
	else{
		//BubbleSort
		int n = blt_timer.currentNum;
		u8 temp[sizeof(blt_time_event_t)];

		for(int i=0;i<n-1;i++)
		{
			for(int j=0;j<n-i-1;j++)
			{
				if(TIME_COMPARE_BIG(blt_timer.timer[j].t, blt_timer.timer[j+1].t))
				{
					//swap
					memcpy(temp, &blt_timer.timer[j], sizeof(blt_time_event_t));
					memcpy(&blt_timer.timer[j], &blt_timer.timer[j+1], sizeof(blt_time_event_t));
					memcpy(&blt_timer.timer[j+1], temp, sizeof(blt_time_event_t));
				}
			}
		}
	}

	return 1;
}



/**
 * @brief		This function is used to add new software timer task
 * @param[in]	func - callback function for software timer task
 * @param[in]	interval_us - the interval for software timer task
 * @return      0 - timer task is full, add fail
 * 				1 - create successfully
 */
int blt_soft_timer_add(blt_timer_callback_t func, u32 interval_us)
{
	u32 now = clock_time();

	if(blt_timer.currentNum >= MAX_TIMER_NUM){  //timer full
		return 	0;
	}
	else{
		blt_timer.timer[blt_timer.currentNum].cb = func;
		blt_timer.timer[blt_timer.currentNum].interval = interval_us * SYSTEM_TIMER_TICK_1US;
		blt_timer.timer[blt_timer.currentNum].t = now + blt_timer.timer[blt_timer.currentNum].interval;
		blt_timer.currentNum ++;

		blt_soft_timer_sort();

		bls_pm_setAppWakeupLowPower(blt_timer.timer[0].t,  1);

		return  1;
	}
}



/**
 * @brief		Timer tasks are originally ordered. When deleting, it will
 * 				be overwritten forward, so the order will not be destroyed
 * 				and there is no need to reorder
 * @param[in]	index - the index for some software timer task
 * @return      0 - delete fail
 * 				other - delete successfully
 */
int  blt_soft_timer_delete_by_index(u8 index)
{
	if(index >= blt_timer.currentNum){
		return 0;
	}


	for(int i=index; i<blt_timer.currentNum - 1; i++){
		memcpy(&blt_timer.timer[i], &blt_timer.timer[i+1], sizeof(blt_time_event_t));
	}

	blt_timer.currentNum --;

	return 0;
}

/**
 * @brief		This function is used to delete timer tasks
 * @param[in]	func - callback function for software timer task
 * @return      0 - delete fail
 * 				1 - delete successfully
 */
int 	blt_soft_timer_delete(blt_timer_callback_t func)
{


	for(int i=0; i<blt_timer.currentNum; i++){
		if(blt_timer.timer[i].cb == func){
			blt_soft_timer_delete_by_index(i);

			if(i == 0){  //The most recent timer is deleted, and the time needs to be updated

				if( (u32)(blt_timer.timer[0].t - clock_time()) < 3000 *  SYSTEM_TIMER_TICK_1MS){
					bls_pm_setAppWakeupLowPower(blt_timer.timer[0].t,  1);
				}
				else{
					bls_pm_setAppWakeupLowPower(0, 0);  //disable
				}

			}

			return 1;
		}
	}

	return 0;
}


/**
 * @brief		This function is used to manage software timer tasks
 * @param[in]	type - the type for trigger
 * @return      none
 */
void  	blt_soft_timer_process(int type)
{
	if(type == CALLBACK_ENTRY){ //callback trigger

	}

	u32 now = clock_time();
	if(!blt_timer.currentNum){
		bls_pm_setAppWakeupLowPower(0, 0);  //disable
		return;
	}

	if( !blt_is_timer_expired(blt_timer.timer[0].t, now) ){
		return;
	}

	int change_flg = 0;
	int result;
	for(int i=0; i<blt_timer.currentNum; i++){
		if(blt_is_timer_expired(blt_timer.timer[i].t ,now) ){ //timer trigger

			if(blt_timer.timer[i].cb == NULL){

			}
			else{
				result = blt_timer.timer[i].cb();

				if(result < 0){
					blt_soft_timer_delete_by_index(i);
				}
				else if(result == 0){
					change_flg = 1;
					blt_timer.timer[i].t = now + blt_timer.timer[i].interval;
				}
				else{  //set new timer interval
					change_flg = 1;
					blt_timer.timer[i].interval = result * SYSTEM_TIMER_TICK_1US;
					blt_timer.timer[i].t = now + blt_timer.timer[i].interval;
				}
			}
		}
	}


	if(blt_timer.currentNum ){ //timer table not empty
		if(change_flg){
			blt_soft_timer_sort();
		}

		if( (u32)(blt_timer.timer[0].t - now) < 3000 *  SYSTEM_TIMER_TICK_1MS){
			bls_pm_setAppWakeupLowPower(blt_timer.timer[0].t,  1);
		}
		else{
			bls_pm_setAppWakeupLowPower(0, 0);  //disable
		}

	}
	else{
		bls_pm_setAppWakeupLowPower(0, 0);  //disable
	}

}

/**
 * @brief		This function is used to register the call back for pm_appWakeupLowPowerCb
 * @param[in]	none
 * @return      none
 */
void 	blt_soft_timer_init(void)
{
	bls_pm_registerAppWakeupLowPowerCb(blt_soft_timer_process);
}


#endif  //end of  BLT_SOFTWARE_TIMER_ENABLE
