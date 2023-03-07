# Copyright 2022 The Pigweed Authors
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
"""A channel adapter layer that introduces lossy behavior to a channel."""

import abc
import collections
import copy
import logging
from typing import Callable, Deque, Optional, Tuple
import random
import time

import pw_rpc

_LOG = logging.getLogger(__name__)


# TODO(amontanez): The surface are of this API could be reduced significantly
# with a few changes to LossyChannel.
class LossController(abc.ABC):
    """Interface for driving loss/corruption decisions of a LossyChannel."""

    @abc.abstractmethod
    def next_packet_duplicated(self) -> bool:
        """Returns true if the next packet should be duplicated."""

    @abc.abstractmethod
    def next_packet_out_of_order(self) -> bool:
        """Returns true if the next packet should be a re-ordered packet."""

    @abc.abstractmethod
    def next_packet_delayed(self) -> bool:
        """Returns true if a delay should occur before sending a packet."""

    @abc.abstractmethod
    def next_packet_dropped(self) -> bool:
        """Returns true if the next incoming packet should be dropped."""

    @abc.abstractmethod
    def next_packet_delay(self) -> int:
        """The delay before sending the next packet, in milliseconds."""

    @abc.abstractmethod
    def next_num_dupes(self) -> int:
        """Returns how many times the next packet should be duplicated."""

    @abc.abstractmethod
    def choose_out_of_order_packet(self, max_idx) -> int:
        """Returns the index of the next reordered packet.

        A return value of 0 represents the newest packet, while max_idx
        represents the oldest packet.
        """


class ManualPacketFilter(LossController):
    """Determines if a packet should be kept or dropped for testing purposes."""

    _Action = Callable[[int], Tuple[bool, bool]]
    _KEEP = lambda _: (True, False)
    _DROP = lambda _: (False, False)

    def __init__(self) -> None:
        self.packet_count = 0
        self._actions: Deque[ManualPacketFilter._Action] = collections.deque()

    def reset(self) -> None:
        self.packet_count = 0
        self._actions.clear()

    def keep(self, count: int) -> None:
        """Keeps the next count packets."""
        self._actions.extend(ManualPacketFilter._KEEP for _ in range(count))

    def drop(self, count: int) -> None:
        """Drops the next count packets."""
        self._actions.extend(ManualPacketFilter._DROP for _ in range(count))

    def drop_every(self, every: int) -> None:
        """Drops every Nth packet forever."""
        self._actions.append(lambda count: (count % every != 0, True))

    def randomly_drop(self, one_in: int, gen: random.Random) -> None:
        """Drops packets randomly forever."""
        self._actions.append(lambda _: (gen.randrange(one_in) != 0, True))

    def keep_packet(self) -> bool:
        """Returns whether the provided packet should be kept or dropped."""
        self.packet_count += 1

        if not self._actions:
            return True

        keep, repeat = self._actions[0](self.packet_count)

        if not repeat:
            self._actions.popleft()

        return keep

    def next_packet_duplicated(self) -> bool:
        return False

    @staticmethod
    def next_packet_out_of_order() -> bool:
        return False

    @staticmethod
    def next_packet_delayed() -> bool:
        return False

    def next_packet_dropped(self) -> bool:
        return not self.keep_packet()

    @staticmethod
    def next_packet_delay() -> int:
        return 0

    @staticmethod
    def next_num_dupes() -> int:
        return 0

    @staticmethod
    def choose_out_of_order_packet(max_idx) -> int:
        return 0


class RandomLossGenerator(LossController):
    """Parametrized random number generator that drives a LossyChannel."""

    def __init__(
        self,
        duplicated_packet_probability: float,
        max_duplications_per_packet: int,
        out_of_order_probability: float,
        delayed_packet_probability: float,
        delayed_packet_range_ms: Tuple[int, int],
        dropped_packet_probability: float,
        seed: Optional[int] = None,
    ):
        self.duplicated_packet_probability = duplicated_packet_probability
        self.max_duplications_per_packet = max_duplications_per_packet
        self.out_of_order_probability = out_of_order_probability
        self.delayed_packet_probability = delayed_packet_probability
        self.delayed_packet_range_ms = delayed_packet_range_ms
        self.dropped_packet_probability = dropped_packet_probability
        self._rng = random.Random(seed)

    def next_packet_duplicated(self) -> bool:
        return self.duplicated_packet_probability > self._rng.uniform(0.0, 1.0)

    def next_packet_out_of_order(self) -> bool:
        return self.out_of_order_probability > self._rng.uniform(0.0, 1.0)

    def next_packet_delayed(self) -> bool:
        return self.delayed_packet_probability > self._rng.uniform(0.0, 1.0)

    def next_packet_dropped(self) -> bool:
        return self.dropped_packet_probability > self._rng.uniform(0.0, 1.0)

    def next_packet_delay(self) -> int:
        return self._rng.randint(*self.delayed_packet_range_ms)

    def next_num_dupes(self) -> int:
        return self._rng.randint(1, self.max_duplications_per_packet)

    def choose_out_of_order_packet(self, max_idx) -> int:
        return self._rng.randint(0, max_idx)


class LossyChannel(pw_rpc.ChannelManipulator):
    """Introduces lossy behaviors into a channel."""

    class _Packet:
        """Container class to keep track of incoming packet sequence number."""

        def __init__(self, sequence_number: int, payload: bytes):
            self.sequence_number = sequence_number
            self.payload = payload

    def __init__(
        self, name, loss_generator: LossController, max_num_old_packets=24
    ):
        super().__init__()
        self.name = name
        self._packets: Deque[LossyChannel._Packet] = collections.deque()
        self._old_packets: Deque[LossyChannel._Packet] = collections.deque()
        self._max_old_packet_window_size = max_num_old_packets
        self.unique_packet_count = 0
        self._rng = loss_generator

    def _enqueue_old_packet(self, packet: _Packet):
        if len(self._old_packets) >= self._max_old_packet_window_size:
            self._old_packets.popleft()
        self._old_packets.append(packet)

    def _enqueue_packet(self, payload: bytes):
        # Generate duplicate packets on ingress.
        packet = self._Packet(self.unique_packet_count, payload)
        self.unique_packet_count += 1

        self._packets.append(packet)
        self._enqueue_old_packet(packet)
        if self._rng.next_packet_duplicated():
            num_dupes = self._rng.next_num_dupes()
            _LOG.debug('[%s] Duplicating packet %d times', self.name, num_dupes)
            for _ in range(num_dupes):
                self._packets.append(packet)

    def _send_packets(self):
        while self._packets:
            packet = None

            if self._rng.next_packet_out_of_order():
                idx = self._rng.choose_out_of_order_packet(
                    len(self._old_packets) - 1
                )
                _LOG.debug(
                    '[%s] Selecting out of order packet at index %d',
                    self.name,
                    idx,
                )
                packet = copy.copy(self._old_packets[idx])
                del self._old_packets[idx]
            else:
                packet = self._packets.popleft()

            if self._rng.next_packet_delayed():
                delay = self._rng.next_packet_delay()
                _LOG.debug('[%s] Delaying channel by %d ms', self.name, delay)
                time.sleep(delay / 1000)

            action_msg = 'Dropped'
            if not self._rng.next_packet_dropped():
                action_msg = 'Sent'
                self.send_packet(packet.payload)
            _LOG.debug(
                '[%s] %s packet #%d: %s',
                self.name,
                action_msg,
                packet.sequence_number,
                str(packet.payload),
            )

    def process_and_send(self, packet: bytes):
        self._enqueue_packet(packet)
        self._send_packets()
