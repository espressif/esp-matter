/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure MSC service (non-secure side).
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "sli_tz_service_msc.h"
#include "sli_tz_ns_interface.h"
#include "sli_tz_s_interface.h"

#include "em_msc.h"

//------------------------------------------------------------------------------
// Function definitions

/***************************************************************************//**
 * @brief
 *   Initialize MSC module. Puts MSC hw in a known state.
 ******************************************************************************/
void MSC_Init(void)
{
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_init,
    SLI_TZ_DISPATCH_UNUSED_ARG);
}

/***************************************************************************//**
 * @brief
 *   Turn off MSC flash write enable and lock MSC registers.
 ******************************************************************************/
void MSC_Deinit(void)
{
  (void)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_deinit,
    SLI_TZ_DISPATCH_UNUSED_ARG);
}

/***************************************************************************//**
 * @brief
 *   Erases a page in flash memory.
 *
 * @note
 *   This function is placed in flash memory: care therefore needs to be taken
 *   in order to avoid erasing the flash page it lives in.
 *
 * @param[in] startAddress
 *   Pointer to the flash page to erase. Must be aligned to beginning of page
 *   boundary.
 * @return
 *   Returns the status of erase operation, @ref MSC_Status_TypeDef
 * @verbatim
 *   mscReturnOk - Operation completed successfully.
 *   mscReturnInvalidAddr - Operation tried to erase a non-flash area.
 *   flashReturnLocked - MSC registers are locked or the operation tried to
 *                       erase a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out.
 * @endverbatim
 ******************************************************************************/
MSC_Status_TypeDef MSC_ErasePage(uint32_t *startAddress)
{
  return (MSC_Status_TypeDef)sli_tz_ns_interface_dispatch_simple(
    (sli_tz_simple_veneer_fn)sli_tz_msc_erase_page,
    (uint32_t)startAddress);
}

/***************************************************************************//**
 * @brief
 *   Writes data to flash memory. Write data must be aligned to words and
 *   contain a number of bytes that is divisible by four.
 * @note
 *   It is recommended to erase the flash page before performing a write.
 *
 *   The Flash memory is organized into 64-bit wide double-words.
 *   Each 64-bit double-word can be written only twice using burst write
 *   operation between erasing cycles. The user's application must store data in
 *   RAM to sustain burst write operation.
 *
 *   EFR32XG21 RevC is not able to program every word twice between erases.
 *
 * @param[in] address
 *   Pointer to the flash word to write to. Must be aligned to words.
 * @param[in] data
 *   Data to write to flash.
 * @param[in] numBytes
 *   Number of bytes to write to flash. NB: Must be divisable by four.
 * @return
 *   Returns the status of the write operation, @ref MSC_Status_TypeDef
 * @verbatim
 *   flashReturnOk - Operation completed successfully.
 *   flashReturnInvalidAddr - Operation tried to write to a non-flash area.
 *   flashReturnLocked - MSC registers are locked or the operation tried to
 *                       program a locked area of the flash.
 *   flashReturnTimeOut - Operation timed out.
 * @endverbatim
 ******************************************************************************/
MSC_Status_TypeDef MSC_WriteWord(uint32_t *address,
                                 const void *data,
                                 uint32_t numBytes)
{
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_MSC_WRITEWORD_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { .base = data, .len = numBytes },
  };
  psa_outvec out_vec[] = {
    { .base = address, .len = numBytes }
  };

  return (MSC_Status_TypeDef)sli_tz_ns_interface_dispatch(
    (sli_tz_veneer_fn)sli_tz_s_interface_dispatch_msc,
    (uint32_t)in_vec,
    IOVEC_LEN(in_vec),
    (uint32_t)out_vec,
    IOVEC_LEN(out_vec));
}

/***************************************************************************//**
 * @brief
 *   Writes data to flash memory using the DMA.
 *
 * @details
 *   This function uses the LDMA to write data to the internal flash memory.
 *   This is the fastest way to write data to the flash and should be used when
 *   the application wants to achieve write speeds like they are reported in the
 *   datasheet. Note that copying data from flash to flash will be slower than
 *   copying from RAM to flash. So the source data must be in RAM in order to
 *   see the write speeds similar to the datasheet numbers.
 *
 *   In order to make use of the LDMA bus master, it first needs to be
 *   (re)configured as a TZ secure bus master. For security reasons, the non-
 *   secure application cannot be allowed to interrupt the exectution of the
 *   secure code during this time period. For this reason, this function will
 *   enter a critical region and wait until any ongoing LDMA operations have
 *   finished before performing the write operaion.
 *
 * @note
 *   This function requires that the LDMA and LDMAXBAR clock is enabled.
 *
 * @param[in] ch
 *   DMA channel to use
 *
 * @param[in] address
 *   A pointer to the flash word to write to. Must be aligned to words.
 *
 * @param[in] data
 *   Data to write to flash.
 *
 * @param[in] numBytes
 *   A number of bytes to write from flash. NB: Must be divisible by four.
 *
 * @return
 *   Returns the status of the write operation.
 * @verbatim
 *   flashReturnOk - The operation completed successfully.
 *   flashReturnInvalidAddr - The operation tried to erase a non-flash area.
 * @endverbatim
 ******************************************************************************/
MSC_Status_TypeDef MSC_WriteWordDma(int ch,
                                    uint32_t *address,
                                    const void *data,
                                    uint32_t numBytes)
{
  sli_tz_fn_id fn_id = SLI_TZ_SERVICE_MSC_WRITEWORDDMA_SID;
  psa_invec in_vec[] = {
    { .base = &fn_id, .len = sizeof(sli_tz_fn_id) },
    { .base = &ch, .len = sizeof(int) },
    { .base = data, .len = numBytes },
  };
  psa_outvec out_vec[] = {
    { .base = address, .len = numBytes }
  };

  return (MSC_Status_TypeDef)sli_tz_ns_interface_dispatch(
    (sli_tz_veneer_fn)sli_tz_s_interface_dispatch_msc,
    (uint32_t)in_vec,
    IOVEC_LEN(in_vec),
    (uint32_t)out_vec,
    IOVEC_LEN(out_vec));
}
