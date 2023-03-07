/*
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */
/*
 *  ======== UIAPacket.h ========
 */
#ifndef ti_uia_linux_UIAPacket__include
#define ti_uia_linux_UIAPacket__include

//TODO why can't I use Bits32 here...
typedef struct UIAPacket_Hdr {
    unsigned int word1;
    unsigned int word2;
    unsigned int word3;
    unsigned int word4;
} UIAPacket_Hdr;

enum UIAPacket_HdrType {
    UIAPacket_HdrType_Reserved0 = 0,
    UIAPacket_HdrType_Reserved1 = 1,
    UIAPacket_HdrType_Reserved2 = 2,
    UIAPacket_HdrType_Reserved3 = 3,
    UIAPacket_HdrType_Reserved4 = 4,
    UIAPacket_HdrType_Reserved5 = 5,
    UIAPacket_HdrType_Reserved6 = 6,
    UIAPacket_HdrType_Reserved7 = 7,
    UIAPacket_HdrType_ChannelizedData = 8,
    UIAPacket_HdrType_Msg = 9,
    UIAPacket_HdrType_EventRec = 10,
    UIAPacket_HdrType_CPUTrace = 11,
    UIAPacket_HdrType_STMTrace = 12,
    UIAPacket_HdrType_USER1 = 13,
    UIAPacket_HdrType_USER2 = 14,
    UIAPacket_HdrType_USER3 = 15
};
typedef enum UIAPacket_HdrType UIAPacket_HdrType;

/* PayloadEndian */
enum UIAPacket_PayloadEndian {
    UIAPacket_PayloadEndian_LITTLE = 0,
    UIAPacket_PayloadEndian_BIG = 1
};
typedef enum UIAPacket_PayloadEndian UIAPacket_PayloadEndian;

/* MsgType */
enum UIAPacket_MsgType {
    UIAPacket_MsgType_ACK=0, /* Reply acknowledging receipt of CMD or DATA packet */
    UIAPacket_MsgType_CMD=1, /* Command packets              */
    UIAPacket_MsgType_RESULT=2, /* Result reply                */
    UIAPacket_MsgType_PARTIALRESULT=3, /* Partial result reply */
    UIAPacket_MsgType_NOTIFY=4, /* Notify messaage (equiv. to the EVENT packet in TCF) */
    UIAPacket_MsgType_FLOWCTRL=5, /* Flow control packet      */
    UIAPacket_MsgType_DATA=6, /* Data packet (used for streaming data to host) */
    UIAPacket_MsgType_RESERVED7,
    UIAPacket_MsgType_RESERVED8,
    UIAPacket_MsgType_RESERVED9,
    UIAPacket_MsgType_RESERVED10,
    UIAPacket_MsgType_RESERVED11,
    UIAPacket_MsgType_RESERVED12,
    UIAPacket_MsgType_RESERVED13,
    UIAPacket_MsgType_NACK_BAD_DATA=14, /* Negative Acknowledge due to bad data */
    UIAPacket_MsgType_NACK_WITH_ERROR_CODE=15 /* Negative Acknowledge -    */
                                    /*    error code in msg body */
};
typedef enum UIAPacket_MsgType UIAPacket_MsgType;

enum UIAPacket_NACKErrorCode {
    UIAPacket_NACKErrorCode_NO_REASON_SPECIFIED=0, /*! Use this when none of the
                                                   defined NACK error codes are
                                                   appropriate */
    UIAPacket_NACKErrorCode_SERVICE_NOT_SUPPORTED=1, /*! A module that handles the
                                                     requested Service Id could
                                                     not be found */
    UIAPacket_NACKErrorCode_CMD_NOT_SUPPORTED=2, /*! The service does not support the
                                                 specified Command Id */
    UIAPacket_NACKErrorCode_QUEUE_FULL=3, /*! Cmd couldn't be passed on to dest. end
                                         point due to a queue full condition */
    UIAPacket_NACKErrorCode_BAD_ENDPOINT_ADDRESS=4, /*! The destination end point
                                                    address does not exists */
    UIAPacket_NACKErrorCode_BAD_MESSAGE_LENGTH=5 /*! packet lenght > endpoint max.
                                                 msg length or not what is
                                                 required to service the cmd  */
};
typedef enum UIAPacket_NACKErrorCode UIAPacket_NACKErrorCode;

