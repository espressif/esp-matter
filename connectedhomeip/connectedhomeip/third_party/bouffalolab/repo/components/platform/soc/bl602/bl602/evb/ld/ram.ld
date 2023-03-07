OUTPUT_ARCH( "riscv" )

ENTRY( bl602_start )

MEMORY
{
  ram (wxa) : ORIGIN = 0x22008000, LENGTH = (48K + 48K + 104K + 8K)
}

SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;

  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >ram

  .text           :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
  } >ram

  .preinit_array :
  {
    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;
  } > ram

  .init_array :
  {
    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP (*(.init_array))
    __init_array_end = .;
  } > ram 

  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
    *(.sdata2.*)
    _bl_static_cli_cmds_start = .;
    KEEP(*(.static_cli_cmds))
    *(.static_cli_cmds)
    _bl_static_cli_cmds_end = .;
    *(.gnu.linkonce.r.*)
  } >ram

  .data          :
  {
    PROVIDE( _data_load = LOADADDR(.data) );
    PROVIDE( _data_run = ADDR(.data) );
    PROVIDE( _data_run_end = ADDR(.data) + SIZEOF(.data));

    *(.data .data.*)
    *(.gnu.linkonce.d.*)

    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)

    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
  } >ram

  .bss (NOLOAD)   :
  {
    PROVIDE( __bss_start = ADDR(.bss) );
    PROVIDE( __bss_end = ADDR(.bss) + SIZEOF(.bss) );

    PROVIDE( __wifi_bss_start = ADDR(.bss) );
    PROVIDE( __wifi_bss_end = ADDR(.bss) );

    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
  } >ram

  PROVIDE( _heap_start = . );
  PROVIDE( _heap_size = ADDR(.stack) - _heap_start );

  .stack ORIGIN(ram) + LENGTH(ram) - __stack_size (NOLOAD) :
  {
    . = . + __stack_size;
    PROVIDE( _sp_main = . );
    __freertos_irq_stack_top = .;
  } >ram


  /*SYMOBOL used in code*/
    PROVIDE( _ld_bl_static_cli_cmds_start = _bl_static_cli_cmds_start );
    PROVIDE( _ld_bl_static_cli_cmds_end   = _bl_static_cli_cmds_end );
    PROVIDE( _ld_ram_size0 = LENGTH(ram) );
    PROVIDE( _ld_ram_addr0 = ORIGIN(ram) );
    PROVIDE( _ld_ram_size1 = 0);
    PROVIDE( _ld_ram_addr1 = 0);
    PROVIDE( _ld_ram_size2 = 0);
    PROVIDE( _ld_ram_addr2 = 0);
 
}
