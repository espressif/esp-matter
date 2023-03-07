#!/usr/bin/env python3
#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

from regex import match, search, sub, findall, S, M, fullmatch
from pprint import pformat, pprint
from os import path, linesep, makedirs
from collections import defaultdict, namedtuple
from typing import NamedTuple
from argparse import ArgumentParser, RawDescriptionHelpFormatter, FileType
from datetime import datetime
from copy import copy
from itertools import tee
from cbor2 import loads, dumps, CBORTag, load, CBORDecodeValueError, CBORDecodeEOF
from yaml import safe_load as yaml_load, dump as yaml_dump
from json import loads as json_load, dumps as json_dump
from io import BytesIO
import sys

indentation = "\t"
newl_ind = "\n" + indentation


# Size of "additional" field if num is encoded as int
def sizeof(num):
    if num <= 23:
        return 0
    elif num < 0x100:
        return 1
    elif num < 0x10000:
        return 2
    elif num < 0x100000000:
        return 4
    elif num < 0x10000000000000000:
        return 8
    else:
        raise ValueError("Number too large (more than 64 bits).")


# Print only if verbose
def verbose_print(verbose_flag, *things):
    if verbose_flag:
        print(*things)


# Pretty print only if verbose
def verbose_pprint(verbose_flag, *things):
    if verbose_flag:
        pprint(*things)


global_counter = 0


# Retrieve a unique id.
def counter(reset=False):
    global global_counter
    if reset:
        global_counter = 0
        return global_counter
    global_counter += 1
    return global_counter


# Replace an element in a list or tuple and return the list. For use in
# lambdas.
def list_replace_if_not_null(lst, i, r):
    if lst[i] == "NULL":
        return lst
    if isinstance(lst, tuple):
        convert = tuple
        lst = list(lst)
    else:
        assert isinstance(lst, list)
        convert = list
    lst[i] = r
    return convert(lst)


# Return a code snippet that assigns the value to a variable var_name and
# returns pointer to the variable, or returns NULL if the value is None.
def val_or_null(value, var_name):
    return "(%s = %d, &%s)" % (var_name, value, var_name) if value is not None else "NULL"


# Assign the tmp_value variable.
def tmp_val_or_null(value):
    return val_or_null(value, "tmp_value")
    # return f"""{f'&(uint32_t){{{value}}}' if value is not None else 'NULL'}"""


# Assign the min_value variable.
def tmp_str_or_null(value):
    return \
        f"""(tmp_str.value = {f'"{value}"' if value is not None else 'NULL'},
    tmp_str.len = sizeof({f'"{value}"'}) - 1, &tmp_str)"""


# Assign the max_value variable.
def min_bool_or_null(value):
    return f"(&(bool){{{int(value)}}})"


def deref_if_not_null(access):
    return access if access == "NULL" else "&" + access


# Return an argument list for a function call to a encoder/decoder function.
def xcode_args(res, *sargs):
    if len(sargs) > 0:
        return "state, %s, %s, %s" % (
            "&(%s)" % res if res != "NULL" else res, sargs[0], sargs[1])
    else:
        return "state, %s" % (
            "(%s)" % res if res != "NULL" else res)


# Return the code that calls a encoder/decoder function with a given set of
# arguments.
def xcode_statement(func, *sargs, **kwargs):
    if func is None:
        return "1"
    return "(%s(%s))" % (func, xcode_args(*sargs, **kwargs))


def add_semicolon(decl):
    if len(decl) != 0 and decl[-1][-1] != ";":
        decl[-1] += ";"
    return decl


def struct_ptr_name(mode):
    return "result" if mode == "decode" else "input"


def ternary_if_chain(access, names, xcode_strings):
    return "((%s == %s) ? %s%s: %s)" % (
        access,
        names[0],
        xcode_strings[0],
        newl_ind,
        ternary_if_chain(access, names[1:], xcode_strings[1:]) if len(names) > 1 else "false")


