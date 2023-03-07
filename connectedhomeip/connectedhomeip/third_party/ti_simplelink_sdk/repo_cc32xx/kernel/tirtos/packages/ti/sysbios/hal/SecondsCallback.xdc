/*
 * Copyright (c) 2014-2019, Texas Instruments Incorporated
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
 */
/*
 *  ======== SecondsCallback.xdc ========
 *
 */
package ti.sysbios.hal;

/*!
 *  ======== SecondsCallback ========
 *  A Seconds module that calls user-supplied callbacks
 *
 *  ISeconds implementation that uses user callback functions
 *  This module provides a implementation of the `{@link ISystemSupport}`
 *  interface that simply calls back the user defined functions to enable the
 *  System module's functionality.
 *
 *  Configuration is as shown below.
 *  @p(code)
 *
 *  var SecondsCallback = xdc.useModule('xdc.runtime.SecondsCallback');
 *  SecondsCallback.getFxn = "&userGetSeconds";
 *  SecondsCallback.setFxn = "&userSetSeconds";
 *  SecondsCallback.getTimeFxn = "&userGetTime";
 *  SecondsCallback.setTimeFxn = "&userSetTime";
 *
 *  @p
 */
module SecondsCallback inherits ti.sysbios.interfaces.ISeconds
{
    /*!
     *  ======== GetFxn ========
     *  'get' function signature
     */
    typedef UInt32 (*GetFxn)();

    /*!
     *  ======== SetFxn ========
     *  'set' function signature
     */
    typedef Void (*SetFxn)(UInt32);

    /*!
     *  ======== GetTimeFxn ========
     *  'getTime' function signature
     */
    typedef UInt32 (*GetTimeFxn)(SecondsCallback.Time *);

    /*!
     *  ======== SetFxn ========
     *  'set' function signature
     */
    typedef UInt32 (*SetTimeFxn)(SecondsCallback.Time *);

    /*!
     *  ======== getFxn =========
     *  User supplied 'get' function
     *
     *  This function is called when the application calls
     *  `{@link Seconds#get()}` function.
     *
     *  By default, this function is configured with a default function
     *  that returns 0.
     */
    config GetFxn getFxn = "&ti_sysbios_hal_SecondsCallback_defaultGet";

    /*!
     *  ======== setFxn =========
     *  User supplied 'set' function
     *
     *  This function is called when the application calls
     *  `{@link Seconds#set()}` function.
     *
     *  By default, this function is configured with a default function
     *  that does nothing.
     */
    config SetFxn setFxn = "&ti_sysbios_hal_SecondsCallback_defaultSet";

    /*!
     *  ======== getTimeFxn =========
     *  User supplied 'getTime' function
     *
     *  This function is called when the application calls
     *  `{@link Seconds#getTime()}` function.
     *
     *  By default, this function is configured with a default function
     *  that returns 0.
     */
    config GetTimeFxn getTimeFxn = "&ti_sysbios_hal_SecondsCallback_defaultGetTime";

    /*!
     *  ======== setTimeFxn =========
     *  User supplied 'set' function
     *
     *  This function is called when the application calls
     *  `{@link Seconds#setTime()}` function.
     *
     *  By default, this function is configured with a default function
     *  that returns 0.
     */
    config SetTimeFxn setTimeFxn = "&ti_sysbios_hal_SecondsCallback_defaultSetTime";
}
