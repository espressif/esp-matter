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
Tests for xml_processing parsers:
  - element_parser_base.py  (ClusterElementBaseParser)
  - attribute_parser.py     (AttributeParser)
  - command_parser.py       (CommandParser)
  - event_parser.py         (EventParser)
  - feature_parser.py       (FeatureParser)
  - parse_context.py        (ClusterParseContext)
"""

import unittest
import sys
import os
from xml.etree.ElementTree import Element, SubElement
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from xml_processing.elements import Cluster, Attribute, Command, Event, Feature  # noqa: E402
from xml_processing.element_parser_base import ClusterElementBaseParser  # noqa: E402
from xml_processing.attribute_parser import AttributeParser  # noqa: E402
from xml_processing.command_parser import CommandParser  # noqa: E402
from xml_processing.event_parser import EventParser  # noqa: E402
from xml_processing.feature_parser import FeatureParser  # noqa: E402
from xml_processing.parse_context import ClusterParseContext  # noqa: E402


# ---------------------------------------------------------------------------
# Helpers to build minimal XML trees
# ---------------------------------------------------------------------------


def _make_cluster(name="TestCluster", id_="0x0001", revision=1):
    c = Cluster(name=name, id=id_, revision=revision)
    c.attribute_types = {
        "uint8": "uint8",
        "bool": "bool",
        "int16": "int16",
        "uint16": "uint16",
    }
    c.data_types = {}
    c.skip_command_cb = False
    c.command_handler_available = False
    c.is_migrated_cluster = False
    return c


def _attr_xml(name, id_, type_="uint8", default="0", mandatory=False):
    elem = Element("attribute", name=name, id=id_, type=type_, default=default)
    if mandatory:
        SubElement(elem, "mandatoryConform")
    return elem


def _cmd_xml(name, id_, direction="commandToServer", response="Y", mandatory=False):
    elem = Element("command", name=name, id=id_, direction=direction, response=response)
    if mandatory:
        SubElement(elem, "mandatoryConform")
    return elem


def _event_xml(name, id_, mandatory=False):
    elem = Element("event", name=name, id=id_)
    if mandatory:
        SubElement(elem, "mandatoryConform")
    return elem


def _cluster_root_with_attrs(*attrs):
    root = Element("cluster", name="Test", id="0x0001", revision="1")
    attrs_section = SubElement(root, "attributes")
    for a in attrs:
        attrs_section.append(a)
    return root


def _cluster_root_with_cmds(*cmds):
    root = Element("cluster", name="Test", id="0x0001", revision="1")
    cmds_section = SubElement(root, "commands")
    for c in cmds:
        cmds_section.append(c)
    return root


def _cluster_root_with_events(*events):
    root = Element("cluster", name="Test", id="0x0001", revision="1")
    events_section = SubElement(root, "events")
    for e in events:
        events_section.append(e)
    return root


# ---------------------------------------------------------------------------
# ClusterElementBaseParser
# ---------------------------------------------------------------------------


class _ConcreteParser(ClusterElementBaseParser):
    """Concrete subclass for testing the abstract base."""

    def parse(self, root):
        pass


class TestClusterElementBaseParser(unittest.TestCase):
    """Test the shared skip / ID logic in ClusterElementBaseParser."""

    def _parser(self, cluster=None, feature_map=None, allowed_ids=None, base=None):
        c = cluster or _make_cluster()
        p = _ConcreteParser(c, feature_map or {}, allowed_ids or [], base or [])
        return p

    def test_skip_missing_name(self):
        p = self._parser()
        elem = Element("attribute", id="0x0001")
        skip, reason = p.can_skip(elem)
        self.assertTrue(skip)
        self.assertIn("missing name", reason)

    def test_skip_duplicate_name(self):
        p = self._parser()
        elem = Element("attribute", name="Temp", id="0x0001")
        p.can_skip(elem)
        skip, reason = p.can_skip(Element("attribute", name="Temp", id="0x0001"))
        self.assertTrue(skip)
        self.assertIn("already processed", reason)

    def test_skip_invalid_id(self):
        p = self._parser()
        elem = Element("attribute", name="Bad", id="INVALID")
        skip, reason = p.can_skip(elem)
        self.assertTrue(skip)
        self.assertIn("invalid id", reason)

    def test_skip_no_id(self):
        p = self._parser()
        elem = Element("attribute", name="NoId")
        skip, _ = p.can_skip(elem)
        self.assertTrue(skip)

    def test_skip_id_not_in_allowed(self):
        p = self._parser(allowed_ids=[1, 2, 3])
        elem = Element("attribute", name="X", id="0x00FF")
        skip, reason = p.can_skip(elem)
        self.assertTrue(skip)
        self.assertIn("not in allowed", reason)

    def test_no_skip_valid(self):
        p = self._parser()
        elem = Element("attribute", name="Valid", id="0x0001")
        skip, _ = p.can_skip(elem)
        self.assertFalse(skip)

    def test_skip_deprecated_conformance(self):
        p = self._parser()
        elem = Element("attribute", name="Dep", id="0x0001")
        SubElement(elem, "deprecateConform")
        skip, reason = p.can_skip(elem)
        self.assertTrue(skip)
        self.assertIn("conformance", reason)

    def test_fill_from_base_sets_id(self):
        base_attr = Attribute(
            name="BaseAttr",
            id="0x00AA",
            type_="uint8",
            default_value="0",
            is_mandatory=True,
        )
        p = self._parser(base=[base_attr])
        elem = Element("attribute", name="BaseAttr")
        p._fill_from_base(elem, base_attr)
        self.assertEqual(elem.get("id"), "0x00AA")


# ---------------------------------------------------------------------------
# AttributeParser
# ---------------------------------------------------------------------------


class TestAttributeParser(unittest.TestCase):
    """Test AttributeParser — create, parse, type overrides, internally managed."""

    def _parser(self, cluster=None, managed=None, base=None):
        c = cluster or _make_cluster()
        return AttributeParser(
            c,
            feature_map={},
            managed_attributes=managed or [],
            base_attributes=base or [],
        )

    def test_create_basic(self):
        p = self._parser()
        elem = _attr_xml("Temp", "0x0001", "uint8", "25", mandatory=True)
        attr = p.create(elem)
        self.assertEqual(attr.func_name, "temp")
        self.assertEqual(attr.type, "uint8")
        self.assertTrue(attr.is_mandatory)

    def test_create_with_access(self):
        p = self._parser()
        elem = _attr_xml("Writable", "0x0002", "uint8", "0")
        SubElement(
            elem,
            "access",
            read="true",
            readPrivilege="view",
            write="true",
            writePrivilege="operate",
        )
        attr = p.create(elem)
        self.assertIsNotNone(attr.access)
        self.assertEqual(attr.access.write, "true")

    def test_create_with_quality(self):
        p = self._parser()
        elem = _attr_xml("Nullable", "0x0003", "uint8", "0")
        SubElement(
            elem,
            "quality",
            nullable="true",
            persistence="nonVolatile",
            changeOmitted="false",
            scene="false",
            reportable="false",
        )
        attr = p.create(elem)
        self.assertIsNotNone(attr.quality)
        self.assertTrue(attr.is_nullable)

    def test_create_with_constraint_max_length(self):
        p = self._parser()
        elem = _attr_xml("Str", "0x0004", "uint8", "0")
        constraint_el = SubElement(elem, "constraint")
        SubElement(constraint_el, "maxLength", value="32")
        attr = p.create(elem)
        self.assertIsNotNone(attr.constraint)
        self.assertEqual(attr.constraint.type, "maxLength")
        self.assertEqual(attr.constraint.value, "32")

    def test_create_with_constraint_between(self):
        p = self._parser()
        elem = _attr_xml("Bounded", "0x0005", "uint8", "0")
        constraint_el = SubElement(elem, "constraint")
        between = SubElement(constraint_el, "between")
        SubElement(between, "from", value="10")
        SubElement(between, "to", value="200")
        attr = p.create(elem)
        self.assertEqual(attr.constraint.type, "between")
        self.assertEqual(attr.constraint.from_, "10")
        self.assertEqual(attr.constraint.to_, "200")

    def test_internally_managed_list_type(self):
        p = self._parser()
        elem = _attr_xml("Entries", "0x0006", "list", "0")
        # list type, so resolve_attribute_type needs "list" in type map
        p.cluster.attribute_types["list"] = "list"
        attr = p.create(elem)
        self.assertTrue(attr.internally_managed)

    def test_internally_managed_by_name(self):
        p = self._parser(managed=["temp"])
        elem = _attr_xml("Temp", "0x0007", "uint8", "0")
        attr = p.create(elem)
        self.assertTrue(attr.internally_managed)

    def test_not_internally_managed(self):
        p = self._parser(managed=[])
        elem = _attr_xml("Level", "0x0008", "uint8", "0")
        attr = p.create(elem)
        self.assertFalse(attr.internally_managed)

    def test_parse_adds_to_cluster(self):
        cluster = _make_cluster()
        p = AttributeParser(cluster, {}, [])
        root = _cluster_root_with_attrs(
            _attr_xml("AttrA", "0x0001", "uint8", "0"),
            _attr_xml("AttrB", "0x0002", "uint8", "0"),
        )
        p.parse(root)
        self.assertEqual(len(cluster.attributes), 2)

    def test_parse_skips_invalid_id(self):
        cluster = _make_cluster()
        p = AttributeParser(cluster, {}, [])
        root = _cluster_root_with_attrs(
            _attr_xml("Good", "0x0001", "uint8", "0"),
            _attr_xml("Bad", "INVALID", "uint8", "0"),
        )
        p.parse(root)
        self.assertEqual(len(cluster.attributes), 1)

    def test_parse_merges_base_attributes(self):
        cluster = _make_cluster()
        base_attr = Attribute(
            name="BaseOnly",
            id="0x00FF",
            type_="uint8",
            default_value="0",
            is_mandatory=True,
        )
        p = AttributeParser(cluster, {}, [], base_attributes=[base_attr])
        root = _cluster_root_with_attrs(
            _attr_xml("New", "0x0001", "uint8", "0"),
        )
        p.parse(root)
        names = {a.func_name for a in cluster.attributes}
        self.assertIn("base_only", names)

    def test_parse_no_constraint(self):
        p = self._parser()
        attr = p._constraint(None)
        self.assertIsNone(attr)


# ---------------------------------------------------------------------------
# CommandParser
# ---------------------------------------------------------------------------


class TestCommandParser(unittest.TestCase):
    """Test CommandParser — create, parse, fields, access."""

    def _parser(self, cluster=None, base=None):
        c = cluster or _make_cluster()
        return CommandParser(c, feature_map={}, base_commands=base or [])

    def test_create_basic(self):
        p = self._parser()
        elem = _cmd_xml("Off", "0x0001", "commandToServer", "Y", mandatory=True)
        cmd = p.create(elem)
        self.assertEqual(cmd.direction, "commandToServer")
        self.assertEqual(cmd.response, "Y")

    def test_create_with_access(self):
        p = self._parser()
        elem = _cmd_xml("Timed", "0x0002")
        SubElement(
            elem, "access", invokePrivilege="admin", timed="true", fabricScoped="false"
        )
        cmd = p.create(elem)
        p._set_access(cmd, elem)
        self.assertIsNotNone(cmd.access)
        self.assertTrue(cmd.access.timed)
        self.assertFalse(cmd.access.fabric_scoped)

    def test_create_with_fields(self):
        p = self._parser()
        elem = _cmd_xml("Move", "0x0003")
        SubElement(elem, "field", id="0x00", name="level", type="uint8")
        SubElement(elem, "field", id="0x01", name="rate", type="uint8")
        cmd = p.create(elem)
        p._set_fields(cmd, elem)
        self.assertEqual(len(cmd.fields), 2)
        self.assertEqual(cmd.fields[0].name, "level")

    def test_create_field_with_constraint(self):
        p = self._parser()
        elem = _cmd_xml("Bounded", "0x0004")
        field = SubElement(elem, "field", id="0x00", name="val", type="uint8")
        constraint = SubElement(field, "constraint")
        SubElement(constraint, "max", value="100")
        cmd = p.create(elem)
        p._set_fields(cmd, elem)
        self.assertIsNotNone(cmd.fields[0].constraint)
        self.assertEqual(cmd.fields[0].constraint["type"], "max")

    def test_create_skips_incomplete_fields(self):
        p = self._parser()
        elem = _cmd_xml("Partial", "0x0005")
        SubElement(elem, "field", id="0x00", name="ok", type="uint8")
        SubElement(elem, "field", id="0x01", name="missing_type")  # no type
        SubElement(elem, "field", name="no_id", type="uint8")  # no id
        cmd = p.create(elem)
        p._set_fields(cmd, elem)
        self.assertEqual(len(cmd.fields), 1)

    def test_parse_adds_to_cluster(self):
        cluster = _make_cluster()
        p = CommandParser(cluster, {})
        root = _cluster_root_with_cmds(
            _cmd_xml("Off", "0x0001"),
            _cmd_xml("On", "0x0002"),
        )
        p.parse(root)
        self.assertEqual(len(cluster.commands), 2)

    def test_parse_skips_invalid(self):
        cluster = _make_cluster()
        p = CommandParser(cluster, {})
        root = _cluster_root_with_cmds(
            _cmd_xml("Good", "0x0001"),
            _cmd_xml("Bad", "INVALID"),
        )
        p.parse(root)
        self.assertEqual(len(cluster.commands), 1)

    def test_parse_merges_base_commands(self):
        cluster = _make_cluster()
        base_cmd = Command(
            id="0x00FF",
            name="BaseCmd",
            direction="commandToServer",
            response="Y",
            is_mandatory=True,
        )
        p = CommandParser(cluster, {}, base_commands=[base_cmd])
        root = _cluster_root_with_cmds(_cmd_xml("New", "0x0001"))
        p.parse(root)
        names = {c.func_name for c in cluster.commands}
        self.assertIn("base_cmd", names)

    def test_skip_command_cb_flag(self):
        cluster = _make_cluster()
        cluster.skip_command_cb = True
        p = CommandParser(cluster, {})
        root = _cluster_root_with_cmds(_cmd_xml("Flagged", "0x0001"))
        p.parse(root)
        for cmd in cluster.commands:
            self.assertTrue(cmd.skip_command_cb)

    def test_command_handler_available(self):
        cluster = _make_cluster()
        cluster.command_handler_available = True
        p = CommandParser(cluster, {})
        elem = _cmd_xml("Handled", "0x0001")
        cmd = p.create(elem)
        self.assertTrue(cmd.command_handler_available)

    def test_field_constraint_between(self):
        p = self._parser()
        elem = Element("constraint")
        between = SubElement(elem, "between")
        SubElement(between, "from", value="0")
        SubElement(between, "to", value="100")
        result = p._field_constraint(elem)
        self.assertEqual(result["type"], "between")
        self.assertEqual(result["min"], "0")
        self.assertEqual(result["max"], "100")

    def test_field_constraint_none(self):
        p = self._parser()
        self.assertIsNone(p._field_constraint(None))


# ---------------------------------------------------------------------------
# EventParser
# ---------------------------------------------------------------------------


class TestEventParser(unittest.TestCase):
    """Test EventParser — create, parse, base event merging."""

    def test_create_basic(self):
        cluster = _make_cluster()
        p = EventParser(cluster, {})
        elem = _event_xml("StateChange", "0x0001", mandatory=True)
        evt = p.create(elem)
        self.assertEqual(evt.get_id(), "0x0001")
        self.assertTrue(evt.is_mandatory)

    def test_parse_adds_to_cluster(self):
        cluster = _make_cluster()
        p = EventParser(cluster, {})
        root = _cluster_root_with_events(
            _event_xml("EvtA", "0x0001"),
            _event_xml("EvtB", "0x0002"),
        )
        p.parse(root)
        self.assertEqual(len(cluster.events), 2)

    def test_parse_skips_invalid(self):
        cluster = _make_cluster()
        p = EventParser(cluster, {})
        root = _cluster_root_with_events(
            _event_xml("Good", "0x0001"),
            _event_xml("Bad", "INVALID"),
        )
        p.parse(root)
        self.assertEqual(len(cluster.events), 1)

    def test_parse_merges_base_events(self):
        cluster = _make_cluster()
        base_evt = Event(id="0x00FF", name="BaseEvt", is_mandatory=True)
        p = EventParser(cluster, {}, base_events=[base_evt])
        root = _cluster_root_with_events(_event_xml("New", "0x0001"))
        p.parse(root)
        names = {e.func_name for e in cluster.events}
        self.assertIn("base_evt", names)


# ---------------------------------------------------------------------------
# FeatureParser
# ---------------------------------------------------------------------------


class TestFeatureParser(unittest.TestCase):
    """Test FeatureParser — feature map creation, feature ID generation, item linking."""

    def _feature_root(self, *features_data):
        root = Element("cluster", name="Test", id="0x0001", revision="1")
        features_section = SubElement(root, "features")
        for name, code, bit in features_data:
            feat_el = SubElement(
                features_section, "feature", name=name, code=code, bit=str(bit)
            )
            SubElement(feat_el, "optionalConform")
        return root

    def test_feature_map_creation(self):
        root = self._feature_root(("Lighting", "LT", 0), ("ColorTemp", "CT", 1))
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [])
        self.assertIn("LT", p.feature_map)
        self.assertIn("CT", p.feature_map)

    def test_feature_id_generation(self):
        root = self._feature_root(("Lighting", "LT", 0))
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [])
        elem = Element("feature", bit="3")
        fid = p._generate_feature_id(elem)
        self.assertEqual(fid, hex(0x1 << 3))

    def test_feature_id_bit_0(self):
        root = self._feature_root(("Lighting", "LT", 0))
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [])
        elem = Element("feature", bit="0")
        fid = p._generate_feature_id(elem)
        self.assertEqual(fid, "0x1")

    def test_parse_adds_to_cluster(self):
        root = self._feature_root(("Lighting", "LT", 0), ("ColorTemp", "CT", 1))
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [])
        p.parse(root)
        self.assertGreaterEqual(len(cluster.features), 1)

    def test_create_feature(self):
        root = self._feature_root(("Lighting", "LT", 0))
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [])
        elem = Element("feature", name="Lighting", code="LT", bit="0")
        feat = p.create(elem)
        self.assertEqual(feat.code, "LT")

    def test_feature_map_excludes_restricted(self):
        root = Element("cluster", name="Test", id="0x0001", revision="1")
        features_section = SubElement(root, "features")
        feat_el = SubElement(
            features_section, "feature", name="Deprecated", code="DP", bit="0"
        )
        SubElement(feat_el, "deprecateConform")
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [])
        self.assertNotIn("DP", p.feature_map)

    def test_base_features_merged_into_map(self):
        base_feat = Feature(name="BaseFeature", code="BF", id="0x0010")
        root = self._feature_root(("Lighting", "LT", 0))
        cluster = _make_cluster()
        p = FeatureParser(root, cluster, [base_feat])
        self.assertIn("BF", p.feature_map)


# ---------------------------------------------------------------------------
# ClusterParseContext
# ---------------------------------------------------------------------------


class TestClusterParseContext(unittest.TestCase):
    """Test ClusterParseContext — pre-loaded metadata lookups."""

    def _ctx(self, zap=None, managed=None):
        return ClusterParseContext(
            delegate_clusters=[],
            plugin_init_cb_clusters=[],
            migrated_clusters=[],
            zap_filter_list=zap or {},
            internally_managed_attributes=managed or {},
        )

    def test_get_allowed_attributes_empty(self):
        ctx = self._ctx()
        self.assertEqual(ctx.get_allowed_attributes("0x0001"), [])

    def test_get_allowed_attributes(self):
        zap = {
            "clusters": {
                "0x0006": {"Attributes": {"OnOff": "0x0000", "Level": "0x0001"}}
            }
        }
        ctx = self._ctx(zap=zap)
        result = ctx.get_allowed_attributes("0x0006")
        self.assertEqual(len(result), 2)
        self.assertIn(0, result)
        self.assertIn(1, result)

    def test_get_allowed_commands(self):
        zap = {"clusters": {"0x0006": {"Commands": {"Off": "0x0000"}}}}
        ctx = self._ctx(zap=zap)
        result = ctx.get_allowed_commands("0x0006")
        self.assertEqual(result, [0])

    def test_get_allowed_events(self):
        zap = {"clusters": {"0x0006": {"Events": {"StateChange": "0x0000"}}}}
        ctx = self._ctx(zap=zap)
        result = ctx.get_allowed_events("0x0006")
        self.assertEqual(result, [0])

    def test_get_internally_managed_attributes(self):
        ctx = self._ctx(managed={"on_off": ["on_off", "global_scene_control"]})
        result = ctx.get_internally_managed_attributes("on_off")
        self.assertEqual(len(result), 2)
        self.assertIn("on_off", result)

    def test_get_internally_managed_missing_cluster(self):
        ctx = self._ctx(managed={})
        result = ctx.get_internally_managed_attributes("unknown")
        self.assertEqual(result, [])


class TestDerivedClusterInheritance(unittest.TestCase):
    """A derived-cluster element without its own conformance reuses the parsed base element."""

    BASE = """<cluster name="Mode Base">
        <features>
            <feature bit="1" code="COREMODES" name="CoreModes">
                <otherwiseConform><provisionalConform/><optionalConform/></otherwiseConform>
            </feature>
        </features>
        <attributes>
            <attribute id="0x0001" name="CurrentMode" type="uint8">
                <access read="true" readPrivilege="view"/>
                <quality persistence="nonVolatile"/>
                <mandatoryConform/>
            </attribute>
            <attribute id="0x0003" name="OnMode" type="uint8"><optionalConform/></attribute>
        </attributes>
        <commands>
            <command id="0x00" name="ChangeToMode" direction="commandToServer" response="ChangeToModeResponse">
                <mandatoryConform/>
            </command>
        </commands>
    </cluster>"""

    def setUp(self):
        base_root = ET.fromstring(self.BASE)
        base = _make_cluster(name="Mode Base")
        base_features = FeatureParser(base_root, base, [])
        AttributeParser(base, base_features.feature_map, []).parse(base_root)
        CommandParser(base, base_features.feature_map).parse(base_root)
        base_features.parse(base_root)
        self.base = base
        self.derived = _make_cluster(name="Derived Mode")
        self.derived.hierarchy = "derived"

    def _parse_attributes(self, xml):
        AttributeParser(
            self.derived, {}, [], base_attributes=list(self.base.attributes)
        ).parse(ET.fromstring(xml))
        return {a.name: a for a in self.derived.attributes}

    def test_bare_reference_reuses_base_attribute(self):
        attrs = self._parse_attributes(
            '<cluster><attributes><attribute id="0x0001" name="CurrentMode"/></attributes></cluster>'
        )
        current_mode = attrs["CurrentMode"]
        self.assertTrue(current_mode.is_mandatory)
        self.assertEqual(current_mode.quality.persistence, "nonVolatile")

    def test_conformance_only_override_keeps_base_quality(self):
        attrs = self._parse_attributes(
            """<cluster><attributes>
                <attribute id="0x0003" name="OnMode"><mandatoryConform/></attribute>
                <attribute id="0x0001" name="CurrentMode"><mandatoryConform/></attribute>
            </attributes></cluster>"""
        )
        self.assertTrue(attrs["OnMode"].is_mandatory)
        self.assertEqual(attrs["CurrentMode"].quality.persistence, "nonVolatile")

    def test_own_conformance_override_is_kept(self):
        attrs = self._parse_attributes(
            '<cluster><attributes><attribute id="0x0003" name="OnMode"><disallowConform/></attribute></attributes></cluster>'
        )
        self.assertNotIn("OnMode", attrs)

    def test_bare_reference_reuses_base_command(self):
        CommandParser(self.derived, {}, base_commands=list(self.base.commands)).parse(
            ET.fromstring(
                '<cluster><commands><command id="0x00" name="ChangeToMode"/></commands></cluster>'
            )
        )
        cmd = next(iter(self.derived.commands))
        self.assertEqual(cmd.direction, "commandToServer")
        self.assertTrue(cmd.is_mandatory)

    def test_bare_reference_to_dropped_base_feature_is_dropped(self):
        """CoreModes is provisional in the base, so a bare derived CoreModes is dropped too"""
        self.assertEqual(len(self.base.features), 0)
        derived_root = ET.fromstring(
            '<cluster><features><feature bit="1" code="COREMODES" name="CoreModes"/></features></cluster>'
        )
        feature_parser = FeatureParser(
            derived_root, self.derived, list(self.base.features)
        )
        feature_parser.parse(derived_root)
        self.assertNotIn("COREMODES", feature_parser.feature_map)
        self.assertEqual(len(self.derived.features), 0)


if __name__ == "__main__":
    unittest.main()