# Class for parsing CDDL. One instance represents one CBOR data item, with a few caveats:
#  - For repeated data, one instance represents all repetitions.
#  - For "OTHER" types, one instance points to another type definition.
#  - For "GROUP" and "UNION" types, there is no separate data item for the instance.
class CddlParser:
    def __init__(self, default_max_qty, my_types, base_name=None):
        self.id_prefix = "temp_" + str(counter())
        self.id_num = None  # Unique ID number. Only populated if needed.
        # The value of the data item. Has different meaning for different
        # types.
        self.value = None
        self.max_value = None  # Maximum value. Only used for numbers and bools.
        self.min_value = None  # Minimum value. Only used for numbers and bools.
        # The readable label associated with the element in the CDDL.
        self.label = None
        self.min_qty = 1  # The minimum number of times this element is repeated.
        self.max_qty = 1  # The maximum number of times this element is repeated.
        # The size of the element. Only used for integers, byte strings, and
        # text strings.
        self.size = None
        self.min_size = None  # Minimum size.
        self.max_size = None  # Maximum size.
        # Key element. Only for children of "MAP" elements. self.key is of the
        # same class as self.
        self.key = None
        # The element specified via.cbor or.cborseq(only for byte
        # strings).self.cbor is of the same class as self.
        self.cbor = None
        # Any tags (type 6) to precede the element.
        self.tags = []
        # The CDDL string used to determine the min_qty and max_qty. Not used after
        # min_qty and max_qty are determined.
        self.quantifier = None
        # The "type" of the element. This follows the CBOR types loosely, but are more related to CDDL
        # concepts. The possible types are "INT", "UINT", "NINT", "FLOAT", "BSTR", "TSTR", "BOOL",  "NIL", "LIST",
        # "MAP","GROUP", "UNION" and "OTHER". "OTHER" represents a CDDL type defined with '='.
        self.type = None
        self.match_str = ""
        self.errors = list()

        self.my_types = my_types
        self.default_max_qty = default_max_qty  # args.default_max_qty
        self.base_name = base_name  # Used as default for self.get_base_name()

    @classmethod
    def from_cddl(cddl_class, cddl_string, default_max_qty, *args, **kwargs):
        my_types = dict()

        type_strings = cddl_class.get_types(cddl_string)
        my_types = {my_type: None for my_type in type_strings.keys()}

        # Parse the definitions, replacing the each string with a
        # CodeGenerator instance.
        for my_type, cddl_string in type_strings.items():
            parsed = cddl_class(*args, default_max_qty, my_types, **kwargs, base_name=my_type)
            parsed.get_value(cddl_string.replace("\n", " "))
            my_types[my_type] = parsed.flatten()[0]
            my_types[my_type].set_id_prefix("")

        counter(True)

        # post_validate all the definitions.
        for my_type in my_types:
            my_types[my_type].post_validate()

        return my_types

    # Strip CDDL comments (';') from the string.
    @staticmethod
    def strip_comments(instr):
        comment_regex = r"\;.*?\n"
        return sub(comment_regex, '', instr)

    # Returns a dict containing multiple typename=>string
    @classmethod
    def get_types(cls, cddl_string):
        instr = cls.strip_comments(cddl_string)
        type_regex = r"(\s*?\$?\$?([\w-]+)\s*(\/{0,2})=\s*(.*?)(?=(\Z|\s*\$?\$?[\w-]+\s*\/{0,2}=(?!\>))))"
        result = defaultdict(lambda: "")
        types = [(key, value, slashes) for (_1, key, slashes, value, _2) in findall(type_regex, instr, S | M)]
        for key, value, slashes in types:
            if slashes:
                result[key] += slashes
                result[key] += value
                result[key] = result[key].lstrip(slashes)  # strip from front
            else:
                result[key] = value
        return dict(result)

    backslash_quotation_mark = r'\"'

    # Generate a (hopefully) unique and descriptive name
    def generate_base_name(self):
        return ((
            self.label
            or (self.key.value if self.key and self.key.type in ["TSTR", "OTHER"] else None)
            or (f"{self.value.replace(self.backslash_quotation_mark, '')}_{self.type.lower()}"
                if self.type == "TSTR" and self.value is not None else None)
            or (f"{self.type.lower()}{self.value}" if self.type in ["INT", "UINT"] and self.value is not None else None)
            or (next((key for key, value in self.my_types.items() if value == self), None))
            or ("_" + self.value if self.type == "OTHER" else None)
            or ("_" + self.value[0].get_base_name()
                if self.type in ["LIST", "GROUP"] and self.value is not None else None)
            or (self.cbor.value if self.cbor and self.cbor.type in ["TSTR", "OTHER"] else None)
            or ((self.key.generate_base_name() + self.type.lower()) if self.key else None)
            or self.type.lower()).replace("-", "_"))

    # Base name used for functions, variables, and typedefs.
    def get_base_name(self):
        generated = self.generate_base_name()
        return (self.base_name or generated).replace("-", "_")

    # Set an explicit base name for this element.
    def set_base_name(self, base_name):
        self.base_name = base_name

    # Add uniqueness to the base name.
    def id(self):
        return "%s%s" % ((self.id_prefix + "_")
                         if self.id_prefix != "" else "", self.get_base_name())

    def init_args(self):
        return (self.default_max_qty,)

    def init_kwargs(self):
        return {"my_types": self.my_types}

    def set_id_prefix(self, id_prefix):
        self.id_prefix = id_prefix
        if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
            for child in self.value:
                child.set_id_prefix(self.child_base_id())
        if self.cbor:
            self.cbor.set_id_prefix(self.child_base_id())
        if self.key:
            self.key.set_id_prefix(self.child_base_id())

    # Id to pass to children for them to use as basis for their id/base name.
    def child_base_id(self):
        return self.id()

    def get_id_num(self):
        if self.id_num is None:
            self.id_num = counter()
        return self.id_num

    # Human readable representation.
    def mrepr(self, newline):
        reprstr = ''
        if self.quantifier:
            reprstr += self.quantifier
        if self.label:
            reprstr += self.label + ':'
        for tag in self.tags:
            reprstr += f"#6.{tag}"
        if self.key:
            reprstr += repr(self.key) + " => "
        if self.is_unambiguous():
            reprstr += '/'
        if self.is_unambiguous_repeated():
            reprstr += '/'
        reprstr += self.type
        if self.size:
            reprstr += '(%d)' % self.size
        if newline:
            reprstr += '\n'
        if self.value:
            reprstr += pformat(self.value, indent=4, width=1)
        if self.cbor:
            reprstr += " cbor: " + repr(self.cbor)
        return reprstr.replace('\n', '\n    ')

    def _flatten(self):
        new_value = []
        if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
            for child in self.value:
                new_value.extend(
                    child.flatten(allow_multi=self.type != "UNION"))
            self.value = new_value
        if self.key:
            self.key = self.key.flatten()[0]
        if self.cbor:
            self.cbor = self.cbor.flatten()[0]

    def flatten(self, allow_multi=False):
        self._flatten()
        if self.type in ["GROUP", "UNION"]\
                and (len(self.value) == 1)\
                and (not (self.key and self.value[0].key)):
            self.value[0].min_qty *= self.min_qty
            self.value[0].max_qty *= self.max_qty
            if not self.value[0].label:
                self.value[0].label = self.label
            if not self.value[0].key:
                self.value[0].key = self.key
            self.value[0].tags.extend(self.tags)
            return self.value
        elif allow_multi and self.type in ["GROUP"] and self.min_qty == 1 and self.max_qty == 1:
            return self.value
        else:
            return [self]

    def set_min_value(self, min_value):
        self.min_value = min_value

    def set_max_value(self, max_value):
        self.max_value = max_value

    # Set the self.type and self.value of this element. For use during CDDL
    # parsing.
    def type_and_value(self, new_type, value_generator):
        if self.type is not None:
            raise TypeError(
                "Cannot have two values: %s, %s" %
                (self.type, new_type))
        if new_type is None:
            raise TypeError("Cannot set None as type")
        if new_type == "UNION" and self.value is not None:
            raise ValueError("Did not expect multiple parsed values for union")

        self.type = new_type
        self.set_value(value_generator)

    def set_value(self, value_generator):
        value = value_generator()
        self.value = value

        if self.type in ["BSTR", "TSTR"]:
            if value is not None:
                self.set_size(len(value))
        if self.type in ["UINT", "NINT"]:
            if value is not None:
                self.size = sizeof(value)
                self.set_min_value(value)
                self.set_max_value(value)
        if self.type == "NINT":
            self.max_value = -1

    # Set the self.type and self.minValue and self.max_value (or self.min_size and self.max_size depending on the type)
    # of this element. For use during CDDL parsing.
    def type_and_range(self, new_type, min_val, max_val):
        if new_type not in ["INT", "UINT", "NINT"]:
            raise TypeError(
                "Only integers (not %s) can have range" %
                (new_type,))
        if min_val > max_val:
            raise TypeError(
                "Range has larger minimum than maximum (min %d, max %d)" %
                (min_val, max_val))
        if min_val == max_val:
            return self.type_and_value(new_type, min_val)
        self.type = new_type
        self.set_min_value(min_val)
        self.set_max_value(max_val)
        if new_type in "UINT":
            self.set_size_range(sizeof(min_val), sizeof(max_val))
        if new_type == "NINT":
            self.set_size_range(sizeof(abs(max_val)), sizeof(abs(min_val)))
        if new_type == "INT":
            self.set_size_range(None, max(sizeof(abs(max_val)), sizeof(abs(min_val))))

    # Set the self.value and self.size of this element. For use during CDDL
    # parsing.
    def type_value_size(self, new_type, value, size):
        self.type_and_value(new_type, value)
        self.set_size(size)

    # Set the self.label of this element. For use during CDDL parsing.
    def set_label(self, label):
        if self.type is not None:
            raise TypeError("Cannot have label after value: " + label)
        self.label = label

    # Set the self.quantifier, self.min_qty, and self.max_qty of this element. For
    # use during CDDL parsing.
    def set_quantifier(self, quantifier):
        if self.type is not None:
            raise TypeError(
                "Cannot have quantifier after value: " + quantifier)

        quantifier_mapping = [
            (r"\?", lambda mo: (0, 1)),
            (r"\*", lambda mo: (0, None)),
            (r"\+", lambda mo: (1, None)),
            (r"(\d*)\*\*(\d*)", lambda mo: (int(mo.groups()[0] or 0), int(mo.groups()[1] or 0))),
        ]

        self.quantifier = quantifier
        for (reg, handler) in quantifier_mapping:
            match_obj = match(reg, quantifier)
            if match_obj:
                (self.min_qty, self.max_qty) = handler(match_obj)
                if self.max_qty is None:
                    self.max_qty = self.default_max_qty
                return
        raise ValueError("invalid quantifier: %s" % quantifier)

    # Set the self.size of this element. This will also set the self.minValue and self.max_value of UINT types.
    # For use during CDDL parsing.
    def set_size(self, size):
        if self.type is None:
            raise TypeError("Cannot have size before value: " + str(size))
        elif self.type in ["INT", "UINT", "NINT"]:
            self.set_size_range(None, size)
        elif self.type in ["BSTR", "TSTR"]:
            self.set_size_range(size, size)
        else:
            raise TypeError(".size cannot be applied to %s" % self.type)

    # Set the self.minValue and self.max_value or self.min_size and self.max_size of this element based on what values
    # can be contained within an integer of a certain size. For use during
    # CDDL parsing.
    def set_size_range(self, min_size, max_size):
        if None not in [min_size, max_size] and min_size > max_size:
            raise TypeError(
                "Invalid size range (min %d, max %d)" %
                (min_size, max_size))

        self.set_min_size(min_size)
        self.set_max_size(max_size)

    # Set self.min_size, and self.minValue if type is UINT.
    def set_min_size(self, min_size):
        if self.type == "UINT":
            self.minValue = 256**min(0, abs(min_size - 1)) if min_size else None
        self.min_size = min_size if min_size else None

    # Set self.max_size, and self.max_value if type is UINT.
    def set_max_size(self, max_size):
        if self.type == "UINT" and max_size and self.max_value is None:
            if max_size > 8:
                raise TypeError(
                    "Size too large for integer. size %d" %
                    max_size)
            self.max_value = 256**max_size - 1
        self.max_size = max_size

    # Set the self.cbor of this element. For use during CDDL parsing.
    def set_cbor(self, cbor, cborseq):
        if self.type != "BSTR":
            raise TypeError(
                "%s must be used with bstr." %
                (".cborseq" if cborseq else ".cbor",))
        self.cbor = cbor
        if cborseq:
            self.cbor.max_qty = self.default_max_qty
        self.cbor.set_base_name("cbor")

    # Set the self.key of this element. For use during CDDL parsing.
    def set_key(self, key):
        if self.key is not None:
            raise TypeError("Cannot have two keys: " + key)
        if key.type == "GROUP":
            raise TypeError("A key cannot be a group")
        self.key = key
        self.key.set_base_name("key")

    # Set the self.label OR self.key of this element. In the CDDL "foo: bar", foo can be either a label or a key
    # depending on whether it is in a map. This code uses a slightly different method for choosing between label and
    # key. If the string is recognized as a type, it is treated as a key. For
    # use during CDDL parsing.
    def set_key_or_label(self, key_or_label):
        if key_or_label.type == "OTHER" and key_or_label.value not in self.my_types:
            self.set_label(key_or_label.value)
        else:
            if key_or_label.type == "OTHER" and self.label is None:
                self.set_label(key_or_label.value)
            self.set_key(key_or_label)

    def add_tag(self, tag):
        self.tags.append(int(tag))

    # Append to the self.value of this element. Used with the "UNION" type, which has
    # a python list as self.value. The list represents the "children" of the
    # type. For use during CDDL parsing.
    def union_add_value(self, value, doubleslash=False):
        if self.type != "UNION":
            convert_val = copy(self)
            self.__init__(*self.init_args(), **self.init_kwargs())
            self.type_and_value("UNION", lambda: [convert_val])

            if not doubleslash:
                self.label = convert_val.label
                self.key = convert_val.key
                self.quantifier = convert_val.quantifier
                self.max_qty = convert_val.max_qty
                self.min_qty = convert_val.min_qty
                self.base_name = convert_val.base_name

                convert_val.label = None
                convert_val.key = None
                convert_val.quantifier = None
                convert_val.max_qty = 1
                convert_val.min_qty = 1
                convert_val.base_name = None
        self.value.append(value)

    def convert_to_key(self):
        convert_val = copy(self)
        self.__init__(*self.init_args(), **self.init_kwargs())
        self.set_key(convert_val)

        self.label = convert_val.label
        self.quantifier = convert_val.quantifier
        self.max_qty = convert_val.max_qty
        self.min_qty = convert_val.min_qty
        self.base_name = convert_val.base_name

        convert_val.label = None
        convert_val.quantifier = None
        convert_val.max_qty = 1
        convert_val.min_qty = 1
        convert_val.base_name = None

    # Parse from the beginning of instr (string) until a full element has been parsed. self will become that element.
    # This function is recursive, so if a nested element ("MAP"/"LIST"/"UNION"/"GROUP") is encountered, this function
    # will create new instances and add them to self.value as a list. Likewise, if a key or cbor definition is
    # encountered, a new element will be created and assigned to self.key or self.cbor. When new elements are created,
    # getValue() is called on those elements, via parse().
    def get_value(self, instr):
        # The following regexes match different parts of the element. The order of the list is important because it
        # implements the operator precendence defined in the CDDL spec. Note that some regexes are inserted afterwards
        # because they involve a match of a concatenation of all the initial
        # regexes (with a '|' between each element).
        types = [
            (r'\[(?P<item>(?>[^[\]]+|(?R))*)\]',
             lambda list_str: self.type_and_value("LIST", lambda: self.parse(list_str))),
            (r'\((?P<item>(?>[^\(\)]+|(?R))*)\)',
             lambda group_str: self.type_and_value("GROUP", lambda:self. parse(group_str))),
            (r'{(?P<item>(?>[^{}]+|(?R))*)}',
             lambda map_str: self.type_and_value("MAP", lambda: self.parse(map_str))),
            (r'\/\/\s*(?P<item>.+?)(?=\/\/|\Z)',
             lambda union_str: self.union_add_value(
                 self.parse("(%s)" % union_str if ',' in union_str else union_str)[0],
                 doubleslash=True)),
            (r'(\=\>)',
             lambda _: self.convert_to_key()),
            (r'([+*?])',
             self.set_quantifier),
            (r'(\d*\*\*\d*)',
             self.set_quantifier),
            (r'uint(?!\w)',
             lambda _: self.type_and_value("UINT", lambda: None)),
            (r'nint(?!\w)',
             lambda _: self.type_and_value("NINT", lambda: None)),
            (r'int(?!\w)',
             lambda _: self.type_and_value("INT", lambda:None)),
            (r'float(?!\w)',
             lambda _: self.type_and_value("FLOAT", lambda: None)),
            (r'float16(?!\w)',
             lambda _: self.type_value_size("FLOAT", None, 2)),
            (r'float32(?!\w)',
             lambda _: self.type_value_size("FLOAT", None, 4)),
            (r'float64(?!\w)',
             lambda _: self.type_value_size("FLOAT", None, 8)),
            (r'\-?\d*\.\d+',
             lambda num: self.type_and_value("FLOAT", lambda: int(num))),
            (r'\d+\.\.\d+',
             lambda _range: self.type_and_range("UINT", *map(int, _range.split("..")))),
            (r'\-\d+\.\.\d+',
             lambda _range: self.type_and_range("INT", *map(int, _range.split("..")))),
            (r'\-\d+\.\.\-\d+',
             lambda _range: self.type_and_range("NINT", *map(int, _range.split("..")))),
            (r'\-\d+',
             lambda num: self.type_and_value("NINT", lambda: int(num))),
            (r'0[xbo]\w+',
             lambda num: self.type_and_value("UINT", lambda: int(num, 0))),
            (r'\d+',
             lambda num: self.type_and_value("UINT", lambda: int(num))),
            (r'bstr(?!\w)',
             lambda _: self.type_and_value("BSTR", lambda: None)),
            (r'\'(?P<item>.*?)(?<!\\)\'',
             lambda string: self.type_and_value("BSTR", lambda: string)),
            (r'tstr(?!\w)',
             lambda _: self.type_and_value("TSTR", lambda: None)),
            (r'\"(?P<item>.*?)(?<!\\)\"',
             lambda string: self.type_and_value("TSTR", lambda: string)),
            (r'bool(?!\w)',
             lambda _: self.type_and_value("BOOL", lambda: None)),
            (r'true(?!\w)',
             lambda _: self.type_and_value("BOOL", lambda: True)),
            (r'false(?!\w)',
             lambda _: self.type_and_value("BOOL", lambda: False)),
            (r'nil(?!\w)',
             lambda _: self.type_and_value("NIL", lambda: None)),
            (r'any(?!\w)',
             lambda _: self.type_and_value("ANY", lambda: None)),
            (r'#6\.(?P<item>\d+)',
             self.add_tag),
            (r'(\$?\$?[\w-]+)',
             lambda other_str: self.type_and_value("OTHER", lambda: other_str.strip("$"))),
            (r'\.size \(?(?P<item>\d+\.\.\d+)\)?',
             lambda _range: self.set_size_range(*map(int, _range.split("..")))),
            (r'\.size \(?(?P<item>\d+)\)?',
             lambda size: self.set_size(int(size))),
            (r'\.gt \(?(?P<item>(-?\d+|0[xbo]\w+))\)?',
             lambda minvalue: self.set_min_value(int(minvalue, 0) + 1)),
            (r'\.lt \(?(?P<item>(-?\d+|0[xbo]\w+))\)?',
             lambda maxvalue: self.set_max_value(int(maxvalue, 0) - 1)),
            (r'\.ge \(?(?P<item>(-?\d+|0[xbo]\w+))\)?',
             lambda minvalue: self.set_min_value(int(minvalue, 0))),
            (r'\.le \(?(?P<item>(-?\d+|0[xbo]\w+))\)?',
             lambda maxvalue: self.set_max_value(int(maxvalue, 0))),
            (r'\.eq \(?(?P<item>(-?\d+|0[xbo]\w+))\)?',
             lambda value: self.set_value(lambda: int(value, 0))),
            (r'\.eq \"(?P<item>.*?)(?<!\\)\"',
             lambda value: self.set_value(lambda: value)),
            (r'\.eq \"(?P<item>.*?)(?<!\\)\"',
             lambda value: self.set_value(lambda: value)),
            (r'\.cbor (?P<item>[\w-]+)',
             lambda type_str: self.set_cbor(self.parse(type_str)[0], False)),
            (r'\.cborseq (?P<item>[\w-]+)',
             lambda type_str: self.set_cbor(self.parse(type_str)[0], True))
        ]
        all_type_regex = '|'.join([regex for (regex, _) in (types[:3] + types[5:])])
        for i in range(0, all_type_regex.count("item")):
            all_type_regex = all_type_regex.replace("item", "it%dem" % i, 1)
        types.insert(5, (r'(?P<item>' + all_type_regex + r')\s*\:',
                         lambda key_str: self.set_key_or_label(self.parse(key_str)[0])))
        types.insert(6, (r'(?P<item>' + all_type_regex + r')\s*\=\>',
                         lambda key_str: self.set_key(self.parse(key_str)[0])))
        types.insert(7, (r'\/\s*(?P<item>((' + all_type_regex + r')\s*)+?)(?=\/|\,|\Z)',
                         lambda union_str: self.union_add_value(self.parse(union_str)[0])))

        # Keep parsing until a comma, or to the end of the string.
        while instr != '' and instr[0] != ',':
            match_obj = None
            for (reg, handler) in types:
                match_obj = match(reg, instr)
                if match_obj:
                    try:
                        match_str = match_obj.group("item")
                    except IndexError:
                        match_str = match_obj.group(0)
                    try:
                        handler(match_str)
                    except Exception as e:
                        raise Exception("Failed while parsing this: '%s'" % match_str) from e
                    self.match_str += match_str
                    old_len = len(instr)
                    instr = sub(reg, '', instr, count=1).lstrip()
                    if old_len == len(instr):
                        raise Exception("empty match")
                    break

            if not match_obj:
                raise TypeError("Could not parse this: '%s'" % instr)

        instr = instr[1:]
        if not self.type:
            raise ValueError("No proper value while parsing: %s" % instr)

        # Return the unparsed part of the string.
        return instr.strip()

    # For checking whether this element has a key (i.e. to check that it is a valid "MAP" child).
    # This must have some recursion since CDDL allows the key to be hidden
    # behind layers of indirection.
    def elem_has_key(self):
        return self.key is not None\
            or (self.type == "OTHER" and self.my_types[self.value].elem_has_key())\
            or (self.type in ["GROUP", "UNION"] and all(child.elem_has_key() for child in self.value))

    # Function for performing validations that must be done after all parsing is complete. This is recursive, so
    # it will post_validate all its children + key + cbor.
    def post_validate(self):
        # Validation of this element.
        if self.type == "MAP":
            none_keys = [child for child in self.value if not child.elem_has_key()]
            if none_keys:
                raise TypeError(
                    "Map entry must have key: " + str(none_keys) + " pointing to "
                    + str([self.my_types[elem.value] for elem in none_keys if elem.type == "OTHER"]))
        if self.type == "OTHER":
            if self.value not in self.my_types.keys() or not isinstance(
                    self.my_types[self.value], type(self)):
                raise TypeError("%s has not been parsed." % self.value)
        if self.type == "LIST":
            for child in self.value[:-1]:
                if child.type == "ANY":
                    if child.min_qty != child.max_qty:
                        raise TypeError(f"ambiguous quantity of 'any' is not supported in list, "
                                        + "except as last element:\n{str(child)}")
        if self.type == "UNION" and len(self.value) > 1:
            if any(((not child.key and child.type == "ANY") or (
                    child.key and child.key.type == "ANY")) for child in self.value):
                raise TypeError(
                    "'any' inside union is not supported since it would always be triggered.")

        # Validation of child elements.
        if self.type in ["MAP", "LIST", "UNION", "GROUP"]:
            for child in self.value:
                child.post_validate()
        if self.key:
            self.key.post_validate()
        if self.cbor:
            self.cbor.post_validate()

    # Parses entire instr and returns a list of instances.
    def parse(self, instr):
        instr = instr.strip()
        values = []
        while instr != '':
            value = type(self)(*self.init_args(), **self.init_kwargs())
            instr = value.get_value(instr)
            values.append(value)
        return values

    def __repr__(self):
        return self.mrepr(False)


