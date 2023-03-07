/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-K04
 */

/*
 * ======== GENERATED SECTIONS ========
 *
 *     PROLOGUE
 *     INCLUDES
 *
 *     INTERNAL DEFINITIONS
 *     MODULE-WIDE CONFIGS
 *     FUNCTION DECLARATIONS
 *     SYSTEM FUNCTIONS
 *
 *     EPILOGUE
 *     STATE STRUCTURES
 *     PREFIX ALIASES
 */


/*
 * ======== PROLOGUE ========
 */

#ifndef ti_sysbios_family_arm_MPU__include
#define ti_sysbios_family_arm_MPU__include

#ifndef __nested__
#define __nested__
#define ti_sysbios_family_arm_MPU__top__
#endif

#ifndef __extern
#ifdef __cplusplus
#define __extern extern "C"
#else
#define __extern extern
#endif
#endif

#define ti_sysbios_family_arm_MPU___VERS 200


/*
 * ======== INCLUDES ========
 */

#include <xdc/std.h>

#include <xdc/runtime/xdc.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/family/arm/package/package.defs.h>

#include <xdc/runtime/IModule.h>
#include <xdc/runtime/Assert.h>


/*
 * ======== AUXILIARY DEFINITIONS ========
 */

/* DeviceRegs */
struct ti_sysbios_family_arm_MPU_DeviceRegs {
    xdc_UInt32 TYPE;
    xdc_UInt32 CTRL;
    xdc_UInt32 RNR;
    xdc_UInt32 RBAR;
    xdc_UInt32 RASR;
    xdc_UInt32 RBAR_A1;
    xdc_UInt32 RASR_A1;
    xdc_UInt32 RBAR_A2;
    xdc_UInt32 RASR_A2;
    xdc_UInt32 RBAR_A3;
    xdc_UInt32 RASR_A3;
};

/* deviceRegs */
__extern volatile ti_sysbios_family_arm_MPU_DeviceRegs ti_sysbios_family_arm_MPU_deviceRegs;

/* RegionSize */
enum ti_sysbios_family_arm_MPU_RegionSize {
    ti_sysbios_family_arm_MPU_RegionSize_32 = 0x8,
    ti_sysbios_family_arm_MPU_RegionSize_64 = 0xA,
    ti_sysbios_family_arm_MPU_RegionSize_128 = 0xC,
    ti_sysbios_family_arm_MPU_RegionSize_256 = 0xE,
    ti_sysbios_family_arm_MPU_RegionSize_512 = 0x10,
    ti_sysbios_family_arm_MPU_RegionSize_1K = 0x12,
    ti_sysbios_family_arm_MPU_RegionSize_2K = 0x14,
    ti_sysbios_family_arm_MPU_RegionSize_4K = 0x16,
    ti_sysbios_family_arm_MPU_RegionSize_8K = 0x18,
    ti_sysbios_family_arm_MPU_RegionSize_16K = 0x1A,
    ti_sysbios_family_arm_MPU_RegionSize_32K = 0x1C,
    ti_sysbios_family_arm_MPU_RegionSize_64K = 0x1E,
    ti_sysbios_family_arm_MPU_RegionSize_128K = 0x20,
    ti_sysbios_family_arm_MPU_RegionSize_256K = 0x22,
    ti_sysbios_family_arm_MPU_RegionSize_512K = 0x24,
    ti_sysbios_family_arm_MPU_RegionSize_1M = 0x26,
    ti_sysbios_family_arm_MPU_RegionSize_2M = 0x28,
    ti_sysbios_family_arm_MPU_RegionSize_4M = 0x2A,
    ti_sysbios_family_arm_MPU_RegionSize_8M = 0x2C,
    ti_sysbios_family_arm_MPU_RegionSize_16M = 0x2E,
    ti_sysbios_family_arm_MPU_RegionSize_32M = 0x30,
    ti_sysbios_family_arm_MPU_RegionSize_64M = 0x32,
    ti_sysbios_family_arm_MPU_RegionSize_128M = 0x34,
    ti_sysbios_family_arm_MPU_RegionSize_256M = 0x36,
    ti_sysbios_family_arm_MPU_RegionSize_512M = 0x38,
    ti_sysbios_family_arm_MPU_RegionSize_1G = 0x3A,
    ti_sysbios_family_arm_MPU_RegionSize_2G = 0x3C,
    ti_sysbios_family_arm_MPU_RegionSize_4G = 0x3E
};
typedef enum ti_sysbios_family_arm_MPU_RegionSize ti_sysbios_family_arm_MPU_RegionSize;

/* RegionAttrs */
struct ti_sysbios_family_arm_MPU_RegionAttrs {
    xdc_Bool enable;
    xdc_Bool bufferable;
    xdc_Bool cacheable;
    xdc_Bool shareable;
    xdc_Bool noExecute;
    xdc_UInt8 accPerm;
    xdc_UInt8 tex;
    xdc_UInt8 subregionDisableMask;
};