enum UIAPacket_Priority {
    UIAPacket_Priority_LOW = 0,       /*! low priority */
    UIAPacket_Priority_STANDARD = 1,  /*! default priority */
    UIAPacket_Priority_HIGH = 2,      /*! use for critical errors, etc. */
    UIAPacket_Priority_SYNC = 3       /*! used only by LogSync's logger */
};

/*
 *  ======== HOST ========
 *  The address of the host
 *
 *  The host address is always 0xFFFF.
 */
#define UIAPacket_HOST 0xFFFF

/*
 *  ======== BROADCAST ========
 *  Used to denote a broadcast message
 *
 *  The broadcast address is always 0xFFFE.
 */
#define UIAPacket_BROADCAST 0xFFFE

#ifdef xdc_target__bigEndian
#error Big endian is not supported
#define UIAPacket_ISBIGENDIAN (TRUE)
#endif

#define UIAPacket_HDRTYPE_WORD    word1
#define UIAPacket_ENDIAN_WORD     word1
#define UIAPacket_MSGLEN_WORD     word1
#define UIAPacket_MSGTYPE_WORD    word1
#define UIAPacket_SERVICEID_WORD  word1
#define UIAPacket_SEQCOUNT_WORD   word2
#define UIAPacket_CMDID_WORD      word2
#define UIAPacket_TAG_WORD        word3
#define UIAPacket_DESTADRS_WORD   word4
#define UIAPacket_SENDERADRS_WORD word4

#define UIAPacket_EVENTLEN_WORD   word1
#define UIAPacket_PRIORITY_WORD   word2
#define UIAPacket_MODULEID_WORD   word3
#define UIAPacket_INSTANCEID_WORD   word3

#ifdef UIAPacket_ISBIGENDIAN
#define UIAPacket_swizzle(a)   (a)
#define UIAPacket_swizzle16(a) (a)

#define UIAPacket_HDRTYPE_BIT_SHIFT           28
#define UIAPacket_HDRTYPE_MASK        0xF0000000

#define UIAPacket_ENDIAN_BIT_SHIFT            27
#define UIAPacket_ENDIAN_MASK         0x08000000

#define UIAPacket_MSGLEN_BIT_SHIFT            16
#define UIAPacket_MSGLEN_MASK         0x07FF0000

#define UIAPacket_MSGTYPE_BIT_SHIFT           12
#define UIAPacket_MSGTYPE_MASK        0x0000F000

#define UIAPacket_SERVICEID_BIT_SHIFT          0
#define UIAPacket_SERVICEID_MASK      0x00000FFF

#define UIAPacket_SEQCOUNT_BIT_SHIFT          16
#define UIAPacket_SEQCOUNT_MASK       0xFFFF0000

#define UIAPacket_PRIORITY_BIT_SHIFT           0
#define UIAPacket_PRIORITY_MASK             0x03

#define UIAPacket_CMDID_BIT_SHIFT              0
#define UIAPacket_CMDID_MASK              0xFFFF

#define UIAPacket_DESTADRS_BIT_SHIFT          16
#define UIAPacket_DESTADRS_MASK       0xFFFF0000

#define UIAPacket_SENDERADRS_BIT_SHIFT         0
#define UIAPacket_SENDERADRS_MASK         0xFFFF

#define UIAPacket_EVENTLEN_BIT_SHIFT           0
#define UIAPacket_EVENTLEN_MASK       0x07FFFFFF

#define UIAPacket_MODULEID_BIT_SHIFT           0
#define UIAPacket_MODULEID_MASK           0xFFFF

#define UIAPacket_INSTANCEID_BIT_SHIFT        16
#define UIAPacket_INSTANCEID_MASK     0xFFFF0000

#define UIAPacket_FOOTER_MAGIC_NUMBER 0x00007777

#else
/* Little endian bit shift and mask definitions:
 * Byte 0  Byte 1  Byte 2  Byte 3
 * 31..24  23..16  15..8    7..0  Big Endian
 *  7..0   15..8   23..16  31..24 Little Endian
 */
