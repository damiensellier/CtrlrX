# Reads in our VERSION file and sticks in it CURRENT_VERSION variable
# Be sure the file has no newlines!
# This exposes CURRENT_VERSION to the build system
# And it's later fed to JUCE so it shows up as VERSION in your IDE


# This will automate the patch level bumping
option(PAMPLEJUCE_AUTO_BUMP_PATCH_LEVEL "Auto bump patch level based on commit count" OFF)

# Build system depends on this file, copy it into the build dir
configure_file(VERSION VERSION COPYONLY)

if (PAMPLEJUCE_AUTO_BUMP_PATCH_LEVEL)
    file(STRINGS "${CMAKE_CURRENT_BINARY_DIR}/VERSION" ORIGINAL_VERSION)
    # Parse a two or three-part version: major.minor or major.minor.patch
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)(\\.([0-9]+))?" ORIGINAL_VERSION_MATCH "${ORIGINAL_VERSION}")
    if (NOT CMAKE_MATCH_1)
        message(FATAL_ERROR "Invalid version format: ${ORIGINAL_VERSION}")
    endif ()

    set(MAJOR_VERSION "${CMAKE_MATCH_1}")
    set(MINOR_VERSION "${CMAKE_MATCH_2}")
    if (CMAKE_MATCH_4)
        set(PATCH_LEVEL "${CMAKE_MATCH_4}")
    else ()
        set(PATCH_LEVEL "0")
    endif ()

    execute_process(
        COMMAND git blame -l "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
        OUTPUT_VARIABLE BLAME_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REGEX MATCH "^[0-9a-fA-F]+" LAST_BUMP_COMMIT "${BLAME_OUTPUT}")

    execute_process(
        COMMAND git rev-list HEAD "^${LAST_BUMP_COMMIT}" --count
        OUTPUT_VARIABLE PATCH_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # When developing in Debug, we don't yet have a commit
    # for what we are currently working on, so bump + 1
    # This means if last production release is 1.2.3, it will build 1.2.4
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        math(EXPR PATCH_LEVEL "${PATCH_COUNT} + 1")
    else ()
        set(PATCH_LEVEL "${PATCH_COUNT}")
    endif ()

    set(CURRENT_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_LEVEL}")
    message(STATUS "Auto bumped patch-level: ${CURRENT_VERSION}")

else ()
    file(STRINGS "${CMAKE_CURRENT_BINARY_DIR}/VERSION" CURRENT_VERSION)
endif ()

# Figure out the major version to append to our PROJECT_NAME
string(REGEX MATCH "([0-9]+)" MAJOR_VERSION ${CURRENT_VERSION})
message(STATUS "Version: ${CURRENT_VERSION}")
