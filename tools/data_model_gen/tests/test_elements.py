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

"""Tests for xml_processing/elements.py — Device, Cluster, Attribute, Command, Event, Feature."""

import unittest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from xml_processing.elements import Device, Cluster, Attribute, Command, Event, Feature  # noqa: E402
from xml_processing.conformance_parser import Conformance  # noqa: E402
from utils.conformance import ConformanceDecision  # noqa: E402


class TestAttribute(unittest.TestCase):
    """Test Attribute — flags, default values, type conversion."""

    def _make_attr(self, name="test_attr", type_="uint8", default="0", **kwargs):
        return Attribute(
            name=name,
            id="0x0001",
            type_=type_,
            default_value=default,
            is_mandatory=True,
            **kwargs,
        )

    def test_basic_creation(self):
        attr = self._make_attr()
        self.assertEqual(attr.type, "uint8")
        self.assertEqual(attr.get_type(), "uint8_t")

    def test_flag_none(self):
        attr = self._make_attr()
        self.assertEqual(attr.get_flag(), "ATTRIBUTE_FLAG_NONE")

    def test_flag_writable(self):
        access = Attribute.Access(
            read="true", readPrivilege="view", write="true", writePrivilege="operate"
        )
        attr = self._make_attr(access=access)
        self.assertIn("ATTRIBUTE_FLAG_WRITABLE", attr.get_flag())

    def test_flag_nullable(self):
        quality = Attribute.Quality(
            changeOmitted="false",
            nullable="true",
            scene="false",
            persistence="volatile",
            reportable="false",
        )
        attr = self._make_attr(quality=quality)
        self.assertTrue(attr.is_nullable)
        self.assertIn("ATTRIBUTE_FLAG_NULLABLE", attr.get_flag())

    def test_flag_nonvolatile(self):
        quality = Attribute.Quality(
            changeOmitted="false",
            nullable="false",
            scene="false",
            persistence="nonVolatile",
            reportable="false",
        )
        attr = self._make_attr(quality=quality)
        self.assertIn("ATTRIBUTE_FLAG_NONVOLATILE", attr.get_flag())

    def test_flag_managed_internally(self):
        access = Attribute.Access(
            read="true", readPrivilege="view", write="false", writePrivilege=""
        )
        attr = self._make_attr(access=access)
        attr.internally_managed = True
        self.assertIn("ATTRIBUTE_FLAG_MANAGED_INTERNALLY", attr.get_flag())

    def test_multiple_flags(self):
        access = Attribute.Access(
            read="true", readPrivilege="view", write="true", writePrivilege="operate"
        )
        quality = Attribute.Quality(
            changeOmitted="false",
            nullable="true",
            scene="false",
            persistence="nonVolatile",
            reportable="false",
        )
        attr = self._make_attr(access=access, quality=quality)
        flag = attr.get_flag()
        self.assertIn("ATTRIBUTE_FLAG_WRITABLE", flag)
        self.assertIn("ATTRIBUTE_FLAG_NULLABLE", flag)
        self.assertIn("ATTRIBUTE_FLAG_NONVOLATILE", flag)
        self.assertIn(" | ", flag)

    def test_default_value_uint(self):
        attr = self._make_attr(type_="uint8", default="42")
        self.assertEqual(attr.get_default_value(), 42)

    def test_default_value_bool_true(self):
        attr = self._make_attr(type_="bool", default="true")
        self.assertEqual(attr.get_default_value(), "true")

    def test_default_value_bool_false(self):
        attr = self._make_attr(type_="bool", default="false")
        self.assertEqual(attr.get_default_value(), "false")

    def test_default_value_bool_none(self):
        attr = self._make_attr(type_="bool", default=None)
        self.assertEqual(attr.get_default_value(), "false")

    def test_default_value_enum(self):
        attr = self._make_attr(type_="enum8", default="0x02")
        self.assertEqual(attr.get_default_value(), 2)

    def test_default_value_string_with_constraint(self):
        constraint = Attribute.Constraint(type="maxLength", value="32")
        attr = self._make_attr(type_="string", default=None, constraint=constraint)
        self.assertEqual(attr.get_default_value(), 32)

    def test_default_value_string_no_constraint(self):
        attr = self._make_attr(type_="string", default=None)
        self.assertEqual(attr.get_default_value(), 0)

    def test_default_value_list(self):
        attr = self._make_attr(type_="list", default=None)
        self.assertEqual(attr.get_default_value(), 0)

    def test_default_value_temperature_degrees(self):
        attr = self._make_attr(type_="int16", default="20°C")
        self.assertEqual(attr.get_default_value(), 2000)

    def test_get_default_value_type_small(self):
        attr = self._make_attr(type_="uint8", default="10")
        self.assertEqual(attr.get_default_value_type(), "uint8_t")

    def test_get_default_value_type_medium(self):
        attr = self._make_attr(type_="uint16", default="1000")
        self.assertEqual(attr.get_default_value_type(), "uint16_t")

    def test_get_default_value_type_large(self):
        attr = self._make_attr(type_="uint32", default="100000")
        self.assertEqual(attr.get_default_value_type(), "uint32_t")

    def test_min_max_values(self):
        attr = self._make_attr()
        attr.min_value = 0
        attr.max_value = 254
        self.assertEqual(attr.get_min_value(), 0)
        self.assertEqual(attr.get_max_value(), 254)

    def test_constraint_to_dict(self):
        c = Attribute.Constraint(type="between", from_="0", to_="100")
        d = c.to_dict()
        self.assertEqual(d["type"], "between")
        self.assertEqual(d["min"], "0")
        self.assertEqual(d["max"], "100")


