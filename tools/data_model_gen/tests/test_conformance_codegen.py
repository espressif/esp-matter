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
Comprehensive test suite for conformance code generation.

Tests the conversion of conformance JSON (from XML parsing) to C++ conditional
expressions used in the ESP Matter SDK.
"""

import unittest
import sys
import os

# Add parent directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from code_generation.conformance_codegen import (  # noqa: E402
    Conformance,
    FeatureConformance,
)
from utils.conformance import ConformanceException  # noqa: E402


class TestBasicCodeGeneration(unittest.TestCase):
    """Test basic conformance to C++ code generation"""

    def test_none_conformance(self):
        """Test that None conformance returns None"""
        result = Conformance(None)()
        self.assertIsNone(result)

    def test_empty_conformance(self):
        """Test that empty dict returns None"""
        result = Conformance({})()
        self.assertIsNone(result)

    def test_no_condition(self):
        """Test conformance with type but no condition"""
        conformance = {"type": "mandatory"}
        result = Conformance(conformance)()
        self.assertIsNone(result)

    def test_no_type(self):
        """Test conformance with condition but no type"""
        conformance = {"condition": {"feature": "lighting"}}
        with self.assertRaises(ConformanceException):
            Conformance(conformance)()


class TestFeatureConditions(unittest.TestCase):
    """Test feature-based conformance code generation"""

    def test_single_feature(self):
        """Test single feature with runtime check"""
        conformance = {"type": "mandatory", "condition": {"feature": "lighting"}}
        result = Conformance(conformance)()

        self.assertEqual("has_feature(lighting)", result)


class TestAttributeConditions(unittest.TestCase):
    """Test attribute-based conformance code generation"""

    def test_attribute_existence_check(self):
        """Test attribute existence condition"""
        conformance = {"type": "mandatory", "condition": {"attribute": "HoldTime"}}
        result = Conformance(conformance)()

        self.assertEqual(
            "has_attribute(HoldTime)",
            result,
        )


class TestCommandConditions(unittest.TestCase):
    """Test command-based conformance code generation"""

    def test_command_existence_check(self):
        """Test command existence condition"""
        conformance = {
            "type": "mandatory",
            "condition": {"command": "SetHoldTime", "flag": "COMMAND_FLAG_ACCEPTED"},
        }
        result = Conformance(conformance)()

        self.assertEqual(
            "has_command(SetHoldTime, COMMAND_FLAG_ACCEPTED)",
            result,
        )

    def test_command_without_flag(self):
        """Test command condition without explicit flag"""
        conformance = {"type": "mandatory", "condition": {"command": "SetHoldTime"}}
        with self.assertRaises(ConformanceException):
            Conformance(conformance)()


class TestBooleanOperations(unittest.TestCase):
    """Test AND, OR, NOT operations in code generation"""

    def test_and_two_features(self):
        """Test AND of two features"""
        conformance = {
            "type": "mandatory",
            "condition": {
                "and": [{"feature": "lighting"}, {"feature": "dead_front_behavior"}]
            },
        }
        result = Conformance(conformance)()

        self.assertEqual(
            "((has_feature(lighting)) && (has_feature(dead_front_behavior)))",
            result,
        )

    def test_or_two_features(self):
        """Test OR of two features"""
        conformance = {
            "type": "mandatory",
            "condition": {"or": [{"feature": "hue_saturation"}, {"feature": "xy"}]},
        }
        result = Conformance(conformance)()

        self.assertEqual(
            "((has_feature(hue_saturation)) || (has_feature(xy)))",
            result,
        )

    def test_not_feature(self):
        """Test NOT of a feature"""
        conformance = {
            "type": "optional",
            "condition": {"not": {"feature": "off_only"}},
        }
        result = Conformance(conformance)()

        self.assertEqual("!(has_feature(off_only))", result)

    def test_complex_nested_and_or_not(self):
        """Test complex nested: AND(OR(...), NOT(...))"""
        conformance = {
            "type": "mandatory",
            "condition": {
                "and": [
                    {"or": [{"feature": "hs"}, {"feature": "xy"}]},
                    {"not": {"feature": "off_only"}},
                ]
            },
        }
        result = Conformance(conformance)()

        self.assertEqual(
            "((((has_feature(hs)) || (has_feature(xy)))) && (!(has_feature(off_only))))",
            result,
        )


class TestOtherwiseConformance(unittest.TestCase):
    """Test otherwise conformance code generation"""

    def test_otherwise_simple(self):
        """Test otherwise with simple sub-conditions"""
        conformance = {
            "type": "otherwise",
            "condition": {"mandatory": {"feature": "lighting"}, "optional": True},
        }
        result = Conformance(conformance)()

        self.assertEqual("(has_feature(lighting))", result)

    def test_otherwise_with_deprecated(self):
        """Test otherwise with deprecated sub-condition"""
        conformance = {
            "type": "otherwise",
            "condition": {
                "mandatory": {"feature": "lighting"},
                "deprecate": True,
            },
        }
        result = Conformance(conformance)()

        self.assertEqual("(has_feature(lighting))", result)

    def test_otherwise_all_true(self):
        """Test otherwise where all sub-conditions are True"""
        conformance = {
            "type": "otherwise",
            "condition": {"mandatory": True, "optional": True},
        }
        result = Conformance(conformance)()

        self.assertIsNone(result)


class TestHasNotCondition(unittest.TestCase):
    """Test has_not_condition helper function"""

    def test_has_not_at_top_level(self):
        """Test detection of NOT at top level"""
        conformance = {
            "type": "optional",
            "condition": {"not": {"feature": "off_only"}},
        }
        self.assertTrue(Conformance(conformance).is_not_term_present)

    def test_no_not_condition(self):
        """Test when there's no NOT condition"""
        conformance = {"type": "mandatory", "condition": {"feature": "lighting"}}
        self.assertFalse(Conformance(conformance).is_not_term_present)

    def test_not_nested_deeper(self):
        """Test NOT nested in AND (not at top level)"""
        conformance = {
            "type": "mandatory",
            "condition": {"and": [{"feature": "a"}, {"not": {"feature": "b"}}]},
        }
        self.assertTrue(FeatureConformance(conformance).is_not_term_present)

    def test_none_conformance(self):
        """Test with None conformance"""
        self.assertFalse(FeatureConformance(None).is_not_term_present)

    def test_no_condition_key(self):
        """Test with conformance missing condition key"""
        self.assertFalse(FeatureConformance({"type": "mandatory"}).is_not_term_present)


