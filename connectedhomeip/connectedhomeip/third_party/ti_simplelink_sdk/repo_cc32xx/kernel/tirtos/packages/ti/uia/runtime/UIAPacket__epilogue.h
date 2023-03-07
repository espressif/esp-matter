/*
 * Copyright (c) 2012-2018, Texas Instruments Incorporated
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
 *  ======== UIAPacket__epilogue.h ========
 */

#ifdef xdc_target__bigEndian
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
#define ti_uia_runtime_UIAPacket_swizzle(a)   (a)
#define ti_uia_runtime_UIAPacket_swizzle16(a) (a)

#define UIAPacket_HDRTYPE_BIT_SHIFT           28
#define UIAPacket_HDRTYPE_MASK        0xF0000000

#define UIAPacket_ENDIAN_BIT_SHIFT            27
#define UIAPacket_ENDIAN_MASK         0x08000000

/*
 *  For optimization, this is used when setting the event length.
 */
#define UIAPacket_EVENTHDR \
        ((ti_uia_runtime_UIAPacket_HdrType_EventPkt << UIAPacket_HDRTYPE_BIT_SHIFT) | \
        (UIAPacket_ISBIGENDIAN << UIAPacket_ENDIAN_BIT_SHIFT))
#define UIAPacket_MINEVENTHDR \
        ((ti_uia_runtime_UIAPacket_HdrType_MinEventPkt << UIAPacket_HDRTYPE_BIT_SHIFT) | \
        (UIAPacket_ISBIGENDIAN << UIAPacket_ENDIAN_BIT_SHIFT))


#define UIAPacket_MSGLEN_BIT_SHIFT            16
#define UIAPacket_MSGLEN_MASK         0x07FF0000

#define UIAPacket_MSGTYPE_BIT_SHIFT           12
#define UIAPacket_MSGTYPE_MASK        0x0000F000

#define UIAPacket_SERVICEID_BIT_SHIFT          0
#define UIAPacket_SERVICEID_MASK      0x00000FFF

#define UIAPacket_SEQCOUNT_BIT_SHIFT          16
#define UIAPacket_SEQCOUNT_MASK       0xFFFF0000

#define UIAPacket_EVTSEQCOUNT_BIT_SHIFT_PRE   2
#define UIAPacket_EVTSEQCOUNT_MASK_PRE   0x3FFF

#define UIAPacket_EVTSEQCOUNT_BIT_SHIFT        0
#define UIAPacket_EVTSEQCOUNT_MASK        0xFFFF

#define UIAPacket_SEQCOUNT2_BIT_SHIFT          16
#define UIAPacket_SEQCOUNT2_MASK       0xFFFF0000

#define UIAPacket_EVTSEQCOUNT2_BIT_SHIFT_PRE   2
#define UIAPacket_EVTSEQCOUNT2_MASK_PRE   0x3FFF

#define UIAPacket_EVTSEQCOUNT2_BIT_SHIFT        0
#define UIAPacket_EVTSEQCOUNT2_MASK        0xFFFF

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

#define UIAPacket_MINEVENTLEN_BIT_SHIFT        0
#define UIAPacket_MINEVENTLEN_MASK        0xFFFF

#define UIAPacket_MODULEID_BIT_SHIFT           0
#define UIAPacket_MODULEID_MASK           0xFFFF

#define UIAPacket_INSTANCEID_BIT_SHIFT        16
#define UIAPacket_INSTANCEID_MASK     0xFFFF0000

#define UIAPacket_LASTTIMESTAMP_BIT_SHIFT      0
#define UIAPacket_LASTTIMESTAMP_MASK      0xFFFF

#define UIAPacket_MINSENDERADRS_BIT_SHIFT     22
#define UIAPacket_MINSENDERADRS_MASK   0x7C00000

#define UIAPacket_MINPKTSEQCOUNT_BIT_SHIFT      16
#define UIAPacket_MINPKTSEQCOUNT_MASK     0xFFFF0000

