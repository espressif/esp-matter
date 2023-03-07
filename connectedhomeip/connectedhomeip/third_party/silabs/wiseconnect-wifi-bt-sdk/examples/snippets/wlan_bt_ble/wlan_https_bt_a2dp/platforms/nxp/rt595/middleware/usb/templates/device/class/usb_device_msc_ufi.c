/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017, 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"

#if USB_DEVICE_CONFIG_MSC
#include "usb_device_msc_ufi.h"
#include "usb_device_msc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Thirteen possible case check.
 *
 * This function handle the thirteen possible cases of host expectations and device intent in the absence of
 *overriding error conditions.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success. more information about return value ,refer to
 *USB_DeviceMscLbaTransfer and USB_DeviceRecvRequest
 */

usb_status_t USB_DeviceMscUfiThirteenCasesCheck(usb_device_msc_struct_t *mscHandle)
{
    usb_status_t error = kStatus_USB_Success;
    usb_device_msc_ufi_struct_t *ufi;
    usb_device_msc_thirteen_case_struct_t *mscCheckEvent;

    mscCheckEvent = (usb_device_msc_thirteen_case_struct_t *)&mscHandle->mscUfi.thirteenCase;
    ufi = &mscHandle->mscUfi;
    /* The following code describe the thirteen possible cases of host
        expectations and device intent in absence of overriding error conditions ,refer to bulk-only spec chapter 6.7
       The Thirteen Cases*/
    if (mscCheckEvent->hostExpectedDataLength == 0)
    {
        /*Host expects no data transfers*/
        mscHandle->mscCsw->dataResidue = 0;
        if (mscCheckEvent->deviceExpectedDataLength == 0)
        { /*case 1, Device intends to transfer no data*/
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
        }
        else
        { /*case 2 ,Device intends to send data to the host; */
            /*case 3, Device intends to receive data from the host*/
            /*set csw status to 02h*/
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_PHASE_ERROR;
        }
    }
    else if (mscCheckEvent->hostExpectedDirection)
    {
        /*Host expects to receive data from the device*/
        if (mscCheckEvent->deviceExpectedDataLength == 0)
        {
            /*case 4, Device intends to transfer no data*/
            mscHandle->mscCsw->dataResidue =
                mscCheckEvent->hostExpectedDataLength - mscCheckEvent->deviceExpectedDataLength;
            error = USB_DeviceSendRequest(mscHandle->handle, mscHandle->bulkInEndpoint, mscCheckEvent->buffer,
                                          mscCheckEvent->deviceExpectedDataLength);

            if (kStatus_USB_Success == error)
            {
                mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
            }
            else
            {
                mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
                ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_UNRECOVERED_READ_ERROR;
            }
            error = kStatus_USB_InvalidRequest;
        }
        else if (mscCheckEvent->deviceExpectedDirection)
        {
            if (mscCheckEvent->hostExpectedDataLength > mscCheckEvent->deviceExpectedDataLength)
            {
                /*case 5, device intends to send less data than the host indicated*/
                mscHandle->mscCsw->dataResidue =
                    mscCheckEvent->hostExpectedDataLength - mscCheckEvent->deviceExpectedDataLength;

                if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                {
                    error = USB_DeviceMscLbaTransfer(mscHandle, USB_IN, &mscCheckEvent->lbaInformation);
                }
                else
                {
                    error = USB_DeviceSendRequest(mscHandle->handle, mscHandle->bulkInEndpoint, mscCheckEvent->buffer,
                                                  mscCheckEvent->deviceExpectedDataLength);
                }

                if (kStatus_USB_Success == error)
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
                }
                else
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
                    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_UNRECOVERED_READ_ERROR;
                }
                error = kStatus_USB_InvalidRequest;
            }
            else if (mscCheckEvent->hostExpectedDataLength == mscCheckEvent->deviceExpectedDataLength)
            { /*case 6*,device intends to send dCBWDataTransferLength excepted by the host*/
                mscHandle->mscCsw->dataResidue = 0;
                if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                {
                    error = USB_DeviceMscLbaTransfer(mscHandle, USB_IN, &mscCheckEvent->lbaInformation);
                }
                else
                {
                    error = USB_DeviceSendRequest(mscHandle->handle, mscHandle->bulkInEndpoint, mscCheckEvent->buffer,
                                                  mscCheckEvent->deviceExpectedDataLength);
                }

                if (kStatus_USB_Success == error)
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
                }
                else
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
                    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_UNRECOVERED_READ_ERROR;
                }
            }
            else
            { /*case 7, device intends to send more data than the host indicated*/
                mscHandle->mscCsw->dataResidue = 0;

                if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                {
                    mscCheckEvent->lbaInformation.transferNumber =
                        mscCheckEvent->hostExpectedDataLength / mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba;
                    mscHandle->mscCsw->dataResidue =
                        mscCheckEvent->hostExpectedDataLength -
                        mscCheckEvent->lbaInformation.transferNumber * mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba;
                    error = USB_DeviceMscLbaTransfer(mscHandle, USB_IN, &mscCheckEvent->lbaInformation);
                }
                else
                {
                    error = USB_DeviceSendRequest(mscHandle->handle, mscHandle->bulkInEndpoint, mscCheckEvent->buffer,
                                                  mscCheckEvent->hostExpectedDataLength);
                }

                if (kStatus_USB_Success == error)
                {
                    if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                    {
                        mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_PHASE_ERROR;
                    }
                    else
                    {
                        mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
                    }
                }
                else
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
                    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_UNRECOVERED_READ_ERROR;
                }
            }
        }
        else
        {
            /*case 8, device intends to receive data from the host*/
            mscHandle->mscCsw->dataResidue = mscCheckEvent->hostExpectedDataLength;
            error = USB_DeviceSendRequest(mscHandle->handle, mscHandle->bulkInEndpoint, mscCheckEvent->buffer, 0);
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_PHASE_ERROR;
            error = kStatus_USB_InvalidRequest;
        }
    }
    else
    {
        /*Host expects to send data to the device*/
        if (0 == mscCheckEvent->deviceExpectedDataLength)
        { /*case 9,Device intends to transfer no data*/
            USB_DeviceStallEndpoint(mscHandle->handle, mscHandle->bulkOutEndpoint);
            mscHandle->mscCsw->dataResidue = mscCheckEvent->hostExpectedDataLength;
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
            mscHandle->outEndpointStallFlag = 1;
            error = kStatus_USB_InvalidRequest;
        }
        else if (mscCheckEvent->deviceExpectedDirection)
        { /*case 10,Device intends to send data to the host*/
            USB_DeviceStallEndpoint(mscHandle->handle, mscHandle->bulkOutEndpoint);
            mscHandle->mscCsw->dataResidue = mscCheckEvent->hostExpectedDataLength;
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_PHASE_ERROR;
            mscHandle->outEndpointStallFlag = 1;
            error = kStatus_USB_InvalidRequest;
        }
        else
        {
            if (mscCheckEvent->hostExpectedDataLength > mscCheckEvent->deviceExpectedDataLength)
            { /*case 11, device intends to process less than the amount of data that the host indicated*/
                mscHandle->mscCsw->dataResidue =
                    mscCheckEvent->hostExpectedDataLength - mscCheckEvent->deviceExpectedDataLength;

                if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                {
                    error = USB_DeviceMscLbaTransfer(mscHandle, USB_OUT, &mscCheckEvent->lbaInformation);
                }
                else
                {
                    error = USB_DeviceRecvRequest(mscHandle->handle, mscHandle->bulkOutEndpoint, mscCheckEvent->buffer,
                                                  mscCheckEvent->deviceExpectedDataLength);
                }

                if (kStatus_USB_Success == error)
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
                }
                else
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
                    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_WRITE_FAULT;
                }
                error = kStatus_USB_InvalidRequest;
            }
            else if (mscCheckEvent->hostExpectedDataLength == mscCheckEvent->deviceExpectedDataLength)
            { /*case 12,device intends to process equal to the amount of data that the host indicated*/
                mscHandle->mscCsw->dataResidue = 0;
                if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                {
                    error = USB_DeviceMscLbaTransfer(mscHandle, USB_OUT, &mscCheckEvent->lbaInformation);
                }
                else
                {
                    error = USB_DeviceRecvRequest(mscHandle->handle, mscHandle->bulkOutEndpoint, mscCheckEvent->buffer,
                                                  mscCheckEvent->deviceExpectedDataLength);
                }
                if (kStatus_USB_Success == error)
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
                }
                else
                {
                    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
                    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_WRITE_FAULT;
                }
            }
            else
            { /*case 13,device  intends to process more data than the host indicated*/
                mscHandle->mscCsw->dataResidue = 0;
                if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                {
                    mscCheckEvent->lbaInformation.transferNumber =
                        mscCheckEvent->hostExpectedDataLength / mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba;
                    mscHandle->mscCsw->dataResidue =
                        mscCheckEvent->hostExpectedDataLength -
                        mscCheckEvent->lbaInformation.transferNumber * mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba;
                    error = USB_DeviceMscLbaTransfer(mscHandle, USB_OUT, &mscCheckEvent->lbaInformation);
                }
                else
                {
                    error = USB_DeviceRecvRequest(mscHandle->handle, mscHandle->bulkOutEndpoint, mscCheckEvent->buffer,
                                                  mscCheckEvent->hostExpectedDataLength);
                }

                if (kStatus_USB_Success == error)
                {
                    if (ufi->thirteenCase.lbaSendRecvSelect == 1)
                    {
                        mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_PHASE_ERROR;
                    }
                    else
                    {
                        mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
                    }
                }
                else
                {
                    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_MEDIUM_ERROR;
                    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_WRITE_FAULT;
                }
            }
        }
    }
    return error;
}