class CddlXcoder(CddlParser):

    def __init__(self, *args, **kwargs):
        super(CddlXcoder, self).__init__(*args, **kwargs)

        # The prefix used for C code accessing this element, i.e. the struct
        # hierarchy leading up to this element.
        self.accessPrefix = None
        # Used as a guard against endless recursion in self.dependsOn()
        self.dependsOnCall = False
        self.skipped = False

    # Name of variables and enum members for this element.
    def var_name(self):
        name = ("_%s" % self.id())
        return name

    def skip_condition(self):
        if self.skipped:
            return True
        if self.type in ["LIST", "MAP", "GROUP"]:
            return not self.multi_val_condition()
        if self.type == "OTHER":
            return (not self.repeated_multi_var_condition()) and self.single_func_impl_condition()
        return False

    def set_skipped(self, skipped):
        if self.range_check_condition() and self.repeated_single_func_impl_condition():
            self.skipped = True
        else:
            self.skipped = skipped

    # Recursively set the access prefix for this element and all its children.
    def set_access_prefix(self, prefix):
        self.accessPrefix = prefix
        if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
            list(map(lambda child: child.set_access_prefix(self.var_access()),
                     self.value))
            list(map(lambda child: child.set_skipped(self.skip_condition()),
                     self.value))
        elif self in self.my_types.values() and self.type != "OTHER":
            self.set_skipped(not self.multi_member())
        if self.key is not None:
            self.key.set_access_prefix(self.var_access())
        if self.cbor_var_condition():
            self.cbor.set_access_prefix(self.var_access())

    # Whether this type has multiple member variables.
    def multi_member(self):
        return self.multi_var_condition() or self.repeated_multi_var_condition()

    def is_unambiguous_value(self):
        return (self.type == "NIL"
                or (self.type in ["INT", "NINT", "UINT", "FLOAT", "BSTR", "TSTR", "BOOL"] and self.value is not None)
                or (self.type == "BSTR" and self.cbor is not None and self.cbor.is_unambiguous())
                or (self.type == "OTHER" and self.my_types[self.value].is_unambiguous()))

    def is_unambiguous_repeated(self):
        return self.is_unambiguous_value() and (self.key is None or self.key.is_unambiguous_repeated()) \
            or (self.type in ["LIST", "GROUP", "MAP"] and len(self.value) == 0)

    def is_unambiguous(self):
        return (self.is_unambiguous_repeated() and (self.min_qty == self.max_qty))

    # Create an access prefix based on an existing prefix, delimiter and a
    # suffix.
    def access_append_delimiter(self, prefix, delimiter, *suffix):
        assert prefix is not None, "No access prefix for %s" % self.var_name()
        return delimiter.join((prefix,) + suffix)

    # Create an access prefix from this element's prefix, delimiter and a
    # provided suffix.
    def access_append(self, *suffix):
        suffix = list(suffix)
        return self.access_append_delimiter(self.accessPrefix, '.', *suffix)

    # "Path" to this element's variable.
    # If full is false, the path to the repeated part is returned.
    def var_access(self):
        if self.is_unambiguous():
            return "NULL"
        return self.access_append()

    # "Path" to access this element's actual value variable.
    def val_access(self):
        if self.is_unambiguous_repeated():
            return "NULL"
        if self.skip_condition():
            return self.var_access()
        return self.access_append(self.var_name())

    def repeated_val_access(self):
        if self.is_unambiguous_repeated():
            return "NULL"
        return self.access_append(self.var_name())

    # Whether to include a "present" variable for this element.
    def present_var_condition(self):
        return self.min_qty == 0 and self.max_qty <= 1

    # Whether to include a "count" variable for this element.
    def count_var_condition(self):
        return self.max_qty > 1

    # Whether to include a "cbor" variable for this element.
    def is_cbor(self):
        return (self.type not in ["NIL", "ANY"]) \
            and ((self.type != "OTHER") or (self.my_types[self.value].is_cbor()))

    # Whether to include a "cbor" variable for this element.
    def cbor_var_condition(self):
        return (self.cbor is not None) and self.cbor.is_cbor()

    # Whether to include a "choice" variable for this element.
    def choice_var_condition(self):
        return self.type == "UNION"

    # Whether to include a "key" variable for this element.
    def key_var_condition(self):
        return self.key is not None

    # Whether this value adds any repeated elements by itself. I.e. excluding
    # multiple elements from children.
    def self_repeated_multi_var_condition(self):
        return (self.key_var_condition()
                or self.cbor_var_condition()
                or self.choice_var_condition())

    # Whether this element's actual value has multiple members.
    def multi_val_condition(self):
        return (
            self.type in ["LIST", "MAP", "GROUP", "UNION"]
            and (len(self.value) > 1 or (len(self.value) == 1 and self.value[0].multi_member())))

    # Whether any extra variables are to be included for this element for each
    # repetition.
    def repeated_multi_var_condition(self):
        return self.self_repeated_multi_var_condition() or self.multi_val_condition()

    # Whether any extra variables are to be included for this element outside
    # of repetitions.
    # Also, whether this element must involve a call to multi_xcode(), i.e. unless
    # it's repeated exactly once.
    def multi_var_condition(self):
        return self.present_var_condition() or self.count_var_condition()

    # Whether this element needs a check (memcmp) for a string value.
    def range_check_condition(self):
        if self.type not in ["INT", "NINT", "UINT", "BSTR", "TSTR"]:
            return False
        if self.value is not None:
            return False
        if self.type in ["INT", "NINT", "UINT"] and (self.min_value is not None or self.max_value is not None):
            return True
        if self.type in ["BSTR", "TSTR"] and (self.min_size is not None or self.max_size is not None):
            return True
        return False

    # Whether this element should have a typedef in the code.
    def type_def_condition(self):
        if self in self.my_types.values() and self.multi_member() and not self.is_unambiguous():
            return True
        return False

    # Whether this type needs a typedef for its repeated part.
    def repeated_type_def_condition(self):
        return (
            self.repeated_multi_var_condition() and self.multi_var_condition() and not self.is_unambiguous_repeated())

    # Whether this element needs its own encoder/decoder function.
    def single_func_impl_condition(self):
        return (
            False
            or self.key_var_condition()
            or self.cbor_var_condition()
            or self.tags
            or self.type_def_condition()
            or (self.type in ["LIST", "MAP"] and len(self.value) != 0))

    # Whether this element needs its own encoder/decoder function.
    def repeated_single_func_impl_condition(self):
        return self.repeated_type_def_condition() \
            or (self.type in ["LIST", "MAP"] and self.multi_member()) \
            or (
                self.multi_var_condition()
                and (self.self_repeated_multi_var_condition() or self.range_check_condition()))

    def uint_val(self):
        if self.key:
            return self.key.uint_val()
        elif self.type == "UINT" and self.is_unambiguous():
            return self.value
        elif self.type == "GROUP" and not self.count_var_condition():
            return self.value[0].uint_val()
        elif self.type == "OTHER" and not self.count_var_condition() and not self.single_func_impl_condition():
            return self.my_types[self.value].uint_val()
        return None

    def is_uint_disambiguated(self):
        return self.uint_val() is not None

    def all_children_uint_disambiguated(self):
        values = set(child.uint_val() for child in self.value)
        return (len(values) == len(self.value)) and None not in values

    # Name of the "present" variable for this element.
    def present_var_name(self):
        return "%s_present" % (self.var_name())

    # Full "path" of the "present" variable for this element.
    def present_var_access(self):
        return self.access_append(self.present_var_name())

    # Name of the "count" variable for this element.
    def count_var_name(self):
        return "%s_count" % (self.var_name())

    # Full "path" of the "count" variable for this element.
    def count_var_access(self):
        return self.access_append(self.count_var_name())

    # Name of the "choice" variable for this element.
    def choice_var_name(self):
        return self.var_name() + "_choice"

    # Name of the enum entry for this element.
    def enum_var_name(self, uint_val=False):
        if not uint_val:
            return self.var_name()
        else:
            return f"{self.var_name()} = {self.uint_val()}"

    # Full "path" of the "choice" variable for this element.
    def choice_var_access(self):
        return self.access_append(self.choice_var_name())


