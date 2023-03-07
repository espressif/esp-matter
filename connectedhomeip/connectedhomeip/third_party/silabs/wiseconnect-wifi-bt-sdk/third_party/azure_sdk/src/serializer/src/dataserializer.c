// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/gballoc.h"

#include "dataserializer.h"
#include "azure_c_shared_utility/xlogging.h"

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(DATA_SERIALIZER_RESULT, DATA_SERIALIZER_RESULT_VALUES);

BUFFER_HANDLE DataSerializer_Encode(MULTITREE_HANDLE multiTreeHandle, DATA_SERIALIZER_MULTITREE_TYPE dataType, DATA_SERIALIZER_ENCODE_FUNC encodeFunc)
{
    BUFFER_HANDLE pBuffer;

    /* Codes_SRS_DATA_SERIALIZER_07_003: [NULL shall be returned when an invalid parameter is supplied.] */
    if (multiTreeHandle == NULL || encodeFunc == NULL)
    {
        pBuffer = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(DATA_SERIALIZER_RESULT, DATA_SERIALIZER_INVALID_ARG) );
    }
    else
    {
        /* Codes_SRS_DATA_SERIALIZER_07_009: [DataSerializer_Encode function shall call into the given DATA_SERIALIZER_ENCODE_FUNC callback with a valid BUFFER object and valid MULTITREE_HANDLE object.] */
        pBuffer = encodeFunc(multiTreeHandle, dataType);
        if (pBuffer == NULL)
        {
            /* Codes_SRS_DATA_SERIALIZER_07_010: [Upon a DATA_SERIALIZER_ENCODE_FUNC failure the DataSerializer_Encode function shall return NULL.] */
            LogError("(Error code: %s)", MU_ENUM_TO_STRING(DATA_SERIALIZER_RESULT, DATA_SERIALIZER_ERROR) );
        }
    }
    /* Codes_SRS_DATA_SERIALIZER_07_002: [DataSerializer_Encode shall return a valid BUFFER_HANDLE when the function executes successfully.] */
    return pBuffer;
}

MULTITREE_HANDLE DataSerializer_Decode(BUFFER_HANDLE data, DATA_SERIALIZER_DECODE_FUNC decodeFunc)
{
    MULTITREE_HANDLE multiTreeHandle;

    /* Codes_SRS_DATA_SERIALIZER_07_007: [NULL shall be returned when an invalid parameter is supplied.] */
    if (data == NULL || decodeFunc == NULL)
    {
        multiTreeHandle = NULL;
        LogError("(Error code: %s)", MU_ENUM_TO_STRING(DATA_SERIALIZER_RESULT, DATA_SERIALIZER_INVALID_ARG) );
    }
    else
    {
        /* Codes_SRS_DATA_SERIALIZER_07_012: [DataSerializer_Decode function shall call into the given DATA_SERIALIZER_DECODE_FUNC callback with a valid BUFFER object and valid MULTITREE_HANDLE object.] */
        multiTreeHandle = decodeFunc(data);
        if (multiTreeHandle == NULL)
        {
            /* Codes_SRS_DATA_SERIALIZER_07_013: [Upon a DATA_SERIALIZER_DECODE_FUNC callback failure the DataSerializer_Encode function Shall return NULL.] */
            LogError("(Error code: %s)", MU_ENUM_TO_STRING(DATA_SERIALIZER_RESULT, DATA_SERIALIZER_ERROR) );
        }
    }

    /* Codes_SRS_DATA_SERIALIZER_07_006: [DataSerializer_Decode shall return a valid MULTITREE_HANDLE when the function executes successfully.] */
    return multiTreeHandle;
}
