cmake_minimum_required(VERSION 3.25)
# This line is the minimal change needed to suppress the warnings you saw previously.
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.31.0") 
  cmake_policy(SET CMP0175 NEW)
endif()

# This tells cmake we have goodies in the /cmake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (PamplejuceVersion)

# Modern concise way to add dependencies to your project
include (CPM)

# Configures universal binaries and decides which version of macOS to support
include(PamplejuceMacOS)

# Couple tweaks that IMO should be JUCE defaults
include(JUCEDefaults)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Change me!
# This is the internal name of the project and the name of JUCE's shared code target
# Note: This cannot have spaces (it may be 2024, but you can't have it all!)
# This name is also used for BUNDLE_ID generation.
set(BASE_ID_NAME "CtrlrX")
set(PROJECT_NAME "${BASE_ID_NAME}")
set(CMAKE_CXX_STANDARD 14) # the lua bindings prevent newer C++ standard usage.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- User-facing display name with spaces (used for AU/Standalone display) ---
set(PLUGIN_DISPLAY_NAME "CtrlrX")

# Change me!
# This is the CLEAN name used for the final binary artifact files (CtrlrX.vst3, CtrlrX.app)
set(PRODUCT_NAME "CtrlrX")

# Change me!
# This is the user-facing Manufacturer Name.
set(COMPANY_NAME "CtrlrX Project")

# The VST3 custom overrides are removed as the names are now set globally.
# This variable is now set using the clean BASE_ID_NAME to ensure the bundle identifier is valid.
set(BUNDLE_ID "com.${BASE_ID_NAME}.${PROJECT_NAME}")

# Change me! Set the plugin formats you want built
# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS Standalone AU VST3)

# For simplicity, the name of the CMake project is also the name of the target
# CURRENT_VERSION is from the VERSION file and shall contain non-negative integer components,
# i.e. <major>[.<minor>[.<patch>[.<tweak>]]]
project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# JUCE is setup as a submodule in the /JUCE folder
# Locally, you must run `git submodule update --init --recursive` once
# and later `git submodule update --remote --merge` to keep it up to date
# On Github Actions, this is done as a part of actions/checkout
add_subdirectory(JUCE)

# ---------------------------------------------------
if ("VST" IN_LIST FORMATS)
    # Set VST2 path relative to the project root
    juce_set_vst2_sdk_path("${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/vst2sdk")
endif()
# ---------------------------------------------------

# ---------------------------------------------------
if ("AAX" IN_LIST FORMATS)
    # Set AAX path relative to the project root
    juce_set_aax_sdk_path("${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/aax-sdk-2-8-1")
endif()
# ---------------------------------------------------

# Add any other modules you want modules here, before the juce_add_plugin call
# juce_add_module(modules/my_module)

# This adds the melatonin inspector module
#add_subdirectory (modules/melatonin_inspector)

# See `docs/CMake API.md` in the JUCE repo for all config options
juce_add_plugin("${PROJECT_NAME}"
    # Icons for the standalone app
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Source/Resources/Icons/ctrlrx_logo.svg"

    # Set the user-facing display name for formats that support spaces (AU/Standalone)
    PLUGIN_NAME "${PLUGIN_DISPLAY_NAME}"

    # Change me! This now includes trailing spaces for all formats.
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"

    # On MacOS, plugin is copied to /Users/yourname/Library/Audio/Plug-Ins/
    COPY_PLUGIN_AFTER_BUILD TRUE

    # Change me!
    # A four-character plugin id
    # First character MUST be uppercase for AU formats
    PLUGIN_MANUFACTURER_CODE cTrX

    # Change me!
    # A unique four-character plugin id
    # Note: this must have at least one upper-case character
    PLUGIN_CODE cTrl
    FORMATS "${FORMATS}"

    # See JUCE/docs/CMake API.md
    # --- CORE PLUGIN CHARACTERISTICS FROM PROJUCER ---
    IS_SYNTH TRUE
    IS_MIDI_EFFECT TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    # -------------------------------------------------

    # FIX: VST3 Category is set using the official plural argument (VST3_CATEGORIES)
    # This is the most reliable, clean way to set the VST3 category.
    # VST3_CATEGORIES "Tools"
    
    # The name of your final executable. This now uses the clean PRODUCT_NAME.
    PRODUCT_NAME "${PRODUCT_NAME}"
)

# --- VST3 Dynamic Name/Manufacturer Override ---
# This block is essential for controlling the final VST3 plugin name
# (which includes the trailing space from PLUGIN_DISPLAY_NAME) and manufacturer.

if ("VST3" IN_LIST FORMATS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # Overwrite the name property for VST3 compilation (includes trailing space)
        JUCE_PLUGIN_NAME "CtrlrX                          "
        # Overwrite the manufacturer property for VST3 compilation
        JUCE_COMPANY_NAME "CtrlrX Project                  "
    )
endif()
# ---------------------------------------------------

# If you had any *other* necessary, non-category format overrides here, they should 
# be re-added below using the old method, but for the VST3 category, the problem is solved.


# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(SharedCode INTERFACE)


# Enable fast math, C++20 and a few other target defaults
include(SharedCodeDefaults)