class CddlValidationError(Exception):
    pass


# Subclass of tuple for holding key,value pairs. This is to make it possible to use isinstance() to
# separate it from other tuples.
class KeyTuple(tuple):
    def __new__(cls, *in_tuple):
        return super(KeyTuple, cls).__new__(cls, *in_tuple)


# Convert data between CBOR, JSON and YAML, and validate against the provided CDDL.
# Decode and validate CBOR into Python structures to be able to make Python scripts that manipulate CBOR code.
class DataTranslator(CddlXcoder):
    # Format a Python object for printing by adding newlines and indentation.
    @staticmethod
    def format_obj(obj):
        formatted = pformat(obj)
        out_str = ""
        indent = 0
        new_line = True
        for c in formatted:
            if new_line:
                if c == " ":
                    continue
                new_line = False
            out_str += c
            if c in "[(":
                indent += 1
            if c in ")]" and indent > 0:
                indent -= 1
            if c in "[(,":
                out_str += linesep
                out_str += "  " * indent
                new_line = True
        return out_str

    # Override the id() function
    def id(self):
        return self.generate_base_name().strip("_")

    # Override the var_name()
    def var_name(self):
        return self.id()

    # Check a condition and raise a CddlValidationError if not.
    def _decode_assert(self, test, msg=""):
        if not test:
            raise CddlValidationError(f"Data did not decode correctly {'('+msg+')' if msg else ''}")

    # Check that no unexpected tags are attached to this data. Return whether a tag was present.
    def _check_tag(self, obj):
        if isinstance(obj, CBORTag):
            self._decode_assert(obj.tag in self.tags or self.type == "ANY", f"Tag ({obj.tag}) not expected for {self}")
            return True
        return False

    # Return our expected python type as returned by cbor2.
    def _expected_type(self):
        return {
            "UINT": lambda: int,
            "INT": lambda: int,
            "NINT": lambda: int,
            "FLOAT": lambda: float,
            "TSTR": lambda: str,
            "BSTR": lambda: bytes,
            "NIL": lambda: type(None),
            "ANY": lambda: (int, float, str, bytes, type(None), bool, list, dict),
            "BOOL": lambda: bool,
            "LIST": lambda: (tuple, list),
            "MAP": lambda: dict,
            "UNION": lambda: tuple((child._expected_type() for child in self.value)),
            "GROUP": lambda: self.value[0]._expected_type(),
            "OTHER": lambda: self.my_types[self.value]._expected_type(),
        }[self.type]()

    # Check that the decoded object has the correct type.
    def _check_type(self, obj):
        exp_type = self._expected_type()
        self._decode_assert(
            isinstance(obj, exp_type), f"{str(self)}: Wrong type of {str(obj)}, expected {str(exp_type)}")

    # Check that the decode value conforms to the restrictions in the CDDL.
    def _check_value(self, obj):
        if self.type in ["UINT", "INT", "NINT", "FLOAT", "TSTR", "BSTR", "BOOL"] and self.value is not None:
            self._decode_assert(
                self.value == obj, f"{obj} should have value {self.value} according to {self.var_name()}")
        if self.type in ["UINT", "INT", "NINT", "FLOAT"]:
            if self.min_value is not None:
                self._decode_assert(obj >= self.min_value, "Minimum value: " + str(self.min_value))
            if self.max_value is not None:
                self._decode_assert(obj <= self.max_value, "Maximum value: " + str(self.max_value))
        if self.type in ["TSTR", "BSTR"]:
            if self.min_size is not None:
                self._decode_assert(len(obj) >= self.min_size, "Minimum length: " + str(self.min_size))
            if self.max_size is not None:
                self._decode_assert(len(obj) <= self.max_size, "Maximum length: " + str(self.max_size))

    # Check that the object is not a KeyTuple since that means it hasn't been properly processed.
    def _check_key(self, obj):
        self._decode_assert(not isinstance(obj, KeyTuple), "Unexpected key found: (key,value)=" + str(obj))

    # Recursively remove intermediate objects that have single members. Keep lists as is.
    def _flatten_obj(self, obj):
        if isinstance(obj, tuple) and len(obj) == 1:
            return self._flatten_obj(obj[0])
        return obj

    # Return the contents of a list if it has a single member and it's name is the same as us.
    def _flatten_list(self, name, obj):
        if isinstance(obj, list) and len(obj) == 1 and len(obj[0]) == 1 and hasattr(obj[0], name):
            return [obj[0][0]]
        return obj

    # Construct a namedtuple object from my_list. my_list contains tuples of name/value.
    # Also, attempt to flatten redundant levels of abstraction.
    def _construct_obj(self, my_list):
        if my_list == []:
            return None
        names, values = tuple(zip(*my_list))
        if len(values) == 1:
            values = (self._flatten_obj(values[0]), )
        values = tuple(self._flatten_list(names[i], values[i]) for i in range(len(values)))
        assert (not any((isinstance(elem, KeyTuple) for elem in values))), f"KeyTuple not processed: {values}"
        return namedtuple("_", names)(*values)

    # Add construct obj and add it to my_list if relevant. Also, process any KeyTuples present.
    def _add_if(self, my_list, obj, expect_key=False, name=None):
        if expect_key and self.type == "OTHER" and self.key is None:
            self.my_types[self.value]._add_if(my_list, obj)
            return
        if self.is_unambiguous():
            return
        if isinstance(obj, list):
            for i in range(len(obj)):
                if isinstance(obj[i], KeyTuple):
                    retvals = list()
                    self._add_if(retvals, obj[i])
                    obj[i] = self._construct_obj(retvals)

            # obj = self._flatten_list(obj)
        if isinstance(obj, KeyTuple):
            key, obj = obj
            if key is not None:
                self.key._add_if(my_list, key, name=self.var_name() + "_key")
        my_list.append((name or self.var_name(), obj))

    # Throw CddlValidationError if iterator is not empty. This consumes one element if present.
    def _iter_is_empty(self, it):
        try:
            val = next(it)
        except StopIteration:
            return True
        raise CddlValidationError(
            f"Iterator not consumed while parsing \n{self}\nRemaining elements:\n elem: "
            + "\n elem: ".join(str(elem) for elem in ([val] + list(it))))

    # Get next element from iterator, throw CddlValidationError instead of StopIteration.
    def _iter_next(self, it):
        try:
            next_obj = next(it)
            return next_obj
        except StopIteration:
            raise CddlValidationError("Iterator empty")

    # Decode single CDDL value, excluding repetitions
    def _decode_single_obj(self, obj):
        self._check_key(obj)
        if self._check_tag(obj):
            return self._decode_single_obj(obj.value)
        self._check_type(obj)
        self._check_value(obj)
        if self.type == "BSTR" and self.cbor_var_condition():
            decoded = self.cbor.decode_str(obj)
            return decoded
        elif self.type in ["UINT", "INT", "NINT", "FLOAT", "TSTR", "BSTR", "BOOL", "NIL", "ANY"]:
            return obj
        elif self.type == "OTHER":
            return self.my_types[self.value]._decode_single_obj(obj)
        elif self.type == "LIST":
            retval = list()
            child_val = iter(obj)
            for child in self.value:
                ret = child._decode_full(child_val)
                child_val, child_obj = ret
                child._add_if(retval, child_obj)
            self._iter_is_empty(child_val)
            return self._construct_obj(retval)
        elif self.type == "MAP":
            retval = list()
            child_val = iter(KeyTuple(item) for item in obj.items())
            for child in self.value:
                child_val, child_key_val = child._decode_full(child_val)
                child._add_if(retval, child_key_val, expect_key=True)
            self._iter_is_empty(child_val)
            return self._construct_obj(retval)
        elif self.type == "UNION":
            retval = list()
            for child in self.value:
                try:
                    child_obj = child._decode_single_obj(obj)
                    child._add_if(retval, child_obj)
                    retval.append(("union_choice", child.var_name()))
                    return self._construct_obj(retval)
                except CddlValidationError as c:
                    self.errors.append(str(c))
            self._decode_assert(False, "No matches for union: " + str(self))
        assert False, "Unexpected type: " + self.type

    # Decode key and value in the form of a KeyTuple
    def _handle_key(self, next_obj):
        self._decode_assert(isinstance(next_obj, KeyTuple), f"Expected key: {self.key} value=" + pformat(next_obj))
        key, obj = next_obj
        key_res = self.key._decode_single_obj(key)
        obj_res = self._decode_single_obj(obj)
        res = KeyTuple((key_res if not self.key.is_unambiguous() else None, obj_res))
        return res

    # Decode single CDDL value, excluding repetitions. May consume 0 to n CBOR objects via the iterator.
    def _decode_obj(self, it):
        my_list = list()
        if self.key is not None:
            it, it_copy = tee(it)
            key_res = self._handle_key(self._iter_next(it_copy))
            return it_copy, key_res
        if self.tags:
            it, it_copy = tee(it)
            maybe_tag = next(it_copy)
            if isinstance(maybe_tag, CBORTag):
                tag_res = self._decode_single_obj(maybe_tag)
                return it_copy, tag_res
        if self.type == "OTHER" and self.key is None:
            return self.my_types[self.value]._decode_full(it)
        elif self.type == "GROUP":
            my_list = list()
            child_it = it
            for child in self.value:
                child_it, child_obj = child._decode_full(child_it)
                if child.key is not None:
                    child._add_if(my_list, child_obj, expect_key=True)
                else:
                    child._add_if(my_list, child_obj)
            ret = (child_it, self._construct_obj(my_list))
        elif self.type == "UNION":
            my_list = list()
            child_it = it
            found = False
            for child in self.value:
                try:
                    child_it, it_copy = tee(child_it)
                    child_it, child_obj = child._decode_full(child_it)
                    child._add_if(my_list, child_obj)
                    my_list.append(("union_choice", child.var_name()))
                    ret = (child_it, self._construct_obj(my_list))
                    found = True
                    break
                except CddlValidationError as c:
                    self.errors.append(str(c))
                    child_it = it_copy
            self._decode_assert(found, "No matches for union: " + str(self))
        else:
            ret = (it, self._decode_single_obj(self._iter_next(it)))
        return ret

    # Decode single CDDL value, with repetitions. May consume 0 to n CBOR objects via the iterator.
    def _decode_full(self, it):
        if self.multi_var_condition():
            retvals = []
            for i in range(self.min_qty):
                it, retval = self._decode_obj(it)
                retvals.append(retval if not self.is_unambiguous_repeated() else None)
            try:
                for i in range(self.max_qty - self.min_qty):
                    it, it_copy = tee(it)
                    it, retval = self._decode_obj(it)
                    retvals.append(retval if not self.is_unambiguous_repeated() else None)
            except CddlValidationError as c:
                it = it_copy
            return it, retvals
        else:
            ret = self._decode_obj(it)
            return ret

    # CBOR object => python object
    def decode_obj(self, obj):
        it = iter([obj])
        _, decoded = self._decode_full(it)
        self._iter_is_empty(it)
        return decoded

    # YAML => python object
    def decode_str_yaml(self, yaml_str):
        yaml_obj = yaml_load(yaml_str)
        obj = self._to_cbor_obj(yaml_obj)
        self.validate_obj(obj)
        return self.decode_obj(obj)

    # CBOR bytestring => python object
    def decode_str(self, cbor_str):
        cbor_obj = loads(cbor_str)
        return self.decode_obj(cbor_obj)

    # Validate CBOR object against CDDL. Exception if not valid.
    def validate_obj(self, obj):
        self.decode_obj(obj)
        return True

    # Validate CBOR bytestring against CDDL. Exception if not valid.
    def validate_str(self, cbor_str):
        cbor_obj = loads(cbor_str)
        return self.validate_obj(cbor_obj)

    # Convert object from YAML/JSON (with special dicts for bstr, tag etc) to CBOR object
    # that cbor2 understands.
    def _to_cbor_obj(self, obj):
        if isinstance(obj, list):
            return [self._to_cbor_obj(elem) for elem in obj]
        elif isinstance(obj, dict):
            if ["bstr"] == list(obj.keys()):
                if isinstance(obj["bstr"], str):
                    bstr = bytes.fromhex(obj["bstr"])
                else:
                    bstr = dumps(self._to_cbor_obj(obj["bstr"]))
                return bstr
            elif ["tag", "val"] == list(obj.keys()):
                return CBORTag(obj["tag"], self._to_cbor_obj(obj["val"]))
            retval = dict()
            for key, val in obj.items():
                match = fullmatch(r"keyval\d+", key)
                if match is not None:
                    new_key = self._to_cbor_obj(val["key"])
                    new_val = self._to_cbor_obj(val["val"])
                    if isinstance(new_key, list):
                        new_key = tuple(new_key)
                    retval[new_key] = new_val
                else:
                    retval[key] = val
            return retval
        return obj

    # inverse of _to_cbor_obj
    def _from_cbor_obj(self, obj):
        if isinstance(obj, list) or isinstance(obj, tuple):
            return [self._from_cbor_obj(elem) for elem in obj]
        elif isinstance(obj, dict):
            retval = dict()
            i = 0
            for key, val in obj.items():
                if not isinstance(key, str):
                    retval[f"keyval{i}"] = {"key": self._from_cbor_obj(key), "val": self._from_cbor_obj(val)}
                    i += 1
                else:
                    retval[key] = val
            return retval
        elif isinstance(obj, bytes):
            f = BytesIO(obj)
            try:
                bstr_obj = self._from_cbor_obj(load(f))
            except (CBORDecodeValueError, CBORDecodeEOF):
                # failed decoding
                bstr_obj = obj.hex()
            else:
                if f.read(1) != b'':
                    # not fully decoded
                    bstr_obj = obj.hex()
            return {"bstr": bstr_obj}
        elif isinstance(obj, CBORTag):
            return {"tag": obj.tag, "val": self._from_cbor_obj(obj.value)}
        assert not isinstance(obj, bytes)
        return obj

    # YAML str => CBOR bytestr
    def from_yaml(self, yaml_str):
        yaml_obj = yaml_load(yaml_str)
        obj = self._to_cbor_obj(yaml_obj)
        self.validate_obj(obj)
        return dumps(obj)

    # CBOR object => YAML str
    def obj_to_yaml(self, obj):
        self.validate_obj(obj)
        return yaml_dump(self._from_cbor_obj(obj))

    # CBOR bytestring => YAML str
    def str_to_yaml(self, cbor_str):
        return self.obj_to_yaml(loads(cbor_str))

    # JSON str => CBOR bytestr
    def from_json(self, json_str):
        obj = self._to_cbor_obj(json_load(json_str))
        self.validate_obj(obj)
        return dumps(obj)

    # CBOR object => JSON str
    def obj_to_json(self, obj):
        self.validate_obj(obj)
        json_obj = self._from_cbor_obj(obj)
        return json_dump(json_obj)

    # CBOR bytestring => JSON str
    def str_to_json(self, cbor_str):
        return self.obj_to_json(loads(cbor_str))

    # CBOR bytestring => C code (uint8_t array initialization)
    def str_to_c_code(self, cbor_str, var_name):
        return f'uint8_t {var_name}[] = {{{", ".join(hex(c) for c in cbor_str)}}};\n'