class TestCommand(unittest.TestCase):
    """Test Command — flags, callback logic."""

    def _make_cmd(self, name="TestCmd", direction="commandToServer", response="Y"):
        return Command(
            id="0x0001",
            name=name,
            direction=direction,
            response=response,
            is_mandatory=True,
        )

    def test_flag_accepted(self):
        cmd = self._make_cmd(direction="commandToServer")
        self.assertEqual(cmd.get_flag(), "COMMAND_FLAG_ACCEPTED")

    def test_flag_generated(self):
        cmd = self._make_cmd(direction="responseFromServer")
        self.assertEqual(cmd.get_flag(), "COMMAND_FLAG_GENERATED")

    def test_flag_none(self):
        cmd = self._make_cmd(direction=None)
        self.assertEqual(cmd.get_flag(), "COMMAND_FLAG_NONE")

    def test_callback_required_server_command(self):
        cmd = self._make_cmd(direction="commandToServer", response="Y")
        self.assertTrue(cmd.callback_required())

    def test_callback_not_required_response(self):
        cmd = self._make_cmd(
            name="TestResponse", direction="responseFromServer", response="Y"
        )
        self.assertFalse(cmd.callback_required())

    def test_callback_not_required_no_response(self):
        cmd = self._make_cmd(direction="commandToServer", response="N")
        self.assertFalse(cmd.callback_required())

    def test_callback_not_required_response_name(self):
        cmd = self._make_cmd(
            name="GetStatusResponse", direction="commandToServer", response="Y"
        )
        self.assertFalse(cmd.callback_required())

    def test_callback_skip_by_handler(self):
        cmd = self._make_cmd()
        cmd.command_handler_available = True
        self.assertFalse(cmd.callback_required())

    def test_command_name_strips_suffix(self):
        cmd = Command(
            id="0x0001",
            name="SetTemp Command",
            direction="commandToServer",
            response="Y",
            is_mandatory=True,
        )
        self.assertNotIn(" Command", cmd.name)

    def test_add_field(self):
        cmd = self._make_cmd()
        field = Command.CommandField(id="0x00", name="mode", type_="uint8")
        cmd.add_field(field)
        self.assertEqual(len(cmd.fields), 1)
        self.assertEqual(cmd.fields[0].name, "mode")


class TestEvent(unittest.TestCase):
    """Test Event — basic creation and serialization."""

    def test_creation(self):
        event = Event(id="0x0001", name="StateChange", is_mandatory=True)
        self.assertEqual(event.get_id(), "0x0001")
        self.assertTrue(event.is_mandatory)
        self.assertIsNone(event.conformance)


