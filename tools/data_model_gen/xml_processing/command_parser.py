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
from .elements import Cluster, Command
from utils.helper import safe_get_attr
from typing import List

logger = logging.getLogger(__name__)


class CommandParser(ClusterElementBaseParser):
    """Parses cluster commands from XML."""

    def __init__(
        self,
        cluster: Cluster,
        feature_map: dict,
        allowed_commands_ids: list = None,
        base_commands: List[Command] = None,
    ):
        super().__init__(
            cluster, feature_map, allowed_commands_ids or [], base_commands or []
        )

    def parse(self, root) -> None:
        """Parse commands from cluster XML root and add to cluster.commands. Merges base_commands if provided."""
        logger.debug(
            f"Parsing commands for cluster {safe_get_attr(self.cluster, 'name')}"
        )
        for elem in root.findall("commands/command"):
            skip, reason = self.can_skip(elem)
            if skip:
                logger.debug("Skipping command %s: %s", elem.get("name"), reason)
                continue
            cmd = self.create(elem)
            self._set_access(cmd, elem)
            cmd.conformance = parse_conformance(elem, self.feature_map)
            self._set_fields(cmd, elem)
            self.cluster.commands.add(cmd)
            if self.cluster.skip_command_cb:
                cmd.skip_command_cb = True

        for base_cmd in self.base_elements:
            if base_cmd.name not in self.processed:
                if base_cmd.conformance is not None:
                    base_cmd.conformance.feature_map = self.feature_map
                    if base_cmd.conformance.is_disallowed():
                        continue
                self.cluster.commands.add(base_cmd)

    def create(self, elem: Element) -> Command:
        name = elem.get("name")
        cmd = Command(
            id=elem.get("id"),
            name=name,
            direction=elem.get("direction"),
            response=elem.get("response"),
            is_mandatory=is_mandatory(elem),
        )
        if safe_get_attr(self.cluster, "command_handler_available") or safe_get_attr(
            self.cluster, "is_migrated_cluster"
        ):
            cmd.command_handler_available = True
        return cmd

    def _set_access(self, cmd: Command, elem: Element) -> None:
        access_elem = elem.find("access")
        if access_elem is None:
            return
        cmd.set_access(
            Command.CommandAccess(
                invokePrivilege=access_elem.get("invokePrivilege"),
                timed=access_elem.get("timed") == "true",
                fabric_scoped=access_elem.get("fabricScoped") == "true",
            )
        )

    def _set_fields(self, cmd: Command, elem: Element) -> None:
        for field_elem in elem.findall("field"):
            fid, fname, ftype = (
                field_elem.get("id"),
                field_elem.get("name"),
                field_elem.get("type"),
            )
            if not fid or not fname or not ftype:
                continue
            constraint = self._field_constraint(field_elem.find("constraint"))
            cmd.add_field(
                Command.CommandField(
                    id=fid,
                    name=fname,
                    type_=ftype,
                    default_value=field_elem.get("default"),
                    is_mandatory=field_elem.find("mandatoryConform") is not None,
                    constraint=constraint,
                )
            )

    def _field_constraint(self, constraint_elem: Element):
        if constraint_elem is None:
            return None
        out = {}
        for child in constraint_elem:
            if child.tag == "maxLength":
                out["type"], out["value"] = "maxLength", child.get("value")
            elif child.tag == "min":
                out["type"], out["value"] = "min", child.get("value")
            elif child.tag == "max":
                out["type"], out["value"] = "max", child.get("value")
            elif child.tag == "between":
                out["type"] = "between"
                from_el, to_el = child.find("from"), child.find("to")
                out["min"] = from_el.get("value", "0") if from_el is not None else "0"
                out["max"] = to_el.get("value", "0") if to_el is not None else "0"
            elif child.tag == "desc":
                out["type"], out["value"] = "desc", None
        return out if out else None
