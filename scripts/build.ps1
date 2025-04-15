# PowerShell version of build.sh for Windows
# Parse command line arguments
param(
    [switch]$Debug,
    [switch]$Clean,
    [switch]$Verbose,
    [switch]$Wasm,
    [switch]$Help
)

# Set strict mode to catch errors
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# Get the directory where this script is located and the project root
$ScriptDir = $PSScriptRoot
$ProjectRoot = Split-Path -Parent $ScriptDir

# Change to project root directory
Set-Location -Path $ProjectRoot

# Detect number of cores for parallel build
$Cores = (Get-CimInstance -ClassName Win32_ComputerSystem).NumberOfLogicalProcessors
if (-not $Cores -or $Cores -le 0) {
    # Default to 2 cores if detection fails
    $Cores = 2
}

# Set build type based on parameters
$BuildType = if ($Debug) { "Debug" } else { "Release" }

# Show help if requested
if ($Help) {
    Write-Host "Usage: build.ps1 [options]"
    Write-Host "Options:"
    Write-Host "  -Debug     Build in debug mode"
    Write-Host "  -Clean     Clean the build directory before building"
    Write-Host "  -Verbose   Show verbose build output"
    Write-Host "  -Wasm      Build for WebAssembly using Emscripten"
    Write-Host "  -Help      Show this help message"
    exit 0
}

# Create build directory if it doesn't exist
if (-not (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Clean build if requested
if ($Clean) {
    Write-Host "Cleaning build directory..."
    if (Test-Path -Path "build") {
        Remove-Item -Path "build\*" -Recurse -Force -ErrorAction SilentlyContinue
    }
}

# Detect architecture
$IsArm64 = $false
if ([System.Environment]::Is64BitOperatingSystem) {
    # Check if ARM64
    $CpuInfo = Get-CimInstance -ClassName Win32_Processor
    if ($CpuInfo.Architecture -eq 12) { # 12 is ARM64
        $IsArm64 = $true
        $Arch = "arm64"
    } else {
        $Arch = "x64"
    }
} else {
    $Arch = "x86"
}

# Set up platform and architecture
if ($Wasm) {
    # Check if emcc is available
    $EmccExists = $null -ne (Get-Command "emcc" -ErrorAction SilentlyContinue)
    if (-not $EmccExists) {
        Write-Host "Error: Emscripten compiler (emcc) not found. Please install Emscripten or make sure it's in your PATH." -ForegroundColor Red
        exit 1
    }
    $Platform = "wasm"
    $Arch = "wasm32"
    Write-Host "Building for WebAssembly using Emscripten..."
} else {
    $Platform = "windows"
    Write-Host "Building for Windows $Arch..."
}

# Create platform-specific directories
$BinDir = "bin\$Platform-$Arch"
$BuildDir = "build\$Platform-$Arch"

if (-not (Test-Path -Path $BinDir)) {
    New-Item -ItemType Directory -Path $BinDir -Force | Out-Null
}

if (-not (Test-Path -Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
}

# Create assets and shaders directories
New-Item -ItemType Directory -Path "$BinDir\assets" -Force | Out-Null
New-Item -ItemType Directory -Path "$BinDir\shaders" -Force | Out-Null

# Configure and build
Write-Host "Configuring build for $Platform-$Arch..."

if ($Wasm) {
    # Check if emcmake is available
    $EmcmakeExists = $null -ne (Get-Command "emcmake" -ErrorAction SilentlyContinue)
    if (-not $EmcmakeExists) {
        Write-Host "Error: emcmake not found. Please install Emscripten or make sure it's in your PATH." -ForegroundColor Red
        exit 1
    }

    # WebAssembly build using Emscripten
    Write-Host "Configuring CMake for Emscripten..."
    $EmcmakeArgs = @(
        "cmake",
        "-B", $BuildDir,
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_CXX_FLAGS=-O3",
        "-DCMAKE_EXE_LINKER_FLAGS=-s MODULARIZE=1 -s EXPORT_NAME='createPixelPulseModule' -s 'EXPORTED_FUNCTIONS=[`"_main`"]'",
        "-DCMAKE_INSTALL_PREFIX=$BinDir"
    )
    & emcmake $EmcmakeArgs

    # Add verbose flag if requested
    $BuildArgs = @("--build", $BuildDir, "--config", $BuildType, "-j", $Cores)
    if ($Verbose) {
        $BuildArgs += "--verbose"
    }

    # Build WebAssembly
    Write-Host "Building WebAssembly with $Cores cores..."
    & cmake $BuildArgs
} else {
    # Native build
    $CmakeArgs = @("-S", ".", "-B", "build", "-DCMAKE_BUILD_TYPE=$BuildType")
    if ($Verbose) {
        $CmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
    }

    # Configure for native platform
    & cmake $CmakeArgs

    Write-Host "Building project with $Cores cores..."
    $BuildArgs = @("--build", "build", "--config", $BuildType, "-j", $Cores)
    if ($Verbose) {
        $BuildArgs += "--verbose"
    }

    # Run the native build
    & cmake $BuildArgs
}

# Output final path
Write-Host "Build complete!" -ForegroundColor Green
if ($Wasm) {
    Write-Host "WebAssembly output location: $BinDir\pixel_pulse.js and $BinDir\pixel_pulse.wasm" -ForegroundColor Cyan
} else {
    Write-Host "Binary location: $BinDir\pixel_pulse.exe" -ForegroundColor Cyan
}