/*
 * ======== INTERNAL DEFINITIONS ========
 */

/* RegionEntry */
struct ti_sysbios_family_arm_MPU_RegionEntry {
    xdc_UInt32 baseAddress;
    xdc_UInt32 sizeAndEnable;
    xdc_UInt32 regionAttrs;
};

/* Module_State */
typedef ti_sysbios_family_arm_MPU_RegionEntry __T1_ti_sysbios_family_arm_MPU_Module_State__regionEntry;
typedef ti_sysbios_family_arm_MPU_RegionEntry *ARRAY1_ti_sysbios_family_arm_MPU_Module_State__regionEntry;
typedef const ti_sysbios_family_arm_MPU_RegionEntry *CARRAY1_ti_sysbios_family_arm_MPU_Module_State__regionEntry;
typedef ARRAY1_ti_sysbios_family_arm_MPU_Module_State__regionEntry __TA_ti_sysbios_family_arm_MPU_Module_State__regionEntry;


/*
 * ======== MODULE-WIDE CONFIGS ========
 */

/* Module__diagsEnabled */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_MPU_Module__diagsEnabled;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__diagsEnabled ti_sysbios_family_arm_MPU_Module__diagsEnabled__C;
#ifdef ti_sysbios_family_arm_MPU_Module__diagsEnabled__CR
#define ti_sysbios_family_arm_MPU_Module__diagsEnabled__C (*((CT__ti_sysbios_family_arm_MPU_Module__diagsEnabled*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__diagsEnabled__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__diagsEnabled (ti_sysbios_family_arm_MPU_Module__diagsEnabled__C)
#endif

/* Module__diagsIncluded */
typedef xdc_Bits32 CT__ti_sysbios_family_arm_MPU_Module__diagsIncluded;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__diagsIncluded ti_sysbios_family_arm_MPU_Module__diagsIncluded__C;
#ifdef ti_sysbios_family_arm_MPU_Module__diagsIncluded__CR
#define ti_sysbios_family_arm_MPU_Module__diagsIncluded__C (*((CT__ti_sysbios_family_arm_MPU_Module__diagsIncluded*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__diagsIncluded__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__diagsIncluded (ti_sysbios_family_arm_MPU_Module__diagsIncluded__C)
#endif

/* Module__diagsMask */
typedef xdc_Bits16 *CT__ti_sysbios_family_arm_MPU_Module__diagsMask;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__diagsMask ti_sysbios_family_arm_MPU_Module__diagsMask__C;
#ifdef ti_sysbios_family_arm_MPU_Module__diagsMask__CR
#define ti_sysbios_family_arm_MPU_Module__diagsMask__C (*((CT__ti_sysbios_family_arm_MPU_Module__diagsMask*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__diagsMask__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__diagsMask (ti_sysbios_family_arm_MPU_Module__diagsMask__C)
#endif

/* Module__gateObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_MPU_Module__gateObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__gateObj ti_sysbios_family_arm_MPU_Module__gateObj__C;
#ifdef ti_sysbios_family_arm_MPU_Module__gateObj__CR
#define ti_sysbios_family_arm_MPU_Module__gateObj__C (*((CT__ti_sysbios_family_arm_MPU_Module__gateObj*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__gateObj__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__gateObj (ti_sysbios_family_arm_MPU_Module__gateObj__C)
#endif

/* Module__gatePrms */
typedef xdc_Ptr CT__ti_sysbios_family_arm_MPU_Module__gatePrms;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__gatePrms ti_sysbios_family_arm_MPU_Module__gatePrms__C;
#ifdef ti_sysbios_family_arm_MPU_Module__gatePrms__CR
#define ti_sysbios_family_arm_MPU_Module__gatePrms__C (*((CT__ti_sysbios_family_arm_MPU_Module__gatePrms*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__gatePrms__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__gatePrms (ti_sysbios_family_arm_MPU_Module__gatePrms__C)
#endif

/* Module__id */
typedef xdc_runtime_Types_ModuleId CT__ti_sysbios_family_arm_MPU_Module__id;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__id ti_sysbios_family_arm_MPU_Module__id__C;
#ifdef ti_sysbios_family_arm_MPU_Module__id__CR
#define ti_sysbios_family_arm_MPU_Module__id__C (*((CT__ti_sysbios_family_arm_MPU_Module__id*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__id__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__id (ti_sysbios_family_arm_MPU_Module__id__C)
#endif

/* Module__loggerDefined */
typedef xdc_Bool CT__ti_sysbios_family_arm_MPU_Module__loggerDefined;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerDefined ti_sysbios_family_arm_MPU_Module__loggerDefined__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerDefined__CR
#define ti_sysbios_family_arm_MPU_Module__loggerDefined__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerDefined*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerDefined__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerDefined (ti_sysbios_family_arm_MPU_Module__loggerDefined__C)
#endif

