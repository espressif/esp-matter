# Copyright 2026 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import logging
from xml.etree.ElementTree import Element

from .conformance_parser import parse_conformance, is_mandatory
from .element_parser_base import ClusterElementBaseParser
from .elements import Event
from utils.helper import safe_get_attr
from typing import List

logger = logging.getLogger(__name__)


class EventParser(ClusterElementBaseParser):
    """Parses cluster events from XML."""

    def __init__(
        self,
        cluster,
        feature_map,
        allowed_events_ids: list = None,
        base_events: List[Event] = None,
    ):
        super().__init__(
            cluster, feature_map, allowed_events_ids or [], base_events or []
        )

    def parse(self, root) -> None:
        """Parse events from cluster XML root and add to cluster.events. Merges base_events if provided."""
        logger.debug(
            f"Parsing events for cluster {safe_get_attr(self.cluster, 'name')}"
        )
        for elem in root.findall("events/event"):
            skip, reason = self.can_skip(elem)
            if skip:
                logger.debug("Skipping event %s: %s", elem.get("name"), reason)
                continue
            evt = self.create(elem)
            self.cluster.events.add(evt)

        for base_evt in self.base_elements:
            if base_evt.name not in self.processed:
                if base_evt.conformance is not None:
                    base_evt.conformance.feature_map = self.feature_map
                    if base_evt.conformance.is_disallowed():
                        continue
                self.cluster.events.add(base_evt)

    def create(self, elem: Element) -> Event:
        event = Event(
            id=elem.get("id"),
            name=elem.get("name"),
            is_mandatory=is_mandatory(elem),
        )
        event.conformance = parse_conformance(elem, self.feature_map)
        return event
