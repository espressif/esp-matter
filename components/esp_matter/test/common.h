/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <lib/support/logging/TextOnlyLogging.h>

namespace esp_matter::test {

inline void discard_matter_log(const char *, uint8_t, const char *, va_list) {}

inline void suppress_matter_logs()
{
    // Suppress Matter logs before ESP32 platform logging prints prefixes that can interleave with Unity output.
    chip::Logging::SetLogRedirectCallback(discard_matter_log);
}

} // namespace esp_matter::test
