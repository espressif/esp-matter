/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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
 * ======== UIAPacket.xdc ========
 */
package ti.uia.runtime;

import ti.uia.runtime.IUIATransfer;

/*!
 * ======== UIAPacket ========
 * Packet format communications between instrumentation clients
 * and instrumentation endpoints
 */
@CustomHeader
module UIAPacket {

    /*!
     *  ======== Hdr ========
     *  UIAPacket Header
     *
     *  The following is a breakdown of the packet header based on type packet.
     *
     *  The packet header is always sent in network byte (big endian) order.
     *
     *  The top 4 leftmost bits of word1 denote what type of packet this is:
     *  HdrType_Msg or HdrType_EventPkt. The contents of the packet depends on
     *  this type.
     *
     *  @p(code)
     *
     *  HdrType_Msg word1
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |H H H H|E|L L L L L L L L L L L|T T T T|S S S S S S S S S S S S|
     *  |---------------------------------------------------------------|
     *
     *  H = HdrType         (4-bits)
     *  E = Payload endian  (1-bit)
     *  L = Message Length  (11-bits)
     *  T = Message Type    (4-bits)
     *  S = Service Id      (12-bits)
     *
     *  HdrType_Msg word2
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |S S S S S S S S S S S S S S S S|C C C C C C C C C C C C C C C C|
     *  |---------------------------------------------------------------|
     *
     *  S = Sequence Number (16-bits)
     *  C = Command Id      (16-bits)
     *
     *  HdrType_Msg word3
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T T|
     *  |---------------------------------------------------------------|
     *
     *  T - Tag             (32-bits)
     *
     *  HdrType_Msg word4
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |D D D D D D D D D D D D D D D D|S S S S S S S S S S S S S S S S|
     *  |---------------------------------------------------------------|
     *
     *  D - Destination Address (16-bits)
     *  S - Source Address      (16-bits)
     * 
     * @p(code)
     *
     *  HdrType_EventPkt word1
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |H H H H|E|L L L L L L L L L L L L L L L L L L L L L L L L L L L|
     *  |---------------------------------------------------------------|
     *
     *  H = HdrType         (4-bits)
     *  E = Payload endian   (1-bit)
     *  L = Packet Length   (27-bits)
     *
     *  HdrType_EventPkt word2
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |S S S S S S S S S S S S S S S S|E E E E E E E E E E E E E E|P P|
     *  |---------------------------------------------------------------|
     *
     *  S = Packet Sequence Number              (16-bits)
     *  E = Event Sequence Number for first event in the packet (14-bits)     
     *  P = Priority: Normal=0, High > 0 (2-bits)
     *
     *  HdrType_EventPkt word3
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |D I I I I I I I I I I I I I I I M M M M M M M M M M M M M M M M|
     *  |---------------------------------------------------------------|
     *
     *  D -  wasLoggerDynamicallyCreated  (1-bit:  0 = statically created,
     *                                            1 = dynamically created )
     *  I - Logger Instance ID           (15-bits)
     *  M - Logger Module ID             (16-bits)
     *
     *  HdrType_EventPkt word4
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |D D D D D D D D D D D D D D D D|S S S S S S S S S S S S S S S S|
     *  |---------------------------------------------------------------|
     *
     *  D - Destination Address (16-bits)
     *  S - Source Address      (16-bits)
     *
     *  @p(code)
     *
     * For HdrType_EventPktWithCRC 
     *  word 5 holds the 16b CRC of the application name (0 for RTOSes such as SysBIOS)
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |R R R R R R R R R R R R R R R|C C C C C C C C C C C C C C C C C|
     *  |---------------------------------------------------------------|
     *  C = CRC16 of the application name (0 if only one process) (32-bits)  
     *  R = Reserved (set to 0)      
     *
     *  @p(code)
     *
     *  For memory constrained systems, ti.uia.loggers.LoggerMin uses the 
     *  following 2 word Event Packet header format, with HdrType =
     *  HdrType_MinEventPkt :
     *
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |H H H H|E|D D D D D|R R R R R R|L L L L L L L L L L L L L L L L|
     *  |---------------------------------------------------------------|
     *
     *  H = HdrType          (4-bits)
     *  E = Payload endian   (1-bit)
     *  D = Sender Adrs / Device ID  (5 bits – DNUM) 
     *  R = Reserved (set to 0)        
     *  L = packet Length     (16-bits)
     *
     *  HdrType_MinEventPkt word2
     *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
     *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *  |---------------------------------------------------------------|
     *  |S S S S S S S S S S S S S S S S|E E E E E E E E E E E E E E|P P|
     *  |---------------------------------------------------------------|
     *
     *  S = Packet Sequence Number              (16-bits)
     *  E = Event Sequence Number for first event in the packet (14-bits)
     *  P = Priority: Normal=0, High > 0 (2-bits)
     */
    struct Hdr {
        Bits32 word1;
        Bits32 word2;
        Bits32 word3;
        Bits32 word4;
    };

