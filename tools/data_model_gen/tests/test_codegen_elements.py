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

"""
Tests for code_generation/elements.py — Cluster, Attribute, Command, Event, Feature, Device
used during Jinja template rendering.
"""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from code_generation.elements import (  # noqa: E402
    Cluster,
    Attribute,
    Command,
    Event,
    Feature,
    Device,
    get_choice_group,
    get_id_name_lambda,
)
from code_generation.conformance_codegen import Conformance, ConformanceDecision  # noqa: E402


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _make_attr(name, id_, type_="uint8", mandatory=True, default="0"):
    a = Attribute(
        name=name, id=id_, type_=type_, is_mandatory=mandatory, default_value=default
    )
    a.converted_type = "uint8_t"
    a._flag = "ATTRIBUTE_FLAG_NONE"
    return a


def _make_cmd(name, id_, mandatory=True, direction="commandToServer", response="Y"):
    c = Command(
        name=name,
        id=id_,
        is_mandatory=mandatory,
        direction=direction,
        response=response,
    )
    c._flag = "COMMAND_FLAG_ACCEPTED"
    c.has_callback = True
    return c


def _make_event(name, id_, mandatory=True):
    return Event(name=name, id=id_, is_mandatory=mandatory)


def _make_feature(name, id_, code="XX", mandatory=False, conformance=None):
    f = Feature(name=name, id=id_, code=code, is_mandatory=mandatory)
    if conformance:
        f.conformance = conformance
    return f


def _make_cluster(name="TestCluster", id_="0x0001", revision=1):
    return Cluster(name=name, id=id_, revision=revision, is_mandatory=True)


# ---------------------------------------------------------------------------
# Attribute (codegen)
# ---------------------------------------------------------------------------


