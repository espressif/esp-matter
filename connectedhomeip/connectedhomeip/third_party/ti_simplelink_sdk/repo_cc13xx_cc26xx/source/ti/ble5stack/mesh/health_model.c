/******************************************************************************

@file  health_model.c

@brief  Health Server Model

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

/*********************************************************************
* INCLUDES
*/
#include "stdint.h"
#include "autoconf.h"
#include "health_model.h"
#include "ti_device_composition.h"
#include "ble_stack_api.h"
#include "mesh_erpc_wrapper.h"

#define HEALTH_TEST_STANDARD         0x00
#define HEALTH_MODELS_OBJ_NUM        HEALTH_SRV_NUM
#define FAULT_TEST_LUT_SIZE           256

static inline struct healthSrv_data * healthSrv_get_data_obj(const struct bt_mesh_model * model);
static int healthSrv_fault_get_cur_cb(struct bt_mesh_model *model, uint8_t *test_id,
                                     uint16_t *company_id, uint8_t *faults,
                                     uint8_t *fault_count);
static int healthSrv_fault_get_reg_cb(struct bt_mesh_model *model, uint16_t company_id,
                                     uint8_t *test_id, uint8_t *faults,
                                     uint8_t *fault_count);
static int healthSrv_fault_clear_cb(struct bt_mesh_model *model, uint16_t company_id);
static int healthSrv_fault_test_cb(struct bt_mesh_model *model, uint8_t test_id,
                                   uint16_t company_id);
static int helathSrv_standard_fault_test(void);

// Health Server Callback
struct bt_mesh_health_srv_cb health_cb =
{
     .fault_test    = healthSrv_fault_test_cb,
     .fault_get_cur = healthSrv_fault_get_cur_cb,
     .fault_get_reg = healthSrv_fault_get_reg_cb,
     .fault_clear   = healthSrv_fault_clear_cb,
};

struct healthSrv_list_t
{
    uint8_t * data;
    int length;
};

static struct healthSrv_data
{
    struct bt_mesh_model * model_inst;
    helathSrv_faultTest_cb fault_test_lut[FAULT_TEST_LUT_SIZE];
    uint8_t last_test_id;
    uint16_t max_faults;
    struct healthSrv_list_t cur_fault_list;
    struct healthSrv_list_t reg_fault_list;
} healthSrv_obj_data_list[HEALTH_MODELS_OBJ_NUM];

/*********************************************************************
 * FUNCTIONS
 */
/** @brief Find and get a health server data object for a given health server model
 *
 *  @param model    Health server model object's address
 *
 *  @return         Health server data object if exists, NULL if no health server
 *                  data object exists for this health server model.
 */
struct healthSrv_data * healthSrv_get_data_obj(const struct bt_mesh_model * model)
{
    int i;

    for(i = 0; i < HEALTH_MODELS_OBJ_NUM; i++)
    {
        if(healthSrv_obj_data_list[i].model_inst == model)
        {
            return &healthSrv_obj_data_list[i];
        }
    }
    return NULL;
}