#Add this if the Release version is crashing due to over optimisation of LuaBind
# -----------------------------------------------------------------------
# --- FIX: Disable optimization for the crashing LuaBind file ---
# This uses COMPILE_FLAGS to correctly pass the -O0 flag to the compiler, 
# bypassing the optimization bug in Release/RelWithDebInfo builds.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/luabind/src/object_rep.cpp"
    PROPERTIES COMPILE_FLAGS "-O0"
)
# -----------------------------------------------------------------------


# Manually list all .h and .cpp files for the plugin
# If you are like me, you'll use globs for your sanity.
# Just ensure you employ CONFIGURE_DEPENDS so the build system picks up changes
# If you want to appease the CMake gods and avoid globs, manually add files like so:
# set(SourceFiles Source/PluginEditor.h Source/PluginProcessor.h Source/PluginEditor.cpp Source/PluginProcessor.cpp)
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/luabind/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/luabind/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/luabind/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/luabind/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/libusb/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/libusb/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/*.c*" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/*.h*"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Lua/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Lua/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/MIDI/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/MIDI/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Native/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Native/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/UIComponents/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Source/UIComponents/*.h"
)

# -----------------------------------------------------------------------
# FIX: Exclude CtrlrLinux files from the build unless we are compiling for Linux.
# This prevents compilation errors on Mac/Windows due to missing headers like <bfd.h>
if (NOT LINUX)
    list(FILTER SourceFiles EXCLUDE REGEX "CtrlrLinux\\.cpp$")
    list(FILTER SourceFiles EXCLUDE REGEX "CtrlrLinux\\.h$")
endif()
# -----------------------------------------------------------------------


#list(FILTER SourceFiles EXCLUDE REGEX "exclude_regex")

target_sources(SharedCode INTERFACE ${SourceFiles})
target_include_directories(SharedCode INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/lua/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/luabind"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/libusb/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/boost"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Misc/vst2sdk"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/MIDI"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/CtrlrManager"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/CtrlrModulator"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/CtrlrPanel"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Native"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Plugin"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/UIComponents"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/Lua"
    "${CMAKE_CURRENT_SOURCE_DIR}/JuceLibraryCode"
)
if(UNIX)
# hide (quite) some warnings...
target_compile_options(SharedCode INTERFACE
    -w -Wno-implicit-fallthrough -Wno-maybe-uninitialized -Wno-missing-field-initializers -Wno-free-nonheap-object)
endif()

# Adds a BinaryData target for embedding assets into the binary
include(Assets)

# MacOS only: Cleans up folder and target organization on Xcode.
include(XcodePrettify)

# This is where you can set preprocessor definitions for JUCE and your plugin
target_compile_definitions(SharedCode
    INTERFACE

    # JUCE_WEB_BROWSER and JUCE_USE_CURL off by default
    JUCE_WEB_BROWSER=0 # If you set this to 1, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0 # If you set this to 1, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1

    # Uncomment if you are paying for a an Indie/Pro license or releasing under GPLv3
    JUCE_DISPLAY_SPLASH_SCREEN=0

    # to prevent warnings on declaring the Bind placeholders (_1, _2, ...) in the global namespace is deprecated
    BOOST_BIND_GLOBAL_PLACEHOLDERS=1

    # lets the app known if we're Debug or Release
    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    # FIX: Suppress the Windows C4996 'unsafe function' warning (e.g., for sprintf in Lua)
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

# Link to any other modules you added (with juce_add_module) here!
# Usually JUCE modules must have PRIVATE visibility
# See https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md#juce_add_module
# However, with Pamplejuce, you'll link modules to SharedCode with INTERFACE visibility
# This allows the JUCE plugin targets and the Tests target to link against it
target_link_libraries(SharedCode
    INTERFACE
    Assets
    juce_audio_basics
    juce_audio_devices
    juce_audio_formats
    juce_audio_plugin_client
    juce_audio_processors
    juce_audio_utils
    juce_core
    juce_cryptography
    juce_data_structures
#    juce_dsp
    juce_events
    juce_graphics
    juce_gui_basics
    juce_gui_extra
    juce_product_unlocking
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

if(LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(libudev REQUIRED libudev)
    pkg_check_modules(x11 REQUIRED x11)
    pkg_check_modules(zlib REQUIRED zlib)

    message(STATUS "libudev version: ${libudev_VERSION}")
    message(STATUS "libX11 version: ${x11_VERSION}")
    message(STATUS "libz version: ${zlib_VERSION}")

    target_link_libraries(SharedCode
        INTERFACE
        ${libudev_LIBRARIES}
        ${zlib_LIBRARIES}
        ${x11_LIBRARIES}
    )
endif()

# Link the JUCE plugin targets our SharedCode target
target_link_libraries("${PROJECT_NAME}" PRIVATE SharedCode)

# IPP support, comment out to disable
include(PamplejuceIPP)


# Everything related to the tests target
#include(Tests) # enable once we have tests...

# A separate target for Benchmarks (keeps the Tests target fast)
#include(Benchmarks) # enable once we have benchmarks...

# Output some config for CI (like our PRODUCT_NAME)
include(GitHubENV)
