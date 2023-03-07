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
 *  ======== DvtTypes.xdc ========
 */

package ti.uia.events;
import xdc.runtime.Diags;

/*!
 *  ======== DvtTypes ========
 *  Constants and types used by DVT meta data
 */

module DvtTypes {

    metaonly enum DvtAnalysisType {
        DvtAnalysisType_START,
        DvtAnalysisType_STOP,
        DvtAnalysisType_STARTSTOP,
        DvtAnalysisType_FUNCTIONENTRY,
        DvtAnalysisType_FUNCTIONEXIT,
        DvtAnalysisType_SYNCPOINT,
        DvtAnalysisType_CONTEXTCHANGE,
        DvtAnalysisType_STATECHANGE,
        DvtAnalysisType_MESSAGEMONITOR,
        DvtAnalysisType_STATISTIC,
        DvtAnalysisType_SEMAPHORE,
        DvtAnalysisType_LOCK,
        DvtAnalysisType_EVENTMARKER,
        DvtAnalysisType_MEMORYSNAPSHOT,
        DvtAnalysisType_STRINGSNAPSHOT,
        DvtAnalysisType_NAMESNAPSHOT,
        DvtAnalysisType_STACKSNAPSHOT,
        DvtAnalysisType_REGISTERSNAPSHOT,
        DvtAnalysisType_CUSTOM
    }

    metaonly enum DvtDataDesc {
        DvtDataDesc_TIMESTAMP32,
        DvtDataDesc_TIMESTAMP64_MSW,
        DvtDataDesc_TIMESTAMP64_LSW,
        DvtDataDesc_TIMESTAMP_CTIME,

        DvtDataDesc_COOKIE,
        DvtDataDesc_SIZE,

        DvtDataDesc_PROGRAMADRS,
        DvtDataDesc_FUNCTIONADRS,
        DvtDataDesc_DATAADRS,
        DvtDataDesc_STRINGADRS,

        DvtDataDesc_INSTANCE,
        DvtDataDesc_INITIATOR,
        DvtDataDesc_PROCESSID,
        DvtDataDesc_THREADID,
        DvtDataDesc_TASKID,
        DvtDataDesc_CHANNELID,
        DvtDataDesc_PACKETID,
        DvtDataDesc_FRAMEID,
        DvtDataDesc_APPID,
        DvtDataDesc_HWIID,
        DvtDataDesc_SWIID,
        DvtDataDesc_STATEID,
        DvtDataDesc_STATESTRINGADRS,

        DvtDataDesc_COUNTER,
        DvtDataDesc_VALUE,
        DvtDataDesc_FLAG,
        DvtDataDesc_SEMAPHORE,
        DvtDataDesc_RETURNVALUE,
        DvtDataDesc_PARAMVALUE,
        DvtDataDesc_EXPECTEDVALUE,

        DvtDataDesc_TESTNUMBER,
        DvtDataDesc_RESULT,
        DvtDataDesc_ERRORCODE,
        DvtDataDesc_EVENTCODE,

        DvtDataDesc_FILENAMESTR,
        DvtDataDesc_LINENUM,
        DvtDataDesc_FMTSTR,

        DvtDataDesc_GLOBALSYMBOLADRS,
        DvtDataDesc_ENUMVALUE,
        DvtDataDesc_LENGTHINMAUS,
        DvtDataDesc_DATAARRAY,
        DvtDataDesc_REFERENCEID,
        DvtDataDesc_SNAPSHOTID,
        DvtDataDesc_CUSTOM
    }

    metaonly struct ParamDescriptor {
        String name;
        DvtDataDesc dataDesc;
        String strCustomDataDesc;
        String dataTypeName;
        String units;
        Bool isHidden;
        Int  lsb;
    }

    /*!
     *  ======== EventDescriptor ========
     *  @_nodoc
     *  DVT event descriptor.
     */
    metaonly struct MetaEventDescriptor {
        String versionId;
        DvtAnalysisType analysisType;
        String strCustomAnalysisType;
        String displayText;
        String tooltipText;
        Bool isMultiEventRecordItem;
        Int multiEventRecordItemIndex;
        Int multiEventRecordLength;
        Int numParameters;
        ParamDescriptor paramInfo[];
    }

}
