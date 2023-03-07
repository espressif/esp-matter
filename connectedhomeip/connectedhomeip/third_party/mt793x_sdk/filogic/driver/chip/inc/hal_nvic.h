/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __HAL_NVIC_H__
#define __HAL_NVIC_H__
#include "hal_platform.h"

#ifdef HAL_NVIC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup NVIC
 * @{
 * This section introduces the Nested Vectored Interrupt Controller(NVIC) HAL APIs including terms and acronyms, details on how to use this driver, supported features, enums, structures and functions.
 * NVIC usage follows the CMSIS except it doesn't support direct registration of ISR in the vector table, but it uses
 * #hal_nvic_register_isr_handler function to improve software efficiency and debugging.
 *
 * @section HAL_NVIC_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * \if MT7933
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M33. For more details, please refer to <a href="https://developer.arm.com/documentation/100230/0004/functional-description/nested-vectored-interrupt-controller"> NVIC introduction in ARM Cortex-M33 Processor Technical Reference Manual </a>.|
 * \else
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 * \endif
* |\b ISR                  | Interrupt service routine.|
 * |\b IRQ                  | Interrupt request.|
 * |\b CMSIS                | Cortex Microcontroller Software Interface Standard. For more information, please refer to <a href="https://www.arm.com/zh/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php"> CMSIS </a>.
 *
 *
 *
 * @section HAL_NVIC_Driver_Usage_Chapter How to use this driver
 *
 * - Using the NVIC. \n
 *
 *  - Step 1. Call the #hal_nvic_init() function to initialize the NVIC IRQ priority according to the exception number. By default, the lower the exception number the higher is the priority.
 *  - Step 2. Call the #hal_nvic_register_isr_handler() function to register an ISR handler before using the NVIC IRQ.
 *  - Step 3. Use CMSIS interface to adjust the IRQ priority, if needed.
 *  - Step 4. Call the #hal_nvic_enable_irq() to enable the IRQ.
 *  - Sample code:
 *    @code
 *       // Define a callback (an ISR) function.
 *       void isr_handler(hal_nvic_irq_t irq_number)
 *       {
 *           // Do something.
 *       }
 *    @endcode
 *    @code
 *       ret = hal_nvic_init();    // Should be called only once at initialization.
 *       if (HAL_NVIC_STATUS_OK != ret) {
 *               // Error handler;
 *       }
 *       ret = hal_nvic_register_isr_handler(irq_number, isr_handler);   // Register an IRQ handler.
 *       if (HAL_NVIC_STATUS_OK !=  ret) {
 *               // Error handler;
 *       }
 *       hal_nvic_enable_irq(irq_number);
 *
 *       // Change the IRQ priority (optional).
 *       hal_nvic_disable_irq(irq_number);   // Disable the IRQ number.
 *       hal_nvic_set_priority(irq_number, DEFAULT_IRQ_PRIORITY);  // Change the priority of the IRQ number, if needed.
 *       hal_nvic_enable_irq(irq_number);    // Enable the IRQ number.
 *
 *    @endcode
 *    @code
 *       void function_task(void)
 *       {
 *           // function_task() runs only in thread mode.
 *       }
 *
 *       void function_isr(void)
 *       {
 *           // function_task() runs only in handler mode.
 *       }
 *
 *    @endcode
 *    @code
 *
 *       // It needs to take different actions according to the Cortex-M current exception status.
 *       if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == 0) {
 *           // Cortex-M is in thread mode.
 *           function_task();
 *       } else {
 *           // Cortex-M is handling the exception and the exception number is HAL_NVIC_QUERY_EXCEPTION_NUMBER.
 *           function_isr();
 *       }
 *
 *    @endcode
 *    @code
 *       // Preserve the original IRQ settings.
 *       void example(void)
 *       {
 *           uint32_t mask;
 *           hal_nvic_save_and_set_interrupt_mask(&mask); // Save the current IRQ settings.
 *           // Do something with the protected critical resource when the IRQ is disabled.
 *           hal_nvic_restore_interrupt_mask(mask);       // Restore the IRQ settings.
 *       }
 *
 *       void test(void)
 *       {
 *           uint32_t mask;
 *           hal_nvic_save_and_set_interrupt_mask(&mask);   // Save the current IRQ settings.
 *           example();
 *           // The IRQ is still disabled;
 *           // Do something with the protected critical resouce when the IRQ is disabled.
 *           hal_nvic_restore_interrupt_mask(mask);  // Restore the original IRQ settings in the mask.
 *       }
 *    @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_nvic_enum Enums
  * @{
  */

/** @brief  This enum defines the return status of the NVIC API.  */
typedef enum {
    HAL_NVIC_STATUS_ERROR_NO_ISR = -4,              /**< ISR routine is null. */
    HAL_NVIC_STATUS_ERROR_IRQ_NUMBER  = -3,         /**< Incorrect IRQ number. */
    HAL_NVIC_STATUS_INVALID_PARAMETER = -2,         /**< Invalid parameter. */
    HAL_NVIC_STATUS_ERROR             = -1,         /**< NVIC unspecified error. */
    HAL_NVIC_STATUS_OK                = 0           /**< No error occurred. */
} hal_nvic_status_t;


/**
  * @}
  */


/** @defgroup hal_nvic_typedef Typedef
  * @{
  */
/** @brief  This defines the callback function prototype.
 *          User should register a callback (an ISR) for each NVIC IRQ in use.
 *          The callback function is called in the NVIC ISR after the NVIC IRQ is triggered.
 *          For more details about the callback, please refer to #hal_nvic_register_isr_handler().
 *  @param [in] irq_number is given by driver to notify the current NVIC IRQ.
 */
typedef void (*hal_nvic_isr_t)(hal_nvic_irq_t irq_number);
/**
  * @}
  */


/** @defgroup hal_nvic_macro Define
  * @{
  */
/** This macro is used to query the current exception status of Cortex-M.
    If the value is 0, the Cortex-M is running successfully, otherwise an exception occured
    and the exception number is the value of the macro. */
#define HAL_NVIC_QUERY_EXCEPTION_NUMBER ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos)

#ifdef MTK_PORTING_AB
/** @brief  This macro defines the nvic no exception occured.
  */
#define HAL_NVIC_NOT_EXCEPTION 0
#endif

/**
  * @}
  */


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function is used to register a callback (an ISR) for NVIC IRQ.
 * @param[in] irq_number is the NVIC IRQ number.
 * @param[in] isr_handler is the NVIC IRQ's ISR.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t isr_handler);


/**
 * @brief This function initializes the NVIC IRQ with default priority. The lower the exception number, the higher the priority.
 */
hal_nvic_status_t hal_nvic_init(void);

/**
 * @brief This function enables a device-specific interrupt in the NVIC interrupt controller.
 * @param[in] irq_number is the NVIC IRQ number.
 */
hal_nvic_status_t hal_nvic_enable_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function disables a device-specific interrupt in the NVIC interrupt controller.
 * @param[in] irq_number is the NVIC IRQ number.
 */
hal_nvic_status_t hal_nvic_disable_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function reads the pending register in the NVIC and returns the pending bit for the specified interrupt.
 * @param[in] irq_number is the NVIC IRQ number.
 * @return    To indicate whether this IRQ number is pending or not.
 *            If the return value is 0, the interrupt status is not pending.
 *            If the return value is 1, the interrupt status is pending.
 *            If the return value is other value, the IRQ number is invalid.
 */
uint32_t hal_nvic_get_pending_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function sets the pending bit of an external interrupt.
 * @param[in] irq_number is the NVIC IRQ number.
 */
hal_nvic_status_t hal_nvic_set_pending_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function clears the pending bit of an external interrupt.
 * @param[in] irq_number is the NVIC IRQ number.
 */
hal_nvic_status_t hal_nvic_clear_pending_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function sets the priority of an interrupt.
 * @param[in] irq_number is the NVIC IRQ number.
 * @param[in] priority is the priority to set.
 */
hal_nvic_status_t hal_nvic_set_priority(hal_nvic_irq_t irq_number, uint32_t priority);

/**
 * @brief This function reads the priority of an interrupt. The interrupt number can be positive to specify an external (device specific)
 * interrupt, or negative to specify an internal (core) interrupt.
 * @param[in] irq_number is the NVIC IRQ number.
 * @return    Interrupt Priority. Value is aligned automatically to the implemented priority bits of the microcontroller.
 *            If the return value is DEFAULT_IRQ_PRIORITY + 63, the IRQ number is invalid.
 */
uint32_t hal_nvic_get_priority(hal_nvic_irq_t irq_number);

/**
 * @brief This function saves the current IRQ settings in a temporary variable, and then disables the IRQ by setting the IRQ mask.
 *  It should be used in conjunction with #hal_nvic_restore_interrupt_mask() to protect the critical resources.
 *  When the IRQ is masked, please don't print any log, it maybe lead to some unexpected errors.
 * @param[out] mask is used to store the current IRQ settings, upon the return of this function.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask);

/**
 * @brief This function restores the IRQ settings as specified in the mask. It should be used in conjunction with
 *  #hal_nvic_save_and_set_interrupt_mask() to protect critical resources.
 * @param[in] mask is an unsigned integer to specify the IRQ settings.
 * @return    Indicates whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask);

#ifdef HAL_NVIC_IRQ_SET_TYPE_FEATURE

/**
 * @brief This function sets irq type.
 * @param[in]  irq_number is the NVIC IRQ number.
 * @param[in]  type is the NVIC IRQ type.
 * @return    Indicates whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_irq_set_type(hal_nvic_irq_t irq_number, hal_nvic_irq_type_t type);
#endif /* HAL_NVIC_IRQ_SET_TYPE_FEATURE */

#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_NVIC_MODULE_ENABLED*/
#endif /* __HAL_NVIC_H__ */

