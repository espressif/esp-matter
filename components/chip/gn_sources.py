#!/usr/bin/env python3
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

"""Write a GN library's C/C++ sources to a CMake file.

Records each source's defines and include directories, and the generated files
the build must create first. Sets CHIP_GN_SRCS, CHIP_GN_DEPENDS,
CHIP_GN_GENERATED_TARGETS and CHIP_GN_GENERATED_FILES.
"""

import argparse
import os
import shlex
import subprocess


def ninja(gn_dir, *args):
    result = subprocess.run(
        ["ninja", "-C", gn_dir, *args], capture_output=True, text=True
    )
    if result.returncode != 0:
        raise SystemExit(
            f"gn_sources.py: `ninja {' '.join(args[:2])}` failed:\n{result.stdout}{result.stderr}"
        )
    return result.stdout


def query(gn_dir, targets):
    """Return {target: (rule, explicit inputs, order-only inputs)} from `ninja -t query`."""
    edges = {}
    target = None
    for line in ninja(gn_dir, "-t", "query", *targets).splitlines():
        if not line.startswith(" "):
            target = line.rstrip(":")
        elif line.startswith("  input: "):
            edges[target] = (line.split(": ", 1)[1], [], [])
        elif line.startswith("  "):
            if not line.startswith("    "):
                target = None  # outputs: / validations: sections
            elif target in edges:
                item = line.strip()
                if item.startswith("|| "):
                    edges[target][2].append(item[3:])
                else:
                    edges[target][1].append(item.removeprefix("| "))
    return edges


def generated_outputs(gn_dir, targets):
    """Find concrete outputs behind the generated targets, including phony groups."""
    pending = set(targets)
    seen = set()
    outputs = set()
    while pending:
        edges = query(gn_dir, sorted(pending))
        seen.update(pending)
        pending = set()
        for target, (rule, inputs, order_only) in edges.items():
            if rule != "phony":
                outputs.add(target)
            pending.update(set(inputs + order_only) - seen)
    return sorted(outputs)


def split_ninja_value(value):
    """Undo ninja escaping and split the value into words."""
    return shlex.split(value.replace("$ ", "\\ ").replace("$:", ":").replace("$$", "$"))


def cmake_quote(value):
    for char in ("\\", '"', ";", "$"):
        value = value.replace(char, "\\" + char)
    return f'"{value}"'


def cmake_list(items):
    return "".join(f"\n    {cmake_quote(item)}" for item in items)


def read_target_flags(ninja_path, srcs, flags):
    """Record defines and include dirs for sources this target compiles."""
    defines, include_dirs = [], []
    with open(ninja_path) as ninja_file:
        for line in ninja_file:
            key, _, value = line.rstrip("\n").partition(" = ")
            if key == "defines":
                defines = [d.removeprefix("-D") for d in split_ninja_value(value)]
            elif key == "include_dirs":
                include_dirs = [i.removeprefix("-I") for i in split_ninja_value(value)]
            elif line.startswith("build "):
                # build <object>: <rule> <source> ...
                rule_and_inputs = line.partition(": ")[2].split()
                if (
                    rule_and_inputs[:1] in (["cc"], ["cxx"])
                    and rule_and_inputs[1] in srcs
                ):
                    flags[rule_and_inputs[1]] = (defines, include_dirs)


def read_compile_flags(gn_dir, srcs):
    flags = {}
    for dirpath, _, files in os.walk(os.path.join(gn_dir, "obj")):
        for name in files:
            if name.endswith(".ninja"):
                read_target_flags(os.path.join(dirpath, name), srcs, flags)
    return flags


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("gn_dir", help="`gn gen` output directory")
    parser.add_argument("output", help="CMake file to write")
    parser.add_argument("libs", nargs="+", help="GN libraries, e.g. lib/libCHIP.a")
    args = parser.parse_args()
    gn_dir = os.path.abspath(args.gn_dir)

    def path(p):
        return os.path.realpath(os.path.join(gn_dir, p))

    # Objects of the libraries, then the compile step of each object. `ninja -t query`
    # is used because the output of `ninja -t inputs` differs between ninja versions.
    objects = sorted(
        {
            o
            for _, inputs, _ in query(gn_dir, args.libs).values()
            for o in inputs
            if o.endswith(".o")
        }
    )
    compiles = [
        edge for edge in query(gn_dir, objects).values() if edge[0] in ("cc", "cxx")
    ]
    srcs = sorted({inputs[0] for _, inputs, _ in compiles})

    # Generated files: what the compile steps need first (their order-only inputs) and
    # every header under gen/include, which code outside the libraries (e.g. esp_matter)
    # also includes. They are built by the chip_codegen target at build time.
    generated = {dep for _, _, order_only in compiles for dep in order_only}
    for line in ninja(gn_dir, "-t", "targets", "all").splitlines():
        target = line.split(":", 1)[0]
        if target.startswith("gen/include/"):
            generated.add(target)
    generated_files = generated_outputs(gn_dir, generated)

    flags = read_compile_flags(gn_dir, set(srcs))
    missing = [src for src in srcs if src not in flags]
    if missing:
        raise SystemExit(f"gn_sources.py: no compile rule found for {missing[:5]}")

    with open(os.path.join(gn_dir, "build.ninja.d")) as depfile:
        gn_files = sorted({path(p) for p in depfile.read().partition(":")[2].split()})

    with open(args.output, "w") as out:
        out.write("# Generated by gn_sources.py. Do not edit.\n")
        out.write(f"set(CHIP_GN_SRCS{cmake_list(path(src) for src in srcs)})\n")
        out.write(f"set(CHIP_GN_DEPENDS{cmake_list(gn_files)})\n")
        out.write(f"set(CHIP_GN_GENERATED_TARGETS{cmake_list(sorted(generated))})\n")
        out.write(
            f"set(CHIP_GN_GENERATED_FILES{cmake_list(path(p) for p in generated_files)})\n"
        )
        for src in srcs:
            defines, include_dirs = flags[src]
            source = cmake_quote(path(src))
            out.write(
                f"set_property(SOURCE {source} PROPERTY COMPILE_DEFINITIONS{cmake_list(defines)})\n"
            )
            out.write(
                f"set_property(SOURCE {source} PROPERTY INCLUDE_DIRECTORIES"
                f"{cmake_list(path(i) for i in include_dirs)})\n"
            )


if __name__ == "__main__":
    main()
