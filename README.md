![alt text](/Source/Resources/Icons/CtrlrX-README-250x315.png?raw=true "CtrlrX")



## Table of Contents
* [About CtrlrX](#about-ctrlrx)
* [Donations](#donations)
* [Licensing](#licensing)
  * [JUCE Framework Licensing](#juce-framework-licensing)
  * [Crucial Note for Commercial Distribution of JUCE based softwares](#crucial-note-for-commercial-distribution-of-juce-based-softwares-including-those-based-on-this-project)
  * [AAX Plugin-Specific Licensing & Avid SDK Terms](#aax-plugin-specific-licensing--avid-sdk-terms)
  * [Crucial Note for Commercial Distribution of AAX Plugins](#crucial-note-for-commercial-distribution-of-aax-plugins-including-those-based-on-this-project)
  * [AudioUnit (AU/AUv3) Plugin-Specific Licensing & Apple SDK Terms](#audiounit-auauv3-plugin-specific-licensing--apple-sdk-terms)
  * [Crucial Note for Commercial Distribution & App Store Submission of AU & AUv3 Plugins](#crucial-note-for-commercial-distribution--app-store-submission-of-au--auv3-plugins-including-those-based-on-this-project)
  * [VST3 Plugin-Specific Licensing & Steinberg SDK Terms](#vst3-plugin-specific-licensing--steinberg-sdk-terms)
  * [Crucial Note for Commercial Distribution of VST3 Plugins](#crucial-note-for-commercial-distribution-of-VST3-plugins-including-those-based-on-this-project)
* [Compiling CtrlrX](#compiling-ctrlrx)
  * [Windows](#windows)
  * [macOS](#macos)
  * [Linux Build Instructions](#linux-build-instructions)  
  * [Linux Debian 12 Specific Build Instructions](#linux-debian-12-specific-build-instructions)
  * [Linux Fedora 42 Specific Build Instructions](#linux-fedora-42-specific-build-instructions)
* [Exporting Plugin Instances](#exporting-plugin-instances)
  * [VST2 Support](#vst2-support)
  * [VST3 Support](#vst3-support)
  * [AAX Support & Build Instructions](#aax-support--build-instructions)
  * [AU Support](#au-support)
  * [AUv3 Support](#auv3-support)
* [ToDo](#todo)
* [Changelog](#changelog)
* [Credits](#credits)
* [Notes](#notes)

---

## About CtrlrX

[CtrlrX](https://github.com/damiensellier/CtrlrX) by [Damien Sellier](https://github.com/DamienSellier/) is an alternative fork of [Ctrlr](https://github.com/RomanKubiak/ctrlr) by [Roman Kubiak](https://github.com/RomanKubiak) under BSD|GPL license. The 'X' in CtrlrX represents a commitment to delivering **extra** features, **extended** functionalities, and a **collaborative** space for its evolution.

This project is **ONLY** aimed at delivering updates, a wiki, documentation, tutorials, or anything that the community cannot share on the original [Ctrlr](https://github.com/RomanKubiak/ctrlr) GitHub due to credential restrictions. Let's keep the original [Ctrlr](https://github.com/RomanKubiak/ctrlr) GitHub alive and continue with what we were all doing [there](https://github.com/RomanKubiak/ctrlr). But for anything that deserves special credentials unavailable on [Ctrlr](https://github.com/RomanKubiak/ctrlr), let's do it on [CtrlrX](https://github.com/damiensellier/CtrlrX).

### About the Original Ctrlr

Ctrlr allows you to control any MIDI-enabled hardware: synthesizers, drum machines, samplers, effects. Create custom User Interfaces and host them as VST or AU plugins in your favorite DAWs.

* **Cross Platform:** Works on Windows (XP and up, both 64 and 32bit binaries are available), macOS (10.5 and up), Linux (any modern distro should run it).
* **Host in your DAW:** Each platform has a VST build of Ctrlr, so you can host your panels as normal VST plugins. For macOS, a special AU build is available.
* **Customize:** Each panel is an XML file and can be edited in Ctrlr to suit your specific needs.
* **Extend:** With the scripting possibilities inside Ctrlr, you can extend your panels in various ways. The Lua scripting language gives you access to all panel elements and hooks to various events.
* **Open Source:** Need special functionality or want to propose a patch/feature update? If you know a bit about C++/JUCE framework, etc., you can always download the source code and build Ctrlr by yourself.

---

## Donations

Development and maintenance of [CtrlrX](https://github.com/damiensellier/CtrlrX) are supported by the community. If you find this project useful and would like to support [Damien Sellier](https://github.com/DamienSellier/), the main person behind [CtrlrX](https://github.com/damiensellier/CtrlrX), any donations are greatly appreciated.

You can donate via PayPal using this link: **[paypal.me/damiensellier/10](https://paypal.me/damiensellier/10)**

---

## Licensing

The [CtrlrX](https://github.com/damiensellier/CtrlrX) project by [Damien Sellier](https://github.com/DamienSellier/) is a fork of [Ctrlr](https://github.com/RomanKubiak/ctrlr) by [Roman Kubiak](https://github.com/RomanKubiak). The original creator has generously granted permission for derivative works (forks) to choose their own licensing terms.

Specifically, Roman Kubiak stated the following regarding the Ctrlr license:

> "It means that you can use Ctrlr code in two ways."
>
> "One is a GPL licence that means if you want to use Ctrlr’s code you must include it with your product."
>
> "The other licence means that you can do whatever you want with the code, you don’t need to publish the source code, you just need to mention where did the code come from."
>
> "Panels are not covered by this license, you can do whatever you want with them, sell them, give them away whatever, noone else has the rights to them, they are your intelectual property."
>
> [Source](https://web.archive.org/web/20240213071445/https://ctrlr.org/forums/topic/ctrlr-licence/#post-37755) (2015-02-11 11:58)

Based on this explicit permission, the core code of this plugin (CtrlrX) is licensed under the **[BSD-3-Clause License](LICENSE.BSD-3.md)** or **[GPL-2.0-or-later License](LICENSE.GPL-2.0.md)** - *choose and keep only the relevant one for your core project*.

---

### JUCE Framework Licensing

CtrlrX is built upon the [JUCE](https://juce.com/) C++ framework for audio applications. JUCE itself operates under a **dual-licensing model**:

* **GNU General Public License v3 (GPLv3)**: Available for open-source projects.
* **Commercial Licenses**: Required for proprietary commercial products.

Given that CtrlrX is an open-source project, it is generally built and used with JUCE under the terms of the **GPLv3**. This means that if you link CtrlrX with JUCE under its GPLv3 license, the resulting combined work (i.e., CtrlrX) becomes subject to the GPLv3's "copyleft" provisions. Consequently, any distribution of CtrlrX binaries linked with GPLv3-licensed JUCE, along with their corresponding source code, must comply with the GPLv3.

* JUCE is a registered trademark of Raw Material Software Limited acquired by Pace Anti-Piracy Inc.

### Crucial Note for Commercial Distribution of JUCE based softwares (including those based on this project)

If you plan to commercially distribute any product (including a derivative of CtrlrX) that uses the JUCE framework, you will typically need to obtain an appropriate **commercial license from JUCE**. Using JUCE under a commercial license allows you to distribute your proprietary products without being bound by the GPLv3's copyleft requirements for your own code.

For detailed and up-to-date information on JUCE's licensing, please visit the official JUCE website:
* [https://juce.com/licensing/](https://juce.com/licensing/)
* [https://juce.com/juce-legal/](https://juce.com/juce-legal/)

---

### AAX Plugin-Specific Licensing & Avid SDK Terms

This plugin is designed to be compiled as an AAX plugin for Pro Tools. To do so, it utilizes the Avid AAX SDK (version 2.8.1 or newer).

The Avid AAX SDK is available under **commercial or open-source licensing**. For the purposes of this open-source project, we are using the AAX SDK under the terms of the **GNU General Public License v3 (GPLv3)**.

Due to the "copyleft" nature of the GPLv3, the entire distributed AAX plugin (including the portions of this project's code linked with the SDK) **must also be licensed under GPLv3**.

Therefore, any distributed AAX binaries, along with their corresponding source code, are subject to the terms of the GPLv3. You can find the full text of the GPLv3 [here](https://www.gnu.org/licenses/gpl-3.0.en.html).

**By building or using this plugin's AAX version, you acknowledge and agree to the terms of:**

* The [Avid AAX SDK License Agreement](https://developer.avid.com/aax)
* The [Avid Privacy Policy](https://www.avid.com/legal/privacy-policy-statement)

* AAX is a registered trademark of Avid Technologies, Inc.

### Crucial Note for Commercial Distribution of AAX Plugins (including those based on this project)

While this project is open-source under GPLv3, the Avid AAX SDK itself offers a dual-licensing model. **If you intend to commercially release a third-party plugin based on this project (or any AAX plugin derived from the Avid AAX SDK), you typically need to obtain a separate commercial license from Avid for the AAX SDK and comply with their specific commercial terms, which often include requirements for AAX plugin code signing via PACE Anti-Piracy tools.**

For information on commercial AAX development and licensing, please contact Avid directly at `audiosdk@avid.com` or visit the [Avid Developer Website](https://developer.avid.com/aax).

---

### AudioUnit (AU/AUv3) Plugin-Specific Licensing & Apple SDK Terms

This plugin can be compiled as an AudioUnit (AU) and AudioUnit v3 (AUv3) plugin for macOS and iOS respectively. Development for these formats utilizes Apple's proprietary macOS/iOS SDKs and Xcode development tools.

Unlike VST3 or AAX, there isn't a separate, redistributable "AudioUnit SDK" with a distinct open-source license. Instead, the use of AudioUnit APIs and the distribution of AU/AUv3 plugins are governed by the **Apple Developer Program License Agreement (ADPPA)** and the terms associated with the macOS and iOS Software Development Kits (SDKs).

For the purposes of this open-source project, the compilation and use of AudioUnit frameworks fall under the general terms of Apple's developer agreements. If this project's core code is licensed under GPLv3 (due to linking with GPLv3-licensed JUCE or other SDKs), then the distributed AU/AUv3 binaries, along with their corresponding source code, are subject to the terms of the GPLv3.

* Audio Unit and AUv3 are trademarks of Apple Inc., registered in the U.S. and other countries.

### Crucial Note for Commercial Distribution & App Store Submission of AU & AUv3 Plugins (including those based on this project)
If you intend to commercially distribute an AU or AUv3 plugin derived from this project, or submit it to the Mac App Store or iOS App Store, you **must be an enrolled member of the Apple Developer Program** and comply with all terms and conditions of the **Apple Developer Program License Agreement**. This includes requirements for code signing, notarization (for macOS), and adherence to App Store review guidelines.

For detailed information on Apple's developer agreements and guidelines, please refer to the official Apple Developer website:
[https://developer.apple.com/support/terms/](https://developer.apple.com/support/terms/)
[https://developer.apple.com/audio-units/](https://developer.apple.com/audio-units/)

---

### VST2 Plugin-Specific Licensing & Steinberg SDK Terms

The VST2 API and SDK were officially discontinued by Steinberg in January 2022. You can find more details about this decision on Steinberg's help center: [VST 2 Discontinued](https://helpcenter.steinberg.de/hc/en-us/articles/4409561018258-VST-2-Discontinued).

Due to this discontinuation, we do not officially distribute a VST2.4 version of CtrlrX. However, if you are an existing licensee of the VST2 SDK from Steinberg, you may still be able to build a VST2 version of CtrlrX for your personal use or under the terms of your specific VST2 SDK license agreement.

* VST is a registered trademark of Steinberg Media Technologies GmbH.

---

### VST3 Plugin-Specific Licensing & Steinberg SDK Terms

This plugin is designed to be compiled as a VST3 plugin. To do so, it utilizes the Steinberg VST3 SDK.

The Steinberg VST3 SDK is available under a **dual-licensing model**:
* **GNU General Public License v3 (GPLv3)**: For open-source projects.
* **Commercial License**: Required for commercial products.

For the purposes of this open-source project, we use the VST3 SDK under the terms of the **GPLv3**. Due to the "copyleft" nature of the GPLv3, any distributed VST3 plugin (including the portions of this project's code linked with the SDK) **must also be licensed under GPLv3**.

Therefore, any distributed VST3 binaries, along with their corresponding source code, are subject to the terms of the GPLv3. You can find the full text of the GPLv3 [here](https://www.gnu.org/licenses/gpl-3.0.en.html).

* VST3 is a registered trademark of Steinberg Media Technologies GmbH.

### Crucial Note for Commercial Distribution of VST3 Plugins (including those based on this project)

If you intend to commercially release a third-party plugin based on this project (or any VST3 plugin derived from the Steinberg VST3 SDK), you **must obtain a separate commercial license from Steinberg**. This commercial license typically has no runtime fees or royalties, but it is necessary for proprietary commercial distribution.

For detailed information on VST3 licensing, please refer to the official Steinberg VST 3 Licensing page:
[https://steinbergmedia.github.io/vst3_dev_portal/pages/VST+3+Licensing/Index.html](https://steinbergmedia.github.io/vst3_dev_portal/pages/VST+3+Licensing/Index.html)

---

## Compiling CtrlrX

This section provides instructions on how to build the CtrlrX plugin from source.

### Windows

A summary will be added here in the future. Links to PDF build guides by @bijlevel and @dnaldoog can be found here:

* [Compiling on Windows 11 with Visual Studio 2022](https://github.com/user-attachments/files/19642077/How.to.compile.Ctrlr.or.CtrlrX.5.6.versions.in.Visual.Studio.2022.pdf)
* [Compiling on Windows 10 with Visual Studio 2019](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/Compiling_Ctrlr_for_Windows_10_v2.1.pdf)

### macOS

* [Compiling on OSX](https://godlike.com.au/fileadmin/godlike/techtools/ctrlr/guides/My_guide_to_compiling_Ctrlr_for_macOS__Mojave__v2.pdf)

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
    pkg-config
```

**Note 1: Boost Library:** Boost is already included with CtrlrX. You need to unzip the file located at `/Source/Misc/boost/boost.zip`. Ensure that its contents are extracted into the `/Source/Misc/boost/` directory, resulting in a structure like `/Source/Misc/boost/boost/_boost_content_` (where `_boost_content_` represents the actual Boost library files and subfolders).
**Note 2:** For other Linux distributions (e.g., Fedora, Arch Linux), the package names and installation commands may differ. Please consult your distribution's documentation for the equivalent packages.

### Linux Debian 12 Specific Build Instructions

```
sudo apt install git g++ -y
sudo apt install libxi-dev -y
sudo apt install libx11-dev binutils-dev -y
sudo apt install libudev-dev libasound2-dev libtiff-dev -y
sudo apt install libcurl4-gnutls-dev libiberty-dev -y
sudo apt install libxrandr-dev libxinerama-dev -y
sudo apt install libxcursor-dev libfreetype-dev pkg-config -y
git clone https://github.com/damiensellier/CtrlrX
cd ~/CtrlrX/Source/Misc/boost
unzip boost.zip 
# Download VST2 toolkit
cd ~
git clone --filter=blob:none --sparse https://github.com/RomanKubiak/ctrlr
cd ~/ctrlr
git sparse-checkout add Source/Misc/vst2sdk
cd ~/CtrlrX/Source/Plugin
cp -av ~/ctrlr/Source/Misc/vst2sdk/pluginterfaces .

cd ~/CtrlrX/Builds/LinuxMakefile
./make CONFIG=Release
# note "make clean" to clean and recompile
```

### Linux Fedora 42 Specific Build Instructions

```
sudo dnf install g++ -y 
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

git clone https://github.com/damiensellier/CtrlrX
cd ~/CtrlrX/Source/Misc/boost
unzip boost.zip
# Download VST2 toolkit
cd ~
git clone --filter=blob:none --sparse https://github.com/RomanKubiak/ctrlr
cd ~/ctrlr
git sparse-checkout add Source/Misc/vst2sdk
cd ~/CtrlrX/Source/Plugin
cp -av ~/ctrlr/Source/Misc/vst2sdk/pluginterfaces .

cd ~/CtrlrX/Builds/LinuxMakefile
./make CONFIG=Release
# note "make clean" to clean and recompile
```

---

## Exporting Plugin Instances

This section covers how to export various plugin formats from CtrlrX.

### VST2 Support & Build Instructions

**Important Note: VST2 Discontinuation and SDK Availability**

**We strongly recommend that users transition to building the VST3 version of CtrlrX instead of VST2, as VST3 is the current and actively supported standard.**

Steinberg officially discontinued the VST2 API and SDK in **January 2022**. As a result, the VST2 SDK is no longer publicly available for download or new licensing. This means that for most new users, building a VST2.4 plugin directly from this project will not be possible without prior access to the SDK. For more information, please refer to Steinberg's official announcement: [VST 2 Discontinued](https://helpcenter.steinberg.de/hc/en-us/articles/4409561018258-VST-2-Discontinued).

However, if you are an **existing licensee** of the VST2 SDK (meaning you had access to it prior to its discontinuation), you can still compile a VST2 version of CtrlrX. Your ability to build and distribute this VST2.4 version of the plugin will be governed solely by the terms of your specific VST2 SDK license agreement with Steinberg.

**To build the VST2 version of this plugin (for existing licensees):**

1.  **Ensure VST2 SDK Availability:**
    Make sure you have your VST2 SDK files (specifically the `vst2.x` folder containing headers like `aeffect.h`) accessible on your system. This SDK is **not included** with JUCE and must be sourced from your prior access.

2.  **Configure JUCE Projucer Global Paths:**
    * Open the JUCE Projucer application (compatible with your JUCE 6.0.8 version).
    * Navigate to **"File" > "Global Paths..."** (or sometimes just "Settings" via the gear icon).
    * In the Global Paths window, locate the entry specifically for **"VST (Legacy) SDK"** or "VST2 SDK".
    * Set this path to the root directory where your VST2 SDK is located (e.g., the folder containing the `pluginterfaces` directory and other VST2 SDK components). This path is critical for JUCE to find the necessary VST2 headers and libraries.
    * Open your CtrlrX `.jucer` project file in Projucer, go to the "Exporters" section, and ensure the VST2 format is enabled for your target platform.

3.  **Build the CtrlrX VST2 binary:**
    Follow your project's general build instructions (e.g., via your IDE after Projucer generation, or directly with CMake) to compile the VST2 target.


### VST3 Support

To compile the VST3 version of this plugin, ensure your **JUCE installation is correctly set up to include the VST3 SDK**, as it's typically bundled with JUCE.

#### UPDATE v5.6.32+

VST3 exported instances of CtrlrX panels are finally working properly. On macOS, you need to export VST3 with at least an ad-hoc signature. This process is managed automatically by CtrlrX; if you have an Apple developer certificate, select it in the appropriate field to prevent the VST3 from being held by Gatekeeper. If you want to sign your Windows exported instances with a PFX Certificate, that's now also possible.

If for some reason you still want to produce your own VST3 intermediates, you can simply disable the unique identifiers replacement process from the appropriate field.

#### Prior v5.6.32

Currently, VST3 instances of Ctrlr panels are not working properly because Ctrlr is not able to generate different VST3 compliant plugin identifiers.

Unfortunately, exported VST3 instances of your panel will always be named after CTRLR | Instigator. The only way to get the correct identifiers for a panel project is to force them at the core during the building step of the VST3 in Xcode/VS/IDE.
To export properly identified VST3 plugins, it is then required to build a different stock CTRLR VST3 plugin with JUCE Projucer and Xcode/VS/IDE.
However, this alternative version of CTRLR VST3 will have the desired panel/plugin identifiers predefined in the Projucer settings (Plugin Name, Manufacturer Name, Plugin ID, Manufacturer ID, etc.).
This intermediate VST3 plugin will then be able to export a final VST3 version of the panel with the proper identifiers.

### AAX Support & Build Instructions

This project is built and tested with **JUCE 6.0.8**. To compile the AAX version of this plugin, you will also need to obtain the **Avid AAX SDK (version 2.8.1 or newer)** separately.

**To build the AAX version of this plugin:**

1.  **Obtain the Avid AAX SDK 2.8.1 (or newer compatible version):**
    * **Join the Avid Developer Program:** Visit the [Avid Developer website](https://developer.avid.com/aax/) and sign up for the AAX SDK program. This typically involves agreeing to their terms and creating an iLok account.
    * **Download the AAX SDK:** Once registered, navigate to the SDK download section and download "AAX SDK 2.8.1" (or the latest GPLv3-licensed version available for use with JUCE 6.0.8).
    * **Extract the SDK:** Extract the downloaded SDK archive to a stable, known location on your system (e.g., `C:\SDKs\AvidAAXSDK_2.8.1\` on Windows or `~/SDKs/AvidAAXSDK_2.8.1/` on macOS/Linux).

2.  **Build the AAX Library:**
    * Before building CtrlrX, you must compile the core AAX Library project included within the SDK. Navigate to the `Libs/AAXLibrary` directory within your extracted AAX SDK and build the project using your chosen IDE (e.g., Visual Studio on Windows, Xcode on macOS). This step generates the necessary static library (`.lib` or `.a`) that CtrlrX will link against.

3.  **Configure your build environment:**
    * **If using JUCE's Projucer (recommended for JUCE 6.0.8):**
        * Open the Projucer application (from your JUCE 6.0.8 clone).
        * Go to **"Settings"** (gear icon or File > Global Paths).
        * Under "SDK Locations" or "Module Paths", locate the entry for **"AAX SDK"** and set its path to the root directory of your extracted Avid AAX SDK (e.g., `C:\SDKs\AvidAAXSDK_2.8.1\`).
        * Open your `.jucer` project file, go to the "Exporters" section, and ensure the AAX format is enabled for your target platform.
    * **If using CMake:**
        * Your `CMakeLists.txt` will need to be configured to locate both your JUCE 6.0.8 root and the AAX SDK root.
        * You'll typically define variables (e.g., `JUCE_ROOT`, `AAX_SDK_ROOT`) and use `find_path` or explicit `set` commands, then pass `AAX_SDK_ROOT` to `juce_add_plugin` via the `AAX_SDK_PATH` argument. (Detailed CMake setup for JUCE 6.0.8 with external SDKs can be complex; a dedicated CMake guide might be needed for this if it's your primary build method).

4.  **Build the CtrlrX AAX binary:** Follow your project's general build instructions (e.g., via your IDE after Projucer generation, or directly with CMake) to compile the AAX target.
    * [Your specific build command/steps for AAX, e.g., for Projucer: "Open the .jucer project in Projucer, select the AAX target, and click 'Save and Open in IDE'. Then build in your IDE." or for CMake: `cmake --build . --config Release --target CtrlrX_AAX`]

**Additional Notes for Development and Distribution:**

* **Pro Tools Developer Version:** To test unsigned AAX plugins, you will likely need a special "Pro Tools Developer" version of Pro Tools, available through the [Avid Developer Program](https://developer.avid.com/aax).

* **PACE Codesigning:** For any distributed AAX plugin (commercial or free), you will generally need to go through the AVID authorization process via PACE codesigning tools. This is a requirement for plugins to load in standard (non-developer) Pro Tools versions. This process is managed by Avid and PACE.

### AU Support

To compile the AU & AUv3 versions of this plugin, ensure your **JUCE installation is correctly set up to include the AU SDK**, as it's typically bundled with JUCE.

Tuturial coming soon.

### AUv3 Support

To compile the VST3 version of this plugin, ensure your **JUCE installation is correctly set up to include the AUv3 SDK**, as it's typically bundled with JUCE.

Tuturial coming soon.

---

## ToDo

* Update to the latest version of JUCE 8
* Upgrade LUABIND to another LUA/C++ library (LuaBridge, Sol, etc.)
* Implement the entire JUCE Classes with LUA

---

## Changelog

#### Version 5.6.34 | 2025.10.13

* **NEW** ProTools AAX plugin support (requires AAX SDK 2.8+ and codesigning via PACE)
* **UPDATED** LuaBind Drawable class. `LCore.cpp`, `LGraphics.cpp`
* **UPDATED** LuaBind function `addChild()` to `valueTree` class. `LCore.cpp`
* **ADDED** LuaBind function `jmap()`, `mapToLog10()`, `mapFromLog10()`, `isWithin()` to GlobalFunctions. `LCore.cpp`
* **ADDED** LuaBind Parse XML support to `XmlDocument` class. `LCore.cpp`
* **UPDATED** README.md Licensing terms, compilation instructions and layout improved. `README.md`
* **FIXED** Subsequent modulators with the same CC index as the first one not reacting. `CtrlrManagerInstance.cpp`, `CtrlrMidiInputComparatorSingle.cpp`, `CtrlrMidiInputComparatorMulti.cpp`
* **UPDATED** MIDI Plugin Options default settings set to 'Input from plugin host" & "Input from host to comparator". `CtrlrPanel.cpp`
* **UPDATED** MIDI Keyboard component, Octave DOWN/UP added with key assignment "x" & "z" by default. Top line colour on focus. `CtrlrMidiKeyboard.cpp`, `CtrlrMidiKeyboard.h`, `CtrlrIDs.xml`, `CtrlrIDs.h` 
* **UPDATED** focusGained(), focusLost() to CtrlrComponent. `CtrlrComponent.cpp` & `.h`
* **UPDATED** Plugin Wrapper is used by CtrlrX only on WIN with Ableton Live as VST2. The "ctrlrUseEditorWrapper" property is deprecated. `CtrlrProcessor.cpp` & `.h`, `CtrlrManager.cpp`, `CtrlrManagerInstance.cpp`, `CtrlrPanel.cpp`
* **UPDATED** LuaBind InputStream class. `LCore.cpp`
* **UPDATED** Boost library v1.8.8. `Source/Misc/boost/`
* **ADDED** Visual Studio 2022 configuration. `CtrlrX.jucer`
* **ADDED** JUCE, Boost, LuaBind version infos in the About window. `CtrlrAbout.cpp` & `.h`
* **REMOVED** VST2.4 SDK due to licensing compliance. `/Source/Misc/vst2sdk/`
* **NEW** Support for online plugin authorization (Requires PHP & SQL Server with keygen and authentication scripts and exec). `CtrlrX.jucer`, `LCore.cpp`, `LMachineIDUtilities.cpp` & `.h`, `LOnlineUnlockStatus.cpp` & `.h`, `LOnlineUnlockStatusCheck.cpp`& `.h`, `LRSAKey.cpp` & `.h`
* **UPDATED** askForTextInputWindow fallback for cancel button value to -1. `CtrlrLuaUtils.cpp`
* **UPDATED** LuaMethodEditor, "Open" and "Rename" method items added to the menu. `CtrlrLuaMethodEditor.cpp` & `.h`
* **UPDATED** Close panel alert window buttons set to "Close" & "Cancel". `CtrlrDocumentPanel.cpp`
* **UPDATED** LnF version & ColourScheme selection method centralized to CtrlrInlineUtilitiesGUI. `CtrlrInlineUtilitiesGUI.cpp` & `.h`, `.jucer`, `juce_LookAndFeel_V4.cpp` & `.h`, `CtrlrEditor.cpp`, `CtrlrButton.cpp`, `CtrlrToggleButton.cpp`, `CtrlrCombo.cpp`, `CtrlrGroup.cpp`, `CtrlrFixedImageSlider.cpp`, `CtrlrFixedSlider.cpp`, `CtrlrFixedSlider.cpp`, `CtrlrImageSlider.cpp`, `CtrlrSlider.cpp`, `CtrlrPanelEditor.cpp`
* **UPDATED** Panel tab close button method with canClose(bool) for consistency. `CtrlrDocumentPanel.cpp`, `CtrlrManager.cpp` & `.h`
* **FIXED** Alert windows LnF version not following the global LnF for V3. `CtrlrEditor.cpp` & `.h`, `CtrlrInlineUtilitiesGUI.cpp` & `.h`
* **ADDED** Preferences "ctrlrPropertyLineImprovedLegibility" to force B&W text for property pane text entry fields. `CtrlrPropertyComponent.cpp`, `CtrlrIDs.xml`, `CtrlrIDs.h`
* **FIXED** CtrlrPanel.cpp panelVersionMajor & panelVersionMinor stored as int. `CtrlrPanel.cpp` 
* **ADDED** Algorithm for Roland, E-mu, Korg, Waldorf, Lexicon, Exclusive OR, Akai, Korg, Sequential checksums. `CtrlrSysexProcessor.cpp` & `.h` `CtrlrMacros.h`, `CtrlrPropertyComponent.cpp`, `CtrlrUtilities.cpp`
* **FIXED** Resource Editor colours follow the general LnF colourScheme. `CtrlrPanelResourceEditor.cpp` & `.h`
* **ADDED** Resource Editor file preview thumbnail. `CtrlrPanelResourceEditor.cpp` & `.h`
* **FIXED** Single quote content in comboBox. `CtrlrValueMap.cpp` & `.h`
* **ADDED** CtrlrPanelProperties highlighted tab when active. `CtrlrPanelProperties.cpp`
* **ADDED** alertWindow warning at export when trying to overwrite an existing panel on macOS. `CtrlrMac.cpp`
* **UPDATED** Export instance alertWindows won't show up if cancelled by user. `CtrlrMac.cpp`, `CtrlrPanelFileOperations.cpp`
* **UPDATED** Export instance fileChooser default directory is now set to panelLastSaveDir or falls back to desktop. `CtrlrMac.cpp`
* **FIXED** CtrlrLuaMethodCodeEditorSettings Highlight colours such as Error, comments, operatiors etc. `CtrlrLuaMethodCodeEditorSettings.cpp`, `CtrlrLuaMethodCodeTokeniserFUnctions.h`
* **UPDATED** CtrlrLuaMethodCodeEditorSettings GUI with a better sample code preview. `CtrlrLuaMethodCodeEditorSettings.cpp`
* **UPDATED** CtrlrPropertyComponent Separated colourChooser from colour field. `CtrlrPropertyComponent.cpp` &`.h`, `CtrlrIDs.xml`
* **UPDATED** CtrlrPropertyComponent Layer selection via comboBox. `CtrlrPropertyComponent.cpp` & `.h`, `CtrlrIDs.xml`
* **UPDATED** CtrlrPropertyComponent Font size, kerning & horizontal-scale sliders replaced by drop-down selector for convenience. `CtrlrPropertyComponent.cpp` &`.h`
* **FIXED** CtrlrX crashing or freezing when closing while the notify bar was visible. `CtrlrPanelEditor.cpp` & `.h`
* **UPDATED** Utils get16bitSigned() & get16bitSigned() funtions bind to LUA. `CtrlrLuaUtils.cpp` & `.h`
* **UPDATED** Layer manager complete redesign for clarity and convenience. `CtrlrPanelLayerList.cpp` & `.h`, `CtrlrPanelLayerListItem.cpp` & `.h`, `CtrlrPanelEditorIcon.cpp` & `.h`
* **UPDATED** DEBIAN 12 and Fedora 42 build instructions. `README.md`
* **NEW** GitHub CI/CD [Pamplejuce](https://github.com/sudara/pamplejuce) Release Workflow. Thanks to [sudara](https://github.com/sudara) & [Sgorpi](https://github.com/sgorpi)
* **ADDED** VST3 Instance export support for Linux. Thanks to [John Goodland](https://github.com/dnaldoog).  `CtrlrLinux.cpp`
* **ADDED** ZLIB & GZIP Support with LuaBind. Thanks to [John Goodland](https://github.com/dnaldoog)
* **ADDED** fromLuaString(), fromLuaStringToAscii() and MemoryBlock(int) init function to memoryBlock Class with LuaBind. Thanks to [John Goodland](https://github.com/dnaldoog)


#### Version 5.6.33 | 2025.05.28

* **FIXED** VST3 Host>CtrlrX interface. Sliders now react to Host automations of parameter values. Related Modulators send MIDI output messages. `CtrlrProcessor.cpp` & `CtrlrProcessor.h`, `CtrlrModulator.cpp` & `CtrlrModulator.h`, `CtrlrModulatorProcessor.cpp` & `CtrlrModulatorProcessor.h`
* **FIXED** Linux Makefile. CtrlrX requires `binutils-dev` & `libsframe1` installed on the system to compile. Thanks @sgorpi for the PR. `Builds/Linux/Makefile/Makefile`
* **FIXED** ADD, REMOVE, RELOAD resource pane buttons not reacting on certain setups. Z-index added. Thanks to @dnaldoog. `CtrlrPanelResourceEditor.cpp`
* **ADDED** Property line height base value in Preferences>GUI. `CrrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrSettings.cpp`, `CtrlrManager.cpp`, `CtrlrManagerInstance.cpp`, `CtrlrPropertyComponent.cpp`
* **ADDED** LuaBind functions `setType()`, `setMidiMessageType()`, `setProperty()`. `CtrlrMidiMessage.cpp`
* **ADDED** "Encrypt exported panel resources", "Delay between steps at export" & "Codesign exported panel" properties to improve panel export process. `CrrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrMac.cpp`, `CtrlrPanel.cpp`
* **FIXED** Useless menu item "Register file extensions" hidden for macOS binaries. `CtrlrEditorApplicationCommandsMenus.cpp`
* **ADDED** LuaBind function `addColumnBreak()` for `PopupMenu`. `LComponents.cpp`
* **ADDED** SliderType LinearBarVertical, RotaryHorizontalVerticalDrag, TwoValueHorizontal, TwoValueVertical, ThreeValueHorizontal, ThreeValueVertical. `CtrlrComponentTypeManager.h` & `CtrlrComponentTypeManager.cpp`, `CtrlrIDs.xml`

#### Version 5.6.32 | 2025.04.05

* **NEW** From now on, `uiSliders` will return `double float` type values to manage decimals. If required values are integers, a new LUA function has been added: `getValueInt()`, `getModulatorInt()`, `getModulatorValueInt()`, `getMinModulatorValueInt()`, `getMaxModulatorValueInt()`, `getValueMappedInt()`, `getValueNonMappedInt()`, `getMinMappedInt()`, `getMaxMappedInt()`
* **NEW** VST3 can now export instances without the need to compile intermediate plugins from the Projucer. VST3 identifiers will be taken from the panel plugin name, plugin ID, Panel Author and manufacturer ID as well as the VST3 plugin type. Exported VST3 plugins are codesigned on export directly with JUCE `childProcess()` automatically, either with local ad-hoc signature or, if selected, with a developer certificate.
* **UPDATED** Exported instances will now hide the preferences and shortcut menu items. `CtrlrEditorApplicationCommandsMenus.cpp`
* **UPDATED** Decimal values are now supported for Sliders (i.e. 3.1416). `CtrlrModulator.cpp` & `h`, `CtrlrModulatorProcessor.cpp` & `h`, `CtrlrIDs.xml`, `CtrlrLuaManager.cpp`, `CtrlrLuaMethodManager.h`, `CtrlrLuaMethodManagerCalls.cpp`, `CtrlrFixedImageSlider.cpp`, `CtrlrFixedImageSlider.cpp`, `CtrlrImageSlider.cpp`, `CtrlrSliderInternal.cpp` & `h`
* **UPDATED** Decimal interval steps are now supported for Sliders (i.e. 0.1)
* **FIXED** Useless shortcuts such as "New Panel", "Export" enabled on restricted instances. `CtrlrEditorApplicationCommands.cpp`
* **FIXED** Build Timestamp not updating on macOS. `CtrlrX.jucer`, `CtrlrRevision.h`
* **FIXED** FileChooser still hanging when exporting instance on OSX Catalina & macOS BigSur. `CtrlrManager.cpp`
* **FIXED** Windows crashing when loading a panel with a faulty modulator callback on value change LUA script. `luabind/detail/call_function.hpp`, `CtrlrModulatorProcessor.cpp`
* **ADDED** Slider value Suffix (i.e. Hz, ms, dB, etc)
* **ADDED** JUCE `systemStats` support for WIN11, macOS 11, macOS 12, macOS 13, macOS 14, macOS 15 and macOS 16. `juce_mac_systemStats.cpp`, `juce_win32_systemStats.cpp`, `juce_systemStats.h`
* **ADDED** Enable/Disable "Run modulator valueChange LUA callback in Bootstrap state". `CtrlrManagerInstance.cpp`, `CtrlrModulatorProcessor.cpp`, `CrrlrIDs.h`, `CtrlrIDs.xml`, `CtrlrSettings.cpp`

#### Version 5.6.31 | 2025.01

* **Security Update:** Encryption of the panel file in the macOS bundle for restricted instances. `CtrlrMac.cpp`, `CtrlrEditorApplicationCommandsMenu.cpp`, `CtrlrManager.cpp`
* **NEW** JUCE Class `MouseInputSource` added to LUA. `LCore.cpp`, `LCore.h`, `LJuce.cpp`, `LJuce.h`, `LMouseInputSource.h`
* **NEW** algorithm for Roland checksums. `CtrlrSysexProcessor.cpp` & `CtrlrUtilities.cpp`
* **NEW** settings for LUA Method Editor. `CtrlrValueTreeEditor.h`, `CtrlrLuaMethodCodeEditorSettings.cpp` & `h`, `CtrlrLuaMethodCodeEditor.cpp`, `CtrlrLuaMethodEditor.cpp`, `CtrlrIDs.h` & `CtrlrIDs.xml`
* **FIXED** MSB 14 bit numbers sending `0xFF` instead of `0x7F`. `CtrlrSysexProcessor.cpp` & `CtrlrUtilities.cpp`
* **FIXED** missing File Management bottom notification bar. `CtrlrPanelFileOperations.cpp`, `CtrlrPanelEditor.cpp`, `CtrlrPanelEditor.h`, `CtrlrPanel.cpp`, `CtrlrPanel.h`
* **FIXED** missing menuBar on export for log and MIDI monitor windows. `CtrlrChildWindowContainer.cpp`
* **FIXED** `uiSlider` value not reaching `maxValue` when using negative values for `minValue`. `CtrlrSlider.cpp`
* **FIXED** `uiImageSlider` value not reaching `maxValue` when using negative values for `minValue`. `CtrlrImageSlider.cpp`
* **FIXED** black text on black background for Modulator List window. `CtrlrPanelModulatorList.cpp`
* **FIXED** typo in alert when closing dirty panel. `CtrlrPanelFileOperations.cpp`
* **FIXED** parameters count passed to VST host is set from the highest vstindex when a panel is an exported VST/VST3 instance, not (64). `CtrlrProcessor.cpp`
* **FIXED** LnF panel close button colour on mouseover follows the panel colourScheme. `CtrlrDocumentPanel.cpp`
* **FIXED** `CtrlrModulator Value` statement precised to help avoid feedback loops between LUA and (delayed) UI. Commit 6e5a0b2 by midibox. `CtrlrLuaManager.cpp`
* **FIXED** exported VST crashing DAW if `panelIsDirty = 0` on export. `CtrlPanelFileOperations.cpp`
* **FIXED** Ctrlr not showing up in Ableton Live. `CtrlrX.jucer`, `CtrlrProcessor.cpp` & `CtrlrProcessor.h`, `CtrlrProcessorEditorForLive.cpp` & `CtrlrProcessorEditorForLive.h`
* **FIXED** MIDI Monitor IN/OUT turned ON by default. `CtrlrManager.cpp`
* **FIXED** Console window & Midi Monitor crashing Cubase if closed from the Menu File>Close. `CtrlrLuaConsole.cpp` & `CtrlrMIDIMonitor.cpp`
* **FIXED** LUA `mod:getMidiMessage():getProperty("propertyName")` `CtrlrMidiMessage.cpp` & `CtrlrMidiMessage.h`
* **FIXED** exported instances not getting the proper LnF version or colourScheme (popup, child windows etc). `CtrlrManagerInstance.cpp`, `CtrlrEditor.cpp`
* **FIXED** property pane tabs not showing up on exported instances. `CtrlrManagerInstance.cpp`
* **FIXED** Property Pane tabs not following panel LnF. `CtrlrPanelProperties.cpp` & `CtrlrPanelProperties.h`
* **FIXED** Child Windows (LUA Editor, console etc) not getting the proper menuBar background. `CtrlrChildWindowContainer.cpp`
* **FIXED** LUA Editor Method Tree selected item not getting the proper colour. `CtrlrLuaMethodEditor.cpp`
* **FIXED** AU AudioUnit version of CtrlX was failing the Apple/Logic Validation Test. `CtrlrX.jucer`, `CtrlrProcessor.cpp` & `CtrlrProcessor.h`
* **FIXED** Panel Tabs showing up on exported instances. `CtrlrDocumentPanel.cpp`
* **FIXED** messy preferences window and settings. `CtrlrSettings.cpp` & `CtrlrSettings.cpp`, `ctrlrIDs.h` & `CtrlrIDs.xml`
* **FIXED** default New Panel LnF not following global LnF from Global Preferences. `ctrlrPanel.cpp`, `ctrlrIDs.h` & `CtrlrIDs.xml`
* **FIXED** CtrlrX.ico too dark to be legible on dark backgrounds. `ctrlr_logo_circle_v3.svg`
* **FIXED** FileChooser hanging when exporting instance on OSX Catalina & macOS BigSur. `CtrlrManager.cpp`
* **FIXED** About popup design refurbished with new CtrlrX logo. `CtrlrAbout.cpp` & `CtrlrAbout.h`
* **FIXED** About popup not getting current build date, fixed with C++ Macro timestamp. `CtrlrRevision.h`
* **FIXED** VST crashing DAW when loading a project with `panelIsDirty = 0`. `CtrlPanelFileOperations.cpp`
* **FIXED** modulator not reacting to MIDI input messages. `CtrlrModulatorProcessor.cpp`

#### Version 5.6.30 | 2024.03.13

* Missing JUCE File Class definitions bound to LUA
* New LookAndFeel_V4 colourScheme added (V4 JetBlack, V4 YamDX, V4 AkAPC, V4 AkMPC, V4 LexiBlue, V4 KurzGreen, V4 KorGrey, V4 KorGold, V4 ArturOrange, V4 AiraGreen).
* Colours fixed in the LUA Method Editor and LUA Console
* File>Save As removes panelDirty asterisk suffix
* `uiButton` & `uiImageButton` can show the MIDI Monitor window by selecting it from the `componentInternalFunction` property
* Legacy mode for older panels protects their background colours
* Close button added to LUA Method Editor Tabs (as in 5.1.198, 5.2 & 5.3 versions)
* LUA Method Editor Tabs won't shrink and will show a `+` sign if the TabBar exceeds the window width
* Close button added to Panel Editor Tabs (as in 5.1.198, 5.2 & 5.3 versions)
* Panel Editor Tabs won't shrink and will show a `+` sign if the TabBar exceeds the window width

#### Version 5.6.29

* Implementation of the entire JUCE LookAndFeel_V4 design with all color schemes
* Panels designed on previous versions (5.3.198 & 5.3.220) are compatible and will automatically use LookAndFeel_V2/V3
* Implementation of the JUCE ColourSelector popup for every colour property
* Description/ID in the property pane switches without selecting other tabs to update
* Overall improvement of Ctrlr GUI, component settings and functionalities

#### Version 5.6.28

* Added support for scalable UI for responsive design via callback on APP/Plugin viewport resize and viewport resize parameters.
* Fixed menuBar not showing up issue for non-restricted exported instance.

#### Version 5.6.27

* `uiPanelViewPortBackgroundColour` property added in the global properties to change the background color of the ViewPort, parent of the Panel canvas.

#### Version 5.6.26

* `setChangeNotificationOnlyOnRelease` added in the component section for all types of sliders. When enabled, it sends the Value only when the mouse button is released.

#### Version 5.6.25

* `mouseUp`, `mouseEnter`, `mouseExit` callbacks added in for Generic Components (buttons, sliders etc)

#### Version 5.6.24

* `ctrlrEditor` window showing scrollbars over canvas in Cubase has been fixed

#### Version 0.0.0

* Current version is forked from Ctrlr 5.6.23
* Requires unified versioning pattern

---

## Credits

* Thanks to [Damien Sellier](https://github.com/DamienSellier) for developing and maintaining [CtrlrX](https://github.com/damiensellier/CtrlrX).
* Thanks to [Roman Kubiak](https://github.com/RomanKubiak) for developing [Ctrlr](https://github.com/RomanKubiak/ctrlr).
* Links to contributors coming soon.

---

## Notes

