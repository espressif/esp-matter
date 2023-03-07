/********************************************************************************************************
 * @file	ll_pm.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
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
#ifndef LL_PM_H_
#define LL_PM_H_




////////////////// Power Management ///////////////////////
/**
 * @brief	Telink defined Low power state Type
 */
#define			SUSPEND_DISABLE				0
#define			SUSPEND_ADV					BIT(0)
#define			SUSPEND_CONN				BIT(1)
#define			DEEPSLEEP_RETENTION_ADV		BIT(2)
#define			DEEPSLEEP_RETENTION_CONN	BIT(3)
#if (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
#define			MCU_STALL					BIT(6)
#endif




/**
 * @brief	Telink defined ble stack low power mode process callback function
 */
typedef 	void (*ll_module_pm_callback_t)(void);

/**
 * @brief	Telink defined application wake up low power mode process callback function
 */
typedef 	void (*pm_appWakeupLowPower_callback_t)(int);



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	for user to initialize low power mode
 * @param	none
 * @return	none
 */
void 		blc_ll_initPowerManagement_module(void);

/**
 * @brief	for user to set low power mode mask
 * @param	mask - low power mode mask
 * @return	none
 */
void		bls_pm_setSuspendMask (u8 mask);

/**
 * @brief	for user to get low power mode mask
 * @param	none
 * @return	bltPm.suspend_mask
 */
u8 			bls_pm_getSuspendMask (void);

/**
 * @brief	for user to set low power mode wake up source
 * @param	source - low power mode wake up source
 * @return	none
 */
void 		bls_pm_setWakeupSource(u8 source);

/**
 * @brief	for user to get low power mode wake up time
 * @param	none
 * @return	bltPm.current_wakeup_tick
 */
u32 		bls_pm_getSystemWakeupTick(void);

#if (MCU_CORE_TYPE == MCU_CORE_9518)
/**
 * @brief	for user to get low power mode next connect event wake up time
 * @param	none
 * @return	blt_next_event_tick
 */
u32 		bls_pm_getNexteventWakeupTick(void);
#endif
/**
 * @brief	for user to set latency manually for save power
 * @param	latency - bltPm.user_latency
 * @return	none
 */
void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power

/**
 * @brief	for user to set application wake up low power mode
 * @param	wakeup_tick - low power mode wake up time
 * @param	enable - low power mode application wake up enable
 * @return	none
 */
void 		bls_pm_setAppWakeupLowPower(u32 wakeup_tick, u8 enable);

/**
 * @brief	for user to register the callback for application wake up low power mode process
 * @param	cb - the pointer of callback function
 * @return  none.
 */
void 		bls_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);

/**
 * @brief	for user to set the threshold of sleep tick for entering deep retention mode
 * @param	adv_thres_ms - the threshold of sleep tick for advertisement state
 * @param	conn_thres_ms - the threshold of sleep tick for connection state
 * @return  none.
 */
void 		blc_pm_setDeepsleepRetentionThreshold(u32 adv_thres_ms, u32 conn_thres_ms);

/**
 * @brief	for user to set early wake up tick for deep retention mode
 * @param	earlyWakeup_us - early wake up tick for deep retention mode
 * @return  none.
 */
void 		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(u32 earlyWakeup_us);

/**
 * @brief	for user to set the type of deep retention mode
 * @param	sleep_type - the type of deep retention mode
 * @return  none.
 */
void 		blc_pm_setDeepsleepRetentionType(SleepMode_TypeDef sleep_type);

#ifdef __cplusplus
}
#endif

#endif /* LL_PM_H_ */
