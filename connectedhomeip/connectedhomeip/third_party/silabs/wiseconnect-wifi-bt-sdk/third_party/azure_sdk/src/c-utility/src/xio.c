// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/xlogging.h"

static const char* CONCRETE_OPTIONS = "concreteOptions";

typedef struct XIO_INSTANCE_TAG
{
    const IO_INTERFACE_DESCRIPTION* io_interface_description;
    CONCRETE_IO_HANDLE concrete_xio_handle;
} XIO_INSTANCE;

XIO_HANDLE xio_create(const IO_INTERFACE_DESCRIPTION* io_interface_description, const void* xio_create_parameters)
{
    XIO_INSTANCE* xio_instance;
    /* Codes_SRS_XIO_01_003: [If the argument io_interface_description is NULL, xio_create shall return NULL.] */
    if ((io_interface_description == NULL) ||
        /* Codes_SRS_XIO_01_004: [If any io_interface_description member is NULL, xio_create shall return NULL.] */
        (io_interface_description->concrete_io_retrieveoptions == NULL) ||
        (io_interface_description->concrete_io_create == NULL) ||
        (io_interface_description->concrete_io_destroy == NULL) ||
        (io_interface_description->concrete_io_open == NULL) ||
        (io_interface_description->concrete_io_close == NULL) ||
        (io_interface_description->concrete_io_send == NULL) ||
        (io_interface_description->concrete_io_dowork == NULL) ||
        (io_interface_description->concrete_io_setoption == NULL))
    {
        xio_instance = NULL;
    }
    else
    {
        xio_instance = (XIO_INSTANCE*)malloc(sizeof(XIO_INSTANCE));

        /* Codes_SRS_XIO_01_017: [If allocating the memory needed for the IO interface fails then xio_create shall return NULL.] */
        if (xio_instance != NULL)
        {
            /* Codes_SRS_XIO_01_001: [xio_create shall return on success a non-NULL handle to a new IO interface.] */
            xio_instance->io_interface_description = io_interface_description;

            /* Codes_SRS_XIO_01_002: [In order to instantiate the concrete IO implementation the function concrete_io_create from the io_interface_description shall be called, passing the xio_create_parameters argument.] */
            xio_instance->concrete_xio_handle = xio_instance->io_interface_description->concrete_io_create((void*)xio_create_parameters);

            /* Codes_SRS_XIO_01_016: [If the underlying concrete_io_create call fails, xio_create shall return NULL.] */
            if (xio_instance->concrete_xio_handle == NULL)
            {
                free(xio_instance);
                xio_instance = NULL;
            }
        }
    }
    return (XIO_HANDLE)xio_instance;
}

void xio_destroy(XIO_HANDLE xio)
{
    /* Codes_SRS_XIO_01_007: [If the argument io is NULL, xio_destroy shall do nothing.] */
    if (xio != NULL)
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        /* Codes_SRS_XIO_01_006: [xio_destroy shall also call the concrete_io_destroy function that is member of the io_interface_description argument passed to xio_create, while passing as argument to concrete_io_destroy the result of the underlying concrete_io_create handle that was called as part of the xio_create call.] */
        xio_instance->io_interface_description->concrete_io_destroy(xio_instance->concrete_xio_handle);

        /* Codes_SRS_XIO_01_005: [xio_destroy shall free all resources associated with the IO handle.] */
        free(xio_instance);
    }
}

int xio_open(XIO_HANDLE xio, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    if (xio == NULL)
    {
        /* Codes_SRS_XIO_01_021: [If handle is NULL, xio_open shall return a non-zero value.] */
        result = MU_FAILURE;
    }
    else
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        /* Codes_SRS_XIO_01_019: [xio_open shall call the specific concrete_xio_open function specified in xio_create, passing callback function and context arguments for three events: open completed, bytes received, and IO error.] */
        if (xio_instance->io_interface_description->concrete_io_open(xio_instance->concrete_xio_handle, on_io_open_complete, on_io_open_complete_context, on_bytes_received, on_bytes_received_context, on_io_error, on_io_error_context) != 0)
        {
            /* Codes_SRS_XIO_01_022: [If the underlying concrete_io_open fails, xio_open shall return a non-zero value.] */
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_XIO_01_020: [On success, xio_open shall return 0.] */
            result = 0;
        }
    }

    return result;
}

int xio_close(XIO_HANDLE xio, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    if (xio == NULL)
    {
        /* Codes_SRS_XIO_01_025: [If handle is NULL, xio_close shall return a non-zero value.] */
        result = MU_FAILURE;
    }
    else
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        /* Codes_SRS_XIO_01_023: [xio_close shall call the specific concrete_io_close function specified in xio_create.] */
        if (xio_instance->io_interface_description->concrete_io_close(xio_instance->concrete_xio_handle, on_io_close_complete, callback_context) != 0)
        {
            /* Codes_SRS_XIO_01_026: [If the underlying concrete_io_close fails, xio_close shall return a non-zero value.] */
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_XIO_01_024: [On success, xio_close shall return 0.] */
            result = 0;
        }
    }

    return result;
}

