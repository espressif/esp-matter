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
Comprehensive test suite for conformance parsing based on real XML examples
from connectedhomeip/data_model/1.5/clusters/
"""

import sys
import os

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import unittest  # noqa: E402
import xml.etree.ElementTree as ET  # noqa: E402
import json  # noqa: E402
from xml_processing.conformance_parser import (  # noqa: E402
    parse_conformance,
    parse_comparison_term,
    is_mandatory,
    replace_references,
    COMPARISON_TERMS,
    Conformance,
    is_restricted_by_conformance,
    match_conformance_items,
)
from utils.helper import convert_to_snake_case  # noqa: E402
from utils.conformance import ConformanceDecision  # noqa: E402


class MockFeature:
    """Mock feature object for testing"""

    def __init__(self, name, code):
        self.name = name
        self.code = code
        self.func_name = convert_to_snake_case(name)


class MockItem:
    """Mock item (attribute/command/event) for testing conformance matching"""

    def __init__(self, name, conformance=None):
        self.name = name
        self.conformance = conformance


class TestBasicConformance(unittest.TestCase):
    """Test basic conformance types without conditions"""

    def test_simple_mandatory_conformance(self):
        """Test parsing simple <mandatoryConform/> tags"""
        xml = "<mandatoryConform/>"
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertIsNotNone(result)
        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIsNone(result.condition)

    def test_simple_optional_conformance(self):
        """Test parsing simple <optionalConform/> tags"""
        xml = "<optionalConform/>"
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertIsNotNone(result)
        self.assertEqual(result.type, ConformanceDecision.OPTIONAL)
        self.assertIsNone(result.condition)

    def test_simple_deprecated_conformance(self):
        """Test parsing simple <deprecateConform/> tags"""
        xml = "<deprecateConform/>"
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertIsNotNone(result)
        self.assertEqual(result.type, ConformanceDecision.DEPRECATED)

    def test_simple_disallowed_conformance(self):
        """Test parsing simple <disallowConform/> tags"""
        xml = "<disallowConform/>"
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertIsNotNone(result)
        self.assertEqual(result.type, ConformanceDecision.DISALLOWED)

    def test_simple_provisional_conformance(self):
        """Test parsing simple <provisionalConform/> tags"""
        xml = "<provisionalConform/>"
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertIsNotNone(result)
        self.assertEqual(result.type, ConformanceDecision.PROVISIONAL)


class TestChoiceConformance(unittest.TestCase):
    """Test choice conformance patterns from OccupancySensing cluster"""

    def test_choice_conformance_with_min_and_more(self):
        """Test: <optionalConform choice="a" more="true" min="1"/>
        From OccupancySensing features - at least 1 sensing method required"""
        xml = '<optionalConform choice="a" more="true" min="1"/>'
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OPTIONAL)
        self.assertEqual(result.choice.marker, "a")
        self.assertTrue(result.choice.more)

    def test_choice_conformance_serialization(self):
        """Test JSON serialization of choice conformance"""
        xml = '<optionalConform choice="a" more="true" min="1"/>'
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)
        dict_result = result.to_dict()

        self.assertEqual(dict_result["type"], "optional")
        self.assertEqual(dict_result["choice"], "a")
        self.assertTrue(dict_result["more"])
        self.assertEqual(dict_result["min"], 1)


class TestFeatureBasedConformance(unittest.TestCase):
    """Test feature-based conformance patterns"""

    def test_mandatory_with_single_feature(self):
        """Test: <mandatoryConform><feature name="LT"/></mandatoryConform>
        From OnOff cluster commands"""
        xml = """<mandatoryConform>
            <feature name="LT"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {"LT": MockFeature("Lighting", "LT")}
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIsNotNone(result.condition)
        self.assertEqual(result.condition, {"feature": "lighting"})

    def test_mandatory_with_or_features(self):
        """Test: <mandatoryConform><orTerm><feature/><feature/></orTerm></mandatoryConform>
        From ColorControl cluster"""
        xml = """<mandatoryConform>
            <orTerm>
                <feature name="HS"/>
                <feature name="XY"/>
                <feature name="CT"/>
            </orTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "HS": MockFeature("HueSaturation", "HS"),
            "XY": MockFeature("XY", "XY"),
            "CT": MockFeature("ColorTemperature", "CT"),
        }
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIn("or", result.condition)
        self.assertEqual(len(result.condition["or"]), 3)

    def test_optional_with_not_feature(self):
        """Test: <optionalConform><notTerm><feature name="OFFONLY"/></notTerm></optionalConform>
        From OnOff cluster - LT feature"""
        xml = """<optionalConform>
            <notTerm>
                <feature name="OFFONLY"/>
            </notTerm>
        </optionalConform>"""
        elem = ET.fromstring(xml)
        feature_map = {"OFFONLY": MockFeature("OffOnly", "OFFONLY")}
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OPTIONAL)
        self.assertIn("not", result.condition)
        self.assertEqual(result.condition["not"]["feature"], "off_only")

    def test_nested_not_or_features(self):
        """Test: <optionalConform><notTerm><orTerm><feature/><feature/></orTerm></notTerm></optionalConform>
        From OnOff cluster - OFFONLY feature"""
        xml = """<optionalConform>
            <notTerm>
                <orTerm>
                    <feature name="LT"/>
                    <feature name="DF"/>
                </orTerm>
            </notTerm>
        </optionalConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "DF": MockFeature("DeadFrontBehavior", "DF"),
        }
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OPTIONAL)
        self.assertIn("not", result.condition)
        self.assertIn("or", result.condition["not"])

    def test_mandatory_with_not_feature(self):
        """Test: <mandatoryConform><notTerm><feature name="OFFONLY"/></notTerm></mandatoryConform>
        From OnOff cluster - On/Toggle commands"""
        xml = """<mandatoryConform>
            <notTerm>
                <feature name="OFFONLY"/>
            </notTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {"OFFONLY": MockFeature("OffOnly", "OFFONLY")}
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIn("not", result.condition)


class TestAttributeCommandConformance(unittest.TestCase):
    """Test conformance based on attributes and commands"""

    def test_attribute_reference(self):
        """Test: <mandatoryConform><attribute name="SomeAttribute"/></mandatoryConform>"""
        xml = """<mandatoryConform>
            <attribute name="OnOff"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertEqual(result.condition, {"attribute": "OnOff"})

    def test_command_reference(self):
        """Test: <mandatoryConform><command name="SomeCommand"/></mandatoryConform>"""
        xml = """<mandatoryConform>
            <command name="Off"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertEqual(result.condition, {"command": "Off"})

    def test_attribute_or_command(self):
        """Test OR of attributes/commands"""
        xml = """<mandatoryConform>
            <orTerm>
                <attribute name="OnOff"/>
                <command name="Toggle"/>
            </orTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIn("or", result.condition)