class TestCodegenAttribute(unittest.TestCase):
    """Test code_generation.elements.Attribute."""

    def test_basic_creation(self):
        a = _make_attr("Temp", "0x0001")
        self.assertEqual(a.get_id(), "0x0001")
        self.assertTrue(a.is_mandatory)

    def test_get_flag(self):
        a = _make_attr("Temp", "0x0001")
        a._flag = "ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE"
        self.assertIn("WRITABLE", a.get_flag())

    def test_get_type(self):
        a = _make_attr("Temp", "0x0001")
        a.converted_type = "int16_t"
        self.assertEqual(a.get_type(), "int16_t")

    def test_get_default_value(self):
        a = _make_attr("Temp", "0x0001", default="42")
        self.assertEqual(a.get_default_value(), "42")

    def test_get_default_value_type_small(self):
        a = _make_attr("Temp", "0x0001", default="10")
        self.assertEqual(a.get_default_value_type(), "uint8_t")

    def test_get_default_value_type_medium(self):
        a = _make_attr("Temp", "0x0001", default="1000")
        self.assertEqual(a.get_default_value_type(), "uint16_t")

    def test_get_default_value_type_large(self):
        a = _make_attr("Temp", "0x0001", default="100000")
        self.assertEqual(a.get_default_value_type(), "uint32_t")

    def test_get_default_value_type_non_numeric(self):
        a = _make_attr("Temp", "0x0001", default="abc")
        self.assertEqual(a.get_default_value_type(), "uint32_t")

    def test_min_max_value(self):
        a = _make_attr("Temp", "0x0001")
        a.min_value = -100
        a.max_value = 200
        self.assertEqual(a.get_min_value(), -100)
        self.assertEqual(a.get_max_value(), 200)

    def test_max_value_falls_back_to_default(self):
        a = _make_attr("Temp", "0x0001", default="50")
        a.max_value = None
        self.assertEqual(a.get_max_value(), "50")

    def test_conformance_condition_default_none(self):
        a = _make_attr("Temp", "0x0001")
        self.assertIsNone(a.get_conformance_condition())

    def test_is_internally_managed_default(self):
        a = _make_attr("Temp", "0x0001")
        self.assertFalse(a.is_internally_managed)

    def test_is_complex_default(self):
        a = _make_attr("Temp", "0x0001")
        self.assertFalse(a.is_complex)

    def test_get_attr_val_expr_bool(self):
        a = _make_attr("OnOff", "0x0000", type_="bool")
        a.converted_type = "bool"
        self.assertEqual(a.get_attr_val_expr("value"), "esp_matter_attr_val(value)")

    def test_get_attr_val_expr_nullable_uint8(self):
        a = _make_attr("Level", "0x0000", type_="uint8")
        a.converted_type = "uint8_t"
        a.is_nullable = True
        self.assertEqual(a.get_attr_val_expr("value"), "esp_matter_attr_val(value)")
        self.assertEqual(
            a.get_attr_val_expr("0"),
            "esp_matter_attr_val(nullable<uint8_t>(0))",
        )

    def test_get_attr_val_expr_uint16_bounds(self):
        a = _make_attr("TargetsPerAccessControlEntry", "0x0000", type_="uint16")
        a.converted_type = "uint16_t"
        self.assertEqual(
            a.get_attr_val_expr("65534"),
            "esp_matter_attr_val(static_cast<uint16_t>(65534))",
        )

    def test_get_attr_val_expr_enum8(self):
        a = _make_attr("Mode", "0x0000", type_="enum8")
        a.converted_type = "uint8_t"
        self.assertEqual(
            a.get_attr_val_expr("value"),
            "esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum)",
        )

    def test_get_attr_val_expr_nullable_enum8_bounds(self):
        a = _make_attr("StartUpOnOff", "0x0000", type_="enum8")
        a.converted_type = "uint8_t"
        a.is_nullable = True
        self.assertEqual(
            a.get_attr_val_expr("0"),
            "esp_matter_attr_val(nullable<uint8_t>(0), "
            "esp_matter_attr_val::uint_sub_type::k_enum)",
        )

    def test_get_attr_val_expr_bitmap32(self):
        a = _make_attr("FeatureMap", "0x0000", type_="bitmap32")
        a.converted_type = "uint32_t"
        self.assertEqual(
            a.get_attr_val_expr("value"),
            "esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap)",
        )

    def test_get_attr_val_expr_string(self):
        a = _make_attr("Name", "0x0000", type_="string")
        a.converted_type = "char *"
        self.assertEqual(
            a.get_attr_val_expr("value"),
            "esp_matter_attr_val(value, length)",
        )

    def test_get_attr_val_expr_list(self):
        a = _make_attr("List", "0x0000", type_="list")
        a.converted_type = "uint8_t *"
        self.assertEqual(
            a.get_attr_val_expr("value"),
            "esp_matter_attr_val(value, length, count)",
        )


# ---------------------------------------------------------------------------
# Command (codegen)
# ---------------------------------------------------------------------------


class TestCodegenCommand(unittest.TestCase):
    """Test code_generation.elements.Command."""

    def test_basic(self):
        c = _make_cmd("Off", "0x0001")
        self.assertEqual(c.get_flag(), "COMMAND_FLAG_ACCEPTED")
        self.assertTrue(c.has_callback)

    def test_conformance_condition_default(self):
        c = _make_cmd("Off", "0x0001")
        self.assertIsNone(c.get_conformance_condition())

    def test_fabric_scoped_default(self):
        c = _make_cmd("Off", "0x0001")
        self.assertFalse(c.is_fabric_scoped)


# ---------------------------------------------------------------------------
# Event (codegen)
# ---------------------------------------------------------------------------


class TestCodegenEvent(unittest.TestCase):
    """Test code_generation.elements.Event."""

    def test_basic(self):
        e = _make_event("StateChange", "0x0001")
        self.assertEqual(e.priority, "Info")

    def test_conformance_condition_default(self):
        e = _make_event("StateChange", "0x0001")
        self.assertIsNone(e.get_conformance_condition())


