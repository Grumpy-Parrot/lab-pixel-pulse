cmake_minimum_required(VERSION 3.14)
project(PixelPulse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDL_VERSION "3.2.10")

if(EMSCRIPTEN)
    set(PLATFORM "wasm")
    set(PLATFORM_NAME "wasm")
    add_compile_definitions(PLATFORM_WASM=1)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist/assets)

    set(EMSCRIPTEN_FLAGS
        "-s USE_SDL=0"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s MAX_WEBGL_VERSION=2"
        "-s FETCH=1"
        "-s USE_PTHREADS=0"
        "-s ASSERTIONS=1"
        "-s WASM=1"
        "-s ASYNCIFY"
        "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/bin/wasm-wasm32/assets@assets"
        "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/bin/wasm-wasm32/shaders@shaders"
    )
    string(REPLACE ";" " " EMSCRIPTEN_FLAGS_STR "${EMSCRIPTEN_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_FLAGS_STR}")
elseif(APPLE)
    set(PLATFORM "macos")
    set(PLATFORM_NAME "macos")
    add_compile_definitions(PLATFORM_MACOS=1)
elseif(WIN32)
    set(PLATFORM "windows")
    set(PLATFORM_NAME "windows")
    add_compile_definitions(PLATFORM_WINDOWS=1)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM "linux")
    set(PLATFORM_NAME "linux")
else()
    set(PLATFORM "unknown")
    set(PLATFORM_NAME "unknown")
endif()

if(EMSCRIPTEN)
    set(ARCH "wasm32")
    set(PLATFORM_ARCH "wasm32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if((APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64") OR
       (WIN32 AND (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")))
        set(ARCH "arm64")
        set(PLATFORM_ARCH "arm64")
    else()
        set(ARCH "x64")
        set(PLATFORM_ARCH "x64")
    endif()
else()
    set(ARCH "x86")
    set(PLATFORM_ARCH "x86")
endif()

if(EMSCRIPTEN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist/assets)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist/assets/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist/assets/lib)
elseif(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/lib)

    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/lib)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/lib)
    endforeach()
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/lib)
endif()

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH})
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/assets)
set(SHADERS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/shaders)
set(WASM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/wasm)

add_custom_target(copy_resources ALL
    # Copy shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS_SOURCE_DIR} ${OUTPUT_DIR}/shaders
    # Copy assets
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${OUTPUT_DIR}/assets
    COMMENT "Copying resources to ${OUTPUT_DIR}"
    VERBATIM
)

if(EMSCRIPTEN)
    add_custom_command(
        TARGET copy_resources
        POST_BUILD
        # Create dist directory for all content except serve.js
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/dist
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/dist/assets
        # Copy serve.js directly to the bin/wasm-wasm32 directory
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${WASM_SOURCE_DIR}/serve.js ${OUTPUT_DIR}/serve.js
        # Copy all other WASM resources to the dist directory
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${WASM_SOURCE_DIR} ${OUTPUT_DIR}/dist
        # Remove serve.js from dist as it should only be in the root
        COMMAND ${CMAKE_COMMAND} -E rm -f ${OUTPUT_DIR}/dist/serve.js
        COMMENT "Setting up WASM directory structure in ${OUTPUT_DIR}"
        VERBATIM
    )
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(pixel_pulse ${SOURCES})

if(WIN32)
    set_target_properties(pixel_pulse PROPERTIES
        DEBUG_OUTPUT_NAME "pixel_pulse-Debug"
        RELEASE_OUTPUT_NAME "pixel_pulse"
        MINSIZEREL_OUTPUT_NAME "pixel_pulse"
        RELWITHDEBINFO_OUTPUT_NAME "pixel_pulse"
    )
endif()

add_dependencies(pixel_pulse copy_resources)

target_include_directories(pixel_pulse PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external
)

