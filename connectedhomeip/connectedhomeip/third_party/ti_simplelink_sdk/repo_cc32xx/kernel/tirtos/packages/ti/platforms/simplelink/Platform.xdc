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

/*
 *  ======== Platform.xdc ========
 */
package ti.platforms.simplelink;

/*!
 *  ======== Platform ========
 *  A generic platform that supports any CC13xx, CC26xx, or CC32xx device
 *
 *  The device to be used by this platform is passed as the platform instance
 *  name. On the `xdc.tools.configuro` command line, it is done in the
 *  following way:
 *  @p(code)
 *  xs xdc.tools.configuro ... -p "ti.platforms.simplelink:CC3200"
 *  @p
 *
 *  In package.bld, the platform instance is selected as in:
 *  @p(code)
 *  Pkg.addExecutable("test", target, "ti.platforms.simplelink:CC3200");
 *  @p
 *
 *  @a(Note)
 *  The 'simplelink' platform provides support for allowing the user to
 *  specify the application's C stack size within their linker command
 *  file. If the user sets 'Program.stack = 0' in their configuration
 *  file, then it is up to the user to add the necessary content to their
 *  linker command file to define the size and placment of the C stack.
 *  Additionally, if GNU tools are being used, then the user must define
 *  'STACKSIZE' in their linker command file and have its value be the
 *  size of the C stack in bytes. The 'STACKSIZE' symbol is used
 *  internally to initialize other symbols that must be provided to SYS/BIOS.
 */
@Template ("./Platform.xdt")
metaonly module Platform inherits xdc.platform.IPlatform
{
    config xdc.platform.IPlatform.Board BOARD = {
        id:             "0",
        boardName:      "simplelink",
        boardFamily:    null,
        boardRevision:  null
    };

    /*!
     *  ======== nameFormat ========
     *  Encoding of instance creation parameters in the instance's name
     *
     *  For this platform, the parameters `deviceName`, `includeLinkCmdFile`
     *  and `clockRate` can be encoded in the instance name supplied on
     *  `xdc.tools.configuro` command line, for example:
     *  @p(code)
     *      xs xdc.tools.configuro ... -p ti.platforms.simplelink:CC3200:1:20
     *  @p
     *  Optional parameters can be omitted:
     *  @p(code)
     *      xs xdc.tools.configuro ... -p ti.platforms.simplelink:CC3200
     *  @p
     */
     readonly config string nameFormat
         = "$(deviceName):$(includeLinkCmdFile):$(clockRate)";

    /*!
     *  ======== useGnuRomLinkCmd ========
     *  Use ROM compatible linker script when building using GNU tools
     *
     *  If this config param is set to true and "includeLinkCmdFile" is true,
     *  the ROM compatible linker script will be used. The ROM compatible
     *  linker script is required when building a ROM based application.
     */
     config Bool useGnuRomLinkCmd = false;

instance:

    /*
     *  This platform supports either Cortex M3 or M4 cores.
     *  The corresponding "actual" catalogName is overwritten
     *  in Platform.instance$meta$init() based on the device name
     */

    config xdc.platform.IExeContext.Cpu CPU = {
        id:             "0",
        clockRate:      80.0,
        catalogName:    "ti.catalog.arm.cortexm4",
        deviceName:     "CC32xx",
        revision:       "",
    };

    /*!
     *  ======== deviceName ========
     *  The name of an `ICpuDataSheet` module for the device
     *
     *  This parameter is required, but it does not have to be set explicitly;
     *  it can be encoded in the instance's name.
     */
    config string deviceName;

    /*!
     *  ======== clockRate ========
     *  The clock rate for this device.
     */
    config Double clockRate;

    override config string codeMemory = null;

    override config string dataMemory = null;

    override config string stackMemory = null;

    /*!
     *  ======== includeLinkCmdFile ========
     *  The flag that specifies if the platform should include a linker command
     *  file.
     *
     *  By default, a user is responsible for adding a linker command file to
     *  the project, or to the linker command line. However, if this flag is
     *  set, this platform will include a default linker command file for the
     *  selected device.
     */
    config Bool includeLinkCmdFile = false;
};
