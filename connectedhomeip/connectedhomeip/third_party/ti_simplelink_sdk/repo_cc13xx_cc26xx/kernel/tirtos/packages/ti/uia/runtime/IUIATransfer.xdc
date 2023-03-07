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

/*!
 *  ======== IUIATransfer ========
 */
interface IUIATransfer inherits xdc.runtime.IFilterLogger {

    enum TransferType {
        TransferType_RELIABLE = 0,
        TransferType_LOSSY = 1
    }

    /*!  @_nodoc
     * ======== Logger Priority enumeration ========
     */
    enum Priority {
        Priority_LOW = 0,       /*! low priority */
        Priority_STANDARD = 1,  /*! default priority */
        Priority_HIGH = 2,      /*! use for critical errors, etc. */
        Priority_SYNC = 3       /*! used only by LogSync's logger */
    };

    /*!
     * ======== getPtrToQueueDescriptorMeta ========
     * Each logger instance has a unique queue descriptor address that is
     * stored in the Event Record header to identify itself to the host.
     * This metaonly configuration parameter allows the UIA Metadata to
     * determine what the address is for each statically created logger instance
     * in order to emit XML code to allow the host to look up information about
     * the logger instance (such as its name) based on the queue descriptor
     * address that is stored in the event record header.
     *
     * The pointer is returned per instance of the logger module. The
     *  instance object is passed to the function as the first argument.
     */
     metaonly function getPtrToQueueDescriptorMeta(inst);
    /*!
     * ======== setPtrToQueueDescriptorMeta ========
     * Sets the queue descriptor address in the logger's object instance data.
     */
     metaonly function setPtrToQueueDescriptorMeta(inst,queueDescriptorAdrs);
     /*!
     *  ======== getLoggerInstanceId ========
     *  returns the id of this logger instance.
     */
    metaonly function getLoggerInstanceId(inst);

    /*! @_nodoc
     *  ======== getLoggerPriority ========
     *  returns the priority of this logger instance.
     */
    metaonly function getLoggerPriority(inst);

    /*! @_nodoc
     *  ======== setLoggerPriority ========
     *  sets the priority of this logger instance.
     */
    metaonly function setLoggerPriority(inst, priority);

    /*!
     *  ======== MetaData ========
     *  This data is added to the RTA MetaData file to support stop mode RTA.
     */
    @XmlDtd metaonly struct MetaData {
        Int instanceId;
        Int priority;
    }

instance:
    config TransferType transferType = TransferType_LOSSY;
    TransferType getTransferType();

    /*!
     *  ======== getContents =========
     *  Fills buffer that is passed in with unread data, up to size bytes
     *  in length.
     *
     *  The logger is responsible for ensuring that no partial event records
     *  are stored in the buffer.  Bytes are in target endianness.
     *
     *  @param(hdrBuf)   Ptr to a buffer that is at least <size> bytes in length
     *  @param(size)     The max number of bytes to be read into the buffer
     *  @param(cpSize)   The number of bytes actually copied
     *
     *  @a(return)       returns false if logger has no more records to read
     */
    @DirectCall
    Bool getContents(Ptr hdrBuf, SizeT size, SizeT *cpSize);

    /*!
     *  ======== isEmpty =========
     *  Returns true if the transfer buffer has no unread data
     *
     *  @a(return)  true if no unread data
     */
    @DirectCall
    Bool isEmpty();

    /*!
     *  ======== getMaxLength =========
     */
    @DirectCall
    SizeT getMaxLength();

    /*!
     *  ======== getInstanceId ========
     *  Returns an ID value that uniquely identifies this instance of the logger.
     *
     *  Note that a value of 0 is reserved to indicate that the instance ID has
     *  not been initialized yet and a unique value needs to be generated.
     */
    @DirectCall
    UInt16 getInstanceId();

    /*! @_nodoc
     *  ======== priority ========
     *  The priority of the logger is used to detrmine which event packets to
     *  transfer to the host first.
     *
     *  A value of Priority_STANDARD (default) indicates normal priority.
     *  Higher values indicate higher priority.
     *  @see #Priority
     */
    config IUIATransfer.Priority priority = Priority_STANDARD;

    /*! @_nodoc
     *  ======== getPriority ========
     *  Returns a 2b value that identifies the relative priority of the event
     *  log.
     *
     *  A value of 1 (default) indicates normal priority.  Higher values indicate
     *  higher priority.
     *  @see #Priority
     */
    @DirectCall
    Priority getPriority();


    /*! @_nodoc
     *  ======== setPriority ========
     *  Sets a 2b value that identifies the relative priority of the event
     *  log.
     *
     *  A value of 1 (default) indicates normal priority.  Higher values indicate
     *  higher priority.
     *  @see #Priority
     */
    @DirectCall
    Void setPriority(Priority priority);

    /*!
     *  ======== reset ========
     *  Reset a log to empty state and enable it
     *
     *  @a(WARNING)  This method is not synchronized with other instance
     *  methods and, as a result, it must never be called when there is a
     *  chance that another instance method is currently in operation or
     *  when another method on this instance may preempt this call.
     */
    Void reset();

    metaonly config Ptr ptrToQueueDescriptorMeta = null;
}
