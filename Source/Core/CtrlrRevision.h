#ifndef __CTRLR_REVISION__
#define __CTRLR_REVISION__

static const char *ctrlrRevision          = JucePlugin_VersionString;

#ifdef JUCE_MAC
// macOS build: Use build script-generated timestamp
#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP "2025-07-10 06:01:18"
#endif
static const char *ctrlrRevisionDate      = BUILD_TIMESTAMP; // Updated v5.6.32. FIX for Xcode not updating build time properly and keeping the first build timestamp as ref

#else
// All other platforms: Use __TIMESTAMP__
static const char *ctrlrRevisionDate      = __TIMESTAMP__;
#endif

#endif // __CTRLR_REVISION__

// Updated v5.6.32.
// Following code should be added to CtrlrX.jucer "Pre-Build Script" field
//#!/bin/bash
//timestamp=$(date +"%Y-%m-%d %H:%M:%S")
//headerFile="../../Source/Core/CtrlrRevision.h"
//# Use sed to replace or append the BUILD_TIMESTAMP definition
//if grep -q "BUILD_TIMESTAMP" "$headerFile"; then
//    sed -i "" "s/#define BUILD_TIMESTAMP "2025-07-10 06:01:18"
//else
//    echo "#define BUILD_TIMESTAMP "2025-07-10 06:01:18"
//fi
