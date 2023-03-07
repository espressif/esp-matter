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
 */
/*
 *  ======== ISettings.xdc ========
 *
 */
package ti.sysbios.interfaces;

/*!
 *  ======== ISettings ========
 *  Interface for family-specific Settings modules.
 *
 *  Every new device family must implement this interface.
 */
metaonly interface ISettings
{
    /*! returns the family-specific Core module delegate */
    String getDefaultCoreDelegate();
    
    /*! returns the family-specific Hwi module delegate */
    String getDefaultHwiDelegate();

    /*! returns the family-specific Timer module delegate */
    String getDefaultTimerDelegate();
    
    /*! returns the family-specific Clock Timer module delegate */
    String getDefaultClockTimerDelegate();

    /*! returns the family-specific Timer Support module delegate */
    String getDefaultTimerSupportDelegate();

    /*! returns the family-specific Timestamp module delegate */
    String getDefaultTimestampDelegate();

    /*! returns the family-specific TaskSupport module delegate */
    String getDefaultTaskSupportDelegate();

    /*! returns the family-specific IntrinsicsSupport module delegate */
    String getDefaultIntrinsicsSupportDelegate();

    /*! returns the family-specific CacheSupport module delegate */
    String getDefaultCacheDelegate();

    /*! returns the family-specific Power module delegate */
    String getDefaultPowerDelegate();

    /*! returns the family-specific Seconds module delegate */
    String getDefaultSecondsDelegate();

    /*! returns the family-specific Boot module. */
    String getDefaultBootModule();

    /*! returns the family-specific Mmu module. */
    String getDefaultMmuModule();
    
    /*! Returns the path to the family-specific Grace page. */
    String getFamilySettingsXml();
    
    /*! returns the family-specific Clock tickPeriod */
    UInt32 getDefaultClockTickPeriod();
}
