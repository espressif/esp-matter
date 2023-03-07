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

#ifndef __XA_MEMORY_STANDARDS_H__
#define __XA_MEMORY_STANDARDS_H__

/*****************************************************************************/
/* Constant hash defines                                                     */
/*****************************************************************************/
/* when you don't need alignment, pass this to memory library */
#define XA_MEM_NO_ALIGN				0x01

/* standard memory types */
/* to be used inter frames */
#define XA_MEMTYPE_PERSIST				0x00 
/* read write, to be used intra frames */
#define XA_MEMTYPE_SCRATCH				0x01 
/* read only memory, intra frame */
#define XA_MEMTYPE_INPUT				0x02 
/* read-write memory, for usable output, intra frame */
#define XA_MEMTYPE_OUTPUT				0x03 
/* readonly memory, inter frame */
#define XA_MEMTYPE_TABLE				0x04 
/* input buffer before mem tabs allocation */
#define XA_MEMTYPE_PRE_FRAME_INPUT		0x05 
/* input buffer before mem tabs allocation */
#define XA_MEMTYPE_PRE_FRAME_SCRATCH	0x06 
/* for local variables */
#define XA_MEMTYPE_AUTO_VAR				0x80 

/* standard memory priorities */
#define XA_MEMPRIORITY_ANYWHERE			0x00
#define XA_MEMPRIORITY_LOWEST			0x01
#define XA_MEMPRIORITY_LOW				0x02
#define XA_MEMPRIORITY_NORM				0x03
#define XA_MEMPRIORITY_ABOVE_NORM		0x04
#define XA_MEMPRIORITY_HIGH				0x05
#define XA_MEMPRIORITY_HIGHER			0x06
#define XA_MEMPRIORITY_CRITICAL			0x07

/* standard memory placements */
/* placement is defined by 64 bits */

#define XA_MEMPLACE_FAST_RAM_0			0x000001
#define XA_MEMPLACE_FAST_RAM_1			0x000002
#define XA_MEMPLACE_FAST_RAM_2			0x000004
#define XA_MEMPLACE_FAST_RAM_3			0x000008
#define XA_MEMPLACE_FAST_RAM_4			0x000010
#define XA_MEMPLACE_FAST_RAM_5			0x000020
#define XA_MEMPLACE_FAST_RAM_6			0x000040
#define XA_MEMPLACE_FAST_RAM_7			0x000080

#define XA_MEMPLACE_INT_RAM_0			0x000100
#define XA_MEMPLACE_INT_RAM_1			0x000200
#define XA_MEMPLACE_INT_RAM_2			0x000400
#define XA_MEMPLACE_INT_RAM_3			0x000800
#define XA_MEMPLACE_INT_RAM_4			0x001000
#define XA_MEMPLACE_INT_RAM_5			0x002000
#define XA_MEMPLACE_INT_RAM_6			0x004000
#define XA_MEMPLACE_INT_RAM_7			0x008000

#define XA_MEMPLACE_EXT_RAM_0			0x010000
#define XA_MEMPLACE_EXT_RAM_1			0x020000
#define XA_MEMPLACE_EXT_RAM_2			0x040000
#define XA_MEMPLACE_EXT_RAM_3			0x080000
#define XA_MEMPLACE_EXT_RAM_4			0x100000
#define XA_MEMPLACE_EXT_RAM_5			0x200000
#define XA_MEMPLACE_EXT_RAM_6			0x400000
#define XA_MEMPLACE_EXT_RAM_7			0x800000

#define XA_MEMPLACE_DONTCARE_H			0xFFFFFFFF
#define XA_MEMPLACE_DONTCARE_L			0xFFFFFFFF

/* the simple common PC RAM */
#define XA_PC_RAM_H					0x00000000
#define XA_PC_RAM_L					XA_MEMPLACE_EXT_RAM_0

#endif /* __XA_MEMORY_STANDARDS_H__ */
