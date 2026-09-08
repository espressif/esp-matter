# Add classic NFC-tag (RFID) credential support to `examples/door_lock`

GitHub issue: [espressif/esp-matter#1836](https://github.com/espressif/esp-matter/issues/1836)

## Context

`examples/door_lock` currently only demonstrates Aliro-over-NFC (phone/wearable
digital-key access). Integrators who just want classic RFID/NFC-tag badges
(ISO14443A/B, FeliCa, ISO15693 — the common warehouse/office badge use case)
have to fork the example and strip Aliro out, which is exactly the redundant
work the issue asks us to eliminate. The requested feature is additive and
independent of Aliro: read a tag UID with the M5Stack Unit NFC (ST25R3916,
I2C @ 0x50), match it against a `CredentialType = RFID` credential via the
existing `SetCredential`/`GetCredentialStatus` plumbing, and unlock — emitting
a `LockOperation` event with the right `OperationSource` and `CredentialIndex`.

Investigation of the current code shows most of the plumbing already exists
and is credential-type-agnostic (`DoorLockStorage` already reserves a slot
namespace for `CredentialTypeEnum::kRfid`, and `esp_matter`'s legacy data
model already has a ready-to-use `rfid_credential` feature). The two real
gaps are: (1) a hardware driver that reads generic tag UIDs — the existing
`m5nfc` component pulled in for Aliro is a thin wrapper that jumps straight
into an Aliro-specific ISO-DEP/APDU session and has no UID/tech-detection API
— and (2) threading the matched credential index through to the emitted
`LockOperation` event, which today only carries `OperationSource` for every
unlock path (the door_lock_callbacks.cpp only logs credential/user info via
`ESP_LOGW`, it never reaches `DoorLockServer::SetLockState`).

## Key design decisions

1. **New local NFC driver, not an extension of `m5nfc`.** `m5nfc.cpp` (fetched
   from `espressif/esp-aliro`) is built on M5Stack's own `M5Unit-NFC` library
   (`M5UnitUnified` stack: `nfc_layer_a.hpp`, `isoDEP.hpp`,
   `unit_ST25R3916.hpp`), but only exposes `m5nfc_init/update/activate/
   deactivate/message_exchange` — Aliro-only, no UID or multi-tech API, and
   it's an external repo we don't own. `M5Unit-NFC` itself, however, natively
   ships example sketches for `NFCA/Detect`, `NFCB/Detect`, `NFCF/*`,
   `NFCV/*` — i.e. it already does exactly what we need. So: add a new local
   component `examples/door_lock/components/rfid_nfc_reader/` that depends
   directly on `m5stack/M5Unit-NFC` (pinned git dependency, same pattern
   `m5nfc`'s own `idf_component.yml` uses) and wraps its per-technology
   detect APIs behind one `RfidNfcReader` class. This keeps RFID-only builds
   completely free of `esp_aliro_lib`/`m5nfc`/Aliro crypto.

2. **Mutually exclusive with Aliro for v1**, gated by a new
   `CONFIG_ENABLE_RFID_NFC` Kconfig option (independent bool, `depends on
   !ENABLE_ALIRO_OVER_NFC`). Both features poll the same physical NFC
   front-end; running two independent activation loops against one ST25R3916
   at once is a real hardware/driver conflict, and the issue explicitly asks
   for RFID *without* Aliro. Combining them is future work.

3. **Reuse `DoorLockStorage` as-is** — it's already generic over
   `CredentialTypeEnum` and `kRfid` already has a reserved slot namespace
   (`door_lock_storage.h:21`, `kCredentialTypeCount = 9`). Only need to (a)
   populate `credentialSlotsByType[kRfid]` and (b) add a `ValidateRfid()`
   matcher mirroring the existing `ValidatePIN()` (`door_lock_storage.cpp:378`).

4. **Fix event reporting generically.** `DoorLockServer::SetLockState` has a
   richer overload:
   `SetLockState(endpoint, state, opSource, userIndex, credentials, fabricIdx, nodeId)`
   that emits `CredentialType`/`CredentialIndex` in the `LockOperation` event
   — today the example only ever calls the 3-arg overload. Thread an optional
   match (`userIndex` + `credentialIndex` + `credentialType`) from
   `BoltLockManager::Lock/Unlock()` through `SetState()`/`StateChangeCallback`
   to `HandleBoltStateChange()`, which then picks the richer overload when a
   credential was used. This is a small, generic change (not RFID-specific)
   that also happens to fix the same long-standing gap for PIN unlocks, since
   `ValidateRemotePIN` already computes a `PinMatch` that's currently thrown
   away except for logging.

## File-by-file changes

### New component: `examples/door_lock/components/rfid_nfc_reader/`
- `idf_component.yml` — depend on `m5stack/M5Unit-NFC` (git, pinned commit),
  scoped to the same target list as `m5nfc` (`esp32, esp32s3, esp32c3,
  esp32c6, esp32h2, esp32p4`).
- `Kconfig.projbuild` — `CONFIG_ST25R3916_PIN_SDA`/`_SCL` (mirroring the
  symbol names the Aliro sdkconfig variants already set — need to confirm
  exact names once `M5Unit-NFC` is vendored via `idf.py reconfigure`; if it
  ships its own I2C pin Kconfig, reuse that instead of inventing new symbols).
- `rfid_nfc_reader.h/.cpp` — `RfidNfcReader` singleton:
  - `esp_err_t Init()` — bring up the ST25R3916 over I2C (mirrors
    `m5nfc_init()`).
  - Polling task (mirrors `AliroDoorLockDelegate::NfcDetectTask`,
    `aliro_door_lock_delegate.cpp:172-214`: 6144-byte stack, prio 5, 100 ms
    idle delay): each iteration cycles NFC-A → NFC-B → NFC-F → NFC-V
    detect/request calls; on any tag response, extract UID bytes (+ ATQA/SAK
    or tech tag, useful for logging only) and stop polling other techs for
    that cycle.
  - On UID read: `DoorLockStorage::Instance().ValidateRfid(uidSpan, match)`;
    on match, `DeviceLayer::PlatformMgr().ScheduleWork(...)` →
    `BoltLockManager::Instance().Unlock(OperationSourceEnum::kRfid /* or
    kKeypad if the Matter enum has no dedicated RFID value — confirm against
    the submodule's `OperationSourceEnum` once checked out */, match)`.
    On no match, log and continue polling (no lockout/backoff logic beyond
    what `WrongCodeEntryLimit`/`UserCodeTemporaryDisableTime` already cover
    via the `rfid_credential` feature attributes).

### `main/lock/door_lock_capabilities.h`
- Add `constexpr uint16_t kRfidCredentialSlots = 6;` (same stride as
  `kPinCredentialSlots`, so `kCredentialSlotsPerType` does not need to
  change) + a `static_assert(kCredentialSlotsPerType >= kRfidCredentialSlots)`.

### `main/lock/door_lock_storage.h` / `.cpp`
- Add `struct RfidMatch { uint16_t userIndex; uint16_t credentialIndex; };`
  and `bool ValidateRfid(const chip::ByteSpan &uid, RfidMatch &match) const;`,
  implemented by mirroring `ValidatePIN()` (`door_lock_storage.cpp:378-410`):
  linear-scan `kRfid`-typed credential slots, compare `data`/`dataLength`
  against the UID bytes, cross-reference the owning enabled user.

### `main/lock/door_lock_callbacks.cpp`
- `ReadCredentialSlotLimits()` (lines 59-78): add, gated by
  `#ifdef CONFIG_ENABLE_RFID_NFC`,
  `limits.credentialSlotsByType[to_underlying(CredentialTypeEnum::kRfid)] =
  DoorLockCapabilities::kRfidCredentialSlots;`
- Extend `HandleBoltStateChange()` (lines 80-109) and its signature (plus
  `BoltLockManager::StateChangeCallback`, `Lock()`/`Unlock()`,
  `StartMovement()`, `SetState()` in `bolt_lock_manager.h/.cpp`) to carry an
  optional `DoorLockStorage`-style match (`Nullable<uint16_t> userIndex`,
  `Nullable<uint16_t> credentialIndex`, `CredentialTypeEnum`). When present,
  call the richer `DoorLockServer::SetLockState(endpoint, lockState, source,
  userIndex, credentials, NullNullable, NullNullable)` overload instead of
  the 3-arg one; when absent, keep today's 3-arg call. Wire this for both the
  new RFID path and (as a natural side-effect, reusing the same plumbing) the
  existing PIN remote-unlock path in `emberAfPluginDoorLockOnDoorUnlockCommand`.

### `main/lock/door_lock_console.cpp`
- Add an RFID occupied/available counter block to `print_door_lock_status()`
  mirroring the existing Aliro block (gated by `CONFIG_ENABLE_RFID_NFC`).
- Add an `rfid-unlock <hex-uid>` diagnostic command mirroring `pin-unlock`
  (for bench testing without a physical tag).

### `main/app_main.cpp`
- Add, gated by `#ifdef CONFIG_ENABLE_RFID_NFC`:
  `cluster::door_lock::feature::rfid_credential::config_t rfid_credential_config;`
  (`number_rfid_users_supported = DoorLockCapabilities::kRfidCredentialSlots`)
  and `cluster::door_lock::feature::rfid_credential::add(door_lock_cluster,
  &rfid_credential_config)` — mirrors the existing `pin_credential` wiring
  (`app_main.cpp:167-177`); uses the already-existing
  `esp_matter_feature.cpp` legacy-data-model support (confirmed present,
  no esp_matter component changes needed).
- `RfidNfcReader::Instance().Init()` call alongside `app_driver_init()`.
- No `Delegate` involvement — RFID does not need a custom `DoorLock::Delegate`
  (that's Aliro-specific reader-config/attrs); it only needs the Ember
  `GetCredential`/`SetCredential` callbacks, which are already generic.

### `main/Kconfig.projbuild`
- New `config ENABLE_RFID_NFC` (`bool`, `default n`, `depends on
  !ENABLE_ALIRO_OVER_NFC`, help text explaining the mutual exclusivity).

### `main/CMakeLists.txt`
- Add `EXCLUDE_SRCS` gating for the new reader source(s) when
  `CONFIG_ENABLE_RFID_NFC` is unset, mirroring the existing
  `aliro_door_lock_delegate.cpp` exclusion (lines 3-5). If the reader lives in
  its own component instead of `main/lock/`, this may instead be a
  conditional `EXTRA_COMPONENT_DIRS`/component-manager rule.

### `main/idf_component.yml`
- No change needed at the `main` level if the reader lives in its own local
  component (component manager auto-discovers `components/` subfolders);
  otherwise add the `m5stack/M5Unit-NFC` dependency directly here, scoped
  like `m5nfc`'s existing rule.

### New sdkconfig variants
- `examples/door_lock/sdkconfig.defaults.nanoc6_rfid_nfc` and
  `...nanoh2_rfid_nfc` — copy the existing `..._aliro_nfc` variants, swap
  `CONFIG_ENABLE_ALIRO_OVER_NFC=y` → `CONFIG_ENABLE_RFID_NFC=y`, keep the
  `# NFC Unit Pins` block (`CONFIG_ST25R3916_PIN_SDA=2` /
  `CONFIG_ST25R3916_PIN_SCL=1`) since it's the same physical M5Unit-NFC
  module/wiring.

### `README.md`
- New section (mirroring the existing "Aliro over NFC Feature" section:
  Hardware Required / Build / Test) documenting the RFID/NFC-tag feature:
  same M5Stack NanoC6/NanoH2 + M5Unit-NFC hardware links, new sdkconfig
  build command, and how to provision an RFID credential (`SetCredential`
  with `CredentialType = RFID`) plus tap-to-unlock behavior. Extend the
  existing "Diagnostic Console" section with the new `rfid-unlock` command.

## Verification

1. `idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.nanoc6_rfid_nfc"
   set-target esp32c6 build` (and the `nanoh2` / esp32h2 variant) — confirms
   the new component/Kconfig/CMake gating compiles cleanly, and lets us
   inspect the vendored `M5Unit-NFC` component's actual Kconfig symbol names
   before finalizing pin config.
2. Also build the default (no RFID, no Aliro) and the existing
   `..._aliro_nfc` variants unchanged, to confirm no regression from the
   `HandleBoltStateChange`/`SetLockState` signature change shared with PIN.
3. On real M5Stack NanoC6/H2 + Unit NFC hardware: commission the device,
   `SetCredential` an RFID UID via a Matter controller (or the new
   `rfid-unlock` console command with a known UID for bench testing without
   a controller), tap the physical tag, confirm the bolt unlocks and a
   `LockOperation` event is emitted with `CredentialType = RFID` and the
   correct `CredentialIndex` (subscribe to the event via `chip-tool` or the
   console).
4. `matter esp dl status` shows correct RFID occupied/available counts.