#define UIAPacket_swizzle(a) \
        (((((unsigned int)a)>>24)&0xff) + ((((unsigned int)a)>>8)&0xff00) + \
     ((((unsigned int)a)<<8)&0x00ff0000) + ((((unsigned int)a)<<24)&0xff000000))

#define UIAPacket_swizzle16(a) \
        (((((unsigned int)a)<<8) & 0xff00) + ((((unsigned int)a)>>8)& 0xff))

#define UIAPacket_HDRTYPE_BIT_SHIFT           4
#define UIAPacket_HDRTYPE_MASK             0xF0

#define UIAPacket_ENDIAN_BIT_SHIFT            3
#define UIAPacket_ENDIAN_MASK              0x08

#define UIAPacket_MSGLEN_BIT_SHIFT            0
#define UIAPacket_MSGLEN_MASK            0xFF07

#define UIAPacket_MSGTYPE_BIT_SHIFT          20
#define UIAPacket_MSGTYPE_MASK       0x00F00000

#define UIAPacket_SERVICEID_BIT_SHIFT        16
#define UIAPacket_SERVICEID_MASK     0xFF0F0000

#define UIAPacket_SEQCOUNT_BIT_SHIFT          0
#define UIAPacket_SEQCOUNT_MASK          0xFFFF

#define UIAPacket_PRIORITY_BIT_SHIFT         24
#define UIAPacket_PRIORITY_MASK      0x03000000

#define UIAPacket_CMDID_BIT_SHIFT            16
#define UIAPacket_CMDID_MASK         0xFFFF0000

#define UIAPacket_DESTADRS_BIT_SHIFT          0
#define UIAPacket_DESTADRS_MASK          0xFFFF

#define UIAPacket_SENDERADRS_BIT_SHIFT       16
#define UIAPacket_SENDERADRS_MASK    0xFFFF0000

#define UIAPacket_EVENTLEN_BIT_SHIFT          0
#define UIAPacket_EVENTLEN_MASK      0xFFFFFF07

#define UIAPacket_MODULEID_BIT_SHIFT         16
#define UIAPacket_MODULEID_MASK      0xFFFF0000

#define UIAPacket_INSTANCEID_BIT_SHIFT        0
#define UIAPacket_INSTANCEID_MASK        0xFFFF

#define UIAPacket_FOOTER_MAGIC_NUMBER 0x77770000

#endif

/*
 *  ======== UIAPacket_getHdrType ========
 *  gets the message header type from the message header
 */
#define UIAPacket_getHdrType(pPktHdr)                           \
    ((UIAPacket_HdrType)                                                       \
     (((unsigned int)((pPktHdr)->UIAPacket_HDRTYPE_WORD) & UIAPacket_HDRTYPE_MASK) >> \
     UIAPacket_HDRTYPE_BIT_SHIFT))

/*
 *  ======== UIAPacket_setHdrType ========
 *  sets the message header type in the message header
 */
#define UIAPacket_setHdrType(pPktHdr, hdrType) \
   ((pPktHdr)->UIAPacket_HDRTYPE_WORD =                       \
       ((((unsigned int)hdrType  << UIAPacket_HDRTYPE_BIT_SHIFT) &  \
         UIAPacket_HDRTYPE_MASK) |                            \
        ((unsigned int)(pPktHdr)->UIAPacket_HDRTYPE_WORD & ~UIAPacket_HDRTYPE_MASK)))

/*
 *  ======== UIAPacket_getPayloadEndianness ========
 *  gets the payload endianness
 */
#define UIAPacket_getPayloadEndianness(pPktHdr) \
    (((unsigned int)((pPktHdr)->UIAPacket_ENDIAN_WORD) & UIAPacket_ENDIAN_MASK) >> \
    UIAPacket_ENDIAN_BIT_SHIFT)

/*
 *  ======== UIAPacket_setHdrType ========
 *  sets the message header type in the message header
 */
#define UIAPacket_setPayloadEndianness(pPktHdr, endianness) \
   ((pPktHdr)->UIAPacket_ENDIAN_WORD =                       \
       ((((unsigned int)endianness  << UIAPacket_ENDIAN_BIT_SHIFT) &  \
         UIAPacket_ENDIAN_MASK) |                            \
        ((unsigned int)(pPktHdr)->UIAPacket_ENDIAN_WORD & ~UIAPacket_ENDIAN_MASK)))