#define UIAPacket_MINEVENTSEQCOUNT_BIT_SHIFT      0
#define UIAPacket_MINEVENTSEQCOUNT_MASK      0xFFFF

#define UIAPacket_FOOTER_MAGIC_NUMBER 0x00007777

#else
/* Little endian bit shift and mask definitions:
 * Byte 0  Byte 1  Byte 2  Byte 3
 * 31..24  23..16  15..8    7..0  Big Endian
 *  7..0   15..8   23..16  31..24 Little Endian
 */
#define ti_uia_runtime_UIAPacket_swizzle(a) \
        (((((Bits32)a) >> 24 ) & 0xff) | \
        ((((Bits32)a) >> 8) & 0xff00)  | \
        ((((Bits32)a) << 8 ) & 0x00ff0000) | \
        (((((Bits32)a) & 0xff) << 24) & 0xff000000))

#define ti_uia_runtime_UIAPacket_swizzle16(a) \
        (((((Bits32)a)<<8) & 0xff00) + ((((Bits32)a)>>8)& 0xff))

#define UIAPacket_HDRTYPE_BIT_SHIFT           4
#define UIAPacket_HDRTYPE_MASK             0xF0

/*
 *  For optimization, this is used when setting the event length.
 */
#define UIAPacket_EVENTHDR \
        ((ti_uia_runtime_UIAPacket_HdrType_EventPkt << UIAPacket_HDRTYPE_BIT_SHIFT))
#define UIAPacket_MINEVENTHDR \
        ((ti_uia_runtime_UIAPacket_HdrType_MinEventPkt << UIAPacket_HDRTYPE_BIT_SHIFT))

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

#define UIAPacket_EVTSEQCOUNT_BIT_SHIFT_PRE   2
#define UIAPacket_EVTSEQCOUNT_MASK_PRE   0x3FFF

#define UIAPacket_EVTSEQCOUNT_BIT_SHIFT      16
#define UIAPacket_EVTSEQCOUNT_MASK   0xFFFF0000

#define UIAPacket_SEQCOUNT2_BIT_SHIFT          16
#define UIAPacket_SEQCOUNT2_MASK       0xFFFF0000

#define UIAPacket_EVTSEQCOUNT2_BIT_SHIFT_PRE   2
#define UIAPacket_EVTSEQCOUNT2_MASK_PRE   0x3FFF

#define UIAPacket_EVTSEQCOUNT2_BIT_SHIFT        0
#define UIAPacket_EVTSEQCOUNT2_MASK        0xFFFF

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

#define UIAPacket_MINEVENTLEN_BIT_SHIFT       0
#define UIAPacket_MINEVENTLEN_MASK   0xFFFF0000

#define UIAPacket_MODULEID_BIT_SHIFT         16
#define UIAPacket_MODULEID_MASK      0xFFFF0000

#define UIAPacket_INSTANCEID_BIT_SHIFT        0
#define UIAPacket_INSTANCEID_MASK        0xFFFF

#define UIAPacket_LASTTIMESTAMP_BIT_SHIFT    16
#define UIAPacket_LASTTIMESTAMP_MASK    0x0FFFF0000

#define UIAPacket_MINSENDERADRS_BIT_SHIFT      0
#define UIAPacket_MINSENDERADRS_MASK      0xC007

#define UIAPacket_MINPKTSEQCOUNT_BIT_SHIFT      0
#define UIAPacket_MINPKTSEQCOUNT_MASK     0x0FFFF

#define UIAPacket_MINEVENTSEQCOUNT_BIT_SHIFT      16
#define UIAPacket_MINEVENTSEQCOUNT_MASK      0xFFFF0000

#define UIAPacket_FOOTER_MAGIC_NUMBER 0x77770000

#endif


/*
 *  ======== UIAPacket_getHdrType ========
 *  gets the message header type from the message header
 */
#define ti_uia_runtime_UIAPacket_getHdrType(pPktHdr)                           \
    ((ti_uia_runtime_UIAPacket_HdrType)                                                       \
     (((Bits32)((pPktHdr)->UIAPacket_HDRTYPE_WORD) & UIAPacket_HDRTYPE_MASK) >> \
     UIAPacket_HDRTYPE_BIT_SHIFT))