if(APPLE)
    set(SDL_ROOT ${CMAKE_SOURCE_DIR}/external/SDL/release-${SDL_VERSION}/macos-universal/static)

    target_include_directories(pixel_pulse PRIVATE
        ${SDL_ROOT}/include
    )

    set(SDL_FRAMEWORKS
        "-framework AppKit"
        "-framework Foundation"
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework ForceFeedback"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework AVFoundation"
        "-framework CoreMedia"
        "-framework GameController"
        "-framework CoreHaptics"
        "-framework CoreServices"
        "-framework UniformTypeIdentifiers"
        "-framework ApplicationServices"
        "-framework Carbon"
    )

    target_link_libraries(pixel_pulse PRIVATE
        ${SDL_ROOT}/libSDL3-universal.a
        ${SDL_FRAMEWORKS}
    )

    set_target_properties(pixel_pulse PROPERTIES
        DEBUG_OUTPUT_NAME "pixel_pulse-Debug"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(pixel_pulse PRIVATE -fsanitize=address)
        target_link_options(pixel_pulse PRIVATE -fsanitize=address)
    endif()
elseif(WIN32)
    set(SDL_ROOT ${CMAKE_SOURCE_DIR}/external/SDL/release-${SDL_VERSION}/windows-universal/static)

    target_include_directories(pixel_pulse PRIVATE
        ${SDL_ROOT}/include
    )

    target_link_libraries(pixel_pulse PRIVATE
        user32.lib
        gdi32.lib
        winmm.lib
        imm32.lib
        ole32.lib
        oleaut32.lib
        version.lib
        uuid.lib
        advapi32.lib
        setupapi.lib
        shell32.lib
    )

    if(PLATFORM_ARCH STREQUAL "arm64")
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_link_libraries(pixel_pulse PRIVATE
                ${SDL_ROOT}/SDL3-arm64-Debug.lib
            )
        else()
            target_link_libraries(pixel_pulse PRIVATE
                ${SDL_ROOT}/SDL3-arm64.lib
            )
        endif()
    else()
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_link_libraries(pixel_pulse PRIVATE
                ${SDL_ROOT}/SDL3-x86_64-Debug.lib
            )
        else()
            target_link_libraries(pixel_pulse PRIVATE
                ${SDL_ROOT}/SDL3-x86_64.lib
            )
        endif()
    endif()
elseif(EMSCRIPTEN)
    set(SDL_ROOT ${CMAKE_SOURCE_DIR}/external/SDL/release-${SDL_VERSION}/wasm-wasm32/static)

    target_include_directories(pixel_pulse PRIVATE
        ${SDL_ROOT}/include
    )

    target_link_libraries(pixel_pulse PRIVATE
        ${SDL_ROOT}/libSDL3.a
    )
else()
    # For Linux, assume SDL is installed in the system
    # or provide path to custom SDL installation
    # Example for a custom path:
    # set(SDL_ROOT ${CMAKE_SOURCE_DIR}/external/SDL/3.2.10/linux)
    # target_include_directories(pixel_pulse PRIVATE ${SDL_ROOT}/include)
    # target_link_libraries(pixel_pulse PRIVATE ${SDL_ROOT}/lib/libSDL3.a)

    # For system SDL:
    find_package(SDL3 REQUIRED)
    target_link_libraries(pixel_pulse PRIVATE SDL3::SDL3)
endif()

# Add stb headers with warning suppression
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/external/stb/master)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(pixel_pulse PRIVATE APP_DEBUG=1)
endif()

