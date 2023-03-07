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
#pragma once

#include "pw_assert/check.h"
#include "pw_digital_io/internal/conversions.h"
#include "pw_function/function.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw::digital_io {

// The logical state of a digital line.
enum class State : bool {
  kActive = true,
  kInactive = false,
};

// The triggering configuration for an interrupt handler.
enum class InterruptTrigger : int {
  // Trigger on transition from kInactive to kActive.
  kActivatingEdge,
  // Trigger on transition from kActive to kInactive.
  kDeactivatingEdge,
  // Trigger on any state transition between kActive and kInactive.
  kBothEdges,
};

// Interrupt handling function. The argument contains the latest known state of
// the line. It is backend-specific if, when, and how this state is updated.
using InterruptHandler = ::pw::Function<void(State sampled_state)>;

// A digital I/O line that may support input, output, and interrupts, but makes
// no guarantees about whether any operations are supported. You must check the
// various provides_* flags before calling optional methods. Unsupported methods
// invoke PW_CRASH.
//
// All methods are potentially blocking. Unless otherwise specified, access from
// multiple threads to a single line must be externally synchronized - for
// example using pw::Borrowable. Unless otherwise specified, none of the methods
// are safe to call from an interrupt handler. Therefore, this abstraction may
// not be suitable for bitbanging and other low-level uses of GPIO.
//
// Note that the initial state of a line is not guaranteed to be consistent with
// either the "enabled" or "disabled" state. Users of the API who need to ensure
// the line is disabled (ex. output not driving the line) should call Disable.
//
// This class should almost never be used in APIs directly. Instead, use one of
// the derived classes that explicitly supports the functionality that your
// API needs.
//
// This class cannot be extended directly. Instead, extend one of the
// derived classes that explicitly support the functionality that you want to
// implement.
//
class DigitalIoOptional {
 public:
  virtual ~DigitalIoOptional() = default;

  // True if input (getting state) is supported.
  constexpr bool provides_input() const { return config_.input; }
  // True if output (setting state) is supported.
  constexpr bool provides_output() const { return config_.output; }
  // True if interrupt handlers can be registered.
  constexpr bool provides_interrupt() const { return config_.interrupt; }

  // Get the state of the line.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //
  //   OK - an active or inactive state.
  //   FAILED_PRECONDITION - The line has not been enabled.
  //   Other status codes as defined by the backend.
  //
  Result<State> GetState() { return DoGetState(); }

  // Set the state of the line.
  //
  // Callers are responsible to wait for the voltage level to settle after this
  // call returns.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //
  //   OK - the state has been set.
  //   FAILED_PRECONDITION - The line has not been enabled.
  //   Other status codes as defined by the backend.
  //
  Status SetState(State state) { return DoSetState(state); }

  // Check if the line is in the active state.
  //
  // The line is in the active state when GetState() returns State::kActive.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //
  //   OK - true if the line is in the active state, otherwise false.
  //   FAILED_PRECONDITION - The line has not been enabled.
  //   Other status codes as defined by the backend.
  //
  Result<bool> IsStateActive() {
    PW_TRY_ASSIGN(const State state, GetState());
    return state == State::kActive;
  }

  // Sets the line to the active state. Equivalent to SetState(State::kActive).
  //
  // Callers are responsible to wait for the voltage level to settle after this
  // call returns.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //
  //   OK - the state has been set.
  //   FAILED_PRECONDITION - The line has not been enabled.
  //   Other status codes as defined by the backend.
  //
  Status SetStateActive() { return SetState(State::kActive); }

  // Sets the line to the inactive state. Equivalent to
  // SetState(State::kInactive).
  //
  // Callers are responsible to wait for the voltage level to settle after this
  // call returns.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //
  //   OK - the state has been set.
  //   FAILED_PRECONDITION - The line has not been enabled.
  //   Other status codes as defined by the backend.
  //
  Status SetStateInactive() { return SetState(State::kInactive); }

