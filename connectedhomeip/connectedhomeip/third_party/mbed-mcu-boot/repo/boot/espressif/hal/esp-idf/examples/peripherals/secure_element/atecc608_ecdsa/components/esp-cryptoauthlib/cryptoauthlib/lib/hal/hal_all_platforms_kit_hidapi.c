/**
 * \file
 * \brief HAL for kit protocol over HID for any platform.
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hidapi.h"

#include "atca_hal.h"
#include "hal/kit_protocol.h"

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

/** \brief HAL implementation of Kit USB HID init
 *  \param[in] hal pointer to HAL specific data that is maintained by this HAL
 *  \param[in] cfg pointer to HAL specific configuration data that is used to initialize this HAL
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_hid_init(ATCAIface iface, ATCAIfaceCfg* cfg)
{
    int i = 0;
    int index = 0;

    // Check the input variables
    if ((cfg == NULL) || (iface == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    // Create the enumerate object
#ifdef KIT_DEBUG
    printf("Enumerate HID device(s)\n");
#endif
    hid_init();

    iface->hal_data = hid_open(cfg->atcahid.vid, cfg->atcahid.pid, NULL);

    return (iface->hal_data) ? ATCA_SUCCESS : ATCA_COMM_FAIL;
}

/** \brief HAL implementation of Kit HID post init
 *  \param[in] iface  instance
 *  \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_hid_post_init(ATCAIface iface)
{
    return ATCA_SUCCESS;
}

/** \brief HAL implementation of kit protocol send over USB HID
 *  \param[in] iface          instance
 *  \param[in] word_address   determine device transaction type
 *  \param[in] txdata         pointer to bytes to send
 *  \param[in] txlength       number of bytes to send
 *  \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_hid_send(ATCAIface iface, uint8_t word_address, uint8_t* txdata, int txlength)
{
    ATCAIfaceCfg *cfg = atgetifacecfg(iface);
    hid_device* pHid = (hid_device*)atgetifacehaldat(iface);
    int bytes_written;

    if ((txdata == NULL) || (cfg == NULL) || (pHid == NULL))
    {
        return ATCA_BAD_PARAM;
    }

#ifdef KIT_DEBUG
    printf("HID layer: Write: %s", txdata);
#endif

    bytes_written = hid_write(pHid, txdata, (size_t)cfg->atcahid.packetsize + 1);
    if (bytes_written != cfg->atcahid.packetsize + 1)
    {
        return ATCA_TX_FAIL;
    }

    return ATCA_SUCCESS;
}

/** \brief HAL implementation of send over USB HID
 * \param[in]    iface          instance
 * \param[in]    word_address   determine device transaction type
 * \param[in]    rxdata         pointer to space to receive the data
 * \param[in,out] rxsize        ptr to expected number of receive bytes to request
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_hid_receive(ATCAIface iface, uint8_t word_address, uint8_t* rxdata, uint16_t* rxsize)
{
    hid_device* pHid = (hid_device*)atgetifacehaldat(iface);

    if ((rxdata == NULL) || (rxsize == NULL) || (pHid == NULL))
    {
        return ATCA_BAD_PARAM;
    }

    *rxsize = (uint16_t)hid_read(pHid, rxdata, (size_t)*rxsize);
    if (*rxsize == -1)
    {
        return ATCA_RX_FAIL;
    }

#ifdef KIT_DEBUG
    printf("HID layer: Read: %s", rxdata);
#endif // KIT_DEBUG

    return ATCA_SUCCESS;
}

/** \brief Perform control operations for the kit protocol
 * \param[in]     iface          Interface to interact with.
 * \param[in]     option         Control parameter identifier
 * \param[in]     param          Optional pointer to parameter value
 * \param[in]     paramlen       Length of the parameter
 * \return ATCA_SUCCESS on success, otherwise an error code.
 */
ATCA_STATUS hal_kit_hid_control(ATCAIface iface, uint8_t option, void* param, size_t paramlen)
{
    (void)param;
    (void)paramlen;

    if (iface && iface->mIfaceCFG)
    {
        return ATCA_UNIMPLEMENTED;
    }
    return ATCA_BAD_PARAM;
}


/** \brief Close the physical port for HID
 * \param[in] hal_data  The hardware abstraction data specific to this HAL
 * \return ATCA_STATUS
 */
ATCA_STATUS hal_kit_hid_release(void* hal_data)
{
    hid_device* pHid = (hid_device*)hal_data;
    int i = 0;

    if (pHid == NULL)
    {
        return ATCA_BAD_PARAM;
    }

    hid_close(pHid);

    return ATCA_SUCCESS;
}

/** @} */
