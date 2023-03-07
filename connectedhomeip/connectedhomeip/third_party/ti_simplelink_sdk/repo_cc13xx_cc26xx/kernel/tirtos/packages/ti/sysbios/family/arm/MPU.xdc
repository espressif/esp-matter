/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== MPU.xdc ========
 */

package ti.sysbios.family.arm;

import xdc.rov.ViewInfo;

/*!
 *  ======== MPU ========
 *  Memory Protection Unit (MPU) Manager.
 *
 *  This module manages the Memory Protect Unit (MPU) present in many ARM
 *  Cortex-R and Cortex-M devices. It enables the application to partition
 *  the memory into different regions and set protection attributes for
 *  each region.
 *
 *  The number of memory regions supported is device specific and may vary
 *  on different devices. The Cortex-R4F based RM48L5XX devices for instance
 *  support 16 memory regions.
 *
 *  Programming a memory region requires specifying the base address and
 *  size of the region, and the region's protection attributes. It is also
 *  possible to overlap different memory regions with higher region numbers
 *  enjoying higher priority than lower region numbers i.e. region 15 has
 *  a higher priority than region 14, and if both were overlapping, the
 *  overlapped memory region's attributes would be defined by region 15's
 *  entry.
 *
 *  The protection attributes for each region include attributes such as
 *  memory type (strongly-ordered, device or normal), shareability,
 *  cacheability and read-write access permission.
 *
 *  @a(Memory region attributes)
 *  Memory regions can be configured as different memory types by setting
 *  the {@link #RegionAttrs bufferable}, {@link #RegionAttrs cacheable} and
 *  {@link #RegionAttrs tex} (type extension) fields of the {@link #RegionAttrs}
 *  structure which is passed as an argument to
 *  {@link #setRegion MPU_setRegion()} function. The three memory types
 *  supported by the hardware are "Normal" (cacheable), "Device" and
 *  "Strongly-ordered" memory. "Device" and "Strongly-ordered" memory types
 *  are recommended for mapping peripheral address space like memory-mapped
 *  registers. These two types ensure that the memory accesses to the
 *  peripheral memory are not performed speculatively, are not repeated and
 *  are performed in order. The "Normal" memory type is recommended for mapping
 *  memory regions storing application code and data.
 *
 *  Here are some common settings for the {@link #RegionAttrs bufferable},
 *  {@link #RegionAttrs cacheable} and {@link #RegionAttrs tex} fields to
 *  define different memory region types:
 *
 *  @p(code)
 *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  + Memory Type                             | bufferable | cacheable | tex +
 *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  + Shareable Strongly-ordered memory       |    false   |   false   |  0  +
 *  +-----------------------------------------+------------+-----------+-----+
 *  + Shareable Device memory                 |    true    |   false   |  0  +
 *  +-----------------------------------------+------------+-----------+-----+
 *  + Outer & Inner Non-cacheable             |    false   |   false   |  1  +
 *  +-----------------------------------------+------------+-----------+-----+
 *  + Outer & Inner Write-back Write-allocate |    true    |   true    |  1  +
 *  + cacheable                               |            |           |     +
 *  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  @p
 *
 *  For an exhaustive list of all different memory type settings and a
 *  detailed explanation of the memory region attributes, please read the
 *  'Protected Memory System Architecture (PMSA)' chapter of the
 *  {@link http://infocenter.arm.com/help/topic/com.arm.doc.ddi0406c/index.html ARM v7AR Architecture Reference Manual}.
 *
 *  @a(Changing shareability attributes of a cacheable memory region)
 *  If changing the shareability attribute of a cacheable memory region,
 *  it is possible for coherency problems to arise. In order to avoid possible
 *  coherency errors, the below sequence should be followed to change the
 *  shareability attributes of the memory region:
 *  - Make the memory region Non-cacheable and outer-shareable
 *  - Clean and invalidate the memory region from the cache
 *  - Change the shareability attribute to the desired value
 *
 *  @a(Examples)
 *  Example showing how to set attributes for a given memory region using
 *  *.cfg script:
 *
 *  @p(code)
 *  var MPU = xdc.useModule('ti.sysbios.family.arm.MPU');
 *
 *  // Mark memory region as normal outer and inner write-back
 *  // and write-through cacheable
 *  var attrs = new MPU.RegionAttrs();
 *  MPU.initRegionAttrsMeta(attrs);
 *  attrs.enable = true;
 *  attrs.bufferable = true;
 *  attrs.cacheable = true;
 *  attrs.shareable = false;
 *  attrs.noExecute = false;
 *  attrs.accPerm = 6;  // Read-only at PL1 and PL0
 *  attrs.tex = 1;
 *
 *  // Set attributes for memory region of size 4MB starting at address 0x0
 *  // using MPU region Id 0 to store the attributes.
 *  MPU.setRegionMeta(0, 0x00000000, MPU.RegionSize_4M, attrs);
 *  @p
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                               -->
 *    <tr><td> {@link #disable}     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #enable}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #initRegionAttrs}  </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #isEnabled}  </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #setRegion}  </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup. </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup.</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
module MPU
{
    // -------- ROV views --------

    /*! @_nodoc */
    metaonly struct RegionAttrsView {
        UInt8       RegionIdx;
        Bool        Enabled;
        String      BaseAddress;
        String      Size;
        Bool        Bufferable;
        Bool        Cacheable;
        Bool        Shareable;
        Bool        Noexecute;
        String      AccessPerm;
        String      Tex;
        String      SubregionDisableMask;
    };

    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['MpuRegionAttrsView', {
                    type: ViewInfo.MODULE_DATA,
                    viewInitFxn: 'viewMpuRegionAttrs',
                    structName: 'RegionAttrsView'
                }],
           ]
       });

    /*!
     *  Memory Protection Unit (MPU) registers. Symbol "MPU_deviceRegs" is
     *  a physical device
     */
    struct DeviceRegs {
        UInt32 TYPE;            /*! 0xD90 Type Register                      */
        UInt32 CTRL;            /*! 0xD94 Control Register                   */
        UInt32 RNR;             /*! 0xD98 Region Register                    */
        UInt32 RBAR;            /*! 0xD9C Region Base Address Register       */
        UInt32 RASR;            /*! 0xDA0 Region Attribute and Size Register */
        UInt32 RBAR_A1;         /*! 0xDA4 MPU Alias 1                        */
        UInt32 RASR_A1;         /*! 0xDA8 MPU Alias 1                        */
        UInt32 RBAR_A2;         /*! 0xDAC MPU Alias 2                        */
        UInt32 RASR_A2;         /*! 0xDB0 MPU Alias 2                        */
        UInt32 RBAR_A3;         /*! 0xDB4 MPU Alias 3                        */
        UInt32 RASR_A3;         /*! 0xDB8 MPU Alias 3                        */
    };

    extern volatile DeviceRegs deviceRegs;

    /*!
     *  ======== RegionSize ========
     */
    enum RegionSize {
        RegionSize_32   = 0x8,
        RegionSize_64   = 0xA,
        RegionSize_128  = 0xC,
        RegionSize_256  = 0xE,
        RegionSize_512  = 0x10,
        RegionSize_1K   = 0x12,
        RegionSize_2K   = 0x14,
        RegionSize_4K   = 0x16,
        RegionSize_8K   = 0x18,
        RegionSize_16K  = 0x1A,
        RegionSize_32K  = 0x1C,
        RegionSize_64K  = 0x1E,
        RegionSize_128K = 0x20,
        RegionSize_256K = 0x22,
        RegionSize_512K = 0x24,
        RegionSize_1M   = 0x26,
        RegionSize_2M   = 0x28,
        RegionSize_4M   = 0x2A,
        RegionSize_8M   = 0x2C,
        RegionSize_16M  = 0x2E,
        RegionSize_32M  = 0x30,
        RegionSize_64M  = 0x32,
        RegionSize_128M = 0x34,
        RegionSize_256M = 0x36,
        RegionSize_512M = 0x38,
        RegionSize_1G   = 0x3A,
        RegionSize_2G   = 0x3C,
        RegionSize_4G   = 0x3E
    };


    /*!
     *  ======== RegionAttrs ========
     *  Structure for setting the region attributes
     *
     *  The B (Bufferable), C (Cacheable), TEX (Type extension), S (Shareable),
     *  XN (No execute or Execute never) and AP (Access permission) bits in the
     *  memory region entry define the memory region attributes.
     *
     *  See the 'Memory region attributes' section in the 'Protected Memory
     *  System Architecture (PMSA)' of the ARM v7-AR Architecture Reference
     *  Manual for more details.
     */
    struct RegionAttrs {
        Bool  enable;               /*! is MPU region enabled                */
        Bool  bufferable;           /*! is memory region bufferable (B)      */
        Bool  cacheable;            /*! is memory region cacheable (C)       */
        Bool  shareable;            /*! is memory region shareable (S)       */
        Bool  noExecute;            /*! is memory region not executable (XN) */
        UInt8 accPerm;              /*! access permission bits value 0-7
                                        (AP[2:0])                            */
        UInt8 tex;                  /*! memory region attr type extension
                                        field (TEX[2:0])                     */
        UInt8 subregionDisableMask; /*! disable mask for all 8 subregions    */
    };

    /*!
     *  ======== defaultAttrs ========
     *  Default region attributes structure
     *
     *  The default attributes structure marks the memory region as outer and
     *  inner non-cacheable and non-shareable, with read-write access in
     *  privileged mode (PL1) only.
     */
    config RegionAttrs defaultAttrs = {
        enable: true,                   /* true by default                   */
        bufferable: false,              /* false by default                  */
        cacheable: false,               /* false by default                  */
        shareable: false,               /* false by default                  */
        noExecute: false,               /* false by default                  */
        accPerm: 1,                     /* allow read/write access at PL1    */
        tex: 1,                         /* 1 by default                      */
        subregionDisableMask: 0         /* no subregions disabled by default */
    };

    /*!
     *  ======== A_nullPointer ========
     *  Assert raised when a pointer is null
     */
    config xdc.runtime.Assert.Id A_nullPointer  = {
        msg: "A_nullPointer: Pointer is null"
    };

    /*!
     *  ======== A_invalidRegionId ========
     *  Assert raised when an invalid region number is passed to MPU_setRegion()
     */
    config xdc.runtime.Assert.Id A_invalidRegionId  = {
        msg: "A_invalidRegionId: MPU Region number passed is invalid."
    };

    /*!
     *  ======== A_unalignedBaseAddr ========
     *  Assert raised when region's base address is not aligned to the region's
     *  size
     */
    config xdc.runtime.Assert.Id A_unalignedBaseAddr  = {
        msg: "A_unalignedBaseAddr: MPU region base address not aligned to size."
    };

    /*!
     *  ======== A_reservedAttrs ========
     *  Assert raised when region's attributes set to a reserved set
     */
    config xdc.runtime.Assert.Id A_reservedAttrs = {
        msg: "A_reservedAttrs: MPU region attributes set to reserved value."
    };

    /*!
     *  ======== enableMPU ========
     *  Configuration parameter to enable MPU. Disabled by default.
     */
    config Bool enableMPU = false;

    /*!
     *  ======== enableBackgroundRegion ========
     *  Configuration parameter to enable background region.
     *
     *  If the MPU is enabled and background region is also enabled, any
     *  privileged access that does not map to any MPU memory region is
     *  handled using the default memory map.
     *
     *  @p(blist)
     *      -See the "Protected Memory System Architecture (PMSA)" chapter
     *       in the {@link http://infocenter.arm.com/help/topic/com.arm.doc.ddi0406c/index.html ARM v7AR Architecture Reference Manual}
     *       for more info on the default memory map.
     *  @p
     */
    config Bool enableBackgroundRegion = true;

    /*!
     *  @_nodoc
     *  ======== numRegions ========
     *  Number of MPU regions. Default is determined based on device type.
     */
    config UInt8 numRegions;

    /*!
     *  ======== initRegionAttrsMeta ========
     *  Initializes the region attribute structure
     *
     *   @param(attrs)        Pointer to region attribute struct
     */
    metaonly Void initRegionAttrsMeta(RegionAttrs *regionAttrs);

    /*!
     *  ======== setRegionMeta ========
     *  Statically sets the MPU region attributes
     *
     *  @see ti.sysbios.family.arm.r5.MPU
     *
     *  @param(regionId)        MPU region number
     *  @param(regionBaseAddr)  MPU region base address
     *  @param(regionSize)      MPU region size
     *  @param(attrs)           Protection attributes
     */
    metaonly Void setRegionMeta(UInt8 regionId, Ptr regionBaseAddr,
        RegionSize regionSize, RegionAttrs attrs);

    /*!
     *  ======== disable ========
     *  Disables the MPU.
     *
     *  If the MPU is already disabled, then simply return.
     *  Otherwise this function does the following:
     *  @p(blist)
     *  - If the L1 data cache is enabled, write back invalidate all
     *  of L1 data cache.
     *  - If the L1 program cache is enabled, invalidate all of L1
     *  program cache.
     *  @p
     *
     *  @a(Note)
     *  This function does not change the cache L1 data/program settings.
     */
    Void disable();

    /*!
     *  ======== enable ========
     *  Enables the MPU.
     *
     *  If the MPU is already enabled, then simply return.
     *  Otherwise this function does the following:
     *  @p(blist)
     *  If the L1 program cache is enabled, invalidate all of L1
     *  program cache.
     *  @p
     *
     *  This function enables the MPU on the core it is called from.
     *
     *  @a(Note)
     *  This function does not change the L1 data/program cache settings.
     */
    Void enable();

    /*!
     *  @_nodoc
     *  ======== disableBR ========
     *  Disable background region
     */
    Void disableBR();

    /*!
     *  @_nodoc
     *  ======== enableBR ========
     *  Enable background region
     */
    Void enableBR();

    /*!
     *  ======== initRegionAttrs() ========
     *  Initializes the region attribute structure
     *
     *  @param(attrs)      Pointer to region attribute struct
     */
    Void initRegionAttrs(RegionAttrs *regionAttrs);

    /*!
     *  ======== isEnabled ========
     *  Determines if the MPU is enabled
     */
    Bool isEnabled();

    /*!
     *  ======== setRegion ========
     *  Sets the MPU region attributes
     *
     *  @see ti.sysbios.family.arm.r5.MPU
     *
     *  @param(regionId)        MPU region number
     *  @param(regionBaseAddr)  MPU region base address
     *  @param(regionSize)      MPU region size
     *  @param(attrs)           Protection attributes
     */
    Void setRegion(UInt8 regionId, Ptr regionBaseAddr,
        RegionSize regionSize, RegionAttrs *attrs);

    /*!
     *  @_nodoc
     *  ======== setRegionRaw ========
     *  Sets the MPU region attributes
     *
     *  Unlike MPU_setRegion(), this API takes compact MPU register
     *  values and programs them without interpreting them.
     *
     *  @param(rbar)            MPU region base address
     *  @param(rasr)            MPU region attribute and size
     */
    Void setRegionRaw(UInt32 rbar, UInt32 rasr);

    /*!
     *  @_nodoc
     *  ======== startup ========
     *  startup function to initialize MPU module
     */
    Void startup();

