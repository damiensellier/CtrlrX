# AGENTS.md

Rules for AI agents (and people) who change CtrlrX. They come from the maintainer, Damien Sellier. If one of your habits conflicts with a rule here, follow the rule.

CtrlrX is a JUCE-based MIDI panel host (Standalone, VST3, AU). Panels are XML documents with Lua
scripts. Upstream is [`damiensellier/CtrlrX`](https://github.com/damiensellier/CtrlrX);
contributors work on forks and send pull requests (PRs).

## Branches

| Branch | What it is | Who commits |
|---|---|---|
| `<version>-Maintenance`, e.g. `5.6.37-Maintenance` | The main line of one version. History is linear, with one commit per change. | Only the maintainer, as squashed commits. |
| Temporary update branch, e.g. `5.6.37-Maintenance-Tests-LUA-API-Sgorpi-UPDT`, `5.6.37-Maintenance-dnaldoog-20261107-UPDT` | Made by the maintainer from a Maintenance branch for one update. PRs are merged into it. When the update is stable and tested, all its commits are squashed into **one** commit on the Maintenance branch. | PRs |
| `master` | The latest release. | Don't target it. |

**Your work branch** (on your fork) uses the same naming, so the maintainer can reuse the name:

```
<base>-<Topic>-<Who>-<SUFFIX>
```

- `<base>`: the Maintenance branch you start from, e.g. `5.6.37-Maintenance`.
- `<Topic>`: a few words, joined with `-`, e.g. `Combo-Crash`, `JUCE-8`, `Lua-Timer`.
- `<Who>`: your GitHub name, e.g. `Sgorpi`, `Dnaldoog`.
- `<SUFFIX>`: `FIX`, `UPDT`, `ADDED` or `UPGRADE`, matching the commit type below.

Examples: `5.6.37-Maintenance-Midi-Monitor-Filters-Sgorpi-ADDED`,
`8.0.0-Maintenance-JUCE-8-Sgorpi-UPGRADE`.

## Workflow for one change

1. Fetch upstream and branch from the newest Maintenance branch.
2. One topic per branch and per PR. A topic can be one fix, one feature, one upgrade, or a few
   small related updates of one area.
3. Commit as often as you like on your branch. The maintainer squashes the PR into one commit.
4. Keep the branch current by **rebasing** on the Maintenance branch. Don't merge the Maintenance
   branch into your branch, and don't push merge commits.
5. Run CI on your fork first (see *Tests*). Then open the PR against the Maintenance branch, or
   the temporary branch the maintainer names. The maintainer may change the target.
6. If your PR needs another PR that is not merged yet, say so in the first line:
   `Depends on #N; review only the commits after <sha>.`

## Commit and PR titles

Every commit on a Maintenance branch has this title:

```
<version> <TYPE> <summary>
```

| TYPE | Use it for |
|---|---|
| `UPDATED` | A code update. It does not have to be final. |
| `ADDED` | A new feature. |
| `FIXED` | A bug fix that you are 99–100 % sure is complete. If you are not sure, use `UPDATED`. |
| `UPGRADED` | An upgrade of a library: JUCE, luabind, LuaJIT, … |

- `<version>` is the version of the target branch: the content of the `VERSION` file at the
  repo root (CMake reads it into `CURRENT_VERSION`).
- `<summary>` is short and says what changed, e.g. `build instructions`, `large-panel load time
  (XML parser, lazy resources, lazy fonts)`.
- Real examples: `5.6.37 UPDATED build instructions (#329)`,
  `5.6.37 FIXED large-panel load time (XML parser, lazy resources, lazy fonts) (#341)`.
- **PR title** = the same title without `(#PR)`. GitHub adds the number when it squashes.
- Commits inside your branch should use the same format where it makes sense. Commits that are
  cherry-picked from other branches may keep their original title (`git cherry-pick -x`).

### Version scheme

`<JUCE major>.<JUCE minor>.<CtrlrX revision>`. For example, a build on JUCE 8.0.x with CtrlrX
revision 80 is `8.0.80`. The `5.6.x` versions use the older scheme from Roman Kubiak's Ctrlr.

## PR description

Write it so the maintainer can review and squash without reading every commit:

1. **What and why**, in a few sentences.
2. **Files added or removed.** The maintainer keeps the `.jucer` files and `JuceLibraryCode/` in
   sync by hand, so list every source file that was added, removed or renamed.
3. **Behaviour changes** that users will notice (saved files, settings paths, Lua results, menus).
4. **Checks**: CI run links, the test summary, manual checks done.
5. **Credits**: links to the commits the code came from.
6. **Suggested squash message** at the end: the full title with `(#N)`, a short body, and the
   credit trailers.

## Credit

A squashed commit has only one author: the PR author. If the code comes from someone else (another
fork, an older branch, another PR), credit them with a trailer in the squash message and link
their commits:

```
Co-authored-by: Name <email-or-github-noreply-address>
```

## Code rules

- **Keep the history comments.** The code has trailing comments like `// Added v5.6.33` and
  `// Updated v5.6.34 …`, and `#if 0` or commented-out blocks with older implementations. They
  show how the code changed across forks. Don't delete or change them during unrelated work. When
  you add or change code in the same way, use the same style with your version
  (`// Added v8.0.80 …`).
- **Porting from a fork: copy the changed parts, not whole files.** The other fork's copy of a
  file may not have the upstream comments, or may have them in a different form.
- **Formatting:** `.clang-format` (LLVM base, spaces, indent 4). Format only the lines you
  change. Don't send reformat-only commits.
- **Build: CMake is the source of truth.** Change `CMakeLists.txt`. Don't edit the `.jucer` files
  or `JuceLibraryCode/`: the maintainer does that (see *PR description*, item 2).
- **Vendored libraries** (`JUCE/`, `Source/Misc/…`): change them only when you must. A local change
  in `JUCE/` is made in place and also saved as `patches/<name>.patch`, with an entry in
  `patches/README.md` (file, reason, upstream issue). These patch files are documentation and the
  build does not apply them. Re-check each one at the next JUCE upgrade.
- **JUCE version:** use the latest patch release of the JUCE minor line in use (for example the
  newest 8.0.x). Upgrading JUCE is its own PR of type `UPGRADED`.
- **No build output or binaries in commits:** no `CMakeFiles/`, generated `Makefile`s,
  `cmake_install.cmake`, `CPack*Config.cmake`, `compile_commands.json`, `*.o`, `*.a`, `*.dll`,
  `*.so`, or personal IDE folders. Run `git status` before you commit. LuaJIT is built by CMake,
  so old build output inside `Source/Misc/luajit/` breaks the build: run `make clean` there.
- **README, `Doc/`, user guides and the `Panels/` submodule:** upstream's versions are the
  reference. Edit them in place; don't replace them with a copy from a fork. Update the `Panels/`
  submodule in a PR of its own.
- **GitHub workflows** (`.github/workflows/`): a change must pass on Linux, macOS and Windows on
  your fork before you send it.

## Tests

GoogleTest, in `Tests/`, built as the `TestsProcessor` target. Every test runs in its own process
through ctest. Tests must run from `build/Tests/` (fixtures are found relative to it).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug .
cmake --build build --target TestsProcessor -j8
cd build/Tests && GTEST_OUTPUT=xml:test-results/ ctest -j8 --output-on-failure
python3 ../../Tests/ci_check_test_results.py --allow ../../Tests/known_failures_linux.txt "test-results/**/*.xml"
```

- CI (`.github/workflows/build_and_test.yml`) builds and tests on Linux, macOS and Windows. A PR
  must be green against the allowlists `Tests/known_failures_{linux,macos,windows}.txt`.
- Don't add a test to an allowlist without a comment that gives the reason. Some listed tests are
  **specification tests**: they describe the behaviour we want. Remove them from the list when
  the behaviour is fixed; don't delete the test.
- If your change deliberately changes behaviour that a test checks, update the test in the same
  PR and say so in the description.
- A bug fix comes with a test that fails before the fix and passes after it, where that is
  possible.

## More

- `skills/ctrlrx-panel-builder/`: how to write and debug panel files by hand.
- `Tests/test_ProcessorFixture.h`: the shared test helpers. Read it before you add a test.
