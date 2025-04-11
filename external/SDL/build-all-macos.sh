#!/bin/bash
set -e

SDL_TAG="${1:-release-3.2.10}"
SDL_REPO="https://github.com/libsdl-org/SDL.git"
SDL_DIR="repo"
BUILD_DIR="build"
OUTPUT_DIR="./${SDL_TAG}"

GREEN='\033[0;32m'
NC='\033[0m'

if [ -d "$SDL_DIR" ]; then
    echo -e "${GREEN}Updating SDL repo...${NC}"
    cd "$SDL_DIR"
    git fetch origin
    git checkout "$SDL_TAG"
    git pull origin "$SDL_TAG"
    cd ..
else
    echo -e "${GREEN}Cloning SDL repo...${NC}"
    git clone --branch "$SDL_TAG" --depth 1 "$SDL_REPO" "$SDL_DIR"
fi

rm -rf "$BUILD_DIR" "$OUTPUT_DIR"
mkdir -p "$BUILD_DIR/arm64" "$BUILD_DIR/x86_64" "$BUILD_DIR/wasm"

build_for_arch() {
    ARCH=$1
    INSTALL_DIR="$BUILD_DIR/install-$ARCH"
    echo -e "${GREEN}Building for $ARCH...${NC}"

    cmake -S "$SDL_DIR" -B "$BUILD_DIR/$ARCH" \
        -DSDL_STATIC=ON \
        -DSDL_SHARED=OFF \
        -DCMAKE_OSX_ARCHITECTURES=$ARCH \
        -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
        -DCMAKE_BUILD_TYPE=Release

    cmake --build "$BUILD_DIR/$ARCH" --config Release --target install
}

build_for_arch "arm64"
build_for_arch "x86_64"

echo -e "${GREEN}Combining static libs into universal binary...${NC}"
mkdir -p "$OUTPUT_DIR/macos-universal/static"

lipo -create \
    "$BUILD_DIR/install-arm64/lib/libSDL3.a" \
    "$BUILD_DIR/install-x86_64/lib/libSDL3.a" \
    -output "$OUTPUT_DIR/macos-universal/static/libSDL3-universal.a"

# Copy non-universal static libs
cp "$BUILD_DIR/install-arm64/lib/libSDL3.a" "$OUTPUT_DIR/macos-universal/static/libSDL3-arm64.a"
cp "$BUILD_DIR/install-x86_64/lib/libSDL3.a" "$OUTPUT_DIR/macos-universal/static/libSDL3-x86_64.a"

# Copy headers
cp -R "$BUILD_DIR/install-arm64/include" "$OUTPUT_DIR/macos-universal/static/include"

echo -e "${GREEN}Building for WebAssembly (WASM)...${NC}"
emcmake cmake -S "$SDL_DIR" -B "$BUILD_DIR/wasm" \
    -DSDL_STATIC=ON \
    -DSDL_SHARED=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/install-wasm"

emmake make -C "$BUILD_DIR/wasm" install

mkdir -p "$OUTPUT_DIR/wasm-wasm32/static"
cp "$BUILD_DIR/install-wasm/lib/libSDL3.a" "$OUTPUT_DIR/wasm-wasm32/static/libSDL3.a"
cp -R "$BUILD_DIR/install-wasm/include" "$OUTPUT_DIR/wasm-wasm32/static/include"

echo -e "${GREEN}✅ Done! Output in: $OUTPUT_DIR${NC}"
echo -e "${GREEN}- macOS universal lib: $OUTPUT_DIR/macos-universal/static/libSDL3-universal.a${NC}"
echo -e "${GREEN}- macOS arm64 lib:     $OUTPUT_DIR/macos-universal/static/libSDL3-arm64.a${NC}"
echo -e "${GREEN}- macOS x86_64 lib:    $OUTPUT_DIR/macos-universal/static/libSDL3-x86_64.a${NC}"
echo -e "${GREEN}- WASM static lib:     $OUTPUT_DIR/wasm-wasm32/static/libSDL3.a${NC}"