/* Module__loggerObj */
typedef xdc_Ptr CT__ti_sysbios_family_arm_MPU_Module__loggerObj;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerObj ti_sysbios_family_arm_MPU_Module__loggerObj__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerObj__CR
#define ti_sysbios_family_arm_MPU_Module__loggerObj__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerObj*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerObj__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerObj (ti_sysbios_family_arm_MPU_Module__loggerObj__C)
#endif

/* Module__loggerFxn0 */
typedef xdc_runtime_Types_LoggerFxn0 CT__ti_sysbios_family_arm_MPU_Module__loggerFxn0;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerFxn0 ti_sysbios_family_arm_MPU_Module__loggerFxn0__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerFxn0__CR
#define ti_sysbios_family_arm_MPU_Module__loggerFxn0__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerFxn0*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerFxn0__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerFxn0 (ti_sysbios_family_arm_MPU_Module__loggerFxn0__C)
#endif

/* Module__loggerFxn1 */
typedef xdc_runtime_Types_LoggerFxn1 CT__ti_sysbios_family_arm_MPU_Module__loggerFxn1;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerFxn1 ti_sysbios_family_arm_MPU_Module__loggerFxn1__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerFxn1__CR
#define ti_sysbios_family_arm_MPU_Module__loggerFxn1__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerFxn1*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerFxn1__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerFxn1 (ti_sysbios_family_arm_MPU_Module__loggerFxn1__C)
#endif

/* Module__loggerFxn2 */
typedef xdc_runtime_Types_LoggerFxn2 CT__ti_sysbios_family_arm_MPU_Module__loggerFxn2;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerFxn2 ti_sysbios_family_arm_MPU_Module__loggerFxn2__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerFxn2__CR
#define ti_sysbios_family_arm_MPU_Module__loggerFxn2__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerFxn2*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerFxn2__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerFxn2 (ti_sysbios_family_arm_MPU_Module__loggerFxn2__C)
#endif

/* Module__loggerFxn4 */
typedef xdc_runtime_Types_LoggerFxn4 CT__ti_sysbios_family_arm_MPU_Module__loggerFxn4;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerFxn4 ti_sysbios_family_arm_MPU_Module__loggerFxn4__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerFxn4__CR
#define ti_sysbios_family_arm_MPU_Module__loggerFxn4__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerFxn4*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerFxn4__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerFxn4 (ti_sysbios_family_arm_MPU_Module__loggerFxn4__C)
#endif

/* Module__loggerFxn8 */
typedef xdc_runtime_Types_LoggerFxn8 CT__ti_sysbios_family_arm_MPU_Module__loggerFxn8;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Module__loggerFxn8 ti_sysbios_family_arm_MPU_Module__loggerFxn8__C;
#ifdef ti_sysbios_family_arm_MPU_Module__loggerFxn8__CR
#define ti_sysbios_family_arm_MPU_Module__loggerFxn8__C (*((CT__ti_sysbios_family_arm_MPU_Module__loggerFxn8*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Module__loggerFxn8__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Module__loggerFxn8 (ti_sysbios_family_arm_MPU_Module__loggerFxn8__C)
#endif

/* Object__count */
typedef xdc_Int CT__ti_sysbios_family_arm_MPU_Object__count;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Object__count ti_sysbios_family_arm_MPU_Object__count__C;
#ifdef ti_sysbios_family_arm_MPU_Object__count__CR
#define ti_sysbios_family_arm_MPU_Object__count__C (*((CT__ti_sysbios_family_arm_MPU_Object__count*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Object__count__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Object__count (ti_sysbios_family_arm_MPU_Object__count__C)
#endif

/* Object__heap */
typedef xdc_runtime_IHeap_Handle CT__ti_sysbios_family_arm_MPU_Object__heap;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Object__heap ti_sysbios_family_arm_MPU_Object__heap__C;
#ifdef ti_sysbios_family_arm_MPU_Object__heap__CR
#define ti_sysbios_family_arm_MPU_Object__heap__C (*((CT__ti_sysbios_family_arm_MPU_Object__heap*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Object__heap__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Object__heap (ti_sysbios_family_arm_MPU_Object__heap__C)
#endif

/* Object__sizeof */
typedef xdc_SizeT CT__ti_sysbios_family_arm_MPU_Object__sizeof;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Object__sizeof ti_sysbios_family_arm_MPU_Object__sizeof__C;
#ifdef ti_sysbios_family_arm_MPU_Object__sizeof__CR
#define ti_sysbios_family_arm_MPU_Object__sizeof__C (*((CT__ti_sysbios_family_arm_MPU_Object__sizeof*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Object__sizeof__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Object__sizeof (ti_sysbios_family_arm_MPU_Object__sizeof__C)
#endif

