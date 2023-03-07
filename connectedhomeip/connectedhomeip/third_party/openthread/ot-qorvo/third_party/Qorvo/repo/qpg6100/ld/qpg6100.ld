/*
 * (c) 2019-2020, Qorvo
 */

/* Memory Spaces Definitions */
MEMORY
{
    /* Last 32 bytes of UCRAM in use by ROM bootloader stack. */
    RAM  (rxw) : ORIGIN = 0x20008000, LENGTH = 0x17fe0
    FLASH (rx) : ORIGIN = 0x4000000, LENGTH = 0x80000
    ROM   (rx) : ORIGIN = 0x04800000, LENGTH = 0x8000
}

SECTIONS
{
    eFLASH = 0x4080000;

    sSYSRAM = 0x20008000;
    eSYSRAM = 0x20010000;
    sUCRAM  = 0x20010000;
    eUCRAM  = 0x20020000;

    /* Linkerscript only for contiguous SYS and UCRAM */
    ASSERT(eSYSRAM == sUCRAM, "Non contiguous SYS and UCRAM")

    /*
     * User license is first - only for FLASH programs.
     *
     * BLE/MAC address entries (+0x10) in the user license skipped to
     * allow injection of custom values by programmer tool.
     */
    .native_user_license   0x4000000 + 0x10 : { KEEP(*(.native_user_license)); } > FLASH

    /* Add the jumptable at this fixed location */
    jumptable_start_offset = 0x800;
    datajumptable_start_address  = ORIGIN(FLASH) + jumptable_start_offset;
    flashjumptable_start_address = datajumptable_start_address + 0x40;
    .datajumptable   datajumptable_start_address  :  {. = ALIGN(4); KEEP(*(.datajumptable));    } > FLASH
    .flashjumptable  flashjumptable_start_address :  {. = ALIGN(4); KEEP(*(.flashjumptable));   } > FLASH
    .isr_vector : ALIGN(0x100) { KEEP(*(.isr_vector)); } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text) *(.text.*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata) *(.rodata.*)
        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;
    .rom :    { . = ALIGN(4); *(.rom)   *(.rom.*);    } > ROM

    /* Start of memory to be retained in sleep */
    _sretain = 0x20008000;

    /* Fixed sections */
    .mw_crc 0x20008000 : { . += 0x4; } > RAM
    .crc    0x20008004 : { . += 0x4; } > RAM
    .ret_hw 0x20008008 : { . += 0x1B4; } > RAM /* 0x20008008+0x1b4 = 0x200081BC = GP_MM_RAM_RETENTION_END */
    .ret_sw 0x200081BC : { . += 0x7C; }  > RAM
    ASSERT(. >= 0x200081c0, "gpmicro areas must fall into retention areas")
    .unused_area 0x20008238 : { . += 0x148; } > RAM
    /* PUF RAM Area */
    .pkey   0x20008380 : { . += 0x80;  }  > RAM
    .pufr   0x20008400 : { . += 0x400; }  > RAM

    /* Windowed sections */
    /* ram_regmap, events, pbm options and pbm data are taken from buffers in .lower_ram_retain */

    .lower_ram_retain_gpmicro_accessible (NOLOAD) :  { . = ALIGN(4); *(.lower_ram_retain_gpmicro_accessible) *(.lower_ram_retain_gpmicro_accessible.*); } > RAM
    ASSERT (. < 0x20008000+0x4000, ".lower_ram_retain_gpmicro_accessible area outside range")
    .lower_ram_retain (NOLOAD) : { . = ALIGN(4); *(.lower_ram_retain) *(.lower_ram_retain.*); } > RAM

    _elower_ram = . ;

    /* Check regions are allocated in lower ram */
    ASSERT(_elower_ram < eSYSRAM, "SYSRAM ram full")

    .data :
    {
        __data_start__ = .;
        *(vtable)
        . = ALIGN (4);
        *(.data) *(.data.*)
        PROVIDE (__ram_func_section_start = .);
        *(.ram)
        PROVIDE (__ram_func_section_end = .);

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    } > RAM AT > FLASH
    .bss :  { . = ALIGN(4); *(.bss)  *(.bss.*) *(COMMON); } > RAM

    /* End of memory to be retained */
    _eretain = . ;

    /* Scroll up to higher ram area for scratchpad variables */
    .higher_ram_noretain (NOLOAD) : {
        . = (_eretain > sUCRAM) ? ALIGN(4) : (sUCRAM - _eretain);
        _shigher_ram_noretain = . ;
        *(.higher_ram_noretain) *(.higher_ram_noretain.*);
        _ehigher_ram_noretain = . ;
    } > RAM
    /* Check if properly allocated in UCRAM only if any variables required specific allocation. */
    ASSERT((_ehigher_ram_noretain - _shigher_ram_noretain) > 0 ? (_shigher_ram_noretain >= sUCRAM) : 1, "higher_ram_noretain not in higher ram")

    _eram = .;

   /* Remove the debugging information from the standard libraries */
    /DISCARD/ : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .gpNvm eFLASH - 0x4000:
    {
        gpNvm_Start = . ;
        . = 0x4000 ;
        gpNvm_End = . ;
    } > FLASH

    /* Linker Symbols */
    _native_user_license_vpp    = ADDR(.isr_vector) >> 8;
    _loaded_user_license_vpp    = ADDR(.isr_vector) >> 8;
    _etext  = ADDR(.text) + SIZEOF(.text);
    _sidata = LOADADDR(.data);
    _sdata  = ADDR(.data);
    _edata  = ADDR(.data) + ALIGN(SIZEOF(.data), 4);
    _ldata  = _edata - _sdata;
    _sbss   = ADDR(.bss);
    _ebss   = ADDR(.bss)  + ALIGN(SIZEOF(.bss),  4);
    _lbss   = _ebss - _sbss;
    /* using remaining RAM for stack */
    _sstack = _eram;

    _estack = ORIGIN(RAM) + LENGTH(RAM);
    _lstack = _estack - _sstack;

    /* check minimum stack size is still available */
    min_stack_size = 0x800;
    stack_size     = _estack - _sstack;
    ASSERT(stack_size >= min_stack_size, "STACK too small")

    /* needed for ram retention configuration */
    __lowerram_retain_size     = _eretain < eSYSRAM ? _eretain - _sretain : eSYSRAM - _sretain;
    __higherram_retain_size    = _eretain < eSYSRAM ? 0 : _eretain - eSYSRAM;

    sw_retention_begin = ADDR(.ret_sw);
    sw_retention_end = ADDR(.ret_sw) + SIZEOF(.ret_sw);
    sw_retention_size = SIZEOF(.ret_sw);
}

ENTRY(reset_handler)
