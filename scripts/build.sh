#!/bin/bash
set -e

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." &> /dev/null && pwd )"

# Change to project root directory
cd "$PROJECT_ROOT"

# Detect number of cores for parallel build
if [ "$(uname)" == "Darwin" ]; then
    # macOS
    CORES=$(sysctl -n hw.logicalcpu)
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    # Linux
    CORES=$(nproc)
else
    # Default to 2 cores
    CORES=2
fi

# Parse command line arguments
BUILD_TYPE="Release"
CLEAN=false
VERBOSE=false
USE_WASM=false

for arg in "$@"; do
    case $arg in
        --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
        --clean)
        CLEAN=true
        shift
        ;;
        --verbose)
        VERBOSE=true
        shift
        ;;
        --wasm)
        USE_WASM=true
        shift
        ;;
        --help)
        echo "Usage: build.sh [options]"
        echo "Options:"
        echo "  --debug    Build in debug mode"
        echo "  --clean    Clean the build directory before building"
        echo "  --verbose  Show verbose build output"
        echo "  --wasm     Build for WebAssembly using Emscripten"
        echo "  --help     Show this help message"
        exit 0
        ;;
    esac
done

# Create build directory if it doesn't exist
mkdir -p build

# Clean build if requested
if [ "$CLEAN" == "true" ]; then
    echo "Cleaning build directory..."
    rm -rf build/*
fi

# Detect platform and architecture
if [ "$USE_WASM" == "true" ]; then
    if ! command -v emcc &> /dev/null; then
        echo "Error: Emscripten compiler (emcc) not found. Please install Emscripten or make sure it's in your PATH."
        exit 1
    fi
    PLATFORM="wasm"
    ARCH="wasm32"
    echo "Building for WebAssembly using Emscripten..."
elif [ "$(uname)" == "Darwin" ]; then
    PLATFORM="macos"
    if [ "$(uname -m)" == "arm64" ]; then
        ARCH="arm64"
    else
        ARCH="x64"
    fi
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    PLATFORM="linux"
    if [ "$(uname -m)" == "aarch64" ]; then
        ARCH="arm64"
    else
        ARCH="x64"
    fi
else
    PLATFORM="unknown"
    ARCH="unknown"
fi

mkdir -p bin/$PLATFORM-$ARCH
mkdir -p build/$PLATFORM-$ARCH

# Create assets/ and shaders/ directory in bin/
mkdir -p bin/$PLATFORM-$ARCH/assets
mkdir -p bin/$PLATFORM-$ARCH/shaders

# Configure with CMake
echo "Configuring build for $PLATFORM-$ARCH..."

if [ "$USE_WASM" == "true" ]; then
    # Check if emcc and emcmake are available
    if ! command -v emcc &> /dev/null || ! command -v emcmake &> /dev/null; then
        echo "Error: Emscripten compiler (emcc) or emcmake not found. Please install Emscripten or make sure it's in your PATH."
        exit 1
    fi

    # WebAssembly build using Emscripten
    echo "Configuring CMake for Emscripten..."
    emcmake cmake -B build/$PLATFORM-$ARCH \
                  -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
                  -DCMAKE_INSTALL_PREFIX=bin/$PLATFORM-$ARCH \
                  -DCMAKE_EXECUTABLE_SUFFIX=".html"

                  #-DCMAKE_EXE_LINKER_FLAGS="-s MODULARIZE=1 -s EXPORT_NAME='createPixelPulseModule' -s 'EXPORTED_FUNCTIONS=[\"_main\"]'" \

    # Add verbose flag if requested
    BUILD_ARGS="--build build/$PLATFORM-$ARCH --config $BUILD_TYPE -j $CORES"
    if [ "$VERBOSE" == "true" ]; then
        BUILD_ARGS="$BUILD_ARGS --verbose"
    fi

    # Build WebAssembly
    echo "Building WebAssembly with $CORES cores..."
    cmake $BUILD_ARGS
else
    # Native build
    CMAKE_ARGS="-S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    if [ "$VERBOSE" == "true" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
    fi

    # Configure and build for native platform
    cmake $CMAKE_ARGS

    echo "Building project with $CORES cores..."
    BUILD_ARGS="--build build --config $BUILD_TYPE -j $CORES"
    if [ "$VERBOSE" == "true" ]; then
        BUILD_ARGS="$BUILD_ARGS --verbose"
    fi

    # Run the native build
    cmake $BUILD_ARGS
fi

# Output final path
echo "Build complete!"
if [ "$USE_WASM" == "true" ]; then
    echo "WebAssembly output location: bin/$PLATFORM-$ARCH/pixel_pulse.js and bin/$PLATFORM-$ARCH/pixel_pulse.wasm"
else
    echo "Binary location: bin/$PLATFORM-$ARCH/pixel_pulse"
fi
