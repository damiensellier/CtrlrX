[← 03 — Installation Guide](03-installation.md) | [Main Index](../../README.md) | Next: [05 — Exporting Instances →](05-exporting-instances.md)

---

# 04 — Compilation Guide

> **TL;DR**
> - CtrlrX is built with **CMake**; [CMakeLists.txt](../../CMakeLists.txt) is the source of truth on
>   every platform.
>   ```bash
>   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
>   cmake --build build -j 8
>   ```
> - Requires Boost. Either `unzip Source/Misc/boost/boost.zip` in place or your distribution's Boost headers.
> - The results land in `build/CtrlrX_artefacts/<Config>/`  (Standalone app, VST3, and AU on macOS).
> - New to this? Follow [Quick start](#quick-start) for your platform, in order. Everything after it
>   is reference material you only need when you want to change *how* the build works.

## Contents

**Start here**
- [What you are building](#what-you-are-building)
- [Prerequisites](#prerequisites)
  - [Linux](#linux-prerequisites) · [macOS](#macos-prerequisites) · [Windows](#windows-prerequisites)
- [Quick start](#quick-start)
- [Step by step](#step-by-step)
  - [1 — Get the source](#1--get-the-source)
  - [2 — Configure](#2--configure)
  - [3 — Build](#3--build)
  - [4 — Run what you built](#4--run-what-you-built)
  - [5 — Run the tests (optional)](#5--run-the-tests-optional)

**Reference**
- [Build scripts](#build-scripts)
- [CMake options](#cmake-options)
- [Generators, IDEs and build configurations](#generators-ides-and-build-configurations)
- [Vendored dependencies](#vendored-dependencies)
- [Plugin formats: VST3, AU, VST2, AAX](#plugin-formats-vst3-au-vst2-aax)
- [macOS: deployment target and universal binaries](#macos-deployment-target-and-universal-binaries)
- [Working with branches](#working-with-branches)
- [Troubleshooting](#troubleshooting)
- [Appendix: building via the Projucer (legacy)](#appendix-building-via-the-projucer-legacy)
- [Appendix: building the original Ctrlr](#appendix-building-the-original-ctrlr)

---

## What you are building

One code base produces several targets at once. A full Release build gives you:

| Target | Path under `build/CtrlrX_artefacts/<Config>/` | Platforms |
|---|---|---|
| Standalone application | `Standalone/CtrlrX` (`.exe` on Windows, `.app` on macOS) | all |
| VST3 plugin | `VST3/CtrlrX.vst3` | all |
| Audio Unit plugin | `AU/CtrlrX.component` | macOS only |

`<Config>` is one of: `Release`, `Debug` or `RelWithDebInfo`. The test
runner is built alongside them, at `build/Tests/TestsProcessor`.

Budget roughly **3 GB of free disk space** and, on a modern 8-core machine, **10–30 minutes** for a
first full build. Later incremental builds take seconds. The Standalone binary itself is ~50 MB
because all resources are compiled into it.

> 💡 **Tip:** if you only want to *run* CtrlrX, you do not need any of this. Grab an installer from
> the [releases page](https://github.com/damiensellier/CtrlrX/releases) and read
> [03 — Installation Guide](03-installation.md) instead.

---

## Prerequisites

On every platform you need:

| | Minimum | Why |
|---|---|---|
| **Git** | any recent version | to clone the repository |
| **CMake** | **3.25** or newer | [CMakeLists.txt](../../CMakeLists.txt#L1) requires it |
| **C++ compiler** | GCC, Clang, AppleClang or MSVC with **C++14** | the project builds as C++14 |
| **Ninja** | optional but recommended | Noticeably faster than Make; used by all the build scripts |
| **Network access** | first configure only | CMake downloads GoogleTest for the test target |

No JUCE, Lua, LuaJIT or luabind installation is needed, see
[Vendored dependencies](#vendored-dependencies).

### Linux prerequisites

CtrlrX links against the system's ALSA, X11, GTK and WebKit libraries, so you need their
development packages. The required set is not a matter of taste: [CMakeLists.txt](../../CMakeLists.txt#L55-L59)
asks `pkg-config` for `libudev`, `x11`, `zlib`, `gtk+-3.0` and `webkit2gtk-4.1`, and configuring
fails immediately if any of them is missing.

**Debian / Ubuntu** (tested on Ubuntu 24.04, which is also what CI uses):

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake ninja-build pkg-config git unzip \
    libasound2-dev \
    libfreetype-dev libfontconfig1-dev \
    libx11-dev libxext-dev libxrender-dev libxcomposite-dev \
    libxinerama-dev libxrandr-dev libxcursor-dev \
    libgtk-3-dev libwebkit2gtk-4.1-dev \
    libudev-dev zlib1g-dev \
    libboost-dev
```

**Fedora** (and RHEL derivatives):

```bash
sudo dnf install -y \
    gcc-c++ cmake ninja-build pkgconf-pkg-config git unzip \
    alsa-lib-devel \
    freetype-devel fontconfig-devel \
    libX11-devel libXext-devel libXrender-devel libXcomposite-devel \
    libXinerama-devel libXrandr-devel libXcursor-devel \
    gtk3-devel webkit2gtk4.1-devel \
    systemd-devel zlib-devel \
    boost-devel
```

`systemd-devel` is what provides `libudev.pc` on Fedora.

**Other distributions:** map the list above to your package manager. The names differ, the libraries
do not. On Arch, for example, `base-devel cmake ninja alsa-lib freetype2 fontconfig libx11 libxext
libxrender libxcomposite libxinerama libxrandr libxcursor gtk3 webkit2gtk-4.1 systemd-libs zlib
boost` covers it.

> 💡 **Tip: running the GUI over SSH or in a container:** the Standalone app and the test suite both
> need an X display. Install `xvfb` and prefix commands with `xvfb-run` (this is how CI does it).

> ⚠️ **Gotcha: `webkit2gtk-4.1`, not 4.0:** older guides list `libwebkit2gtk-4.0-dev`. CtrlrX asks
> for **4.1**. During configure you will still see JUCE probe for the old one:
> `Package 'webkit2gtk-4.0', required by 'virtual:world', not found`. That line is harmless
> (CtrlrX builds with `JUCE_WEB_BROWSER=0`) and configuring continues. Only if it does stop the
> configure, alias the pkg-config file rather than installing the obsolete 4.0 package:
> ```bash
> sudo ln -sf /usr/lib/x86_64-linux-gnu/pkgconfig/webkit2gtk-4.1.pc \
>             /usr/lib/x86_64-linux-gnu/pkgconfig/webkit2gtk-4.0.pc
> ```

<details>
<summary>Packages older instructions asked for, and whether you still need them</summary>

| Package(s) | Status |
|---|---|
| `binutils-dev`, `libiberty-dev`, `libsframe1` | **Not needed.** [cmake/Findlibbfd.cmake](../../cmake/Findlibbfd.cmake) is no longer used by the build. CI still installs them, harmlessly. |
| `libcurl4-gnutls-dev` | **Not needed.** The build sets `JUCE_USE_CURL=0`. |
| `libgl1-mesa-dev`, `libglapi-mesa`, `libglu1-mesa-dev` | **Not needed.** No OpenGL JUCE module is linked. |
| `libtiff-dev` | **Not needed.** |
| `xorg-dev` | Convenient umbrella for the individual `libx*-dev` packages, installing it instead is fine. |
| `libluajit-5.1-dev`, `luajit` | Only for `-DCTRLRX_LUAJIT_USE_SYSTEM=ON`; see [CMake options](#cmake-options). |

Installing them anyway costs nothing but disk space, so don't worry if you already did.
</details>

### macOS prerequisites

1. **Xcode Command Line Tools**: `xcode-select --install`. For building the AU and for signing,
   install full **Xcode** from the App Store.
2. **CMake, Ninja and (optionally) Boost** via [Homebrew](https://brew.sh):
   ```bash
   brew install cmake ninja boost
   ```

The build targets macOS **10.13 (High Sierra)** and newer, set in
[cmake/PamplejuceMacOS.cmake](../../cmake/PamplejuceMacOS.cmake#L5).

### Windows prerequisites

1. **Visual Studio 2022** (Community is fine) with the *Desktop development with C++* workload, but
   VS 2019 also works. This provides MSVC and a CMake, but a standalone CMake is easier to script.
2. **CMake**: `winget install -e --id Kitware.CMake`.
3. **Ninja** (recommended): `winget install -e --id Ninja-build.Ninja`.
4. **Git**: `winget install -e --id Git.Git`.
5. Optional: **Intel IPP**. If CMake finds it, it is linked in for extra performance; if not, the
   build simply reports `IPP LIBRARIES *NOT* FOUND` and carries on
   ([cmake/PamplejuceIPP.cmake](../../cmake/PamplejuceIPP.cmake)).

Run the commands below from a **x64 Native Tools Command Prompt for VS 2022** (Start Menu → Visual
Studio 2022), which puts `cl.exe` on the `PATH`. If you prefer clicking through an IDE, two
community walkthroughs cover that route end to end:

* [Compiling on Windows 11 with Visual Studio 2022](https://github.com/user-attachments/files/19642077/How.to.compile.Ctrlr.or.CtrlrX.5.6.versions.in.Visual.Studio.2022.pdf) by @bijlevel
* [Compiling on Windows 10 with Visual Studio 2019](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/Compiling_Ctrlr_for_Windows_10_v2.1.pdf) by @dnaldoog

---

## Quick start

With the prerequisites in place:

```bash
git clone --recurse-submodules https://github.com/damiensellier/CtrlrX.git
cd CtrlrX
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
```

Then run it:

| Platform | Command |
|---|---|
| Linux | `./build/CtrlrX_artefacts/Release/Standalone/CtrlrX` |
| macOS | `open build/CtrlrX_artefacts/Release/Standalone/CtrlrX.app` |
| Windows | `build\CtrlrX_artefacts\Release\Standalone\CtrlrX.exe` |

That is the whole build. The rest of this page explains each step, and what you can change.

> 💡 **Prefer a menu?** [Build scripts](#build-scripts) wraps the same commands in an interactive
> script per platform (`autobuild_linux.sh`, `autobuild_mac.sh`, `autobuild_win.bat`).

---

## Step by step

### 1 — Get the source

```bash
git clone --recurse-submodules https://github.com/damiensellier/CtrlrX.git
cd CtrlrX
```

`--recurse-submodules` fetches [Panels/](../../Panels), the library of community panels. It is not
needed to compile, so if you already cloned without it: `git submodule update --init --recursive`
adds it later, or you can skip it entirely.

**Boost headers are required** (luabind needs them) and there are two ways to provide them — pick
one:

- *Your distribution's Boost*: `libboost-dev` / `boost-devel` / `brew install boost`. Convenient,
  but not every version has been tested.
- *The vendored Boost 1.88*: shipped as a zip in the repository, and what the maintainers test
  against. Extract it **in place** so the headers end up at `Source/Misc/boost/boost/...`:
  ```bash
  cd Source/Misc/boost && unzip boost.zip && cd -
  ```
  `Source/Misc/boost` is on the include path ahead of the system directories, so once extracted the
  vendored copy is the one that gets used.

### 2 — Configure

Configuring reads `CMakeLists.txt` and generates the actual build files into `build/`:

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

- `-B build` — put everything generated in `build/`, keeping the source tree clean. Delete that
  directory to start over; nothing else is touched.
- `-G Ninja` — build with Ninja. Drop it to get your platform's default (Make, or a Visual Studio
  solution). See [Generators, IDEs and build configurations](#generators-ides-and-build-configurations).
- `-DCMAKE_BUILD_TYPE=Release` — optimised build. Use `Debug` for debugger-friendly builds, or
  `RelWithDebInfo` for Release speed with symbols (a good middle ground for debugging inside a DAW).

> ⚠️ **Gotcha: always pass `CMAKE_BUILD_TYPE`** with Ninja or Make. Leave it out and you get an
> unoptimised build whose artefacts land in `build/CtrlrX_artefacts/` with no config subdirectory,
> which is confusing later.

This is also where CMake downloads GoogleTest (once, into `build/`) and sets up the LuaJIT build, so
the first configure needs a network connection and takes a little longer than later ones.

### 3 — Build

```bash
cmake --build build -j 8
```

`-j 8` is the number of parallel compile jobs. `-j"$(nproc)"` (Linux) or
`-j"$(sysctl -n hw.ncpu)"` (macOS) uses every core.

> ⚠️ **Gotcha: memory, not cores, is the real limit.** luabind's templates make some translation
> units need well over 1 GB each. If your machine has less than ~2 GB of RAM per core, a full
> `-j$(nproc)` will be killed by the OOM killer or start swapping. Cap it (`-j4`, `-j12`) instead.

Useful variants:

```bash
cmake --build build -j1                        # one job at a time, readable error logs
cmake --build build --target CtrlrX_Standalone # just the app
cmake --build build --target CtrlrX_VST3       # just the plugin
cmake --build build --target TestsProcessor    # just the test runner
cmake --build build --target clean             # throw away objects, keep the CMake cache
```

### 4 — Run what you built

Artefacts are written to `build/CtrlrX_artefacts/<Config>/`:

```bash
ls build/CtrlrX_artefacts/Release/
# Standalone/  VST3/  (AU/ on macOS)
```

To test the **plugin** you must put it where your DAW looks for it. Either copy it, or let the build
do it by configuring with `-DCTRLRX_COPY_PLUGIN_AFTER_BUILD=ON`, which installs into the standard
user plugin folders after every build:

| Platform | VST3 | AU |
|---|---|---|
| Linux | `~/.vst3/` | — |
| macOS | `~/Library/Audio/Plug-Ins/VST3/` | `~/Library/Audio/Plug-Ins/Components/` |
| Windows | `C:\Program Files\Common Files\VST3\` | — |

> ⚠️ **Gotcha:** on Windows that folder needs administrator rights; either run the build elevated or
> copy the `.vst3` by hand.

### 5 — Run the tests (optional)

The GoogleTest suite in [Tests/](../../Tests) is built as part of the normal build and registered
with CTest. Every test case is its own CTest entry, so they can run in parallel, each in a fresh
process:

```bash
cd build/Tests
ctest -j 8 --output-on-failure
ctest -R MidiRouting --output-on-failure        # one suite
ctest -N                                       # list the cases without running them
./TestsProcessor --gtest_filter='MidiSysex.sysex_input_updates_modulator'   # one case, directly
```

Run them **from `build/Tests`**, because the fixtures are resolved relative to the working directory.

Some tests are expected to fail: they describe behaviour that has been specified but not yet fixed.
The expected set is listed per platform in `Tests/known_failures_*.txt`. You can use
[Tests/ci_check_test_results.py](../../Tests/ci_check_test_results.py) to provide a
pass/fail verdict against that allowlist. For example (this is exactly what CI does):

```bash
cd build/Tests
GTEST_OUTPUT="xml:test-results/" ctest -j 8 --output-on-failure
../../Tests/ci_check_test_results.py --allow ../../Tests/known_failures_linux.txt "test-results/*.xml"
```

> 🔗 **Deeper:** [Tests/test_ProcessorFixture.h](../../Tests/test_ProcessorFixture.h) holds the
> shared helpers, and `Tests/mock_MidiDevice*.cpp` fakes the platform MIDI layer at link time so
> tests need no hardware.

---

## Build scripts

Three interactive wrappers around the commands above live in the repository root. They all offer
Release / Debug / clean / incremental / test choices from a menu:

| Script | Platform | Notes |
|---|---|---|
| [autobuild_linux.sh](../../autobuild_linux.sh) | Linux | Ninja; option 1/2 wipe `build/` first |
| [autobuild_mac.sh](../../autobuild_mac.sh) | macOS | Ninja; adds a `RelWithDebInfo` choice |
| [autobuild_win.bat](../../autobuild_win.bat) | Windows | Ninja; bootstraps the MSVC environment itself |

```bash
./autobuild_linux.sh     # or ./autobuild_mac.sh, or autobuild_win.bat
```

> ⚠️ **Gotcha: `autobuild_win.bat` has two hardcoded paths.** It builds into
> `%USERPROFILE%\Documents\CtrlrX\build`, so it expects the checkout at
> `%USERPROFILE%\Documents\CtrlrX`, and it looks for `vcvars64.bat` under *Visual Studio 2022
> Community*. Edit the `BUILD_DIR` line and the `vcvars64.bat` path if your layout differs, or just
> run the two `cmake` commands yourself.

---

## CMake options

Pass these at configure time as `-D<NAME>=<VALUE>`. Defaults are what CI and the release builds use.

| Option | Default | What it does |
|---|---|---|
| `CMAKE_BUILD_TYPE` | *(empty)* | `Release`, `Debug` or `RelWithDebInfo`. Always set it. |
| `CTRLRX_USE_LUAJIT` | `ON` | Build and link the vendored LuaJIT. `OFF` falls back to the vendored vanilla Lua 5.1.4 amalgamation. |
| `CTRLRX_USE_SYSTEM_JUCE` | `OFF` | Use an installed JUCE (`find_package(JUCE)`) instead of the in-tree copy. For distro packagers. |
| `CTRLRX_COPY_PLUGIN_AFTER_BUILD` | `OFF` | Install the plugin into your user plugin folders after each build. |
| `CTRLRX_ENABLE_CPM` | `OFF` | Enable the CPM package manager for optional dependencies. |
| `CTRLRX_LUAJIT_USE_SYSTEM` | `OFF` | Link a system LuaJIT found via pkg-config instead of building the vendored one. |
| `CTRLRX_LUAJIT_DEBUG` | `OFF` | Build LuaJIT itself unoptimised, for debugging LuaJIT. |
| `CTRLRX_LUAJIT_SOURCE_DIR` | `Source/Misc/luajit` | Build a different LuaJIT checkout. |
| `PAMPLEJUCE_AUTO_BUMP_PATCH_LEVEL` | `OFF` | Derive the patch level from the commit count instead of reading [VERSION](../../VERSION). |

`CMAKE_EXPORT_COMPILE_COMMANDS` is always on, so `build/compile_commands.json` is written for
clangd and other tooling.

> 🔗 **Deeper:** the option declarations are at [CMakeLists.txt:18-21](../../CMakeLists.txt#L18-L21)
> and [cmake/luajit/CMakeLists.txt:48-52](../../cmake/luajit/CMakeLists.txt#L48-L52).

---

## Generators, IDEs and build configurations

CMake generates build files; which kind is up to you.

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release        # fastest, all platforms
cmake -B build -DCMAKE_BUILD_TYPE=Release                 # platform default (Make / MSBuild)
cmake -B build -G Xcode                                   # macOS: Xcode project
cmake -B build -G "Visual Studio 17 2022" -A x64          # Windows: .sln for the IDE
```

**Single-config vs multi-config matters.** Ninja and Make bake the configuration in at *configure*
time (`-DCMAKE_BUILD_TYPE=…`). Xcode and Visual Studio choose it at *build* time and ignore
`CMAKE_BUILD_TYPE`:

```bash
cmake --build build --config Release        # Xcode / Visual Studio
```

After generating, open `build/CtrlrX.xcodeproj` or `build/CtrlrX.sln` and work in the IDE as usual;
the CtrlrX_Standalone target is runnable and debuggable directly.

> 💡 **Tip:** to speed up repeated full builds, point CMake at a compiler cache:
> `-DCMAKE_CXX_COMPILER_LAUNCHER=ccache` (or `sccache`, which is what CI uses).

---

## Vendored dependencies

CtrlrX deliberately carries its dependencies in-tree, so a fresh clone builds without hunting for
versions:

| Dependency | Location | Notes |
|---|---|---|
| **JUCE** | [JUCE/](../../JUCE) | Checked in, not a submodule. Added with `add_subdirectory(JUCE)`. |
| **LuaJIT** | [Source/Misc/luajit/](../../Source/Misc/luajit) | Unmodified upstream source, **compiled from source by CMake** on all three platforms. |
| **luabind** | [Source/Misc/luabind/](../../Source/Misc/luabind) | Compiled straight into the plugin; provides the Lua ↔ C++ bindings. |
| **Lua 5.1.4** | [Source/Misc/lua/](../../Source/Misc/lua) | Fallback engine, used only with `CTRLRX_USE_LUAJIT=OFF`. |
| **Boost 1.88** | `Source/Misc/boost/boost.zip` | Headers only, unzip in place or use your system Boost. |
| **libusb**, **rapidfuzz** | `Source/Misc/` | Compiled in. |
| **GoogleTest / gmock** | downloaded at configure time | Only for the test target. |

The LuaJIT build is the interesting one: LuaJIT has no upstream CMake build, so CtrlrX ships its own
at [cmake/luajit/CMakeLists.txt](../../cmake/luajit/CMakeLists.txt), which reproduces LuaJIT's
two-stage `minilua` → `buildvm` bootstrap. It lives outside the vendored tree so that tree stays
byte-identical to upstream, and everything generated goes to `build/cmake/luajit/gen/`. You never
need to run `make` or `msvcbuild.bat` yourself.

> 🔗 **Deeper:** [Doc/LUAJIT.md](../LUAJIT.md) explains how the LuaJIT build works, the options, how to
> build LuaJIT standalone, how to upgrade it, and the CI drift checks that keep the CMake source
> lists in sync with upstream's `Makefile`.

---

## Plugin formats: VST3, AU, VST2, AAX

The formats built are listed at [CMakeLists.txt:46](../../CMakeLists.txt#L46):

```cmake
set(FORMATS Standalone AU VST3)
```

`AU` is silently skipped on non-Apple platforms. To build the legacy formats you need the
corresponding SDK. Neither is included, and both require agreeing to a vendor licence:

**VST2** (Steinberg; JUCE 6 still supports it, later JUCE versions do not):

1. Obtain the VST2 SDK and place it at `Source/Misc/vst2sdk/`, because that is where
   [CMakeLists.txt:106](../../CMakeLists.txt#L106) looks. The old Ctrlr repository carried a copy:
   ```bash
   git clone --filter=blob:none --sparse https://github.com/RomanKubiak/ctrlr /tmp/ctrlr
   git -C /tmp/ctrlr sparse-checkout add Source/Misc/vst2sdk
   cp -av /tmp/ctrlr/Source/Misc/vst2sdk Source/Misc/
   ```
2. Add `VST` to `FORMATS` in `CMakeLists.txt`, then configure from scratch (delete `build/`).

**AAX** (Avid): place the SDK at `Source/Misc/aax-sdk-2-8-1/`
([CMakeLists.txt:110](../../CMakeLists.txt#L110)) and add `AAX` to `FORMATS`. Distributing an AAX
build additionally requires being a registered Avid developer.

> ⚠️ **Commercial warning:** the licence terms of these SDKs, and of the GPL/BSD code they link
> against, constrain what you may distribute. See [02 — Licensing](02-licensing.md).

---

## macOS: deployment target and universal binaries

- The deployment target is **10.13**, forced in
  [cmake/PamplejuceMacOS.cmake](../../cmake/PamplejuceMacOS.cmake#L5).
- Local builds are **native-architecture only**, to keep build times down. CI builds universal
  binaries. To do the same locally:
  ```bash
  cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
  ```
  This roughly doubles the build time; the LuaJIT bootstrap runs once per slice and the results are
  merged with `lipo`.
- Codesigning and notarization are handled by the release workflow, not by CMake. An unsigned local
  build will be blocked by Gatekeeper on first launch, see [03 — Installation Guide](03-installation.md).

---

## Working with branches

CtrlrX development happens on feature branches; to try one:

```bash
git fetch
git branch -r                       # list what's available
git checkout <branch-name>
```

After switching branches, **reconfigure from scratch** if the build behaves oddly:

```bash
rm -rf build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
```

Branches differ in JUCE version and in how Lua is built, so a `build/` directory from another branch
can hold stale cached paths.

---

## Troubleshooting

| Symptom | Cause and fix |
|---|---|
| `Could NOT find PkgConfig` / `None of the required 'gtk+-3.0' found` (Linux) | A development package is missing. Re-run the [Linux](#linux-prerequisites) install command; the message names the pkg-config module, e.g. `webkit2gtk-4.1` → `libwebkit2gtk-4.1-dev`. |
| `CMake 3.25 or higher is required` | Your distribution's CMake is too old. Install a current one from [cmake.org](https://cmake.org/download/), `pip install cmake`, or Homebrew. |
| `boost/…: No such file or directory` | Neither system Boost nor the vendored copy is available. Install `libboost-dev`, or `cd Source/Misc/boost && unzip boost.zip`. |
| `LuaJIT: build output found in the vendored source tree` at configure time | Someone ran LuaJIT's own `make`/`msvcbuild.bat` in `Source/Misc/luajit`. Generated headers there shadow the ones CMake generates. Fix: `git clean -xdf Source/Misc/luajit`. |
| Configure fails downloading GoogleTest | First configure needs network access. Retry online; a proxy needs `http_proxy`/`https_proxy` set. |
| Compiler killed, `c++: fatal error: Killed signal terminated`, or heavy swapping | Out of RAM. luabind's translation units are large. Lower the job count: `cmake --build build -j4`. |
| Artefacts are not in `build/CtrlrX_artefacts/Release/` | You configured without `-DCMAKE_BUILD_TYPE`; they are directly in `build/CtrlrX_artefacts/`. Reconfigure with the build type set. |
| Standalone app exits with `Cannot open display` (Linux) | No X display (SSH, container). Use `xvfb-run ./CtrlrX` or forward X11. |
| `LNK4098` / mismatched CRT, or `LNK2001` on `lua_*` symbols (Windows) | Only happens on the Projucer route, from mixing runtime libraries or a wrong-architecture `lua51.lib`. See [Doc/LUAJIT.md](../LUAJIT.md) and the [Projucer appendix](#appendix-building-via-the-projucer-legacy). |
| The VST3 loads but the host crashes on scan (Linux) | Make sure you did not remove the `-Wl,--exclude-libs,ALL` link option at [CMakeLists.txt:302](../../CMakeLists.txt#L302), because it prevents a symbol collision between the plugin's static libraries and the host's. |
| A handful of tests fail | Expected; compare against `Tests/known_failures_<platform>.txt`. See [step 5](#5--run-the-tests-optional). |

Still stuck? CI builds all three platforms from scratch on every push, so
[.github/workflows/build_and_test.yml](../../.github/workflows/build_and_test.yml) is a known-good,
fully explicit recipe, including the exact package lists. The logs of a green run show what
correct output looks like.

---

## Appendix: building via the Projucer (legacy)

The `.jucer` files in the repository root are kept for upstream contributors who work in the
Projucer. They carry exporters for Visual Studio 2019/2022, Xcode, Linux Makefile and Code::Blocks,
and generate into `Builds/<exporter>/`.

**CMake is the supported path.** The Projucer route exists for continuity, and its one real
complication is LuaJIT: the exporters cannot run CMake, so they link prebuilt LuaJIT bundles from
`Source/Misc/luajit/precompiled/`, extracted automatically by a pre-build step. Nothing to do by
hand, but do not copy anything out of `precompiled/` into `Source/Misc/luajit/src/`.

> 🔗 **Deeper:** [Doc/LUAJIT.md](../LUAJIT.md) explains the precompiled bundles, why Windows ships
> one library per configuration, and how to regenerate them.

---

## Appendix: building the original Ctrlr

If you need Roman Kubiak's original Ctrlr rather than CtrlrX:

```bash
git clone https://github.com/RomanKubiak/ctrlr.git
cd ctrlr
sudo apt install cmake libgtk-3-dev libwebkit2gtk-4.0-dev
```

Then edit its `CMakeLists.txt` so the VST2 SDK path points into your own checkout:

```diff
 elseif(UNIX)
-juce_set_vst2_sdk_path(/home/kbar/devel/vst2sdk)
+juce_set_vst2_sdk_path(/home/YOUR_USERNAME/ctrlr/Source/Misc/vst2sdk)
```

and build the standalone target:

```bash
cmake .
make ctrlr_Standalone
# result: ctrlr_artefacts/Standalone/
```

---

[← 03 — Installation Guide](03-installation.md) | [Main Index](../../README.md) | Next: [05 — Exporting Instances →](05-exporting-instances.md)
