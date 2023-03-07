/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 * ======== UIAMetaData.xdc
 */

import xdc.runtime.Types;

/*!
 * @_nodoc
 *
 * Unified Arch. Hardware Interrupt Context Instrumentation
 *
 * The UIAHWICtx module defines context change events
 * and methods that allow tooling to identify hardware interrutp context
 * switches and to enable HWI-aware filtering, trace and
 * analysis.
 */

module UIAMetaData {

    /*!
     *  ======== UploadMode ========
     */
    enum UploadMode {
        UploadMode_SIMULATOR = 1, /* note that simulators use probe points */
        UploadMode_PROBEPOINT = 2,
        UploadMode_JTAGSTOPMODE = 3,
        UploadMode_JTAGRUNMODE = 4,
        UploadMode_NONJTAGTRANSPORT = 5,
        UploadMode_CUSTOM = 6
    }

    /*!
     *  ======== cpuFreq ========
     *  CPU frequency in Hz
     *
     *  Use this configuration parameter to set the CPU frequency, for
     *  non-BIOS programs.
     *
     *  @a(Example)
     *  If CPU frequency is 720MHz, the following configuration script
     *  configures the proper clock frequency:
     *  @p(code)
     *     var UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
     *     UIAMetaData.cpuFreq.lo = 720000000;
     *  @p
     */
    metaonly config Types.FreqHz cpuFreq;

    /*!
     *  ======== timestampFreq ========
     *  Timestamp frequency in Hz
     *
     *  Use this configuration parameter to set the timestamp frequency, for
     *  non-BIOS programs.
     *
     *  @a(Example)
     *  If timestamp frequency is 720MHz, the following configuration script
     *  configures the proper timestamp frequency:
     *  @p(code)
     *     var UIAMetaData = xdc.useModule('ti.uia.runtime.UIAMetaData');
     *     UIAMetaData.timestampFreq.lo = 720000000;
     *  @p
     */
    metaonly config Types.FreqHz timestampFreq;

    metaonly function generateXML();

    /*!
     *  ======== genXmlEntry ========
     *  write an entry into an xml file
     *
     *  e.g. <myTag>myValue</myTag>\n
     */
    metaonly function genXmlEntry(indent, tag, value);

    /*!
     *  ======== genXmlEntryOpen ========
     *  write an entry into an xml file with a key field
     *
     *  e.g. <myTag myKeyName=myKeyValue>
     */
    metaonly function  genXmlEntryOpen(indent,  tag);

    /*!
     *  ======== genXmlEntryOpenWithKey ========
     *  write an entry into an xml file with a key field
     *
     *  e.g. <myTag myKeyName=myKeyValue>
     */
    metaonly function  genXmlEntryOpenWithKey(indent,tag,keyName,keyValue,value);

    /*!
     *  ======== genXmlEntryWithKey ========
     *  write an entry into an xml file with a key field
     *
     *  e.g. <myTag myKeyName=myKeyValue>myValue</myTag>\n
     */
     metaonly function  genXmlEntryWithKey(indent,tag,keyName,keyValue,value);

     /*!
      *  ======== genXmlComment ========
      *  add an XML style comment to the active xml file
      */
     metaonly function  genXmlComment(indent,  comment);
    /*!
     *  ======== genXmlEntryClose ========
     *  write an entry into an xml file with a key field
     *
     *  e.g. </myTag>\n
     */
    metaonly function  genXmlEntryClose(indent,  tag);

    /*!
     *  ======== setTransportFields ========
     *  Function to populate the transport fields
     *
     *  This function is used to populate the UIA XML file with
     *  transport information. It  writes <transport2> entries.
     *  The <transport2> is supported by System Analyzer in 5.2
     *  and later.
     *
     *  The parameters map directly to individual fields in the
     *  <transport2> entries. For example
     *
     *  @a(Example)
     *  UIAMetaData.setTransportFields(false, "UART", "UIAPACKET",
     *                      true, "timestamps=32bits,sequence=disabled");
     *
     *  would yield
     *
     * <transport2>
     *    <isMultiCoreTransport>false</isMultiCoreTransport>
     *    <supportControl>true</supportControl>
     *    <format>UIAPacket</format>
     *    <transportType>UART</transportType>
     *    <customFields>
     *       <timestamps>32bits</timestamps>
     *       <sequence>disabled</sequence>
     *    </customFields>
     * </transport2>
     *  @p
     *
     *  This function can be called multiple times.
     *
     *  @param(isMultiCoreTransport) Boolean whether it is a multicore
     *                               transport. If true, then multiple
     *                               cores route their events through
     *                               the transport on this core.
     *  @param(transportType)        The type of transport (e.g. UART)
     *  @param(format)               The format of the data (e.g. UIAPACKET)
     *  @param(supportControl)       Boolean whether the transport can receive
     *                               control messages from the instrumentation
     *                               host.
     *  @param(customFields)         Custom fields. Fields are comma separated.
     *                               Field name/values are separated by an
     *                               '='.
     */
    metaonly function setTransportFields(isMultiCoreTransport, uploadMode,
                                         transportType, format, supportControl,
                                         customFields);

    /*! @_nodoc
     * ======== setLoggingSetupConfigured ========
     * Called by the LoggingSetup module to indicate it is in the .cfg file
     *
     * Sets an internal metaonly flag that is used to determine the transport used.
     */
    metaonly function setLoggingSetupConfigured();

    /*! @_nodoc
     * ======== isLoggingSetupConfigured ========
     * Called by other modules to see if the LoggingSetup module is in the .cfg file
     *
     * Returns true if the LoggingSetup module can be used to determine the transport used.
     */
    metaonly function isLoggingSetupConfigured();

    /*! @_nodoc
     * ======== setLogSyncInfo ========
     * Called by the LogSync module to provide the module name and isEnabled flag
     */
    metaonly function setLogSyncInfo(moduleName, isEnabled);

    /*!
     *  ======== overrideCpuFreq ========
     *  True if app has set cpuFreq.
     *
     *  @_nodoc
     */
    metaonly config bool overrideCpuFreq = false;

    /*!
     *  ======== overrideTimestampFreq ========
     *  True if app has set timestampFreq.
     *
     *  @_nodoc
     */
    metaonly config bool overrideTimestampFreq = false;
}
