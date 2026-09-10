#!/usr/bin/env python3
"""Check cmake/luajit/CMakeLists.txt against the vendored LuaJIT src/Makefile.

CMakeLists.txt hand-transcribes three source lists and one set of DynASM flags from
upstream's src/Makefile, because upstream ships no CMake build. Nothing links the two,
so an upgrade of the vendored tree can change the Makefile without anyone noticing --
and LJLIB_C's *order* fixes fast-function and bytecode numbering, so drift there is a
silent miscompile rather than a build error.

This parses both sides and compares them directly. There is no blessed copy to keep in
step: after a legitimate upgrade you fix CMakeLists.txt and the check goes green.

Compared:

    Makefile LJLIB_O   (:.o=.c)  vs  CMake LJ_LIB_C      ordered
    Makefile LJCORE_O  (:.o=.c)  vs  CMake LJ_CORE_C     ordered
    Makefile BUILDVM_O (:.o=.c)  vs  CMake LJ_BUILDVM_C  ordered
    Makefile DASM_AFLAGS tokens  vs  tokens _luajit_detect() emits, as sets

Exit code: 0 if the two agree, 1 on any drift.
"""
import argparse
import os
import re
import sys

# DynASM flags the Makefile can emit that CMakeLists.txt deliberately does not.
#
# All of these belong to targets CtrlrX does not build -- ppc (SQRT, ROUND, GPR64, and
# PPE/TOC for PS3), mips (MIPSR6) and 32-bit arm on iOS (IOS). _luajit_detect() rejects
# any architecture outside x86/x64/arm/arm64 with a FATAL_ERROR, so these can never be
# reached. Listing them here documents the gap rather than hiding it: a Makefile token
# that is in neither this set nor CMakeLists.txt is real drift.
UNSUPPORTED_ARCH_TOKENS = {
    "GPR64",    # ppc, LJ_ARCH_PPC32ON64
    "IOS",      # arm, TARGET_SYS=iOS
    "MIPSR6",   # mips
    "PPE",      # ppc, TARGET_SYS=PS3
    "ROUND",    # ppc, LJ_ARCH_ROUND
    "SQRT",     # ppc, LJ_ARCH_SQRT
    "TOC",      # ppc, TARGET_SYS=PS3
}

HERE = os.path.dirname(os.path.abspath(__file__))
DEFAULT_CMAKE = os.path.join(HERE, "CMakeLists.txt")
DEFAULT_MAKEFILE = os.path.join(HERE, "..", "..", "Source", "Misc", "luajit", "src", "Makefile")


