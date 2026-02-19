# Developer Guide

This guide covers the development workflow and tooling for contributors to esp-matter.

## Pre-commit Hooks

The repository uses [pre-commit](https://pre-commit.com/) to run automated checks before each commit. Following hooks are configured:

1. astyle_py — C/C++ Code Formatter

   [astyle_py](https://github.com/espressif/astyle_py) enforces consistent C/C++ formatting.
   The formatting rules are defined in `tools/ci/astyle-rules.yml`.

2. keep-sorted — Sorted Block Enforcer

   [keep-sorted](https://github.com/google/keep-sorted) is used in documentation and
   source files to maintain alphabetical ordering of lists and sections.

3. codespell — Spell Checker

   [codespell](https://github.com/codespell-project/codespell) catches common
   misspellings in source code, documentation, and comments.
   Configuration is in `.codespellrc`.

### Setup

1. Install pre-commit

```bash
python3 -m pip install pre-commit
```

2. Install the hooks

From the repository root:

```bash
cd $ESP_MATTER_PATH
pre-commit install
```

This registers the hooks so they run automatically on `git commit`.

### Usage

Once installed, the hooks run on every `git commit` against the staged files. If a hook reformats a file, the commit is aborted — review the changes, `git add` the updated files, and commit again.

### Running locally

Run all hooks on every file in the repository:

```bash
pre-commit run --all-files
```

Run all hooks only on staged files (same as what happens on commit):

```bash
pre-commit run
```

Run a specific hook:

```bash
pre-commit run astyle_py --all-files
pre-commit run keep-sorted --all-files
pre-commit run codespell --all-files
```

Run hooks on specific files:

```bash
pre-commit run --files path/to/file.cpp path/to/other_file.h
```

Skip hooks for a one-off commit (not recommended):

```bash
git commit --no-verify
```