internal:

    /*
     *  ======== RegionEntry ========
     */
    struct RegionEntry {
        UInt32      baseAddress;
        UInt32      sizeAndEnable;
        UInt32      regionAttrs;
    };

    /*
     *  ======== regionEntry ========
     *  Array to hold statically configured MPU region entries
     */
    config RegionEntry regionEntry[];

    /*
     *  ======== isMemoryMapped ========
     *  Are the MPU registers memory mapped ?
     *
     *  This internal config param is used to determine whether
     *  or not a copy of the memory region settings needs to be
     *  maintained in the module state for use by ROV. If the
     *  MPU registers are memory mapped then a copy is not required,
     *  however, if they are accessed through CP15 regs, a copy is
     *  required for ROV.
     */
    config Bool isMemoryMapped;

    /*
     *  ======== disableAsm ========
     *  Assembly function to disable the MPU.
     */
    Void disableAsm();

    /*
     *  ======== enableAsm ========
     *  Assembly function to enable the MPU.
     */
    Void enableAsm();

    /*
     *  ======== disableBRAsm ========
     *  Assembly function to disable background region
     */
    Void disableBRAsm();

    /*
     *  ======== enableBRAsm ========
     *  Assembly function to enable background region
     */
    Void enableBRAsm();

    /*!
     *  ======== isEnabledAsm ========
     *  Assembly function that determines if the MPU is enabled
     */
    Bool isEnabledAsm();

    /*
     *  ======== setRegionAsm ========
     *  Write MPU region base address, size, enable status and protection
     *  attributes to respective CP15 registers.
     */
    Void setRegionAsm(UInt8 regionId, UInt32 regionBaseAddr, UInt32 regionSize,
        UInt32 regionAttrs);

    /*! Module state */
    struct Module_State {
        RegionEntry regionEntry[];  // Array to hold a copy of MPU region
                                    // settings
    }
}