int xio_send(XIO_HANDLE xio, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    /* Codes_SRS_XIO_01_011: [No error check shall be performed on buffer and size.] */
    /* Codes_SRS_XIO_01_010: [If handle is NULL, xio_send shall return a non-zero value.] */
    if (xio == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        /* Codes_SRS_XIO_01_008: [xio_send shall pass the sequence of bytes pointed to by buffer to the concrete IO implementation specified in xio_create, by calling the concrete_io_send function while passing down the buffer and size arguments to it.] */
        /* Codes_SRS_XIO_01_009: [On success, xio_send shall return 0.] */
        /* Codes_SRS_XIO_01_015: [If the underlying concrete_io_send fails, xio_send shall return a non-zero value.] */
        /* Codes_SRS_XIO_01_027: [xio_send shall pass to the concrete_io_send function the on_send_complete and callback_context arguments.] */
        result = xio_instance->io_interface_description->concrete_io_send(xio_instance->concrete_xio_handle, buffer, size, on_send_complete, callback_context);
    }

    return result;
}

void xio_dowork(XIO_HANDLE xio)
{
    /* Codes_SRS_XIO_01_018: [When the handle argument is NULL, xio_dowork shall do nothing.] */
    if (xio != NULL)
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        /* Codes_SRS_XIO_01_012: [xio_dowork shall call the concrete XIO implementation specified in xio_create, by calling the concrete_io_dowork function.] */
        xio_instance->io_interface_description->concrete_io_dowork(xio_instance->concrete_xio_handle);
    }
}

int xio_setoption(XIO_HANDLE xio, const char* optionName, const void* value)
{
    int result;

    /* Codes_SRS_XIO_03_030: [If the xio argument or the optionName argument is NULL, xio_setoption shall return a non-zero value.] */
    if (xio == NULL || optionName == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;

        if (strcmp(CONCRETE_OPTIONS, optionName) == 0)
        {
            /*then value is a pointer to OPTIONHANDLER_HANDLE*/
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, xio_instance->concrete_xio_handle) != OPTIONHANDLER_OK)
            {
                LogError("unable to OptionHandler_FeedOptions");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        else /*passthrough*/
        {
            /* Codes_SRS_XIO_003_028: [xio_setoption shall pass the optionName and value to the concrete IO implementation specified in xio_create by invoking the concrete_xio_setoption function.] */
            /* Codes_SRS_XIO_03_029: [xio_setoption shall return 0 upon success.] */
            /* Codes_SRS_XIO_03_031: [If the underlying concrete_xio_setoption fails, xio_setOption shall return a non-zero value.] */
            result = xio_instance->io_interface_description->concrete_io_setoption(xio_instance->concrete_xio_handle, optionName, value);
        }
    }

    return result;
}

static void* xio_CloneOption(const char* name, const void* value)
{
    void *result;
    if (
        (name == NULL) ||
        (value == NULL)
        )
    {
        LogError("invalid argument detected: const char* name=%p, const void* value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, CONCRETE_OPTIONS) == 0)
        {
            result = (void*)value;
        }
        else
        {
            LogError("unknown option: %s", name);
            result = NULL;
        }
    }
    return result;
}


static void xio_DestroyOption(const char* name, const void* value)
{
    if (
        (name == NULL) ||
        (value == NULL)
        )
    {
        LogError("invalid argument detected: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (strcmp(name, CONCRETE_OPTIONS) == 0)
        {
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
        else
        {
            LogError("unknown option: %s", name);
        }
    }
}

OPTIONHANDLER_HANDLE xio_retrieveoptions(XIO_HANDLE xio)
{
    OPTIONHANDLER_HANDLE result;

    if (xio == NULL)
    {
        LogError("invalid argument detected: XIO_HANDLE xio=%p", xio);
        result = NULL;
    }
    else
    {
        XIO_INSTANCE* xio_instance = (XIO_INSTANCE*)xio;
        /*xio_retrieveoptions shall return a OPTIONHANDLER_HANDLE that has 1 option called "underlyingOptions" which is of type OPTIONHANDLER_HANDLE*/
        result = OptionHandler_Create(xio_CloneOption, xio_DestroyOption, (pfSetOption)xio_setoption);
        if (result == NULL)
        {
            LogError("unable to OptionHandler_Create");
            /*return as is*/
        }
        else
        {
            OPTIONHANDLER_HANDLE concreteOptions = xio_instance->io_interface_description->concrete_io_retrieveoptions(xio_instance->concrete_xio_handle);
            if (concreteOptions == NULL)
            {
                LogError("unable to concrete_io_retrieveoptions");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else
            {
                if (OptionHandler_AddOption(result, CONCRETE_OPTIONS, concreteOptions) != OPTIONHANDLER_OK)
                {
                    LogError("unable to OptionHandler_AddOption");
                    OptionHandler_Destroy(concreteOptions);
                    OptionHandler_Destroy(result);
                    result = NULL;
                }
                else
                {
                    /*all is fine*/
                }
            }
        }
    }

    return result;
}