class XcoderTuple(NamedTuple):
    body: list
    func_name: str
    type_name: str


# Class for generating C code that encode/decodes CBOR and validates it according
# to the CDDL.
class CodeGenerator(CddlXcoder):
    def __init__(self, mode, entry_type_names, *args, **kwargs):
        super(CodeGenerator, self).__init__(*args, **kwargs)
        self.mode = mode
        self.entry_type_names = entry_type_names

    @classmethod
    def from_cddl(cddl_class, mode, *args, **kwargs):
        my_types = super(CodeGenerator, cddl_class).from_cddl(*args, **kwargs)

        # set access prefix (struct access paths) for all the definitions.
        for my_type in my_types:
            my_types[my_type].set_access_prefix(f"(*{struct_ptr_name(mode)})")

        return my_types

    # Whether to include a "cbor" variable for this element.
    def is_cbor(self):
        return (self.type_name() is not None) and (
            (self.type != "OTHER")
            or ((self.value not in self.entry_type_names) and self.my_types[self.value].is_cbor()))

    def init_args(self):
        return (self.mode, self.entry_type_names, self.default_max_qty)

    # Declaration of the "present" variable for this element.
    def present_var(self):
        return ["uint32_t %s;" % self.present_var_name()]

    # Declaration of the "count" variable for this element.
    def count_var(self):
        return ["uint32_t %s;" % self.count_var_name()]

    # Declaration of the "choice" variable for this element.
    def choice_var(self):
        var = self.enclose(
            "enum", [val.enum_var_name(self.all_children_uint_disambiguated()) + "," for val in self.value])
        var[-1] += f" {self.choice_var_name()};"
        return var

    # Name of the encoder/decoder function for this element.
    def xcode_func_name(self):
        return f"{self.mode}{self.var_name()}"

    # Name of the encoder/decoder function for the repeated part of this element.
    def repeated_xcode_func_name(self):
        return f"{self.mode}_repeated{self.var_name()}"

    # Declaration of the variables of all children.
    def child_declarations(self):
        decl = [line for child in self.value for line in child.full_declaration()]
        return decl

    # Declaration of the variables of all children.
    def child_single_declarations(self):
        decl = [
            line for child in self.value for line in child.add_var_name(child.single_var_type(), anonymous=True)]
        return decl

    def simple_func_condition(self):
        if self.single_func_impl_condition():
            return True
        if self.type == "OTHER" and self.my_types[self.value].simple_func_condition():
            return True
        return False

    # Base name if this element needs to declare a type.
    def raw_type_name(self):
        return "struct %s" % self.id()

    # Name of the type of this element's actual value variable.
    def val_type_name(self):
        if self.multi_val_condition():
            return self.raw_type_name()

        # Will fail runtime if we don't use lambda for type_name()
        # pylint: disable=unnecessary-lambda
        name = {
            "INT": lambda: "int32_t",
            "UINT": lambda: "uint32_t",
            "NINT": lambda: "int32_t",
            "FLOAT": lambda: "float_t",
            "BSTR": lambda: "cbor_string_type_t",
            "TSTR": lambda: "cbor_string_type_t",
            "BOOL": lambda: "bool",
            "NIL": lambda: None,
            "ANY": lambda: None,
            "LIST": lambda: self.value[0].type_name() if len(self.value) >= 1 else None,
            "MAP": lambda: self.value[0].type_name() if len(self.value) >= 1 else None,
            "GROUP": lambda: self.value[0].type_name() if len(self.value) >= 1 else None,
            "UNION": lambda: self.union_type(),
            "OTHER": lambda: self.my_types[self.value].type_name(),
        }[self.type]()

        return name

    # Name of the type of for the repeated part of this element.
    def repeated_type_name(self):
        if self.self_repeated_multi_var_condition():
            name = self.raw_type_name()
            if self.val_type_name() == name:
                name = name + "_"
        else:
            name = self.val_type_name()
        return name

    # Name of the type for this element.
    def type_name(self):
        if self.multi_var_condition():
            name = self.raw_type_name()
            if self.val_type_name() == name:
                name = name + "_"
            if self.repeated_type_name() == name:
                name = name + "_"
        else:
            name = self.repeated_type_name()
        return name

    # Take a multi member type name and create a variable declaration. Make it an array if the element is repeated.
    def add_var_name(self, var_type, full=False, anonymous=False):
        if var_type:
            assert(var_type[-1][-1] == "}" or len(var_type) == 1), f"Expected single var: {var_type!r}"
            if not anonymous or var_type[-1][-1] != "}":
                var_type[-1] += " %s%s" % (self.var_name(), "[%d]" % self.max_qty if full and self.max_qty != 1 else "")
            var_type = add_semicolon(var_type)
        return var_type

    # The type for this element as a member variable.
    def var_type(self):
        if not self.multi_val_condition() and self.val_type_name() is not None:
            return [self.val_type_name()]
        elif self.type == "UNION":
            return self.union_type()
        return []

    # Enclose a list of declarations in a block (struct, union or enum).
    def enclose(self, ingress, declaration):
        return [f"{ingress} {{"] + [indentation + line for line in declaration] + ["}"]

    # Type declaration for unions.
    def union_type(self):
        declaration = self.enclose("union", self.child_single_declarations())
        return declaration

    # Declaration of the repeated part of this element.
    def repeated_declaration(self):
        if self.is_unambiguous_repeated():
            return []

        var_type = self.var_type()
        multi_var = False

        decl = self.add_var_name(var_type, anonymous=(self.type == "UNION"))

        if self.type in ["LIST", "MAP", "GROUP"]:
            decl += self.child_declarations()
            multi_var = len(decl) > 1

        if self.key_var_condition():
            key_var = self.key.full_declaration()
            decl = key_var + decl
            multi_var = key_var != []

        if self.choice_var_condition():
            choice_var = self.choice_var()
            decl += choice_var
            multi_var = choice_var != []

        if self.cbor_var_condition():
            cbor_var = self.cbor.full_declaration()
            decl += cbor_var
            multi_var = cbor_var != []

        # if self.type not in ["LIST", "MAP", "GROUP"] or len(self.value) <= 1:
            # This assert isn't accurate for value lists with NIL or ANY
            # members.
            # assert multi_var == self.repeated_multi_var_condition(
            # ), f"""assert {multi_var} == {self.repeated_multi_var_condition()}
            # type: {self.type}
            # decl {decl}
            # self.key_var_condition() is {self.key_var_condition()}
            # self.key is {self.key}
            # self.key.full_declaration() is {self.key.full_declaration()}
            # self.cbor_var_condition() is {self.cbor_var_condition()}
            # self.choice_var_condition() is {self.choice_var_condition()}
            # self.value is {self.value}"""
        return decl

    # Declaration of the full type for this element.
    def full_declaration(self):
        multi_var = False

        if self.is_unambiguous():
            return []

        if self.multi_var_condition():
            if self.is_unambiguous_repeated():
                decl = []
            else:
                decl = self.add_var_name(
                    [self.repeated_type_name()] if self.repeated_type_name() is not None else [], full=True)
        else:
            decl = self.repeated_declaration()

        if self.count_var_condition():
            count_var = self.count_var()
            decl += count_var
            multi_var = count_var != []

        if self.present_var_condition():
            present_var = self.present_var()
            decl += present_var
            multi_var = present_var != []

        assert multi_var == self.multi_var_condition()
        return decl

    # Return the type definition of this element. If there are multiple variables, wrap them in a struct so the function
    # always returns a single type with no name.
    # If full is False, only repeated part is used.
    def single_var_type(self, full=True):
        if full and self.multi_member():
            return self.enclose("struct", self.full_declaration())
        elif not full and self.repeated_multi_var_condition():
            return self.enclose("struct", self.repeated_declaration())
        else:
            return self.var_type()

    # Return the type definition of this element, and all its children + key +
    # cbor.
    def type_def(self):
        ret_val = []
        if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
            ret_val.extend([elem for typedef in [child.type_def() for child in self.value] for elem in typedef])
        if self.cbor_var_condition():
            ret_val.extend(self.cbor.type_def())
        if self.key_var_condition():
            ret_val.extend(self.key.type_def())
        if self.type == "OTHER":
            ret_val.extend(self.my_types[self.value].type_def())
        if self.repeated_type_def_condition():
            ret_val.extend([(self.single_var_type(full=False), self.repeated_type_name())])
        if self.type_def_condition():
            ret_val.extend([(self.single_var_type(), self.type_name())])
        return ret_val

    def single_func_prim_prefix(self):
        if self.type == "OTHER":
            return self.my_types[self.value].single_func_prim_prefix()
        return ({
            "INT": f"intx32",
            "UINT": f"uintx32",
            "NINT": f"intx32",
            "FLOAT": f"float",
            "BSTR": f"bstrx",
            "TSTR": f"tstrx",
            "BOOL": f"boolx",
            "NIL": f"nilx",
            "ANY": f"any",
        }[self.type])

    # Name of the encoder/decoder function for this element.
    def xcode_func_name(self):
        return f"{self.mode}{self.var_name()}"

    # Name of the encoder/decoder function for the repeated part of this element.
    def repeated_xcode_func_name(self):
        return f"{self.mode}_repeated{self.var_name()}"

    def single_func_prim_name(self, union_uint=False):
        """Function name for xcoding this type, when it is a primitive type"""
        func_prefix = self.single_func_prim_prefix()
        if self.mode == "decode":
            if not self.is_unambiguous_value():
                func = f"{func_prefix}_decode"
            elif not union_uint:
                func = f"{func_prefix}_expect"
            elif union_uint == "EXPECT":
                func = f"{func_prefix}_expect_union"
            elif union_uint == "DROP":
                return None
        else:
            if (not self.is_unambiguous_value()) or self.type in ["TSTR", "BSTR"]:
                func = f"{func_prefix}_encode"
            else:
                func = f"{func_prefix}_put"
        return func

    def single_func_prim(self, access, union_uint=None, ptr_result=False):
        """Return the function name and arguments to call to encode/decode this element. Only used when this element
        DOESN'T define its own encoder/decoder function (when it's a primitive type, for which functions already exist,
        or when the function is defined elsewhere ("OTHER"))
        """
        assert self.type not in ["LIST", "MAP"], "Must have wrapper function for list or map."

        if self.type == "OTHER":
            return self.my_types[self.value].single_func(access, union_uint)

        func_name = self.single_func_prim_name(union_uint)
        if func_name is None:
            return (None, None)

        if self.type in ["NIL", "ANY"]:
            arg = "NULL"
        elif not self.is_unambiguous_value():
            arg = deref_if_not_null(access)
        elif self.type in ["BSTR", "TSTR"]:
            arg = tmp_str_or_null(self.value)
        elif self.type in ["UINT", "INT", "NINT", "FLOAT", "BOOL"]:
            arg = ("(void *) " if ptr_result else "") + str(self.value).lower()  # Make False and True lower case
        else:
            arg = tmp_val_or_null(self.value)

        min_val = None
        max_val = None

        if self.value is None:
            if self.type in ["BSTR", "TSTR"]:
                min_val = self.min_size
                max_val = self.max_size
            else:
                min_val = self.min_value
                max_val = self.max_value
        return (func_name, arg)

    # Return the function name and arguments to call to encode/decode this element.
    def single_func(self, access=None, union_uint=None):
        if self.single_func_impl_condition():
            return (self.xcode_func_name(), deref_if_not_null(access or self.var_access()))
        else:
            return self.single_func_prim(access or self.val_access(), union_uint)

    # Return the function name and arguments to call to encode/decode the repeated
    # part of this element.
    def repeated_single_func(self, ptr_result=False):
        if self.repeated_single_func_impl_condition():
            return (self.repeated_xcode_func_name(), deref_if_not_null(self.repeated_val_access()))
        else:
            return self.single_func_prim(self.repeated_val_access(), ptr_result=ptr_result)

    def has_backup(self):
        return (self.cbor_var_condition() or self.type in ["LIST", "MAP", "UNION"])

    def num_backups(self):
        total = 0
        if self.key:
            total += self.key.num_backups()
        if self.cbor_var_condition():
            total += self.cbor.num_backups()
        if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
            total += max([child.num_backups() for child in self.value] + [0])
        if self.type == "OTHER":
            total += self.my_types[self.value].num_backups()
        if self.has_backup():
            total += 1
        return total

    # Return a number indicating how many other elements this element depends on. Used putting functions and typedefs
    # in the right order.
    def depends_on(self):
        ret_vals = [1]

        if not self.dependsOnCall:
            self.dependsOnCall = True
            if self.cbor_var_condition():
                ret_vals.append(self.cbor.depends_on())
            if self.key:
                ret_vals.append(self.key.depends_on())
            if self.type == "OTHER":
                ret_vals.append(1 + self.my_types[self.value].depends_on())
            if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
                ret_vals.extend(child.depends_on() for child in self.value)
            self.dependsOnCall = False

        return max(ret_vals)

    # Make a string from the list returned by single_func_prim()
    def xcode_single_func_prim(self, union_uint=None):
        return xcode_statement(*self.single_func_prim(self.val_access(), union_uint))

    # Recursively sum the total minimum and maximum element count for this
    # element.
    def list_counts(self):
        retval = ({
            "INT": lambda: (self.min_qty, self.max_qty),
            "UINT": lambda: (self.min_qty, self.max_qty),
            "NINT": lambda: (self.min_qty, self.max_qty),
            "FLOAT": lambda: (self.min_qty, self.max_qty),
            "BSTR": lambda: (self.min_qty, self.max_qty),
            "TSTR": lambda: (self.min_qty, self.max_qty),
            "BOOL": lambda: (self.min_qty, self.max_qty),
            "NIL": lambda: (self.min_qty, self.max_qty),
            "ANY": lambda: (self.min_qty, self.max_qty),
            # Lists are their own element
            "LIST": lambda: (self.min_qty, self.max_qty),
            # Maps are their own element
            "MAP": lambda: (self.min_qty, self.max_qty),
            "GROUP": lambda: (self.min_qty * sum((child.list_counts()[0] for child in self.value)),
                              self.max_qty * sum((child.list_counts()[1] for child in self.value))),
            "UNION": lambda: (self.min_qty * min((child.list_counts()[0] for child in self.value)),
                              self.max_qty * max((child.list_counts()[1] for child in self.value))),
            "OTHER": lambda: (self.min_qty * self.my_types[self.value].list_counts()[0],
                              self.max_qty * self.my_types[self.value].list_counts()[1]),
        }[self.type]())
        return retval

    # Return the full code needed to encode/decode a "LIST" or "MAP" element with children.
    def xcode_list(self):
        start_func = f"{self.type.lower()}_start_{self.mode}"
        end_func = f"{self.type.lower()}_end_{self.mode}"
        assert start_func in ["list_start_decode", "list_start_encode", "map_start_decode", "map_start_encode"]
        assert end_func in ["list_end_decode", "list_end_encode", "map_end_decode", "map_end_encode"]
        assert self.type in ["LIST", "MAP"], "Expected LIST or MAP type, was %s." % self.type
        min_counts, max_counts = zip(*(child.list_counts() for child in self.value)) if self.value else ((0,), (0,))
        count_arg = f', {str(sum(max_counts))}' if self.mode == 'encode' else ''
        with_children = "(%s && (int_res = (%s), ((%s) && int_res)))" % (
            f"{start_func}(state{count_arg})",
            f"{newl_ind}&& ".join(child.full_xcode() for child in self.value),
            f"{end_func}(state{count_arg})")
        without_children = "(%s && %s)" % (
            f"{start_func}(state{count_arg})",
            f"{end_func}(state{count_arg})")
        return with_children if len(self.value) > 0 else without_children

    # Return the full code needed to encode/decode a "GROUP" element's children.
    def xcode_group(self, union_uint=None):
        assert self.type in ["GROUP"], "Expected GROUP type."
        return "(%s)" % (newl_ind + "&& ").join(
            [self.value[0].full_xcode(union_uint)]
            + [child.full_xcode() for child in self.value[1:]])

    # Return the full code needed to encode/decode a "UNION" element's children.
    def xcode_union(self):
        assert self.type in ["UNION"], "Expected UNION type."
        if self.mode == "decode":
            if self.all_children_uint_disambiguated():
                lines = []
                lines.extend(
                    ["((%s == %s) && (%s))" %
                        (self.choice_var_access(), child.var_name(),
                            child.full_xcode(union_uint="DROP"))
                        for child in self.value])
                return "((%s) && (%s))" % (
                    f"(uintx32_{self.mode}(state, (uint32_t *)&{self.choice_var_access()}))",
                    f"{newl_ind}|| ".join(lines), )
            child_values = ["(%s && ((%s = %s) || 1))" %
                            (child.full_xcode(union_uint="EXPECT" if child.is_uint_disambiguated() else None),
                                self.choice_var_access(), child.var_name())
                            for child in self.value]

            # Reset state for all but the first child.
            for i in range(1, len(child_values)):
                if ((not self.value[i].is_uint_disambiguated())
                        and (self.value[i].simple_func_condition()
                             or self.value[i - 1].simple_func_condition())):
                    child_values[i] = f"(union_elem_code(state) && {child_values[i]})"

            return "(%s && (int_res = (%s), %s, int_res))" \
                % ("union_start_code(state)",
                   f"{newl_ind}|| ".join(child_values),
                   "union_end_code(state)")
        else:
            return ternary_if_chain(
                self.choice_var_access(),
                [child.var_name() for child in self.value],
                [child.full_xcode() for child in self.value])

    def xcode_bstr(self):
        if self.cbor_var_condition():
            xcode_cbor = "(%s)" % ((newl_ind + "&& ").join(
                [f"(int_res = (bstrx_cbor_start_{self.mode}(state, &{self.val_access()})",
                 f"{self.cbor.full_xcode()})), bstrx_cbor_end_{self.mode}(state), int_res"]))
            if self.mode == "decode":
                return xcode_cbor
            else:
                return f"({self.val_access()}.value ? ({self.xcode_single_func_prim()}) : ({xcode_cbor}))"
        return self.xcode_single_func_prim()

    def xcode_tags(self):
        return [f"tag_{self.mode if (self.mode == 'encode') else 'expect'}(state, {tag})" for tag in self.tags]

    def range_checks(self, access):
        if self.value is not None:
            return []

        # return []
        range_checks = []

        if self.type in ["INT", "UINT", "NINT", "FLOAT", "BOOL"]:
            if self.min_value is not None:
                range_checks.append(f"({access} >= {self.min_value})")
            if self.max_value is not None:
                range_checks.append(f"({access} <= {self.max_value})")
        elif self.type in ["BSTR", "TSTR"]:
            if self.min_size is not None:
                range_checks.append(f"({access}.len >= {self.min_size})")
            if self.max_size is not None:
                range_checks.append(f"({access}.len <= {self.max_size})")
        elif self.type == "OTHER":
            range_checks.extend(self.my_types[self.value].range_checks(access))

        return range_checks

    # Return the full code needed to encode/decode this element, including children,
    # key and cbor, excluding repetitions.
    def repeated_xcode(self, union_uint=None):
        range_checks = self.range_checks(self.val_access())
        xcoder = {
            "INT": self.xcode_single_func_prim,
            "UINT": lambda: self.xcode_single_func_prim(union_uint),
            "NINT": self.xcode_single_func_prim,
            "FLOAT": self.xcode_single_func_prim,
            "BSTR": self.xcode_bstr,
            "TSTR": self.xcode_single_func_prim,
            "BOOL": self.xcode_single_func_prim,
            "NIL": self.xcode_single_func_prim,
            "ANY": self.xcode_single_func_prim,
            "LIST": self.xcode_list,
            "MAP": self.xcode_list,
            "GROUP": lambda: self.xcode_group(union_uint),
            "UNION": self.xcode_union,
            "OTHER": lambda: self.xcode_single_func_prim(union_uint),
        }[self.type]
        xcoders = []
        if self.key:
            xcoders.append(self.key.full_xcode(union_uint))
        if self.tags:
            xcoders.extend(self.xcode_tags())
        if self.mode == "decode":
            xcoders.append(xcoder())
            xcoders.extend(range_checks)
        else:
            xcoders.extend(range_checks)
            xcoders.append(xcoder())

        return "(%s)" % ((newl_ind + "&& ").join(xcoders),)

    # Code for the size of the repeated part of this element.
    def result_len(self):
        if self.repeated_type_name() is None or self.is_unambiguous_repeated():
            return "0"
        else:
            return "sizeof(%s)" % self.repeated_type_name()

    # Return the full code needed to encode/decode this element, including children,
    # key, cbor, and repetitions.
    def full_xcode(self, union_uint=None):
        if self.present_var_condition():
            func, *arguments = self.repeated_single_func(ptr_result=True)
            return (
                f"present_{self.mode}(&(%s), (void *)%s, %s)" %
                (self.present_var_access(),
                 func,
                 xcode_args(*arguments),))
        elif self.count_var_condition():
            func, *arguments = self.repeated_single_func(ptr_result=True)
            return (
                f"multi_{self.mode}(%s, %s, &%s, (void *)%s, %s, %s)" %
                (self.min_qty,
                 self.max_qty,
                 self.count_var_access(),
                 func,
                 xcode_args(*arguments),
                 self.result_len()))
        else:
            return self.repeated_xcode(union_uint)

    # Return the body of the encoder/decoder function for this element.
    def xcode(self):
        return self.full_xcode()

    # Recursively return a list of the bodies of the encoder/decoder functions for
    # this element and its children + key + cbor.
    def xcoders(self):
        if self.type in ["LIST", "MAP", "GROUP", "UNION"]:
            for child in self.value:
                for xcoder in child.xcoders():
                    yield xcoder
        if self.cbor:
            for xcoder in self.cbor.xcoders():
                yield xcoder
        if self.key:
            for xcoder in self.key.xcoders():
                yield xcoder
        if self.type == "OTHER" and self.value not in self.entry_type_names:
            for xcoder in self.my_types[self.value].xcoders():
                yield xcoder
        if self.repeated_single_func_impl_condition():
            yield XcoderTuple(self.repeated_xcode(), self.repeated_xcode_func_name(), self.repeated_type_name())
        if (self.single_func_impl_condition()):
            xcode_body = self.xcode()
            yield XcoderTuple(xcode_body, self.xcode_func_name(), self.type_name())

    def public_xcode_func_sig(self):
        return f"""
bool cbor_{self.xcode_func_name()}(
		{"const " if self.mode == "decode" else ""}uint8_t *payload, uint32_t payload_len,
		{"" if self.mode == "decode" else "const "}{self.type_name()} *{struct_ptr_name(self.mode)},
		{"uint32_t *payload_len_out"})"""

    def type_test_xcode_func_sig(self):
        return f"""
__attribute__((unused)) static bool type_test_{self.xcode_func_name()}(
		{"" if self.mode == "decode" else "const "}{self.type_name()} *{struct_ptr_name(self.mode)})"""


