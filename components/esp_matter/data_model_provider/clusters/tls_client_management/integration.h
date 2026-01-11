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

#include <app/clusters/tls-certificate-management-server/CertificateTableImpl.h>
#include <app/clusters/tls-client-management-server/TLSClientManagementCluster.h>

namespace chip::app::Clusters::TlsClientManagement {

void SetDelegate(EndpointId endpointId, TLSClientManagementDelegate &delegate);

void SetCertificateTable(EndpointId endpointId, Tls::CertificateTableImpl &certificate_table);

} // namespace chip::app::Clusters::TlsClientManagement