    /*!
     * ======== HdrType ========
     *  Enumeration of the various types of packet
     *  headers.
     *
     *  Stored in a 4 bit bitfield (b31-b28) of the first word in the
     *  packet.
     *
     *  The HdrType_InvalidData denotes that following data is invalid.
     *  This can used to pad to the end of a buffer. Only the first word
     *  of the header is filled in. The length includes the header field.
     *
     */
    enum HdrType {
        HdrType_InvalidData = 0, /*! reserved for future use */
        HdrType_MsgWithPID = 1, /*! message with Process ID field (for multi-process O/Ses) */
        HdrType_EventPktWithCRC = 2, /*! event packet with CRC of applciation name (for multi-process O/Ses)  */
        HdrType_MinEventPkt = 3, /*! small footprint event packet used by ti.uia.loggers.LoggerMin */
        HdrType_Reserved4 = 4, /*! reserved for future use */
        HdrType_Reserved5 = 5, /*! reserved for future use */
        HdrType_Reserved6 = 6, /*! reserved for future use */
        HdrType_Reserved7 = 7, /*! reserved for future use */
        HdrType_ChannelizedData = 8, /*! Channelized data stream */
        HdrType_Msg = 9, /*! Message  (4 words header, 1 word footer) */
        HdrType_EventPkt = 10, /*! Event rec. containing multiple events:
                                  (4 word hdr, 1 word footer) */
        HdrType_CPUTrace = 11, /*! CPU Trace ETB data
                                   (4 word hdr, 1 word footer) */
        HdrType_STMTrace = 12, /*! STM Trace ETB data
                                   (4 word hdr, 1 word footer) */
        HdrType_MemoryBuffer = 13, /*! Memory Buffer data */
        HdrType_USER2 = 14, /*! User defined header type 2 */
        HdrType_USER3 = 15  /*! User defined header type 3 */
    };
    

    /*!
     * ======== PayloadEndian ========
     * Enumeration of payload endianness
     */
    enum PayloadEndian {
        PayloadEndian_LITTLE = 0,
        PayloadEndian_BIG = 1
    };

    /*!
     * ======== Footer ========
     * Packet Footer
     *
     * The packet footer is always in network byte order
     * i.e. big-endian byte ordering
     */
    struct Footer
    {
        Int32 word1;        /*! 16 MSBs contain msg length in Bytes,
                                    16 LSBs contain 0x7777 */
    }

