/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 * ======== EventHdr.xdc ========
 */
package ti.uia.runtime;

/*!
 *  ======== EventHdr ========
 *  Module defines the header format to be used when logging an event
 *
 *  Each event's first word includes the type, length of the event and
 *  sequence number. The length includes the first word and is in bytes.
 *
 *  The following is the desciption of the first word.
 *
 *  @p(code)
 *  EventHdr
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  |---------------------------------------------------------------|
 *  |H H H H H|L L L L L L L L L L L|S S S S S S S S S S S S S S S S|
 *  |---------------------------------------------------------------|
 *
 *  H = HdrType         (5-bits)
 *  L = Event Length    (11-bits)
 *  S = Sequence Number (16-bits)
 *  @p
 *
 *  There are currently 4 different types of events supported in UIA. The
 *  following discusses the format of each one of the types.
 *
 *  @p(code)
 *  HdrType_Event
 *    word0: EventHdr
 *    word1: event Id (top 16 bits) & module Id (bottom 16 bits)
 *
 *  HdrType_EventWithTimestamp
 *    word0: EventHdr
 *    word1: Timestamp lower 32 bits
 *    word2: Timestamp upper 32 bits
 *    word3: event Id (top 16 bits) & module Id (bottom 16 bits)
 *
 *  HdrType_EventWithSnapshotId
 *    word0: EventHdr
 *    word1: event Id (top 16 bits) & module Id (bottom 16 bits)
 *    word2: filename pointer
 *    word3: linenum
 *    word4: snapshotId
 *    word5: address where the data was located
 *    word6: total length of data (top 16-bits)
 *           length for this record (bottom 16 bits)
 *    word7: format pointer
 *    data:  the rest of the record contains the data
 *
 *  HdrType_EventWithSnapshotIdAndTimestamp:
 *    word0: EventHdr
 *    word1: Timestamp lower 32 bits
 *    word2: Timestamp upper 32 bits
 *    word3: event Id (top 16 bits) & module Id (bottom 16 bits)
 *    word4: filename pointer
 *    word5: linenum
 *    word6: snapshotId
 *    word7: address where the data was located
 *    word8: total length of data (top 16-bits)
 *           length for this record (bottom 16 bits)
 *    word9: format pointer
 *    data:  the rest of the record contains the data
 *  @p
 *
 *  Snapshot events can span multiple records. The snapshotId is used to
 *  correlate the records when this occurs.
 *  The length field (word6 or word8) contains two lengths. The bottom 16 bits
 *  length of data for this record. The top 16 bits is the sum of all
 *  lengths for this specific snapshotId.
 *
 *  The address field points to the location of the data. When the snapshot
 *  spans multiple records, the address field is updated accordingly. Therefore
 *  this field can be used to detect dropped records. Note the snapshot
 *  event data is contiguous.
 *
 *  EventHdr for HdrType_EventWithPrevLen* and HdrType_SnapshotEventWithPrevLen*
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  |---------------------------------------------------------------|
 *  |H H H H H|L L L L L L L L L L L|P P P P P P P P P P P|S S S S S|
 *  |---------------------------------------------------------------|
 *
 *  H = HdrType         (5-bits)
 *  L = Event Length in bytes    (11-bits: # of bytes in current event)
 *  P = Number of Bytes in Previous Event (5-bits: # of bytes in prev. event)
 *  S = Sequence Number (5-bits)
 */
