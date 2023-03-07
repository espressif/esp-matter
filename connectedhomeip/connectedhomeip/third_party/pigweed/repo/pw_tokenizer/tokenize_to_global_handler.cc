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

#include "pw_tokenizer/tokenize_to_global_handler.h"

#include "pw_tokenizer/encode_args.h"

namespace pw {
namespace tokenizer {

extern "C" void _pw_tokenizer_ToGlobalHandler(pw_tokenizer_Token token,
                                              pw_tokenizer_ArgTypes types,
                                              ...) {
  va_list args;
  va_start(args, types);
  EncodedMessage encoded(token, types, args);
  va_end(args);

  pw_tokenizer_HandleEncodedMessage(encoded.data_as_uint8(), encoded.size());
}

}  // namespace tokenizer
}  // namespace pw