/** @brief Initialize health server data objects
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_init(void)
{
    int i = 0, j = 0, healthModelsFound = 0;
    uint16_t model_index;
    struct bt_mesh_model * p_health_srv_obj;
    size_t element_count = comp.elem_count;
    struct healthSrv_data * cur_obj;

    //  Go through all elements, in each element find if a
    //  health server model exists
    for(i = 0; i < element_count; i++)
    {
        model_index = bt_mesh_model_find_wrapper(i, BT_MESH_MODEL_ID_HEALTH_SRV);
        p_health_srv_obj = get_model_data(i, 0, model_index);

        // If model is a health serve model, then initiate it's corresponded
        // health server data object
        if(p_health_srv_obj && p_health_srv_obj->user_data &&
            ((struct bt_mesh_health_srv *)p_health_srv_obj->user_data)->cb == &health_cb)
        {
            // A sanity test to verify that we did not find more health model objects than
            // the given health model objects number (given by HEALTH_MODELS_OBJ_NUM)
            // If we did, return an error.
            if(healthModelsFound == HEALTH_MODELS_OBJ_NUM)
            {
                for(j = 0; j < healthModelsFound; j++)
                {
                    ICall_free(healthSrv_obj_data_list[j].cur_fault_list.data);
                    ICall_free(healthSrv_obj_data_list[j].reg_fault_list.data);
                }
                return -1;
            }

            cur_obj = &healthSrv_obj_data_list[healthModelsFound];
            cur_obj->model_inst = p_health_srv_obj;
            memset(&cur_obj->fault_test_lut, 0, FAULT_TEST_LUT_SIZE);
            cur_obj->last_test_id = 0;

            // Get the maximum faults out of the message buffer size, since health status message
            // consists of Test ID (1 byte long) , Company ID (2 bytes long) and Fault Array (N byte long, one for each fault)
            // Zephyr adds one more byte to the buffer, thus 1+2+1 = 4 and N = message_buffer_len - 4
            cur_obj->max_faults =p_health_srv_obj->pub->msg->size - 4;

            cur_obj->cur_fault_list.data = ICall_malloc(cur_obj->max_faults);
            if(cur_obj->cur_fault_list.data == NULL)
            {
                for(j = 0; j < healthModelsFound; j++)
                {
                    ICall_free(healthSrv_obj_data_list[j].cur_fault_list.data);
                    ICall_free(healthSrv_obj_data_list[j].reg_fault_list.data);
                }
                return -1;
            }
            cur_obj->cur_fault_list.length = 0;

            cur_obj->reg_fault_list.data = ICall_malloc(cur_obj->max_faults);
            if(cur_obj->reg_fault_list.data == NULL)
            {
                for(j = 0; j < healthModelsFound; j++)
                {
                    ICall_free(healthSrv_obj_data_list[j].cur_fault_list.data);
                    ICall_free(healthSrv_obj_data_list[j].reg_fault_list.data);
                }
                ICall_free(cur_obj->cur_fault_list.data);
                return -1;
            }
            cur_obj->reg_fault_list.length = 0;

            // Add the fault standard test to the fault test LUT
            healthSrv_register_fault_test_cb(p_health_srv_obj, HEALTH_TEST_STANDARD, helathSrv_standard_fault_test);
            healthModelsFound++;
        }
    }

    return 0;
}

/** @brief Adding fault to the Health Server fault array
 *
 *  @param fault_id  fault id number to add
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_cur_fault_add(const struct bt_mesh_model * model, const uint8_t fault_id)
{
    int i;
    struct healthSrv_data * healthSrv_data_obj;

    healthSrv_data_obj = healthSrv_get_data_obj(model);
    if(healthSrv_data_obj == NULL)
    {
        return -1;
    }

    // Check that the lists are not full
    if(  (healthSrv_data_obj->cur_fault_list.length >= healthSrv_data_obj->max_faults) ||
         (healthSrv_data_obj->reg_fault_list.length >= healthSrv_data_obj->max_faults) )
    {
        return -1;
    }

    // Verifying that the current fault id does not exists in the array
    for(i = 0; (i < healthSrv_data_obj->cur_fault_list.length) && (healthSrv_data_obj->cur_fault_list.data[i] != fault_id); i++);

    // If it does not, add it to the current fault array
    if(i == healthSrv_data_obj->cur_fault_list.length)
    {
        healthSrv_data_obj->cur_fault_list.data[healthSrv_data_obj->cur_fault_list.length] = fault_id;
        healthSrv_data_obj->cur_fault_list.length++;
    }

    // Verifying that the current fault id does not exists in the array
    for(i = 0; (i < healthSrv_data_obj->reg_fault_list.length) && (healthSrv_data_obj->reg_fault_list.data[i] != fault_id); i++);

    // If it does not, add it to the registered fault array
    if(i == healthSrv_data_obj->reg_fault_list.length)
    {
        healthSrv_data_obj->reg_fault_list.data[healthSrv_data_obj->reg_fault_list.length] = fault_id;
        healthSrv_data_obj->reg_fault_list.length++;
    }

    return 0;
}


/** @brief clear all faults in current fault array
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_cur_fault_clear(const struct bt_mesh_model * model)
{
    struct healthSrv_data * healthSrv_data_obj;

    healthSrv_data_obj = healthSrv_get_data_obj(model);
    if(healthSrv_data_obj == NULL)
    {
        return -1;
    }

    healthSrv_data_obj->cur_fault_list.length = 0;
    memset(healthSrv_data_obj->cur_fault_list.data, 0, healthSrv_data_obj->max_faults);
    return 0;
}


/** @brief clear all faults in registered fault array
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_reg_fault_clear(const struct bt_mesh_model * model)
{
    struct healthSrv_data * healthSrv_data_obj;

    healthSrv_data_obj = healthSrv_get_data_obj(model);
    if(healthSrv_data_obj == NULL)
    {
        return -1;
    }

    healthSrv_data_obj->reg_fault_list.length = 0;
    memset(healthSrv_data_obj->reg_fault_list.data, 0, healthSrv_data_obj->max_faults);
    return 0;
}

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
                                    helathSrv_faultTest_cb test_func_ptr )
{
    struct healthSrv_data * healthSrv_data_obj;
    healthSrv_data_obj = healthSrv_get_data_obj(model);

    if(healthSrv_data_obj == NULL)
    {
        return -1;
    }

    healthSrv_data_obj->fault_test_lut[test_id] = test_func_ptr;
    return 0;
}

/* @brief   A standard fault test.
 *          This is an empty implementation of fault test.
 *          It is needed by the IEEE mesh spec.
 *  @return 0 on success, or (negative) error code otherwise.
 */
int helathSrv_standard_fault_test(void)
{
    return 0;
}