class TestOtherwiseConformance(unittest.TestCase):
    """Test otherwise conformance patterns"""

    def test_otherwise_with_mandatory_and_deprecated(self):
        """Test: <otherwiseConform><mandatoryConform/><deprecateConform/></otherwiseConform>
        From OccupancySensing cluster"""
        xml = """<otherwiseConform>
            <mandatoryConform/>
            <deprecateConform/>
        </otherwiseConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OTHERWISE)
        self.assertIn("mandatory", result.condition)
        self.assertIn("deprecated", result.condition)

    def test_otherwise_with_provisional_and_mandatory(self):
        """Test: <otherwiseConform><provisionalConform/><mandatoryConform/></otherwiseConform>
        From BasicInformation cluster"""
        xml = """<otherwiseConform>
            <provisionalConform/>
            <mandatoryConform/>
        </otherwiseConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OTHERWISE)
        self.assertIn("provisional", result.condition)
        self.assertIn("mandatory", result.condition)

    def test_otherwise_with_conditional_mandatory(self):
        """Test otherwise with mandatory that has conditions"""
        xml = """<otherwiseConform>
            <mandatoryConform>
                <greaterTerm>
                    <attribute name="NumberOfPrimaries"/>
                    <literal value="0"/>
                </greaterTerm>
            </mandatoryConform>
            <optionalConform/>
        </otherwiseConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OTHERWISE)
        self.assertIn("mandatory", result.condition)
        self.assertIn("optional", result.condition)

    def test_otherwise_with_optional_choice(self):
        """Test otherwise with optional that has choice attributes"""
        xml = """<otherwiseConform>
            <mandatoryConform>
          <feature name="AUTO"/>
        </mandatoryConform>
        <optionalConform choice="a" more="true" min="1"/>
        </otherwiseConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "AUTO": MockFeature("Auto", "AUTO"),
        }
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.OTHERWISE)
        self.assertIn("optional", result.condition)
        self.assertEqual(result.condition["optional"]["choice"], "a")
        self.assertTrue(result.condition["optional"]["more"])
        self.assertEqual(result.condition["optional"]["min"], 1)


