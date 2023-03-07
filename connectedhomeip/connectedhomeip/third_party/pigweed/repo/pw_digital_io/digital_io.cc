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

#include "pw_digital_io/digital_io.h"

namespace pw::digital_io {

Status DigitalInterrupt::DoSetState(State) {
  PW_CRASH("DoSetState not implemented");
}
Result<State> DigitalInterrupt::DoGetState() {
  PW_CRASH("DoGetState not implemented");
}

Status DigitalIn::DoSetState(State) { PW_CRASH("DoSetState not implemented"); }
Status DigitalIn::DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) {
  PW_CRASH("DoSetInterruptHandler not implemented");
}
Status DigitalIn::DoEnableInterruptHandler(bool) {
  PW_CRASH("DoEnableInterruptHandler not implemented");
}

Status DigitalInInterrupt::DoSetState(State) {
  PW_CRASH("DoSetState not implemented");
}

Result<State> DigitalOut::DoGetState() {
  PW_CRASH("DoGetState not implemented");
}
Status DigitalOut::DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) {
  PW_CRASH("DoSetInterruptHandler not implemented");
}
Status DigitalOut::DoEnableInterruptHandler(bool) {
  PW_CRASH("DoEnableInterruptHandler not implemented");
}

Result<State> DigitalOutInterrupt::DoGetState() {
  PW_CRASH("DoGetState not implemented");
}

Status DigitalInOut::DoSetInterruptHandler(InterruptTrigger,
                                           InterruptHandler&&) {
  PW_CRASH("DoSetInterruptHandler not implemented");
}
Status DigitalInOut::DoEnableInterruptHandler(bool) {
  PW_CRASH("DoEnableInterruptHandler not implemented");
}

}  // namespace pw::digital_io
