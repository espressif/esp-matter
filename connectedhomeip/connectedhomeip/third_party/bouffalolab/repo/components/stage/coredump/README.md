<!--
Copyright (c) 2016-2022 Bouffalolab.

This file is part of
    *** Bouffalolab Software Dev Kit ***
     (see www.bouffalolab.com).

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. Neither the name of Bouffalo Lab nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->
# Core dump

在系统异常时dump出指定的内存数据

## 功能

1. 可选配置默认dump区域，在exception时自动dump至UART，通过串口工具保存
2. 输出端口可配（用户自行适配）：
  - 串口
  - flash
  - spi

3. base64加密编码输出

## 结构

1. Dump Tool命令


| type     | header | operation | *data | operation | *data | stop |
| -------- | ------ | --------- | ----- | --------- | ----- | ---- |
| **len**  | 1      | 1         | 4     | 1         | 4     | 1    |
| **data** | $      | command   | addr  | command   | len   | #    |

operation支持以下命令：

> 所有命令均为字符型

| command | info             |
| ------- | ---------------- |
| d       | dump默认内存区域 |
| x       | 指定dump地址     |
| l       | 指定dump长度     |
| ...     |                  |

**注：**

- **x和l参数成对传入，先传入x，再传入l，若未提供l，默认dump长度为0x1000**
- **一次命令仅可包含一对`addr/len`**
- **若未配置默认dump区域，d命令不会响应；传入d命令时，其他指令会被忽略**



2. dump data

| header                            | data | crc stop              |
| --------------------------------- | ---- | --------------------- |
| ------ DATA BEGIN addr@len ------ | ***  | ------ END crc ------ |

CRC为解码后原数据的CRC，不是base64编码的CRC