    /*!
     * ======== MsgType ========
     * Message Types
     *
     * Enumeration of the various types of packets.
     * Stored in a 4 bit bitfield.
     *
     * MsgType_ACK:
     * Each packet of type CMD or DATA is immediately acknowledged by
     * sending either an ACK reply (if the packet can be processed),
     * a RESULT or PARTIALRESULT reply (if requested data can be provided
     * immediately) or a NACK reply (if the packet cannot be processed).
     * The Service that receives the command is responsible for sending
     * the ACK or NACK.  If the requested service is not available, the
     * Endpoint is responsible for sending the NACK packet.
     *
     * MsgType_CMD:
     * CMD packets are typically sent from the host to the target.
     * The receiving endpoint routes the command to the service
     * identified in the Service Id field.
     *
     * MsgType_RESULT:
     * RESULT replies are sent along with any requested data.
     * If the Service cannot reply with the requested data immediately,
     * it should return with an ACK packet and send a RESULT packet
     * that echoes the header info of the original command.
     *
     * MsgType_PARTIALRESULT:
     * PARTIALRESULT replies are sent along with any requested data when the
     * requested action can only be partly fulfilled.  Examples are when the
     * service is reporting on some ongoing operation or has been requested
     * to periodically poll some data value.
     *
     * MsgType_EVENT:
     * EVENT packets are sent to all interested parties in order to notify them
     * about state changes.   They can, for example, be used to report an error
     * that has occurred which may impact the ability of the target to continue
     * operating normally.
     *
     * MsgType_FLOWCTRL
     * FLOWCTRL packets are provided for TCF compliance.  They are used to avoid
     * flooding of communication links.  In response, endpoints can either
     * increase or decrease rate of packets sent, although this is only loosely
     * defined. TCF services are not directly concerned with flow control.
     * The data passed with FLOWCTRL packets reports the traffic congestion level
     * as an integer between -100 and 100, where 0 = "optimal load"
     *
     * MsgType_DATA
     * DATA packets are used for sending event log data and other streams of data.
     * Details about how this will be used are being worked out to ensure alignment
     * with BIOS RTA Log Servers.
     *
     * MsgType_NACK_BAD_DATA:
     * Negative Acknowledge due to bad data.  Sent in response to a command that
     * had invalid data in the body of the packet.  The original data is echoed
     * in the NACK response.
     *
     * MsgType_NACK_WITH_ERROR_CODE
     * Negative Acknowledge with Error Code. Sent along with an error code in
     * the first word of the body of the reply that describes the error that was
     * encountered
     */
    enum MsgType {
        MsgType_ACK=0, /*! Reply acknowledging receipt of CMD or DATA packet */
        MsgType_CMD=1, /*! Command packets              */
        MsgType_RESULT=2, /*! Result reply                */
        MsgType_PARTIALRESULT=3, /*! Partial result reply */
        MsgType_NOTIFY=4, /*! Notify messaage (equiv. to the EVENT packet in TCF) */
        MsgType_FLOWCTRL=5, /*! Flow control packet      */
        MsgType_DATA=6, /*! Data packet (used for streaming data to host) */
        MsgType_RESERVED7,
        MsgType_RESERVED8,
        MsgType_RESERVED9,
        MsgType_RESERVED10,
        MsgType_RESERVED11,
        MsgType_RESERVED12,
        MsgType_RESERVED13,
        MsgType_NACK_BAD_DATA=14, /*! Negative Acknowledge due to bad data */
        MsgType_NACK_WITH_ERROR_CODE=15 /*! Negative Acknowledge -
                                            error code in msg body */
    };

    /*!
     * ======== NACKErrorCode ========
     * Message Negative Acknowledge Error Codes
     *
     * Stored in the first 32b word of the data body of the packet
     * in Network Byte Order
     */
    enum NACKErrorCode {
        NACKErrorCode_NO_REASON_SPECIFIED=0, /*! Use this when none of the
                                                 defined NACK error codes are
                                                 appropriate */
        NACKErrorCode_SERVICE_NOT_SUPPORTED=1, /*! A module that handles the
                                                   requested Service Id could
                                                   not be found */
        NACKErrorCode_CMD_NOT_SUPPORTED=2, /*! The service does not support the
                                               specified Command Id */
        NACKErrorCode_QUEUE_FULL=3, /*! Cmd couldn't be passed on to dest. end
                                        point due to a queue full condition */
        NACKErrorCode_BAD_ENDPOINT_ADDRESS=4, /*! The destination end point
                                                  address does not exists */
        NACKErrorCode_BAD_MESSAGE_LENGTH=5 /*! packet lenght > endpoint max.
                                               msg length or not what is
                                               required to service the cmd  */
    };

    /*!
     *  ======== HOST ========
     *  The address of the host
     *
     *  The host address is always 0xFFFF.
     */
    const UInt16 HOST = 0xFFFF;

    /*!
     *  ======== BROADCAST ========
     *  Used to denote a broadcast message
     *
     *  The broadcast address is always 0xFFFE.
     */
    const UInt16 BROADCAST = 0xFFFE;

    /*!
     * ========= maxPktLengthInBytes =========
     * maximum number of bytes a packet can contain
     *
     * used to limit the amount of memory needed by the
     * packet factories in order to handle a new packet
     */
    config Int maxPktLengthInBytes = 128;

    /*
     * See section 8.2.2 of spru187n.pdf for info on bitfield packing and
     * endianness. For big-endian mode, bit fields are packed into registers
     * from most significant bit (MSB) to least significant bit (LSB) in the
     * order in which they are defined. Bit fields are packed in memory from
     * most significant byte (MSbyte) to least significant byte (LSbyte).
     * For little-endian mode, bit fields are packed into registers from
     * the LSB to the MSB in the order in which they are defined, and packed
     * in memory from LSbyte to MSbyte.
     *
     * In general, different compilers will handle bitfields in different \
     * (i.e. incompatible)ways.  To overcome this, the header is treated as
     * an array of 32b words, and the bitfields are accessed via macros that
     * handle the masking and shifting operations needed to insert or extract
     * the information from the relevant bitfields
     */

