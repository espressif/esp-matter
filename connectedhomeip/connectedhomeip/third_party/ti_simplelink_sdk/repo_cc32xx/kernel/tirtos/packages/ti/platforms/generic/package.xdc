/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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

requires xdc.platform [1,0,1];

/*!
 *  ======== ti.platforms.generic ========
 *  Platform package for the generic platform.
 *
 *  This package implements the interfaces (xdc.platform.IPlatform)
 *  necessary to build and run executables on a "generic" platform; a
 *  platform specified by:
 *  @p(blist)
 *      - device name; e.g., "TMS320C2812"
 *      - catalog name; the name of a package containing the device named above
 *      - clock rate; the clock rate in MHz of the CPU
 *      - external memory map; an array of memory blocks external to the device
 *  @p
 *  For a complate list of parameters that can be specified when creating a
 *  platform instance see the instance configuration parameters specified
 *  by `{@link ti.platforms.generic.Platform}`.  This list, of course,
 *  includes all the instance config parameters specified by the
 *  `{@link xdc.platform.IPlatform}` interface.
 *
 *
 *  @a(Throws)
 *  `XDCException` exceptions are thrown for fatal errors. The following error
 *  codes are reported in the exception message:
 *  @p(dlist)
 *      -  `ti.platfoms.generic.LINK_TEMPLATE_ERROR`
 *           This error is raised when this platform cannot found the default
 *           linker command template `linkcmd.xdt` in the build target's
 *           package. When a target does not contain this file, the config
 *           parameter `{@link xdc.cfg.Program#linkTemplate}` must be set.
 *  @p
 *
 *  @a(EXAMPLES)
 *  Example 1: Suppose you need to create an application for a HW platform
 *  that uses the TMS320C2812 running at 150 MHz.  You can use this package
 *  (in lieu of one specifically created for the HW platform) by defining a
 *  named instance of this package's Platform module.  Add the following
 *  statements to your `config.bld` file to add the platform instance named
 *  "ti.platforms.generic:C28".
 *  @p(code)
 *      Build.platformTable["ti.platforms.generic:C28"] = {
 *          clockRate:   150,
 *          catalogName: "ti.catalog.c2800",
 *          deviceName:  "TMS320C2812"
 *      };
 *  @p
 *  With this name defined, it is now possible to configure an application
 *  using the platform instance name "ti.platforms.generic:C28".  For example,
 *  if you are using `{@link xdc.tools.configuro}` to configure your
 *  application, the string "ti.platforms.generic:C28" can now be used to
 *  identify your platform:
 *  @p(code)
 *      xs xdc.tools.configuro -b config.bld -p ti.platforms.generic:C28 ...
 *  @p
 *
 *  @p(html)
 *  <hr/>
 *  @p
 *
 *  Example 2: The following example illustrates how to specify a platform
 *  instance with memory regions external to the specified device.  In this
 *  case, we define a platform using a TMS320C6416 running at 600 MHz on a
 *  board with two external SDRAM memory blocks.
 *  @p(code)
 *      Build.platformTable["ti.platforms.generic:C64"] = {
 *          clockRate:          600,
 *          catalogName:        "ti.catalog.c6000",
 *          deviceName:         "TMS320C6416",
 *          externalMemoryMap : [
 *              ["SDRAM1", {
 *                  name: "SDRAM1",
 *                  base: 0x80000000, len: 0x1000000, space: "code/data"
 *              }],
 *              ["SDRAM2", {
 *                  name: "SDRAM2",
 *                  base: 0x90000000, len: 0x1000000, space: "code/data"
 *              }],
 *          ]
 *      }
 *  @p
 *
 *  The `externalMemoryMap` attribute is a map of string names to
 *  `{@link xdc.platform.IPlatform#Memory}` structures.
 *
 *  @see ti.platforms.generic.Platform
 *  @see xdc.bld.BuildEnvironment#platformTable
 *  @see xdc.platform.IPlatform
 */
package ti.platforms.generic [1,0,0,1] {
    module Platform;
}
