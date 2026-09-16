#!/usr/bin/env python3
"""Check the precompiled LuaJIT bundles against the sources they were built from.

Source/Misc/luajit/precompiled/ holds per-OS zips of a prebuilt LuaJIT so that
people building through the Projucer exporters (Visual Studio, Xcode) need no
local CMake -- see Doc/LUAJIT.md. Nothing links those binaries to the sources, so
a change to the vendored tree or to the CMake build silently leaves them stale
and a Projucer build links yesterday's LuaJIT.

Each bundle ships a manifest recording the git object ids of its *inputs*. This
compares those against the working tree and fails on any difference.

Inputs fingerprinted, per manifest key:

    luajit_src      Source/Misc/luajit/src       tree
    luajit_dynasm   Source/Misc/luajit/dynasm    tree
    luajit_relver   Source/Misc/luajit/.relver   blob
    cmake_luajit    cmake/luajit                 tree

Deliberately NOT fingerprinted:

  * Source/Misc/luajit as a whole -- precompiled/ lives inside it, so committing
    a bundle would change the hash that bundle records. Circular.
  * The build output itself. lib.exe stamps the archive header, every .obj
    carries IMAGE_FILE_HEADER.TimeDateStamp without /Brepro, zip stores mtimes,
    and the runner images bump MSVC and Xcode roughly monthly -- a byte compare
    would go red on identical source, on a schedule, and get ignored.
  * The compiler version. A library built from identical source by an older
    toolchain is still the library that was validated.

Exit code: 0 if every bundle is current, 1 on any drift or missing bundle.
"""
import argparse
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
PRECOMPILED = os.path.join("Source", "Misc", "luajit", "precompiled")

# manifest key -> repo-relative path whose git object id it records.
FINGERPRINT = {
    "luajit_src": "Source/Misc/luajit/src",
    "luajit_dynasm": "Source/Misc/luajit/dynasm",
    "luajit_relver": "Source/Misc/luajit/.relver",
    "cmake_luajit": "cmake/luajit",
}

# Bundles that must exist. Windows and macOS only: the Projucer exporters for
# those two are the whole point, and every Linux developer already has CMake.
PLATFORMS = ("windows", "macos")


def emit(text):
    print(text)
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as fh:
            fh.write(text + "\n")


def git_object_id(repo, path, rev="HEAD"):
    """Object id git records for `path` at `rev`: a tree id for a directory, a
    blob id for a file. Raises ValueError if the path is not tracked there."""
    try:
        out = subprocess.run(
            ["git", "-C", repo, "rev-parse", "{}:{}".format(rev, path)],
            check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as exc:
        raise ValueError("{} is not tracked at {} ({})".format(
            path, rev, exc.stderr.strip() or "git rev-parse failed"))
    return out.stdout.strip()


def report(platform, manifest_path, expected, repo):
    """Compare one manifest against the tree. Returns True on drift."""
    if not os.path.isfile(manifest_path):
        emit("- **{}: MISSING** -- no `{}`".format(
            platform, os.path.relpath(manifest_path, repo)))
        return True

    try:
        with open(manifest_path, encoding="utf-8") as fh:
            manifest = json.load(fh)
    except (OSError, ValueError) as exc:
        emit("- **{}: UNREADABLE** -- {}".format(platform, exc))
        return True

    stale = []
    for key, path in sorted(FINGERPRINT.items()):
        recorded = manifest.get(key)
        if recorded != expected[key]:
            stale.append((key, path, recorded, expected[key]))

    if not stale:
        emit("- {}: OK (built from {} by run {})".format(
            platform,
            (manifest.get("built_from_commit") or "?")[:12],
            manifest.get("built_by_run", "?")))
        return False

    emit("- **{}: STALE**".format(platform))
    for key, path, recorded, current in stale:
        emit("  `{}` ({}): bundle has `{}`, tree has `{}`".format(
            key, path, (recorded or "absent")[:12], current[:12]))
    return True


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--repo", default=REPO, help="path to the repository root")
    parser.add_argument("--rev", default="HEAD",
                        help="revision to fingerprint against (default HEAD)")
    parser.add_argument("--platform", action="append", choices=PLATFORMS,
                        help="check only this platform (repeatable)")
    parser.add_argument("--github-output", action="store_true",
                        help="write stale=true|false to $GITHUB_OUTPUT")
    args = parser.parse_args()

    platforms = tuple(args.platform) if args.platform else PLATFORMS

    try:
        expected = {key: git_object_id(args.repo, path, args.rev)
                    for key, path in FINGERPRINT.items()}
    except ValueError as exc:
        emit("::error::LuaJIT precompiled check could not fingerprint the tree: {}"
             .format(exc))
        return 1

    emit("### LuaJIT: precompiled bundles vs vendored source")
    drift = False
    for platform in platforms:
        manifest = os.path.join(args.repo, PRECOMPILED,
                                "manifest-{}.json".format(platform))
        drift |= report(platform, manifest, expected, args.repo)

    if args.github_output and os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as fh:
            fh.write("stale={}\n".format("true" if drift else "false"))

    if drift:
        emit("\nThe checked-in bundles no longer match the sources they were built "
             "from. Run the **LuaJIT** workflow (Actions -> LuaJIT -> Run workflow), "
             "download the `luajit-precompiled-<os>` artifacts, unzip each over "
             "`{}/`, and commit the `.zip` and `.json`. "
             "See Doc/LUAJIT.md, \"Regenerating the precompiled bundles\".".format(
                 PRECOMPILED))
        emit("::error::LuaJIT precompiled bundles are stale")
        return 1

    emit("\nNo drift.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