# Aggressive compiler warnings and modern C++ enforcement
if(MSVC)
    target_compile_options(pixel_pulse PRIVATE
        /W4                  # Warning level 4 (most important warnings)
        /WX                  # Treat warnings as errors
        /permissive-         # Enforce strict standard compliance
        /w14640             # Thread un-safe static member initialization
        /w14265             # Class has virtual functions but destructor is not virtual
        /w14062             # Enumerator in switch of enum is not handled
        /w14242             # Conversion from 'type1' to 'type2', possible loss of data
        /utf-8              # Set source and execution character sets to UTF-8
        /Zc:__cplusplus     # Enable updated __cplusplus macro
        /Zc:preprocessor    # Use the new conforming preprocessor
        /MP                 # Multi-processor compilation
        /diagnostics:column  # Show column information in diagnostics
    )

    # Set runtime library based on configuration
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(pixel_pulse PRIVATE /MDd)
    else()
        target_compile_options(pixel_pulse PRIVATE /MD)
    endif()
else()
    # Common GCC/Clang options
    target_compile_options(pixel_pulse PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror                  # Treat warnings as errors
        -Wconversion             # Warn on type conversions that may lose data
        -Wshadow                 # Warn when a local variable shadows another variable
        -Wunused                 # Warn on anything being unused
        -Wcast-align             # Warn for potential performance problem casts
        -Woverloaded-virtual     # Warn when overloading virtual functions
        -Wsign-conversion        # Warn on sign conversions
        -Wnon-virtual-dtor       # Warn when a class with virtual functions has non-virtual destructor
        -Wdouble-promotion       # Warn if float is implicitly promoted to double
        -Wformat=2               # Warn on security issues around functions that format output
        -Wimplicit-fallthrough   # Warn when switch cases fall through
        -Wmisleading-indentation # Warn when indentation implies blocks where there are none
        -Wstrict-overflow=5      # Warn about various type-based optimizations
        -Wundef                  # Warn if an undefined identifier is evaluated
        -fstrict-aliasing
    )

    # Special flags for Emscripten/WASM
    if(EMSCRIPTEN)
        target_compile_options(pixel_pulse PRIVATE
            -Wno-disabled-macro-expansion # Disable warnings about disabled macro expansion
            -Wno-unsafe-buffer-usage-in-libc-call # Disable warnings about unsafe buffer usage in libc calls
        )
    endif()

    # Disable specific warnings that are problematic with stb headers
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(pixel_pulse PRIVATE
            -Wno-sign-conversion      # Disable sign conversion warnings for stb headers
            -Wno-format-nonliteral    # Disable format nonliteral warnings for stb headers
        )
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(pixel_pulse PRIVATE
            -Wall                  # Basic warnings
            -Wextra                # Extra warnings
            -Wpedantic             # Ensure standard compliance
            -Werror                # Treat warnings as errors
            -Wconversion           # Warn on type conversions that may lose data
            -Wshadow               # Warn when a local variable shadows another variable
            -Wunused               # Warn on anything being unused
            -Woverloaded-virtual   # Warn when overloading virtual functions
            -Wsign-conversion      # Warn on sign conversions
            -Wnon-virtual-dtor     # Warn when a class with virtual functions has non-virtual destructor
            -Wdouble-promotion     # Warn if float is implicitly promoted to double
            -Wformat=2             # Warn on security issues around functions that format output
            -Wimplicit-fallthrough # Warn when switch cases fall through
            -Wctad-maybe-unsupported # Warn about potentially unintended class template argument deduction
            -Wmissing-noreturn     # Warn about functions that never return but aren't marked
            -Wunreachable-code     # Warn about unreachable code
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC specific options - focused on most important warnings
        target_compile_options(pixel_pulse PRIVATE
            -Wall                   # Basic warnings
            -Wextra                 # Extra warnings
            -Wpedantic              # Ensure standard compliance
            -Werror                 # Treat warnings as errors
            -Wlogical-op           # Warn about suspicious logical operations
            -Wuseless-cast         # Warn about casting to the same type
            -Wduplicated-cond      # Warn about duplicated conditions in if-else-if chains
            -Wsuggest-override     # Warn if a virtual function is not marked with override
            -Wcast-qual            # Warn when cast removes type qualifiers
            -Wconversion           # Warn on type conversions that may lose data
            -fstack-protector-strong # Buffer overflow protection
        )
    endif()
endif()