class CodeRenderer():
    def __init__(self, entry_types, mode, print_time, default_max_qty):
        self.entry_types = entry_types
        self.mode = mode
        self.print_time = print_time
        self.default_max_qty = default_max_qty

        # Sort type definitions so the typedefs will come in the correct order in the header file and the function in
        # the correct order in the c file.
        self.sorted_types = list(sorted(self.entry_types, key=lambda _type: _type.depends_on(), reverse=False))

        self.functions = self.unique_funcs()
        self.functions = self.used_funcs()
        self.type_defs = self.unique_types()

    def header_guard(self, file_name):
        return path.basename(file_name).replace(".", "_").replace("-", "_").upper() + "__"

    # Return a list of typedefs for all defined types, with duplicate typedefs
    # removed.
    def unique_types(self):
        type_names = {}
        out_types = []
        for mtype in self.sorted_types:
            for type_def in mtype.type_def():
                type_name = type_def[1]
                if type_name not in type_names.keys():
                    type_names[type_name] = type_def[0]
                    out_types.append(type_def)
                else:
                    assert (
                        ''.join(type_names[type_name]) == ''.join(type_def[0])), \
                        "Two elements share the type name %s, but their implementations are not identical. " \
                        + "Please change one or both names. One of them is %s" \
                        % (type_name, pformat(mtype.type_def()))
        return out_types

    # Return a list of encoder/decoder functions for all defined types, with duplicate
    # functions removed.
    def unique_funcs(self):
        func_names = {}
        out_types = []
        for mtype in self.sorted_types:
            xcoders = list(mtype.xcoders())
            for funcType in xcoders:
                func_xcode = funcType[0]
                func_name = funcType[1]
                if func_name not in func_names.keys():
                    func_names[func_name] = funcType
                    out_types.append(funcType)
                elif func_name in func_names.keys():
                    assert func_names[func_name][0] == func_xcode,\
                        ("Two elements share the function name %s, but their implementations are not identical. "
                            + "Please change one or both names.\n\n%s\n\n%s") % \
                        (func_name, func_names[func_name][0], func_xcode)

        return out_types

    # Return a list of encoder/decoder functions for all defined types, with unused
    # functions removed.
    def used_funcs(self):
        mod_entry_types = [
            XcoderTuple(
                func_type.xcode(),
                func_type.xcode_func_name(),
                func_type.type_name()) for func_type in self.entry_types]
        out_types = [func_type for func_type in mod_entry_types]
        full_code = "".join([func_type[0] for func_type in mod_entry_types])
        for func_type in reversed(self.functions):
            func_name = func_type[1]
            if func_type not in mod_entry_types and search(r"%s\W" % func_name, full_code):
                full_code += func_type[0]
                out_types.append(func_type)
        return list(reversed(out_types))

    # Return a list of typedefs for all defined types, with unused types removed.
    def used_types(self, type_defs, entry_types):
        out_types = [typeDef for typeDef in entry_types]
        full_code = "".join(["".join(typeDef[0]) for typeDef in entry_types])
        for typeDef in reversed(type_defs):
            type_name = typeDef[1]
            if typeDef not in entry_types and search(r"%s\W" % type_name, full_code):
                full_code += "".join(typeDef[0])
                out_types.append(typeDef)
        return list(reversed(out_types))

    # Render a single decoding function with signature and body.
    def render_function(self, xcoder):
        body = xcoder.body
        return f"""
static bool {xcoder.func_name}(
		cbor_state_t *state, {"" if self.mode == "decode" else "const "}{
            xcoder.type_name if struct_ptr_name(self.mode) in body else "void"} *{struct_ptr_name(self.mode)})
{{
	cbor_print("%s\\n", __func__);
	{f"uint32_t temp_elem_counts[{body.count('temp_elem_count')}];" if "temp_elem_count" in body else ""}
	{"uint32_t *temp_elem_count = temp_elem_counts;" if "temp_elem_count" in body else ""}
	{"uint32_t current_list_num;" if "current_list_num" in body else ""}
	{"uint8_t const *payload_bak;" if "payload_bak" in body else ""}
	{"uint32_t elem_count_bak;" if "elem_count_bak" in body else ""}
	{"uint32_t tmp_value;" if "tmp_value" in body else ""}
	{"cbor_string_type_t tmp_str;" if "tmp_str" in body else ""}
	{"bool int_res;" if "int_res" in body else ""}

	bool tmp_result = ({ body });

	if (!tmp_result)
		cbor_trace();

	{"state->elem_count = temp_elem_counts[0];" if "temp_elem_count" in body else ""}
	return tmp_result;
}}""".replace("	\n", "")  # call replace() to remove empty lines.

    # Render a single entry function (API function) with signature and body.
    def render_entry_function(self, xcoder):
        func_name, func_arg = (xcoder.xcode_func_name(), struct_ptr_name(self.mode))
        return f"""
{xcoder.type_test_xcode_func_sig()}
{{
	/* This function should not be called, it is present only to test that
	 * the types of the function and struct match, since this information
	 * is lost with the casts in the entry function.
	 */
	return {func_name}(NULL, {func_arg});
}}

{xcoder.public_xcode_func_sig()}
{{
	return entry_function(payload, payload_len, (const void *){func_arg},
		payload_len_out, (void *){func_name},
		{xcoder.list_counts()[1]}, {xcoder.num_backups()});
}}"""

    # Render the entire generated C file contents.
    def render_c_file(self, header_file_name):
        return f"""/*
 * Generated with cddl_gen.py (https://github.com/NordicSemiconductor/cddl-gen){'''
 * at: ''' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') if self.print_time else ''}
 * Generated with a default_max_qty of {self.default_max_qty}
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_{self.mode}.h"
#include "{header_file_name}"

#if DEFAULT_MAX_QTY != {self.default_max_qty}
#error "The type file was generated with a different default_max_qty than this file"
#endif

{linesep.join([self.render_function(xcoder) for xcoder in self.functions])}

{linesep.join([self.render_entry_function(xcoder) for xcoder in self.entry_types])}
"""

    # Render the entire generated header file contents.
    def render_h_file(self, type_def_file, header_guard):
        return \
            f"""/*
 * Generated with cddl_gen.py (https://github.com/NordicSemiconductor/cddl-gen){'''
 * at: ''' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') if self.print_time else ''}
 * Generated with a default_max_qty of {self.default_max_qty}
 */

#ifndef {header_guard}
#define {header_guard}

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_{self.mode}.h"
#include "{type_def_file}"

#if DEFAULT_MAX_QTY != {self.default_max_qty}
#error "The type file was generated with a different default_max_qty than this file"
#endif

{(linesep+linesep).join([f"{xcoder.public_xcode_func_sig()};" for xcoder in self.entry_types])}


#endif /* {header_guard} */
"""

    def render_type_file(self, header_guard):
        return \
            f"""/*
 * Generated with cddl_gen.py (https://github.com/NordicSemiconductor/cddl-gen){'''
 * at: ''' + datetime.now().strftime('%Y-%m-%d %H:%M:%S') if self.print_time else ''}
 * Generated with a default_max_qty of {self.default_max_qty}
 */

#ifndef {header_guard}
#define {header_guard}

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_{self.mode}.h"

#define DEFAULT_MAX_QTY {self.default_max_qty}

{(linesep+linesep).join([f"{typedef[1]} {{{linesep} {linesep.join(typedef[0][1:])};" for typedef in self.type_defs])}


#endif /* {header_guard} */
"""

    def render(self, h_file, c_file, type_file_in):
        h_dir, h_name = path.split(h_file.name)
        type_file = type_file_in or open(path.join(h_dir, f"types_{h_name}"), 'w')

        # Create and populate the generated c and h file.
        makedirs("./" + path.dirname(c_file.name), exist_ok=True)

        print("Writing to " + c_file.name)
        c_file.write(self.render_c_file(header_file_name=h_name))

        makedirs("./" + h_dir, exist_ok=True)
        type_file_name = path.basename(type_file.name)

        print("Writing to " + h_file.name)
        h_file.write(self.render_h_file(type_def_file=type_file_name, header_guard=self.header_guard(h_file.name)))

        print("Writing to " + type_file_name)
        type_file.write(self.render_type_file(header_guard=self.header_guard(type_file_name)))


