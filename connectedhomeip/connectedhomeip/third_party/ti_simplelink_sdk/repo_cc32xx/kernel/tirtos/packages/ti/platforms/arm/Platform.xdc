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

package ti.platforms.arm;

/*!
 *  ======== Platform ========
 *  Generic platform support for the arm
 *
 *  This platform requires a memory map file called `board.xs` to be available
 *  in the same directory as the application config file. It should define
 *  values for the config parameters which are declared in this platform.
 *
 *  @a(Examples)
 *  A sample `board.xs` file.
 *
 *  @p(code)
 *  var boardARM = {
 *      CPU: {
 *          clockRate:      300.0,
 *          catalogName:    "ti.catalog.arm.cortexm3",
 *          deviceName:     "TMS320DM8168",
 *      },
 *      externalMemoryMap: [
 *          ["EXTRAM",  {
 *              name: "DDR",  base: 0xC0000000, len: 0x08000000,
 *          }]
 *      ],
 *      codeMemory:  "DDR",
 *      dataMemory:  "DDR",
 *      stackMemory: "DDR",
 *      l1PMode:     "32k",
 *      l1DMode:     "32k",
 *      l2Mode:      "0k",
 *  };
 *  @p
 */
metaonly module Platform inherits xdc.platform.IPlatform
{
    config xdc.platform.IPlatform.Board BOARD;

instance:

    /*!
     *  ======== CPU ========
     *  CPU Attributes necessary to create an execution context
     *
     *  The platform requires these attributes to get the device internal
     *  memory map.
     *
     *  @see xdc.platform.IExeContext#Cpu
     */
    config xdc.platform.IExeContext.Cpu CPU = {
        id:             "0",
        clockRate:      1.0,
        catalogName:    "ti.catalog.arm",
        deviceName:     "",
        revision:       "",
    };

    override config String codeMemory = null;

    override config String dataMemory = null;

    override config String stackMemory = null;

    /*!
     *  ======== sectionMap ========
     *  A section name to SectionSpec mapping
     *
     *  @see xdc.cfg.Program#sectMap
     */
    config Any sectionMap[string];

    /*!
     *  ======== sectionsExclude ========
     *  Section to exclude from linker command file generation
     *
     *  @see xdc.cfg.Program#sectionsExclude
     */
    config String sectionsExclude = null;

    /*!
     *  ======== memoryExclude ========
     *  Section to exclude from linker command file generation
     *
     *  @see xdc.cfg.Program#memoryExclude
     */
    config Bool memoryExclude = false;

    /*!
     *  ======== sectionsTemplate ========
     *  Replace the sections portion of the generated linker command file.
     *
     *  @see xdc.cfg.Program#sectionsTemplate
     */
    config String sectionsTemplate = null;

    /*!
     *  ======== l1PMode ========
     *  Define the amount of L1P RAM used for L1 Program Cache.
     */
    config String l1PMode = null;

    /*!
     *  ======== l1DMode ========
     *  Define the amount of L1D RAM used for L1 Data Cache.
     */
    config String l1DMode = null;

    /*!
     *  ======== l2Mode ========
     *  Define the amount of L2 RAM used for L2 Cache.
     */
    config String l2Mode = null;

    /*!
     *  ======== sectMap ========
     *  @_nodoc
     */
    override config String sectMap[string];

    /*!
     *  ======== getCpuDataSheet ========
     *  @_nodoc
     */
    override function getCpuDataSheet(cpuId);

    /*!
     *  ======== getExeContext ========
     *  @_nodoc
     */
    override function getExeContext(prog);

    /*!
     *  ======== getExecCmd ========
     *  @_nodoc
     */
    override function getExecCmd(prog, platPath);

    /*!
     *  ======== getLinkTemplate ========
     *  @_nodoc
     */
    override function getLinkTemplate(prog);
};
