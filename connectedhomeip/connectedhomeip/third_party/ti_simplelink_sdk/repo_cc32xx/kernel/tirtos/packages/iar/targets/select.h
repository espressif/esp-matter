/*
 *  Copyright 2021 by Texas Instruments Incorporated.
 *
 */

/*
 *  ======== select.h ========
 *  This header selects an appropriate target name based on TI compiler
 *  predefined macros and includes the appropriate target-specific std.h.
 *
 *  This header is entirely optional: the user can directly select any
 *  existing target by adding definition of the following two symbols to
 *  the compile line:
 *    xdc_target_name__     the short name of the target; e.g., C64P
 *    xdc_target_types__    a package path relative path to the types header
 *                          for the target; e.g., ti/targets/std.h
 *
 *  For more information about these symbols see:
 *  http://rtsc.eclipse.org/docs-tip/Integrating_RTSC_Modules
 */

#if defined (__ICCARM__)
#  define xdc_target_types__ iar/targets/arm/std.h

#  if (__CORE__ == __ARM7M__)
#      define xdc_target_name__ M3
#  elif (__CORE__ == __ARM7EM__)
#      if defined(__ARMVFP__)
#          define xdc_target_name__ M4F
#      else
#          define xdc_target_name__ M4
#      endif
#  endif

#endif

#if defined(xdc_target_name__) && defined(xdc_target_types__)
/*
 *  ======== include the selected type header ========
 */
#define xdc_target__ <xdc_target_types__>
#include xdc_target__

#else
  /* if we get here, this header was unable to select an appropriate set of
   * types.  If the target exists, you can avoid the warnings below by
   * explicitly defining the symbols xdc_target_name__ and
   * xdc_target_types__ on the compile line.
   */
#  ifndef xdc_target_name__
#    warning "can't determine an appropriate setting for xdc_target_name__"
#  endif
#  ifndef xdc_target_types__
#    warning "can't determine an appropriate setting for xdc_target_types__"
#  endif
#endif
/*
 *  @(#) iar.targets; 1, 0, 1,; 7-28-2021 06:57:34; /db/ztree/library/trees/xdctargets/xdctargets-w20/src/ xlibrary

 */