def parse_args():
    parser = ArgumentParser(
        description='''Parse a CDDL file and validate/convert between YAML, JSON, and CBOR.
Can also generate C code for validation/encoding/decoding of CBOR.''')

    parser.add_argument(
        "-c", "--cddl", required=True, type=FileType('r'), help="Path to input CDDL file.")
    parser.add_argument(
        "--default-max-qty", required=False, type=int, default=3,
        help="""Default maximum number of repetitions when no maximum
is specified. This is needed to construct complete C types.""")
    parser.add_argument(
        "-v", "--verbose", required=False, action="store_true", default=False,
        help="Print more information while parsing CDDL and generating code.")

    subparsers = parser.add_subparsers()
    code_parser = subparsers.add_parser(
        "code", description='''Parse a CDDL file and produce C code that validates and xcodes CBOR.
The output from this script is a C file and a header file. The header file
contains typedefs for all the types specified in the cddl input file, as well
as declarations to xcode functions for the types designated as entry types when
running the script. The c file contains all the code for decoding and validating
the types in the CDDL input file. All types are validated as they are xcoded.

Where a `bstr .cbor <Type>` is specified in the CDDL, AND the Type is an entry
type, the xcoder will not xcode the string, only provide a pointer into the
payload buffer. This is useful to reduce the size of typedefs, or to break up
decoding. Using this mechanism is necessary when the CDDL contains self-
referencing types, since the C type cannot be self referencing.

This script requires 'regex' for lookaround functionality not present in 're'.''',
        formatter_class=RawDescriptionHelpFormatter)

    code_parser.add_argument(
        "--output-c", "--oc", required=True, type=FileType('w'), help="Path to output C file.")
    code_parser.add_argument(
        "--output-h", "--oh", required=True, type=FileType('w'), help="Path to output header file.")
    code_parser.add_argument(
        "--output-h-types", "--oht", required=False, type=FileType('w'),
        help="Path to output header file with typedefs (shared between decode and encode).")
    code_parser.add_argument(
        "-t", "--entry-types", required=True, type=str, nargs="+",
        help="Names of the types which should have their xcode functions exposed.")
    code_parser.add_argument(
        "--time-header", required=False, action="store_true", default=False,
        help="Put the current time in a comment in the generated files.")
    code_parser.add_argument(
        "-d", "--decode", required=False, action="store_true", default=False, help="Generate decoding code.")
    code_parser.add_argument(
        "-e", "--encode", required=False, action="store_true", default=False, help="Generate encoding code.")
    code_parser.set_defaults(process=process_code)

    convert_parser = subparsers.add_parser(
        "convert", description='''Parse a CDDL file and verify/convert between CBOR and YAML/JSON.
The script decodes the CBOR/YAML/JSON data from a file or stdin
and verifies that it conforms to the CDDL description.
The script fails if the data does not conform. The script can
also be used to just verify.

JSON and YAML do not support all data types that CBOR/CDDL supports.
bytestrings (BSTR), tags, and maps with non-text keys need special handling:

All strings in JSON/YAML are text strings. If a BSTR is needed, use
a dict with a single entry, with "bstr" as the key, and the byte
string (as a hex string) as the value, e.g.
{"bstr": "0123456789abcdef"}.
The value can also be another type, e.g. which will be interpreted as a
BSTR with the given value as contents (in cddl: 'bstr .cbor SomeType').
E.g.
{"bstr": ["first element", 2, [3]]}

Dicts in JSON/YAML only support text strings for keys, so if a dict
needs other types of keys, add the key and value as two entries
named "n_key" and "n_val", where n is an arbitrary integer.
e.g. {"name": "foo", "0_key": 123, "0_val": "bar"}
which will conform to the CDDL {tstr => tstr, int => tstr}.

Lastly, tags are specified by a dict with two elements, e.g.
{"tag": 1234, "value": ["tagged string within list"]}''')

    convert_parser.add_argument(
        "-i", "--input", required=True, type=str,
        help='''Input data file. The option --input-as specifies how to interpret the contents.
Use "-" to indicate stdin.''')
    convert_parser.add_argument(
        "--input-as", required=False, choices=["yaml", "json", "cbor", "cborhex"],
        help='''Which format to interpret the input file as.
If omitted, the format is inferred from the file name.
.yaml, .yml => YAML, .json => JSON, .cborhex => CBOR as hex string, everything else => CBOR''')
    convert_parser.add_argument(
        "-o", "--output", required=False, type=str,
        help='''Output data file. The option --output-as specifies how to interpret the contents.
If --output is omitted, no conversion is done, only verification of
the input. Use "-" to indicate stdout.''')
    convert_parser.add_argument(
        "--output-as", required=False, choices=["yaml", "json", "cbor", "cborhex", "c_code"],
        help='''Which format to interpret the output file as.
If omitted, the format is inferred from the file name.
.yaml, .yml => YAML, .json => JSON, .c, .h => C code,, .cborhex => CBOR as hex string, everything else => CBOR''')
    convert_parser.add_argument(
        "--c-code-var-name", required=False, type=str,
        help='''Only relevant together with '--output-as c_code' or .c files.''')
    convert_parser.add_argument(
        "-t", "--entry-type", required=True, type=str,
        help='''Name of the type (from the CDDL) to interpret the data as.''')
    convert_parser.set_defaults(process=process_convert)

    return parser.parse_args()


