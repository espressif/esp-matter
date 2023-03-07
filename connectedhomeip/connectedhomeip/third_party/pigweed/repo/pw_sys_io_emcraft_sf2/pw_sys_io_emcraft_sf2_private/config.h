// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#pragma once

// Defaults to USART1 on the SmartFusion2, but can be overridden.

// The USART peripheral number to use. (1 for USART1, 2 for USART2, etc.)
#ifndef PW_SYS_IO_EMCRAFT_SF2_USART_NUM
#define PW_SYS_IO_EMCRAFT_SF2_USART_NUM 1
#endif  // PW_SYS_IO_EMCRAFT_SF2_USART_NUM
