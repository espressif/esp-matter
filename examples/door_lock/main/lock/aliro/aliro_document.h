/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "door_lock_storage.h"

#include <esp_aliro.h>

struct AliroProof {
    uint64_t generation = 0;
    uint32_t clockGeneration = 0;
    uint8_t endpointKey[65] = {};
    bool issuerPath = false;
    DoorLockStorage::Match credential;
    bool capabilityAllowed = false;
    bool hasIteration = false;
    uint64_t iteration = 0;
    char validFrom[21] = {};
    char validUntil[21] = {};
};

namespace AliroDocument {

constexpr uint8_t kElement[] = {'m', 'a', 't', 't', 'e', 'r', '1'};

/** Verify a borrowed ENVELOPE response against current issuers and the captured clock generation; copy proof facts. */
esp_err_t Verify(const uint8_t *response, size_t responseSize,
                 const esp_aliro_expedited_result_t &authentication, AliroProof &proof);

/** Check the verified document window against currently synchronized UTC. */
bool CurrentTimeValid(const AliroProof &proof);

} // namespace AliroDocument