# ---------------------------------------------------------------------------
# Feature (codegen)
# ---------------------------------------------------------------------------


class TestCodegenFeature(unittest.TestCase):
    """Test code_generation.elements.Feature."""

    def test_basic(self):
        f = _make_feature("Lighting", "0x0001", code="LT")
        self.assertEqual(f.code, "LT")

    def test_get_attributes_sorted(self):
        f = _make_feature("Lighting", "0x0001", code="LT")
        f.attributes = [_make_attr("B", "0x0002"), _make_attr("A", "0x0001")]
        attrs = f.get_attributes()
        self.assertEqual(attrs[0].get_id(), "0x0001")

    def test_get_externally_managed_attributes(self):
        f = _make_feature("Lighting", "0x0001", code="LT")
        a1 = _make_attr("Ext", "0x0001")
        a1.is_internally_managed = False
        a2 = _make_attr("Int", "0x0002")
        a2.is_internally_managed = True
        f.attributes = [a1, a2]
        ext = f.get_externally_managed_attributes()
        self.assertEqual(len(ext), 1)
        self.assertEqual(ext[0].get_id(), "0x0001")

    def test_get_commands_sorted(self):
        f = _make_feature("Lighting", "0x0001", code="LT")
        f.commands = [_make_cmd("B", "0x0002"), _make_cmd("A", "0x0001")]
        cmds = f.get_commands()
        self.assertEqual(cmds[0].get_id(), "0x0001")

    def test_get_events_sorted(self):
        f = _make_feature("Lighting", "0x0001", code="LT")
        f.events = [_make_event("B", "0x0002"), _make_event("A", "0x0001")]
        evts = f.get_events()
        self.assertEqual(evts[0].get_id(), "0x0001")


# ---------------------------------------------------------------------------
# Cluster (codegen)
# ---------------------------------------------------------------------------


class TestCodegenCluster(unittest.TestCase):
    """Test code_generation.elements.Cluster — sorting, mandatory, choice groups."""

    def test_get_attributes_sorted(self):
        c = _make_cluster()
        c.attributes = [_make_attr("B", "0x0002"), _make_attr("A", "0x0001")]
        self.assertEqual(c.get_attributes()[0].get_id(), "0x0001")

    def test_get_commands_sorted(self):
        c = _make_cluster()
        c.commands = [_make_cmd("B", "0x0002"), _make_cmd("A", "0x0001")]
        self.assertEqual(c.get_commands()[0].get_id(), "0x0001")

    def test_get_events_sorted(self):
        c = _make_cluster()
        c.events = [_make_event("B", "0x0002"), _make_event("A", "0x0001")]
        self.assertEqual(c.get_events()[0].get_id(), "0x0001")

    def test_get_features_sorted(self):
        c = _make_cluster()
        c.features = [
            _make_feature("B", "0x0002", code="BB"),
            _make_feature("A", "0x0001", code="AA"),
        ]
        self.assertEqual(c.get_features()[0].get_id(), "0x0001")

    def test_get_mandatory_attributes(self):
        c = _make_cluster()
        a1 = _make_attr("M", "0x0001", mandatory=True)
        a2 = _make_attr("O", "0x0002", mandatory=False)
        c.attributes = [a1, a2]
        mandatory = c.get_mandatory_attributes()
        self.assertEqual(len(mandatory), 1)
        self.assertEqual(mandatory[0].get_id(), "0x0001")

    def test_get_mandatory_commands(self):
        c = _make_cluster()
        c1 = _make_cmd("M", "0x0001", mandatory=True)
        c2 = _make_cmd("O", "0x0002", mandatory=False)
        c.commands = [c1, c2]
        mandatory = c.get_mandatory_commands()
        self.assertEqual(len(mandatory), 1)

    def test_get_mandatory_events(self):
        c = _make_cluster()
        e1 = _make_event("M", "0x0001", mandatory=True)
        e2 = _make_event("O", "0x0002", mandatory=False)
        c.events = [e1, e2]
        mandatory = c.get_mandatory_events()
        self.assertEqual(len(mandatory), 1)

    def test_has_choice_features_false(self):
        c = _make_cluster()
        self.assertFalse(c.has_choice_features())

    def test_get_cluster_init_callback(self):
        c = _make_cluster(name="OnOff")
        cb = c.get_cluster_init_callback()
        self.assertIn("OnOff", cb)
        self.assertIn("Init", cb)

    def test_get_cluster_shutdown_callback(self):
        c = _make_cluster(name="OnOff")
        cb = c.get_cluster_shutdown_callback()
        self.assertIn("OnOff", cb)
        self.assertIn("Shutdown", cb)

    def test_get_response_command(self):
        c = _make_cluster()
        cmd = _make_cmd("GetResponse", "0x0001")
        c.commands = [cmd]
        self.assertEqual(c.get_response_command("GetResponse"), cmd)
        self.assertIsNone(c.get_response_command("Unknown"))

    def test_get_destroyable_elements_empty(self):
        c = _make_cluster()
        c.attributes = [_make_attr("A", "0x0001")]
        result = c.get_destroyable_elements("lighting")
        self.assertEqual(result["attributes"], [])
        self.assertEqual(result["commands"], [])
        self.assertEqual(result["events"], [])

    def test_get_independent_features_all_independent(self):
        c = _make_cluster()
        f1 = _make_feature("A", "0x0001", code="AA")
        f2 = _make_feature("B", "0x0002", code="BB")
        c.features = [f1, f2]
        independent = c.get_independent_features()
        self.assertEqual(len(independent), 2)

    def test_standalone_choice_groups_empty(self):
        c = _make_cluster()
        self.assertEqual(c.get_standalone_choice_groups(), [])