     /*!
      * ======== swizzle ========
      * returns the value in big endian format
      */
    @Macro Bits32 swizzle(Bits32 value);

     /*!
      * ======== swizzle ========
      * returns the value in big endian format
      */
    @Macro Bits16 swizzle16(Bits16 value);

    /*!
     *  ======== getHdrType ========
     *  gets the packet header type from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro HdrType getHdrType(Hdr *pHdr);

    /*!
     *  ======== setHdrType ========
     *  sets the header type in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(HdrType hdrType) the new header type
     */
    @Macro Void setHdrType(Hdr *pHdr, HdrType hdrType);

    /*!
     *  ======== getPayloadEndianness ========
     *  gets the payload endianness bit from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro Bits32 getPayloadEndianness(Hdr *pHdr);

    /*!
     *  ======== setPayloadEndianness ========
     *  sets the payload endianness bit from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(Hdr* msgHdr)   Endianness of the payload
     */
    @Macro Bits32 setPayloadEndianness(Hdr *pHdr, PayloadEndian endianess);

    /*!
     *  ======== getMsgLength ========
     *  gets the packet length (in bytes) from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro Int32 getMsgLength(Hdr *pHdr);

    /*!
     *  ======== setMsgLength ========
     *  sets the packet length (in bytes) from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(pktLength) the new packet length
     *
     */
    @Macro Void setMsgLength(Hdr *pHdr, UInt16 pktLength);

    /*!
     *  ======== getEventLength ========
     *  Gets the packet length (in bytes) from the packet header
     *
     *  Includes the packet header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro Int32 getEventLength(Hdr *pHdr);

    /*!
     *  @_nodoc
     *  ======== getMinEventLength ========
     *  Gets the packet length (in bytes) from a MinEvent packet header
     *  (for testing).
     *
     *  Includes the packet header
     *
     *  @param(Hdr* pHdr)   pointer to the packet header
     */
    @Macro Int32 getMinEventLength(Hdr *pHdr);

    /*!
     *  ======== setEventLength ========
     *  Sets the packet length (in bytes) from the packet header
     *
     *  Includes the packet header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(pktLength) the new packet length
     *
     */
    @Macro Void setEventLength(Hdr *pHdr, Bits32 pktLength);
    
    /*!
     *  ======== setEventLengthFast ========
     *  Sets the packet length (in bytes) from the packet header
     *  by just writing the length, instead of reading the old
     *  value, setting the length, and writing it back.
     *
     *  Includes the packet header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(pktLength) the new packet length
     *
     */
    @Macro Void setEventLengthFast(Hdr *pHdr, Bits32 pktLength);
    
    /*!
     * ======== setMinEventPacketLength ========
     * Set UIA packet length of the completed packet
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(pktLength) the new packet length
     *  @param(loggerInstanceId) the logger's instanceId
     *  @param(senderAdrs) the sender address (i.e. DNUM or endpoint ID, 0 if only 1 CPU per device)
     */
    @Macro Void setMinEventPacketLength(Hdr *pHdr, Bits32 pktLength, 
                                UInt16 loggerInstanceId, 
                                UInt16 senderAdrs);
    /*!
     *  ======== getLength ========
     *  Gets the packet length (in bytes) from the packet header
     *
     *  Includes the packet header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    Int32 getLength(Hdr *pHdr);

    /*!
     *  ======== getSequenceCount ========
     *  gets the sequence count from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro UInt16 getSequenceCount(Hdr *pHdr);

    /*!
     *  ======== setSequenceCount ========
     *  sets the packet sequence count in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(seqCount) the new packet sequence count
     */
    @Macro Void setSequenceCount(Hdr *pHdr, Bits16 seqCount);
    
    /*!
     *  ======== setSequenceCountFast ========
     *  Sets the packet sequence count in the packet header
     *  by directly writing the word, and not reading it first.
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(seqCount) the new packet sequence count
     */
    @Macro Void setSequenceCountFast(Hdr *pHdr, Bits16 seqCount);
    
