# Copyright 2021 The Pigweed Authors
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
"""Simple watchdog class."""

import threading
from typing import Any, Callable, Optional


class Watchdog:
    """Simple class that times out unless reset.

    This class could be used, for example, to track a device's connection state
    for devices that send a periodic heartbeat packet.
    """

    def __init__(
        self,
        on_reset: Callable[[], Any],
        on_expiration: Callable[[], Any],
        while_expired: Callable[[], Any] = lambda: None,
        timeout_s: float = 1,
        expired_timeout_s: Optional[float] = None,
    ):
        """Creates a watchdog; start() must be called to start it.

        Args:
          on_reset: Function called when the watchdog is reset after having
              expired.
          on_expiration: Function called when the timeout expires.
          while_expired: Function called repeatedly while the watchdog is
              expired.
          timeout_s: If reset() is not called for timeout_s, the watchdog
              expires and calls the on_expiration callback.
          expired_timeout_s: While expired, the watchdog calls the
              while_expired callback every expired_timeout_s.
        """
        self._on_reset = on_reset
        self._on_expiration = on_expiration
        self._while_expired = while_expired

        self.timeout_s = timeout_s

        if expired_timeout_s is None:
            self.expired_timeout_s = self.timeout_s * 10
        else:
            self.expired_timeout_s = expired_timeout_s

        self.expired: bool = False
        self._watchdog = threading.Timer(0, self._timeout_expired)

    def start(self) -> None:
        """Starts the watchdog; must be called for the watchdog to work."""
        self._watchdog.cancel()
        self._watchdog = threading.Timer(
            self.expired_timeout_s if self.expired else self.timeout_s,
            self._timeout_expired,
        )
        self._watchdog.daemon = True
        self._watchdog.start()

    def reset(self) -> bool:
        """Resets the timeout; calls the on_reset callback if expired.

        Returns True if was expired.
        """
        if self.expired:
            self.expired = False
            self._on_reset()
            return True

        self.start()
        return False

    def _timeout_expired(self) -> None:
        if self.expired:
            self._while_expired()
        else:
            self.expired = True
            self._on_expiration()

        self.start()
