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
 */
/*
 *  ======== package.xdc ========
 */

requires ti.sysbios.family;

/*!
 *  ======== ti.sysbios ========
 *  SYS/BIOS Scalable/Extensible Real-time Kernel
 *
 *  SYS/BIOS is a scalable real-time kernel. It is designed to be used 
 *  by applications that require real-time scheduling and synchronization 
 *  or real-time instrumentation. SYS/BIOS provides preemptive 
 *  multi-threading, hardware abstraction, real-time analysis, and 
 *  configuration tools. SYS/BIOS is designed to minimize memory and 
 *  CPU requirements on the target. 
 *
 *  This package contains the {@link ti.sysbios.BIOS BIOS} module, which
 *  serves as the "top level" configuration entry point for SYS/BIOS.  The
 *  BIOS module provides basic configuration parameters necessary to use
 *  SYS/BIOS and provides links to more advanced configuration options that
 *  can be used to further optimize SYS/BIOS for your application.
 *
 *  SYS/BIOS is implemented as a set of RTSC packages, each of which 
 *  delivers a subset of the functionality. The RTSC standard recommends 
 *  a naming convention for packages to aid readability and uniqueness. 
 *  If you are familiar with the Java package naming convention, 
 *  you will find SYS/BIOS's packages to be quite similar.
 *
 *  SYS/BIOS packages conform to this convention with names that consist 
 *  of a hierarchical naming pattern; each level is separated by a period 
 *  ("."). Usually, the highest level of the name is the vendor ("ti"), 
 *  followed by the product ("sysbios"), and then followed by sub-package
 *  names (for example, "knl").
 *
 *  These names have the added benefit of reflecting the physical layout 
 *  of the package within the file system where SYS/BIOS has been installed. 
 *  For example, the {@link ti.sysbios.knl} package files can be found at 
 *  `BIOS_INSTALL_DIR/bios_6_##_##/packages/ti/sysbios/knl`.
 *
 *  For more information about SYS/BIOS, see the following:
 *
 *  @p(html)
 *  <ul>
 *  <li><a 
 *  href=http://processors.wiki.ti.com/index.php/SYS/BIOS_Getting_Started_Guide>
 *  <i>SYS/BIOS Getting Started Guide</i></a></li>
 *  <li><a href=http://www-s.ti.com/sc/techlit/spruex3>
 *  <i>SYS/BIOS 6.x User's Guide</i></a></li>
 *  <li><a href=http://www-s.ti.com/sc/techlit/spraas7>
 *  <i>Migrating a DSP/BIOS 5 Application to SYS/BIOS 6</i></a></li>
 *  <li><a href=http://rtsc.eclipse.org/docs-tip/Main_Page>
 *  RTSC-pedia wiki</a></li>
 *  <li><a href=http://processors.wiki.ti.com/index.php/Category:SYSBIOS>
 *  SYS/BIOS Information Wiki</a></li>
 *  <li><a href=http://focus.ti.com/docs/toolsw/folders/print/dspbios6.html>
 *  SYS/BIOS 6.x Product Information</a></li>
 *  <li><a href=http://e2e.ti.com/support/embedded/f/355.aspx>
 *  SYS/BIOS support forum in TI E2E Community</a></li>
 *  </ul>
 *  @p
 *
 */
package ti.sysbios [2,0,0,0] {
    module BIOS;
    module Build;
}