    /*!
     *  ======== setSequenceCounts ========
     *  sets the packet sequence count and the event sequence count for the first 
     *  event in the packet header
     *      
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *   
     *  @param(Hdr* pHdr)   pointer to the packet header
     *  @param(pktSeqCount) the new packet sequence count
     *  @param(eventSeqCount) the event sequence count for the first event in the packet
     */    
    @Macro Void setSequenceCounts(Hdr *pHdr, Bits16 pktSeqCount, Bits16 eventSeqCount );
    
    /*!
     *  ======== setMinEventPacketSequenceCountFast ========
     *   stores the current event sequence number and 
     *   12 msbs of last timestamp in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* pHdr)   pointer to the packet header
     *  @param(pktSeqCount) the new packet sequence count
     *  @param(eventSeqCount) the event sequence count for the first event in the packet
     */
    @Macro Void setMinEventPacketSequenceCount(Hdr *pHdr,  Bits16 pktSeqCount, Bits16 eventSeqCount);

    /*! @_nodoc
     *  ======== getLoggerPriority ========
     *  gets the logger priority field from the event packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @a(return) the priority of the logger that logged the events
     *  @see IUIATransfer#Priority
     */
    @Macro IUIATransfer.Priority getLoggerPriority(Hdr *pHdr);

    /*! @_nodoc
     *  ======== setLoggerPriority ========
     *  sets the logger priority field in the event packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(priority) the logger priority ( @see IUIATransfer#Priority )
     */
    @Macro Void setLoggerPriority(Hdr *pHdr, IUIATransfer.Priority priority);
    /*!
     *  ======== getLoggerModuleId ========
     *  gets the logger ID from the event packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro Bits16 getLoggerModuleId(Hdr *pHdr);

    /*!
     *  ======== setLoggerModuleId ========
     *  sets the module ID in the event packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(loggerModuleId) the moduleID of the logger that logged the events
     */
    @Macro Void setLoggerModuleId(Hdr *pHdr, Bits16 loggerModuleId);

    /*!
     *  ======== getLoggerInstanceId ========
     *  gets the logger instance ID from the event packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro Bits16 getLoggerInstanceId(Hdr *pHdr);

    /*!
     *  ======== setLoggerInstanceId ========
     *  sets the sequence count in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(seqCount) the new packet sequence count
     */
    @Macro Void setLoggerInstanceId(Hdr *pHdr, Bits16 loggerInstanceId);

    /*!
     *  ======== getMsgType ========
     *  gets the packet type from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* pHdr)   pointer to the packet header
     */
    @Macro MsgType getMsgType(Hdr *pHdr);

    /*!
     *  ======== setMsgType ========
     *  sets the packet type in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(msgType) the new packet type
     */
    @Macro Void setMsgType(Hdr *pHdr, MsgType msgType);

    /*!
     *  ======== getCmdId ========
     *  gets the packet type from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro UInt16 getCmdId(Hdr *pHdr);

    /*!
     *  ======== setCmdId ========
     *  sets the command Id in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(cmdId) the new command Id
     */
    @Macro Void setCmdId(Hdr *pHdr, UInt16 cmdId);

    /*!
     *  ======== getServiceId ========
     *  gets the packet type from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro UInt16 getServiceId(Hdr *pHdr);
    /*!
     *  ======== setServiceId ========
     *  sets the packet type in the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(serviceId) the new service Id
     */
    @Macro Void setServiceId(Hdr *pHdr, UInt16 serviceId);

    /*!
     * ======== getTag =========
     * gets the 32b tag field from the packet header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro Bits32 getTag(Hdr *pHdr);

    /*!
     * ======== setTag =========
     * sets the 32b tag field in the packet header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(tagValue) the tag value to store in the header
     */
    @Macro Void setTag(Hdr *pHdr, Bits32 tagValue);

    /*!
     *  ======== getDestAdrs ========
     *  gets the packet destination address from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  Example1 :
     *   Void example1(UIAPacket_Object *obj){
     *      Int16 destAdrs = UIAPacket_getDestAdrs(&obj->hdr);
     *      ...
     *   }
     *  Example2 :
     *    Void example2(UIAPacket_Hdr *pHdr){
     *      Int16 destAdrs = UIAPacket_getDestAdrs(pHdr);
     *      ...
     *    }
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro UInt16 getDestAdrs(Hdr *pHdr);

    /*!
     *  ======== setDestAdrs ========
     *  sets the packet destination address in the packet header
     *
     *  The destination address identifies which endpoint (i.e. which
     *  CPU or process) the packet should be sent to.
     *  0 is used to broadcast the packet.
     *  1 is used for point-to-point connections.
     *  Replies to a packet always use the sender address in the
     *  originating packet as the destination address.
     *  Destination addresses other than the above can be determined
     *  via a discovery process.
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(destAdrs) the new destination address
     */
    @Macro Void setDestAdrs(Hdr *pHdr, UInt16 destAdrs);

