/*
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
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
 *
 */

/*
 *  ======== ble_peripheral_docs.js ========
 */

"use strict";

// Long description for the paramUpdateReq configuration parameter
const paramUpdateReqLongDescription = `Defines how to handle a Parameter Update\
Request.\n
__Default__: Pass to App\n

Options for responding to connection parameter update requests:\n
Options | Description
--- | ---
Accept all | Accept all parameter update requests
Deny all | Deny all parameter update requests
Pass to app | Pass a GAP_UPDATE_LINK_PARAM_REQ_EVENT to the app for it to \
decide by responding with GAP_UpdateLinkParamReqReply
`

// Long description for the sendParamsUpdateReq configuration parameter
const sendParamsUpdateReqLongDescription = `Sends GAP_UpdateLinkParamReq after connection establishment.\n

__Default__: True (checked)`

  // Exports the long descriptions for each configurable
  exports = {
    paramUpdateReqLongDescription: paramUpdateReqLongDescription,
    sendParamsUpdateReqLongDescription: sendParamsUpdateReqLongDescription,
};