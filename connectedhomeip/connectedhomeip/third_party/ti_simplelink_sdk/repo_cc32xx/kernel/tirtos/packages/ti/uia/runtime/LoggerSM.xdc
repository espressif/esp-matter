/*
 * Copyright (c) 2012-2015, Texas Instruments Incorporated
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
 *  ======== LoggerSM.xdc ========
 */
import xdc.runtime.ITimestampClient;
import xdc.runtime.IHeap;
import xdc.runtime.Types;
import xdc.runtime.Log;
import xdc.runtime.ILogger;
import xdc.runtime.Error;
import xdc.runtime.Diags;
import xdc.rov.ViewInfo;

/*!
 *  ======== LoggerSM ========
 *  Logger implementation that stores Log records into shared memory
 *
 *  This logger implementation stores xdc.runtime.Log records into shared memory.
 *  This logger is intended to be used for SoC system (e.g. EVMTI816X),
 *  where Linux is running on the host core (e.g. CortexA8) and
 *  SYS/BIOS is running on the targets (e.g. M3 and DSP).
 *
 *  This logger is used on the targets. The host is then responsible
 *  for reading the shared memory and processing the records. UIA
 *  ships the ti/uia/linux/LoggerSM module that can be used to
 *  process the records. UIA also ships a cmdline app,
 *  ti/uia/examples/evmti816x/loggerSMDump.out, that show
 *  how to use the linux/LoggerSM module.
 *
 *  @a(Cache management)
 *
 *  The location of the shared memory that is used by LoggerSM must be
 *  specified by the application. This shared memory must be in a
 *  non-cacheable region. The '{@link #bufSection}' configuration
 *  parameter can be used to help place the Logger's buffer.
 *  Refer to the device specific LoggerSM
 *  examples (e.g. ti/uia/examples/evmti816x/readme.txt) to see how this
 *  was accomplished.
 *
 *  @a(Partitions)
 *
 *  The application gets to specify the size of the shared region. This
 *  size is divided between each of the targets. For example, on the evmti816x,
 *  if the '{@link #sharedMemorySize}' was 0x3000, each target,
 *  dsp, videoM3 and vpssM3, would get 0x1000 amount of shared memory for
 *  log records. Each target region is called a partition. Since LoggerSM is a
 *  generic logger that can be used on multiple devices, the number of
 *  partitions (e.g. number of targets using the shared memory) is specified
 *  wtih the '{@link #numPartitions}' configuration option. Each target needs
 *  a unique '{@link #partitionId}' also. For example, on evmti816x examples,
 *  the dsp is assigned 0, videoM3 is assigned 1 and vpssM3 is assigned 2. This
 *  corresponds with the IPC Multicore Ids for simplicity sake. Note: the
 *  partition id can be set during target startup also via the
 *  '{@link #setPartitionId}' API.
 *
 *  LoggerSM supports multiple instances, but all instances are writing to
 *  the same shared memory (in a thread safe manner). This was done to
 *  simplify the design. Because of this, application really should only
 *  create one LoggerSM instance.
 *
 *  @a(decode and overwrite)
 *  The LoggerSM module has two key configuration options: '{@link #decode}'
 *  and '{@link #overwrite}'
 *
 *  The '{@link #decode}' configuration determines whether the target will
 *  decode the Log record during the Log call. If '{@link #decode}'  is true,
 *  the Log record is converted to an ASCII string and then written into the
 *  shared memory. This approach is expensive from a performance standpoint.
 *  Its value is that it is easy to manage and view on the host
 *  (e.g. ti/uia/examples/evmti816x/loggerSMDump.out prints the ASCII strings
 *  to the console on the CortexA8).
 *
 *  If '{@link #decode}'  is false, the Log records are not decoded. Instead
 *  they are stored in the shared memory as binary data. This allows the
 *  Log calls to be much faster. The burden for decoding is on the readers side.
 *  For example, ti/uia/examples/evmti816x/loggerSMDump.out dumps the encoded
 *  records into a binary file that can be post-processed in
 *  CCS' System Analyzer.
 *
 *  The '{@link #overwrite}' configuration determines whether the target
 *  will overwrite old records
 *  when the shared memory is full. The default setting is false, so when the
 *  logger is full, new records are dropped. This mode allows the reader
 *  (e.g. ti/uia/examples/evmti816x/loggerSMDump.out) to read the records
 *  while the target is running.
 *
 *  When '{@link #overwrite}', old records are overwritten when the logger is
 *  full. However, the reader should only run when the targets are halted (or
 *  crashed).
 *
 *  @a(caveats)
 *  @p(blist)
 *  -Currently LoggerSM assumes the endianness and word size of the host
 *  and targets are the same.
 *  -{@link ti.uia.sysbios.LoggingSetup} and {@link ti.uia.runtime.LogSync}
 *  cannot be used with LoggerSM.
 *  @p
 */
