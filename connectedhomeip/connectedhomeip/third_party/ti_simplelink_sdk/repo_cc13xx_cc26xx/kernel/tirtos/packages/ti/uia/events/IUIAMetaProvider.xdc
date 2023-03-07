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
 *  ======== IUIAMetaProvider.xdc ========
 */

package ti.uia.events;

/*!
 *  @_nodoc
 *  ======== IUIAMetaProvider ========
 *  Interface to identify UIA MetaData providers to the UIAMetaData xml
 *     generation script
 *
 *  Inheriting from this interface is sufficient to identify to
 *  ti.uia.runtime.UIAMetaData.xs that the module is able to provide metadata
 *  for inclusion in the .uia.xml file
 */
interface IUIAMetaProvider {

    /*! @_nodoc
     * ======== hasMetaData =========
     * set to true to indicate that the module has metadata to contribute to the
     *    uia.xml file.
     */
    metaonly config Bool hasMetaData = false;

    /*! @_nodoc
     *  ======== writeUIAMetaData ========
     *  Writes any UIA metadata required to support the module
     *
     *  NOTE: This should only be called by ti.uia.runtime.UIAMetaData.xs in
     *  order to ensure that the UIA xml file has been properly opened and is
     *  ready for writing.
     *  @param(inst) instance of the module to generate data for, null for
     *     module-level info
     *  @param(instNum) the instance number used in .h files
     *  @param(indent) the number of spaces to preceed the xml tag with
     */
    metaonly function writeUIAMetaData(inst, instNum, indent);

}