  // Set an interrupt handler to execute when an interrupt is triggered, and
  // Configure the condition for triggering the interrupt.
  //
  // The handler is executed in a backend-specific context - this may be a
  // system interrupt handler or a shared notification thread. Do not do any
  // blocking or expensive work in the handler. The only universally safe
  // operations are the IRQ-safe functions on pw_sync primitives.
  //
  // In particular, it is NOT safe to get the state of a DigitalIo line - either
  // from this line or any other DigitalIoOptional instance - inside the
  // handler.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Precondition: no handler is currently set.
  //
  // Returns:
  //   OK - the interrupt handler was configured.
  //   INVALID_ARGUMENT - handler is empty.
  //   Other status codes as defined by the backend.
  //
  Status SetInterruptHandler(InterruptTrigger trigger,
                             InterruptHandler&& handler) {
    if (handler == nullptr) {
      return Status::InvalidArgument();
    }
    return DoSetInterruptHandler(trigger, std::move(handler));
  }

  // Clear the interrupt handler and disable interrupts if enabled.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //   OK - the itnerrupt handler was cleared.
  //   Other status codes as defined by the backend.
  //
  Status ClearInterruptHandler() {
    return DoSetInterruptHandler(InterruptTrigger::kActivatingEdge, nullptr);
  }

  // Enable interrupts which will trigger the interrupt handler.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Precondition: a handler has been set using SetInterruptHandler.
  //
  // Returns:
  //   OK - the interrupt handler was configured.
  //   FAILED_PRECONDITION - The line has not been enabled.
  //   Other status codes as defined by the backend.
  //
  Status EnableInterruptHandler() { return DoEnableInterruptHandler(true); }

  // Disable the interrupt handler. This is a no-op if interrupts are disabled.
  //
  // This method can be called inside the interrupt handler for this line
  // without any external synchronization. However, the exact behavior is
  // backend-specific. There may be queued events that will trigger the handler
  // again after this call returns.
  //
  // Returns:
  //   OK - the interrupt handler was configured.
  //   Other status codes as defined by the backend.
  //
  Status DisableInterruptHandler() { return DoEnableInterruptHandler(false); }

  // Enable the line to initialize it into the default state as determined by
  // the backend. This may enable pull-up/down resistors, drive the line high or
  // low, etc. The line must be enabled before getting/setting the state
  // or enabling interrupts.
  //
  // Callers are responsible to wait for the voltage level to settle after this
  // call returns.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //   OK - the line is enabled and ready for use.
  //   Other status codes as defined by the backend.
  //
  Status Enable() { return DoEnable(true); }

  // Disable the line to power down any pull-up/down resistors and disconnect
  // from any voltage sources. This is usually done to save power. Interrupt
  // handlers are automatically disabled.
  //
  // This method is not thread-safe and cannot be used in interrupt handlers.
  //
  // Returns:
  //   OK - the line is disabled.
  //   Other status codes as defined by the backend.
  //
  Status Disable() { return DoEnable(false); }

 private:
  friend class DigitalInterrupt;
  friend class DigitalIn;
  friend class DigitalInInterrupt;
  friend class DigitalOut;
  friend class DigitalOutInterrupt;
  friend class DigitalInOut;
  friend class DigitalInOutInterrupt;

  // Private constructor so that only friends can extend us.
  constexpr DigitalIoOptional(internal::Provides config) : config_(config) {}

  // Implemented by derived classes to provide different functionality.
  // See the documentation of the public functions for requirements.
  virtual Status DoEnable(bool enable) = 0;
  virtual Result<State> DoGetState() = 0;
  virtual Status DoSetState(State level) = 0;
  virtual Status DoSetInterruptHandler(InterruptTrigger trigger,
                                       InterruptHandler&& handler) = 0;
  virtual Status DoEnableInterruptHandler(bool enable) = 0;

  // The configuration of this line.
  const internal::Provides config_;
};

// A digital I/O line that supports only interrupts.
//
// The input and output methods are hidden and must not be called.
//
// Use this class in APIs when only interrupt functionality is required.
// Extend this class to implement a line that only supports interrupts.
//
class DigitalInterrupt
    : public DigitalIoOptional,
      public internal::Conversions<DigitalInterrupt, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::SetInterruptHandler;

 protected:
  constexpr DigitalInterrupt()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalInterrupt>()) {}

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;

  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;
  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

  // These overrides invoke PW_CRASH.
  Status DoSetState(State) final;
  Result<State> DoGetState() final;
};

