// Copyright 2020 The Pigweed Authors
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

#include <stdint.h>

#include "pw_preprocessor/util.h"
#include "pw_tokenizer/internal/argument_types.h"

PW_EXTERN_C_START

pw_tokenizer_ArgTypes pw_TestTokenizerNoArgs(void);

pw_tokenizer_ArgTypes pw_TestTokenizerChar(void);
pw_tokenizer_ArgTypes pw_TestTokenizerUint8(void);
pw_tokenizer_ArgTypes pw_TestTokenizerUint16(void);
pw_tokenizer_ArgTypes pw_TestTokenizerInt32(void);
pw_tokenizer_ArgTypes pw_TestTokenizerInt64(void);
pw_tokenizer_ArgTypes pw_TestTokenizerUint64(void);
pw_tokenizer_ArgTypes pw_TestTokenizerFloat(void);
pw_tokenizer_ArgTypes pw_TestTokenizerDouble(void);
pw_tokenizer_ArgTypes pw_TestTokenizerString(void);
pw_tokenizer_ArgTypes pw_TestTokenizerMutableString(void);

pw_tokenizer_ArgTypes pw_TestTokenizerIntFloat(void);
pw_tokenizer_ArgTypes pw_TestTokenizerUint64Char(void);
pw_tokenizer_ArgTypes pw_TestTokenizerStringString(void);
pw_tokenizer_ArgTypes pw_TestTokenizerUint16Int(void);
pw_tokenizer_ArgTypes pw_TestTokenizerFloatString(void);

pw_tokenizer_ArgTypes pw_TestTokenizerNull(void);
pw_tokenizer_ArgTypes pw_TestTokenizerPointer(void);
pw_tokenizer_ArgTypes pw_TestTokenizerPointerPointer(void);

PW_EXTERN_C_END
