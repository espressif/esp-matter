/********************************************************************************************************
 * @file	ble_host.h
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
#ifndef STACK_BLE_HOST_BLE_HOST_H_
#define STACK_BLE_HOST_BLE_HOST_H_

#include "stack/ble/ble_common.h"
#include "stack/ble/ble_format.h"

#include "stack/ble/host/l2cap/l2cap.h"


#include "stack/ble/host/attr/attr_stack.h"
#include "stack/ble/host/attr/att.h"
#include "stack/ble/host/attr/gatt.h"


#include "stack/ble/host/smp/smp.h"
#include "stack/ble/host/smp/smp_alg.h"
#include "stack/ble/host/smp/smp_peripheral.h"
#if (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
#include "stack/ble/host/smp/smp_central.h"
#endif
#include "stack/ble/host/smp/smp_storage.h"


#include "stack/ble/host/gap/gap.h"
#include "stack/ble/host/gap/gap_event.h"


/*********************************************************/
//Remove when file merge to SDK //
#include "stack/ble/ble_stack.h"
#include "stack/ble/ble_config.h"
//#include "stack/ble/debug.h"
#include "stack/ble/trace.h"

#include "stack/ble/host/host_stack.h"
#include "stack/ble/host/l2cap/l2cap_stack.h"
#include "stack/ble/host/attr/attr_stack.h"
#include "stack/ble/host/smp/smp_stack.h"
#include "stack/ble/host/gap/gap_stack.h"
//#include "stack/ble/host/gap/gap_stack.h"
/*********************************************************/


#endif /* STACK_BLE_HOST_BLE_HOST_H_ */
