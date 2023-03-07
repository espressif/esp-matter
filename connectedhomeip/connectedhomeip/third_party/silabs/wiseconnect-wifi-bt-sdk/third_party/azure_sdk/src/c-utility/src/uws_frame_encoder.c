 // Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/gb_rand.h"
#include "azure_c_shared_utility/uws_frame_encoder.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/uniqueid.h"

BUFFER_HANDLE uws_frame_encoder_encode(WS_FRAME_TYPE opcode, const unsigned char* payload, size_t length, bool is_masked, bool is_final, unsigned char reserved)
{
    BUFFER_HANDLE result;

    if (reserved > 7)
    {
        /* Codes_SRS_UWS_FRAME_ENCODER_01_052: [ If reserved has any bits set except the lowest 3 then uws_frame_encoder_encode shall fail and return NULL. ]*/
        LogError("Bad reserved value: 0x%02x", reserved);
        result = NULL;
    }
    else if (opcode > 0x0F)
    {
        /* Codes_SRS_UWS_FRAME_ENCODER_01_006: [ If an unknown opcode is received, the receiving endpoint MUST _Fail the WebSocket Connection_. ]*/
        LogError("Invalid opcode: 0x%02x", opcode);
        result = NULL;
    }
    else if ((length > 0) &&
        (payload == NULL))
    {
        /* Codes_SRS_UWS_FRAME_ENCODER_01_054: [ If length is greater than 0 and payload is NULL, then uws_frame_encoder_encode shall fail and return NULL. ]*/
        LogError("Invalid arguments: NULL payload and length=%u", (unsigned int)length);
        result = NULL;
    }
    else
    {
        size_t needed_bytes = 2;
        size_t header_bytes;

        /* Codes_SRS_UWS_FRAME_ENCODER_01_044: [ On success uws_frame_encoder_encode shall return a non-NULL handle to the result buffer. ]*/
        /* Codes_SRS_UWS_FRAME_ENCODER_01_048: [ The newly created buffer shall be created by calling BUFFER_new. ]*/
        result = BUFFER_new();
        if (result == NULL)
        {
            /* Codes_SRS_UWS_FRAME_ENCODER_01_049: [ If BUFFER_new fails then uws_frame_encoder_encode shall fail and return NULL. ]*/
            LogError("Cannot create new buffer");
        }
        else
        {
            /* Codes_SRS_UWS_FRAME_ENCODER_01_001: [ uws_frame_encoder_encode shall encode the information given in opcode, payload, length, is_masked, is_final and reserved according to the RFC6455 into a new buffer.]*/
            if (length > 65535)
            {
                needed_bytes += 8;
            }
            else if (length > 125)
            {
                needed_bytes += 2;
            }

            if (is_masked)
            {
                needed_bytes += 4;
            }

            header_bytes = needed_bytes;
            needed_bytes += length;

            /* Codes_SRS_UWS_FRAME_ENCODER_01_046: [ The result buffer shall be resized accordingly using BUFFER_enlarge. ]*/
            if (BUFFER_enlarge(result, needed_bytes) != 0)
            {
                /* Codes_SRS_UWS_FRAME_ENCODER_01_047: [ If BUFFER_enlarge fails then uws_frame_encoder_encode shall fail and return NULL. ]*/
                LogError("Cannot allocate memory for encoded frame");
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_UWS_FRAME_ENCODER_01_050: [ The allocated memory shall be accessed by calling BUFFER_u_char. ]*/
                unsigned char* buffer = BUFFER_u_char(result);
                if (buffer == NULL)
                {
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_051: [ If BUFFER_u_char fails then uws_frame_encoder_encode shall fail and return a NULL. ]*/
                    LogError("Cannot get encoded buffer pointer");
                    BUFFER_delete(result);
                    result = NULL;
                }
                else
                {
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_007: [ *  %x0 denotes a continuation frame ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_008: [ *  %x1 denotes a text frame ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_009: [ *  %x2 denotes a binary frame ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_010: [ *  %x3-7 are reserved for further non-control frames ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_011: [ *  %x8 denotes a connection close ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_012: [ *  %x9 denotes a ping ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_013: [ *  %xA denotes a pong ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_014: [ *  %xB-F are reserved for further control frames ]*/
                    buffer[0] = (unsigned char)opcode;

                    /* Codes_SRS_UWS_FRAME_ENCODER_01_002: [ Indicates that this is the final fragment in a message. ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_003: [ The first fragment MAY also be the final fragment. ]*/
                    if (is_final)
                    {
                        buffer[0] |= 0x80;
                    }

                    /* Codes_SRS_UWS_FRAME_ENCODER_01_004: [ MUST be 0 unless an extension is negotiated that defines meanings for non-zero values. ]*/
                    buffer[0] |= reserved << 4;

                    /* Codes_SRS_UWS_FRAME_ENCODER_01_022: [ Note that in all cases, the minimal number of bytes MUST be used to encode the length, for example, the length of a 124-byte-long string can't be encoded as the sequence 126, 0, 124. ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_018: [ The length of the "Payload data", in bytes: ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_023: [ The payload length is the length of the "Extension data" + the length of the "Application data". ]*/
                    /* Codes_SRS_UWS_FRAME_ENCODER_01_042: [ The payload length, indicated in the framing as frame-payload-length, does NOT include the length of the masking key. ]*/
                    if (length > 65535)
                    {
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_020: [ If 127, the following 8 bytes interpreted as a 64-bit unsigned integer (the most significant bit MUST be 0) are the payload length. ]*/
                        buffer[1] = 127;

                        /* Codes_SRS_UWS_FRAME_ENCODER_01_021: [ Multibyte length quantities are expressed in network byte order. ]*/
                        buffer[2] = (unsigned char)((uint64_t)length >> 56) & 0xFF;
                        buffer[3] = (unsigned char)((uint64_t)length >> 48) & 0xFF;
                        buffer[4] = (unsigned char)((uint64_t)length >> 40) & 0xFF;
                        buffer[5] = (unsigned char)((uint64_t)length >> 32) & 0xFF;
                        buffer[6] = (unsigned char)((uint64_t)length >> 24) & 0xFF;
                        buffer[7] = (unsigned char)((uint64_t)length >> 16) & 0xFF;
                        buffer[8] = (unsigned char)((uint64_t)length >> 8) & 0xFF;
                        buffer[9] = (unsigned char)(length & 0xFF);
                    }
                    else if (length > 125)
                    {
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_019: [ If 126, the following 2 bytes interpreted as a 16-bit unsigned integer are the payload length. ]*/
                        buffer[1] = 126;

                        /* Codes_SRS_UWS_FRAME_ENCODER_01_021: [ Multibyte length quantities are expressed in network byte order. ]*/
                        buffer[2] = (unsigned char)(length >> 8);
                        buffer[3] = (unsigned char)(length & 0xFF);
                    }
                    else
                    {
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_043: [ if 0-125, that is the payload length. ]*/
                        buffer[1] = (unsigned char)length;
                    }

                    if (is_masked)
                    {
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_015: [ Defines whether the "Payload data" is masked. ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_033: [ A masked frame MUST have the field frame-masked set to 1, as defined in Section 5.2. ]*/
                        buffer[1] |= 0x80;

                        /* Codes_SRS_UWS_FRAME_ENCODER_01_053: [ In order to obtain a 32 bit value for masking, gb_rand shall be used 4 times (for each byte). ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_016: [ If set to 1, a masking key is present in masking-key, and this is used to unmask the "Payload data" as per Section 5.3. ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_026: [ This field is present if the mask bit is set to 1 and is absent if the mask bit is set to 0. ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_034: [ The masking key is contained completely within the frame, as defined in Section 5.2 as frame-masking-key. ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_036: [ The masking key is a 32-bit value chosen at random by the client. ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_037: [ When preparing a masked frame, the client MUST pick a fresh masking key from the set of allowed 32-bit values. ]*/
                        /* Codes_SRS_UWS_FRAME_ENCODER_01_038: [ The masking key needs to be unpredictable; thus, the masking key MUST be derived from a strong source of entropy, and the masking key for a given frame MUST NOT make it simple for a server/proxy to predict the masking key for a subsequent frame. ]*/
                        buffer[header_bytes - 4] = (unsigned char)gb_rand();
                        buffer[header_bytes - 3] = (unsigned char)gb_rand();
                        buffer[header_bytes - 2] = (unsigned char)gb_rand();
                        buffer[header_bytes - 1] = (unsigned char)gb_rand();
                    }

                    if (length > 0)
                    {
                        if (is_masked)
                        {
                            size_t i;

                            /* Codes_SRS_UWS_FRAME_ENCODER_01_035: [ It is used to mask the "Payload data" defined in the same section as frame-payload-data, which includes "Extension data" and "Application data". ]*/
                            /* Codes_SRS_UWS_FRAME_ENCODER_01_039: [ To convert masked data into unmasked data, or vice versa, the following algorithm is applied. ]*/
                            /* Codes_SRS_UWS_FRAME_ENCODER_01_040: [ The same algorithm applies regardless of the direction of the translation, e.g., the same steps are applied to mask the data as to unmask the data. ]*/
                            for (i = 0; i < length; i++)
                            {
                                /* Codes_SRS_UWS_FRAME_ENCODER_01_041: [ Octet i of the transformed data ("transformed-octet-i") is the XOR of octet i of the original data ("original-octet-i") with octet at index i modulo 4 of the masking key ("masking-key-octet-j"): ]*/
                                buffer[header_bytes + i] = ((unsigned char*)payload)[i] ^ buffer[header_bytes - 4 + (i % 4)];
                            }
                        }
                        else
                        {
                            (void)memcpy(buffer + header_bytes, payload, length);
                        }
                    }
                }
            }
        }
    }

    return result;
}