/* Object__table */
typedef xdc_Ptr CT__ti_sysbios_family_arm_MPU_Object__table;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_Object__table ti_sysbios_family_arm_MPU_Object__table__C;
#ifdef ti_sysbios_family_arm_MPU_Object__table__CR
#define ti_sysbios_family_arm_MPU_Object__table__C (*((CT__ti_sysbios_family_arm_MPU_Object__table*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_Object__table__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_Object__table (ti_sysbios_family_arm_MPU_Object__table__C)
#endif

/* defaultAttrs */
typedef ti_sysbios_family_arm_MPU_RegionAttrs CT__ti_sysbios_family_arm_MPU_defaultAttrs;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_defaultAttrs ti_sysbios_family_arm_MPU_defaultAttrs__C;
#ifdef ti_sysbios_family_arm_MPU_defaultAttrs__CR
#define ti_sysbios_family_arm_MPU_defaultAttrs (*((CT__ti_sysbios_family_arm_MPU_defaultAttrs*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_defaultAttrs__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_defaultAttrs (ti_sysbios_family_arm_MPU_defaultAttrs__C)
#endif

/* A_nullPointer */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_MPU_A_nullPointer;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_A_nullPointer ti_sysbios_family_arm_MPU_A_nullPointer__C;
#ifdef ti_sysbios_family_arm_MPU_A_nullPointer__CR
#define ti_sysbios_family_arm_MPU_A_nullPointer (*((CT__ti_sysbios_family_arm_MPU_A_nullPointer*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_A_nullPointer__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_A_nullPointer (ti_sysbios_family_arm_MPU_A_nullPointer__C)
#endif

/* A_invalidRegionId */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_MPU_A_invalidRegionId;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_A_invalidRegionId ti_sysbios_family_arm_MPU_A_invalidRegionId__C;
#ifdef ti_sysbios_family_arm_MPU_A_invalidRegionId__CR
#define ti_sysbios_family_arm_MPU_A_invalidRegionId (*((CT__ti_sysbios_family_arm_MPU_A_invalidRegionId*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_A_invalidRegionId__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_A_invalidRegionId (ti_sysbios_family_arm_MPU_A_invalidRegionId__C)
#endif

/* A_unalignedBaseAddr */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_MPU_A_unalignedBaseAddr;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_A_unalignedBaseAddr ti_sysbios_family_arm_MPU_A_unalignedBaseAddr__C;
#ifdef ti_sysbios_family_arm_MPU_A_unalignedBaseAddr__CR
#define ti_sysbios_family_arm_MPU_A_unalignedBaseAddr (*((CT__ti_sysbios_family_arm_MPU_A_unalignedBaseAddr*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_A_unalignedBaseAddr__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_A_unalignedBaseAddr (ti_sysbios_family_arm_MPU_A_unalignedBaseAddr__C)
#endif

/* A_reservedAttrs */
typedef xdc_runtime_Assert_Id CT__ti_sysbios_family_arm_MPU_A_reservedAttrs;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_A_reservedAttrs ti_sysbios_family_arm_MPU_A_reservedAttrs__C;
#ifdef ti_sysbios_family_arm_MPU_A_reservedAttrs__CR
#define ti_sysbios_family_arm_MPU_A_reservedAttrs (*((CT__ti_sysbios_family_arm_MPU_A_reservedAttrs*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_A_reservedAttrs__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_A_reservedAttrs (ti_sysbios_family_arm_MPU_A_reservedAttrs__C)
#endif

/* enableMPU */
typedef xdc_Bool CT__ti_sysbios_family_arm_MPU_enableMPU;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_enableMPU ti_sysbios_family_arm_MPU_enableMPU__C;
#ifdef ti_sysbios_family_arm_MPU_enableMPU__CR
#define ti_sysbios_family_arm_MPU_enableMPU (*((CT__ti_sysbios_family_arm_MPU_enableMPU*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_enableMPU__C_offset)))
#else
#ifdef ti_sysbios_family_arm_MPU_enableMPU__D
#define ti_sysbios_family_arm_MPU_enableMPU (ti_sysbios_family_arm_MPU_enableMPU__D)
#else
#define ti_sysbios_family_arm_MPU_enableMPU (ti_sysbios_family_arm_MPU_enableMPU__C)
#endif
#endif

/* enableBackgroundRegion */
typedef xdc_Bool CT__ti_sysbios_family_arm_MPU_enableBackgroundRegion;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_enableBackgroundRegion ti_sysbios_family_arm_MPU_enableBackgroundRegion__C;
#ifdef ti_sysbios_family_arm_MPU_enableBackgroundRegion__CR
#define ti_sysbios_family_arm_MPU_enableBackgroundRegion (*((CT__ti_sysbios_family_arm_MPU_enableBackgroundRegion*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_enableBackgroundRegion__C_offset)))
#else
#ifdef ti_sysbios_family_arm_MPU_enableBackgroundRegion__D
#define ti_sysbios_family_arm_MPU_enableBackgroundRegion (ti_sysbios_family_arm_MPU_enableBackgroundRegion__D)
#else
#define ti_sysbios_family_arm_MPU_enableBackgroundRegion (ti_sysbios_family_arm_MPU_enableBackgroundRegion__C)
#endif
#endif

