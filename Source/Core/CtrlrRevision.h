#ifndef __CTRLR_REVISION__
#define __CTRLR_REVISION__

static const char *ctrlrRevision          = JucePlugin_VersionString;

#ifdef JUCE_MAC
// macOS build: Use build script-generated timestamp
#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP "2026-05-02 19:29:10"
#endif
static const char *ctrlrRevisionDate      = BUILD_TIMESTAMP; // Updated v5.6.32. FIX for Xcode not updating build time properly and keeping the first build timestamp as ref

#else
// All other platforms: Use __TIMESTAMP__
static const char *ctrlrRevisionDate      = __TIMESTAMP__;
#endif

#endif