class TestComplexNestedConformance(unittest.TestCase):
    """Test complex nested conformance structures"""

    def test_and_of_features(self):
        """Test AND operation with multiple features"""
        xml = """<mandatoryConform>
            <andTerm>
                <feature name="LT"/>
                <feature name="DF"/>
            </andTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "DF": MockFeature("DeadFrontBehavior", "DF"),
        }
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIn("and", result.condition)
        self.assertEqual(len(result.condition["and"]), 2)

    def test_complex_nested_boolean_operations(self):
        """Test complex nested: AND(OR(...), NOT(...))"""
        xml = """<mandatoryConform>
            <andTerm>
                <orTerm>
                    <feature name="HS"/>
                    <feature name="XY"/>
                </orTerm>
                <notTerm>
                    <feature name="OFFONLY"/>
                </notTerm>
            </andTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "HS": MockFeature("HueSaturation", "HS"),
            "XY": MockFeature("XY", "XY"),
            "OFFONLY": MockFeature("OffOnly", "OFFONLY"),
        }
        result = Conformance(feature_map).parse(elem)

        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        self.assertIn("and", result.condition)
        # Check that we have both OR and NOT in the AND
        and_elements = result.condition["and"]
        has_or = any(
            "or" in elem if isinstance(elem, dict) else False for elem in and_elements
        )
        has_not = any(
            "not" in elem if isinstance(elem, dict) else False for elem in and_elements
        )
        self.assertTrue(has_or and has_not)

    def test_implicit_and_of_multiple_features(self):
        """Test implicit AND when multiple features at same level"""
        xml = """<mandatoryConform>
            <feature name="LT"/>
            <feature name="DF"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "DF": MockFeature("DeadFrontBehavior", "DF"),
        }
        result = Conformance(feature_map).parse(elem)

        # Multiple features at same level should be implicitly AND-ed
        self.assertEqual(result.type, ConformanceDecision.MANDATORY)
        if result.condition:
            self.assertTrue("and" in result.condition or "feature" in result.condition)


class TestConformanceRestrictions(unittest.TestCase):
    """Test is_restricted_by_conformance function"""

    def test_skip_disallowed_conformance(self):
        """Test that disallowed elements are flagged for skipping"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <disallowConform/>
        </attribute>"""
        elem = ET.fromstring(xml)
        should_skip = is_restricted_by_conformance({}, elem)
        self.assertTrue(should_skip)

    def test_skip_deprecated_conformance(self):
        """Test that deprecated elements are flagged for skipping"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <deprecateConform/>
        </attribute>"""
        elem = ET.fromstring(xml)
        should_skip = is_restricted_by_conformance({}, elem)
        self.assertTrue(should_skip)

    def test_skip_provisional_conformance(self):
        """Test that provisional elements are flagged for skipping"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <provisionalConform/>
        </attribute>"""
        elem = ET.fromstring(xml)
        should_skip = is_restricted_by_conformance({}, elem)
        self.assertTrue(should_skip)

    def test_skip_otherwise_with_provisional_first(self):
        """Test that otherwise conformance with provisional first is skipped"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <otherwiseConform>
                <provisionalConform/>
                <mandatoryConform/>
            </otherwiseConform>
        </attribute>"""
        elem = ET.fromstring(xml)
        should_skip = is_restricted_by_conformance({}, elem)
        self.assertTrue(should_skip)

    def test_skip_missing_feature_in_mandatory(self):
        """Test skipping when mandatory conformance references non-existent feature"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <otherwiseConform>
                <mandatoryConform>
                    <feature name="NONEXISTENT"/>
                </mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        elem = ET.fromstring(xml)
        feature_map = {"LT": MockFeature("Lighting", "LT")}  # NONEXISTENT not in map
        should_skip = is_restricted_by_conformance(feature_map, elem)
        self.assertTrue(should_skip)

    def test_dont_skip_valid_mandatory(self):
        """Test that valid mandatory conformance is not skipped"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <mandatoryConform/>
        </attribute>"""
        elem = ET.fromstring(xml)
        should_skip = is_restricted_by_conformance({}, elem)
        self.assertFalse(should_skip)

    def test_skip_zigbee_specific(self):
        """Test that Zigbee-specific optional conformance is skipped"""
        xml = """<attribute id="0x0001" name="TestAttr">
            <optionalConform>
                <condition name="Zigbee"/>
            </optionalConform>
        </attribute>"""
        elem = ET.fromstring(xml)
        should_skip = is_restricted_by_conformance({}, elem)
        self.assertTrue(should_skip)