# ---------------------------------------------------------------------------
# Makefile side
# ---------------------------------------------------------------------------
def read_make_vars(text):
    """Return {name: raw value} for every ``NAME= ...`` assignment, joining
    backslash-continued lines. Later assignments of a name win, as make does."""
    text = re.sub(r"\\\n", " ", text)
    variables = {}
    for line in text.splitlines():
        match = re.match(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*\+?=\s*(.*)$", line)
        if match:
            name, value = match.group(1), match.group(2)
            if line.lstrip().startswith(name + "+="):
                variables[name] = variables.get(name, "") + " " + value
            else:
                variables[name] = value
    return variables


def expand(variables, name, seen=None):
    """Expand ``$(NAME)`` references and ``$(NAME:.o=.c)`` substitution, recursively."""
    seen = seen or set()
    if name in seen:
        raise ValueError("recursive make variable: {}".format(name))
    if name not in variables:
        raise ValueError("make variable not found: {}".format(name))

    out = []
    for token in variables[name].split():
        ref = re.match(r"^\$\(([A-Za-z_][A-Za-z0-9_]*)(?::([^=]*)=([^)]*))?\)$", token)
        if ref:
            nested = expand(variables, ref.group(1), seen | {name})
            if ref.group(2) is not None:
                old, new = ref.group(2), ref.group(3)
                nested = [n[: -len(old)] + new if n.endswith(old) else n for n in nested]
            out.extend(nested)
        elif token.startswith("$"):
            raise ValueError("unhandled make expansion in {}: {}".format(name, token))
        else:
            out.append(token)
    return out


def make_sources(variables, name):
    """Expand an object list and turn it back into the .c files it is built from."""
    return [re.sub(r"\.o$", ".c", entry) for entry in expand(variables, name)]


def make_dasm_tokens(text):
    """Every ``-D TOKEN`` appearing on a DASM_AFLAGS assignment, as a set.

    Some lines carry two (``-D PPE -D TOC``), and VER arrives as ``-D VER=$(subst ...)``,
    so the value is dropped and only the token name kept."""
    text = re.sub(r"\\\n", " ", text)
    tokens = set()
    for line in text.splitlines():
        if re.match(r"^\s*DASM_AFLAGS\s*\+?=", line):
            for token in re.findall(r"-D\s+([A-Za-z_][A-Za-z0-9_]*)", line):
                tokens.add(token)
    return tokens


# ---------------------------------------------------------------------------
# CMake side
# ---------------------------------------------------------------------------
def cmake_list(text, name):
    """The entries of a ``set(NAME a b c)`` block, expanding ``${OTHER_LIST}``."""
    match = re.search(r"^\s*set\(\s*" + re.escape(name) + r"\b(.*?)\)\s*$",
                      text, re.MULTILINE | re.DOTALL)
    if not match:
        raise ValueError("set({} ...) not found in CMakeLists.txt".format(name))

    body = re.sub(r"#[^\n]*", "", match.group(1))
    out = []
    for token in body.split():
        ref = re.match(r"^\$\{([A-Za-z_][A-Za-z0-9_]*)\}$", token)
        if ref:
            out.extend(cmake_list(text, ref.group(1)))
        elif token.startswith("$"):
            raise ValueError("unhandled CMake expansion in {}: {}".format(name, token))
        else:
            out.append(token.strip('"'))
    return out


def cmake_dasm_tokens(text):
    """Every ``-D TOKEN`` _luajit_detect() can pass to dynasm, as a set.

    Covers both branches: the MSVC table and the lj_arch.h probe. VER is emitted as
    ``-D "VER=<n>"``, so quotes and value are stripped."""
    match = re.search(r"^function\(_luajit_detect\b.*?^endfunction\(\)",
                      text, re.MULTILINE | re.DOTALL)
    if not match:
        raise ValueError("function(_luajit_detect ...) not found in CMakeLists.txt")

    body = re.sub(r"#[^\n]*", "", match.group(0))
    return set(re.findall(r'-D\s+"?([A-Za-z_][A-Za-z0-9_]*)', body))


# ---------------------------------------------------------------------------
# Reporting
# ---------------------------------------------------------------------------
def emit(text):
    print(text)
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as fh:
            fh.write(text + "\n")


def report_list(label, make_var, cmake_var, expected, actual):
    """Compare two ordered lists. Returns True on drift."""
    if expected == actual:
        emit("- {}: OK ({} entries)".format(label, len(expected)))
        return False

    emit("- **{}: DRIFT**".format(label))
    emit("  Makefile `{}` and CMakeLists.txt `set({} ...)` disagree.".format(make_var, cmake_var))

    missing = [e for e in expected if e not in actual]
    extra = [a for a in actual if a not in expected]
    if missing:
        emit("  Missing from `{}` (add, in Makefile order): {}".format(
            cmake_var, ", ".join(missing)))
    if extra:
        emit("  Not in the Makefile any more, remove from `{}`: {}".format(
            cmake_var, ", ".join(extra)))
    if not missing and not extra:
        emit("  Same entries, different ORDER -- significant for {}.".format(cmake_var))

    for index, (want, got) in enumerate(zip(expected, actual)):
        if want != got:
            emit("  First difference at position {}: Makefile has `{}`, CMake has `{}`."
                 .format(index, want, got))
            break
    else:
        emit("  Lists agree up to position {}, then differ in length ({} vs {})."
             .format(min(len(expected), len(actual)), len(expected), len(actual)))

    emit("::error::LuaJIT source list drift: {} does not match the Makefile's {}"
         .format(cmake_var, make_var))
    return True


def report_dasm(make_tokens, cmake_tokens):
    """Compare the DynASM flag token sets. Returns True on drift."""
    unhandled = make_tokens - cmake_tokens - UNSUPPORTED_ARCH_TOKENS
    unknown = cmake_tokens - make_tokens

    if not unhandled and not unknown:
        emit("- DASM flags: OK ({} shared, {} allowlisted as unsupported-arch)".format(
            len(make_tokens & cmake_tokens), len(make_tokens & UNSUPPORTED_ARCH_TOKENS)))
        return False

    emit("- **DASM flags: DRIFT**")
    if unhandled:
        emit("  In the Makefile's DASM_AFLAGS but not handled by `_luajit_detect()`: {}"
             .format(", ".join(sorted(unhandled))))
        emit("  Either handle them in `_luajit_detect()`, or -- if they belong to an "
             "architecture CtrlrX does not build -- add them to UNSUPPORTED_ARCH_TOKENS "
             "in this script, with a comment saying which.")
    if unknown:
        emit("  Passed by `_luajit_detect()` but no longer in the Makefile: {}"
             .format(", ".join(sorted(unknown))))
        emit("  Upstream dropped these; remove them from `_luajit_detect()`.")

    emit("::error::LuaJIT DynASM flag drift between the Makefile and _luajit_detect()")
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--cmake", default=DEFAULT_CMAKE,
                        help="path to cmake/luajit/CMakeLists.txt")
    parser.add_argument("--makefile", default=DEFAULT_MAKEFILE,
                        help="path to the vendored LuaJIT src/Makefile")
    args = parser.parse_args()

    for path in (args.cmake, args.makefile):
        if not os.path.isfile(path):
            emit("::error::LuaJIT drift check: file not found: {}".format(path))
            return 1

    with open(args.makefile, encoding="utf-8", errors="replace") as fh:
        makefile = fh.read()
    with open(args.cmake, encoding="utf-8") as fh:
        cmakelists = fh.read()

    try:
        variables = read_make_vars(makefile)
        comparisons = [
            ("LJLIB_C", "LJLIB_O", "LJ_LIB_C",
             make_sources(variables, "LJLIB_O"), cmake_list(cmakelists, "LJ_LIB_C")),
            ("LJCORE_C", "LJCORE_O", "LJ_CORE_C",
             make_sources(variables, "LJCORE_O"), cmake_list(cmakelists, "LJ_CORE_C")),
            ("BUILDVM_C", "BUILDVM_O", "LJ_BUILDVM_C",
             make_sources(variables, "BUILDVM_O"), cmake_list(cmakelists, "LJ_BUILDVM_C")),
        ]
        make_tokens = make_dasm_tokens(makefile)
        cmake_tokens = cmake_dasm_tokens(cmakelists)
    except ValueError as exc:
        emit("::error::LuaJIT drift check could not parse its inputs: {}".format(exc))
        return 1

    emit("### LuaJIT: CMakeLists.txt vs src/Makefile")
    drift = False
    for label, make_var, cmake_var, expected, actual in comparisons:
        drift |= report_list(label, make_var, cmake_var, expected, actual)
    drift |= report_dasm(make_tokens, cmake_tokens)

    if drift:
        emit("\n`{}` no longer matches `{}`. See Doc/LUAJIT.md, "
             "\"Upgrading the vendored LuaJIT\"."
             .format(os.path.relpath(args.cmake), os.path.relpath(args.makefile)))
        return 1

    emit("\nNo drift.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
