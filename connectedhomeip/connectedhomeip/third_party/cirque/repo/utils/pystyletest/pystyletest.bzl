"""
Build rules for pystyletest.
"""

load("@rules_python//python:defs.bzl", "py_test")
load("@cirque_deps//:requirements.bzl", "requirement")

def py_style_test(name, srcs, deps = []):
    src_expanded_paths = ["$(location " + src + ")" for src in srcs]

    py_test(
        name = name,
        srcs = ["@//utils/pystyletest:styletest.py"],
        main = "@//utils/pystyletest:styletest.py",
        args = src_expanded_paths,
        data = srcs,
        deps = deps + [
            requirement("pylint"),
            requirement("pycodestyle"),
        ],
    )
