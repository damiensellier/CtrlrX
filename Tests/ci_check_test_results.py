#!/usr/bin/env python3
"""Evaluate GoogleTest JUnit XML for CI checks.

Reads one or more JUnit result files (gtest --gtest_output=xml / GTEST_OUTPUT) and
decides pass/fail. With --allow <file>, failures whose fully-qualified name is listed in
that allowlist are tolerated (used by the "Tests (excluding known failures)" check);
without it, any failure fails the run (the "Tests" check).

A test's fully-qualified name is its JUnit ``classname`` + "." + ``name`` -- the same
string gtest prints after "[ FAILED ]".

A test that crashes, times out or is killed writes no gtest XML, so on its own this check
can't see it. With --ctest-junit <file> (ctest --output-junit), every ctest case that
failed must be explained by a failing or allowlisted JUnit case that belongs to it: the
same name, a parameterised instance of it (ctest ``Suite/Fixture.test/("a", "b")`` is gtest
``Suite/Fixture.test/<n>``), or a case of a suite it writes (``PanelProbeSelfTest`` writes
``PanelProbeSelfTest.scenario_NN_*``). Any other failed ctest case is an unexpected failure.
(Added v5.6.37)

Exit code: 0 if no unexpected failures, 1 otherwise (also 1 if no results were found).
"""
import argparse
import glob
import os
import sys
import xml.etree.ElementTree as ET


def load_allowlist(path):
    allowed = set()
    with open(path, encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            if line and not line.startswith("#"):
                allowed.add(line)
    return allowed


def collect(paths):
    """Return (all_names, failing_names) across every result file found."""
    all_names, failing = set(), set()
    files = []
    for pattern in paths:
        files.extend(glob.glob(pattern, recursive=True))
    for path in sorted(set(files)):
        root = ET.parse(path).getroot()
        for case in root.iter("testcase"):
            fqn = "{}.{}".format(case.get("classname", ""), case.get("name", ""))
            all_names.add(fqn)
            if case.find("failure") is not None or case.find("error") is not None:
                failing.add(fqn)
    return all_names, failing, files


def collect_ctest_failures(paths):
    """Return (ctest cases that failed, files) from ctest --output-junit files."""
    failed, files = set(), []
    for pattern in paths:
        files.extend(glob.glob(pattern, recursive=True))
    for path in sorted(set(files)):
        for case in ET.parse(path).getroot().iter("testcase"):
            if case.get("status") == "fail" or case.find("failure") is not None:
                failed.add(case.get("name", ""))
    return failed, files


def belongs_to(fqn, ctest_name):
    """True if the JUnit case `fqn` is part of the ctest case `ctest_name` (see the docstring)."""
    if fqn == ctest_name or fqn.startswith(ctest_name + "."):
        return True
    cut = ctest_name.find("/(")
    return cut > 0 and ctest_name.endswith(")") and fqn.startswith(ctest_name[:cut] + "/")


def emit(text):
    print(text)
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as fh:
            fh.write(text + "\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--allow", metavar="FILE",
                        help="file of allowlisted fully-qualified test names")
    parser.add_argument("--ctest-junit", metavar="FILE", action="append", default=[],
                        help="ctest --output-junit file or glob; checks that every failed ctest case "
                             "has a failing or allowlisted JUnit case (catches crashes and timeouts)")
    parser.add_argument("results", nargs="+",
                        help="JUnit XML files or globs (** supported)")
    args = parser.parse_args()

    allowed = load_allowlist(args.allow) if args.allow else set()
    all_names, failing, files = collect(args.results)

    if not files:
        emit("::error::No test result files matched: {}".format(" ".join(args.results)))
        return 1

    unexpected = sorted(failing - allowed)

    # Failed ctest cases with no failing or allowlisted JUnit case: a crash, a timeout, or a
    # non-zero exit after the gtest XML said "passed".
    unexplained = []
    if args.ctest_junit:
        ctest_failed, ctest_files = collect_ctest_failures(args.ctest_junit)
        if not ctest_files:
            emit("::error::No ctest JUnit file matched: {}".format(" ".join(args.ctest_junit)))
            return 1
        unexplained = sorted(c for c in ctest_failed
                             if not any(belongs_to(f, c) for f in failing | allowed))
    # Allowlisted names that ran and passed -> the list is stale.
    stale = sorted((allowed & all_names) - failing)
    # Allowlisted names that ran and (still) failed -> known, expected failures.
    known_failing = sorted(failing & allowed)

    emit("### {}".format("Tests (excluding known failures)" if args.allow else "Tests"))
    emit("- result files: {}".format(len(files)))
    emit("- testcases: {}".format(len(all_names)))
    emit("- failing: {} ({} allowlisted)".format(len(failing), len(failing & allowed)))
    if args.ctest_junit:
        emit("- ctest cases failed without a JUnit failure: {}".format(len(unexplained)))

    # Collapsible breakdown so a reader can tell the known/expected failures apart from a real
    # problem without cross-referencing known_failures.txt. Only meaningful with an allowlist.
    if args.allow:
        emit("<details><summary>**Still failing (known): {}**</summary>".format(len(known_failing)))
        for name in known_failing:
            emit("- {}".format(name))
        if not known_failing:
            emit("- none")
        emit("\n</details>")

        emit("\n<details><summary>**Newly passing ({}) — remove from known_failures.txt**</summary>".format(len(stale)))
        for name in stale:
            emit("- {}".format(name))
        if not stale:
            emit("- none")
        emit("\n</details>")

    for name in stale:
        emit("::warning::Allowlisted test now passes, remove from known_failures: {}".format(name))

    if unexpected or unexplained:
        emit("- **unexpected failures: {}**".format(len(unexpected) + len(unexplained)))
        for name in unexpected:
            emit("::error::Unexpected test failure: {}".format(name))
        for name in unexplained:
            emit("::error::Unexpected test failure (crash, timeout or exit code; no JUnit "
                 "failure, see the ctest log): {}".format(name))
        return 1

    emit("- no unexpected failures")
    return 0


if __name__ == "__main__":
    sys.exit(main())
