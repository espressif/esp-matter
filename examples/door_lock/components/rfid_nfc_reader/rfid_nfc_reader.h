/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

#include <cstddef>
#include <cstdint>

// Invoked from the NFC polling task's context (not the Matter/CHIP event
// loop) whenever a classic NFC tag is detected, with its raw identifier
// bytes (ISO14443A/B UID, FeliCa IDm, or ISO15693 UID -- whichever
// technology responded). The callback is responsible for hopping onto the
// Matter event loop (e.g. via chip::DeviceLayer::PlatformMgr().ScheduleWork())
// before touching any Matter/door-lock state.
using RfidTagDetectedCallback = void (*)(const uint8_t * uid, size_t uidLength);

// Hardware driver for the M5Stack Unit NFC (ST25R3916, I2C @ 0x50). Polls in
// turn for ISO14443A, ISO14443B, FeliCa and ISO15693 tags and reports the raw
// identifier of any tag it finds. Carries no Matter/door-lock knowledge --
// credential matching and unlocking are the caller's responsibility.
class RfidNfcReader final {
public:
    static RfidNfcReader  &Instance()
    {
        static RfidNfcReader reader;
        return reader;
    }

    // Brings up the ST25R3916 over I2C and starts the polling task. `callback`
    // is invoked once per detected tag; it must not block for long, since it
    // runs on the polling task and delays the next poll cycle.
    esp_err_t Init(RfidTagDetectedCallback callback);

private:
    RfidNfcReader() = default;

    static void PollTask(void * context);

    RfidTagDetectedCallback mCallback = nullptr;
    void * mTaskHandle = nullptr;
};
