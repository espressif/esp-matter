/*
 * Copyright (c) 2020-2022, Qorvo Inc
 */
/* Generated for A25L080 SPIFLASH */


upgrade_image_user_license_start__    = 0x4005000;
upgrade_image_user_license_end__      = 0x40050ff;


/* Location of the application entry point */
/* flash_start_abs 0x4000000 application_offset 0x6000 */
__app_Start__                             =  0x4006000 ;
/* Memory Spaces Definitions */
MEMORY
{
    RAM  (rxw)      : ORIGIN = 0x20038000, LENGTH = 0x20000
    FLASH (rx)      : ORIGIN = 0x04000000, LENGTH = 0x100000
    FLASH_VIRT (rx) : ORIGIN = 0x04400000, LENGTH = 0x100000
    ROM   (rx)      : ORIGIN = 0x04800000, LENGTH = 0x40000
}

SECTIONS
{
    eFLASH = 0x4100000;

    sSYSRAM = 0x20038000;
    eSYSRAM = 0x20040000;
    sUCRAM  = 0x20040000;
    eUCRAM  = 0x20058000;

    /* Linkerscript only for contiguous SYS and UCRAM */
    ASSERT(eSYSRAM == sUCRAM, "Non contiguous SYS and UCRAM")

    .bl_user_license   0x4000000 + 0x10 : { KEEP(*(bl_user_license.data)); } > FLASH
    /* Add the jumptable at this fixed location 0x800 */
    jumptable_start_offset = 0x800;
    datajumptable_start_address = 0x4000000 + jumptable_start_offset;
    flashjumptable_start_address = datajumptable_start_address + 0x40;
    .datajumptable   datajumptable_start_address  :  {. = ALIGN(4); KEEP(*(.datajumptable));    } > FLASH
    .flashjumptable  flashjumptable_start_address :  {. = ALIGN(4); KEEP(*(.flashjumptable));   } > FLASH

    /* Add the Matter jumptable at this fixed location */
    m_jumptable_start_offset = 0x1600;
    m_flashjumptable_start_address = 0x4000000 + m_jumptable_start_offset;
    .m_flashjumptable  m_flashjumptable_start_address :  {. = ALIGN(4); KEEP(*(.matterjumptable));   } > FLASH

    .bootloader 0x4001800 : { KEEP(*(bootloader.data)); } > FLASH
    .upgrade_image_user_license   0x4005000 : {
        /* fill with 0x00 to ensure it is erased */
        FILL(0x00);
        _upgrade_image_user_license_start = . ;
        BYTE(0x00);
        .  += 0x1000 - 1;
    } > FLASH
    /* User license is first - only for FLASH programs. */
    .loaded_user_license   0x4006000 : { KEEP(*(.loaded_user_license)); } > FLASH
    .extended_user_license   0x4006000 + 0x80 : { KEEP(*(.extended_user_license)); } > FLASH

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

    /* Start of memory to be retained in sleep */
    _sretain = 0x20038000;

    /* Fixed sections */
    .mw_crc 0x20038000 : { . += 0x4; } > RAM
    .crc    0x20038004 : { . += 0x4; } > RAM
    .ret_hw 0x20038008 : { . += 0x1B4; } > RAM /* 0x20038008+0x1b4 = 0x200381BC = GP_MM_RAM_RETENTION_END */
    .ret_sw 0x200381BC : { . += 0x7C; }  > RAM
    ASSERT(. >= 0x200381c0, "gpmicro areas must fall into retention areas")

    /* Windowed sections */
    /* ram_regmap, events, pbm options and pbm data are taken from buffers in .lower_ram_retain */
    .lower_ram_retain_gpmicro_accessible_mrom (NOLOAD) :  {
        . = ALIGN(4);
        _slower_ram_retain_gpmicro_accessible_m = .;
        . += 0x2fe;
        _elower_ram_retain_gpmicro_accessible_m = .;
    } > RAM
    _llower_ram_retain_gpmicro_accessible_m = _elower_ram_retain_gpmicro_accessible_m - _slower_ram_retain_gpmicro_accessible_m;
    .lower_ram_retain_gpmicro_accessible (NOLOAD) :  {
        . = ALIGN(4);
        *(.lower_ram_retain_gpmicro_accessible) *(.lower_ram_retain_gpmicro_accessible.*);
    } > RAM
    ASSERT (. < 0x20038000+0x4000, ".lower_ram_retain_gpmicro_accessible area outside range")
    .lower_ram_retain (NOLOAD) : { . = ALIGN(4); *(.lower_ram_retain) *(.lower_ram_retain.*); } > RAM
    _elower_ram = . ;

    /* Check regions are allocated in lower ram */
    ASSERT(_elower_ram < eSYSRAM, "SYSRAM ram full")

    ram_m_start_address = 0x20039e00;
    ram_m_max_size = 0x2000;

    ASSERT(. < ram_m_start_address, "SYSRAM is overflowing into the matter-rom RAM area")

    .unused_area : {
        . += (ram_m_start_address - _elower_ram);
    } > RAM

    .rom_m_bss ram_m_start_address :
    {
        . = ALIGN(4);
        _sbss_m = .;
        . += 0x965;
        _ebss_m = .;
    } > RAM
    _lbss_m   = _ebss_m - _sbss_m;

    .rom_m_data :
    {
        . = ALIGN(4);
        _sdata_m = .;
        . += 0x14c;
        _edata_m = .;
    } > RAM AT > ROM
    _sidata_m = 0x4823d81;
    _ldata_m = _edata_m - _sdata_m;

    ASSERT((_lbss_m + _ldata_m) <= ram_m_max_size, "ROM used .bss + .data does not fit reserved area")

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
        KEEP(*(.preinit_array));
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)));
        KEEP(*(.init_array));
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)));
        KEEP(*(.fini_array));
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*));
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    } > RAM AT > FLASH
    .bss :  { . = ALIGN(4); *(.bss)  *(.bss.*) *(COMMON); } > RAM

    /* setting a minimum heap size maximises heap and reduces stack */
    __dyn_heap_start    = ALIGN(4);
    __dyn_heap_end      = ORIGIN(RAM) + LENGTH(RAM) - ALIGN(0x200,4);
    __dyn_heap_size     =  __dyn_heap_end - __dyn_heap_start;
    ASSERT(__dyn_heap_size >= 0x1000, "HEAP too small")
    .heap   (NOLOAD) :    ALIGN(4)        { . = ALIGN(4); . += __dyn_heap_size; } > RAM

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

    .factory_data 0x4004800:
    {
        factory_data_Start = . ;
        KEEP(*(factory_data.data));
        .  = factory_data_Start + 0x800;
        factory_data_End = . ;
    } > FLASH

    .JTOTA eFLASH - 0x61000:
    {
        JTOTA_Start = . ;
        KEEP(*(JTOTA.data));
        .  = JTOTA_Start + 0x1000;
        JTOTA_End = . ;
    } > FLASH

    .OTA eFLASH - 0x60000:
    {
        OTA_Start = . ;
        KEEP(*(OTA.data));
        .  = OTA_Start + 0x5c000;
        OTA_End = . ;
    } > FLASH

    .gpNvm eFLASH - 0x4000:
    {
        gpNvm_Start = . ;
        KEEP(*(gpNvm.data));
        .  = gpNvm_Start + 0x4000;
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
    _sheap  = ADDR(.heap);
    _eheap  = ADDR(.heap)  + ALIGN(SIZEOF(.heap),  4);
    _lheap  = _eheap - _sheap;

    /* stack size is a constant */
    _sstack = _eheap;

    _estack = ORIGIN(RAM) + LENGTH(RAM);
    _lstack = _estack - _sstack;

    /* check minimum stack size is still available */
    min_stack_size = 0x200;
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
