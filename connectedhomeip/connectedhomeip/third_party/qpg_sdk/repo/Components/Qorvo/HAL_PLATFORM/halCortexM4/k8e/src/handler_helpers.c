

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// IVT entries can be stack pointer or function pointer ==> extra mapping needed
typedef void (*intfunc)(void);
#if defined(__GNUC__)
typedef union { intfunc __fun; const void * __ptr; } intvec_elem;
#endif
#if defined(__IAR_SYSTEMS_ICC__)
typedef union { intfunc __fun; void * __ptr; } intvec_elem;
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define HAL_SP_BACKUP_SIZE   192
#define HAL_SP_BACKUP_MARGIN 0x20

#if defined(__GNUC__)
#if !defined(__SEGGER_LINKER)
#define SYMBOL_STACK_END            _estack
#else
#define SYMBOL_STACK_END            __stack_end__
#endif /* __SEGGER_LINKER */
#define IVT_GET_STACK_POINTER()    (&(SYMBOL_STACK_END))
#elif defined(__IAR_SYSTEMS_ICC__)
#define IVT_GET_STACK_POINTER()    (__sfe("CSTACK"))
#endif

#define HAL_SP_STACK_END_ADDRESS   IVT_GET_STACK_POINTER()

#define RAM_MW_RETAINED_SLEEP 0xAABBC000

#ifdef GP_HALCORTEXM4_PRE_MAIN_INIT
extern void GP_HALCORTEXM4_PRE_MAIN_INIT(void);
#endif // GP_HALCORTEXM4_PRE_MAIN_INIT

// Wait times
#define HAL_WAIT_TIME_XTAL_CLOCK_READY_US 10000
#define HAL_WAIT_TIME_TIME_REF_UPDATE_US  1000

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if defined(__IAR_SYSTEMS_ICC__)
#pragma language = extended
#pragma segment = "CSTACK"
#endif

// Backed up application stack during sleep.
static UInt8 hal_sp_backup[HAL_SP_BACKUP_SIZE] LINKER_SECTION(".lower_ram_retain");

/* Backed up state of NVIC during sleep. */
static UInt32 hal_nvic_backup[2] LINKER_SECTION(".lower_ram_retain");
static UInt32 hal_intprio_backup[36 / sizeof(UInt32)] LINKER_SECTION(".lower_ram_retain");
static UInt32 hal_systick_backup[2] LINKER_SECTION(".lower_ram_retain");
static UInt8 hal_shpr_backup[12] LINKER_SECTION(".lower_ram_retain");

// List of custom registers to be saved/restored during sleep.
static const UInt16 hal_SleepModeRetentionList[] = {GP_BSP_SLEEPMODERETENTIONLIST};

// Buffer to save/restore custom registers.
static UInt8 hal_CustomRetentionBuf[sizeof(hal_SleepModeRetentionList) / sizeof(UInt16)] LINKER_SECTION(".lower_ram_retain");

#if defined(GP_KX_HEAP_SIZE) && defined(__GNUC__)
// Heap start and end locations in RAM provided by linker
extern const unsigned long _sheap;
extern const unsigned long _eheap;
#endif //__GNUC__

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/

#ifndef GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT
void _exit(int rc)
{
    for(;;)
    {
    }
}

void abort(void)
{
    _exit(1);
}

char* _sbrk(void* reentr, size_t incr)
{
#if defined(GP_KX_HEAP_SIZE) && defined(__GNUC__)
    static char* heapEnd = 0;
    char* prevHeapEnd;

    if(heapEnd == 0)
    {
        heapEnd = (char*)&_sheap;
    }
    prevHeapEnd = heapEnd;
    if(incr > (UIntPtr)&_eheap - (UIntPtr)heapEnd)
    {
        //Heap depleted
        return ((void*)-1); // error - no more memory
    }

    heapEnd += incr;
    return prevHeapEnd;
#else
    _exit(1);
    return NULL;
#endif
}

#if defined(__GNUC__)
// Stub functions for C++ related lib _init/_fini functions
void __attribute__((weak)) _init(void)
{

}
void __attribute__((weak)) _fini(void)
{

}

void __attribute__((weak)) __cxa_pure_virtual(void)
{
  while (1);
}

// Stub functions when using 'nostdlib' */
void __attribute__((weak)) _write(void)
{
  while (1);
}

void __attribute__((weak)) _close(void)
{
  while (1);
}

void __attribute__((weak)) _fstat(void)
{
  while (1);
}

void __attribute__((weak)) _isatty(void)
{
  while (1);
}

void __attribute__((weak)) _lseek(void)
{
  while (1);
}

void __attribute__((weak)) _read(void)
{
  while (1);
}
#endif //__GNUC__
#endif //ifndef GP_HALCORTEXM4_DIVERSITY_CUSTOM_IVT

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/* Restore NVIC state during wake-up. */
static INLINE void hal_RestoreNvic(void)
{
    // Restore Interrupt Set Enable registers.
    NVIC->ISER[0] = hal_nvic_backup[0];
    NVIC->ISER[1] = hal_nvic_backup[1];

    // Restore Interrupt Priority registers.
    MEMCPY((void*)&NVIC->IP, hal_intprio_backup, 36);

    MEMCPY((void*)SCB->SHP, hal_shpr_backup, 12);

    SysTick->LOAD = hal_systick_backup[0];
    SysTick->CTRL = hal_systick_backup[1];
}

/* Back up NVIC state before sleep. */
static INLINE void hal_BackupNvic(void)
{
    // Backup Interrupt Set Enable registers.
    hal_nvic_backup[0] = NVIC->ISER[0];
    hal_nvic_backup[1] = NVIC->ISER[1];

    // Backup Interrupt Priority registers.
    MEMCPY(hal_intprio_backup, (void*)&NVIC->IP, 36);

    /* Backup system handler priority registers */
    MEMCPY(hal_shpr_backup, (void*)SCB->SHP, 12);

    hal_systick_backup[0] = SysTick->LOAD;
    hal_systick_backup[1] = SysTick->CTRL;
}

/* Back up hardware registers listed in BSP retention list. */
static INLINE void hal_CustomBackup(void)
{
    UIntLoop i;
    for(i = 0; hal_SleepModeRetentionList[i] != 0; i++)
    {
        GP_ASSERT_DEV_EXT(i < sizeof(hal_CustomRetentionBuf));
        hal_CustomRetentionBuf[i] = GP_WB_READ_U8(GP_MM_WISHB_ADDR_FROM_COMPRESSED(hal_SleepModeRetentionList[i]));
    }
}

/* Restore hardware registers listed in BSP retention list. */
static INLINE void hal_CustomRestore(void)
{
    UIntLoop i;
    for(i = 0; hal_SleepModeRetentionList[i] != 0; i++)
    {
        GP_ASSERT_DEV_EXT(i < sizeof(hal_CustomRetentionBuf));
        GP_WB_WRITE_U8(GP_MM_WISHB_ADDR_FROM_COMPRESSED(hal_SleepModeRetentionList[i]), hal_CustomRetentionBuf[i]);
    }
}