class TestFeatureConformanceClass(unittest.TestCase):
    """Test FeatureConformance analysis class"""

    def test_simple_mandatory_feature(self):
        """Test extraction of simple mandatory feature"""
        conformance = {"type": "mandatory", "condition": {"feature": "lighting"}}
        fc = FeatureConformance(conformance)()

        self.assertEqual("has_feature(lighting)", fc)

    def test_mandatory_parent_feature_name(self):
        """Test extraction of mandatory parent feature name"""
        conformance = {
            "type": "otherwise",
            "condition": {"mandatory": {"feature": "lighting"}, "optional": True},
        }
        fc = FeatureConformance(conformance)

        self.assertEqual("lighting", fc.mandatory_parent)

    def test_exact_one_feature_detection(self):
        """Test detection of 'exactly one' choice conformance"""
        conformance = {"type": "optional", "choice": "a"}
        fc = FeatureConformance(conformance)
        self.assertTrue(fc.is_exact_one())

    def test_at_least_one_feature_detection(self):
        """Test detection of 'at least one' choice conformance"""
        conformance = {"type": "optional", "choice": "a", "more": True, "min": 1}
        fc = FeatureConformance(conformance)

        self.assertTrue(fc.is_at_least_one())
        self.assertFalse(fc.is_exact_one())


class TestEdgeCases(unittest.TestCase):
    """Test edge cases and error handling"""

    def test_invalid_condition_type(self):
        """Test with invalid condition type (not a dict)"""
        conformance = {"type": "mandatory", "condition": "invalid"}
        with self.assertRaises(AttributeError):
            Conformance(conformance)()

    def test_empty_and_list(self):
        """Test AND with empty list"""
        conformance = {"type": "mandatory", "condition": {"and": []}}
        result = Conformance(conformance)()

        self.assertIsNone(result)

    def test_empty_or_list(self):
        """Test OR with empty list"""
        conformance = {"type": "mandatory", "condition": {"or": []}}
        result = Conformance(conformance)()

        self.assertIsNone(result)

    def test_single_element_and(self):
        """Test AND with single element"""
        conformance = {
            "type": "mandatory",
            "condition": {"and": [{"feature": "lighting"}]},
        }
        result = Conformance(conformance)()

        self.assertIsNotNone(result)
        # Should not have && operator for single element
        self.assertNotIn("&&", result)

    def test_single_element_or(self):
        """Test OR with single element"""
        conformance = {
            "type": "mandatory",
            "condition": {"or": [{"feature": "lighting"}]},
        }
        result = Conformance(conformance)()

        self.assertIsNotNone(result)
        # Should not have || operator for single element
        self.assertNotIn("||", result)

    def test_and_with_none_subconditions(self):
        """Test AND where some subconditions are invalid"""
        conformance = {
            "type": "mandatory",
            "condition": {"and": [{"feature": "lighting"}, {"invalid": "data"}]},
        }
        result = Conformance(conformance)()

        # Should still generate code for valid subcondition
        self.assertIsNotNone(result)
        self.assertIn("has_feature(lighting)", result)