// A digital I/O line that supports only input (getting state).
//
// The output and interrupt methods are hidden and must not be called.
//
// Use this class in APIs when only input functionality is required.
// Extend this class to implement a line that only supports getting state.
//
class DigitalIn : public DigitalIoOptional,
                  public internal::Conversions<DigitalIn, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;

 protected:
  constexpr DigitalIn()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalIn>()) {}

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;

  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::SetInterruptHandler;
  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

  // These overrides invoke PW_CRASH.
  Status DoSetState(State) final;
  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) final;
  Status DoEnableInterruptHandler(bool) final;
};

// An input line that supports interrupts.
//
// The output methods are hidden and must not be called.
//
// Use in APIs when input and interrupt functionality is required.
//
// Extend this class to implement a line that supports input (getting state) and
// listening for interrupts at the same time.
//
class DigitalInInterrupt
    : public DigitalIoOptional,
      public internal::Conversions<DigitalInInterrupt, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;
  using DigitalIoOptional::SetInterruptHandler;

 protected:
  constexpr DigitalInInterrupt()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalInInterrupt>()) {}

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;

  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

  // These overrides invoke PW_CRASH.
  Status DoSetState(State) final;
};

// A digital I/O line that supports only output (setting state).
//
// Input and interrupt functions are hidden and must not be called.
//
// Use in APIs when only output functionality is required.
// Extend this class to implement a line that supports output only.
//
class DigitalOut : public DigitalIoOptional,
                   public internal::Conversions<DigitalOut, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

 protected:
  constexpr DigitalOut()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalOut>()) {}

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;

  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;
  using DigitalIoOptional::SetInterruptHandler;

  // These overrides invoke PW_CRASH.
  Result<State> DoGetState() final;
  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) final;
  Status DoEnableInterruptHandler(bool) final;
};

// A digital I/O line that supports output and interrupts.
//
// Input methods are hidden and must not be called.
//
// Use in APIs when output and interrupt functionality is required. For
// example, to represent a two-way signalling line.
//
// Extend this class to implement a line that supports both output and
// listening for interrupts at the same time.
//
class DigitalOutInterrupt
    : public DigitalIoOptional,
      public internal::Conversions<DigitalOutInterrupt, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::SetInterruptHandler;
  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

 protected:
  constexpr DigitalOutInterrupt()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalOutInterrupt>()) {}

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;

  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;

  // These overrides invoke PW_CRASH.
  Result<State> DoGetState() final;
};

// A digital I/O line that supports both input and output.
//
// Use in APIs when both input and output functionality is required. For
// example, to represent a line which is shared by multiple controllers.
//
// Extend this class to implement a line that supports both input and output at
// the same time.
//
class DigitalInOut
    : public DigitalIoOptional,
      public internal::Conversions<DigitalInOut, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;
  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

 protected:
  constexpr DigitalInOut()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalInOut>()) {}

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;

  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::SetInterruptHandler;

  // These overrides invoke PW_CRASH.
  Status DoSetInterruptHandler(InterruptTrigger, InterruptHandler&&) final;
  Status DoEnableInterruptHandler(bool) final;
};

// A line that supports input, output, and interrupts.
//
// Use in APIs when input, output, and interrupts are required. For example to
// represent a two-way shared line with state transition notifications.
//
// Extend this class to implement a line that supports all the functionality at
// the same time.
//
class DigitalInOutInterrupt
    : public DigitalIoOptional,
      public internal::Conversions<DigitalInOutInterrupt, DigitalIoOptional> {
 public:
  // Available functionality
  using DigitalIoOptional::ClearInterruptHandler;
  using DigitalIoOptional::DisableInterruptHandler;
  using DigitalIoOptional::EnableInterruptHandler;
  using DigitalIoOptional::GetState;
  using DigitalIoOptional::IsStateActive;
  using DigitalIoOptional::SetInterruptHandler;
  using DigitalIoOptional::SetState;
  using DigitalIoOptional::SetStateActive;
  using DigitalIoOptional::SetStateInactive;

 protected:
  constexpr DigitalInOutInterrupt()
      : DigitalIoOptional(internal::AlwaysProvidedBy<DigitalInOutInterrupt>()) {
  }

 private:
  // Unavailable functionality
  using DigitalIoOptional::provides_input;
  using DigitalIoOptional::provides_interrupt;
  using DigitalIoOptional::provides_output;
};

}  // namespace pw::digital_io