/*!
 * @brief request sense command.
 *
 * The REQUEST SENSE command instructs the UFI device to transfer sense data to the host for the specified  logical
 *unit.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
 */
usb_status_t USB_DeviceMscUfiRequestSenseCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;

    ufi = &mscHandle->mscUfi;
    ufi->thirteenCase.deviceExpectedDataLength = USB_DEVICE_MSC_UFI_REQ_SENSE_DATA_LENGTH;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = (uint8_t *)ufi->requestSense;
    ufi->thirteenCase.lbaSendRecvSelect = 0;
    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    return error;
}

/*!
 * @brief inquiry command.
 *
 * The INQUIRY command requests that information regarding parameters of the UFI device itself be sent to the host.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiInquiryCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];
    temp.size = 0U;
    temp.buffer = NULL;

    if (mscHandle->configurationStruct->classCallback != NULL)
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventInquiry,
                                                      (void *)&temp);
    }
    ufi->thirteenCase.deviceExpectedDataLength = temp.size;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = temp.buffer;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    return error;
}

/*!
 * @brief read command.
 *
 * The READ(10),READ(12) command requests that the UFI device transfer data to the host.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiReadCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    uint32_t logicalBlockAddress = 0;
    uint32_t lbaTransferLength = 0;

    ufi = &mscHandle->mscUfi;

    logicalBlockAddress = ((uint32_t)mscHandle->mscCbw->cbwcb[2] << 24);
    logicalBlockAddress |= ((uint32_t)mscHandle->mscCbw->cbwcb[3] << 16);
    logicalBlockAddress |= ((uint32_t)mscHandle->mscCbw->cbwcb[4] << 8);
    logicalBlockAddress |= ((uint32_t)mscHandle->mscCbw->cbwcb[5]);

    if (mscHandle->mscCbw->cbwcb[0] == USB_DEVICE_MSC_READ_10_COMMAND)
    {
        lbaTransferLength = (uint16_t)((uint16_t)mscHandle->mscCbw->cbwcb[7] << 8);
        lbaTransferLength |= (uint16_t)mscHandle->mscCbw->cbwcb[8];
    }
    else if (mscHandle->mscCbw->cbwcb[0] == USB_DEVICE_MSC_READ_12_COMMAND)
    {
        lbaTransferLength = ((uint32_t)mscHandle->mscCbw->cbwcb[6] << 24);
        lbaTransferLength |= ((uint32_t)mscHandle->mscCbw->cbwcb[7] << 16);
        lbaTransferLength |= ((uint32_t)mscHandle->mscCbw->cbwcb[8] << 8);
        lbaTransferLength |= ((uint32_t)mscHandle->mscCbw->cbwcb[9]);
    }
    else
    {
    }

    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.deviceExpectedDataLength = mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba * lbaTransferLength;
    ufi->thirteenCase.buffer = NULL;

    ufi->thirteenCase.lbaSendRecvSelect = 1;
    ufi->thirteenCase.lbaInformation.startingLogicalBlockAddress = logicalBlockAddress;
    ufi->thirteenCase.lbaInformation.transferNumber = lbaTransferLength;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    return error;
}

/*!
 * @brief write command.
 *
 * The WRITE(10),WRITE(12) command requests that the UFI device write the data transferred by the host to the medium.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiWriteCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    uint32_t logicalBlockAddress = 0;
    uint32_t lbaTransferLength = 0;

    ufi = &mscHandle->mscUfi;

    logicalBlockAddress = ((uint32_t)mscHandle->mscCbw->cbwcb[2] << 24);
    logicalBlockAddress |= ((uint32_t)mscHandle->mscCbw->cbwcb[3] << 16);
    logicalBlockAddress |= ((uint32_t)mscHandle->mscCbw->cbwcb[4] << 8);
    logicalBlockAddress |= ((uint32_t)mscHandle->mscCbw->cbwcb[5]);

    if (mscHandle->mscCbw->cbwcb[0] == USB_DEVICE_MSC_WRITE_10_COMMAND)
    {
        lbaTransferLength = (uint16_t)((uint16_t)mscHandle->mscCbw->cbwcb[7] << 8);
        lbaTransferLength |= (uint16_t)mscHandle->mscCbw->cbwcb[8];
    }
    else if (mscHandle->mscCbw->cbwcb[0] == USB_DEVICE_MSC_WRITE_12_COMMAND)
    {
        lbaTransferLength = ((uint32_t)mscHandle->mscCbw->cbwcb[6] << 24);
        lbaTransferLength |= ((uint32_t)mscHandle->mscCbw->cbwcb[7] << 16);
        lbaTransferLength |= ((uint32_t)mscHandle->mscCbw->cbwcb[8] << 8);
        lbaTransferLength |= ((uint32_t)mscHandle->mscCbw->cbwcb[9]);
    }
    else
    {
    }
    ufi->thirteenCase.deviceExpectedDirection = USB_OUT;
    ufi->thirteenCase.deviceExpectedDataLength = mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba * lbaTransferLength;
    ufi->thirteenCase.buffer = NULL;

    ufi->thirteenCase.lbaSendRecvSelect = 1;
    ufi->thirteenCase.lbaInformation.startingLogicalBlockAddress = logicalBlockAddress;
    ufi->thirteenCase.lbaInformation.transferNumber = lbaTransferLength;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    return error;
}

/*!
 * @brief test unit ready command.
 *
 * The TEST UNIT READY command provides a means to check if the UFI device is ready.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiTestUnitReadyCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = NULL;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    if (mscHandle->configurationStruct->classCallback != NULL)
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventTestUnitReady,
                                                      (void *)&temp);
    }
    return error;
}

/*!
 * @brief verify command.
 *
 * The VERIFY command requests that the UFI device verify the data on the medium.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiVerifyCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;

    ufi = &mscHandle->mscUfi;

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = NULL;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    return error;
}

/*!
 * @brief mode sense command.
 *
 * The MODE SENSE command allows the UFI device to report medium or device parameters to the host.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiModeSenseCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;

    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];
    temp.size = 0U;
    temp.buffer = NULL;

    if (mscHandle->configurationStruct->classCallback != NULL)
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventModeSense,
                                                      (void *)&temp);
    }
    ufi->thirteenCase.deviceExpectedDataLength = temp.size;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = temp.buffer;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    return error;
}

/*!
 * @brief mode select command.
 *
 * The MODE SELECT command allows the host to specify medium or device parameters to the UFI device.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiModeSelectCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error = kStatus_USB_TransferFailed;

    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];
    temp.buffer = NULL;
    temp.size = 0;
    if (mscHandle->configurationStruct->classCallback != NULL)
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventModeSelect,
                                                      (void *)&temp);
    }

    ufi->thirteenCase.deviceExpectedDataLength = temp.size;
    ufi->thirteenCase.deviceExpectedDirection = USB_OUT;
    ufi->thirteenCase.buffer = temp.buffer;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    if (mscHandle->mscCbw->cbwcb[1] & 0x01)
    {
        ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_ILLEGAL_REQUEST;
        ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_INVALID_FIELD_IN_COMMAND_PKT;
    }
    return error;
}

/*!
 * @brief read capacity command.
 *
 * The READ CAPACITIY command allows the host to request capacities of the currently installed medium.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiReadCapacityCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error = kStatus_USB_TransferFailed;

    ufi = &mscHandle->mscUfi;

    if (mscHandle->mscCbw->cbwcb[0] == USB_DEVICE_MSC_READ_CAPACITY_10_COMMAND)
    {
        ufi->thirteenCase.deviceExpectedDataLength = USB_DEVICE_MSC_UFI_READ_CAPACITY_DATA_LENGTH;
        ufi->readCapacity->lastLogicalBlockAddress = USB_LONG_TO_BIG_ENDIAN(mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].totalLbaNumberSupports - 1);
        ufi->readCapacity->blockSize = USB_LONG_TO_BIG_ENDIAN((uint32_t)mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba);
        ufi->thirteenCase.buffer = (uint8_t *)(ufi->readCapacity);
    }
    else
    {
        ufi->thirteenCase.deviceExpectedDataLength = USB_DEVICE_MSC_UFI_READ_CAPACITY16_DATA_LENGTH;
        ufi->readCapacity16->lastLogicalBlockAddress1 = USB_LONG_TO_BIG_ENDIAN(mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].totalLbaNumberSupports - 1);
        ufi->readCapacity16->blockSize = USB_LONG_TO_BIG_ENDIAN((uint32_t)mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba);
        ufi->thirteenCase.buffer = (uint8_t *)(ufi->readCapacity16);
    }
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    return error;
}

/*!
 * @brief read format capacity command.
 *
 * The READ FORMAT CAPACITIES command allows the host to request a list of the possible capacities that
* can be formatted on the currently installed medium.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t   USB_DeviceMscUfiReadFormatCapacityCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error = kStatus_USB_TransferFailed;
    usb_device_current_max_capacity_descriptor_struct_t current_max_head;
    usb_device_formattable_capacity_descriptor_struct_t formattable_capacity_head;
    usb_device_capacity_list_header_struct_t capacityListHead = {{0x00, 0x00, 0x00}, 0x00};
    uint32_t response_size;
    uint16_t allocation_length;
    uint8_t num_formattable_cap_desc;
    uint8_t descriptor_code;
    uint8_t count = 0;
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t *ptr;

    ufi = &mscHandle->mscUfi;
    allocation_length = (uint16_t)((uint8_t)(mscHandle->mscCbw->cbwcb[7] << 8) | mscHandle->mscCbw->cbwcb[8]);
    /*reference ufi command spec table-33 Descriptor Code definition*/
    num_formattable_cap_desc = (uint8_t)(ufi->formattedDisk ? (mscHandle->implementingDiskDrive ? 0x02 : 0x03) : 0x00);

    formattable_capacity_head.blockNumber = USB_LONG_TO_BIG_ENDIAN(mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].totalLbaNumberSupports);
    formattable_capacity_head.blockLength = USB_LONG_TO_BIG_ENDIAN(mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba);

    descriptor_code =
        (uint8_t)(ufi->formattedDisk ? USB_DEVICE_MSC_UFI_FORMATTED_MEDIA : USB_DEVICE_MSC_UFI_UNFORMATTED_MEDIA);
    capacityListHead.capacityListLength = num_formattable_cap_desc * 8;
    current_max_head.blockNumber = USB_LONG_TO_BIG_ENDIAN(mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].totalLbaNumberSupports);
    current_max_head.descriptorCodeBlockLength =
        USB_LONG_TO_BIG_ENDIAN(((uint8_t)(descriptor_code << 24) | mscHandle->luInformations[mscHandle->mscCbw->logicalUnitNumber].lengthOfEachLba));

    response_size = sizeof(usb_device_capacity_list_header_struct_t) +
                    sizeof(usb_device_current_max_capacity_descriptor_struct_t) +
                    sizeof(usb_device_formattable_capacity_descriptor_struct_t) * num_formattable_cap_desc;

    if (response_size > allocation_length)
    {
        response_size = allocation_length;
    }

    ptr = (uint8_t *)&capacityListHead;
    for (count = 0; count < sizeof(capacityListHead); count++)
    {
        ufi->formatCapacityData[count] = ptr[i++];
    }
    ptr = (uint8_t *)&current_max_head;
    i = 0;
    for (; i < sizeof(current_max_head); count++)
    {
        ufi->formatCapacityData[count] = ptr[i++];
    }

    if (ufi->formattedDisk)
    {
        for (i = 0; i < num_formattable_cap_desc; i++)
        {
            ptr = (uint8_t *)&formattable_capacity_head;

            for (; count < sizeof(formattable_capacity_head); count++)
            {
                ufi->formatCapacityData[count] = ptr[j++];
            }
        }
    }

    ufi->thirteenCase.deviceExpectedDataLength = response_size;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = (uint8_t *)ufi->formatCapacityData;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    return error;
}