# ---------------------------------------------------------------------------
# Device (codegen)
# ---------------------------------------------------------------------------


class TestCodegenDevice(unittest.TestCase):
    """Test code_generation.elements.Device."""

    def test_basic(self):
        d = Device(id="0x0100", name="On/Off Light", revision=3)
        self.assertEqual(d.get_device_type_id(), "0x0100")
        self.assertEqual(d.get_device_type_version(), 3)

    def test_get_clusters_sorted(self):
        d = Device(id="0x0100", name="Test", revision=1)
        c1 = _make_cluster("B", "0x0006")
        c1.server_cluster = True
        c2 = _make_cluster("A", "0x0003")
        c2.server_cluster = True
        d.clusters = [c1, c2]
        self.assertEqual(d.get_clusters()[0].get_id(), "0x0003")

    def test_get_mandatory_clusters(self):
        d = Device(id="0x0100", name="Test", revision=1)
        c1 = _make_cluster("M", "0x0001")
        c1.is_mandatory = True
        c2 = Cluster(name="O", id="0x0002", revision=1, is_mandatory=False)
        d.clusters = [c1, c2]
        self.assertEqual(len(d.get_mandatory_clusters()), 1)

    def test_get_unique_clusters_deduplicates(self):
        d = Device(id="0x0100", name="Test", revision=1)
        c1 = _make_cluster("Same", "0x0006")
        c1.server_cluster = True
        c2 = _make_cluster("Same", "0x0006")
        c2.server_cluster = False
        d.clusters = [c1, c2]
        unique = d.get_unique_clusters()
        self.assertEqual(len(unique), 1)

    def test_binding_cluster_available(self):
        d = Device(id="0x0100", name="Test", revision=1)
        c = _make_cluster("Test", "0x0006")
        c.client_cluster = True
        c.is_mandatory = True
        d.clusters = [c]
        self.assertTrue(d.binding_cluster_available())

    def test_no_binding_cluster(self):
        d = Device(id="0x0100", name="Test", revision=1)
        c = _make_cluster("Test", "0x0006")
        c.server_cluster = True
        c.client_cluster = False
        c.is_mandatory = True
        d.clusters = [c]
        self.assertFalse(d.binding_cluster_available())

    def test_filename(self):
        d = Device(id="0x0100", name="Temperature Sensor", revision=1)
        self.assertTrue(d.filename.endswith("_device"))


