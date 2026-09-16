#!/bin/bash

# CtrlrX macOS Build Script
# Requires: ninja cmake (Install via Homebrew: brew install ninja cmake)

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
NUM_CORES=$(sysctl -n hw.ncpu)

echo "========================================"
echo "  CtrlrX macOS Build System (Ninja)"
echo "========================================"
echo ""
echo "Select build type:"
echo "  1) Full Release       (Clean + Configure + Build)"
echo "  2) Full Debug         (Clean + Configure + Build)"
echo "  3) Full RelWithDebInfo (Clean + Configure + Build)"
echo "  4) Clean Rebuild      (Ninja Clean + Build)"
echo "  5) Quick Build        (Ninja Only)"
echo "  6) Test"
echo ""
read -p "Enter choice [1-6]: " choice

case $choice in
    1|2|3)
        BUILD_TYPE="Release"
        [ "$choice" == "2" ] && BUILD_TYPE="Debug"
        [ "$choice" == "3" ] && BUILD_TYPE="RelWithDebInfo"

        echo ""
        echo "--- Starting Full $BUILD_TYPE Build ---"
        rm -rf "$BUILD_DIR"

        cmake -S "$PROJECT_ROOT" \
              -B "$BUILD_DIR" \
              -G Ninja \
              -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
              -DCTRLRX_USE_LUAJIT=ON

        if [ $? -ne 0 ]; then
            echo "CMake configure failed."
            exit 1
        fi

        cmake --build "$BUILD_DIR" --parallel "$NUM_CORES"
        ;;

    4)
        if [ ! -d "$BUILD_DIR" ]; then
            echo "Build directory not found. Run option 1, 2 or 3 first."
            exit 1
        fi
        echo ""
        echo "--- Cleaning and Rebuilding ---"
        cmake --build "$BUILD_DIR" --target clean
        cmake --build "$BUILD_DIR" --parallel "$NUM_CORES"
        ;;

    5)
        if [ ! -d "$BUILD_DIR" ]; then
            echo "Build directory not found. Run option 1, 2 or 3 first."
            exit 1
        fi
        echo ""
        echo "--- Fast Incremental Build ---"
        cmake --build "$BUILD_DIR" --parallel "$NUM_CORES"
        ;;
    6)
        echo "--- Running Tests ---"
        pushd "${BUILD_DIR}/Tests"
        GTEST_OUTPUT="xml:test-results/" GTEST_COLOR=1 ctest -j"$(nproc)" --verbose
        ../../Tests/ci_check_test_results.py --allow ../../Tests/known_failures_macos.txt "test-results/*.xml"
        popd
        ;;
    *)
        echo "Invalid selection. Exiting."
        exit 1
        ;;
esac

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================"
    echo "  Build complete! [$BUILD_TYPE]"
    echo "========================================"
else
    echo ""
    echo "*** BUILD FAILED ***"
    exit 1
fi