/*!
 * @brief format unit command.
 *
 * The Host sends the FORMAT UNIT command to physically format one track of a diskette according to the selected
 *options.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiFormatUnitCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;

    ufi = &mscHandle->mscUfi;

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = NULL;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    if (mscHandle->mscCsw->cswStatus != USB_DEVICE_MSC_PHASE_ERROR)
    {
        if ((mscHandle->mscCbw->cbwcb[1] & 0x1f) == 0x17)
        {
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_PASSED;
        }
        else
        {
            mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;
            ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_ILLEGAL_REQUEST;
            ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_INVALID_FIELD_IN_COMMAND_PKT;
        }
    }
    return error;
}

/*!
 * @brief prevent allow medium command.
 *
 * This command tells the UFI device to enable or disable the removal of the medium in the logical unit.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiPreventAllowMediumCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = NULL;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    if (mscHandle->configurationStruct->classCallback != NULL)
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventRemovalRequest,
                                                      (void *)&temp);
    }

    return error;
}

/*!
 * @brief send diagnostic command.
 *
 * The SEND DIAGNOSTIC command requests the UFI device to do a reset or perform a self-test.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiSendDiagnosticCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = NULL;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    if (mscHandle->configurationStruct->classCallback != NULL)
    {
        /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
        it is from the second parameter of classInit */
        mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventSendDiagnostic,
                                                      (void *)&temp);
    }
    return error;
}

