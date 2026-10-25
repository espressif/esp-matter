// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

// esp-matter implements this cluster's server in its own integration.cpp and registers it through
// the esp_matter data-model provider registry. We reuse the cluster/Instance declarations from
// upstream's CodegenIntegration.h (which is designed for the codegen data model provider) instead of
// redeclaring them here; the codegen RegisterServer path itself is not used.
#include <app/clusters/water-heater-management-server/CodegenIntegration.h>