# ---------------------------------------------------------------------------
# Deserializer roundtrip helpers
# ---------------------------------------------------------------------------


class TestGetIdNameLambda(unittest.TestCase):
    """Test sorting helper."""

    def test_sorts_correctly(self):
        items = [
            _make_attr("C", "0x0003"),
            _make_attr("A", "0x0001"),
            _make_attr("B", "0x0002"),
        ]
        sorted_items = sorted(items, key=get_id_name_lambda())
        self.assertEqual(
            [i.get_id() for i in sorted_items], ["0x0001", "0x0002", "0x0003"]
        )


class TestGetChoiceGroup(unittest.TestCase):
    """Test get_choice_group() utility."""

    def test_empty_features(self):
        result = get_choice_group("mandatory_parent", ConformanceDecision.OTHERWISE, [])
        self.assertEqual(result, [])

    def test_no_matching_conformance(self):
        f = _make_feature("A", "0x0001", code="AA")
        result = get_choice_group(
            "mandatory_parent", ConformanceDecision.OTHERWISE, [f]
        )
        self.assertEqual(result, [])


def _make_comparison_conformance(op_key, func_name, literal, nullable=True):
    """Return a codegen Conformance for an otherwise+comparison attribute."""
    return Conformance(
        {
            "type": "otherwise",
            "condition": {
                "mandatory": {
                    op_key: {
                        "attribute": "NumberOfPrimaries",
                        "literal": literal,
                        "func_name": func_name,
                        "nullable": nullable,
                    }
                },
                "optional": True,
            },
        }
    )