class TestFeature(unittest.TestCase):
    """Test Feature — attribute/command/event management."""

    def test_creation(self):
        feat = Feature(name="Lighting", code="LT", id="0x0001")
        self.assertEqual(feat.code, "LT")

    def test_add_and_get_attributes(self):
        feat = Feature(name="Lighting", code="LT", id="0x0001")
        attr = Attribute(
            name="OnOff",
            id="0x0001",
            type_="bool",
            default_value="false",
            is_mandatory=True,
        )
        feat.add_attribute_list({attr})
        attrs = feat.get_attributes()
        self.assertEqual(len(attrs), 1)

    def test_add_and_get_commands(self):
        feat = Feature(name="Lighting", code="LT", id="0x0001")
        cmd = Command(
            id="0x0001",
            name="Toggle",
            direction="commandToServer",
            response="Y",
            is_mandatory=True,
        )
        feat.add_command_list({cmd})
        cmds = feat.get_commands()
        self.assertEqual(len(cmds), 1)

    def test_add_and_get_events(self):
        feat = Feature(name="Lighting", code="LT", id="0x0001")
        evt = Event(id="0x0001", name="StateChange", is_mandatory=True)
        feat.add_event_list({evt})
        evts = feat.get_events()
        self.assertEqual(len(evts), 1)

    def test_attributes_sorted_by_id(self):
        feat = Feature(name="Lighting", code="LT", id="0x0001")
        a1 = Attribute(
            name="Attr3",
            id="0x0003",
            type_="uint8",
            default_value="0",
            is_mandatory=True,
        )
        a2 = Attribute(
            name="Attr1",
            id="0x0001",
            type_="uint8",
            default_value="0",
            is_mandatory=True,
        )
        feat.add_attribute_list({a1, a2})
        attrs = feat.get_attributes()
        self.assertEqual(attrs[0].get_id(), "0x0001")
        self.assertEqual(attrs[1].get_id(), "0x0003")


class TestDevice(unittest.TestCase):
    """Test Device — cluster management."""

    def test_creation(self):
        device = Device(id="0x0100", name="On/Off Light", revision=3)
        self.assertIn("on_off_light", device.esp_name)

    def test_get_clusters_empty(self):
        device = Device(id="0x0100", name="Test Device", revision=1)
        self.assertEqual(device.get_clusters(), [])

    def test_get_unique_clusters(self):
        device = Device(id="0x0100", name="Test Device", revision=1)
        c1 = Cluster(name="OnOff", id="0x0006", revision=6)
        c1.server_cluster = True
        c2 = Cluster(name="OnOff", id="0x0006", revision=6)
        c2.server_cluster = False
        device.clusters = {c1, c2}
        unique = device.get_unique_clusters()
        self.assertEqual(len(unique), 1)

    def test_filename(self):
        device = Device(id="0x0100", name="On/Off Light", revision=3)
        self.assertTrue(device.filename.endswith("_device"))


