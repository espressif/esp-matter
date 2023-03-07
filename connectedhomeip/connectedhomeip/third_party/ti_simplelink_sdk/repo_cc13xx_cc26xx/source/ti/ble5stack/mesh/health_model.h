/******************************************************************************

@file  health_model.h

@brief  Health Server Model defines and APIs

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************


*****************************************************************************/

#ifndef HEALTH_MODEL_H_
#define HEALTH_MODEL_H_
/*********************************************************************
* INCLUDES
*/
#include <zephyr.h>
#include <sys/printk.h>
#include <settings/settings.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh.h>

typedef int (*helathSrv_faultTest_cb)(void);

/*********************************************************************
 * FUNCTIONS
 */
/** @brief Adding fault to the Health Server fault array
 *
 *  @param fault_id  fault id number to add
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_cur_fault_add(const struct bt_mesh_model * model, const uint8_t fault_id);

/** @brief clear all faults in current fault array
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_cur_fault_clear(const struct bt_mesh_model * model);

/** @brief clear all faults in registered fault array
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_reg_fault_clear(const struct bt_mesh_model * model);

/* @brief register a health fault test.
 *        The registered fault test would be called when a
 *        fault test packet is received with the given test ID.
 *
 * @param test_id: The test ID of the fault test
 * @param test_func_ptr: A pointer to the fault test function
 *
 * @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_register_fault_test_cb( const struct bt_mesh_model * model, const uint8_t test_id,
                                    helathSrv_faultTest_cb test_func_ptr );

/** @brief Initialize health server data objects
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_init(void);

#endif /* HEALTH_MODEL_H_ */