/*
 *  ======== UIAPacket_setHdrType ========
 *  sets the message header type in the message header
 */
#define ti_uia_runtime_UIAPacket_setHdrType(pPktHdr, hdrType) \
   ((pPktHdr)->UIAPacket_HDRTYPE_WORD =                       \
       ((((Bits32)hdrType  << UIAPacket_HDRTYPE_BIT_SHIFT) &  \
         UIAPacket_HDRTYPE_MASK) |                            \
        ((Bits32)(pPktHdr)->UIAPacket_HDRTYPE_WORD & ~UIAPacket_HDRTYPE_MASK)))

/*
 *  ======== UIAPacket_getPayloadEndianness ========
 *  gets the payload endianness
 */
#define ti_uia_runtime_UIAPacket_getPayloadEndianness(pPktHdr) \
    (((Bits32)((pPktHdr)->UIAPacket_ENDIAN_WORD) & UIAPacket_ENDIAN_MASK) >> \
    UIAPacket_ENDIAN_BIT_SHIFT)

/*
 *  ======== UIAPacket_setHdrType ========
 *  sets the message header type in the message header
 */
#define ti_uia_runtime_UIAPacket_setPayloadEndianness(pPktHdr, endianness) \
   ((pPktHdr)->UIAPacket_ENDIAN_WORD =                       \
       ((((Bits32)endianness  << UIAPacket_ENDIAN_BIT_SHIFT) &  \
         UIAPacket_ENDIAN_MASK) |                            \
        ((Bits32)(pPktHdr)->UIAPacket_ENDIAN_WORD & ~UIAPacket_ENDIAN_MASK)))

/*
 *  ======== UIAPacket_getMsgLength ========
 *  gets the message length (in bytes) from the message header
 */
#define ti_uia_runtime_UIAPacket_getMsgLength(pPktHdr)                          \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                                \
        (((Bits32)((pPktHdr)->UIAPacket_MSGLEN_WORD) & UIAPacket_MSGLEN_MASK) >> \
        UIAPacket_MSGLEN_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setMsgLength ========
 *  sets the message length (in bytes) in the message header
 */
#define ti_uia_runtime_UIAPacket_setMsgLength(pPktHdr, msgLen)           \
   ((pPktHdr)->UIAPacket_MSGLEN_WORD =                                   \
       (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)msgLen) <<   \
        UIAPacket_MSGLEN_BIT_SHIFT) & UIAPacket_MSGLEN_MASK) |           \
        ((Bits32)(pPktHdr)->UIAPacket_MSGLEN_WORD & ~UIAPacket_MSGLEN_MASK))
/*
 *  ======== UIAPacket_getMsgType ========
 *  gets the message type from the message header
 */
#define ti_uia_runtime_UIAPacket_getMsgType(pPktHdr)                           \
    ((UIAPacket_MsgType)                                                       \
     (((Bits32)((pPktHdr)->UIAPacket_MSGTYPE_WORD) & UIAPacket_MSGTYPE_MASK) >> \
     UIAPacket_MSGTYPE_BIT_SHIFT))

/*
 *  ======== UIAPacket_setMsgType ========
 *  sets the message type in the message header
 */
#define ti_uia_runtime_UIAPacket_setMsgType(pPktHdr, msgType) \
    ((pPktHdr)->UIAPacket_MSGTYPE_WORD =                      \
       ((((Bits32)msgType  << UIAPacket_MSGTYPE_BIT_SHIFT) &  \
         UIAPacket_MSGTYPE_MASK) |                            \
        ((Bits32)(pPktHdr)->UIAPacket_MSGTYPE_WORD & ~UIAPacket_MSGTYPE_MASK)))

/*
 *  ======== UIAPacket_getServiceId ========
 *  gets the message type from the message header
 */