/* numRegions */
typedef xdc_UInt8 CT__ti_sysbios_family_arm_MPU_numRegions;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_numRegions ti_sysbios_family_arm_MPU_numRegions__C;
#ifdef ti_sysbios_family_arm_MPU_numRegions__CR
#define ti_sysbios_family_arm_MPU_numRegions (*((CT__ti_sysbios_family_arm_MPU_numRegions*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_numRegions__C_offset)))
#else
#ifdef ti_sysbios_family_arm_MPU_numRegions__D
#define ti_sysbios_family_arm_MPU_numRegions (ti_sysbios_family_arm_MPU_numRegions__D)
#else
#define ti_sysbios_family_arm_MPU_numRegions (ti_sysbios_family_arm_MPU_numRegions__C)
#endif
#endif

/* regionEntry */
typedef ti_sysbios_family_arm_MPU_RegionEntry __T1_ti_sysbios_family_arm_MPU_regionEntry;
typedef ti_sysbios_family_arm_MPU_RegionEntry *ARRAY1_ti_sysbios_family_arm_MPU_regionEntry;
typedef const ti_sysbios_family_arm_MPU_RegionEntry *CARRAY1_ti_sysbios_family_arm_MPU_regionEntry;
typedef CARRAY1_ti_sysbios_family_arm_MPU_regionEntry __TA_ti_sysbios_family_arm_MPU_regionEntry;
typedef CARRAY1_ti_sysbios_family_arm_MPU_regionEntry CT__ti_sysbios_family_arm_MPU_regionEntry;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_regionEntry ti_sysbios_family_arm_MPU_regionEntry__C;
#ifdef ti_sysbios_family_arm_MPU_regionEntry__CR
#define ti_sysbios_family_arm_MPU_regionEntry (*((CT__ti_sysbios_family_arm_MPU_regionEntry*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_regionEntry__C_offset)))
#else
#define ti_sysbios_family_arm_MPU_regionEntry (ti_sysbios_family_arm_MPU_regionEntry__C)
#endif

/* isMemoryMapped */
typedef xdc_Bool CT__ti_sysbios_family_arm_MPU_isMemoryMapped;
__extern __FAR__ const CT__ti_sysbios_family_arm_MPU_isMemoryMapped ti_sysbios_family_arm_MPU_isMemoryMapped__C;
#ifdef ti_sysbios_family_arm_MPU_isMemoryMapped__CR
#define ti_sysbios_family_arm_MPU_isMemoryMapped (*((CT__ti_sysbios_family_arm_MPU_isMemoryMapped*)(xdcRomConstPtr + ti_sysbios_family_arm_MPU_isMemoryMapped__C_offset)))
#else
#ifdef ti_sysbios_family_arm_MPU_isMemoryMapped__D
#define ti_sysbios_family_arm_MPU_isMemoryMapped (ti_sysbios_family_arm_MPU_isMemoryMapped__D)
#else
#define ti_sysbios_family_arm_MPU_isMemoryMapped (ti_sysbios_family_arm_MPU_isMemoryMapped__C)
#endif
#endif


/*
 * ======== FUNCTION DECLARATIONS ========
 */

/* Module_startup */
#define ti_sysbios_family_arm_MPU_Module_startup( state ) (-1)

/* Module__startupDone__S */
xdc__CODESECT(ti_sysbios_family_arm_MPU_Module__startupDone__S, "ti_sysbios_family_arm_MPU_Module__startupDone__S")
__extern xdc_Bool ti_sysbios_family_arm_MPU_Module__startupDone__S( void);

/* disable__E */
#define ti_sysbios_family_arm_MPU_disable ti_sysbios_family_arm_MPU_disable__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_disable__E, "ti_sysbios_family_arm_MPU_disable")
__extern xdc_Void ti_sysbios_family_arm_MPU_disable__E( void);

/* enable__E */
#define ti_sysbios_family_arm_MPU_enable ti_sysbios_family_arm_MPU_enable__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_enable__E, "ti_sysbios_family_arm_MPU_enable")
__extern xdc_Void ti_sysbios_family_arm_MPU_enable__E( void);

/* disableBR__E */
#define ti_sysbios_family_arm_MPU_disableBR ti_sysbios_family_arm_MPU_disableBR__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_disableBR__E, "ti_sysbios_family_arm_MPU_disableBR")
__extern xdc_Void ti_sysbios_family_arm_MPU_disableBR__E( void);

/* enableBR__E */
#define ti_sysbios_family_arm_MPU_enableBR ti_sysbios_family_arm_MPU_enableBR__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_enableBR__E, "ti_sysbios_family_arm_MPU_enableBR")
__extern xdc_Void ti_sysbios_family_arm_MPU_enableBR__E( void);

