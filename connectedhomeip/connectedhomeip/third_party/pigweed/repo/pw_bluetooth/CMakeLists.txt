# Copyright 2022 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

pw_add_library(pw_bluetooth INTERFACE
  HEADERS
    public/pw_bluetooth/address.h
    public/pw_bluetooth/assigned_uuids.h
    public/pw_bluetooth/gatt/client.h
    public/pw_bluetooth/gatt/constants.h
    public/pw_bluetooth/gatt/error.h
    public/pw_bluetooth/gatt/server.h
    public/pw_bluetooth/gatt/types.h
    public/pw_bluetooth/internal/hex.h
    public/pw_bluetooth/internal/raii_ptr.h
    public/pw_bluetooth/low_energy/advertising_data.h
    public/pw_bluetooth/low_energy/bond_data.h
    public/pw_bluetooth/low_energy/central.h
    public/pw_bluetooth/low_energy/connection.h
    public/pw_bluetooth/low_energy/peripheral.h
    public/pw_bluetooth/low_energy/security_mode.h
    public/pw_bluetooth/constants.h
    public/pw_bluetooth/controller.h
    public/pw_bluetooth/host.h
    public/pw_bluetooth/pairing_delegate.h
    public/pw_bluetooth/peer.h
    public/pw_bluetooth/result.h
    public/pw_bluetooth/types.h
    public/pw_bluetooth/uuid.h
    public/pw_bluetooth/vendor.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_containers
    pw_function
    pw_status
    pw_string.string
    pw_chrono.system_clock
)

pw_add_test(pw_bluetooth.address_test
  SOURCES
    address_test.cc
  PRIVATE_DEPS
    pw_bluetooth
  GROUPS
    pw_bluetooth
)

pw_add_test(pw_bluetooth.api_test
  SOURCES
    api_test.cc
  PRIVATE_DEPS
    pw_bluetooth
  GROUPS
    pw_bluetooth
)

pw_add_test(pw_bluetooth.result_test
  SOURCES
    result_test.cc
  PRIVATE_DEPS
    pw_bluetooth
  GROUPS
    pw_bluetooth
)

pw_add_test(pw_bluetooth.uuid_test
  SOURCES
    uuid_test.cc
  PRIVATE_DEPS
    pw_bluetooth
  GROUPS
    pw_bluetooth
)
