[← 03 — Installation Guide](03-installation.md) | [Main Index](../../README.md) | Next: [05 — Exporting Instances →](05-exporting-instances.md)

---

# 04 — Compilation Guide

> **TL;DR**
> - Building CtrlrX from scratch utilizes native CMake workflows alongside environment-specific IDE project tools.
>   - configure: `cmake -B "build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCTRLRX_USE_LUAJIT=ON`
>   - compile:   `cmake --build "build" --config Release -j$(nproc)`
> - Ensure the vendored `boost.zip` asset is unzipped directly in place before running initial environment configurations.
> - Explicit platform dependency chains are detailed for Debian and Fedora Linux distributions below.

## Contents
- [Platform Guidelines](#platform-guidelines)
- [Windows](#windows)
- [macOS](#macos)
- [Linux Build Instructions](#linux-build-instructions)  
- [Linux Debian Specific Build Instructions](#linux-debian-specific-build-instructions)
- [Linux Fedora Specific Build Instructions](#linux-fedora-specific-build-instructions)
- [Create CtrlrX folder](#create-ctrlrx-folder)
- [Retrieve CtrlrX from GitHub](#retrieve-ctrlrx-from-github)
- [Recompile CtrlrX (If required)](#recompile-ctrlrx-if-required)
- [Change CtrlrX branch](#change-ctrlrx-branch)
- [Build VST2 (Optional)](#build-vst2-optional)
- [Build the original Ctrlr](#build-the-original-ctrlr)

---

## Platform Guidelines

* **Windows:** Compiling targets modern environments such as Visual Studio 2022 or Visual Studio 2019. Refer to community configuration manuals for step-by-step IDE instructions.
* **macOS:** Standard generation builds run via Xcode development tools and CMake configurations.

* **Boost Setup:** Regardless of your operating system, [boost](https://www.boost.org/) is required. You may have varying success with a version already installed on your system, so we recommend using the vendored version (1.88): Navigate to `Source/Misc/boost/` and fully extract the packed `boost.zip` archive. Ensure that its contents are extracted into the `/Source/Misc/boost/` directory, resulting in a structure like `/Source/Misc/boost/boost/_boost_content_` (where `_boost_content_` represents the actual Boost library files and subfolders).

* **LuaJIT Setup:** LuaJIT is included as source in `Source/Misc/luajit`, and automatically compiled for your target system in the CMake workflow.
> 💡 **Build LuaJIT (optional):** follow the instructions from [building LuaJIT for CtrlrX](../../Source/Resources/LuaJIT/LUAJIT_BUILD.md) for **Windows / macOS / Linux**.

---

### Windows

A summary will be added here in the future. Links to PDF build guides by @bijlevel and @dnaldoog can be found here:

* [Compiling on Windows 11 with Visual Studio 2022](https://github.com/user-attachments/files/19642077/How.to.compile.Ctrlr.or.CtrlrX.5.6.versions.in.Visual.Studio.2022.pdf)
* [Compiling on Windows 10 with Visual Studio 2019](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/Compiling_Ctrlr_for_Windows_10_v2.1.pdf)

If you have all the dependencies installed, try:
```
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCTRLRX_USE_LUAJIT=ON
cmake --build build -j 4
```
(where 4 is the number of parallel jobs, match it to your CPU core count), or the batch-script around these commands: `autobuild_win.bat`

---

### macOS

* [Compiling on OSX](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/My_guide_to_compiling_Ctrlr_for_macOS__Mojave__v2.pdf)

If you have all the dependencies installed, try:
```
cmake -B "build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCTRLRX_USE_LUAJIT=ON
cmake --build "build" --config Release -j$(sysctl -n hw.ncpu)
```
or use the 'autobuild' script:
```
./autobuild_mac.sh
```


---

### Linux Build Instructions

For Linux users, it is recommended to use CMake for compiling CtrlrX binaries. This requires certain development dependencies to be installed on your system.

**Required Development Dependencies (for Debian/Ubuntu-based systems):**

To ensure a successful build, install the following packages:

* **Core Build Tools:** `build-essential`, `cmake`, `pkg-config`
* **System & Utility Libraries:** `libiberty-dev`, `binutils-dev`, `libudev-dev`, `libasound2-dev`, `libtiff-dev`, `libcurl4-gnutls-dev`, `libboost-dev`
* **Graphics & X11 Development:** `libxi-dev`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libfreetype-dev`, `libgl1-mesa-dev`, `libglapi-mesa`, `xorg-dev`
* **Specific Libraries:** `libsframe1`

To install the necessary packages, open a terminal and run the appropriate `sudo apt install` command with these dependencies. Please consult your distribution's documentation for the exact command and package names if you encounter issues.

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libxi-dev \
    libx11-dev \
    binutils-dev \
    libudev-dev \
    libasound2-dev \
    libtiff-dev \
    libcurl4-gnutls-dev \
    libgl1-mesa-dev \
    libglapi-mesa \
    libboost-dev \
    xorg-dev \
    libiberty-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libfreetype-dev \
    ninja-build \
    pkg-config
```

**Note 1: Boost Library:** The Boost libraries may be available from your distribution, but we haven't tested all Boost versions. Boost 1.88 is included with CtrlrX. If the version from your distribution doesn't work, unzip the file located at `/Source/Misc/boost/boost.zip`. Ensure that its contents are extracted into the `/Source/Misc/boost/` directory, resulting in a structure like `/Source/Misc/boost/boost/_boost_content_` (where `_boost_content_` represents the actual Boost library files and subfolders).

**Note 2:** For other Linux distributions (e.g., Fedora, Arch Linux), the package names and installation commands may differ. Please consult your distribution's documentation for the equivalent packages.

### Linux Debian Specific Build Instructions

```
sudo apt install git cmake g++ -y
sudo apt install libxi-dev -y
sudo apt install libx11-dev binutils-dev -y
sudo apt install libudev-dev libasound2-dev libtiff-dev -y
sudo apt install libcurl4-gnutls-dev libiberty-dev -y
sudo apt install libxrandr-dev libxinerama-dev -y
sudo apt install libxcursor-dev libfreetype-dev pkg-config -y
sudo apt install libgtk-3-0 -y
sudo apt install libgtk-3-dev -y
sudo apt install libwebkit2gtk-4.1-dev -y
--- optional Extras 
sudo apt install libluajit-5.1-dev 
sudo apt install luajit
sudo apt install ninja-build build-essential cmake meson pkg-config
```

### Linux Fedora Specific Build Instructions

```
sudo dnf install g++ -y 
sudo dnf install cmake -y 
sudo dnf install git -y
sudo dnf install alsa-lib-devel -y ;
sudo dnf install freetype-devel -y ;
sudo dnf install libcurl-devel -y ;
sudo dnf install libXrandr-devel -y ;
sudo dnf install libXinerama-devel -y ;
sudo dnf install libXcursor-devel -y ;
sudo dnf install libudev-devel -y ;
sudo dnf install binutils-devel -y ;
sudo dnf install libzstd-devel -y
sudo dnf install webkit2gtk4.1-devel -y
sudo dnf install webkit2gtk4.1
sudo dnf install ninja-build -y
sudo dnf install gtk3-devel -y
--- optional Extras 
sudo dnf install luajit-devel -y
```

### Retrieve CtrlrX from GitHub

```
git clone https://github.com/damiensellier/CtrlrX
```
and if you want to use the vendored Boost libraries:
```
pushd CtrlrX/Source/Misc/boost
unzip boost.zip 
popd
```

### Recompile CtrlrX (If required)

```
cd CtrlrX
cmake -B "build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCTRLRX_USE_LUAJIT=ON
cmake --build "build" --config Release -j$(nproc)
```

or use the 'autobuild' script:
```
./autobuild_linux.sh
```

### Change CtrlrX branch

If you want to change to different branch under development :

```
cd ~/CtrlrX
git fetch
git branch -r
git checkout --track <branch-name>
```


### Build VST2 (Optional)

```
git clone --filter=blob:none --sparse https://github.com/RomanKubiak/ctrlr
cd ~/ctrlr
git sparse-checkout add Source/Misc/vst2sdk
cd ~/CtrlrX/Source/Plugin
cp -av ~/ctrlr/Source/Misc/vst2sdk/pluginterfaces .
```

Your compiled files can be found in : 

```
cd ~/CtrlrX/build/CtrlrX_artifacts/Release
```

### Build the original Ctrlr

If you need to build the original ctrlr for some reason :

```
git clone https://github.com/RomanKubiak/ctrlr.git
cd ~/ctrlr
sudo apt install libwebkit2gtk-4.0-dev
sudo apt install cmake
sudo apt install libgtk-3-dev
- Edit CMakeLists.txt to point to vst2sdk path ** // <-- SEE NOTE BELOW
cmake .
make ctrlr_Standalone
#Built file is found in
~/ctrlr/ctrlr_artefacts/Standalone
```

** Find the following line in CMakeLists.txt :

```
elseif(UNIX)
juce_set_vst2_sdk_path(/home/kbar/devel/vst2sdk) // <-THIS
```

And replace with :

```
elseif(UNIX)
juce_set_vst2_sdk_path(/home/($USER)/ctrlr/Source/Misc/vst2sdk) // <-THIS
```

---

[← 03 — Installation Guide](02-installation.md) | [Main Index](../../README.md) | Next: [05 — Exporting Instances →](05-exporting-instances.md)
