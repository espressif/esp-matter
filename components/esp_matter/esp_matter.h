// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

/*
This is a common include file which includes all the other esp_matter component files which would be required by the
application.
*/

#include <esp_matter_attribute.h>
#include <esp_matter_cluster.h>
#include <esp_matter_command.h>
#include <esp_matter_client.h>
#include <esp_matter_core.h>
#include <esp_matter_endpoint.h>
#include <esp_matter_event.h>

#include <app-common/zap-generated/att-storage.h>
#include <app-common/zap-generated/attribute-id.h>
#include <app-common/zap-generated/cluster-id.h>
#include <app-common/zap-generated/command-id.h>

#include <app/server/Dnssd.h>
#include <platform/CHIPDeviceLayer.h>

#define ATTRIBUTE_MASK_NONE 0x0
#define COMMAND_MASK_NONE 0x0
#define CLUSTER_MASK_NONE 0x0
#define ENDPOINT_MASK_NONE 0x0

#define ENDPOINT_MASK_DELETABLE 0x1

#define COMMAND_MASK_CUSTOM 0x80