@CustomHeader
module EventHdr {
    /*!
     *  ======== HdrType ========
     *  Enumeration of the various types of events headers
     *
     *  Stored in a 5 bit bitfield (b31-b27) of the first word in the event.
     */
    enum HdrType {
        HdrType_Event = 0, /*! Event with no timestamp or other optional parameters */
        HdrType_EventWithTimestamp = 1, /*! Event with 64 bit Timestamp */
        HdrType_EventWithSnapshotId = 2, /*! Snapshot event */
        HdrType_EventWithSnapshotIdAndTimestamp = 3, /*! Snapshot event with 64 bit Timestamp */
        HdrType_EventWith32bTimestamp = 4, /*! Event with 32 bit Timestamp */
        HdrType_EventWithTimestampAndEndpointId = 5,
        HdrType_EventWithSnapshotIdAnd32bTimestamp = 6, /*! Snapshot event with 32 bit Timestamp */
        HdrType_EventWithSnapshotIdAndTimestampAndEndpointId = 7,
        HdrType_EventWithPrevLenWithNoTimestamp = 8, /*! reverse param event with prev. length field */
        HdrType_EventWithPrevLenWith32bTimestamp = 9,  /*! reverse param event with prev. length field */
        HdrType_EventWithPrevLenWith64bTimestamp = 10,  /*! reverse param event with prev. length field */
        HdrType_SnapshotEventWithPrevLen = 11, /*! Snapshot event with no timestamp*/
        HdrType_SnapshotEventWithPrevLenAnd64bTimestamp = 12, /*! Snapshot event with 64 bit Timestamp */
        HdrType_Reserved13 = 13, /*! reserved for future use */
        HdrType_Reserved14 = 14, /*! reserved for future use */
        HdrType_Reserved15 = 15, /*! reserved for future use */
        HdrType_Reserved16 = 16, /*! reserved for future use */
        HdrType_Reserved17 = 17, /*! reserved for future use */
        HdrType_Reserved18 = 18, /*! reserved for future use */
        HdrType_Reserved19 = 19, /*! reserved for future use */
        HdrType_Reserved20 = 20, /*! reserved for future use */
        HdrType_Reserved21 = 21, /*! reserved for future use */
        HdrType_Reserved22 = 22, /*! reserved for future use */
        HdrType_Reserved23 = 23, /*! reserved for future use */
        HdrType_Reserved24 = 24, /*! reserved for future use */
        HdrType_Reserved25 = 25, /*! reserved for future use */
        HdrType_Reserved26 = 26, /*! reserved for future use */
        HdrType_Reserved27 = 27, /*! reserved for future use */
        HdrType_Reserved28 = 28, /*! reserved for future use */
        HdrType_Reserved29 = 29, /*! reserved for future use */
        HdrType_Reserved30 = 30, /*! reserved for future use */
        HdrType_Reserved31 = 31  /*! reserved for future use */
    };

    /*!
     *  ======== getHdrType ========
     *  Gets the message header type from the first word of the event header
     *
     *  @param(eventWord1)   first word of the event header
     *
     *  @a(returns)          HdrType of the event
     */
    @Macro HdrType getHdrType(UInt32 eventWord1);

    /*!
     *  ======== setHdrType ========
     *  Sets the header type in the event header
     *
     *  @param(eventWord1)   the first word of the event header to be updated
     *  @param(HdrType hdrType)  the new header type
     */
    @Macro Void setHdrType(UInt32 eventWord1, EventHdr.HdrType hdrType);

    /*!
     *  ======== getLength ========
     *  gets the event length (in bytes) from the event header
     *
     *  @param(eventWord1)   first word of the event header
     */
    @Macro SizeT getLength(UInt32 eventWord1);

    /*!
     *  ======== setLength ========
     *  sets the event length (in bytes) in the event header
     *
     *  @param(eventWord1)   first word of the event header
     *  @param(eventLength)    the new event length
     */
    @Macro Void setLength(UInt32 eventWord1, SizeT eventLength);

    /*!
     *  ======== getSeqCount ========
     *  gets the sequence count from the message header
     *
     *  @param(eventWord1)   first word of the event header
     */
    @Macro UInt16 getSeqCount(UInt32 eventWord1);

    /*!
     *  ======== setSeqCount ========
     *  sets the sequence count in the message header
     *
     *  @param(eventWord1)   first word of the event header
     *  @param(seqCount)     the new message sequence count
     */
    @Macro Void setSeqCount(UInt32 eventWord1, UInt16 seqCount);

    /*!
     *  ======== genEventHdrWord1 ========
     *  generates the first word to use in an Event header
     *
     *  @param(numBytesInEvent)  number of bytes in the event
     *  @param(seqCount)            sequence count number to tag the event with
     *  @param(eventHdrType)        type of event header for this event
     */
    @Macro UInt32 genEventHdrWord1(SizeT numBytesInEvent, UInt16 seqCount,
                                   EventHdr.HdrType eventHdrType);

    /*!
     *  ======== genEventWithPrevLenHdrWord1 ========
     *  generates the first word to use in a header that contains the length of the previous event
     *
     *  @param(numBytesInEvent)  number of bytes in the event
     *  @param(numBytesInPrevEvent)  number of bytes in the previous event
     *  @param(seqCount)            sequence count number to tag the event with
     *  @param(eventHdrType)        type of event header for this event
     */
    @Macro UInt32 genEventWithPrevLenHdrWord1(SizeT numBytesInEvent, SizeT numBytesInPrevEvent, UInt16 seqCount,
                                   EventHdr.HdrType eventHdrType);

    /*!
     *  @_nodoc
     *  ======== getEventHdrWithPrevLenSeqCount ========
     *  Extract the sequence number from an EventHdr with previous length.
     *  For testing.
     */
    @Macro UInt32 getEventHdrWithPrevLenSeqCount(UInt32 word);

    /*!
     *  @_nodoc
     *  ======== getPrevLength ========
     *  Extract the length of the previous event from an EventHdr with previous
     *  length.  For testing.
     */
    @Macro UInt32 getPrevLength(UInt32 word);
}
