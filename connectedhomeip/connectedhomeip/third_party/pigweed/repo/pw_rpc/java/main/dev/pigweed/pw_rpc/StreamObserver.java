// Copyright 2021 The Pigweed Authors
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

package dev.pigweed.pw_rpc;

import com.google.protobuf.MessageLite;

/** Receives events from an RPC stream. Used by all RPC types. */
public interface StreamObserver<T extends MessageLite> {
  /** Called when an RPC message is received. */
  void onNext(T value);

  /** Called when the RPC completes with the status returned from the RPC. */
  void onCompleted(Status status);

  /** Called when an RPC terminates unexpectedly due to an error. */
  void onError(Status status);
}
