/********************************************************************************************************
 * @file	HIDReportData.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#pragma once

#define HID_RPT_DATA_SIZE_MASK                  (0x03)
#define HID_RPT_TYPE_MASK                       (0x0C)
#define HID_RPT_TAG_MASK                        (0xF0)

#define HID_RPT_TYPE_MAIN                       (0x00)
#define HID_RPT_TYPE_GLOBAL                     (0x04)
#define HID_RPT_TYPE_LOCAL                      (0x08)

#define HID_RPT_DATA_BITS_0                     (0x00)
#define HID_RPT_DATA_BITS_8                     (0x01)
#define HID_RPT_DATA_BITS_16                    (0x02)
#define HID_RPT_DATA_BITS_32                    (0x03)
#define HID_RPT_DATA_BITS(Data_bits)            HID_RPT_DATA_BITS_##Data_bits

#define _HID_RPT_DATA_ENCODE_0(Data)
#define _HID_RPT_DATA_ENCODE_8(Data)            , (Data & 0xFF)
#define _HID_RPT_DATA_ENCODE_16(Data)           _HID_RPT_DATA_ENCODE_8(Data)  _HID_RPT_DATA_ENCODE_8(Data>>8)
#define _HID_RPT_DATA_ENCODE_32(Data)           _HID_RPT_DATA_ENCODE_16(Data) _HID_RPT_DATA_ENCODE_16(Data>>16)
#define _HID_RPT_DATA_ENCODE(Data_bits, ...)    _HID_RPT_DATA_ENCODE_##Data_bits(__VA_ARGS__)


#define _HID_RPT_DATA_ENTRY(Type, Tag, Data_bits, ...)  \
												(Type | Tag | HID_RPT_DATA_BITS(Data_bits)) _HID_RPT_DATA_ENCODE(Data_bits, (__VA_ARGS__))


#define HID_IOF_CONSTANT                        BIT(0)
#define HID_IOF_DATA                            (0<<0)
#define HID_IOF_VARIABLE                        BIT(1)
#define HID_IOF_ARRAY                           (0<<1)
#define HID_IOF_RELATIVE                        BIT(2)
#define HID_IOF_ABSOLUTE                        (0<<2)
#define HID_IOF_WRAP                            BIT(3)
#define HID_IOF_NO_WRAP                         (0<<3)
#define HID_IOF_NON_LINEAR                      BIT(4)
#define HID_IOF_LINEAR                          (0<<4)
#define HID_IOF_NO_PREFERRED_STATE              BIT(5)
#define HID_IOF_PREFERRED_STATE                 (0<<5)
#define HID_IOF_NULLSTATE                       BIT(6)
#define HID_IOF_NO_NULL_POSITION                (0<<6)
#define HID_IOF_VOLATILE                        BIT(7)
#define HID_IOF_NON_VOLATILE                    (0<<7)
#define HID_IOF_BUFFERED_BYTES                  BIT(8)
#define HID_IOF_BITFIELD                        (0<<8)


#define HID_RPT_INPUT(Data_bits, ...)             _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0x80, Data_bits, __VA_ARGS__)
#define HID_RPT_OUTPUT(Data_bits, ...)            _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0x90, Data_bits, __VA_ARGS__)
#define HID_RPT_COLLECTION(Data_bits, ...)        _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0xA0, Data_bits, __VA_ARGS__)
#define HID_RPT_FEATURE(Data_bits, ...)           _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0xB0, Data_bits, __VA_ARGS__)
#define HID_RPT_END_COLLECTION(Data_bits, ...)    _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_MAIN  , \
																0xC0, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE_PAGE(Data_bits, ...)        _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x00, Data_bits, __VA_ARGS__)
#define HID_RPT_LOGICAL_MINIMUM(Data_bits, ...)   _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x10, Data_bits, __VA_ARGS__)
#define HID_RPT_LOGICAL_MAXIMUM(Data_bits, ...)   _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x20, Data_bits, __VA_ARGS__)
#define HID_RPT_PHYSICAL_MINIMUM(Data_bits, ...)  _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x30, Data_bits, __VA_ARGS__)
#define HID_RPT_PHYSICAL_MAXIMUM(Data_bits, ...)  _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x40, Data_bits, __VA_ARGS__)
#define HID_RPT_UNIT_EXPONENT(Data_bits, ...)     _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x50, Data_bits, __VA_ARGS__)
#define HID_RPT_UNIT(Data_bits, ...)              _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x60, Data_bits, __VA_ARGS__)
#define HID_RPT_REPORT_SIZE(Data_bits, ...)       _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x70, Data_bits, __VA_ARGS__)
#define HID_RPT_REPORT_ID(Data_bits, ...)         _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x80, Data_bits, __VA_ARGS__)
#define HID_RPT_REPORT_COUNT(Data_bits, ...)      _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0x90, Data_bits, __VA_ARGS__)
#define HID_RPT_PUSH(Data_bits, ...)              _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0xA0, Data_bits, __VA_ARGS__)
#define HID_RPT_POP(Data_bits, ...)               _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_GLOBAL, \
																0xB0, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE(Data_bits, ...)             _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_LOCAL , \
																0x00, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE_MINIMUM(Data_bits, ...)     _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_LOCAL , \
																0x10, Data_bits, __VA_ARGS__)
#define HID_RPT_USAGE_MAXIMUM(Data_bits, ...)     _HID_RPT_DATA_ENTRY(HID_RPT_TYPE_LOCAL , \
																0x20, Data_bits, __VA_ARGS__)