@ModuleStartup      /* Initialize static instances */
@Template("./LoggerSM.xdt")
module LoggerSM inherits xdc.runtime.IFilterLogger
{
    /*!
     *  @_nodoc
     *  ======== ModuleView ========
     */
    metaonly struct ModuleView {
        Bool       isTimestampEnabled;
        Bool       decode;
        Bool       overwrite;
    }

    /*!
     *  @_nodoc
     *  ======== InstanceView ========
     */
    metaonly struct InstanceView {
        String    label;
    }

    /*!
     *  @_nodoc
     *  ======== rovViewInfo ========
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Module',
                    {
                        type: ViewInfo.MODULE,
                        viewInitFxn: 'viewInitModule',
                        structName: 'ModuleView'
                    }
                ],
                ['Instances',
                    {
                        type: ViewInfo.INSTANCE,
                        viewInitFxn: 'viewInitInstances',
                        structName: 'InstanceView'
                    }
                ],
            ]
        });

    /*! Error raised if get or setFilterLevel receive a bad level value */
    config Error.Id E_badLevel =
        {msg: "E_badLevel: Bad filter level value: %d"};

    /*!
     *  ======== isTimestampEnabled ========
     *  Enable or disable logging the 64b local CPU timestamp
     *  at the start of each event
     */
    config Bool isTimestampEnabled = true;

    /*!
     *  ======== decode ========
     *  Flag to determine whether to decode the events in shared memory
     *
     *  If true, all the events will be decoded into ASCII strings
     *  when it is written into shared memory. If false, binary
     *  data is written instead.
     */
    config Bool decode = true;

    /*!
     *  ======== overwrite ========
     *  Flag to determine whether to overwrite records when full
     *
     *  If true and when the buffer is full, the logger will overwrite
     *  the oldest record. Reading the records can only occur when the
     *  targets have been halted.
     *
     *  If false and when the buffer is full, the logger will discard
     *  the new record.
     */
    config Bool overwrite = false;

    /*!
     *  ======== level1Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL1
     *
     *  See '{@link #level4Mask}' for details.
     */
    config Diags.Mask level1Mask = 0;

    /*!
     *  ======== level2Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL2
     *
     *  See '{@link #level4Mask}' for details.
     */
    config Diags.Mask level2Mask = 0;

    /*!
     *  ======== level3Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL3
     *
     *  See '{@link #level4Mask}' for details.
     */
    config Diags.Mask level3Mask = 0;

    /*!
     *  ======== level4Mask ========
     *  Mask of diags categories whose initial filtering level is Diags.LEVEL4
     *
     *  If 'filterByLevel' is true, then all LoggerBuf instances will filter
     *  incoming events based on their event level.
     *
     *  The LoggerSM module allows for specifying a different filter level for
     *  every Diags bit. These filtering levels are module wide; LoggerBuf does
     *  not support specifying the levels on a per-instance basis.
     *
     *  The setFilterLevel API can be used to change the filtering levels at
     *  runtime.
     *
     *  The default filtering levels are assigned using the 'level1Mask' -
     *  'level4Mask' config parameters. These are used to specify, for each of
     *  the four event levels, the set of bits which should filter at that
     *  level by default.
     *
     *  The default filtering configuration sets the filter level to
     *  Diags.LEVEL4 for all logging-related diags bits so that all events are
     *  logged by default.
     */
    config Diags.Mask level4Mask = Diags.ALL_LOGGING;

