/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>

// Wires the RfidNfcReader hardware driver to the door-lock's credential
// storage and bolt actuator: starts polling for classic NFC tags and, on a
// match against a provisioned RFID credential, unlocks the door and reports
// the credential on the emitted LockOperation event.
esp_err_t rfid_door_lock_init();