/*!
 * @brief start stop unit command.
 *
 * The START-STOP UNIT command instructs the UFI device to enable or disable media access operations.
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiStartStopUnitCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    usb_device_ufi_app_struct_t temp;

    ufi = &mscHandle->mscUfi;
    temp.requestSense = ufi->requestSense;
    temp.cbwcb = &mscHandle->mscCbw->cbwcb[0];

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->thirteenCase.deviceExpectedDirection = USB_IN;
    ufi->thirteenCase.buffer = NULL;
    ufi->thirteenCase.lbaSendRecvSelect = 0;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);

    if (mscHandle->mscCsw->cswStatus != USB_DEVICE_MSC_PHASE_ERROR)
    {
        if (mscHandle->configurationStruct->classCallback != NULL)
        {
            /* classCallback is initialized in classInit of s_UsbDeviceClassInterfaceMap,
            it is from the second parameter of classInit */
            mscHandle->configurationStruct->classCallback((class_handle_t)mscHandle, kUSB_DeviceMscEventStopEjectMedia,
                                                          (void *)&temp);
        }
    }

    return error;
}

/*!
 * @brief  un-support command.
 *
 * Handle unsupported command .
 *
 * @param handle          The device msc class handle.
 *
 *@return A USB error code or kStatus_USB_Success.
*/
usb_status_t USB_DeviceMscUfiUnsupportCommand(usb_device_msc_struct_t *mscHandle)
{
    usb_device_msc_ufi_struct_t *ufi = NULL;
    usb_status_t error;
    ufi = &mscHandle->mscUfi;

    mscHandle->mscCsw->dataResidue = 0;
    mscHandle->mscCsw->cswStatus = USB_DEVICE_MSC_COMMAND_FAILED;

    ufi->thirteenCase.deviceExpectedDataLength = 0;
    ufi->requestSense->senseKey = USB_DEVICE_MSC_UFI_ILLEGAL_REQUEST;
    ufi->requestSense->additionalSenseCode = USB_DEVICE_MSC_UFI_INVALID_COMMAND_OPCODE;
    ufi->requestSense->additionalSenseQualifer = USB_DEVICE_MSC_UFI_NO_SENSE;

    error = USB_DeviceMscUfiThirteenCasesCheck(mscHandle);
    
    return error;
}

#endif
