# PROJECT:     ExectOS
# LICENSE:     See the COPYING.md in the top level directory
# FILE:        configure.ps1
# DESCRIPTION: Project configuration script for preparing the build environment
# DEVELOPERS:  Aiken Harris <harraiken91@gmail.com>

# Check XTchain
if (-not $env:XTCVER) {
    Write-Error "XTChain not detected or corrupted!"
    exit 1
}

# Set target architecture defaulting to amd64
$ARCH = if ($env:TARGET) { $env:TARGET } else { "amd64" }

# Set target build type defaulting to Debug
$env:BUILD_TYPE = if ($env:BUILD_TYPE -in @("Debug", "Release")) { $env:BUILD_TYPE } else { "Debug" }

# Set variables
$EXECTOS_SOURCE_DIR = $PSScriptRoot
$EXECTOS_BINARY_DIR = Join-Path $EXECTOS_SOURCE_DIR "build-$ARCH-$($env:BUILD_TYPE.ToLower())"

# Create build directory
if (-not (Test-Path $EXECTOS_BINARY_DIR)) {
    Write-Host "Creating build directory: $EXECTOS_BINARY_DIR"
    New-Item -ItemType Directory -Path $EXECTOS_BINARY_DIR -Force | Out-Null
}

Set-Location $EXECTOS_BINARY_DIR

# Delete old cache
Remove-Item "CMakeCache.txt", "host-tools/CMakeCache.txt" -ErrorAction SilentlyContinue

# Configure project using CMake
& cmake -G Ninja "-DARCH:STRING=$ARCH" "-DBUILD_TYPE:STRING=$($env:BUILD_TYPE)" $EXECTOS_SOURCE_DIR

# Check if configuration succeeded
if ($LASTEXITCODE -ne 0) {
    Write-Error "Configure script failed."
    exit 1
}

$ARCH | Out-File -Encoding ASCII -NoNewline "build.arch"
Write-Host "Configure completed. Run 'xbuild' to build ExectOS."