    /*!
     *  ======== getSenderAdrs ========
     *  gets the packet sender address from the packet header
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     */
    @Macro UInt16 getSenderAdrs(Hdr *pHdr);

    /*!
     *  ======== setSenderAdrs ========
     *  sets the packet sender address in the packet header
     *
     *  The sender address identifies which endpoint (i.e. which
     *  CPU or process) the packet is coming from
     *  1 is used by default.
     *  Sender addresses other than 1 can be set via configuration
     *  (e.g. to identify which CPU core a packet originated from).
     *
     *  Message header is always layed out as big endian (network order).
     *  This macro is designed to work on both little endian and
     *  big endian targets, since 32b long words are consistently
     *  handled for both endianness - it is only when converting
     *  from bytes to longs that bit orders get swapped.
     *  #ifdef xdc_target__bigEndian can be used if necessary.
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(senderAdrs) the new destination address
     */
    @Macro Void setSenderAdrs(Hdr *pHdr, UInt16 senderAdrs);

    /*!
     * ======== initMsgHdr ========
     * initializes all bitfields in the message header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(endianness) endianness of the payload
     *  @param(msgType) the packet type
     *  @param(msgLength) the number of Bytes in the packet, including the
     *     header and footer
     *  @param(serviceId) the service Id
     *  @param(seqCount) the sequence number for the packet
     *  @param(cmdId) the command Id
     *  @param(tag) the tag value to store in the header
     *  @param(destAdrs) the destination address
     *  @param(senderAdrs) the sender address
     */
    @Macro Void initMsgHdr(Hdr *pHdr, PayloadEndian endianness,
                           MsgType msgType, UInt16 msgLength,
                           UInt16 serviceId, UInt16 seqCount,
                           UInt16 cmdId,  UInt32 tag,
                           UInt16 destAdrs, UInt16 senderAdrs);

    /*!
     * ======== initEventRecHdr ========
     * initializes all bitfields in the message header
     *
     *  @param(Hdr* msgHdr)   pointer to the packet header
     *  @param(endianness) endianness of the payload
     *  @param(eventLength) the number of Bytes in the packet, including
     *     the header and footer
     *  @param(seqCount) the sequence number for the packet
     *  @param(priority) the logger's priority
     *  @param(moduleId) the logger's module Id
     *  @param(instanceId) the logger's instanceId
     *  @param(destAdrs) the destination address
     *  @param(senderAdrs) the sender address
     */
    @Macro Void initEventRecHdr(Hdr *pHdr, PayloadEndian endianness,
                                UInt32 eventLength, UInt16 seqCount,
                                IUIATransfer.Priority priority, UInt16 moduleId,
                                UInt16 instanceId, UInt16 destAdrs,
                                UInt16 senderAdrs);

    /*!
     * ======== initMinEventRecHdr ========
     * initializes all bitfields in the minimum event packet header,
     * setting the length, sequence count and last timestamp fields to 0
     *
     *  @param(Hdr* pPktHdr)   pointer to the packet header
     *  @param(endianness) endianness of the payload
     *  @param(loggerInstanceId) the logger's instanceId
     *  @param(senderAdrs) the sender address (i.e. DNUM / endpoint ID for the CPU core)
     */
    @Macro Void initMinEventRecHdr(Hdr *pHdr, PayloadEndian endianness, 
                                UInt16 loggerInstanceId, 
                                UInt16 senderAdrs);
    /*!
     * ======== getFooter ========
     * returns the integer to use as the packet footer
     *
     *  @param(Hdr* pHdr)   pointer to the packet header
     */
    @Macro Int32 getFooter(Hdr *pHdr);

    /*!
     * ======== setInvalidHdr ========
     * Used to set the header type as invalid
     *
     *  @param(Hdr* pHdr)   pointer to the packet header
     *  @param(eventLength) the number of Bytes in the packet of memory
     *       that is invalid (including the first word of the 32-bit hdr)
     */
    @Macro Void setInvalidHdr(Hdr *pHdr, UInt32 eventLength);
}
