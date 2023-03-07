/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
* the following conditions:
* 
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************************/

#ifndef __XA_ERROR_STANDARDS_H__
#define __XA_ERROR_STANDARDS_H__

/*****************************************************************************/
/* File includes                                                             */
/*  xa_type_def.h                                                            */
/*****************************************************************************/

/*****************************************************************************/
/* Constant hash defines                                                     */
/*****************************************************************************/
#define XA_NO_ERROR	0
#define XA_FATAL_ERROR	0x80000000

enum xa_error_severity {
  xa_severity_nonfatal = 0,
  xa_severity_fatal    = 0xffffffff
};

enum xa_error_class {
  xa_class_api     = 0,
  xa_class_config  = 1,
  xa_class_execute = 2,
  xa_class_proxy   = 3
};

#define XA_CODEC_GENERIC	0

#define XA_ERROR_CODE(severity, class, codec, index)	((severity << 15) | (class << 11) | (codec << 6) | index)
#define XA_ERROR_SEVERITY(code)	(((code) & XA_FATAL_ERROR) != 0)
#define XA_ERROR_CLASS(code)	(((code) >> 11) & 0x0f)
#define XA_ERROR_CODEC(code)    (((code) >>  6) & 0x1f)
#define XA_ERROR_SUBCODE(code)	(((code) >>  0) & 0x3f)

/* Our convention is that only api-class errors can be generic ones. */

/*****************************************************************************/
/* Class 0: API Errors                                                       */
/*****************************************************************************/
/* Non Fatal Errors */
/* (none) */
/* Fatal Errors */
enum xa_error_fatal_api_generic {
  XA_API_FATAL_MEM_ALLOC        = XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_GENERIC, 0),
  XA_API_FATAL_MEM_ALIGN        = XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_GENERIC, 1),
  XA_API_FATAL_INVALID_CMD      = XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_GENERIC, 2),
  XA_API_FATAL_INVALID_CMD_TYPE = XA_ERROR_CODE(xa_severity_fatal, xa_class_api, XA_CODEC_GENERIC, 3)
};

#endif /* __XA_ERROR_STANDARDS_H__ */