class TestCluster(unittest.TestCase):
    """Test Cluster — callbacks, attributes, commands."""

    def _make_cluster(self, name="OnOff", id="0x0006", revision=6):
        return Cluster(name=name, id=id, revision=revision)

    def test_creation(self):
        c = self._make_cluster()
        self.assertEqual(c.get_revision(), 6)

    def test_callback_functions_empty(self):
        c = self._make_cluster()
        self.assertEqual(c.get_callback_functions(), [])

    def test_callback_functions_init(self):
        c = self._make_cluster()
        c.init_function_available = True
        cbs = c.get_callback_functions()
        self.assertEqual(len(cbs), 1)
        self.assertIn("Init", cbs[0])

    def test_callback_functions_all(self):
        c = self._make_cluster()
        c.init_function_available = True
        c.attribute_changed_function_available = True
        c.shutdown_function_available = True
        c.pre_attribute_change_function_available = True
        cbs = c.get_callback_functions()
        self.assertEqual(len(cbs), 4)

    def test_plugin_server_init_callback(self):
        c = self._make_cluster(name="OnOff")
        c.plugin_init_cb_available = True
        cb = c.get_plugin_server_init_callback()
        self.assertIn("MatterOnOffPluginServerInitCallback", cb)

    def test_plugin_server_init_callback_skip(self):
        c = self._make_cluster(name="ICD Management", id="0x0046")
        c.plugin_init_cb_available = True
        cb = c.get_plugin_server_init_callback()
        self.assertIsNone(cb)

    def test_default_role(self):
        c = self._make_cluster()
        self.assertEqual(c.role, "application")

    def test_add_and_get_attributes(self):
        c = self._make_cluster()
        attr = Attribute(
            name="OnOff",
            id="0x0000",
            type_="bool",
            default_value="false",
            is_mandatory=True,
        )
        c.attributes.add(attr)
        attrs = c.get_attributes()
        self.assertEqual(len(attrs), 1)

    def test_attributes_sorted(self):
        c = self._make_cluster()
        a1 = Attribute(
            name="B", id="0x0002", type_="uint8", default_value="0", is_mandatory=True
        )
        a2 = Attribute(
            name="A", id="0x0001", type_="uint8", default_value="0", is_mandatory=True
        )
        c.attributes = {a1, a2}
        attrs = c.get_attributes()
        self.assertEqual(attrs[0].get_id(), "0x0001")

    def test_add_and_get_commands(self):
        c = self._make_cluster()
        cmd = Command(
            id="0x0001",
            name="Off",
            direction="commandToServer",
            response="Y",
            is_mandatory=True,
        )
        c.commands.add(cmd)
        cmds = c.get_commands()
        self.assertEqual(len(cmds), 1)

    def test_add_and_get_events(self):
        c = self._make_cluster()
        evt = Event(id="0x0001", name="StateChange", is_mandatory=True)
        c.events.add(evt)
        evts = c.get_events()
        self.assertEqual(len(evts), 1)

    def test_add_and_get_features(self):
        c = self._make_cluster()
        feat = Feature(name="Lighting", code="LT", id="0x0001")
        c.features.add(feat)
        feats = c.get_features()
        self.assertEqual(len(feats), 1)


def _make_attribute_comparison_conformance(operator_key):
    """Build an OTHERWISE Conformance with an attribute-comparison mandatory branch."""
    conf = Conformance(feature_map={})
    conf.type = ConformanceDecision.OTHERWISE
    conf.condition = {
        "mandatory": {operator_key: {"attribute": "SomeAttr", "literal": 0}},
        "optional": True,
    }
    return conf


def _make_revision_conformance(operator_key, threshold):
    """Build an OTHERWISE Conformance with a revision-gated mandatory branch."""
    conf = Conformance(feature_map={})
    conf.type = ConformanceDecision.OTHERWISE
    conf.condition = {
        "mandatory": {operator_key: {"revision": True, "literal": threshold}},
        "optional": True,
    }
    return conf


def _make_mandatory_conformance():
    conf = Conformance(feature_map={})
    conf.type = ConformanceDecision.MANDATORY
    conf.condition = None
    return conf


def _make_attr(name, id_, is_mandatory, conformance):
    attr = Attribute(
        name=name, id=id_, type_="uint8", default_value="0", is_mandatory=is_mandatory
    )
    attr.conformance = conformance
    return attr


def _make_cmd(name, id_, is_mandatory, conformance):
    cmd = Command(
        name=name,
        id=id_,
        direction="commandToServer",
        response="Y",
        is_mandatory=is_mandatory,
    )
    cmd.conformance = conformance
    return cmd


def _make_evt(name, id_, is_mandatory, conformance):
    evt = Event(name=name, id=id_, is_mandatory=is_mandatory)
    evt.conformance = conformance
    return evt