/** @brief Callback for fetching current faults.
*
*  Fault values may either be defined by the specification, or by a
*  vendor. Vendor specific faults should be interpreted in the context
*  of the accompanying Company ID. Specification defined faults may be
*  reported for any Company ID, and the same fault may be presented
*  for multiple Company IDs.
*
*  All faults shall be associated with at least one Company ID,
*  representing the device vendor or some other vendor whose vendor
*  specific fault values are used.
*
*  If there are multiple Company IDs that have active faults,
*  return only the faults associated with one of them at the time.
*  To report faults for multiple Company IDs, interleave which Company
*  ID is reported for each call.
*
*  @param model       Health Server model instance to get faults of.
*  @param test_id     Contains the last performed test ID.
*  @param company_id  Contains the company ID.
*  @param faults      An array containing the current faults
*  @param fault_count Contains the length of the faults array.
*
*  @return 0 on success, or (negative) error code otherwise.
*/
int healthSrv_fault_get_cur_cb(struct bt_mesh_model *model, uint8_t *test_id,
                             uint16_t *company_id, uint8_t *faults,
                             uint8_t *fault_count)
{
    struct healthSrv_data * healthSrv_data_obj;

    healthSrv_data_obj = healthSrv_get_data_obj(model);
    if(healthSrv_data_obj == NULL)
    {
        return -1;
    }

    *test_id        = healthSrv_data_obj->last_test_id;
    *company_id     = BT_COMP_ID;
    *fault_count    = healthSrv_data_obj->cur_fault_list.length;

    if(healthSrv_data_obj->cur_fault_list.length)
    {
        memcpy( (void *)faults, (const void *)healthSrv_data_obj->cur_fault_list.data,
                (sizeof(uint8_t)*healthSrv_data_obj->cur_fault_list.length) );
    }

    return 0;
}


/** @brief Callback for fetching all registered faults.
*
*  Registered faults are all past and current faults since the last
*  call to @c fault_clear. Only faults associated with the given
*  Company ID should be reported.
*
*  Fault values may either be defined by the specification, or by a
*  vendor. Vendor specific faults should be interpreted in the context
*  of the accompanying Company ID. Specification defined faults may be
*  reported for any Company ID, and the same fault may be presented
*  for multiple Company IDs.
*
*  @param model       Health Server model instance to get faults of.
*  @param company_id  Company ID to get faults for.
*  @param test_id     Contains the last performed test ID.
*  @param faults      An array containing the registered faults
*  @param fault_count Contains the length of the faults array.
*
*  @return 0 on success, or (negative) error code otherwise.
*/
int healthSrv_fault_get_reg_cb(struct bt_mesh_model *model, uint16_t company_id,
                             uint8_t *test_id, uint8_t *faults,
                             uint8_t *fault_count)
{
    struct healthSrv_data * healthSrv_data_obj;

    healthSrv_data_obj = healthSrv_get_data_obj(model);

    if( (healthSrv_data_obj == NULL) || (company_id != BT_COMP_ID) )
    {
        return -1;
    }

    *test_id        = healthSrv_data_obj->last_test_id;
    *fault_count    = healthSrv_data_obj->reg_fault_list.length;

    if(healthSrv_data_obj->reg_fault_list.length)
    {
        memcpy( (void *)faults, (const void *)healthSrv_data_obj->reg_fault_list.data,
                (sizeof(uint8_t)*healthSrv_data_obj->reg_fault_list.length) );
    }

    return 0;
}


/** @brief Clear all registered faults associated with the given Company
 * ID.
 *
 *  @param model      Health Server model instance to clear faults of.
 *  @param company_id Company ID to clear faults for.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int healthSrv_fault_clear_cb(struct bt_mesh_model *model, uint16_t company_id)
{
    if(company_id != BT_COMP_ID)
    {
        return -1;
    }

    return healthSrv_reg_fault_clear(model);
}


/* Health Server - Fault Test Callback */
/** @brief Run a self-test.
 *
 *  The Health server may support up to 256 self-tests for each Company
 *  ID. The behavior for all test IDs are vendor specific, and should be
 *  interpreted based on the accompanying Company ID. Test failures
 *  should result in changes to the fault array.
 *
 *  @param model      Health Server model instance to run test for.
 *  @param test_id    Test ID to run.
 *  @param company_id Company ID to run test for.
 *
 *  @return 0 if the test execution was started successfully, or
 * (negative) error code otherwise. Note that the fault array will not
 * be reported back to the client if the test execution didn't start.
 */
int healthSrv_fault_test_cb(struct bt_mesh_model *model, uint8_t test_id,
                            uint16_t company_id)
{
    struct healthSrv_data * healthSrv_data_obj;

    healthSrv_data_obj = healthSrv_get_data_obj(model);

    if( (company_id != BT_COMP_ID) || (healthSrv_data_obj == NULL) ||
         (healthSrv_data_obj->fault_test_lut[test_id] == NULL) )
    {
        return -1;
    }

    healthSrv_cur_fault_clear(model);
    healthSrv_reg_fault_clear(model);
    healthSrv_data_obj->last_test_id = test_id;

    return healthSrv_data_obj->fault_test_lut[test_id]();
}