#define ti_uia_runtime_UIAPacket_getServiceId(pPktHdr)                           \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                                 \
     (((Bits32)(pPktHdr)->UIAPacket_SERVICEID_WORD & UIAPacket_SERVICEID_MASK) >> \
      UIAPacket_SERVICEID_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setServiceId ========
 *  sets the message type in the message header
 */
#define ti_uia_runtime_UIAPacket_setServiceId(pPktHdr, serviceId)          \
    ((pPktHdr)->UIAPacket_SERVICEID_WORD =                                 \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)serviceId) <<    \
      UIAPacket_SERVICEID_BIT_SHIFT) & UIAPacket_SERVICEID_MASK) |         \
     ((Bits32)(pPktHdr)->UIAPacket_SERVICEID_WORD & ~UIAPacket_SERVICEID_MASK))

/*
 *  ======== UIAPacket_getSequenceCount ========
 *  gets the sequence count from the message header
 */
#define ti_uia_runtime_UIAPacket_getSequenceCount(pPktHdr)                      \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                                \
     (((Bits32)(pPktHdr)->UIAPacket_SEQCOUNT_WORD & UIAPacket_SEQCOUNT_MASK) >> \
      UIAPacket_SEQCOUNT_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setSequenceCount ========
 *  sets the sequence count in the message header
 */
#define ti_uia_runtime_UIAPacket_setSequenceCount(pPktHdr, seqCount)       \
    ((pPktHdr)->UIAPacket_SEQCOUNT_WORD =                                  \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)seqCount) <<     \
      UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK) |           \
     ((Bits32)(pPktHdr)->UIAPacket_SEQCOUNT_WORD & ~UIAPacket_SEQCOUNT_MASK))

/*
 *  ======== UIAPacket_setSequenceCountFast ========
 *  sets the sequence count in the event packet header, without reading in the
 *  sequence count word.
 */
#define ti_uia_runtime_UIAPacket_setSequenceCountFast(pPktHdr, seqCount)       \
    ((pPktHdr)->UIAPacket_SEQCOUNT_WORD =                                  \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)seqCount) <<     \
      UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK))

/*
 *  ======== UIAPacket_setSequenceCounts ========
 *  sets the packet sequence count and event sequence count in the event packet header,
 *  without reading in the sequence count word.
 */
#define ti_uia_runtime_UIAPacket_setSequenceCounts(pPktHdr, pktSeqCount, eventSeqCount) \
    ((pPktHdr)->UIAPacket_SEQCOUNT_WORD = \
     (Bits32)ti_uia_runtime_UIAPacket_swizzle( \
         ((((Bits32)pktSeqCount) << UIAPacket_SEQCOUNT2_BIT_SHIFT) & UIAPacket_SEQCOUNT2_MASK) | \
     ((((Bits32)(UIAPacket_EVTSEQCOUNT2_MASK_PRE & eventSeqCount)<< \
         UIAPacket_EVTSEQCOUNT2_BIT_SHIFT_PRE)  << UIAPacket_EVTSEQCOUNT2_BIT_SHIFT) \
           & UIAPacket_EVTSEQCOUNT2_MASK)))

/*
 *  ======== setMinEventPacketSequenceCount ========
 *  stores the packet sequence count and event sequence count of the
 *  first event in the packet in the second word of the MinPacket header
 *  without reading in header word.
 *
 *  Message header is always layed out as big endian (network order).
 *  This macro is designed to work on both little endian and
 *  big endian targets, since 32b long words are consistently
 *  handled for both endianness - it is only when converting
 *  from bytes to longs that bit orders get swapped.
 *  #ifdef xdc_target__bigEndian can be used if necessary.
 *
 *  @param(PktHdr* pPktHdr)   pointer to the packet header
 *  @param(eventSeqCount) the event sequence count of the first event in the packet
 *  @param(pktSeqCount)  the new packet sequence count
 */