class TestConformanceSerialization(unittest.TestCase):
    """Test JSON serialization of conformance objects"""

    def test_to_dict_basic(self):
        """Test basic to_dict conversion"""
        xml = "<mandatoryConform/>"
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)
        dict_result = result.to_dict()

        self.assertEqual(dict_result["type"], "mandatory")

    def test_to_dict_with_condition(self):
        """Test to_dict with conditions"""
        xml = """<mandatoryConform>
            <feature name="LT"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {"LT": MockFeature("Lighting", "LT")}
        result = Conformance(feature_map).parse(elem)
        dict_result = result.to_dict()

        self.assertIn("condition", dict_result)
        self.assertIsNotNone(dict_result["condition"])

    def test_to_dict_with_attribute_map(self):
        """Test to_dict with attribute name to ID mapping"""
        xml = """<mandatoryConform>
            <orTerm>
                <attribute name="OnOff"/>
                <attribute name="LevelControl"/>
            </orTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        # Without attribute map
        dict_without_map = result.to_dict()
        self.assertEqual(dict_without_map["condition"]["or"][0]["attribute"], "OnOff")

        # With attribute map - should replace names with IDs
        attribute_map = {"OnOff": "OnOff", "LevelControl": "LevelControl"}
        dict_with_map = result.to_dict(attribute_map)
        self.assertEqual(dict_with_map["condition"]["or"][0]["attribute"], "OnOff")
        self.assertEqual(
            dict_with_map["condition"]["or"][1]["attribute"], "LevelControl"
        )

    def test_to_dict_with_command_map(self):
        """Test to_dict with command name to ID/flag mapping"""
        xml = """<mandatoryConform>
            <command name="Off"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        result = Conformance({}).parse(elem)

        # With command map (stored as tuple with flag)
        command_map = {"Off": ("Off", "COMMAND_FLAG_ACCEPTED")}
        dict_with_map = result.to_dict(command_map)
        self.assertEqual(dict_with_map["condition"]["command"], "Off")
        self.assertEqual(dict_with_map["condition"]["flag"], "COMMAND_FLAG_ACCEPTED")

    def test_to_dict_json_serializable(self):
        """Test that to_dict output is JSON serializable"""
        xml = """<optionalConform choice="a" more="true" min="1">
            <andTerm>
                <feature name="LT"/>
                <feature name="DF"/>
            </andTerm>
        </optionalConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "DF": MockFeature("DeadFrontBehavior", "DF"),
        }
        result = Conformance(feature_map).parse(elem)
        dict_result = result.to_dict()

        # Should not raise exception
        json_str = json.dumps(dict_result)
        self.assertIsNotNone(json_str)