class TestComparisonConditionExpressions(unittest.TestCase):
    """Test all 6 comparison operators generate correct C++ conditional strings."""

    _OPS = [
        ("greater", ">"),
        ("greater_or_equal", ">="),
        ("less_than", "<"),
        ("less_or_equal", "<="),
        ("equal", "=="),
        ("not_equal", "!="),
    ]

    def _make_conf(self, op_key, func_name="num_primaries", literal=5, nullable=False):
        return {
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

    def test_all_operators_non_nullable(self):
        """Each operator produces the correct C++ comparison string for non-nullable attributes."""
        for op_key, sym in self._OPS:
            with self.subTest(op=op_key):
                result = Conformance(self._make_conf(op_key))()
                self.assertIn(f"config->num_primaries {sym} 5", result)

    def test_all_operators_nullable(self):
        """Nullable attribute uses .value_or(0) in the generated expression."""
        for op_key, sym in self._OPS:
            with self.subTest(op=op_key):
                result = Conformance(self._make_conf(op_key, nullable=True))()
                self.assertIn(f"config->num_primaries.value_or(0) {sym} 5", result)

    def test_literal_zero(self):
        result = Conformance(self._make_conf("greater", literal=0))()
        self.assertIn("config->num_primaries > 0", result)

    def test_missing_func_name_yields_none(self):
        """Comparison dict without func_name → NonExpr → None output."""
        conf = {
            "type": "otherwise",
            "condition": {
                "mandatory": {
                    "greater": {"attribute": "NumberOfPrimaries", "literal": 0}
                },
                "optional": True,
            },
        }
        self.assertIsNone(Conformance(conf)())


class TestGetComparisonConditionInfo(unittest.TestCase):
    """Test Conformance.get_comparison_condition_info()."""

    def _make_conf(
        self, op_key, func_name="number_of_primaries", literal=0, nullable=True
    ):
        return {
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

    def test_returns_none_for_non_comparison_types(self):
        """Returns None for mandatory, optional, feature-gated, and unconditional OTHERWISE."""
        cases = [
            {"type": "mandatory", "condition": {"feature": "lighting"}},
            {"type": "optional"},
            {
                "type": "otherwise",
                "condition": {"mandatory": {"feature": "lighting"}, "optional": True},
            },
            {"type": "otherwise", "condition": {"mandatory": True, "optional": True}},
        ]
        for conf in cases:
            with self.subTest(type=conf.get("type")):
                self.assertIsNone(Conformance(conf).get_comparison_condition_info())

    def test_returns_none_when_func_name_missing(self):
        """Comparison term without func_name → None."""
        conf = {
            "type": "otherwise",
            "condition": {
                "mandatory": {
                    "greater": {"attribute": "NumberOfPrimaries", "literal": 0}
                },
                "optional": True,
            },
        }
        self.assertIsNone(Conformance(conf).get_comparison_condition_info())

    def test_all_operators_return_non_none(self):
        """All 6 comparison operators return a non-None (expr_str, literal) tuple."""
        for op in [
            "greater",
            "greater_or_equal",
            "less_than",
            "less_or_equal",
            "equal",
            "not_equal",
        ]:
            with self.subTest(op=op):
                result = Conformance(
                    self._make_conf(op)
                ).get_comparison_condition_info()
                self.assertIsNotNone(result)

    def test_nullable_attribute_uses_value_or(self):
        expr_str, _ = Conformance(
            self._make_conf("greater", nullable=True)
        ).get_comparison_condition_info()
        self.assertIn(".value_or(0)", expr_str)

    def test_non_nullable_attribute_no_value_or(self):
        expr_str, _ = Conformance(
            self._make_conf("greater", nullable=False)
        ).get_comparison_condition_info()
        self.assertNotIn("value_or", expr_str)

    def test_color_control_primary_1x_pattern(self):
        """Integration: full otherwise+comparison pattern produces correct expr and literal."""
        conf = {
            "type": "otherwise",
            "condition": {
                "mandatory": {
                    "greater": {
                        "attribute": "NumberOfPrimaries",
                        "literal": 0,
                        "func_name": "number_of_primaries",
                        "nullable": True,
                    }
                },
                "optional": True,
            },
        }
        expr_str, literal = Conformance(conf).get_comparison_condition_info()
        self.assertEqual(expr_str, "config->number_of_primaries.value_or(0) > 0")
        self.assertEqual(literal, 0)


class TestGetComparisonRefConfigName(unittest.TestCase):
    """Test Conformance.get_comparison_ref_config_name()."""

    def _make_conf(self, op_key, func_name="number_of_primaries"):
        return {
            "type": "otherwise",
            "condition": {
                "mandatory": {
                    op_key: {
                        "attribute": "NumberOfPrimaries",
                        "literal": 0,
                        "func_name": func_name,
                        "nullable": True,
                    }
                },
                "optional": True,
            },
        }

    def test_returns_func_name_for_all_operators(self):
        for op in [
            "greater",
            "greater_or_equal",
            "less_than",
            "less_or_equal",
            "equal",
            "not_equal",
        ]:
            with self.subTest(op=op):
                result = Conformance(
                    self._make_conf(op)
                ).get_comparison_ref_config_name()
                self.assertEqual(result, "number_of_primaries")

    def test_returns_none_for_non_comparison(self):
        """Returns None for mandatory type, feature-gated otherwise, and no func_name."""
        cases = [
            {"type": "mandatory", "condition": {"feature": "lighting"}},
            {
                "type": "otherwise",
                "condition": {"mandatory": {"feature": "lighting"}, "optional": True},
            },
            {
                "type": "otherwise",
                "condition": {
                    "mandatory": {
                        "greater": {"attribute": "NumberOfPrimaries", "literal": 0}
                    },
                    "optional": True,
                },
            },
        ]
        for conf in cases:
            with self.subTest(conf=conf.get("type")):
                self.assertIsNone(Conformance(conf).get_comparison_ref_config_name())


class TestGetMandatoryRefCommandNames(unittest.TestCase):
    """Test Conformance.get_mandatory_ref_command_names()."""

    def test_returns_empty_for_non_command_conditions(self):
        """Returns [] for otherwise type, optional, feature condition, and None conformance."""
        cases = [
            {
                "type": "otherwise",
                "condition": {
                    "mandatory": {"command": "X", "flag": "COMMAND_FLAG_ACCEPTED"},
                    "optional": True,
                },
            },
            {"type": "optional"},
            {"type": "mandatory", "condition": {"feature": "lighting"}},
        ]
        for conf in cases:
            with self.subTest(type=conf.get("type")):
                self.assertEqual(
                    Conformance(conf).get_mandatory_ref_command_names(), []
                )
        self.assertEqual(Conformance(None).get_mandatory_ref_command_names(), [])

    def test_returns_command_name_for_simple_condition(self):
        conf = {
            "type": "mandatory",
            "condition": {"command": "ChangeToMode", "flag": "COMMAND_FLAG_ACCEPTED"},
        }
        self.assertEqual(
            Conformance(conf).get_mandatory_ref_command_names(), ["ChangeToMode"]
        )

    def test_returns_all_names_for_or_condition(self):
        conf = {
            "type": "mandatory",
            "condition": {
                "or": [
                    {"command": "Pause", "flag": "COMMAND_FLAG_ACCEPTED"},
                    {"command": "Stop", "flag": "COMMAND_FLAG_ACCEPTED"},
                    {"command": "Start", "flag": "COMMAND_FLAG_ACCEPTED"},
                ]
            },
        }
        names = Conformance(conf).get_mandatory_ref_command_names()
        self.assertEqual(sorted(names), ["Pause", "Start", "Stop"])


def run_tests():
    """Run all tests with verbose output"""
    loader = unittest.TestLoader()
    suite = loader.loadTestsFromModule(__import__(__name__))
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    return result.wasSuccessful()


if __name__ == "__main__":
    sys.exit(0 if run_tests() else 1)
