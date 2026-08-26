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
import json
import re
import logging

from utils.conversion_utils import is_hex_value
from utils.overrides import normalize_element_name

logger = logging.getLogger(__name__)


def _chip_name_word(word):
    """Title-case a single word using CHIP's convention.

    All-caps acronyms and plain lowercase words are title-cased to match CHIP
    (AV -> Av, LAN -> Lan, tls -> Tls), while mixed-case acronyms are preserved as-is
    (WebRTC -> WebRTC, WiFi -> WiFi).
    """
    if word.isupper() or word.islower():
        return word.capitalize()
    return word[0].upper() + word[1:]


def chip_name(name):
    """Convert a name to as per the chip naming convention e.g. On/Off -> OnOff

    Args:
        name: The name to convert
    Returns:
        The converted name
    """
    if re.match(r"^[A-Z][a-zA-Z0-9]+$", name):
        return name
    name = re.sub(r"[^a-zA-Z0-9]", " ", name)
    words = [_chip_name_word(word) for word in name.split()]
    name = "".join(words)
    name = name.replace("DishWasher", "Dishwasher")
    return name


def esp_name(name):
    """Convert a name to as per the esp matter naming convention e.g. On/Off -> on_off

    Args:
        name: The name to convert
    Returns:
        The converted name
    """
    name = re.sub(r"[^a-zA-Z0-9_]", "_", name)
    return name.lower()


def convert_to_snake_case(name):
    """Convert a name to snake_case. PM2.5 Concentration Measurement -> pm2_5_concentration_measurement

    Args:
        name: The name to convert
    Returns:
        The converted name
    """
    if not name:
        return name
    if name.endswith("Command"):
        name = name[:-7].replace(" ", "_")
    name = normalize_element_name(name)
    name = re.sub(r"\s+", "_", name)
    name = re.sub(r"[\/_|\{\}\(\)\\-]", "_", name)
    name = re.sub(r"([A-Z]+)([A-Z][a-z])", r"\1_\2", name)
    name = re.sub(r"([a-zA-Z])([0-9])", r"\1_\2", name)
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    return name.lower()


def check_valid_id(id):
    """Check if an id is valid.

    Args:
        id: The id to check
    Returns:
        True if the id is valid, False otherwise
    """
    if id is None or id == "":
        return False
    elif not id.startswith("0x"):
        return False
    elif not is_hex_value(id):
        return False
    return True


def safe_get_attr(obj, attr_name, default=None):
    """Safely get an attribute from an object, returning default if attribute doesn't exist

    Args:
        obj: The object to get the attribute from
        attr_name: The name of the attribute to get
        default: The default value to return if the attribute doesn't exist
    Returns:
        The attribute value if it exists, otherwise the default value
    """
    return getattr(obj, attr_name, default) if obj else default


def write_to_file(file_path, data, file_type="default") -> bool:
    """Write data to a file.

    Args:
        file_path: Path to the file where the data will be written.
        data: The data to write. For ``file_type="json"`` this must be a
              JSON-serializable object; otherwise it must be a string.
        file_type: ``"json"`` to ``json.dump()`` the data with indent=4,
                   anything else to write the raw string.

    Returns:
        ``True`` on success.

    Raises:
        Exception: Re-raised wrapping the underlying I/O error on failure.
    """
    try:
        with open(file_path, "w") as f:
            if file_type == "json":
                json.dump(data, f, indent=4)
            else:
                f.write(data)
        return True
    except Exception as e:
        raise Exception("Error writing to file") from e
