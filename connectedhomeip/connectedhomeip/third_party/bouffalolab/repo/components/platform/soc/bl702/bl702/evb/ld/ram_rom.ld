OUTPUT_ARCH( "riscv" )

ENTRY( bl702_start )

__EM_SIZE = DEFINED(ble_controller_init) ? 8K : 0K;

MEMORY
{
  flash     (wxa) : ORIGIN = 0x22030000, LENGTH = (2048K - 64K) /* we have 2MB ocram from 0x22020000 on fpga, use high ocram address as flash */
  tcm_ocram (wxa) : ORIGIN = 0x42014000, LENGTH = (48K + 64K - __EM_SIZE)
  hbnram    (wxa) : ORIGIN = 0x40010000, LENGTH = (4K)
}

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
  BOOT2_PT_ADDR = 0x4202DC00;

  .init :
  {
    KEEP (*(SORT_NONE(.init)))
  } >flash

  .text :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
  } >flash

  .rodata :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.sdata2.*)
    *(.gcc_except_table.*)

    /* camera driver */
    . = ALIGN(4);
    _bl_driver_camera_start = .;
    KEEP(*(.camera_desc))
    *(.camera_desc)
    _bl_driver_camera_end = .;

    /* static cli cmds */
    . = ALIGN(4);
    _bl_static_cli_cmds_start = .;
    KEEP(*(.static_cli_cmds))
    *(.static_cli_cmds)
    _bl_static_cli_cmds_end = .;

    /* static blog code1 */
    . = ALIGN(4);
    _bl_static_blogcomponent_code_start = .;
    KEEP(SORT(*)(.static_blogcomponent_code*))
    *(.static_blogcomponent_code*)
    _bl_static_blogcomponent_code_end = .;

    /* static blog code2 */
    . = ALIGN(4);
    _bl_static_blogfile_code_start = .;
    KEEP(SORT(*)(.static_blogfile_code*))
    *(.static_blogfile_code*)
    _bl_static_blogfile_code_end = .;

    /* static blog code3 */
    . = ALIGN(4);
    _bl_static_blogpri_code_start = .;
    KEEP(SORT(*)(.static_blogpri_code*))
    *(.static_blogpri_code*)
    _bl_static_blogpri_code_end = .;

    *(.gnu.linkonce.r.*)
  } >flash

  .init_array :
  {
    __init_array_start = .;
    KEEP(*(.init_array))
    __init_array_end = .;
  } >flash

  .bleromrw (NOLOAD) :
  {
    . = . + 0x84;
  } >tcm_ocram

  .rtosromrw (NOLOAD) : ALIGN(16)
  {
    /*always put freetos under global_pointer with the following order. No change!*/
    . = . + 0x20; /* reserve (0x800-0x7E0) bytes, make sure scope of __global_pointer start from rtosromrw, DO NOT cover bleromrw */
    PROVIDE( __global_pointer_head$ =  . );
    PROVIDE( __global_pointer$ = . + 0x7E0 );
    . = . + 0x4A0;
  } >tcm_ocram

  .hbn : ALIGN(4)
  {
    PROVIDE( _hbn_load = LOADADDR(.hbn) );
    PROVIDE( _hbn_run = ADDR(.hbn) );
    PROVIDE( _hbn_run_end = ADDR(.hbn) + SIZEOF(.hbn));

    *(.hbn_code.*)
    *(.hbn_data)
    *(.retention)
  } >hbnram AT > flash

  .hbn_noinit (NOLOAD) :
  {
    *(.hbn_noinit)
    *(.retention_noinit)

    /* PDS backup address */
    . = ALIGN(4);
    PROVIDE ( __ld_pds_bak_addr = . );
    . = . + 256;
  } >hbnram

  .rsvd (NOLOAD) :
  {
    *(.rsvd_data)
    *(.ble_rsvd_mem)
  } >hbnram

  .tcmcode : ALIGN(4)
  {
    PROVIDE( _tcm_load = LOADADDR(.tcmcode) );
    PROVIDE( _tcm_run = ADDR(.tcmcode) );
    PROVIDE( _tcm_run_end = ADDR(.tcmcode) + SIZEOF(.tcmcode));

    *(.tcm_code.*)
    *(.tcm_const.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)
  } >flash

  .data : ALIGN(4)
  {
    PROVIDE( _data_load = LOADADDR(.data) );
    PROVIDE( _data_run = ADDR(.data) );
    PROVIDE( _data_run_end = ADDR(.data) + SIZEOF(.data));

    /* put bl702_rf_private.o at the top to avoid ocram conflict during rf calibration */
    *libbl702_rf.a:bl702_rf_private.o(.retention .retention_noinit)
    *libbl702_rf.a:bl702_rf_private.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)

    *(.pds_code.*)
    *(.data .data.*)
    *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)

    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
    
    . = ALIGN(8);
    *(._k_queue.static.*)
    *(._k_sem.static.*)
    *(._k_mutex.static.*)
    _bt_gatt_service_static_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
    _bt_gatt_service_static_list_end = .;
    _bt_l2cap_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
    _bt_l2cap_fixed_chan_list_end = .;
  } >flash

  .boot2 (NOLOAD) :
  {
    PROVIDE ( __boot2_pt_addr_start = . );
    *(.bss.boot2_partition_table)
    PROVIDE ( __boot2_pt_addr_end   = . );

    PROVIDE ( __boot2_flashCfg_start = . );
    *(.bss.boot2_flashCfg)
    PROVIDE ( __boot2_flashCfg_end = . );
  } >tcm_ocram

  .bss (NOLOAD) :
  {
    PROVIDE( __bss_start = ADDR(.bss) );
    PROVIDE( __bss_end = ADDR(.bss) + SIZEOF(.bss) );

    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
  } >tcm_ocram

  . = ALIGN(8);
  PROVIDE( _heap_start = . );
  PROVIDE( _heap_size = ADDR(.stack) - _heap_start );

  PROVIDE( _heap2_start = 0 );
  PROVIDE( _heap2_size = 0 );

  .stack ORIGIN(tcm_ocram) + LENGTH(tcm_ocram) - __stack_size (NOLOAD) :
  {
    PROVIDE ( _sp_base = . );
    . = . + __stack_size;
    PROVIDE( _sp_main = . );
    __freertos_irq_stack_top = .;
  } >tcm_ocram


  /* camera driver */
  PROVIDE( _ld_bl_driver_camera_start = _bl_driver_camera_start );
  PROVIDE( _ld_bl_driver_camera_end   = _bl_driver_camera_end );

  /* cli */
  PROVIDE( _ld_bl_static_cli_cmds_start = _bl_static_cli_cmds_start );
  PROVIDE( _ld_bl_static_cli_cmds_end   = _bl_static_cli_cmds_end );

  /* blog */
  PROVIDE( _ld_bl_static_blogcomponent_code_start = _bl_static_blogcomponent_code_start );
  PROVIDE( _ld_bl_static_blogcomponent_code_end   = _bl_static_blogcomponent_code_end );
  PROVIDE( _ld_bl_static_blogfile_code_start = _bl_static_blogfile_code_start );
  PROVIDE( _ld_bl_static_blogfile_code_end   = _bl_static_blogfile_code_end );
  PROVIDE( _ld_bl_static_blogpri_code_start = _bl_static_blogpri_code_start );
  PROVIDE( _ld_bl_static_blogpri_code_end   = _bl_static_blogpri_code_end );

  /* ram information */
  PROVIDE( _ld_ram_size0 = LENGTH(flash) );
  PROVIDE( _ld_ram_addr0 = ORIGIN(flash) );
  PROVIDE( _ld_ram_size1 = LENGTH(tcm_ocram) - SIZEOF(.stack) );
  PROVIDE( _ld_ram_addr1 = ORIGIN(tcm_ocram) );
  PROVIDE( _ld_ram_size2 = LENGTH(hbnram) );
  PROVIDE( _ld_ram_addr2 = ORIGIN(hbnram) );
  PROVIDE( _ld_ram_size3 = SIZEOF(.stack) );
  PROVIDE( _ld_ram_addr3 = ADDR(.stack) );

  /* BOOT2 parameter */
  PROVIDE ( __boot2_pt_addr_src = BOOT2_PT_ADDR );

  /* EM size */
  PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);

  PROVIDE(xTaskGetTickCount = 0x0000000021022642);
  PROVIDE(xTaskGetTickCountFromISR = 0x0000000021022658);
  PROVIDE(pvPortMalloc = 0x0000000021024762);
  PROVIDE(vPortFree = 0x000000002102483a);
  PROVIDE(vTaskNotifyGiveFromISR = 0x0000000021023a28);
  PROVIDE(vTaskSwitchContext = 0x0000000021022af2);
  PROVIDE(ulTaskNotifyTake = 0x0000000021023674);
  PROVIDE(vTaskExitCritical = 0x0000000021023520);
  PROVIDE(vTaskEnterCritical = 0x0000000021023510);
  PROVIDE(xTaskGetCurrentTaskHandle = 0x000000002102327e);
  PROVIDE(xQueueSemaphoreTake = 0x0000000021020ce8);
  PROVIDE(xQueueGenericSend = 0x0000000021020834);
  PROVIDE(xQueueGenericSendFromISR = 0x0000000021020a4c);
  PROVIDE(xTaskCreateStatic = 0x000000002102204e);
  PROVIDE(xTaskCreate = 0x00000000210220e4);
  PROVIDE(xQueueCreateMutex = 0x0000000021020a1c);
  PROVIDE(xQueueCreateMutexStatic = 0x0000000021020994);
  PROVIDE(vQueueDelete = 0x00000000210211d8);
  PROVIDE(xQueueGenericCreateStatic = 0x00000000210206c2);
  PROVIDE(xQueueGenericCreate = 0x0000000021020744);
  PROVIDE(xQueueReceive = 0x0000000021020b8a);
  PROVIDE(uxQueueMessagesWaiting = 0x0000000021021168);
  PROVIDE(vTaskDelay = 0x00000000210229dc);
  PROVIDE(vTaskDelayUntil = 0x0000000021022968);
  PROVIDE(xPortGetFreeHeapSize = 0x00000000210248ce);
  PROVIDE(vTaskList = 0x0000000021023534);
  PROVIDE(xTimerGenericCommand = 0x0000000021023d2c);
  PROVIDE(xTimerCreateTimerTask = 0x0000000021023bde);
  PROVIDE(xTimerCreate = 0x0000000021023c36);
  PROVIDE(xTimerCreateStatic = 0x0000000021023ca6);
  PROVIDE(xQueueCreateCountingSemaphoreStatic = 0x00000000210207c2);
  PROVIDE(xQueueCreateCountingSemaphore = 0x0000000021020800);
  PROVIDE(pTrapNetCounter = __global_pointer_head$);
  PROVIDE(TrapNetCounter = __global_pointer_head$ + 0x5C);
  PROVIDE(vEventGroupDelete = 0x00000000210203be);
  PROVIDE(xEventGroupWaitBits = 0x0000000021020086);
  PROVIDE(xEventGroupCreateStatic = 0x0000000021020000);
  PROVIDE(xEventGroupSetBits = 0x00000000210201e0);
  PROVIDE(xStreamBufferGenericCreateStatic = 0x00000000210215c0);
  PROVIDE(xStreamBufferReceive = 0x00000000210219ae);
  PROVIDE(xStreamBufferSend = 0x00000000210217a8);
  PROVIDE(pvTimerGetTimerID = 0x0000000021024114);
  PROVIDE(xTaskGenericNotify = 0x00000000210249d0);
  PROVIDE(xTaskGenericNotifyFromISR = 0x00000000210249e4);
  PROVIDE(xQueueGiveMutexRecursive = 0x00000000210209c8);
  PROVIDE(xQueueTakeMutexRecursive = 0x0000000021020e70);
  PROVIDE(xTaskGetTickCount2 = 0x0000000021022648);
  PROVIDE(xQueueGiveFromISR = 0x0000000021020b0e);
  PROVIDE(vTaskDelete = 0x0000000021022160);
  PROVIDE(uxTaskGetStackHighWaterMark = 0x000000002102323c);
  PROVIDE(pcTaskGetName = 0x0000000021022664);
  PROVIDE(vTaskStartScheduler = 0x00000000210225ba);
  PROVIDE(vPortDefineHeapRegions = 0x00000000210248da);
}
