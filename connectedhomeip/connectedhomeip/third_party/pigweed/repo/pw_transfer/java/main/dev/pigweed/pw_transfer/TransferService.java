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

package dev.pigweed.pw_transfer;

import static dev.pigweed.pw_rpc.Service.bidirectionalStreamingMethod;

import dev.pigweed.pw_rpc.Service;

/** Provides a service definition for the pw_transfer service. */
public class TransferService {
  private static final Service SERVICE = new Service("pw.transfer.Transfer",
      bidirectionalStreamingMethod("Read", Chunk.class, Chunk.class),
      bidirectionalStreamingMethod("Write", Chunk.class, Chunk.class));

  public static Service get() {
    return SERVICE;
  }

  private TransferService() {}
}
