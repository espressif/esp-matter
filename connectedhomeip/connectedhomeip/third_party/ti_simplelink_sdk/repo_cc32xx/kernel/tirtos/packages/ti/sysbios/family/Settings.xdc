/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== Settings.xdc ========
 *
 */
package ti.sysbios.family;

/*!
 *  ======== Settings ========
 *  Select the device-specific family modules 
 *
 *  This module selects target-specific implementation modules required by the
 *  various SYS/BIOS packages.  These modules are mainted in "family" packages
 *  which may also contain modules that are specific to a particular family;
 *  for example, cache or power management modules.
 */

metaonly module Settings inherits ti.sysbios.interfaces.ISettings
{
    /*!
     *  ======== familyConfigPage ========
     *  GUI Config layout page
     *
     *  This config parameter is the name of the GUI config layout page
     *  for the family-specific SYS/BIOS modules.  It is a path relative
     *  to the ti.sysbios package base directory, _not_ relative to this
     *  package's base.
     *
     *  @_nodoc
     */
    config String familyConfigPage = "family/defaultConfig.xml";

    /*!
     *  ======== bootModule ========
     *  Device-specific Boot module
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific Boot module.
     *
     *  @_nodoc
     */
    config String bootModule;
    
    /*!
     *  ======== cacheDelegate ========
     *  Device-specific Cache module
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific Cache module.
     *
     *  @_nodoc
     */
    config String cacheDelegate;
    
    /*!
     *  ======== hwiDelegate ========
     *  Device-specific Hwi support services
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific module.
     *
     *  @_nodoc
     */
    config String hwiDelegate;

    /*!
     *  ======== mmuModule ========
     *  Device-specific MMU module
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific MMU module.
     *
     *  @_nodoc
     */
    config String mmuModule;
    
    /*!
     *  ======== clockTimerDelegate ========
     *  Device-specific Clock module Timer support services
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific module.
     *
     *  @_nodoc
     */   
    config String clockTimerDelegate;
    
    /*!
     *  ======== secondsDelegate ========
     *  Device-specific Seconds module support services
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a
     *  link to the device-specific module.
     *
     *  @_nodoc
     */
    config String secondsDelegate;

    /*!
     *  ======== timerDelegate ========
     *  Device-specific Timer support services
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific module.
     *
     *  @_nodoc
     */   
    config String timerDelegate;
    
    /*!
     *  ======== timestampDelegate ========
     *  Device-specific Timestamp support services
     *
     *  This parameter is used by the BIOS Overview Grace page to provide a 
     *  link to the device-specific module.
     *
     *  @_nodoc
     */    
    config String timestampDelegate;  
    
    /*!
     *  ======== familyName ========
     *  Name of the SYS/BIOS family package
     *
     *  @_nodoc
     */
    config String familyName;

    /*!
     *  ======== getFamilyName ========
     *  Get SYS/BIOS device family name
     */
    String getFamilyName();
}