class TestClusterRevisionFiltering(unittest.TestCase):
    """get_mandatory_attributes/commands/events must honour is_mandatory_at_revision()."""

    def _cluster(self, revision):
        return Cluster(name="TestCluster", id="0x0001", revision=revision)

    def test_revision_gated_attr_excluded_below_threshold(self):
        c = self._cluster(3)
        c.attributes.add(
            _make_attr(
                "GatedAttr",
                "0x0001",
                True,
                _make_revision_conformance("greater_or_equal", 4),
            )
        )
        self.assertEqual(c.get_mandatory_attributes(), [])

    def test_revision_gated_attr_included_at_or_above_threshold(self):
        for rev in (4, 7):
            with self.subTest(revision=rev):
                c = self._cluster(rev)
                c.attributes.add(
                    _make_attr(
                        "GatedAttr",
                        "0x0001",
                        True,
                        _make_revision_conformance("greater_or_equal", 4),
                    )
                )
                self.assertEqual(len(c.get_mandatory_attributes()), 1)

    def test_unconditional_mandatory_always_included(self):
        c = self._cluster(1)
        c.attributes.add(
            _make_attr("AlwaysAttr", "0x0001", True, _make_mandatory_conformance())
        )
        c.commands.add(
            _make_cmd("AlwaysCmd", "0x0001", True, _make_mandatory_conformance())
        )
        c.events.add(
            _make_evt("AlwaysEvt", "0x0001", True, _make_mandatory_conformance())
        )
        self.assertEqual(len(c.get_mandatory_attributes()), 1)
        self.assertEqual(len(c.get_mandatory_commands()), 1)
        self.assertEqual(len(c.get_mandatory_events()), 1)

    def test_non_mandatory_never_included(self):
        c = self._cluster(10)
        c.attributes.add(
            _make_attr("OptAttr", "0x0001", False, _make_mandatory_conformance())
        )
        self.assertEqual(c.get_mandatory_attributes(), [])

    def test_greater_than_operator_boundary(self):
        """'greater' excludes at boundary, includes above."""
        c_at = self._cluster(4)
        c_at.attributes.add(
            _make_attr(
                "GatedAttr", "0x0001", True, _make_revision_conformance("greater", 4)
            )
        )
        self.assertEqual(c_at.get_mandatory_attributes(), [])

        c_above = self._cluster(5)
        c_above.attributes.add(
            _make_attr(
                "GatedAttr", "0x0001", True, _make_revision_conformance("greater", 4)
            )
        )
        self.assertEqual(len(c_above.get_mandatory_attributes()), 1)

    def test_attribute_comparison_conditional_excluded_from_mandatory(self):
        """OTHERWISE+attribute-comparison excluded from attrs, commands, and events."""
        c = self._cluster(5)
        c.attributes.add(
            _make_attr(
                "GatedAttr",
                "0x0001",
                True,
                _make_attribute_comparison_conformance("greater"),
            )
        )
        c.commands.add(
            _make_cmd(
                "GatedCmd",
                "0x0001",
                True,
                _make_attribute_comparison_conformance("greater"),
            )
        )
        c.events.add(
            _make_evt(
                "GatedEvt",
                "0x0001",
                True,
                _make_attribute_comparison_conformance("greater"),
            )
        )
        self.assertEqual(c.get_mandatory_attributes(), [])
        self.assertEqual(c.get_mandatory_commands(), [])
        self.assertEqual(c.get_mandatory_events(), [])

    def test_result_sorted_by_id(self):
        c = self._cluster(5)
        c.attributes.add(_make_attr("B", "0x0002", True, _make_mandatory_conformance()))
        c.attributes.add(_make_attr("A", "0x0001", True, _make_mandatory_conformance()))
        result = c.get_mandatory_attributes()
        self.assertEqual(result[0].get_id(), "0x0001")
        self.assertEqual(result[1].get_id(), "0x0002")

    def test_commands_and_events_respect_revision_filtering(self):
        """Revision gating works for commands and events (smoke test)."""
        c_below = self._cluster(3)
        c_below.commands.add(
            _make_cmd(
                "Cmd", "0x0001", True, _make_revision_conformance("greater_or_equal", 4)
            )
        )
        c_below.events.add(
            _make_evt(
                "Evt", "0x0001", True, _make_revision_conformance("greater_or_equal", 4)
            )
        )
        self.assertEqual(c_below.get_mandatory_commands(), [])
        self.assertEqual(c_below.get_mandatory_events(), [])

        c_at = self._cluster(4)
        c_at.commands.add(
            _make_cmd(
                "Cmd", "0x0001", True, _make_revision_conformance("greater_or_equal", 4)
            )
        )
        c_at.events.add(
            _make_evt(
                "Evt", "0x0001", True, _make_revision_conformance("greater_or_equal", 4)
            )
        )
        self.assertEqual(len(c_at.get_mandatory_commands()), 1)
        self.assertEqual(len(c_at.get_mandatory_events()), 1)


if __name__ == "__main__":
    unittest.main()
