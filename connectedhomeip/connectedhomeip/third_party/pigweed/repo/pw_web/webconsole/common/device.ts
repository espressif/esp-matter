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

import {Device} from "pigweedjs";
import {createDefaultProtoCollection} from "./protos";

/**
 * Returns an instance of Device, ensures there is only one Device in
 * current session.
 *
 * We do this to avoid multiple clients listening on single serial port.
 */
export default async function SingletonDevice(): Promise<Device> {
  if ((window as any).device === undefined) {
    const protoCollection = await createDefaultProtoCollection();
    (window as any).device = new Device(protoCollection);
  }
  return (window as any).device;
}