/* initRegionAttrs__E */
#define ti_sysbios_family_arm_MPU_initRegionAttrs ti_sysbios_family_arm_MPU_initRegionAttrs__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_initRegionAttrs__E, "ti_sysbios_family_arm_MPU_initRegionAttrs")
__extern xdc_Void ti_sysbios_family_arm_MPU_initRegionAttrs__E( ti_sysbios_family_arm_MPU_RegionAttrs *regionAttrs);

/* isEnabled__E */
#define ti_sysbios_family_arm_MPU_isEnabled ti_sysbios_family_arm_MPU_isEnabled__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_isEnabled__E, "ti_sysbios_family_arm_MPU_isEnabled")
__extern xdc_Bool ti_sysbios_family_arm_MPU_isEnabled__E( void);

/* setRegion__E */
#define ti_sysbios_family_arm_MPU_setRegion ti_sysbios_family_arm_MPU_setRegion__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_setRegion__E, "ti_sysbios_family_arm_MPU_setRegion")
__extern xdc_Void ti_sysbios_family_arm_MPU_setRegion__E( xdc_UInt8 regionId, xdc_Ptr regionBaseAddr, ti_sysbios_family_arm_MPU_RegionSize regionSize, ti_sysbios_family_arm_MPU_RegionAttrs *attrs);

/* setRegionRaw__E */
#define ti_sysbios_family_arm_MPU_setRegionRaw ti_sysbios_family_arm_MPU_setRegionRaw__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_setRegionRaw__E, "ti_sysbios_family_arm_MPU_setRegionRaw")
__extern xdc_Void ti_sysbios_family_arm_MPU_setRegionRaw__E( xdc_UInt32 rbar, xdc_UInt32 rasr);

/* startup__E */
#define ti_sysbios_family_arm_MPU_startup ti_sysbios_family_arm_MPU_startup__E
xdc__CODESECT(ti_sysbios_family_arm_MPU_startup__E, "ti_sysbios_family_arm_MPU_startup")
__extern xdc_Void ti_sysbios_family_arm_MPU_startup__E( void);

/* disableAsm__I */
#define ti_sysbios_family_arm_MPU_disableAsm ti_sysbios_family_arm_MPU_disableAsm__I
xdc__CODESECT(ti_sysbios_family_arm_MPU_disableAsm__I, "ti_sysbios_family_arm_MPU_disableAsm")
__extern xdc_Void ti_sysbios_family_arm_MPU_disableAsm__I( void);

/* enableAsm__I */
#define ti_sysbios_family_arm_MPU_enableAsm ti_sysbios_family_arm_MPU_enableAsm__I
xdc__CODESECT(ti_sysbios_family_arm_MPU_enableAsm__I, "ti_sysbios_family_arm_MPU_enableAsm")
__extern xdc_Void ti_sysbios_family_arm_MPU_enableAsm__I( void);

/* disableBRAsm__I */
#define ti_sysbios_family_arm_MPU_disableBRAsm ti_sysbios_family_arm_MPU_disableBRAsm__I
xdc__CODESECT(ti_sysbios_family_arm_MPU_disableBRAsm__I, "ti_sysbios_family_arm_MPU_disableBRAsm")
__extern xdc_Void ti_sysbios_family_arm_MPU_disableBRAsm__I( void);

/* enableBRAsm__I */
#define ti_sysbios_family_arm_MPU_enableBRAsm ti_sysbios_family_arm_MPU_enableBRAsm__I
xdc__CODESECT(ti_sysbios_family_arm_MPU_enableBRAsm__I, "ti_sysbios_family_arm_MPU_enableBRAsm")
__extern xdc_Void ti_sysbios_family_arm_MPU_enableBRAsm__I( void);

/* isEnabledAsm__I */
#define ti_sysbios_family_arm_MPU_isEnabledAsm ti_sysbios_family_arm_MPU_isEnabledAsm__I
xdc__CODESECT(ti_sysbios_family_arm_MPU_isEnabledAsm__I, "ti_sysbios_family_arm_MPU_isEnabledAsm")
__extern xdc_Bool ti_sysbios_family_arm_MPU_isEnabledAsm__I( void);

/* setRegionAsm__I */
#define ti_sysbios_family_arm_MPU_setRegionAsm ti_sysbios_family_arm_MPU_setRegionAsm__I
xdc__CODESECT(ti_sysbios_family_arm_MPU_setRegionAsm__I, "ti_sysbios_family_arm_MPU_setRegionAsm")
__extern xdc_Void ti_sysbios_family_arm_MPU_setRegionAsm__I( xdc_UInt8 regionId, xdc_UInt32 regionBaseAddr, xdc_UInt32 regionSize, xdc_UInt32 regionAttrs);