def process_code(args):
    mode = "decode" if args.decode else "encode"

    if args.decode == args.encode:
        args.error("Please specify exactly one of --decode or --encode.")

    print("Parsing " + args.cddl.name)

    my_types = CodeGenerator.from_cddl(mode, args.cddl.read(), args.default_max_qty, mode, args.entry_types)

    renderer = CodeRenderer(entry_types=[my_types[entry] for entry in args.entry_types], mode=mode,
                            print_time=args.time_header,
                            default_max_qty=args.default_max_qty)

    renderer.render(args.output_h, args.output_c, args.output_h_types)


def process_convert(args):
    my_types = DataTranslator.from_cddl(args.cddl.read(), args.default_max_qty)

    # Parsing is done, pretty print the result.
    verbose_print(args.verbose, "Parsed CDDL types:")
    verbose_pprint(args.verbose, my_types)

    cddl = my_types[args.entry_type]
    _, in_file_ext = path.splitext(args.input)
    in_file_format = args.input_as or in_file_ext.strip(".")
    if in_file_format in ["yaml", "yml"]:
        f = sys.stdin if args.input == "-" else open(args.input, "r")
        cbor_str = cddl.from_yaml(f.read())
    elif in_file_format == "json":
        f = sys.stdin if args.input == "-" else open(args.input, "r")
        cbor_str = cddl.from_json(f.read())
    elif in_file_format == "cborhex":
        f = sys.stdin if args.input == "-" else open(args.input, "r")
        cbor_str = bytes.fromhex(f.read().replace("\n", ""))
        cddl.validate_str(cbor_str)
    else:
        f = sys.stdin.buffer if args.input == "-" else open(args.input, "rb")
        cbor_str = f.read()
        cddl.validate_str(cbor_str)

    if args.output is not None:
        _, out_file_ext = path.splitext(args.output)
        out_file_format = args.output_as or out_file_ext.strip(".")
        if out_file_format in ["yaml", "yml"]:
            f = sys.stdout if args.output == "-" else open(args.output, "w")
            f.write(cddl.str_to_yaml(cbor_str))
        elif out_file_format == "json":
            f = sys.stdout if args.output == "-" else open(args.output, "w")
            f.write(cddl.str_to_json(cbor_str))
        elif out_file_format in ["c", "h", "c_code"]:
            f = sys.stdout if args.output == "-" else open(args.output, "w")
            assert args.c_code_var_name is not None, "Must specify --c-code-var-name when outputting c code."
            f.write(cddl.str_to_c_code(cbor_str, args.c_code_var_name))
        elif out_file_format == "cborhex":
            f = sys.stdout if args.output == "-" else open(args.output, "w")
            f.write(sub(r"(.{1,64})", r"\1 ", cbor_str.hex()))  # Add newlines every 64 chars
        else:
            f = sys.stdout.buffer if args.output == "-" else open(args.output, "wb")
            f.write(cbor_str)


def main():
    args = parse_args()
    args.process(args)


if __name__ == "__main__":
    main()