    /*!
     *  ======== partitionId ========
     *  Unique id for each core using the shared memory
     */
     metaonly config Int partitionId = 0;

     /*!
     *  ======== numPartitions ========
     *  Number of partitions sharing the shared memory
     */
     config Int numPartitions = 3;

     /*!
      *  ======== sharedMemorySize ========
      *  Total size of shared memory in MAU that will be divided by the number
      *  of partitions
      */
     config SizeT sharedMemorySize = 0x20000;

    /*!
     *  ======== userTimestamp ========
     *  Use the user configured Timestamp_get64() instead of TCSL/TCSH.
     *
     *  For C6x devices, LoggerSM defaults to using the built-in hardware
     *  counter, TCSL/TCSH for timestamping.  This improves the performance
     *  of Log statements.  However, in some cases, the user may want to
     *  provide their own timestamp provider to LoggerSM, instead of using
     *  TCSL/TCSH.
     *  If userTimestamp is set to true, Timestamp_get64() will be used
     *  instead of reading TCSL/TCSH.  For non-C6x devices, changing
     *  this configuration parameter has no effect.
     */
    metaonly config Bool userTimestamp = false;

    /*!
     *  ======== bufSection ========
     *  Section name for the buffer in shared memory
     */
    metaonly config String bufSection = null;

     /*!
      *  ======== setPartitionId ========
      *  Change the partitionId at runtime.
      *
      *  Must be called early before module startup occurs.
      *  Generally the best place to do this is via the
      *  xdc.runtime.Startup.firstFxns array.   If using the {@link #setSharedMemory}
      *  API, make sure that LoggerSM_setPartitionId() is called before
      *  {@link #setSharedMemory}, if changing the partition Id.
      */
     Void setPartitionId(Int partitionId);

     /*!
     *  ======== MetaData ========
     *  This data is added to the RTA MetaData file.
     */
    @XmlDtd metaonly struct MetaData {
        Int instanceId;
        Int priority;
    }

    /*!
     *  ======== setSharedMemory ========
     *  Specify the shared memory to be used by LoggerSM
     *
     *  This runtime API can be called once to initialize the shared memory
     *  that all LoggerSM instances use. This API can only be called
     *  if '{@link #sharedMemorySize}' is set to zero. If
     *  '{@link #sharedMemorySize}' is non-zero, the shared memory is
     *  statically defined and initialized instead. Since the memory
     *  is already defined, this API cannot change it.
     *  Also, the {@link #partitionId} must not be changed after calling
     *  this API.
     *
     *  The Linux host utility that reads the buffer requires it to be
     *  aligned on a page boundary.  So make sure that the buffer passed
     *  to setSharedMemory() is page-aligned (e.g. on a 4096 byte boundary).
     *
     *  @param(sharedMemory) Base address of the shared memory to be used
     *                       by LoggerSM.
     *  @param(sharedMemorySize) Size of the shared memory.
     *
     *  @a(returns)
     *  This function returns TRUE if successful. It returns FALSE if not.
     *  The reasons for failure are either '{@link #sharedMemorySize}' is
     *  non-zero or the API has been called multiple times.
     */
    @DirectCall
    Bool setSharedMemory(Ptr sharedMemory, Bits32 sharedMemorySize);

instance:
    /*!
     *  ======== create ========
     *  Create a `LoggerSM` logger
     *
     *  @see LoggerSM#Params
     */
    @DirectCall
    create();

    /*!
     *  ======== enable ========
     *  Enable a log
     *
     *  @a(returns)
     *  The function returns the state of the log (`TRUE` if enabled,
     *  `FALSE` if disabled) before the call. That allow clients to restore
     *  the previous state.
     */
    @DirectCall
    override Bool enable();