/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Module_startupDone */
#define ti_sysbios_family_arm_MPU_Module_startupDone() ti_sysbios_family_arm_MPU_Module__startupDone__S()

/* Object_heap */
#define ti_sysbios_family_arm_MPU_Object_heap() ti_sysbios_family_arm_MPU_Object__heap__C

/* Module_heap */
#define ti_sysbios_family_arm_MPU_Module_heap() ti_sysbios_family_arm_MPU_Object__heap__C

/* Module_id */
static inline CT__ti_sysbios_family_arm_MPU_Module__id ti_sysbios_family_arm_MPU_Module_id(void);
static inline CT__ti_sysbios_family_arm_MPU_Module__id ti_sysbios_family_arm_MPU_Module_id( void ) 
{
    return ti_sysbios_family_arm_MPU_Module__id__C;
}

/* Module_hasMask */
static inline xdc_Bool ti_sysbios_family_arm_MPU_Module_hasMask(void);
static inline xdc_Bool ti_sysbios_family_arm_MPU_Module_hasMask(void)
{
    return (xdc_Bool)(ti_sysbios_family_arm_MPU_Module__diagsMask__C != (CT__ti_sysbios_family_arm_MPU_Module__diagsMask)NULL);
}

/* Module_getMask */
static inline xdc_Bits16 ti_sysbios_family_arm_MPU_Module_getMask(void);
static inline xdc_Bits16 ti_sysbios_family_arm_MPU_Module_getMask(void)
{
    return (ti_sysbios_family_arm_MPU_Module__diagsMask__C != (CT__ti_sysbios_family_arm_MPU_Module__diagsMask)NULL) ? *ti_sysbios_family_arm_MPU_Module__diagsMask__C : (xdc_Bits16)0;
}

/* Module_setMask */
static inline xdc_Void ti_sysbios_family_arm_MPU_Module_setMask(xdc_Bits16 mask);
static inline xdc_Void ti_sysbios_family_arm_MPU_Module_setMask(xdc_Bits16 mask)
{
    if (ti_sysbios_family_arm_MPU_Module__diagsMask__C != (CT__ti_sysbios_family_arm_MPU_Module__diagsMask)NULL) {
        *ti_sysbios_family_arm_MPU_Module__diagsMask__C = mask;
    }
}


/*
 * ======== EPILOGUE ========
 */

#ifdef ti_sysbios_family_arm_MPU__top__
#undef __nested__
#endif

#endif /* ti_sysbios_family_arm_MPU__include */


/*
 * ======== STATE STRUCTURES ========
 */

#if defined(__config__) || (!defined(__nested__) && defined(ti_sysbios_family_arm_MPU__internalaccess))

#ifndef ti_sysbios_family_arm_MPU__include_state
#define ti_sysbios_family_arm_MPU__include_state

/* Module_State */
struct ti_sysbios_family_arm_MPU_Module_State {
    __TA_ti_sysbios_family_arm_MPU_Module_State__regionEntry regionEntry;
};

/* Module__state__V */
#ifndef ti_sysbios_family_arm_MPU_Module__state__VR
extern struct ti_sysbios_family_arm_MPU_Module_State__ ti_sysbios_family_arm_MPU_Module__state__V;
#else
#define ti_sysbios_family_arm_MPU_Module__state__V (*((struct ti_sysbios_family_arm_MPU_Module_State__*)(xdcRomStatePtr + ti_sysbios_family_arm_MPU_Module__state__V_offset)))
#endif

#endif /* ti_sysbios_family_arm_MPU__include_state */

#endif


/*
 * ======== PREFIX ALIASES ========
 */

#if !defined(__nested__) && !defined(ti_sysbios_family_arm_MPU__nolocalnames)

#ifndef ti_sysbios_family_arm_MPU__localnames__done
#define ti_sysbios_family_arm_MPU__localnames__done