class TestComparisonConditionalAttributes(unittest.TestCase):
    """Test Cluster.get_mandatory_attributes() and get_comparison_conditional_attributes()."""

    # The referenced attribute (e.g. NumberOfPrimaries) must be unconditionally mandatory
    # in the cluster for comparison-conditional attributes to be included.
    _REF_ATTR_NAME = "NumberOfPrimaries"
    _REF_ATTR_ID = "0x0010"
    _REF_FUNC_NAME = "number_of_primaries"

    def _make_plain_mandatory_attr(self, name, id_):
        a = _make_attr(name, id_, mandatory=True)
        # Default conformance from Attribute.__init__ is Conformance() → NOT_APPLICABLE
        return a

    def _make_ref_attr(self):
        """Create the unconditionally mandatory referenced attribute (NumberOfPrimaries)."""
        return self._make_plain_mandatory_attr(self._REF_ATTR_NAME, self._REF_ATTR_ID)

    def _make_comparison_attr(self, name, id_, op_key, literal, ref_config_name=None):
        """Create an attribute whose creation is guarded by a runtime comparison.

        ref_config_name is the config_name of the *referenced* attribute (e.g. NumberOfPrimaries),
        not the attribute being created.  All attributes with the same op_key + literal +
        ref_config_name will share the same condition expression and land in the same group.
        """
        if ref_config_name is None:
            ref_config_name = self._REF_FUNC_NAME
        a = _make_attr(name, id_, mandatory=True)
        a.conformance = _make_comparison_conformance(op_key, ref_config_name, literal)
        return a

    def test_get_mandatory_attributes_excludes_comparison_conditional(self):
        """Comparison-conditional attributes must NOT appear in get_mandatory_attributes()."""
        c = _make_cluster()
        plain = self._make_plain_mandatory_attr("Plain", "0x0001")
        ref = self._make_ref_attr()
        conditional = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        c.attributes = [plain, ref, conditional]

        mandatory = c.get_mandatory_attributes()
        names = [a.name for a in mandatory]
        self.assertIn("Plain", names)
        self.assertNotIn("Primary1X", names)

    def test_get_comparison_conditional_attributes_returns_grouped(self):
        """Comparison-conditional attributes appear in get_comparison_conditional_attributes()."""
        c = _make_cluster()
        ref = self._make_ref_attr()
        conditional = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        c.attributes = [ref, conditional]

        groups = c.get_comparison_conditional_attributes()
        self.assertEqual(len(groups), 1)
        _expr, attrs = groups[0]
        self.assertEqual(len(attrs), 1)
        self.assertEqual(attrs[0].name, "Primary1X")

    def test_get_comparison_conditional_attributes_groups_by_condition(self):
        """Attributes sharing the same condition expression are grouped together."""
        c = _make_cluster()
        ref = self._make_ref_attr()
        a1 = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        a2 = self._make_comparison_attr("Primary1Y", "0x0012", "greater", 0)
        a3 = self._make_comparison_attr("Primary2X", "0x0015", "greater", 1)
        c.attributes = [ref, a1, a2, a3]

        groups = c.get_comparison_conditional_attributes()
        # a1/a2 share literal=0, a3 has literal=1 → 2 groups
        self.assertEqual(len(groups), 2)

    def test_get_comparison_conditional_attributes_sorted_by_literal(self):
        """Groups are ordered by ascending literal value."""
        c = _make_cluster()
        ref = self._make_ref_attr()
        a_lit5 = self._make_comparison_attr("Primary6X", "0x0020", "greater", 5)
        a_lit0 = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        c.attributes = [ref, a_lit5, a_lit0]

        groups = c.get_comparison_conditional_attributes()
        self.assertEqual(len(groups), 2)
        _expr0, attrs0 = groups[0]
        _expr5, attrs5 = groups[1]
        self.assertIn("Primary1X", [a.name for a in attrs0])
        self.assertIn("Primary6X", [a.name for a in attrs5])

    def test_get_comparison_conditional_attributes_attrs_sorted_by_id(self):
        """Attributes within a group are sorted by ID."""
        c = _make_cluster()
        ref = self._make_ref_attr()
        a1 = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        a2 = self._make_comparison_attr("Primary1Y", "0x0012", "greater", 0)
        a3 = self._make_comparison_attr("Primary1Intensity", "0x0013", "greater", 0)
        c.attributes = [ref, a3, a1, a2]

        groups = c.get_comparison_conditional_attributes()
        self.assertEqual(len(groups), 1)
        _, attrs = groups[0]
        self.assertEqual([a.get_id() for a in attrs], ["0x0011", "0x0012", "0x0013"])

    def test_condition_expression_string_in_group_key(self):
        """The condition expression string contains the operator and literal."""
        c = _make_cluster()
        ref = self._make_ref_attr()
        conditional = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        c.attributes = [ref, conditional]

        groups = c.get_comparison_conditional_attributes()
        self.assertEqual(len(groups), 1)
        expr, _ = groups[0]
        self.assertIn(">", expr)
        self.assertIn("0", expr)

    def test_skipped_when_referenced_attribute_not_mandatory(self):
        """Comparison-conditional attributes are skipped if the referenced attribute is not mandatory."""
        c = _make_cluster()
        conditional = self._make_comparison_attr("Primary1X", "0x0011", "greater", 0)
        c.attributes = [conditional]
        self.assertEqual(len(c.get_comparison_conditional_attributes()), 0)


def _make_response_conformance(ref_cmd_name):
    """Return a codegen Conformance for a response command: mandatory if ref_cmd_name is present."""
    return Conformance(
        {
            "type": "mandatory",
            "condition": {"command": ref_cmd_name, "flag": "COMMAND_FLAG_ACCEPTED"},
        }
    )