    /*!
     *  ======== disable ========
     *  Disable a log
     *
     *  Events written to a disabled log are silently discarded.
     *
     *  @a(returns)
     *  The function returns the state of the log (`TRUE` if enabled,
     *  `FALSE` if disabled) before the call. That allow clients to restore
     *  the previous state.
     */
    @DirectCall
    override Bool disable();

    /*!
     *  ======== write0 ========
     *  Process a log event with 0 arguments and the calling address.
     *
     *  Same as `write4` except with 0 arguments rather than 4.
     *  @see #write4()
     */
    @DirectCall
    override Void write0(Log.Event evt, Types.ModuleId mid);

    /*!
     *  ======== write1 ========
     *  Process a log event with 1 arguments and the calling address.
     *
     *  Same as `write4` except with 1 arguments rather than 4.
     *  @see #write4()
     */
    @DirectCall
    override Void write1(Log.Event evt, Types.ModuleId mid, IArg a1);

    /*!
     *  ======== write2 ========
     *  Process a log event with 2 arguments and the calling address.
     *
     *  Same as `write4` except with 2 arguments rather than 4.
     *
     *  @see #write4()
     */
    @DirectCall
    override Void write2(Log.Event evt, Types.ModuleId mid, IArg a1, IArg a2);

    /*!
     *  ======== write4 ========
     *  Process a log event with 4 arguments and the calling address.
     *
     *  @see ILogger#write4()
     */
    @DirectCall
    override Void write4(Log.Event evt, Types.ModuleId mid, IArg a1, IArg a2,
                         IArg a3, IArg a4);

    /*!
     *  ======== write8 ========
     *  Process a log event with 8 arguments and the calling address.
     *
     *  Same as `write4` except with 8 arguments rather than 4.
     *
     *  @see #write4()
     */
    @DirectCall
    override Void write8(Log.Event evt, Types.ModuleId mid, IArg a1, IArg a2,
                         IArg a3, IArg a4, IArg a5, IArg a6, IArg a7, IArg a8);

    /*!
     *  ======== setFilterLevel ========
     *  Sets the level of detail that instances will log.
     *
     *  Events with the specified level or higher will be logged, events
     *  below the specified level will be dropped.
     *
     *  Events are filtered first by diags category, then by level. If an
     *  event's diags category is disabled in the module's diags mask, then it
     *  will be filtered out regardless of level. The event will not even be
     *  passed to the logger.
     *
     *  This API allows for setting the filtering level for more than one
     *  diags category at a time. The mask parameter can be a single category
     *  or multiple categories combined, and the level will be set for all of
     *  those categories.
     *
     *  @param(mask) The diags categories to set the level for
     *  @param(filterLevel) The new filtering level for the specified
     *                      categories
     */
    @DirectCall
    override Void setFilterLevel(Diags.Mask mask, Diags.EventLevel filterLevel);

    /*!
     *  ======== getFilterLevel ========
     *  Returns the mask of diags categories currently set to the specified
     *  level.
     *
     *  See '{@link #setFilterLevel}' for an explanation of level filtering.
     */
    @DirectCall
    override Diags.Mask getFilterLevel(Diags.EventLevel level);

internal:

    const UInt16 VERSION = 1;

    /*!
     *  ======== filterOutEvent ========
     */
    @DirectCall
    Bool filterOutEvent(Diags.Mask mask);

    /*!
     *  ======== Module_State ========
     */
    struct Module_State {
        Int partitionId;
        Diags.Mask level1;
        Diags.Mask level2;
        Diags.Mask level3;
        SharedObj *sharedObj;
        Char sharedBuffer[];
        Bits16 serial;
        Bool enabled;
    };

    /*!
     *  ======== SharedObj ========
     */
    struct SharedObj {
        Bits32 headerTag;
        Bits32 version;
        Bits32 numPartitions;
        Char *endPtr;
        volatile Char *readPtr;
        Char *writePtr;
        Char *buffer;
        Bits32 bufferSizeMAU;
        Bits32 droppedEvents;
        Bits16 moduleId;
        Bits16 instanceId;
        Bits16 decode;
        Bits16 overwrite;
    };
}
