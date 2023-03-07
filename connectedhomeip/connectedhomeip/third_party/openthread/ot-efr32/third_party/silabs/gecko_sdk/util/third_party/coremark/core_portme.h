/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/
/* File : core_portme.h */

/*
	Author : Shay Gal-On, EEMBC
	Legal : TODO!
*/ 
/* Topic : Description
	This file contains configuration constants required to execute on different platforms
*/
#ifndef CORE_PORTME_H
#define CORE_PORTME_H
/************************/
/* Data types and settings */
/************************/
/* Configuration : HAS_FLOAT 
	Define to 1 if the platform supports floating point.
*/
#ifndef HAS_FLOAT 
#define HAS_FLOAT 1
#endif
/* Configuration : HAS_TIME_H
	Define to 1 if platform has the time.h header file,
	and implementation of functions thereof.
*/
#ifndef HAS_TIME_H
#define HAS_TIME_H 0
#endif
/* Configuration : USE_CLOCK
	Define to 1 if platform has the time.h header file,
	and implementation of functions thereof.
*/
#ifndef USE_CLOCK
#define USE_CLOCK 0
#endif
/* Configuration : HAS_STDIO
	Define to 1 if the platform has stdio.h.
*/
#ifndef HAS_STDIO
#define HAS_STDIO 1
#endif
/* Configuration : HAS_PRINTF
	Define to 1 if the platform has stdio.h and implements the printf function.
*/
#ifndef HAS_PRINTF
#define HAS_PRINTF 0
#endif

/* Configuration : CORE_TICKS
	Define type of return from the timing functions.
 */
#include <time.h>
typedef clock_t CORE_TICKS;

/* Definitions : COMPILER_VERSION, COMPILER_FLAGS, MEM_LOCATION
	Initialize these strings per platform
*/
#ifndef COMPILER_VERSION 
#ifdef __GNUC__
#define COMPILER_VERSION "GCC"__VERSION__
#define COMPILER_FLAGS "-O3" /* "Please put compiler flags here (e.g. -o3)" */
#else
#ifdef __ICCARM__
#define SYMBHELPER(l) #l
#define TOSTRING(l) SYMBHELPER(l)
#define COMPILER_VERSION ("IAR " TOSTRING(__VER__))
#define COMPILER_FLAGS "-Os" /* "Please put compiler flags here (e.g. -o3)" */
#else
#ifdef __CC_ARM
#define SYMBHELPER(l) #l
#define TOSTRING(l) SYMBHELPER(l)
#define COMPILER_VERSION ("MDK-ARM " TOSTRING(__ARMCC_VERSION))
#define COMPILER_FLAGS "-O3 -Otime --acps=interwork" /* "Please put compiler flags here (e.g. -o3)" */
#else
#define COMPILER_VERSION "CC"
#define COMPILER_FLAGS "CFLAGS" /* "Please put compiler flags here (e.g. -o3)" */
#endif
#endif
#endif
#endif
#ifndef MEM_LOCATION 
 #define MEM_LOCATION "FLASH"
#endif

/* Data Types :
	To avoid compiler issues, define the data types that need ot be used for 8b, 16b and 32b in <core_portme.h>.
	
	*Imprtant* :
	ee_ptr_int needs to be the data type used to hold pointers, otherwise coremark may fail!!!
*/
typedef signed short ee_s16;
typedef unsigned short ee_u16;
typedef signed int ee_s32;
typedef double ee_f32;
typedef unsigned char ee_u8;
typedef unsigned int ee_u32;
typedef ee_u32 ee_ptr_int;
typedef size_t ee_size_t;
/* align_mem :
	This macro is used to align an offset to point to a 32b value. It is used in the Matrix algorithm to initialize the input memory blocks.
*/
#define align_mem(x) (void *)(4 + (((ee_ptr_int)(x) - 1) & ~3))

/* Configuration : SEED_METHOD
	Defines method to get seed values that cannot be computed at compile time.
	
	Valid values :
	SEED_ARG - from command line.
	SEED_FUNC - from a system function.
	SEED_VOLATILE - from volatile variables.
*/
#ifndef SEED_METHOD
#define SEED_METHOD SEED_VOLATILE
#endif

/* Configuration : MEM_METHOD
	Defines method to get a block of memry.
	
	Valid values :
	MEM_MALLOC - for platforms that implement malloc and have malloc.h.
	MEM_STATIC - to use a static memory array.
	MEM_STACK - to allocate the data block on the stack (NYI).
*/
#ifndef MEM_METHOD
#define MEM_METHOD MEM_STATIC
#endif

/* Configuration : MULTITHREAD
	Define for parallel execution 
	
	Valid values :
	1 - only one context (default).
	N>1 - will execute N copies in parallel.
	
	Note : 
	If this flag is defined to more then 1, an implementation for launching parallel contexts must be defined.
	
	Two sample implementations are provided. Use <USE_PTHREAD> or <USE_FORK> to enable them.
	
	It is valid to have a different implementation of <core_start_parallel> and <core_end_parallel> in <core_portme.c>,
	to fit a particular architecture. 
*/
#ifndef MULTITHREAD
#define MULTITHREAD 1
#define USE_PTHREAD 0
#define USE_FORK 0
#define USE_SOCKET 0
#endif

/* Configuration : MAIN_HAS_NOARGC
	Needed if platform does not support getting arguments to main. 
	
	Valid values :
	0 - argc/argv to main is supported
	1 - argc/argv to main is not supported
	
	Note : 
	This flag only matters if MULTITHREAD has been defined to a value greater then 1.
*/
#ifndef MAIN_HAS_NOARGC 
#define MAIN_HAS_NOARGC 1
#endif

/* Configuration : MAIN_HAS_NORETURN
	Needed if platform does not support returning a value from main. 
	
	Valid values :
	0 - main returns an int, and return value will be 0.
	1 - platform does not support returning a value from main
*/
#ifndef MAIN_HAS_NORETURN
#define MAIN_HAS_NORETURN 0
#endif

/* Variable : default_num_contexts
	Not used for this simple port, must cintain the value 1.
*/
extern ee_u32 default_num_contexts;

typedef struct CORE_PORTABLE_S {
	ee_u8	portable_id;
} core_portable;

/* target specific init/fini */
void portable_init(core_portable *p, int *argc, char *argv[]);
void portable_fini(core_portable *p);

#if !defined(PROFILE_RUN) && !defined(PERFORMANCE_RUN) && !defined(VALIDATION_RUN)
#if (TOTAL_DATA_SIZE==1200)
#define PROFILE_RUN 1
#elif (TOTAL_DATA_SIZE==2000)
#define PERFORMANCE_RUN 1
#else
#define VALIDATION_RUN 1
#endif
#endif

#endif /* CORE_PORTME_H */
