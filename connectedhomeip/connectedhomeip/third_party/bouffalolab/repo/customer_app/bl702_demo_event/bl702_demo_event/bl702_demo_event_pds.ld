OUTPUT_ARCH( "riscv" )

ENTRY( bl702_start )

__EM_SIZE = DEFINED(ble_controller_init) ? 8K : 0K;
__CACHE_SIZE = 16K;

MEMORY
{
  flash (rxai!w) : ORIGIN = 0x23000000, LENGTH = (2M)
  tcm      (wxa) : ORIGIN = 0x42014000, LENGTH = (48K)
  ocram_1  (wxa) : ORIGIN = 0x42020000, LENGTH = (24K)  /* will not be initialized in bootrom */
  ocram_2  (wxa) : ORIGIN = 0x42026000, LENGTH = (16K)  /* will be initialized in bootrom */
  ocram_3  (wxa) : ORIGIN = 0x4202A000, LENGTH = (24K - __EM_SIZE)  /* will not be initialized in bootrom */
  hbnram   (wxa) : ORIGIN = 0x40010000, LENGTH = (4K)
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

  .bleromro :
  {
    KEEP(*(.bleromro))
  } >flash

  .bleromrw (NOLOAD) :
  {
    KEEP(*(.bleromrw))
  } >ocram_1

  .hbn : ALIGN(4)
  {
    PROVIDE( _hbn_load = LOADADDR(.hbn) );
    PROVIDE( _hbn_run = ADDR(.hbn) );
    PROVIDE( _hbn_run_end = ADDR(.hbn) + SIZEOF(.hbn));

    /* put bl702_rf_private.o in hbnram to avoid ocram conflict during rf calibration */
    *libbl702_rf.a:bl702_rf_private.o(.retention .retention_noinit)
    *libbl702_rf.a:bl702_rf_private.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    PROVIDE ( _rf_data_end = . );

    /* tcm code that should be retentive after hbn wakeup */
    *libbl702_std.a:bl702_romapi.o(.tcm_code.* .sclock_rlt_code.*)
    *libbl702_std.a:bl702_glb.o(.tcm_code.* .sclock_rlt_code.*)
    *libbl702_std.a:bl702_aon.o(.tcm_code.* .sclock_rlt_code.*)
    PROVIDE ( _hbn_restore_tcm_run_end = . );

    /* data that should be retentive after hbn wakeup */
    *liblmac154.a:*(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libbl702_std.a:*(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libhosal.a:bl_adc.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libhosal.a:bl_gpio.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libhosal.a:bl_sys.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libhosal.a:bl_timer.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libhosal.a:bl_uart.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    *libhosal.a:hosal_uart.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
    PROVIDE ( _hbn_restore_data_run_end = . );

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
    . = . + 116;
  } >hbnram

  .rsvd (NOLOAD) :
  {
    *libutils.a:utils_log.o(.bss.log_buf)
    *libcli.a:cli.o(.bss.argvall.*)
    *libbl702.a:debug.o(.bss.string)
    *(.bss.aos_loop_proc_stack.*)
    *(.bss.aos_loop_proc_task.*)

    *(.rsvd_data)
    *(.ble_rsvd_mem)
  } >ocram_2

  .tcmcode : ALIGN(4)
  {
    PROVIDE( _tcm_load = LOADADDR(.tcmcode) );
    PROVIDE( _tcm_run = ADDR(.tcmcode) );
    PROVIDE( _tcm_run_end = ADDR(.tcmcode) + SIZEOF(.tcmcode));

    /* tcm code that should be retored after pds wakeup */
    *libbl702_std.a:bl702_pds.o(.tcm_code.* .sclock_rlt_code.*)
    *libblecontroller_702_m0s1p.a:rwip.o(.tcm_code.* .sclock_rlt_code.*)
    PROVIDE ( _pds_restore_tcm_run_end = . );

    *(.tcm_code.*)
    *(.tcm_const.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)

    *(.tcm_data)
    *(.data.uart_stdio)
    *libblestack.a:ble_cli_cmds.o(.sdata.* .srodata.* .sbss.* .bss.* COMMON)
  } >tcm AT > flash

  .data : ALIGN(4)
  {
    PROVIDE( _data_load = LOADADDR(.data) );
    PROVIDE( _data_run = ADDR(.data) );
    PROVIDE( _data_run_end = ADDR(.data) + SIZEOF(.data));

    *(.pds_code.*)
    *(.data .data.*)
    *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    /*always put freetos under global_pointer with the following order. No change!*/
    PROVIDE( __global_pointer$ = . + 0x7F0 );
    KEEP(*libbl702_freertos.a:*(.sdata.*))
    KEEP(*libbl702_freertos.a:*(.sbss.*))
    KEEP(*libbl702_freertos.a:*(.bss.*))
    KEEP(*libbl702_freertos.a:*(COMMON))
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
  } >ocram_1 AT > flash

  .boot2 (NOLOAD) :
  {
    PROVIDE ( __boot2_pt_addr_start = . );
    *(.bss.boot2_partition_table)
    PROVIDE ( __boot2_pt_addr_end   = . );

    PROVIDE ( __boot2_flashCfg_start = . );
    *(.bss.boot2_flashCfg)
    PROVIDE ( __boot2_flashCfg_end = . );
  } >ocram_1

  .bss (NOLOAD) :
  {
    PROVIDE( __bss_start = ADDR(.bss) );
    PROVIDE( __bss_end = ADDR(.bss) + SIZEOF(.bss) );

    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
  } >ocram_1

  . = ALIGN(8);
  PROVIDE( _heap_start = . );
  PROVIDE( _heap_size = ORIGIN(ocram_1) + LENGTH(ocram_1) - _heap_start );

  PROVIDE( _heap2_start = ORIGIN(ocram_3) );
  PROVIDE( _heap2_size = LENGTH(ocram_3) );

  .stack ORIGIN(ocram_2) + LENGTH(ocram_2) - __stack_size (NOLOAD) :
  {
    PROVIDE ( _sp_base = . );
    . = . + __stack_size;
    PROVIDE( _sp_main = . );
    __freertos_irq_stack_top = .;
  } >ocram_2


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
  PROVIDE( _ld_ram_size1 = LENGTH(tcm) + LENGTH(ocram_1) );
  PROVIDE( _ld_ram_addr1 = ORIGIN(tcm) );
  PROVIDE( _ld_ram_size2 = LENGTH(ocram_3) - SIZEOF(.stack) );
  PROVIDE( _ld_ram_addr2 = ORIGIN(ocram_3) );
  PROVIDE( _ld_ram_size3 = SIZEOF(.stack) );
  PROVIDE( _ld_ram_addr3 = ADDR(.stack) );

  /* BOOT2 parameter */
  PROVIDE ( __boot2_pt_addr_src = BOOT2_PT_ADDR );

  /* EM size */
  PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);
}