#define ti_uia_runtime_UIAPacket_setMinEventPacketSequenceCount(pPktHdr, pktSeqCount, eventSeqCount)       \
    ((pPktHdr)->UIAPacket_SEQCOUNT_WORD =                                  \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)pktSeqCount) <<     \
         UIAPacket_MINPKTSEQCOUNT_BIT_SHIFT) & UIAPacket_MINPKTSEQCOUNT_MASK) |  \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(eventSeqCount)  <<          \
         UIAPacket_MINEVENTSEQCOUNT_BIT_SHIFT) & UIAPacket_MINEVENTSEQCOUNT_MASK))
/*
 *  ======== UIAPacket_getLoggerPriority ========
 *
 *  gets the priority field from the event packet header
 */
#define ti_uia_runtime_UIAPacket_getLoggerPriority(pPktHdr)                    \
    ((IUIATransfer_Priority)                                                   \
    (((Bits32)((pPktHdr)->UIAPacket_PRIORITY_WORD & UIAPacket_PRIORITY_MASK)>> \
     UIAPacket_PRIORITY_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setLoggerPriority ========
 *  sets the priority in the event packet header
 */
#define ti_uia_runtime_UIAPacket_setLoggerPriority(pPktHdr, priority)  \
    ((pPktHdr)->UIAPacket_PRIORITY_WORD =                              \
     ((((Bits32)priority) << UIAPacket_PRIORITY_BIT_SHIFT) &           \
     UIAPacket_PRIORITY_MASK) |                                        \
     ((Bits32)(pPktHdr)->UIAPacket_PRIORITY_WORD & ~UIAPacket_PRIORITY_MASK))

/*
 *  ======== UIAPacket_getCmdId ========
 *  gets the message type from the message header
 */
#define ti_uia_runtime_UIAPacket_getCmdId(pPktHdr)                        \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                          \
     (((Bits32)(pPktHdr)->UIAPacket_CMDID_WORD & UIAPacket_CMDID_MASK) >> \
      UIAPacket_CMDID_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setCmdId ========
 *  sets the command Id in the message header
 */
#define ti_uia_runtime_UIAPacket_setCmdId(pPktHdr, cmdId)            \
    ((pPktHdr)->UIAPacket_CMDID_WORD =                               \
    (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)cmdId) <<   \
      UIAPacket_CMDID_BIT_SHIFT) & UIAPacket_CMDID_MASK) |           \
     ((Bits32)(pPktHdr)->UIAPacket_CMDID_WORD & ~UIAPacket_CMDID_MASK))

/*
 * ======== UIAPacket_getTag =========
 * gets the 32b tag field from the message header
 */
#define ti_uia_runtime_UIAPacket_getTag(pPktHdr) \
        ( (Bits32)ti_uia_runtime_UIAPacket_swizzle((pPktHdr)->UIAPacket_TAG_WORD))

/*
 * ======== UIAPacket_setTag =========
 * sets the 32b tag field in the message header
 */
#define ti_uia_runtime_UIAPacket_setTag(pPktHdr, tagValue) \
        ((pPktHdr)->UIAPacket_TAG_WORD = ti_uia_runtime_UIAPacket_swizzle(tagValue))

/*!
 *  ======== UIAPacket_getLoggerModuleId ========
 *  gets the logger ID from the event packet header
 */
#define ti_uia_runtime_UIAPacket_getLoggerModuleId(pPktHdr)                          \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                               \
     (((Bits32)(pPktHdr)->UIAPacket_MODULEID_WORD & UIAPacket_MODULEID_MASK) >> \
      UIAPacket_MODULEID_BIT_SHIFT)))

/*!
 *  ======== UIAPacket_setLoggerModuleId ========
 *  sets the module ID in the event packet header
 */
#define ti_uia_runtime_UIAPacket_setLoggerModuleId(pPktHdr, moduleId) \
    ((pPktHdr)->UIAPacket_MODULEID_WORD =                             \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(moduleId) <<        \
      UIAPacket_MODULEID_BIT_SHIFT) & UIAPacket_MODULEID_MASK) |      \
     ((Bits32)(pPktHdr)->UIAPacket_MODULEID_WORD & ~UIAPacket_MODULEID_MASK))

/*!
 *  ======== UIAPacket_getLoggerInstanceId ========
 *  gets the logger instance ID from the event packet header
 */
