OUTPUT_ARCH( "riscv" )

ENTRY( bl702_start )

MEMORY
{
  rtosrom(rxai!w) : ORIGIN = 0x21020000, LENGTH = (24K) /* low 128K is used by bootROM */
  blerom (rxai!w) : ORIGIN = 0x21026000, LENGTH = (40K)
  flash  (rxai!w) : ORIGIN = 0x23000000, LENGTH = (1M)
  tcm       (wxa) : ORIGIN = 0x42014000, LENGTH = (48K)
  ocram     (wxa) : ORIGIN = 0x42020000, LENGTH = (56K)

}

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;

  .rtosromro :
  {
    KEEP(*libbl702_freertos_rom.a:*(.text*))
    KEEP(*libbl702_freertos_rom.a:*(.rodata*))
    KEEP(*libbl702_freertos_rom.a:*(.sdata2.*))
  } > rtosrom

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

    . = ALIGN(4);
    _bl_static_cli_cmds_start = .;
    KEEP(*(.static_cli_cmds))
    *(.static_cli_cmds)
    _bl_static_cli_cmds_end = .;

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
  } >blerom

  .bleromrw :
  {
    KEEP(*(.bleromrw))
  } >ocram AT > flash

  .rtosromrw : ALIGN(16)
  {
    /*always put freetos under global_pointer with the following order. No change!*/
    . = . + 0x20; /* reserve (0x800-0x7E0) bytes, make sure scope of __global_pointer start from rtosromrw, DO NOT cover bleromrw */
    PROVIDE( __global_pointer_head$ =  . );
    PROVIDE( __global_pointer$ = . + 0x7E0 );
    KEEP(*libbl702_freertos_rom.a:*(.sdata.*))
    KEEP(*libbl702_freertos_rom.a:*(.sbss.*))
    KEEP(*libbl702_freertos_rom.a:*(.bss.*))
    KEEP(*libbl702_freertos_rom.a:*(COMMON))
  } > ocram AT > flash

  .bss (NOLOAD) :
  {
    PROVIDE( __bss_start = ADDR(.bss) );
    PROVIDE( __bss_end = ADDR(.bss) + SIZEOF(.bss) );

    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
  } >ocram

  .data : ALIGN(4)
  {
    PROVIDE( _data_load = LOADADDR(.data) );
    PROVIDE( _data_run = ADDR(.data) );
    PROVIDE( _data_run_end = ADDR(.data) + SIZEOF(.data));

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
  } >ocram AT > flash

  . = ALIGN(8);
  PROVIDE( _heap_start = . );
  PROVIDE( _heap_size = ADDR(.stack) - _heap_start );
  PROVIDE( _heap_end = _heap_start + _heap_size );

  .stack ORIGIN(ocram) + LENGTH(ocram) - __stack_size (NOLOAD) :
  {
    . = . + __stack_size;
    PROVIDE( _sp_main = . );
    __freertos_irq_stack_top = .;
  } >ocram

  /*SYMOBOL used in code*/
    PROVIDE( _ld_bl_static_cli_cmds_start = _bl_static_cli_cmds_start );
    PROVIDE( _ld_bl_static_cli_cmds_end   = _bl_static_cli_cmds_end );
    PROVIDE( _ld_ram_size0 = LENGTH(ocram) );
    PROVIDE( _ld_ram_addr0 = ORIGIN(ocram) );
    PROVIDE( _ld_ram_size1 = 0);
    PROVIDE( _ld_ram_addr1 = 0);
    PROVIDE( _ld_ram_size2 = 0);
    PROVIDE( _ld_ram_addr2 = 0);

}