/* module prefix */
#define MPU_DeviceRegs ti_sysbios_family_arm_MPU_DeviceRegs
#define MPU_deviceRegs ti_sysbios_family_arm_MPU_deviceRegs
#define MPU_RegionSize ti_sysbios_family_arm_MPU_RegionSize
#define MPU_RegionAttrs ti_sysbios_family_arm_MPU_RegionAttrs
#define MPU_RegionEntry ti_sysbios_family_arm_MPU_RegionEntry
#define MPU_Module_State ti_sysbios_family_arm_MPU_Module_State
#define MPU_RegionSize_32 ti_sysbios_family_arm_MPU_RegionSize_32
#define MPU_RegionSize_64 ti_sysbios_family_arm_MPU_RegionSize_64
#define MPU_RegionSize_128 ti_sysbios_family_arm_MPU_RegionSize_128
#define MPU_RegionSize_256 ti_sysbios_family_arm_MPU_RegionSize_256
#define MPU_RegionSize_512 ti_sysbios_family_arm_MPU_RegionSize_512
#define MPU_RegionSize_1K ti_sysbios_family_arm_MPU_RegionSize_1K
#define MPU_RegionSize_2K ti_sysbios_family_arm_MPU_RegionSize_2K
#define MPU_RegionSize_4K ti_sysbios_family_arm_MPU_RegionSize_4K
#define MPU_RegionSize_8K ti_sysbios_family_arm_MPU_RegionSize_8K
#define MPU_RegionSize_16K ti_sysbios_family_arm_MPU_RegionSize_16K
#define MPU_RegionSize_32K ti_sysbios_family_arm_MPU_RegionSize_32K
#define MPU_RegionSize_64K ti_sysbios_family_arm_MPU_RegionSize_64K
#define MPU_RegionSize_128K ti_sysbios_family_arm_MPU_RegionSize_128K
#define MPU_RegionSize_256K ti_sysbios_family_arm_MPU_RegionSize_256K
#define MPU_RegionSize_512K ti_sysbios_family_arm_MPU_RegionSize_512K
#define MPU_RegionSize_1M ti_sysbios_family_arm_MPU_RegionSize_1M
#define MPU_RegionSize_2M ti_sysbios_family_arm_MPU_RegionSize_2M
#define MPU_RegionSize_4M ti_sysbios_family_arm_MPU_RegionSize_4M
#define MPU_RegionSize_8M ti_sysbios_family_arm_MPU_RegionSize_8M
#define MPU_RegionSize_16M ti_sysbios_family_arm_MPU_RegionSize_16M
#define MPU_RegionSize_32M ti_sysbios_family_arm_MPU_RegionSize_32M
#define MPU_RegionSize_64M ti_sysbios_family_arm_MPU_RegionSize_64M
#define MPU_RegionSize_128M ti_sysbios_family_arm_MPU_RegionSize_128M
#define MPU_RegionSize_256M ti_sysbios_family_arm_MPU_RegionSize_256M
#define MPU_RegionSize_512M ti_sysbios_family_arm_MPU_RegionSize_512M
#define MPU_RegionSize_1G ti_sysbios_family_arm_MPU_RegionSize_1G
#define MPU_RegionSize_2G ti_sysbios_family_arm_MPU_RegionSize_2G
#define MPU_RegionSize_4G ti_sysbios_family_arm_MPU_RegionSize_4G
#define MPU_defaultAttrs ti_sysbios_family_arm_MPU_defaultAttrs
#define MPU_A_nullPointer ti_sysbios_family_arm_MPU_A_nullPointer
#define MPU_A_invalidRegionId ti_sysbios_family_arm_MPU_A_invalidRegionId
#define MPU_A_unalignedBaseAddr ti_sysbios_family_arm_MPU_A_unalignedBaseAddr
#define MPU_A_reservedAttrs ti_sysbios_family_arm_MPU_A_reservedAttrs
#define MPU_enableMPU ti_sysbios_family_arm_MPU_enableMPU
#define MPU_enableBackgroundRegion ti_sysbios_family_arm_MPU_enableBackgroundRegion
#define MPU_numRegions ti_sysbios_family_arm_MPU_numRegions
#define MPU_regionEntry ti_sysbios_family_arm_MPU_regionEntry
#define MPU_isMemoryMapped ti_sysbios_family_arm_MPU_isMemoryMapped
#define MPU_disable ti_sysbios_family_arm_MPU_disable
#define MPU_enable ti_sysbios_family_arm_MPU_enable
#define MPU_disableBR ti_sysbios_family_arm_MPU_disableBR
#define MPU_enableBR ti_sysbios_family_arm_MPU_enableBR
#define MPU_initRegionAttrs ti_sysbios_family_arm_MPU_initRegionAttrs
#define MPU_isEnabled ti_sysbios_family_arm_MPU_isEnabled
#define MPU_setRegion ti_sysbios_family_arm_MPU_setRegion
#define MPU_setRegionRaw ti_sysbios_family_arm_MPU_setRegionRaw
#define MPU_startup ti_sysbios_family_arm_MPU_startup
#define MPU_Module_name ti_sysbios_family_arm_MPU_Module_name
#define MPU_Module_id ti_sysbios_family_arm_MPU_Module_id
#define MPU_Module_startup ti_sysbios_family_arm_MPU_Module_startup
#define MPU_Module_startupDone ti_sysbios_family_arm_MPU_Module_startupDone
#define MPU_Module_hasMask ti_sysbios_family_arm_MPU_Module_hasMask
#define MPU_Module_getMask ti_sysbios_family_arm_MPU_Module_getMask
#define MPU_Module_setMask ti_sysbios_family_arm_MPU_Module_setMask
#define MPU_Object_heap ti_sysbios_family_arm_MPU_Object_heap
#define MPU_Module_heap ti_sysbios_family_arm_MPU_Module_heap

#endif /* ti_sysbios_family_arm_MPU__localnames__done */
#endif