#define ti_uia_runtime_UIAPacket_getLoggerInstanceId(pPktHdr)                          \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                               \
     (((Bits32)(pPktHdr)->UIAPacket_INSTANCEID_WORD & UIAPacket_INSTANCEID_MASK) >> \
      UIAPacket_INSTANCEID_BIT_SHIFT)))

/*!
 *  ======== UIAPacket_setLoggerInstanceId ========
 *  sets the sequence count in the packet header
 */
#define ti_uia_runtime_UIAPacket_setLoggerInstanceId(pPktHdr, instanceId) \
    ((pPktHdr)->UIAPacket_INSTANCEID_WORD =                               \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(instanceId) <<          \
      UIAPacket_INSTANCEID_BIT_SHIFT) & UIAPacket_INSTANCEID_MASK) |      \
     ((Bits32)(pPktHdr)->UIAPacket_INSTANCEID_WORD & ~UIAPacket_INSTANCEID_MASK))

/*
 *  ======== UIAPacket_getEventLength ========
 *  gets the len of the event from the message header
 */
#define ti_uia_runtime_UIAPacket_getEventLength(pPktHdr) \
    (ti_uia_runtime_UIAPacket_swizzle((Bits32)           \
         ((pPktHdr)->UIAPacket_EVENTLEN_WORD & UIAPacket_EVENTLEN_MASK)))

#define ti_uia_runtime_UIAPacket_getMinEventLength(pMinPktHdr) \
    (ti_uia_runtime_UIAPacket_swizzle((Bits32)           \
         ((pPktHdr)->UIAPacket_EVENTLEN_WORD & UIAPacket_MINEVENTLEN_MASK)))

/*
 *  ======== UIAPacket_setEventLength ========
 *  sets the len of the event from the message header
 */
#define ti_uia_runtime_UIAPacket_setEventLength(pPktHdr, eventLength)     \
    ((pPktHdr)->UIAPacket_EVENTLEN_WORD =                                 \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle(eventLength) <<           \
      UIAPacket_EVENTLEN_BIT_SHIFT) & UIAPacket_EVENTLEN_MASK) |          \
     ((Bits32)(pPktHdr)->UIAPacket_EVENTLEN_WORD & ~UIAPacket_EVENTLEN_MASK))

/*
 *  ======== UIAPacket_setEventLengthFast ========
 *  Sets the length field of the event packet header. Don't read the event
 *  length word, just write it out.
 */
#define ti_uia_runtime_UIAPacket_setEventLengthFast(pPktHdr, eventLength)    \
    ((pPktHdr)->UIAPacket_EVENTLEN_WORD =                                 \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle(eventLength) <<           \
      UIAPacket_EVENTLEN_BIT_SHIFT) & UIAPacket_EVENTLEN_MASK) |          \
     (Bits32)(UIAPacket_EVENTHDR))

/*
 *  ======== UIAPacket_setMinEventPacketLength ========
 *  sets the length field in the minEventPacket header. Don't read the event
 *  length word, just write it out.
 */
#define ti_uia_runtime_UIAPacket_setMinEventPacketLength(pPktHdr, eventLength,senderAdrs)    \
    ((pPktHdr)->UIAPacket_EVENTLEN_WORD =                                 \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle(eventLength) <<           \
      UIAPacket_EVENTLEN_BIT_SHIFT) & UIAPacket_EVENTLEN_MASK) |          \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(senderAdrs)  <<          \
      UIAPacket_MINSENDERADRS_BIT_SHIFT) & UIAPacket_MINSENDERADRS_MASK) | \
     (Bits32)(UIAPacket_MINEVENTHDR))
/*
 *  ======== UIAPacket_getDestAdrs ========
 *  gets the message destination address from the message header
 */