/*
 *  ======== UIAPacket_getMsgLength ========
 *  gets the message length (in bytes) from the message header
 */
#define UIAPacket_getMsgLength(pPktHdr)                          \
    (UIAPacket_swizzle16((unsigned short int)                                \
        (((unsigned int)((pPktHdr)->UIAPacket_MSGLEN_WORD) & UIAPacket_MSGLEN_MASK) >> \
        UIAPacket_MSGLEN_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setMsgLength ========
 *  sets the message length (in bytes) in the message header
 */
#define UIAPacket_setMsgLength(pPktHdr, msgLen)   \
   ((pPktHdr)->UIAPacket_MSGLEN_WORD =                           \
       ((UIAPacket_swizzle16((unsigned short int)msgLen) <<   \
        UIAPacket_MSGLEN_BIT_SHIFT) & UIAPacket_MSGLEN_MASK) |    \
        ((unsigned int)(pPktHdr)->UIAPacket_MSGLEN_WORD & ~UIAPacket_MSGLEN_MASK))
/*
 *  ======== UIAPacket_getMsgType ========
 *  gets the message type from the message header
 */
#define UIAPacket_getMsgType(pPktHdr)                           \
    ((UIAPacket_MsgType)                                                       \
     (((unsigned int)((pPktHdr)->UIAPacket_MSGTYPE_WORD) & UIAPacket_MSGTYPE_MASK) >> \
     UIAPacket_MSGTYPE_BIT_SHIFT))

/*
 *  ======== UIAPacket_setMsgType ========
 *  sets the message type in the message header
 */
#define UIAPacket_setMsgType(pPktHdr, msgType) \
    ((pPktHdr)->UIAPacket_MSGTYPE_WORD =                      \
       ((((unsigned int)msgType  << UIAPacket_MSGTYPE_BIT_SHIFT) &  \
         UIAPacket_MSGTYPE_MASK) |                            \
        ((unsigned int)(pPktHdr)->UIAPacket_MSGTYPE_WORD & ~UIAPacket_MSGTYPE_MASK)))

/*
 *  ======== UIAPacket_getServiceId ========
 *  gets the message type from the message header
 */
#define UIAPacket_getServiceId(pPktHdr)                           \
    (UIAPacket_swizzle16((unsigned short int)                                 \
     (((unsigned int)(pPktHdr)->UIAPacket_SERVICEID_WORD & UIAPacket_SERVICEID_MASK) >> \
      UIAPacket_SERVICEID_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setServiceId ========
 *  sets the message type in the message header
 */
#define UIAPacket_setServiceId(pPktHdr, serviceId)         \
    ((pPktHdr)->UIAPacket_SERVICEID_WORD =                                \
     ((UIAPacket_swizzle16((unsigned short int)serviceId) <<  \
      UIAPacket_SERVICEID_BIT_SHIFT) & UIAPacket_SERVICEID_MASK) |         \
     ((unsigned int)(pPktHdr)->UIAPacket_SERVICEID_WORD & ~UIAPacket_SERVICEID_MASK))

/*
 *  ======== UIAPacket_getSequenceCount ========
 *  gets the sequence count from the message header
 */
#define UIAPacket_getSequenceCount(pPktHdr)                      \
    (UIAPacket_swizzle16((unsigned short int)                                \
     (((unsigned int)(pPktHdr)->UIAPacket_SEQCOUNT_WORD & UIAPacket_SEQCOUNT_MASK) >> \
      UIAPacket_SEQCOUNT_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setSequenceCount ========
 *  sets the sequence count in the message header
 */
#define UIAPacket_setSequenceCount(pPktHdr, seqCount)  \
    ((pPktHdr)->UIAPacket_SEQCOUNT_WORD =                             \
     ((UIAPacket_swizzle16((unsigned short int)seqCount) <<        \
      UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK) |      \
     ((unsigned int)(pPktHdr)->UIAPacket_SEQCOUNT_WORD & ~UIAPacket_SEQCOUNT_MASK))

/*
 *  ======== UIAPacket_getLoggerPriority ========
 *
 *  gets the priority field from the event packet header
 */
#define UIAPacket_getLoggerPriority(pPktHdr)                    \
    ((IUIATransfer_Priority)                                                   \
    (((unsigned int)((pPktHdr)->UIAPacket_PRIORITY_WORD & UIAPacket_PRIORITY_MASK)>> \
     UIAPacket_PRIORITY_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setLoggerPriority ========
 *  sets the priority in the event packet header
 */
#define UIAPacket_setLoggerPriority(pPktHdr, priority)  \
    ((pPktHdr)->UIAPacket_PRIORITY_WORD =                              \
     ((((unsigned int)priority) << UIAPacket_PRIORITY_BIT_SHIFT) &           \
     UIAPacket_PRIORITY_MASK) |                                        \
     ((unsigned int)(pPktHdr)->UIAPacket_PRIORITY_WORD & ~UIAPacket_PRIORITY_MASK))

/*
 *  ======== UIAPacket_getCmdId ========
 *  gets the message type from the message header
 */
#define UIAPacket_getCmdId(pPktHdr)                        \
    (UIAPacket_swizzle16((unsigned short int)                          \
     (((unsigned int)(pPktHdr)->UIAPacket_CMDID_WORD & UIAPacket_CMDID_MASK) >> \
      UIAPacket_CMDID_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setCmdId ========
 *  sets the command Id in the message header
 */
#define UIAPacket_setCmdId(pPktHdr, cmdId)       \
    ((pPktHdr)->UIAPacket_CMDID_WORD =                          \
    ((UIAPacket_swizzle16((unsigned short int)cmdId) <<      \
      UIAPacket_CMDID_BIT_SHIFT) & UIAPacket_CMDID_MASK) |      \
     ((unsigned int)(pPktHdr)->UIAPacket_CMDID_WORD & ~UIAPacket_CMDID_MASK))

/*
 * ======== UIAPacket_getTag =========
 * gets the 32b tag field from the message header
 */
#define UIAPacket_getTag(pPktHdr) \
        ( (unsigned int)UIAPacket_swizzle((pPktHdr)->UIAPacket_TAG_WORD))

/*
 * ======== UIAPacket_setTag =========
 * sets the 32b tag field in the message header
 */
#define UIAPacket_setTag(pPktHdr, tagValue) \
        ((pPktHdr)->UIAPacket_TAG_WORD = UIAPacket_swizzle(tagValue))

/*!
 *  ======== UIAPacket_getLoggerModuleId ========
 *  gets the logger ID from the event packet header
 */
#define UIAPacket_getLoggerModuleId(pPktHdr)                          \
    (UIAPacket_swizzle16((unsigned short int)                               \
     (((unsigned int)(pPktHdr)->UIAPacket_MODULEID_WORD & UIAPacket_MODULEID_MASK) >> \
      UIAPacket_MODULEID_BIT_SHIFT)))

/*!
 *  ======== UIAPacket_setLoggerModuleId ========
 *  sets the module ID in the event packet header
 */
#define UIAPacket_setLoggerModuleId(pPktHdr, moduleId)      \
    ((pPktHdr)->UIAPacket_MODULEID_WORD =                            \
     ((UIAPacket_swizzle16(moduleId) <<               \
      UIAPacket_MODULEID_BIT_SHIFT) & UIAPacket_MODULEID_MASK) |      \
     ((unsigned int)(pPktHdr)->UIAPacket_MODULEID_WORD & ~UIAPacket_MODULEID_MASK))

/*!
 *  ======== UIAPacket_getLoggerInstanceId ========
 *  gets the logger instance ID from the event packet header
 */
#define UIAPacket_getLoggerInstanceId(pPktHdr)                          \
    (UIAPacket_swizzle16((unsigned short int)                               \
     (((unsigned int)(pPktHdr)->UIAPacket_INSTANCEID_WORD & UIAPacket_INSTANCEID_MASK) >> \
      UIAPacket_INSTANCEID_BIT_SHIFT)))

/*!
 *  ======== UIAPacket_setLoggerInstanceId ========
 *  sets the sequence count in the packet header
 */
#define UIAPacket_setLoggerInstanceId(pPktHdr, instanceId)      \
    ((pPktHdr)->UIAPacket_INSTANCEID_WORD =                            \
     ((UIAPacket_swizzle16(instanceId) <<               \
      UIAPacket_INSTANCEID_BIT_SHIFT) & UIAPacket_INSTANCEID_MASK) |      \
     ((unsigned int)(pPktHdr)->UIAPacket_INSTANCEID_WORD & ~UIAPacket_INSTANCEID_MASK))

/*
 *  ======== UIAPacket_getEventLength ========
 *  gets the len of the event from the message header
 */
#define UIAPacket_getEventLength(pPktHdr) \
    (UIAPacket_swizzle((unsigned int)           \
         ((pPktHdr)->UIAPacket_EVENTLEN_WORD & UIAPacket_EVENTLEN_MASK)))

/*
 *  ======== UIAPacket_setEventLength ========
 *  sets the len of the event from the message header
 */
#define UIAPacket_setEventLength(pPktHdr, eventLength)     \
    ((pPktHdr)->UIAPacket_EVENTLEN_WORD =                                 \
     ((UIAPacket_swizzle(eventLength) <<                   \
      UIAPacket_EVENTLEN_BIT_SHIFT) & UIAPacket_EVENTLEN_MASK) |           \
     ((unsigned int)(pPktHdr)->UIAPacket_EVENTLEN_WORD & ~UIAPacket_EVENTLEN_MASK))

/*
 *  ======== UIAPacket_getDestAdrs ========
 *  gets the message destination address from the message header
 */
#define UIAPacket_getDestAdrs(pPktHdr)                           \
    (UIAPacket_swizzle16((unsigned short int)                                \
     (((unsigned int)(pPktHdr)->UIAPacket_DESTADRS_WORD & UIAPacket_DESTADRS_MASK) >>  \
      UIAPacket_DESTADRS_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setDestAdrs ========
 *  sets the message destination address in the message header
 */
#define UIAPacket_setDestAdrs(pPktHdr, destAdrs)      \
    ((pPktHdr)->UIAPacket_DESTADRS_WORD =                            \
     ((UIAPacket_swizzle16(destAdrs)  <<            \
      UIAPacket_DESTADRS_BIT_SHIFT) & UIAPacket_DESTADRS_MASK) |      \
     ((unsigned int)(pPktHdr)->UIAPacket_DESTADRS_WORD & ~UIAPacket_DESTADRS_MASK))

/*
 *  ======== UIAPacket_getSenderAdrs ========
 *  gets the message sender address from the message header
 */
#define UIAPacket_getSenderAdrs(pPktHdr)                   \
    (UIAPacket_swizzle16((unsigned short int)                          \
     (((unsigned int)(pPktHdr)->UIAPacket_SENDERADRS_WORD & UIAPacket_SENDERADRS_MASK) >> \
      UIAPacket_SENDERADRS_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setSenderAdrs ========
 *  sets the message sender address in the message header
 */
#define UIAPacket_setSenderAdrs(pPktHdr, senderAdrs)     \
    ((pPktHdr)->UIAPacket_SENDERADRS_WORD =                             \
     ((UIAPacket_swizzle16(senderAdrs)  <<               \
      UIAPacket_SENDERADRS_BIT_SHIFT) & UIAPacket_SENDERADRS_MASK) |     \
     ((unsigned int)(pPktHdr)->UIAPacket_SENDERADRS_WORD & ~UIAPacket_SENDERADRS_MASK))

/*
 * ======== UIAPacket_initMsgHdr ========
 * initializes all bitfields in the message header
 */
#define UIAPacket_initMsgHdr(pPktHdr, endianness, msgType,     \
            msgLength, serviceId, seqCount, cmdId, tag, destAdrs, senderAdrs) \
    {(pPktHdr)->word1 = (                                                     \
        ((unsigned int)(UIAPacket_HdrType_Msg) <<                    \
          UIAPacket_HDRTYPE_BIT_SHIFT)                             |          \
        (((unsigned int)(endianness) <<                                             \
          UIAPacket_ENDIAN_BIT_SHIFT) & UIAPacket_ENDIAN_MASK)     |          \
        ((UIAPacket_swizzle16((unsigned short int)msgLength) <<            \
          UIAPacket_MSGLEN_BIT_SHIFT) & UIAPacket_MSGLEN_MASK)     |          \
        (((unsigned int)(msgType) <<                                                \
          UIAPacket_MSGTYPE_BIT_SHIFT) & UIAPacket_MSGTYPE_MASK)   |          \
        ((UIAPacket_swizzle16((unsigned short int)serviceId) <<            \
          UIAPacket_SERVICEID_BIT_SHIFT) & UIAPacket_SERVICEID_MASK));        \
                                                                              \
    (pPktHdr)->word2 =                                                        \
        ((UIAPacket_swizzle16((unsigned short int)seqCount) <<             \
          UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK) |          \
        ((UIAPacket_swizzle16((unsigned short int)cmdId) <<                \
          UIAPacket_CMDID_BIT_SHIFT) & UIAPacket_CMDID_MASK);                 \
                                                                              \
    (pPktHdr)->word3 =                                                        \
        UIAPacket_swizzle(tag);                                \
                                                                              \
    (pPktHdr)->word4 =                                                        \
        ((UIAPacket_swizzle16(destAdrs)  <<                    \
          UIAPacket_DESTADRS_BIT_SHIFT) & UIAPacket_DESTADRS_MASK) |          \
        ((UIAPacket_swizzle16(senderAdrs)  <<                  \
          UIAPacket_SENDERADRS_BIT_SHIFT) & UIAPacket_SENDERADRS_MASK);       \
    }

/*
 * ======== UIAPacket_initEventRecHdr ========
 * initializes all bitfields in the event record header
 */
#define UIAPacket_initEventRecHdr(pPktHdr, endianness,         \
            eventLength, seqCount, priority, moduleId, instanceId, destAdrs, senderAdrs)  \
    {(pPktHdr)->word1 = (                                                     \
        ((unsigned int)(UIAPacket_HdrType_EventRec) <<               \
          UIAPacket_HDRTYPE_BIT_SHIFT)                             |          \
        (((unsigned int)(endianness) <<                                             \
          UIAPacket_ENDIAN_BIT_SHIFT) & UIAPacket_ENDIAN_MASK)     |          \
        ((UIAPacket_swizzle(eventLength) <<                                   \
          UIAPacket_EVENTLEN_BIT_SHIFT) & UIAPacket_EVENTLEN_MASK));          \
                                                                              \
    (pPktHdr)->word2 =                                                        \
        ((UIAPacket_swizzle16((unsigned short int)seqCount) <<                            \
          UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK) |          \
        ((((unsigned int)priority) <<                                               \
          UIAPacket_PRIORITY_BIT_SHIFT) & UIAPacket_PRIORITY_MASK);           \
                                                                              \
    (pPktHdr)->word3 =                                                        \
        ((UIAPacket_swizzle16((unsigned short int)moduleId) <<                                \
          UIAPacket_MODULEID_BIT_SHIFT) & UIAPacket_MODULEID_MASK) |          \
        ((UIAPacket_swizzle16((unsigned short int)instanceId) <<                          \
          UIAPacket_INSTANCEID_BIT_SHIFT) & UIAPacket_INSTANCEID_MASK);       \
                                                                              \
    (pPktHdr)->word4 =                                                        \
        ((UIAPacket_swizzle16(destAdrs)  <<                                   \
          UIAPacket_DESTADRS_BIT_SHIFT) & UIAPacket_DESTADRS_MASK) |          \
        ((UIAPacket_swizzle16(senderAdrs)  <<                                 \
          UIAPacket_SENDERADRS_BIT_SHIFT) & UIAPacket_SENDERADRS_MASK);       \
    }

/*
 * ======== getFooter ========
 * returns the integer to use as the packet footer
 */
#ifdef UIAPacket_ISBIGENDIAN
#define UIAPacket_getFooter(pPktHdr) \
        (((Int32)(pPktHdr)->UIAPacket_MSGLEN_WORD & UIAPacket_LENGTH_SHIFTED_MASK) | UIAPacket_FOOTER_MAGIC_NUMBER)
#else
#define UIAPacket_getFooter(pPktHdr) \
        ((Int32)(pPktHdr)->UIAPacket_MSGLEN_WORD & (UIAPacket_LENGTH_WORDMASK_0 | UIAPacket_LENGTH_WORDMASK_1) | UIAPacket_FOOTER_MAGIC_NUMBER)
#endif

#endif