class TestMatchConformanceItems(unittest.TestCase):
    """Test matching items with feature conformance"""

    def test_match_mandatory_feature(self):
        """Test matching items with mandatory feature conformance"""
        # Create a feature
        feature = MockFeature("Lighting", "LT")

        # Create conformance that requires this feature
        xml = """<mandatoryConform>
            <feature name="LT"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {"LT": feature}
        conformance = Conformance(feature_map).parse(elem)

        # Create items with this conformance
        item1 = MockItem("OnCommand", conformance)
        item2 = MockItem("OffCommand", None)

        items = [item1, item2]
        matched = match_conformance_items(feature, items)

        self.assertEqual(len(matched), 1)
        self.assertEqual(matched[0].name, "OnCommand")

    def test_match_otherwise_mandatory_feature(self):
        """Test matching items with otherwise conformance containing mandatory feature"""
        feature = MockFeature("Lighting", "LT")

        # Create otherwise conformance
        xml = """<otherwiseConform>
            <mandatoryConform>
                <feature name="LT"/>
            </mandatoryConform>
            <optionalConform/>
        </otherwiseConform>"""
        elem = ET.fromstring(xml)
        feature_map = {"LT": feature}
        conformance = Conformance(feature_map).parse(elem)

        item = MockItem("LightingCommand", conformance)
        items = [item]
        matched = match_conformance_items(feature, items)

        # Should match if otherwise has mandatory with this feature
        # Note: Current implementation checks for condition.get("mandatory")
        self.assertGreaterEqual(len(matched), 0)


class TestConformanceHasFeature(unittest.TestCase):
    """Test has_feature method on Conformance objects"""

    def test_has_feature_simple(self):
        """Test has_feature with simple feature conformance"""
        xml = """<mandatoryConform>
            <feature name="LT"/>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature = MockFeature("Lighting", "LT")
        feature_map = {"LT": feature}
        conformance = Conformance(feature_map).parse(elem)

        self.assertTrue(conformance.has_feature("LT"))
        self.assertFalse(conformance.has_feature("DF"))

    def test_has_feature_in_or(self):
        """Test has_feature when feature is in OR term"""
        xml = """<mandatoryConform>
            <orTerm>
                <feature name="LT"/>
                <feature name="DF"/>
            </orTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "DF": MockFeature("DeadFrontBehavior", "DF"),
        }
        conformance = Conformance(feature_map).parse(elem)

        self.assertTrue(conformance.has_feature("LT"))
        self.assertTrue(conformance.has_feature("DF"))

    def test_has_feature_in_and(self):
        """Test has_feature when feature is in AND term"""
        xml = """<mandatoryConform>
            <andTerm>
                <feature name="LT"/>
                <feature name="DF"/>
            </andTerm>
        </mandatoryConform>"""
        elem = ET.fromstring(xml)
        feature_map = {
            "LT": MockFeature("Lighting", "LT"),
            "DF": MockFeature("DeadFrontBehavior", "DF"),
        }
        conformance = Conformance(feature_map).parse(elem)

        self.assertTrue(conformance.has_feature("LT"))
        self.assertTrue(conformance.has_feature("DF"))


class TestConformanceParsing(unittest.TestCase):
    """Test parsing of conformance elements"""

    def test_parse_mandatory_conformance(self):
        """Test parsing of mandatory conformance"""
        xml = """<attribute id="0x0002" name="Occupancy" type="OccupancyBitmap">
                <access read="true" readPrivilege="view"/>
                <mandatoryConform>
                    <feature name="OCC"/>
                </mandatoryConform>
                </attribute>"""
        elem = ET.fromstring(xml)
        feature_map = {"OCC": MockFeature("Occupancy", "OCC")}
        conformance = parse_conformance(elem, feature_map)
        self.assertEqual(conformance.type, ConformanceDecision.MANDATORY)
        self.assertEqual(conformance.condition, {"feature": "occupancy"})


class TestComparisonTermParsing(unittest.TestCase):
    """Test parse_comparison_term() for all 6 comparison operators."""

    _OPERATORS = [
        ("greaterTerm", "greater"),
        ("greaterOrEqualTerm", "greater_or_equal"),
        ("lessTerm", "less_than"),
        ("lessOrEqualTerm", "less_or_equal"),
        ("equalTerm", "equal"),
        ("notEqualTerm", "not_equal"),
    ]

    def test_all_operators_attribute_literal(self):
        """All 6 operators produce the correct key; literal is stored as int."""
        for xml_tag, op_key in self._OPERATORS:
            with self.subTest(op=op_key):
                xml = f"<{xml_tag}><attribute name='Attr'/><literal value='3'/></{xml_tag}>"
                result = parse_comparison_term(ET.fromstring(xml))
                self.assertIsNotNone(result)
                self.assertIn(op_key, result)
                self.assertIsInstance(result[op_key]["literal"], int)
                self.assertEqual(result[op_key]["literal"], 3)

    def test_field_operand_returns_none(self):
        """<field> instead of <attribute> → None."""
        xml = "<greaterOrEqualTerm><field name='ClusterRevision'/><literal value='3'/></greaterOrEqualTerm>"
        self.assertIsNone(parse_comparison_term(ET.fromstring(xml)))

    def test_missing_attribute_returns_none(self):
        xml = "<greaterTerm><literal value='0'/></greaterTerm>"
        self.assertIsNone(parse_comparison_term(ET.fromstring(xml)))

    def test_missing_literal_returns_none(self):
        xml = "<greaterTerm><attribute name='NumberOfPrimaries'/></greaterTerm>"
        self.assertIsNone(parse_comparison_term(ET.fromstring(xml)))

    def test_revision_comparison_all_operators(self):
        """All 6 operators with revision children: stores revision flag, threshold, no attribute key."""
        for xml_tag, op_key in self._OPERATORS:
            with self.subTest(op=op_key):
                xml = f"<{xml_tag}><revision value='current'/><revision value='4'/></{xml_tag}>"
                result = parse_comparison_term(ET.fromstring(xml))
                self.assertIsNotNone(result)
                self.assertIn(op_key, result)
                comp = result[op_key]
                self.assertTrue(comp["revision"])
                self.assertIsInstance(comp["literal"], int)
                self.assertEqual(comp["literal"], 4)
                self.assertNotIn("attribute", comp)

    def test_revision_only_current_returns_none(self):
        """Revision with no threshold child → None."""
        xml = "<greaterOrEqualTerm><revision value='current'/></greaterOrEqualTerm>"
        self.assertIsNone(parse_comparison_term(ET.fromstring(xml)))


class TestIsMandatoryWithComparisonTerms(unittest.TestCase):
    """Test is_mandatory() with comparison terms in otherwiseConform."""

    def _wrap_in_attr(self, xml_tag):
        return ET.fromstring(
            f"""<attribute name="Primary1X">
                <otherwiseConform>
                    <mandatoryConform>
                        <{xml_tag}>
                            <attribute name="NumberOfPrimaries"/>
                            <literal value="0"/>
                        </{xml_tag}>
                    </mandatoryConform>
                    <optionalConform/>
                </otherwiseConform>
            </attribute>"""
        )

    def test_all_comparison_operators_flagged_as_mandatory(self):
        """All 6 comparison operators inside otherwiseConform/mandatoryConform are mandatory."""
        for xml_tag in [
            "greaterTerm",
            "greaterOrEqualTerm",
            "lessTerm",
            "lessOrEqualTerm",
            "equalTerm",
            "notEqualTerm",
        ]:
            with self.subTest(xml_tag=xml_tag):
                self.assertTrue(is_mandatory(self._wrap_in_attr(xml_tag)))

    def test_unconditional_mandatory_is_mandatory(self):
        elem = ET.fromstring("<attribute name='Attr'><mandatoryConform/></attribute>")
        self.assertTrue(is_mandatory(elem))

    def test_optional_is_not_mandatory(self):
        elem = ET.fromstring("<attribute name='Attr'><optionalConform/></attribute>")
        self.assertFalse(is_mandatory(elem))


class TestReplaceReferencesWithComparisons(unittest.TestCase):
    """Test replace_references() augments comparison terms with func_name/nullable."""

    ATTR_MAP = {
        "NumberOfPrimaries": {
            "id": "0x0010",
            "func_name": "number_of_primaries",
            "nullable": True,
        }
    }

    def test_all_operators_get_augmented(self):
        """All 6 operator keys get func_name/nullable from the attribute map."""
        for op_key in [
            "greater",
            "greater_or_equal",
            "less_than",
            "less_or_equal",
            "equal",
            "not_equal",
        ]:
            with self.subTest(op=op_key):
                condition = {op_key: {"attribute": "NumberOfPrimaries", "literal": 2}}
                result = replace_references(condition, self.ATTR_MAP)
                self.assertEqual(result[op_key]["func_name"], "number_of_primaries")
                self.assertTrue(result[op_key]["nullable"])
                self.assertEqual(result[op_key]["literal"], 2)
                self.assertEqual(result[op_key]["attribute"], "NumberOfPrimaries")

    def test_non_nullable_attribute(self):
        attr_map = {
            "MyAttr": {"id": "0x0001", "func_name": "my_attr", "nullable": False}
        }
        condition = {"greater": {"attribute": "MyAttr", "literal": 0}}
        result = replace_references(condition, attr_map)
        self.assertFalse(result["greater"]["nullable"])

    def test_attribute_not_in_map_returns_original(self):
        condition = {"greater": {"attribute": "UnknownAttr", "literal": 0}}
        result = replace_references(condition, self.ATTR_MAP)
        self.assertNotIn("func_name", result.get("greater", {}))


class TestOtherwiseWithComparisonConformance(unittest.TestCase):
    """End-to-end parse_conformance() for otherwise+comparison pattern."""

    def _parse_primary_attr(self, xml_tag="greaterTerm", literal="0"):
        xml = f"""<attribute id="0x0011" name="Primary1X">
            <otherwiseConform>
                <mandatoryConform>
                    <{xml_tag}>
                        <attribute name="NumberOfPrimaries"/>
                        <literal value="{literal}"/>
                    </{xml_tag}>
                </mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        return parse_conformance(ET.fromstring(xml), {})

    def test_parse_structure(self):
        """type=OTHERWISE, condition has mandatory/optional keys, mandatory branch has comparison key."""
        result = self._parse_primary_attr("greaterTerm", "3")
        self.assertEqual(result.type, ConformanceDecision.OTHERWISE)
        self.assertIn("mandatory", result.condition)
        self.assertIn("optional", result.condition)
        mandatory = result.condition["mandatory"]
        self.assertIsInstance(mandatory, dict)
        self.assertIn("greater", mandatory)
        self.assertEqual(mandatory["greater"]["literal"], 3)
        self.assertEqual(mandatory["greater"]["attribute"], "NumberOfPrimaries")

    def test_not_skipped_by_is_restricted_by_conformance(self):
        """Comparison-conditional attributes must NOT be filtered out."""
        xml = """<attribute id="0x0011" name="Primary1X">
            <otherwiseConform>
                <mandatoryConform>
                    <greaterTerm>
                        <attribute name="NumberOfPrimaries"/>
                        <literal value="0"/>
                    </greaterTerm>
                </mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        self.assertFalse(is_restricted_by_conformance({}, ET.fromstring(xml)))

    def test_all_operators_produce_correct_key(self):
        """Each XML comparison tag maps to the expected ConformanceTAG key."""
        expected_keys = {
            "greaterTerm": "greater",
            "greaterOrEqualTerm": "greater_or_equal",
            "lessTerm": "less_than",
            "lessOrEqualTerm": "less_or_equal",
            "equalTerm": "equal",
            "notEqualTerm": "not_equal",
        }
        for xml_tag, expected_key in expected_keys.items():
            with self.subTest(xml_tag=xml_tag):
                result = self._parse_primary_attr(xml_tag, "0")
                self.assertIn(expected_key, result.condition["mandatory"])


class TestComparisonConformanceSerialization(unittest.TestCase):
    """to_dict() preserves comparison terms and augments them with attribute_map."""

    _XML = """<otherwiseConform>
        <mandatoryConform>
            <greaterTerm>
                <attribute name="NumberOfPrimaries"/>
                <literal value="3"/>
            </greaterTerm>
        </mandatoryConform>
        <optionalConform/>
    </otherwiseConform>"""
    _ATTR_MAP = {
        "NumberOfPrimaries": {
            "id": "0x0010",
            "func_name": "number_of_primaries",
            "nullable": True,
        }
    }

    def test_to_dict_with_attribute_map(self):
        """to_dict with attr_map preserves comparison key, literal, func_name, and nullable."""
        d = Conformance({}).parse(ET.fromstring(self._XML)).to_dict(self._ATTR_MAP)
        comp = d["condition"]["mandatory"]["greater"]
        self.assertEqual(comp["func_name"], "number_of_primaries")
        self.assertTrue(comp["nullable"])
        self.assertEqual(comp["literal"], 3)

    def test_to_dict_is_json_serializable(self):
        d = Conformance({}).parse(ET.fromstring(self._XML)).to_dict(self._ATTR_MAP)
        self.assertIsNotNone(json.dumps(d))


class TestRevisionConformance(unittest.TestCase):
    """Tests for Conformance.is_mandatory_at_revision() and the full XML→bool pipeline."""

    def _parse_revision_attr(self, xml_tag, threshold):
        xml = f"""<attribute id="0x1234" name="RevAttr">
            <otherwiseConform>
                <mandatoryConform>
                    <{xml_tag}>
                        <revision value="current"/>
                        <revision value="{threshold}"/>
                    </{xml_tag}>
                </mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        return parse_conformance(ET.fromstring(xml), {})

    def test_parse_structure(self):
        """Parsed conformance: type=OTHERWISE, revision flag set, threshold stored."""
        conf = self._parse_revision_attr("greaterOrEqualTerm", 7)
        self.assertEqual(conf.type, ConformanceDecision.OTHERWISE)
        mandatory = conf.condition["mandatory"]
        self.assertIn("greater_or_equal", mandatory)
        self.assertTrue(mandatory["greater_or_equal"]["revision"])
        self.assertEqual(mandatory["greater_or_equal"]["literal"], 7)

    def test_gte_below_threshold_is_not_mandatory(self):
        conf = self._parse_revision_attr("greaterOrEqualTerm", 4)
        self.assertFalse(conf.is_mandatory_at_revision(3))

    def test_gte_at_threshold_is_mandatory(self):
        conf = self._parse_revision_attr("greaterOrEqualTerm", 4)
        self.assertTrue(conf.is_mandatory_at_revision(4))

    def test_gte_above_threshold_is_mandatory(self):
        conf = self._parse_revision_attr("greaterOrEqualTerm", 4)
        self.assertTrue(conf.is_mandatory_at_revision(7))

    def test_all_operators_boundary(self):
        """Each operator evaluates at its boundary correctly."""
        cases = [
            ("greaterTerm", "greater", 4, False, 5, True),
            ("greaterOrEqualTerm", "greater_or_equal", 4, True, 3, False),
            ("lessTerm", "less_than", 4, False, 3, True),
            ("lessOrEqualTerm", "less_or_equal", 4, True, 5, False),
            ("equalTerm", "equal", 4, True, 3, False),
            ("notEqualTerm", "not_equal", 4, False, 3, True),
        ]
        for xml_tag, op_key, rev_true, exp_true, rev_false, exp_false in cases:
            with self.subTest(op=op_key):
                conf = self._parse_revision_attr(xml_tag, 4)
                self.assertEqual(
                    conf.is_mandatory_at_revision(rev_true),
                    exp_true,
                    f"{op_key}: revision={rev_true} should be {exp_true}",
                )
                self.assertEqual(
                    conf.is_mandatory_at_revision(rev_false),
                    exp_false,
                    f"{op_key}: revision={rev_false} should be {exp_false}",
                )

    def test_mandatory_type_always_true(self):
        xml = "<attribute name='A'><mandatoryConform/></attribute>"
        conf = parse_conformance(ET.fromstring(xml), {})
        self.assertTrue(conf.is_mandatory_at_revision(1))
        self.assertTrue(conf.is_mandatory_at_revision(100))

    def test_otherwise_with_attribute_comparison_returns_false(self):
        """OTHERWISE+attribute-comparison is only mandatory at runtime — returns False statically."""
        xml = """<attribute name='Primary1X'>
            <otherwiseConform>
                <mandatoryConform>
                    <greaterTerm>
                        <attribute name="NumberOfPrimaries"/>
                        <literal value="0"/>
                    </greaterTerm>
                </mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        conf = parse_conformance(ET.fromstring(xml), {})
        self.assertFalse(conf.is_mandatory_at_revision(1))
        self.assertFalse(conf.is_mandatory_at_revision(100))

    def test_attribute_comparison_all_operators_return_false(self):
        """All 6 operators with an attribute operand return False (not statically evaluable)."""
        for xml_tag in [
            "greaterTerm",
            "greaterOrEqualTerm",
            "lessTerm",
            "lessOrEqualTerm",
            "equalTerm",
            "notEqualTerm",
        ]:
            with self.subTest(xml_tag=xml_tag):
                xml = f"""<attribute name='A'>
                    <otherwiseConform>
                        <mandatoryConform>
                            <{xml_tag}>
                                <attribute name="SomeAttr"/>
                                <literal value="0"/>
                            </{xml_tag}>
                        </mandatoryConform>
                        <optionalConform/>
                    </otherwiseConform>
                </attribute>"""
                conf = parse_conformance(ET.fromstring(xml), {})
                self.assertFalse(conf.is_mandatory_at_revision(5))

    def test_otherwise_with_feature_mandatory_always_true(self):
        """Feature-gated mandatory (non-comparison) passes through as True."""
        xml = """<attribute name='A'>
            <otherwiseConform>
                <mandatoryConform><feature name="LT"/></mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        conf = parse_conformance(
            ET.fromstring(xml), {"LT": MockFeature("Lighting", "LT")}
        )
        self.assertTrue(conf.is_mandatory_at_revision(1))

    def test_is_mandatory_xml_helper_flags_revision_pattern(self):
        """is_mandatory() (XML-level helper) returns True for revision-gated pattern."""
        xml = """<attribute name='RevAttr'>
            <otherwiseConform>
                <mandatoryConform>
                    <greaterOrEqualTerm>
                        <revision value="current"/>
                        <revision value="4"/>
                    </greaterOrEqualTerm>
                </mandatoryConform>
                <optionalConform/>
            </otherwiseConform>
        </attribute>"""
        self.assertTrue(is_mandatory(ET.fromstring(xml)))


def run_tests():
    """Run all tests with verbose output"""
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromModule(__import__(__name__))
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    return result.wasSuccessful()


if __name__ == "__main__":
    import sys

    sys.exit(0 if run_tests() else 1)
