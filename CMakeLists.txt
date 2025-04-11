cmake_minimum_required(VERSION 3.14)
project(PixelPulse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(EMSCRIPTEN)
    set(PLATFORM "wasm")
    set(PLATFORM_NAME "wasm")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist)

    set(EMSCRIPTEN_FLAGS
        "-s USE_GLFW=3"
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
elseif(WIN32)
    set(PLATFORM "windows")
    set(PLATFORM_NAME "windows")
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

# Set output directories based on platform
if(EMSCRIPTEN)
    # For WASM builds, put executable files in the dist subdirectory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM}-${ARCH}/dist/lib)
else()
    # For non-WASM builds, put executable files directly in platform directory
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

# For WASM, create proper directory structure
if(EMSCRIPTEN)
    add_custom_command(
        TARGET copy_resources
        POST_BUILD
        # Create dist directory for all content except serve.js
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR}/dist
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

add_dependencies(pixel_pulse copy_resources)

target_include_directories(pixel_pulse PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Aggressive compiler warnings and modern C++ enforcement
if(MSVC)
    target_compile_options(pixel_pulse PRIVATE
        /W4                  # Warning level 4 (higher than /Wall)
        /WX                  # Treat warnings as errors
        /permissive-         # Enforce strict standard compliance
        /w14640             # Enable warning on thread un-safe static member initialization
        /w14928             # Avoid silent lifetime extensions and narrowing conversions
        /w14265             # Class has virtual functions but destructor is not virtual
        /w14826             # Conversion from 'type1' to 'type2' is sign-extended
        /w14263             # Member function does not override any base class virtual member function
        /w14062             # Enumerator in switch of enum is not handled
        /w14062             # Enumerator in switch of enum is not handled
        /w14242             # Conversion from 'type1' to 'type2', possible loss of data
        /w14254             # Larger type converted to smaller type, possible loss of data
        /w14263             # Member function does not override any base class virtual member function
        /w14296             # Expression is always false/true
        /w14311             # Pointer truncation from 'type1' to 'type2'
        /utf-8              # Set source and execution character sets to UTF-8
        /Zc:__cplusplus     # Enable updated __cplusplus macro
        /Zc:rvalueCast      # Enforce type conversion rules for rvalue references
        /Zc:inline          # Remove unreferenced functions or data if they're explicitly marked inline
        /Zc:strictStrings   # Disable string literal type conversion to non-const
        /Zc:threadSafeInit  # Enable thread-safe local static initialization
        /Zc:preprocessor    # Use the new conforming preprocessor
        /Zc:referenceBinding # Enforce reference binding rules
        /MP                 # Multi-processor compilation
        /external:W3         # Warnings from external headers are level 3
        /diagnostics:column  # Show column information in diagnostics
    )

    # Disable specific warnings if needed
    # target_compile_options(pixel_pulse PRIVATE /wd4100) # Disable specific warning

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
        -Wold-style-cast         # Warn for c-style casts
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

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(pixel_pulse PRIVATE
            -Wmost
            -Weverything
            -Wno-c++98-compat    # Don't warn about C++98 compatibility
            -Wno-c++98-compat-pedantic # Don't warn about C++98 compatibility pedantic issues
            -Wno-padded          # Don't warn about padding
            -Wno-reserved-id-macro # Some libraries use reserved identifiers
            -Wno-documentation   # Don't warn about minor doc issues
            -Wno-poison-system-directories # Don't warn about system include directories
            -Wno-newline-eof     # Don't warn about missing newline at end of file
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC specific options
        target_compile_options(pixel_pulse PRIVATE
            -Wlogical-op           # Warn about suspicious logical operations
            -Wuseless-cast         # Warn about casting to the same type
            -Wduplicated-branches  # Warn about identical branches in if-else
            -Wduplicated-cond      # Warn about duplicated conditions in if-else-if chains
            -Wsuggest-override     # Warn if a virtual function is not marked with override
            -Wsuggest-final-types  # Suggest to mark types as final if possible
            -Wsuggest-final-methods # Suggest to mark methods as final if possible
            -Wplacement-new=2      # Make sure there's enough memory for placement new
            -Wextra-semi           # Warn about redundant semicolons
            -Wcast-qual            # Warn when cast removes type qualifiers
            -Wzero-as-null-pointer-constant # Using 0 for nullptr
            -fdiagnostics-show-option # Show which warning flag is responsible for each warning
            -fvar-tracking-assignments # Better debug info
            -fstack-protector-strong # Buffer overflow protection
            -fstack-check          # Stack corruption checking
        )

        # Add additional options based on GCC version
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 9.0)
            target_compile_options(pixel_pulse PRIVATE
                -Wclass-memaccess     # Warn about potentially unsafe memcpy usage with classes
                -Wstringop-truncation # Warn about string truncation
                -Wmultistatement-macros # Warn if a macro has multiple statements without braces
            )
        endif()

        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10.0)
            target_compile_options(pixel_pulse PRIVATE
                -Warith-conversion      # Warn on implicit conversions in arithmetic operations
                -Wredundant-tags        # Redundant class/struct/enum/union in declarations
            )
        endif()
    endif()
endif()