#define ti_uia_runtime_UIAPacket_getDestAdrs(pPktHdr)                           \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                                \
     (((Bits32)(pPktHdr)->UIAPacket_DESTADRS_WORD & UIAPacket_DESTADRS_MASK) >> \
      UIAPacket_DESTADRS_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setDestAdrs ========
 *  sets the message destination address in the message header
 */
#define ti_uia_runtime_UIAPacket_setDestAdrs(pPktHdr, destAdrs)      \
    ((pPktHdr)->UIAPacket_DESTADRS_WORD =                            \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(destAdrs)  <<      \
      UIAPacket_DESTADRS_BIT_SHIFT) & UIAPacket_DESTADRS_MASK) |     \
     ((Bits32)(pPktHdr)->UIAPacket_DESTADRS_WORD & ~UIAPacket_DESTADRS_MASK))

/*
 *  ======== UIAPacket_getSenderAdrs ========
 *  gets the message sender address from the message header
 */
#define ti_uia_runtime_UIAPacket_getSenderAdrs(pPktHdr)                   \
    (ti_uia_runtime_UIAPacket_swizzle16((Bits16)                          \
     (((Bits32)(pPktHdr)->UIAPacket_SENDERADRS_WORD & UIAPacket_SENDERADRS_MASK) >> \
      UIAPacket_SENDERADRS_BIT_SHIFT)))

/*
 *  ======== UIAPacket_setSenderAdrs ========
 *  sets the message sender address in the message header
 */
#define ti_uia_runtime_UIAPacket_setSenderAdrs(pPktHdr, senderAdrs)     \
    ((pPktHdr)->UIAPacket_SENDERADRS_WORD =                             \
     (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(senderAdrs)  <<       \
      UIAPacket_SENDERADRS_BIT_SHIFT) & UIAPacket_SENDERADRS_MASK) |    \
     ((Bits32)(pPktHdr)->UIAPacket_SENDERADRS_WORD & ~UIAPacket_SENDERADRS_MASK))

/*
 * ======== UIAPacket_initMsgHdr ========
 * initializes all bitfields in the message header
 */
#define ti_uia_runtime_UIAPacket_initMsgHdr(pPktHdr, endianness, msgType,     \
            msgLength, serviceId, seqCount, cmdId, tag, destAdrs, senderAdrs) \
    {(pPktHdr)->word1 = (                                                     \
        ((Bits32)(ti_uia_runtime_UIAPacket_HdrType_Msg) <<                    \
          UIAPacket_HDRTYPE_BIT_SHIFT)                             |          \
        (((Bits32)(endianness) <<                                             \
          UIAPacket_ENDIAN_BIT_SHIFT) & UIAPacket_ENDIAN_MASK)     |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)msgLength) <<    \
          UIAPacket_MSGLEN_BIT_SHIFT) & UIAPacket_MSGLEN_MASK)     |          \
        (((Bits32)(msgType) <<                                                \
          UIAPacket_MSGTYPE_BIT_SHIFT) & UIAPacket_MSGTYPE_MASK)   |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)serviceId) <<    \
          UIAPacket_SERVICEID_BIT_SHIFT) & UIAPacket_SERVICEID_MASK));        \
                                                                              \
    (pPktHdr)->word2 =                                                        \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)seqCount) <<     \
          UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK) |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)cmdId) <<        \
          UIAPacket_CMDID_BIT_SHIFT) & UIAPacket_CMDID_MASK);                 \
                                                                              \
    (pPktHdr)->word3 =                                                        \
        ti_uia_runtime_UIAPacket_swizzle(tag);                                \
                                                                              \
    (pPktHdr)->word4 =                                                        \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(destAdrs)  <<            \
          UIAPacket_DESTADRS_BIT_SHIFT) & UIAPacket_DESTADRS_MASK) |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(senderAdrs)  <<          \
          UIAPacket_SENDERADRS_BIT_SHIFT) & UIAPacket_SENDERADRS_MASK);       \
    }

/*
 * ======== UIAPacket_initEventRecHdr ========
 * initializes all bitfields in the event record header
 */
