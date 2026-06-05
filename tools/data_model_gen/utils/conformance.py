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
from enum import Enum, auto
from dataclasses import dataclass
from typing import Optional

from utils.exceptions import DataModelGenError

logger = logging.getLogger(__name__)

SUPPORTED_CONFORMANCE_TAGS = {
    "mandatoryConform",
    "optionalConform",
    "otherwiseConform",
    "deprecateConform",
    "disallowConform",
    "provisionalConform",
    "describedConform",
}


class ConformanceException(DataModelGenError):
    """Exception raised when conformance parsing fails"""

    def __init__(
        self, msg, context: Optional[str] = None, suggestion: Optional[str] = None
    ):
        super().__init__(msg, context=context, suggestion=suggestion)


class ConformanceDecision(Enum):
    MANDATORY = auto()
    OPTIONAL = auto()
    OTHERWISE = auto()
    DEPRECATED = auto()
    DISALLOWED = auto()
    PROVISIONAL = auto()
    DESCRIBED = auto()
    NOT_APPLICABLE = auto()

    def to_string(self):
        return self.name.lower()


class ConformanceTAG(Enum):
    FEATURE = "feature"
    ATTRIBUTE = "attribute"
    COMMAND = "command"
    COMMAND_FLAG = "flag"
    EVENT = "event"
    CONDITION = "condition"
    GREATER = "greater"
    GREATER_OR_EQUAL = "greater_or_equal"
    LESS_THAN = "less_than"
    LESS_OR_EQUAL = "less_or_equal"
    EQUAL = "equal"
    NOT_EQUAL = "not_equal"
    LITERAL = "literal"
    REVISION = "revision"
    TRUE = "true"
    FALSE = "false"
    NON = "non"
    NOT = "not"
    AND = "and"
    OR = "or"


def get_conformance_type(conformance_type: str) -> ConformanceDecision:
    if conformance_type == "mandatory" or conformance_type == "mandatoryConform":
        return ConformanceDecision.MANDATORY
    elif conformance_type == "optional" or conformance_type == "optionalConform":
        return ConformanceDecision.OPTIONAL
    elif conformance_type == "otherwise" or conformance_type == "otherwiseConform":
        return ConformanceDecision.OTHERWISE
    elif conformance_type == "deprecated" or conformance_type == "deprecateConform":
        return ConformanceDecision.DEPRECATED
    elif conformance_type == "disallow" or conformance_type == "disallowConform":
        return ConformanceDecision.DISALLOWED
    elif conformance_type == "provisional" or conformance_type == "provisionalConform":
        return ConformanceDecision.PROVISIONAL
    elif conformance_type == "described" or conformance_type == "describedConform":
        return ConformanceDecision.DESCRIBED
    else:
        logger.warning(f"Unknown conformance type: {conformance_type}")
        return ConformanceDecision.NOT_APPLICABLE


@dataclass(frozen=True)
class Choice:
    marker: str = None
    more: bool = False

    def __str__(self):
        marker_str = self.marker if self.marker else ""
        more_str = "+" if self.more else ""
        return marker_str + more_str

    def to_dict(self):
        result = {}
        if self.marker:
            result["choice"] = self.marker
        if self.more:
            result["more"] = self.more
            result["min"] = 1
        return result


@dataclass
class BaseConformance:
    """Base class for conformance."""

    type: ConformanceDecision = None
    choice: Optional[Choice] = None
