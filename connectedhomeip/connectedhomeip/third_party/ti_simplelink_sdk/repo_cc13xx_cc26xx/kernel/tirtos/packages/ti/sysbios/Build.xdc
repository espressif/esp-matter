/*
 * Copyright (c) 2013-2017 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== Build.xdc ========
 *  @_nodoc
 *  metaonly module to support building various package/product libraries
 *
 */

/*!
 *  ======== Build ========
 */

@Template("./Build.xdt")
metaonly module Build
{
    /*!
     *  ======== buildROM ========
     *  Setting this to true causes the ROM to be built.
     */
    metaonly config Bool buildROM = false;

    /*!
     *  ======== buildROMApp ========
     *  Setting this to true tells the Build system
     *  that the user wants their application linked with
     *  the ROM image.
     */
    metaonly config Bool buildROMApp = false;

    /*!
     *  ======== includePaths ========
     *  Array of header file include search paths
     *  used to build the custom RTOS library
     *
     *  To add to this array, use the following syntax:
     *  Build.includePaths.$add("/path/to/my/include/files/");
     */
    metaonly config String includePaths[];

    /*!
     *  ======== ccArgs ========
     *  Array of strings added to the compile line
     *  used to build the custom RTOS library
     *
     *  To add to this array, use the following syntax:
     *  Build.ccArgs.$add("-DMY_MACRO=1");
     */
    metaonly config String ccArgs[];

    /*!
     *  ======== Component ========
     *  Define an annex component
     *
     *  Annex components may be defined by adding this object type
     *  to the {@link #annex} array. All components in this array
     *  will participate in the kernel build flow.
     *
     *  @field(repo)    Specify the fully qualified path to the component
     *                  repository. This will become a `vpath` directive
     *                  in the generated makefile. For example, if you
     *                  specify repo as the following
     *                  @p(code)
     *                      /path/to/component/repository
     *                  @p
     *                  the generated makefile will contain
     *                  @p(code)
     *                      vpath %.c /path/to/component/repository
     *                  @p
     *
     *  @field(files)   An array of component source files. These will be
     *                  added to the kernel build rule as dependencies.
     *                  The file name must be found on the vpath given above.
     *                  To avoid file name conflicts, it is recommended to
     *                  specify package qualified file names. For example:
     *                  @p(code)
     *                      "my/package/fileA.c"
     *                      "my/package/fileB.c"
     *                  @p
     */
    struct Component {
        String  repo;           /*! full path to component repository */
        String  incs[];         /*! list of include paths */
        String  files[];        /*! list of component source files */
    };

    /*!
     *  ======== annex ========
     *  The array of annex components
     *
     *  All components defined in this array will participate in the
     *  kernel build flow. Components are of type {@link #Component}.
     *
     *  To add a component to this array, use the following syntax:
     *
     *  @p(code)
     *  Build.annex.$add({
     *      repo: "/path/to/component/repository",
     *      files: [
     *          "my/package/fileA.c",
     *          "my/package/fileB.c"
     *      ]
     *  });
     *  @p
     */
    metaonly config Component annex[];

    /*!
     *  ======== getDefaultCustomCCOpts ========
     */
    metaonly String getDefaultCustomCCOpts();

    /*!
     *  ======== getDefs ========
     *  Get the compiler -D options necessary to build
     */
    metaonly String getDefs();

    /*!
     *  ======== getCFiles ========
     *  Get the library C source files.
     */
    metaonly String getCFiles(String target);

    /*!
     *  ======== getAsmFiles ========
     *  Get the library Asm source files.
     */
    metaonly Any getAsmFiles(String target);

    /*!
     *  ======== getCommandLineDefs ========
     *  Get the set of -D strings to insert into the makefile.
     */
    metaonly String getCommandLineDefs();

    /*!
     *  ======== getIncludePaths ========
     *  Get the set of -I strings to insert into the makefile.
     */
    metaonly String getIncludePaths();

    /*!
     *  ======== getCcArgs ========
     *  Get the compiler commpand line  args
     */
    metaonly String getCcArgs();

    /*
     *  ======== buildLibs ========
     *  This function generates the makefile goals for the libraries
     *  produced by a ti.sysbios package.
     */
    function buildLibs(objList, relList, filter, xdcArgs);

    /*!
     *  ======== getLibs ========
     *  Common getLibs() for all sysbios packages.
     */
    function getLibs(pkg);
}