#define ti_uia_runtime_UIAPacket_initEventRecHdr(pPktHdr, endianness,         \
            eventLength, seqCount, priority, moduleId, instanceId, destAdrs, senderAdrs) \
    {(pPktHdr)->word1 = (                                                     \
        ((Bits32)(ti_uia_runtime_UIAPacket_HdrType_EventPkt) <<               \
          UIAPacket_HDRTYPE_BIT_SHIFT)                             |          \
        (((Bits32)(endianness) <<                                             \
          UIAPacket_ENDIAN_BIT_SHIFT) & UIAPacket_ENDIAN_MASK)     |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle(eventLength) <<            \
          UIAPacket_EVENTLEN_BIT_SHIFT) & UIAPacket_EVENTLEN_MASK));          \
                                                                              \
    (pPktHdr)->word2 =                                                        \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)seqCount) <<     \
          UIAPacket_SEQCOUNT_BIT_SHIFT) & UIAPacket_SEQCOUNT_MASK) |          \
        ((((Bits32)priority) <<                                               \
          UIAPacket_PRIORITY_BIT_SHIFT) & UIAPacket_PRIORITY_MASK);           \
                                                                              \
    (pPktHdr)->word3 =                                                        \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)moduleId) <<     \
          UIAPacket_MODULEID_BIT_SHIFT) & UIAPacket_MODULEID_MASK) |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16((Bits16)instanceId) <<   \
          UIAPacket_INSTANCEID_BIT_SHIFT) & UIAPacket_INSTANCEID_MASK);       \
                                                                              \
    (pPktHdr)->word4 =                                                        \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(destAdrs)  <<            \
          UIAPacket_DESTADRS_BIT_SHIFT) & UIAPacket_DESTADRS_MASK) |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(senderAdrs)  <<          \
          UIAPacket_SENDERADRS_BIT_SHIFT) & UIAPacket_SENDERADRS_MASK);       \
    }

/*
 * ======== UIAPacket_initMinEventRecHdr ========
 * initializes all bitfields in the minEventPacket header, setting
 * length, sequence count, last timestamps fields to 0
 */
#define ti_uia_runtime_UIAPacket_initMinEventRecHdr(pPktHdr, endianness,      \
            senderAdrs)                                                       \
    {(pPktHdr)->word1 = (                                                     \
        ((Bits32)(ti_uia_runtime_UIAPacket_HdrType_MinEventPkt) <<            \
          UIAPacket_HDRTYPE_BIT_SHIFT)                             |          \
        (((Bits32)(endianness) <<                                             \
          UIAPacket_ENDIAN_BIT_SHIFT) & UIAPacket_ENDIAN_MASK)     |          \
        (((Bits32)ti_uia_runtime_UIAPacket_swizzle16(senderAdrs)  <<          \
          UIAPacket_MINSENDERADRS_BIT_SHIFT) & UIAPacket_MINSENDERADRS_MASK)); \
    }

/*
 * ======== getFooter ========
 * returns the integer to use as the packet footer
 */
#ifdef UIAPacket_ISBIGENDIAN
#define ti_uia_runtime_UIAPacket_getFooter(pPktHdr) \
        (((Int32)(pPktHdr)->UIAPacket_MSGLEN_WORD & UIAPacket_LENGTH_SHIFTED_MASK) | UIAPacket_FOOTER_MAGIC_NUMBER)
#else
#define ti_uia_runtime_UIAPacket_getFooter(pPktHdr) \
        ((Int32)(pPktHdr)->UIAPacket_MSGLEN_WORD & (UIAPacket_LENGTH_WORDMASK_0 | UIAPacket_LENGTH_WORDMASK_1) | UIAPacket_FOOTER_MAGIC_NUMBER)
#endif


/*
 *  ======== UIAPacket_setInvalidHdr ========
 *  sets the len of the event from the invalid header
 *
 *  Using the fact that HdrType_InvalidData is zero and we don't care about the
 *  endianness of the payload.
 */
#define ti_uia_runtime_UIAPacket_setInvalidHdr(pPktHdr, eventLength)     \
    *((UInt32 *)(pPktHdr)) = (UInt32)ti_uia_runtime_UIAPacket_swizzle(eventLength)

/*
 */