def _make_or_response_conformance(ref_cmd_names):
    """Return a Conformance for a response that is mandatory if ANY listed command is present."""
    return Conformance(
        {
            "type": "mandatory",
            "condition": {
                "or": [
                    {"command": n, "flag": "COMMAND_FLAG_ACCEPTED"}
                    for n in ref_cmd_names
                ]
            },
        }
    )


class TestResponseCommandPromotion(unittest.TestCase):
    """Test Cluster.get_mandatory_commands() promotes response commands."""

    def _make_req_cmd(self, name, id_, mandatory=True):
        c = _make_cmd(name, id_, mandatory=mandatory)
        return c

    def _make_resp_cmd(self, name, id_, ref_cmd_name):
        """Response command: mandatory if ref_cmd_name is present."""
        c = _make_cmd(name, id_, mandatory=True, direction="responseFromServer")
        c.conformance = _make_response_conformance(ref_cmd_name)
        return c

    def test_response_included_when_request_is_mandatory(self):
        """Response command is promoted when its request command is unconditionally mandatory."""
        c = _make_cluster()
        req = self._make_req_cmd("ChangeToMode", "0x0000")
        resp = self._make_resp_cmd("ChangeToModeResponse", "0x0001", "ChangeToMode")
        c.commands = [req, resp]

        mandatory = c.get_mandatory_commands()
        names = [cmd.name for cmd in mandatory]
        self.assertIn("ChangeToMode", names)
        self.assertIn("ChangeToModeResponse", names)

    def test_response_excluded_when_request_is_optional(self):
        """Response command is NOT promoted when its request command is optional."""
        c = _make_cluster()
        req = self._make_req_cmd("CopyScene", "0x0040", mandatory=False)
        resp = self._make_resp_cmd("CopySceneResponse", "0x0040", "CopyScene")
        c.commands = [req, resp]

        mandatory = c.get_mandatory_commands()
        names = [cmd.name for cmd in mandatory]
        self.assertNotIn("CopyScene", names)
        self.assertNotIn("CopySceneResponse", names)

    def test_or_response_included_when_any_request_is_mandatory(self):
        """Response with OR condition is promoted when at least one referenced command is mandatory."""
        c = _make_cluster()
        req_a = self._make_req_cmd("Pause", "0x0000", mandatory=True)
        req_b = self._make_req_cmd("Stop", "0x0001", mandatory=False)
        resp = _make_cmd(
            "OperationalCommandResponse",
            "0x0004",
            mandatory=True,
            direction="responseFromServer",
        )
        resp.conformance = _make_or_response_conformance(["Pause", "Stop"])
        c.commands = [req_a, req_b, resp]

        mandatory = c.get_mandatory_commands()
        names = [cmd.name for cmd in mandatory]
        self.assertIn("Pause", names)
        self.assertIn("OperationalCommandResponse", names)

    def test_or_response_excluded_when_no_request_is_mandatory(self):
        """Response with OR condition is NOT promoted when no referenced command is mandatory."""
        c = _make_cluster()
        req_a = self._make_req_cmd("Pause", "0x0000", mandatory=False)
        req_b = self._make_req_cmd("Stop", "0x0001", mandatory=False)
        resp = _make_cmd(
            "OperationalCommandResponse",
            "0x0004",
            mandatory=True,
            direction="responseFromServer",
        )
        resp.conformance = _make_or_response_conformance(["Pause", "Stop"])
        c.commands = [req_a, req_b, resp]

        mandatory = c.get_mandatory_commands()
        names = [cmd.name for cmd in mandatory]
        self.assertNotIn("OperationalCommandResponse", names)

    def test_unconditionally_mandatory_response_always_included(self):
        """Response commands with unconditional mandatoryConform are unaffected by promotion logic."""
        c = _make_cluster()
        resp = self._make_req_cmd("AddSceneResponse", "0x0000")  # plain mandatory
        c.commands = [resp]

        mandatory = c.get_mandatory_commands()
        self.assertIn("AddSceneResponse", [cmd.name for cmd in mandatory])


if __name__ == "__main__":
    unittest.main()
